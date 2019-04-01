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
typedef int socklen_t;
#else

#endif

#if defined (LINUX) || defined(DARWIN)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>
    #include <cerrno>
#elif defined (SUNOS) || defined (IRIX)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>
#elif defined (HPUX)
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <arpa/inet.h>
	#include <netinet/tcp.h>
#endif

#include "VistaTCPServerSocket.h"
#include "VistaSocketAddress.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaTCPServerSocket::VistaTCPServerSocket()
: VistaTCPSocket()
{
}


VistaTCPServerSocket::~VistaTCPServerSocket()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaTCPSocket *VistaTCPServerSocket::Accept() const
{
	VistaSocketAddress remote;
#if defined (WIN32)
	SOCKET iNewSockAddr=INVALID_SOCKET;
#else
	SOCKET iNewSockAddr=-1;
#endif
	int iNewSockAddrLength=remote.GetINAddressLength();

	//printf("sockid = %d\n", GetSocketID());

#if defined(HPUX)
	if((iNewSockAddr = accept(GetSocketID(), (sockaddr*)remote.GetINAddress(), &iNewSockAddrLength))<0)
#elif defined (WIN32)
	if((iNewSockAddr = accept(SOCKET(GetSocketID()), (sockaddr*)remote.GetINAddress(), (socklen_t*)&iNewSockAddrLength))==INVALID_SOCKET)
#else
	if((iNewSockAddr = TEMP_FAILURE_RETRY( accept(SOCKET(GetSocketID()), (sockaddr*)remote.GetINAddress(), (socklen_t*)&iNewSockAddrLength)))<0)
#endif
	{
		// error!!
		PrintErrorMessage("VistaTCPSocket::Accept()");
		return NULL;
	}

	//printf("new sockid = %d\n", iNewSockAddr);
	VistaTCPSocket *pSock = new VistaTCPSocket(HANDLE(iNewSockAddr));
	pSock->SetIsConnected(true);
	return pSock;
}


bool VistaTCPServerSocket::Listen(int iBacklog)
{
	if(listen(SOCKET(GetSocketID()), iBacklog) < 0)
	{
		return false;
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


