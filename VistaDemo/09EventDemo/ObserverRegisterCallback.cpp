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

#include "ObserverRegisterCallback.h"
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaTools/VistaMemoryInfo.h>

#include <VistaKernel/EventManager/VistaCentralEventHandler.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include "TimeObserver.h"

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

ObserverRegisterCallback::ObserverRegisterCallback(
    VistaEventManager* pEventManager, int iObserverType, int iEventType)
    : m_pEventManager(pEventManager)
    , m_iObserverType(iObserverType)
    , m_iEventType(iEventType) {
  m_pObserver = NULL;
}

ObserverRegisterCallback::~ObserverRegisterCallback() {
  // do NOT delete the EventManager
  m_pEventManager = NULL;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool ObserverRegisterCallback::PrepareCallback() {
  return true;
}

bool ObserverRegisterCallback::Do() {
  bool bState = false;

  if (!m_pObserver) {
    std::cout << "TRY" << std::endl;
    switch (m_iObserverType) {
    case TIME_OBSERVER:
      m_pObserver = new TimeObserver(m_pEventManager, m_iEventType);
      break;
    default:
      std::cout << "[ObserverRegisterCallback] failed - unknown observer" << std::endl;
      return false;
      break;
    }
    bState = m_pEventManager->RegisterObserver(m_pObserver, m_iEventType);
  } else {
    m_pEventManager->UnregisterObserver(m_pObserver, m_iEventType);
    delete m_pObserver;
    m_pObserver = NULL;
    bState      = true;
  }

  if (bState) {
    std::cout << "[ObserverRegisterCallback] success" << std::endl;
  } else {
    std::cout << "[ObserverRegisterCallback] failed" << std::endl;
  }

  return bState;
}

bool ObserverRegisterCallback::PostCallback() {
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE                                                               */
/*============================================================================*/
