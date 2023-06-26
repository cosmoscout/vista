/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
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

#if defined(WIN32)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4275)
#pragma warning(disable : 4267)
#pragma warning(disable : 4251)
#pragma warning(disable : 4231)
#endif

#include "VistaOpenSGParticles.h"

#include <VistaBase/VistaVersion.h>
#include <VistaKernel/GraphicsManager/VistaExtensionNode.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>

#include <VistaTools/VistaFileSystemFile.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <OpenSG/OSGBlendChunk.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGImageFileHandler.h>
#include <OpenSG/OSGImageFunctions.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGParticles.h>
#include <OpenSG/OSGSHLChunk.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGSimpleMaterial.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>
#include <OpenSG/OSGSimpleTexturedMaterialBase.h>
#include <OpenSG/OSGTexGenChunk.h>
#include <OpenSG/OSGTextureChunk.h>

using namespace osg;

struct VistaOpenSGParticlesData {
  ParticlesRefPtr          ptrParticles;
  MFPnt3f*                 pPositionsField;
  MFPnt3f*                 pSecPositionsField;
  MFVec3f*                 pSizesField;
  MFColor4f*               pColors4Field;
  MFColor3f*               pColors3Field;
  bool                     bHasAlpha;
  osg::ChunkMaterialRefPtr m_pMaterial;
  osg::TextureChunkRefPtr  m_pTexture;
  osg::BlendChunkRefPtr    m_pBlendChunk;
  osg::MaterialChunkRefPtr m_pMaterialChunk;
  SHLChunkPtr              m_pSHLChunk; // for shader quads

  VistaOpenSGParticlesData(ParticlesPtr ptr, const bool bWithAlpha = false)
      : ptrParticles(ptr)
      , pPositionsField(NULL)
      , pSecPositionsField(NULL)
      , pColors4Field(NULL)
      , pColors3Field(NULL)
      , pSizesField(NULL)
      , bHasAlpha(bWithAlpha)
      , m_pMaterial(osg::ChunkMaterial::create())
      , m_pBlendChunk(osg::BlendChunk::create())
      , m_pTexture(osg::TextureChunk::create())
      , m_pMaterialChunk(osg::MaterialChunk::create())
      , m_pSHLChunk(osg::NullFC) {
    CPEditAll(ptrParticles);

    // set "immersive billboarding" mode
    ptrParticles->setMode(Particles::ViewDirQuads);

    // create positions field
    GeoPositions3fPtr pos = GeoPositions3f::create();
    ptrParticles->setPositions(pos);
    pPositionsField = pos->getFieldPtr();

    // create secpositions field
    GeoPositions3fPtr secpos = GeoPositions3f::create();
    ptrParticles->setSecPositions(secpos);
    pSecPositionsField = secpos->getFieldPtr();

    // create color field (either 3f or 4f)
    GeoColorsPtr col;
    if (GetHasAlpha()) {
      GeoColors4fPtr ptrColors = GeoColors4f::create();
      col                      = ptrColors;
      pColors4Field            = ptrColors->getFieldPtr();
    } else {
      GeoColors3fPtr ptrColors = GeoColors3f::create();
      col                      = ptrColors;
      pColors3Field            = ptrColors->getFieldPtr();
    }

    ptrParticles->setColors(col);

    pSizesField = ptrParticles->getMFSizes();

    // configure our material
    {
      CPEditAll(m_pMaterialChunk);
      m_pMaterialChunk->setDiffuse(Color4f(1, 1, 1, 1));
      m_pMaterialChunk->setLit(false);

      CPEditAll(m_pBlendChunk);
      m_pBlendChunk->setSrcFactor(GL_SRC_ALPHA);
      m_pBlendChunk->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
      m_pBlendChunk->setAlphaFunc(GL_NONE);
      m_pBlendChunk->setAlphaValue(0);

      CPEditAll(m_pTexture);
      m_pTexture->setEnvMode(GL_REPLACE);
      m_pTexture->setMagFilter(GL_LINEAR);

      // add all chunks to the material
      CPEditAll(m_pMaterial);
      m_pMaterial->addChunk(m_pMaterialChunk);
      m_pMaterial->addChunk(TextureChunk::create());
      m_pMaterial->addChunk(TexGenChunk::create());
      m_pMaterial->addChunk(m_pBlendChunk);
    }

    ptrParticles->setMaterial(m_pMaterial);
  }

