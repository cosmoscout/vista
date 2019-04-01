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


#include "VistaInterProcClusterDataSync.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaInterProcComm/Connections/VistaConnectionNamedPipe.h>

#include <cstring>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/


/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaInterProcClusterLeaderDataSync::VistaInterProcClusterLeaderDataSync(
									const bool bActAsInterProcServer,
									const bool bVerbose )
: VistaClusterBytebufferLeaderDataSyncBase( bVerbose )
, m_bActAsInterProcServer( bActAsInterProcServer )
{	
}

VistaInterProcClusterLeaderDataSync::~VistaInterProcClusterLeaderDataSync()
{	

	for( std::vector<VistaConnectionNamedPipe*>::iterator itPipe = m_vecConnections.begin();
			itPipe != m_vecConnections.end(); ++itPipe )
	{
		delete (*itPipe);
	}
}


bool VistaInterProcClusterLeaderDataSync::AddInterProc( const std::string& sInterProcName )
{	
	m_vecConnections.push_back( new VistaConnectionNamedPipe( sInterProcName, m_bActAsInterProcServer ) );
	return true;
}


bool VistaInterProcClusterLeaderDataSync::ConnectPipes()
{
	for( std::vector<VistaConnectionNamedPipe*>::iterator itPipe = m_vecConnections.begin();
			itPipe != m_vecConnections.end(); ++itPipe )
	{
		if( (*itPipe)->Open() == false )
		{
			// @todo
		}
	}
	return GetIsValid();
}

bool VistaInterProcClusterLeaderDataSync::GetIsValid() const
{
	return ( m_vecConnections.empty() == false );
}


std::string VistaInterProcClusterLeaderDataSync::GetDataSyncType() const
{
	return "VistaInterProcClusterLeaderDataSync";
}

int VistaInterProcClusterLeaderDataSync::GetSendBlockingThreshold() const
{
	return -1;
}

bool VistaInterProcClusterLeaderDataSync::SetSendBlockingThreshold( const int nNumBytes )
{
	return false;
}

