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

#include "VistaClusterBarrierIPBase.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

VistaClusterLeaderBarrierIPBase::Follower::Follower(
    const std::string& sName, VistaConnectionIP* pConn, bool bDelete, int nID)
    : m_sName(sName)
    , m_pLeaderConn(pConn)
    , m_bManageConnDeletion(bDelete)
    , m_nID(nID)
    , m_nSyncFailures(0) {
}

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaClusterLeaderBarrierIPBase::VistaClusterLeaderBarrierIPBase(const bool bVerbose)
    : IVistaClusterBarrier(bVerbose, true)
    , m_nBarrierWaitCount(0)
    , m_nAllowedConsecutiveSyncFailures(10) {
}

VistaClusterLeaderBarrierIPBase::~VistaClusterLeaderBarrierIPBase() {
  for (std::vector<Follower*>::iterator itFollower = m_vecFollowers.begin();
       itFollower != m_vecFollowers.end(); ++itFollower) {
    if ((*itFollower)->m_bManageConnDeletion)
      delete (*itFollower)->m_pLeaderConn;
    delete (*itFollower);
  }
}

int VistaClusterLeaderBarrierIPBase::AddFollower(const std::string& sName,
    VistaConnectionIP* pConnection, const bool bManageConnectionDeletion) {
  m_nLastChangedFollower = (int)m_vecFollowers.size();
  if (pConnection == NULL || pConnection->GetIsConnected() == false) {
    if (GetIsVerbose()) {
      vstr::warnp() << "[" << GetBarrierType() << "]: "
                    << "Received invalid leader connection for Follower [" << sName
                    << "] - syncing on it will not work" << std::endl;
    }
    m_vecFollowers.push_back(
        new Follower(sName, NULL, bManageConnectionDeletion, m_nLastChangedFollower));
  } else {
    Follower* pFollower =
        new Follower(sName, pConnection, bManageConnectionDeletion, m_nLastChangedFollower);
    m_vecFollowers.push_back(pFollower);
    m_vecAliveFollowers.push_back(pFollower);
  }

  Notify(MSG_FOLLOWER_ADDED);
  return m_nLastChangedFollower;
}

bool VistaClusterLeaderBarrierIPBase::WaitForAllFollowers(int iTimeOut) {
  ++m_nBarrierWaitCount;

  std::vector<Follower*> vecDiedFollowers;

  // first: wait for all acks
  for (std::vector<Follower*>::iterator itFollower = m_vecAliveFollowers.begin();
       itFollower != m_vecAliveFollowers.end(); ++itFollower) {
    (*itFollower)->m_pLeaderConn->SetReadTimeout(iTimeOut);
    (*itFollower)->m_pLeaderConn->SetIsBlocking(true);

    VistaType::sint32 nAckCounter = -1;
    try {
      int nRes = (*itFollower)->m_pLeaderConn->ReadInt32(nAckCounter);
      if (nRes == 0) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[" << GetBarrierType() << "]: "
                        << "Timeout while reading Ack from follower [" << (*itFollower)->m_sName
                        << "] " << std::endl;
        }
        if (++(*itFollower)->m_nSyncFailures >= m_nAllowedConsecutiveSyncFailures) {
          vstr::errp() << "[" << GetBarrierType() << "]: " << (*itFollower)->m_nSyncFailures
                       << " consecutive sync failures for [" << (*itFollower)->m_sName
                       << "] - removing it from sync list" << std::endl;
          vecDiedFollowers.push_back((*itFollower));
        }
        continue;
      } else if (nRes != sizeof(VistaType::sint32)) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[" << GetBarrierType() << "]: "
                        << "Reading Ack from follower [" << (*itFollower)->m_sName
                        << "] failed - removing it from sync list" << std::endl;
        }
        vecDiedFollowers.push_back((*itFollower));
        continue;
      }
      if (nAckCounter != m_nBarrierWaitCount) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[" << GetBarrierType() << "]: "
                        << "Follower [" << (*itFollower)->m_sName << "] reported count ["
                        << nAckCounter << "], but expected [" << m_nBarrierWaitCount << "] "
                        << std::endl;
        }
        if (++(*itFollower)->m_nSyncFailures >= m_nAllowedConsecutiveSyncFailures) {
          vstr::errp() << "[" << GetBarrierType() << "]: " << m_nAllowedConsecutiveSyncFailures
                       << " consecutive sync failures for [" << (*itFollower)->m_sName
                       << "] - removing it from sync list" << std::endl;
          vecDiedFollowers.push_back((*itFollower));
        }
        continue;
      } else
        (*itFollower)->m_nSyncFailures = 0;

      (*itFollower)->m_pLeaderConn->SetReadTimeout(0);
    } catch (VistaExceptionBase&) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[" << GetBarrierType() << "]: "
                      << "Exception while reading ack from follower [" << (*itFollower)->m_sName
                      << "] - removing it from sync list" << std::endl;
      }
      vecDiedFollowers.push_back((*itFollower));
    }
  }

  // kill dead followers
  for (std::vector<Follower*>::iterator itDied = vecDiedFollowers.begin();
       itDied != vecDiedFollowers.end(); ++itDied) {
    RemoveFollower((*itDied));
  }
  vecDiedFollowers.clear();

  return (m_vecFollowers.empty() == false);
}