  bool GetHasAlpha() const {
    return bHasAlpha;
  }
  void SetHasAlpha(const bool state) {
    if (state == GetHasAlpha())
      return;

    // else
    bHasAlpha = state;

    GeoColorsPtr col;
    if (GetHasAlpha()) {
      GeoColors4fPtr ptr = GeoColors4f::create();
      col                = ptr;
      pColors4Field      = ptr->getFieldPtr();
      pColors3Field      = NULL;
    } else {
      GeoColors3fPtr ptr = GeoColors3f::create();
      col                = ptr;
      pColors3Field      = ptr->getFieldPtr();
      pColors4Field      = NULL;
    }

    CPEdit(ptrParticles, Particles::ColorsFieldMask);
    ptrParticles->setColors(col);
  }

  void SetImageAsTexture(ImagePtr pImage) {
    {
      CPEditAll(m_pTexture);
      m_pTexture->setImage(pImage);
    }

    CPEditAll(m_pMaterial);
    m_pMaterial->addChunk(m_pTexture, 0);
  }
};

VistaOpenSGParticles::VistaOpenSGParticles(
    VistaSceneGraph* pSG, VistaGroupNode* pParentNode, int eParticleMode)
    : m_pParticleNode(NULL)
    , m_pData(NULL)
    , m_eParticleMode(eParticleMode) {
  m_pParticleNode = pSG->NewExtensionNode(pParentNode, NULL);
  NodePtr ptrNode =
      static_cast<VistaOpenSGExtensionNodeData*>(m_pParticleNode->GetData())->GetNode();

  // create OpenSG particles
  m_pData = new VistaOpenSGParticlesData(Particles::create());
  beginEditCP(ptrNode, Node::CoreFieldMask);
  ptrNode->setCore(m_pData->ptrParticles);
  endEditCP(ptrNode, Node::CoreFieldMask);
  SetMode(eParticleMode);
}

VistaOpenSGParticles::~VistaOpenSGParticles() {
  DetachAndDelete();
  delete m_pData;
}

void VistaOpenSGParticles::DetachAndDelete() {
  VistaGroupNode* pP = m_pParticleNode->GetParent();
  if (pP)
    pP->DisconnectChild(m_pParticleNode);

  delete m_pParticleNode->GetExtension();
  delete m_pParticleNode;
}

void VistaOpenSGParticles::BeginEdit() const {
  beginEditCP(m_pData->ptrParticles);
}

void VistaOpenSGParticles::EndEdit() const {
  endEditCP(m_pData->ptrParticles);
}

void VistaOpenSGParticles::SetColorsWithAlpha(const bool bWithAlpha) const {
  m_pData->SetHasAlpha(bWithAlpha);
}

void VistaOpenSGParticles::SetNumParticles(
    const int nSize, const bool bUseSeparateColors, const bool bUseSeparateSizes) const {
  BeginEdit();
  m_pData->pPositionsField->resize(nSize, Pnt3f(0, 0, 0));
  // if( m_eParticleMode == PM_VIEWERQUADS ) I THINK I NEED THIS ALWAYS ...
  m_pData->pSecPositionsField->resize(nSize, Pnt3f(0, 0, 0));

  if (m_pData->GetHasAlpha())
    m_pData->pColors4Field->resize(bUseSeparateColors ? nSize : 1, Color4f(1.0f, 1.0f, 1.0f, 1.0f));
  else
    m_pData->pColors3Field->resize(bUseSeparateColors ? nSize : 1, Color3f(1.0f, 1.0f, 1.0f));

  m_pData->pSizesField->resize(bUseSeparateSizes ? nSize : 1, Vec3f(1.0f, 1.0f, 1.0f));
  EndEdit();
}

