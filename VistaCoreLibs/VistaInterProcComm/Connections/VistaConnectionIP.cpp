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

#if defined(WIN32)
#define _WINSOCKAPI_
#include <Windows.h>
#include <winsock2.h> // should be included *before* windows.h
#endif

#include "VistaConnectionIP.h"

#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/IPNet/VistaIPAddress.h>
#include <VistaInterProcComm/IPNet/VistaIPComm.h>
#include <VistaInterProcComm/IPNet/VistaSocketAddress.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#if defined(VISTA_IPC_USE_EXCEPTIONS)
#include <VistaBase/VistaExceptionBase.h>
#endif
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaBase/VistaStreamUtils.h>

#include <string>
using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaConnectionIP::VistaConnectionIP() {
#if defined(WIN32)
  m_pWaitEvent            = NULL;
  m_bEventSelectIsEnabled = false;
#endif
  VistaIPComm::UseIPComm();
  m_pSocket     = NULL;
  m_eProtocol   = CT_NONE;
  m_iPeerPort   = -1;
  m_iLingerTime = 0;

  SetByteorderSwapFlag(VistaSerializingToolset::GetDefaultPlatformSwapBehavior());
}

VistaConnectionIP::VistaConnectionIP(IVistaSocket* pSocket) {
#if defined(WIN32)
  m_pWaitEvent            = NULL;
  m_bEventSelectIsEnabled = false;
#endif
  m_iLingerTime = 0;
  VistaIPComm::UseIPComm();
  m_pSocket = pSocket;
  if (m_pSocket->GetSocketTypeString() == "TCP")
    m_eProtocol = CT_TCP;
  else if (m_pSocket->GetSocketTypeString() == "UDP")
    m_eProtocol = CT_UDP;
  else
    m_eProtocol = CT_NONE;

  SetIsOpen(pSocket->GetIsOpen());

  // yes, we do call the base class here, and this is on purpose,
  // as we do only want to toggle the flags of the base class
  VistaConnection::SetIsBlocking(pSocket->GetIsBlocking());
  VistaConnection::SetIsBuffering(pSocket->GetIsBuffering());

  if (GetIsOpen()) {
    // set port and host-name
    VistaSocketAddress adr;
    pSocket->GetLocalSockName(adr);
    m_iPeerPort = adr.GetPortNumber();
    adr.GetIPAddress().GetAddressString(m_sPeerName);
    m_pSocket->SetLingerOnClose(m_iLingerTime);
    m_pSocket->SetKeepConnectionsAlive(0);
  } else
    m_iPeerPort = -1;

  SetByteorderSwapFlag(VistaSerializingToolset::GetDefaultPlatformSwapBehavior());
}

VistaConnectionIP::VistaConnectionIP(IVistaSocket* pSocket, const string& sHost, const int iPort) {
  m_iLingerTime = 0;
#if defined(WIN32)
  m_pWaitEvent            = NULL;
  m_bEventSelectIsEnabled = false;
#endif
  VistaIPComm::UseIPComm();
  m_pSocket = pSocket;
  if (m_pSocket->GetSocketTypeString() == "TCP")
    m_eProtocol = CT_TCP;
  else if (m_pSocket->GetSocketTypeString() == "UDP")
    m_eProtocol = CT_UDP;
  else
    m_eProtocol = CT_NONE;

  SetIsOpen(pSocket->GetIsOpen());

  // yes, we do call the base class here, and this is on purpose,
  // as we do only want to toggle the flags of the base class
  VistaConnection::SetIsBlocking(pSocket->GetIsBlocking());
  VistaConnection::SetIsBuffering(pSocket->GetIsBuffering());

  if (GetIsOpen()) {
    // set port and host-name
    VistaSocketAddress adr;
    pSocket->GetLocalSockName(adr);
    m_iPeerPort = adr.GetPortNumber();
    adr.GetIPAddress().GetAddressString(m_sPeerName);

    VistaSocketAddress peer(sHost, iPort);
    if (!m_pSocket->ConnectToAddress(peer)) {
      (*m_pSocket).CloseSocket(true);
      delete m_pSocket;
      m_pSocket = NULL;
      SetIsOpen(false);
    } else {
      m_sPeerName = sHost;
      m_iPeerPort = iPort;
      SetIsOpen(true); // only set open to true on success
      m_pSocket->SetLingerOnClose(m_iLingerTime);
      m_pSocket->SetKeepConnectionsAlive(0);
    }
  } else
    m_iPeerPort = -1;

  SetByteorderSwapFlag(VistaSerializingToolset::GetDefaultPlatformSwapBehavior());
}

