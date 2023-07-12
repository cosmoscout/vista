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

#include "VistaOpenSGMultiMaterial.h"

#include "VistaKernel/DisplayManager/VistaDisplayManager.h"
#include "VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h"

#if defined(WIN32)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4275)
#pragma warning(disable : 4267)
#pragma warning(disable : 4251)
#pragma warning(disable : 4231)
#endif

#include <OpenSG/OSGBlendChunk.h>
#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGCubeTextureChunk.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGPolygonChunk.h>
#include <OpenSG/OSGSHLChunk.h>
#include <OpenSG/OSGSHLParameterChunk.h>
#include <OpenSG/OSGTextureChunk.h>

#include "OpenSG/OSGGeoPropColors.h"
#include "VistaBase/VistaUtilityMacros.h"
#include "VistaKernel/GraphicsManager/VistaGeomNode.h"
#include "VistaKernel/GraphicsManager/VistaNode.h"
#include "VistaKernel/GraphicsManager/VistaSceneGraph.h"
#include "VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h"
#include "VistaOpenSGMultiMaterialShaderCreator.h"
#include "VistaOpenSGTextureLoader.h"

class VistaOpenSGMultiMaterial::Internals {
 public:
  Internals(VistaOpenSGTextureLoader* pTexLoader)
      : m_pMaterial(osg::ChunkMaterial::create())
      , m_pMaterialChunk(osg::MaterialChunk::create())
      , m_pPolygonChunk(osg::PolygonChunk::create())
      , m_pShaderUniforms(osg::SHLParameterChunk::create())
      , m_bCullFrontFace(false)
      , m_bCullBackFace(false)
      , m_pCurrentShader(osg::NullFC)
      , m_pBaseMap(osg::NullFC)
      , m_pBlendMap(osg::NullFC)
      , m_pSpecularMap(osg::NullFC)
      , m_pWorldEnvironmentMap(osg::NullFC)
      , m_pObjectEnvironmentMap(osg::NullFC)
      , m_pNormalMap(osg::NullFC)
      , m_pBlendChunk(osg::NullFC)
      , m_bBlendingEnabled(false)
      , m_nBlendFactor(1)
      , m_nReflectionFactor(0)
      , m_nReflectionLodBias(0)
      , m_nOpacity(1.0f)
      , m_fTexScaleS(1.0f)
      , m_fTexScaleT(1.0f)
      , m_bUseNativePipeline(false)
      , m_bBeforeFirstCreation(true)
      , m_eBlendingMode(BM_AUTO)
      , m_pTextureLoader(pTexLoader)
      , m_bUseFog(false)
      , m_nFogType(GL_EXP2)
      , m_bDrawWireframe(false) {
    beginEditCP(m_pMaterialChunk);
    m_pMaterialChunk->setAmbient(osg::Color4f(1.0f, 1.0f, 1.0f, 1.0f));
    m_pMaterialChunk->setDiffuse(osg::Color4f(1.0f, 1.0f, 1.0f, 1.0f));
    m_pMaterialChunk->setSpecular(osg::Color4f(1.0f, 1.0f, 1.0f, 1.0f));
    m_pMaterialChunk->setEmission(osg::Color4f(0.0f, 0.0f, 0.0f, 0.0f));
    m_pMaterialChunk->setShininess(32);
    m_pMaterialChunk->setLit(true);
    endEditCP(m_pMaterialChunk);

    beginEditCP(m_pMaterial);
    m_pMaterial->addChunk(m_pMaterialChunk);
    m_pMaterial->addChunk(m_pPolygonChunk);
    m_pMaterial->addChunk(m_pShaderUniforms);
    endEditCP(m_pMaterial);

    UpdateFogParameter();
    UpdateTexScaleParameters();
    UpdateBlendMapParameters();
    UpdateReflectionParameters();
  }

  virtual ~Internals() {
  }

  bool VerifyShadersHaveBeenCreated() {
    if (m_bBeforeFirstCreation == false)
      return true;

    if (m_bUseNativePipeline)
      return true;

    m_bBeforeFirstCreation = false;
    UpdateShaders();
    return true;
  }

  void UpdateShaders() {
    if (m_bBeforeFirstCreation)
      return;

    if (m_bUseNativePipeline) {
      if (m_pCurrentShader != osg::NullFC) {
        m_pMaterial->subChunk(m_pShaderUniforms);
        m_pMaterial->subChunk(m_pCurrentShader);
        m_pCurrentShader = osg::NullFC;
      }
    } else {
      beginEditCP(m_pMaterial);
      if (m_pCurrentShader != osg::NullFC) {
        m_pMaterial->subChunk(m_pShaderUniforms);
        m_pMaterial->subChunk(m_pCurrentShader);
      }
      m_pCurrentShader = GetShader();

      beginEditCP(m_pShaderUniforms, osg::SHLParameterChunk::SHLChunkFieldMask);
      m_pShaderUniforms->setSHLChunk(m_pCurrentShader);
      // m_pShaderUniforms->clearUniformParameters();
      m_pShaderUniforms->subUniformParameter("nReflectionFactor");
      m_pShaderUniforms->subUniformParameter("nEnvMapBias");
      m_pShaderUniforms->subUniformParameter("nFogType");
      m_pShaderUniforms->subUniformParameter("u_v4TexCoordScale");
      m_pShaderUniforms->subUniformParameter("nBlendFactor");
      endEditCP(m_pShaderUniforms, osg::SHLParameterChunk::SHLChunkFieldMask);

      m_pMaterial->addChunk(m_pCurrentShader);
      m_pMaterial->addChunk(m_pShaderUniforms);
      endEditCP(m_pMaterial);
      UpdateReflectionParameters();
      UpdateBlendMapParameters();
      UpdateTexScaleParameters();
      UpdateFogParameter();
    }
  }

  osg::SHLChunkPtr GetShader() {
    VistaOpenSGMultiMaterialShaderCreator::ShaderConfig oConfig;
    oConfig.m_bHasBaseMap      = (m_pBaseMap != osg::NullFC);
    oConfig.m_bHasBlendMap     = (m_pBlendMap != osg::NullFC);
    oConfig.m_bHasSpecularMap  = (m_pSpecularMap != osg::NullFC);
    oConfig.m_bHasNormalMap    = (m_pNormalMap != osg::NullFC);
    oConfig.m_bHasWorldEnvMap  = (m_pWorldEnvironmentMap != osg::NullFC);
    oConfig.m_bHasObjectEnvMap = (m_pObjectEnvironmentMap != osg::NullFC);
    oConfig.m_bHasGlossMap     = (m_pGlossMap != osg::NullFC);
    oConfig.m_bUseFog          = m_bUseFog;

    return VistaOpenSGMultiMaterialShaderCreator::GetShader(oConfig);
  }