int VistaOpenSGParticles::GetNumParticles() const {
  return m_pData->pPositionsField->size();
}

int VistaOpenSGParticles::GetNumColors() const {
  if (m_pData->GetHasAlpha())
    return m_pData->pColors4Field->size();
  else
    return m_pData->pColors3Field->size();
}

bool VistaOpenSGParticles::GetColorsWithAlpha() const {
  return m_pData->GetHasAlpha();
}

int VistaOpenSGParticles::GetNumSizes() const {
  return m_pData->pSizesField->size();
}

float* VistaOpenSGParticles::GetParticlePositions3fField(const int& idx) const {
  return &(*m_pData->pPositionsField)[idx][0];
}

float* VistaOpenSGParticles::GetSecParticlePositions3fField(const int& idx) const {
  if (m_eParticleMode != PM_DIRECTEDQUADS)
    return NULL;
  return &(*m_pData->pSecPositionsField)[idx][0];
}

float* VistaOpenSGParticles::GetParticleColors3fField(const int& idx) const {
  return &(*m_pData->pColors3Field)[idx][0];
}

float* VistaOpenSGParticles::GetParticleColors4fField(const int& idx) const {
  return &(*m_pData->pColors4Field)[idx][0];
}

float* VistaOpenSGParticles::GetParticleColorsXfField(const int& idx) const {
  if (m_pData->GetHasAlpha())
    return GetParticleColors4fField(idx);
  else
    return GetParticleColors3fField(idx);
}

float* VistaOpenSGParticles::GetParticleSizes3fField(const int& idx) const {
  return (float*)&(*m_pData->pSizesField)[idx][0];
}

VistaExtensionNode* VistaOpenSGParticles::GetParticleNode() const {
  return m_pParticleNode;
}

static inline float gaussian(const float& x, const float& y, const float& sigma) {
  return osg::osgexp(-0.5f * (x * x + y * y) / (sigma * sigma));
}
static inline float gaussian2(const float& x, const float& y, const float& sigma) {
  // SQRT(PI) = 2.506628274631000502415765284811045253006
  return osg::osgexp(-0.5f * (x * x + y * y) / (sigma * sigma)) /
         (2.506628274631000502415765284811045253006f * sigma);
}

static void makeGaussBlobAlpha(
    ImagePtr& img, const float& sigma, const float& m, const bool bApplyScale = true) {
  assert(img != osg::NullFC);
  assert(img->hasAlphaChannel());
  beginEditCP(img);
  osg::UInt8  bpp    = img->getBpp();
  osg::UInt8* tex    = img->getData();
  int         width  = img->getWidth();
  int         height = img->getHeight();

  // modify alpha
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      float fValue = 0;
      // clean border
      if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
        fValue = 0;
      } else {
        float nDeltaX = x - (width - 1) / 2;
        float nDeltaY = y - (width - 1) / 2;
        if (bApplyScale == false) {
          nDeltaX /= width;
          nDeltaY /= height;
          fValue = m * gaussian(nDeltaX, nDeltaY, sigma);
        } else
          fValue = m * gaussian(nDeltaX, nDeltaY, sigma);
      }

      fValue                               = (osg::UInt8)osg::osgMin(255.0f, 256.0f * fValue);
      tex[bpp * (y * width + x) + bpp - 1] = fValue;
    }
  }
  endEditCP(img);
}

void VistaOpenSGParticles::SetUseGaussBlobTexture(
    const int& size, const float& sigma, const float& m) const {
  ImagePtr pImage = Image::create();
  beginEditCP(pImage);
  pImage->set(Image::OSG_RGBA_PF, size, size);
  pImage->clear(0xFF);
  endEditCP(pImage);
  makeGaussBlobAlpha(pImage, sigma, m);

  m_pData->SetImageAsTexture(pImage);
}

