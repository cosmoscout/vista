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
#elif defined (LINUX) || defined(DARWIN)
	#include <netinet/tcp.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/select.h>
	#include <errno.h>

#elif defined (SUNOS) || defined (IRIX)
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/filio.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>

#elif defined (HPUX)
	#include <unistd.h>          // ioctl(), FIONREAD
	#include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
	#include <sys/socket.h>      // socket(),gethostbyname(),connect()
	#include <arpa/inet.h>
	#include <netinet/tcp.h>
#endif
#include "VistaTCPSocket.h"

#include <VistaBase/VistaStreamUtils.h>

#include <cstdio>
#include <string>
using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaTCPSocket::VistaTCPSocket()
: IVistaSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP),
  m_bCloseRecursionFlag(false)
{
}


VistaTCPSocket::VistaTCPSocket(HANDLE iSockID)
: IVistaSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP),
  m_bCloseRecursionFlag(false)
{
	SetSocketID(iSockID);
}

VistaTCPSocket::~VistaTCPSocket()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

string VistaTCPSocket::GetSocketTypeString() const
{
	return "TCP";
}

void VistaTCPSocket::SetIsBuffering(bool bBuffering)
{
	if(GetIsValid())
	{
		int iBuffering = (bBuffering ? 0 : 1);

		if(!SetSockOpt(IPPROTO_TCP, TCP_NODELAY, (& iBuffering), sizeof ( int ) ))
		{
				// display error message
				PrintErrorMessage ("SetBuffering");
				return;
		}
	}

	// everything went alright
	IVistaSocket::SetIsBuffering(bBuffering);
}

bool VistaTCPSocket::CloseSocket(bool bSkipRead)
{
	// close socket will try to read off the last bytes on
	// the socket, which might lead to error and a subsequent
	// close that will be called by ReceiveRaw()
	// this is a flag to detect a recursing close in close
	if(m_bCloseRecursionFlag)
		return true;

	m_bCloseRecursionFlag = true;
	//vstr::outi() << "VistaTCPSocket[" << this << "]::CloseSocket(" 
	//		<< ( bSkipRead ? "true)" : "false" ) << std::endl;


	if(GetIsConnected())
	{
		// a connected tcp socket, ok, so we better wait for
		// send to finish, in case there are pending requests.
		if(!GetIsBlocking() && !GetErrorState())
		{
			WaitForSendFinish(0); // send
		}

		// shutdown write channel, will send FIN to the other side.
		if(shutdown(SOCKET(GetSocketID()), 1) < 0)
		{
			// NOTCONN errors are okay
#ifdef WIN32
			if( WSAGetLastError() != WSAENOTCONN )			
#else
			if( errno != ENOTCONN )
#endif
			{
				// ERROR, we should do something here (print some info)
				PrintErrorMessage("Error on Shutdown write channel. Bogus.\n");
			}
		}
	}


	char buf;
	if(GetIsBlocking())
		SetIsBlocking(false);


	// anyhow, the read channel still has a chance to receive something
	// it is good practice to read off this data
	if(!bSkipRead && HasBytesPending()) // see iff anything comes along
		while(ReceiveRaw(&buf, sizeof(char),500)>0); // read all off this socket in 500ms


	// the following code does not call CloseSocket(),
	// we reset the flag
	m_bCloseRecursionFlag = false;
	int iPrm = 0; // shutdown RD only
	if(GetIsConnected())
	{
		iPrm = 0; // shutdown RD, both should be closed now
		if(shutdown(SOCKET(GetSocketID()), iPrm) < 0)
		{
 			// NOTCONN errors are okay
#ifdef WIN32
			if( WSAGetLastError() != WSAENOTCONN )			
#else
			if( errno != ENOTCONN )
#endif
			{
				// ERROR, we should do something here (print some info)
				PrintErrorMessage("Error on Shutdown -- socket may not be marked closed!\n");			
			}
		}
	}
	return IVistaSocket::CloseSocket(); // close me
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


