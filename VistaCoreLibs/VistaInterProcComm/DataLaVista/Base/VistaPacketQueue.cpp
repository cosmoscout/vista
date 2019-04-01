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



/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

#include "VistaPacketQueue.h"
#include "VistaDataPacket.h"
#include "VistaPipeComponent.h"
#include "VistaRegistration.h"
#include <algorithm>

#include <cstdio>


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaPacketQueue::DLVistaPacketQueue(IDLVistaPipeComponent *pComp, int iQueueSize)
{
	m_pquPackets = new PQUEUE;
	m_pOwner = pComp;

	Resize(iQueueSize);
}

DLVistaPacketQueue::~DLVistaPacketQueue()
{
	ClearQueue();
	delete m_pquPackets;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

IDLVistaDataPacket *DLVistaPacketQueue::GiveFreshPacket()
{
	if(!(*m_pquPackets).empty())
	{
		IDLVistaDataPacket *p = (*m_pquPackets).front();
		if(p->IsLocked())
		{
			// this is effectively a drop
	        printf("front packet locked. Drop?\n");			
            return NULL;    
		}

		(*m_pquPackets).pop_front(); // remove
		p->Lock();
		(*m_pquPackets).push_back(p); // now add to tail

		return p;
	}
	else
	{
		printf("No packets, no fun (queue empty).\n");
		return NULL;
	}
}

bool DLVistaPacketQueue::RecyclePacket(IDLVistaDataPacket *pPacket)
{
	PQUEUE::iterator it = find((*m_pquPackets).begin(), (*m_pquPackets).end(), pPacket);
	if(it!=(*m_pquPackets).end())
	{
		
		pPacket->Unlock(); // ready to rumble
		pPacket->Stamp(-1,-1); // kill old timestamp value
		(*m_pquPackets).erase(it); // wherever you were, sweet 'lil packet
		(*m_pquPackets).push_front(pPacket); // you are a soldier now (right at the front)
		return true;
	}
	else
		printf("NOT MY (%lx) PACKET [%lx]\n", long(this), long(pPacket)); // we SHOULD exit here!

	return false;
}

int DLVistaPacketQueue::GetPacketCount() const
{
	return (int)(*m_pquPackets).size();
}

int DLVistaPacketQueue::GetAvailablePacketCount() const
{
	int iRet = 0;
	PQUEUE::const_iterator it;
	for(it = (*m_pquPackets).begin(); it != (*m_pquPackets).end(); ++it)
	{
		if(!(*it)->IsLocked())
			++iRet;
		else
			break; // the first on locked is the end of the available packets
	}

	return iRet;
}

int DLVistaPacketQueue::Resize(int iNewSize)
{
   ClearQueue();
   int iTypeId = -1;
   IDLVistaRegistration *pRegistration = IDLVistaRegistration::GetRegistrationSingleton();
   
   for(int i=0; i < iNewSize; ++i)
   {
		IDLVistaDataPacket *pPacket = (*m_pOwner).CreatePacket();
		if(pRegistration && (iTypeId == -1))
		{
			iTypeId = (*pRegistration).GetTypeIdForName((*pPacket).GetSignature());
			pPacket->SetPacketType(iTypeId);
		}
		
		(*m_pquPackets).push_back(pPacket);
		
	}

	return (int)(*m_pquPackets).size();
}

void DLVistaPacketQueue::ClearQueue()
{
	PQUEUE::iterator it;
	for(it = (*m_pquPackets).begin(); it != (*m_pquPackets).end(); ++it)
	{
		if((*it)->IsLocked())
		{
			// OUCH!
		}

		delete *it;
	}

	(*m_pquPackets).clear();
}

void DLVistaPacketQueue::PrintQueue() const
{
	PQUEUE::const_iterator it;
	for(it = (*m_pquPackets).begin(); it != (*m_pquPackets).end(); ++it)
		printf("[%lx], %s\n", long(*it), ((*it)->IsLocked() ? "locked" : "unlocked"));
}


int DLVistaPacketQueue::GetPacketType() const
{
	return (*m_pquPackets).front()->GetPacketType();
}

IDLVistaDataPacket *DLVistaPacketQueue::GetPacketByIndex(int iIndex) const
{
	return (*m_pquPackets)[iIndex];
}

