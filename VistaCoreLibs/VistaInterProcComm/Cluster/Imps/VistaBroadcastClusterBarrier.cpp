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

#include "VistaBroadcastClusterBarrier.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaBroadcastClusterLeaderBarrier::VistaBroadcastClusterLeaderBarrier(
    const std::string& sBroadcastIP, const int nBroadcastPort, const bool bVerbose)
    : VistaClusterLeaderBarrierIPBase(bVerbose) {
  AddBroadcast(sBroadcastIP, nBroadcastPort);
}

VistaBroadcastClusterLeaderBarrier::VistaBroadcastClusterLeaderBarrier(
    const std::string& sBroadcastIP, const std::vector<int>& vecBroadcastPorts, const bool bVerbose)
    : VistaClusterLeaderBarrierIPBase(bVerbose) {
  for (int i = 0; i < (int)vecBroadcastPorts.size(); ++i) {
    AddBroadcast(sBroadcastIP, vecBroadcastPorts[i]);
  }
}

VistaBroadcastClusterLeaderBarrier::VistaBroadcastClusterLeaderBarrier(
    VistaUDPSocket* pBroadcastSocket, const bool bManageSocketDeletion, const bool bVerbose)
    : VistaClusterLeaderBarrierIPBase(bVerbose) {
  AddBroadcast(pBroadcastSocket, bManageSocketDeletion);
}

VistaBroadcastClusterLeaderBarrier::VistaBroadcastClusterLeaderBarrier(
    const std::vector<VistaUDPSocket*>& vecSockets, const bool bManageDeletion, const bool bVerbose)
    : VistaClusterLeaderBarrierIPBase(bVerbose) {
  for (int i = 0; i < (int)vecSockets.size(); ++i) {
    AddBroadcast(vecSockets[i], bManageDeletion);
  }
}

VistaBroadcastClusterLeaderBarrier::~VistaBroadcastClusterLeaderBarrier() {
  for (int i = 0; i < (int)m_vecBroadcastSockets.size(); ++i) {
    if (m_vecDeleteBroadcastSocket[i]) {
      m_vecBroadcastSockets[i]->WaitForSendFinish(5);
      delete m_vecBroadcastSockets[i];
    }
  }
}

bool VistaBroadcastClusterLeaderBarrier::AddBroadcast(
    const std::string& sBroadcastIP, const int nBroadcastPort) {
  VistaUDPSocket*    pSocket = new VistaUDPSocket;
  VistaSocketAddress oAddress(sBroadcastIP, nBroadcastPort);

  if ((*pSocket).OpenSocket() == false) {
    if (GetIsVerbose()) {
      vstr::errp() << "[" << GetBarrierType() << "]: "
                   << "Could not open BroadcastSocket - syncing on it will not work." << std::endl;
    }
    delete pSocket;
    return false;
  }

  if (oAddress.GetIsValid()) {
    if (GetIsVerbose()) {
      vstr::errp() << "[" << GetBarrierType() << "]: "
                   << "BroadcastAddress [" << sBroadcastIP << ":" << nBroadcastPort
                   << " is invalid - syncing on it will not work." << std::endl;
    }
    delete pSocket;
    return false;
  }

  pSocket->SetPermitBroadcast(1);
  pSocket->ConnectToAddress(oAddress);
  pSocket->SetIsBuffering(false);
  pSocket->SetIsBlocking(false);

  if (pSocket->GetIsConnected() == false) {
    if (GetIsVerbose()) {
      vstr::errp() << "[" << GetBarrierType() << "]: "
                   << "Connection to [" << sBroadcastIP << ":" << nBroadcastPort
                   << " failed - syncing on it will not work" << std::endl;
    }
    return false;
  }

  m_vecBroadcastSockets.push_back(pSocket);
  m_vecDeleteBroadcastSocket.push_back(true);
  return true;
}

bool VistaBroadcastClusterLeaderBarrier::AddBroadcast(
    VistaUDPSocket* pBroadcastSocket, const bool bManageDeletion) {
  if (pBroadcastSocket->GetIsConnected() == false) {

    if (GetIsVerbose()) {
      vstr::warnp() << "[" << GetBarrierType() << "]: "
                    << "Received BroadcastSocket is not connected - syncing on it will not work"
                    << std::endl;
    }
    return false;
  }

  pBroadcastSocket->SetPermitBroadcast(1);

  pBroadcastSocket->SetIsBuffering(false);
  pBroadcastSocket->SetIsBlocking(false);

  m_vecBroadcastSockets.push_back(pBroadcastSocket);
  m_vecDeleteBroadcastSocket.push_back(bManageDeletion);
  return true;
}

bool VistaBroadcastClusterLeaderBarrier::BarrierWait(int iTimeOut) {
  if (GetIsValid() == false)
    return false;

  if (VistaClusterLeaderBarrierIPBase::WaitForAllFollowers(iTimeOut) == false)
    return false;

  // send out go token to every follower
  bool                         bResult = true;
  std::vector<VistaUDPSocket*> vecDeadSocks;
  for (std::vector<VistaUDPSocket*>::iterator itSocket = m_vecBroadcastSockets.begin();
       itSocket != m_vecBroadcastSockets.end(); ++itSocket) {
    try {
      VistaType::sint32 nCount = GetBarrierWaitCount();
      int               nRes   = (*itSocket)->SendRaw(&nCount, sizeof(VistaType::sint32));
      if (nRes != sizeof(VistaType::sint32)) {
        vecDeadSocks.push_back((*itSocket));
      }
    } catch (VistaExceptionBase&) { vecDeadSocks.push_back((*itSocket)); }
  }

  for (std::vector<VistaUDPSocket*>::const_iterator itDead = vecDeadSocks.begin();
       itDead != vecDeadSocks.end(); ++itDead) {
    vstr::warnp() << "[" << GetBarrierType() << "]: "
                  << "Exception while sending on BroadcastSocket - removing it" << std::endl;
    for (int i = 0; i < (int)m_vecBroadcastSockets.size(); ++i) {
      if (m_vecBroadcastSockets[i] == (*itDead)) {
        if (m_vecDeleteBroadcastSocket[i])
          delete m_vecBroadcastSockets[i];
        m_vecBroadcastSockets.erase(m_vecBroadcastSockets.begin() + i);
      }
    }
    bResult = false;
  }

  return bResult;
}

