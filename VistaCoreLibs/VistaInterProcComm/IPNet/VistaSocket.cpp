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


#include "VistaSocket.h"
#include "VistaIPAddress.h"
#include "VistaSocketAddress.h"
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>

#if defined(VISTA_IPC_USE_EXCEPTIONS)
#include <VistaBase/VistaExceptionBase.h>
#endif

#include <VistaBase/VistaStreamUtils.h>

#include <cassert>
#ifdef WIN32
	#include <winsock2.h>
	typedef int socklen_t;
#else
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/ioctl.h>
	#include <arpa/inet.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <time.h>
#endif
#include <iostream>

#include <cerrno>     // errno
#include <cstring>
#include <cstdio>
#include <fcntl.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

bool IVistaSocket::PrintErrorMessage(const std::string &sMethodName)
{
	std::string sErrorMessage;
	#ifdef WIN32
		int nErrorCode = WSAGetLastError();
		if( nErrorCode && ( nErrorCode != WSAEWOULDBLOCK ) )
		{
			sErrorMessage.resize( 1000, '\0' );
			int nMessageSize = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
												NULL, nErrorCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
												(LPTSTR) &sErrorMessage[0], 1024, NULL );
			sErrorMessage.resize( nMessageSize );
		}
		else
			return false;
	#else
		int nErrorCode = errno;
		if( nErrorCode & ( nErrorCode != EWOULDBLOCK ) )
		{
			char sBuffer[1024];
#if ( _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600 ) && ! _GNU_SOURCE
			if( strerror_r( nErrorCode, sBuffer, 1024 ) == 0 )
				sErrorMessage = sBuffer;
#else
			sErrorMessage = strerror_r( nErrorCode, sBuffer, 1024 );
#endif
		}
		else
			return false;
	#endif
	if( sErrorMessage.empty() )
		sErrorMessage = "Unknown Error";

	vstr::errp() << "IVistaSocket::" << sMethodName << " failed with code " << nErrorCode << ": \n";
	vstr::errp() << vstr::singleindent << sErrorMessage << std::endl;

	return true;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaSocket::IVistaSocket(int iDomain, int iType, int iProtocol)
{
	m_iDomain = iDomain;
	m_iType   = iType;
	m_iProtocol = iProtocol;
	m_bIsBound = false;
	m_bIsConnected = false;
	m_bIsBlocking = true;
	m_bIsBuffering = false;
	m_bShowRaw = false;
	m_bErrorState = false;
	m_iSocketID = (HANDLE)~0;
	//m_bMulticast = false;
#if defined(WIN32)
	m_pWaitEvent = new VistaThreadEvent( VistaThreadEvent::WAITABLE_EVENT ); 
#else
	m_pWaitEvent = NULL;
#endif
}

