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


#include "VistaClusterSyncedTimerImp.h"

#include "VistaClusterMode.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include "VistaBase/VistaStreamUtils.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaClusterSyncedTimerImp::VistaClusterSyncedTimerImp()
: m_bIsLeader( false )
, m_bOwnConnections( false )
{
}

VistaClusterSyncedTimerImp::~VistaClusterSyncedTimerImp()
{
	if( m_bOwnConnections )
	{
		for( std::vector<VistaConnectionIP*>::iterator itConn = m_vecConnections.begin();
				itConn != m_vecConnections.end(); ++itConn )
		{
			delete (*itConn);
		}
	}
}


bool VistaClusterSyncedTimerImp::Init( VistaClusterMode* pMode )
{
	m_bOwnConnections = true;
	m_bIsLeader = pMode->GetIsLeader();
	return ( pMode->CreateConnections( m_vecConnections ) && m_vecConnections.empty() == false );
}

bool VistaClusterSyncedTimerImp::InitAsLeader( VistaConnectionIP* pConn, bool bManageConnDeletion )
{
	m_bOwnConnections = bManageConnDeletion;
	m_vecConnections.push_back( pConn );
	return true;
}

bool VistaClusterSyncedTimerImp::InitAsFollower( std::vector<VistaConnectionIP*> vecConns, bool bManageConnDeletion /*= true */ )
{
	m_bOwnConnections = bManageConnDeletion;
	m_vecConnections = vecConns;
	return ( m_vecConnections.empty() == false );
}


bool VistaClusterSyncedTimerImp::Sync( int nIterations )
{
	if( m_vecConnections.empty() )
		return false;
	if( m_bIsLeader == false )
	{
		VistaConnectionIP* pConn = m_vecConnections.front();
#ifdef WIN32
		// read initial system time and stamp
		pConn->ReadDouble( m_nInitialSystemTime );
		VistaType::uint64 nInitStamp;
		pConn->ReadUInt64( nInitStamp );
		m_nInitialStamp = nInitStamp;
#elif LINUX
		VistaType::systemtime nInitialMasterTime;
		// read initial system time
		pConn->ReadDouble( nInitialMasterTime );
		//m_nInitialTime = nInitialMasterTime;
#else
		VISTA_THROW_NOT_IMPLEMENTED;
#endif
		VistaType::sint32 nSyncedIterations = nIterations;
		pConn->ReadInt32( nSyncedIterations );
		std::vector<VistaType::microtime> vecReceivedTimes( nSyncedIterations );
		std::vector<VistaType::microtime> vecOwnTimes( nSyncedIterations );
		std::vector<VistaType::microtime>::iterator itReceived = vecReceivedTimes.begin();
		std::vector<VistaType::microtime>::iterator itOwn = vecOwnTimes.begin();
		for( ; itReceived != vecReceivedTimes.end(); ++itReceived, ++itOwn )
		{			
			pConn->ReadDouble( (*itReceived) );
			(*itOwn) = GetMicroTime();
			pConn->WriteDouble( (*itOwn) );
		}

		// now: process the info
		std::vector<VistaType::microtime> vecOwnDeltas( nSyncedIterations - 1 );
		std::vector<VistaType::microtime> vecReceivedDeltas( nSyncedIterations - 1 );
		std::vector<VistaType::microtime> vecOffsets( nSyncedIterations - 1 );
		VistaType::microtime nAvgDeltasRec = 0.0;
		VistaType::microtime nAvgDeltasOwn = 0.0;
		VistaType::microtime nAvgOffsets = 0.0;
		for( int i = 1; i < nSyncedIterations - 1; ++i )
		{
			vecReceivedDeltas[i] = vecReceivedTimes[i+1] - vecReceivedTimes[i];
			vecOwnDeltas[i] = vecOwnTimes[i+1] - vecOwnTimes[i];
			vecOffsets[i] = vecOwnTimes[i] - vecReceivedTimes[i];
			nAvgDeltasRec += vecReceivedDeltas[i];
			nAvgDeltasOwn += vecOwnDeltas[i];
			nAvgOffsets += vecOffsets[i];
		}
		nAvgDeltasRec /= (double)( nSyncedIterations - 2 );
		nAvgDeltasOwn /= (double)( nSyncedIterations - 2 );
		nAvgOffsets /= (double)( nSyncedIterations - 2 );
		VistaType::microtime nAvgDelay = nAvgOffsets - 0.5 * ( 0.5 * ( nAvgDeltasRec + nAvgDeltasOwn ) );		
#ifdef WIN32
		VistaType::microstamp nAvgDelayStamp = (VistaType::microstamp)( nAvgDelay / m_nFrequencyDenom + 0.5 );
		m_nInitialStamp -= nAvgDelayStamp;
#else
		m_nInitialTime += nAvgDelay;
#endif		
	}
	else // m_bIsLeader
	{
		for( std::vector<VistaConnectionIP*>::iterator itConn = m_vecConnections.begin();
				itConn != m_vecConnections.end(); ++itConn )
		{
#ifdef WIN32
			// first, send initial system time and initial stamp
			(*itConn)->WriteDouble( m_nInitialSystemTime );
			(*itConn)->WriteInt64( VistaType::sint64( m_nInitialStamp ) );
#elif LINUX
			(*itConn)->WriteDouble( m_nInitialTime );
#else
			VISTA_THROW_NOT_IMPLEMENTED;
#endif
			// write num iterations 
			(*itConn)->WriteInt32( VistaType::sint32( nIterations ) );
			VistaType::microtime nTime;
			for( int i = 0; i < nIterations; ++i )
			{
				nTime = GetMicroTime();
				(*itConn)->WriteDouble( nTime );
				(*itConn)->ReadDouble( nTime );
			}
		}
	}

	return true;
}
