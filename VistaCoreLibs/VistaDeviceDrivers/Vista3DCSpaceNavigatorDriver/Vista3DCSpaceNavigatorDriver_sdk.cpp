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

// include header here

#include <windows.h>

#include <si.h>
#include <siapp.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimerImp.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>

#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>

#include "Vista3DCSpaceNavigatorDriver.h"

#include <memory>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace {
const char* g_sLogPrefix = "[SpaceNavigator] ";

std::string Get3DCDevicTypeString(int dev_type) {
  switch (dev_type) {
  case SI_SPACEBALL_2003:
    return "SPACEBALL_2003";
  case SI_SPACEBALL_3003:
    return "SPACEBALL_3003";
  case SI_SPACE_CONTROLLER:
    return "SPACE_CONTROLLER";
  case SI_SPACEEXPLORER:
    return "SPACEEXPLORER";
  case SI_SPACENAVIGATOR_FOR_NOTEBOOKS:
    return "SPACENAVIGATOR_FOR_NOTEBOOKS";
  case SI_SPACENAVIGATOR:
    return "SPACENAVIGATOR";
  case SI_SPACEBALL_2003A:
    return "SPACEBALL_2003A";
  case SI_SPACEBALL_2003B:
    return "SPACEBALL_2003B";
  case SI_SPACEBALL_2003C:
    return "SPACEBALL_2003C";
  case SI_SPACEBALL_3003A:
    return "SPACEBALL_3003A";
  case SI_SPACEBALL_3003B:
    return "SPACEBALL_3003B";
  case SI_SPACEBALL_3003C:
    return "SPACEBALL_3003C";
  case SI_SPACEBALL_4000:
    return "SPACEBALL_4000";
  case SI_SPACEMOUSE_CLASSIC:
    return "SPACEMOUSE_CLASSIC";
  case SI_SPACEMOUSE_PLUS:
    return "SPACEMOUSE_PLUS";
  case SI_SPACEMOUSE_XT:
    return "SPACEMOUSE_XT";
  case SI_CYBERMAN:
    return "CYBERMAN";
  case SI_CADMAN:
    return "CADMAN";
  case SI_SPACEMOUSE_CLASSIC_PROMO:
    return "SPACEMOUSE_CLASSIC_PROMO";
  case SI_SERIAL_CADMAN:
    return "SERIAL_CADMAN";
  case SI_SPACEBALL_5000:
    return "SPACEBALL_5000";
  case SI_TEST_NO_DEVICE:
    return "TEST_NO_DEVICE";
  case SI_3DX_KEYBOARD_BLACK:
    return "3DC_KEYBOARD_BLACK";
  case SI_3DX_KEYBOARD_WHITE:
    return "3DX_KEYBOARD_WHITE";
  case SI_TRAVELER:
    return "TRAVELER";
  case SI_TRAVELER1:
    return "TRAVELER1";
  case SI_SPACEBALL_5000A:
    return "SPACEBALL_5000A";
  case SI_SPACEDRAGON:
    return "SPACEDRAGON";
  case SI_SPACEPILOT:
    return "SPACEPILOT";
  case SI_MB:
    return "MB";
  case SI_SPACEPILOT_PRO:
    return "SPACEPILOT_PRO";
  case SI_SPACEMOUSE_PRO:
    return "SPACEMOUSE_PRO";
  case SI_SPACEMOUSE_TOUCH:
    return "SPACEMOUSE_TOUCH";
  case SI_SPACEMOUSE_WIRELESS:
    return "SPACEMOUSE_WIRELESS";
  case SI_SPACEMOUSE_PRO_WIRELESS:
    return "SPACEMOUSE_PRO_WIRELESS";
  case SI_CADMOUSE:
    return "CADMOUSE";
  case SI_SCOUT:
    return "SCOUT";
  case SI_UNKNOWN_DEVICE:
  default:
    return "ST_UNKNOWN_DEVICE";
  }
}

static LPCSTR WindowClassName = "3dcMessagePort";

