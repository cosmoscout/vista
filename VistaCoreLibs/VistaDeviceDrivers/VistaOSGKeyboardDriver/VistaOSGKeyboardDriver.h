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

#ifndef _VISTAOSGKEYBOARDDRIVER_H
#define _VISTAOSGKEYBOARDDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/Drivers/VistaKeyboardDriver.h>
#include <map>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAOSGKEYBOARDDRIVER_STATIC)
#ifdef VISTAOSGKEYBOARDDRIVER_EXPORTS
#define VISTAOSGKEYBOARDDRIVERAPI __declspec(dllexport)
#else
#define VISTAOSGKEYBOARDDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAOSGKEYBOARDDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverAbstractWindowAspect;
class VistaWindow;
class VistaOSGWindowingToolkit;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAOSGKEYBOARDDRIVERAPI VistaOSGKeyboardDriver : public IVistaKeyboardDriver {
 public:
  VistaOSGKeyboardDriver(IVistaDriverCreationMethod* pCreation);
  virtual ~VistaOSGKeyboardDriver();

  bool RegisterWindow(VistaWindow* pWindow);
  bool UnregisterWindow(VistaWindow* pWindow);

 protected:
  virtual bool DoConnect();
  virtual bool DoDisconnect();

  virtual bool DoSensorUpdate(VistaType::microtime dTs);

 private:
  typedef std::vector<VistaWindow*> WindowList;
  WindowList                        m_vecWindows;
  VistaDriverAbstractWindowAspect*  m_pWindowAspect;
  VistaOSGWindowingToolkit*         m_pWindowingToolkit;
};

class VISTAOSGKEYBOARDDRIVERAPI VistaOSGKeyboardDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  VistaOSGKeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();

 protected:
 private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGOSGKEYBOARDDRIVER_H
