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

#include "VistaTransformFilter.h"

#include "VistaDataPacket.h"
#include "VistaPacketQueue.h"
#include <cstdio>
#include <iostream>
#include <cstdio>

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

IDLVistaTransformFilter::IDLVistaTransformFilter()
	:	IDLVistaFilter(),
		m_iOutboundQueueSize(1),
		m_iFilterCount(0),
		m_iDropCount(0)
{
	m_pOutboundPackets = new DLVistaPacketQueue(this, 0);
}


IDLVistaTransformFilter::~IDLVistaTransformFilter()
{
#ifdef DEBUG	
	printf("\n\nIDLVistaTransformFilter::~IDLVistaTransformFilter() -- \
		%d drop for %d filtered packets (%d packets in queue: ratio [%f]).\n\n", 
        m_iDropCount, m_iFilterCount, 
        m_pOutboundPackets->GetPacketCount(), 
        m_iFilterCount!=0 ? ((float)m_iDropCount)/((float)m_iFilterCount) : 0.0f);
#endif
	delete m_pOutboundPackets;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


void IDLVistaTransformFilter::SetOutboundQueueSize(int iSize)
{
	m_iOutboundQueueSize = iSize;
}

int IDLVistaTransformFilter::GetOutboundQueueSize() const
{
	return m_iOutboundQueueSize;
}


bool IDLVistaTransformFilter::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	// we gave back the incoming packet directly after the
	// transform step, so we are the one to claim this
	// packet.
	return (*m_pOutboundPackets).RecyclePacket(pPacket);
}

bool IDLVistaTransformFilter::AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	// we should do the following
	// assert(pPacket != NULL)
	IDLVistaDataPacket *p = FilterPacketL(pPacket);
	// ok, since we are a transform filter, we can safely assume:
	// assert(p!=pPacket)
	// if this is not the case, we should not reach the next line.
	
	//we are a transform filter so give back the input packet 
	//immediately (independent from the outcome of FilterPacketL)
	pComp->RecycleDataPacket(pPacket, this);
	// ok, filtering worked
	if(p) 
	{
		// forward the new one to output
		this->m_pOutput->AcceptDataPacket(p, this);
		// we are done, get loose
		return true;
	}
	return false;
}

bool IDLVistaTransformFilter::InitPacketMgmt()
{
	return ((*m_pOutboundPackets).Resize(m_iOutboundQueueSize)==m_iOutboundQueueSize);
}

IDLVistaDataPacket *IDLVistaTransformFilter::FilterPacketL(IDLVistaDataPacket *pPacket )
{
	//o.k. we filter a packet here -> count this one
	++m_iFilterCount;

	IDLVistaDataPacket *pRet = NULL;
	IDLVistaDataPacket *p = NULL;
	//get a fresh packet for sending it downstream
	if(this->m_pOutboundPackets->GetAvailablePacketCount() > 0)
	{
		p = (*m_pOutboundPackets).GiveFreshPacket();
	}
	else
	{
		//reclaim a packet which is currently under way and reuse that one if possible
		p = this->GetOutboundByIndex(0)->ReturnPacket();
		
		if (p)
		{
			this->RecycleDataPacket(p, this);
			p = (*m_pOutboundPackets).GiveFreshPacket();
		}
		else
		{
			// we have to drop this packet!
			printf("IDLVistaTransformFilter::FilterPacketL(%lx) -- drop packet, no outbound present.\n", long(pPacket));
			++m_iDropCount;
			return NULL;
		}
	}
	
	if(DoTransform(pPacket, p))
		pRet = p;
	else
	{
		// recycle the fresh one in our own outbound queue
		RecycleDataPacket(p, this);
	}
	return pRet;
}

IDLVistaDataPacket *IDLVistaTransformFilter::GivePacket(bool bBlock)
{
	// ok, first get input packet
	IDLVistaDataPacket *pInputPkg = m_pInput->GivePacket(bBlock);
	IDLVistaDataPacket *pOutputPkg = this->FilterPacketL(pInputPkg);
	//we are a transform filter so the input packet is passed back immediately
	m_pInput->RecycleDataPacket(pInputPkg, this);
	return pOutputPkg;
}


IDLVistaDataPacket *IDLVistaTransformFilter::ReturnPacket()
{
	return NULL; // we gave back all incoming packets, so no need to return any
}

