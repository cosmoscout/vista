/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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

#include <openvr/openvr.h>
#include <openvr/openvr_capi.h>

#include "VistaViveDriver.h"
#include "VistaViveDriverConfig.h"
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaBase/VistaStreamUtils.h>

/*VistaViveParameterContainerCreate :
        public VistaDriverGenericParameterAspect::IContainerCreate
{
public:
        IParameterContainer *CreateContainer();
        bool DeleteContainer( IParameterContainer *pContainer );
};
*/
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaViveDriver::VistaViveDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pVRSystem(NULL)
    , m_pThread(NULL) {
  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  VistaDeviceSensor* pStickSensor = new VistaDeviceSensor;
  pStickSensor->SetTypeHint("STICK");
  pStickSensor->SetSensorName("STICK0");
  AddDeviceSensor(pStickSensor);
  pStickSensor->SetMeasureTranscode(
      GetFactory()->GetTranscoderFactoryForSensor("STICK")->CreateTranscoder());

  VistaDeviceSensor* pHeadSensor = new VistaDeviceSensor;
  pHeadSensor->SetTypeHint("HEAD");
  pHeadSensor->SetSensorName("HEAD0");
  AddDeviceSensor(pHeadSensor);
  pHeadSensor->SetMeasureTranscode(
      GetFactory()->GetTranscoderFactoryForSensor("HEAD")->CreateTranscoder());

  m_pThread = new VistaDriverThreadAspect(this);
  RegisterAspect(m_pThread);
}

