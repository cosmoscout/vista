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

#include <string>
using namespace std;

#if defined(WIN32)
#include <winsock2.h>

typedef int socklen_t;
#elif defined(LINUX) || defined(DARWIN)
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cerrno>
#elif defined(SUNOS) || defined(IRIX)
#include <unistd.h>
#include <netdb.h>
#include <sys/filio.h>
#include <arpa/inet.h>

#elif defined(HPUX)
#include <unistd.h>     // ioctl(), FIONREAD
#include <netdb.h>      // getprotoent() libc-Version!,gethostbyname()
#include <sys/socket.h> // socket(),gethostbyname(),connect()
#include <arpa/inet.h>

#else
#error You have to define the target platform in order to compile ViSTA

#endif

#include "VistaUDPSocket.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaUDPSocket::VistaUDPSocket()
    : IVistaSocket(PF_INET, SOCK_DGRAM, IPPROTO_UDP) {
}

VistaUDPSocket::~VistaUDPSocket() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaUDPSocket::SendDatagramRaw(
    const void* pvBuffer, const int iLength, const VistaSocketAddress& peer, int flags) {
#ifdef WIN32
  int iRet = sendto(SOCKET(GetSocketID()), (const char*)pvBuffer, iLength, flags,
      (sockaddr*)peer.GetINAddress(), peer.GetINAddressLength());
#else
  int iRet = TEMP_FAILURE_RETRY(sendto(SOCKET(GetSocketID()), (const char*)pvBuffer, iLength, flags,
      (sockaddr*)peer.GetINAddress(), peer.GetINAddressLength()));
#endif
  if (iRet < 0) {
    PrintErrorMessage("VistaUDPSocket::SendDatagram()");
  }

  return iRet;
}

int VistaUDPSocket::ReceiveDatagramRaw(void* pvBuffer, const int iLength,
    const VistaSocketAddress& fromAddress, int iTimeout, int flags) {
  if (!GetIsBoundToAddress())
    return -1; // you have to be bound to an adress in order to receive anything

  int iReadLength = iLength;
  if (iTimeout != 0) {
    int iReady = WaitForIncomingData(iTimeout);
    if (iReady == 0)
      return 0;

    if (iReady < iLength)
      iReadLength = iReady; // adjust, without buffer size-violence ;)
  }
  int iAddrLength = fromAddress.GetINAddressLength();
#if defined(HPUX)
  int iRet = recvfrom(GetSocketID(), (char*)pvBuffer, iReadLength, flags,
      (sockaddr*)fromAddress.GetINAddress(), &iAddrLength);
#elif defined WIN32
  int iRet = recvfrom(SOCKET(GetSocketID()), (char*)pvBuffer, iReadLength, flags,
      (sockaddr*)fromAddress.GetINAddress(), (socklen_t*)&iAddrLength);
#else
  int iRet = TEMP_FAILURE_RETRY(recvfrom(SOCKET(GetSocketID()), (char*)pvBuffer, iReadLength, flags,
      (sockaddr*)fromAddress.GetINAddress(), (socklen_t*)&iAddrLength));
#endif

  if (iRet == -1) {
    PrintErrorMessage("VistaUDPSocket::ReceiveDatagram()");
    return 0;
  }
  return iRet;
}

string VistaUDPSocket::GetSocketTypeString() const {
  return "UDP";
}

void VistaUDPSocket::SetIsBuffering(bool bBuffering) {
  // we ignore this.
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
