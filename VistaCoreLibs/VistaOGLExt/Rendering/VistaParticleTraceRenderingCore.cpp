/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                             .                              */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      FileName :  VflGpuParticleRenderer.cpp   RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :  VistaFlowLib                 RR  R    WW  WW  TT  HH  HH  */
/*                                                                            */
/*      Project  :  ViSTA                          Rheinisch-Westfaelische    */
/*                                               Technische Hochschule Aachen */
/*      Purpose  :  ...                                                       */
/*                                                                            */
/*                                                 Copyright (c)  1998-2016   */
/*                                                 by  RWTH-Aachen, Germany   */
/*                                                 All rights reserved.       */
/*                                             .                              */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES			                                                          */
/*============================================================================*/
#include "VistaParticleTraceRenderingCore.h"
#include "VistaParticleRenderingProperties.h"

#include <VistaBase/VistaStreamUtils.h>

#include "../VistaBufferObject.h"
#include "../VistaFramebufferObj.h"
#include "../VistaGLSLShader.h"
#include "../VistaOGLUtils.h"
#include "../VistaShaderRegistry.h"
#include "../VistaTexture.h"
#include "../VistaVertexArrayObject.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

const char* VPTRC_SHADER_LINES_VERT = "VistaParticleTraceRenderingCore_Lines_vert.glsl";
const char* VPTRC_SHADER_LINES_FRAG = "VistaParticleTraceRenderingCore_Lines_frag.glsl";

const char* VPTRC_SHADER_SMOKE_VERT = "VistaParticleTraceRenderingCore_Smoke_vert.glsl";
const char* VPTRC_SHADER_SMOKE_FRAG = "VistaParticleTraceRenderingCore_Smoke_frag.glsl";

const char* VPTRC_SHADER_BILLBOARD_VERT = "VistaParticleTraceRenderingCore_Billboard_vert.glsl";
const char* VPTRC_SHADER_BILLBOARD_FRAG = "VistaParticleTraceRenderingCore_Billboard_frag.glsl";

const char* VPTRC_SHADER_BUMPED_VERT = "VistaParticleTraceRenderingCore_Bumped_vert.glsl";
const char* VPTRC_SHADER_BUMPED_FRAG = "VistaParticleTraceRenderingCore_Bumped_frag.glsl";

const char* VPTRC_SHADER_DEPTH_VERT = "VistaParticleTraceRenderingCore_Depth_vert.glsl";
const char* VPTRC_SHADER_DEPTH_FRAG = "VistaParticleTraceRenderingCore_Depth_frag.glsl";

const char* VPTRC_SHADER_HALO_VERT = "VistaParticleTraceRenderingCore_Halos_vert.glsl";
const char* VPTRC_SHADER_HALO_FRAG = "VistaParticleTraceRenderingCore_Halos_frag.glsl";

const char* VPTRC_SHADER_DIFFUSE_LIGHTING = "VflParticleRenderingCore_Lighting_Diffuse_aux.glsl";
const char* VPTRC_SHADER_PHONG_LIGHTING   = "VflParticleRenderingCore_Lighting_Phong_aux.glsl";
const char* VPTRC_SHADER_GOOCH_LIGHTING   = "VflParticleRenderingCore_Lighting_Gooch_aux.glsl";
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaParticleTraceRenderingCore::VistaParticleTraceRenderingCore()
    : m_pProperties(new VistaParticleRenderingProperties())
    , m_fLineWidth(2.0f)
    , m_fParticleRadius(0.1f)
    , m_oParticleColor(VistaColor::WHITE)
    , m_pLUT(NULL)
    , m_nPrimitiveRestartIndex(~0)
    , m_pBodyVAO(NULL)
    , m_pCapVAO(NULL)
    , m_nBodyIndices(0)
    , m_nCapCount(0)
    , m_v3ViewerPos(0.0f, 0.0f, 0.0f)
    , m_v3LightDir(0.0f, 0.0f, 1.0f)
    , m_fMaxLuminanceFactor(0.0f)
    , m_fLuminanceReductionFactor(0.0f)
    , m_pLineShader(NULL)
    , m_pSmokeShader(NULL)
    , m_pBillboardShader(NULL)
    , m_pHaloShader(NULL)
    , m_pBlendingTexture(NULL)
    , m_pIlluminationTexture(NULL)
    , m_pTubeNormalsTexture(NULL)
    , m_pCapNormalsTexture(NULL)
    , m_pWhiteTexture(NULL)
    , m_pFBO(NULL)
    , m_pDepthTexture(NULL) {
  m_pBumpedShaders[0] = NULL;
  m_pBumpedShaders[1] = NULL;
  m_pBumpedShaders[2] = NULL;

  m_pDepthShaders[0] = NULL;
  m_pDepthShaders[1] = NULL;
  m_pDepthShaders[2] = NULL;

  m_aLookupRange[0] = 0.0f;
  m_aLookupRange[1] = 1.0f;

  m_aDepthTextureSize[0] = -1;
  m_aDepthTextureSize[1] = -1;

  m_pProperties->AttachObserver(this);
}

