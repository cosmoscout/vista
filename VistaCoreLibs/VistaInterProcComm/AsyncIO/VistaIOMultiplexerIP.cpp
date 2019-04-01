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
#include <winsock2.h>
#include <Windows.h>
#endif
#include "VistaIOMultiplexerIP.h"

#include <VistaInterProcComm/Concurrency/VistaThreadCondition.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>

#include <VistaBase/VistaStreamUtils.h>

#include <cassert>
#include <algorithm>
#include <cstdio>
using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaIOMultiplexerIP::VistaIOMultiplexerIP()
:VistaIOMultiplexer()
{
	m_pActionMutex = new VistaMutex;
	m_pDoneMutex = new VistaMutex;
	m_pHandleMapMutex = new VistaMutex;
	m_pMetaEvent = new VistaThreadEvent( VistaThreadEvent::NON_WAITABLE_EVENT );
	m_pWaitingForAction = new VistaThreadCondition;
	m_pActionDone = new VistaThreadCondition;
	// control handle
	AddPoint((*m_pMetaEvent).GetEventSignalHandle(), 0, MP_IOIN);
}


VistaIOMultiplexerIP::~VistaIOMultiplexerIP()
{
	for(map<int, VistaThreadEvent*>::const_iterator cit = m_mpHandles.begin();
		cit != m_mpHandles.end(); ++cit)
	{
		// simply delete 'em all
		delete (*cit).second;
	}

	delete m_pActionDone;
	delete m_pWaitingForAction;
	delete m_pMetaEvent;
	delete m_pActionMutex;
	delete m_pDoneMutex;
	delete m_pHandleMapMutex;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaIOMultiplexerIP::Demultiplex(unsigned int nTimeout)
{
	vstr::debug() << "VistaIOMultiplexerIP::Demultiplex()" << std::endl;
#ifdef WIN32
	bool bDone = false;
	while(!bDone)
	{
		// first of all, prepare the handle-set
		//printf ("size= %d . Entering multiplexer again.\n", (int)m_veHandles.size());
		SetState(MP_PLEXING);
		(*m_pDoneMutex).Lock();
		DWORD nRet = WaitForMultipleObjects(DWORD(m_veHandles.size()), &m_veHandles[0], FALSE, INFINITE);
		SetState(MP_DISPATCHING);
		(*m_pDoneMutex).Unlock();
		printf ("VistaIOMultiplexerIP::Demultiplex() -- nRet = %d \n", nRet);
		if(nRet == WAIT_FAILED)
		{
			// failed
			printf ("[MultiplexIP] WAIT FAILED , leave loop");
			bDone = true; // leave loop
		}
		else
		{
			switch(m_eCom)
			{
			case MPC_SHUTDOWN:
				{
					// leave loop
					printf ("[MultiplexIP] state SHUTDOWN , leave loop");
					bDone = true;
					break;
				}
			case MPC_ADDHANDLE:
			case MPC_REMHANDLE:
				{
					// ok, we got signaled by the control event
					// indicate that we are waiting
				 //   VistaTimer::Sleep(200);
				//    (*m_pWaitingForAction).SignalCondition();
					(*m_pActionMutex).Lock();
					printf ("[MultiplexIP] Got COMMAND %d ", m_eCom);
					// wait for action done
				 //   (*m_pDoneMutex).Lock();
				 //   (*m_pActionDone).WaitForCondition(*m_pDoneMutex);
				 //   (*m_pDoneMutex).Unlock();
					(*m_pActionMutex).Unlock();
					break;
				}
			case MPC_NONE:
				{
					// we have to return the index here
					SetState(MP_IDLE);
					unsigned int iIndex = nRet - WAIT_OBJECT_0;
					assert(iIndex <= m_veHandles.size());
					return GetTicketForHandle(m_veHandles[iIndex]);
				}
			default:
				printf ("[MultiplexIP] Unknown state, leave loop");
				bDone = true; // leave loop
				break;
			}
			// reset command
			m_eCom = MPC_NONE;
		}
	}

#else
#endif
	SetState(MP_NONE);
	printf ("VistaIOMultiplexerIP::Demultiplex() -- LEAVING\n");
	(*m_pShutdown).SignalCondition();
	return -1;
}

void VistaIOMultiplexerIP::Shutdown()
{
	VistaIOMultiplexer::Shutdown();
	(*m_pMetaEvent).SignalEvent();

}


bool VistaIOMultiplexerIP::AddMultiplexPoint(HANDLE han, int iTicket, eIODir eDir)
{
   VistaMutexLock l (*m_pHandleMapMutex);
	assert(iTicket > 1);
	bool bRet = false;
	if(GetState() == MP_PLEXING)
	{
		// ok, we are serving right now, signal multiplexer
	   SetCommand(MPC_ADDHANDLE);
		(*m_pActionMutex).Lock();
	   (*m_pMetaEvent).SignalEvent();
	  //  (*m_pWaitingForAction).WaitForCondition(*m_pActionMutex);
		(*m_pDoneMutex).Lock();
	 //   (*m_pWaitingForAction).WaitForCondition(*m_pDoneMutex);
//        printf ("VistaIOMultiplexerIP::AddMultiplexPoint() -- SIGNALLED\n");
		// ok, IO-multiplexer is waiting for a signal on action-done
		// lets insert the handle
		vector<HANDLE>::iterator it = find(m_veHandles.begin(), m_veHandles.end(), han);
		if(it != m_veHandles.end())
		{
			// we already have a ticket for this!
		}
		else
		{
			bRet=AddPoint(han, iTicket, eDir);

		}

	   (*m_pDoneMutex).Unlock();
	   (*m_pActionMutex).Unlock();
   
	//   (*m_pActionDone).SignalCondition(); // proceed
	}
	else if(GetState() == MP_IDLE || GetState() == MP_NONE)
	{
		bRet = AddPoint(han, iTicket, eDir);
	}
	return bRet;
}


bool VistaIOMultiplexerIP::RemMultiplexPoint(HANDLE han)
{
   VistaMutexLock l (*m_pHandleMapMutex);
	bool bRet = false;
	if(GetState() == MP_PLEXING)
	{
		SetCommand(MPC_REMHANDLE);
		(*m_pActionMutex).Lock();
	   (*m_pMetaEvent).SignalEvent();
	 //  (*m_pWaitingForAction).WaitForCondition(*m_pActionMutex);
	   (*m_pDoneMutex).Lock();
	//   (*m_pWaitingForAction).WaitForCondition(*m_pDoneMutex);

		printf ("VistaIOMultiplexerIP::RemMultiplexPoint() -- SIGNALLED\n");

		// ok, we can now nodify the vector safely
		bRet = RemPoint(han, -1);
		(*m_pDoneMutex).Unlock();
	   (*m_pActionMutex).Unlock();

	 //   (*m_pActionDone).SignalCondition(); // proceed
	}
	else
   {
	   printf ("VistaIOMultiplexerIP::RemMultiplexPoint()\n");
		bRet = RemPoint(han, -1);
   }

   return bRet;
}

bool VistaIOMultiplexerIP::RemMultiplexPointByTicket(int iTicket)
{
	VistaMutexLock l (*m_pHandleMapMutex);
	TICKETMAP::iterator it = m_mpTicketMap.find(iTicket);
	if(it != m_mpTicketMap.end())
	{
		// found
		return RemPoint((*it).second, iTicket);
	}
	return false;
}


bool VistaIOMultiplexerIP::AddPoint(HANDLE han, int iTicket, eIODir eDir)
{
	vector<HANDLE>::iterator it = find(m_veHandles.begin(), m_veHandles.end(), han);
	if(it == m_veHandles.end())
	{
	   m_veHandles.push_back(han);
	   m_mpTicketMap[iTicket] = han;
	   m_mpHandleMap[han] = pair<int, eIODir>(iTicket, eDir);
	   return true;
	}
	else
		return false;
}

int  VistaIOMultiplexerIP::GetTicketForHandle(HANDLE han) const
{
	HANDLEMAP::const_iterator cit = m_mpHandleMap.find(han);
	if(cit == m_mpHandleMap.end())
		return -1;
	
	return (*cit).second.first; 
}

HANDLE VistaIOMultiplexerIP::GetHandleForTicket(int iTicket) const
{
	TICKETMAP::const_iterator cit = m_mpTicketMap.find(iTicket);
	if(cit == m_mpTicketMap.end())
		return 0; // invalid handle!

	return (*cit).second;
}

VistaIOMultiplexerIP::eIODir VistaIOMultiplexerIP::GetDirForHandle(HANDLE han) const
{
	HANDLEMAP::const_iterator cit = m_mpHandleMap.find(han);
	if(cit == m_mpHandleMap.end())
		return MP_IONONE;
	
	return (*cit).second.second;         
}

bool VistaIOMultiplexerIP::RemPoint(HANDLE han, int iTicket)
{
	vector<HANDLE>::iterator it = find(m_veHandles.begin(), m_veHandles.end(), han);
	if(it != m_veHandles.end())
	{
		// ok, we found it
		m_veHandles.erase(it);
		TICKETMAP::iterator itmp = m_mpTicketMap.find(iTicket);
		if(itmp != m_mpTicketMap.end())
			m_mpTicketMap.erase(itmp);

		HANDLEMAP::iterator hnmp = m_mpHandleMap.find(han);
		if(hnmp != m_mpHandleMap.end())
			m_mpHandleMap.erase(hnmp);


		return true;
	}
	else
	{
		vstr::errp() << "VistaIOMultiplexerIP::RemPoint() - Handle not found" 
				<< std::endl;
	}

	return false;
}


HANDLE VistaIOMultiplexerIP::CreateHandleForDescriptor(int iDesc,eIODir eDir)
{
#ifdef WIN32
	// first probe for existing handle
	map<int, VistaThreadEvent*>::iterator it = m_mpHandles.find(iDesc);
	if(it == m_mpHandles.end())
	{
		// ok, we do not have a handle for this, so create one
		VistaThreadEvent *pEvent = new VistaThreadEvent( VistaThreadEvent::WAITABLE_EVENT );
		m_mpHandles[iDesc] = pEvent;

		// associate direction
		if(eDir != VistaIOMultiplexer::MP_IONONE)
		{

			long nEvts = 0;
			switch(eDir)
			{
			case MP_IOIN:
				nEvts = FD_READ|FD_ACCEPT|FD_CLOSE;
				break;
			case MP_IOOUT:
				nEvts = FD_WRITE|FD_CLOSE;
				break;
			default:
				break;
			}

			WSAEventSelect(iDesc, 
					(*pEvent).GetEventSignalHandle(), 
					nEvts);

		}
		return pEvent->GetEventWaitHandle();
	}
	else
		return (*it).second->GetEventWaitHandle();
#else
	// no prob on unix, return iDesc
	return iDesc;
#endif
}

bool   VistaIOMultiplexerIP::DeleteHandleForDescriptor(int iDesc, HANDLE han, eIODir eDir)
{
#ifdef WIN32
   map<int, VistaThreadEvent*>::iterator it = m_mpHandles.find(iDesc);
	if(it == m_mpHandles.end())
	{
		return false;
	}
	else
	{
		VistaThreadEvent *pEvent = (*it).second;
		WSAEventSelect(iDesc, 
				(*pEvent).GetEventSignalHandle(), 
				0);

		m_mpHandles.erase(it);
		delete pEvent;
		return true;
	}
#else
	// no prob on unix, we do not do any housekeeping
	// as iDesc == han
	return true;
#endif
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

