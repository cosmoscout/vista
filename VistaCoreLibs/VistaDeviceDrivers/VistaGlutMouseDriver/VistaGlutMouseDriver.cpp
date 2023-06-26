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

#include "VistaGlutMouseDriver.h"
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <algorithm>
#include <functional>
#include <map>

#ifdef WIN32
// disable warnings from unreferenced functions in glut.h
// sadly, C4505 cannot be disabled locally with push/pop
#pragma warning(disable : 4505)
#endif
#if defined(USE_NATIVE_GLUT)
#if defined(DARWIN) // we use the mac os GLUT framework on darwin
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#else
#include <GL/freeglut.h>
#endif
#include "VistaDeviceDriversBase/Drivers/VistaMouseDriver.h"

#if !defined(GLUT_WHEEL_UP)
#define GLUT_WHEEL_UP 3
#endif

#if !defined(GLUT_WHEEL_DOWN)
#define GLUT_WHEEL_DOWN 4
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace {
class UVistaMouseMap {
 public:
  typedef std::map<int, VistaGlutMouseDriver*> DEVMAP;
  DEVMAP                                       m_mapDevices;

  VistaGlutMouseDriver* RetrieveMouseFromWindowId(int nWindow) {
    DEVMAP::const_iterator cit = m_mapDevices.find(nWindow);
    if (cit == m_mapDevices.end())
      return NULL; // ?

    return (*cit).second;
  }

  bool RegisterMouseWithWindow(int nWindowId, VistaGlutMouseDriver* pAddDriver) {
    VistaGlutMouseDriver* pDriver = RetrieveMouseFromWindowId(nWindowId);
    if (pDriver)
      return (pDriver == pAddDriver);

    m_mapDevices[nWindowId] = pAddDriver;

    return true;
  }

  bool UnregisterMouseFromWindow(int nWindowId, VistaGlutMouseDriver* pAddDriver) {
    DEVMAP::iterator it = m_mapDevices.find(nWindowId);
    if (it == m_mapDevices.end())
      return true;

    if ((*it).second != pAddDriver)
      return false;

    m_mapDevices.erase(it);

    return true;
  }
};

UVistaMouseMap S_mapMouseMap;

// track state to determine funtion that updates mouse wheel info
// freeglut should always use the wheel func, but not all implementations seems to
// adhere to this, so the first variant to see a wheel update takes responsibility
// the other method will ignore wheel changes (to avoid potential double events)
enum WheelUpdateMode {
  WUM_UNKNOWN,
  WUM_WHEEL_FUNC,
  WUM_MOUSE_FUNC,
};
WheelUpdateMode S_eWheelUpdateMode = WUM_UNKNOWN;
} // namespace

void VistaGlutMouseDriver::MouseFunction(int iButton, int iState, int iX, int iY) {
  int nWindow = glutGetWindow();
  if (nWindow == 0)
    return; // ?

  VistaGlutMouseDriver* pMouse = S_mapMouseMap.RetrieveMouseFromWindowId(nWindow);
  if (!pMouse)
    return;

  int nWheelNumber   = -1;
  int nWheelDirState = pMouse->m_nWheelDirState;

  if ((iButton >= GLUT_LEFT_BUTTON) && (iButton <= GLUT_RIGHT_BUTTON)) {
    // only write 'valid' LEFT|MIDDLE|RIGHT (0|1|2) values as a current state
    pMouse->m_nButtonStates[iButton] = 1 - iState;
  } else if ((iButton == GLUT_WHEEL_UP || iButton == GLUT_WHEEL_DOWN) && iState == 1) {
    if (S_eWheelUpdateMode == WUM_UNKNOWN)
      S_eWheelUpdateMode = WUM_MOUSE_FUNC;
    if (S_eWheelUpdateMode == WUM_MOUSE_FUNC) {
      // glut on linux reports mouse wheel (by default) as button 3 and 4
      // existing glut patches for windows have their own defines.
      if (iButton == GLUT_WHEEL_UP) {
        nWheelDirState = 1;
        nWheelNumber   = 0;
      } else if (iButton == GLUT_WHEEL_DOWN) {
        nWheelDirState = -1;
        nWheelNumber   = 0;
      }
    }
  }

  // save current state of buttons for eventually
  // incoming MotionFunctions, otherwise, the button press
  // or release might be lost.
  _state s(nWindow, iX, iY, pMouse->m_nButtonStates[0], pMouse->m_nButtonStates[1],
      pMouse->m_nButtonStates[2], nWheelNumber, nWheelDirState);
  pMouse->Receive(s);
}

