/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2014 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id: VistaVolumeRaycasterCore.cpp 47481 2015-02-03 21:22:33Z sp841227 $

#include "VistaVolumeRaycasterCore.h"

#include "../VistaBufferObject.h"
#include "../VistaFramebufferObj.h"
#include "../VistaGLSLShader.h"
#include "../VistaShaderRegistry.h"
#include "../VistaTexture.h"
#include "../VistaVertexArrayObject.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaTools/VistaRandomNumberGenerator.h>

#include <algorithm>
#include <cassert>
#include <cmath>

//#define DEBUG_RAYCASTER_OUTPUT

namespace {
const size_t g_nNumVertexEntries = 36;
}

VistaVolumeRaycasterCore::VistaVolumeRaycasterCore()
    : m_pProxyGeometry(new VistaBufferObject())
    , m_pProxyGeometrySetup(new VistaVertexArrayObject())
    , m_pRayGenerationShader(NULL)
    , m_pRaycastOutsideShader(NULL)
    , m_pRaycastInsideShader(NULL)
    , m_pRayGenerationFBO(new VistaFramebufferObj())
    , m_iUserTextureOffset(4)
    , m_pEntryPoints(new VistaTexture(GL_TEXTURE_RECTANGLE))
    , m_pExitPoints(new VistaTexture(GL_TEXTURE_RECTANGLE))
    , m_pDepthTexture(new VistaTexture(GL_TEXTURE_RECTANGLE))
    , m_bPerformDepthTest(false)
    , m_bPerformRayJittering(false)
    , m_pJitterTexture(new VistaTexture(GL_TEXTURE_RECTANGLE))
    , m_iJitterTextureSize(32)
    , m_bUseAlphaClassifiedGradient(false)
    , m_iGradientVolumeId(0)   //< Matches initial volume texture
    , m_iGradientChannelIdx(0) //< Matches initial volume texture's red-channel
    , m_fStepSize(0.01f)
    , m_fAlphaSaturationLimit(0.99f)
    , m_fAlphaCompensationFactor(1.0f)
    , m_bIsLightingActive(false) {
  printf("\t [VistaVolumeRaycasterCore::VistaVolumeRaycasterCore()] \n");

  m_pRayGenerationFBO->Bind();
  GLenum eTargets[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, eTargets);
  m_pRayGenerationFBO->Release();

  m_aViewportSize[0] = m_aViewportSize[1] = -1;

  // Unit cube of [-1,1]^3
  m_aDataExtents[0] = m_aDataExtents[1] = m_aDataExtents[2] = 1.0f;

  GenerateProxyGeometry();
  UpdateJitterTexture();
}

VistaVolumeRaycasterCore::~VistaVolumeRaycasterCore() {
  delete m_pJitterTexture;
  delete m_pDepthTexture;
  delete m_pExitPoints;
  delete m_pEntryPoints;
  delete m_pRayGenerationFBO;
  delete m_pRaycastInsideShader;
  delete m_pRaycastOutsideShader;
  delete m_pRayGenerationShader;
  delete m_pProxyGeometrySetup;
  delete m_pProxyGeometry;
}

bool VistaVolumeRaycasterCore::SetStepSize(const float fSize) {
  if (fSize <= 0.0f)
    return false;
  if (m_fStepSize == fSize)
    return true;

  m_fStepSize = fSize;
  RefreshUniforms();

  return true;
}

float VistaVolumeRaycasterCore::GetStepSize() const {
  return m_fStepSize;
}

bool VistaVolumeRaycasterCore::SetAlphaSaturationLimit(const float fLimit) {
  if (fLimit < 0.0f || fLimit > 1.0f)
    return false;
  if (m_fAlphaSaturationLimit == fLimit)
    return true;

  m_fAlphaSaturationLimit = fLimit;
  RefreshUniforms();

  return true;
}

float VistaVolumeRaycasterCore::GetAlphaSaturationLimit() const {
  return m_fAlphaSaturationLimit;
}

bool VistaVolumeRaycasterCore::SetAlphaCompensationFactor(const float fFactor) {
  if (fFactor <= 0.0f)
    return false;
  if (m_fAlphaCompensationFactor == fFactor)
    return true;

  m_fAlphaCompensationFactor = fFactor;
  RefreshUniforms();

  return true;
}

float VistaVolumeRaycasterCore::GetAlphaCompensationFactor() const {
  return m_fAlphaCompensationFactor;
}

void VistaVolumeRaycasterCore::SetIsLightingActive(const bool bIsActive) {
  if (m_bIsLightingActive == bIsActive)
    return;

  m_bIsLightingActive = bIsActive;
  BuildShaders(true);
}

