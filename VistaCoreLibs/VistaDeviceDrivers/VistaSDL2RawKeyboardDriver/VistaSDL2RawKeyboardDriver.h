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

#ifndef _VISTASDL2RAWKEYBOARDDRIVER_H
#define _VISTASDL2RAWKEYBOARDDRIVER_H

#include <VistaDeviceDriversBase/Drivers/VistaKeyboardDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <map>
#include <vector>
#include <SDL2/SDL_scancode.h>

// Windows DLL build
#if defined(WIN32) && !defined(VISTASDL2RAWKEYBOARDDRIVER_STATIC)
#ifdef VISTASDL2RAWKEYBOARDDRIVER_EXPORTS
#define VISTASDL2RAWKEYBOARDDRIVERAPI __declspec(dllexport)
#else
#define VISTASDL2RAWKEYBOARDDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2RAWKEYBOARDDRIVERAPI
#endif

class VISTASDL2RAWKEYBOARDDRIVERAPI VistaSDL2RawKeyboardDriver : public IVistaKeyboardDriver {
 public:
  VistaSDL2RawKeyboardDriver(IVistaDriverCreationMethod*);
  virtual ~VistaSDL2RawKeyboardDriver();

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

class VISTASDL2RAWKEYBOARDDRIVERAPI VistaSDL2RawKeyboardDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  VistaSDL2RawKeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();
};

#endif // _VISTASDL2RAWKEYBOARDDRIVER_H