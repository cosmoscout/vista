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

#include "VistaSDL2ControllerDriver.h"
#include "VistaBase/VistaStreamUtils.h"
#include "VistaKernel/DisplayManager/SDL2WindowImp/VistaSDL2WindowingToolkit.h"
#include "VistaSDL2ControllerState.h"
#include "VistaTools/VistaIniFileParser.h"

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <cmath>
#include <limits>
#include <stdint.h>

VistaSDL2ControllerDriver::VistaSDL2ControllerDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_sdl2Toolkit(dynamic_cast<VistaSDL2WindowingToolkit*>(GetVistaSystem()->GetDisplayManager()->GetWindowingToolkit()))
    , m_currentController(nullptr)
    , m_connected(false) {
      

  m_currentState = {
    false,
    false,
    false,
    false,

    false,
    false,
    false,
    false,

    false,
    false,

    false,
    false,

    false,
    false,
    false,
    false,

    false,
    false,
    false,
    false,

    false,

    0,
    0,

    0,
    0,

    0,
    0,
  };

  if (!m_sdl2Toolkit) {
    vstr::errp() << "[VistaSDL2ControllerDriver] Can't initialize without SDL2 windowing toolkit!" << std::endl;
    GetVistaSystem()->Quit();
  }

  SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);

  VistaDeviceSensor* sensor = new VistaDeviceSensor();
  AddDeviceSensor(sensor);
  sensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder());

  VistaIniFileParser parser(true);
  VistaPropertyList interactionProperties;
  parser.ReadProplistFromFile(GetVistaSystem()->GetInteractionIniFile(), interactionProperties, true);

  std::string dbFile;
  bool hasDBFile = interactionProperties.GetValueInSubList("CONTROLLER_DB", "SDL2CONTROLLER", dbFile);
  if (hasDBFile) {
    int result = SDL_GameControllerAddMappingsFromFile(dbFile.c_str());
    if (result == -1) {
      vstr::warnp() << "[VistaSDL2ControllerDriver] Error while loading controller database: " << SDL_GetError() << std::endl;
    }
  }

  m_addControllerListener = m_sdl2Toolkit->RegisterEventCallback(SDL_CONTROLLERDEVICEADDED, [this] (SDL_Event e) {
    if (!m_currentController) {
      m_currentController = SDL_GameControllerOpen(e.cdevice.which);
      vstr::outi() << "[VistaSDL2ControllerDriver] Controller connected: " << SDL_GameControllerName(m_currentController) << std::endl;
    }
  });

  m_removeControllerListener = m_sdl2Toolkit->RegisterEventCallback(SDL_CONTROLLERDEVICEREMOVED, [this] (SDL_Event e) {
    if (m_currentController == SDL_GameControllerFromInstanceID(e.cdevice.which)) {
      SDL_GameControllerClose(m_currentController);
      m_currentController = nullptr;
    }
  });

  m_buttonDownListener = m_sdl2Toolkit->RegisterEventCallback(SDL_CONTROLLERBUTTONDOWN, [this] (SDL_Event e) {
    if (m_currentController == SDL_GameControllerFromInstanceID(e.cbutton.which)) {
      m_buttonEvents.push_back(e.cbutton);
    }
  });

  m_buttonUpListener = m_sdl2Toolkit->RegisterEventCallback(SDL_CONTROLLERBUTTONUP, [this] (SDL_Event e) {
    if (m_currentController == SDL_GameControllerFromInstanceID(e.cbutton.which)) {
      m_buttonEvents.push_back(e.cbutton);
    }
  });

  m_axisListener = m_sdl2Toolkit->RegisterEventCallback(SDL_CONTROLLERAXISMOTION, [this] (SDL_Event e) {
    if (m_currentController == SDL_GameControllerFromInstanceID(e.caxis.which)) {
      m_axisEvents.push_back(e.caxis);
    }
  });
}

