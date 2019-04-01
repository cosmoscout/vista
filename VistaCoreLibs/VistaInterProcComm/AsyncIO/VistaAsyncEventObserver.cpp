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


#ifdef WIN32
#include <winsock2.h> // should be included *before* windows.h
#include <Windows.h>
#endif

#include "VistaAsyncEventObserver.h" 

#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/Concurrency/VistaThreadCondition.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/IPNet/VistaTCPServerSocket.h>
#include <VistaInterProcComm/AsyncIO/VistaIOHandleBasedMultiplexer.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <vector>
#include <map>
#include <cassert>
#include <set>
#include <algorithm>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaAEObserverTask::VistaAEObserverTask(const std::string &sHost, int iPort)
: IVistaThreadedTask(), m_sHostName(sHost), m_iHostPort(iPort),
m_pTranslator(NULL)
{
	m_pMp                = new VistaIOHandleBasedIOMultiplexer;
	m_pNotificationEvent = new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT);
	m_pModelEvent        = new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT);

#ifdef WIN32
	m_pServerEvent = new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT);
	m_pClientEvent = new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT);
#else
	m_pServerEvent = NULL;
	m_pClientEvent = NULL;
#endif
	(*m_pMp).AddMultiplexPoint(m_pNotificationEvent->GetEventWaitHandle(), 
		E_NOTIFY, 
		VistaIOMultiplexer::MP_IOIN);

	(*m_pMp).AddMultiplexPoint(m_pModelEvent->GetEventWaitHandle(), 
		E_MODEL, 
		VistaIOMultiplexer::MP_IOIN);

	m_pEventChannel = NULL;


	m_pServer = new VistaTCPServerSocket;
	(*m_pServer).SetSocketReuse(true);

	m_pClient    = NULL;
	m_pClientCon = NULL;
	m_bConnected = false;
}

VistaAEObserverTask::~VistaAEObserverTask()
{
	delete m_pModelEvent;
	delete m_pNotificationEvent;
	delete m_pServerEvent;
	delete m_pClientEvent;

	delete m_pMp;

	if(m_pClientCon)
		m_pClientCon->Close();

	delete m_pClientCon;


	if(m_pServer)
		m_pServer->CloseSocket();

	delete m_pServer;
}

void VistaAEObserverTask::PreWork()
{
	SetupServerSocket();
}


void VistaAEObserverTask::PostWork()
{
	ShutdownServerSocket();
}


bool VistaAEObserverTask::SetupServerSocket()
{

	if(m_sHostName == "" || m_iHostPort == 0)
		return false;

	if((*m_pServer).OpenSocket())
	{
		vstr::outi() << "CVfvObserverTask::SetupServerSocket("
			<< m_sHostName << ", " <<  m_iHostPort << ")" << std::endl;

		VistaSocketAddress addr(m_sHostName, m_iHostPort);
		if((*m_pServer).BindToAddress(addr))
		{
			// children inherit blocking
			// so beware!
			(*m_pServer).SetIsBlocking(false); 
			(*m_pServer).Listen(1); // we accept only one client at a time
			VistaTCPSocket *pSock = (*m_pServer).Accept();
			if(pSock)
			{
				// ok, this should not happen: a client was already waiting at
				// the entry port
				// HOUSTON...
				SetupClient(pSock);
			}
			else
			{
#ifdef WIN32
				EnableEventSelect((*m_pServer).GetSocketID(), m_pServerEvent, FD_READ|FD_CLOSE|FD_ACCEPT);
				(*m_pMp).AddMultiplexPoint((*m_pServerEvent).GetEventSignalHandle(), 
					E_SERVER, 
					VistaIOMultiplexer::MP_IOIN);
#else
				(*m_pMp).AddMultiplexPoint((*m_pServer).GetSocketID(),E_SERVER, VistaIOMultiplexer::MP_IOIN);
				//(*m_pMp).AddMultiplexPoint((*m_pServer).GetSocketID(),E_SERVER, VistaIOMultiplexer::MP_IOERR);
#endif
			}
		}

	}
	return true;
}

