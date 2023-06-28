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
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/

#include "VistaVRPNDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <string.h>
#include <time.h>

#include <vrpn_Analog.h>
#include <vrpn_Button.h>
#include <vrpn_Tracker.h>

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {

class VRPNTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(VRPNTranscoder, IVistaMeasureTranscode);

 public:
  VRPNTranscoder() {
    // inherited as protected member
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VRPNTranscoder";
  }
};

class VRPNAnalogTranscoder : public VRPNTranscoder {
  REFL_INLINEIMP(VRPNAnalogTranscoder, VRPNTranscoder)
 public:
  VRPNAnalogTranscoder() {
    m_nNumberOfScalars = 128;
  }

  static std::string GetTypeString() {
    return "VRPNAnalogTranscoder";
  }
};

class VRPNButtonTranscoder : public VRPNTranscoder {
  REFL_INLINEIMP(VRPNButtonTranscoder, VRPNTranscoder)
 public:
  VRPNButtonTranscoder() {
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VRPNButtonTranscoder";
  }
};

class VRPNTrackerTranscoder : public VRPNTranscoder {
  REFL_INLINEIMP(VRPNTrackerTranscoder, VRPNTranscoder)
 public:
  VRPNTrackerTranscoder() {
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VRPNTrackerTranscoder";
  }
};

class VRPNTimeStampGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaType::microtime> {
 public:
  VRPNTimeStampGet()
      : IVistaMeasureTranscode::TTranscodeValueGet<VistaType::microtime>("VRPN_TIMECODE",
            VRPNTranscoder::GetTypeString(),
            "transformed VistaType::microstamp from the VRPN sample") {
  }

  virtual VistaType::microtime GetValue(const VistaSensorMeasure* pMeasure) const {
    // the timeval is always the first member in the CB structs
    struct timeval* tv = (struct timeval*)&(*pMeasure).m_vecMeasures[0];
    return double(tv->tv_sec + (tv->tv_usec / 1000000.0));
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaType::microtime& dtOut) const {
    dtOut = GetValue(pMeasure);
    return true;
  }
};

class VRPNAnalogValueGet : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VRPNAnalogValueGet()
      : IVistaMeasureTranscode::ScalarDoubleGet("VALUE", VRPNAnalogTranscoder::GetTypeString(),
            "get analog value for a sample (indexed)") {
  }

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dValue, unsigned int nIdx) const {
    vrpn_ANALOGCB* cb = (vrpn_ANALOGCB*)&(*pMeasure).m_vecMeasures[0];
    if (cb->num_channel < (int)nIdx)
      return false; // not my channel, channel request out of bounds

    if (nIdx >= vrpn_CHANNEL_MAX)
      return false; // index too high

    dValue = cb->channel[nIdx];

    return true;
  }
};

class VRPNAnalogChannelGet : public IVistaMeasureTranscode::UIntGet {
 public:
  VRPNAnalogChannelGet()
      : IVistaMeasureTranscode::UIntGet("CHANNELS", VRPNAnalogTranscoder::GetTypeString(),
            "query channel from current sample") {
  }

  virtual unsigned int GetValue(const VistaSensorMeasure* pMeasure) const {
    vrpn_ANALOGCB* cb = (vrpn_ANALOGCB*)&(*pMeasure).m_vecMeasures[0];
    return cb->num_channel;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, unsigned int& nValue) const {
    nValue = GetValue(pMeasure);
    return true;
  }
};

class VRPNAnalogChannelBlindGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<double>> {
 public:
  VRPNAnalogChannelBlindGet()
      : IVistaMeasureTranscode::TTranscodeValueGet<std::vector<double>>("VALUES",
            VRPNAnalogTranscoder::GetTypeString(),
            "retrieve current analog value by using the channel as given in the data") {
  }

  virtual std::vector<double> GetValue(const VistaSensorMeasure* pMeasure) const {
    // the timeval is always the first member in the CB structs
    std::vector<double> vecRet;
    GetValue(pMeasure, vecRet);
    return vecRet;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, std::vector<double>& dtOut) const {
    vrpn_ANALOGCB* cb = (vrpn_ANALOGCB*)&(*pMeasure).m_vecMeasures[0];
    dtOut.resize(cb->num_channel);

    // we put trust in this that vrpn_VistaType::float64 is always sizeof double ;)
    if (cb->num_channel > 0)
      memcpy(&dtOut[0], cb->channel, cb->num_channel * sizeof(double));
    return true;
  }
};

// ##############################################################################

class VRPNButtonValueGet : public IVistaMeasureTranscode::UIntGet {
 public:
  VRPNButtonValueGet()
      : IVistaMeasureTranscode::UIntGet("STATE", VRPNButtonTranscoder::GetTypeString(),
            "return the state of the button in this sample") {
  }

  virtual unsigned int GetValue(const VistaSensorMeasure* pMeasure) const {
    vrpn_BUTTONCB* cb = (vrpn_BUTTONCB*)&(*pMeasure).m_vecMeasures[0];
    return cb->state;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, unsigned int& nValue) const {
    nValue = GetValue(pMeasure);
    return true;
  }
};

class VRPNButtonNumberGet : public IVistaMeasureTranscode::UIntGet {
 public:
  VRPNButtonNumberGet()
      : IVistaMeasureTranscode::UIntGet("NUMBER", VRPNButtonTranscoder::GetTypeString(),
            "returns the number of the button pushed") {
  }

