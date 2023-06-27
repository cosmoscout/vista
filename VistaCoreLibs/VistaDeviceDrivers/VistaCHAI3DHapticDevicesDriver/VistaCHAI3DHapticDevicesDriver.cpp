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

#include "VistaCHAI3DHapticDevicesDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDeviceIdentificationAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaAspects/VistaReferenceCountable.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaInterProcComm/Concurrency/VistaPriority.h>
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>

#include <chai3d.h>

#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>
#include <limits>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* Class just used to cyclicallay call the dirvers update function             */
/*============================================================================*/
class VistaCHAI3DHapticDevicesDriverLoop : public VistaThreadLoop {
 private:
  VistaCHAI3DHapticDevicesDriver* m_pDriver;
  VistaTimer*                     m_pTimer;

 protected:
 public:
  VistaCHAI3DHapticDevicesDriverLoop(VistaCHAI3DHapticDevicesDriver* pDriver) {
    m_pDriver = pDriver;
    m_pTimer  = new VistaTimer();
  };

  virtual ~VistaCHAI3DHapticDevicesDriverLoop() {
    if (m_pTimer)
      delete m_pTimer;
    m_pTimer = NULL;
  };

  bool LoopBody();
};

bool VistaCHAI3DHapticDevicesDriverLoop::LoopBody() {
  m_pDriver->Update();

  VistaTimeUtils::Sleep(1);

  return true;
}

class VistaCHAI3DHapticDevicesDriverProtocolAspect : public IVistaDriverProtocolAspect {
 public:
  VistaCHAI3DHapticDevicesDriverProtocolAspect(VistaCHAI3DHapticDevicesDriver* pDriver)
      : IVistaDriverProtocolAspect()
      , m_pParent(pDriver) {
  }

  virtual bool SetProtocol(const _cVersionTag& oTag) {
    m_pParent->SetDeviceString(oTag.m_strProtocolName);
    return true;
  }

 private:
  VistaCHAI3DHapticDevicesDriver* m_pParent;
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

struct VistaCHAI3DHapticDevicesDriver::_sCHAI3DHapticDevicesPrivate {
  _sCHAI3DHapticDevicesPrivate()
      : m_pHapticDevice(NULL)
      , m_strDeviceString("") {
  }

  cGenericHapticDevice* m_pHapticDevice;

  std::string m_strDeviceString;
};

typedef TVistaDriverEnableAspect<VistaCHAI3DHapticDevicesDriver>
    VistaCHAI3DHapticDeviceEnableAspect;

VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pWorkSpace(new VistaDriverWorkspaceAspect)
    , m_pInfo(new VistaDriverInfoAspect)
    , m_pIdentification(new VistaDeviceIdentificationAspect)
    , m_pProtocol(NULL)
    , m_pHapticDevicePrivate(new _sCHAI3DHapticDevicesPrivate)
    , m_pForceFeedBack(NULL)
    , m_pUpdateLoop(NULL) {

  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  // create and register a sensor for the CHAI3DHapticDevices
  VistaDeviceSensor* pSensor = new VistaDeviceSensor;
  pSensor->SetTypeHint("");

  AddDeviceSensor(pSensor);

  pSensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder());

  m_pProtocol = new VistaCHAI3DHapticDevicesDriverProtocolAspect(this);
  RegisterAspect(m_pProtocol);

  RegisterAspect(m_pWorkSpace);
  RegisterAspect(m_pInfo);
  RegisterAspect(m_pIdentification);
  RegisterAspect(new VistaCHAI3DHapticDeviceEnableAspect(
      this, &VistaCHAI3DHapticDevicesDriver::PhysicalEnable));
}

