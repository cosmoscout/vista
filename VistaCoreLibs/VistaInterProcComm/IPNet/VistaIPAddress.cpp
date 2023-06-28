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

#include "VistaIPAddress.h"
#include <cstdio>

#include <VistaBase/VistaStreamUtils.h>

#if defined(WIN32)
#pragma warning(disable : 4996)
#endif

#if defined(WIN32)
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#elif defined(LINUX) || defined(DARWIN)
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>

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

#include <iostream>
using namespace std;

#include <cstdio>
#include <cstring>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
static void TranslateHostResolveError(int iErrno) {
  vstr::errp() << "VistaIPAddress::HostResolveError: ";
#ifdef WIN32
  switch (iErrno) {
  case WSAEACCES:
    vstr::err() << "Permission denied.";
    break;
  case WSAEADDRINUSE:
    vstr::err() << "Address already in use.";
    break;
  case WSAEADDRNOTAVAIL:
    vstr::err() << "Cannot assign requested address.";
    break;
  case WSAEAFNOSUPPORT:
    vstr::err() << "Address family not supported by protocol family.";
    break;
  case WSAEALREADY:
    vstr::err() << "Operation already in progress.";
    break;
  case WSAECONNABORTED:
    vstr::err() << "Software caused connection abort.";
    break;
  case WSAECONNREFUSED:
    vstr::err() << "Connection refused.";
    break;
  case WSAECONNRESET:
    vstr::err() << "Connection reset by peer.";
    break;
  case WSAEDESTADDRREQ:
    vstr::err() << "Destination address required.";
    break;
  case WSAEFAULT:
    vstr::err() << "Bad address.";
    break;
  case WSAEHOSTDOWN:
    vstr::err() << "Host is down.";
    break;
  case WSAEHOSTUNREACH:
    vstr::err() << "No route to host.";
    break;
  case WSAEINPROGRESS:
    vstr::err() << "Operation now in progress.";
    break;
  case WSAEINTR:
    vstr::err() << "Interrupted function call.";
    break;
  case WSAEINVAL:
    vstr::err() << "Invalid argument.";
    break;
  case WSAEISCONN:
    vstr::err() << "Socket is already connected.";
    break;
  case WSAEMFILE:
    vstr::err() << "Too many open files.";
    break;
  case WSAEMSGSIZE:
    vstr::err() << "Message too long.";
    break;
  case WSAENETDOWN:
    vstr::err() << "Network is down.";
    break;
  case WSAENETUNREACH:
    vstr::err() << "Network is unreachable.";
    break;
  case WSAENETRESET:
    vstr::err() << "Network dropped connection on reset.";
    break;
  case WSAENOBUFS:
    vstr::err() << "No buffer space available.";
    break;
  case WSAENOPROTOOPT:
    vstr::err() << "Bad protocol option.";
    break;
  case WSAENOTCONN:
    vstr::err() << "Socket is not connected.";
    break;
  case WSAENOTSOCK:
    vstr::err() << "Socket operation on non-socket.";
    break;
  case WSAEOPNOTSUPP:
    vstr::err() << "Operation not supported.";
    break;
  case WSAEPFNOSUPPORT:
    vstr::err() << "Protocol family not supported.";
    break;
  case WSAEPROCLIM:
    vstr::err() << "Too many processes.";
    break;
  case WSAEPROTONOSUPPORT:
    vstr::err() << "Protocol not supported.";
    break;
  case WSAEPROTOTYPE:
    vstr::err() << "Protocol wrong type for socket.";
    break;
  case WSAESHUTDOWN:
    vstr::err() << "Cannot send after socket shutdown.";
    break;
  case WSAESOCKTNOSUPPORT:
    vstr::err() << "Socket type not supported.";
    break;
  case WSAETIMEDOUT:
    vstr::err() << "Connection timed out.";
    break;
  /*	case WSATYPE_NOT_FOUND:
                  vstr::err() << "Class type not found.";
                  break;
          case WSAHOST_NOT_FOUND:
                  vstr::err() << "Host not found.";
                  break;
          case WSA_INVALID_HANDLE:
                  vstr::err() << "Specified event object handle is invalid.";
                  break;
          case WSA_INVALID_PARAMETER:
                  vstr::err() << "One or more parameters are invalid.";
                  break;
          case WSAINVALIDPROCTABLE:
                  vstr::err() << "Invalid procedure table from service provider.";
                  break;
          case WSAINVALIDPROVIDER:
                  vstr::err() << "Invalid service provider version number.";
                  break;
          case WSA_IO_INCOMPLETE:
                  vstr::err() << "Overlapped I/O event object not in signaled state.";
                  break;
          case WSA_IO_PENDING:
                  vstr::err() << "Overlapped operations will complete later.";
                  break;
          case WSA_NOT_ENOUGH_MEMORY:
                  vstr::err() << "Insufficient memory available.";
                  break;
  */
  case WSANOTINITIALISED:
    vstr::err() << "Successful WSAStartup not yet performed.";
    break;
  case WSANO_DATA:
    vstr::err() << "Valid name, no data record of requested type.";
    break;
  case WSANO_RECOVERY:
    vstr::err() << "This is a non-recoverable error.";
    break;
  /*	case WSAPROVIDERFAILEDINIT:
                  vstr::err() << "Unable to initialize a service provider.";
                  break;
          case WSASYSCALLFAILURE:
                  vstr::err() << "System call failure.";
                  break;
  */
  case WSASYSNOTREADY:
    vstr::err() << "Network subsystem is unavailable.";
    break;
  case WSATRY_AGAIN:
    vstr::err() << "Non-authoritative host not found.";
    break;
  case WSAVERNOTSUPPORTED:
    vstr::err() << "WINSOCK.DLL version out of range.";
    break;
  case WSAEDISCON:
    vstr::err() << "Graceful shutdown in progress.";
    break;
    /*		case WSA_OPERATION_ABORTED:
                    vstr::err() << "Overlapped operation aborted.";
                    break;
    */
  case WSAEWOULDBLOCK:
    break;
  default:
    vstr::err() << "Unknown error number." << iErrno;
    break;
  }
#else
  switch (iErrno) {
  case HOST_NOT_FOUND: {
    vstr::err() << "HOST NOT FOUND";
    break;
  }
  case NO_ADDRESS: {
    vstr::err() << "NO IP ADDRESS";
    break;
  }
  case NO_RECOVERY:
  case TRY_AGAIN: {
    vstr::err() << "DNS ERROR? (NO RECOVERY OR TRY AGAIN LATER)";
    break;
  }
  default: {
    vstr::err() << "UNKNOWN ERROR (" << iErrno << ")";
    break;
  }
  }
#endif
  vstr::err() << std::endl;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaIPAddress::VistaIPAddress() {
  m_iAddressType = VIPA_UNASSIGNED;
  m_bIsValid     = false;
}

VistaIPAddress::VistaIPAddress(const string& sHostName) {
  m_iAddressType = VIPA_UNASSIGNED;
  m_bIsValid     = false;
  SetHostName(sHostName);
}

VistaIPAddress::VistaIPAddress(const VistaIPAddress& adr) {
  m_iAddressType = adr.m_iAddressType;
  m_sHostName    = adr.m_sHostName;
  m_bIsValid     = adr.m_bIsValid;

  // SetHostName(m_sHostName);
  //   m_bIsValid = adr.m_bIsValid;
  //   m_sHostName = adr.m_sHostName;
  if (m_iAddressType != VIPA_UNASSIGNED)
    m_sInAddress.assign(adr.m_sInAddress.begin(), adr.m_sInAddress.end());
}

VistaIPAddress::~VistaIPAddress() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaIPAddress::ResolveHostName(const string& sHostName) {
  struct hostent* pheDetails;

  if (isalpha(sHostName[0])) {
    // this failed, this means that we have no dotted
    // notation in sHostName, we can use gethostbyname
    // for resolving the address
    pheDetails = gethostbyname(sHostName.c_str());
  } else {
    // this worked! we have a dotted notation now
    // use gethostbyaddr for hostent resolvement
    int iAddr  = inet_addr(sHostName.c_str());
    pheDetails = gethostbyaddr((char*)&iAddr, sizeof(iAddr), AF_INET);
  }

  if (pheDetails) {
    m_sHostName = pheDetails->h_name; // ok, hostname resolved
    m_sInAddress.resize(pheDetails->h_length);
    m_sInAddress.assign(pheDetails->h_addr_list[0], pheDetails->h_length);
    m_iAddressType = pheDetails->h_addrtype;
    return true;
  }

  // gethostbyname/addr failed - lets try getaddrinfo
  int              nStatus;
  struct addrinfo  oHints;
  struct addrinfo* pServInfo;

  memset(&oHints, 0, sizeof(addrinfo)); // make sure the struct is empty
  oHints.ai_family   = AF_INET;         // we only support IPv4
  oHints.ai_socktype = 0;
  oHints.ai_flags    = AI_CANONNAME;

  if ((nStatus = getaddrinfo(sHostName.c_str(), NULL, &oHints, &pServInfo)) != 0) {
    vstr::warnp() << "[VistaIpAddress]: resolving hostname [" << sHostName.c_str()
                  << "] failed with error: " << gai_strerror(nStatus) << std::endl;
    m_bIsValid = false;
    return false;
  }

  if (pServInfo->ai_canonname)
    m_sHostName = pServInfo->ai_canonname; // ok, hostname resolved
  else
    m_sHostName = ""; // no hostname

  m_sInAddress.resize(pServInfo->ai_addrlen);
  m_sInAddress.assign((char*)pServInfo->ai_addr + 4, pServInfo->ai_addrlen - 4);
  m_iAddressType = pServInfo->ai_family;

  freeaddrinfo(pServInfo);

  return true;
}

void VistaIPAddress::SetHostName(const string& sHostName) {
  if (sHostName != m_sHostName)
    m_bIsValid = ResolveHostName(sHostName);
}

void VistaIPAddress::SetAddress(const string& sAddress) {
#ifdef LINUX
  char acHost[1024];
  char acService[20];

  sockaddr_in* pAddr = new sockaddr_in;
  memset(pAddr, 0, sizeof(sockaddr_in));
  pAddr->sin_family = AF_INET;
  pAddr->sin_port   = htons(0);
  memcpy((void*)&pAddr->sin_addr, (const void*)sAddress.data(), sAddress.length());

  int nRes = getnameinfo((sockaddr*)pAddr, sizeof(sockaddr_in), acHost, 1024, acService, 20, 0);
  if (nRes != 0) {

    vstr::warnp() << "[VistaIpAddress]: getnameinfo failed with error:\n";

    switch (nRes) {
    case EAI_AGAIN:
      vstr::warni() << "[EAI_AGAIN] The name could nor be resolved at this time. Try again later."
                    << std::endl;
      break;
    case EAI_BADFLAGS:
      vstr::warni() << "EAI_BADFLAGS" << std::endl;
      break;
    case EAI_FAIL:
      vstr::warni() << "EAI_FAIL" << std::endl;
      break;
    case EAI_FAMILY:
      vstr::warni() << "EAI_FAMILY" << std::endl;
      vstr::outi() << sAddress.length() << std::endl;
      break;
    case EAI_MEMORY:
      vstr::warni() << "TEAI_MEMORY" << std::endl;
      break;
    case EAI_NONAME:
      vstr::warni() << "EAI_NONAME" << std::endl;
      break;
    case EAI_OVERFLOW:
      vstr::warni() << "EAI_OVERFLOW" << std::endl;
      break;
    case EAI_SYSTEM:
      vstr::warni() << "EAI_SYSTEM" << std::endl;
      break;
    }

    m_bIsValid = false;
    return;
  }

  m_sHostName    = acHost; // ok, hostname resolved
  m_sInAddress   = sAddress;
  m_iAddressType = (int)((const sockaddr*)sAddress.data())->sa_family;
  m_bIsValid     = true;

#else
  struct hostent* pheDetails = gethostbyaddr(sAddress.data(), (int)sAddress.length(), AF_INET);

  if (!pheDetails) {
#if !defined(HPUX)
    /** @todo fix this 'compatibility' feature */
    TranslateHostResolveError(h_errno);
#endif
    m_bIsValid = false;
    return;
  }

  m_sHostName = pheDetails->h_name; // ok, hostname resolved
  m_sInAddress.resize(pheDetails->h_length);
  m_sInAddress.assign(pheDetails->h_addr_list[0], pheDetails->h_length);

  m_iAddressType = pheDetails->h_addrtype;
  m_bIsValid     = true;
#endif
}

void VistaIPAddress::GetHostName(string& sHostName) const {
  sHostName = m_sHostName;
}

const char* VistaIPAddress::GetHostNameC() const {
  return m_sHostName.c_str();
}

string VistaIPAddress::GetIPAddressC() const {
  if (!m_bIsValid)
    return "[INVALID]";

  char buffer[256];
  memset(buffer, 0, 256);

  sprintf(buffer, "%d.%d.%d.%d", (int)((unsigned char)m_sInAddress[0]),
      (int)((unsigned char)m_sInAddress[1]), (int)((unsigned char)m_sInAddress[2]),
      (int)((unsigned char)m_sInAddress[3]));

  return string(buffer);
}

const void* VistaIPAddress::GetINAddress() const {
  return (const void*)m_sInAddress.data();
}

void VistaIPAddress::SetRawINAddress(struct in_addr* pData, int iSize, bool bSetName) {
  m_sInAddress.resize(iSize);
  m_sInAddress.assign((const char*)pData, iSize);

  if (bSetName)
    GetAddressString(m_sHostName);
}

int VistaIPAddress::GetINAddressLength() const {
  return (int)m_sInAddress.size();
}

void VistaIPAddress::GetAddressString(string& sString) const {
  sString = inet_ntoa(*((const in_addr*)(m_sInAddress.data())));
}

void VistaIPAddress::GetAddressTypeString(string& sString) const {
  switch (m_iAddressType) {
  case VIPA_UNASSIGNED: {
    sString = "Unassigned";
    break;
  }
  case AF_INET: {
    sString = "IPV4 (AF_INET)";
    break;
  }
  default: {
    sString = "Unknown, sorry";
    break;
  }
  }
}

int VistaIPAddress::GetAddressType() const {
  return m_iAddressType;
}

VistaIPAddress& VistaIPAddress::operator=(const VistaIPAddress& inAddr) {
  // printf("VistaIPAddress::operator=\n");
  SetHostName(inAddr.GetHostNameC());
  m_iAddressType = inAddr.m_iAddressType;
  //    m_bIsValid = inAddr.m_bIsValid;
  //    m_sInAddress.assign(inAddr.m_sInAddress.begin(), inAddr.m_sInAddress.end());
  return *this;
}

VistaIPAddress& VistaIPAddress::operator=(const string& sHostName) {
  SetHostName(sHostName);
  m_iAddressType = AF_INET; // we simply assume this...
  return *this;
}

bool VistaIPAddress::GetIsValid() const {
  return m_bIsValid;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
