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

#ifndef _VISTAPROXIMITYFADEOUT_H
#define _VISTAPROXIMITYFADEOUT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaProximityWarningBase.h"

#include <VistaBase/VistaColor.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaColor;
class VistaViewport;
class VistaDisplayManager;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaProximityFadeout : public IVistaProximityWarningBase {
 public:
  VistaProximityFadeout(VistaEventManager* pManager, const float nBeginWarningDistance,
      const float nMaxWarningDistance);
  virtual ~VistaProximityFadeout();

  void AddViewport(VistaViewport* pViewport);
  void AddAllViewports(VistaDisplayManager* pDisplayManager);

  VistaColor GetFadeoutColor() const;
  void       SetFadeoutColor(const VistaColor& oColor);

  virtual bool DoUpdate(const float nMinDistance, const float nWarningLevel,
      const VistaVector3D& v3PointOnBounds, const VistaVector3D& v3UserPosition,
      const VistaQuaternion& qUserOrientation);

  virtual bool GetIsEnabled() const;
  virtual bool SetIsEnabled(const bool bSet);

  virtual bool DoTimeUpdate(
      VistaType::systemtime nTime, const float nOpacityScale, const bool bFlashState);

 private:
  VistaColor m_oFadeoutColor;
  class FadeoutOverlay;
  std::vector<FadeoutOverlay*> m_vecOverlays;
};

#endif //_VISTAPROXIMITYFADEOUT_H
