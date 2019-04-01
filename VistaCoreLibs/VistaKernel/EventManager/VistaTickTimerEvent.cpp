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


#include "VistaTickTimerEvent.h"

#include "VistaEventManager.h"

int VistaTickTimerEvent::m_nEventId = VistaEvent::VET_INVALID;


VistaTickTimerEvent::VistaTickTimerEvent(VistaTickTimer *watch)
{
	SetType(VistaTickTimerEvent::GetTypeId());
	m_pSource = watch;
}

VistaTickTimerEvent::~VistaTickTimerEvent()
{}


std::string VistaTickTimerEvent::GetName() const
{
	return "TickTimerEvent";
}


std::string VistaTickTimerEvent::GetIdString(int nId) 
{
	switch(nId)
	{
	case TTID_TIMEOUT:
		return "TTID_TIMEOUT";
	case TTID_UPPER_BOUND:
		return "TTID_UPPER_BOUND";
	default:
		return VistaEvent::GetIdString(nId);
	}
}


int VistaTickTimerEvent::GetTypeId()
{
	return VistaTickTimerEvent::m_nEventId;
}

void VistaTickTimerEvent::SetTypeId(int nId)
{
	if(VistaTickTimerEvent::m_nEventId == VistaEvent::VET_INVALID)
		VistaTickTimerEvent::m_nEventId = nId;
}

void VistaTickTimerEvent::SetTickEventTime( double dTime )
{
	m_nTime = dTime;
}

void VistaTickTimerEvent::RegisterEventTypes( VistaEventManager* pEventManager )
{
	VistaEventManager::EVENTTYPE eTp = pEventManager->RegisterEventType( "VET_TICK" );
	SetTypeId( eTp );
	pEventManager->RegisterEventId( eTp, VistaTickTimerEvent::GetIdString( VistaTickTimerEvent::TTID_TIMEOUT ) );
}

