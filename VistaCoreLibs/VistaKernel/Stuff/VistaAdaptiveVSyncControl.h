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

#ifndef _VISTAADAPTIVEVSYNCCONTROL_H
#define _VISTAADAPTIVEVSYNCCONTROL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/EventManager/VistaEventHandler.h>

#include <VistaBase/VistaColor.h>
#include <VistaBase/VistaTimer.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplayManager;
class VistaEventManager;
class VistaWindow;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Helper class that checks each frame if the framerate is lower or higher than
 * the desired target framerate. If this happens for several consecutive frames,
 * the vsync mode is switched accordingly.
 * This class automatically registers and unregisters itself in ctor/dtor
 */
class VISTAKERNELAPI VistaAdaptiveVSyncControl : public VistaEventHandler {
 public:
  VistaAdaptiveVSyncControl(VistaDisplayManager* pDisplayManager, VistaEventManager* pEventManager);
  VistaAdaptiveVSyncControl(VistaWindow* pWindow, VistaEventManager* pEventManager);
  VistaAdaptiveVSyncControl(
      const std::vector<VistaWindow*>& vecWindows, VistaEventManager* pEventManager);
  virtual ~VistaAdaptiveVSyncControl();

  virtual void HandleEvent(VistaEvent* pEvent);

  float GetSwitchAtFramerate() const;
  void  SetSwitchAtFramerate(const float& oValue);

  float GetFramerateTolerance() const;
  void  SetFramerateTolerance(const float& oValue);

  int  GetFrameCountToDisable() const;
  void SetFrameCountToDisable(const int& oValue);

  int  GetFrameCountToEnable() const;
  void SetFrameCountToEnable(const int& oValue);

  virtual void SetIsEnabled(bool bSet);

 private:
  VistaEventManager*        m_pEventManager;
  std::vector<VistaWindow*> m_vecWindows;

  float                 m_nSwitchAtFramerate;
  float                 m_nFramerateTolerance;
  int                   m_nFrameCountToDisable;
  int                   m_nFrameCountToEnable;
  VistaType::systemtime m_nLastUpdate;

  bool m_bPrintStatusOnChange;

  int m_nFramesExceedingLimit;
};

#endif //_VISTAADAPTIVEVSYNCCONTROL_H
