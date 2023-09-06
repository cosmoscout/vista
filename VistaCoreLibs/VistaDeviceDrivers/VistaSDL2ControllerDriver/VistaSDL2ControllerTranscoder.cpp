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

class SDL2BoolTranscoder final : public IVistaMeasureTranscode::BoolGet {
 public:
  SDL2BoolTranscoder(const std::string& name, const std::string& description,
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

class SDL2SensorTranscoder final : public IVistaMeasureTranscode::V3Get {
 public:
  SDL2SensorTranscoder(const std::string& name, const std::string& description,
      std::function<VistaVector3D(const VistaSDL2ControllerState*)> getFunction)
      : IVistaMeasureTranscode::V3Get(
            name, SDL2ControllerMeasureTranscoder::GetTypeString(), description)
      , m_getFunction(std::move(getFunction)) {
  }

  VistaVector3D GetValue(const VistaSensorMeasure* measure) const final {
    auto m = reinterpret_cast<const VistaSDL2ControllerState*>(measure->m_vecMeasures.data());
    return m_getFunction(m);
  }

  bool GetValue(const VistaSensorMeasure* measure, VistaVector3D& axisValue) const final {
    axisValue = GetValue(measure);
    return true;
  }

 private:
  const std::function<VistaVector3D(const VistaSDL2ControllerState*)> m_getFunction;
};

IVistaPropertyGetFunctor* SapGetter[] = {
    new SDL2BoolTranscoder("A_PRESSED", "SDL2 controller's A button",
        [](const VistaSDL2ControllerState* m) { return m->aPressed; }),
    new SDL2BoolTranscoder("B_PRESSED", "SDL2 controller's B button",
        [](const VistaSDL2ControllerState* m) { return m->bPressed; }),
    new SDL2BoolTranscoder("X_PRESSED", "SDL2 controller's X button",
        [](const VistaSDL2ControllerState* m) { return m->xPressed; }),
    new SDL2BoolTranscoder("Y_PRESSED", "SDL2 controller's Y button",
        [](const VistaSDL2ControllerState* m) { return m->yPressed; }),

    new SDL2BoolTranscoder("DPAD_UP_PRESSED", "SDL2 controller's UP button",
        [](const VistaSDL2ControllerState* m) { return m->dpadUpPressed; }),
    new SDL2BoolTranscoder("DPAD_DOWN_PRESSED", "SDL2 controller's DOWN button",
        [](const VistaSDL2ControllerState* m) { return m->dpadDownPressed; }),
    new SDL2BoolTranscoder("DPAD_LEFT_PRESSED", "SDL2 controller's LEFT button",
        [](const VistaSDL2ControllerState* m) { return m->dpadLeftPressed; }),
    new SDL2BoolTranscoder("DPAD_RIGHT_PRESSED", "SDL2 controller's RIGHT button",
        [](const VistaSDL2ControllerState* m) { return m->dpadRightPressed; }),

    new SDL2BoolTranscoder("STICK_LEFT_PRESSED", "SDL2 controller's left stick button",
        [](const VistaSDL2ControllerState* m) { return m->stickLeftPressed; }),
    new SDL2BoolTranscoder("STICK_RIGHT_PRESSED", "SDL2 controller's right stick button",
        [](const VistaSDL2ControllerState* m) { return m->stickRightPressed; }),

    new SDL2BoolTranscoder("SHOULDER_LEFT_PRESSED", "SDL2 controller's left shoulder button",
        [](const VistaSDL2ControllerState* m) { return m->shoulderLeftPressed; }),
    new SDL2BoolTranscoder("SHOULDER_RIGHT_PRESSED", "SDL2 controller's right shoulder button",
        [](const VistaSDL2ControllerState* m) { return m->shoulderRightPressed; }),

    new SDL2BoolTranscoder("BACK_PRESSED", "SDL2 controller's back button",
        [](const VistaSDL2ControllerState* m) { return m->backPressed; }),
    new SDL2BoolTranscoder("GUIDE_PRESSED", "SDL2 controller's guide button",
        [](const VistaSDL2ControllerState* m) { return m->guidePressed; }),
    new SDL2BoolTranscoder("START_PRESSED", "SDL2 controller's start button",
        [](const VistaSDL2ControllerState* m) { return m->startPressed; }),

    new SDL2BoolTranscoder("MISC1_PRESSED", "SDL2 controller's misc 1 button",
        [](const VistaSDL2ControllerState* m) { return m->misc1Pressed; }),

    new SDL2BoolTranscoder("PADDLE1_PRESSED", "SDL2 controller's paddle 1 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle1Pressed; }),
    new SDL2BoolTranscoder("PADDLE2_PRESSED", "SDL2 controller's paddle 2 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle2Pressed; }),
    new SDL2BoolTranscoder("PADDLE3_PRESSED", "SDL2 controller's paddle 3 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle3Pressed; }),
    new SDL2BoolTranscoder("PADDLE4_PRESSED", "SDL2 controller's paddle 4 button",
        [](const VistaSDL2ControllerState* m) { return m->paddle4Pressed; }),

    new SDL2BoolTranscoder("TOUCHPAD_PRESSED", "SDL2 controller's touchpad button",
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

    new SDL2BoolTranscoder("HAS_ACCEL_SENSOR", "If the controller has an acceleration sensor.",
        [](const VistaSDL2ControllerState* m) { return m->hasAcceleration; }),
    new SDL2SensorTranscoder("IMU_ACCELERATION", "The IMU acceleration.",
        [](const VistaSDL2ControllerState* m) { return VistaVector3D(m->imuAcceleration); }),

    new SDL2BoolTranscoder("HAS_GYRO_SENSOR", "If the controller has an gyro sensor.",
        [](const VistaSDL2ControllerState* m) { return m->hasGyro; }),
    new SDL2SensorTranscoder("IMU_GYRO", "The IMU gyro.",
        [](const VistaSDL2ControllerState* m) { return VistaVector3D(m->imuGyro); }),

    new SDL2BoolTranscoder("TOUCHPAD_FINGER_1_DOWN", "If one finger is on the touchpad.",
        [](const VistaSDL2ControllerState* m) { return m->touchpadFinger1Down; }),
    new SDL2AxisTranscoder("TOUCHPAD_FINGER_1_X", "The x positon of finger one on the touchpad",
        [](const VistaSDL2ControllerState* m) { return m->touchpadFinger1X; }),
    new SDL2AxisTranscoder("TOUCHPAD_FINGER_1_Y", "The y positon of finger one on the touchpad",
        [](const VistaSDL2ControllerState* m) { return m->touchpadFinger1Y; }),

    new SDL2BoolTranscoder("TOUCHPAD_FINGER_2_DOWN", "If two fingers are on the touchpad.",
        [](const VistaSDL2ControllerState* m) { return m->touchpadFinger2Down; }),
    new SDL2AxisTranscoder("TOUCHPAD_FINGER_2_X", "The x positon of finger two on the touchpad",
        [](const VistaSDL2ControllerState* m) { return m->touchpadFinger2X; }),
    new SDL2AxisTranscoder("TOUCHPAD_FINGER_2_Y", "The y positon of finger two on the touchpad",
        [](const VistaSDL2ControllerState* m) { return m->touchpadFinger2Y; }),

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