  void UpdateBlending() {
    bool bSet;
    switch (m_eBlendingMode) {
    case BM_AUTO: {
      bSet = m_nOpacity < 1.0f;
      if (m_pBaseMap && m_pBaseMap->getImage()->hasAlphaChannel())
        bSet = true;
      break;
    }
    case BM_BLENDING:
      bSet = true;
      return;
    case BM_NO_BLENDING:
      bSet = false;
      return;
    default:
      VISTA_THROW("invalid BlendingMode value", -1);
    }
    if (m_bBlendingEnabled == bSet)
      return;
    if (bSet) {
      if (m_pBlendChunk == osg::NullFC) {
        m_pBlendChunk = osg::BlendChunk::create();
        beginEditCP(m_pBlendChunk);
        m_pBlendChunk->setColor(osg::Color4f(1, 1, 1, m_nOpacity));
        m_pBlendChunk->setSrcFactor(GL_SRC_ALPHA);
        m_pBlendChunk->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
        m_pBlendChunk->setAlphaValue(m_nOpacity);
        endEditCP(m_pBlendChunk);
      } else {
        beginEditCP(m_pBlendChunk);
        m_pBlendChunk->setAlphaValue(m_nOpacity);
        m_pBlendChunk->setColor(osg::Color4f(1, 1, 1, m_nOpacity));
        endEditCP(m_pBlendChunk);
      }
      beginEditCP(m_pMaterial);
      m_pMaterial->addChunk(m_pBlendChunk);
      endEditCP(m_pMaterial);
    } else {
      beginEditCP(m_pMaterial);
      m_pMaterial->subChunk(m_pBlendChunk);
      endEditCP(m_pMaterial);
    }
    m_bBlendingEnabled = bSet;
  }

  void UpdateFogParameter() {
    if (m_pShaderUniforms == osg::NullFC || m_bUseFog == false)
      return;
    int nInternalMode = 0;
    switch (m_nFogType) {
    case GL_LINEAR:
      nInternalMode = 0;
      break;
    case GL_EXP:
      nInternalMode = 1;
      break;
    case GL_EXP2:
      nInternalMode = 2;
      break;
    default:
      VISTA_THROW("Invalid GL Fog mode", -1);
    }
    beginEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
    m_pShaderUniforms->setUniformParameter("nFogType", nInternalMode);
    endEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
  }

  void UpdateTexScaleParameters() {
    if (m_pShaderUniforms == osg::NullFC || GetUseTextureScale() == false)
      return;
    beginEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
    m_pShaderUniforms->setUniformParameter(
        "u_v4TexCoordScale", osg::Vec4f(m_fTexScaleS, m_fTexScaleT, 1.0f, 1.0f));
    endEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
  }

  void UpdateBlendMapParameters() {
    if (m_pShaderUniforms == osg::NullFC || m_pBlendMap == osg::NullFC)
      return;
    beginEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
    m_pShaderUniforms->setUniformParameter("nBlendFactor", m_nBlendFactor);
    endEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
  }

  void UpdateReflectionParameters() {
    if (m_pShaderUniforms == osg::NullFC ||
        (m_pWorldEnvironmentMap == osg::NullFC && m_pObjectEnvironmentMap == osg::NullFC))
      return;
    beginEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
    m_pShaderUniforms->setUniformParameter("nReflectionFactor", m_nReflectionFactor);
    m_pShaderUniforms->setUniformParameter("nEnvMapBias", m_nReflectionLodBias);
    endEditCP(m_pShaderUniforms, osg::SHLParameterChunk::ParametersFieldMask);
  }

  bool GetUseTextureScale() const {
    return ((m_pBaseMap != osg::NullFC) || (m_pBlendMap != osg::NullFC) ||
            (m_pSpecularMap != osg::NullFC) || (m_pNormalMap != osg::NullFC) ||
            (m_pGlossMap != osg::NullFC &&
                (m_pWorldEnvironmentMap != osg::NullFC || m_pObjectEnvironmentMap != osg::NullFC)));
  }

  bool AddTexture(osg::TextureChunkPtr pTexture, const int nTextureId) {
    beginEditCP(m_pMaterial, osg::ChunkMaterial::ChunksFieldId);
    bool bRes = m_pMaterial->addChunk(pTexture, nTextureId);
    endEditCP(m_pMaterial, osg::ChunkMaterial::ChunksFieldId);
    return bRes;
  }
  void RemoveTexture(osg::TextureChunkPtr pTexture, const int nTextureId) {
    beginEditCP(m_pMaterial, osg::ChunkMaterial::ChunksFieldId);
    m_pMaterial->subChunk(pTexture, nTextureId);
    endEditCP(m_pMaterial, osg::ChunkMaterial::ChunksFieldId);
  }

 public:
  bool                     m_bBeforeFirstCreation;
  osg::ChunkMaterialRefPtr m_pMaterial;
  osg::MaterialChunkRefPtr m_pMaterialChunk;
  osg::BlendChunkRefPtr    m_pBlendChunk;
  osg::PolygonChunkRefPtr  m_pPolygonChunk;
  bool                     m_bBlendingEnabled;
  float                    m_nOpacity;

  osg::SHLChunkRefPtr       m_pCurrentShader;
  osg::SHLParameterChunkPtr m_pShaderUniforms;

  osg::TextureChunkRefPtr     m_pBaseMap;
  osg::TextureChunkRefPtr     m_pBlendMap;
  osg::TextureChunkRefPtr     m_pSpecularMap;
  osg::TextureChunkRefPtr     m_pNormalMap;
  osg::CubeTextureChunkRefPtr m_pWorldEnvironmentMap;
  osg::CubeTextureChunkRefPtr m_pObjectEnvironmentMap;
  osg::TextureChunkRefPtr     m_pGlossMap;

  float m_nBlendFactor;
  float m_nReflectionFactor;
  float m_nReflectionLodBias;

  bool m_bCullFrontFace;
  bool m_bCullBackFace;

  float m_fTexScaleS;
  float m_fTexScaleT;

  bool m_bUseNativePipeline;
  bool m_bDrawWireframe;

  bool  m_bUseFog;
  GLint m_nFogType;

  BlendingMode m_eBlendingMode;

  VistaOpenSGTextureLoader* m_pTextureLoader;
};

VistaOpenSGMultiMaterial::VistaOpenSGMultiMaterial(VistaOpenSGTextureLoader* pTexLoader)
    : m_pInternals(new VistaOpenSGMultiMaterial::Internals(pTexLoader)) {
}