VistaParticleTraceRenderingCore::~VistaParticleTraceRenderingCore() {
  m_pProperties->DetachObserver(this);
  delete m_pProperties;

  // delete Shaders
  DeleteShaders();

  // delete Textures
  delete m_pBlendingTexture;
  delete m_pIlluminationTexture;
  delete m_pTubeNormalsTexture;
  delete m_pCapNormalsTexture;

  delete m_pWhiteTexture;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaParticleRenderingProperties* VistaParticleTraceRenderingCore::GetProperties() {
  return m_pProperties;
}

bool VistaParticleTraceRenderingCore::Init() {
  if (!InitShaders())
    return false;

  if (!InitTextures())
    return false;

  m_pDepthTexture = new VistaTexture(GL_TEXTURE_RECTANGLE);
  m_pDepthTexture->Bind();
  m_pDepthTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pDepthTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pDepthTexture->SetMinFilter(GL_NEAREST);
  m_pDepthTexture->SetMagFilter(GL_NEAREST);
  m_pDepthTexture->Unbind();

  m_pFBO = new VistaFramebufferObj();
  m_pFBO->Attach(m_pDepthTexture, GL_DEPTH_ATTACHMENT);

  return true;
}

void VistaParticleTraceRenderingCore::Draw() {
  if (!m_pBodyVAO || !m_pCapVAO || 0 == m_nBodyIndices || 0 == m_nCapCount)
    return;

  glPushAttrib(GL_ENABLE_BIT);

  glPrimitiveRestartIndex(m_nPrimitiveRestartIndex);
  glEnable(GL_PRIMITIVE_RESTART);
  glEnable(GL_CULL_FACE);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);

  VistaTexture* pLookupTexture = (m_pLUT) ? (m_pLUT) : (m_pWhiteTexture);

  pLookupTexture->Bind(GL_TEXTURE0);

  switch (m_pProperties->GetDrawMode()) {
  case VistaParticleRenderingProperties::DM_SIMPLE:
    RenderLines();
    break;

  case VistaParticleRenderingProperties::DM_SMOKE:
    RenderTransparentBillboards();
    break;

  case VistaParticleRenderingProperties::DM_BILLBOARDS:
    RenderBodies(m_pBillboardShader, m_pIlluminationTexture);
    RenderCaps(m_pBillboardShader, m_pIlluminationTexture);
    break;

  case VistaParticleRenderingProperties::DM_BUMPED_BILLBOARDS:
    RenderBodies(m_pBumpedShaders[m_pProperties->GetLightingMode()], m_pTubeNormalsTexture);
    RenderCaps(m_pBumpedShaders[m_pProperties->GetLightingMode()], m_pCapNormalsTexture);
    break;

  case VistaParticleRenderingProperties::DM_BUMPED_BILLBOARDS_DEPTH_REPLACE:
    RenderCapsIntoDeapthTexture();

    m_pDepthTexture->Bind(GL_TEXTURE2);
    RenderBodies(m_pDepthShaders[m_pProperties->GetLightingMode()], m_pTubeNormalsTexture);
    m_pDepthTexture->Unbind(GL_TEXTURE2);

    RenderCaps(m_pBumpedShaders[m_pProperties->GetLightingMode()], m_pCapNormalsTexture);
    break;
  }

  if (m_pProperties->GetRenderHalos() &&
      m_pProperties->GetDrawMode() > VistaParticleRenderingProperties::DM_SMOKE)
    RenderHalos();

  pLookupTexture->Unbind(GL_TEXTURE0);

  glPopAttrib();

  VistaOGLUtils::CheckForOGLError(__FILE__, __LINE__);
}
/******************************************************************************/

