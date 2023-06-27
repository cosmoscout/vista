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

#include "VistaOpenSGSkybox.h"

#if defined(WIN32)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#endif

#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGImageFileHandler.h>
#include <OpenSG/OSGSkyBackground.h>
#include <OpenSG/OSGTextureChunk.h>
#include <OpenSG/OSGViewport.h>

#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class SkyboxData {
 public:
  SkyboxData(VistaOpenSGSkybox* pParent, const std::string& strTop, const std::string& strBottom,
      const std::string& strLeft, const std::string& strRight, const std::string& strFront,
      const std::string& strBack)
      : m_pParent(pParent) {
    m_pSkyBox = MakeBox(strTop, strBottom, strLeft, strRight, strFront, strBack);
    addRefCP(m_pSkyBox);
  }

  ~SkyboxData() {
    while (!m_liVp.empty()) {
      m_pParent->DetachFromViewport(m_liVp.begin()->m_pVp);
    }

    for (std::size_t i = 0; i < m_vecTextures.size(); ++i) {
      subRefCP(m_vecTextures[i]);
    }

    subRefCP(m_pSkyBox);
  }

  OSG::TextureChunkPtr MakeBoxTexture(const char* sTexName) {
    OSG::ImagePtr pImage = OSG::ImageFileHandler::the().read(sTexName);
    if (pImage == OSG::NullFC)
      return OSG::NullFC;

    OSG::TextureChunkPtr pTexChunkPtr = OSG::TextureChunk::create();
    beginEditCP(pTexChunkPtr);
    {
      pTexChunkPtr->setImage(pImage);

      pTexChunkPtr->setWrapS(GL_CLAMP_TO_EDGE);
      pTexChunkPtr->setWrapT(GL_CLAMP_TO_EDGE);
      pTexChunkPtr->setWrapR(GL_CLAMP_TO_EDGE);

      pTexChunkPtr->setMinFilter(GL_LINEAR);
      pTexChunkPtr->setMagFilter(GL_LINEAR);
      // pTexChunkPtr->setEnvMode( GL_MODULATE );
      pTexChunkPtr->setEnvMode(GL_REPLACE);
    }
    endEditCP(pTexChunkPtr);

    m_vecTextures.push_back(pTexChunkPtr);
    addRefCP(pTexChunkPtr);

    return pTexChunkPtr;
  }

  OSG::SkyBackgroundPtr MakeBox(const std::string& strTop, const std::string& strBottom,
      const std::string& strLeft, const std::string& strRight, const std::string& strFront,
      const std::string& strBack, OSG::UInt32 nSphereRes = 16) {
    OSG::SkyBackgroundPtr pOsgSky = OSG::SkyBackground::create();
    beginEditCP(pOsgSky);

    pOsgSky->setBackTexture(MakeBoxTexture(strBack.c_str()));
    pOsgSky->setTopTexture(MakeBoxTexture(strTop.c_str()));
    pOsgSky->setLeftTexture(MakeBoxTexture(strLeft.c_str()));
    pOsgSky->setRightTexture(MakeBoxTexture(strRight.c_str()));
    pOsgSky->setBottomTexture(MakeBoxTexture(strBottom.c_str()));
    pOsgSky->setFrontTexture(MakeBoxTexture(strFront.c_str()));
    /*
     */
    pOsgSky->setSphereRes(nSphereRes);

    pOsgSky->getMFSkyColor()->push_back(osg::Color4f(1, 1, 1, 1));
    pOsgSky->getMFGroundColor()->push_back(osg::Color4f(1, 1, 1, 1));

    endEditCP(pOsgSky);

    return pOsgSky;
  }

  bool SetColor(float fR, float fG, float fB, float fA) {
    if (m_pSkyBox == NULL)
      return false;
    beginEditCP(m_pSkyBox);
    m_pSkyBox->getMFSkyColor()->setValue(osg::Color4f(fR, fG, fB, fA), 0);
    endEditCP(m_pSkyBox);
    return true;
  }
  bool GetColor(float& fR, float& fG, float& fB, float& fA) const {
    if (m_pSkyBox == NULL)
      return false;
    fR = m_pSkyBox->getMFSkyColor()->getValue(0)[0];
    fG = m_pSkyBox->getMFSkyColor()->getValue(0)[1];
    fB = m_pSkyBox->getMFSkyColor()->getValue(0)[2];
    fA = m_pSkyBox->getMFSkyColor()->getValue(0)[3];
    return true;
  }

  OSG::SkyBackgroundPtr m_pSkyBox;

  struct _sHlp {
    _sHlp(VistaViewport* pVp, OSG::BackgroundPtr p, OSG::BackgroundPtr pR)
        : m_pVp(pVp)
        , m_pOldBackground(p)
        , m_pOldRightBack(pR) {
    }

    bool operator==(const _sHlp& other) const {
      return other.m_pVp == m_pVp;
    }
    bool operator<(const _sHlp& other) const {
      return other.m_pVp < m_pVp;
    }

    VistaViewport*     m_pVp;
    OSG::BackgroundPtr m_pOldBackground, m_pOldRightBack;
  };
  typedef std::set<_sHlp> VPSET;
  VPSET                   m_liVp;

  std::vector<OSG::TextureChunkPtr> m_vecTextures;

  VistaOpenSGSkybox* m_pParent;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenSGSkybox::VistaOpenSGSkybox(const std::string& strTop, const std::string& strBottom,
    const std::string& strLeft, const std::string& strRight, const std::string& strFront,
    const std::string& strBack)
    : m_pData(NULL) {
  m_pData = new SkyboxData(this, strTop, strBottom, strLeft, strRight, strFront, strBack);
}