bool VistaBroadcastClusterLeaderBarrier::GetIsValid() const {
  // we are valid as long as we have at least one valid slave and one valid broadcast socket
  if (VistaClusterLeaderBarrierIPBase::GetIsValid() == false)
    return false;
  if (m_vecBroadcastSockets.empty())
    return false;
  return true;
}

std::string VistaBroadcastClusterLeaderBarrier::GetBarrierType() const {
  return "BroadcastLeaderBarrier";
}

int VistaBroadcastClusterLeaderBarrier::GetNumberOfBroadcasts() const {
  return (int)m_vecBroadcastSockets.size();
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaBroadcastClusterFollowerBarrier::VistaBroadcastClusterFollowerBarrier(
    const std::string& sBroadcastIP, const int nBroadcastPort, VistaConnectionIP* pLeaderConnection,
    const bool bManageConnection, const bool bVerbose)
    : VistaClusterFollowerBarrierIPBase(pLeaderConnection, bManageConnection, bVerbose)
    , m_bManageSocket(true) {
  m_pBroadcastSocket = new VistaUDPSocket;

  VistaSocketAddress oAdress(sBroadcastIP, nBroadcastPort);

  if (oAdress.GetIsValid() == false) {
    vstr::errp() << "[" << GetBarrierType() << "]: "
                 << "Provided address [" << sBroadcastIP << ":" << nBroadcastPort << "] is invalid"
                 << std::endl;
    ProcessError();
  } else if ((*m_pBroadcastSocket).OpenSocket() == false) {
    vstr::errp() << "[" << GetBarrierType() << "]: "
                 << "Could not open swap sync socket - syncing will not work." << std::endl;
    ProcessError();
  } else {
    m_pBroadcastSocket->SetPermitBroadcast(1);
    m_pBroadcastSocket->ConnectToAddress(oAdress);
    m_pBroadcastSocket->SetSocketReuse(true);
    if (m_pBroadcastSocket->GetIsConnected() == false) {
      vstr::errp() << "[" << GetBarrierType() << "]: "
                   << "Connection to [" << sBroadcastIP << ":" << nBroadcastPort
                   << " failed - syncing will not work" << std::endl;
      ProcessError();
    } else {
      m_pBroadcastSocket->SetIsBuffering(false);
    }
  }
}

VistaBroadcastClusterFollowerBarrier::VistaBroadcastClusterFollowerBarrier(
    VistaUDPSocket* pBroadcastSocket, VistaConnectionIP* pLeaderConn, const bool bManageSocket,
    const bool bManageConnection, const bool bVerbose)
    : VistaClusterFollowerBarrierIPBase(pLeaderConn, bManageConnection, bVerbose)
    , m_pBroadcastSocket(pBroadcastSocket)
    , m_bManageSocket(bManageSocket) {
  if (m_pBroadcastSocket == NULL || m_pBroadcastSocket->GetIsBoundToAddress() == false) {
    if (GetIsVerbose()) {
      vstr::warnp() << "[" << GetBarrierType() << "]: "
                    << "Received socket is not connected - follower will "
                    << "report to leader, but not wait for go token" << std::endl;
    }
    if (m_bManageSocket)
      delete m_pBroadcastSocket;
    m_pBroadcastSocket = NULL;
  } else
    m_pBroadcastSocket->SetPermitBroadcast(1);
}

VistaBroadcastClusterFollowerBarrier::~VistaBroadcastClusterFollowerBarrier() {
  if (m_bManageSocket)
    delete m_pBroadcastSocket;
}

bool VistaBroadcastClusterFollowerBarrier::GetIsValid() const {
  return VistaClusterFollowerBarrierIPBase::GetIsValid();
}

bool VistaBroadcastClusterFollowerBarrier::BarrierWait(int iTimeOut) {
  if (GetIsValid() == false)
    return false;

  if (VistaClusterFollowerBarrierIPBase::SendReadyTokenToLeader(iTimeOut) == false)
    return false;

  VistaType::sint32 nReceive;

  try {
    if (m_pBroadcastSocket) {
      int nRet = m_pBroadcastSocket->ReceiveRaw(&nReceive, sizeof(VistaType::sint32), iTimeOut);
      if (nRet == 0) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[" << GetBarrierType() << "]: "
                        << "Timeout while waiting for broadcast token" << std::endl;
        }
        return false;
      } else if (nRet != sizeof(VistaType::sint32)) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[" << GetBarrierType() << "]: "
                        << "Error receiving go token from leader" << std::endl;
        }
        ProcessError();
        return false;
      }

      if (GetDoesSwap())
        VistaSerializingToolset::Swap(&nReceive, sizeof(VistaType::sint32));

      if (nReceive != GetBarrierWaitCount()) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[" << GetBarrierType() << "]: "
                        << "Received BarrierCounter is [" << nReceive << "], but expected ["
                        << GetBarrierWaitCount() << "]" << std::endl;
        }
        SetBarrierWaitCount(nReceive);
      }
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

std::string VistaBroadcastClusterFollowerBarrier::GetBarrierType() const {
  return "BroadcastFollowerBarrier";
}