void VistaOpenSGParticles::SetUseUnscaledGaussBlobTexture(
    const int iTexSize, const float fSigma, const float fCenterHeight) const {
  ImagePtr pImage = Image::create();
  beginEditCP(pImage);
  pImage->set(Image::OSG_RGBA_PF, iTexSize, iTexSize);
  pImage->clear(0xFF);
  endEditCP(pImage);
  makeGaussBlobAlpha(pImage, fSigma, fCenterHeight, false);

  m_pData->SetImageAsTexture(pImage);
}

void VistaOpenSGParticles::SetMaterialTransparency(const float& a) const {
  VISTA_THROW_NOT_IMPLEMENTED
  // ChunkMaterialPtr ptrChMat	= ChunkMaterialPtr::dcast
  //	(m_pData->ptrParticles->getMaterial());
  // assert(ptrChMat);

  // MaterialChunkPtr ptrMatChunk = MaterialChunkPtr::dcast(
  //	ptrChMat->find(MaterialChunk::getClassType());
  // beginEditCP(ptrMatChunk, MaterialChunk::TransparencyFieldMask);
  // ptrMat
  // endEditCP  (ptrMatChunk, MaterialChunk::TransparencyFieldMask);
}

void VistaOpenSGParticles::SetGLTexEnvMode(const GLenum& eMode) const {
  beginEditCP(m_pData->m_pTexture, osg::TextureChunk::EnvModeFieldMask);
  m_pData->m_pTexture->setEnvMode(eMode);
  endEditCP(m_pData->m_pTexture, osg::TextureChunk::EnvModeFieldMask);
  SetUniformParameter("bModulate", (eMode == GL_MODULATE));
}

void VistaOpenSGParticles::SetGLBlendSrcFactor(const GLenum& eMode) const {
  beginEditCP(m_pData->m_pBlendChunk, osg::BlendChunk::SrcFactorFieldMask);
  m_pData->m_pBlendChunk->setSrcFactor(eMode);
  endEditCP(m_pData->m_pBlendChunk, osg::BlendChunk::SrcFactorFieldMask);
}

void VistaOpenSGParticles::SetGLBlendDestFactor(const GLenum& eMode) const {
  beginEditCP(m_pData->m_pBlendChunk, osg::BlendChunk::DestFactorFieldMask);
  m_pData->m_pBlendChunk->setDestFactor(eMode);
  endEditCP(m_pData->m_pBlendChunk, osg::BlendChunk::DestFactorFieldMask);
}

bool VistaOpenSGParticles::GetParticlesAreLit() const {
  return m_pData->m_pMaterialChunk->getLit();
}

void VistaOpenSGParticles::SetParticlesAreLit(bool bSet) {
  beginEditCP(m_pData->m_pMaterialChunk, osg::MaterialChunk::LitFieldMask);
  m_pData->m_pMaterialChunk->setLit(bSet);
  endEditCP(m_pData->m_pMaterialChunk, osg::MaterialChunk::LitFieldMask);
}

void VistaOpenSGParticles::SetDrawOrder(const VistaOpenSGParticles::eDrawOrder& mode) const {
  beginEditCP(m_pData->ptrParticles, Particles::DrawOrderFieldMask);
  m_pData->ptrParticles->setDrawOrder(mode);
  endEditCP(m_pData->ptrParticles, Particles::DrawOrderFieldMask);
}

bool VistaOpenSGParticles::SetUseSpriteImage(const std::string& strFileName) const {
  VistaFileSystemFile oFile(strFileName);
  if (oFile.Exists() == false) {
    vstr::warnp() << "VistaOpenSGParticles::SetUseSpriteImage() -- "
                  << "File [" << strFileName << "] does not exist!" << std::endl;
    return false;
  }

  osg::ImagePtr pImage = osg::Image::create();
  if (pImage->read(strFileName.c_str()) == false) {
    return false;
  }
  m_pData->SetImageAsTexture(pImage);
  return true;
}