float VistaParticleTraceRenderingCore::GetLineWidth() const {
  return m_fLineWidth;
}
float VistaParticleTraceRenderingCore::GetParticleRadius() const {
  return m_fParticleRadius;
}
const VistaColor& VistaParticleTraceRenderingCore::GetParticleColor() const {
  return m_oParticleColor;
}

VistaTexture* VistaParticleTraceRenderingCore::GetLookupTexture() const {
  return m_pLUT;
}
void VistaParticleTraceRenderingCore::GetLookupRange(float& fMin, float& fMax) const {
  fMin = m_aLookupRange[0];
  fMax = m_aLookupRange[1];
}

unsigned int VistaParticleTraceRenderingCore::GetPrimitiveRestartIndex() const {
  return m_nPrimitiveRestartIndex;
}

const VistaVector3D& VistaParticleTraceRenderingCore::GetViewerPosition() const {
  return m_v3ViewerPos;
}
const VistaVector3D& VistaParticleTraceRenderingCore::GetLightDirection() const {
  return m_v3LightDir;
}

/******************************************************************************/

void VistaParticleTraceRenderingCore::SetLineWidth(float fWidth) {
  m_fLineWidth = fWidth;
}

void VistaParticleTraceRenderingCore::SetParticleRadius(float fRadius) {
  m_fParticleRadius = fRadius;
}

bool VistaParticleTraceRenderingCore::SetMaxLuminanceFactor(float fFactor) {
  if (m_fMaxLuminanceFactor == fFactor)
    return false;

  m_fMaxLuminanceFactor = fFactor;
  return true;
}
bool VistaParticleTraceRenderingCore::SetLuminanceReductionFactor(float fFactor) {
  if (m_fLuminanceReductionFactor == fFactor)
    return false;

  m_fLuminanceReductionFactor = fFactor;
  return true;
}

void VistaParticleTraceRenderingCore::SetParticleColor(const VistaColor& rColor) {
  m_oParticleColor = rColor;
}

void VistaParticleTraceRenderingCore::SetLookupTexture(VistaTexture* pLUT) {
  m_pLUT = pLUT;
}
void VistaParticleTraceRenderingCore::SetLookupRange(float fMin, float fMax) {
  m_aLookupRange[0] = fMin;
  m_aLookupRange[1] = fMax;
}

void VistaParticleTraceRenderingCore::SetViewerPosition(const VistaVector3D& v3ViewerPos) {
  m_v3ViewerPos = v3ViewerPos;
}
void VistaParticleTraceRenderingCore::SetLightDirection(const VistaVector3D& v3LightDir) {
  m_v3LightDir = v3LightDir;
}

void VistaParticleTraceRenderingCore::SetPrimitiveRestartIndex(unsigned int n) {
  m_nPrimitiveRestartIndex = n;
}

void VistaParticleTraceRenderingCore::SetBodyVBO(VistaVertexArrayObject* pVAO) {
  m_pBodyVAO = pVAO;
}
void VistaParticleTraceRenderingCore::SetNumBodyIndices(unsigned int n) {
  m_nBodyIndices = n;
}

void VistaParticleTraceRenderingCore::SetCapVBO(VistaVertexArrayObject* pVAO) {
  m_pCapVAO = pVAO;
}
void VistaParticleTraceRenderingCore::SetCapCount(unsigned int n) {
  m_nCapCount = n;
}

/******************************************************************************/