bool VistaInterProcClusterLeaderDataSync::DoSendMessage()
{
	for( std::vector<VistaConnectionNamedPipe*>::iterator itPipe = m_vecConnections.begin();
			itPipe != m_vecConnections.end(); ++itPipe )
	{
		try
		{
			if( m_oMessage.GetBufferSize() > 0 )
				(*itPipe)->WriteRawBuffer( m_oMessage.GetBuffer(), m_oMessage.GetBufferSize() );
			if( m_pExtBuffer )
				(*itPipe)->WriteRawBuffer( m_pExtBuffer, m_nExtBufferSize );
			//static int nCount = 0;
			//std::cout << "[" << nCount++ << "] - sent " << std::setw(6) <<  m_oMessage.GetBufferSize() << "bytes" << std::endl;
		}
		catch( VistaExceptionBase& )
		{
			return false;
		}
		
	}
	return true;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaInterProcClusterFollowerDataSync::VistaInterProcClusterFollowerDataSync( 
									const std::string& sInterProcName,
									const bool bActAsInterProcServer,
									const bool bVerbose )
: VistaClusterBytebufferFollowerDataSyncBase( false, bVerbose )
{	
	m_pConnection = new VistaConnectionNamedPipe( sInterProcName, bActAsInterProcServer );
}

VistaInterProcClusterFollowerDataSync::~VistaInterProcClusterFollowerDataSync()
{
	delete m_pConnection;
}


bool VistaInterProcClusterFollowerDataSync::GetIsValid() const
{
	return ( m_pConnection != NULL );
}

std::string VistaInterProcClusterFollowerDataSync::GetDataSyncType() const
{
	return "VistaInterProcClusterFollowerDataSync";
}

int VistaInterProcClusterFollowerDataSync::GetSendBlockingThreshold() const
{
	return -1;
}

bool VistaInterProcClusterFollowerDataSync::SetSendBlockingThreshold( const int nNumBytes )
{
	return false;
}

bool VistaInterProcClusterFollowerDataSync::DoReceiveMessage()
{
	try
	{
		VistaType::sint32 nMessageSize;
		m_pConnection->ReadInt32( nMessageSize );
		if( (int)m_vecMessageBuffer.size() < nMessageSize )
			m_vecMessageBuffer.resize( nMessageSize );
		m_pConnection->ReadRawBuffer( &m_vecMessageBuffer[0], nMessageSize );
		m_oMessage.SetBuffer( &m_vecMessageBuffer[0], nMessageSize );

		//static int nCount = 0;
		//std::cout << "[" << nCount++ << "] - received " << std::setw(6) << nMessageSize+4 << "bytes" << std::endl;
	}
	catch( VistaExceptionBase& )
	{
		// @todo. proper error handling
		VISTA_THROW( "INTERPROC FAIL", -1 );
	}
	return true;
}

bool VistaInterProcClusterFollowerDataSync::ConnectPipe()
{
	if( m_pConnection->Open() == false )
	{
		delete m_pConnection;
		m_pConnection = NULL;
		return false;
	}
	return true;
}



/*============================================================================*/
/* REDISTRIBUTEFOLLOWER                                                       */
/*============================================================================*/

VistaInterProcClusterRedirectDataSync::VistaInterProcClusterRedirectDataSync( 
									IVistaClusterDataSync* pOriginalClusterSync,
									const bool bVerbose )
: VistaInterProcClusterLeaderDataSync( bVerbose )
{
	SetDependentDataSync( pOriginalClusterSync );
}

VistaInterProcClusterRedirectDataSync::VistaInterProcClusterRedirectDataSync( 
									const bool bVerbose )
: VistaInterProcClusterLeaderDataSync( bVerbose )
, m_pDependentDataSync( NULL )
, m_pDependentDataSyncMessage( NULL )
{
	m_bIsLeader = false;
}

VistaInterProcClusterRedirectDataSync::~VistaInterProcClusterRedirectDataSync()
{	
}

bool VistaInterProcClusterRedirectDataSync::GetIsValid() const
{
	return ( m_vecConnections.empty() == false && m_pDependentDataSync != NULL );
}


std::string VistaInterProcClusterRedirectDataSync::GetDataSyncType() const
{
	return "VistaInterProcClusterRedistributeDataSync";
}
bool VistaInterProcClusterRedirectDataSync::SyncTime( VistaType::systemtime& nTime )
{
	if( m_pDependentDataSync->SyncTime( nTime ) == false )
		return false;
	if( m_pDependentDataSyncMessage )
	{
		return SendRedirectedMessage();
	}
	else
	{
		// not very efficient, but oh well
		return VistaInterProcClusterLeaderDataSync::SyncTime( nTime );
	}
}

bool VistaInterProcClusterRedirectDataSync::SyncData( VistaPropertyList& oList )
{
	if( m_pDependentDataSync->SyncData( oList ) == false )
		return false;
	if( m_pDependentDataSyncMessage )
	{
		return SendRedirectedMessage();
	}
	else
	{
		// not very efficient, but oh well
		return VistaInterProcClusterLeaderDataSync::SyncData( oList );
	}
}

bool VistaInterProcClusterRedirectDataSync::SyncData( IVistaSerializable& oSerializable )
{
	if( m_pDependentDataSync->SyncData( oSerializable ) == false )
		return false;
	if( m_pDependentDataSyncMessage )
	{
		return SendRedirectedMessage();
	}
	else
	{
		// not very efficient, but oh well
		return VistaInterProcClusterLeaderDataSync::SyncData( oSerializable );
	}
}

bool VistaInterProcClusterRedirectDataSync::SyncData( VistaType::byte* pData, 
														const int iDataSize )
{
	if( m_pDependentDataSync->SyncData( pData, iDataSize ) == false )
		return false;
	if( m_pDependentDataSyncMessage )
	{
		return SendRedirectedMessage();
	}
	else
	{
		// not very efficient, but oh well
		return VistaInterProcClusterLeaderDataSync::SyncData( pData, iDataSize );
	}
}

bool VistaInterProcClusterRedirectDataSync::SyncData( VistaType::byte* pDataBuffer, 
															const int iBufferSize,
															int& iDataSize )
{
	if( m_pDependentDataSync->SyncData( pDataBuffer, iBufferSize, iDataSize ) == false )
		return false;
	if( m_pDependentDataSyncMessage )
	{
		return SendRedirectedMessage();
	}
	else
	{
		// not very efficient, but oh well
		return VistaInterProcClusterLeaderDataSync::SyncData( pDataBuffer, iBufferSize, iDataSize );
	}
}

bool VistaInterProcClusterRedirectDataSync::SyncData( std::vector<VistaType::byte>& vecData )
{
	if( m_pDependentDataSync->SyncData( vecData ) == false )
		return false;
	if( m_pDependentDataSyncMessage )
	{
		return SendRedirectedMessage();
	}
	else
	{
		// not very efficient, but oh well
		return VistaInterProcClusterLeaderDataSync::SyncData( vecData );
	}
}

bool VistaInterProcClusterRedirectDataSync::SyncData( std::string& sData )
{
	if( m_pDependentDataSync->SyncData( sData ) == false )
		return false;
	if( m_pDependentDataSyncMessage )
	{
		return SendRedirectedMessage();
	}
	else
	{
		// not very efficient, but oh well
		return VistaInterProcClusterLeaderDataSync::SyncData( sData );
	}
}



bool VistaInterProcClusterRedirectDataSync::SendRedirectedMessage()
{
	// this looks a little hacked -- probably because it is...		
	m_pExtBuffer = m_pDependentDataSyncMessage->GetMessageBuffer();
	//@todo: HACKHACK
	memcpy( &m_nExtBufferSize, m_pExtBuffer, sizeof(VistaType::sint32) );
	return DoSendMessage();
}

IVistaClusterDataSync* VistaInterProcClusterRedirectDataSync::GetDependentDataSync() const
{
	return m_pDependentDataSync;
}

void VistaInterProcClusterRedirectDataSync::SetDependentDataSync( IVistaClusterDataSync* pSync )
{
	m_pDependentDataSync = pSync;
	m_pDependentDataSyncMessage = dynamic_cast<VistaClusterBytebufferFollowerDataSyncBase*>( pSync );
	m_bIsLeader = m_pDependentDataSyncMessage->GetIsLeader();
}