bool VistaAEObserverTask::ShutdownServerSocket()
{
	if(m_pServer->GetIsOpen())
	{
#ifdef WIN32
		DisableEventSelect(m_pServer->GetSocketID(), m_pServerEvent);
		(*m_pMp).RemMultiplexPoint(m_pServerEvent->GetEventSignalHandle(), VistaIOMultiplexer::MP_IOIN);
#else
		(*m_pMp).RemMultiplexPoint(m_pServer->GetSocketID(), VistaIOMultiplexer::MP_IOIN);
		//(*m_pMp).RemMultiplexPoint(m_pServer->GetSocketID(), VistaIOMultiplexer::MP_IOERR);
#endif
		return m_pServer->CloseSocket();
	}

	return true;
}


void VistaAEObserverTask::IndicateModelEvent(IVistaObserveable *pObj, int iMsg, int iToken)
{
	// we use the scope operator to release the lock
	{
		VistaMutexLock l(m_ChgSetLock);
		m_ChgSet.insert(_objHlp(pObj, iMsg, iToken));
	}

	if(m_bConnected)
		(*m_pModelEvent).SignalEvent();
}

void VistaAEObserverTask::IndicateNotificationEvent()
{
	(*m_pNotificationEvent).SignalEvent();
}

void VistaAEObserverTask::StopProcessing()
{
	(*m_pMp).Shutdown();
}


void VistaAEObserverTask::EnableEventSelect(HANDLE iSocketDesc, VistaThreadEvent *pEvent, long iMask)
{
#ifdef WIN32
	WSAEventSelect( SOCKET(iSocketDesc), 
		(*pEvent).GetEventSignalHandle(), 
		iMask);
#endif
}

void VistaAEObserverTask::DisableEventSelect(HANDLE iSocketDesc, VistaThreadEvent *pEvent)
{
#ifdef WIN32
	WSAEventSelect( SOCKET(iSocketDesc), 
		(*pEvent).GetEventSignalHandle(), 
		0);
#endif
}

enum eServerEvType
{
	ETS_UNKNOWN = 0,
	ETS_CLOSE,
	ETS_READPOSSIBLE,
	ETS_ACCEPT
};

enum eClientEvType
{
	ETC_UNKNOWN=0,
	ETC_CLOSE,
	ETC_MESSAGE
};

VistaAEObserverTask::eClientEvType VistaAEObserverTask::DetermineClientEvent()
{
#ifdef WIN32
	WSANETWORKEVENTS NetworkEvents;
	if(WSAEnumNetworkEvents(SOCKET((*m_pClient).GetSocketID()), 
		(*m_pClientEvent).GetEventSignalHandle(),
		&NetworkEvents) == 0)
	{
		// ok, this worked
		if(NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			// close
			//vstr::outi() << "CLOSE EVENT!" << std::endl;
			return ETC_CLOSE;

		}
		else if(NetworkEvents.lNetworkEvents & FD_READ)
		{
			// we can read on the socket?
			//vstr::outi() << "READ?" << std::endl;
			return ETC_MESSAGE;
		}
		//else
		//	vstr::outi() << "Unknown" << std::endl;
	}
	return ETC_UNKNOWN;
#else
	return ETC_MESSAGE; // we will always assume a read event on unix
#endif

}

VistaAEObserverTask::eServerEvType VistaAEObserverTask::DetermineServerEvent()
{
#ifdef WIN32
	WSANETWORKEVENTS NetworkEvents;
	if(WSAEnumNetworkEvents(SOCKET((*m_pServer).GetSocketID()), 
		(*m_pServerEvent).GetEventSignalHandle(),
		&NetworkEvents) == 0)
	{
		// ok, this worked
		if(NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			// close
			//vstr::errp() << "CLOSE EVENT!" << std::endl;
			return ETS_CLOSE;

		}
		else if(NetworkEvents.lNetworkEvents & FD_READ)
		{
			// we can read on the socket?
			//vstr::errp() << "READ?" << std::endl;
			return ETS_READPOSSIBLE;
		}
		else if(NetworkEvents.lNetworkEvents & FD_ACCEPT)
		{
			// we have a client
			//vstr::errp() << "ACCEPT!" << std::endl;
			return ETS_ACCEPT;
		}
		//else
		//	vstr::errp() << "Unknown." << std::endl;
	}
	return ETS_UNKNOWN;
#else

	return ETS_ACCEPT; // we will always assume an accept on unix
#endif
}

