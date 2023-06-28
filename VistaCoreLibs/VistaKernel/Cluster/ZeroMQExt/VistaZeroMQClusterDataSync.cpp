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

#include "VistaZeroMQClusterDataSync.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>

#include <zmq.hpp>

#include "VistaZeroMQCommon.h"
#include <cassert>

#ifdef SendMessage
#undef SendMessage
#endif

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

namespace {
// size of setsockopts option changed from uint64_t to int in >=zmq-3.2
#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(3, 2, 0)
const int         S_nMaxBandwidth     = 1024 * 1024; // in kilobit/sec
const std::size_t S_nMaxBandwidthSize = sizeof(int);
#else
const VistaType::uint64 S_nMaxBandwidth     = 1024 * 1024; // in kilobit/sec
const std::size_t       S_nMaxBandwidthSize = sizeof(VistaType::uint64);
#endif
} // namespace

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaZeroMQClusterLeaderDataSync::VistaZeroMQClusterLeaderDataSync(
    const std::string& sHostname, const int nServerPort, const bool bVerbose)
    : VistaClusterBytebufferLeaderDataSyncBase(bVerbose) {
  VistaZeroMQCommon::RegisterZeroMQUser();

  std::string sName = "tcp://" + sHostname + ":" + VistaConversion::ToString(nServerPort);
  try {
    m_pSocket = new zmq::socket_t(VistaZeroMQCommon::GetContext(), ZMQ_PUB);
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
    m_pSocket->bind(sName.c_str());
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
  } catch (zmq::error_t& oZmqException) {
    vstr::warnp() << "[ZeroMQLeaderDataSync]: Could not create/bind zmq socket at address[ "
                  << sName << "] with exception:\n";
    vstr::warni() << oZmqException.what() << std::endl;
    delete m_pSocket;
    m_pSocket = NULL;
  }
}

VistaZeroMQClusterLeaderDataSync::VistaZeroMQClusterLeaderDataSync(
    const std::string& sZeroMQAddress, const bool bVerbose)
    : VistaClusterBytebufferLeaderDataSyncBase(bVerbose) {
  VistaZeroMQCommon::RegisterZeroMQUser();

  try {
    m_pSocket = new zmq::socket_t(VistaZeroMQCommon::GetContext(), ZMQ_PUB);
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
    m_pSocket->bind(sZeroMQAddress.c_str());
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
  } catch (zmq::error_t& oZmqException) {
    vstr::warnp() << "[ZeroMQLeaderDataSync]: Could not create/bind zmq socket at address ["
                  << sZeroMQAddress << "] with exception:\n";
    vstr::warni() << oZmqException.what() << std::endl;
    delete m_pSocket;
    m_pSocket = NULL;
  }
}

VistaZeroMQClusterLeaderDataSync::~VistaZeroMQClusterLeaderDataSync() {
  delete m_pSocket;
  VistaZeroMQCommon::UnregisterZeroMQUser();
}
bool VistaZeroMQClusterLeaderDataSync::GetIsValid() const {
  return (m_pSocket != NULL);
}

bool VistaZeroMQClusterLeaderDataSync::DoSendMessage() {
  VistaType::sint32 nSize = m_oMessage.GetBufferSize();
  if (m_pExtBuffer)
    nSize += m_nExtBufferSize;
  zmq::message_t oZmqMsg(nSize);
  memcpy(oZmqMsg.data(), m_oMessage.GetBuffer(), m_oMessage.GetBufferSize());
  if (m_pExtBuffer)
    memcpy((VistaType::byte*)oZmqMsg.data() + m_oMessage.GetBufferSize(), m_pExtBuffer,
        m_nExtBufferSize);

  for (;;) {
    try {
      if (m_pSocket->send(oZmqMsg))
        break;
    } catch (zmq::error_t& oZmqException) {
      if (oZmqException.num() == EINTR) // EINTR are okay, we just retry
        continue;
      vstr::warnp() << "[ZeroMQLeaderDataSync]: Sending ZeroMQ message failed with exception:\n";
      vstr::warni() << oZmqException.what() << std::endl;
      return false;
    }
  }

  return GetIsValid();
}