VistaOpenSGMultiMaterial::~VistaOpenSGMultiMaterial() {
  delete m_pInternals;
}

bool VistaOpenSGMultiMaterial::SetBaseMap(const std::string& sImageFile) {
  if (m_pInternals->m_pBaseMap) {
    m_pInternals->RemoveTexture(
        m_pInternals->m_pBaseMap, VistaOpenSGMultiMaterialShaderCreator::S_nBaseMapTextureId);
    m_pInternals->m_pBaseMap = osg::NullFC;
  }
  if (sImageFile.empty()) {
    m_pInternals->UpdateBlending();
    m_pInternals->UpdateShaders();
    return true;
  }
  m_pInternals->m_pBaseMap = m_pInternals->m_pTextureLoader->RetrieveTexture(sImageFile);
  m_pInternals->UpdateBlending();
  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pBaseMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(
      m_pInternals->m_pBaseMap, VistaOpenSGMultiMaterialShaderCreator::S_nBaseMapTextureId);
}

bool VistaOpenSGMultiMaterial::SetBaseMap(osg::TextureChunkPtr pTexture) {
  if (m_pInternals->m_pBaseMap) {
    m_pInternals->RemoveTexture(
        m_pInternals->m_pBaseMap, VistaOpenSGMultiMaterialShaderCreator::S_nBaseMapTextureId);
    m_pInternals->m_pBaseMap = osg::NullFC;
  }
  m_pInternals->m_pBaseMap = pTexture;
  m_pInternals->UpdateBlending();
  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pBaseMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(
      m_pInternals->m_pBaseMap, VistaOpenSGMultiMaterialShaderCreator::S_nBaseMapTextureId);
}

bool VistaOpenSGMultiMaterial::SetBaseMap(osg::ImagePtr pImage) {
  osg::TextureChunkPtr pTexChunk = osg::TextureChunk::create();
  pTexChunk->setImage(pImage);
  return SetBaseMap(pTexChunk);
}

bool VistaOpenSGMultiMaterial::GetHasBaseMap() const {
  return (m_pInternals->m_pBaseMap != osg::NullFC);
}

bool VistaOpenSGMultiMaterial::SetBlendMap(
    const std::string& sImageFile, const float nBlendFactor) {
  SetBlendFactor(nBlendFactor);
  return SetBlendMap(sImageFile);
}

bool VistaOpenSGMultiMaterial::SetBlendMap(const std::string& sImageFile) {
  if (m_pInternals->m_pBlendMap) {
    m_pInternals->RemoveTexture(
        m_pInternals->m_pBlendMap, VistaOpenSGMultiMaterialShaderCreator::S_nBlendMapTextureId);
    m_pInternals->m_pBlendMap = osg::NullFC;
  }
  if (sImageFile.empty()) {
    m_pInternals->UpdateShaders();
    return true;
  }
  m_pInternals->m_pBlendMap = m_pInternals->m_pTextureLoader->RetrieveTexture(sImageFile);
  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pBlendMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(
      m_pInternals->m_pBlendMap, VistaOpenSGMultiMaterialShaderCreator::S_nBlendMapTextureId);
}

bool VistaOpenSGMultiMaterial::GetHasBlendMap() const {
  return (m_pInternals->m_pBlendMap != osg::NullFC);
}

bool VistaOpenSGMultiMaterial::SetBlendFactor(const float nBlendFactor) {
  m_pInternals->m_nBlendFactor = nBlendFactor;
  m_pInternals->UpdateBlendMapParameters();
  return true;
}

float VistaOpenSGMultiMaterial::GetBlendFactor() const {
  return m_pInternals->m_nBlendFactor;
}

bool VistaOpenSGMultiMaterial::SetSpecularMap(const std::string& sImageFile) {
  if (m_pInternals->m_pSpecularMap) {
    m_pInternals->RemoveTexture(m_pInternals->m_pSpecularMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nSpecularMapTextureId);
    m_pInternals->m_pSpecularMap = osg::NullFC;
  }

  m_pInternals->m_pSpecularMap = m_pInternals->m_pTextureLoader->RetrieveTexture(sImageFile);
  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pSpecularMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(
      m_pInternals->m_pSpecularMap, VistaOpenSGMultiMaterialShaderCreator::S_nSpecularMapTextureId);
}
bool VistaOpenSGMultiMaterial::GetHasSpecularMap() const {
  return (m_pInternals->m_pSpecularMap != osg::NullFC);
}

bool VistaOpenSGMultiMaterial::SetWorldEnvironmentMap(const std::string& sTopImage,
    const std::string& sBottomImage, const std::string& sLeftImage, const std::string& sRightImage,
    const std::string& sFrontImage, const std::string& sBackImage) {
  if (m_pInternals->m_pWorldEnvironmentMap) {
    m_pInternals->RemoveTexture(m_pInternals->m_pWorldEnvironmentMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nWorldEnvMapTextureId);
    m_pInternals->m_pWorldEnvironmentMap = osg::NullFC;
  }
  if (sTopImage.empty()) {
    m_pInternals->UpdateShaders();
    return true;
  }

  m_pInternals->m_pWorldEnvironmentMap = m_pInternals->m_pTextureLoader->RetrieveCubemapTexture(
      sTopImage, sBottomImage, sLeftImage, sRightImage, sFrontImage, sBackImage);

  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pWorldEnvironmentMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(m_pInternals->m_pWorldEnvironmentMap,
      VistaOpenSGMultiMaterialShaderCreator::S_nWorldEnvMapTextureId);
}
bool VistaOpenSGMultiMaterial::SetWorldEnvironmentMap(const std::string& sFolder) {
  if (m_pInternals->m_pWorldEnvironmentMap) {
    m_pInternals->RemoveTexture(m_pInternals->m_pWorldEnvironmentMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nWorldEnvMapTextureId);
    m_pInternals->m_pWorldEnvironmentMap = osg::NullFC;
  }
  if (sFolder.empty()) {
    m_pInternals->UpdateShaders();
    return true;
  }

  m_pInternals->m_pWorldEnvironmentMap =
      m_pInternals->m_pTextureLoader->RetrieveCubemapTexture(sFolder);

  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pWorldEnvironmentMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(m_pInternals->m_pWorldEnvironmentMap,
      VistaOpenSGMultiMaterialShaderCreator::S_nWorldEnvMapTextureId);
}

bool VistaOpenSGMultiMaterial::GetHasWorldEnvironmentMap() const {
  return (m_pInternals->m_pWorldEnvironmentMap != osg::NullFC);
}