bool VistaVolumeRaycasterCore::GetIsLightingActive() const {
  return m_bIsLightingActive;
}

void VistaVolumeRaycasterCore::SetPerformDepthTest(const bool bIsActive) {
  if (bIsActive != m_bPerformDepthTest) {
    m_bPerformDepthTest = bIsActive;
    BuildShaders(true);
  }
}

bool VistaVolumeRaycasterCore::GetPerformDepthTest() const {
  return m_bPerformDepthTest;
}

void VistaVolumeRaycasterCore::SetPerformRayJittering(const bool bIsActive) {
  if (bIsActive != m_bPerformRayJittering) {
    m_bPerformRayJittering = bIsActive;
    BuildShaders(true);
  }
}

bool VistaVolumeRaycasterCore::GetPerformRayJittering() const {
  return m_bPerformRayJittering;
}

bool VistaVolumeRaycasterCore::SetSamplerShaderFilename(const std::string& strFilename) {
  std::string strCache      = m_strSampleShaderFilename;
  m_strSampleShaderFilename = strFilename;
  if (!BuildShaders(false)) {
    m_strSampleShaderFilename = strCache;
    return false;
  }
  return true;
}

std::string VistaVolumeRaycasterCore::GetSamplerShaderFilename() const {
  return m_strSampleShaderFilename;
}

int VistaVolumeRaycasterCore::DeclareVolumeTexture(const std::string& strSamplerName) {
  return DeclareTexture(strSamplerName, m_vecVolumeTextures);
}

int VistaVolumeRaycasterCore::DeclareLookupTexture(const std::string& strSamplerName) {
  return DeclareTexture(strSamplerName, m_vecLookupTextures);
}

int VistaVolumeRaycasterCore::DeclareLookupRange(const std::string& strUniformName) {
  if (!IsUniformAvailable(strUniformName))
    return -1;

  UniformVariable oNewLookupRange;
  oNewLookupRange.m_strUniformName = strUniformName;
  m_vecLookupRanges.push_back(oNewLookupRange);

  return static_cast<int>(m_vecLookupRanges.size() - 1);
}

void VistaVolumeRaycasterCore::SetUseAlphaClassifiedGradient(const bool bUse) {
  if (bUse != m_bUseAlphaClassifiedGradient) {
    m_bUseAlphaClassifiedGradient = bUse;
    BuildShaders(true);
  }
}

bool VistaVolumeRaycasterCore::GetUseAlphaClassifiedGradient() const {
  return m_bUseAlphaClassifiedGradient;
}

void VistaVolumeRaycasterCore::GetGradientVolume(int& iVolumeId, int& iChannelIdx) const {
  iVolumeId   = m_iGradientVolumeId;
  iChannelIdx = m_iGradientChannelIdx;
}

bool VistaVolumeRaycasterCore::SetGradientVolume(const int iVolumeId, const int iChannelIdx) {
  if (iVolumeId < 0 || iVolumeId >= m_vecVolumeTextures.size() || iChannelIdx < 0 ||
      iChannelIdx > 3) {
    return false;
  }

  m_iGradientVolumeId   = iVolumeId;
  m_iGradientChannelIdx = iChannelIdx;

  RefreshUniforms();

  return true;
}

bool VistaVolumeRaycasterCore::SetVolumeTexture(
    VistaTexture* pVolumeTexture, int iVolumeId /* = 0 */) {
  if (iVolumeId < 0 || iVolumeId >= static_cast<int>(m_vecVolumeTextures.size()))
    return false;

  m_vecVolumeTextures[iVolumeId].m_pTexture = pVolumeTexture;

  return true;
}

VistaTexture* VistaVolumeRaycasterCore::GetVolumeTexture(int iVolumeId /* = 0 */) const {
  if (iVolumeId < 0 || iVolumeId >= static_cast<int>(m_vecVolumeTextures.size()))
    return NULL;
  return m_vecVolumeTextures[iVolumeId].m_pTexture;
}

bool VistaVolumeRaycasterCore::SetExtents(float aDataExtents[3]) {
  printf("[VistaVolumeRaycasterCore::SetExtents] %f %f %f \n", aDataExtents[0], aDataExtents[1],
      aDataExtents[2]);
  if (m_aDataExtents[0] < 0.0f || aDataExtents[1] < 0.0f || m_aDataExtents[2] < 0.0f)
    return false;
  std::memcpy(m_aDataExtents, aDataExtents, 3 * sizeof(m_aDataExtents[0]));
  return true;
}