bool VistaParticleTraceRenderingCore::Observes(IVistaObserveable* pObserveable) {
  return pObserveable == m_pProperties;
}
void VistaParticleTraceRenderingCore::Observe(IVistaObserveable* pObserveable, int nTicket) {
}
bool VistaParticleTraceRenderingCore::ObserveableDeleteRequest(
    IVistaObserveable* pObserveable, int nTicket) {
  return true;
}
void VistaParticleTraceRenderingCore::ObserveableDelete(
    IVistaObserveable* pObserveable, int nTicket) {
}
void VistaParticleTraceRenderingCore::ReleaseObserveable(
    IVistaObserveable* pObserveable, int nTicket) {
}

void VistaParticleTraceRenderingCore::ObserverUpdate(
    IVistaObserveable* pObserveable, int nMsg, int nTicket) {
  if (pObserveable == m_pProperties &&
      (nMsg == VistaParticleRenderingProperties::MSG_GENERATE_MIPMAPS_CHANGED ||
          nMsg == VistaParticleRenderingProperties::MSG_TEXTURE_RESOLUTION_CHANGED)) {
    UpdateTextures();
  }
}

/******************************************************************************/

bool VistaParticleTraceRenderingCore::InitShaders() {
  if (m_pLineShader)
    return false;

  m_pLineShader      = CreateShader(VPTRC_SHADER_LINES_VERT, VPTRC_SHADER_LINES_FRAG);
  m_pSmokeShader     = CreateShader(VPTRC_SHADER_SMOKE_VERT, VPTRC_SHADER_SMOKE_FRAG);
  m_pBillboardShader = CreateShader(VPTRC_SHADER_BILLBOARD_VERT, VPTRC_SHADER_BILLBOARD_FRAG);

  m_pBumpedShaders[0] = CreateShader(
      VPTRC_SHADER_BUMPED_VERT, VPTRC_SHADER_BUMPED_FRAG, VPTRC_SHADER_DIFFUSE_LIGHTING);
  m_pBumpedShaders[1] =
      CreateShader(VPTRC_SHADER_BUMPED_VERT, VPTRC_SHADER_BUMPED_FRAG, VPTRC_SHADER_PHONG_LIGHTING);
  m_pBumpedShaders[2] =
      CreateShader(VPTRC_SHADER_BUMPED_VERT, VPTRC_SHADER_BUMPED_FRAG, VPTRC_SHADER_GOOCH_LIGHTING);

  m_pDepthShaders[0] =
      CreateShader(VPTRC_SHADER_DEPTH_VERT, VPTRC_SHADER_DEPTH_FRAG, VPTRC_SHADER_DIFFUSE_LIGHTING);
  m_pDepthShaders[1] =
      CreateShader(VPTRC_SHADER_DEPTH_VERT, VPTRC_SHADER_DEPTH_FRAG, VPTRC_SHADER_PHONG_LIGHTING);
  m_pDepthShaders[2] =
      CreateShader(VPTRC_SHADER_DEPTH_VERT, VPTRC_SHADER_DEPTH_FRAG, VPTRC_SHADER_GOOCH_LIGHTING);

  m_pHaloShader = CreateShader(VPTRC_SHADER_HALO_VERT, VPTRC_SHADER_HALO_FRAG);

  if (!m_pLineShader || !m_pSmokeShader || !m_pBillboardShader || !m_pBumpedShaders[0] ||
      !m_pBumpedShaders[1] || !m_pBumpedShaders[2] || !m_pDepthShaders[0] || !m_pDepthShaders[1] ||
      !m_pDepthShaders[2] || !m_pHaloShader) {
    DeleteShaders();
    return false;
  }
  return true;
}
bool VistaParticleTraceRenderingCore::InitTextures() {
  if (m_pIlluminationTexture || m_pTubeNormalsTexture)
    return false;

  m_pBlendingTexture     = new VistaTexture(GL_TEXTURE_2D);
  m_pIlluminationTexture = new VistaTexture(GL_TEXTURE_2D);
  m_pTubeNormalsTexture  = new VistaTexture(GL_TEXTURE_2D);
  m_pCapNormalsTexture   = new VistaTexture(GL_TEXTURE_2D);

  m_pBlendingTexture->Bind();
  m_pBlendingTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pBlendingTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pBlendingTexture->SetMinFilter(GL_LINEAR);
  m_pBlendingTexture->SetMagFilter(GL_LINEAR);
  m_pBlendingTexture->Unbind();

  m_pIlluminationTexture->Bind();
  m_pIlluminationTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pIlluminationTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pIlluminationTexture->SetMinFilter(GL_LINEAR);
  m_pIlluminationTexture->SetMagFilter(GL_LINEAR);
  m_pIlluminationTexture->Unbind();

  m_pTubeNormalsTexture->Bind();
  m_pTubeNormalsTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pTubeNormalsTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pTubeNormalsTexture->SetMinFilter(GL_LINEAR);
  m_pTubeNormalsTexture->SetMagFilter(GL_LINEAR);
  m_pTubeNormalsTexture->Unbind();

  m_pCapNormalsTexture->Bind();
  m_pCapNormalsTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pCapNormalsTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pCapNormalsTexture->SetMinFilter(GL_LINEAR);
  m_pCapNormalsTexture->SetMagFilter(GL_LINEAR);
  m_pCapNormalsTexture->Unbind();

  VistaColor colWhite(VistaColor::WHITE);
  m_pWhiteTexture = new VistaTexture(GL_TEXTURE_1D);

  m_pWhiteTexture->Bind();
  m_pWhiteTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pWhiteTexture->SetMinFilter(GL_NEAREST);
  m_pWhiteTexture->SetMagFilter(GL_NEAREST);
  m_pWhiteTexture->UploadTexture(1, 1, &colWhite[0], false, GL_RGBA, GL_FLOAT);
  m_pWhiteTexture->Unbind();

  UpdateTextures();

  return true;
}