bool VistaClusterLeaderBarrierIPBase::GetIsValid() const {
  // we are valid as long as we have at least one valid slave
  if (m_vecAliveFollowers.empty())
    return false;
  return true;
}

bool VistaClusterLeaderBarrierIPBase::RemoveFollower(Follower* pFollower) {
  if (pFollower->m_bManageConnDeletion)
    delete pFollower->m_pLeaderConn;
  pFollower->m_pLeaderConn = NULL;

  bool bFound = false;
  for (std::vector<Follower*>::iterator itFollower = m_vecAliveFollowers.begin();
       itFollower != m_vecAliveFollowers.end(); ++itFollower) {
    if ((*itFollower)->m_nID == pFollower->m_nID) {
      m_vecAliveFollowers.erase(itFollower);
      bFound = true;
      break;
    }
  }

  if (bFound) {
    m_nLastChangedFollower = pFollower->m_nID;
    Notify(MSG_FOLLOWER_LOST);
  }
  return bFound;
}

bool VistaClusterLeaderBarrierIPBase::DeactivateFollower(const std::string& sName) {
  int nID = GetFollowerIdForName(sName);
  if (nID == -1)
    return false;
  return DeactivateFollower(nID);
}

bool VistaClusterLeaderBarrierIPBase::DeactivateFollower(const int nID) {
  return RemoveFollower(m_vecFollowers[nID]);
}

int VistaClusterLeaderBarrierIPBase::GetNumberOfFollowers() const {
  return (int)m_vecFollowers.size();
}
int VistaClusterLeaderBarrierIPBase::GetNumberOfActiveFollowers() const {
  return (int)m_vecAliveFollowers.size();
}
int VistaClusterLeaderBarrierIPBase::GetNumberOfDeadFollowers() const {
  return (int)(m_vecFollowers.size() - m_vecAliveFollowers.size());
}
std::string VistaClusterLeaderBarrierIPBase::GetFollowerNameForId(const int nID) const {
  return m_vecFollowers[nID]->m_sName;
}
int VistaClusterLeaderBarrierIPBase::GetFollowerIdForName(const std::string& sName) const {
  for (std::vector<Follower*>::const_iterator itFollower = m_vecFollowers.begin();
       itFollower != m_vecFollowers.end(); ++itFollower) {
    if ((*itFollower)->m_sName == sName)
      return (*itFollower)->m_nID;
  }
  return -1;
}
bool VistaClusterLeaderBarrierIPBase::GetFollowerIsAlive(const int nID) const {
  return (m_vecFollowers[nID]->m_pLeaderConn != NULL);
}
int VistaClusterLeaderBarrierIPBase::GetLastChangedFollower() {
  return m_nLastChangedFollower;
}

