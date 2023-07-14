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

#include "VistaSDL2ControllerState.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

#include <utility>

namespace {
class SDL2ControllerMeasureTranscoder final : public IVistaMeasureTranscode {
 public:
  SDL2ControllerMeasureTranscoder() {
    m_nNumberOfScalars = 27;
  }

  ~SDL2ControllerMeasureTranscoder() final = default;

  static std::string GetTypeString() {
    return "SDL2ControllerMeasureTranscoder";
  }

  REFL_INLINEIMP(SDL2ControllerMeasureTranscoder, IVistaMeasureTranscode);
};

class SDL2ButtonTranscoder final : public IVistaMeasureTranscode::BoolGet {
 public:
  SDL2ButtonTranscoder(const std::string& name, const std::string& description,
      std::function<bool(const VistaSDL2ControllerState*)> getFunction)
      : IVistaMeasureTranscode::BoolGet(
            name, SDL2ControllerMeasureTranscoder::GetTypeString(), description)
      , m_getFunction(std::move(getFunction)) {
  }

  bool GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m_getFunction(m);
  }

  bool GetValue(const VistaSensorMeasure* measure, bool& axisValue) const final {
    axisValue = GetValue(measure);
    return true;
  }

 private:
  const std::function<bool(const VistaSDL2ControllerState*)> m_getFunction;
};

class SDL2AxisTranscoder final : public IVistaMeasureTranscode::FloatGet {
 public:
  SDL2AxisTranscoder(const std::string& name, const std::string& description,
      std::function<float(const VistaSDL2ControllerState*)> getFunction)
      : IVistaMeasureTranscode::FloatGet(
            name, SDL2ControllerMeasureTranscoder::GetTypeString(), description)
      , m_getFunction(std::move(getFunction)) {
  }

  float GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m_getFunction(m);
  }

  bool GetValue(const VistaSensorMeasure* measure, float& axisValue) const final {
    axisValue = GetValue(measure);
    return true;
  }

 private:
  const std::function<float(const VistaSDL2ControllerState*)> m_getFunction;
};

IVistaPropertyGetFunctor* SapGetter[] = {
    new SDL2ButtonTranscoder("A_PRESSED", "SDL2 controller's A button",
        [](const VistaSDL2ControllerState* m) { return m->aPressed; }),
    new SDL2ButtonTranscoder("B_PRESSED", "SDL2 controller's B button",
        [](const VistaSDL2ControllerState* m) { return m->bPressed; }),
    new SDL2ButtonTranscoder("X_PRESSED", "SDL2 controller's X button",
        [](const VistaSDL2ControllerState* m) { return m->xPressed; }),
    new SDL2ButtonTranscoder("Y_PRESSED", "SDL2 controller's Y button",
        [](const VistaSDL2ControllerState* m) { return m->yPressed; }),

    new SDL2ButtonTranscoder("DPAD_UP_PRESSED", "SDL2 controller's UP button",
        [](const VistaSDL2ControllerState* m) { return m->dpadUpPressed; }),
    new SDL2ButtonTranscoder("DPAD_DOWN_PRESSED", "SDL2 controller's DOWN button",
        [](const VistaSDL2ControllerState* m) { return m->dpadDownPressed; }),
    new SDL2ButtonTranscoder("DPAD_LEFT_PRESSED", "SDL2 controller's LEFT button",
        [](const VistaSDL2ControllerState* m) { return m->dpadLeftPressed; }),
    new SDL2ButtonTranscoder("DPAD_RIGHT_PRESSED", "SDL2 controller's RIGHT button",
        [](const VistaSDL2ControllerState* m) { return m->dpadRightPressed; }),

    new SDL2ButtonTranscoder("STICK_LEFT_PRESSED", "SDL2 controller's left stick button",
        [](const VistaSDL2ControllerState* m) { return m->stickLeftPressed; }),
    new SDL2ButtonTranscoder("STICK_RIGHT_PRESSED", "SDL2 controller's right stick button",
        [](const VistaSDL2ControllerState* m) { return m->stickRightPressed; }),

    new SDL2ButtonTranscoder("SHOULDER_LEFT_PRESSED", "SDL2 controller's left shoulder button",
        [](const VistaSDL2ControllerState* m) { return m->shoulderLeftPressed; }),
    new SDL2ButtonTranscoder("SHOULDER_RIGHT_PRESSED", "SDL2 controller's right shoulder button",
        [](const VistaSDL2ControllerState* m) { return m->shoulderRightPressed; }),

    new SDL2ButtonTranscoder("BACK_PRESSED", "SDL2 controller's back button",
        [](const VistaSDL2ControllerState* m) { return m->backPressed; }),
    new SDL2ButtonTranscoder("GUIDE_PRESSED", "SDL2 controller's guide button",
        [](const VistaSDL2ControllerState* m) { return m->guidePressed; }),
    new SDL2ButtonTranscoder("START_PRESSED", "SDL2 controller's start button",
        [](const VistaSDL2ControllerState* m) { return m->startPressed; }),

    new SDL2ButtonTranscoder("MISC1_PRESSED", "SDL2 controller's misc 1 button",
        [](const VistaSDL2ControllerState* m) { return m->misc1Pressed; }),

    new SDL2ButtonTranscoder("PADDLE1_PRESSED", "SDL2 controller's paddle 1 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle1Pressed; }),
    new SDL2ButtonTranscoder("PADDLE2_PRESSED", "SDL2 controller's paddle 2 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle2Pressed; }),
    new SDL2ButtonTranscoder("PADDLE3_PRESSED", "SDL2 controller's paddle 3 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle3Pressed; }),
    new SDL2ButtonTranscoder("PADDLE4_PRESSED", "SDL2 controller's paddle 4 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle4Pressed; }),

    new SDL2ButtonTranscoder("TOUCHPAD_PRESSED", "SDL2 controller's touchpad button",
        [](const VistaSDL2ControllerState* m) { return m->touchpadPressed; }),

    new SDL2AxisTranscoder("STICK_LEFT_X", "SDL2 controller's left stick x value",
        [](const VistaSDL2ControllerState* m) { return m->stickLeftX; }),
    new SDL2AxisTranscoder("STICK_LEFT_Y", "SDL2 controller's left stick y value",
        [](const VistaSDL2ControllerState* m) { return m->stickLeftY; }),

    new SDL2AxisTranscoder("STICK_RIGHT_X", "SDL2 controller's right stick x value",
        [](const VistaSDL2ControllerState* m) { return m->stickRightX; }),
    new SDL2AxisTranscoder("STICK_RIGHT_Y", "SDL2 controller's right stick y value",
        [](const VistaSDL2ControllerState* m) { return m->stickRightY; }),

    new SDL2AxisTranscoder("TRIGGER_LEFT", "SDL2 controller's left trigger value",
        [](const VistaSDL2ControllerState* m) { return m->triggerLeft; }),
    new SDL2AxisTranscoder("TRIGGER_RIGHT", "SDL2 controller's right trigger value",
        [](const VistaSDL2ControllerState* m) { return m->triggerRight; }),

    nullptr};

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
DEFTRANSCODERPLUG_FUNC_EXPORTS(
    TSimpleTranscoderFactoryFactory<VistaSDL2ControllerTranscoderFactory>)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(
    TSimpleTranscoderFactoryFactory<VistaSDL2ControllerTranscoderFactory>)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaSDL2ControllerTranscoderFactory>)
