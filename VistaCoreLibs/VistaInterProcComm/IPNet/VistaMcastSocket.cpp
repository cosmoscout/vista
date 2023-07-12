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

#include "VistaMcastSocket.h"
#include "VistaMcastIPAddress.h"
#include "VistaMcastSocketAddress.h"

#include <VistaBase/VistaStreamUtils.h>

#if defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#elif defined(LINUX) || defined(DARWIN)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#elif defined(SUNOS) || defined(IRIX)
#include <unistd.h>
#include <netdb.h>
#include <sys/filio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#elif defined(HPUX)
#include <unistd.h>     // ioctl(), FIONREAD
#include <netdb.h>      // getprotoent() libc-Version!,gethostbyname()
#include <sys/socket.h> // socket(),gethostbyname(),connect()
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#else
#error You have to define the target platform in order to compile ViSTA
#endif

#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMcastSocket::VistaMcastSocket()
    : VistaUDPSocket() {
  m_iTTL            = 0;
  m_bLoop           = true;
  m_bIsJoin         = false;
  m_bIsSetMulticast = false;
}

VistaMcastSocket::~VistaMcastSocket() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaMcastSocket::GetIsJoinedMulticast() const {
  return m_bIsJoin;
}

bool VistaMcastSocket::GetIsSetMulticast() const {
  return m_bIsSetMulticast;
}

void VistaMcastSocket::SetIsJoinedMulticast(bool bJoin) {
  m_bIsJoin = bJoin;
}

void VistaMcastSocket::SetIsSetMulticast(bool bSet) {
  m_bIsSetMulticast = bSet;
}

int VistaMcastSocket::GetTTL() const {
  return m_iTTL;
}

bool VistaMcastSocket::SetMulticastTTL(int iTTL) {

#if defined(WIN32)
  unsigned int tmp_TTL = (unsigned int)iTTL;
#else
  unsigned char tmp_TTL  = (unsigned char)iTTL;
#endif /* defined WIN32 */

  if (!SetSockOpt(IPPROTO_IP, IP_MULTICAST_TTL, &tmp_TTL, sizeof(tmp_TTL))) {
    PrintErrorMessage("VistaMcastSocket::SetMulticastTTL(). \n");
    return false;
  }
  m_iTTL = iTTL;
  return true;
}

bool VistaMcastSocket::GetLoopBack() const {
  return m_bLoop;
}

bool VistaMcastSocket::SetMulticastLoopBack(bool bLoop) {
#if defined(WIN32)
  unsigned int tmp_LOOP = (unsigned int)(bLoop ? 1 : 0);
#else
  unsigned char tmp_LOOP = (unsigned char)(bLoop ? 1 : 0);
#endif /* defined WIN32 */

  if (!SetSockOpt(IPPROTO_IP, IP_MULTICAST_LOOP, &tmp_LOOP, sizeof(tmp_LOOP))) {
    PrintErrorMessage("VistaMcastSocket::MulticastJoin(): SetMulticastLoopBack(). \n");
    return false;
  }

  m_bLoop = bLoop;
  return true;
}

bool VistaMcastSocket::ActiveMulticast(int iMode, const string& sIpLocalString,
    const string& sIpMultiString, int iPort, int iTTL, bool bLoop) {
  if (!GetIsOpen()) {
    vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: OpenSocket() not jet" << std::endl;
    return false;
  }

  VistaIPAddress          localInterface(sIpLocalString);
  VistaMcastSocketAddress remoteSocketAddr(sIpMultiString, iPort);
  if (iMode == 0) {
    VistaSocketAddress localSockAddr(localInterface, 0);

    if (!BindToAddress(localSockAddr)) {
      vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: Can't bind to "
                   << sIpLocalString.c_str() << std::endl;
      return false;
    }

    if (!SetMulticast(remoteSocketAddr)) {
      vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: SetMulticast()" << std::endl;
      return false;
    }

    if (!SetMulticastTTL(iTTL)) {
      vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: SetMulticastTTL()" << std::endl;
      return false;
    }

    if (!SetMulticastLoopBack(bLoop)) {
      vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: SetMulticastLoopBack()"
                   << std::endl;
      return false;
    }

    struct in_addr iface_addr;
    memcpy(&iface_addr, localInterface.GetINAddress(), localInterface.GetINAddressLength());

    if (!SetSockOpt(IPPROTO_IP, IP_MULTICAST_IF, (void*)&iface_addr, sizeof(iface_addr))) {
      PrintErrorMessage(
          "VistaMcastSocket::ActiveMulticast() error: Set multicast network interface.\n");
      return false;
    }

  } else if (iMode == 1) {

    SetSocketReuse(true);

#if defined(WIN32)

    // win32 requires to bind to ANY address and iPort
    VistaSocketAddress bindSockAddr;
    bindSockAddr.SetPortNumber(iPort);

#else
    // linux requires to bind to multicast group ip
    VistaMcastSocketAddress bindSockAddr;
    bindSockAddr = remoteSocketAddr;
#endif

    if (!BindToAddress(bindSockAddr)) {
      vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: Can't bind to "
#if defined(WIN32)
                   << "INADDR_ANY"
#else
                   << sIpMultiString.c_str()
#endif
                   << ", port " << iPort << std::endl;
      return false;
    }

    if (!SetMulticast(remoteSocketAddr)) {
      vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: SetMulticast()" << std::endl;
      return false;
    }

    if (!JoinMulticast(localInterface)) {
      vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: JoinMulticast()" << std::endl;
      return false;
    }

  } else {

    vstr::errp() << "VistaMcastSocket::ActiveMulticast() error: Unknown mode." << std::endl;
    return false;
  }

  return true;
}