VistaGLSLShader* VistaParticleTraceRenderingCore::CreateShader(
    const std::string& strVertexShaderName, const std::string& strFragmentShaderName,
    const std::string& strLingtingShaderName) {
  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  string strVert, strFrag, strLighting;

  strVert = rShaderReg.RetrieveShader(strVertexShaderName);
  strFrag = rShaderReg.RetrieveShader(strFragmentShaderName);

  if (!strLingtingShaderName.empty())
    strLighting = rShaderReg.RetrieveShader(strLingtingShaderName);

  if (strVert.empty() || strFrag.empty() ||
      (!strLingtingShaderName.empty() && strLighting.empty())) {
    vstr::errp() << "[VistaTubeletRenderingCore] can't find required shaders" << endl;
    vstr::IndentObject oIndent;

    if (strVert.empty())
      vstr::erri() << "can't find " << strVertexShaderName << endl;
    if (strFrag.empty())
      vstr::erri() << "can't find " << strFragmentShaderName << endl;

    if (!strLingtingShaderName.empty() && strLighting.empty())
      if (strFrag.empty())
        vstr::erri() << "can't find " << strLingtingShaderName << endl;

    return NULL;
  }

  VistaGLSLShader* pShader = new VistaGLSLShader;

  pShader->InitFromStrings(strVert, strFrag);
  if (!strLingtingShaderName.empty())
    pShader->InitFragmentShaderFromString(strLighting);

  if (!pShader->Link()) {
    delete pShader;
    return NULL;
  }

  pShader->Bind();

  int iUniformLocation;

  iUniformLocation = pShader->GetUniformLocation("u_texLookupTable");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 0);
  iUniformLocation = pShader->GetUniformLocation("u_texBillboard");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 1);
  iUniformLocation = pShader->GetUniformLocation("u_texDepth");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 2);

  iUniformLocation = pShader->GetUniformLocation("u_v4LightingCoeffs");
  if (iUniformLocation >= 0)
    glUniform4f(iUniformLocation, 0.1f, 0.6f, 1.0f, 32.0f);

  pShader->Release();
  return pShader;
}

