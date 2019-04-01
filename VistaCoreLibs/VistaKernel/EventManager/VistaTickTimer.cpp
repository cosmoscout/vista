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


#include "VistaTickTimer.h"

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaBase/VistaUtilityMacros.h>


#include "VistaTickTimerEvent.h"

#include <stdio.h>

//int VistaTickTimer::m_iTickTimerEventType = -1;

VistaTickTimer::VistaTickTimer( VistaEventManager *evMa, 
								 VistaClusterMode *pClusterAux )
: VistaEventObserver(),
  m_pClusterAux(pClusterAux)
{
	m_eTickerGranularity = SWT_MILLI;
	SetTickTime(1.0);
	m_pEventManager = evMa;

	if( VistaTickTimerEvent::GetTypeId() == VistaEvent::VET_INVALID )
	{
		VistaTickTimerEvent::RegisterEventTypes( evMa );
	}
	VistaTickTimerEvent::SetTypeId( VistaTickTimerEvent::GetTypeId() );

	m_pTickTimerTick = new VistaTickTimerEvent(this);
	m_pTickTimerTick->SetId(VistaTickTimerEvent::TTID_TIMEOUT);

	m_bIsRunning = false;

	m_dLastTick = -1;

	m_bIsPulsingTimer = true; // default!
	m_dRealLength = 0;
}


VistaTickTimer::~VistaTickTimer()
{
	if(IsRunning())
		StopTickTimer(); // remove as an observer
	delete m_pTickTimerTick;
}


void VistaTickTimer::Notify(const VistaEvent *pEvent)
{
	const VistaSystemEvent *pEv = Vista::assert_cast< const VistaSystemEvent* >( pEvent );
	if(pEv->GetId() != VistaSystemEvent::VSE_PREAPPLICATIONLOOP)
		return;


	if(IsRunning())
	{
		if(m_dLastTick == -1)
		{
			m_dLastTick = m_pClusterAux->GetFrameClock();
			return;
		}

		if((m_dRealLength=(m_pClusterAux->GetFrameClock() - m_dLastTick)) >= m_dTickTime)
		{
			// process Event!!
			if(m_eTickerGranularity == SWT_MILLI)
				m_dRealLength *= 1000; // convert from seconds to milliseconds

			m_pTickTimerTick->SetTickEventTime( pEvent->GetTime() );

			m_pEventManager->ProcessEvent(m_pTickTimerTick);
			m_dLastTick = m_pClusterAux->GetFrameClock();
		}
	}
}


	
void VistaTickTimer::SetGranularity(GRANULARITY eGran)
{
	m_eTickerGranularity = eGran;
	ResetTickTimer();
}

VistaTickTimer::GRANULARITY VistaTickTimer::GetGranularity() const
{
	return m_eTickerGranularity;
}

void VistaTickTimer::SetTickTime(double dTickTime)
{
	m_dOriginValue = dTickTime;
	ResetTickTimer();
}


bool VistaTickTimer::StartTickTimer()
{
	m_bIsRunning = true;
	m_pEventManager->RegisterObserver(this, VistaSystemEvent::GetTypeId());
	return IsRunning();
}


bool VistaTickTimer::StopTickTimer()
{
	m_bIsRunning = false;
	m_pEventManager->UnregisterObserver(this, VistaSystemEvent::GetTypeId());
	return IsRunning();
}

bool VistaTickTimer::IsRunning() const
{
	return m_bIsRunning;
}

bool VistaTickTimer::ResetTickTimer()
{
	m_dLastTick = -1;
	m_dTickTime = m_dOriginValue/m_eTickerGranularity; // normalize to fractions of a second!
	return false;
}


bool VistaTickTimer::GetPulsingTimer() const
{
	return m_bIsPulsingTimer;
}

void VistaTickTimer::SetPulsingTimer(bool bPulsing)
{
	m_bIsPulsingTimer = bPulsing;
}


double VistaTickTimer::GetRealIntervalLength() const
{
	return m_dRealLength;
}


double VistaTickTimer::GetTickTime() const
{
	return m_dOriginValue;
}
