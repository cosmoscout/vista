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

#include "VistaProximitySign.h"

#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <cassert>

#ifdef VISTA_SYS_OPENSG
#if defined(WIN32)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4312)
#pragma warning(disable : 4267)
#pragma warning(disable : 4275)
#endif

#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>

#include <OpenSG/OSGDepthChunk.h>
#include <OpenSG/OSGMaterial.h>
#include <OpenSG/OSGMaterialDrawable.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace vpsign {
VistaType::uint32 S_nTransparent = 0X00000000;
VistaType::uint32 S_nBackground  = 0xFF0000FF;
VistaType::uint32 S_nBorder      = 0xFFFFFFFF;
VistaType::uint32 S_nStrip       = 0xFFFFFFFF;

// creates an array containing 32bit-RGBA-data of a no-passage sign
// the returned array is allocated on the heap, and should be delete[]-ed
VistaType::uint32* CreateTexture(const int nSize, const float nBorderWidth = 0.2f,
    const float nStripPercentageHor = 0.6f, const float nStripPercentageVert = 0.15f) {
  VistaType::uint32* pBuffer = new VistaType::uint32[nSize * nSize];
  float              nDelta  = 1.0f / (float)nSize;
  float              nYPos   = -0.5f + 0.5f * nDelta;
  VistaType::uint32* pVal    = pBuffer;
  for (int nY = 0; nY < nSize; ++nY, nYPos += nDelta) {
    float nXPos = -0.5f + 0.5f * nDelta;
    for (int nX = 0; nX < nSize; ++nX, nXPos += nDelta) {
      float nSqDist = nXPos * nXPos + nYPos * nYPos;
      if (nSqDist > 0.25f) {
        // outside of circle
        (*pVal) = S_nTransparent;
      } else if (nSqDist > (1.0f - nBorderWidth) * 0.25f) {
        // Border
        (*pVal) = S_nBorder;
      } else if (std::abs(2.0f * nXPos) < nStripPercentageHor &&
                 std::abs(2.0f * nYPos) < nStripPercentageVert) {
        // in strip
        (*pVal) = S_nStrip;
      } else {
        // normal sign color
        (*pVal) = S_nBackground;
      }

      ++pVal;
    }
  }
  return pBuffer;
}
} // namespace vpsign

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaProximitySign::VistaProximitySign(VistaEventManager* pManager,
    const float nBeginWarningDistance, const float nMaxWarningDistance,
    const bool bDisableOcclusion, VistaGraphicsManager* pGraphicsManager)
    : IVistaProximityWarningBase(pManager, nBeginWarningDistance, nMaxWarningDistance)
    , m_bEnabled(true)
    , m_nWarningLevel(0)
    , m_bCurrentFlashState(false) {
  VistaSceneGraph* pSG = pGraphicsManager->GetSceneGraph();
  m_pPositionNode      = pSG->NewTransformNode(pSG->GetRoot());
  m_pPositionNode->SetName("ProximityWarning_Pos");
  m_pScaleNode = pSG->NewTransformNode(m_pPositionNode);
  m_pPositionNode->SetName("ProximityWarning_Scake");
  VistaGeometryFactory oFac(pSG);
  m_pGeometry = oFac.CreatePlane(1.0f, 1.0f, 1, 1, Vista::Z);
  m_pGeometry->SetMaterial(VistaMaterial(VistaColor::BLACK, VistaColor::BLACK, VistaColor::BLACK,
      VistaColor::WHITE, 0.0f, 1.0f, "sign_material"));

  pSG->NewGeomNode(m_pScaleNode, m_pGeometry)->SetName("ProximityWarning_Geom");

  if (bDisableOcclusion) {
#ifdef VISTA_SYS_OPENSG
    VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(m_pGeometry->GetData());
    assert(pData);

    osg::ChunkMaterialPtr pMaterial =
        osg::ChunkMaterialPtr::dcast(pData->GetGeometry()->getMaterial());
    if (pMaterial == osg::NullFC) {
      // create new one
      pMaterial = osg::ChunkMaterial::create();
      pData->GetGeometry()->setMaterial(pMaterial);
    }
    osg::DepthChunkPtr pDepthCunk = osg::DepthChunk::create();
    beginEditCP(pDepthCunk);
    pDepthCunk->setEnable(false); // disable depth testing
    endEditCP(pDepthCunk);
    beginEditCP(pMaterial);
    pMaterial->setSortKey(42);
    pMaterial->addChunk(pDepthCunk);
    endEditCP(pMaterial);
#endif
  }

  m_pPositionNode->SetIsEnabled(false);
}

VistaProximitySign::~VistaProximitySign() {
  delete m_pPositionNode;
}

bool VistaProximitySign::DoUpdate(const float nMinDistance, const float nWarningLevel,
    const VistaVector3D& v3PointOnBounds, const VistaVector3D& v3UserPosition,
    const VistaQuaternion& qUserOrientation) {
  if (m_bEnabled == false)
    return false;
  m_nWarningLevel = nWarningLevel;
  if (m_nWarningLevel > 0) {
    VistaVector3D v3RelDir = v3PointOnBounds - v3UserPosition;
    v3RelDir.Normalize();
    m_pPositionNode->SetRotation(VistaQuaternion(Vista::ViewVector, v3RelDir));
    m_pPositionNode->SetTranslation(v3PointOnBounds);
  }
  return true;
}

void VistaProximitySign::SetScale(const float nXScale, const float nYScale) {
  m_pScaleNode->SetScale(nXScale, nYScale, 1.0f);
}

bool VistaProximitySign::SetTexture(const std::string sTexture) const {
  return m_pGeometry->SetTexture(sTexture);
}

bool VistaProximitySign::SetDefaultTexture() const {
  const int          nRes  = 256;
  VistaType::uint32* pData = vpsign::CreateTexture(nRes);
  bool               bRes  = m_pGeometry->SetTexture(nRes, nRes, 8, true, (VistaType::byte*)pData);
  delete[] pData;
  return bRes;
}

void VistaProximitySign::SetParentNode(VistaGroupNode* pNode) {
  pNode->AddChild(m_pPositionNode);
}

bool VistaProximitySign::GetIsEnabled() const {
  return m_bEnabled;
}

bool VistaProximitySign::SetIsEnabled(const bool bSet) {
  m_bEnabled = bSet;
  if (m_bEnabled == false)
    m_pPositionNode->SetIsEnabled(false);
  return true;
}

bool VistaProximitySign::DoTimeUpdate(
    VistaType::systemtime nTime, const float nOpacityScale, const bool bFlashState) {
  float nOpacity = m_nWarningLevel * nOpacityScale;

  if (nOpacity == 0) {
    m_pPositionNode->SetIsEnabled(false);
  } else {
    m_pPositionNode->SetIsEnabled(true);
    if (bFlashState != m_bCurrentFlashState) {
      if (bFlashState)
        m_pGeometry->SetMaterial(VistaMaterial(VistaColor::BLACK, VistaColor::BLACK,
            VistaColor::BLACK, VistaColor::GRAY, 0.0f, 1.0f, "sign_material_flash"));
      else
        m_pGeometry->SetMaterial(VistaMaterial(VistaColor::BLACK, VistaColor::BLACK,
            VistaColor::BLACK, VistaColor::WHITE, 0.0f, 1.0f, "sign_material"));
      m_bCurrentFlashState = bFlashState;
    }
    m_pGeometry->SetTransparency(1.0f - nOpacity);
  }
  return true;
}
