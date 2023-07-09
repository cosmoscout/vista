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

#ifndef _VISTASDL2KEYBOARDDRIVER_H
#define _VISTASDL2KEYBOARDDRIVER_H

#include "VistaDeviceDrivers/VistaOSGKeyboardDriver/VistaOSGKeyboardDriver.h"
#include <VistaDeviceDriversBase/Drivers/VistaKeyboardDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <map>
#include <vector>
#include <SDL2/SDL_scancode.h>

// Windows DLL build
#if defined(WIN32) && !defined(VISTASDL2KEYBOARDDRIVER_STATIC)
#ifdef VISTASDL2KEYBOARDDRIVER_EXPORTS
#define VISTASDL2KEYBOARDDRIVERAPI __declspec(dllexport)
#else
#define VISTASDL2KEYBOARDDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2KEYBOARDDRIVERAPI
#endif

class VISTASDL2KEYBOARDDRIVERAPI VistaSDL2KeyboardDriver : public IVistaKeyboardDriver {
 public:
  VistaSDL2KeyboardDriver(IVistaDriverCreationMethod*);
  virtual ~VistaSDL2KeyboardDriver();

 protected:
  bool DoSensorUpdate(VistaType::microtime dTs) final;

  bool DoConnect() final;
  bool DoDisconnect() final;

 private:
  int SDLKeyToVistaKey(int key);

  std::vector<Uint8> m_currentKeyboardState;
  std::vector<Uint8> m_lastKeyboardState;
  bool               m_lastFrameValue;
  bool               m_connected;
};

class VISTASDL2KEYBOARDDRIVERAPI VistaSDL2KeyboardDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  VistaSDL2KeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();
};

#endif // _VISTASDL2KEYBOARDDRIVER_H
