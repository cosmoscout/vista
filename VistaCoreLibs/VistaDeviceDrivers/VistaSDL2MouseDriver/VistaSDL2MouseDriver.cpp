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

#include "VistaSDL2MouseDriver.h"
#include "VistaBase/VistaStreamUtils.h"
#include "VistaKernel/DisplayManager/SDL2WindowImp/VistaSDL2WindowingToolkit.h"
#include "VistaKernel/VistaSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_events.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>
#include <algorithm>
#include <cstring>
#include <map>
#include <array>
#include <memory>

VistaSDL2MouseDriverCreationMethod::VistaSDL2MouseDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType(
      "", sizeof(IVistaMouseDriver::_sMouseMeasure), 20, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaSDL2MouseDriverCreationMethod::CreateDriver() {
  return new VistaSDL2MouseDriver(this);
}

VistaSDL2MouseDriver::VistaSDL2MouseDriver(IVistaDriverCreationMethod* crm)
    : IVistaMouseDriver(crm)
    , m_mouseSensor(new VistaDeviceSensor)
    , m_sdl2Toolkit(dynamic_cast<VistaSDL2WindowingToolkit*>(GetVistaSystem()->GetDisplayManager()->GetWindowingToolkit()))
    , m_x(0)
    , m_y(0)
    , m_lmb(0)
    , m_mmb(0)
    , m_rmb(0)
    , m_wheel(0)
    , m_connected(false) {

  if (!m_sdl2Toolkit) {
    vstr::errp() << "[VistaSDL2MouseDriver] Can't initialize without SDL2 windowing toolkit!" << std::endl;
    GetVistaSystem()->Quit();
  }

  m_motionEventListener = m_sdl2Toolkit->RegisterEventCallback(SDL_MOUSEMOTION, [this] (SDL_Event event) {
    m_motionEvents.push_back(event.motion);
  });
  
  m_buttonDownEventListener = m_sdl2Toolkit->RegisterEventCallback(SDL_MOUSEBUTTONDOWN, [this] (SDL_Event event) {
    m_buttonEvents.push_back(event.button);
  });
  
  m_buttonUpEventListener = m_sdl2Toolkit->RegisterEventCallback(SDL_MOUSEBUTTONUP, [this] (SDL_Event event) {
    m_buttonEvents.push_back(event.button);
  });
  
  m_wheelEventListener = m_sdl2Toolkit->RegisterEventCallback(SDL_MOUSEWHEEL, [this] (SDL_Event event) {
    m_wheelEvents.push_back(event.wheel);
  });

  AddDeviceSensor(m_mouseSensor, 0);
}

VistaSDL2MouseDriver::~VistaSDL2MouseDriver() {
  RemDeviceSensor(m_mouseSensor);

  m_sdl2Toolkit->UnregisterEventCallback(SDL_MOUSEMOTION, m_motionEventListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_MOUSEBUTTONDOWN, m_buttonDownEventListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_MOUSEBUTTONUP, m_buttonUpEventListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_MOUSEWHEEL, m_wheelEventListener);
  
  delete m_mouseSensor;
}

bool VistaSDL2MouseDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_connected) {
    return true;
  }

  // Just get the latest position and discard all others.
  if (!m_motionEvents.empty()) {
    SDL_MouseMotionEvent e = m_motionEvents.back();
    m_x = e.x;
    m_y = e.y;
    m_motionEvents.clear();
  }

  while (!m_buttonEvents.empty()) {
    SDL_MouseButtonEvent e = m_buttonEvents.front();
    
    switch (e.button) {
      case SDL_BUTTON_LEFT:
        m_lmb = e.state;
        break;
      case SDL_BUTTON_MIDDLE:
        m_mmb = e.state;
        break;
      case SDL_BUTTON_RIGHT:
        m_rmb = e.state;
        break;
    }

    m_buttonEvents.pop_front();
  }

  // Just get the latest wheel event and discard all others.
  if (!m_wheelEvents.empty()) {
    SDL_MouseWheelEvent e = m_wheelEvents.back();
    m_wheel = e.preciseY;
    m_wheelEvents.clear();
  } else {
    m_wheel = 0;
  }


  MeasureStart(0, dTs);
  UpdateMousePosition(0, m_x, m_y);
  UpdateMouseButton(0, IVistaMouseDriver::BT_LEFT, m_lmb);
  UpdateMouseButton(0, IVistaMouseDriver::BT_MIDDLE, m_mmb);
  UpdateMouseButton(0, IVistaMouseDriver::BT_RIGHT, m_rmb);
  UpdateMouseButton(0, IVistaMouseDriver::BT_WHEEL_DIR, m_wheel);
  MeasureStop(0);

  return true;
};

bool VistaSDL2MouseDriver::DoConnect() {
  m_connected = true;
  return true;
}

bool VistaSDL2MouseDriver::DoDisconnect() {
  m_connected = false;
  return true;
}

