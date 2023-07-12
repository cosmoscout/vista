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

#include "VistaBase/VistaStreamUtils.h"
#include "VistaSDL2ControllerState.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

namespace {
class SDL2ControllerMeasureTranscoder final : public IVistaMeasureTranscode {
 public:
  SDL2ControllerMeasureTranscoder() {
    m_nNumberOfScalars = 21;
  }

  ~SDL2ControllerMeasureTranscoder() final = default;

  static std::string GetTypeString() {
    return "SDL2ControllerMeasureTranscoder";
  }

  REFL_INLINEIMP(SDL2ControllerMeasureTranscoder, IVistaMeasureTranscode);
};

class SDL2APressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2APressedTranscoder() 
    : IVistaMeasureTranscode::TTranscodeValueGet<bool>("A_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's A button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->aPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2BPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2BPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("B_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's B button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->bPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2XPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2XPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("X_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's X button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->xPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2YPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2YPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("Y_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's Y button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->yPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2UpPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2UpPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("DPAD_UP_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's UP button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->dpadUpPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2DownPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2DownPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("DPAD_DOWN_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's DOWN button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->dpadDownPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2LeftPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2LeftPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("DPAD_LEFT_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's LEFT button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->dpadLeftPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2RightPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2RightPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("DPAD_RIGHT_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's RIGHT button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->dpadRightPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2StickLeftPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2StickLeftPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("STICK_LEFT_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's STICK_LEFT button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->stickLeftPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2StickRightPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2StickRightPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("STICK_RIGHT_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's STICK_RIGHT button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->stickRightPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2ShoulderLeftPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2ShoulderLeftPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("SHOULDER_LEFT_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's SHOULDER_LEFT button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->shoulderLeftPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2ShoulderRightPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2ShoulderRightPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("SHOULDER_RIGHT_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's SHOULDER_RIGHT button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->shoulderRightPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2BackPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2BackPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("BACK_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's BACK button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->backPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2GuidePressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2GuidePressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("GUIDE_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's GUIDE button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->guidePressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2StartPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2StartPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("START_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's START button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->startPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2Misc1PressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2Misc1PressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("MISC1_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's MISC1 button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->misc1Pressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2Paddle1PressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2Paddle1PressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("PADDLE1_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's PADDLE1 button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->paddle1Pressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2Paddle2PressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2Paddle2PressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("PADDLE2_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's PADDLE2 button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->paddle2Pressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2Paddle3PressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2Paddle3PressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("PADDLE3_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's PADDLE3 button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->paddle3Pressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2Paddle4PressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2Paddle4PressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("PADDLE4_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's PADDLE4 button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->paddle4Pressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2TouchPadPressedTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  SDL2TouchPadPressedTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>("TOUCHPAD_PRESSED",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's TOUCHPAD button") {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->touchpadPressed;
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2StickLeftXTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<int16_t> {
 public:
  SDL2StickLeftXTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<int16_t>("STICK_LEFT_X",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's left stick x value") {
  }

  int16_t GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->stickLeftX;
  }

  bool GetValue(const VistaSensorMeasure* measure, int16_t& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2StickLeftYTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<int16_t> {
 public:
  SDL2StickLeftYTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<int16_t>("STICK_LEFT_Y",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's left stick y value") {
  }

  int16_t GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->stickLeftY;
  }

  bool GetValue(const VistaSensorMeasure* measure, int16_t& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2StickRightXTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<int16_t> {
 public:
  SDL2StickRightXTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<int16_t>("STICK_RIGHT_X",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's right stick x value") {
  }

  int16_t GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->stickRightX;
  }

  bool GetValue(const VistaSensorMeasure* measure, int16_t& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2StickRightYTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<int16_t> {
 public:
  SDL2StickRightYTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<int16_t>("STICK_RIGHT_Y",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's right stick y value") {
  }

  int16_t GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->stickRightY;
  }

  bool GetValue(const VistaSensorMeasure* measure, int16_t& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2TriggerLeftTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<int16_t> {
 public:
  SDL2TriggerLeftTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<int16_t>("TRIGGER_LEFT",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's left trigger value") {
  }

  int16_t GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->triggerLeft;
  }

  bool GetValue(const VistaSensorMeasure* measure, int16_t& button) const final {
    button = GetValue(measure);
    return true;
  }
};

class SDL2TriggerRightTranscoder final : public IVistaMeasureTranscode::TTranscodeValueGet<int16_t> {
 public:
  SDL2TriggerRightTranscoder()
      : IVistaMeasureTranscode::TTranscodeValueGet<int16_t>("TRIGGER_RIGHT",
            SDL2ControllerMeasureTranscoder::GetTypeString(), "SDL2 controller's right trigger value") {
  }

  int16_t GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m->triggerRight;
  }

  bool GetValue(const VistaSensorMeasure* measure, int16_t& button) const final {
    button = GetValue(measure);
    return true;
  }
};

IVistaPropertyGetFunctor* SapGetter[] = {
  new SDL2APressedTranscoder,
  new SDL2BPressedTranscoder,
  new SDL2XPressedTranscoder,
  new SDL2YPressedTranscoder,

  new SDL2UpPressedTranscoder,
  new SDL2DownPressedTranscoder,
  new SDL2LeftPressedTranscoder,
  new SDL2RightPressedTranscoder,

  new SDL2StickLeftPressedTranscoder,
  new SDL2StickRightPressedTranscoder,

  new SDL2ShoulderLeftPressedTranscoder,
  new SDL2ShoulderRightPressedTranscoder,

  new SDL2BackPressedTranscoder,
  new SDL2GuidePressedTranscoder,
  new SDL2StartPressedTranscoder,

  new SDL2Misc1PressedTranscoder,

  new SDL2Paddle1PressedTranscoder,
  new SDL2Paddle2PressedTranscoder,
  new SDL2Paddle3PressedTranscoder,
  new SDL2Paddle4PressedTranscoder,

  new SDL2TouchPadPressedTranscoder,

  new SDL2StickLeftXTranscoder,
  new SDL2StickLeftYTranscoder,

  new SDL2StickRightXTranscoder,
  new SDL2StickRightYTranscoder,

  new SDL2TriggerLeftTranscoder,
  new SDL2TriggerRightTranscoder,

  nullptr
};

class VistaSDL2ControllerTranscoderFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder() {
    return new SDL2ControllerMeasureTranscoder;
  }

  virtual void DestroyTranscoder(IVistaMeasureTranscode* transcoder) {
    delete transcoder;
  }

  virtual std::string GetTranscoderName() const {
    return "VistaSDL2ControllerTranscoderFactory";
  }
};

} // namespace

#ifdef VISTASDL2CONTROLLERTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(TSimpleTranscoderFactoryFactory<VistaSDL2ControllerTranscoderFactory>)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(TSimpleTranscoderFactoryFactory<VistaSDL2ControllerTranscoderFactory>)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaSDL2ControllerTranscoderFactory>)