VistaSDL2ControllerDriver::~VistaSDL2ControllerDriver() {
  if (m_currentController) {
    SDL_GameControllerClose(m_currentController);
  }

  m_sdl2Toolkit->UnregisterEventCallback(SDL_CONTROLLERAXISMOTION, m_axisListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_CONTROLLERBUTTONUP, m_buttonUpListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_CONTROLLERBUTTONDOWN, m_buttonDownListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_CONTROLLERDEVICEREMOVED, m_removeControllerListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_CONTROLLERDEVICEADDED, m_addControllerListener);

  VistaDeviceSensor* sensor = GetSensorByIndex(0);
  RemDeviceSensor(sensor);
  delete sensor;
}

bool VistaSDL2ControllerDriver::DoConnect() {
  m_connected = true;
  return true;
}

bool VistaSDL2ControllerDriver::DoDisconnect() {
  m_connected = false;
  return true;
}

bool VistaSDL2ControllerDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_connected || !m_currentController) {
    return true;
  }

  VistaSensorMeasure* measure = MeasureStart(0, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (!measure) {
    return false;
  }

  auto state = measure->getWrite<VistaSDL2ControllerState>();

  while (!m_buttonEvents.empty()) {
    auto& e = m_buttonEvents.front();
    bool pressed = e.state == SDL_PRESSED;

    switch (e.button) {
      case SDL_CONTROLLER_BUTTON_A:
        m_currentState.aPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_B:
        m_currentState.bPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_X:
        m_currentState.xPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_Y:
        m_currentState.yPressed = pressed;
        break;

      case SDL_CONTROLLER_BUTTON_DPAD_UP:
        m_currentState.dpadUpPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        m_currentState.dpadDownPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        m_currentState.dpadLeftPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        m_currentState.dpadRightPressed = pressed;
        break;

      case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        m_currentState.stickLeftPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        m_currentState.stickRightPressed = pressed;
        break;
        
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        m_currentState.shoulderLeftPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        m_currentState.shoulderRightPressed = pressed;
        break;

      case SDL_CONTROLLER_BUTTON_BACK:
        m_currentState.backPressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_GUIDE:
        m_currentState.guidePressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_START:
        m_currentState.startPressed = pressed;
        break;

      case SDL_CONTROLLER_BUTTON_MISC1:
        m_currentState.misc1Pressed = pressed;
        break;

      case SDL_CONTROLLER_BUTTON_PADDLE1:
        m_currentState.paddle1Pressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_PADDLE2:
        m_currentState.paddle2Pressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_PADDLE3:
        m_currentState.paddle3Pressed = pressed;
        break;
      case SDL_CONTROLLER_BUTTON_PADDLE4:
        m_currentState.paddle4Pressed = pressed;
        break;
        
      case SDL_CONTROLLER_BUTTON_TOUCHPAD:
        m_currentState.touchpadPressed = pressed;
        break;
    }

    m_buttonEvents.pop_front();
  }

  while (!m_axisEvents.empty()) {
    SDL_ControllerAxisEvent e = m_axisEvents.front();
    float value = static_cast<double>(e.value) / static_cast<double>(std::numeric_limits<int16_t>::max());
    switch (e.axis) {
      case SDL_CONTROLLER_AXIS_LEFTX:
        m_currentState.stickLeftX = value;
        break;
      case SDL_CONTROLLER_AXIS_LEFTY:
        m_currentState.stickLeftY = value;
        break;
        
      case SDL_CONTROLLER_AXIS_RIGHTX:
        m_currentState.stickRightX = value;
        break;
      case SDL_CONTROLLER_AXIS_RIGHTY:
        m_currentState.stickRightY = value;
        break;
        
      case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
        m_currentState.triggerLeft = value;
        break;
      case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
        m_currentState.triggerRight = value;
        break;
    }

    m_axisEvents.pop_front();
  }

  *state = m_currentState;

  MeasureStop(0);
  return true;
}
