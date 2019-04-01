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


#include "VistaAdaptiveVSyncControl.h"

#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <map>
#include "../EventManager/VistaSystemEvent.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaAdaptiveVSyncControl::VistaAdaptiveVSyncControl( VistaDisplayManager* pDisplayManager, VistaEventManager* pEventManager )
: m_nLastUpdate( 0 )
, m_nFramesExceedingLimit( -1 )
, m_nFrameCountToDisable( 10 )
, m_nFrameCountToEnable( 10 )
, m_nFramerateTolerance( 0.05f )
, m_nSwitchAtFramerate( 30.0f )
, m_bPrintStatusOnChange( true )
, m_pEventManager( pEventManager )
{
	const std::map<std::string, VistaWindow*>& mapWindows = pDisplayManager->GetWindowsConstRef();
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = mapWindows.begin();
			itWin != mapWindows.end(); ++itWin )
	{
		m_vecWindows.push_back( (*itWin).second );
	}
	m_pEventManager->AddEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREAPPLICATIONLOOP );
}

VistaAdaptiveVSyncControl::VistaAdaptiveVSyncControl( VistaWindow* pWindow, VistaEventManager* pEventManager )
: m_nLastUpdate( 0 )
, m_nFramesExceedingLimit( -1 )
, m_nFrameCountToDisable( 30 )
, m_nFrameCountToEnable( 30 )
, m_nFramerateTolerance( 0.05f )
, m_nSwitchAtFramerate( 30.0f )
, m_bPrintStatusOnChange( true )
, m_pEventManager( pEventManager )
{
	m_vecWindows.push_back( pWindow );
	m_pEventManager->AddEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREAPPLICATIONLOOP );
}

VistaAdaptiveVSyncControl::VistaAdaptiveVSyncControl( const std::vector<VistaWindow*>& vecWindows, VistaEventManager* pEventManager )
: m_nLastUpdate( 0 )
, m_nFramesExceedingLimit( -1 )
, m_nFrameCountToDisable( 30 )
, m_nFrameCountToEnable( 30 )
, m_nFramerateTolerance( 0.05f )
, m_nSwitchAtFramerate( 30.0f )
, m_bPrintStatusOnChange( true )
, m_vecWindows( vecWindows )
, m_pEventManager( pEventManager )
{
	m_pEventManager->AddEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREAPPLICATIONLOOP );
}

VistaAdaptiveVSyncControl::~VistaAdaptiveVSyncControl()
{
	m_pEventManager->RemEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREAPPLICATIONLOOP );
}

void VistaAdaptiveVSyncControl::HandleEvent( VistaEvent* pEvent )
{
	if( m_vecWindows.empty() )
		return;

	int nVSyncMode = m_vecWindows.front()->GetWindowProperties()->GetVSyncEnabled();
	if( nVSyncMode == -2 )
		return;

	bool bVSyncStatus = ( nVSyncMode != 0 );

	if( m_nLastUpdate == 0 )
	{
		m_nLastUpdate = pEvent->GetTime();
		m_nFramesExceedingLimit = 0;
		return;
	}

	VistaType::microtime nDeltaT = pEvent->GetTime() - m_nLastUpdate;

	if( bVSyncStatus && nDeltaT > (double)( 1.0f / m_nSwitchAtFramerate ) * (double)( 1.0f + m_nFramerateTolerance ) )
	{
		++m_nFramesExceedingLimit;
		if( m_nFramesExceedingLimit >= m_nFrameCountToDisable )
		{
			for( std::vector<VistaWindow*>::iterator itWin = m_vecWindows.begin();
				itWin != m_vecWindows.end(); ++itWin )
			{
				(*itWin)->GetWindowProperties()->SetVSyncEnabled( false );
			}
			if( m_bPrintStatusOnChange )
			{
				vstr::outi() << "[VistaAdaptiveVSyncControl]: VSync changed to OFF" << std::endl;
			}
			m_nFramesExceedingLimit = 0;
		}
	}
	else if( !bVSyncStatus && nDeltaT < (double)( 1.0f / m_nSwitchAtFramerate ) * (double)( 1.0f - m_nFramerateTolerance ) )
	{
		++m_nFramesExceedingLimit;
		if( m_nFramesExceedingLimit >= m_nFrameCountToEnable )
		{
			for( std::vector<VistaWindow*>::iterator itWin = m_vecWindows.begin();
				itWin != m_vecWindows.end(); ++itWin )
			{
				(*itWin)->GetWindowProperties()->SetVSyncEnabled( true );
			}
			if( m_bPrintStatusOnChange )
			{
				vstr::outi() << "[VistaAdaptiveVSyncControl]: VSync changed to ON" << std::endl;
			}
			m_nFramesExceedingLimit = 0;
		}
	}
	else
	{
		m_nFramesExceedingLimit = 0;
	}

	m_nLastUpdate = pEvent->GetTime();
}

float VistaAdaptiveVSyncControl::GetSwitchAtFramerate() const
{
	return m_nSwitchAtFramerate;
}

void VistaAdaptiveVSyncControl::SetSwitchAtFramerate( const float& oValue )
{
	m_nSwitchAtFramerate = oValue;
}

float VistaAdaptiveVSyncControl::GetFramerateTolerance() const
{
	return m_nFramerateTolerance;
}

void VistaAdaptiveVSyncControl::SetFramerateTolerance( const float& oValue )
{
	m_nFramerateTolerance = oValue;
}

int VistaAdaptiveVSyncControl::GetFrameCountToDisable() const
{
	return m_nFrameCountToDisable;
}

void VistaAdaptiveVSyncControl::SetFrameCountToDisable( const int& oValue )
{
	m_nFrameCountToDisable = oValue;
}

int VistaAdaptiveVSyncControl::GetFrameCountToEnable() const
{
	return m_nFrameCountToEnable;
}

void VistaAdaptiveVSyncControl::SetFrameCountToEnable( const int& oValue )
{
	m_nFrameCountToEnable = oValue;
}

void VistaAdaptiveVSyncControl::SetIsEnabled( bool bSet )
{
	m_nLastUpdate = 0;
	VistaEventHandler::SetIsEnabled( bSet );
}