void VistaGlutMouseDriver::MotionFunction(int iX, int iY) {
  int nWindow = glutGetWindow();
  if (nWindow == 0)
    return; // ?

  VistaGlutMouseDriver* pMouse = S_mapMouseMap.RetrieveMouseFromWindowId(nWindow);
  if (!pMouse)
    return;

  if (pMouse->GetParameters()->GetCaptureCursor()) {
    if (pMouse->GetMouseWarpPending()) {
      pMouse->SetMouseWarpPending(false);
      return;
    }

    int nCenterX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    int nCenterY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

    int nDeltaX = iX - nCenterX;
    int nDeltaY = iY - nCenterY;

    if (nDeltaX != 0 || nDeltaY != 0) {
      pMouse->SetMouseWarpPending(true);
      glutWarpPointer(nCenterX, nCenterY);
    }

    if (pMouse->GetGrabCursorChanged()) {
      pMouse->SetGrabCursorChanged(false);
      return; // skip first frame after change, since we have to center the cursor first
    }

    _state s(nWindow, nDeltaX, nDeltaY, pMouse->m_nButtonStates[0], pMouse->m_nButtonStates[1],
        pMouse->m_nButtonStates[2], -1, pMouse->m_nWheelState);
    pMouse->Receive(s);

  } else {
    _state s(nWindow, iX, iY, pMouse->m_nButtonStates[0], pMouse->m_nButtonStates[1],
        pMouse->m_nButtonStates[2], -1, pMouse->m_nWheelState);
    pMouse->Receive(s);
  }
}

void VistaGlutMouseDriver::MouseWheelFunction(int nWheelNumber, int nDirection, int nX, int nY) {
  int nWindow = glutGetWindow();
  if (nWindow == 0)
    return; // ?

  VistaGlutMouseDriver* pMouse = S_mapMouseMap.RetrieveMouseFromWindowId(nWindow);
  if (!pMouse)
    return;

  if (S_eWheelUpdateMode == WUM_UNKNOWN)
    S_eWheelUpdateMode = WUM_WHEEL_FUNC;
  if (S_eWheelUpdateMode == WUM_WHEEL_FUNC) {
    _state s(nWindow, nX, nY, pMouse->m_nButtonStates[0], pMouse->m_nButtonStates[1],
        pMouse->m_nButtonStates[2], nWheelNumber, nDirection);
    pMouse->Receive(s);
  }
}