VistaCHAI3DHapticDevicesDriver::~VistaCHAI3DHapticDevicesDriver() {
  // stop the update Loop Thread
  if (m_pUpdateLoop) {
    m_pUpdateLoop->StopGently(true);
    delete m_pUpdateLoop;
  }

  // close haptic device
  if (m_pHapticDevicePrivate->m_pHapticDevice)
    m_pHapticDevicePrivate->m_pHapticDevice->close();

  VistaDeviceSensor* pSensor = GetSensorByIndex(0);
  RemDeviceSensor(pSensor);
  IVistaMeasureTranscode* pTC = pSensor->GetMeasureTranscode();
  pSensor->SetMeasureTranscode(NULL);
  delete pSensor;
  delete pTC;

  UnregisterAspect(GetAspectById(VistaDriverWorkspaceAspect::GetAspectId()));
  // UnregisterAspect( GetAspectById( IVistaDriverProtocolAspect::GetAspectId() ) );
  UnregisterAspect(GetAspectById(VistaDriverInfoAspect::GetAspectId()));
  UnregisterAspect(GetAspectById(IVistaDriverForceFeedbackAspect::GetAspectId()));
  UnregisterAspect(GetAspectById(VistaDeviceIdentificationAspect::GetAspectId()));

  VistaCHAI3DHapticDeviceEnableAspect* enabler =
      GetAspectAs<VistaCHAI3DHapticDeviceEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;

  delete m_pHapticDevicePrivate;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaCHAI3DHapticDevicesDriver::DoConnect() {
  // create a haptic device handler
  if (!m_pHapticDeviceHandler)
    m_pHapticDeviceHandler = new cHapticDeviceHandler();

  if (!m_pHapticDeviceHandler) {
    std::cerr << "[CHAI3DHapticDevicesDriver]: Could not create CHAI3D HapticDeviceHandler"
              << std::endl;
    return false;
  }

  // read the number of haptic devices currently connected to the computer
  unsigned int numHapticDevices = m_pHapticDeviceHandler->getNumDevices();
  if (numHapticDevices < 1) {
    std::cerr << "[CHAI3DHapticDevicesDriver]: Does not recognize any haptic device" << std::endl;
    return false;
  }

  // get the by the user defined device name
  m_pHapticDevicePrivate->m_strDeviceString = m_pIdentification->GetDeviceName();

  // get a handle to the haptic device with this device name
  // if this is not successful, try to take the first available device
  for (int i = 0; i < (int)numHapticDevices; i++) {
    cHapticDeviceInfo a_deviceSpecification;
    m_pHapticDeviceHandler->getDeviceSpecifications(a_deviceSpecification, i);
    if ((m_pHapticDevicePrivate->m_strDeviceString).compare(a_deviceSpecification.m_modelName) ==
        0) {
      m_pHapticDeviceHandler->getDevice(m_pHapticDevicePrivate->m_pHapticDevice, i);
      break;
    }
  }
  if (!(m_pHapticDevicePrivate->m_pHapticDevice)) {
    std::cerr
        << "[CHAI3DHapticDevicesDriver]: Can't get a handle to the haptic device with the name: "
        << m_pHapticDevicePrivate->m_strDeviceString << std::endl;
    std::cerr << "[CHAI3DHapticDevicesDriver]: The names of the available devices are: ";
    for (unsigned int i = 0; i < numHapticDevices; ++i) {
      cHapticDeviceInfo a_deviceSpecification;
      m_pHapticDeviceHandler->getDeviceSpecifications(a_deviceSpecification, i);
      std::cerr << a_deviceSpecification.m_modelName << ", ";
    }
    std::cerr << std::endl;

    // get a handle to the first haptic device
    m_pHapticDeviceHandler->getDevice(m_pHapticDevicePrivate->m_pHapticDevice, 0);
    if (!(m_pHapticDevicePrivate->m_pHapticDevice)) {
      std::cerr << "[CHAI3DHapticDevicesDriver]: Also can't get a handle to the first haptic device"
                << std::endl;
      return false;
    } else {
      cHapticDeviceInfo a_deviceSpecification;
      m_pHapticDeviceHandler->getDeviceSpecifications(a_deviceSpecification, 0);
      m_pHapticDevicePrivate->m_strDeviceString = a_deviceSpecification.m_modelName;
      std::cerr << "[CHAI3DHapticDevicesDriver]: Took the first device that came along: "
                << m_pHapticDevicePrivate->m_strDeviceString << std::endl;
    }
  }

  // open connection to haptic device (0 indicates success).
  if (m_pHapticDevicePrivate->m_pHapticDevice->open() != 0) {
    std::cerr << "[CHAI3DHapticDevicesDriver]: Can't open connection to the haptic device"
              << std::endl;
    return false;
  }

  // initialize haptic device
  if (m_pHapticDevicePrivate->m_pHapticDevice->initialize() != 0) {
    std::cerr << "[CHAI3DHapticDevicesDriver]: Can't initialize the haptic device" << std::endl;
    return false;
  }

  VistaPropertyList& oInfo = m_pInfo->GetInfoPropsWrite();

  // retrieve information about the current haptic device
  cHapticDeviceInfo info = m_pHapticDevicePrivate->m_pHapticDevice->getSpecifications();

  /////////////////////////////////////////
  // push the device info into a proplist
  // as we want to provide the same "interface" as the phantom we also create
  // props which are not supported by the CHAI3D API

  // supported properties
  oInfo.SetValue("VENDOR",
      std::string((!(info.m_manufacturerName.empty()) ? info.m_manufacturerName : "<none>")));
  oInfo.SetValue("MODEL", std::string((!(info.m_modelName.empty()) ? info.m_modelName : "<none>")));
  oInfo.SetValue("MAXSTIFFNESS", double(info.m_maxForceStiffness));
  oInfo.SetValue("MAXFORCE", double(info.m_maxForce));

  // unsupported properties
  oInfo.SetValue<double>("FIRMWARE_REVISION", 0);
  oInfo.SetValue("CALSTYLES", std::string("<none>"));
  oInfo.SetValue<int>("CALFLAGS", 0);
  oInfo.SetValue<double>("TABLETOP_OFFSET", 0);
  oInfo.SetValue("DRIVERVERSION", std::string("<none>"));
  oInfo.SetValue("SERIALNUMBER", std::string("<none>"));

  // not directly supported
  float min[] = {0, 0, 0};
  // todo this hard coded values are inspired by phantom
  float max[] = {0.001, 1, 100};
  m_pWorkSpace->SetWorkspace("MAXWORKSPACE", VistaBoundingBox(min, max));
  m_pWorkSpace->SetWorkspace("USABLEWORKSPACE", VistaBoundingBox(min, max));
  ///////////////////////////////////////////

  // check how many degrees of freedom the current device supports in in and out direction
  // we assume here that each device will have at least 3 degrees of threedom in in and out
  // direction additionally we neglect devices with a
  int nInputDOF, nOutputDOF;
  nInputDOF  = info.m_sensedRotation ? 6 : 3;
  nOutputDOF = info.m_actuatedRotation ? 6 : 3;

  // setup the force feedback aspect which handles force rendering
  m_pForceFeedBack                  = new VistaCHAI3DHapticDevicesForceFeedbackAspect(this);
  m_pForceFeedBack->m_nInputDOF     = nInputDOF;
  m_pForceFeedBack->m_nOutputDOF    = nOutputDOF;
  m_pForceFeedBack->m_nMaxStiffness = info.m_maxForceStiffness;
  m_pForceFeedBack->m_nMaxForce     = info.m_maxForce;
  RegisterAspect(m_pForceFeedBack);

  // create the thread which runs a driver update loop which collects the data from the CHAI3D
  // device
  m_pUpdateLoop = new VistaCHAI3DHapticDevicesDriverLoop(this);
  m_pUpdateLoop->SetPriority(VistaPriority::VISTA_MAX_PRIORITY);
  m_pUpdateLoop->Run();

  return true;
}

bool VistaCHAI3DHapticDevicesDriver::DoDisconnect() {
  m_pUpdateLoop->StopGently(true);

  // close haptic device
  if (m_pHapticDevicePrivate->m_pHapticDevice)
    m_pHapticDevicePrivate->m_pHapticDevice->close();

  m_pHapticDevicePrivate->m_pHapticDevice = 0;

  UnregisterAspect(m_pForceFeedBack);
  m_pForceFeedBack = 0;

  VistaDeviceSensor* pSensor = GetSensorByIndex(0);
  RemDeviceSensor(pSensor);
  pSensor->SetMeasureTranscode(NULL);
  GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());

  delete pSensor;

  return true;
}

