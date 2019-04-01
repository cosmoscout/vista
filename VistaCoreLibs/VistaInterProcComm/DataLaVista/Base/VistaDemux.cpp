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
#include "VistaDemux.h"

#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaDemux::DLVistaDemux() : m_itNextInput(m_liInputs.end())
{
}


DLVistaDemux::DLVistaDemux(const DLVistaDemux &rFilter)
: IDLVistaTransformFilter()
{
}


DLVistaDemux::~DLVistaDemux()
{
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
bool DLVistaDemux::AttachInputComponent(IDLVistaPipeComponent *pIn )
{
	VistaMutexLock oL(m_oFilterLock);
	m_liInputs.push_back(pIn);
	if(m_liInputs.size() == 1)
		m_itNextInput = m_liInputs.begin();
	return true;
}

bool DLVistaDemux::DetachInputComponent(IDLVistaPipeComponent *pIn )
{
	VistaMutexLock oL(m_oFilterLock);
	std::list<IDLVistaPipeComponent *>::iterator it(m_liInputs.begin());
	while (it!=m_liInputs.end())
	{
		if(*it == pIn)
		{
			m_itNextInput = m_liInputs.erase(it);
			return true;
		}
		++it;
	}
	return false;
}

IDLVistaPipeComponent *DLVistaDemux::GetInboundByIndex(int i) const
{
	//VistaMutexLock oL(m_oFilterLock);
	std::list<IDLVistaPipeComponent*>::size_type n(i);
	if(n>=m_liInputs.size())
		return NULL;
	std::list<IDLVistaPipeComponent *>::const_iterator it(m_liInputs.begin());
	for(; i>0; --i, ++it);
	return *it;
}

bool DLVistaDemux::IsInputComponent(IDLVistaPipeComponent *pIn ) const
{
	//VistaMutexLock oL(m_oFilterLock);
	std::list<IDLVistaPipeComponent *>::const_iterator it(m_liInputs.begin());
	while (it!=m_liInputs.end())
	{
		if(*it == pIn)
			return true;
		++it;
	}
	return false;
}

bool DLVistaDemux::AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pComp, bool bBlock)
{
	//we just have to ensure, that the output is accessed exclusively -> so lock here
	VistaMutexLock oL(m_oFilterLock);
	//packet transport itself is the same as with a normal filter
	return IDLVistaTransformFilter::AcceptDataPacket(pPacket, pComp, bBlock);
}

IDLVistaDataPacket *DLVistaDemux::GivePacket(bool bBlock)
{
	//sanity check 
	/** @todo remove this? */
	if(m_liInputs.empty())
		return NULL;

	VistaMutexLock oL(m_oFilterLock);

	//try to get an input packet from the "next" inbound
	IDLVistaDataPacket *pInputPkg = NULL;
	IDLVistaPipeComponent *pInput = NULL;
	std::list<IDLVistaPipeComponent*>::size_type iNumTries = 0;
	while(!pInputPkg && iNumTries < m_liInputs.size())
	{
		pInputPkg = (*m_itNextInput)->GivePacket(bBlock);
		//remember the input from which we got the packet
		pInput = *m_itNextInput; 
		++m_itNextInput;
		if(m_itNextInput == m_liInputs.end())
			m_itNextInput = m_liInputs.begin();
		++iNumTries;
	}
	if(!pInputPkg)
		return NULL;
	
	//transform filter the packet and recycle it
	IDLVistaDataPacket *pOutputPkg = this->FilterPacketL(pInputPkg);
	pInput->RecycleDataPacket(pInputPkg, this);
	return pOutputPkg;
}