std::string VistaZeroMQClusterLeaderDataSync::GetDataSyncType() const {
  return "ZeroMQLeaderSync";
}

bool VistaZeroMQClusterLeaderDataSync::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}

int VistaZeroMQClusterLeaderDataSync::GetSendBlockingThreshold() const {
  return -1;
}

bool VistaZeroMQClusterLeaderDataSync::WaitForConnection(
    const std::vector<VistaConnectionIP*>& vecConnections) {
  for (std::vector<VistaConnectionIP*>::const_iterator itConn = vecConnections.begin();
       itConn != vecConnections.end(); ++itConn) {
    (*itConn)->SetIsBlocking(true);
    assert((*itConn)->PendingDataSize() == 0);
  }

  std::vector<bool> vecConnectionSuccess(vecConnections.size(), false);
  VistaType::sint32 nCount        = 0;
  bool              bAllConnected = false;
  bool              bReadValue;
  try {
    while (bAllConnected == false) {
      bAllConnected = true;
      zmq::message_t oMessage(sizeof(VistaType::sint32));
      memcpy(oMessage.data(), &nCount, sizeof(VistaType::sint32));
      for (;;) {
        try {
          if (m_pSocket->send(oMessage))
            break;
        } catch (zmq::error_t& oZmqException) {
          if (oZmqException.num() == EINTR) // EINTR are okay, we just retry
            continue;
          vstr::warnp()
              << "[ZeroMQLeaderDataSync]: Sending ZEroMQ message failed with exception:\n";
          vstr::warni() << oZmqException.what() << std::endl;
          return false;
        }
      }
      ++nCount;
      VistaTimeUtils::Sleep(1);
      for (std::size_t nIndex = 0; nIndex < vecConnections.size(); ++nIndex) {
        if (vecConnectionSuccess[nIndex])
          continue;
        if (vecConnections[nIndex]->HasPendingData()) {
          vecConnections[nIndex]->ReadBool(bReadValue);
          vecConnectionSuccess[nIndex] = true;
          assert(bReadValue);
        } else {
          bAllConnected = false;
        }
      }
    }
    // send out finish message
    nCount = -1;
    zmq::message_t oMessage(sizeof(VistaType::sint32));
    memcpy(oMessage.data(), &nCount, sizeof(VistaType::sint32));
    for (;;) {
      try {
        if (m_pSocket->send(oMessage))
          break;
      } catch (zmq::error_t& oZmqException) {
        if (oZmqException.num() == EINTR) // EINTR are okay, we just retry
          continue;
        vstr::warnp() << "[ZeroMQLeaderDataSync]: Sending ZEroMQ message failed with exception:\n";
        vstr::warni() << oZmqException.what() << std::endl;
        return false;
      }
    }
  } catch (zmq::error_t& oZmqException) {
    vstr::warnp() << "[ZeroMQLeaderDataSync]: WaitForConnection failed:\n";
    vstr::warni() << oZmqException.what() << std::endl;
    return false;
  }

  return true;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaZeroMQClusterFollowerDataSync::VistaZeroMQClusterFollowerDataSync(
    const std::string& sHostname, const int nServerPort, const bool bSwap, const bool bVerbose)
    : VistaClusterBytebufferFollowerDataSyncBase(bSwap, bVerbose) {
  VistaZeroMQCommon::RegisterZeroMQUser();

  std::string sName = "tcp://" + sHostname + ":" + VistaConversion::ToString(nServerPort);
  try {
    m_pZmqMessage = new zmq::message_t;
    m_pSocket     = new zmq::socket_t(VistaZeroMQCommon::GetContext(), ZMQ_SUB);
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
    m_pSocket->connect(sName.c_str());
    m_pSocket->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
  } catch (zmq::error_t& oZmqException) {
    vstr::warnp() << "[ZeroMQFollowerDataSync]: Could not create/connect zmq socket to address ["
                  << sName << "] with exception:\n";
    vstr::warni() << oZmqException.what() << std::endl;
    delete m_pSocket;
    m_pSocket = NULL;
  }
}

VistaZeroMQClusterFollowerDataSync::VistaZeroMQClusterFollowerDataSync(
    const std::string& sZeroMQAddress, const bool bSwap, const bool bVerbose)
    : VistaClusterBytebufferFollowerDataSyncBase(bSwap, bVerbose) {
  VistaZeroMQCommon::RegisterZeroMQUser();

  try {
    m_pZmqMessage = new zmq::message_t;
    m_pSocket     = new zmq::socket_t(VistaZeroMQCommon::GetContext(), ZMQ_SUB);
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
    m_pSocket->connect(sZeroMQAddress.c_str());
    m_pSocket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    m_pSocket->setsockopt(ZMQ_RATE, &S_nMaxBandwidth, S_nMaxBandwidthSize);
  } catch (zmq::error_t& oZmqException) {
    vstr::warnp() << "[ZeroMQFollowerDataSync]: Could not create/connect zmq socket to address ["
                  << sZeroMQAddress << "] with exception:\n";
    vstr::warni() << oZmqException.what() << std::endl;
    delete m_pSocket;
    m_pSocket = NULL;
  }
}

VistaZeroMQClusterFollowerDataSync::~VistaZeroMQClusterFollowerDataSync() {
  delete m_pSocket;
  delete m_pZmqMessage;
  VistaZeroMQCommon::UnregisterZeroMQUser();
}

bool VistaZeroMQClusterFollowerDataSync::GetIsValid() const {
  return (m_pSocket != NULL);
}

bool VistaZeroMQClusterFollowerDataSync::DoReceiveMessage() {
  for (;;) {
    try {
      if (m_pSocket->recv(m_pZmqMessage))
        break;
    } catch (zmq::error_t& oZmqException) {
      if (oZmqException.num() == EINTR) // EINTR are okay, we just retry
        continue;
      vstr::warnp() << "[ZeroMQLeaderDataSync]: receiving ZeroMQ message failed with exception:\n";
      vstr::warni() << oZmqException.what() << std::endl;
      return false;
    }
  }

  VistaType::sint32 nSize;
  memcpy(&nSize, m_pZmqMessage->data(), sizeof(VistaType::sint32));
  if (m_bSwap)
    VistaSerializingToolset::Swap(&nSize, sizeof(VistaType::sint32));

  m_oMessage.SetBuffer(
      static_cast<VistaType::byte*>(m_pZmqMessage->data()) + sizeof(VistaType::sint32), nSize);

  return true;
}

std::string VistaZeroMQClusterFollowerDataSync::GetDataSyncType() const {
  return "ZeroMQFollowerSync";
}

bool VistaZeroMQClusterFollowerDataSync::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}
int VistaZeroMQClusterFollowerDataSync::GetSendBlockingThreshold() const {
  return -1;
}