void VistaVolumeRaycasterCore::GetExtents(float aDataExtents[3]) {
  std::memcpy(aDataExtents, m_aDataExtents, 3 * sizeof(aDataExtents[0]));
}

bool VistaVolumeRaycasterCore::SetLookupTexture(
    VistaTexture* pLookupTexture, int iLookupId /* = 0 */) {
  if (iLookupId < 0 || iLookupId >= static_cast<int>(m_vecLookupTextures.size()))
    return false;

  m_vecLookupTextures[iLookupId].m_pTexture = pLookupTexture;

  return true;
}

VistaTexture* VistaVolumeRaycasterCore::GetLookupTexture(int iLookupId /* = 0 */) const {
  if (iLookupId < 0 || iLookupId >= static_cast<int>(m_vecLookupTextures.size()))
    return NULL;

  return m_vecLookupTextures[iLookupId].m_pTexture;
}

bool VistaVolumeRaycasterCore::SetLookupRange(float* pLookupRange, int iRangeId /* = 0 */,
    unsigned int uiNumComponents /* = 2 */, unsigned int uiNumVectors /* = 1 */) {
  if (iRangeId < 0 || iRangeId >= static_cast<int>(m_vecLookupRanges.size()))
    return false;

  // Adapt to new number of components if necessary. Cache needed to refresh
  // uniform variables if necessary.
  if (m_vecLookupRanges[iRangeId].m_uiNumComponents != uiNumComponents ||
      m_vecLookupRanges[iRangeId].m_uiNumVectors != uiNumVectors) {
    delete[] m_vecLookupRanges[iRangeId].m_pRangeValues;
    m_vecLookupRanges[iRangeId].m_pRangeValues    = new float[uiNumComponents * uiNumVectors];
    m_vecLookupRanges[iRangeId].m_uiNumComponents = uiNumComponents;
    m_vecLookupRanges[iRangeId].m_uiNumVectors    = uiNumVectors;
  }

  std::memcpy(m_vecLookupRanges[iRangeId].m_pRangeValues, pLookupRange,
      sizeof(float) * uiNumComponents * uiNumVectors);

  RefreshUniforms();

  return true;
}

bool VistaVolumeRaycasterCore::GetLookupRange(float* pLookupRange, int iRangeId /* = 0 */,
    unsigned int uiNumComponents /* = 2 */, unsigned int uiNumVectors /* = 1 */) const {
  if (iRangeId < 0 || iRangeId >= static_cast<int>(m_vecLookupRanges.size()))
    return false;

  if (m_vecLookupRanges[iRangeId].m_uiNumComponents != uiNumComponents ||
      m_vecLookupRanges[iRangeId].m_uiNumVectors != uiNumVectors) {
    return false;
  }

  std::memcpy(pLookupRange, m_vecLookupRanges[iRangeId].m_pRangeValues,
      sizeof(float) * uiNumComponents * uiNumVectors);

  return true;
}

void VistaVolumeRaycasterCore::Draw() {
  // Shader pointer used to detect whether raycaster is ready to render
  if (m_pRaycastOutsideShader) {
    // If the viewport size changed, the off-screen targets which hold the
    // entry and exit points for ray traversal need to be adapted
    if (CheckForViewportResize())
      ResizeTextures();

    // Pass 0: update depth texture for occlusion checks
    UpdateDepthTexture();
    // Pass 1: generate ray entry and exit point textures
    GenerateRays();
    // Pass 2: perform actual ray tracing to generate the image
    PerformRaycasting();
  }
}

