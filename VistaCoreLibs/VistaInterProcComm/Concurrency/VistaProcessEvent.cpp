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

#include "VistaProcessEvent.h"
#include "Imp/VistaProcessEventImp.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaProcessEventSignaller::VistaProcessEventSignaller(const std::string& sEventName) {
  m_pImpl = IVistaProcessEventImp::CreateProcessEventSignallerImp(sEventName);
}

VistaProcessEventSignaller::~VistaProcessEventSignaller() {
  delete m_pImpl;
}

bool VistaProcessEventSignaller::GetIsValid() const {
  return m_pImpl->GetIsValid();
}

bool VistaProcessEventSignaller::SignalEvent() {
  return m_pImpl->SignalEvent();
}

std::string VistaProcessEventSignaller::GetEventName() const {
  return m_pImpl->GetEventName();
}

// ============================================================================
// ============================================================================

VistaProcessEventReceiver::VistaProcessEventReceiver(
    const std::string& sEventName, const int nMaxWaitForSignaller) {
  m_pImpl = IVistaProcessEventImp::CreateProcessEventReceiverImp(sEventName, nMaxWaitForSignaller);
}

VistaProcessEventReceiver::~VistaProcessEventReceiver() {
  delete m_pImpl;
}

bool VistaProcessEventReceiver::WaitForEvent(int iBlockTime) {
  return m_pImpl->WaitForEvent(iBlockTime);
}

bool VistaProcessEventReceiver::WaitForEvent(bool bBlock) {
  return m_pImpl->WaitForEvent(bBlock);
}

std::string VistaProcessEventReceiver::GetEventName() const {
  return m_pImpl->GetEventName();
}

bool VistaProcessEventReceiver::GetIsValid() const {
  return m_pImpl->GetIsValid();
}