IVistaSocket::~IVistaSocket()
{
	delete m_pWaitEvent;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void IVistaSocket::SetIsConnected(bool bConnected)
{
	m_bIsConnected = bConnected;
}

bool IVistaSocket::GetSockOpt(int iCodeLevel, int iOptionName, void *pOptionData, int &iDataLength) const
{
#if !defined(HPUX)
	if (getsockopt(SOCKET(m_iSocketID), iCodeLevel, iOptionName, (char*)pOptionData, (socklen_t*)&iDataLength) < 0)
#else
	if (getsockopt(int(m_iSocketID), iCodeLevel, iOptionName, (char*)pOptionData, &iDataLength) < 0)
#endif
	{
		// error.
		return false;
	}
	return true;
}

bool IVistaSocket::SetSockOpt(int iCodeLevel, int iOptionName, void *pOptionData, int iDataLength)
{
#ifdef WIN32
	if( setsockopt(SOCKET(m_iSocketID), iCodeLevel, iOptionName, (char*)pOptionData, iDataLength) < 0)
#else
	if( TEMP_FAILURE_RETRY( setsockopt(SOCKET(m_iSocketID), iCodeLevel, iOptionName, (char*)pOptionData, iDataLength) ) < 0) 	
#endif
	{
		return false;
	}
	return true;
}

bool IVistaSocket::OpenSocket()
{
	if( m_iSocketID != HANDLE( ~0 ) )
		return false;
	if((m_iSocketID = HANDLE(socket (m_iDomain, m_iType, m_iProtocol))) <= 0)
	{
		// error
		m_iSocketID = HANDLE(~0);
		PrintErrorMessage("OpenSocket()");
		return false;
	}
	SetIsBuffering(m_bIsBuffering);
	SetIsBlocking(m_bIsBlocking);
	SetErrorState(false);
	//printf("IVistaSocket::OpenSocket() -- %d\n", m_iSocketID);
	return true;
}

bool IVistaSocket::CloseSocket(bool bSkipRead)
{
	if ( this->m_iSocketID != HANDLE(~0) )
	{
		//printf("IVistaSocket::CloseSocket() -- %d\n", m_iSocketID);
		// close socket
		#ifdef WIN32
			closesocket ( SOCKET(this->m_iSocketID) );
		#else
			close ( int(this->m_iSocketID) );
		#endif
			m_iSocketID = HANDLE(~0);
			m_bIsConnected=false;
			m_bIsBound=false;
	}
	SetErrorState(false);
	return true; // a closed socket stays closed
}

bool IVistaSocket::GetIsBuffering() const
{
	return m_bIsBuffering;
}

bool IVistaSocket::GetIsBlocking() const
{
	return m_bIsBlocking;
}

bool IVistaSocket::VerifyBlocking()
{
#if !defined(WIN32)
	if(!GetIsValid())
		return true; // no sockets verify to true

	int iRes = TEMP_FAILURE_RETRY( fcntl(int(m_iSocketID), F_GETFD) );
	if( ( iRes & O_NONBLOCK ) == O_NONBLOCK)
	{
		// ok, marked non-blocking
		if(GetIsBlocking())
			{
				// internal flag says we are blocking
				// we swap this
				m_bIsBlocking = false;
			}
	}
	else
	{
		// ok, desc is marked blocking
		if(!GetIsBlocking())
		{
			// internal flag says, we are non-blocking
			// we flip this
			m_bIsBlocking = true;
		}
	}

	return m_bIsBlocking;
#else
	if(!GetIsValid())
		return true;
	if(GetIsBlocking())
	{
		// ok, we are supposed to be blocking
		u_long isNonBlocking=0;
		if (ioctlsocket ( SOCKET(m_iSocketID), FIONBIO, & isNonBlocking) == SOCKET_ERROR)
		{
			int iErr = WSAGetLastError();
			if(WSAGetLastError() == WSAEINVAL)
			{
				// setting socket to blocket state returned
				// WSAEINVAL
				// according to the MSDN, this socket was touched
				// by WSAAsyncSelect/WSAEventSelect, and marked non-blocking
				m_bIsBlocking = true;
			}
			else
			{
				vstr::errp() << "IVistaSocket::VerifyBlocking() -- WSA error ("
						<< iErr << ")" << std::endl;
			}
		}
	}
	return m_bIsBlocking;
#endif
}

bool IVistaSocket::GetIsBoundToAddress() const
{
	return m_bIsBound;

}

bool IVistaSocket::GetIsConnected() const
{
	return m_bIsConnected;
}

bool IVistaSocket::GetIsOpen() const
{
	return (m_iSocketID !=  HANDLE(~0));
}

bool IVistaSocket::GetIsValid() const
{
	return (m_iSocketID != HANDLE(~0));
}

void IVistaSocket::SetIsBuffering(bool bBuffering)
{
   m_bIsBuffering = bBuffering;
}

void IVistaSocket::SetIsBlocking(bool bBlocking)
{
	if(GetIsValid())
	{

		#if defined( WIN32 )
				unsigned long isNonBlocking = (bBlocking ? 0 : 1);
			if (ioctlsocket ( SOCKET(this->m_iSocketID), FIONBIO, & isNonBlocking) == SOCKET_ERROR)
		#else
			int isNonBlocking = (bBlocking ? 0:1);
			if (ioctl ( int(this->m_iSocketID), FIONBIO, & isNonBlocking))
		#endif
			{
				// display error message
				this->PrintErrorMessage ("SetBlocking");
				return;
			}
	}
	// everything went alright
	m_bIsBlocking = bBlocking;
}

bool IVistaSocket::BindToAddress(const VistaSocketAddress &rMyAddress)
{
	int nRet = bind( SOCKET(m_iSocketID), (sockaddr *)rMyAddress.GetINAddress(), rMyAddress.GetINAddressLength() );
	if( nRet != 0 )
	{
		PrintErrorMessage("BindToAddress()");
		return false;
	}
	m_bIsBound = true; // toggle
	return true;
}

bool IVistaSocket::ConnectToAddress(const VistaSocketAddress &rAddress)
{
#ifdef WIN32
	int nRet = connect( SOCKET(this->m_iSocketID), (sockaddr *)rAddress.GetINAddress(), rAddress.GetINAddressLength() );
#else
	int nRet = TEMP_FAILURE_RETRY( connect( SOCKET(this->m_iSocketID), (sockaddr *)rAddress.GetINAddress(), rAddress.GetINAddressLength() ) );
#endif
	if( nRet == -1 )
	{
		//m_iSocketID = -1;
		if(PrintErrorMessage("ConnectToAddress()")==false)
		{
			vstr::warnp() << "IVistaSocket::ConnectToAddress("
					<< rAddress.GetIPAddress().GetHostNameC()
					<< " [" << rAddress.GetIPAddress().GetIPAddressC()
					<< "] -- error during connect" << std::endl;
			SetErrorState(true);
			return false;
		}
		else
		{
			if(GetIsBlocking())
				return false; // could not connect on a blocking socket

			/**
			 *@todo
			 * note: we should treat connected state as
			 * yes, no, pending (tri-nary)
			 */
		}
	}
	m_bIsConnected = true; // toggle
	return true;
}

bool IVistaSocket::GetLocalSockName(VistaSocketAddress &rSocket) const
{

#if !defined(HPUX)
	socklen_t slDummy=rSocket.GetINAddressLength();
	if (getsockname(SOCKET(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), (socklen_t*)&slDummy) < 0)
#else
	int slDummy=rSocket.GetINAddressLength();
	if (getsockname(int(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), &slDummy) < 0)
#endif
	{
		PrintErrorMessage("GetLocalSocket()");
		return false;
	}
	return true;
}

bool IVistaSocket::GetPeerSockName(VistaSocketAddress &rSocket) const
{
#if !defined(HPUX)
	socklen_t slDummy=rSocket.GetINAddressLength();
	if(getpeername(SOCKET(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), (socklen_t*)&slDummy) < 0)
#else
	int slDummy=rSocket.GetINAddressLength();
	if(getpeername(int(m_iSocketID), (sockaddr *)rSocket.GetINAddress(), &slDummy) < 0)
#endif
	{
		PrintErrorMessage("IVistaSocket::GetPeerSockName()");
		return false;
	}
	return true;
}

unsigned long IVistaSocket::HasBytesPending() const
{
	if(!GetIsValid())
		return 0; // no socket has no data

	#ifdef WIN32
		unsigned long result = 0;
	// windows function names
		if ( ! ioctlsocket ( SOCKET(m_iSocketID), FIONREAD, & result ) )
			return result;
	#else
		int result = 0;
		if ( ioctl ( int(m_iSocketID), FIONREAD, & result ) != -1 )
			return result;
	#endif

	return 0;
}

unsigned long IVistaSocket::WaitForIncomingData(int iTimeout)
{
	assert(GetIsValid());
#if defined(WIN32)
	bool b=GetIsBlocking();
	// associate with event (socket gets non-blocking!)
	WSAEventSelect(SOCKET(GetSocketID()),
			   (*m_pWaitEvent).GetEventSignalHandle(),
			   FD_READ|FD_CLOSE);
	DWORD iRet = WaitForSingleObject((*m_pWaitEvent).GetEventSignalHandle(), (iTimeout>0 ? iTimeout : INFINITE));
	// remove event association
	WSAEventSelect(SOCKET(GetSocketID()),
			   (*m_pWaitEvent).GetEventSignalHandle(),
			   0);
	if(b) // set to blocking again
		SetIsBlocking(true);
	switch(iRet)
	{
	case WAIT_ABANDONED:
		{			
		#if defined(VISTA_IPC_USE_EXCEPTIONS)
			VISTA_THROW("WaitForIncomingData -- Exception", 0x00000104)
		#endif
			break;
		}
	case WAIT_TIMEOUT:
		{
			return ~0; // omit tcp-query for pending bytes
			break;
		}
	case WAIT_OBJECT_0: // all clear
		{
			break;
		}
	case WAIT_FAILED:
		{
		#if defined(VISTA_IPC_USE_EXCEPTIONS)
			VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000105)
		#endif
			break;
		}
	default:
		printf("WaitForIncomingData() -- %d\n", HasBytesPending());
		break;
	}
	return HasBytesPending();
#else // UNIX

  struct timeval tv;
  struct timeval *etv = &tv;
  if(iTimeout != 0)
  {
	tv.tv_sec  = iTimeout/1000;
	tv.tv_usec = (iTimeout%1000)*1000;
  }
  else
  {
	  //tv.tv_sec = 0;
	  //tv.tv_usec= 0;
	  etv = NULL;
  }
	fd_set myRdSocks;
	fd_set myExSocks;
	FD_ZERO(&myRdSocks);
	FD_ZERO(&myExSocks);
	FD_SET(int(m_iSocketID), &myRdSocks);
	FD_SET(int(m_iSocketID), &myExSocks);
	
	int iRet = TEMP_FAILURE_RETRY( select( int(m_iSocketID)+1, &myRdSocks, NULL, &myExSocks, etv ) );	
	if(iRet == 0)
	{
		// timeout
		return ~0;
	}
	if(iRet == -1)
	{
		PrintErrorMessage( "WaitForIncomingData" );
#if defined(VISTA_IPC_USE_EXCEPTIONS)
		VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000104)
#endif
	}

	if(FD_ISSET(int(m_iSocketID), &myExSocks))
	{
		// whoah!
		printf("-- WARNING -- socket is in exception array.\n");
#if defined(VISTA_IPC_USE_EXCEPTIONS)
		VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000104)