void VistaParticleTraceRenderingCore::DeleteShaders() {
  delete m_pLineShader;
  m_pLineShader = NULL;

  delete m_pSmokeShader;
  m_pSmokeShader = NULL;

  delete m_pBillboardShader;
  m_pBillboardShader = NULL;

  for (int i = 0; i < 3; ++i) {
    delete m_pBumpedShaders[i];
    m_pBumpedShaders[i] = NULL;

    delete m_pDepthShaders[i];
    m_pDepthShaders[i] = NULL;
  }

  delete m_pHaloShader;
  m_pHaloShader = NULL;
}

void VistaParticleTraceRenderingCore::UpdateTextures() {
  GLenum eMinFilter = m_pProperties->GetGenerateMipmaps() ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  int iResolution = m_pProperties->GetTextureResolution();

  if (m_pBlendingTexture) {
    float* pData = VistaOGLUtils::CreateGaussianBlendingTextureData(iResolution, iResolution);

    m_pBlendingTexture->Bind();
    glTexImage2D(m_pBlendingTexture->GetTarget(), 0, GL_ALPHA, iResolution, iResolution, 0,
        GL_ALPHA, GL_FLOAT, pData);
    if (m_pProperties->GetGenerateMipmaps())
      glGenerateMipmap(m_pBlendingTexture->GetTarget());
    m_pBlendingTexture->SetMinFilter(eMinFilter);
    m_pBlendingTexture->Unbind();

    VistaOGLUtils::Delete(pData);
  }

  if (m_pIlluminationTexture) {
    float* pData = VistaOGLUtils::CreateSphereIlluminationTextureData(iResolution, iResolution);

    m_pIlluminationTexture->Bind();
    glTexImage2D(m_pIlluminationTexture->GetTarget(), 0, GL_LUMINANCE_ALPHA, iResolution,
        iResolution, 0, GL_LUMINANCE_ALPHA, GL_FLOAT, pData);
    if (m_pProperties->GetGenerateMipmaps())
      glGenerateMipmap(m_pIlluminationTexture->GetTarget());
    m_pIlluminationTexture->SetMinFilter(eMinFilter);
    m_pIlluminationTexture->Unbind();

    VistaOGLUtils::Delete(pData);
  }

  if (m_pTubeNormalsTexture) {
    float* pData = VistaOGLUtils::CreateTubeNormalsTextureData(iResolution, iResolution, true);

    m_pTubeNormalsTexture->Bind();
    glTexImage2D(m_pTubeNormalsTexture->GetTarget(), 0, GL_RGBA, iResolution, iResolution, 0,
        GL_RGBA, GL_FLOAT, pData);
    if (m_pProperties->GetGenerateMipmaps())
      glGenerateMipmap(m_pTubeNormalsTexture->GetTarget());
    m_pTubeNormalsTexture->SetMinFilter(eMinFilter);
    m_pTubeNormalsTexture->Unbind();

    VistaOGLUtils::Delete(pData);
  }

  if (m_pCapNormalsTexture) {
    float* pData = VistaOGLUtils::CreateTubeNormalsTextureData(iResolution, iResolution, true);

    for (int i = 0; i < iResolution * iResolution; ++i) {
      pData[4 * i + 0] = 1.0f;
      pData[4 * i + 1] = 0.5f;
      pData[4 * i + 2] = 0.5f;
    }

    m_pCapNormalsTexture->Bind();
    glTexImage2D(m_pCapNormalsTexture->GetTarget(), 0, GL_RGBA, iResolution, iResolution, 0,
        GL_RGBA, GL_FLOAT, pData);
    if (m_pProperties->GetGenerateMipmaps())
      glGenerateMipmap(m_pCapNormalsTexture->GetTarget());
    m_pCapNormalsTexture->SetMinFilter(eMinFilter);
    m_pCapNormalsTexture->Unbind();

    VistaOGLUtils::Delete(pData);
  }
}
void VistaParticleTraceRenderingCore::UpdateUniformVariables(VistaGLSLShader* pShader) {
  if (!pShader)
    return;

  int iUniformLocation;

  iUniformLocation = pShader->GetUniformLocation("u_v4ViewerPos");
  if (iUniformLocation >= 0)
    glUniform4fv(iUniformLocation, 1, &m_v3ViewerPos[0]);

  iUniformLocation = pShader->GetUniformLocation("u_fRadius");
  if (iUniformLocation >= 0)
    glUniform1f(iUniformLocation, m_fParticleRadius);

  iUniformLocation = pShader->GetUniformLocation("u_v4ParticleColor");
  if (iUniformLocation >= 0) {
    if (!m_pLUT)
      glUniform4fv(iUniformLocation, 1, &m_oParticleColor[0]);
    else
      glUniform4f(iUniformLocation, 1.0f, 1.0f, 1.0f, 1.0f);
  }

  iUniformLocation = pShader->GetUniformLocation("u_v2RangeParams");
  if (iUniformLocation >= 0) {
    glUniform2f(
        iUniformLocation, m_aLookupRange[0], 1.0f / (m_aLookupRange[1] - m_aLookupRange[0]));
  }

  iUniformLocation = pShader->GetUniformLocation("u_fStartTime");
  if (iUniformLocation >= 0)
    glUniform1f(iUniformLocation, m_fMaxLuminanceFactor);

  iUniformLocation = pShader->GetUniformLocation("u_fLightReductionFactor");
  if (iUniformLocation >= 0)
    glUniform1f(iUniformLocation, m_fLuminanceReductionFactor);

  iUniformLocation = pShader->GetUniformLocation("u_v4LightDir");
  if (iUniformLocation >= 0)
    glUniform4fv(iUniformLocation, 1, &m_v3LightDir[0]);

  iUniformLocation = pShader->GetUniformLocation("u_fHaloSize");
  if (iUniformLocation >= 0)
    glUniform1f(iUniformLocation, m_pProperties->GetHaloSize());

  iUniformLocation = pShader->GetUniformLocation("u_v4HaloColor");
  if (iUniformLocation >= 0)
    glUniform4fv(iUniformLocation, 1, &m_pProperties->GetHaloColor()[0]);
}