bool VistaOpenSGMultiMaterial::SetObjectEnvironmentMap(const std::string& sTopImage,
    const std::string& sBottomImage, const std::string& sLeftImage, const std::string& sRightImage,
    const std::string& sFrontImage, const std::string& sBackImage) {
  if (m_pInternals->m_pWorldEnvironmentMap) {
    m_pInternals->RemoveTexture(m_pInternals->m_pObjectEnvironmentMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nObjectEnvMapTextureId);
    m_pInternals->m_pWorldEnvironmentMap = osg::NullFC;
  }
  if (sTopImage.empty()) {
    m_pInternals->UpdateShaders();
    return true;
  }

  m_pInternals->m_pObjectEnvironmentMap = m_pInternals->m_pTextureLoader->RetrieveCubemapTexture(
      sTopImage, sBottomImage, sLeftImage, sRightImage, sFrontImage, sBackImage);

  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pObjectEnvironmentMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(m_pInternals->m_pObjectEnvironmentMap,
      VistaOpenSGMultiMaterialShaderCreator::S_nObjectEnvMapTextureId);
}

bool VistaOpenSGMultiMaterial::SetObjectEnvironmentMap(const std::string& sFolder) {
  if (m_pInternals->m_pWorldEnvironmentMap) {
    m_pInternals->RemoveTexture(m_pInternals->m_pObjectEnvironmentMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nObjectEnvMapTextureId);
    m_pInternals->m_pWorldEnvironmentMap = osg::NullFC;
  }
  if (sFolder.empty()) {
    m_pInternals->UpdateShaders();
    return true;
  }

  m_pInternals->m_pObjectEnvironmentMap =
      m_pInternals->m_pTextureLoader->RetrieveCubemapTexture(sFolder);

  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pObjectEnvironmentMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(m_pInternals->m_pObjectEnvironmentMap,
      VistaOpenSGMultiMaterialShaderCreator::S_nObjectEnvMapTextureId);
}

bool VistaOpenSGMultiMaterial::GetHasObjectEnvironmentMap() const {
  return (m_pInternals->m_pObjectEnvironmentMap != osg::NullFC);
}

bool VistaOpenSGMultiMaterial::SetReflectionFactor(const float nReflectionFactor) {
  m_pInternals->m_nReflectionFactor = nReflectionFactor;
  m_pInternals->UpdateReflectionParameters();
  return true;
}
float VistaOpenSGMultiMaterial::GetReflectionFactor() const {
  return m_pInternals->m_nReflectionFactor;
}

bool VistaOpenSGMultiMaterial::SetReflectionBlur(const float nBlur) {
  m_pInternals->m_nReflectionLodBias = nBlur;
  m_pInternals->UpdateReflectionParameters();
  return true;
}
float VistaOpenSGMultiMaterial::GetReflectionBlur() const {
  return m_pInternals->m_nReflectionLodBias;
}

bool VistaOpenSGMultiMaterial::SetBumpMap(
    const std::string& sImageFile, const float nDepthMagnitude, const bool bSaveNormalMap) {
  if (m_pInternals->m_pNormalMap) {
    m_pInternals->RemoveTexture(
        m_pInternals->m_pNormalMap, VistaOpenSGMultiMaterialShaderCreator::S_nNormalMapTextureId);
    m_pInternals->m_pNormalMap = osg::NullFC;
  }
  if (sImageFile.empty()) {
    m_pInternals->UpdateBlending();
    m_pInternals->UpdateShaders();
    return true;
  }
  m_pInternals->m_pNormalMap = m_pInternals->m_pTextureLoader->RetrieveNormalMapTextureFromBumpMap(
      sImageFile, nDepthMagnitude, bSaveNormalMap);
  m_pInternals->UpdateBlending();
  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pNormalMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(
      m_pInternals->m_pNormalMap, VistaOpenSGMultiMaterialShaderCreator::S_nNormalMapTextureId);
}

bool VistaOpenSGMultiMaterial::SetNormalMap(const std::string& sImageFile) {
  if (m_pInternals->m_pNormalMap) {
    m_pInternals->RemoveTexture(
        m_pInternals->m_pNormalMap, VistaOpenSGMultiMaterialShaderCreator::S_nNormalMapTextureId);
    m_pInternals->m_pNormalMap = osg::NullFC;
  }
  if (sImageFile.empty()) {
    m_pInternals->UpdateBlending();
    m_pInternals->UpdateShaders();
    return true;
  }

  m_pInternals->m_pNormalMap = m_pInternals->m_pTextureLoader->RetrieveNormalMapTexture(sImageFile);
  m_pInternals->UpdateBlending();
  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pNormalMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(
      m_pInternals->m_pNormalMap, VistaOpenSGMultiMaterialShaderCreator::S_nNormalMapTextureId);
}
bool VistaOpenSGMultiMaterial::GetHasNormalMap() const {
  return (m_pInternals->m_pNormalMap != osg::NullFC);
}

bool VistaOpenSGMultiMaterial::SetGlossMap(const std::string& sImageFile) {
  if (m_pInternals->m_pGlossMap) {
    m_pInternals->RemoveTexture(
        m_pInternals->m_pGlossMap, VistaOpenSGMultiMaterialShaderCreator::S_nGlossMapTextureId);
    m_pInternals->m_pGlossMap = osg::NullFC;
  }
  if (sImageFile.empty()) {
    m_pInternals->UpdateBlending();
    m_pInternals->UpdateShaders();
    return true;
  }

  m_pInternals->m_pGlossMap = m_pInternals->m_pTextureLoader->RetrieveTexture(sImageFile);
  m_pInternals->UpdateBlending();
  m_pInternals->UpdateShaders();
  if (m_pInternals->m_pGlossMap == osg::NullFC)
    return false;
  return m_pInternals->AddTexture(
      m_pInternals->m_pGlossMap, VistaOpenSGMultiMaterialShaderCreator::S_nGlossMapTextureId);
}
bool VistaOpenSGMultiMaterial::GetHasGlossMap() const {
  return (m_pInternals->m_pGlossMap != osg::NullFC);
}

void VistaOpenSGMultiMaterial::SetUseNativeGLPipeline(const bool bSet) {
  m_pInternals->m_bUseNativePipeline = bSet;
  m_pInternals->UpdateShaders();
}

bool VistaOpenSGMultiMaterial::GetUseNativeGLPipeline() const {
  return m_pInternals->m_bUseNativePipeline;
}

