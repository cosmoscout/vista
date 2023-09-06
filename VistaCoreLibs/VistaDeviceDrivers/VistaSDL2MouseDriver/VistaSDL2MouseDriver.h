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

#ifndef _VISTASDL2MOUSEDRIVER_H
#define _VISTASDL2MOUSEDRIVER_H

#include "VistaDeviceDriversBase/VistaDeviceSensor.h"
#include "VistaKernel/DisplayManager/SDL2WindowImp/VistaSDL2WindowingToolkit.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <VistaDeviceDriversBase/Drivers/VistaMouseDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <map>
#include <vector>

// Windows DLL build
#if defined(WIN32) && !defined(VISTASDL2MOUSEDRIVER_STATIC)
#ifdef VISTASDL2MOUSEDRIVER_EXPORTS
#define VISTASDL2MOUSEDRIVERAPI __declspec(dllexport)
#else
#define VISTASDL2MOUSEDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2MOUSEDRIVERAPI
#endif

class VISTASDL2MOUSEDRIVERAPI VistaSDL2MouseDriver : public IVistaMouseDriver {
 public:
  explicit VistaSDL2MouseDriver(IVistaDriverCreationMethod*);
  ~VistaSDL2MouseDriver() override;

 protected:
  bool DoSensorUpdate(VistaType::microtime dTs) final;

  bool DoConnect() final;
  bool DoDisconnect() final;

 private:
  VistaDeviceSensor*         m_mouseSensor;
  VistaSDL2WindowingToolkit* m_sdl2Toolkit;

  /** X-Position of the mouse pointer inside the window. */
  int m_x;

  /** Y-Position of the mouse pointer inside the window. */
  int m_y;

  /** If the left mouse button is pressed or released. Either SDL_PRESSED or SDL_RELEASED */
  Uint8 m_lmb;

  /** If the middle mouse button is pressed or released. Either SDL_PRESSED or SDL_RELEASED */
  Uint8 m_mmb;

  /** If the right mouse button is pressed or released. Either SDL_PRESSED or SDL_RELEASED */
  Uint8 m_rmb;

  /** The last change in wheel position. */
  double m_wheel;

  /** The accumulated change in wheel position. */
  double m_wheelState;

  std::deque<SDL_MouseMotionEvent> m_motionEvents;
  std::deque<SDL_MouseButtonEvent> m_buttonEvents;
  std::deque<SDL_MouseWheelEvent>  m_wheelEvents;

  size_t m_motionEventListener;
  size_t m_buttonDownEventListener;
  size_t m_buttonUpEventListener;
  size_t m_wheelEventListener;

  bool m_connected;
};

class VISTASDL2MOUSEDRIVERAPI VistaSDL2MouseDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  explicit VistaSDL2MouseDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  IVistaDeviceDriver* CreateDriver() override;
};

#endif // _VISTASDL2MOUSEDRIVER_H
