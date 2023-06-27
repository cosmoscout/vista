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

#include "VistaTCPIPClusterDataCollect.h"

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

enum {
  COLLECT_TIME = 42030,
  COLLECT_PROPLIST,
  COLLECT_SERIALIZABLE,
  COLLECT_DATA,
};

struct VistaTCPIPClusterLeaderDataCollect::Follower {
  int                m_nID;
  VistaConnectionIP* m_pConn;
  std::string        m_sName;
  bool               m_bManageConnDeletion;
};

#define VERIFY_READ(readcall)                                                                      \
  {                                                                                                \
    if (readcall <= 0) {                                                                           \
      VISTA_THROW("TCPIPClusterDataCollect Protocol Error", -1);                                   \
    }                                                                                              \
  }
#define VERIFY_READ_SIZE(readcall, size)                                                           \
  {                                                                                                \
    if (readcall != size) {                                                                        \
      VISTA_THROW("TCPIPClusterDataCollect Protocol Error", -1);                                   \
    }                                                                                              \
  }

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaTCPIPClusterLeaderDataCollect::VistaTCPIPClusterLeaderDataCollect(const bool bVerbose)
    : IVistaClusterDataCollect(bVerbose, true)
    , m_nCollectCount(0) {
  m_oDeSer.SetByteorderSwapFlag(
      VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES); // only slaves are responsible for
                                                                // swapping
}

VistaTCPIPClusterLeaderDataCollect::~VistaTCPIPClusterLeaderDataCollect() {
  for (std::vector<Follower*>::iterator itFollower = m_vecFollowers.begin();
       itFollower != m_vecFollowers.end(); ++itFollower) {
    if ((*itFollower)->m_bManageConnDeletion)
      delete (*itFollower)->m_pConn;
    delete (*itFollower);
  }
}
bool VistaTCPIPClusterLeaderDataCollect::GetIsValid() const {
  return (m_vecAliveFollowers.empty() == false);
}