void VistaOpenSGMultiMaterial::SetDrawAsWireFrame(const bool bSet) {
  m_pInternals->m_bDrawWireframe = bSet;
  if (bSet) {
    beginEditCP(m_pInternals->m_pPolygonChunk);
    m_pInternals->m_pPolygonChunk->setFrontMode(GL_LINE);
    m_pInternals->m_pPolygonChunk->setBackMode(GL_LINE);
    endEditCP(m_pInternals->m_pPolygonChunk);
  } else {
    beginEditCP(m_pInternals->m_pPolygonChunk);
    m_pInternals->m_pPolygonChunk->setFrontMode(GL_FILL);
    m_pInternals->m_pPolygonChunk->setBackMode(GL_FILL);
    endEditCP(m_pInternals->m_pPolygonChunk);
  }
}

bool VistaOpenSGMultiMaterial::GetDrawAsWireFrame() const {
  return m_pInternals->m_bDrawWireframe;
}

void VistaOpenSGMultiMaterial::SetUseFog(const bool bSet) {
  m_pInternals->m_bUseFog = bSet;
  m_pInternals->UpdateShaders();
}

bool VistaOpenSGMultiMaterial::GetUseFog() const {
  return m_pInternals->m_bUseFog;
}

void VistaOpenSGMultiMaterial::SetColor(const VistaColor& oColor,
    const float nRelativeDiffuse /*= 1.0f*/, const float nRelativeSpecular /*= 1.0f*/,
    const float nRelativeAmbient /*= 1.0f*/, const float nRelativeEmissive /*= 0.0f */) {
  SetAmbientColor(nRelativeAmbient * oColor);
  SetDiffuseColor(nRelativeDiffuse * oColor);
  SetSpecularColor(nRelativeSpecular * oColor);
  SetEmissiveColor(nRelativeEmissive * oColor);
}

VistaColor VistaOpenSGMultiMaterial::GetAmbientColor() const {
  osg::Color4f oColor = m_pInternals->m_pMaterialChunk->getAmbient();
  return VistaColor(oColor[0], oColor[1], oColor[2], oColor[3]);
}

void VistaOpenSGMultiMaterial::SetAmbientColor(const VistaColor& oColor) {
  beginEditCP(m_pInternals->m_pMaterialChunk);
  m_pInternals->m_pMaterialChunk->setAmbient(
      osg::Color4f(oColor[0], oColor[1], oColor[2], m_pInternals->m_nOpacity));
  endEditCP(m_pInternals->m_pMaterialChunk);
}

VistaColor VistaOpenSGMultiMaterial::GetDiffuseColor() const {
  osg::Color4f oColor = m_pInternals->m_pMaterialChunk->getDiffuse();
  return VistaColor(oColor[0], oColor[1], oColor[2], oColor[3]);
}

void VistaOpenSGMultiMaterial::SetDiffuseColor(const VistaColor& oColor) {
  beginEditCP(m_pInternals->m_pMaterialChunk);
  m_pInternals->m_pMaterialChunk->setDiffuse(
      osg::Color4f(oColor[0], oColor[1], oColor[2], m_pInternals->m_nOpacity));
  endEditCP(m_pInternals->m_pMaterialChunk);
}

VistaColor VistaOpenSGMultiMaterial::GetSpecularColor() const {
  osg::Color4f oColor = m_pInternals->m_pMaterialChunk->getSpecular();
  return VistaColor(oColor[0], oColor[1], oColor[2], oColor[3]);
}

void VistaOpenSGMultiMaterial::SetSpecularColor(const VistaColor& oColor) {
  beginEditCP(m_pInternals->m_pMaterialChunk);
  m_pInternals->m_pMaterialChunk->setSpecular(
      osg::Color4f(oColor[0], oColor[1], oColor[2], m_pInternals->m_nOpacity));
  endEditCP(m_pInternals->m_pMaterialChunk);
}

VistaColor VistaOpenSGMultiMaterial::GetEmissiveColor() const {
  osg::Color4f oColor = m_pInternals->m_pMaterialChunk->getEmission();
  return VistaColor(oColor[0], oColor[1], oColor[2], oColor[3]);
}

void VistaOpenSGMultiMaterial::SetEmissiveColor(const VistaColor& oColor) {
  beginEditCP(m_pInternals->m_pMaterialChunk);
  m_pInternals->m_pMaterialChunk->setEmission(
      osg::Color4f(oColor[0], oColor[1], oColor[2], m_pInternals->m_nOpacity));
  endEditCP(m_pInternals->m_pMaterialChunk);
}

void VistaOpenSGMultiMaterial::SetShininess(const float nShininess) {
  beginEditCP(m_pInternals->m_pMaterialChunk);
  m_pInternals->m_pMaterialChunk->setShininess(nShininess);
  endEditCP(m_pInternals->m_pMaterialChunk);
}
float VistaOpenSGMultiMaterial::GetShininess() const {
  return m_pInternals->m_pMaterialChunk->getShininess();
}

void VistaOpenSGMultiMaterial::SetOpacity(const float nOpacity) {
  beginEditCP(m_pInternals->m_pMaterialChunk);
  osg::Color4f oColor = m_pInternals->m_pMaterialChunk->getDiffuse();
  oColor[3]           = nOpacity;
  m_pInternals->m_pMaterialChunk->setDiffuse(oColor);
  oColor    = m_pInternals->m_pMaterialChunk->getSpecular();
  oColor[3] = nOpacity;
  m_pInternals->m_pMaterialChunk->setSpecular(oColor);
  oColor    = m_pInternals->m_pMaterialChunk->getAmbient();
  oColor[3] = nOpacity;
  m_pInternals->m_pMaterialChunk->setAmbient(oColor);
  endEditCP(m_pInternals->m_pMaterialChunk);
  m_pInternals->m_nOpacity = nOpacity;
  m_pInternals->UpdateBlending();
}
float VistaOpenSGMultiMaterial::GetOpacity() const {
  return m_pInternals->m_nOpacity;
}

void VistaOpenSGMultiMaterial::SetBackfaceCulling(bool bSet) {
  m_pInternals->m_bCullBackFace = bSet;
  GLenum eMode;
  if (m_pInternals->m_bCullBackFace && m_pInternals->m_bCullFrontFace)
    eMode = GL_FRONT_AND_BACK;
  else if (m_pInternals->m_bCullBackFace)
    eMode = GL_BACK;
  else if (m_pInternals->m_bCullFrontFace)
    eMode = GL_FRONT;
  else
    eMode = GL_NONE;
  beginEditCP(m_pInternals->m_pPolygonChunk);
  m_pInternals->m_pPolygonChunk->setCullFace(eMode);
  endEditCP(m_pInternals->m_pPolygonChunk);
}

