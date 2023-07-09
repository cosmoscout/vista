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
#include <VistaDeviceDriversBase/Drivers/VistaMouseDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <map>
#include <vector>
#include <SDL2/SDL_scancode.h>

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
  VistaSDL2MouseDriver(IVistaDriverCreationMethod*);
  virtual ~VistaSDL2MouseDriver();

 protected:
  bool DoSensorUpdate(VistaType::microtime dTs) final;

  bool DoConnect() final;
  bool DoDisconnect() final;

 private:
  VistaDeviceSensor* m_mouseSensor;

  Uint8 m_currentMouseState;
  Uint8 m_lastMouseState;
  bool  m_lastFrameValue;
  bool  m_connected;
};

class VISTASDL2MOUSEDRIVERAPI VistaSDL2MouseDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  VistaSDL2MouseDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();
};

#endif // _VISTASDL2MOUSEDRIVER_H
