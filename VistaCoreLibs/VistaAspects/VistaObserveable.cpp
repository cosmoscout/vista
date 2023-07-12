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

#include "VistaObserveable.h"
#include "VistaObserver.h"
#include <algorithm>
#include <cstdio>
#include <list>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
bool IVistaObserveable::OBSERVER_INFO::operator==(const OBSERVER_INFO& oOther) const {
  return (m_pObserver == oOther.m_pObserver) && (m_eTicket == oOther.m_eTicket);
}

bool IVistaObserveable::OBSERVER_INFO::operator==(const IVistaObserver* pObs) const {
  return (m_pObserver == pObs);
}

IVistaObserveable::OBSERVER_INFO::OBSERVER_INFO(IVistaObserver* pObs, int eTicket)
    : m_pObserver(pObs)
    , m_eTicket(eTicket) {
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaObserveable::IVistaObserveable()
    : m_bNotificationFlag(true)
    , m_bNotificationRunning(false) {
}

IVistaObserveable::~IVistaObserveable() {
  // printf("IVistaObserveable[%s]::~IVistaObserveable() @ %X, number of observers: %d\n",
  // m_SObserveableName.c_str(), this, m_lIVistaObservers.size());
  SendDeleteMessage();
}

IVistaObserveable::IVistaObserveable(const IVistaObserveable& oOther)
    : m_bNotificationFlag(oOther.m_bNotificationFlag)
    , m_bNotificationRunning(false) {
}

IVistaObserveable& IVistaObserveable::operator=(const IVistaObserveable& oOther) {
  m_bNotificationFlag = oOther.m_bNotificationFlag;
  return *this;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int IVistaObserveable::AttachObserver(IVistaObserver* pObserver, int eTicket) {
  // when notification is active: deferred attach!
  if (m_bNotificationRunning) {
    this->AttachObserverDeferred(pObserver, eTicket);
    return static_cast<int>(m_vecVistaObservers.size());
  }

  m_vecVistaObservers.push_back(OBSERVER_INFO(pObserver, eTicket));
  // Notify(MSG_ATTACH);
  return static_cast<int>(m_vecVistaObservers.size());
}

int IVistaObserveable::DetachObserver(IVistaObserver* pObserver) {
  // when notification is active: deferred detach!
  if (m_bNotificationRunning) {
    this->DetachObserverDeferred(pObserver);
    return static_cast<int>(m_vecVistaObservers.size());
  }

  // locate suitable observer
  std::vector<OBSERVER_INFO>::iterator it =
      std::remove(m_vecVistaObservers.begin(), m_vecVistaObservers.end(), pObserver);
  if (it != m_vecVistaObservers.end()) {
    m_vecVistaObservers.erase(it, m_vecVistaObservers.end());
    // Notify(MSG_DETACH);
  }

  return static_cast<int>(m_vecVistaObservers.size());
}

bool IVistaObserveable::SendDeleteRequest() {
  /**
   * Observes will probably modify the observer list (detaching),
   * so we can not traverse the original list.
   * We will make a copy of the observerlist on which we will
   * progress and afterwards forget about it
   */
  std::vector<OBSERVER_INFO> vecCopy = m_vecVistaObservers; // we use a copy to traverse this list

  for (std::vector<OBSERVER_INFO>::iterator it = vecCopy.begin(); it != vecCopy.end(); ++it) {
    if (!(*it).m_pObserver->ObserveableDeleteRequest(this, (*it).m_eTicket))
      return false;
  }

  return true;
}

void IVistaObserveable::SendDeleteMessage() {
  /**
   * Observes will probably modify the observer list (detaching),
   * so we can not traverse the original list.
   * We will make a copy of the observerlist on which we will
   * progress and afterwards forget about it
   */
  std::vector<OBSERVER_INFO> vecCopy = m_vecVistaObservers;

  for (std::vector<OBSERVER_INFO>::const_iterator it = vecCopy.begin(); it != vecCopy.end(); ++it) {
    // command to release me!
    (*it).m_pObserver->ObserveableDelete(static_cast<IVistaObserveable*>(this), (*it).m_eTicket);
  }

  m_vecVistaObservers.clear();
}

bool IVistaObserveable::GetIsObservedBy(IVistaObserver* pObs) const {
  std::vector<OBSERVER_INFO>::const_iterator cit =
      std::find(m_vecVistaObservers.begin(), m_vecVistaObservers.end(), pObs);

  return (cit != m_vecVistaObservers.end());
}

void IVistaObserveable::Notify(int msg) {
  // in case there is no work...
  // do no work ;)
  // this, too, avoids piling of changes in
  // the change set!
  if (m_vecVistaObservers.empty())
    return;

  if (!m_bNotificationFlag) {
    // the idea is to catch subsequent non-notified
    // messages to give the observers a chance to
    // realize what happened.
    // so we insert the msg to the set, regardless
    // of whether we really call VistaObserver::ObserverUpdate()
    // or not.
    m_sChangedSet.insert(msg);
    return; // NO NOTIFICATION (disabled on user request)!!
  }

  // notification process starts
  m_bNotificationRunning = true;

  std::vector<OBSERVER_INFO>::const_iterator it;
  for (it = m_vecVistaObservers.begin(); it != m_vecVistaObservers.end(); ++it) {
    (*it).m_pObserver->UpdateRequest(this, msg, (*it).m_eTicket);
  }

  if (!m_sChangedSet.empty()) {
    // ok, we passed through this, so we consider the model to be
    // "unchanged"! we can clear the set now.
    m_sChangedSet.clear();
  }

  // notification process done
  m_bNotificationRunning = false;

  // process pending deferred actions
  ProcessDeferredActions();
}

void IVistaObserveable::SetNotificationFlag(bool bNotifyEnabled) {
  m_bNotificationFlag = bNotifyEnabled;
}

bool IVistaObserveable::GetNotificationFlag() const {
  return m_bNotificationFlag;
}

const std::set<int>& IVistaObserveable::GetChangedSetConstRef() const {
  return m_sChangedSet;
}

void IVistaObserveable::ClearChangeSet() {
  m_sChangedSet.clear();
}

void IVistaObserveable::AttachObserverDeferred(
    IVistaObserver* pObs, int eTicket /*= TICKET_NONE*/) {
  m_vecDeferredActions.push_back(std::make_pair(OBSERVER_INFO(pObs, eTicket), true));
}

void IVistaObserveable::DetachObserverDeferred(IVistaObserver* pObs) {
  // note: ticket is ignored for the actual call
  m_vecDeferredActions.push_back(std::make_pair(OBSERVER_INFO(pObs, 0), false));
}

void IVistaObserveable::ProcessDeferredActions() {
  // process all attach and detach actions in the order they are supposed
  // to be called

  // not during the notification
  if (m_bNotificationRunning)
    return;

  // nothing to do?
  if (m_vecDeferredActions.empty())
    return;

  for (size_t i = 0; i < m_vecDeferredActions.size(); ++i) {
    if (m_vecDeferredActions[i].second == true) // attach
    {
      AttachObserver(
          m_vecDeferredActions[i].first.m_pObserver, m_vecDeferredActions[i].first.m_eTicket);
    } else // detach
    {
      DetachObserver(m_vecDeferredActions[i].first.m_pObserver);
    }
  }

  // clear list
  m_vecDeferredActions.clear();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
