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


#include "VistaAcceptor.h"
#include "VistaTCPServerSocket.h"
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>


#include <VistaBase/VistaStreamUtils.h>

#if defined(WIN32)
#pragma warning(disable: 4996)
#endif


#if defined(WIN32)
#include <winsock2.h>
#include <Windows.h>
#elif !defined(HPUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <cerrno>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#endif



#include <vector>
#include <deque>
#include <iostream>
using namespace std;

#include <cstdio>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaAcceptor::IVistaAcceptor(const string &sInterfaceName, int iPort)
: m_sInterfaceName(sInterfaceName), m_iInterfacePort(iPort)
{
	m_pSyncEvent = new VistaThreadEvent( VistaThreadEvent::WAITABLE_EVENT );
	m_bConnected = false;
	m_pServer = NULL;

}

IVistaAcceptor::~IVistaAcceptor()
{
	delete m_pSyncEvent;
	if(m_pServer)
	{
		m_pServer->CloseSocket();
	}

	delete m_pServer;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void IVistaAcceptor::SetAbortSignal()
{
	(*m_pSyncEvent).SignalEvent();
}


bool IVistaAcceptor::GetIsConnected() const
{
	return m_bConnected;
}

void IVistaAcceptor::PreWork()
{
	m_bConnected=false;
}

void IVistaAcceptor::DefinedThreadWork()
{
	if(!m_pServer)
	{
		m_pServer = new VistaTCPServerSocket;
		VistaSocketAddress addr(m_sInterfaceName, m_iInterfacePort);


		if((*m_pServer).OpenSocket())
		{
			(*m_pServer).SetSocketReuse(true);
			if((*m_pServer).BindToAddress(addr))
			{
				(*m_pServer).SetIsBuffering(true);
			}
			else
			{
				printf("VistaAcceptWork::DefinedThreadWork() -- ERROR, could not open/bind socket!\n");
				delete m_pServer;
				m_pServer = NULL;
				return; // we are done!
			}
		}
	}

	char buffer[256];
	sprintf(buffer, "%d", m_iInterfacePort);

	vstr::outi() << "[ViMsgP]: Created Message Port ["	<< m_sInterfaceName
				<< ", " << string(buffer)<< "]" << std::endl;

	eSockState eSock = SOCK_NONE;

	(*m_pServer).SetIsBlocking(false);
	
	
#if defined(WIN32)
	vector<HANDLE> veHandles;
	VistaThreadEvent sockEvent(VistaThreadEvent::WAITABLE_EVENT);
	// we must convert socket-ids to events


	WSAEventSelect(SOCKET((*m_pServer).GetSocketID()),
				   sockEvent.GetEventWaitHandle(),
				   FD_READ|FD_CLOSE|FD_ACCEPT);

	veHandles.push_back((*m_pSyncEvent).GetEventWaitHandle());
	veHandles.push_back(sockEvent.GetEventWaitHandle());

	(*m_pServer).Listen(1);
	(*m_pServer).Accept(); // this should not block

	printf("VistaAcceptWork::DefinedThreadWork() -- WAITING FOR CONNECT (OR ABORT)\n");
	DWORD iRet = WaitForMultipleObjects(2, &veHandles[0], FALSE, INFINITE);
	if((iRet != WAIT_ABANDONED) && (iRet != WAIT_FAILED))
	{
		if(iRet == 0)
		{
			// exit msg
			printf("-- EXIT MSG\n");
			eSock = SOCK_EXIT;
		}
		else
		{
			printf("-- SIGNAL ON SOCKET\n");
			eSock = SOCK_CLIENT;
			// we should make the socket blocking again!
			WSAEventSelect( SOCKET((*m_pServer).GetSocketID()),
				sockEvent.GetEventWaitHandle(),
				0);
			(*m_pServer).SetIsBlocking(true);
		}
	}
#else // UNIX!
	fd_set myRdSocks;
	fd_set myExSocks;


	FD_ZERO(&myRdSocks);
	FD_ZERO(&myExSocks);

	int iSckId = (*m_pServer).GetSocketID();
	int iSyncId = (int)(*m_pSyncEvent).GetEventWaitHandle();

	printf("VistaAcceptWork::DefinedThreadWork: iSckId = %d, iSyncId = %d\n", iSckId, iSyncId);

	FD_SET(iSckId, &myRdSocks);
	FD_SET(iSckId, &myExSocks);
	FD_SET(iSyncId, &myExSocks);
	FD_SET(iSyncId, &myRdSocks);

	printf("VistaAcceptWork::DefinedThreadWork -- accepting\n");
	(*m_pServer).Listen(1);
	(*m_pServer).Accept(); // this should not block

	printf("VistaAcceptWork::DefinedThreadWork() -- WAITING FOR CONNECT (OR ABORT)\n");
	int iRet = TEMP_FAILURE_RETRY( select( (iSckId>iSyncId ? iSckId+1 : iSyncId+1), &myRdSocks, NULL, &myExSocks, NULL ) );	

	if((iRet < 0) || FD_ISSET((*m_pServer).GetSocketID(), &myExSocks))
	{
		// whoah!
		eSock = SOCK_EXIT;
	}
	else
	{
		if(FD_ISSET(iSyncId, &myRdSocks) || FD_ISSET(iSyncId, &myExSocks) 
		  || FD_ISSET(iSckId, &myExSocks))
		{
			// either told to stop, error on stop or error on read socket
			// in either case, we will exit.
			eSock = SOCK_EXIT;
		}
		else
		{
			// ok, no error on sckid and no exit trigger, we will accept now
			eSock = SOCK_CLIENT;
		}
	}

#endif // OS-specific end

	switch(eSock)
	{
	case SOCK_CLIENT:
		{

			/*
			 * We set the "parent" server socket to blocking, as this is
			 * usually what the vista programmer expects: a socket is blocking
			 * until told not to do so...
			 */
//           	(*m_pServer).SetIsBlocking(true);
			
			// socket accept!
			VistaTCPSocket *pSocket = (*m_pServer).Accept();
			if(pSocket)
			{
				// this should be true!
				pSocket->VerifyBlocking();
				if(!HandleIncomingClient(pSocket))
				{
					// we think this is a failure
					pSocket->CloseSocket();
					delete pSocket;
				}
				else
				{
					// ok, this seems to be a success
					m_bConnected = true;
				}
			}
			break;
		}
	case SOCK_EXIT:
		{
			vstr::outi() << "[VMsgP]: Received Exit signal." << std::endl;
			HandleAbortMessage();
			break;
		}
	default:
		{
			vstr::outi() << "[VMsgP]: Received some unknown signal (Error?: " 
				<< eSock << ")" << std::endl;
		}
		break;
	}

  printf("VistaAcceptWork::DefinedThreadWork() -- LEAVING\n");
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
