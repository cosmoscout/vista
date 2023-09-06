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

#ifndef _VISTASDL2EVENTKEYBOARDDRIVER_H
#define _VISTASDL2EVENTKEYBOARDDRIVER_H

#include <VistaDeviceDriversBase/Drivers/VistaKeyboardDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <map>
#include <vector>
#include <SDL2/SDL_scancode.h>
#include <VistaKernel/DisplayManager/SDL2WindowImp/VistaSDL2WindowingToolkit.h>

// Windows DLL build
#if defined(WIN32) && !defined(VISTASDL2EVENTKEYBOARDDRIVER_STATIC)
#ifdef VISTASDL2EVENTKEYBOARDDRIVER_EXPORTS
#define VISTASDL2EVENTKEYBOARDDRIVERAPI __declspec(dllexport)
#else
#define VISTASDL2EVENTKEYBOARDDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2EVENTKEYBOARDDRIVERAPI
#endif

class VISTASDL2EVENTKEYBOARDDRIVERAPI VistaSDL2EventKeyboardDriver : public IVistaKeyboardDriver {
 public:
  explicit VistaSDL2EventKeyboardDriver(IVistaDriverCreationMethod*);
  ~VistaSDL2EventKeyboardDriver() override;

 protected:
  bool DoSensorUpdate(VistaType::microtime dTs) final;

  bool DoConnect() final;
  bool DoDisconnect() final;

 private:
  VistaSDL2WindowingToolkit* m_sdl2Toolkit;

  std::deque<SDL_KeyboardEvent> m_keyEvents;

  size_t m_keyDownListener;
  size_t m_keyUpListener;

  bool m_lastFrameValue;

  bool m_connected;
};

class VISTASDL2EVENTKEYBOARDDRIVERAPI VistaSDL2EventKeyboardDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  explicit VistaSDL2EventKeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  IVistaDeviceDriver* CreateDriver() override;
};

#endif // _VISTASDL2EVENTKEYBOARDDRIVER_H
