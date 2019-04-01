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


#include "VistaTeeFilter.h"
#include "VistaPacketQueue.h"
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <algorithm>
using namespace std;
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/


IDLVistaTeeFilter::IDLVistaTeeFilter()
{
	m_pOutbounds = new deque<IDLVistaPipeComponent *>;
	m_pMutexIn = new VistaMutex;
	m_pMutexOut = new VistaMutex;
	m_pModifyOutbounds = new VistaMutex;
	m_pRecycleEvent = new VistaThreadEvent(VistaThreadEvent::NON_WAITABLE_EVENT);

	m_pLockEmpty = NULL;
}

IDLVistaTeeFilter::~IDLVistaTeeFilter()
{
	(*m_pMutexIn).Lock(); // no new packets
	(*m_pMutexOut).Lock(); // no more recycling now

	delete m_pMutexIn;
	delete m_pMutexOut;
	delete m_pModifyOutbounds;
	delete m_pRecycleEvent;
	delete m_pLockEmpty;

	delete m_pOutbounds;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

VistaMutex *IDLVistaTeeFilter::GrabMutex(VistaMutex *pMutex, bool bBlock)
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


bool IDLVistaTeeFilter::AttachOutputComponent(IDLVistaPipeComponent * pComp)
{
	bool bRet = false;
	(*m_pModifyOutbounds).Lock();
	if(find((*m_pOutbounds).begin(), (*m_pOutbounds).end(), pComp) == (*m_pOutbounds).end())
	{
		(*m_pOutbounds).push_back(pComp);
		if(!m_pOutput)
			m_pOutput = pComp; // make the first one the default!
		bRet = true;
	}

	(*m_pModifyOutbounds).Unlock();
	return bRet;
}


bool IDLVistaTeeFilter::DetachOutputComponent(IDLVistaPipeComponent * pComp)
{
	bool bRet = false;
	(*m_pModifyOutbounds).Lock();

	deque<IDLVistaPipeComponent *>::iterator it = find((*m_pOutbounds).begin(), (*m_pOutbounds).end(), pComp);
	if(it!=(*m_pOutbounds).end())
	{
		(*m_pOutbounds).erase(it);
		bRet = true;
	}

	(*m_pModifyOutbounds).Unlock();
	return bRet;
}


int IDLVistaTeeFilter::GetNumberOfOutbounds() const
{
	(*m_pModifyOutbounds).Lock();
	int iRet = (int)(*m_pOutbounds).size();
	(*m_pModifyOutbounds).Unlock();

	return iRet;
}

IDLVistaPipeComponent *IDLVistaTeeFilter::GetOutboundByIndex(int iIdx) const
{
	(*m_pModifyOutbounds).Lock();
	IDLVistaPipeComponent *p = (*m_pOutbounds)[iIdx];
	(*m_pModifyOutbounds).Unlock();
	return p;
}