#endif
	}

	// printf("IVistaSocket[%d]::WaitForIncomingData() -- %d bytes ready\n", m_iSocketID, iSize);
	return HasBytesPending();
#endif // UNIX PART
}

unsigned long IVistaSocket::WaitForSendFinish(int iTimeout)
{
	if(!GetIsValid())
		return 0;
	//assert(m_iSocketID>=0);
  struct timeval tv;
  struct timeval *etv = &tv;
  //iTimeout *= 1000;
  if(iTimeout != 0)
  {
	tv.tv_sec  = iTimeout/1000;
	tv.tv_usec = (iTimeout%1000)*1000;
  }
  else
  {
#if defined(WIN32)
	  tv.tv_sec = 0;
	  tv.tv_usec= 0;
#else
	  etv = NULL;
#endif
  }

#if defined(WIN32)
	FD_SET myWrSocks;
	FD_SET myExSocks;
#else
	fd_set myWrSocks;
	fd_set myExSocks;
#endif
	FD_ZERO(&myWrSocks);
	FD_ZERO(&myExSocks);
#ifdef WIN32
	// FD_SET emits a "conditional expression is constant" warning - we disable it
#pragma warning( push )
#pragma warning( disable : 4127 )
#endif
	FD_SET(SOCKET(m_iSocketID), &myWrSocks);
	FD_SET(SOCKET(m_iSocketID), &myExSocks);
#ifdef WIN32
#pragma warning( pop )
#endif

#ifdef WIN32
	select(int(SOCKET(m_iSocketID)+1), NULL, &myWrSocks, &myExSocks, etv);
#else
	TEMP_FAILURE_RETRY( select(int(SOCKET(m_iSocketID)+1), NULL, &myWrSocks, &myExSocks, etv) );
#endif
	// @TODO: adjust timeout if we receive an EINTR
	
	if(FD_ISSET(SOCKET(m_iSocketID), &myExSocks))
	{
		// whoah!
		printf("-- WARNING -- socket is in exception array.\n");
#if defined(VISTA_IPC_USE_EXCEPTIONS)
		VISTA_THROW("IVistaSocket::WaitForSendFinish -- Exception", 0x00000103)
#else
		return 0;
#endif		
	}
	return 1;
}