bool VistaOpenSGMultiMaterial::GetBackfaceCulling() const {
  return m_pInternals->m_bCullBackFace;
}

void VistaOpenSGMultiMaterial::SetFrontfaceCulling(bool bSet) {
  m_pInternals->m_bCullFrontFace = bSet;
  GLenum eMode;
  if (m_pInternals->m_bCullBackFace && m_pInternals->m_bCullFrontFace)
    eMode = GL_FRONT_AND_BACK;
  else if (m_pInternals->m_bCullBackFace)
    eMode = GL_BACK;
  else if (m_pInternals->m_bCullFrontFace)
    eMode = GL_FRONT;
  else
    eMode = GL_NONE;
  beginEditCP(m_pInternals->m_pPolygonChunk);
  m_pInternals->m_pPolygonChunk->setCullFace(eMode);
  endEditCP(m_pInternals->m_pPolygonChunk);
}

bool VistaOpenSGMultiMaterial::GetFrontfaceCulling() const {
  return m_pInternals->m_bCullFrontFace;
}

void VistaOpenSGMultiMaterial::SetIsLit(const bool bSet) {
  beginEditCP(m_pInternals->m_pMaterialChunk);
  m_pInternals->m_pMaterialChunk->setLit(bSet);
  endEditCP(m_pInternals->m_pMaterialChunk);
}
bool VistaOpenSGMultiMaterial::GetIsLit() const {
  return m_pInternals->m_pMaterialChunk->getLit();
}

bool VistaOpenSGMultiMaterial::Configure(const VistaPropertyList& oConfig) {
  std::string              sFilename;
  std::vector<std::string> vecFiles;
  if (oConfig.GetValue("BASE_MAP", sFilename)) {
    if (SetBaseMap(sFilename) == false) {
      vstr::warnp() << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set base map \""
                    << sFilename << "\"" << std::endl;
    }
  }
  if (oConfig.GetValue("BLEND_MAP", sFilename)) {
    if (SetBlendMap(sFilename) == false) {
      vstr::warnp()
          << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set blend map \""
          << sFilename << "\"" << std::endl;
    }
  }
  if (oConfig.GetValue("SPECULAR_MAP", sFilename)) {
    if (SetSpecularMap(sFilename) == false) {
      vstr::warnp()
          << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set specular map \""
          << sFilename << "\"" << std::endl;
    }
  }
  if (oConfig.GetValue("BUMP_MAP", sFilename)) {
    bool  bStore          = oConfig.GetValueOrDefault("STORE_NORMALMAP", true);
    float nDepthMagnitude = oConfig.GetValueOrDefault("NORMAL_MAGNITUDE", 1.0f);
    if (SetBumpMap(sFilename, nDepthMagnitude, bStore) == false) {
      vstr::warnp() << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set bump map \""
                    << sFilename << "\"" << std::endl;
    }
  }
  if (oConfig.GetValue("NORMAL_MAP", sFilename)) {
    if (SetNormalMap(sFilename) == false) {
      vstr::warnp()
          << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set normal map \""
          << sFilename << "\"" << std::endl;
    }
  }
  bool bEnvMapIsRelative = oConfig.GetValueOrDefault("ENVIRONMENT_MAP_IS_RELATIVE", true);
  if (oConfig.GetValue("WORLD_ENVIRONMENT_MAP", vecFiles) ||
      (!bEnvMapIsRelative && oConfig.GetValue("ENVIRONMENT_MAP", vecFiles))) {
    bool bSuccess = false;
    if (vecFiles.size() == 6) {
      bSuccess = SetWorldEnvironmentMap(
          vecFiles[0], vecFiles[1], vecFiles[2], vecFiles[3], vecFiles[4], vecFiles[5]);
    } else if (vecFiles.size() == 1) {
      bSuccess = SetWorldEnvironmentMap(vecFiles[0]);
    }
    if (bSuccess == false) {
      vstr::warnp()
          << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set world environment map"
          << std::endl;
    }
  }
  if (oConfig.GetValue("OBJECT_ENVIRONMENT_MAP", vecFiles) ||
      (bEnvMapIsRelative && oConfig.GetValue("ENVIRONMENT_MAP", vecFiles))) {
    bool bSuccess = false;
    if (vecFiles.size() == 6) {
      bSuccess = SetObjectEnvironmentMap(
          vecFiles[0], vecFiles[1], vecFiles[2], vecFiles[3], vecFiles[4], vecFiles[5]);
    } else if (vecFiles.size() == 1) {
      bSuccess = SetObjectEnvironmentMap(vecFiles[0]);
    }
    if (bSuccess == false) {
      vstr::warnp() << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set object "
                       "environment map"
                    << std::endl;
    }
  }
  if (oConfig.GetValue("GLOSS_MAP", sFilename)) {
    if (SetGlossMap(sFilename) == false) {
      vstr::warnp()
          << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() failed to set gloss map \""
          << sFilename << "\"" << std::endl;
    }
  }

  VistaColor oColor;
  if (oConfig.GetValue("AMBIENT", oColor)) {
    SetAmbientColor(oColor);
  }
  if (oConfig.GetValue("DIFFUSE", oColor)) {
    SetDiffuseColor(oColor);
  }
  if (oConfig.GetValue("SPECULAR", oColor)) {
    SetSpecularColor(oColor);
  }
  if (oConfig.GetValue("EMISSIVE", oColor)) {
    SetEmissiveColor(oColor);
  }

  float nValue;
  if (oConfig.GetValue("SHININESS", nValue)) {
    SetShininess(nValue);
  }
  if (oConfig.GetValue("OPACITY", nValue)) {
    SetOpacity(nValue);
  }
  if (oConfig.GetValue("BLEND_FACTOR", nValue)) {
    SetBlendFactor(nValue);
  }
  if (oConfig.GetValue("REFLECTION_FACTOR", nValue)) {
    SetReflectionFactor(nValue);
  }
  if (oConfig.GetValue("REFLECTION_BLUR", nValue)) {
    SetReflectionBlur(nValue);
  }

  if (oConfig.GetValue("TEXTURE_SCALE_S", nValue)) {
    SetTextureScaleS(nValue);
  }
  if (oConfig.GetValue("TEXTURE_SCALE_T", nValue)) {
    SetTextureScaleT(nValue);
  }

  std::string sValue;
  if (oConfig.GetValue("BLENDING_MODE", sValue)) {
    if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "AUTO")) {
      SetBlendingMode(BM_AUTO);
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "BLEND")) {
      SetBlendingMode(BM_BLENDING);
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "DONT_BLEND")) {
      SetBlendingMode(BM_NO_BLENDING);
    } else {
      vstr::warnp() << "ParseMaterialsFromFile( \"" << sFilename << "\" ) -- BLENDING_MODE value ["
                    << sValue << "] is invalid" << std::endl;
    }
  }
  if (oConfig.GetValue("CULLING", sValue)) {
    if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "NO") ||
        VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "OFF") ||
        VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "FALSE")) {
      SetBackfaceCulling(false);
      SetFrontfaceCulling(false);
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "TRUE") ||
               VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "BACK")) {
      SetBackfaceCulling(true);
      SetFrontfaceCulling(false);
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sValue, "FRONT")) {
      SetBackfaceCulling(false);
      SetFrontfaceCulling(true);
    } else {
      vstr::warnp() << "ParseMaterialsFromFile( \"" << sFilename << "\" ) -- CULLING value ["
                    << sValue << "] is invalid" << std::endl;
    }
  }
  if (oConfig.HasProperty("IS_LIT")) {
    SetIsLit(oConfig.GetValue<bool>("IS_LIT"));
  }
  if (oConfig.HasProperty("USE_NATIVE_GL_PIPELINE")) {
    SetUseNativeGLPipeline(oConfig.GetValue<bool>("USE_NATIVE_GL_PIPELINE"));
  }
  if (oConfig.HasProperty("DRAW_WIREFRAME")) {
    SetDrawAsWireFrame(oConfig.GetValue<bool>("DRAW_WIREFRAME"));
  }

  bool bUseFog = false;
  if (oConfig.GetValue("USE_FOG", bUseFog)) {
    SetUseFog(bUseFog);
  }

  VistaPropertyList oCopy(oConfig);
  oCopy.RemoveProperty("BASE_MAP");
  oCopy.RemoveProperty("BLEND_MAP");
  oCopy.RemoveProperty("SPECULAR_MAP");
  oCopy.RemoveProperty("STORE_NORMALMAP");
  oCopy.RemoveProperty("NORMAL_MAGNITUDE");
  oCopy.RemoveProperty("NORMAL_MAP");
  oCopy.RemoveProperty("BUMP_MAP");
  oCopy.RemoveProperty("ENVIRONMENT_MAP");
  oCopy.RemoveProperty("WORLD_ENVIRONMENT_MAP");
  oCopy.RemoveProperty("OBJECT_ENVIRONMENT_MAP");
  oCopy.RemoveProperty("ENVIRONMENT_MAP_IS_RELATIVE");
  oCopy.RemoveProperty("GLOSS_MAP");
  oCopy.RemoveProperty("AMBIENT");
  oCopy.RemoveProperty("DIFFUSE");
  oCopy.RemoveProperty("SPECULAR");
  oCopy.RemoveProperty("EMISSIVE");
  oCopy.RemoveProperty("SHININESS");
  oCopy.RemoveProperty("OPACITY");
  oCopy.RemoveProperty("TEXTURE_SCALE_S");
  oCopy.RemoveProperty("TEXTURE_SCALE_T");
  oCopy.RemoveProperty("BLEND_FACTOR");
  oCopy.RemoveProperty("REFLECTION_FACTOR");
  oCopy.RemoveProperty("REFLECTION_BLUR");
  oCopy.RemoveProperty("USE_EXISTING_MATERIAL");
  oCopy.RemoveProperty("REPLACE_VERTEX_COLORS");
  oCopy.RemoveProperty("BLEND_MODE");
  oCopy.RemoveProperty("CULLING");
  oCopy.RemoveProperty("IS_LIT");
  oCopy.RemoveProperty("USE_FOG");
  oCopy.RemoveProperty("USE_NATIVE_GL_PIPELINE");
  oCopy.RemoveProperty("DRAW_WIREFRAME");
  for (VistaPropertyList::const_iterator it = oCopy.begin(); it != oCopy.end(); ++it) {
    vstr::warnp() << "VistaOpenSGMultiMaterial::NewMaterialFromConfig() -- "
                     "unknown property list entry ["
                  << (*it).first << "]" << std::endl;
  }

  return true;
}