void VistaOpenSGParticles::SetMode(const int& mode) {
  beginEditCP(m_pData->ptrParticles);

  switch (mode) {
  case PM_VIEWERQUADS: {
    m_pData->ptrParticles->setMode(Particles::ViewerQuads);
    m_pData->pSecPositionsField->resize(0, Pnt3f(0, 0, 0));
    RemoveShader();
    break;
  }
  case PM_DIRECTEDQUADS: {
    m_pData->ptrParticles->setMode(Particles::ShaderQuads);
    m_pData->pSecPositionsField->resize(m_pData->pPositionsField->getSize(), Pnt3f(0, 0, 0));
    SetNormal();
    // SetParticleShader();
    break;
  }
  default:
    break;
  }

  endEditCP(m_pData->ptrParticles);

  m_eParticleMode = mode;
}

void VistaOpenSGParticles::CopyPosToSecpos() const {
  if (m_eParticleMode != PM_DIRECTEDQUADS)
    return;

  float* secpos;
  float* pos;

  pos    = GetParticlePositions3fField();
  secpos = GetSecParticlePositions3fField();

  int i = 0;
  for (i = 0; i < (int)m_pData->pSecPositionsField->size(); i++) {
    secpos[3 * i + 0] = pos[3 * i + 0];
    secpos[3 * i + 1] = pos[3 * i + 1];
    secpos[3 * i + 2] = pos[3 * i + 2];
  }
}

void VistaOpenSGParticles::SetNormal() const {
  beginEditCP(m_pData->ptrParticles, Particles::NormalsFieldMask);

  osg::GeoNormals3fPtr norms = osg::GeoNormals3f::create();
  norms->push_back(Vec3f(0, 1, 0));

  m_pData->ptrParticles->setNormals(norms);

  endEditCP(m_pData->ptrParticles, Particles::NormalsFieldMask);
}

void VistaOpenSGParticles::SetParticleShader() {
  // vertex shader program.
  static std::string vp_program =
      "\n"
      "#define pPos gl_MultiTexCoord1\n"
      "#define pSecPos gl_MultiTexCoord2\n"
      "#define pSize gl_MultiTexCoord3\n"
      "#define pOffset gl_Vertex\n"
      "\n"
      "varying float brightness;\n"
      "\n"
      "void main(void)\n"
      "{\n"
      "\n"
      "gl_TexCoord[0] = gl_MultiTexCoord0;\n"
      "\n"
      "vec4 pos;\n"
      "vec3 diff_secpos_pos = vec3(pSecPos-pPos);\n"
      "float move_length = length(diff_secpos_pos);\n"
      "diff_secpos_pos = diff_secpos_pos / move_length;\n"
      "vec3 cameraPosition = normalize(vec3(gl_ModelViewMatrixInverse*vec4(0,0,0,1.0)-pPos));"
      "vec3 other_direction = (-cross(diff_secpos_pos,cameraPosition));\n"
      "\n"
      "gl_FrontColor = gl_Color;\n"
      "brightness = pSize[2];\n"
      "\n"
      "pos = pPos - vec4(pSize[0] * ( pOffset[0] - 0.5 )* diff_secpos_pos,0)\n"
      "           + vec4(pSize[1] * pOffset[1] * other_direction,0);\n"
      "gl_Position = gl_ModelViewProjectionMatrix * pos;\n"
      "}\n";

  // fragment shader program.
  static std::string fp_program = "\n"
                                  "uniform sampler2D myTexture;\n"
                                  "\n"
                                  "varying float brightness;\n"
                                  "\n"
                                  "void main(void)\n"
                                  "{\n"
                                  "\n"
                                  "   //gl_FragColor = vec4(my_color, 0);\n"
                                  "	vec4 tex2d = texture2D(myTexture, vec2(gl_TexCoord[0]));\n"
                                  "	float yval = 0.0;\n"
                                  "	float uval = 0.0;\n"
                                  "	float vval = 0.0;\n"
                                  "	float rval = 0.0;\n"
                                  "	float gval = 0.0;\n"
                                  "	float bval = 0.0;\n"
                                  "	yval = 0.3*tex2d.r + 0.6*tex2d.g + 0.1*tex2d.b;\n"
                                  "	uval = -0.3*tex2d.r - 0.6*tex2d.g + 0.9*tex2d.b;\n"
                                  "	vval = 0.7*tex2d.r - 0.6*tex2d.g - 0.1*tex2d.b;\n"
                                  "   yval = yval * (1.0+6.0*brightness);\n"
                                  "	if (yval>1.0) {yval=1.0;}\n"
                                  //"\n"
                                  //"	rval = yval+vval;\n"
                                  //"	gval = yval - 0.166667*uval - 0.5*vval;\n"
                                  //"	bval = yval + uval;\n"
                                  //"\n"
                                  //"	rval = rval*(1.0+gl_Color.r);\n"
                                  //"	if (rval > 1.0) {rval = 1.0;}\n"
                                  //"\n"
                                  //"	gval = gval*(1.0+gl_Color.g);\n"
                                  //"	if (gl_Color.g == 0.0) {gval = 0.0;}\n"
                                  //"\n"
                                  //"	bval = bval*(1.0+gl_Color.b);\n"
                                  //"	if (bval > 255.0) {bval = 255.0;}\n"
                                  "	rval = yval+vval;\n"
                                  "	gval = yval - 0.166667*uval - 0.5*vval;\n"
                                  "	bval = yval + uval;\n"
                                  "\n"
                                  "	gl_FragColor  = "
                                  "vec4(rval*(1.0+gl_Color.r),gval*(1.0-gl_Color.g),bval*(1.0-gl_"
                                  "Color.b),tex2d.a*gl_Color.a);\n"
                                  "}\n";

  SetShaderPrograms(vp_program, fp_program);
}