//! have to have the static SDK_Initializer, to make sure that init and terminate are only called on
//! load-time and on unload time respectively. This way we can have as many 3dc devices attached /
//! active as we want in the same process / application.
struct SDK_Initializer {
  SDK_Initializer()
      : m_window_class()
      , m_window_instance(NULL)
      , m_class_atom(NULL) {
    SpwRetVal err_code;
    if ((err_code = SiInitialize()) != SPW_NO_ERROR) {
      vstr::errp() << "Error calling SiInitialize() -- " << SpwErrorString(err_code)
                   << " -- 3DC devices will probably not work." << std::endl;
    }

    /* Register message-only window class */
    m_window_class.style         = 0;
    m_window_class.lpfnWndProc   = (WNDPROC)DefWindowProc;
    m_window_class.cbClsExtra    = 8;
    m_window_class.cbWndExtra    = 0;
    m_window_class.hInstance     = m_window_instance;
    m_window_class.hIcon         = NULL;
    m_window_class.hCursor       = NULL;
    m_window_class.hbrBackground = NULL;
    m_window_class.lpszMenuName  = NULL;
    m_window_class.lpszClassName = WindowClassName;

    m_class_atom = RegisterClass(&m_window_class);
  }

  virtual ~SDK_Initializer() {
    UnregisterClass(WindowClassName, m_window_instance);
    SiTerminate();
  }

  WNDCLASS  m_window_class;
  HINSTANCE m_window_instance;
  ATOM      m_class_atom;
};

static SDK_Initializer sdk_initializer;
} // namespace

typedef TVistaDriverEnableAspect<Vista3DCSpaceNavigator> Vista3DCEnableAspect;

struct Vista3DCSpaceNavigator::_sPrivate {
  class ThreadPoolActivationContext : public VistaDriverThreadAspect::IActivationContext {
   public:
    class ActivationTask : public IVistaThreadPoolWorkInstance {
     public:
      ActivationTask(Vista3DCSpaceNavigator::_sPrivate* parent)
          : m_parent(parent) {
      }

     protected:
      virtual void DefinedThreadWork() {
        m_parent->m_message_window = CreateWindowEx(
            0, WindowClassName, "3DCContextWindow", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
        SiOpenWinInit(&m_parent->m_si_open_data, m_parent->m_message_window);
        if ((m_parent->m_si_device_handle = SiOpen("ViSTA", SI_ANY_DEVICE, SI_NO_MASK, SI_EVENT,
                 &m_parent->m_si_open_data)) == SI_NO_HANDLE) {
          vstr::errp() << "Could not open 3dc device. SiOpen returned NULL." << std::endl;
          return;
        }

        SpwRetVal err_code;
        if ((err_code = SiSetUiMode(m_parent->m_si_device_handle, SI_UI_NO_CONTROLS)) !=
            SPW_NO_ERROR) {
          vstr::errp() << "Could not disable UI controls: " << SpwErrorString(err_code)
                       << std::endl;
        }
      }

     private:
      Vista3DCSpaceNavigator::_sPrivate* m_parent;
    };

    class DeactivationTask : public IVistaThreadPoolWorkInstance {
     public:
      DeactivationTask(Vista3DCSpaceNavigator::_sPrivate* parent)
          : m_parent(parent) {
      }

     protected:
      virtual void DefinedThreadWork() {

        if (m_parent->m_si_device_handle != NULL) {
          SiClose(m_parent->m_si_device_handle);
          m_parent->m_si_device_handle = NULL;
        }

        if (m_parent->m_message_window)
          DestroyWindow(m_parent->m_message_window);

        m_parent->m_message_window = 0;
      }

     private:
      Vista3DCSpaceNavigator::_sPrivate* m_parent;
    };

    class ExecutionTask : public IVistaThreadPoolWorkInstance {
     public:
      ExecutionTask(Vista3DCSpaceNavigator::_sPrivate* parent)
          : IVistaThreadPoolWorkInstance()
          , m_prepare(0)
          , m_parent(parent) {
      }

     protected:
      //! blocking call to GetMessage() to receive update messages from, send WM_USER to break loop
      //! (or close of window)
      virtual void DefinedThreadWork() {
        MSG  msg;
        BOOL msg_handled;

        while (GetMessage(&msg, m_parent->m_message_window, 0, 0) >= 0) {
          if (msg.message == WM_USER)
            return; // leave this thread

          msg_handled = SPW_FALSE;

          SiGetEventWinInit(&m_parent->m_si_event_data, msg.message, msg.wParam, msg.lParam);

          if (SiGetEvent(m_parent->m_si_device_handle, SI_AVERAGE_EVENTS,
                  &m_parent->m_si_event_data, &m_parent->m_si_event) == SI_IS_EVENT) {
            if (m_prepare)
              m_prepare->PrePoll();

            if (m_parent->m_parent->Update() == false)
              vstr::errp() << "Error updating 3dc driver." << std::endl;

            if (m_prepare)
              m_prepare->PostPoll();
          }

          if (msg_handled == SPW_FALSE) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
          }
        }
      }