bool VistaZeroMQClusterFollowerDataSync::WaitForConnection(VistaConnectionIP* pConnection) {
  pConnection->SetIsBlocking(true);
  pConnection->SetIsBuffering(false);

  zmq::message_t oMessage;
  for (;;) {
    try {
      if (m_pSocket->recv(&oMessage))
        break;
    } catch (zmq::error_t& oZmqException) {
      if (oZmqException.num() == EINTR) // EINTR are okay, we just retry
        continue;
      vstr::warnp() << "[ZeroMQLeaderDataSync]: receiving ZeroMQ message failed with exception:\n";
      vstr::warni() << oZmqException.what() << std::endl;
      return false;
    }
  }
  VistaType::sint32 nCount = 666;
  memcpy(&nCount, oMessage.data(), sizeof(VistaType::sint32));
  pConnection->WriteBool(true);
  // read of all remaining messages
  VistaType::sint32 nRead;
  do {
    for (;;) {
      try {
        if (m_pSocket->recv(&oMessage))
          break;
      } catch (zmq::error_t& oZmqException) {
        if (oZmqException.num() == EINTR) // EINTR are okay, we just retry
          continue;
        vstr::warnp()
            << "[ZeroMQLeaderDataSync]: receiving ZeroMQ message failed with exception:\n";
        vstr::warni() << oZmqException.what() << std::endl;
        return false;
      }
    }
    memcpy(&nRead, oMessage.data(), sizeof(VistaType::sint32));
    if (m_bSwap)
      VistaSerializingToolset::Swap(&nRead, sizeof(VistaType::sint32));
  } while (nRead != -1);
  return true;
}
