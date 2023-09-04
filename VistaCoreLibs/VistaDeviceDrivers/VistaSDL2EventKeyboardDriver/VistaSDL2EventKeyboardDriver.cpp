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

#include "VistaSDL2EventKeyboardDriver.h"
#include "VistaBase/VistaStreamUtils.h"
#include "VistaKernel/VistaSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>
#include <algorithm>
#include <cstring>
#include <map>
#include <array>

#include <SDL2/SDL_keyboard.h>

VistaSDL2EventKeyboardDriverCreationMethod::VistaSDL2EventKeyboardDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType(
      "", sizeof(IVistaKeyboardDriver::_sKeyboardMeasure), 20, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaSDL2EventKeyboardDriverCreationMethod::CreateDriver() {
  return new VistaSDL2EventKeyboardDriver(this);
}

int VistaSDL2EventKeyboardDriver::SDLKeyToVistaKey(int key) {
  int result = 0;
  switch (key) {
    case SDL_SCANCODE_ESCAPE:
      result = VISTA_KEY_ESC;
      break;
    case SDL_SCANCODE_RETURN:
      result = VISTA_KEY_ENTER;
      break;
    case SDL_SCANCODE_BACKSPACE:
      result = VISTA_KEY_BACKSPACE;
      break;
    case SDL_SCANCODE_F1:
      result = VISTA_KEY_F1;
      break;
    case SDL_SCANCODE_F2:
      result = VISTA_KEY_F2;
      break;
    case SDL_SCANCODE_F3:
      result = VISTA_KEY_F3;
      break;
    case SDL_SCANCODE_F4:
      result = VISTA_KEY_F4;
      break;
    case SDL_SCANCODE_F5:
      result = VISTA_KEY_F5;
      break;
    case SDL_SCANCODE_F6:
      result = VISTA_KEY_F6;
      break;
    case SDL_SCANCODE_F7:
      result = VISTA_KEY_F7;
      break;
    case SDL_SCANCODE_F8:
      result = VISTA_KEY_F8;
      break;
    case SDL_SCANCODE_F9:
      result = VISTA_KEY_F9;
      break;
    case SDL_SCANCODE_F10:
      result = VISTA_KEY_F10;
      break;
    case SDL_SCANCODE_F11:
      result = VISTA_KEY_F11;
      break;
    case SDL_SCANCODE_F12:
      result = VISTA_KEY_F12;
      break;
    case SDL_SCANCODE_LEFT:
      result = VISTA_KEY_LEFTARROW;
      break;
    case SDL_SCANCODE_RIGHT:
      result = VISTA_KEY_RIGHTARROW;
      break;
    case SDL_SCANCODE_UP:
      result = VISTA_KEY_UPARROW;
      break;
    case SDL_SCANCODE_DOWN:
      result = VISTA_KEY_DOWNARROW;
      break;
    case SDL_SCANCODE_PAGEUP:
      result = VISTA_KEY_PAGEUP;
      break;
    case SDL_SCANCODE_PAGEDOWN:
      result = VISTA_KEY_PAGEDOWN;
      break;
    case SDL_SCANCODE_HOME:
      result = VISTA_KEY_HOME;
      break;
    case SDL_SCANCODE_END:
      result = VISTA_KEY_END;
      break;
    case SDL_SCANCODE_DELETE:
      result = VISTA_KEY_DELETE;
      break;
    case SDL_SCANCODE_LSHIFT:
      result = VISTA_KEY_SHIFT_LEFT;
      break;
    case SDL_SCANCODE_RSHIFT:
      result = VISTA_KEY_SHIFT_RIGHT;
      break;
    case SDL_SCANCODE_LCTRL:
      result = VISTA_KEY_CTRL_LEFT;
      break;
    case SDL_SCANCODE_RCTRL:
      result = VISTA_KEY_CTRL_RIGHT;
      break;
    case SDL_SCANCODE_LALT:
      result = VISTA_KEY_ALT_LEFT;
      break;
    case SDL_SCANCODE_RALT:
      result = VISTA_KEY_ALT_RIGHT;
      break;
    default:
      result = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(key));
      break;
  }

  return result;
}

int GetVistaModifiers(SDL_Keymod mod) {
  int modifiers = VISTA_KEYMOD_NONE;

  if (mod & KMOD_LCTRL || mod & KMOD_RCTRL) {
    modifiers |= VISTA_KEYMOD_CTRL;
  }
  
  if (mod & KMOD_LALT || mod & KMOD_RALT) {
    modifiers |= VISTA_KEYMOD_ALT;
  }
  
  if (mod & KMOD_LSHIFT || mod & KMOD_RSHIFT) {
    modifiers |= VISTA_KEYMOD_SHIFT;
  }

  return modifiers;
}

bool isModifier(SDL_Scancode key) {
  return key == SDL_SCANCODE_LCTRL  || key == SDL_SCANCODE_RCTRL
      || key == SDL_SCANCODE_LALT   || key == SDL_SCANCODE_RALT
      || key == SDL_SCANCODE_LSHIFT || key == SDL_SCANCODE_RSHIFT;
}

VistaSDL2EventKeyboardDriver::VistaSDL2EventKeyboardDriver(IVistaDriverCreationMethod* crm)
    : IVistaKeyboardDriver(crm)
    , m_sdl2Toolkit(dynamic_cast<VistaSDL2WindowingToolkit*>(GetVistaSystem()->GetDisplayManager()->GetWindowingToolkit()))
    , m_lastFrameValue(false)
    , m_connected(false) {

  m_keyDownListener = m_sdl2Toolkit->RegisterEventCallback(SDL_KEYDOWN, [this](SDL_Event e) {
    m_keyEvents.push_back(e.key);
  });
  
  m_keyUpListener = m_sdl2Toolkit->RegisterEventCallback(SDL_KEYUP, [this](SDL_Event e) {
    m_keyEvents.push_back(e.key);
  });
}

VistaSDL2EventKeyboardDriver::~VistaSDL2EventKeyboardDriver() {
  m_sdl2Toolkit->UnregisterEventCallback(SDL_KEYUP, m_keyUpListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_KEYDOWN, m_keyDownListener);
}


bool VistaSDL2EventKeyboardDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_connected) {
    return true;
  }

  if (m_keyEvents.empty()) {
    if (m_lastFrameValue) {
      m_lastFrameValue = false;

      MeasureStart(dTs);
      UpdateKey(0, 0);
      MeasureStop();

      return true;
    }
    return false;
  }

  while (!m_keyEvents.empty()) {
    SDL_KeyboardEvent e = m_keyEvents.front();
    int key = SDLKeyToVistaKey(e.keysym.scancode);
    int modifiers = isModifier(e.keysym.scancode) ? VISTA_KEYMOD_NONE : GetVistaModifiers(static_cast<SDL_Keymod>(e.keysym.mod));
    
    switch (e.type) {
    case SDL_KEYDOWN:
      MeasureStart(dTs);
      UpdateKey(key, modifiers);
      MeasureStop();
      break;
    
    case SDL_KEYUP:
      MeasureStart(dTs);
      UpdateKey(-key, modifiers);
      MeasureStop();
      m_lastFrameValue = true;
      break;
    }

    m_keyEvents.pop_front();
  }

  return true;
};

bool VistaSDL2EventKeyboardDriver::DoConnect() {
  m_connected = true;
  return true;
}

bool VistaSDL2EventKeyboardDriver::DoDisconnect() {
  m_connected = false;
  return true;
}