int  IVistaSocket::SendRaw(const void *pvBuffer, const int iLength, int iFlags)
{
	if(!GetIsValid())
		return -1;
	if(m_bShowRaw)
	{
		printf("IVistaSocket[%d]::SendRaw() -- [%d] bytes\n", int(m_iSocketID), iLength);
		for(int i=0; i < iLength; ++i)
		{
			printf("%02x ", ((const unsigned char*)pvBuffer)[i]);
			if(i && (i%16==0))
				printf("\n");
		}
		printf("\n");
	}

	
	int iRec = 0;
	do
	{
#ifdef WIN32
		iRec = send(SOCKET(m_iSocketID), (const char *)pvBuffer, iLength, iFlags);
#else
		iRec = TEMP_FAILURE_RETRY( send(SOCKET(m_iSocketID), (const char *)pvBuffer, iLength, iFlags) );
#endif
	} while( iRec == -1 && errno == EINTR );
	if(iRec < 0)
	{
		if(PrintErrorMessage("SendRaw()") == false)
		{
			// we mark the socket close
			SetErrorState(true);
			CloseSocket();
			// idicate network failure with an exception.
			#if defined(VISTA_IPC_USE_EXCEPTIONS)
			VISTA_THROW("IVistaSocket::SendRaw -- Exception\n", 0x00000101)
			#endif
		}
		else
		{
			// PrintErrorMessage returned true, so this is a non-blocking write
			// (most probably), we will return 0 here, as this is our semantics
			// on a non-blocking partially non-data write
			return 0;
		}
	}
	return iRec;
}


