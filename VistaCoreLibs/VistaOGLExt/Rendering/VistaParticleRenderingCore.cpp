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
#include "VistaParticleRenderingCore.h"
#include "VistaParticleRenderingProperties.h"

#include <VistaBase/VistaStreamUtils.h>

#include "../VistaBufferObject.h"
#include "../VistaGLSLShader.h"
#include "../VistaOGLUtils.h"
#include "../VistaShaderRegistry.h"
#include "../VistaTexture.h"
#include "../VistaVertexArrayObject.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
const char* VPRC_SHADER_POINTS_VERT         = "VflParticleRenderingCore_Points_vert.glsl";
const char* VPRC_SHADER_POINTS_FRAG         = "VflParticleRenderingCore_Points_frag.glsl";
const char* VPRC_SHADER_SMOKE_VERT          = "VflParticleRenderingCore_Smoke_vert.glsl";
const char* VPRC_SHADER_SMOKE_FRAG          = "VflParticleRenderingCore_Smoke_frag.glsl";
const char* VPRC_SHADER_BILLBOARDS_VERT     = "VflParticleRenderingCore_Billboards_vert.glsl";
const char* VPRC_SHADER_BILLBOARDS_FRAG     = "VflParticleRenderingCore_Billboards_frag.glsl";
const char* VPRC_SHADER_LIT_BILLBOARDS_VERT = "VflParticleRenderingCore_LitBillboards_vert.glsl";
const char* VPRC_SHADER_LIT_BILLBOARDS_FRAG = "VflParticleRenderingCore_LitBillboards_frag.glsl";
const char* VPRC_SHADER_DEPTH_SPRITES_VERT  = "VflParticleRenderingCore_DepthSprites_vert.glsl";
const char* VPRC_SHADER_DEPTH_SPRITES_FRAG  = "VflParticleRenderingCore_DepthSprites_frag.glsl";

const char* VPRC_SHADER_HALO_VERT = "VflParticleRenderingCore_Halo_vert.glsl";
const char* VPRC_SHADER_HALO_FRAG = "VflParticleRenderingCore_Halo_frag.glsl";

const char* VPRC_SHADER_DIFFUSE_LIGHTING = "VflParticleRenderingCore_Lighting_Diffuse_aux.glsl";
const char* VPRC_SHADER_PHONG_LIGHTING   = "VflParticleRenderingCore_Lighting_Phong_aux.glsl";
const char* VPRC_SHADER_GOOCH_LIGHTING   = "VflParticleRenderingCore_Lighting_Gooch_aux.glsl";

using namespace std;
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaParticleRenderingCore::VistaParticleRenderingCore()
    : m_pProperties(new VistaParticleRenderingProperties())
    , m_fPointSize(2.0f)
    , m_fParticleRadius(0.1f)
    , m_pLookUpTexture(NULL)
    , m_pParticleDataTexture(NULL)
    , m_pParticleRadiiTexture(NULL)
    , m_pParticleMappingTexture(NULL)
    , m_iParticleCount(0)
    , m_pPointShader(NULL)
    , m_pVBO(NULL)
    , m_pVAO(NULL)
    , m_pBillboardShader(NULL)
    , m_pSmokeShader(NULL)
    , m_pHaloShader(NULL)
    , m_pSphereIlluminationTexture(NULL)
    , m_pGaussianBlendingTexture(NULL)
    , m_pSphereNormalsTexture(NULL)
    , m_pDefaultLookUpTexture(NULL)
    , m_pDefaultParticleRadiiTexture(NULL) {
  m_pLitBBShaders[0] = NULL;
  m_pLitBBShaders[1] = NULL;
  m_pLitBBShaders[2] = NULL;

  m_pDepthShaders[0] = NULL;
  m_pDepthShaders[1] = NULL;
  m_pDepthShaders[2] = NULL;

  m_aLookupRange[0] = 0.0f;
  m_aLookupRange[1] = 1.0f;

  m_aParticleDataTextureSize[0] = 1;
  m_aParticleDataTextureSize[1] = 1;

  m_pProperties->AttachObserver(this);
}