bool VistaMcastSocket::SetMulticast(const VistaMcastSocketAddress& sAddr) {
  if (GetIsJoinedMulticast()) {
    vstr::errp() << "VistaMcastSocket::SetMulticast() error: Overset Multicast Group" << std::endl;
    return false;
  }

  m_multiAddress    = sAddr;
  m_bIsSetMulticast = true;

  return true;
}

int VistaMcastSocket::SendtoMulticast(void* pvBuffer, const int iLength, int flags) {
  if (!GetIsSetMulticast()) {

    vstr::errp() << "VistaMcastSocket::SendtoMulticast() error: Multicast Group is not set"
                 << std::endl;
    return -1;
  }

  return SendDatagramRaw(pvBuffer, iLength, m_multiAddress, flags);
}

int VistaMcastSocket::ReceivefromMulticast(
    void* pvBuffer, const int iLength, int iTimeout, int flags) {
  if (!GetIsJoinedMulticast()) {
    vstr::errp() << "VistaMcastSocket::ReceivefromMulticast() error: Not Join multicast group jet"
                 << std::endl;
    return -1;
  }
  int iAddrLength = m_multiAddress.GetINAddressLength();
  return recvfrom(SOCKET(GetSocketID()), (char*)pvBuffer, iLength, 0,
      (struct sockaddr*)(m_multiAddress.GetINAddress()), (socklen_t*)&iAddrLength);

  // return ReceiveDatagramRaw( pvBuffer, iLength, m_multiAddress, iTimeout, flags);
}

string VistaMcastSocket::GetSocketTypeString() const {
  return "MULTICAST-UDP";
}

bool VistaMcastSocket::JoinMulticast(const VistaIPAddress& rNIFAddr) {

  if (GetIsJoinedMulticast()) {
    vstr::errp() << "VistaMcastSocket::MulticastJoin() warn: Joined multicast group already"
                 << std::endl;
    return true;
  }

  if (!GetIsSetMulticast()) {

    vstr::errp() << "VistaMcastSocket::MulticastJoin() error: Multicast Group is not set"
                 << std::endl;
    return false;
  }

  if (!GetIsBoundToAddress()) {

    vstr::errp() << "VistaMcastSocket::MulticastJoin() error: Socket is not bound" << std::endl;
    return false;
  }

  /*
  VistaSocketAddress localAddr;
  GetLocalSockName(localAddr);

  if (localAddr.GetPortNumber() != m_multiAddress.GetPortNumber())
  {
          vstr::errp() << "VistaMcastSocket::MulticastJoin() warn: The bound port ["
                   << (int)(localAddr.GetPortNumber())
                   << "] is different from multicast group port ["
                   << (int)(m_multiAddress.GetPortNumber())
                   << "]." << std::endl;
          return false;
  }
  */

  // Joins a specified multicast session
  struct ip_mreq ipmr;

  ipmr.imr_multiaddr.s_addr = ((sockaddr_in*)m_multiAddress.GetINAddress())->sin_addr.s_addr;
  // ipmr.imr_interface.s_addr = (in_addr *)(rNIFAddr.GetINAddress())->s_addr;
  memcpy(&(ipmr.imr_interface), rNIFAddr.GetINAddress(), rNIFAddr.GetINAddressLength());

  if (!SetSockOpt(IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&ipmr, sizeof(ipmr))) {
    PrintErrorMessage("VistaMcastSocket::MulticastJoin(): Failed to add multicast membership");
    return false;
  }

  /*
  if( setsockopt(GetSocketID(), IPPROTO_IP, IP_ADD_MEMBERSHIP,(char *)&ipmr, sizeof(ipmr) ) != 0  )
  {
          PrintErrorMessage("VistaMcastSocket::MulticastJoin(): Failed to add multicast
  membership"); return false;

  }
  */

  m_bIsJoin = true;

  vstr::debugi() << "Join multicast!" << std::endl;
  return true;
}

bool VistaMcastSocket::DropMulticast() {

  if (!GetIsJoinedMulticast()) {
    vstr::errp() << "VistaUDPSocket::MulticastDrop() error: Not join multicast group jet"
                 << std::endl;
    return false;
  }

  VistaSocketAddress localAddr;
  GetLocalSockName(localAddr);

  // Add socket to be a member of the multicast group
  struct ip_mreq ipmr;

  ipmr.imr_multiaddr.s_addr = ((sockaddr_in*)m_multiAddress.GetINAddress())->sin_addr.s_addr;
  ipmr.imr_interface.s_addr = ((sockaddr_in*)localAddr.GetINAddress())->sin_addr.s_addr;

  if (!SetSockOpt(IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&ipmr, sizeof(ipmr))) {
    PrintErrorMessage("VistaMcastSocket::MulticastJoin(): Failed to drop multicast membership");
    return false;
  }

  m_bIsJoin = false;
  m_iTTL    = 0;

  return true;
}
