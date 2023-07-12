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

#ifndef _VISTAOPENSGGRAPHICSSTATSOVERLAY_H
#define _VISTAOPENSGGRAPHICSSTATSOVERLAY_H

#include "VistaKernelOpenSGExtConfig.h"

#include "VistaBase/VistaBaseTypes.h"
#include "VistaBase/VistaColor.h"
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <vector>

namespace osg {
class StatCollector;
}
class VistaDisplayManager;
class VistaSimpleTextOverlay;
class VistaEventManager;
class VistaViewport;

class VISTAKERNELOPENSGEXTAPI VistaOpenSGGraphicsStatsOverlay : public VistaEventHandler {
 public:
  VistaOpenSGGraphicsStatsOverlay(VistaDisplayManager* pManager, VistaEventManager* pEventManager,
      const bool bRegisterAllViewports = true);
  ~VistaOpenSGGraphicsStatsOverlay();

  void         SetIsEnabled(bool bSet);
  virtual void HandleEvent(VistaEvent* pEvent);

  VistaColor GetTextColor() const;
  void       SetTextColor(const VistaColor& colText);

  int  GetTextSize() const;
  void SetTextSize(const int nTextSize);

  VistaType::microtime GetUpdateInterval() const;
  void                 SetUpdateIntercal(const VistaType::microtime nInterval);

  virtual void AddViewport(VistaViewport* pViewport);

 private:
  struct Line;
  VistaDisplayManager*                 m_pDisplayManager;
  VistaEventManager*                   m_pEventManager;
  std::vector<VistaSimpleTextOverlay*> m_vecTextOverlays;
  osg::StatCollector*                  m_pStatistics;
  std::vector<Line*>                   m_vecLines;
  bool                                 m_bEnabled;
  VistaType::microtime                 m_nUpdateCycle;
  VistaType::systemtime                m_nLastUpdate;
  VistaColor                           m_oColor;
  int                                  m_nTextSize;
};

#endif // _VISTAOPENSGGRAPHICSSTATSOVERLAY_H
