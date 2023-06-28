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

#include "VistaInterProcClusterBarrier.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaInterProcComm/Concurrency/VistaProcessEvent.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaInterProcClusterLeaderBarrier::VistaInterProcClusterLeaderBarrier(
    IVistaClusterBarrier* pDependentBarrier, const bool bVerbose)
    : IVistaClusterBarrier(bVerbose, true)
    , m_pDependentBarrier(pDependentBarrier) {
}

VistaInterProcClusterLeaderBarrier::~VistaInterProcClusterLeaderBarrier() {

  for (std::vector<std::pair<VistaProcessEventReceiver*, int>>::iterator itEvent =
           m_vecReadyEvents.begin();
       itEvent != m_vecReadyEvents.end(); ++itEvent) {
    delete (*itEvent).first;
  }
  for (std::vector<VistaProcessEventSignaller*>::iterator itEvent = m_vecGoEvents.begin();
       itEvent != m_vecGoEvents.end(); ++itEvent) {
    delete (*itEvent);
  }
}

bool VistaInterProcClusterLeaderBarrier::AddInterProc(const std::string& sInterProcName) {
  VistaProcessEventReceiver* pReadyEvent =
      new VistaProcessEventReceiver(sInterProcName + "_READY", 1000);
  VistaProcessEventSignaller* pGoEvent = new VistaProcessEventSignaller(sInterProcName + "_GO");
  if (pReadyEvent->GetIsValid() && pGoEvent->GetIsValid()) {
    m_vecReadyEvents.push_back(std::pair<VistaProcessEventReceiver*, int>(pReadyEvent, 0));
    m_vecGoEvents.push_back(pGoEvent);
    return true;
  }

  vstr::warnp() << "[" << GetBarrierType() << "]: Could not create process events for name ["
                << sInterProcName << "]" << std::endl;
  delete pReadyEvent;
  delete pGoEvent;
  return false;
}

bool VistaInterProcClusterLeaderBarrier::BarrierWait(int iTimeOut) {
  if (GetIsValid() == false)
    return false;

  // wait for all followers
  for (std::vector<std::pair<VistaProcessEventReceiver*, int>>::iterator itEvent =
           m_vecReadyEvents.begin();
       itEvent != m_vecReadyEvents.end(); ++itEvent) {
    bool nRet;
    if (iTimeOut == 0)
      nRet = (*itEvent).first->WaitForEvent(true);
    else
      nRet = (*itEvent).first->WaitForEvent(iTimeOut);
    if (nRet == false) {
      if (++((*itEvent).second) >= 10) {
        vstr::errp() << "[" << GetBarrierType() << "]: event [" << (*itEvent).first->GetEventName()
                     << "] timed out 10 times in a row - removing it" << std::endl;
        delete (*itEvent).first;
        itEvent = m_vecReadyEvents.erase(itEvent);
        --itEvent; // since it will be ++'ed by the loop
      } else {
        vstr::warnp() << "[" << GetBarrierType() << "]: timeout while waiting for ready event ["
                      << (*itEvent).first->GetEventName() << "]" << std::endl;
      }
    } else {
      (*itEvent).second = 0;
    }
  }

  if (m_pDependentBarrier) {
    if (m_pDependentBarrier->BarrierWait() == false)
      return false;
  }

  // send go to all
  for (std::vector<VistaProcessEventSignaller*>::iterator itEvent = m_vecGoEvents.begin();
       itEvent != m_vecGoEvents.end(); ++itEvent) {
    (*itEvent)->SignalEvent();
  }

  return true;
}

bool VistaInterProcClusterLeaderBarrier::GetIsValid() const {
  return (m_vecReadyEvents.empty() == false);
}

std::string VistaInterProcClusterLeaderBarrier::GetBarrierType() const {
  return "InterProcEmittingBarrier";
}

int VistaInterProcClusterLeaderBarrier::GetSendBlockingThreshold() const {
  return -1;
}

bool VistaInterProcClusterLeaderBarrier::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaInterProcClusterFollowerBarrier::VistaInterProcClusterFollowerBarrier(
    const std::string& sInterProcName, const bool bVerbose)
    : IVistaClusterBarrier(bVerbose, false)
    , m_nTimeoutCount(0) {
  m_pReadyEvent = new VistaProcessEventSignaller(sInterProcName + "_READY");
  m_pGoEvent    = new VistaProcessEventReceiver(sInterProcName + "_GO", 1000);
  if (m_pReadyEvent->GetIsValid() == false && m_pGoEvent->GetIsValid() == false) {
    vstr::warnp() << "[" << GetBarrierType() << "]: Could not create process events for name ["
                  << sInterProcName << "]" << std::endl;
    delete m_pReadyEvent;
    m_pReadyEvent = NULL;
    delete m_pGoEvent;
    m_pGoEvent = NULL;
  }
}

VistaInterProcClusterFollowerBarrier::~VistaInterProcClusterFollowerBarrier() {
  delete m_pReadyEvent;
  delete m_pGoEvent;
}

bool VistaInterProcClusterFollowerBarrier::GetIsValid() const {
  return (m_pReadyEvent && m_pGoEvent);
}

bool VistaInterProcClusterFollowerBarrier::BarrierWait(int iTimeOut) {
  m_pReadyEvent->SignalEvent(); // signal our readiness
  bool nRet;
  if (iTimeOut == 0)
    nRet = m_pGoEvent->WaitForEvent(true); // wait for go
  else
    nRet = m_pGoEvent->WaitForEvent(iTimeOut); // wait for go
  if (nRet == false) {
    if (++m_nTimeoutCount >= 10) {
      vstr::errp() << "[" << GetBarrierType() << "]: 10 concecutive timeouts - aborting"
                   << std::endl;
      delete m_pReadyEvent;
      m_pReadyEvent = NULL;
      delete m_pGoEvent;
      m_pGoEvent = NULL;
    } else {
      vstr::warnp() << "[" << GetBarrierType() << "]: timeout while waiting for go event"
                    << std::endl;
    }
  } else
    m_nTimeoutCount = 0;
  return nRet;
}

std::string VistaInterProcClusterFollowerBarrier::GetBarrierType() const {
  return "InterProcFollowerBarrier";
}

int VistaInterProcClusterFollowerBarrier::GetSendBlockingThreshold() const {
  return -1;
}

bool VistaInterProcClusterFollowerBarrier::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}
