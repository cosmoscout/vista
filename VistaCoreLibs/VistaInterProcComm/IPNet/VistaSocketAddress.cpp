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
#include <winsock2.h>
#elif defined(LINUX) || defined(DARWIN)
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

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

#include <cstring>
#include <string>

using namespace std;

#include "VistaIPAddress.h"
#include "VistaSocketAddress.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaSocketAddress::VistaSocketAddress() {
  m_myAddress = new sockaddr_in;
  memset(m_myAddress, 0, sizeof(sockaddr_in));
  (*m_myAddress).sin_family      = AF_INET; // default, eh?
  (*m_myAddress).sin_addr.s_addr = htonl(INADDR_ANY);
}

VistaSocketAddress::VistaSocketAddress(const VistaSocketAddress& adr) {
  // do deep copy
  m_myAddress = new sockaddr_in;
  memcpy(m_myAddress, adr.m_myAddress, sizeof(sockaddr_in));
}

VistaSocketAddress::VistaSocketAddress(const string& sHostName, int iPort) {
  m_myAddress = new sockaddr_in;

  memset(m_myAddress, 0, sizeof(sockaddr_in));
  (*m_myAddress).sin_family      = AF_INET; // default, eh?
  (*m_myAddress).sin_addr.s_addr = htonl(INADDR_ANY);

  VistaIPAddress ip;
  ip.SetHostName(sHostName);

  SetIPAddress(ip);
  SetPortNumber(iPort);
}

VistaSocketAddress::VistaSocketAddress(const VistaIPAddress& rAddress, int iPort) {
  m_myAddress = new sockaddr_in;
  memset(m_myAddress, 0, sizeof(sockaddr_in));
  (*m_myAddress).sin_family      = AF_INET; // default, eh?
  (*m_myAddress).sin_addr.s_addr = htonl(INADDR_ANY);
  SetIPAddress(rAddress);
  SetPortNumber(iPort);
}

VistaSocketAddress::~VistaSocketAddress() {
  delete m_myAddress;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaSocketAddress::GetPortNumber() const {
  return ntohs((*m_myAddress).sin_port);
}

void VistaSocketAddress::SetPortNumber(int iPort) {
  (*m_myAddress).sin_port = htons(iPort);
}

void VistaSocketAddress::SetIPAddress(const VistaIPAddress& rIn) {
  memcpy(&(*m_myAddress).sin_addr, (const void*)rIn.GetINAddress(), rIn.GetINAddressLength());
}

VistaIPAddress VistaSocketAddress::GetIPAddress() const {
  VistaIPAddress adr;
  string         sTmp;
  sTmp.resize(sizeof(in_addr));
  sTmp.assign((char*)&(*m_myAddress).sin_addr, sizeof(in_addr));
  adr.SetAddress(sTmp);
  return adr;
}

void VistaSocketAddress::GetIPAddress(VistaIPAddress& rOut) {
  rOut = GetIPAddress();
}

void* VistaSocketAddress::GetINAddress() const {
  return (void*)m_myAddress;
}

int VistaSocketAddress::GetINAddressLength() const {
  return sizeof(*m_myAddress);
}

bool VistaSocketAddress::GetIsValid() const {
  return ((GetPortNumber() > 0) && GetIPAddress().GetIsValid());
}

VistaSocketAddress& VistaSocketAddress::operator=(const VistaSocketAddress& adr) {
  // do deep copy

  memcpy(m_myAddress, adr.m_myAddress, sizeof(sockaddr_in));
  return *this;
}

VistaSocketAddress& VistaSocketAddress::operator=(const sockaddr_in& rIn) {
  memcpy(m_myAddress, &rIn, sizeof(sockaddr_in));
  return *this;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