bool VistaVolumeRaycasterCore::BuildShaders(const bool bPreserveUserUniforms) {
  delete m_pRaycastInsideShader;
  delete m_pRaycastOutsideShader;
  delete m_pRayGenerationShader;

  VistaShaderRegistry& rShaderReg   = VistaShaderRegistry::GetInstance();
  std::string          strGeneralVS = rShaderReg.RetrieveShader("VistaVanillaRaycaster_vert.glsl");
  std::string strRayGenFS  = rShaderReg.RetrieveShader("VistaRaycastRayGenerator_frag.glsl");
  std::string strOutsideFS = rShaderReg.RetrieveShader("VistaVanillaRaycasterOutside_frag.glsl");
  std::string strInsideFS  = rShaderReg.RetrieveShader("VistaVanillaRaycasterInside_frag.glsl");
  std::string strLoopFS = rShaderReg.RetrieveShader("VistaVanillaRaycasterTraversalLoop_frag.glsl");
  std::string strSampleFS = rShaderReg.RetrieveShader(m_strSampleShaderFilename);

  if (strGeneralVS.empty() || strRayGenFS.empty() || strOutsideFS.empty() || strInsideFS.empty() ||
      strLoopFS.empty()) {
    vstr::errp() << "[VistaVolumeRaycasterCore] One of the shader files \
						could not be opened or is empty."
                 << std::endl;
    return false;
  }

  // It has to be ensured that the line including #version comes first in
  // the source code of the shader (only potentially preceeded by comments).
  // We assume that the location of #version is correct w.r.t. OpenGL,
  // search for it, and split the source in two right after the \n following
  // the #version line. The definitions are then inserted in between the first
  // and second halves of the source. It is assumed that there is a #version
  // string in the source!!
  std::string::size_type nVersionStart = strLoopFS.find_first_of("#version");
  assert(nVersionStart != std::string::npos &&
         "Error. No #version definition in loop shader source code.");
  std::string::size_type nVersionLineEnd = strLoopFS.find_first_of('\n', nVersionStart);
  const std::string      strToVersion    = strLoopFS.substr(0, nVersionLineEnd + 1);
  const std::string      strFromVersion  = strLoopFS.substr(nVersionLineEnd + 1);

  strLoopFS = strToVersion;

  if (m_bPerformRayJittering)
    strLoopFS += "#define PERFORM_RAY_JITTERING\n";
  if (m_bPerformDepthTest)
    strLoopFS += "#define PERFORM_DEPTH_TEST\n";
  if (m_bIsLightingActive)
    strLoopFS += "#define PERFORM_GRADIENT_LIGHTING\n";
  if (m_bUseAlphaClassifiedGradient)
    strLoopFS += "#define USE_ALPHA_CLASSIFIED_GRADIENT\n";

  strLoopFS += strFromVersion;

  m_pRayGenerationShader = new VistaGLSLShader();
  m_pRayGenerationShader->InitShaderFromString(GL_VERTEX_SHADER, strGeneralVS);
  m_pRayGenerationShader->InitShaderFromString(GL_FRAGMENT_SHADER, strRayGenFS);
  m_pRayGenerationShader->Link();

  m_pRaycastOutsideShader = new VistaGLSLShader();
  m_pRaycastOutsideShader->InitShaderFromString(GL_VERTEX_SHADER, strGeneralVS);
  m_pRaycastOutsideShader->InitShaderFromString(GL_FRAGMENT_SHADER, strOutsideFS);
  m_pRaycastOutsideShader->InitShaderFromString(GL_FRAGMENT_SHADER, strLoopFS);
  m_pRaycastOutsideShader->InitShaderFromString(GL_FRAGMENT_SHADER, strSampleFS);
  m_pRaycastOutsideShader->Link();

  m_pRaycastInsideShader = new VistaGLSLShader();
  m_pRaycastInsideShader->InitShaderFromString(GL_VERTEX_SHADER, strGeneralVS);
  m_pRaycastInsideShader->InitShaderFromString(GL_FRAGMENT_SHADER, strInsideFS);
  m_pRaycastInsideShader->InitShaderFromString(GL_FRAGMENT_SHADER, strLoopFS);
  m_pRaycastInsideShader->InitShaderFromString(GL_FRAGMENT_SHADER, strSampleFS);
  m_pRaycastInsideShader->Link();

  if (!bPreserveUserUniforms) {
    // Reset declarations as they usually do not fit the new shader.
    m_vecVolumeTextures.clear();
    m_vecLookupTextures.clear();
    m_vecLookupRanges.clear();
  }

  RefreshUniforms();
  return true;
}

void VistaVolumeRaycasterCore::RefreshUniforms() {
  RefreshUniformsForShader(m_pRaycastOutsideShader);
  RefreshUniformsForShader(m_pRaycastInsideShader);

  // Just for Testing
  RefreshUniformsForShader(m_pRayGenerationShader);
}

