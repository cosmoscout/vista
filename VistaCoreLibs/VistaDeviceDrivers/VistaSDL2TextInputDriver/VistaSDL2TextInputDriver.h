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

#ifndef _VISTASDL2TEXTINPUTDRIVER_H
#define _VISTASDL2TEXTINPUTDRIVER_H

#include <VistaDeviceDriversBase/Drivers/VistaKeyboardDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <map>
#include <vector>
#include <VistaKernel/DisplayManager/SDL2WindowImp/VistaSDL2WindowingToolkit.h>

// Windows DLL build
#if defined(WIN32) && !defined(VISTASDL2TEXTINPUTDRIVER_STATIC)
#ifdef VISTASDL2TEXTINPUTDRIVER_EXPORTS
#define VISTASDL2TEXTINPUTDRIVERAPI __declspec(dllexport)
#else
#define VISTASDL2TEXTINPUTDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2TEXTINPUTDRIVERAPI
#endif

/**
 * The TextInputDriver is the most complex keyboard driver. It is the only driver, to report capital
 * letters and composite characters. To also support navigation it uses a combination of
 * - SDL_TextInputEvents: For handling character input.
 * - SDL_KeyboardEvents: For handling all events except character input.
 * - SDL_GetKeyboardState: For handling modifiers.
 *
 * See:
 * - https://wiki.libsdl.org/SDL2/SDL_TextInputEvent
 * - https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
 * - https://wiki.libsdl.org/SDL2/SDL_GetKeyboardState,
 */
class VISTASDL2TEXTINPUTDRIVERAPI VistaSDL2TextInputDriver : public IVistaKeyboardDriver {
 public:
  explicit VistaSDL2TextInputDriver(IVistaDriverCreationMethod*);
  ~VistaSDL2TextInputDriver() override;

 protected:
  bool DoSensorUpdate(VistaType::microtime dTs) final;

  bool DoConnect() final;
  bool DoDisconnect() final;

 private:
  VistaSDL2WindowingToolkit* m_sdl2Toolkit;

  std::deque<SDL_TextInputEvent> m_textEvents;
  std::deque<SDL_KeyboardEvent>  m_keyEvents;

  size_t m_keyTextListener;
  size_t m_keyDownListener;
  size_t m_keyUpListener;

  bool m_lastFrameValue;

  bool m_connected;
};

class VISTASDL2TEXTINPUTDRIVERAPI VistaSDL2TextInputDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  explicit VistaSDL2TextInputDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  IVistaDeviceDriver* CreateDriver() override;
};

#endif // _VISTASDL2TEXTINPUTDRIVER_H