VistaParticleRenderingCore::~VistaParticleRenderingCore() {
  m_pProperties->DetachObserver(this);
  delete m_pProperties;

  // delete Shaders
  delete m_pPointShader;
  delete m_pSmokeShader;
  delete m_pBillboardShader;

  delete m_pLitBBShaders[0];
  delete m_pLitBBShaders[1];
  delete m_pLitBBShaders[2];

  delete m_pDepthShaders[0];
  delete m_pDepthShaders[1];
  delete m_pDepthShaders[2];

  delete m_pHaloShader;

  // delete Textures
  delete m_pSphereIlluminationTexture;
  delete m_pGaussianBlendingTexture;
  delete m_pSphereNormalsTexture;

  delete m_pDefaultLookUpTexture;
  delete m_pDefaultParticleRadiiTexture;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaParticleRenderingCore::Init() {
  const float aVertices[] = {
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
  };

  m_pVBO = new VistaBufferObject;
  m_pVBO->BindAsVertexDataBuffer();
  m_pVBO->BufferData(2 * 4 * sizeof(float), aVertices, GL_STATIC_DRAW);
  m_pVBO->Release();

  m_pVAO = new VistaVertexArrayObject;
  m_pVAO->Bind();
  m_pVAO->EnableAttributeArray(0);
  m_pVAO->SpecifyAttributeArrayFloat(0, 2, GL_FLOAT, false, 0, 0, m_pVBO);
  m_pVAO->Release();

  if (!InitShaders())
    return false;

  if (!InitTextures())
    return false;

  return true;
}

void VistaParticleRenderingCore::Draw() {
  if (!m_pVAO || !m_pParticleDataTexture || m_iParticleCount == 0 ||
      m_aParticleDataTextureSize[0] <= 0 || m_aParticleDataTextureSize[1] <= 0)
    return;

  glPushAttrib(GL_ENABLE_BIT);

  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);

  VistaTexture* pRadiiTexture =
      (m_pParticleRadiiTexture) ? (m_pParticleRadiiTexture) : (m_pDefaultParticleRadiiTexture);
  VistaTexture* pLookupTexture =
      (m_pLookUpTexture) ? (m_pLookUpTexture) : (m_pDefaultLookUpTexture);

  m_pParticleDataTexture->Bind(GL_TEXTURE0);
  pRadiiTexture->Bind(GL_TEXTURE1);
  pLookupTexture->Bind(GL_TEXTURE2);

  m_pVAO->Bind();

  switch (m_pProperties->GetDrawMode()) {
  case VistaParticleRenderingProperties::DM_SIMPLE:
    RenderPoints();
    break;

  case VistaParticleRenderingProperties::DM_BILLBOARDS:
    RenderBillboards(m_pBillboardShader, m_pSphereIlluminationTexture);
    break;

  case VistaParticleRenderingProperties::DM_SMOKE:
    RenderTransparentBillboards();
    break;

  case VistaParticleRenderingProperties::DM_BUMPED_BILLBOARDS:
    RenderBillboards(m_pLitBBShaders[m_pProperties->GetLightingMode()], m_pSphereNormalsTexture);
    break;

  case VistaParticleRenderingProperties::DM_BUMPED_BILLBOARDS_DEPTH_REPLACE:
    RenderBillboards(m_pDepthShaders[m_pProperties->GetLightingMode()], m_pSphereNormalsTexture);
    break;
  }

  m_pVAO->Release();

  pLookupTexture->Unbind(GL_TEXTURE2);
  pRadiiTexture->Unbind(GL_TEXTURE1);
  m_pParticleDataTexture->Unbind(GL_TEXTURE0);

  glPopAttrib();

  VistaOGLUtils::CheckForOGLError(__FILE__, __LINE__);
}
/******************************************************************************/

VistaParticleRenderingProperties* VistaParticleRenderingCore::GetProperties() {
  return m_pProperties;
}

float VistaParticleRenderingCore::GetPointSize() const {
  return m_fPointSize;
}
float VistaParticleRenderingCore::GetParticleRadiusScale() const {
  return m_fParticleRadius;
}

const VistaColor& VistaParticleRenderingCore::GetParticleColor() const {
  return m_oParticleColor;
}

VistaTexture* VistaParticleRenderingCore::GetLookupTexture() const {
  return m_pLookUpTexture;
}
void VistaParticleRenderingCore::GetLookupRange(float& fMin, float& fMax) const {
  fMin = m_aLookupRange[0];
  fMax = m_aLookupRange[1];
}