void VistaVolumeRaycasterCore::RefreshUniformsForShader(VistaGLSLShader* pShader) {
  pShader->Bind();

  // Prefixed sampler bindings
  int iLoc = pShader->GetUniformLocation("u_ray_entry"); // Optional
  if (iLoc != -1)
    pShader->SetUniform(iLoc, 0);

  iLoc = pShader->GetUniformLocation("u_ray_exit");
  if (iLoc != -1)
    pShader->SetUniform(iLoc, 1);

  iLoc = pShader->GetUniformLocation("u_depth_sampler"); // Optional
  if (iLoc != -1)
    pShader->SetUniform(iLoc, 2);

  iLoc = pShader->GetUniformLocation("u_jitter_sampler"); // Optional
  if (iLoc != -1)
    pShader->SetUniform(iLoc, 3);

  // Check is necessary, as the first volume texture might not have been
  // registered yet. This can only be the case directly after initialization.
  // Subsequent calls to the SetGradientVolume and DeclareVolumeTexture will,
  // however, correct this issue.
  if (m_iGradientVolumeId >= 0 &&
      m_iGradientVolumeId < static_cast<int>(m_vecVolumeTextures.size())) {
    pShader->SetUniform(pShader->GetUniformLocation("u_gradient_sampler"),
        m_vecVolumeTextures[m_iGradientVolumeId].m_iTextureUnit);
  }
  pShader->SetUniform(pShader->GetUniformLocation("u_gradient_channel"), m_iGradientChannelIdx);

  pShader->SetUniform(pShader->GetUniformLocation("u_step_size"), m_fStepSize);
  pShader->SetUniform(
      pShader->GetUniformLocation("u_alpha_saturation_limit"), m_fAlphaSaturationLimit);
  pShader->SetUniform(
      pShader->GetUniformLocation("u_alpha_compensation"), m_fAlphaCompensationFactor);

  if (m_bPerformRayJittering) {
    pShader->SetUniform(pShader->GetUniformLocation("u_jitter_texture_size"), m_iJitterTextureSize);
  }

  for (size_t i = 0; i < m_vecVolumeTextures.size(); ++i) {
    pShader->SetUniform(pShader->GetUniformLocation(m_vecVolumeTextures[i].m_strSamplerName),
        m_vecVolumeTextures[i].m_iTextureUnit);
  }

  for (size_t i = 0; i < m_vecLookupTextures.size(); ++i) {
    pShader->SetUniform(pShader->GetUniformLocation(m_vecLookupTextures[i].m_strSamplerName),
        m_vecLookupTextures[i].m_iTextureUnit);
  }

  for (size_t i = 0; i < m_vecLookupRanges.size(); ++i) {
    if (m_vecLookupRanges[i].m_uiNumComponents > 0 && m_vecLookupRanges[i].m_uiNumVectors > 0) {
      pShader->SetUniform(pShader->GetUniformLocation(m_vecLookupRanges[i].m_strUniformName),
          m_vecLookupRanges[i].m_uiNumComponents, m_vecLookupRanges[i].m_uiNumVectors,
          m_vecLookupRanges[i].m_pRangeValues);
    }
  }

  pShader->Release();
}

void VistaVolumeRaycasterCore::UpdateDepthTexture() {
  if (m_bPerformDepthTest) {
    m_pDepthTexture->Bind();

    glCopyTexSubImage2D(
        GL_TEXTURE_RECTANGLE, 0, 0, 0, 0, 0, m_aViewportSize[0], m_aViewportSize[1]);

    m_pDepthTexture->Unbind();
  }
}

void VistaVolumeRaycasterCore::GenerateRays() {
  // Cull Face and Blending States
  glPushAttrib(GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT);

  // Note: the FBO has no depth buffer. Therefore, no depth-testing will take
  // place and _ALL_ fragments will be generated (even the once occluded by
  // fragments in front of them). This is a requirement!!
  m_pRayGenerationFBO->Bind();

  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // We need depth clamp to avoid problems with near-plane clipping. If the
  // volume is intersected by the near plane, all fragments of the front-facing
  // faces would be lost. This would leave us without entry points for the
  // clipped region. Depth clamping ensures that those fragments are not thrown
  // away but their depth value is changed to that of the near plane hence
  // not clipping them away.
  glEnable(GL_DEPTH_CLAMP);

  m_pRayGenerationShader->Bind();

  // printf("Draw proxy geometry to generate entry and exit point textures to the FBO \n");

  DrawProxyGeometry();

  m_pRayGenerationShader->Release();

  glDisable(GL_DEPTH_CLAMP);

  m_pRayGenerationFBO->Release();

  glPopAttrib();
}

// You'll need this include to use the debug draw code of the following function

#ifdef DEBUG_RAYCASTER_OUTPUT
//#	include "../VistaOGLHelper.h"
#include "../VistaOGLUtils.h"
#endif