bool VistaAEObserverTask::SetupClient(VistaTCPSocket *pSock)
{
	if(m_pClientCon && m_pClientCon->GetIsOpen())
		m_pClientCon->Close();

	delete m_pClientCon;
	m_bConnected = false;

	m_pClient = pSock;

	m_pClientCon = new VistaConnectionIP(m_pClient); // wrap

#ifdef WIN32
	EnableEventSelect((*pSock).GetSocketID(), m_pClientEvent, FD_READ|FD_WRITE|FD_CLOSE);
	(*m_pMp).AddMultiplexPoint((*m_pClientEvent).GetEventSignalHandle(), E_CLIENT, VistaIOMultiplexer::MP_IOIN);
#else
	(*m_pMp).AddMultiplexPoint((*pSock).GetSocketID(), E_CLIENT, VistaIOMultiplexer::MP_IOIN);
#endif
	m_bConnected = true;
	return true;
}

void VistaAEObserverTask::ReadClient()
{
	std::string MyString;
	int iRet = m_pClientCon->ReadDelimitedString(MyString, '\n');
	if(iRet==0) // should be blocking
	{
		vstr::errp() << "[VistaAEObserverTask]: Error on ReadClient()" << std::endl;
		DetachClient();
		SetupServerSocket();
	}
	else
		vstr::outi() << "str = " << MyString << std::endl;
}

bool VistaAEObserverTask::DetachClient()
{

	if(!m_pClientCon)
		return true;

#ifdef WIN32
	DisableEventSelect(m_pClientCon->GetSocketID(), m_pClientEvent);
	(*m_pMp).RemMultiplexPoint((*m_pClientEvent).GetEventSignalHandle(), VistaIOMultiplexer::MP_IOIN);
#else
	(*m_pMp).RemMultiplexPoint(m_pClientCon->GetConnectionDescriptor(), VistaIOMultiplexer::MP_IOIN);
#endif

	if(m_pClientCon && m_pClientCon->GetIsOpen())
		m_pClientCon->Close();

	delete m_pClientCon; // should kill socket as well
	m_pClientCon = NULL;
	m_bConnected = false;
	m_pClient = NULL; // not valid anymore

	return true;
}

void VistaAEObserverTask::HandleModelChange()
{
	if(m_pTranslator && !m_ChgSet.empty())
	{
		VistaByteBufferSerializer ser(8192);

		// copy entries
		std::list<_objHlp> li;

		// we use the scope operator to release the lock
		{
			VistaMutexLock l(m_ChgSetLock);
			li.assign(m_ChgSet.begin(), m_ChgSet.end());
			m_ChgSet.clear();
		}

		for(std::list<_objHlp>::iterator it = li.begin();
			it != li.end(); ++it)
		{
			bool bPrependSize = true;
			if((*m_pTranslator)((*it).m_pObj, (*it).m_iMsg, (*it).m_iTicket, bPrependSize, ser))
			{
				try
				{
					m_pClientCon->WaitForSendFinish();
					if(bPrependSize)
						m_pClientCon->WriteInt32(ser.GetBufferSize());
					m_pClientCon->WriteRawBuffer((const void*)ser.GetBuffer(), ser.GetBufferSize());			
				}
				catch(VistaExceptionBase &x)
				{
					x.PrintException();
					DetachClient();
					SetupServerSocket(); // reschedule
				}				
			}
			ser.ClearBuffer(); // clear as we reuse the serializer
		}

	}
	else
	{
		// we should utter something?
	}
}

bool VistaAEObserverTask::RegisterUpdateTranslator(VistaAsyncEventObserver::IUpdateTranslator *pTranslator)
{
	m_pTranslator = pTranslator;
	return true;
}

VistaAsyncEventObserver::IUpdateTranslator *VistaAEObserverTask::GetUpdateTranslator() const
{
	return m_pTranslator;
}

