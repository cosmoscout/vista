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

#include "VistaSDL2TextInputDriver.h"
#include "VistaKernel/VistaSystem.h"
#include <SDL2/SDL.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <array>

VistaSDL2TextInputDriverCreationMethod::VistaSDL2TextInputDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType(
      "", sizeof(IVistaKeyboardDriver::_sKeyboardMeasure), 20, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaSDL2TextInputDriverCreationMethod::CreateDriver() {
  return new VistaSDL2TextInputDriver(this);
}

/**
 * Converts an SDL special key to a Vista compatible key. For special keys a translation of the
 * scancode is being done, otherwise a 0 being returned.
 */
int32_t SDLKeyToVistaKey(uint8_t key) {
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
    return static_cast<int32_t>(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(key)));
  }
}

bool IsSpecialKey(uint8_t key) {
  switch (key) {
  case SDL_SCANCODE_ESCAPE:
  case SDL_SCANCODE_RETURN:
  case SDL_SCANCODE_BACKSPACE:
  case SDL_SCANCODE_F1:
  case SDL_SCANCODE_F2:
  case SDL_SCANCODE_F3:
  case SDL_SCANCODE_F4:
  case SDL_SCANCODE_F5:
  case SDL_SCANCODE_F6:
  case SDL_SCANCODE_F7:
  case SDL_SCANCODE_F8:
  case SDL_SCANCODE_F9:
  case SDL_SCANCODE_F10:
  case SDL_SCANCODE_F11:
  case SDL_SCANCODE_F12:
  case SDL_SCANCODE_LEFT:
  case SDL_SCANCODE_RIGHT:
  case SDL_SCANCODE_UP:
  case SDL_SCANCODE_DOWN:
  case SDL_SCANCODE_PAGEUP:
  case SDL_SCANCODE_PAGEDOWN:
  case SDL_SCANCODE_HOME:
  case SDL_SCANCODE_END:
  case SDL_SCANCODE_DELETE:
  case SDL_SCANCODE_LSHIFT:
  case SDL_SCANCODE_RSHIFT:
  case SDL_SCANCODE_LCTRL:
  case SDL_SCANCODE_RCTRL:
  case SDL_SCANCODE_LALT:
  case SDL_SCANCODE_RALT:
    return true;
  }

  return false;
}

/**
 * Checks the keyboard for pressed modifiers and returns a Vista compatible integer.
 */
int32_t GetVistaModifiers(const Uint8* keyboard) {
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

VistaSDL2TextInputDriver::VistaSDL2TextInputDriver(IVistaDriverCreationMethod* crm)
    : IVistaKeyboardDriver(crm)
    , m_sdl2Toolkit(dynamic_cast<VistaSDL2WindowingToolkit*>(
          GetVistaSystem()->GetDisplayManager()->GetWindowingToolkit()))
    , m_lastFrameValue(false)
    , m_connected(false) {

  m_keyTextListener = m_sdl2Toolkit->RegisterEventCallback(
      SDL_TEXTINPUT, [this](SDL_Event e) { m_textEvents.push_back(e.text); });

  m_keyDownListener = m_sdl2Toolkit->RegisterEventCallback(
      SDL_KEYDOWN, [this](SDL_Event e) { m_keyEvents.push_back(e.key); });

  m_keyUpListener = m_sdl2Toolkit->RegisterEventCallback(
      SDL_KEYUP, [this](SDL_Event e) { m_keyEvents.push_back(e.key); });
}

VistaSDL2TextInputDriver::~VistaSDL2TextInputDriver() {
  m_sdl2Toolkit->UnregisterEventCallback(SDL_KEYUP, m_keyUpListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_KEYDOWN, m_keyDownListener);
  m_sdl2Toolkit->UnregisterEventCallback(SDL_TEXTINPUT, m_keyTextListener);
}

bool VistaSDL2TextInputDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_connected) {
    return true;
  }

  // Get the keyboard state. It is used for easily checking the pressed modifiers.
  int          keyboardSize = 0;
  const Uint8* keyboard     = SDL_GetKeyboardState(&keyboardSize);

  // If last frame a key was released, we want to add a 0 to the history.
  if (m_keyEvents.empty() && m_textEvents.empty()) {
    if (m_lastFrameValue) {
      m_lastFrameValue = false;

      MeasureStart(dTs);
      UpdateKey(0, 0);
      MeasureStop();

      return true;
    }
    return false;
  }

  int modifiers = GetVistaModifiers(keyboard);

  // This loop registers the following key events:
  // - key down
  //   - the key is a special key
  //   - the key is a character key and alt or ctrl are pressed
  // - key up
  //   - all keys
  while (!m_keyEvents.empty()) {
    SDL_KeyboardEvent e   = m_keyEvents.front();
    int32_t           key = SDLKeyToVistaKey(e.keysym.scancode);

    switch (e.type) {
    case SDL_KEYDOWN:
      if (IsSpecialKey(e.keysym.scancode) || modifiers & VISTA_KEYMOD_ALT ||
          modifiers & VISTA_KEYMOD_CTRL) {
        MeasureStart(dTs);
        UpdateKey(key, modifiers);
        MeasureStop();
      }
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

  // This loop registers the following key events
  // - key down
  //   - a text character has been typed
  while (!m_textEvents.empty()) {
    SDL_TextInputEvent e = m_textEvents.front();

    // We convert the UTF-8 string from SDL to a UTF-16 string. We just get the first character,
    // since we aren't handling multi-character input.
    char16_t character = m_convertUtf8ToUtf16.from_bytes(e.text)[0];

    MeasureStart(dTs);
    // We pass no key mods here, since this is exclusive for text input. SDL already handled key
    // combinations to create capital or special characters and has given these to us. Key
    // combinations for other tasks than character input are handled above.
    UpdateKey(static_cast<int32_t>(character), VISTA_KEYMOD_NONE);
    MeasureStop();

    m_textEvents.pop_front();
  }

  return true;
};

bool VistaSDL2TextInputDriver::DoConnect() {
  m_connected = true;
  return true;
}

bool VistaSDL2TextInputDriver::DoDisconnect() {
  m_connected = false;
  return true;
}