// #############################################################################
VistaGlutMouseDriverCreationMethod::VistaGlutMouseDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType(
      "", sizeof(IVistaMouseDriver::_sMouseMeasure), 100, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaGlutMouseDriverCreationMethod::CreateDriver() {
  return new VistaGlutMouseDriver(this);
}

// #############################################################################
class VistaGlutMouseTouchSequence
    : public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence {

 public:
  VistaGlutMouseTouchSequence(VistaGlutMouseDriver* pDriver)
      : VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence()
      , m_pMouseDriver(pDriver) {
  }

  bool AttachSequence(VistaDriverAbstractWindowAspect::IWindowHandle* oHandle) {
    VistaDriverAbstractWindowAspect::NativeWindowHandle* oWindow =
        dynamic_cast<VistaDriverAbstractWindowAspect::NativeWindowHandle*>(oHandle);
    if (oWindow == 0)
      return false;

    // check whether this driver is already registered with the window
    WINMAP::const_iterator cit = m_mapWindows.find(oWindow);
    if (cit != m_mapWindows.end()) {
      vstr::warnp() << "[GlutMouseDriver]: Trying to attach "
                    << "to Window which is already registered for a mouse!" << std::endl;
      return false;
    }

    int nWindowId = oWindow->GetID();
    if (nWindowId <= 0)
      return false;

    // ok, register with the window is in the statics
    // section
    S_mapMouseMap.RegisterMouseWithWindow(nWindowId, m_pMouseDriver);

    int nCurWindow = glutGetWindow();

    // create a sensor for this window
    unsigned int nIdx = nWindowId - 1; // internal knowledge: glut window id start with count '1'
    VistaDeviceSensor* pSensor = m_pMouseDriver->GetSensorByIndex(nIdx);

    if (pSensor == 0) {
      pSensor = new VistaDeviceSensor();
      m_pMouseDriver->AddDeviceSensor(pSensor, nIdx);
    }

    // create new entry in the sensor map to attach the
    // sensor id to the glut window id
    m_mapSensor[nWindowId] = _sSensorState(nIdx);

    // register this window with the instance variable
    m_mapWindows[oWindow] = nWindowId;
    glutSetWindow(nWindowId);

    // it should be ok to register these functions more
    // than once. The interaction stuff from the implementation
    // layer _has_ to be initialized *after* the display stuff anyways
    // and we always set the same function points. Sad that there is
    // no way to actually get the current value of the callbacks in glut.
    // note that, however, these functions are not unregistered, as there
    // may be more than one mouse.
    glutMouseFunc(&VistaGlutMouseDriver::MouseFunction);
    glutMotionFunc(&VistaGlutMouseDriver::MotionFunction);
    glutPassiveMotionFunc(&VistaGlutMouseDriver::MotionFunction);
#if !defined(USE_NATIVE_GLUT)
    glutMouseWheelFunc(&VistaGlutMouseDriver::MouseWheelFunction);
#endif
    glutSetWindow(nCurWindow); // reset old window
    return true;
  }

  bool DetachSequence(VistaDriverAbstractWindowAspect::IWindowHandle* oHandle) {
    VistaDriverAbstractWindowAspect::NativeWindowHandle* oWindow =
        dynamic_cast<VistaDriverAbstractWindowAspect::NativeWindowHandle*>(oHandle);
    if (oWindow == 0)
      return false;

    WINMAP::iterator cit = m_mapWindows.find(oWindow);
    if (cit != m_mapWindows.end()) {
      SENMAP::iterator it = m_mapSensor.find((*cit).second);
      if (it != m_mapSensor.end()) {
        unsigned int nSensorIdx = (*it).second.m_nIndex;
        // remove sensor from driver
        VistaDeviceSensor* pSensor = m_pMouseDriver->GetSensorByIndex(nSensorIdx);
        m_pMouseDriver->RemDeviceSensor(pSensor);
        m_pMouseDriver->GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(
            pSensor->GetMeasureTranscode());
        delete pSensor;

        m_mapSensor.erase(it);
      }

      // erase from window map

      m_mapWindows.erase(cit);

      S_mapMouseMap.UnregisterMouseFromWindow(oWindow->GetID(), m_pMouseDriver);
      return true;
    }
    return false;
  }

  typedef std::map<VistaDriverAbstractWindowAspect::IWindowHandle*, int> WINMAP;

  class _copyIn : public std::unary_function<const WINMAP::value_type&, void> {
   public:
    _copyIn(std::list<VistaDriverAbstractWindowAspect::IWindowHandle*>& list)
        : m_list(list) {
    }

    std::list<VistaDriverAbstractWindowAspect::IWindowHandle*>& m_list;

    void operator()(const WINMAP::value_type& p) {
      m_list.push_back(p.first);
    }
  };

  virtual std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> GetWindowList() const {
    std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> list;
    std::for_each(m_mapWindows.begin(), m_mapWindows.end(), _copyIn(list));
    return list;
  }

  VistaGlutMouseDriver* m_pMouseDriver;

  struct _sSensorState {
    _sSensorState()
        : m_nIndex(~0)
        , m_nWheelState(0)
        , m_nWheelDir(0) {
    }

    _sSensorState(unsigned int nIdx)
        : m_nIndex(nIdx)
        , m_nWheelState(0)
        , m_nWheelDir(0) {
    }

    unsigned int m_nIndex;
    int          m_nWheelState, m_nWheelDir;
  };

  typedef std::map<int, _sSensorState> SENMAP;
  WINMAP                               m_mapWindows;
  SENMAP                               m_mapSensor;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutMouseDriver::VistaGlutMouseDriver(IVistaDriverCreationMethod* crm)
    : IVistaMouseDriver(crm)
    , m_nWheelState(0)
    , m_nWheelDirState(0)
    , m_pWindowAspect(new VistaDriverAbstractWindowAspect)
    , m_bGrabCursorChanged(true)
    , m_bMouseWarpPending(false)
    , m_bConnected(false)
    , m_update_vec_mutex() {
  RegisterAspect(m_pWindowAspect);
  // is deleted by aspect, later on
  m_nButtonStates[0] = m_nButtonStates[1] = m_nButtonStates[2] = 0;
  m_pWindowAspect->SetTouchSequence(new VistaGlutMouseTouchSequence(this));
}

VistaGlutMouseDriver::~VistaGlutMouseDriver() {
  UnregisterAspect(m_pWindowAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaGlutMouseDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_bConnected)
    return false;

  if (m_vecUpdates.empty())
    return false;

  VistaGlutMouseTouchSequence* pTs =
      static_cast<VistaGlutMouseTouchSequence*>(m_pWindowAspect->GetTouchSequence());
  // hopefully, the measure history is large enough
  std::vector<_state> processing_set;
  {
    VistaMutexLock l(m_update_vec_mutex);
    processing_set.swap(m_vecUpdates);
  }

  std::vector<_state>::const_iterator begin = processing_set.begin();
  std::vector<_state>::const_iterator end   = processing_set.end();

  for (std::vector<_state>::const_iterator cit = begin; cit != end; ++cit) {
    VistaGlutMouseTouchSequence::SENMAP::iterator it = pTs->m_mapSensor.find((*cit).m_nWinId);

    if (it == pTs->m_mapSensor.end()) {
      // utter error!
      continue;
    }

    unsigned int nSensorIdx = (*it).second.m_nIndex; // get sensor index

    MeasureStart(nSensorIdx, dTs);
    UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_LEFT, (*cit).m_nButtonStates[0]);
    UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_MIDDLE, (*cit).m_nButtonStates[1]);
    UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_RIGHT, (*cit).m_nButtonStates[2]);
    UpdateMousePosition(nSensorIdx, (*cit).m_nX, (*cit).m_nY);
    if ((*cit).m_nWheelNumber >= 0) {
      (*it).second.m_nWheelState += (*cit).m_nWheelDirection;
      (*it).second.m_nWheelDir = (*cit).m_nWheelDirection;
    }
    UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_WHEEL_STATE, (*it).second.m_nWheelState);
    UpdateMouseButton(nSensorIdx, IVistaMouseDriver::BT_WHEEL_DIR, (*it).second.m_nWheelDir);

    MeasureStop(nSensorIdx);
  }
  return true;
}

bool VistaGlutMouseDriver::GetMouseWarpPending() const {
  return m_bMouseWarpPending;
}

void VistaGlutMouseDriver::SetMouseWarpPending(const bool& oValue) {
  m_bMouseWarpPending = oValue;
}

bool VistaGlutMouseDriver::GetGrabCursorChanged() const {
  return m_bGrabCursorChanged;
}

void VistaGlutMouseDriver::SetGrabCursorChanged(const bool& oValue) {
  m_bGrabCursorChanged = oValue;
}

bool VistaGlutMouseDriver::DoConnect() {
  m_bConnected = true;
  return true;
}

bool VistaGlutMouseDriver::DoDisconnect() {
  m_bConnected = false;
  return true;
}

void VistaGlutMouseDriver::Receive(const _state& s) {
  VistaMutexLock l(m_update_vec_mutex);
  m_vecUpdates.push_back(s);
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