VistaConnectionIP::VistaConnectionIP(const string& sMyHost, const int iMyPort, bool bUDPBroadcast) {
  m_iLingerTime = 0;
#if defined(WIN32)
  m_pWaitEvent            = NULL;
  m_bEventSelectIsEnabled = false;
#endif
  VistaIPComm::UseIPComm();
  DoSetConnectionType(CT_UDP);
  if (m_pSocket) {
    if (m_pSocket->OpenSocket()) {
      (*m_pSocket).SetIsBlocking(true);
      (*m_pSocket).SetPermitBroadcast((bUDPBroadcast ? 1 : 0));
      VistaSocketAddress myAdr(sMyHost, iMyPort);
      if ((*m_pSocket).BindToAddress(myAdr)) {
        SetIsOpen(true);
        m_pSocket->SetLingerOnClose(m_iLingerTime);
        m_pSocket->SetKeepConnectionsAlive(0);
      } else {
        (*m_pSocket).CloseSocket(true);
        delete m_pSocket;
        m_pSocket = NULL;
        SetIsOpen(false);
      }
    } else
      SetIsOpen(false);
  }

  SetByteorderSwapFlag(VistaSerializingToolset::GetDefaultPlatformSwapBehavior());
}

VistaConnectionIP::VistaConnectionIP(
    const VistaProtocol type, const string& sHost, const int iPort, bool bUDPBroadcast) {
  m_iLingerTime = 0;
#if defined(WIN32)
  m_pWaitEvent            = NULL;
  m_bEventSelectIsEnabled = false;
#endif
  VistaIPComm::UseIPComm();
  DoSetConnectionType(type); // <- THIS will CREATE a proper TCP socket
  if (m_pSocket)             // <- THIS will only fail, iff socket could not be created!
  {
    if ((*m_pSocket).OpenSocket()) {
      (*m_pSocket).SetIsBlocking(true);
      if (GetProtocol() == CT_UDP)
        (*m_pSocket).SetPermitBroadcast((bUDPBroadcast ? 1 : 0));

      VistaSocketAddress adr(sHost, iPort);
      if (!m_pSocket->ConnectToAddress(adr)) {
        (*m_pSocket).CloseSocket(true);
        delete m_pSocket;
        m_pSocket = NULL;
        SetIsOpen(false);
      } else {
        m_sPeerName = sHost;
        m_iPeerPort = iPort;
        SetIsOpen(true); // only set open to true on success
        m_pSocket->SetLingerOnClose(m_iLingerTime);
        m_pSocket->SetKeepConnectionsAlive(0);
      }
    } else {
      m_iPeerPort = -1;
      delete m_pSocket;
      m_pSocket = NULL;
      SetIsOpen(false);
    }
  }

  SetByteorderSwapFlag(VistaSerializingToolset::GetDefaultPlatformSwapBehavior());
}

VistaConnectionIP::VistaConnectionIP(const VistaProtocol type) {
  m_iLingerTime = 0;
#if defined(WIN32)
  m_pWaitEvent            = NULL;
  m_bEventSelectIsEnabled = false;
#endif
  VistaIPComm::UseIPComm();
  DoSetConnectionType(type); // <- THIS will CREATE a proper socket
  m_iPeerPort = -1;
  SetByteorderSwapFlag(VistaSerializingToolset::GetDefaultPlatformSwapBehavior());
}