     public:
      Vista3DCSpaceNavigator::_sPrivate*             m_parent;
      VistaDriverThreadAspect::IDriverUpdatePrepare* m_prepare;
    };

    ThreadPoolActivationContext(Vista3DCSpaceNavigator::_sPrivate* parent)
        : VistaDriverThreadAspect::IActivationContext()
        , m_pool(1, 1, "tp_acc_")
        , m_activation_task()
        , m_update_task()
        , m_deactivation_task()
        , m_parent(parent) {
      m_activation_task.reset(new ActivationTask(parent));
      m_activation_task->SetReenqueueTask(false);
      m_activation_task->SetAutoRemoveDoneJob(false);

      m_deactivation_task.reset(new DeactivationTask(parent));
      m_deactivation_task->SetReenqueueTask(false);
      m_deactivation_task->SetAutoRemoveDoneJob(false);

      m_update_task.reset(new ExecutionTask(parent));
      m_update_task->SetReenqueueTask(false);
      m_update_task->SetAutoRemoveDoneJob(true);
    }

    virtual ~ThreadPoolActivationContext() {
    }

    virtual int GetProcessorAffinity() const {
      return m_update_task->GetCpuAffinity();
    }

    virtual bool SetProcessorAffinity(int cpu_affinity) {
      m_update_task->SetCpuAffinity(cpu_affinity);
      return true;
    }

    virtual bool SetPriority(const VistaPriority& prio) {
      m_update_task->SetJobPriority(prio);
      return true;
    }

    virtual VistaPriority GetPriority() const {
      VistaPriority p;
      m_update_task->GetJobPriority(p);
      return p;
    }

    //! @brief activate is called to spawn the thread context
    //! this is a blocking call. The activated context directly executes its update function, i.e.,
    //! no need to UnPause
    //! @returns true when that worked.
    virtual bool Activate() {
      if (m_pool.StartPoolWork()) {
        int activation_job_id = m_pool.AddWork(m_activation_task.get());
        if (m_pool.WaitForAllJobsDone()) {
          m_pool.RemoveDoneJob(activation_job_id);

          VistaPropertyList& props = m_parent->m_info_aspect->GetInfoPropsWrite();
          SpwRetVal          err_code;
          SiDevInfo          dev_info;

          // gather some info
          if ((err_code = SiGetDeviceInfo(m_parent->m_si_device_handle, &dev_info)) == SPW_ERROR) {
            vstr::errp() << "3DC-SDK: Could not get device info" << std::endl;
          } else {
            props.SetValue<bool>("CAN_BEEP", dev_info.canBeep == 0 ? false : true);
            props.SetValue<int>("DEV_TYPE_NUMBER", dev_info.devType);
            props.SetValue<std::string>("DEV_TYPE", Get3DCDevicTypeString(dev_info.devType));
            props.SetValue<std::string>("FIRMWARE", dev_info.firmware);
            props.SetValue<int>("VER_MAJOR", dev_info.majorVersion);
            props.SetValue<int>("VER_MINOR", dev_info.minorVersion);
            props.SetValue<int>("NUM_BUTTONS", dev_info.numButtons);
            props.SetValue<int>("NUM_DEGREES", dev_info.numDegrees);
          }

          SiDeviceName dev_name;
          if ((err_code = SiGetDeviceName(m_parent->m_si_device_handle, &dev_name)) !=
              SPW_NO_ERROR) {
            vstr::errp() << "Could not retrieve the device name" << std::endl;
          }

          props.SetValue<std::string>("DEV_NAME", dev_name.name);

          SiVerInfo drv_info;
          if ((err_code = SiGetDriverInfo(&drv_info)) == SPW_ERROR) {
            vstr::errp() << "3DC-SDK: Could not get driver version..." << std::endl;
          } else {
            props.SetValue<int>("DRV_BUILD", drv_info.build);
            props.SetValue<std::string>("DRV_BUILD_STRING", drv_info.date);
            props.SetValue<int>("DRV_MAJOR", drv_info.major);
            props.SetValue<int>("DRV_MINOR", drv_info.minor);
          }

          SiVerInfo lib_info;
          SiGetLibraryInfo(&lib_info);

          props.SetValue<int>("LIB_BUILD", lib_info.build);
          props.SetValue<std::string>("LIB_BUILD_STRING", lib_info.date);
          props.SetValue<int>("LIB_MAJOR", lib_info.major);
          props.SetValue<int>("LIB_MINOR", lib_info.minor);

          // SiDevID dev_id;
          // if( ( dev_id = SiGetDeviceID( m_parent->m_si_device_handle ) ) != SPW_NO_ERROR )
          //{
          //	vstr::errp() << "Could not retrieve device ID" << std::endl;
          //}

          // props.SetValue<int>("DEV_ID", dev_id );

          // props.Print();

          m_pool.AddWork(m_update_task.get()); // rock'n'roll
          return true;
        }

        return false;
      }

      return false;
    };