// The following functions could be an All-In-One Solution. I decided to leave them seperate so one
// couly easily re-use them. Still they're hidden as Private!
void VistaOpenSGParticles::SetShaderPrograms(
    const std::string& VertexShaderProgram, const std::string& FragmentShaderProgram) {
  if (m_pData->m_pSHLChunk == osg::NullFC) {
    m_pData->m_pSHLChunk = osg::SHLChunk::create();
    beginEditCP(m_pData->m_pMaterial);
    m_pData->m_pMaterial->addChunk(m_pData->m_pSHLChunk);
    endEditCP(m_pData->m_pMaterial);
  }

  osg::beginEditCP(m_pData->m_pSHLChunk);
  m_pData->m_pSHLChunk->setVertexProgram(VertexShaderProgram);
  m_pData->m_pSHLChunk->setFragmentProgram(FragmentShaderProgram);
  osg::endEditCP(m_pData->m_pSHLChunk);

  SetUniformParameter("myTexture", 0);
  bool bModulate = (m_pData->m_pTexture->getEnvMode() == GL_MODULATE);
  SetUniformParameter("bModulate", bModulate);
}

void VistaOpenSGParticles::RemoveShader() {
  if (m_pData->m_pSHLChunk != osg::NullFC) {
    beginEditCP(m_pData->m_pMaterial);
    m_pData->m_pMaterial->subChunk(m_pData->m_pSHLChunk);
    endEditCP(m_pData->m_pMaterial);
    m_pData->m_pSHLChunk = osg::NullFC;
  }
}

void VistaOpenSGParticles::SetUniformParameter(
    const std::string& UniformName, const int& NumValue) const {
  if (m_pData->m_pSHLChunk) {
    osg::beginEditCP(m_pData->m_pSHLChunk);
    m_pData->m_pSHLChunk->setUniformParameter(UniformName.c_str(), NumValue);
    osg::endEditCP(m_pData->m_pSHLChunk);
  }
}

int VistaOpenSGParticles::GetMode() const {
  return m_eParticleMode;
}