VistaViveDriver::~VistaViveDriver() {
  UnregisterAspect(m_pThread, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pThread;

  VistaDeviceSensor* pStickSensor = GetSensorByIndex(0);
  VistaDeviceSensor* pHeadSensor  = GetSensorByIndex(1);

  RemDeviceSensor(pStickSensor);
  RemDeviceSensor(pHeadSensor);

  delete pStickSensor;
  delete pHeadSensor;

  delete m_pVRSystem;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaViveDriver::PhysicalEnable(bool bEnabled) {
  // TODO: ?

  return true;
}

bool VistaViveDriver::DoConnect() {
  vstr::debugi() << "Connecting to Vive." << std::endl;

  if (m_pVRSystem)
    return false; // do not connect on state connected

  if (!vr::VR_IsHmdPresent()) {

    vstr::errp() << "Error: No Vive HMD present while connecting VistaViveDriver!\n";
    return false;
  }

  vr::EVRInitError eError = vr::VRInitError_None;
  m_pVRSystem             = vr::VR_Init(&eError, vr::VRApplication_Scene);

  if (eError != vr::VRInitError_None) {
    vstr::errp() << "Error while connecting to Vive API in VistaViveDriver: "
              << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
    return false;
  }

  if (!vr::VRCompositor()) {
    vstr::errp() << "Error: Vive compositor initialization failed in VistaViveDriver!\n";
  }

  return true;
}

bool VistaViveDriver::DoDisconnect() {
  if (m_pVRSystem) {
    vr::VR_Shutdown();

    delete m_pVRSystem;
    m_pVRSystem = 0;
  }
  return true;
}

bool VistaViveDriver::UpdateStickSensor(VistaType::microtime dTs) {
  VistaSensorMeasure* pM = MeasureStart(0, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (pM == NULL)
    return false; // no measure, no fun

  vr::TrackedDevicePose_t devices[vr::k_unMaxTrackedDeviceCount];
  vr::HmdMatrix34_t       pose;
  vr::VRCompositor()->GetLastPoses(devices, vr::k_unMaxTrackedDeviceCount, NULL, 0);
  for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
    if (devices[i].bPoseIsValid) {
      if (m_pVRSystem->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller) {
        pose = devices[i].mDeviceToAbsoluteTracking;
        break;
      }
    }
  }

  VistaTransformMatrix orientation( // column or row major????
      pose.m[0][0], pose.m[1][0], pose.m[2][0], 0.0, pose.m[0][1], pose.m[1][1], pose.m[2][1], 0.0,
      pose.m[0][2], pose.m[1][2], pose.m[2][2], 0.0, pose.m[0][3], pose.m[1][3], pose.m[2][3], 1.0);

  orientation = orientation.GetInverted();

  // write position and orientation of stick:
  VistaViveConfig::VISTA_vive_stick_type* m =
      (*pM).getWrite<VistaViveConfig::VISTA_vive_stick_type>();
  m->orientation = orientation.GetRotationAsQuaternion();
  m->loc[0]      = pose.m[0][3];
  m->loc[1]      = pose.m[1][3];
  m->loc[2]      = pose.m[2][3];

  m->pose = orientation;

  // Button:
  // Process SteamVR controller state
  // TODO: Distinguish left and right hand controllers
  auto trigger_pressed       = false;
  auto trigger_touched       = false;
  auto grip_pressed          = false;
  auto trackpad_pressed      = false;
  auto trackpad_touched      = false;
  auto button_system_pressed = false;
  auto button_menu_pressed   = false;

  for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount;
       unDevice++) {
    vr::VRControllerState_t state;
    if (m_pVRSystem->GetControllerState(unDevice, &state, sizeof(vr::VRControllerState_t))) {
      uint64_t grip_mask = vr::ButtonMaskFromId(static_cast<vr::EVRButtonId>(vr::k_EButton_Grip));

      uint64_t button_sys_mask =
          vr::ButtonMaskFromId(static_cast<vr::EVRButtonId>(vr::k_EButton_System));

      uint64_t button_menu_mask =
          vr::ButtonMaskFromId(static_cast<vr::EVRButtonId>(vr::k_EButton_ApplicationMenu));

      for (int j = 0; j < vr::k_unControllerStateAxisCount; ++j) {
        int32_t axis_type = m_pVRSystem->GetInt32TrackedDeviceProperty(
            unDevice, static_cast<vr::TrackedDeviceProperty>(vr::Prop_Axis0Type_Int32 + j));

        uint64_t button_mask =
            vr::ButtonMaskFromId(static_cast<vr::EVRButtonId>(vr::k_EButton_Axis0 + j));

        switch (axis_type) {

        case vr::k_eControllerAxis_Trigger:

          if ((state.ulButtonPressed & button_mask) != 0) {
            trigger_pressed = true;
          }

          if ((state.ulButtonTouched & button_mask) != 0) {
            trigger_touched = true;
            m->trigger_x    = state.rAxis[j].x;
          }

          if ((state.ulButtonPressed & grip_mask) != 0) {
            grip_pressed = true;
          }

          if ((state.ulButtonPressed & button_sys_mask) != 0) {
            button_system_pressed = true;
          }

          if ((state.ulButtonPressed & button_menu_mask) != 0) {
            button_menu_pressed = true;
          }

          break;

        case vr::k_eControllerAxis_TrackPad:

          if ((state.ulButtonPressed & button_mask) != 0)
            trackpad_pressed = true;

          if ((state.ulButtonTouched & button_mask) != 0) {
            trackpad_touched = true;
            m->trackpad_x    = state.rAxis[j].x;
            m->trackpad_y    = state.rAxis[j].y;
          }
          break;
        }
      }
    }
  }

  m->trigger_pressed       = trigger_pressed;
  m->trigger_touched       = trigger_touched;
  m->grip_pressed          = grip_pressed;
  m->trackpad_pressed      = trackpad_pressed;
  m->trackpad_touched      = trackpad_touched;
  m->button_system_pressed = button_system_pressed;
  m->button_menu_pressed   = button_menu_pressed;

  MeasureStop(0);
  return true;
}

bool VistaViveDriver::UpdateHeadSensor(VistaType::microtime dTs) {
  VistaSensorMeasure* pM = MeasureStart(1, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (pM == NULL)
    return false; // no measure, no fun

  vr::TrackedDevicePose_t devices[vr::k_unMaxTrackedDeviceCount];
  vr::HmdMatrix34_t       pose;
  vr::VRCompositor()->GetLastPoses(devices, vr::k_unMaxTrackedDeviceCount, NULL, 0);
  for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
    if (devices[i].bPoseIsValid) {
      if (m_pVRSystem->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_HMD) {
        pose = devices[i].mDeviceToAbsoluteTracking;
        break;
      }
    }
  }

  VistaTransformMatrix orientation( // column or row major????
      pose.m[0][0], pose.m[1][0], pose.m[2][0], 0.0, pose.m[0][1], pose.m[1][1], pose.m[2][1], 0.0,
      pose.m[0][2], pose.m[1][2], pose.m[2][2], 0.0, pose.m[0][3], pose.m[1][3], pose.m[2][3], 1.0);

  orientation = orientation.GetInverted();

  // write position and orientation of stick:
  VistaViveConfig::VISTA_vive_head_type* m =
      (*pM).getWrite<VistaViveConfig::VISTA_vive_head_type>();
  m->orientation = orientation.GetRotationAsQuaternion();
  m->loc[0]      = pose.m[0][3];
  m->loc[1]      = pose.m[1][3];
  m->loc[2]      = pose.m[2][3];

  m->pose = orientation;

  MeasureStop(1);
  return true;
}

bool VistaViveDriver::DoSensorUpdate(VistaType::microtime dTs) {
  UpdateStickSensor(dTs);
  UpdateHeadSensor(dTs);

  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/