int  IVistaSocket::ReceiveRaw(void *pvBuffer, const int iLength, int iTimeout, int iFlags)
{
	if(!GetIsValid())
		return -1;


	int iReadLength = iLength;
	if(iTimeout)
	{
		int iReady = WaitForIncomingData(iTimeout);
		if(iReady == ~0) // timeout == ~0
			return 0; // the user provided timeout, so 0 is a good guess ;)

		// a failure results in an exception, so we will not get here (hopefully)
		// go a catch the exception in an upper level

		if(iReady < iLength)
			iReadLength = iReady; // adjust without buffer-size-violence
		if(iReadLength == 0)
			return (iReadLength != iLength ? -1 : iLength); // iff we could not read the desired value, we return -1
	}
	int iRet = 0;
	do
	{
#ifdef WIN32
		iRet = recv(SOCKET(m_iSocketID), (char *)pvBuffer, iReadLength, iFlags);
#else
		iRet = TEMP_FAILURE_RETRY( recv(SOCKET(m_iSocketID), (char *)pvBuffer, iReadLength, iFlags) );		
#endif
	} while( iRet == -1 && errno == EINTR );
	if(m_bShowRaw)
	{
		printf("ReceiveRaw() -- [%d] bytes\n", iRet);
		for(int i=0; i < iRet; ++i)
		{
			printf("%02x ", ((unsigned char*)pvBuffer)[i]);
			if(i && (i%16==0))
				printf("\n");
		}
		printf("\n");
   }
	if(iRet < 0)
	{
		if(PrintErrorMessage("ReceiveRaw") == false)
		{
			// it's a real error and _not_ WOULDBLOCK
			if(GetSocketTypeString() != std::string("UDP"))
			{
				// we have a real error on a tcp socket
				SetErrorState(true);
				CloseSocket();
				#if defined(VISTA_IPC_USE_EXCEPTIONS)
				VISTA_THROW("IVistaSocket::ReceiveRaw -- Exception", 0x00000102)
				#endif
			}
			// we are less strict with UPD sockets, so we return the -1 in iRet below
		}
		else
		{
			// ok, recv returned -1, PrintErrorMessage() returned true,
			// this indicates a WOULDBLOCK for non blocking read
			// we will return 0, as this is our sematics for
			// sucessful, but no-data non-blocking receive
			return 0;
		}

	}
	return iRet;
}

void IVistaSocket::SetShowRawSendAndReceive(bool bShowRaw)
{
	m_bShowRaw = bShowRaw;
}

bool IVistaSocket::GetShowRawSendAndReceive() const
{
	return m_bShowRaw;
}

HANDLE  IVistaSocket::GetSocketID() const
{
	return m_iSocketID;
}

bool IVistaSocket::SetSocketID(HANDLE iSockID)
{
	if(GetIsValid())
		return false;
	m_iSocketID = iSockID;
	return true;
}

bool IVistaSocket::Flush()
{
	return false;
}

