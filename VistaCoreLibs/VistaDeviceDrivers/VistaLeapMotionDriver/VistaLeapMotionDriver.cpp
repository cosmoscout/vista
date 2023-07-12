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

#include "VistaLeapMotionDriver.h"

#include "VistaLeapMotionCommonShare.h"

#include <Leap.h>

#include "VistaBase/VistaUtilityMacros.h"
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>

#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

REFL_IMPLEMENT_FULL(
    VistaLeapMotionDriver::Parameters, VistaDriverGenericParameterAspect::IParameterContainer);

// ############################################################################
// LEAPLISTENER
// ############################################################################

class VistaLeapMotionDriver::LeapListener : public Leap::Listener {
 public:
  LeapListener(VistaLeapMotionDriver* pDriver)
      : m_pDriver(pDriver) {
  }

  virtual void onConnect(const Leap::Controller&) {
    m_pDriver->GetParameters()->SetListenerConnectionState(true);
  }

  virtual void onFrame(const Leap::Controller&) {
    m_pDriver->Update();
  }

  virtual void onDisconnect(const Leap::Controller&) {
    m_pDriver->GetParameters()->SetListenerConnectionState(false);
  }

 private:
  VistaLeapMotionDriver* m_pDriver;
};

// ############################################################################
// CREATION METHOD
// ############################################################################

namespace {
class LeapMotionActivationContext : public VistaDriverThreadAspect::IActivationContext {
 public:
  LeapMotionActivationContext(VistaLeapMotionDriver* parent)
      : VistaDriverThreadAspect::IActivationContext()
      , m_parent(parent)
      , m_update_prepare(0) {
  }

  virtual ~LeapMotionActivationContext() {
  }

  virtual int GetProcessorAffinity() const {
    return -1;
  }

  virtual bool SetProcessorAffinity(int) {
    return false;
  }

  virtual bool SetPriority(const VistaPriority&) {
    return false;
  }

  virtual VistaPriority GetPriority() const {
    return VistaPriority();
  }

  virtual bool Activate() {
    m_parent->SetIsEnabled(true);
    return IsActive();
  }

  virtual bool IsActive() const {
    return m_parent->GetIsEnabled();
  }

  virtual bool DeActivate() {
    m_parent->SetIsEnabled(false);
    return !m_parent->GetIsEnabled();
  }

  virtual bool Pause() {
    return DeActivate();
  }

  virtual bool IsPaused() const {
    return IsActive();
  }

  virtual bool UnPause() {
    return Activate();
  }

  virtual VistaDriverThreadAspect::IDriverUpdatePrepare* GetDriverUpdatePrepare() const {
    return m_update_prepare;
  }

  virtual void SetDriverUpdatePrepare(VistaDriverThreadAspect::IDriverUpdatePrepare* pUpdate) {
    m_update_prepare = pUpdate;
  }

  VistaLeapMotionDriver*                         m_parent;
  VistaDriverThreadAspect::IDriverUpdatePrepare* m_update_prepare;
};

class GesturePropGetter : public IVistaPropertyGetFunctor {
 public:
  GesturePropGetter(
      const std::string& prop_name, const std::string& class_name, const std::string& desc)
      : IVistaPropertyGetFunctor(prop_name, class_name, desc) {
  }

  virtual bool operator()(const IVistaPropertyAwareable& o, VistaProperty& set_prop) const {
    const VistaLeapMotionDriver::Parameters* params =
        dynamic_cast<const VistaLeapMotionDriver::Parameters*>(&o);
    if (params == 0)
      return false;

    float v = params->GetGestureProp(GetNameForNameable());

    set_prop.SetValue(VistaConversion::ToString(v));
    set_prop.SetPropertyType(VistaProperty::PROPT_DOUBLE);
    return true;
  }

  std::string m_prop_name;
};

class GesturePropSetter : public IVistaPropertySetFunctor {
 public:
  GesturePropSetter(
      const std::string& prop_name, const std::string& class_name, const std::string& desc)
      : IVistaPropertySetFunctor(prop_name, class_name, desc) {
  }

  virtual bool operator()(IVistaPropertyAwareable& o, const VistaProperty& p) {
    VistaLeapMotionDriver::Parameters* params =
        dynamic_cast<VistaLeapMotionDriver::Parameters*>(&o);
    if (params == 0)
      return false;

    return params->SetGestureProp(
        GetNameForNameable(), VistaConversion::ConvertType<float, std::string>(p.GetValue()));
  }
};

class PolicyPropSetter : public IVistaPropertySetFunctor {
 public:
  PolicyPropSetter(
      const std::string& prop_name, const std::string& class_name, const std::string& desc)
      : IVistaPropertySetFunctor(prop_name, class_name, desc) {
  }