void VistaVolumeRaycasterCore::PerformRaycasting() {
  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_POLYGON_BIT);

  // See GenerateRays() function for the reason why we need to use depth clamping
  // glEnable( GL_DEPTH_CLAMP );
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  m_pEntryPoints->Bind(GL_TEXTURE0);
  m_pExitPoints->Bind(GL_TEXTURE1);

  if (m_bPerformDepthTest)
    m_pDepthTexture->Bind(GL_TEXTURE2);

  if (m_bPerformRayJittering)
    m_pJitterTexture->Bind(GL_TEXTURE3);

  // Bind volume ...
  for (size_t i = 0; i < m_vecVolumeTextures.size(); ++i) {
    if (m_vecVolumeTextures[i].m_pTexture) {
      m_vecVolumeTextures[i].m_pTexture->Bind(GL_TEXTURE0 + m_vecVolumeTextures[i].m_iTextureUnit);
    }
  }

  // ... and lookup textures.
  for (size_t i = 0; i < m_vecLookupTextures.size(); ++i) {
    if (m_vecLookupTextures[i].m_pTexture) {
      m_vecLookupTextures[i].m_pTexture->Bind(GL_TEXTURE0 + m_vecLookupTextures[i].m_iTextureUnit);
    }
  }

  float aTexCoordsForViewer[3];

  if (!IsInsideVolume(aTexCoordsForViewer)) {
    m_pRaycastOutsideShader->Bind();

    DrawProxyGeometry();

    m_pRaycastOutsideShader->Release();
  } else {
    // Front-facing fragments don't exist anyways as we are in the volume
    // and _EVERY_ face of the volume is a back face. However, as we need
    // fragments to trigger the raycasting fragment shader this also ensures
    // that back faces are all drawn and their fragment generated.
    glCullFace(GL_FRONT);

    m_pRaycastInsideShader->Bind();

    // The texture coordinate for the viewer will be used as entry point
    printf("Upload %f %f %f \n", aTexCoordsForViewer[0], aTexCoordsForViewer[1],
        aTexCoordsForViewer[2]);
    m_pRaycastInsideShader->SetUniform(
        m_pRaycastInsideShader->GetUniformLocation("tex_coords_for_viewer"), 3, 1,
        aTexCoordsForViewer);

    DrawProxyGeometry();

    m_pRaycastInsideShader->Release();
  }

  // Release lookup ...
  for (size_t i = 0; i < m_vecLookupTextures.size(); ++i) {
    if (m_vecLookupTextures[i].m_pTexture) {
      m_vecLookupTextures[i].m_pTexture->Unbind(
          GL_TEXTURE0 + m_vecLookupTextures[i].m_iTextureUnit);
    }
  }
  // ... and volume textures.
  for (size_t i = 0; i < m_vecVolumeTextures.size(); ++i) {
    if (m_vecVolumeTextures[i].m_pTexture) {
      m_vecVolumeTextures[i].m_pTexture->Unbind(
          GL_TEXTURE0 + m_vecVolumeTextures[i].m_iTextureUnit);
    }
  }

  if (m_bPerformRayJittering)
    m_pJitterTexture->Unbind(GL_TEXTURE3);

  if (m_bPerformDepthTest)
    m_pDepthTexture->Unbind(GL_TEXTURE2);

  m_pExitPoints->Unbind(GL_TEXTURE1);
  m_pEntryPoints->Unbind(GL_TEXTURE0);

  // GL_DEPTH_CLAMP cannot be attrib-pushed
  // glDisable( GL_DEPTH_CLAMP );
  glPopAttrib();

#ifdef DEBUG_RAYCASTER_OUTPUT
  // Some debug code to show the entry & exit point textures
  // The include right in front of this function is required for this code to work
  VistaOGLUtils::BeginOrtho2D(0.0, 1.0, 0.0, 1.0);

  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_RECTANGLE);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  m_pEntryPoints->Bind();
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0);
  glVertex2f(0.0f, 0.0f);
  glTexCoord2i(m_aViewportSize[0], 0);
  glVertex2f(0.2f, 0.0f);
  glTexCoord2i(m_aViewportSize[0], m_aViewportSize[1]);
  glVertex2f(0.2f, 0.2f);
  glTexCoord2i(0, m_aViewportSize[1]);
  glVertex2f(0.0f, 0.2f);
  glEnd();
  m_pEntryPoints->Unbind();

  m_pExitPoints->Bind();
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0);
  glVertex2f(0.0f, 0.25f);
  glTexCoord2i(m_aViewportSize[0], 0);
  glVertex2f(0.2f, 0.25f);
  glTexCoord2i(m_aViewportSize[0], m_aViewportSize[1]);
  glVertex2f(0.2f, 0.45f);
  glTexCoord2i(0, m_aViewportSize[1]);
  glVertex2f(0.0f, 0.45f);
  glEnd();
  m_pExitPoints->Unbind();

  if (m_bPerformDepthTest) {
    m_pDepthTexture->Bind();
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2f(0.0f, 0.5f);
    glTexCoord2i(m_aViewportSize[0], 0);
    glVertex2f(0.2f, 0.5f);
    glTexCoord2i(m_aViewportSize[0], m_aViewportSize[1]);
    glVertex2f(0.2f, 0.7f);
    glTexCoord2i(0, m_aViewportSize[1]);
    glVertex2f(0.0f, 0.7f);
    glEnd();
    m_pDepthTexture->Unbind();
  }

  glDisable(GL_TEXTURE_RECTANGLE);
  glColor3f(0.2f, 0.4f, 0.6f);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0.0f, 0.0f);
  glVertex2f(0.2f, 0.0f);
  glVertex2f(0.2f, 0.2f);
  glVertex2f(0.0f, 0.2f);
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex2f(0.0f, 0.25f);
  glVertex2f(0.2f, 0.25f);
  glVertex2f(0.2f, 0.45f);
  glVertex2f(0.0f, 0.45f);
  glEnd();
  if (m_bPerformDepthTest) {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.2f, 0.5f);
    glVertex2f(0.2f, 0.7f);
    glVertex2f(0.0f, 0.7f);
    glEnd();
  }

  glPopAttrib();

  VistaOGLUtils::EndOrtho();
