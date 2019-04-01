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

#include "VistaFilter.h"

#include "VistaDataPacket.h"
#include "VistaPacketQueue.h"

#include <list>
using namespace std;
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

IDLVistaFilter::IDLVistaFilter()
{
	m_pInput = m_pOutput = 0;

}


IDLVistaFilter::IDLVistaFilter(IDLVistaFilter &)
{
	
	//throw;
}


IDLVistaFilter::~IDLVistaFilter()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

IDLVistaDataPacket *IDLVistaFilter::PullPacket(bool bBlock)
{
	return m_pInput->GivePacket(bBlock);
}

void IDLVistaFilter::ConsumePacket(IDLVistaDataPacket * )
{
}

bool IDLVistaFilter::AttachOutputComponent(IDLVistaPipeComponent *pOut )
{
	m_pOutput = pOut;
	return true;
}

bool IDLVistaFilter::AttachInputComponent(IDLVistaPipeComponent *pIn )
{
	m_pInput = pIn;
	return true;
}

bool IDLVistaFilter::DetachInputComponent(IDLVistaPipeComponent * pComp)
{
	if(pComp == m_pInput)
	{
		m_pInput = 0;
		return true;
	}
	return false;
}

bool IDLVistaFilter::DetachOutputComponent(IDLVistaPipeComponent *pComp )
{
	if(pComp == m_pOutput)
	{
		m_pOutput = 0;
		return true;
	}
	return false;
}

bool IDLVistaFilter::RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	(*m_pInput).RecycleDataPacket(pPacket, this);
	//(*m_pOutboundPackets).push_front(pPacket);
	return true;
}

bool IDLVistaFilter::AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	IDLVistaDataPacket *p = FilterPacketL(pPacket);
	if(p) // ok, filtering worked
	{
		return this->m_pOutput->AcceptDataPacket(p, this);
	}
	return false;
}

bool IDLVistaFilter::IsInputComponent(IDLVistaPipeComponent *pComp) const
{
	return (m_pInput == pComp);
}

bool IDLVistaFilter::IsOutputComponent(IDLVistaPipeComponent *pComp) const
{
	return (m_pOutput == pComp);
}


list<IDLVistaPipeComponent *> IDLVistaFilter::GetOutputComponents() const
{
	list<IDLVistaPipeComponent *> list;
	if(m_pOutput)
		list.push_back(m_pOutput);
	return list;
}

list<IDLVistaPipeComponent *> IDLVistaFilter::GetInputComponents() const
{
	list<IDLVistaPipeComponent *> list;
	if(m_pInput)
	{
		list.push_back(m_pInput);
	}
	return list;
}


IDLVistaPipeComponent *IDLVistaFilter::GetOutboundByIndex(int iIndex) const 
{
	return (iIndex == 0 ? m_pOutput : NULL);
}

int IDLVistaFilter::GetNumberOfOutbounds() const 
{
	return (m_pInput ? 1 : 0);
}


int IDLVistaFilter::GetNumberOfInbounds() const
{
	return (m_pOutput ? 1 : 0);
}


IDLVistaPipeComponent *IDLVistaFilter::GetInboundByIndex(int iIndex) const 
{
	return (iIndex == 0 ? m_pInput : NULL);
}

IDLVistaDataPacket *IDLVistaFilter::ReturnPacket()
{
	//forward return call to output
	if(m_pOutput != 0)
		return m_pOutput->ReturnPacket();
	else
		return NULL;
}