void VistaOpenSGMultiMaterial::SetSortKey(const int nKey) {
  beginEditCP(m_pInternals->m_pMaterial);
  m_pInternals->m_pMaterial->setSortKey(nKey);
  endEditCP(m_pInternals->m_pMaterial);
}

int VistaOpenSGMultiMaterial::GetSortKey() const {
  return m_pInternals->m_pMaterial->getSortKey();
}

int VistaOpenSGMultiMaterial::GetBlendingMode() const {
  return m_pInternals->m_eBlendingMode;
}

void VistaOpenSGMultiMaterial::SetBlendingMode(const int eMode) {
  m_pInternals->m_eBlendingMode = (BlendingMode)eMode;
  m_pInternals->UpdateBlending();
}

osg::MaterialPtr VistaOpenSGMultiMaterial::GetOSGMaterial() {
  m_pInternals->VerifyShadersHaveBeenCreated();
  return m_pInternals->m_pMaterial;
}

bool VistaOpenSGMultiMaterial::ApplyTo(osg::MaterialDrawablePtr pDrawable) {
  if (m_pInternals->VerifyShadersHaveBeenCreated() == false)
    return false;
  beginEditCP(pDrawable);
  pDrawable->setMaterial(m_pInternals->m_pMaterial);
  endEditCP(pDrawable);
  return true;
}

