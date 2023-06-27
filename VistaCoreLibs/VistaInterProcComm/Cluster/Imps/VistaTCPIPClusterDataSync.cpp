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

#include "VistaTCPIPClusterDataSync.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cassert>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

struct VistaTCPIPClusterLeaderDataSync::Follower {
  int                m_nID;
  VistaConnectionIP* m_pConn;
  std::string        m_sName;
  bool               m_bManageConnDeletion;
};

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaTCPIPClusterLeaderDataSync::VistaTCPIPClusterLeaderDataSync(const bool bVerbose)
    : VistaClusterBytebufferLeaderDataSyncBase(bVerbose)
    , m_nBlockByteCount(0) {
}

VistaTCPIPClusterLeaderDataSync::~VistaTCPIPClusterLeaderDataSync() {
  for (std::vector<Follower*>::iterator itFollower = m_vecFollowers.begin();
       itFollower != m_vecFollowers.end(); ++itFollower) {
    if ((*itFollower)->m_bManageConnDeletion)
      delete (*itFollower)->m_pConn;
    delete (*itFollower);
  }
}
bool VistaTCPIPClusterLeaderDataSync::GetIsValid() const {
  return (m_vecAliveFollowers.empty() == false);
}

int VistaTCPIPClusterLeaderDataSync::AddFollower(
    const std::string& sName, VistaConnectionIP* pConnection, const bool bManageDeletion) {
  Follower* pFollower = new Follower;
  pFollower->m_nID    = (int)m_vecFollowers.size();
  pFollower->m_sName  = sName;
  if (pConnection != NULL && pConnection->GetIsConnected())
    pFollower->m_pConn = pConnection;
  else
    pFollower->m_pConn = NULL;
  pFollower->m_bManageConnDeletion = bManageDeletion;
  m_vecFollowers.push_back(pFollower);
  if (pFollower->m_pConn != NULL)
    m_vecAliveFollowers.push_back(pFollower);
  m_nLastChangedFollower = pFollower->m_nID;
  Notify(MSG_FOLLOWER_ADDED);
  return true;
}

bool VistaTCPIPClusterLeaderDataSync::DoSendMessage() {
  std::vector<Follower*> vecDiedFollowers;

  // we never swap bytes - leave that to the slaves
  for (std::vector<Follower*>::iterator itFollower = m_vecAliveFollowers.begin();
       itFollower != m_vecAliveFollowers.end(); ++itFollower) {
    try {
      VistaConnectionIP* pConn = (*itFollower)->m_pConn;
      if (pConn == NULL)
        continue;
      int nTotalSize = m_oMessage.GetBufferSize();
      if (m_pExtBuffer)
        nTotalSize += m_nExtBufferSize;
      if (m_nBlockByteCount < 0 || nTotalSize < m_nBlockByteCount)
        pConn->SetIsBlocking(false);
      else
        pConn->SetIsBlocking(true);

      int nReturn = pConn->WriteRawBuffer(m_oMessage.GetBuffer(), m_oMessage.GetBufferSize());
      if (nReturn != m_oMessage.GetBufferSize()) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[VistaTCPIPClusterLeaderSync]: "
                        << "Writing data for follower " << (*itFollower)->m_sName
                        << " failed - removing it from sync list" << std::endl;
        }
        vecDiedFollowers.push_back((*itFollower));
        continue;
      } else if (m_pExtBuffer) {
        if (pConn->WriteRawBuffer(m_pExtBuffer, m_nExtBufferSize) != m_nExtBufferSize) {
          if (GetIsVerbose()) {
            vstr::warnp() << "[VistaTCPIPClusterLeaderSync]: "
                          << "Writing data for follower " << (*itFollower)->m_sName
                          << " failed - removing it from sync list" << std::endl;
          }
          vecDiedFollowers.push_back((*itFollower));
          continue;
        }
      }
    } catch (VistaExceptionBase&) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[VistaTCPIPClusterLeaderSync]: "
                      << "Exception while writing data for follower " << (*itFollower)->m_sName
                      << " - removing it from sync list" << std::endl;
      }
      vecDiedFollowers.push_back((*itFollower));
      continue;
    }
  }

  // kill dead followers
  for (std::vector<Follower*>::iterator itDied = vecDiedFollowers.begin();
       itDied != vecDiedFollowers.end(); ++itDied) {
    RemoveFollower((*itDied));
  }

  return GetIsValid();
}

std::string VistaTCPIPClusterLeaderDataSync::GetDataSyncType() const {
  return "TCPIPLeaderSync";
}

