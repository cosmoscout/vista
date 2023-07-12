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

#include "VistaThreadEvent.h"
#include "Imp/VistaThreadEventImp.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaThreadEvent::VistaThreadEvent(bool waitable_event) {
  m_pImpl = IVistaThreadEventImp::CreateThreadEventImp(waitable_event);
}

VistaThreadEvent::VistaThreadEvent(WaitBehavior wait_behavior) {
  m_pImpl =
      IVistaThreadEventImp::CreateThreadEventImp(wait_behavior == WAITABLE_EVENT ? true : false);
}

VistaThreadEvent::~VistaThreadEvent() {
  delete m_pImpl;
}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaThreadEvent::SignalEvent() {
  m_pImpl->SignalEvent();
}

bool VistaThreadEvent::WaitForEvent(int iTimeoutMSecs) {
  return m_pImpl->WaitForEvent(iTimeoutMSecs);
}

bool VistaThreadEvent::WaitForEvent(bool bBlock) {
  return m_pImpl->WaitForEvent(bBlock);
}

HANDLE VistaThreadEvent::GetEventSignalHandle() const {
  return m_pImpl->GetEventSignalHandle();
}

HANDLE VistaThreadEvent::GetEventWaitHandle() const {
  return m_pImpl->GetEventWaitHandle();
}

bool VistaThreadEvent::ResetThisEvent(ResetBehavior reset_behavior) {
  return m_pImpl->ResetThisEvent(reset_behavior == RESET_ALL_EVENTS
                                     ? IVistaThreadEventImp::RESET_ALL_EVENTS
                                     : IVistaThreadEventImp::RESET_JUST_ONE_EVENT);
}

// ============================================================================
// ============================================================================