    //! @brief reflects, whether a thread context was spawned or not
    virtual bool IsActive() const {
      return m_pool.GetIsPoolRunning();
    }

    //! @brief deactivate eliinates the thread context spawned by Activate()
    //! this is a blocking call
    //! @returns true when that worked
    virtual bool DeActivate() {
      if (m_pool.GetIsPoolRunning()) {
        // m_update_task->SetReenqueueTask( false );
        if (m_update_task->GetIsProcessed())
          PostMessage(m_parent->m_message_window, WM_USER, 0, 0);

        m_pool.WaitForAllJobsDone();

        int deactivation_job_id = m_pool.AddWork(m_deactivation_task.get());
        if (m_pool.WaitForAllJobsDone())
          m_pool.RemoveDoneJob(deactivation_job_id);
        else
          vstr::errp() << "Thread Pool Wait for all jobs done did not succeed." << std::endl;

        return m_pool.StopPoolWork();
      } else
        return true;
    }

    //! @brief pause is used to keep the activated context, but suspend / pause execution
    //! @return true if that worked
    virtual bool Pause() {
      // do not re-enqueue another execution task on next round
      // m_update_task->SetReenqueueTask( false );
      if (m_update_task->GetIsProcessed())
        PostMessage(m_parent->m_message_window, WM_USER, 0, 0);

      return m_pool.WaitForAllJobsDone();
    }

    //! @brief reflects is the activation context is in paused state
    virtual bool IsPaused() const {
      return (m_update_task->GetIsProcessed() == false && m_pool.GetNumberOfJobs() == 0);
    }

    //! @brief gets a paused context back into execution
    virtual bool UnPause() {
      assert(IsPaused());
      m_pool.AddWork(m_update_task.get());
      return true;
    }

    virtual VistaDriverThreadAspect::IDriverUpdatePrepare* GetDriverUpdatePrepare() const {
      return m_update_task->m_prepare;
    }

    virtual void SetDriverUpdatePrepare(VistaDriverThreadAspect::IDriverUpdatePrepare* pUpdate) {
      m_update_task->m_prepare = pUpdate;
    }

   private:
    VistaThreadPool                   m_pool;
    std::unique_ptr<ActivationTask>   m_activation_task;
    std::unique_ptr<ExecutionTask>    m_update_task;
    std::unique_ptr<DeactivationTask> m_deactivation_task;

    Vista3DCSpaceNavigator::_sPrivate* m_parent;
  };

  _sPrivate(Vista3DCSpaceNavigator* parent)
      : m_parent(parent)
      , m_pWorkspace(new VistaDriverWorkspaceAspect)
      , m_thread_aspect()
      , m_activation_context()
      , m_info_aspect(new VistaDriverInfoAspect)
      , m_message_window(NULL)

      , m_si_open_data()
      , m_si_device_handle(NULL) {
    m_activation_context.reset(new ThreadPoolActivationContext(this));
    m_thread_aspect.reset(new VistaDriverThreadAspect(parent, m_activation_context.get()));

    VistaBoundingBox bb;

    bb.m_v3Min[0] = bb.m_v3Min[1] = bb.m_v3Min[2] = -750;
    bb.m_v3Max[0] = bb.m_v3Max[1] = bb.m_v3Max[2] = 750;

    m_pWorkspace->SetWorkspace("KNOB", bb);

    memset(&m_current_state, 0, sizeof(Vista3DCSpaceNavigator::_sMeasure));
    memset(&m_si_event, 0, sizeof(SiSpwEvent));
    memset(&m_si_event_data, 0, sizeof(SiGetEventData));
  }