int VistaTCPIPClusterLeaderDataSync::GetNumberOfFollowers() const {
  return (int)m_vecFollowers.size();
}
int VistaTCPIPClusterLeaderDataSync::GetNumberOfActiveFollowers() const {
  return (int)m_vecAliveFollowers.size();
}
int VistaTCPIPClusterLeaderDataSync::GetNumberOfDeadFollowers() const {
  return (int)(m_vecFollowers.size() - m_vecAliveFollowers.size());
}
std::string VistaTCPIPClusterLeaderDataSync::GetFollowerNameForId(const int nID) const {
  return m_vecFollowers[nID]->m_sName;
}
int VistaTCPIPClusterLeaderDataSync::GetFollowerIdForName(const std::string& sName) const {
  for (std::vector<Follower*>::const_iterator itFollower = m_vecFollowers.begin();
       itFollower != m_vecFollowers.end(); ++itFollower) {
    if ((*itFollower)->m_sName == sName)
      return (*itFollower)->m_nID;
  }
  return -1;
}
bool VistaTCPIPClusterLeaderDataSync::GetFollowerIsAlive(const int nID) const {
  return (m_vecFollowers[nID]->m_pConn != NULL);
}
int VistaTCPIPClusterLeaderDataSync::GetLastChangedFollower() {
  return m_nLastChangedFollower;
}

bool VistaTCPIPClusterLeaderDataSync::RemoveFollower(Follower* pFollower) {
  if (pFollower->m_bManageConnDeletion)
    delete pFollower->m_pConn;
  pFollower->m_pConn = NULL;

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

bool VistaTCPIPClusterLeaderDataSync::DeactivateFollower(const std::string& sName) {
  int nID = GetFollowerIdForName(sName);
  if (nID == -1)
    return false;
  return DeactivateFollower(nID);
}

bool VistaTCPIPClusterLeaderDataSync::DeactivateFollower(const int nID) {
  return RemoveFollower(m_vecFollowers[nID]);
}

bool VistaTCPIPClusterLeaderDataSync::SetSendBlockingThreshold(const int nNumBytes) {
  m_nBlockByteCount = nNumBytes;
  return true;
}

int VistaTCPIPClusterLeaderDataSync::GetSendBlockingThreshold() const {
  return m_nBlockByteCount;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaTCPIPClusterFollowerDataSync::VistaTCPIPClusterFollowerDataSync(VistaConnectionIP* pLeaderConn,
    const bool bSwap, const bool bManageDeletion, const bool bVerbose)
    : VistaClusterBytebufferFollowerDataSyncBase(bSwap, bVerbose)
    , m_pLeaderConn(pLeaderConn)
    , m_bManageDeletion(bManageDeletion) {
  if (pLeaderConn->GetIsConnected() == false) {
    if (GetIsVerbose()) {
      vstr::errp() << "[VistaTCPIPClusterSyncLeader]: "
                   << "Received leader connection is not connected - syncing will not work"
                   << std::endl;
    }
    Cleanup();
  }
}

VistaTCPIPClusterFollowerDataSync::~VistaTCPIPClusterFollowerDataSync() {
  Cleanup();
}

bool VistaTCPIPClusterFollowerDataSync::GetIsValid() const {

  return (m_pLeaderConn && m_pLeaderConn->GetIsConnected());
}

void VistaTCPIPClusterFollowerDataSync::Cleanup() {
  if (m_bManageDeletion) {
    delete m_pLeaderConn;
  }
  m_pLeaderConn = NULL;
}

bool VistaTCPIPClusterFollowerDataSync::DoReceiveMessage() {
  if (GetIsValid() == false)
    return false;

  try {
    m_pLeaderConn->SetReadTimeout(0);
    m_pLeaderConn->SetIsBlocking(true);
    m_pLeaderConn->SetByteorderSwapFlag(
        m_bSwap ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
    VistaType::sint32 nMessageSize;
    if (m_pLeaderConn->ReadInt32(nMessageSize) != (int)sizeof(VistaType::sint32)) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[VistaTCPIPClusterFollowerSync]: "
                      << "Error receiving message from leader" << std::endl;
      }
      Cleanup();
      return false;
    }
    if ((int)m_vecMessageBuffer.size() < nMessageSize)
      m_vecMessageBuffer.resize(nMessageSize);
    if (m_pLeaderConn->ReadRawBuffer(&m_vecMessageBuffer[0], nMessageSize) != nMessageSize) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[VistaTCPIPClusterFollowerSync]: "
                      << "Error receiving message from leader" << std::endl;
      }
      Cleanup();
      return false;
    }
    m_oMessage.SetBuffer(&m_vecMessageBuffer[0], nMessageSize);
  } catch (VistaExceptionBase&) {
    if (GetIsVerbose()) {
      vstr::warnp() << "[VistaTCPIPClusterFollowerSync]: "
                    << "Exception while receiving message from leader" << std::endl;
    }
    Cleanup();
    return false;
  }

  return true;
}

std::string VistaTCPIPClusterFollowerDataSync::GetDataSyncType() const {
  return "TCPIPFollowerSync";
}

bool VistaTCPIPClusterFollowerDataSync::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}
int VistaTCPIPClusterFollowerDataSync::GetSendBlockingThreshold() const {
  return -1;
}