int VistaTCPIPClusterLeaderDataCollect::AddFollower(
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

bool VistaTCPIPClusterLeaderDataCollect::CollectTime(
    const VistaType::systemtime nOwnTime, std::vector<VistaType::systemtime>& vecCollected) {
  // first: our own, we are the master
  vecCollected.clear();
  vecCollected.push_back(nOwnTime);
  VistaType::systemtime nReadTime;
  // now, iterate over all followers and collect their data
  for (std::size_t i = 0; i < m_vecAliveFollowers.size(); ++i) {
    if (ReceivePartFromFollower(COLLECT_TIME, m_vecAliveFollowers[i])) {
      VERIFY_READ_SIZE(m_oDeSer.ReadDouble(nReadTime), sizeof(double));
      vecCollected.push_back(nReadTime);
    } else {
      --i; // one follower was removed -> next one is at pos i-1
    }
  }
  ++m_nCollectCount;
  return GetIsValid();
}
bool VistaTCPIPClusterLeaderDataCollect::CollectData(
    const VistaPropertyList& oList, std::vector<VistaPropertyList>& vecCollected) {
  // first: our own, we are the master
  vecCollected.clear();
  vecCollected.push_back(oList);
  VistaPropertyList oReadList;
  // now, iterate over all followers and collect their data
  for (std::size_t i = 0; i < m_vecAliveFollowers.size(); ++i) {
    if (ReceivePartFromFollower(COLLECT_PROPLIST, m_vecAliveFollowers[i])) {
      oReadList.clear();
      std::string sDummy;
      VistaPropertyList::DeSerializePropertyList(m_oDeSer, oReadList, sDummy);
      vecCollected.push_back(oReadList);
    } else {
      --i; // one follower was removed -> next one is at pos i-1
    }
  }
  ++m_nCollectCount;
  return GetIsValid();
}
bool VistaTCPIPClusterLeaderDataCollect::CollectData(const VistaType::byte* pDataBuffer,
    const int iBufferSize, std::vector<std::vector<VistaType::byte>>& vecCollected) {
  // first: our own, we are the master
  vecCollected.clear();
  vecCollected.push_back(std::vector<VistaType::byte>());
  vecCollected.back().resize(iBufferSize);
  memcpy(&vecCollected[0][0], pDataBuffer, iBufferSize);
  // now, iterate over all followers and collect their data
  for (std::size_t i = 0; i < m_vecAliveFollowers.size(); ++i) {
    if (ReceivePartFromFollower(COLLECT_DATA, m_vecAliveFollowers[i])) {
      VistaType::sint32 nSize = -1;
      VERIFY_READ_SIZE(m_oDeSer.ReadInt32(nSize), sizeof(VistaType::sint32));
      assert(nSize > 0);
      vecCollected.push_back(std::vector<VistaType::byte>());
      vecCollected.back().resize(nSize);
      VERIFY_READ_SIZE(m_oDeSer.ReadRawBuffer(&vecCollected.back()[0], nSize), nSize);
    } else {
      --i; // one follower was removed -> next one is at pos i-1
    }
  }
  ++m_nCollectCount;
  return GetIsValid();
}

std::string VistaTCPIPClusterLeaderDataCollect::GetDataCollectType() const {
  return "VistaTCPIPClusterLeaderDataCollect";
}

int VistaTCPIPClusterLeaderDataCollect::GetNumberOfFollowers() const {
  return (int)m_vecFollowers.size();
}
int VistaTCPIPClusterLeaderDataCollect::GetNumberOfActiveFollowers() const {
  return (int)m_vecAliveFollowers.size();
}
int VistaTCPIPClusterLeaderDataCollect::GetNumberOfDeadFollowers() const {
  return (int)(m_vecFollowers.size() - m_vecAliveFollowers.size());
}
std::string VistaTCPIPClusterLeaderDataCollect::GetFollowerNameForId(const int nID) const {
  return m_vecFollowers[nID]->m_sName;
}
int VistaTCPIPClusterLeaderDataCollect::GetFollowerIdForName(const std::string& sName) const {
  for (std::vector<Follower*>::const_iterator itFollower = m_vecFollowers.begin();
       itFollower != m_vecFollowers.end(); ++itFollower) {
    if ((*itFollower)->m_sName == sName)
      return (*itFollower)->m_nID;
  }
  return -1;
}
bool VistaTCPIPClusterLeaderDataCollect::GetFollowerIsAlive(const int nID) const {
  return (m_vecFollowers[nID]->m_pConn != NULL);
}
int VistaTCPIPClusterLeaderDataCollect::GetLastChangedFollower() {
  return m_nLastChangedFollower;
}

bool VistaTCPIPClusterLeaderDataCollect::RemoveFollower(Follower* pFollower) {
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

bool VistaTCPIPClusterLeaderDataCollect::DeactivateFollower(const std::string& sName) {
  int nID = GetFollowerIdForName(sName);
  if (nID == -1)
    return false;
  return DeactivateFollower(nID);
}

bool VistaTCPIPClusterLeaderDataCollect::DeactivateFollower(const int nID) {
  return RemoveFollower(m_vecFollowers[nID]);
}

bool VistaTCPIPClusterLeaderDataCollect::ReceivePartFromFollower(
    int nExpectedType, Follower* pFollower, const int nTimeout) {
  try {
    VistaType::sint32 nSize = -1;
    VERIFY_READ_SIZE(pFollower->m_pConn->ReadInt32(nSize), sizeof(VistaType::sint32));
    assert(nSize > 0);
    if ((int)m_vecDeSerData.size() < nSize)
      m_vecDeSerData.resize(nSize);
    VERIFY_READ_SIZE(pFollower->m_pConn->ReadRawBuffer(&m_vecDeSerData[0], nSize), nSize);
    m_oDeSer.SetBuffer(&m_vecDeSerData[0], nSize);
  } catch (VistaExceptionBase&) {
    vstr::warnp() << "[VistaTCPIPClusterLeaderDataCollect]: "
                  << "Exception while reading data from follower " << pFollower->m_sName
                  << "] - removing it" << std::endl;
    RemoveFollower(pFollower);
    return false;
  }

  VistaType::uint64 nReadCount = -1;
  VERIFY_READ_SIZE(m_oDeSer.ReadUInt64(nReadCount), sizeof(VistaType::uint64));
  VistaType::sint32 nReadType = -1;
  VERIFY_READ_SIZE(m_oDeSer.ReadInt32(nReadType), sizeof(VistaType::sint32));
  if (nReadCount != m_nCollectCount)
    VISTA_THROW("TCPIPDataCollect Protocol Error", -1);
  if (nReadType != nExpectedType)
    VISTA_THROW("TCPIPDataCollect Protocol Error", -1);

  return true;
}

bool VistaTCPIPClusterLeaderDataCollect::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}

int VistaTCPIPClusterLeaderDataCollect::GetSendBlockingThreshold() const {
  return -1;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaTCPIPClusterFollowerDataCollect::VistaTCPIPClusterFollowerDataCollect(
    VistaConnectionIP* pLeaderConn, const bool bSwap, const bool bManageDeletion,
    const bool bVerbose)
    : IVistaClusterDataCollect(bVerbose, false)
    , m_nCollectCount(0)
    , m_pLeaderConn(pLeaderConn)
    , m_bManageDeletion(bManageDeletion)
    , m_nBlockByteCount(0) {
  if (pLeaderConn->GetIsConnected() == false) {
    if (GetIsVerbose()) {
      vstr::errp() << "[VistaTCPIPClusterFollowerDataCollect]: "
                   << "Received leader connection is not connected - Collecting will not work"
                   << std::endl;
    }
    Cleanup();
  }

  m_oMessage.SetByteorderSwapFlag(pLeaderConn->GetByteorderSwapFlag());
  m_oMessage.WriteInt32(0); // dummy for size
  m_oMessage.WriteUInt64((VistaType::sint32)m_nCollectCount);
}

VistaTCPIPClusterFollowerDataCollect::~VistaTCPIPClusterFollowerDataCollect() {
  Cleanup();
}

bool VistaTCPIPClusterFollowerDataCollect::GetIsValid() const {

  return (m_pLeaderConn && m_pLeaderConn->GetIsConnected());
}

void VistaTCPIPClusterFollowerDataCollect::Cleanup() {
  if (m_bManageDeletion) {
    delete m_pLeaderConn;
  }
  m_pLeaderConn = NULL;
}

bool VistaTCPIPClusterFollowerDataCollect::CollectTime(
    const VistaType::systemtime nOwnTime, std::vector<VistaType::systemtime>& vecCollected) {
  m_oMessage.WriteInt32(COLLECT_TIME);
  m_oMessage.WriteDouble(nOwnTime);
  return SendMessage();
}
bool VistaTCPIPClusterFollowerDataCollect::CollectData(
    const VistaPropertyList& oList, std::vector<VistaPropertyList>& vecCollected) {
  m_oMessage.WriteInt32(COLLECT_PROPLIST);
  VistaPropertyList::SerializePropertyList(m_oMessage, oList, "");
  return SendMessage();
}
bool VistaTCPIPClusterFollowerDataCollect::CollectData(const VistaType::byte* pDataBuffer,
    const int iBufferSize, std::vector<std::vector<VistaType::byte>>& vecCollected) {
  m_oMessage.WriteInt32(COLLECT_DATA);
  m_oMessage.WriteInt32(iBufferSize);
  m_oMessage.WriteRawBuffer(pDataBuffer, iBufferSize);
  return SendMessage();
}

bool VistaTCPIPClusterFollowerDataCollect::SendMessage() {
  if (GetIsValid() == false)
    return false;

  // rewrite message size
  VistaType::sint32 nSize = m_oMessage.GetBufferSize() - sizeof(VistaType::sint32);
  // rewrite first dummy bite with size
  m_oMessage.SetBufferRewritePosition(0);
  m_oMessage.WriteInt32(nSize);
  // VistaType::byte* pSizeMem = const_cast<VistaType::byte*>( m_oMessage.GetBuffer() );
  // memcpy( pSizeMem, &nSize, sizeof(VistaType::sint32) );

  try {
    if (m_nBlockByteCount < 0 || m_oMessage.GetBufferSize() < m_nBlockByteCount)
      m_pLeaderConn->SetIsBlocking(false);
    else
      m_pLeaderConn->SetIsBlocking(true);
    int nReturn = m_pLeaderConn->WriteRawBuffer(m_oMessage.GetBuffer(), m_oMessage.GetBufferSize());
    if (nReturn != m_oMessage.GetBufferSize())
      VISTA_THROW("VistaTCPIPDataCollect Connection Error", -1);
  } catch (VistaExceptionBase&) {
    if (GetIsVerbose()) {
      vstr::warnp() << "[VistaTCPIPClusterFollowerCollect]: "
                    << "Exception while writing message to leader" << std::endl;
    }
    Cleanup();
    return false;
  }

  ++m_nCollectCount;

  // prepare message for next send
  m_oMessage.ClearBuffer();
  m_oMessage.WriteInt32(0); // dummy for size
  m_oMessage.WriteUInt64(m_nCollectCount);

  return true;
}

std::string VistaTCPIPClusterFollowerDataCollect::GetDataCollectType() const {
  return "TCPIPFollowerCollect";
}

bool VistaTCPIPClusterFollowerDataCollect::SetSendBlockingThreshold(const int nNumBytes) {
  m_nBlockByteCount = nNumBytes;
  return true;
}

int VistaTCPIPClusterFollowerDataCollect::GetSendBlockingThreshold() const {
  return m_nBlockByteCount;
}