#endif // Is DEBUG_RAYCASTER_OUTPUT defined?
}

void VistaVolumeRaycasterCore::DrawProxyGeometry() {
  m_pProxyGeometrySetup->Bind();

  // The proxy geometry cube is defined to lie in the range [0,1]^3. Hence, it
  // must be manually re-centered to the origin in the following.
  glPushMatrix();
  glScalef(m_aDataExtents[0], m_aDataExtents[1], m_aDataExtents[2]);
  glTranslatef(-0.5f, -0.5f, -0.5f);
  glDrawArrays(GL_TRIANGLES, 0, ::g_nNumVertexEntries);
  glPopMatrix();
  m_pProxyGeometrySetup->Release();
}

bool VistaVolumeRaycasterCore::IsInsideVolume(float aTexCoordsForViewer[3]) {
  GLfloat aModelViewMatrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, aModelViewMatrix);

  // 'True' to indicate column-major layout
  VistaTransformMatrix oModelViewMatrix(aModelViewMatrix, true);
  oModelViewMatrix.Invert();
  VistaVector3D v3LocalViewPosition =
      oModelViewMatrix.TransformPoint(VistaVector3D(0.0f, 0.0f, 0.0f));

  const bool bIsInVolume = std::abs(v3LocalViewPosition[0]) <= 0.5f * m_aDataExtents[0] &&
                           std::abs(v3LocalViewPosition[1]) <= 0.5f * m_aDataExtents[1] &&
                           std::abs(v3LocalViewPosition[2]) <= 0.5f * m_aDataExtents[2];

  if (bIsInVolume) {
    aTexCoordsForViewer[0] =
        (v3LocalViewPosition[0] + 0.5f * m_aDataExtents[0]) / m_aDataExtents[0];
    aTexCoordsForViewer[1] =
        (v3LocalViewPosition[1] + 0.5f * m_aDataExtents[1]) / m_aDataExtents[1];
    aTexCoordsForViewer[2] =
        (v3LocalViewPosition[2] + 0.5f * m_aDataExtents[2]) / m_aDataExtents[2];
  }

  return bIsInVolume;
}

bool VistaVolumeRaycasterCore::CheckForViewportResize() {
  // would be better to use viewport size and position here, but that is
  // a more complex change
  GLint aViewport[4];
  glGetIntegerv(GL_VIEWPORT, aViewport);

  if (m_aViewportSize[0] != aViewport[2] + aViewport[0] ||
      m_aViewportSize[1] != aViewport[3] + aViewport[1]) {
    m_aViewportSize[0] = aViewport[2] + aViewport[0];
    m_aViewportSize[1] = aViewport[3] + aViewport[1];

    return true;
  }
  return false;
}

void VistaVolumeRaycasterCore::ResizeTextures() {
  m_pDepthTexture->Bind();
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, m_aViewportSize[0],
      m_aViewportSize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_pDepthTexture->Unbind();

  m_pEntryPoints->Bind();
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, m_aViewportSize[0], m_aViewportSize[1], 0,
      GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_pEntryPoints->Unbind();

  m_pExitPoints->Bind();
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, m_aViewportSize[0], m_aViewportSize[1], 0,
      GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_pExitPoints->Unbind();

  m_pRayGenerationFBO->Attach(m_pEntryPoints, GL_COLOR_ATTACHMENT0);
  m_pRayGenerationFBO->Attach(m_pExitPoints, GL_COLOR_ATTACHMENT1);
}

