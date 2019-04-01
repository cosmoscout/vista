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


#include "VistaSlotPipe.h"
#include <VistaInterProcComm/DataLaVista/Base/VistaPacketQueue.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <cstdio>
#include <deque>
using namespace std;
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/


DLVistaSlotPipe::DLVistaSlotPipe()
{
	this->m_pInValue = NULL;
	this->m_pquRecycleQueue = new deque<IDLVistaDataPacket *>;

	m_pMutexIn = new VistaMutex;
	m_pMutexOut = new VistaMutex;
	m_pRecycleEvent = new VistaThreadEvent(VistaThreadEvent::NON_WAITABLE_EVENT);
}

DLVistaSlotPipe::~DLVistaSlotPipe()
{
	(*m_pMutexIn).Lock(); // no new packets
	(*m_pMutexOut).Lock(); // no more recycling now

	// flush in Value
	if(m_pInValue)
		(*m_pquRecycleQueue).push_back(m_pInValue);

	deque<IDLVistaDataPacket *>::const_iterator out_q;
	for(out_q = (*m_pquRecycleQueue).begin(); out_q != (*m_pquRecycleQueue).end(); ++out_q)
	{
		// send back to its maker
		this->m_pInput->RecycleDataPacket(*out_q, this);
	}

	delete m_pquRecycleQueue;

	delete m_pMutexIn;
	delete m_pMutexOut;
	delete m_pRecycleEvent;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

IDLVistaDataPacket * DLVistaSlotPipe::GivePacket(bool bBlock)
{

	IDLVistaDataPacket *pPacket = NULL;

	if(GrabMutex(m_pMutexIn, bBlock))
	{
		pPacket = m_pInValue;
		m_pInValue = NULL; // clear!
		(*m_pMutexIn).Unlock();
	}
	return pPacket;
}

IDLVistaDataPacket *DLVistaSlotPipe::ReturnPacket()
{
	IDLVistaDataPacket *pPacket = NULL;
	if(GrabMutex(m_pMutexOut, false))
	{
		if((*m_pquRecycleQueue).empty())
		{
			// wait for some packets
			(*m_pMutexOut).Unlock();
			//printf("Recylce-request pending on empty queue... thread blocking.\n");
			if((*m_pRecycleEvent).WaitForEvent(5)==1) // block, but only wayit for a short burst!
			{
//				printf("Got signal... there must be some packets in the recycle-queue\n");
			}
			else
			{
				//printf("TIMEOUT, continue.\n");
				return NULL;
			}

			if(GrabMutex(m_pMutexOut, true))
			{
				//printf("Got mutex to queue.\n");
			}
			// ok, now we are sure that there is at least one packet in this queue!
			// AND we have exclusive access to recycle-queue
		}

		if(!(*m_pquRecycleQueue).empty())
		{
			pPacket = (*m_pquRecycleQueue).front();
			(*m_pquRecycleQueue).pop_front();
//			printf("DLVistaSlotPipe::ReturnPacket() -- Returning packet %X to active component. -- recycle queue-size = %d\n", pPacket, (*m_pquRecycleQueue).size());
		}
//		else
//			printf("DLVistaSlotPipe::ReturnPacket() -- RETURN QUEUE EMPTY.\n");

		(*m_pMutexOut).Unlock();
	}
	else
		printf("no mutex, no fun.\n");

//	printf("Returning from Recycling queue: %X\n", pPacket);

	return pPacket;
}

VistaMutex *DLVistaSlotPipe::GrabMutex(VistaMutex *pMutex, bool bBlock)
{
	if(bBlock)
	{
		(*pMutex).Lock();
	}
	else
	{
		if((*pMutex).TryLock()==false)
			return NULL;
	}

	return pMutex;
}


bool DLVistaSlotPipe::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	if(GrabMutex(m_pMutexOut, bBlock))
	{
		(*m_pquRecycleQueue).push_back(pPacket);
//		printf("DLVistaSlotPipe::RecycleDataPacket(%X) -- packets in queue: %d\n", pPacket, (*m_pquRecycleQueue).size());
		(*m_pMutexOut).Unlock();
		(*m_pRecycleEvent).SignalEvent(); // signal in case someone is waiting.
		return true;
	}
//	else
//		printf("DLVistaSlotPipe::RecycleDataPacket(%X) -- failed to grad mutex!\n", pPacket);

	return false;
}

bool DLVistaSlotPipe::AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	bool bRet = false;
	if(GrabMutex(m_pMutexIn, bBlock))
	{
		if(m_pInValue)
		{
			// recycle me, we can drop this one
			RecycleDataPacket(m_pInValue, this, true);
		}

		m_pInValue = pPacket;
//		printf("DLVistaSlotPipe::AcceptDataPacket(%x) -- accepted, packets in queue: %d\n", pPacket, (*m_pquInQueue).size());
		(*m_pMutexIn).Unlock();
		bRet = true;
	}
	return bRet;
}


bool DLVistaSlotPipe::InitPacketMgmt()
{
	return true;
}


bool DLVistaSlotPipe::IsFull() const
{
	return (*m_pquRecycleQueue).empty();
}

bool DLVistaSlotPipe::IsEmpty() const
{
	if(m_pInValue==NULL)
		return true;
	return false;
	//return (*m_pquInQueue).GetPacketCount();
}