VistaOpenSGSkybox::~VistaOpenSGSkybox() {
  delete m_pData;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaOpenSGSkybox::AttachToViewport(VistaViewport* pViewport) {
  VistaOpenSGDisplayBridge::ViewportData* pData =
      dynamic_cast<VistaOpenSGDisplayBridge::ViewportData*>(pViewport->GetData());
  if (!pData)
    return false;

  OSG::ViewportPtr   pOsgViewPort   = pData->GetOpenSGViewport();
  OSG::BackgroundPtr pOsgBackground = pOsgViewPort->getBackground();
  beginEditCP(pOsgViewPort);
  pOsgViewPort->setBackground(m_pData->m_pSkyBox);
  endEditCP(pOsgViewPort);

  OSG::BackgroundPtr pOsgOriginalBackground = OSG::NullFC;

  pOsgViewPort = pData->GetOpenSGRightViewport();
  if (pOsgViewPort != OSG::NullFC) {
    pOsgOriginalBackground = pOsgViewPort->getBackground();
    beginEditCP(pOsgViewPort);
    pOsgViewPort->setBackground(m_pData->m_pSkyBox);
    endEditCP(pOsgViewPort);
  }

  SkyboxData::_sHlp s(pViewport, pOsgBackground, pOsgOriginalBackground);
  m_pData->m_liVp.insert(s);
  return true;
}

bool VistaOpenSGSkybox::DetachFromViewport(VistaViewport* pViewport) {
  VistaOpenSGDisplayBridge::ViewportData* pData =
      dynamic_cast<VistaOpenSGDisplayBridge::ViewportData*>(pViewport->GetData());
  if (!pData)
    return false;

  const SkyboxData::_sHlp     s1(pViewport, OSG::NullFC, OSG::NullFC);
  SkyboxData::VPSET::iterator it = m_pData->m_liVp.find(s1);
  if (it == m_pData->m_liVp.end())
    return false;

  OSG::ViewportPtr pOsgViewport = pData->GetOpenSGViewport();
  pOsgViewport->setBackground(it->m_pOldBackground);

  pOsgViewport = pData->GetOpenSGRightViewport();
  if (pOsgViewport != OSG::NullFC) {
    pOsgViewport->setBackground(it->m_pOldRightBack);
  }

  m_pData->m_liVp.erase(s1);
  return true;
}

bool VistaOpenSGSkybox::GetIsAttachedToViewport(VistaViewport* pViewport) {
  return (m_pData->m_liVp.find(SkyboxData::_sHlp(pViewport, OSG::NullFC, OSG::NullFC)) !=
          m_pData->m_liVp.end());
}

bool VistaOpenSGSkybox::SetColor(float fR, float fG, float fB, float fA) {
  return m_pData->SetColor(fR, fG, fB, fA);
}

bool VistaOpenSGSkybox::SetColor(const VistaColor& oColor) {
  return m_pData->SetColor(oColor[0], oColor[1], oColor[2], oColor[3]);
}

bool VistaOpenSGSkybox::GetColor(float& fR, float& fG, float& fB, float& fA) const {
  return m_pData->GetColor(fR, fG, fB, fA);
}

VistaColor VistaOpenSGSkybox::GetColor() const {
  VistaColor oRet;
  return m_pData->GetColor(oRet[0], oRet[1], oRet[2], oRet[3]);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
