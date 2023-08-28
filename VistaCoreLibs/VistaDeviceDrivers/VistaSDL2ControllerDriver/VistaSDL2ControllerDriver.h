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

#ifndef __VISTASDL2CONTROLLER_H
#define __VISTASDL2CONTROLLER_H

#include "VistaDeviceDrivers/VistaSDL2ControllerDriver/VistaSDL2ControllerState.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/DisplayManager/SDL2WindowImp/VistaSDL2WindowingToolkit.h>

// Windows DLL build
#if defined(WIN32) && !defined(VISTASDL2CONTROLLERDRIVER_STATIC)
#ifdef VISTASDL2CONTROLLERDRIVER_EXPORTS
#define VISTASDL2CONTROLLERAPI __declspec(dllexport)
#else
#define VISTASDL2CONTROLLERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2CONTROLLERAPI
#endif


class VISTASDL2CONTROLLERAPI VistaSDL2ControllerDriver final : public IVistaDeviceDriver {

 public:
  VistaSDL2ControllerDriver(IVistaDriverCreationMethod* crm);
  ~VistaSDL2ControllerDriver();

 protected:
  bool DoSensorUpdate(VistaType::microtime dTs);

  bool DoConnect();
  bool DoDisconnect();

 private:
  VistaSDL2WindowingToolkit* m_sdl2Toolkit;
  
  SDL_GameController* m_currentController;
  
  std::deque<SDL_ControllerButtonEvent> m_buttonEvents;
  std::deque<SDL_ControllerAxisEvent>   m_axisEvents;
  std::deque<SDL_ControllerSensorEvent> m_sensorEvents;

  size_t m_addControllerListener;
  size_t m_removeControllerListener;
  size_t m_buttonDownListener;
  size_t m_buttonUpListener;
  size_t m_axisListener;
  size_t m_sensorListener;

  VistaSDL2ControllerState m_currentState;

  bool  m_connected;
};

class VISTASDL2CONTROLLERAPI VistaSDL2ControllerCreationMethod : public IVistaDriverCreationMethod {
 public:
  VistaSDL2ControllerCreationMethod(IVistaTranscoderFactoryFactory* fac)
      : IVistaDriverCreationMethod(fac) {
    RegisterSensorType("", sizeof(VistaSDL2ControllerState), 120, fac->CreateFactoryForType(""));
  }

  ~VistaSDL2ControllerCreationMethod() {
    UnregisterType("", false);
  }

  virtual IVistaDeviceDriver* CreateDriver() {
    return new VistaSDL2ControllerDriver(this);
  }
};

#endif