VistaTexture* VistaParticleRenderingCore::GetDataTexture() const {
  return m_pParticleDataTexture;
}
VistaTexture* VistaParticleRenderingCore::GetRadiiTexture() const {
  return m_pParticleRadiiTexture;
}
VistaTexture* VistaParticleRenderingCore::GetMappingTexture() const {
  return m_pParticleMappingTexture;
}
void VistaParticleRenderingCore::GetDataTextureSize(int& iWidth, int& iHeight) const {
  iWidth  = m_aParticleDataTextureSize[0];
  iHeight = m_aParticleDataTextureSize[0];
}

unsigned int VistaParticleRenderingCore::GetParticleCount() const {
  return m_iParticleCount;
}
const VistaVector3D& VistaParticleRenderingCore::GetViewerPosition() const {
  return m_v3ViewerPos;
}
const VistaVector3D& VistaParticleRenderingCore::GetLightDirection() const {
  return m_v3LightDir;
}

/******************************************************************************/

void VistaParticleRenderingCore::SetPointSize(float fSize) {
  m_fPointSize = fSize;
}
void VistaParticleRenderingCore::SetParticleRadiusScale(float fRadius) {
  m_fParticleRadius = fRadius;
}

void VistaParticleRenderingCore::SetParticleColor(const VistaColor& rColor) {
  m_oParticleColor = rColor;
}

void VistaParticleRenderingCore::SetLookupTexture(VistaTexture* pLUT) {
  m_pLookUpTexture = pLUT;
}
void VistaParticleRenderingCore::SetLookupRange(float fMin, float fMax) {
  m_aLookupRange[0] = fMin;
  m_aLookupRange[1] = fMax;
}

void VistaParticleRenderingCore::SetDataTexture(VistaTexture* pDataTexture) {
  m_pParticleDataTexture = pDataTexture;
}
void VistaParticleRenderingCore::SetRadiiTexture(VistaTexture* pRadiiTexture) {
  m_pParticleRadiiTexture = pRadiiTexture;
}
void VistaParticleRenderingCore::SetMappingTexture(VistaTexture* pMappingTexture) {
  m_pParticleMappingTexture = pMappingTexture;
}
void VistaParticleRenderingCore::SetDataTextureSize(int iWidth, int iHeight) {
  m_aParticleDataTextureSize[0] = iWidth;
  m_aParticleDataTextureSize[1] = iHeight;
}
void VistaParticleRenderingCore::SetParticleCount(unsigned int iNumParticles) {
  m_iParticleCount = iNumParticles;
}
void VistaParticleRenderingCore::SetViewerPosition(const VistaVector3D& v3ViewerPos) {
  m_v3ViewerPos = v3ViewerPos;
}
void VistaParticleRenderingCore::SetLightDirection(const VistaVector3D& v3LightDir) {
  m_v3LightDir = v3LightDir;
}

/******************************************************************************/

bool VistaParticleRenderingCore::Observes(IVistaObserveable* pObserveable) {
  return pObserveable == m_pProperties;
}
void VistaParticleRenderingCore::Observe(IVistaObserveable* pObserveable, int nTicket) {
}
bool VistaParticleRenderingCore::ObserveableDeleteRequest(
    IVistaObserveable* pObserveable, int nTicket) {
  return true;
}
void VistaParticleRenderingCore::ObserveableDelete(IVistaObserveable* pObserveable, int nTicket) {
}
void VistaParticleRenderingCore::ReleaseObserveable(IVistaObserveable* pObserveable, int nTicket) {
}

void VistaParticleRenderingCore::ObserverUpdate(
    IVistaObserveable* pObserveable, int nMsg, int nTicket) {
  if (pObserveable == m_pProperties &&
      (nMsg == VistaParticleRenderingProperties::MSG_TEXTURE_RESOLUTION_CHANGED ||
          nMsg == VistaParticleRenderingProperties::MSG_GENERATE_MIPMAPS_CHANGED)) {
    UpdateTextures();
  }
}

/******************************************************************************/