void VistaParticleTraceRenderingCore::RenderLines() {
  if (!m_pLineShader)
    return;

  glLineWidth(m_fLineWidth);

  m_pLineShader->Bind();
  UpdateUniformVariables(m_pLineShader);

  m_pBodyVAO->Bind();
  glDrawElements(GL_LINE_STRIP, m_nBodyIndices, GL_UNSIGNED_INT, 0);
  m_pBodyVAO->Release();

  m_pLineShader->Release();
}
void VistaParticleTraceRenderingCore::RenderBodies(
    VistaGLSLShader* pShader, VistaTexture* pTexture) {
  if (!pShader || !pTexture)
    return;

  pTexture->Bind(GL_TEXTURE1);

  pShader->Bind();
  UpdateUniformVariables(pShader);

  m_pBodyVAO->Bind();
  glDrawElements(GL_TRIANGLE_STRIP, m_nBodyIndices, GL_UNSIGNED_INT, 0);
  m_pBodyVAO->Release();

  pShader->Release();

  pTexture->Unbind(GL_TEXTURE1);
}
void VistaParticleTraceRenderingCore::RenderCaps(VistaGLSLShader* pShader, VistaTexture* pTexture) {
  if (!pShader || !pTexture)
    return;

  pTexture->Bind(GL_TEXTURE1);

  pShader->Bind();
  UpdateUniformVariables(pShader);

  m_pCapVAO->Bind();
  if (m_pCapVAO->GetIsIndexBufferObjectSpecifed())
    glDrawElements(GL_QUADS, 4 * m_nCapCount, m_pCapVAO->GetIndexBufferObjectType(), 0);
  else
    glDrawArrays(GL_QUADS, 0, 4 * m_nCapCount);
  m_pCapVAO->Release();

  pShader->Release();

  pTexture->Unbind(GL_TEXTURE1);
}
void VistaParticleTraceRenderingCore::RenderCapsIntoDeapthTexture() {
  if (!m_pHaloShader || !m_pBillboardShader || !m_pIlluminationTexture)
    return;

  int aViewport[4];
  glGetIntegerv(GL_VIEWPORT, aViewport);

  if (aViewport[2] > m_aDepthTextureSize[0] || aViewport[3] > m_aDepthTextureSize[1]) {
    m_aDepthTextureSize[0] = aViewport[2];
    m_aDepthTextureSize[1] = aViewport[3];

    m_pDepthTexture->Bind();
    glTexImage2D(m_pDepthTexture->GetTarget(), 0, GL_DEPTH_COMPONENT, m_aDepthTextureSize[0],
        m_aDepthTextureSize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    m_pDepthTexture->Unbind();
  }

  glViewport(0, 0, m_aDepthTextureSize[0], m_aDepthTextureSize[1]);

  m_pFBO->Bind();

  glClear(GL_DEPTH_BUFFER_BIT);

  // render caps
  RenderCaps(m_pBillboardShader, m_pIlluminationTexture);
  if (m_pProperties->GetRenderHalos())
    RenderCaps(m_pHaloShader, m_pIlluminationTexture);

  m_pFBO->Release();

  glViewport(aViewport[0], aViewport[1], aViewport[2], aViewport[3]);

//#define VISTATUBELETRENDERINGCORE_DEBUG_DEPTH_TEXTURE
#ifdef VISTATUBELETRENDERINGCORE_DEBUG_DEPTH_TEXTURE
  // Some debug code to show the entry & exit point textures
  // The include right in front of this function is required for this code to work
  VistaOGLUtils::BeginOrtho2D(0.0, 1.0, 0.0, 1.0);

  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_RECTANGLE);
  // glDisable( GL_LIGHTING );
  glDisable(GL_DEPTH_TEST);

  m_pDepthTexture->Bind();
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0);
  glVertex2f(0.8f, 0.0f);
  glTexCoord2i(m_aDepthTextureSize[0], 0);
  glVertex2f(1.0f, 0.0f);
  glTexCoord2i(m_aDepthTextureSize[0], m_aDepthTextureSize[1]);
  glVertex2f(1.0f, 0.2f);
  glTexCoord2i(0, m_aDepthTextureSize[1]);
  glVertex2f(0.8f, 0.2f);
  glEnd();
  m_pDepthTexture->Unbind();

  glPopAttrib();

  VistaOGLUtils::EndOrtho();
