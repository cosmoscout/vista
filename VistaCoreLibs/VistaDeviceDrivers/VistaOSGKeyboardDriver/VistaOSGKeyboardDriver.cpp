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

#include "VistaOSGKeyboardDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/DisplayManager/OpenSceneGraphWindowImp/VistaOSGWindowingToolkit.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

#include <osgGA/EventQueue>
#include <osgGA/GUIEventAdapter>

#include <algorithm>
#include <iterator>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

bool InterpreteOSGKey(int& nKey, const int nMod) {
  switch (nKey) {
  case osgGA::GUIEventAdapter::KEY_Space:
    nKey = ' ';
    break;
  case osgGA::GUIEventAdapter::KEY_0:
  case osgGA::GUIEventAdapter::KEY_1:
  case osgGA::GUIEventAdapter::KEY_2:
  case osgGA::GUIEventAdapter::KEY_3:
  case osgGA::GUIEventAdapter::KEY_4:
  case osgGA::GUIEventAdapter::KEY_5:
  case osgGA::GUIEventAdapter::KEY_6:
  case osgGA::GUIEventAdapter::KEY_7:
  case osgGA::GUIEventAdapter::KEY_8:
  case osgGA::GUIEventAdapter::KEY_9:
    break;
  case osgGA::GUIEventAdapter::KEY_A:
  case osgGA::GUIEventAdapter::KEY_B:
  case osgGA::GUIEventAdapter::KEY_C:
  case osgGA::GUIEventAdapter::KEY_D:
  case osgGA::GUIEventAdapter::KEY_E:
  case osgGA::GUIEventAdapter::KEY_F:
  case osgGA::GUIEventAdapter::KEY_G:
  case osgGA::GUIEventAdapter::KEY_H:
  case osgGA::GUIEventAdapter::KEY_I:
  case osgGA::GUIEventAdapter::KEY_J:
  case osgGA::GUIEventAdapter::KEY_K:
  case osgGA::GUIEventAdapter::KEY_L:
  case osgGA::GUIEventAdapter::KEY_M:
  case osgGA::GUIEventAdapter::KEY_N:
  case osgGA::GUIEventAdapter::KEY_O:
  case osgGA::GUIEventAdapter::KEY_P:
  case osgGA::GUIEventAdapter::KEY_Q:
  case osgGA::GUIEventAdapter::KEY_R:
  case osgGA::GUIEventAdapter::KEY_S:
  case osgGA::GUIEventAdapter::KEY_T:
  case osgGA::GUIEventAdapter::KEY_U:
  case osgGA::GUIEventAdapter::KEY_V:
  case osgGA::GUIEventAdapter::KEY_W:
  case osgGA::GUIEventAdapter::KEY_X:
  case osgGA::GUIEventAdapter::KEY_Y:
  case osgGA::GUIEventAdapter::KEY_Z:
  case osgGA::GUIEventAdapter::KEY_A - 32:
  case osgGA::GUIEventAdapter::KEY_B - 32:
  case osgGA::GUIEventAdapter::KEY_C - 32:
  case osgGA::GUIEventAdapter::KEY_D - 32:
  case osgGA::GUIEventAdapter::KEY_E - 32:
  case osgGA::GUIEventAdapter::KEY_F - 32:
  case osgGA::GUIEventAdapter::KEY_G - 32:
  case osgGA::GUIEventAdapter::KEY_H - 32:
  case osgGA::GUIEventAdapter::KEY_I - 32:
  case osgGA::GUIEventAdapter::KEY_J - 32:
  case osgGA::GUIEventAdapter::KEY_K - 32:
  case osgGA::GUIEventAdapter::KEY_L - 32:
  case osgGA::GUIEventAdapter::KEY_M - 32:
  case osgGA::GUIEventAdapter::KEY_N - 32:
  case osgGA::GUIEventAdapter::KEY_O - 32:
  case osgGA::GUIEventAdapter::KEY_P - 32:
  case osgGA::GUIEventAdapter::KEY_Q - 32:
  case osgGA::GUIEventAdapter::KEY_R - 32:
  case osgGA::GUIEventAdapter::KEY_S - 32:
  case osgGA::GUIEventAdapter::KEY_T - 32:
  case osgGA::GUIEventAdapter::KEY_U - 32:
  case osgGA::GUIEventAdapter::KEY_V - 32:
  case osgGA::GUIEventAdapter::KEY_W - 32:
  case osgGA::GUIEventAdapter::KEY_X - 32:
  case osgGA::GUIEventAdapter::KEY_Y - 32:
  case osgGA::GUIEventAdapter::KEY_Z - 32:
    break;

  case osgGA::GUIEventAdapter::KEY_Exclaim:
  case osgGA::GUIEventAdapter::KEY_Quotedbl:
  case osgGA::GUIEventAdapter::KEY_Hash:
  case osgGA::GUIEventAdapter::KEY_Dollar:
  case osgGA::GUIEventAdapter::KEY_Ampersand:
  case osgGA::GUIEventAdapter::KEY_Quote:
  case osgGA::GUIEventAdapter::KEY_Leftparen:
  case osgGA::GUIEventAdapter::KEY_Rightparen:
  case osgGA::GUIEventAdapter::KEY_Asterisk:
  case osgGA::GUIEventAdapter::KEY_Plus:
  case osgGA::GUIEventAdapter::KEY_Comma:
  case osgGA::GUIEventAdapter::KEY_Minus:
  case osgGA::GUIEventAdapter::KEY_Period:
  case osgGA::GUIEventAdapter::KEY_Slash:
  case osgGA::GUIEventAdapter::KEY_Colon:
  case osgGA::GUIEventAdapter::KEY_Semicolon:
  case osgGA::GUIEventAdapter::KEY_Less:
  case osgGA::GUIEventAdapter::KEY_Equals:
  case osgGA::GUIEventAdapter::KEY_Greater:
  case osgGA::GUIEventAdapter::KEY_Question:
  case osgGA::GUIEventAdapter::KEY_At:
  case osgGA::GUIEventAdapter::KEY_Leftbracket:
  case osgGA::GUIEventAdapter::KEY_Backslash:
  case osgGA::GUIEventAdapter::KEY_Rightbracket:
  case osgGA::GUIEventAdapter::KEY_Caret:
  case osgGA::GUIEventAdapter::KEY_Underscore:
  case osgGA::GUIEventAdapter::KEY_Backquote:
    break;

  case osgGA::GUIEventAdapter::KEY_BackSpace:
    nKey = VISTA_KEY_BACKSPACE;
    break;
  case osgGA::GUIEventAdapter::KEY_Tab:
    nKey = VISTA_KEY_TAB;
    break;
  case osgGA::GUIEventAdapter::KEY_Linefeed:
    nKey = VISTA_KEY_ENTER;
    break;
    // case osgGA::GUIEventAdapter::KEY_Clear           = 0xFF0B,
  case osgGA::GUIEventAdapter::KEY_Return:
    nKey = VISTA_KEY_ENTER;
    break;
    // case osgGA::GUIEventAdapter::KEY_Pause
    // case osgGA::GUIEventAdapter::KEY_Scroll_Lock     = 0xFF14,
    // case osgGA::GUIEventAdapter::KEY_Sys_Req         = 0xFF15,
  case osgGA::GUIEventAdapter::KEY_Escape:
    nKey = VISTA_KEY_ESC;
    break;
  case osgGA::GUIEventAdapter::KEY_Delete:
    nKey = VISTA_KEY_DELETE;
    break;

    /* Cursor control & motion */

  case osgGA::GUIEventAdapter::KEY_Home:
    nKey = VISTA_KEY_HOME;
    break;
  case osgGA::GUIEventAdapter::KEY_Left:
    nKey = VISTA_KEY_LEFTARROW;
    break;
  case osgGA::GUIEventAdapter::KEY_Up:
    nKey = VISTA_KEY_UPARROW;
    break;
  case osgGA::GUIEventAdapter::KEY_Right:
    nKey = VISTA_KEY_RIGHTARROW;
    break;
  case osgGA::GUIEventAdapter::KEY_Down:
    nKey = VISTA_KEY_DOWNARROW;
    break;
    // case osgGA::GUIEventAdapter::KEY_Prior           = 0xFF55,        /* Prior, previous */
  case osgGA::GUIEventAdapter::KEY_Page_Up:
    nKey = VISTA_KEY_PAGEUP;
    break;
    // case osgGA::GUIEventAdapter::KEY_Next            = 0xFF56,        /* Next */
  case osgGA::GUIEventAdapter::KEY_Page_Down:
    nKey = VISTA_KEY_PAGEDOWN;
    break;
  case osgGA::GUIEventAdapter::KEY_End:
    nKey = VISTA_KEY_END;
    break;
    // case osgGA::GUIEventAdapter::KEY_Begin           = 0xFF58,        /* BOL */

    /* Misc Functions */

    // case osgGA::GUIEventAdapter::KEY_Select          = 0xFF60,        /* Select, mark */
    // case osgGA::GUIEventAdapter::KEY_Print           = 0xFF61,
    // case osgGA::GUIEventAdapter::KEY_Execute         = 0xFF62,        /* Execute, run, do */
    // case osgGA::GUIEventAdapter::KEY_Insert          = 0xFF63,        /* Insert, insert here */
    // case osgGA::GUIEventAdapter::KEY_Undo            = 0xFF65,        /* Undo, oops */
    // case osgGA::GUIEventAdapter::KEY_Redo            = 0xFF66,        /* redo, again */
    // case osgGA::GUIEventAdapter::KEY_Menu            = 0xFF67,        /* On Windows, this is
    // VK_APPS, the context-menu case osgGA::GUIEventAdapter::KEY */ case
    // osgGA::GUIEventAdapter::KEY_Find            = 0xFF68,        /* Find, search */ case
    // osgGA::GUIEventAdapter::KEY_Cancel          = 0xFF69,        /* Cancel, stop, abort, exit */
    // case osgGA::GUIEventAdapter::KEY_Help            = 0xFF6A,        /* Help */
    // case osgGA::GUIEventAdapter::KEY_Break           = 0xFF6B,
    // case osgGA::GUIEventAdapter::KEY_Mode_switch     = 0xFF7E,        /* Character set switch */
    // case osgGA::GUIEventAdapter::KEY_Script_switch   = 0xFF7E,        /* Alias for mode_switch */
    // case osgGA::GUIEventAdapter::KEY_Num_Lock        = 0xFF7F,

    /* case osgGA::GUIEventAdapter::KEYpad Functions, case osgGA::GUIEventAdapter::KEYpad numbers
     * cleverly chosen to map to ascii */

  case osgGA::GUIEventAdapter::KEY_KP_Space:
    nKey = ' ';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Tab:
    nKey = VISTA_KEY_TAB;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Enter:
    nKey = VISTA_KEY_ENTER;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_F1:
    nKey = VISTA_KEY_F1;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_F2:
    nKey = VISTA_KEY_F2;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_F3:
    nKey = VISTA_KEY_F3;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_F4:
    nKey = VISTA_KEY_F4;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Home:
    nKey = VISTA_KEY_HOME;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Left:
    nKey = VISTA_KEY_LEFTARROW;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Up:
    nKey = VISTA_KEY_UPARROW;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Right:
    nKey = VISTA_KEY_RIGHTARROW;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Down:
    nKey = VISTA_KEY_DOWNARROW;
    break;
  // case osgGA::GUIEventAdapter::KEY_KP_Prior:
  case osgGA::GUIEventAdapter::KEY_KP_Page_Up:
    nKey = VISTA_KEY_PAGEUP;
    break;
  // case osgGA::GUIEventAdapter::KEY_KP_Next:
  case osgGA::GUIEventAdapter::KEY_KP_Page_Down:
    nKey = VISTA_KEY_PAGEDOWN;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_End:
    nKey = VISTA_KEY_END;
    break;
  // case osgGA::GUIEventAdapter::KEY_KP_Begin:
  // case osgGA::GUIEventAdapter::KEY_KP_Insert:
  case osgGA::GUIEventAdapter::KEY_KP_Delete:
    nKey = VISTA_KEY_DELETE;
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Equal:
    nKey = '=';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Multiply:
    nKey = '*';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Add:
    nKey = '+';
    break;
  // case osgGA::GUIEventAdapter::KEY_KP_Separator:
  case osgGA::GUIEventAdapter::KEY_KP_Subtract:
    nKey = '-';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Decimal:
    nKey = '.';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_Divide:
    nKey = '/';
    break;

  case osgGA::GUIEventAdapter::KEY_KP_0:
    nKey = '0';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_1:
    nKey = '1';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_2:
    nKey = '2';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_3:
    nKey = '3';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_4:
    nKey = '4';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_5:
    nKey = '5';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_6:
    nKey = '6';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_7:
    nKey = '7';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_8:
    nKey = '8';
    break;
  case osgGA::GUIEventAdapter::KEY_KP_9:
    nKey = '9';
    break;

    /*
     * Auxiliary Functions; note the duplicate definitions for left and right
     * function case osgGA::GUIEventAdapter::KEYs;  Sun case osgGA::GUIEventAdapter::KEYboards and a
     * few other manufactures have such function case osgGA::GUIEventAdapter::KEY groups on the left
     * and/or right sides of the case osgGA::GUIEventAdapter::KEYboard. We've not found a case
     * osgGA::GUIEventAdapter::KEYboard with more than 35 function case osgGA::GUIEventAdapter::KEYs
     * total.
     */

  case osgGA::GUIEventAdapter::KEY_F1:
    nKey = VISTA_KEY_F1;
    break;
  case osgGA::GUIEventAdapter::KEY_F2:
    nKey = VISTA_KEY_F2;
    break;
  case osgGA::GUIEventAdapter::KEY_F3:
    nKey = VISTA_KEY_F3;
    break;
  case osgGA::GUIEventAdapter::KEY_F4:
    nKey = VISTA_KEY_F4;
    break;
  case osgGA::GUIEventAdapter::KEY_F5:
    nKey = VISTA_KEY_F5;
    break;
  case osgGA::GUIEventAdapter::KEY_F6:
    nKey = VISTA_KEY_F6;
    break;
  case osgGA::GUIEventAdapter::KEY_F7:
    nKey = VISTA_KEY_F7;
    break;
  case osgGA::GUIEventAdapter::KEY_F8:
    nKey = VISTA_KEY_F8;
    break;
  case osgGA::GUIEventAdapter::KEY_F9:
    nKey = VISTA_KEY_F9;
    break;
  case osgGA::GUIEventAdapter::KEY_F10:
    nKey = VISTA_KEY_F10;
    break;
  case osgGA::GUIEventAdapter::KEY_F11:
    nKey = VISTA_KEY_F11;
    break;
  case osgGA::GUIEventAdapter::KEY_F12:
    nKey = VISTA_KEY_F12;
    break;
    // case osgGA::GUIEventAdapter::KEY_F13             = 0xFFCA,
    // case osgGA::GUIEventAdapter::KEY_F14             = 0xFFCB,
    // case osgGA::GUIEventAdapter::KEY_F15             = 0xFFCC,
    // case osgGA::GUIEventAdapter::KEY_F16             = 0xFFCD,
    // case osgGA::GUIEventAdapter::KEY_F17             = 0xFFCE,
    // case osgGA::GUIEventAdapter::KEY_F18             = 0xFFCF,
    // case osgGA::GUIEventAdapter::KEY_F19             = 0xFFD0,
    // case osgGA::GUIEventAdapter::KEY_F20             = 0xFFD1,
    // case osgGA::GUIEventAdapter::KEY_F21             = 0xFFD2,
    // case osgGA::GUIEventAdapter::KEY_F22             = 0xFFD3,
    // case osgGA::GUIEventAdapter::KEY_F23             = 0xFFD4,
    // case osgGA::GUIEventAdapter::KEY_F24             = 0xFFD5,
    // case osgGA::GUIEventAdapter::KEY_F25             = 0xFFD6,
    // case osgGA::GUIEventAdapter::KEY_F26             = 0xFFD7,
    // case osgGA::GUIEventAdapter::KEY_F27             = 0xFFD8,
    // case osgGA::GUIEventAdapter::KEY_F28             = 0xFFD9,
    // case osgGA::GUIEventAdapter::KEY_F29             = 0xFFDA,
    // case osgGA::GUIEventAdapter::KEY_F30             = 0xFFDB,
    // case osgGA::GUIEventAdapter::KEY_F31             = 0xFFDC,
    // case osgGA::GUIEventAdapter::KEY_F32             = 0xFFDD,
    // case osgGA::GUIEventAdapter::KEY_F33             = 0xFFDE,
    // case osgGA::GUIEventAdapter::KEY_F34             = 0xFFDF,
    // case osgGA::GUIEventAdapter::KEY_F35             = 0xFFE0,

    /* Modifiers */

  case osgGA::GUIEventAdapter::KEY_Shift_L:
  case osgGA::GUIEventAdapter::KEY_Shift_R:
  case osgGA::GUIEventAdapter::KEY_Control_L:
  case osgGA::GUIEventAdapter::KEY_Control_R:
  case osgGA::GUIEventAdapter::KEY_Caps_Lock:
  case osgGA::GUIEventAdapter::KEY_Shift_Lock:
    return false;

    // case osgGA::GUIEventAdapter::KEY_Meta_L          = 0xFFE7,        /* Left meta */
    // case osgGA::GUIEventAdapter::KEY_Meta_R          = 0xFFE8,        /* Right meta */
    // case osgGA::GUIEventAdapter::KEY_Alt_L           = 0xFFE9,        /* Left alt */
    // case osgGA::GUIEventAdapter::KEY_Alt_R           = 0xFFEA,        /* Right alt */
    // case osgGA::GUIEventAdapter::KEY_Super_L         = 0xFFEB,        /* Left super */
    // case osgGA::GUIEventAdapter::KEY_Super_R         = 0xFFEC,        /* Right super */
    // case osgGA::GUIEventAdapter::KEY_Hyper_L         = 0xFFED,        /* Left hyper */
    // case osgGA::GUIEventAdapter::KEY_Hyper_R         = 0xFFEE

  default: {
    vstr::warnp() << "OSGKEyboardDriver: Cannot interpret key [" << nKey << "]" << std::endl;
    return false;
  }
  }
  return true;
}