bool VistaOpenSGMultiMaterial::ApplyTo(
    osg::GeometryPtr pGeometry, const bool bReplaceVertexColors) {
  if (m_pInternals->VerifyShadersHaveBeenCreated() == false)
    return false;
  beginEditCP(pGeometry);
  pGeometry->setMaterial(m_pInternals->m_pMaterial);
  endEditCP(pGeometry);

  if (bReplaceVertexColors) {
    // beginEditCP( pGeometry );
    //	osg::MFUInt16& vecGeoIndices = pGeometry->getIndexMapping();
    //	std::size_t nColorIndex = -1;
    //	std::size_t nTotalIndices = vecGeoIndices.size();
    //	for( int i = 0; i < vecGeoIndices.size(); ++i )
    //	{
    //		if( vecGeoIndices[i] == osg::Geometry::MapNormal )
    //		{
    //			vecGeoIndices.erase( vecGeoIndices.begin() + i );
    //			nColorIndex = 1;
    //			break;
    //		}
    //	}
    //	if( nColorIndex >= 0 )
    //	{
    //		osg::GeoIndicesPtr pIndices = pGeometry->editIndices();
    //		beginEditCP( pIndices );
    //		{
    //			// indices for the polygon
    //			for( i = 0; i < n; ++i )
    //			{
    //				if( vFormat.coordinate == VistaVertexFormat::COORDINATE )
    //					indices->push_back(vertices[i].GetCoordinateIndex());

    //				if( vFormat.color ==  VistaVertexFormat::COLOR_RGB )
    //					indices->push_back(vertices[i].GetColorIndex());

    //				if( vFormat.normal == VistaVertexFormat::NORMAL )
    //					indices->push_back(vertices[i].GetNormalIndex());

    //				if( vFormat.textureCoord == VistaVertexFormat::TEXTURE_COORD_2D    )
    //					indices->push_back(vertices[i].GetTextureCoordinateIndex());
    //			}
    //		}
    //		endEditCP( pIndices );
    //	}
    // endEditCP( pGeometry );

    beginEditCP(pGeometry);
    osg::Color4f        oFullColor = m_pInternals->m_pMaterialChunk->getDiffuse();
    osg::GeoColorsPtr   pColors    = pGeometry->getColors();
    osg::GeoColors4fPtr pNewColors = osg::GeoColors4f::create();
    if (pColors != osg::NullFC && pColors->size() > 0) {
      beginEditCP(pNewColors);
      pNewColors->getField().resize(pColors->size());
      for (osg::UInt32 nIndex = 0; nIndex < pColors->size(); ++nIndex) {
        pNewColors->getField().setValue(oFullColor, nIndex);
      }
      endEditCP(pNewColors);
    }
    pGeometry->setColors(pNewColors);
    endEditCP(pGeometry);
  }
  return true;
}

bool VistaOpenSGMultiMaterial::ApplyTo(VistaGeometry* pGeometry, const bool bReplaceVertexColors) {
  VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeometry->GetData());
  if (pData == NULL)
    return false;
  return ApplyTo(pData->GetGeometry(), bReplaceVertexColors);
}

bool VistaOpenSGMultiMaterial::ApplyToGeomNodesInSubtree(
    IVistaNode* pNode, const bool bReplaceVertexColors) {
  std::vector<IVistaNode*> vecGeomNodes;
  VistaSceneGraph::GetAllSubNodesOfType(vecGeomNodes, VISTA_GEOMNODE, pNode);
  bool bSuccess = true;
  for (std::vector<IVistaNode*>::iterator itNode = vecGeomNodes.begin();
       itNode != vecGeomNodes.end(); ++itNode) {
    VistaGeomNode* pNode = Vista::assert_cast<VistaGeomNode*>((*itNode));
    bSuccess &= ApplyTo(pNode->GetGeometry(), bReplaceVertexColors);
  }
  return bSuccess;
}

VistaOpenSGMultiMaterial* VistaOpenSGMultiMaterial::Clone() {
  VistaOpenSGMultiMaterial* pClone = new VistaOpenSGMultiMaterial(m_pInternals->m_pTextureLoader);
  CopyPropertiesToOtherMaterial(pClone);
  return pClone;
}

int VistaOpenSGMultiMaterial::GetFogType() const {
  return m_pInternals->m_nFogType;
}

void VistaOpenSGMultiMaterial::SetFogType(const int nGLFogType) {
  m_pInternals->m_nFogType = nGLFogType;
  m_pInternals->UpdateFogParameter();
}

void VistaOpenSGMultiMaterial::SetTextureScale(const float fScaleS, const float fScaleT) {
  m_pInternals->m_fTexScaleS = fScaleS;
  m_pInternals->m_fTexScaleT = fScaleT;
  m_pInternals->UpdateTexScaleParameters();
}

void VistaOpenSGMultiMaterial::SetTextureScaleS(const float fScale) {
  m_pInternals->m_fTexScaleS = fScale;
  m_pInternals->UpdateTexScaleParameters();
}

void VistaOpenSGMultiMaterial::SetTextureScaleT(const float fScale) {
  m_pInternals->m_fTexScaleT = fScale;
  m_pInternals->UpdateTexScaleParameters();
}

float VistaOpenSGMultiMaterial::GetTextureScaleS() const {
  return m_pInternals->m_fTexScaleS;
}

float VistaOpenSGMultiMaterial::FetTextureScaleT() const {
  return m_pInternals->m_fTexScaleT;
}

void VistaOpenSGMultiMaterial::CopyPropertiesToOtherMaterial(
    VistaOpenSGMultiMaterial* pOther) const {
  *pOther->m_pInternals = *m_pInternals;
  // we can copy everythink except for the actual material and its non-texture chunks
  pOther->m_pInternals->m_pMaterialChunk =
      osg::MaterialChunkPtr::dcast(deepClone(m_pInternals->m_pMaterialChunk));
  pOther->m_pInternals->m_pPolygonChunk =
      osg::PolygonChunkPtr::dcast(deepClone(m_pInternals->m_pPolygonChunk));
  pOther->m_pInternals->m_pBlendChunk          = osg::NullFC;
  pOther->m_pInternals->m_bBlendingEnabled     = false;
  pOther->m_pInternals->m_bBeforeFirstCreation = true;
  pOther->m_pInternals->m_pMaterial            = osg::ChunkMaterial::create();

  beginEditCP(pOther->m_pInternals->m_pMaterial);
  pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pMaterialChunk);
  pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pPolygonChunk);
  pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pShaderUniforms);
  if (pOther->m_pInternals->m_pBaseMap)
    pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pBaseMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nBaseMapTextureId);
  if (pOther->m_pInternals->m_pBlendMap)
    pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pBlendMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nBlendMapTextureId);
  if (pOther->m_pInternals->m_pSpecularMap)
    pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pSpecularMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nSpecularMapTextureId);
  if (pOther->m_pInternals->m_pNormalMap)
    pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pNormalMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nNormalMapTextureId);
  if (pOther->m_pInternals->m_pWorldEnvironmentMap)
    pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pWorldEnvironmentMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nWorldEnvMapTextureId);
  if (pOther->m_pInternals->m_pObjectEnvironmentMap)
    pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pObjectEnvironmentMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nObjectEnvMapTextureId);
  if (pOther->m_pInternals->m_pGlossMap)
    pOther->m_pInternals->m_pMaterial->addChunk(pOther->m_pInternals->m_pGlossMap,
        VistaOpenSGMultiMaterialShaderCreator::S_nGlossMapTextureId);
  endEditCP(pOther->m_pInternals->m_pMaterial);
  pOther->m_pInternals->UpdateBlending();
}

bool VistaOpenSGMultiMaterial::GetIsUsingBlending() const {
  return m_pInternals->m_bBlendingEnabled;
}