VistaConnectionIP::~VistaConnectionIP() {
#if defined(WIN32)
  if (m_pWaitEvent && m_pSocket)
    m_pSocket->DisableEventSelect(m_pWaitEvent);
  delete m_pWaitEvent;
#endif
  delete m_pSocket;
  m_iPeerPort = -1;
  m_sPeerName = "";
  m_pSocket   = (IVistaSocket*)0xDEADBEEF;
  VistaIPComm::CloseIPComm();
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaConnectionIP::Open() {
  if (GetIsOpen())
    return true; // opend connections stay open

  if (!m_pSocket) // sanity check
    return false;

  SetIsOpen(m_pSocket->OpenSocket());
  if (GetIsOpen() && (!(m_sPeerName == "") && m_iPeerPort != -1))
    if (!Connect()) // convinience: try a connect if host and port are present already
    {
      m_pSocket->CloseSocket(true);
      delete m_pSocket;
      m_pSocket = NULL;
      SetIsOpen(false);
    }

  if (GetIsOpen()) // hmm... did this open the socket?
  {
    m_pSocket->SetLingerOnClose(m_iLingerTime);
    m_pSocket->SetKeepConnectionsAlive(0);
    return true;
  }
  return false;
}

void VistaConnectionIP::Close() {
  if (!GetIsOpen() || !m_pSocket)
    return; // closed connections stay closed
  SetIsOpen(!m_pSocket->CloseSocket(false));
}

void VistaConnectionIP::Close(bool bSkipData) {
  if (!GetIsOpen() || !m_pSocket)
    return; // closed connections stay closed
  SetIsOpen(!m_pSocket->CloseSocket(bSkipData));
}

bool VistaConnectionIP::Connect(const string& sHost, const int iPort, bool bBlockingConnect) {
  if (m_pSocket) {
    bool bRevertBlockingState = false;
    if (GetProtocol() == CT_TCP && bBlockingConnect) {
      if (m_pSocket->GetIsConnected())
        return false; // we could check iff current connection equals incoming args and return true
                      // then, but we do not ;)
      if (!m_pSocket->GetIsBlocking()) {
        bRevertBlockingState = true;
        m_pSocket->SetIsBlocking(true); // we should make a connect using a blocking socket
      }
    }

    // this is either UPD or unconnected TCP
    // reconnecting UDP is ok
    VistaSocketAddress adr(sHost, iPort);
    if (adr.GetIsValid()) {

      bool bRet = m_pSocket->ConnectToAddress(adr);
      if (bRevertBlockingState && (GetProtocol() == CT_TCP)) {
        m_pSocket->SetIsBlocking(
            false); // set blocking state to user settings, independant of connection state

        // note: a connected socket is not *realy* connected
        // after the call returns! It simply means that a first
        // ACK from the server got through. A direct
        // attempt to send will result in a broken pipe

        // ok, lets wait for write enabled
        m_pSocket->WaitForSendFinish(0);
        if (m_pSocket->GetErrorStatus()) {
          // something went wrong!!
          Close();
#if defined(VISTA_IPC_USE_EXCEPTIONS)
          VISTA_THROW("VistaConnectionIP::Connect() -- error state != 0", 0x00000203)
#endif
        }
        // now things *should* be stable
        // this may be a hack, until we know better ;)
      }
      return bRet;
    }
  }
  return false;
}

bool VistaConnectionIP::Connect(const VistaSocketAddress& adr, bool bBlockingConnect) {
  // PRE: adr.GetIsValid() == true
  if (m_pSocket) {
    if (GetProtocol() == CT_TCP) {
      if (m_pSocket->GetIsConnected()) {
        return false;
      } else {
        return Connect(adr.GetIPAddress().GetHostNameC(), adr.GetPortNumber(), bBlockingConnect);
      }
    }

    // this is either UPD or unconnected TCP
    // reconnecting UDP is ok
    return m_pSocket->ConnectToAddress(adr);
  }

  return false;
}

bool VistaConnectionIP::Connect(bool bBlockingConnect) {
  if ((m_sPeerName == "") || (m_iPeerPort == -1))
    return false;
  return Connect(m_sPeerName, m_iPeerPort, bBlockingConnect);
}

bool VistaConnectionIP::GetIsConnected() const {
  if (m_pSocket)
    return m_pSocket->GetIsConnected();
  return false;
}

void VistaConnectionIP::SetPeerName(const string& sHostName) {
  m_sPeerName = sHostName;
}

void VistaConnectionIP::SetPeerPort(int iPort) {
  m_iPeerPort = iPort;
}

void VistaConnectionIP::SetHostNameAndPort(const string& sHostName, int iPort) {
  m_sPeerName = sHostName;
  m_iPeerPort = iPort;
}

VistaSocketAddress VistaConnectionIP::GetLocalAddress() const {
  VistaSocketAddress adr;
  if (m_pSocket) {
    m_pSocket->GetLocalSockName(adr);
  }

  return adr;
}

int VistaConnectionIP::Receive(void* buffer, const int length, int iTimeout) {
  if (iTimeout == 0) {
    // no timeout
    if (GetProtocol() == CT_TCP && GetIsBlocking()) {
      /* new behavior -> Receive returns iff iLen Bytes have been received*/
      VistaType::byte* pTempBuffer   = (VistaType::byte*)buffer;
      int              iNumBytesRead = 0;
      int              iCurrentNum   = 0;
      int              iBytesToRead  = length;
      while (iBytesToRead > 0) {
        iCurrentNum = m_pSocket->ReceiveRaw(pTempBuffer, iBytesToRead);
        if (iCurrentNum <= 0) {
          // blocking but reading <= 0: socket closed!
          // or socket exception!
          /** @todo check if this conforms with the */
          // application expectations!
          vstr::errp() << "VistaConnectionIP::Receive(" << length << ") -- received ["
                       << iCurrentNum << "] after [" << iNumBytesRead << "]" << std::endl;
          return -1;
        }

        iBytesToRead -= iCurrentNum;
        iNumBytesRead += iCurrentNum;
        pTempBuffer += iCurrentNum;
      }
      return iNumBytesRead;
    } else {
      return m_pSocket->ReceiveRaw(buffer, length, iTimeout);
    }
  } else // timeout given
  {
    // disregard blocking state and protocol, try to read as many bytes
    // as possible, given the timeout
#if defined(VISTA_IPC_USE_EXCEPTIONS)
    if (!m_pSocket)
      VISTA_THROW("VistaConnectionIP::Receive() -- no valid socket id", 0x00000201)
    try {
      return m_pSocket->ReceiveRaw(buffer, length, iTimeout);
    } catch (VistaExceptionBase& x) {
      Close(); // clean exit
      throw x; // rethrow
    }
#if !defined(_MSC_VER)
    return -1; // avoids a warning on non-msvc compilers
#endif
#else
    if (!m_pSocket)
      return -1;
    return m_pSocket->ReceiveRaw(buffer, length, iTimeout);
#endif
  }
}

int VistaConnectionIP::Send(const void* buffer, const int length) {
  if (!m_pSocket)
#if defined(VISTA_IPC_USE_EXCEPTIONS)
    VISTA_THROW("VistaConnectionIP::Send() -- no valid socket id", 0x00000202)
#else
    return -1;
#endif

#if defined(VISTA_IPC_USE_EXCEPTIONS)
  try {
#endif
    return m_pSocket->SendRaw(buffer, length);
#if defined(VISTA_IPC_USE_EXCEPTIONS)
  } catch (VistaExceptionBase& x) {
    Close(); // clean exit
    throw x; // rethrow
  }
#endif
}

bool VistaConnectionIP::GetIsFine() const {
  if (m_eProtocol == VistaConnectionIP::CT_TCP)
    return VistaConnection::GetIsFine() && GetIsConnected();
  return VistaConnection::GetIsFine();
}

VistaConnectionIP::VistaProtocol VistaConnectionIP::GetProtocol() const {
  return m_eProtocol;
}

bool VistaConnectionIP::SetConnectionType(const VistaProtocol type) {
  if (m_pSocket)
    return false; // only allow this on instances with constructed with default-constructor!

  return DoSetConnectionType(type);
}

bool VistaConnectionIP::DoSetConnectionType(const VistaProtocol type) {
  m_eProtocol = type;
  switch (type) {
  case CT_TCP: {

    m_pSocket = new VistaTCPSocket;
    break;
  }
  case CT_UDP: {

    m_pSocket = new VistaUDPSocket;
    break;
  }
  case CT_NONE:
  default:
    m_pSocket   = NULL;
    m_eProtocol = CT_NONE;
    break;
  }

  return (m_pSocket != NULL);
}

VistaSocketAddress VistaConnectionIP::GetPeerAddress() const {
  VistaSocketAddress peer;
  if (GetIsOpen()) {
    m_pSocket->GetPeerSockName(peer);
  }

  return peer;
}

bool VistaConnectionIP::HasPendingData() const {
  if (!m_pSocket)
    return false;

  return (m_pSocket->HasBytesPending() > 0);
}

unsigned long VistaConnectionIP::WaitForIncomingData(int timeout) {
  if (!m_pSocket)
    return 0;

  return m_pSocket->WaitForIncomingData(timeout);
}

unsigned long VistaConnectionIP::WaitForSendFinish(int timeout) {
  if (!m_pSocket)
    return 0;
  return m_pSocket->WaitForSendFinish(timeout);
}

unsigned long VistaConnectionIP::PendingDataSize() const {
  if (!m_pSocket)
    return 0;
  return m_pSocket->HasBytesPending();
}

void VistaConnectionIP::SetIsBlocking(bool bBlocking) {
  if (!m_pSocket) // sanity check
    return;

  m_pSocket->SetIsBlocking(bBlocking);
  VistaConnection::SetIsBlocking(m_pSocket->GetIsBlocking()); // our state is the socket's state!
}

void VistaConnectionIP::SetIsBuffering(bool bBuffering) {
  if (!m_pSocket)
    return;

  m_pSocket->SetIsBuffering(bBuffering);
  VistaConnection::SetIsBuffering(m_pSocket->GetIsBuffering()); // our state is the socket's state!
}

void VistaConnectionIP::SetShowRawSendAndReceive(bool bShowRaw) {
  if (!m_pSocket) /** @todo we should remember the flag set here for later */
    return;
  m_pSocket->SetShowRawSendAndReceive(bShowRaw);
}

bool VistaConnectionIP::GetShowRawSendAndReceive() const {
  if (!m_pSocket)
    return false;

  return m_pSocket->GetShowRawSendAndReceive();
}

HANDLE VistaConnectionIP::GetSocketID() const {
  if (!m_pSocket)
    return HANDLE(~0);             // default for no socket
  return m_pSocket->GetSocketID(); // could still be -1, but hey... we do not care ;)
}

HANDLE VistaConnectionIP::GetConnectionDescriptor() const {
  return GetSocketID();
}

HANDLE VistaConnectionIP::GetConnectionWaitForDescriptor() {
  if (!m_pSocket)
    return HANDLE(~0);
#if !defined(WIN32)
  return GetConnectionDescriptor();
#else
  SetWaitForDescriptorEventSelectIsEnabled(true);
  return m_pWaitEvent->GetEventSignalHandle();
#endif
}

bool VistaConnectionIP::GetWaitForDescriptorEventSelectIsEnabled() const {
#ifdef WIN32
  return m_bEventSelectIsEnabled;
#else
  return true;
#endif
}

bool VistaConnectionIP::SetWaitForDescriptorEventSelectIsEnabled(const bool bSet) {
#ifdef WIN32
  if (m_pWaitEvent == NULL)
    m_pWaitEvent = new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT);

  if (bSet)
    m_pSocket->EnableEventSelect(m_pWaitEvent, FD_READ | FD_CLOSE);
  else
    m_pSocket->DisableEventSelect(m_pWaitEvent);

  m_bEventSelectIsEnabled = bSet;
  return true;
#else
  return (bSet == true);
#endif
}