int InterpreteOSGModifiers(const int nModifiers) {
  int nResMask = VISTA_KEYMOD_NONE;
  if (nModifiers & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
    nResMask |= VISTA_KEYMOD_SHIFT;
  }
  if (nModifiers & osgGA::GUIEventAdapter::MODKEY_CTRL) {
    nResMask |= VISTA_KEYMOD_CTRL;
  }
  if (nModifiers & osgGA::GUIEventAdapter::MODKEY_ALT) {
    nResMask |= VISTA_KEYMOD_ALT;
  }
  return nResMask;
}

VistaOSGKeyboardDriverCreationMethod::VistaOSGKeyboardDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType(
      "", sizeof(IVistaKeyboardDriver::_sKeyboardMeasure), 100, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaOSGKeyboardDriverCreationMethod::CreateDriver() {
  return new VistaOSGKeyboardDriver(this);
}

/*============================================================================*/
/*============================================================================*/

class VistaOSGKeyboardTouchSequence
    : public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence {
 public:
  typedef IVistaWindowingToolkit::VistaKernelWindowHandle             WindowHandle;
  typedef std::list<IVistaWindowingToolkit::VistaKernelWindowHandle*> WindowList;

 public:
  VistaOSGKeyboardTouchSequence(VistaOSGKeyboardDriver* pDriver)
      : VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence()
      , m_pKeyboardDriver(pDriver) {
  }

  bool AttachSequence(VistaDriverAbstractWindowAspect::IWindowHandle* pWindow) {
    WindowHandle* pKernelWindow = dynamic_cast<WindowHandle*>(pWindow);
    if (pKernelWindow == NULL)
      return false;
    // check whether this driver is already registered with the window
    WindowList::const_iterator itWin =
        std::find(m_vecWindows.begin(), m_vecWindows.end(), pKernelWindow);
    if (itWin != m_vecWindows.end())
      return false;

    if (m_pKeyboardDriver->RegisterWindow(pKernelWindow->GetWindow()) == false)
      return false;

    m_vecWindows.push_back(pKernelWindow);
    return true;
  }

  bool DetachSequence(VistaDriverAbstractWindowAspect::IWindowHandle* pWindow) {
    WindowHandle* pKernelWindow = dynamic_cast<WindowHandle*>(pWindow);
    if (pWindow == NULL)
      return false;
    WindowList::iterator itWin = std::find(m_vecWindows.begin(), m_vecWindows.end(), pKernelWindow);
    if (itWin == m_vecWindows.end())
      return false;
    m_vecWindows.erase(itWin);
    return true;
  }

  virtual std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> GetWindowList() const {
    std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> liWins;

    std::copy(m_vecWindows.begin(), m_vecWindows.end(), std::back_inserter(liWins));
    return liWins;
  }

 private:
  WindowList              m_vecWindows;
  VistaOSGKeyboardDriver* m_pKeyboardDriver;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOSGKeyboardDriver::VistaOSGKeyboardDriver(IVistaDriverCreationMethod* crm)
    : IVistaKeyboardDriver(crm)
    , m_pWindowAspect(new VistaDriverAbstractWindowAspect)
    , m_pWindowingToolkit(NULL) {
  RegisterAspect(m_pWindowAspect);
  // is deleted by aspect, later on
  m_pWindowAspect->SetTouchSequence(new VistaOSGKeyboardTouchSequence(this));
}

VistaOSGKeyboardDriver::~VistaOSGKeyboardDriver() {
  UnregisterAspect(m_pWindowAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

struct TemporaryKey {
  bool operator<(const TemporaryKey& oOther) {
    return m_nTime < oOther.m_nTime;
  }
  int    m_nKey;
  int    m_nModifier;
  double m_nTime;
};

typedef std::vector<TemporaryKey> TemporaryKeyList;

bool VistaOSGKeyboardDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (m_pWindowingToolkit == NULL || m_vecWindows.empty())
    return false;

  TemporaryKeyList vecKeys;
  for (WindowList::iterator itWin = m_vecWindows.begin(); itWin != m_vecWindows.end(); ++itWin) {
    osgGA::EventQueue::Events&          oEvents = m_pWindowingToolkit->GetEventsForWindow((*itWin));
    osgGA::EventQueue::Events::iterator itEvent;
    for (itEvent = oEvents.begin(); itEvent != oEvents.end(); ++itEvent) {
      osgGA::GUIEventAdapter* pEvent = itEvent->get();
      if (pEvent->getHandled())
        continue;
      switch (pEvent->getEventType()) {
      // @OSGTODO: how to best merge?
      case (osgGA::GUIEventAdapter::KEYDOWN): {
        TemporaryKey oKeyInfo;
        int          nKey = pEvent->getKey();
        if (InterpreteOSGKey(nKey, pEvent->getModKeyMask())) {
          oKeyInfo.m_nKey      = nKey;
          oKeyInfo.m_nModifier = InterpreteOSGModifiers(pEvent->getModKeyMask());
          oKeyInfo.m_nTime     = pEvent->getTime();
          vecKeys.push_back(oKeyInfo);
          pEvent->setHandled(true);
        }
        break;
      }
      case (osgGA::GUIEventAdapter::KEYUP): {
        TemporaryKey oKeyInfo;
        int          nKey = pEvent->getKey();
        if (InterpreteOSGKey(nKey, pEvent->getModKeyMask())) {
          oKeyInfo.m_nKey      = -nKey;
          oKeyInfo.m_nModifier = InterpreteOSGModifiers(pEvent->getModKeyMask());
          oKeyInfo.m_nTime     = pEvent->getTime();
          vecKeys.push_back(oKeyInfo);
          pEvent->setHandled(true);
        };
        break;
      }
      default:
        break;
      }
    }
  }

  if (vecKeys.empty())
    return false;

  MeasureStart(dTs); // @OSGTODO time
  for (TemporaryKeyList::const_iterator itKey = vecKeys.begin(); itKey != vecKeys.end(); ++itKey) {
    UpdateKey((*itKey).m_nKey, (*itKey).m_nModifier);
  }
  MeasureStop();

  return true;
};

bool VistaOSGKeyboardDriver::RegisterWindow(VistaWindow* pWindow) {
  if (m_pWindowingToolkit == NULL) {
    m_pWindowingToolkit =
        dynamic_cast<VistaOSGWindowingToolkit*>(pWindow->GetDisplayBridge()->GetWindowingToolkit());
    if (m_pWindowingToolkit == NULL) {
      VISTA_THROW("VistaOSGKeyboardDriver requires OSGWindowingToolkit", -1);
    }
  }

  m_vecWindows.push_back(pWindow);
  return true;
}

bool VistaOSGKeyboardDriver::UnregisterWindow(VistaWindow* pWindow) {
  WindowList::iterator itWin = std::find(m_vecWindows.begin(), m_vecWindows.end(), pWindow);
  if (itWin == m_vecWindows.end())
    return false;
  m_vecWindows.erase(itWin);
  return true;
}

bool VistaOSGKeyboardDriver::DoConnect() {
  return true;
}

bool VistaOSGKeyboardDriver::DoDisconnect() {
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
