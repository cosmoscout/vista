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

#include "VistaSDL2RawKeyboardDriver.h"
#include "VistaBase/VistaStreamUtils.h"
#include "VistaKernel/VistaSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <cstring>
#include <map>
#include <array>

#include <SDL2/SDL_keyboard.h>

VistaSDL2RawKeyboardDriverCreationMethod::VistaSDL2RawKeyboardDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType(
      "", sizeof(IVistaKeyboardDriver::_sKeyboardMeasure), 20, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaSDL2RawKeyboardDriverCreationMethod::CreateDriver() {
  return new VistaSDL2RawKeyboardDriver(this);
}

int VistaSDL2RawKeyboardDriver::SDLKeyToVistaKey(int key) {
  Uint8 curr = m_currentKeyboardState[key];
  Uint8 prev = m_lastKeyboardState[key];

  // If the key is inactive last and current frame we don't need to update it.
  if (curr == 0 && prev == 0) {
    return 0;
  }

  // If the key is currently pressed or not.
  int upModifier = curr ? 1 : -1;

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

  return upModifier * result;
}

int GetVistaModifiers(const Uint8* keyboard) {
  int modifiers = VISTA_KEYMOD_NONE;

  if (keyboard[SDL_SCANCODE_LCTRL] || keyboard[SDL_SCANCODE_RCTRL]) {
    modifiers |= VISTA_KEYMOD_CTRL;
  }

  if (keyboard[SDL_SCANCODE_LALT] || keyboard[SDL_SCANCODE_RALT]) {
    modifiers |= VISTA_KEYMOD_ALT;
  }

  if (keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT]) {
    modifiers |= VISTA_KEYMOD_SHIFT;
  }

  return modifiers;
}

constexpr std::array<SDL_Scancode, 6> MODIFIER_KEYS = {{SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL,
    SDL_SCANCODE_LALT, SDL_SCANCODE_RALT, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT}};

bool isModifier(SDL_Scancode key) {
  return key == SDL_SCANCODE_LCTRL || key == SDL_SCANCODE_RCTRL || key == SDL_SCANCODE_LALT ||
         key == SDL_SCANCODE_RALT || key == SDL_SCANCODE_LSHIFT || key == SDL_SCANCODE_RSHIFT;
}

VistaSDL2RawKeyboardDriver::VistaSDL2RawKeyboardDriver(IVistaDriverCreationMethod* crm)
    : IVistaKeyboardDriver(crm)
    , m_lastFrameValue(false)
    , m_connected(false) {
  int          keyboardSize = 0;
  const Uint8* keyboard     = SDL_GetKeyboardState(&keyboardSize);

  m_currentKeyboardState.resize(keyboardSize);
  std::memcpy(m_currentKeyboardState.data(), keyboard, keyboardSize);

  m_lastKeyboardState.resize(keyboardSize);
  std::memcpy(m_lastKeyboardState.data(), keyboard, keyboardSize);
}

VistaSDL2RawKeyboardDriver::~VistaSDL2RawKeyboardDriver() = default;

bool VistaSDL2RawKeyboardDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_connected) {
    return true;
  }

  int          keyboardSize = 0;
  const Uint8* keyboard     = SDL_GetKeyboardState(&keyboardSize);

  m_currentKeyboardState.resize(keyboardSize);
  std::memcpy(m_currentKeyboardState.data(), keyboard, keyboardSize);

  // Get the current state of modifier keys.
  int modifiers = GetVistaModifiers(keyboard);

  // First we update all keys that
  // - Are no modifiers
  // - Are pressed down
  // - Have been released since last frame
  bool updated = false;
  for (int i = 0; i < keyboardSize; ++i) {
    auto code = static_cast<SDL_Scancode>(i);
    int  key  = SDLKeyToVistaKey(code);

    if (key && !isModifier(code)) {
      MeasureStart(dTs);
      UpdateKey(key, modifiers);
      MeasureStop();
      updated = true;
    }
  }

  // Now, if no "normal" keys have been updated, we will consider the modifiers as keys to press.
  if (!updated) {
    for (SDL_Scancode code : MODIFIER_KEYS) {
      int key = SDLKeyToVistaKey(code);
      if (key) {
        MeasureStart(dTs);
        UpdateKey(key, VISTA_KEYMOD_NONE);
        MeasureStop();
        updated = true;
      }
    }
  }

  m_lastKeyboardState.swap(m_currentKeyboardState);

  // If this time nothing got updated, but last time we add a 0 to the history.
  if (!updated && m_lastFrameValue) {
    MeasureStart(dTs);
    UpdateKey(0, 0);
    MeasureStop();
    m_lastFrameValue = false;
  } else if (!updated) {
    m_lastFrameValue = false;
    return false;
  }

  if (updated) {
    m_lastFrameValue = true;
  }

  return true;
};

bool VistaSDL2RawKeyboardDriver::DoConnect() {
  m_connected = true;
  return true;
}

bool VistaSDL2RawKeyboardDriver::DoDisconnect() {
  m_connected = false;
  return true;
}