  virtual unsigned int GetValue(const VistaSensorMeasure* pMeasure) const {
    vrpn_BUTTONCB* cb = (vrpn_BUTTONCB*)&(*pMeasure).m_vecMeasures[0];
    return cb->button;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, unsigned int& nValue) const {
    nValue = GetValue(pMeasure);
    return true;
  }
};

class VRPNButtonMaskGet : public IVistaMeasureTranscode::UIntGet {
 public:
  VRPNButtonMaskGet()
      : IVistaMeasureTranscode::UIntGet("BTMASK", VRPNButtonTranscoder::GetTypeString(),
            "returns a 32bit mask for button state") {
  }

  virtual unsigned int GetValue(const VistaSensorMeasure* pMeasure) const {
    vrpn_BUTTONCB* cb = (vrpn_BUTTONCB*)&(*pMeasure).m_vecMeasures[0];
    return (1UL << cb->button) * cb->state;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, unsigned int& nValue) const {
    nValue = GetValue(pMeasure);
    return true;
  }
};

// ##############################################################################

class VRPNSensorNumberGet : public IVistaMeasureTranscode::UIntGet {
 public:
  VRPNSensorNumberGet()
      : IVistaMeasureTranscode::UIntGet("SENSORINDEX", VRPNTrackerTranscoder::GetTypeString(),
            "retrieve the sensor index of the report") {
  }

  virtual unsigned int GetValue(const VistaSensorMeasure* pMeasure) const {
    vrpn_TRACKERCB* cb = (vrpn_TRACKERCB*)&(*pMeasure).m_vecMeasures[0];
    return cb->sensor;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, unsigned int& value) const {
    value = GetValue(pMeasure);
    return true;
  }
};

class VRPNSensorPositionGet : public IVistaMeasureTranscode::V3Get {
 public:
  VRPNSensorPositionGet()
      : IVistaMeasureTranscode::V3Get("POSITION", VRPNTrackerTranscoder::GetTypeString(),
            "retrieve position value of tracker record") {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const {
    VistaVector3D vec;
    GetValue(pMeasure, vec);
    return vec;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& vec) const {
    vrpn_TRACKERCB* cb = (vrpn_TRACKERCB*)&(*pMeasure).m_vecMeasures[0];
    vec[0]             = float(cb->pos[0]); // have to round here
    vec[1]             = float(cb->pos[1]);
    vec[2]             = float(cb->pos[2]);
    return true;
  }
};

class VRPNSensorOrientationGet : public IVistaMeasureTranscode::QuatGet {
 public:
  VRPNSensorOrientationGet()
      : IVistaMeasureTranscode::QuatGet("ORIENTATION", VRPNTrackerTranscoder::GetTypeString(),
            "retrieve orientation value of tracker record") {
  }

  virtual VistaQuaternion GetValue(const VistaSensorMeasure* pMeasure) const {
    VistaQuaternion q;
    GetValue(pMeasure, q);
    return q;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaQuaternion& q) const {
    vrpn_TRACKERCB* cb = (vrpn_TRACKERCB*)&(*pMeasure).m_vecMeasures[0];

    // vrpn stores q = (x,y,z,w) (like in vista)
    q[Vista::X] = (float)cb->quat[0];
    q[Vista::Y] = (float)cb->quat[1];
    q[Vista::Z] = (float)cb->quat[2];
    q[Vista::W] = (float)cb->quat[3];

    return true;
  }
};

IVistaPropertyGetFunctor* SaGetter[] = {new VRPNTimeStampGet, new VRPNAnalogValueGet,
    new VRPNAnalogChannelBlindGet, new VRPNAnalogChannelGet,

    new VRPNButtonValueGet, new VRPNButtonNumberGet, new VRPNButtonMaskGet,

    new VRPNSensorNumberGet, new VRPNSensorPositionGet, new VRPNSensorOrientationGet,

    NULL};

IVistaPropertyGetFunctor* SaParameterGetter[] = {NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {NULL};

class VistaVRPNDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory {
 public:
  virtual IVistaMeasureTranscoderFactory* CreateFactoryForType(const std::string& strTypeName) {
    if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(strTypeName, "ANALOG")) {
      return new TDefaultTranscoderFactory<VRPNAnalogTranscoder>(
          VRPNAnalogTranscoder::GetTypeString());
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(strTypeName, "BUTTON")) {
      return new TDefaultTranscoderFactory<VRPNButtonTranscoder>(
          VRPNButtonTranscoder::GetTypeString());
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(strTypeName, "TRACKER")) {
      return new TDefaultTranscoderFactory<VRPNTrackerTranscoder>(
          VRPNTrackerTranscoder::GetTypeString());
    } else
      return NULL;
  }

  virtual void DestroyTranscoderFactory(IVistaMeasureTranscoderFactory* fac) {
    delete fac;
  }

  static void OnUnload() {
    TDefaultTranscoderFactory<VRPNAnalogTranscoder> a(VRPNAnalogTranscoder::GetTypeString());
    a.OnUnload();
    TDefaultTranscoderFactory<VRPNButtonTranscoder> b(VRPNButtonTranscoder::GetTypeString());
    b.OnUnload();
    TDefaultTranscoderFactory<VRPNTrackerTranscoder> t(VRPNTrackerTranscoder::GetTypeString());
    t.OnUnload();
  }
};

//	VistaVRPNDriverTranscoderFactoryFactory* SpFactory = NULL;
} // namespace

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTAVRPNTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(VistaVRPNDriverTranscoderFactoryFactory)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(VistaVRPNDriverTranscoderFactoryFactory)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaVRPNDriverTranscoderFactoryFactory)