int IVistaSocket::GetPermitBroadcast() const
{
	int iOn;      // 0 disables broadcast else enables
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_BROADCAST, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

void IVistaSocket::SetPermitBroadcast(int iOn)
{   // 0 disables broadcast else enables
	int iSize = sizeof(iOn);
	if(!SetSockOpt(SOL_SOCKET, SO_BROADCAST, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetDebugInfoRecord() const
{
	int iOn;
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_DEBUG, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

int IVistaSocket::GetDontRoute() const
{
	int iOn;
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_DONTROUTE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

void IVistaSocket::SetDontRoute(int iOn)
{   // A nonzero value enables SO_DONTROUTE. A value of 0 disables SO_DONTROUTE.
	int iSize = sizeof(iOn);
	if(!SetSockOpt(SOL_SOCKET, SO_DONTROUTE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetErrorStatus() const
{
	int iValue;
	int iLength = sizeof(iValue);
	if(!GetSockOpt(SOL_SOCKET, SO_ERROR, (void*)&iValue, iLength))
	{
	  // we should utter a warning here...
	}
	return iValue;
}

int IVistaSocket::GetKeepConnectionsAlive() const
{
	int iOn;
	int iSize = sizeof(iOn);
	if(!GetSockOpt(SOL_SOCKET, SO_KEEPALIVE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
	return iOn;
}

void IVistaSocket::SetKeepConnectionsAlive(int iOn)
{   //A nonzero value enables keepalives. A value of 0 disables keepalives.
	int iSize = sizeof(iOn);
	if(!SetSockOpt(SOL_SOCKET, SO_KEEPALIVE, (void*)&iOn, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetLingerOnClose() const
{ //When the l_onoff field is nonzero, SO_LINGER is enabled. When it is 0, SO_LINGER is disabled.
  //If SO_LINGER is being enabled, the l_linger field specifies the timeout period, in seconds
	linger ling;
	int iSize = sizeof(linger*);
	if(!GetSockOpt(SOL_SOCKET, SO_LINGER, &ling, iSize))
	{
	  // we should utter a warning here...
	}
	if((ling.l_onoff > 0) && (ling.l_linger > 0))
		return ling.l_linger;
	return 0;
}

void IVistaSocket::SetLingerOnClose(int iLingerTimeInSecs)
{
	struct linger ling;
	ling.l_linger = iLingerTimeInSecs;
	ling.l_onoff =  iLingerTimeInSecs;
	int iSize = sizeof(struct linger*);
	if(!SetSockOpt(SOL_SOCKET, SO_LINGER, &ling, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetReceiveBufferSize() const
{
	int iBufSize=-1;
	int iSize = sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_RCVBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
	return iBufSize;
}

void IVistaSocket::SetReceiveBufferSize(int iBufSize)
{
	int iSize = sizeof(int);
	if(!SetSockOpt(SOL_SOCKET, SO_RCVBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetSendBufferSize() const
{
	int iBufSize=-1;
	int iSize = sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_SNDBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
	return iBufSize;
}

void IVistaSocket::SetSendBufferSize(int iBufSize)
{
	int iSize = sizeof(int);
	if(!SetSockOpt(SOL_SOCKET, SO_SNDBUF, (void*)&iBufSize, iSize))
	{
	  // we should utter a warning here...
	}
}

int IVistaSocket::GetSocketType() const
{
	int iValue=0;
	int iSize=sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_TYPE, (void*)&iValue,iSize))
	{
	  // we should utter a warning here...
	}
	return iValue;
}

bool IVistaSocket::GetSocketReuse() const
{
	int iValue=0;
	int iSize = sizeof(int);
	if(!GetSockOpt(SOL_SOCKET, SO_REUSEADDR, (void*)&iValue,iSize))
	{
	  // we should utter a warning here...
	}
	return (iValue!=0);
}

void IVistaSocket::SetSocketReuse(bool bReuse)
{
	int iDo = bReuse ? 1:0;
	int iSize = sizeof(int);
	if(!SetSockOpt(SOL_SOCKET, SO_REUSEADDR, (void*)&iDo, iSize))
	{
	  // we should utter a warning here...
	}
}

void IVistaSocket::SetErrorState(bool bState)
{
	m_bErrorState = bState;
}

bool IVistaSocket::GetErrorState() const
{
	return m_bErrorState;
}

#if defined(WIN32)


void IVistaSocket::EnableEventSelect(VistaThreadEvent *pEvent, long lEvents)
{
	if(GetIsOpen())
	{
		WSAEventSelect( SOCKET(GetSocketID()),
						(*pEvent).GetEventSignalHandle(),
						lEvents);
	}
}

void IVistaSocket::DisableEventSelect(VistaThreadEvent *pEvent)
{
	if(GetIsOpen())
	{
		WSAEventSelect( SOCKET(GetSocketID()),
						(*pEvent).GetEventSignalHandle(),
						0);
	}
}

VistaThreadEvent *IVistaSocket::GetWaitEvent() const
{
	return m_pWaitEvent;
}


#endif


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
