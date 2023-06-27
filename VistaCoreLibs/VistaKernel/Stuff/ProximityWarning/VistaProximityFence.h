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

#ifndef _VISTAPROXIMITYBARRIERTAPE_H
#define _VISTAPROXIMITYBARRIERTAPE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaProximityWarningBase.h"

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGraphicsManager;
class VistaOpenGLNode;
class VistaGroupNode;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaProximityBarrierTape : public IVistaProximityWarningBase {
 public:
  VistaProximityBarrierTape(VistaEventManager* pMananger, const float nBeginWarningDistance,
      const float nMaxWarningDistance, const bool bDisableOcclusion,
      VistaGraphicsManager* pGraphicsManager);
  virtual ~VistaProximityBarrierTape();

  enum HeightMode {
    HM_FIXED_HEIGHT,
    HM_ADJUST_TO_VIEWER_POS_AT_START,
    HM_ADJUST_TO_VIEWER_POS,
    HM_ADJUST_TO_OBJECT_POS_AT_START,
    HM_ADJUST_TO_OBJECT_POS,
  };
  HeightMode GetHeightMode() const;
  void       SetHeightMode(const HeightMode oValue);

  // height of the first tape
  float GetTapeHeight() const;
  void  SetTapeHeight(const float nHeight);

  // horizontal spacing between tape, 0=just one tape
  float GetTapeSpacing() const;
  void  SetTapeSpacing(const float nSpacing);

  // horizontal width of the tape
  float GetTapeWidth() const;
  void  SetTapeWidth(const float nWidth) const;

  float GetDistanceFromWall() const;
  void  SetDistanceFromWall(const float oValue);

  void SetParentNode(VistaGroupNode* pParent);

  virtual void AddHalfPlane(const VistaVector3D& v3Center, const VistaVector3D& v3Normal);
  virtual void SetUseExtents(
      const VistaBoundingBox& bbExtents, const VistaQuaternion& qBoxRotation = VistaQuaternion());

  virtual bool DoUpdate(const float nMinDistance, const float nWarningLevel,
      const VistaVector3D& v3PointOnBounds, const VistaVector3D& v3UserPosition,
      const VistaQuaternion& qUserOrientation);

  virtual bool GetIsEnabled() const;
  virtual bool SetIsEnabled(const bool bSet);

  virtual bool DoTimeUpdate(
      VistaType::systemtime nTime, const float nOpacityScale, const bool bFlashState);

 private:
  class DrawCallback;
  DrawCallback*    m_pDraw;
  VistaOpenGLNode* m_pDrawNode;
  HeightMode       m_eHeightMode;
  float            m_nTapeHeight;
  float            m_nDistanceFromWall;
};

#endif //_VISTAPROXIMITYBARRIERTAPE_H
