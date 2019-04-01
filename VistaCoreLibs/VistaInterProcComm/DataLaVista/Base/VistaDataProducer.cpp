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

#include "VistaDataProducer.h"

#include <cstddef>
#include <list>
#include <cstddef>
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/


IDLVistaDataProducer::IDLVistaDataProducer()
{
	m_pDatipcoutput = 0;
}


IDLVistaDataProducer::~IDLVistaDataProducer()
{
	m_pDatipcoutput = 0;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


bool IDLVistaDataProducer::AttachInputComponent(IDLVistaPipeComponent *)
{
	return false;
}

bool IDLVistaDataProducer::AttachOutputComponent(IDLVistaPipeComponent *pComp)
{
	m_pDatipcoutput = pComp;
	return true;
}

bool IDLVistaDataProducer::DetachInputComponent(IDLVistaPipeComponent *pComp)
{
	return false;
}

bool IDLVistaDataProducer::DetachOutputComponent(IDLVistaPipeComponent *pComp)
{
	if(pComp == m_pDatipcoutput)
	{
		m_pDatipcoutput = 0;
		return true;
	}
	return false;
}


bool IDLVistaDataProducer::IsOutputComponent(IDLVistaPipeComponent *pComp) const
{
	return (pComp == m_pDatipcoutput);
}

list<IDLVistaPipeComponent *> IDLVistaDataProducer::GetInputComponents() const
{
	list<IDLVistaPipeComponent *> list; 
	return list;// return empty list
}

list<IDLVistaPipeComponent *> IDLVistaDataProducer::GetOutputComponents() const
{
	list<IDLVistaPipeComponent *> list;
	if(m_pDatipcoutput)
	{
		list.push_back(m_pDatipcoutput);
	}
	return list;
}

IDLVistaPipeComponent *IDLVistaDataProducer::GetOutboundByIndex(int iIndex) const
{
	return (iIndex == 0 ? m_pDatipcoutput : NULL);
}


IDLVistaPipeComponent *IDLVistaDataProducer::GetInboundByIndex(int iIndex) const
{
	return NULL;
}

int IDLVistaDataProducer::GetNumberOfOutbounds() const
{
	return m_pDatipcoutput ? 1 : 0;
}

int IDLVistaDataProducer::GetNumberOfInbounds() const
{
	return 0;
}


int IDLVistaDataProducer::TryToReclaimPendingPackets()
{
	IDLVistaDataPacket *p = m_pDatipcoutput->ReturnPacket();
	int i=0;
	while( p != NULL )
	{
		RecycleDataPacket(p, this); // fill 'er up!
		++i;
		p = m_pDatipcoutput->ReturnPacket();
	}
	//printf("DLVistaDataSource::FillUp() -- retrieved %d packets.\n", i);
	return i;
}

bool IDLVistaDataProducer::WaitForNextPacket(int iTimeout) const
{
	return HasPacket();
}