void VistaAEObserverTask::DefinedThreadWork()
{
	int iTicket = -1;
	do
	{
		iTicket = (*m_pMp).Demultiplex();
		switch(iTicket)
		{
		case E_NOTIFY:
			{
				m_pNotificationEvent->ResetThisEvent();
				break;
			}
		case E_MODEL:
			{
				m_pModelEvent->ResetThisEvent();
				HandleModelChange();
				break;
			}
		case E_SERVER:
			{
				switch(DetermineServerEvent())
				{
				case ETS_ACCEPT:
					{
						VistaTCPSocket *pSock = (*m_pServer).Accept();
						if(!SetupClient(pSock))
							break;
					}
				case ETS_CLOSE:
					{
						// failure or close
						ShutdownServerSocket();
						break;
					}
				case ETS_READPOSSIBLE:
				case ETS_UNKNOWN:
				default:
					break;
				}
				break;
			}
		case E_CLIENT:
			{

				switch(DetermineClientEvent())
				{
				case ETC_MESSAGE: //read
					{
						ReadClient();
						break;
					}
				case ETC_CLOSE: // close
					{
						DetachClient();
						SetupServerSocket();
						break;
					}
				case ETC_UNKNOWN:
				default:
					break;
				}
				break;
			}
		case E_FAILURE:
			{
				vstr::outi() << "VistaAEObserverTask::DefinedThreadWork() -- LEAVE" << std::endl;
				break;
			}
#if !defined(WIN32)
			// we have to read off the "signal" on the pipe
			// on unix
		case E_METAEVENT:
			{
				// this was a meta-event!
				/** @todo remove this manual reset of the event */
				// on unix systems
				(*m_pMp).Remedy();
				break;
			}	
#endif
		default:
			break;
		}

	}
	while(iTicket != -1);

	DetachClient();
	ShutdownServerSocket();
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaAsyncEventObserver::VistaAsyncEventObserver(const std::string &sHostName, 
												 int iHostPort)
{
	m_pThread        = new VistaThreadTask;
	m_pThread->SetThreadName("VistaAsyncEventObserver() -- ObserverTask");
	m_pActivatorTask = new VistaAEObserverTask(sHostName, iHostPort);
	m_pThread->SetThreadedTask(m_pActivatorTask);
}

VistaAsyncEventObserver::~VistaAsyncEventObserver()
{
	StopObserver();

	for(std::set<IVistaObserveable*>::iterator it = m_setProConts.begin();
		it != m_setProConts.end(); ++it)
	{
		(*it)->DetachObserver(this);
	}

	delete m_pThread;
	delete m_pActivatorTask;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaAsyncEventObserver::ObserveableDeleteRequest(IVistaObserveable *pObserveable,
													   int nTicket)
{
	return true;
}


void VistaAsyncEventObserver::ObserveableDelete(IVistaObserveable *pObserveable, int nTicket) 
{
	ReleaseObserveable(pObserveable, nTicket);
}

void VistaAsyncEventObserver::ReleaseObserveable(IVistaObserveable *pObserveable, int nTicket) 
{
	pObserveable->DetachObserver(this);
	std::set<IVistaObserveable*>::iterator it = std::find(m_setProConts.begin(), m_setProConts.end(), pObserveable);
	if (it != m_setProConts.end())
	{
		m_setProConts.erase(it);
	}
}

void VistaAsyncEventObserver::ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket) 
{
	m_pActivatorTask->IndicateModelEvent(pObserveable, msg, ticket);
}

bool VistaAsyncEventObserver::Observes(IVistaObserveable *pObserveable) 
{
	return (m_setProConts.find(pObserveable) != m_setProConts.end());
}

void VistaAsyncEventObserver::Observe(IVistaObserveable *pObservable, int eTicket) 
{
	pObservable->AttachObserver(this, eTicket);
	m_setProConts.insert( pObservable );
}


bool VistaAsyncEventObserver::StartObserver()
{
	if(m_pThread->IsRunning())
		return true;
	return m_pThread->Run();
}

bool VistaAsyncEventObserver::StopObserver()
{   
	if(m_pThread && m_pThread->IsRunning())
	{
		m_pActivatorTask->StopProcessing();
		m_pThread->Join(); // wait for it to finish
	}
	return true;
}


VistaAsyncEventObserver::IUpdateTranslator::~IUpdateTranslator()
{
}

VistaAsyncEventObserver::IUpdateTranslator::IUpdateTranslator()
{
}

bool VistaAsyncEventObserver::RegisterUpdateTranslator(IUpdateTranslator *pTrans)
{
	return m_pActivatorTask->RegisterUpdateTranslator(pTrans);
}

VistaAsyncEventObserver::IUpdateTranslator *VistaAsyncEventObserver::GetUpdateTranslator() const
{
	return m_pActivatorTask->GetUpdateTranslator();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

