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

#include "VistaEventManager.h"

#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <algorithm>
#include <cassert>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

struct VistaEventManager::_qCompPrio {
  bool operator()(
      const VistaEventManager::HANDLER& left, const VistaEventManager::HANDLER& right) const {
    return left.m_nPrio > right.m_nPrio;
  }
};

bool VistaEventManager::HANDLER::operator==(const HANDLER& other) {
  return this->m_pHandler == other.m_pHandler;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaEventManager::VistaEventManager()
    : m_bObserverQueueDirty(false)
    , m_iProcessRecursionCount(0)
    , m_nEventCount(0)
    , m_bResetEventTimeToLocalTime(true) {
}

VistaEventManager::~VistaEventManager() {
  CleanupHandlerMapping();
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaEventManager::Init() {
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ProcessEvent                                                */
/*                                                                            */
/*============================================================================*/
bool VistaEventManager::ProcessEvent(VistaEvent* pEvent) {
  // make sure, we do have a valid event
  if (pEvent == NULL) {
    vstr::warnp() << "[ViEvMa] received NULL event pointer" << std::endl;
    return false;
  }

  // find proper mapping
  int iEventType = pEvent->GetType();

  // Is the event itself valid?
  if (!IsValidEventType(iEventType)) {
    vstr::warni() << "[ViEvMa] received invalid event - event type: " << pEvent->GetType()
                  << std::endl;
    return false;
  }

  // increase recursion counter
  ++m_iProcessRecursionCount;

  pEvent->m_nCount = ++m_nEventCount;

  // set some data members of the event
  pEvent->m_bHandled = false;

  if (m_bResetEventTimeToLocalTime)
    pEvent->m_nTime = VistaTimeUtils::GetStandardTimer().GetSystemTime();

  // find event handler for this type
  HANP& hp    = m_veHandlerMapping[iEventType];
  int   nEvId = pEvent->GetId();

  // notify observers first
  OBSQUEUE* pO = hp.second.second;
  for (OBSQUEUE::const_iterator cit = (*pO).begin(); cit != (*pO).end(); ++cit) {
    if (!(*cit)->m_bIsDef)
      (*cit)->Notify(pEvent);
  }

  if (nEvId < (int)hp.second.first.size()) {
    HANQUEUE* q = hp.second.first[nEvId].second;
    for (HANQUEUE::iterator it = (*q).begin(); it != (*q).end(); ++it) {
      if ((*it).m_pHandler->GetIsEnabled()) {
        (*it).m_pHandler->HandleEvent(pEvent);

        // check, if event was consumed
        if (pEvent->IsHandled())
          break; // leave loop
      }
    }
  }

  // ok, we are done, decrease recursion count
  --m_iProcessRecursionCount;

  // in case we are at the top-level call to ProcessEvent, we can
  // compute the new observer lists, but only iff there was any
  // change at all
  if ((m_iProcessRecursionCount == 0) && m_bObserverQueueDirty)
    HandleObserverQueueChange();

  // indicate success to caller
  return pEvent->IsHandled();
}

int VistaEventManager::HandleObserverQueueChange() {
  int i = 0;
  // register / unregister pending observers

  // note the order: first register the pendings, then unregister the
  // pendings. Iff an observer was added and removed in the very
  // same frame, this should be ok (which can happen when, e.g.,
  // observers are registered in the constructor and unregistered
  // in the destructor and something fails in between.

  // note that the order: 1st: remove 2nd: add will resolve in remove!

  // first: register pending observers
  for (OBSSET::const_iterator cit2 = m_stPendingForRegistration.begin();
       cit2 != m_stPendingForRegistration.end(); ++cit2) {
    DoRegisterObserver((*cit2).first, (*cit2).second);
  }

  m_stPendingForRegistration.clear();

  // now: unregister pending observers.
  for (OBSSET::const_iterator cit = m_stPendingForUnregistration.begin();
       cit != m_stPendingForUnregistration.end(); ++cit) {
    DoUnregisterObserver((*cit).first, (*cit).second);
  }

  m_stPendingForUnregistration.clear();

  m_bObserverQueueDirty = false;
  return i;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   RegisterObserver                                            */
/*                                                                            */
/*============================================================================*/
bool VistaEventManager::RegisterObserver(VistaEventObserver* pObserver, int iEventType) {
  if (!pObserver)
    return false;
  if (m_iProcessRecursionCount == 0) {
#ifdef DEBUG
    vstr::outi() << "[ViEvMa] currently not processing -- registering observer directly..."
                 << std::endl;
#endif
    return DoRegisterObserver(pObserver, iEventType);
  } else {

#ifdef DEBUG
    vstr::outi() << "[ViEvMa] queuing observer for registration..." << std::endl;
#endif
    m_stPendingForRegistration.insert(std::pair<VistaEventObserver*, int>(pObserver, iEventType));
    m_bObserverQueueDirty = true;
    return true;
  }
}

bool VistaEventManager::DoRegisterObserver(VistaEventObserver* pObserver, int iEventType) {
#ifdef DEBUG
  vstr::outi() << "[ViEvMa] registering observer for event type " << iEventType << std::endl;
#endif

  if (iEventType == VistaEvent::VET_ALL) {
    for (std::vector<HANP>::size_type n = 0; n < m_veHandlerMapping.size(); ++n) {
      OBSQUEUE* pO = m_veHandlerMapping[n].second.second;
      (*pO).push_back(pObserver);
    }
    return true;
  } else {
    OBSQUEUE* pO = m_veHandlerMapping[iEventType].second.second;
    (*pO).push_back(pObserver);
    return true;
  }
}

bool VistaEventManager::DoUnregisterObserver(VistaEventObserver* pObserver, int iEventType) {
#ifdef DEBUG
  vstr::outi() << "[ViEvMa] unregistering observer for event type " << iEventType << std::endl;
#endif

  if (iEventType == VistaEvent::VET_ALL) {
    for (std::vector<HANP>::size_type n = 0; n < m_veHandlerMapping.size(); ++n) {
      OBSQUEUE* pO = m_veHandlerMapping[n].second.second;
      (*pO).remove(pObserver);
    }
    return true;
  } else {
    OBSQUEUE* pO = m_veHandlerMapping[iEventType].second.second;
    (*pO).remove(pObserver);
    return true;
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   UnregisterObserver                                          */
/*                                                                            */
/*============================================================================*/
bool VistaEventManager::UnregisterObserver(
    VistaEventObserver* pObserver, int iEventType /* = VistaEvent::VET_ALL */) {
  if (!pObserver)
    return false;

  if (m_iProcessRecursionCount == 0) {
#ifdef DEBUG
    vstr::outi() << "[ViEvMa] currently not processing -- registering observer directly..."
                 << std::endl;
#endif
    return DoUnregisterObserver(pObserver, iEventType);
  }

#ifdef DEBUG
  vstr::outi() << "[ViEvMa] queuing observer for unregistration..." << std::endl;
#endif

#ifdef DEBUG
  if (!GetIsObserver(pObserver, iEventType)) {
    vstr::outi() << "[ViEvMa]: requesting to unregister an non-registered observer." << std::endl;
  }
#endif
  m_stPendingForUnregistration.insert(std::pair<VistaEventObserver*, int>(pObserver, iEventType));
  m_bObserverQueueDirty = true;
  return true;
}
bool VistaEventManager::GetIsObserver(VistaEventObserver* pObserver, int iEventType) const {
  if (IsValidEventType(iEventType)) {
    OBSQUEUE* pO = m_veHandlerMapping[iEventType].second.second;
    if (std::find((*pO).begin(), (*pO).end(), pObserver) != (*pO).end())
      return true;
  }
  return false;
}

void VistaEventManager::SetResetEventTimeToLocalTime(bool bRETTLT) {
  m_bResetEventTimeToLocalTime = bRETTLT;
}
bool VistaEventManager::AddEventHandler(
    VistaEventHandler* pHan, EVENTTYPE nEventType, EVENTID nEventId, int nPrio) {
  if (nEventType == -1 || nEventId == -1) // reject invalids
    return false;

  // check for the existence of the event type
  if (nEventType != NVET_ALL) {
    if (nEventType > EVENTTYPE(m_veHandlerMapping.size()))
      return false;
    // retrieve id mapping for this type
    HANP& idmap = m_veHandlerMapping[nEventType];
    return AddToIdList(idmap, nEventId, nPrio, pHan);
  } else {
    // NVET_ALL
    // only valid when nEventId == NVET_ALL
    if (nEventId != NVET_ALL)
      return false;

    bool bRet = true;
    for (std::vector<HANP>::iterator it = m_veHandlerMapping.begin();
         it != m_veHandlerMapping.end(); ++it) {
      bRet &= AddToIdList((*it), nEventId, nPrio, pHan);
    }
    return bRet;
  }
}

bool VistaEventManager::AddToIdList(
    HANP& idmap, EVENTID nEventId, int nPrio, VistaEventHandler* pHan) {
  if (nEventId != NVET_ALL) {
    // check id mapping validity
    if (nEventId > EVENTID(idmap.second.first.size()))
      return false;

    // retrieve handler-list
    HQUEUE& hanlist = idmap.second.first[nEventId];

    HANQUEUE::const_iterator cit =
        std::find(hanlist.second->begin(), hanlist.second->end(), HANDLER(-1, pHan));
    if (cit == hanlist.second->end()) {
      hanlist.second->push_back(HANDLER(nPrio, pHan));
      hanlist.second->sort(_qCompPrio());
      return true;
    } else {
      return false; // handler already registered!
    }
  } else // NVET_ALL
  {
    std::vector<HQUEUE>& v = idmap.second.first;
    for (std::vector<HQUEUE>::const_iterator cit = v.begin(); cit != v.end(); ++cit) {
      HANQUEUE* p = (*cit).second;
      // test for existence
      HANQUEUE::const_iterator lookup = std::find(p->begin(), p->end(), HANDLER(-1, pHan));
      if (lookup == p->end()) {
        p->push_back(HANDLER(nPrio, pHan));
        p->sort(_qCompPrio());
      }
      // else: skip
    }
    return true;
  }
}

bool VistaEventManager::RemoveFromIdList(HANP& idmap, EVENTID nEventId, VistaEventHandler* pHan) {
  if (nEventId != NVET_ALL) {

    // check id mapping validity
    if (nEventId > EVENTID(idmap.second.first.size()))
      return false;

    // retrieve handler-list
    HQUEUE&            hanlist = idmap.second.first[nEventId];
    HANDLER            h(-1, pHan);
    HANQUEUE*          p  = hanlist.second;
    HANQUEUE::iterator it = std::find(p->begin(), p->end(), HANDLER(-1, pHan));
    if (it != p->end())
      p->erase(it);
    // we do not need to re-sort the list, removing
    // elements preserves the order of the others
  } else // NVET_ALL
  {
    const std::vector<HQUEUE>& v = idmap.second.first;
    for (std::vector<HQUEUE>::const_iterator cit = v.begin(); cit != v.end(); ++cit) {
      HANQUEUE*          p  = (*cit).second;
      HANQUEUE::iterator it = std::find(p->begin(), p->end(), HANDLER(-1, pHan));
      if (it != p->end())
        p->erase(it);
    }
  }
  return true;
}

bool VistaEventManager::RemEventHandler(
    VistaEventHandler* pHan, EVENTTYPE nEventType, EVENTID nEventId) {
  if (nEventType == -1 || nEventId == -1)
    return false;

  if (nEventType > 0 && nEventType > EVENTTYPE(m_veHandlerMapping.size()))
    return false;

  if (nEventType == VistaEventManager::NVET_ALL) {
    for (std::vector<HANP>::iterator it = m_veHandlerMapping.begin();
         it != m_veHandlerMapping.end(); ++it) {
      RemoveFromIdList(*it, nEventId, pHan);
    }
  } else {
    // retrieve id mapping for this type
    HANP& idmap = m_veHandlerMapping[nEventType];
    RemoveFromIdList(idmap, nEventId, pHan);
  }
  return true;
}

VistaEventManager::EVENTTYPE VistaEventManager::RegisterEventType(const std::string& sDebugString) {
  return AddEventType(sDebugString);
}

VistaEventManager::EVENTID VistaEventManager::RegisterEventId(
    int nEventType, const std::string& sDebugString) {
  return AddEventId(nEventType, sDebugString);
}

int VistaEventManager::AddEventType(const std::string& sDebugString) {
  m_veHandlerMapping.push_back(HANP(sDebugString, HOPAIR(std::vector<HQUEUE>(), new OBSQUEUE)));
  //#if defined(DEBUG)
  //	std::cout << "[ViEvMa]: VistaEventManager::AddEventType(["
  //			  << sDebugString << "]) -- returning type: "
  //			  << int(m_veHandlerMapping.size()-1) << "" << std::endl;
  //#endif
  return int(m_veHandlerMapping.size() - 1);
}

int VistaEventManager::AddEventId(int nEventType, const std::string& sDebugString) {
  if (std::vector<HANP>::size_type(nEventType) > m_veHandlerMapping.size())
    return -1;

  std::vector<HQUEUE>& hq = m_veHandlerMapping[nEventType].second.first;

  HANQUEUE* pL = new HANQUEUE;
  hq.push_back(HQUEUE(sDebugString, pL));

  //#if defined(DEBUG)
  //	std::cout << "[ViEvMa]: VistaEventManager::AddEventId(["
  //		<< m_veHandlerMapping[nEventType].first << ", "
  //		      << nEventType << "]; ["
  //			  << sDebugString << "]) -- returning id: "
  //			  << int(hq.size()-1) << "" << std::endl;
  //#endif
  return int(hq.size() - 1);
}

bool VistaEventManager::CleanupHandlerMapping() {
  // iterate over all types
  for (std::vector<HANP>::size_type nIdx = 0; nIdx < m_veHandlerMapping.size(); ++nIdx) {
    // iterate over all registered ids
    HANP                 hq = m_veHandlerMapping[nIdx];
    std::vector<HQUEUE>& h  = hq.second.first;
    delete hq.second.second; // clear obs-list

    for (std::vector<HQUEUE>::size_type nIdIdx = 0; nIdIdx < h.size(); ++nIdIdx) {
      // free memory for -> the pointer vector <-
      // NOT: the objects pointed at!
      delete h[nIdIdx].second;
    }
  }
  return true;
}

inline bool VistaEventManager::IsValidEventType(int iEventType) const {
  return (iEventType >= 0 && iEventType < int(m_veHandlerMapping.size()));
}

VistaEventManager::HANQUEUE* VistaEventManager::GetHandlerList(
    EVENTTYPE nEventType, EVENTID nEventId) const {
  const std::vector<HQUEUE>& hq = m_veHandlerMapping[nEventType].second.first;

  if (std::vector<HQUEUE>::size_type(nEventId) > hq.size())
    return NULL;

  return hq[nEventId].second;
}

int VistaEventManager::GetPriority(
    VistaEventHandler* pHandler, EVENTTYPE nEventType, EVENTID nEventId) const {

  if (!IsValidEventType(nEventType) || nEventId == -1)
    return -1;

  HANQUEUE* pL = GetHandlerList(nEventType, nEventId);
  if (pL == NULL)
    return -1;

  HANQUEUE::const_iterator cit = std::find((*pL).begin(), (*pL).end(), HANDLER(-1, pHandler));
  if (cit == (*pL).end())
    return -1;

  return (*cit).m_nPrio;
}

bool VistaEventManager::SetPriority(
    VistaEventHandler* pHandler, EVENTTYPE nEventType, EVENTID nEventId, int nPriority) {
  if (!IsValidEventType(nEventType) || nEventId == -1)
    return false;

  HANQUEUE* pL = GetHandlerList(nEventType, nEventId);
  if (pL == NULL)
    return false;

  HANQUEUE::iterator cit = std::find((*pL).begin(), (*pL).end(), HANDLER(-1, pHandler));
  if (cit == (*pL).end())
    return false;

  (*cit).m_nPrio = nPriority;
  (*pL).sort(_qCompPrio());

  return true;
}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaEventManager::Debug(std::ostream& out) const {
  out << "[ViEvMa]: number of registered event types: " << m_veHandlerMapping.size() << std::endl;
  for (std::vector<HANP>::size_type n = 0; n < m_veHandlerMapping.size(); ++n) {
    const std::vector<HQUEUE>& hq = m_veHandlerMapping[n].second.first;
    out << "  [\"" << m_veHandlerMapping[n].first << "\", " << n << "] has " << hq.size()
        << " ids registered." << std::endl;

    const OBSQUEUE* pO = m_veHandlerMapping[n].second.second;
    out << "  Observers: " << (*pO).size() << std::endl;
    for (OBSQUEUE::const_iterator kit = (*pO).begin(); kit != (*pO).end(); ++kit) {
      out << "\t[" << (*kit) << "]" << std::endl;
    }

    for (std::vector<HQUEUE>::size_type k = 0; k < hq.size(); ++k) {
      HANQUEUE* pL = hq[k].second;
      out << "\t[\"" << hq[k].first << "\", " << k << "] has (" << (*pL).size()
          << ") handlers registered." << std::endl;
      for (HANQUEUE::const_iterator cit = (*pL).begin(); cit != (*pL).end(); ++cit) {
        out << "\t\t[" << (*cit).m_pHandler << "], prio: " << (*cit).m_nPrio << "\t("
            << ((*cit).m_pHandler->GetIsEnabled() ? "enabled" : "disabled") << ")\t["
            << (*cit).m_pHandler->GetHandlerToken() << "]" << std::endl;
      }
    }
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
std::ostream& operator<<(std::ostream& oStream, const VistaEventManager& oDevice) {
  oDevice.Debug(oStream);
  return oStream;
}
