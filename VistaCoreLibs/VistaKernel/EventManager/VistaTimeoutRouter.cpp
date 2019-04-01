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


#include "VistaTimeoutRouter.h" 
#include "VistaTickTimer.h"
#include "VistaEvent.h"

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

bool VistaTimeoutRouter::CTimedCallback::Callback(double dTimeStamp, 
											  double dDesiredLength, 
											  double dRealLength)
{
	if(m_nRepeatCount > 0)
		--m_nRepeatCount;
	return DoCallback(dTimeStamp, dDesiredLength, dRealLength);
}

int VistaTimeoutRouter::CTimedCallback::GetRepeatCount() const
{
	return m_nRepeatCount;
}
void VistaTimeoutRouter::CTimedCallback::SetRepeatCount(int nRepeatCount)
{
	m_nRepeatCount = nRepeatCount;
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaTimeoutRouter::VistaTimeoutRouter(VistaEventManager *pEvMgr,
										 VistaClusterMode *pClusterMode)
: VistaTimeoutHandler(pEvMgr, pClusterMode),
  m_dEventTick(0),
  m_bInCallback(false)
{
}


VistaTimeoutRouter::~VistaTimeoutRouter()
{
	// kill list pointers in map
	for(CANDLIST::const_iterator cit = m_mpUpdateCandidates.begin();
		cit != m_mpUpdateCandidates.end(); ++cit)
	{
		delete (*cit).second; // kill list
	}
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


void VistaTimeoutRouter::AddUpdateCall(int iMsecs, CTimedCallback *pCb)
{
	HD_TIMER objTimer = NULL;
	CBLIST *liObjs = NULL;
	// a) check for existing timer
	std::map<int, HD_TIMER>::const_iterator it1 = m_mpTimers.find(iMsecs);
	if(it1 != m_mpTimers.end())
	{
		// we already have an update length here
		objTimer = (*it1).second;
	}
	else
	{
		// no timer, yet, so lets add it
		objTimer = AddTimeout((double)iMsecs);
		
		// add to time map
		m_mpTimers.insert( std::map<int,HD_TIMER>::value_type( iMsecs, objTimer ) );
	}

	// check for an existing list in the updaters map
	CANDLIST::iterator it = m_mpUpdateCandidates.find(objTimer);
	if(it == m_mpUpdateCandidates.end())
	{
		// nope... strange, but hey...
		liObjs = new CBLIST;

		// add to lookup map
		m_mpUpdateCandidates.insert(CANDLIST::value_type(objTimer, liObjs));
	}
	else
	{
		// assign old value
		liObjs = (*it).second;
	}

	// push object to list of object candidates
	liObjs->push_back(pCb);
}

bool VistaTimeoutRouter::RemUpdateCall(int iMsecs, CTimedCallback *pCb)
{
	HD_TIMER objTimer = NULL;
	
	std::map<int, HD_TIMER>::iterator it1 = m_mpTimers.find(iMsecs);
	if(it1 != m_mpTimers.end())
	{
		objTimer = it1->second;

		// look for list entry in candidate map
		   // check for an existing list in the updaters map
		CANDLIST::iterator it = m_mpUpdateCandidates.find(objTimer);
		if(it != m_mpUpdateCandidates.end())
		{
			if(m_bInCallback)
				m_liScheduledRemoval.push_back(REMPAIR(objTimer, pCb));
			else
			{
				DoReleaseTim( REMPAIR(objTimer, pCb) );
			}
			return true;
		}
	}
	return false;
}


void VistaTimeoutRouter::HandleTimeout(HD_TIMER tim)
{
   CANDLIST::const_iterator it = m_mpUpdateCandidates.find(tim);
	if(it != m_mpUpdateCandidates.end())
	{
		if(!CallUpdaters(it->second, tim))
		{
			vstr::errp() << "[VistaTimeoutRouter::HandleTimeout]: update candidate list!" << std::endl;
		}
	}

	for(TIMLIST::iterator lit = m_liScheduledRemoval.begin();
		lit != m_liScheduledRemoval.end(); ++lit)
	{
		DoReleaseTim( *lit );
	}
	m_liScheduledRemoval.clear();
}

void VistaTimeoutRouter::DoReleaseTim( const REMPAIR &rempair )
{
	CANDLIST::iterator it = m_mpUpdateCandidates.find(rempair.first);
	if(it != m_mpUpdateCandidates.end())
	{
		// found it
		CBLIST *liObjs = (*it).second;
		liObjs->remove( rempair.second );

		if(liObjs->empty())
		{
			// remove it1 from m_mpTimers
			std::map<int, HD_TIMER>::iterator it1 = m_mpTimers.find( (int)rempair.first->GetTickTime() );
			m_mpTimers.erase(it1);

			delete liObjs;
			m_mpUpdateCandidates.erase(it); // remove reference in map
			RemoveTimeout(const_cast<HD_TIMER>(rempair.first));
		}
	}
}

void VistaTimeoutRouter::HandleEvent(VistaEvent *pEvent)
{
	m_dEventTick = pEvent->GetTime();
	VistaTimeoutHandler::HandleEvent(pEvent);
}

bool VistaTimeoutRouter::CallUpdaters(CBLIST *pList, HD_TIMER tim)
{
	m_bInCallback = true;
	for(CBLIST::const_iterator lit = (*pList).begin();
		lit != (*pList).end(); ++lit)
	{
		(*lit)->Callback(m_dEventTick, tim->GetTickTime(), tim->GetRealIntervalLength());
		if( (*lit)->GetRepeatCount() == 0 )
		{
			(*lit)->CallbackDoneHandshake();
			RemUpdateCall( (int)tim->GetTickTime(), (*lit) );
		}
	}
	m_bInCallback = false;
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