namespace {
inline float alignDoubleToFloatMinMax(double val) {
#undef min
#undef max

  float back = (float)val;
  if (std::numeric_limits<float>::min() < back)
    return std::numeric_limits<float>::min();
  else if (std::numeric_limits<float>::max() > back)
    return std::numeric_limits<float>::max();
  else
    return back;
}
} // namespace

bool VistaCHAI3DHapticDevicesDriver::DoSensorUpdate(VistaType::microtime nTs) {
  VistaDeviceSensor* pSensor = GetSensorByIndex(0);

  MeasureStart(0, nTs);
  // get the current place for the decoding for sensor 0
  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

  // for the sake of readability: interpret the memory blob
  // as a place to store a sample for the CHAI3DHapticDevices
  VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure* s =
      reinterpret_cast<VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*>(
          &(*pM).m_vecMeasures[0]);

  // read position of haptic device and push into history memory blob
  cVector3d newPosition;
  m_pHapticDevicePrivate->m_pHapticDevice->getPosition(newPosition);
  s->m_afPosition[0] = (float)(newPosition.y);
  s->m_afPosition[1] = (float)(newPosition.z);
  s->m_afPosition[2] = (float)(newPosition.x);

  // read orientation of haptic device
  cMatrix3d newOrientation;
  m_pHapticDevicePrivate->m_pHapticDevice->getRotation(newOrientation);
  // fill into a vista transformation matrix under consideration of different coordinate szstem of
  // chai3D
  s->m_afRotMatrix[0] = newOrientation[1][1];
  s->m_afRotMatrix[1] = newOrientation[1][2];
  s->m_afRotMatrix[2] = newOrientation[1][0];
  s->m_afRotMatrix[3] = 0;

  s->m_afRotMatrix[4] = newOrientation[2][1];
  s->m_afRotMatrix[5] = newOrientation[2][2];
  s->m_afRotMatrix[6] = newOrientation[2][0];
  s->m_afRotMatrix[7] = 0;

  s->m_afRotMatrix[8]  = newOrientation[0][1];
  s->m_afRotMatrix[9]  = newOrientation[0][2];
  s->m_afRotMatrix[10] = newOrientation[0][0];
  s->m_afRotMatrix[11] = 0;

  // up to now we only got the rotation, but m_afRotMatrix should contain the full transformation
  // matrix
  s->m_afRotMatrix[12] = s->m_afPosition[0];
  s->m_afRotMatrix[13] = s->m_afPosition[1];
  s->m_afRotMatrix[14] = s->m_afPosition[2];
  s->m_afRotMatrix[15] = 1;

  // in some cases the values delivered by the CHAI3D device are extremely large, e.g.
  // when using the virtual device and setting the position manually, the velocity gets very high
  // in those cases we have to align the values to the maximum/minimum of the float range
  // as they would be set to infinity

  // read position of haptic device, align to bounds and push into history memory blob
  if (m_pIdentification->GetDeviceName() != "virtual") {
    cVector3d linearVelocity;
    m_pHapticDevicePrivate->m_pHapticDevice->getLinearVelocity(linearVelocity);
    s->m_afVelocity[0] = (float)(linearVelocity.x);
    s->m_afVelocity[1] = (float)(linearVelocity.y);
    s->m_afVelocity[2] = (float)(linearVelocity.z);
  } else {
    s->m_afVelocity[0] = 0.0f;
    s->m_afVelocity[1] = 0.0f;
    s->m_afVelocity[2] = 0.0f;
  }

  // read position of haptic device, align to bounds and push into history memory blob
  cVector3d newAngularVelocity;
  m_pHapticDevicePrivate->m_pHapticDevice->getAngularVelocity(newAngularVelocity);
  s->m_afAngularVelocity[0] = alignDoubleToFloatMinMax(newAngularVelocity.x);
  s->m_afAngularVelocity[1] = alignDoubleToFloatMinMax(newAngularVelocity.y);
  s->m_afAngularVelocity[2] = alignDoubleToFloatMinMax(newAngularVelocity.z);

  // iterate over the switches until no switch is left
  // set the according bit in bit field representing the button states
  int  bitmask   = 1;
  int  switchIdx = 0;
  bool switchStatus;
  while (m_pHapticDevicePrivate->m_pHapticDevice->getUserSwitch(switchIdx, switchStatus)) {
    s->m_nButtonState |= (bitmask & bitmask);
    bitmask <<= 1;
    switchStatus = !switchStatus;
  }

  // Not Supported by CHAI3D but still here to remind you that they exist in the phantom driver
  /*
  hdGetFloatv(HD_MOTOR_TEMPERATURE,          s->m_afOverheatState);
  hdGetFloatv(HD_INSTANTANEOUS_UPDATE_RATE, &s->m_nUpdateRate);
  hdGetFloatv(HD_CURRENT_JOINT_ANGLES,       s->m_afJointAngles);
  hdGetFloatv(HD_CURRENT_GIMBAL_ANGLES,      s->m_afGimbalAngles);
  hdGetLongv(HD_CURRENT_ENCODER_VALUES,      s->m_nEncoderValues);
  HDboolean bSwitch = 0;
  hdGetBooleanv(HD_CURRENT_INKWELL_SWITCH,  &bSwitch);
  s->m_bInkwellSwitch = bSwitch ? true : false;
  */

  // force rendering
  // if enabled it gets the current force rendering algorithm from the force feedback aspect and
  // lets it calculate a force respectively a torque and pushes them to the device
  if (m_pForceFeedBack->GetForcesEnabled()) {
    VistaVector3D v3Force;
    VistaVector3D v3Torque;

    IVistaDriverForceFeedbackAspect::IForceAlgorithm* pForceAlgorithm =
        m_pForceFeedBack->GetForceAlgorithm();
    if (pForceAlgorithm) {
      VistaVector3D   pos(s->m_afPosition[0], s->m_afPosition[1], s->m_afPosition[2]);
      VistaVector3D   vel(s->m_afVelocity[0], s->m_afVelocity[1], s->m_afVelocity[2]);
      VistaQuaternion qAngVel(
          VistaVector3D(0, 0, 1), VistaVector3D(s->m_afAngularVelocity[0],
                                      s->m_afAngularVelocity[1], s->m_afAngularVelocity[2]));

      VistaTransformMatrix t(float(s->m_afRotMatrix[0]), float(s->m_afRotMatrix[1]),
          float(s->m_afRotMatrix[2]), 0, float(s->m_afRotMatrix[4]), float(s->m_afRotMatrix[5]),
          float(s->m_afRotMatrix[6]), 0, float(s->m_afRotMatrix[8]), float(s->m_afRotMatrix[9]),
          float(s->m_afRotMatrix[10]), 0, 0, 0, 0, 1);

      VistaQuaternion qQuat = -VistaQuaternion(t);

      pForceAlgorithm->UpdateForce(double(nTs), pos, vel, qQuat, v3Force, v3Torque);
    } else {
      v3Force = m_pForceFeedBack->m_v3Force;
    }

    if (m_pForceFeedBack->m_nOutputDOF >= 3) {
      cVector3d force((double)v3Force[2], (double)v3Force[0], (double)v3Force[1]);
      m_pHapticDevicePrivate->m_pHapticDevice->setForce(force);
    }

    if (m_pForceFeedBack->m_nOutputDOF == 6) {
      cVector3d torque((double)v3Torque[0], (double)v3Torque[1], (double)v3Torque[2]);
      m_pHapticDevicePrivate->m_pHapticDevice->setTorque(torque);
    }
  }

  // read currently applied force from the haptic device and push into history memory blob
  cVector3d newForce;
  m_pHapticDevicePrivate->m_pHapticDevice->getForce(newForce);
  s->m_afForce[1] = (float)(newForce.x);
  s->m_afForce[2] = (float)(newForce.y);
  s->m_afForce[0] = (float)(newForce.y);

  // we are done. Indicate that to the history
  MeasureStop(0);
  pSensor->SetUpdateTimeStamp(nTs);

  return true;
}

