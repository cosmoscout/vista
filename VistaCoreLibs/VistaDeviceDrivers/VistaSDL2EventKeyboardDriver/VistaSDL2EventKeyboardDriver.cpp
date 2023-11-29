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

#include <VistaKernel/VistaSystem.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL.h>

#include <array>

VistaSDL2EventKeyboardDriverCreationMethod::VistaSDL2EventKeyboardDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  IVistaDriverCreationMethod::RegisterSensorType(
      "", sizeof(IVistaKeyboardDriver::_sKeyboardMeasure), 20, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaSDL2EventKeyboardDriverCreationMethod::CreateDriver() {
  return new VistaSDL2EventKeyboardDriver(this);
}

/**
 * Converts an SDL key to a Vista compatible key. For special keys a translation of the scancode is
 * being done, otherwise the keycode is being returned.
 */
int SDLKeyToVistaKey(int key, SDL_Keycode code) {
  switch (key) {
  case SDL_SCANCODE_ESCAPE:
    return VISTA_KEY_ESC;
  case SDL_SCANCODE_RETURN:
    return VISTA_KEY_ENTER;
  case SDL_SCANCODE_BACKSPACE:
    return VISTA_KEY_BACKSPACE;
  case SDL_SCANCODE_F1:
    return VISTA_KEY_F1;
  case SDL_SCANCODE_F2:
    return VISTA_KEY_F2;
  case SDL_SCANCODE_F3:
    return VISTA_KEY_F3;
  case SDL_SCANCODE_F4:
    return VISTA_KEY_F4;
  case SDL_SCANCODE_F5:
    return VISTA_KEY_F5;
  case SDL_SCANCODE_F6:
    return VISTA_KEY_F6;
  case SDL_SCANCODE_F7:
    return VISTA_KEY_F7;
  case SDL_SCANCODE_F8:
    return VISTA_KEY_F8;
  case SDL_SCANCODE_F9:
    return VISTA_KEY_F9;
  case SDL_SCANCODE_F10:
    return VISTA_KEY_F10;
  case SDL_SCANCODE_F11:
    return VISTA_KEY_F11;
  case SDL_SCANCODE_F12:
    return VISTA_KEY_F12;
  case SDL_SCANCODE_LEFT:
    return VISTA_KEY_LEFTARROW;
  case SDL_SCANCODE_RIGHT:
    return VISTA_KEY_RIGHTARROW;
  case SDL_SCANCODE_UP:
    return VISTA_KEY_UPARROW;
  case SDL_SCANCODE_DOWN:
    return VISTA_KEY_DOWNARROW;
  case SDL_SCANCODE_PAGEUP:
    return VISTA_KEY_PAGEUP;
  case SDL_SCANCODE_PAGEDOWN:
    return VISTA_KEY_PAGEDOWN;
  case SDL_SCANCODE_HOME:
    return VISTA_KEY_HOME;
  case SDL_SCANCODE_END:
    return VISTA_KEY_END;
  case SDL_SCANCODE_DELETE:
    return VISTA_KEY_DELETE;
  case SDL_SCANCODE_LSHIFT:
    return VISTA_KEY_SHIFT_LEFT;
  case SDL_SCANCODE_RSHIFT:
    return VISTA_KEY_SHIFT_RIGHT;
  case SDL_SCANCODE_LCTRL:
    return VISTA_KEY_CTRL_LEFT;
  case SDL_SCANCODE_RCTRL:
    return VISTA_KEY_CTRL_RIGHT;
  case SDL_SCANCODE_LALT:
    return VISTA_KEY_ALT_LEFT;
  case SDL_SCANCODE_RALT:
    return VISTA_KEY_ALT_RIGHT;
  default:
    return static_cast<int>(code);
  }
}

/**
 * Translates SDL_Keymod to a Vista compatible integer.
 */
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

VistaSDL2EventKeyboardDriver::VistaSDL2EventKeyboardDriver(IVistaDriverCreationMethod* crm)
    : IVistaKeyboardDriver(crm)
    , m_sdl2Toolkit(dynamic_cast<VistaSDL2WindowingToolkit*>(
          GetVistaSystem()->GetDisplayManager()->GetWindowingToolkit()))
    , m_lastFrameValue(false)
    , m_connected(false) {

  m_keyDownListener = m_sdl2Toolkit->RegisterEventCallback(
      SDL_KEYDOWN, [this](SDL_Event const& e) { m_keyEvents.push_back(e.key); });

  m_keyUpListener = m_sdl2Toolkit->RegisterEventCallback(
      SDL_KEYUP, [this](SDL_Event const& e) { m_keyEvents.push_back(e.key); });
}

VistaSDL2EventKeyboardDriver::~VistaSDL2EventKeyboardDriver() {
  m_sdl2Toolkit->UnregisterEventCallback(SDL_KEYUP, m_keyUpListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_KEYDOWN, m_keyDownListener);
}

bool VistaSDL2EventKeyboardDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_connected) {
    return true;
  }

  // If no new key was registered, but last frame a key was released, we want to follow that up with
  // a single 0 in the history.
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

  // Go through all collected key events and handle them accordingly,
  while (!m_keyEvents.empty()) {
    SDL_KeyboardEvent e         = m_keyEvents.front();
    int               key       = SDLKeyToVistaKey(e.keysym.scancode, e.keysym.sym);
    int               modifiers = GetVistaModifiers(static_cast<SDL_Keymod>(e.keysym.mod));

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