bool VistaParticleRenderingCore::InitShaders() {
  if (m_pPointShader || m_pSmokeShader || m_pBillboardShader || m_pLitBBShaders[0] ||
      m_pLitBBShaders[1] || m_pLitBBShaders[2] || m_pDepthShaders[0] || m_pDepthShaders[1] ||
      m_pDepthShaders[2])
    return false;

  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  string strPointVert = rShaderReg.RetrieveShader(VPRC_SHADER_POINTS_VERT);
  string strPointFrag = rShaderReg.RetrieveShader(VPRC_SHADER_POINTS_FRAG);
  string strSmokeVert = rShaderReg.RetrieveShader(VPRC_SHADER_SMOKE_VERT);
  string strSmokeFrag = rShaderReg.RetrieveShader(VPRC_SHADER_SMOKE_FRAG);
  string strBBVert    = rShaderReg.RetrieveShader(VPRC_SHADER_BILLBOARDS_VERT);
  string strBBFrag    = rShaderReg.RetrieveShader(VPRC_SHADER_BILLBOARDS_FRAG);
  string strLitVert   = rShaderReg.RetrieveShader(VPRC_SHADER_LIT_BILLBOARDS_VERT);
  string strLitFrag   = rShaderReg.RetrieveShader(VPRC_SHADER_LIT_BILLBOARDS_FRAG);
  string strDepthVert = rShaderReg.RetrieveShader(VPRC_SHADER_DEPTH_SPRITES_VERT);
  string strDepthFrag = rShaderReg.RetrieveShader(VPRC_SHADER_DEPTH_SPRITES_FRAG);

  string strHaloVert = rShaderReg.RetrieveShader(VPRC_SHADER_HALO_VERT);
  string strHaloFrag = rShaderReg.RetrieveShader(VPRC_SHADER_HALO_FRAG);

  string strLighting[3] = {rShaderReg.RetrieveShader(VPRC_SHADER_DIFFUSE_LIGHTING),
      rShaderReg.RetrieveShader(VPRC_SHADER_PHONG_LIGHTING),
      rShaderReg.RetrieveShader(VPRC_SHADER_GOOCH_LIGHTING)};

  if (strPointVert.empty() || strPointFrag.empty() || strSmokeVert.empty() ||
      strSmokeFrag.empty() || strBBVert.empty() || strBBFrag.empty() || strLitVert.empty() ||
      strLitFrag.empty() || strDepthVert.empty() || strDepthFrag.empty() || strHaloVert.empty() ||
      strHaloFrag.empty() || strLighting[0].empty() || strLighting[1].empty() ||
      strLighting[2].empty()) {
    vstr::errp() << "[VflParticleRendereringCore] can't find required shaders" << endl;
    vstr::IndentObject oIndent;

    if (strPointVert.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_POINTS_VERT << endl;
    if (strPointFrag.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_POINTS_FRAG << endl;
    if (strSmokeVert.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_SMOKE_VERT << endl;
    if (strSmokeFrag.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_SMOKE_FRAG << endl;
    if (strBBVert.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_BILLBOARDS_VERT << endl;
    if (strBBFrag.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_BILLBOARDS_FRAG << endl;
    if (strLitVert.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_LIT_BILLBOARDS_VERT << endl;
    if (strLitFrag.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_LIT_BILLBOARDS_FRAG << endl;
    if (strDepthVert.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_DEPTH_SPRITES_VERT << endl;
    if (strDepthFrag.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_DEPTH_SPRITES_FRAG << endl;

    if (strHaloVert.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_HALO_VERT << endl;
    if (strHaloFrag.empty())
      vstr::erri() << "can't find " << VPRC_SHADER_HALO_FRAG << endl;

    if (strLighting[0].empty())
      vstr::erri() << "can't find " << VPRC_SHADER_DIFFUSE_LIGHTING << endl;
    if (strLighting[1].empty())
      vstr::erri() << "can't find " << VPRC_SHADER_PHONG_LIGHTING << endl;
    if (strLighting[2].empty())
      vstr::erri() << "can't find " << VPRC_SHADER_GOOCH_LIGHTING << endl;
    return false;
  }
  bool bSucsess = true;

  vstr::debugi() << "[VflParticleRendereringCore] creating PointShader" << endl;
  m_pPointShader = new VistaGLSLShader;
  m_pPointShader->InitFromStrings(strPointVert, strPointFrag);
  bSucsess &= m_pPointShader->Link();

  vstr::debugi() << "[VflParticleRendereringCore] creating SmokeShader" << endl;
  m_pSmokeShader = new VistaGLSLShader;
  m_pSmokeShader->InitFromStrings(strSmokeVert, strSmokeFrag);
  bSucsess &= m_pSmokeShader->Link();

  vstr::debugi() << "[VflParticleRendereringCore] creating BillboardShader" << endl;
  m_pBillboardShader = new VistaGLSLShader;
  m_pBillboardShader->InitFromStrings(strBBVert, strBBFrag);
  bSucsess &= m_pBillboardShader->Link();

  for (int i = 0; i < 3; ++i) {
    vstr::debugi() << "[VflParticleRendereringCore] creating LitBBShader" << i << endl;
    m_pLitBBShaders[i] = new VistaGLSLShader;
    m_pLitBBShaders[i]->InitFromStrings(strLitVert, strLitFrag);
    m_pLitBBShaders[i]->InitFragmentShaderFromString(strLighting[i]);
    bSucsess &= m_pLitBBShaders[i]->Link();

    vstr::debugi() << "[VflParticleRendereringCore] creating DepthShader" << i << endl;
    m_pDepthShaders[i] = new VistaGLSLShader;
    m_pDepthShaders[i]->InitFromStrings(strDepthVert, strDepthFrag);
    m_pDepthShaders[i]->InitFragmentShaderFromString(strLighting[i]);
    bSucsess &= m_pDepthShaders[i]->Link();
  }

  m_pHaloShader = new VistaGLSLShader;
  m_pHaloShader->InitFromStrings(strHaloVert, strHaloFrag);
  bSucsess &= m_pHaloShader->Link();

  if (!bSucsess) {
    vstr::errp() << "[VflParticleRendereringCore] can't link shader" << endl;
    delete m_pPointShader;
    m_pPointShader = NULL;

    delete m_pBillboardShader;
    m_pBillboardShader = NULL;

    delete m_pSmokeShader;
    m_pSmokeShader = NULL;

    for (int i = 0; i < 3; ++i) {
      delete m_pLitBBShaders[i];
      m_pLitBBShaders[i] = NULL;

      delete m_pDepthShaders[i];
      m_pDepthShaders[i] = NULL;
    }

    delete m_pHaloShader;
    m_pHaloShader = NULL;

    return false;
  }

  InitUniformVariables(m_pPointShader);

  InitUniformVariables(m_pSmokeShader);
  InitUniformVariables(m_pBillboardShader);

  for (int i = 0; i < 3; ++i) {
    InitUniformVariables(m_pLitBBShaders[i]);
    InitUniformVariables(m_pDepthShaders[i]);
  }

  InitUniformVariables(m_pHaloShader);

  return true;
}

bool VistaParticleRenderingCore::InitTextures() {
  if (m_pSphereIlluminationTexture || m_pGaussianBlendingTexture || m_pSphereNormalsTexture)
    return false;

  m_pSphereIlluminationTexture = new VistaTexture(GL_TEXTURE_2D);
  m_pGaussianBlendingTexture   = new VistaTexture(GL_TEXTURE_2D);
  m_pSphereNormalsTexture      = new VistaTexture(GL_TEXTURE_2D);

  m_pSphereIlluminationTexture->Bind();
  m_pSphereIlluminationTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pSphereIlluminationTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pSphereIlluminationTexture->SetMinFilter(GL_LINEAR);
  m_pSphereIlluminationTexture->SetMagFilter(GL_LINEAR);
  m_pSphereIlluminationTexture->Unbind();

  m_pGaussianBlendingTexture->Bind();
  m_pGaussianBlendingTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pGaussianBlendingTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pGaussianBlendingTexture->SetMinFilter(GL_LINEAR);
  m_pGaussianBlendingTexture->SetMagFilter(GL_LINEAR);
  m_pGaussianBlendingTexture->Unbind();

  m_pSphereNormalsTexture->Bind();
  m_pSphereNormalsTexture->SetWrapR(GL_CLAMP_TO_EDGE);
  m_pSphereNormalsTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pSphereNormalsTexture->SetMinFilter(GL_LINEAR);
  m_pSphereNormalsTexture->SetMagFilter(GL_LINEAR);
  m_pSphereNormalsTexture->Unbind();

  static VistaColor colWhite(VistaColor::WHITE);
  m_pDefaultLookUpTexture = new VistaTexture(GL_TEXTURE_1D);

  m_pDefaultLookUpTexture->Bind();
  m_pDefaultLookUpTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pDefaultLookUpTexture->SetMinFilter(GL_NEAREST);
  m_pDefaultLookUpTexture->SetMagFilter(GL_NEAREST);
  m_pDefaultLookUpTexture->UploadTexture(1, 1, &colWhite[0], false, GL_RGBA, GL_FLOAT);
  m_pDefaultLookUpTexture->Unbind();

  m_pDefaultParticleRadiiTexture = new VistaTexture(GL_TEXTURE_2D);

  m_pDefaultParticleRadiiTexture->Bind();
  m_pDefaultParticleRadiiTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  m_pDefaultParticleRadiiTexture->SetWrapT(GL_CLAMP_TO_EDGE);
  m_pDefaultParticleRadiiTexture->SetMinFilter(GL_NEAREST);
  m_pDefaultParticleRadiiTexture->SetMagFilter(GL_NEAREST);
  m_pDefaultParticleRadiiTexture->UploadTexture(1, 1, &colWhite[0], false, GL_RGBA, GL_FLOAT);
  m_pDefaultParticleRadiiTexture->Unbind();

  UpdateTextures();

  return true;
}

void VistaParticleRenderingCore::InitUniformVariables(VistaGLSLShader* pShader) {
  if (!pShader)
    return;

  pShader->Bind();

  int iUniformLocation;

  iUniformLocation = pShader->GetUniformLocation("u_texParticleData");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 0);
  iUniformLocation = pShader->GetUniformLocation("u_texParticleRadii");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 1);
  iUniformLocation = pShader->GetUniformLocation("u_texLookupTable");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 2);
  iUniformLocation = pShader->GetUniformLocation("u_texBillboard");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 3);
  iUniformLocation = pShader->GetUniformLocation("u_texParticleMapping");
  if (iUniformLocation >= 0)
    glUniform1i(iUniformLocation, 4);

  iUniformLocation = pShader->GetUniformLocation("u_v4LightingCoeffs");
  if (iUniformLocation >= 0)
    glUniform4f(iUniformLocation, 0.1f, 0.6f, 1.0f, 32.0f);

  pShader->Release();
}

void VistaParticleRenderingCore::UpdateTextures() {
  GLenum eMinFilter = m_pProperties->GetGenerateMipmaps() ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  int iResolution = m_pProperties->GetTextureResolution();

  if (m_pSphereIlluminationTexture) {
    float* pData = VistaOGLUtils::CreateSphereIlluminationTextureData(iResolution, iResolution);

    m_pSphereIlluminationTexture->Bind();
    glTexImage2D(m_pSphereIlluminationTexture->GetTarget(), 0, GL_LUMINANCE_ALPHA, iResolution,
        iResolution, 0, GL_LUMINANCE_ALPHA, GL_FLOAT, pData);
    if (m_pProperties->GetGenerateMipmaps())
      glGenerateMipmap(m_pSphereIlluminationTexture->GetTarget());
    m_pSphereIlluminationTexture->SetMinFilter(eMinFilter);
    m_pSphereIlluminationTexture->Unbind();

    VistaOGLUtils::Delete(pData);
  }

  if (m_pGaussianBlendingTexture) {
    float* pData = VistaOGLUtils::CreateGaussianBlendingTextureData(iResolution, iResolution);

    m_pGaussianBlendingTexture->Bind();
    glTexImage2D(m_pGaussianBlendingTexture->GetTarget(), 0, GL_ALPHA, iResolution, iResolution, 0,
        GL_ALPHA, GL_FLOAT, pData);
    if (m_pProperties->GetGenerateMipmaps())
      glGenerateMipmap(m_pGaussianBlendingTexture->GetTarget());
    m_pGaussianBlendingTexture->SetMinFilter(eMinFilter);
    m_pGaussianBlendingTexture->Unbind();

    VistaOGLUtils::Delete(pData);
  }

  if (m_pSphereNormalsTexture) {
    float* pData = VistaOGLUtils::CreateSphereNormalsTextureData(iResolution, iResolution, true);

    m_pSphereNormalsTexture->Bind();
    glTexImage2D(m_pSphereNormalsTexture->GetTarget(), 0, GL_RGBA, iResolution, iResolution, 0,
        GL_RGBA, GL_FLOAT, pData);
    if (m_pProperties->GetGenerateMipmaps())
      glGenerateMipmap(m_pSphereNormalsTexture->GetTarget());
    m_pSphereNormalsTexture->SetMinFilter(eMinFilter);
    m_pSphereNormalsTexture->Unbind();

    VistaOGLUtils::Delete(pData);
  }
}
void VistaParticleRenderingCore::UpdateUniformVariables(VistaGLSLShader* pShader) {
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
    if (!m_pLookUpTexture)
      glUniform4fv(iUniformLocation, 1, &m_oParticleColor[0]);
    else
      glUniform4f(iUniformLocation, 1.0f, 1.0f, 1.0f, 1.0f);
  }

  iUniformLocation = pShader->GetUniformLocation("u_v2RangeParams");
  if (iUniformLocation >= 0) {
    glUniform2f(
        iUniformLocation, m_aLookupRange[0], 1.0f / (m_aLookupRange[1] - m_aLookupRange[0]));
  }

  iUniformLocation = pShader->GetUniformLocation("u_v2ParticleDataSize");
  if (iUniformLocation >= 0)
    glUniform2iv(iUniformLocation, 1, m_aParticleDataTextureSize);

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

void VistaParticleRenderingCore::RenderPoints() {
  if (!m_pPointShader)
    return;

  // VistaTexture* pLookupTexture = ( m_pLookUpTexture )?( m_pLookUpTexture ):(
  // m_pDefaultLookUpTexture );

  glPointSize(m_fPointSize);

  m_pPointShader->Bind();
  UpdateUniformVariables(m_pPointShader);

  glDrawArraysInstanced(GL_POINTS, 0, 1, m_iParticleCount);

  m_pPointShader->Release();
}

void VistaParticleRenderingCore::RenderBillboards(
    VistaGLSLShader* pShader, VistaTexture* pTexture) {
  if (!pShader || !pTexture)
    return;

  pTexture->Bind(GL_TEXTURE3);
  pShader->Bind();
  UpdateUniformVariables(pShader);

  glDrawArraysInstanced(GL_QUADS, 0, 4, m_iParticleCount);

  pShader->Release();
  pTexture->Unbind(GL_TEXTURE3);

  if (m_pProperties->GetRenderHalos())
    RenderHalos();
}

void VistaParticleRenderingCore::RenderTransparentBillboards() {
  if (!m_pSmokeShader || !m_pGaussianBlendingTexture)
    return;

  glEnable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glDepthMask(false);

  int iUseMappingTexture = 0;

  if (m_pProperties->GetBlendingMode() == VistaParticleRenderingProperties::ADDITIVE_BLENDING)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  else {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (m_pParticleMappingTexture) {
      m_pParticleMappingTexture->Bind(GL_TEXTURE4);
      iUseMappingTexture = 1;
    }
  }

  m_pGaussianBlendingTexture->Bind(GL_TEXTURE3);
  m_pSmokeShader->Bind();
  UpdateUniformVariables(m_pSmokeShader);

  int iLoc = m_pSmokeShader->GetUniformLocation("u_bUseMapping");
  if (iLoc >= 0)
    glUniform1i(iLoc, iUseMappingTexture);

  glDrawArraysInstanced(GL_QUADS, 0, 4, m_iParticleCount);

  m_pSmokeShader->Release();
  m_pGaussianBlendingTexture->Unbind(GL_TEXTURE3);

  if (iUseMappingTexture)
    m_pParticleMappingTexture->Unbind(GL_TEXTURE4);

  glDepthMask(true);
}

void VistaParticleRenderingCore::RenderHalos() {
  if (!m_pHaloShader)
    return;

  m_pSphereIlluminationTexture->Bind(GL_TEXTURE3);
  m_pHaloShader->Bind();
  UpdateUniformVariables(m_pHaloShader);

  glDrawArraysInstanced(GL_QUADS, 0, 4, m_iParticleCount);
  m_pHaloShader->Release();
  m_pSphereIlluminationTexture->Unbind(GL_TEXTURE3);
}
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