  virtual bool operator()(IVistaPropertyAwareable& o, const VistaProperty& p) {
    VistaLeapMotionDriver::Parameters* params =
        dynamic_cast<VistaLeapMotionDriver::Parameters*>(&o);
    if (params == 0)
      return false;

    std::list<std::string> prop_values =
        VistaConversion::FromString<std::list<std::string>>(p.GetValue());

    VistaLeapMotionDriver::Parameters::PolicyFlags policy =
        VistaLeapMotionDriver::Parameters::POLICY_DEFAULT;
    if (std::find(prop_values.begin(), prop_values.end(), "POLICY_BACKGROUND_FRAMES") !=
        prop_values.end()) {
      policy = VistaLeapMotionDriver::Parameters::POLICY_BACKGROUND_FRAMES;
    }
    if (std::find(prop_values.begin(), prop_values.end(), "POLICY_IMAGES") != prop_values.end()) {
      policy = VistaLeapMotionDriver::Parameters::PolicyFlags(
          int(policy) | int(VistaLeapMotionDriver::Parameters::POLICY_IMAGES));
    }

    if (std::find(prop_values.begin(), prop_values.end(), "POLICY_OPTIMIZE_HMD") !=
        prop_values.end()) {
      policy = VistaLeapMotionDriver::Parameters::PolicyFlags(
          int(policy) | int(VistaLeapMotionDriver::Parameters::POLICY_OPTIMIZE_HMD));
    }

    return params->SetPolicyFlags(policy);
  }
};

class PolicyGetter : public IVistaPropertyGetFunctor {
 public:
  PolicyGetter(const std::string& prop_name, const std::string& class_name, const std::string& desc)
      : IVistaPropertyGetFunctor(prop_name, class_name, desc) {
  }