VistaType::sint32 VistaClusterLeaderBarrierIPBase::GetBarrierWaitCount() {
  return m_nBarrierWaitCount;
}

std::vector<VistaClusterLeaderBarrierIPBase::Follower*>&
VistaClusterLeaderBarrierIPBase::GetFollowersRef() {
  return m_vecAliveFollowers;
}

bool VistaClusterLeaderBarrierIPBase::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}

int VistaClusterLeaderBarrierIPBase::GetSendBlockingThreshold() const {
  return -1;
}

int VistaClusterLeaderBarrierIPBase::GetAllowedConsecutiveSyncFailures() const {
  return m_nAllowedConsecutiveSyncFailures;
}

void VistaClusterLeaderBarrierIPBase::SetAllowedConsecutiveSyncFailures(const int oValue) {
  m_nAllowedConsecutiveSyncFailures = oValue;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaClusterFollowerBarrierIPBase::VistaClusterFollowerBarrierIPBase(
    VistaConnectionIP* pLeaderConnection, const bool bManageConnection, const bool bVerbose)
    : IVistaClusterBarrier(bVerbose, false)
    , m_bManageConnection(bManageConnection)
    , m_pLeaderConn(pLeaderConnection)
    , m_bSwap(pLeaderConnection->GetByteorderSwapFlag() ==
              VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES)
    , m_nBarrierWaitCount(0) {
  if (m_pLeaderConn == NULL || m_pLeaderConn->GetIsConnected() == false) {
    vstr::errp() << "[VistaClusterFollowerBarrierIPBase]: "
                 << "Received leader connection is not connected - syncing will not work"
                 << std::endl;
    if (m_bManageConnection)
      delete m_pLeaderConn;
    m_pLeaderConn = NULL;
  }
}

VistaClusterFollowerBarrierIPBase::~VistaClusterFollowerBarrierIPBase() {
  if (m_bManageConnection)
    delete m_pLeaderConn;
}

bool VistaClusterFollowerBarrierIPBase::GetIsValid() const {
  return (m_pLeaderConn != NULL);
}

bool VistaClusterFollowerBarrierIPBase::SendReadyTokenToLeader(int iTimeOut) {
  ++m_nBarrierWaitCount;

  try {
    if (m_pLeaderConn->WriteInt32(m_nBarrierWaitCount) != sizeof(VistaType::sint32)) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[" << GetBarrierType() << "]: "
                      << "Error while sending ready signal to leader" << std::endl;
      }
      ProcessError();
      return false;
    }
  } catch (VistaExceptionBase&) {
    if (GetIsVerbose()) {
      vstr::warnp() << "[" << GetBarrierType() << "]: "
                    << "Exception during BarrierWait" << std::endl;
    }
    ProcessError();
    return false;
  }

  return true;
}

void VistaClusterFollowerBarrierIPBase::ProcessError() {
  if (m_bManageConnection) {
    delete m_pLeaderConn;
  }
  m_pLeaderConn = NULL;

  Notify(MSG_FATAL_ERROR);
}

VistaType::sint32 VistaClusterFollowerBarrierIPBase::GetBarrierWaitCount() const {
  return m_nBarrierWaitCount;
}

void VistaClusterFollowerBarrierIPBase::SetBarrierWaitCount(VistaType::sint32 nCount) {
  m_nBarrierWaitCount = nCount;
}

bool VistaClusterFollowerBarrierIPBase::GetDoesSwap() const {
  return m_bSwap;
}

VistaConnectionIP* VistaClusterFollowerBarrierIPBase::GetConnection() {
  return m_pLeaderConn;
}

bool VistaClusterFollowerBarrierIPBase::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}

int VistaClusterFollowerBarrierIPBase::GetSendBlockingThreshold() const {
  return -1;
}