void VistaVolumeRaycasterCore::GenerateProxyGeometry() {
  printf("[VistaVolumeRaycasterCore::GenerateProxyGeometry()] \n");
  float aProxyGeometry[::g_nNumVertexEntries * 6] = {// TexCoords (x, y, z)		VertexPos
                                                     // (x, y, z) Back plane
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      // Front plane
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      // Left plane
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      // Right plane
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      // Top plane
      0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      // Bottom plane
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

  m_pProxyGeometry->Bind(GL_ARRAY_BUFFER);
  m_pProxyGeometry->BufferData(sizeof(aProxyGeometry), aProxyGeometry, GL_DYNAMIC_DRAW);
  m_pProxyGeometry->Release();

  // Texture coordinates
  m_pProxyGeometrySetup->EnableAttributeArray(1);
  m_pProxyGeometrySetup->SpecifyAttributeArrayFloat(
      1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(aProxyGeometry[0]), 0, m_pProxyGeometry);
  // Vertex positions
  m_pProxyGeometrySetup->EnableAttributeArray(0);
  m_pProxyGeometrySetup->SpecifyAttributeArrayFloat(0, 3, GL_FLOAT, GL_FALSE,
      6 * sizeof(aProxyGeometry[0]), 3 * sizeof(aProxyGeometry[0]), m_pProxyGeometry);
}

bool VistaVolumeRaycasterCore::IsUniformAvailable(const std::string& strUniformName) const {
  if (IsUniformDeclared(strUniformName)
      // Only check for inside program as outside program uses the same sampler shader
      || m_pRaycastInsideShader->GetUniformLocation(strUniformName) == -1) {
    return false;
  }
  return true;
}

bool VistaVolumeRaycasterCore::IsUniformDeclared(const std::string& strUniformName) const {
  // Declared as volume texture
  std::vector<Texture>::const_iterator itTexure =
      std::find(m_vecVolumeTextures.begin(), m_vecVolumeTextures.end(), strUniformName);

  if (itTexure != m_vecVolumeTextures.end())
    return true;

  // Declared as lookup texture
  itTexure = std::find(m_vecLookupTextures.begin(), m_vecLookupTextures.end(), strUniformName);

  if (itTexure != m_vecLookupTextures.end())
    return true;

  // Declared as lookup range
  std::vector<UniformVariable>::const_iterator itUniform =
      std::find(m_vecLookupRanges.begin(), m_vecLookupRanges.end(), strUniformName);

  if (itUniform != m_vecLookupRanges.end())
    return true;

  return false;
}

int VistaVolumeRaycasterCore::DeclareTexture(
    const std::string& strSamplerName, std::vector<Texture>& vecTarget) {
  if (!IsUniformAvailable(strSamplerName))
    return -1;

  Texture oNewVolumeTexture;
  oNewVolumeTexture.m_strSamplerName = strSamplerName;
  // Texture unit m_iUserTextureOffset is the base for user-defined textures.
  // Texture units 0, 1 and 2 are used by internal mechanisms and hence must
  // be avoided.
  oNewVolumeTexture.m_iTextureUnit = m_iUserTextureOffset + GetNumberOfTextures();
  oNewVolumeTexture.m_pTexture     = NULL;
  vecTarget.push_back(oNewVolumeTexture);

  // Push texture unit to shader sampler
  RefreshUniforms();

  return static_cast<int>(vecTarget.size() - 1);
}

unsigned int VistaVolumeRaycasterCore::GetNumberOfTextures() const {
  return static_cast<unsigned int>(m_vecVolumeTextures.size() + m_vecLookupTextures.size());
}

void VistaVolumeRaycasterCore::UpdateJitterTexture() {
  // @TODO Fix seed might be a bad idea, but pseudo-random generatio is enough.
  //		 Also, think about the cluster-mode: all cluster slaves should(?)
  //		 have identical seeds.
  VistaRandomNumberGenerator::GetStandardRNG()->SetSeed(0);
  VistaRandomNumberGenerator* pRnd = VistaRandomNumberGenerator::GetStandardRNG();

  // Generate jitter texture data
  const int iSize = 2 * m_iJitterTextureSize * m_iJitterTextureSize;
  float*    pData = new float[iSize];
  for (int n = 0; n < iSize; ++n)
    pData[n] = pRnd->GenerateFloat(0.0f, 1.0f);

  // Upload texture data
  m_pJitterTexture->Bind();
  // @TODO Using RG*F instead of (a sufficient) R*F because that format
  //		 somehow causes problems on NVIDIA GPUs (tested on GTX480 with
  //		 310.90 drivers on 2013/02/12 by Sebastian Pick).
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG16F, m_iJitterTextureSize, m_iJitterTextureSize, 0,
      GL_RG, GL_FLOAT, pData);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_pJitterTexture->Unbind();
}