#endif // Is VISTATUBELETRENDERINGCORE_DEBUG_DEPTH_TEXTURE defined?
}

void VistaParticleTraceRenderingCore::RenderTransparentBillboards() {
  glDepthMask(false);
  glDisable(GL_ALPHA_TEST);
  glEnable(GL_BLEND);

  if (m_pProperties->GetBlendingMode() == VistaParticleRenderingProperties::ADDITIVE_BLENDING)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  else
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_pBlendingTexture->Bind(GL_TEXTURE1);

  m_pSmokeShader->Bind();
  UpdateUniformVariables(m_pSmokeShader);

  m_pBodyVAO->Bind();
  glDrawElements(GL_TRIANGLE_STRIP, m_nBodyIndices, GL_UNSIGNED_INT, 0);
  m_pBodyVAO->Release();

  m_pSmokeShader->Release();

  m_pBlendingTexture->Unbind(GL_TEXTURE1);

  glDepthMask(true);
}

void VistaParticleTraceRenderingCore::RenderHalos() {
  m_pIlluminationTexture->Bind(GL_TEXTURE1);
  m_pHaloShader->Bind();
  UpdateUniformVariables(m_pHaloShader);

  m_pBodyVAO->Bind();
  glDrawElements(GL_TRIANGLE_STRIP, m_nBodyIndices, GL_UNSIGNED_INT, 0);
  m_pBodyVAO->Release();

  m_pCapVAO->Bind();
  if (m_pCapVAO->GetIsIndexBufferObjectSpecifed())
    glDrawElements(GL_QUADS, 4 * m_nCapCount, m_pCapVAO->GetIndexBufferObjectType(), 0);
  else
    glDrawArrays(GL_QUADS, 0, 4 * m_nCapCount);
  m_pCapVAO->Release();

  m_pHaloShader->Release();
  m_pIlluminationTexture->Unbind(GL_TEXTURE1);
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
