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

#include "VistaTimeoutHandler.h"

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include "VistaTickTimer.h"
#include "VistaTickTimerEvent.h"

#include <algorithm>

//#include <stdio.h>

VistaTimeoutHandler::VistaTimeoutHandler(VistaEventManager* pEvMgr, VistaClusterMode* pClusterMode)
    : VistaEventHandler()
    , m_pEventManager(pEvMgr)
    , m_pClusterAux(pClusterMode) {
}

VistaTimeoutHandler::~VistaTimeoutHandler() {
  std::vector<VistaTickTimer*>::iterator it;
  for (it = m_veWatches.begin(); it != m_veWatches.end(); ++it)
    delete *it;

  // m_veWatches.clear();

  for (std::list<VistaTickTimer*>::const_iterator cit = m_liGarbage.begin();
       cit != m_liGarbage.end(); ++cit) {
    delete *cit;
  }

  m_pEventManager->RemEventHandler(this, VistaEventManager::NVET_ALL, VistaEventManager::NVET_ALL);
}

void VistaTimeoutHandler::ResetTimeout(HD_TIMER hd) {
  if (hd->IsRunning())
    hd->StopTickTimer();

  hd->ResetTickTimer();
  hd->StartTickTimer();
}

bool VistaTimeoutHandler::IsValidHandle(HD_TIMER handle) const {
  if (ConvertFromHandle(handle) == NULL)
    return false;

  std::vector<VistaTickTimer*>::const_iterator it =
      std::find(m_veWatches.begin(), m_veWatches.end(), handle);
  if (it != m_veWatches.end())
    return true;

  return false;
}

void VistaTimeoutHandler::SetTimeout(HD_TIMER hd, double dTime) {

  VistaTickTimer* pTickTimer = hd;

  if (pTickTimer->IsRunning())
    pTickTimer->StopTickTimer();

  pTickTimer->SetTickTime(dTime);

  ResetTimeout(hd);
}

VistaTimeoutHandler::HD_TIMER VistaTimeoutHandler::AddTimeout(double dTime) {
  VistaTickTimer* pWatch = NULL;
  if (VistaTickTimerEvent::GetTypeId() == VistaEvent::VET_INVALID) {
    VistaTickTimerEvent::RegisterEventTypes(m_pEventManager);
  }

  m_pEventManager->AddEventHandler(
      this, VistaTickTimerEvent::GetTypeId(), VistaTickTimerEvent::TTID_TIMEOUT);

  if (m_liGarbage.empty()) {
    pWatch = new VistaTickTimer(m_pEventManager, m_pClusterAux);
  } else {
    pWatch = m_liGarbage.front();
    m_liGarbage.pop_front(); // remove from list
  }

  m_veWatches.push_back(pWatch);

  HD_TIMER ti = pWatch;
  SetTimeout(ti, dTime);
  return ti;
}

void VistaTimeoutHandler::HandleEvent(VistaEvent* pEvent) {
  // we do only listen for tick-timer events, so this is safe
  VistaTickTimerEvent* ev = static_cast<VistaTickTimerEvent*>(pEvent);

  // call subclass with the demultiplexed handle
  HandleTimeout(ev->GetSourceTickTimer());

  // check for pulsing timers
  if (!ev->GetSourceTickTimer()->GetPulsingTimer()) {
    // no pulsing timer, so we will stop this timer here
    ev->GetSourceTickTimer()->StopTickTimer();
  }

  // in any case: we did something to this event, so we advise the
  // EventManager not to propagate in any longer
  pEvent->SetHandled(true);
}

VistaTimeoutHandler::HD_TIMER VistaTimeoutHandler::FindTimer(double dTimeout) const {
  for (std::vector<VistaTickTimer*>::const_iterator it = m_veWatches.begin();
       it != m_veWatches.end(); ++it) {
    if ((*it)->GetTickTime() == dTimeout)
      return (*it);
  }
  return NULL;
}

bool VistaTimeoutHandler::RemoveTimeout(HD_TIMER handle) {
  std::vector<VistaTickTimer*>::iterator it =
      std::find(m_veWatches.begin(), m_veWatches.end(), handle);
  if (it != m_veWatches.end()) {
    // printf("Removing timer @ %x\n", handle);
    (*it)->StopTickTimer();
    m_liGarbage.push_back(*it);
    m_veWatches.erase(it);

    // printf("current registered timers: %d\n", m_veWatches.size());
    return true;
  }

  return false;
  //    pWatch->StopTickTimer();
  //  m_
  // delete pWatch;
}

VistaTickTimer* VistaTimeoutHandler::GetTimerForHandle(HD_TIMER tim) const {
  if (IsValidHandle(tim)) // check if this is our handle
    return tim;
  return NULL;
}

VistaTickTimer* VistaTimeoutHandler::ConvertFromHandle(HD_TIMER tim) const {
  return tim;
}