bool VistaCHAI3DHapticDevicesDriver::PhysicalEnable(bool bEnable) {
  if (bEnable) {
    if (m_pUpdateLoop->IsPausing())
      m_pUpdateLoop->UnpauseThread();
    else if (!(m_pUpdateLoop->IsRunning()))
      m_pUpdateLoop->Run();
    return true;
  } else {
    if (m_pUpdateLoop->IsRunning())
      m_pUpdateLoop->StopGently(true);
    return true;
  }
}

std::string VistaCHAI3DHapticDevicesDriver::GetDeviceString() const {
  return m_pHapticDevicePrivate->m_strDeviceString;
}

void VistaCHAI3DHapticDevicesDriver::SetDeviceString(const std::string& strDevice) {
  m_pHapticDevicePrivate->m_strDeviceString = strDevice;
}

// #############################################################################

VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::
    VistaCHAI3DHapticDevicesForceFeedbackAspect(VistaCHAI3DHapticDevicesDriver* pDriver)
    : IVistaDriverForceFeedbackAspect()
    , m_pParent(pDriver)
    , m_nInputDOF(0)
    , m_nOutputDOF(0)
    , m_nMaxForce(0)
    , m_nMaxStiffness(0)
    , m_bEnabled(true) {
}

VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::
    ~VistaCHAI3DHapticDevicesForceFeedbackAspect() {
}

bool VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::SetForce(
    const VistaVector3D& v3Force, const VistaVector3D& v3AngularForce) {
  // check thread safety here!
  m_v3Force        = v3Force;
  m_v3AngularForce = v3AngularForce;
  return true;
}

bool VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::SetForcesEnabled(
    bool bEnabled) {
  m_bEnabled = bEnabled;
  return true;
}

bool VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::GetForcesEnabled()
    const {
  return m_bEnabled;
}

int VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::GetNumInputDOF()
    const {
  return m_nInputDOF;
}

int VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::GetNumOutputDOF()
    const {
  return m_nOutputDOF;
}

float VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::
    GetMaximumStiffness() const {
  return m_nMaxStiffness;
}

float VistaCHAI3DHapticDevicesDriver::VistaCHAI3DHapticDevicesForceFeedbackAspect::GetMaximumForce()
    const {
  return m_nMaxForce;
}