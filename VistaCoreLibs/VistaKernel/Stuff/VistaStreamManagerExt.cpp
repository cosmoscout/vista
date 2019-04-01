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


#include "VistaStreamManagerExt.h"

#include <VistaBase/VistaStreamUtils.h>

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/VistaFrameLoop.h>

#include <algorithm>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/


/*============================================================================*/
/* VistaKernelStreamInfoInterface                                            */
/*============================================================================*/


VistaKernelStreamInfoInterface::VistaKernelStreamInfoInterface( VistaSystem* pVistaSystem )
: VistaStreamManager::IInfoInterface()
, m_pVistaSystem( pVistaSystem )
{
}

VistaKernelStreamInfoInterface::~VistaKernelStreamInfoInterface()
{
}

VistaType::systemtime VistaKernelStreamInfoInterface::GetFrameClock() const
{
	return m_pVistaSystem->GetFrameClock();
}

std::string VistaKernelStreamInfoInterface::GetNodeName() const
{
	return m_pVistaSystem->GetClusterMode()->GetNodeName();
}

unsigned int VistaKernelStreamInfoInterface::GetFrameCount() const
{
	return m_pVistaSystem->GetFrameLoop()->GetFrameCount();
}

float VistaKernelStreamInfoInterface::GetFrameRate() const
{
	return m_pVistaSystem->GetFrameLoop()->GetFrameRate();
}


/*============================================================================*/
/* VistaSystemEventLogger                                                    */
/*============================================================================*/

VistaSystemEventLogger::VistaSystemEventLogger( VistaEventManager* pManager )
: VistaEventObserver()
, m_pManager( pManager )
, m_bRegistered( false )
{
	m_vecLogStreams.resize( VistaSystemEvent::VSE_UPPER_BOUND );
}

VistaSystemEventLogger::~VistaSystemEventLogger()
{
	m_pManager->UnregisterObserver( this, VistaSystemEvent::GetTypeId() );
}

void VistaSystemEventLogger::UnregisterStream( std::ostream& oStream )
{
	for( std::vector<std::list<std::ostream*> >::iterator 
			itVec = m_vecLogStreams.begin();
			itVec != m_vecLogStreams.end();
			++itVec )
	{
		std::list<std::ostream*>::iterator itStream = std::find( 
						(*itVec).begin(), (*itVec).end(), &oStream );
		if( itStream != (*itVec).end() )
			(*itVec).erase( itStream );
	}
}

void VistaSystemEventLogger::SetStreamEventMask( std::ostream& oStream, unsigned int iEventMask )
{
	if( m_bRegistered == false )
		m_pManager->RegisterObserver( this, VistaSystemEvent::GetTypeId() );

	int iEventId = 0;
	for( std::vector<std::list<std::ostream*> >::iterator 
			itVec = m_vecLogStreams.begin();
			itVec != m_vecLogStreams.end();
			++itVec, ++iEventId )
	{
		std::list<std::ostream*>::iterator itStream = std::find( 
			(*itVec).begin(), (*itVec).end(), &oStream );

		int iEventIndex = 1 << iEventId;
		if( iEventMask & iEventIndex )
		{
			if( itStream == (*itVec).end() )
				(*itVec).push_back( &oStream );
		}
		else
		{
			if( itStream != (*itVec).end() )
				(*itVec).erase( itStream );
		}
	}
}

void VistaSystemEventLogger::Notify( const VistaEvent *pEvent )
{
	for( std::list<std::ostream*>::iterator 
			itStream = m_vecLogStreams[pEvent->GetId()].begin();
			itStream != m_vecLogStreams[pEvent->GetId()].end();
			++itStream )
	{
		(*(*itStream)) << vstr::info << "System Event with ID " << std::setw(2) << pEvent->GetId() << ":"
						<< std::setw(31) << VistaSystemEvent::GetIdString( pEvent->GetId() )
						<< " | time " << vstr::formattime( pEvent->GetTime() ) << std::endl;
	}
}
