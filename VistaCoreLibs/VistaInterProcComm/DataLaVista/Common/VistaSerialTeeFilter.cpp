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


#include "VistaSerialTeeFilter.h"
#include <VistaInterProcComm/DataLaVista/Base/VistaPacketQueue.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>


#include <algorithm>

#include <cstdio>


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/


DLVistaSerialTeeFilter::DLVistaSerialTeeFilter()
{
	m_mpPacketMap = new PACKETMAP;
	m_pMapMutex = new VistaMutex;


}

DLVistaSerialTeeFilter::~DLVistaSerialTeeFilter()
{
	delete m_mpPacketMap;
	m_mpPacketMap = NULL;

	delete m_pMapMutex;

}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void DLVistaSerialTeeFilter::UpdatePacketIndex(IDLVistaDataPacket *pPacket, int iIndex)
{
	PACKETMAP::iterator it = (*m_mpPacketMap).find(pPacket);
	if(it != (*m_mpPacketMap).end())
	{
//#ifdef DEBUG
		//printf("DLVistaSerialTeeFilter::UpdatePacketIndex(%X,%d)\n", pPacket, iIndex);
//#endif
		(*m_mpPacketMap).erase(it);
	}

	if(iIndex!=-1)
	{
		(*m_mpPacketMap).insert(PACKETMAP::value_type(pPacket, iIndex));
//#ifdef DEBUG
		//printf("DLVistaSerialTeeFilter::UpdatePacketIndex(%X) -- setting packet index to %d\n", pPacket, iIndex);
//#endif
	}
}

IDLVistaDataPacket * DLVistaSerialTeeFilter::ReturnPacket()
{
	// simply do the following: as we always process the same type of packets for all outbounds
	// we linearily query all attached outbounds and return the first packet we get
	//printf("DLVistaSerialTeeFilter::UpdatePacketIndex() -- inside return packet.\n");

	for(int i=0; i < GetNumberOfOutbounds(); ++i)
	{
		IDLVistaPipeComponent *pComponent = GetOutboundByIndex(i);
		IDLVistaDataPacket *pPacket = pComponent->ReturnPacket();
		if(pPacket)
		{
			int nIdx = GetPacketIndex(pPacket);
			//if we are finished with this packet -> check out and return it directly 
			//is is assumed that the calling component does recycle this packet itself!
			if( nIdx == (GetNumberOfOutbounds()-1)) 
			{
				//printf("DLVistaSerialTeeFilter::UpdatePacketIndex(%X) -- got finished packet from (%d)\n", pPacket, nIdx);
				if(GrabMutex(m_pMapMutex, true))
				{
					UpdatePacketIndex(pPacket, -1);
					m_pMapMutex->Unlock();
				}
				return pPacket;
			}
			else //packet has yet to be routed to other components
			{
				//printf("DLVistaSerialTeeFilter::UpdatePacketIndex(%X) -- return packet on non-finished packet\n", pPacket);
				// while we are at it... simply resend the packet to the next outbound
				if(GrabMutex(m_pMapMutex, true))
				{
					UpdatePacketIndex(pPacket, ++nIdx);
					IDLVistaPipeComponent *pComp = GetOutboundByIndex(nIdx);
					(*m_pMapMutex).Unlock();
					pComp->AcceptDataPacket(pPacket, this, true);
					pPacket = NULL; // clear pointer (just for clearing the pointer ;)
				}
			}
		}
	}
	//printf("DLVistaSerialTeeFilter::UpdatePacketIndex() -- returning NULL (no packet available).\n");	
	return NULL;
}

int DLVistaSerialTeeFilter::GetPacketIndex(IDLVistaDataPacket *pPacket) const
{
	PACKETMAP::iterator cit = (*m_mpPacketMap).find(pPacket);
	if(cit != (*m_mpPacketMap).end())
		return (*cit).second;
	return -1;
}

IDLVistaDataPacket *DLVistaSerialTeeFilter::FilterPacketL(IDLVistaDataPacket *pPacket)
{
	int iIdx = GetPacketIndex(pPacket);
	if(iIdx == -1)
	{
		// ok, this packet was not sent, yet
		//IDLVistaPipeComponent *pComp = GetOutboundByIndex(0);
		if(GrabMutex(m_pMapMutex, true))
		{        
			UpdatePacketIndex(pPacket, 0);
			// else: erased
			(*m_pMapMutex).Unlock();
		}
		//printf("DLVistaSerialTeeFilter::FilterPacketL(%X), %d\n", pPacket, 0);
		return pPacket;
	}
	else
	{
		// we have a problem, trying to resend an already teed instance
#ifdef DEBUG
		printf("DLVistaSerialTeeFilter::FilterPacketL(%lx) -- TRYING TO REFILTER A PACKET WITH INDEX (%d)\n", long(pPacket), iIdx);
#endif
		throw;
	}    
}


bool DLVistaSerialTeeFilter::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock)
{
	int iIdx = GetPacketIndex(pPacket);
	if(iIdx == -1)
	{
		throw;
	}

	if(iIdx == (GetNumberOfOutbounds()-1))
	{
		// packet scheduled, give back
		//printf("DLVistaSerialTeeFilter::RecycleDataPacket(%X) -- returning packet via recycle (%d)\n", pPacket, iIdx);
		if(GrabMutex(m_pMapMutex, true))
		{        
			UpdatePacketIndex(pPacket, -1); // erase
			bool b = IDLVistaTeeFilter::RecycleDataPacket(pPacket, pSender, bBlock);
			(*m_pMapMutex).Unlock();
			return b;
		}
	}


	// ok, we have to resend this packet
	if(GrabMutex(m_pMapMutex, true))
	{        
		//printf("DLVistaSerialTeeFilter::RecycleDataPacket(%X) -- forwaring packet to new outbound [%d]\n", pPacket, iIdx+1);
		UpdatePacketIndex(pPacket, ++iIdx);
		IDLVistaPipeComponent *pComp = GetOutboundByIndex(iIdx);
		(*m_pMapMutex).Unlock();
		return pComp->AcceptDataPacket(pPacket, this, true);
	}
	return false;
}

