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
#include <winsock2.h>
#include <ws2tcpip.h>

typedef int socklen_t;
#elif defined (LINUX) || (DARWIN)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>

#elif defined (SUNOS) || defined (IRIX)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>

#elif defined (HPUX)
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <arpa/inet.h>
	#include <netinet/in.h>

#else
	#error You have to define the target platform in order to compile ViSTA

#endif


#include <iostream>
using namespace std;


#include <cstring>

#include "VistaMcastIPAddress.h"
#include "VistaMcastSocketAddress.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaMcastSocketAddress::VistaMcastSocketAddress():VistaSocketAddress()
{
}

VistaMcastSocketAddress::VistaMcastSocketAddress(const VistaMcastSocketAddress &adr)
: VistaSocketAddress( adr )
{
}

VistaMcastSocketAddress::VistaMcastSocketAddress(const VistaSocketAddress &adr)
: VistaSocketAddress( adr )
{
   
}

VistaMcastSocketAddress::VistaMcastSocketAddress(const string &sHostName, int iPort)
: VistaSocketAddress(VistaIPAddress(sHostName), iPort)
{
	//struct hostent *h;
	//h=gethostbyname(sHostName.c_str());
	//memcpy( &(m_myAddress->sin_addr) , h->h_addr_list[0],h->h_length);
	//SetPortNumber(iPort);
}

VistaMcastSocketAddress::VistaMcastSocketAddress(const VistaMcastIPAddress &rAddress, int iPort)
: VistaSocketAddress(rAddress, iPort)
{

}


VistaMcastSocketAddress::~VistaMcastSocketAddress()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaMcastIPAddress VistaMcastSocketAddress::GetMcastIPAddress() const
{
	VistaMcastIPAddress adr;
	string sTmp;
	sTmp.resize(sizeof(in_addr));
	sTmp.assign( (char*)GetINAddress(), sizeof(in_addr));
	adr.SetAddress(sTmp);
	return adr;
}


void VistaMcastSocketAddress::GetMcastIPAddress(VistaMcastIPAddress &rOut)
{
	rOut = GetMcastIPAddress();
}


VistaMcastSocketAddress &VistaMcastSocketAddress::operator= (const VistaMcastSocketAddress &adr)
{
	// do deep copy
	 memcpy(m_myAddress, adr.m_myAddress, sizeof(sockaddr_in));
	return *this;
}


VistaMcastSocketAddress &VistaMcastSocketAddress::operator= (const sockaddr_in &rIn)
{
	memcpy(m_myAddress, &rIn, sizeof(sockaddr_in));
	return *this;
}