  virtual bool operator()(const IVistaPropertyAwareable& o, VistaProperty& set_prop) const {
    const VistaLeapMotionDriver::Parameters* params =
        dynamic_cast<const VistaLeapMotionDriver::Parameters*>(&o);
    if (params == 0)
      return false;

    VistaLeapMotionDriver::Parameters::PolicyFlags policy = params->GetPolicyFlags();

    if (policy == VistaLeapMotionDriver::Parameters::POLICY_DEFAULT) {
      set_prop.SetValue("POLICY_DEFAULT");
      set_prop.SetPropertyType(VistaProperty::PROPT_STRING);
      return true;
    }

    std::list<std::string> policy_symbols;
    if (policy & VistaLeapMotionDriver::Parameters::POLICY_BACKGROUND_FRAMES)
      policy_symbols.push_back("POLICYBACKGROUND_FRAMES");

    if (policy & VistaLeapMotionDriver::Parameters::POLICY_IMAGES)
      policy_symbols.push_back("POLICY_IMAGES");

    if (policy & VistaLeapMotionDriver::Parameters::POLICY_OPTIMIZE_HMD)
      policy_symbols.push_back("POLICY_OPTIMIZE_HMD");

    set_prop.SetValue(VistaConversion::ToString<std::list<std::string>>(policy_symbols));
    set_prop.SetPropertyType(VistaProperty::PROPT_STRING);

    return true;
  }
};

IVistaPropertyGetFunctor* SaParameterGetter[] = {
    new GesturePropGetter("Gesture.Circle.MinRadius", SsReflectionName, "defaults to 5.0 mm"),
    new GesturePropGetter(
        "Gesture.Circle.MinArc", SsReflectionName, "defaults to 1.5 * pi radians"),
    new GesturePropGetter("Gesture.Swipe.MinLength", SsReflectionName, "defaults to 150 mm"),
    new GesturePropGetter("Gesture.Swipe.MinVelocity", SsReflectionName, "defaults to 1000 mm/s"),
    new GesturePropGetter(
        "Gesture.KeyTap.MinDownVelocity", SsReflectionName, "defaults to 50 mm/s"),
    new GesturePropGetter("Gesture.KeyTap.HistorySeconds", SsReflectionName, "defaults to 0.1 s"),
    new GesturePropGetter("Gesture.KeyTap.MinDistance", SsReflectionName, "defaults to 3.0 mm"),
    new GesturePropGetter(
        "Gesture.ScreenTap.MinForwardVelocity", SsReflectionName, "defaults to 50 mm"),
    new GesturePropGetter(
        "Gesture.ScreenTap.HistorySeconds", SsReflectionName, "defaults to 0.1 mm"),
    new GesturePropGetter("Gesture.ScreenTap.MinDistance", SsReflectionName, "defaults to 5.0 mm"),

    new TVistaPropertyGet<VistaType::microtime, VistaLeapMotionDriver::Parameters,
        VistaProperty::PROPT_DOUBLE>("CONNECTION_TIMEOUT",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaLeapMotionDriver::Parameters::GetConnectionTimeout,
        "returns the timout on connect in seconds."),
    new TVistaPropertyGet<bool, VistaLeapMotionDriver::Parameters, VistaProperty::PROPT_BOOL>(
        "LISTENER_CONNECTION_STATE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaLeapMotionDriver::Parameters::GetListenerConnectionState,
        "returns the current connection state as reported from the listener (false w/o listener)."),

    new PolicyGetter(
        "POLICY", SsReflectionName, "Returns the string list of currently set policy flags"),

    NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {
    new GesturePropSetter("Gesture.Circle.MinRadius", SsReflectionName, "defaults to 5.0 mm"),
    new GesturePropSetter(
        "Gesture.Circle.MinArc", SsReflectionName, "defaults to 1.5 * pi radians"),
    new GesturePropSetter("Gesture.Swipe.MinLength", SsReflectionName, "defaults to 150 mm"),
    new GesturePropSetter("Gesture.Swipe.MinVelocity", SsReflectionName, "defaults to 1000 mm/s"),
    new GesturePropSetter(
        "Gesture.KeyTap.MinDownVelocity", SsReflectionName, "defaults to 50 mm/s"),
    new GesturePropSetter("Gesture.KeyTap.HistorySeconds", SsReflectionName, "defaults to 0.1 s"),
    new GesturePropSetter("Gesture.KeyTap.MinDistance", SsReflectionName, "defaults to 3.0 mm"),
    new GesturePropSetter(
        "Gesture.ScreenTap.MinForwardVelocity", SsReflectionName, "defaults to 50 mm"),
    new GesturePropSetter(
        "Gesture.ScreenTap.HistorySeconds", SsReflectionName, "defaults to 0.1 mm"),
    new GesturePropSetter("Gesture.ScreenTap.MinDistance", SsReflectionName, "defaults to 5.0 mm"),
    new TVistaPropertySet<VistaType::microtime, VistaType::microtime,
        VistaLeapMotionDriver::Parameters>("CONNECTION_TIMEOUT", SsReflectionName,
        &VistaLeapMotionDriver::Parameters::SetConnectionTimeout,
        "sets the timeout on connect in seconds, defaults to 5s."),

    new PolicyPropSetter("POLICY", SsReflectionName,
        "Specify policy for leap controller as string list {POLICY_BACKGROUND_FRAMES, "
        "POLICY_IMAGES, POLICY_OPTIMIZE_HMD, POLICY_DEFAULT}"),

    NULL};

class VistaLeapController : public Leap::Controller {};
} // namespace

VistaLeapMotionCreationMethod::VistaLeapMotionCreationMethod(
    IVistaTranscoderFactoryFactory* pMetaFac)
    : IVistaDriverCreationMethod(pMetaFac) {
  RegisterSensorType("HAND", sizeof(VistaLeapMotionMeasures::HandMeasure), 120,
      pMetaFac->CreateFactoryForType("HAND"));
  RegisterSensorType("FINGERS", sizeof(VistaLeapMotionMeasures::FingersMeasure), 120,
      pMetaFac->CreateFactoryForType("FINGERS"));
  RegisterSensorType("TOOLS", sizeof(VistaLeapMotionMeasures::ToolsMeasure), 120,
      pMetaFac->CreateFactoryForType("TOOLS"));
  RegisterSensorType("KEYTAP", sizeof(VistaLeapMotionMeasures::GestureMeasure), 120,
      pMetaFac->CreateFactoryForType("GESTURE"));
  RegisterSensorType("SCREENTAP", sizeof(VistaLeapMotionMeasures::GestureMeasure), 120,
      pMetaFac->CreateFactoryForType("GESTURE"));
  RegisterSensorType("CIRCLE", sizeof(VistaLeapMotionMeasures::GestureMeasure), 120,
      pMetaFac->CreateFactoryForType("GESTURE"));
  RegisterSensorType("SWIPE", sizeof(VistaLeapMotionMeasures::GestureMeasure), 120,
      pMetaFac->CreateFactoryForType("GESTURE"));
  RegisterSensorType("GESTURE", sizeof(VistaLeapMotionMeasures::GestureMeasure), 120,
      pMetaFac->CreateFactoryForType("GESTURE"));
  RegisterSensorType("IMAGE", sizeof(VistaLeapMotionMeasures::ImageMeasure), 120,
      pMetaFac->CreateFactoryForType("IMAGE"));
}

IVistaDeviceDriver* VistaLeapMotionCreationMethod::CreateDriver() {
  return new VistaLeapMotionDriver(this);
}

// ############################################################################
// Helper functions
// ############################################################################

namespace {
const float s_nMillimetersToMeters = 1e-3f;
void        LeapToVistaVector(
           VistaVector3D& v3VistaVector, const Leap::Vector& oLeapVector, const bool bIsPosition = true) {
  v3VistaVector.SetValues(oLeapVector.toFloatPointer());
  if (bIsPosition == false) {
    v3VistaVector[3] = 0;
  } else {
    v3VistaVector[3] = 1;
    v3VistaVector *= s_nMillimetersToMeters;
  }
}

void FillPointable(VistaLeapMotionMeasures::Pointable& oMeasure, const Leap::Pointable& oPointable,
    VistaType::uint64 time_stamp) {
  oMeasure.m_driver_timestamp = time_stamp;
  oMeasure.m_bVisible         = oPointable.isValid();
  oMeasure.m_nId              = oPointable.id();
  oMeasure.m_nTimeVisible     = oPointable.timeVisible();
  oMeasure.m_nHandId          = oPointable.hand().id();

  oMeasure.m_nWidth  = s_nMillimetersToMeters * oPointable.width();
  oMeasure.m_nLength = s_nMillimetersToMeters * oPointable.length();
  LeapToVistaVector(oMeasure.m_v3TipPosition, oPointable.tipPosition());
  oMeasure.m_nLength = oPointable.length();

  LeapToVistaVector(oMeasure.m_v3StabilizedTipPosition, oPointable.stabilizedTipPosition());
  LeapToVistaVector(oMeasure.m_v3TipVelocity, oPointable.tipVelocity(), false);
  LeapToVistaVector(oMeasure.m_v3Direction, oPointable.direction(), false);
  oMeasure.m_nTouchDistance = s_nMillimetersToMeters * oPointable.touchDistance();
}

void FillFinger(VistaLeapMotionMeasures::Finger& oMeasure, const Leap::Finger& oFinger,
    VistaType::uint64 time_stamp) {
  FillPointable(oMeasure, oFinger, time_stamp);

  oMeasure.m_bExtended   = oFinger.isExtended();
  oMeasure.m_nFingerType = oFinger.type();
  LeapToVistaVector(
      oMeasure.m_v3MetacarpalBoneStartPoint, oFinger.bone(Leap::Bone::TYPE_METACARPAL).prevJoint());
  LeapToVistaVector(
      oMeasure.m_v3ProximalBoneStartPoint, oFinger.bone(Leap::Bone::TYPE_PROXIMAL).prevJoint());
  LeapToVistaVector(oMeasure.m_v3IntermediateBoneStartPoint,
      oFinger.bone(Leap::Bone::TYPE_INTERMEDIATE).prevJoint());
  LeapToVistaVector(
      oMeasure.m_v3DistalBoneStartPoint, oFinger.bone(Leap::Bone::TYPE_DISTAL).prevJoint());
}

void FillHand(VistaLeapMotionMeasures::HandMeasure& oMeasure, const Leap::Hand& oHand,
    VistaType::uint64 time_stamp) {
  oMeasure.m_driver_timestamp = time_stamp;
  oMeasure.m_bVisible         = oHand.isValid();
  oMeasure.m_nId              = oHand.id();
  oMeasure.m_nTimeVisible     = oHand.timeVisible();
  oMeasure.m_bIsLeft          = oHand.isLeft();
  oMeasure.m_bIsRight         = oHand.isRight();

  for (int i = 0; i < 5; ++i)
    oMeasure.m_a5oFingers[i] = VistaLeapMotionMeasures::Finger();
  const Leap::FingerList liFingers = oHand.fingers();
  for (Leap::FingerList::const_iterator itFinger = liFingers.begin(); itFinger != liFingers.end();
       ++itFinger) {
    FillFinger(oMeasure.m_a5oFingers[(*itFinger).type()], (*itFinger), time_stamp);
  }

  for (int i = 0; i < 2; ++i)
    oMeasure.m_a2oTools[i] = VistaLeapMotionMeasures::Tool();
  const Leap::ToolList liTools = oHand.tools();
  int                  nIndex  = 0;
  for (Leap::ToolList::const_iterator itTool = liTools.begin(); itTool != liTools.end();
       ++itTool, ++nIndex) {
    FillPointable(oMeasure.m_a2oTools[nIndex], (*itTool), time_stamp);
  }

  LeapToVistaVector(oMeasure.m_v3PalmPosition, oHand.palmPosition());
  LeapToVistaVector(oMeasure.m_v3StabilizedPalmPosition, oHand.stabilizedPalmPosition());
  LeapToVistaVector(oMeasure.m_v3PalmVelocity, oHand.palmVelocity(), false);
  LeapToVistaVector(oMeasure.m_v3PalmNormal, oHand.palmNormal(), false);
  LeapToVistaVector(oMeasure.m_v3PalmDirection, oHand.direction(), false);
  oMeasure.m_nPalmWidth = s_nMillimetersToMeters * oHand.palmWidth();

  Leap::Arm oArm = oHand.arm();
  LeapToVistaVector(oMeasure.m_v3ElbowPosition, oArm.elbowPosition());
  LeapToVistaVector(oMeasure.m_v3WristPostion, oArm.wristPosition());
  oMeasure.m_nArmWidth = s_nMillimetersToMeters * oArm.width();

  oMeasure.m_nConfidence = oHand.confidence();
}

void FillGesture(VistaLeapMotionMeasures::GestureMeasure& oMeasure, const Leap::Gesture& oGesture,
    VistaType::uint64 time_stamp) {
  oMeasure.m_driver_timestamp = time_stamp;

  oMeasure.m_nId       = oGesture.id();
  oMeasure.m_nDuration = VistaType::microtime(oGesture.durationSeconds());

  switch (oGesture.type()) {
  case Leap::Gesture::TYPE_SWIPE: {
    const Leap::SwipeGesture oCastGesture = oGesture;
    assert(oCastGesture.isValid());
    LeapToVistaVector(oMeasure.m_v3Position, oCastGesture.startPosition());
    LeapToVistaVector(oMeasure.m_v3Direction, oCastGesture.direction(), false);
    Leap::Vector oDistVector = oCastGesture.position() - oCastGesture.startPosition();
    oMeasure.m_nLength       = std::sqrt(oDistVector.dot(oDistVector)) * s_nMillimetersToMeters;
    oMeasure.m_eType         = VistaLeapMotionMeasures::GestureMeasure::TP_SWIPE;
    oMeasure.m_nPerformingPointableId = oCastGesture.pointable().id();
    break;
  }
  case Leap::Gesture::TYPE_CIRCLE: {
    const Leap::CircleGesture oCastGesture = oGesture;
    assert(oCastGesture.isValid());
    LeapToVistaVector(oMeasure.m_v3Position, oCastGesture.center());
    LeapToVistaVector(oMeasure.m_v3Direction, oCastGesture.normal(), false);
    oMeasure.m_nLength                = oCastGesture.radius() * s_nMillimetersToMeters;
    oMeasure.m_eType                  = VistaLeapMotionMeasures::GestureMeasure::TP_CIRCLE;
    oMeasure.m_nPerformingPointableId = oCastGesture.pointable().id();
    break;
  }
  case Leap::Gesture::TYPE_KEY_TAP: {
    const Leap::KeyTapGesture oCastGesture = oGesture;
    assert(oCastGesture.isValid());
    LeapToVistaVector(oMeasure.m_v3Position, oCastGesture.position());
    LeapToVistaVector(oMeasure.m_v3Direction, oCastGesture.direction(), false);
    oMeasure.m_nLength                = 0;
    oMeasure.m_eType                  = VistaLeapMotionMeasures::GestureMeasure::TP_KEY_TAP;
    oMeasure.m_nPerformingPointableId = oCastGesture.pointable().id();
    break;
  }
  case Leap::Gesture::TYPE_SCREEN_TAP: {
    const Leap::ScreenTapGesture oCastGesture = oGesture;
    assert(oCastGesture.isValid());
    LeapToVistaVector(oMeasure.m_v3Position, oCastGesture.position());
    LeapToVistaVector(oMeasure.m_v3Direction, oCastGesture.direction(), false);
    oMeasure.m_nLength                = 0;
    oMeasure.m_eType                  = VistaLeapMotionMeasures::GestureMeasure::TP_SCREEN_TAP;
    oMeasure.m_nPerformingPointableId = oCastGesture.pointable().id();
    break;
  }
  case Leap::Gesture::TYPE_INVALID:
  default:
    break;
  }
}
} // namespace

typedef TVistaDriverEnableAspect<VistaLeapMotionDriver> VistaLeapMotionEnableAspect;

/*============================================================================*/
/* DRIVER                                                                     */
/*============================================================================*/
VistaLeapMotionDriver::VistaLeapMotionDriver(IVistaDriverCreationMethod* pCreationMethod)
    : IVistaDeviceDriver(pCreationMethod)
    , m_pSensors(new VistaDriverSensorMappingAspect(pCreationMethod))
    , m_pListener(NULL)
    , m_pLeapController(NULL)
    , m_bListenerIsRegistered(false)
    , m_nLastFrameId(~0u) {
  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  m_pThreadAspect = new VistaDriverThreadAspect(this, new LeapMotionActivationContext(this));
  RegisterAspect(m_pThreadAspect);

  m_pConfigAspect = new VistaDriverGenericParameterAspect(
      new TParameterCreate<VistaLeapMotionDriver, VistaLeapMotionDriver::Parameters>(this));
  RegisterAspect(m_pConfigAspect);
  m_pParameters = Vista::assert_cast<Parameters*>(m_pConfigAspect->GetParameterContainer());

  // register sensor type STICK measuring 18 values
  m_nHandsSensorId = m_pSensors->GetTypeId("HAND");
  assert(m_nHandsSensorId != ~0u);
  m_nFingerSensorId = m_pSensors->GetTypeId("FINGERS");
  assert(m_nFingerSensorId != ~0u);
  m_nToolsSensorId = m_pSensors->GetTypeId("TOOLS");
  assert(m_nToolsSensorId != ~0u);
  m_nGestureSensorId = m_pSensors->GetTypeId("GESTURE");
  assert(m_nGestureSensorId != ~0u);
  m_nKeyTapGestureSensorId = m_pSensors->GetTypeId("KEYTAP");
  assert(m_nKeyTapGestureSensorId != ~0u);
  m_nScreenTapGestureSensorId = m_pSensors->GetTypeId("SCREENTAP");
  assert(m_nScreenTapGestureSensorId != ~0u);
  m_nSwipeGestureSensorId = m_pSensors->GetTypeId("SWIPE");
  assert(m_nSwipeGestureSensorId != ~0u);
  m_nCircleGestureSensorId = m_pSensors->GetTypeId("CIRCLE");
  assert(m_nCircleGestureSensorId != ~0u);
  m_nImageSensorId = m_pSensors->GetTypeId("IMAGE");
  assert(m_nImageSensorId != ~0u);

  RegisterAspect(m_pSensors);

  m_pListener = new LeapListener(this);

  RegisterAspect(new VistaLeapMotionEnableAspect(this, &VistaLeapMotionDriver::PhysicalEnable));
}

VistaLeapMotionDriver::~VistaLeapMotionDriver() {
  UnregisterAspect(m_pThreadAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pThreadAspect->GetActivationContext();
  delete m_pThreadAspect;

  UnregisterAspect(m_pSensors, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pSensors;

  VistaLeapMotionEnableAspect* enabler =
      GetAspectAs<VistaLeapMotionEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;

  // generically delete all sensors
  DeleteAllSensors();

  delete m_pLeapController;
  delete m_pListener;
  delete m_pParameters;
}

bool VistaLeapMotionDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (m_pLeapController->isConnected() == false)
    return false;

  if (m_pThreadAspect->GetDriverUpdatePrepare())
    m_pThreadAspect->GetDriverUpdatePrepare()->PrePoll();

  const Leap::Frame oLatestFrame   = m_pLeapController->frame();
  VistaType::uint64 nLatestFrameId = oLatestFrame.id();

  if (nLatestFrameId > m_nLastFrameId + 1) {
    VistaType::uint64 nFramesToProcess = nLatestFrameId - m_nLastFrameId - 1;
    for (int nPast = nFramesToProcess; nPast > 0; --nPast) {
      const Leap::Frame oPastFrame = m_pLeapController->frame(nPast);
      if (oPastFrame.isValid() && oPastFrame.id() > m_nLastFrameId)
        ProcessFrame(oLatestFrame, dTs);
    }
  }
  ProcessFrame(oLatestFrame, dTs);
  m_nLastFrameId = nLatestFrameId;

  if (m_pThreadAspect->GetDriverUpdatePrepare())
    m_pThreadAspect->GetDriverUpdatePrepare()->PostPoll();
  return true;
}

void VistaLeapMotionDriver::ProcessFrame(const Leap::Frame& oFrame, VistaType::microtime dTs) {
  // Fill hand sensor
  unsigned int   nNumHands = m_pSensors->GetNumRegisteredSensorsForType(m_nHandsSensorId);
  Leap::HandList oHands    = oFrame.hands();
  for (unsigned int nHand = 0; nHand < nNumHands; ++nHand) {
    int                 nSensorId = m_pSensors->GetSensorId(m_nHandsSensorId, nHand);
    VistaSensorMeasure* pMeasure =
        MeasureStart(nSensorId, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);

    VistaLeapMotionMeasures::HandMeasure* pData =
        pMeasure->getWrite<VistaLeapMotionMeasures::HandMeasure>();
    FillHand(*pData, oHands[nHand], oFrame.timestamp());
    MeasureStop(nSensorId);
  }

  unsigned int nFingersSensorId = m_pSensors->GetSensorId(m_nFingerSensorId, 0);
  if (nFingersSensorId != ~0u) {
    VistaSensorMeasure* pMeasure =
        MeasureStart(nFingersSensorId, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
    VistaLeapMotionMeasures::FingersMeasure* pData =
        pMeasure->getWrite<VistaLeapMotionMeasures::FingersMeasure>();
    Leap::FingerList liFingers = oFrame.fingers();
    int              nCount    = std::max<int>(liFingers.count(), LEAPMEASURES_MAX_FINGERS);
    int              nIndex    = 0;
    for (; nIndex < nCount; ++nIndex)
      FillFinger(pData->m_aFingers[nIndex], liFingers[nIndex], oFrame.timestamp());

    for (; nIndex < LEAPMEASURES_MAX_FINGERS; ++nIndex)
      pData->m_aFingers[nIndex] = VistaLeapMotionMeasures::Finger();

    MeasureStop(nFingersSensorId);
  }

  unsigned int nToolsSensorId = m_pSensors->GetSensorId(m_nToolsSensorId, 0);
  if (nToolsSensorId != ~0u) {
    VistaSensorMeasure* pMeasure =
        MeasureStart(nToolsSensorId, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);

    VistaLeapMotionMeasures::ToolsMeasure* pData =
        pMeasure->getWrite<VistaLeapMotionMeasures::ToolsMeasure>();
    Leap::ToolList liTools = oFrame.tools();
    int            nCount  = std::max<int>(liTools.count(), LEAPMEASURES_MAX_TOOLS);
    int            nIndex  = 0;
    for (; nIndex < nCount; ++nIndex)
      FillPointable(pData->m_aTools[nIndex], liTools[nIndex], oFrame.timestamp());

    for (; nIndex < LEAPMEASURES_MAX_TOOLS; ++nIndex)
      pData->m_aTools[nIndex] = VistaLeapMotionMeasures::Tool();

    MeasureStop(nToolsSensorId);
  }

  Leap::GestureList oList = oFrame.gestures();
  int               count = oList.count();

  for (int i = 0; i < count; ++i) {
    Leap::Gesture oGesture = oList[i];
    ProcessGesture(m_nGestureSensorId, oGesture, dTs, oFrame.timestamp());
    switch (oGesture.type()) {
    case Leap::Gesture::TYPE_SWIPE:
      ProcessGesture(m_nSwipeGestureSensorId, oGesture, dTs, oFrame.timestamp());
      break;
    case Leap::Gesture::TYPE_CIRCLE:
      ProcessGesture(m_nCircleGestureSensorId, oGesture, dTs, oFrame.timestamp());
      break;
    case Leap::Gesture::TYPE_KEY_TAP:
      ProcessGesture(m_nKeyTapGestureSensorId, oGesture, dTs, oFrame.timestamp());
      break;
    case Leap::Gesture::TYPE_SCREEN_TAP:
      ProcessGesture(m_nScreenTapGestureSensorId, oGesture, dTs, oFrame.timestamp());
      break;
    case Leap::Gesture::TYPE_INVALID:
    default:
      break;
    }
  }

  // two conditions to receive images:
  // 1) policy was set
  // 2) image sensor was specified
  if (GetParameters()->GetPolicyFlags() & VistaLeapMotionDriver::Parameters::POLICY_IMAGES) {
    unsigned int image_sensors[2];
    image_sensors[0] = m_pSensors->GetSensorId(m_nImageSensorId, 0);
    image_sensors[1] = m_pSensors->GetSensorId(m_nImageSensorId, 1);

    Leap::ImageList                 images = oFrame.images();
    Leap::ImageList::const_iterator begin  = images.begin();
    Leap::ImageList::const_iterator end    = images.end();

    for (Leap::ImageList::const_iterator cit = begin; cit != end; ++cit) {
      unsigned int sensor_id = image_sensors[(*cit).id()];
      if (sensor_id != VistaDriverSensorMappingAspect::INVALID_ID) {
        VistaSensorMeasure* m =
            MeasureStart(sensor_id, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
        VistaLeapMotionMeasures::ImageMeasure* image =
            m->getWrite<VistaLeapMotionMeasures::ImageMeasure>();

        image->m_driver_timestamp = oFrame.timestamp();
        image->m_nId              = (*cit).id();
        image->m_nTimeVisible     = 1.0f / 120.0f;
        image->m_bVisible         = true;

        image->m_format          = VistaType::uint32((*cit).format());
        image->m_height          = (*cit).width();
        image->m_width           = (*cit).height();
        image->m_bytes_per_pixel = (*cit).bytesPerPixel();
        image->m_ray_offset_x    = (*cit).rayOffsetX();
        image->m_ray_offset_y    = (*cit).rayOffsetY();
        image->m_ray_scale_x     = (*cit).rayScaleX();
        image->m_ray_scale_y     = (*cit).rayScaleY();

        // copy data fields
        VistaType::uint32 data_size = image->m_width * image->m_height * image->m_bytes_per_pixel;
        VistaAutoWriteBuffer ib(data_size);
        memcpy(ib.data(), (*cit).data(), data_size);

        image->m_data = ib;

        MeasureStop(sensor_id);
      } // sensor available
    }   // for each image
  }     // POLICY IMAGES enables
}

void VistaLeapMotionDriver::ProcessGesture(const int nSensorTypeId, const Leap::Gesture& oGesture,
    const VistaType::microtime nTime, VistaType::uint64 driver_time_stamp) {
  unsigned int nSensorId = m_pSensors->GetSensorId(nSensorTypeId, 0);
  if (nSensorId == ~0u)
    return;

  VistaSensorMeasure* pMeasure =
      MeasureStart(nSensorId, nTime, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (pMeasure == 0)
    return;

  FillGesture(
      *pMeasure->getWrite<VistaLeapMotionMeasures::GestureMeasure>(), oGesture, driver_time_stamp);
  MeasureStop(nSensorId);
}

bool VistaLeapMotionDriver::PhysicalEnable(bool bEnable) {
  if (m_pLeapController == 0)
    return (bEnable == false);

  VistaMutexLock g(m_oConnectionGuardMutex);

  bool bRes = false;
  if (bEnable) {
    if (m_bListenerIsRegistered == false) {
      if ((bRes = m_pLeapController->addListener(*m_pListener)) == false) {
        vstr::warnp() << "VistaLeapMotionDriver::PhysicalEnable( true ) -- failed to add listener "
                         "callback to leap controller"
                      << std::endl;
        m_bListenerIsRegistered = false;
      } else
        m_bListenerIsRegistered = true;
    }
  } else {
    if (m_bListenerIsRegistered == true) {
      if ((bRes = m_pLeapController->removeListener(*m_pListener)) == false) {
        vstr::warnp() << "VistaLeapMotionDriver::PhysicalEnable( false ) -- failed to remove "
                         "listener callback from leap controller"
                      << std::endl;
        m_bListenerIsRegistered = true;
      } else {
        m_bListenerIsRegistered = false;
      }
    }
  }
  m_nLastFrameId = ~0u;
  return bRes;
}

bool VistaLeapMotionDriver::DoConnect() {
  if (m_pLeapController == NULL)
    m_pLeapController = new VistaLeapController;

  VistaTimer           t;
  VistaType::microtime time_out = GetParameters()->GetConnectionTimeout();

  while ((m_pLeapController->isServiceConnected() == false) && (t.GetLifeTime() < time_out)) {
    VistaTimeUtils::Sleep(100);
  }

  if (m_pLeapController->isServiceConnected() == false) {
    delete m_pLeapController;
    m_pLeapController = 0;
    return false;
  }

  if (m_pLeapController->isConnected() == false)
    return false;

  m_pLeapController->setPolicyFlags(
      (Leap::Controller::PolicyFlag)GetParameters()->GetPolicyFlags());

  Parameters::GestureFlags g_flags = GetParameters()->GetGestureFlags();

  if (g_flags & Parameters::CIRCLE_ENABLED)
    m_pLeapController->enableGesture(Leap::Gesture::TYPE_CIRCLE);

  if (g_flags & Parameters::SWIPE_ENABLED)
    m_pLeapController->enableGesture(Leap::Gesture::TYPE_SWIPE);

  if (g_flags & Parameters::KEY_TAP_ENABLED)
    m_pLeapController->enableGesture(Leap::Gesture::TYPE_KEY_TAP);

  if (g_flags & Parameters::SCREEN_TAP_ENABLED)
    m_pLeapController->enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);

  GetParameters()->ApplyGestureConfiguration();
  GetParameters()->ApplyPolicyFlags();

  m_nLastFrameId = ~0u;

  return true;
}

bool VistaLeapMotionDriver::DoDisconnect() {
  if (m_pLeapController && m_pLeapController->isConnected()) {
    delete m_pLeapController;
    m_pLeapController = 0;
    m_nLastFrameId    = ~0u;
  }
  return true;
}

VistaLeapMotionDriver::Parameters* VistaLeapMotionDriver::GetParameters() const {
  return m_pParameters;
}

Leap::Controller* VistaLeapMotionDriver::GetLeapController() const {
  return m_pLeapController;
}

// ############################################################################

VistaLeapMotionDriver::Parameters::Parameters(VistaLeapMotionDriver* pDriver)
    : VistaDriverGenericParameterAspect::IParameterContainer()
    , m_parent(pDriver)
    , m_gesture_flags(ALL_ENABLED)
    , m_policy_flags(POLICY_DEFAULT)
    , m_connection_timeout(5) // 5s
    , m_listener_connection_state(false) {
  m_param_cache["Gesture.Circle.MinRadius"]             = 5.0f;
  m_param_cache["Gesture.Circle.MinArc"]                = 1.5f * Vista::Pi;
  m_param_cache["Gesture.Swipe.MinLength"]              = 150.0f;
  m_param_cache["Gesture.Swipe.MinVelocity"]            = 1000.0f;
  m_param_cache["Gesture.KeyTap.MinDownVelocity"]       = 50.0f;
  m_param_cache["Gesture.KeyTap.HistorySeconds"]        = 0.1f;
  m_param_cache["Gesture.KeyTap.MinDistance"]           = 3.0f;
  m_param_cache["Gesture.ScreenTap.MinForwardVelocity"] = 50.0f;
  m_param_cache["Gesture.ScreenTap.HistorySeconds"]     = 0.1f;
  m_param_cache["Gesture.ScreenTap.MinDistance"]        = 5.0f;
}

VistaLeapMotionDriver::Parameters::PolicyFlags
VistaLeapMotionDriver::Parameters::GetPolicyFlags() const {
  return m_policy_flags;
}

bool VistaLeapMotionDriver::Parameters::SetPolicyFlags(PolicyFlags flags) {
  if (compAndAssignFunc<PolicyFlags>(flags, m_policy_flags)) {
    if (m_parent->GetLeapController()) {
      // flags changed
      m_parent->GetLeapController()->setPolicy(Leap::Controller::PolicyFlag(m_policy_flags));
    }
    Notify(MSG_POLICY_FLAGS_CHG);
    return true;
  }
  return false;
}

float VistaLeapMotionDriver::Parameters::GetGestureProp(const std::string& prop_name) const {
  if (m_parent->GetLeapController() == 0) {
    std::map<std::string, float>::const_iterator it = m_param_cache.find(prop_name);
    if (it == m_param_cache.end())
      return -1.0f;
    return (*it).second;
  } else {
    Leap::Config cfg = m_parent->GetLeapController()->config();
    return cfg.getFloat(prop_name);
  }
}

bool VistaLeapMotionDriver::Parameters::SetGestureProp(const std::string& prop_name, float value) {
  if (m_parent->GetLeapController() == 0) {
    std::map<std::string, float>::iterator it = m_param_cache.find(prop_name);
    if (it == m_param_cache.end()) {
      // prop does not exist
      return false;
    }
    return compAssignAndNotify(value, (*it).second, *this, MSG_GESTURE_FLAGS_CHG);
  } else {
    float old_value = GetGestureProp(prop_name);
    if (old_value != value) {
      Leap::Config cfg = m_parent->GetLeapController()->config();
      cfg.setFloat(prop_name, value);
      Notify(MSG_GESTURE_FLAGS_CHG);
      return true;
    }
  }
  return false;
}

VistaLeapMotionDriver::Parameters::GestureFlags
VistaLeapMotionDriver::Parameters::GetGestureFlags() const {
  return m_gesture_flags;
}

bool VistaLeapMotionDriver::Parameters::SetGestureFlags(GestureFlags flags) {
  return compAssignAndNotify<GestureFlags>(flags, m_gesture_flags, *this, MSG_GESTURE_FLAGS_CHG);
}

VistaType::microtime VistaLeapMotionDriver::Parameters::GetConnectionTimeout() const {
  return m_connection_timeout;
}

bool VistaLeapMotionDriver::Parameters::SetConnectionTimeout(VistaType::microtime timout_in_s) {
  return compAssignAndNotify<VistaType::microtime>(
      timout_in_s, m_connection_timeout, *this, MSG_CONNECTION_TIMEOUT_CHG);
}

void VistaLeapMotionDriver::Parameters::ApplyGestureConfiguration() {
  if (m_parent->GetLeapController() == 0)
    return;

  std::map<std::string, float>::const_iterator begin = m_param_cache.begin();
  std::map<std::string, float>::const_iterator end   = m_param_cache.end();

  for (std::map<std::string, float>::const_iterator cit = begin; cit != end; ++cit) {
    SetGestureProp((*cit).first, (*cit).second);
  }
}

void VistaLeapMotionDriver::Parameters::ApplyPolicyFlags() {
  if (m_parent->GetLeapController() == 0)
    return;

  m_parent->GetLeapController()->setPolicy(Leap::Controller::PolicyFlag(m_policy_flags));
}

bool VistaLeapMotionDriver::Parameters::GetListenerConnectionState() const {
  return m_listener_connection_state;
}

bool VistaLeapMotionDriver::Parameters::SetListenerConnectionState(bool connection_state) {
  return compAssignAndNotify<bool>(
      connection_state, m_listener_connection_state, *this, MSG_CONNECTION_STATE_CHG);
}