  ~_sPrivate() {
    if (m_si_device_handle) {
      SpwRetVal err_code;
      if ((err_code = SiClose(m_si_device_handle)) != SPW_NO_ERROR) {
        vstr::errp() << "Could not close device:" << SpwErrorString(err_code) << std::endl;
      }
    }
  }

  Vista3DCSpaceNavigator*                      m_parent;
  std::unique_ptr<VistaDriverWorkspaceAspect>  m_pWorkspace;
  std::unique_ptr<VistaDriverThreadAspect>     m_thread_aspect;
  std::unique_ptr<ThreadPoolActivationContext> m_activation_context;
  std::unique_ptr<VistaDriverInfoAspect>       m_info_aspect;

  HWND m_message_window;

  SiOpenData m_si_open_data;
  SiHdl      m_si_device_handle;

  Vista3DCSpaceNavigator::_sMeasure m_current_state;

  SiSpwEvent     m_si_event;
  SiGetEventData m_si_event_data;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
Vista3DCSpaceNavigator::Vista3DCSpaceNavigator(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pPrivate(NULL) {
  m_pPrivate = new _sPrivate(this);

  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  VistaDeviceSensor* pSensor = new VistaDeviceSensor;
  AddDeviceSensor(pSensor);
  pSensor->SetTypeHint("");
  pSensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder());

  RegisterAspect(m_pPrivate->m_pWorkspace.get());
  RegisterAspect(m_pPrivate->m_thread_aspect.get());
  RegisterAspect(m_pPrivate->m_info_aspect.get());

  RegisterAspect(new Vista3DCEnableAspect(this, &Vista3DCSpaceNavigator::PhysicalEnable));
}

Vista3DCSpaceNavigator::~Vista3DCSpaceNavigator() {
  VistaDeviceSensor*      pSensor = GetSensorByIndex(0);
  IVistaMeasureTranscode* pTr     = pSensor->GetMeasureTranscode();
  pSensor->SetMeasureTranscode(NULL);
  GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(pTr);

  RemDeviceSensor(pSensor);
  delete pSensor;

  UnregisterAspect(m_pPrivate->m_pWorkspace.get(), IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  UnregisterAspect(m_pPrivate->m_thread_aspect.get(), IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  UnregisterAspect(m_pPrivate->m_info_aspect.get(), IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);

  Vista3DCEnableAspect* enabler = GetAspect<Vista3DCEnableAspect>();
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;

  delete m_pPrivate;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool Vista3DCSpaceNavigator::DoConnect() {
  return true;
}

bool Vista3DCSpaceNavigator::DoDisconnect() {
  return true;
}

bool Vista3DCSpaceNavigator::PhysicalEnable(bool enable_flag) {
  if (enable_flag == true) {
    return m_pPrivate->m_thread_aspect->StartProcessing();
  } else {
    return m_pPrivate->m_thread_aspect->StopProcessing();
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

Vista3DCSpaceNavigatorCreateMethod::Vista3DCSpaceNavigatorCreateMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {

  // we assume an update rate of 100Hz at max. Devices we inspected reported
  // an update rate of about 70Hz, which seem reasonable.
  RegisterSensorType(
      "", sizeof(Vista3DCSpaceNavigator::_sMeasure), 100, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* Vista3DCSpaceNavigatorCreateMethod::CreateDriver() {
  return new Vista3DCSpaceNavigator(this);
}

bool Vista3DCSpaceNavigator::DoSensorUpdate(VistaType::microtime dTs) {
  // this method is being called only when the (blocking) GetMessage() call above
  // detected an SiMessage. The co-responding event and event data is stored in the
  // private state prior to calling update.
  switch (m_pPrivate->m_si_event.type) {
  case SI_MOTION_EVENT: {
    m_pPrivate->m_current_state.m_nPositionX = m_pPrivate->m_si_event.u.spwData.mData[SI_TX];
    m_pPrivate->m_current_state.m_nPositionY = m_pPrivate->m_si_event.u.spwData.mData[SI_TY];
    m_pPrivate->m_current_state.m_nPositionZ = -m_pPrivate->m_si_event.u.spwData.mData[SI_TZ];

    m_pPrivate->m_current_state.m_nLength = VistaVector3D(m_pPrivate->m_current_state.m_nPositionX,
        m_pPrivate->m_current_state.m_nPositionY, m_pPrivate->m_current_state.m_nPositionZ)
                                                .GetLength();

    // scale the angles like this:
    // -750 -> -PI/2
    //  750 ->  PI/2
    // -> 90 degrees in both directions
    const float pi_half = Vista::Pi / 2.0f;
    float       rx      = m_pPrivate->m_si_event.u.spwData.mData[SI_RX] * pi_half / 750.0f;
    float       ry      = m_pPrivate->m_si_event.u.spwData.mData[SI_RY] * pi_half / 750.0f;
    float       rz      = m_pPrivate->m_si_event.u.spwData.mData[SI_RZ] * pi_half / 750.0f;

    if (rx < -pi_half)
      rx = -pi_half;
    if (rx > pi_half)
      rx = pi_half;
    if (ry < -pi_half)
      ry = -pi_half;
    if (ry > pi_half)
      ry = pi_half;
    if (rz < -pi_half)
      rz = -pi_half;
    if (rz > pi_half)
      rz = pi_half;

    VistaQuaternion xrot(VistaAxisAndAngle(VistaVector3D(1, 0, 0), rx));
    VistaQuaternion yrot(VistaAxisAndAngle(VistaVector3D(0, 1, 0), ry));
    VistaQuaternion zrot(VistaAxisAndAngle(VistaVector3D(0, 0, 1), rz));

    VistaQuaternion q = xrot * yrot * zrot;
    q.Normalize();
    VistaAxisAndAngle aaa(q.GetAxisAndAngle());

    m_pPrivate->m_current_state.m_nRotationX     = aaa.m_v3Axis[0];
    m_pPrivate->m_current_state.m_nRotationY     = aaa.m_v3Axis[1];
    m_pPrivate->m_current_state.m_nRotationZ     = aaa.m_v3Axis[2];
    m_pPrivate->m_current_state.m_nRotationAngle = aaa.m_fAngle * 180.0f / Vista::Pi;

    break;
  }
  case SI_ZERO_EVENT: {
    m_pPrivate->m_current_state.m_nPositionX     = m_pPrivate->m_current_state.m_nPositionY =
        m_pPrivate->m_current_state.m_nPositionZ = m_pPrivate->m_current_state.m_nLength

        = m_pPrivate->m_current_state.m_nRotationX = m_pPrivate->m_current_state.m_nRotationY =
            m_pPrivate->m_current_state.m_nRotationZ =
                m_pPrivate->m_current_state.m_nRotationAngle = 0;
    break;
  }
  case SI_BUTTON_EVENT: {
    // TBD: dynamic number of buttons
    for (long i = 1; i <= 2; ++i) {
      long mask = 1L << i; // seems the sdk starts with button 1 at bit index 1 (not 0)
      if (mask & m_pPrivate->m_si_event.u.spwData.bData.current)
        m_pPrivate->m_current_state.m_nKeys[i - 1] = 1;
      else
        m_pPrivate->m_current_state.m_nKeys[i - 1] = 0;
    }

    break;
  }
  case SI_BUTTON_PRESS_EVENT: {
    // documented, but not occuring on the space navigator
    break;
  }
  case SI_BUTTON_RELEASE_EVENT: {
    // documented, but not occuring on the space navigator
    break;
  }
  case SI_DEVICE_CHANGE_EVENT:
  case SI_RECONNECT_EVENT: {
    vstr::outi() << "SI_DEVICE_CHANGE / SI_DEVICE_RECONNECT_EVENT" << std::endl;
    break;
  }
  case SI_CMD_EVENT: {
    break;
  }

  default:
    break;
  }

  // there was a change since last time, copy current state to history
  VistaDeviceSensor* p = GetSensorByIndex(0);

  VistaSensorMeasure* m = MeasureStart(*p, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (m == 0)
    return false;

  *m->getWrite<Vista3DCSpaceNavigator::_sMeasure>() = m_pPrivate->m_current_state;

  MeasureStop(*p);
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