VistaConnection::eConState VistaConnectionIP::DetermineConState() const {
#if defined(WIN32)
  if (!m_pSocket || !m_pWaitEvent)
    return CON_NONE;

  WSANETWORKEVENTS NetworkEvents;
  if (WSAEnumNetworkEvents(SOCKET((*m_pSocket).GetSocketID()),
          (*m_pWaitEvent).GetEventSignalHandle(), &NetworkEvents) == 0) {
    // ok, this worked
    if (NetworkEvents.lNetworkEvents & FD_CLOSE) {
      // close
      return CON_CLOSE;
    } else if (NetworkEvents.lNetworkEvents & FD_READ) {
      return CON_IO;
    } else if (NetworkEvents.lNetworkEvents & FD_WRITE) {
      return CON_IO;
    }
  }
#else

#endif
  return CON_NONE;
}

void VistaConnectionIP::SetLingerTime(int iLingerTimeSec) {
  m_iLingerTime = iLingerTimeSec;
  if (m_pSocket)
    m_pSocket->SetLingerOnClose(iLingerTimeSec);
}

int VistaConnectionIP::GetLingerTime() const {
  return m_iLingerTime;
}

int VistaConnectionIP::ReadRawBuffer(void* pBuffer, int iLen) {
  if (GetProtocol() == CT_TCP && GetIsBlocking()) {
    /* new behavior -> ReadRawBuffer returns iff iLen Bytes have been received*/
    VistaType::byte* pTempBuffer   = (VistaType::byte*)pBuffer;
    int              iNumBytesRead = 0;
    int              iCurrentNum   = 0;
    int              iBytesToRead  = iLen;
    while (iBytesToRead > 0) {
      iCurrentNum = m_pSocket->ReceiveRaw(pTempBuffer, iBytesToRead, 0);
      if (iCurrentNum <= 0) {
        // blocking but reading <= 0: socket closed!
        // or socket exception!
        /** @todo check if this conforms with the */
        // application expectations!
        vstr::errp() << "VistaConnectionIP::ReadRawBuffer(" << iLen << ") -- received ["
                     << iCurrentNum << "] after [" << iNumBytesRead << "]" << std::endl;
        return -1;
      }

      iBytesToRead -= iCurrentNum;
      iNumBytesRead += iCurrentNum;
      pTempBuffer += iCurrentNum;
    }
    return iNumBytesRead;
  } else {
    return Receive(pBuffer, iLen);
  }
}

string VistaConnectionIP::GetPeerName() const {
  return m_sPeerName;
}

int VistaConnectionIP::GetPeerPort() const {
  return m_iPeerPort;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
