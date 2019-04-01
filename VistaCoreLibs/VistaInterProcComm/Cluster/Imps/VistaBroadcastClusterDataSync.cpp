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


#include "VistaBroadcastClusterDataSync.h"

#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaAspects/VistaPropertyList.h>

#include <cassert>
#include "VistaBase/VistaTimeUtils.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

namespace
{
	const int S_nMaxPackageSize = 512;
}

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaBroadcastClusterLeaderDataSync::VistaBroadcastClusterLeaderDataSync(
									const bool bVerbose )
: VistaClusterBytebufferLeaderDataSyncBase( bVerbose )
{
}

VistaBroadcastClusterLeaderDataSync::VistaBroadcastClusterLeaderDataSync(
									const std::string& sBroadcastIP,
									const int nBroadcastPort,
									const bool bVerbose )
: VistaClusterBytebufferLeaderDataSyncBase( bVerbose )
{
	AddBroadcast( sBroadcastIP, nBroadcastPort );
}


VistaBroadcastClusterLeaderDataSync::VistaBroadcastClusterLeaderDataSync(
									VistaUDPSocket* pBroadcastSocket,
									const bool bManageDeletion,
									const bool bVerbose )
: VistaClusterBytebufferLeaderDataSyncBase( bVerbose )
{
	AddBroadcast( pBroadcastSocket, bManageDeletion );
}


VistaBroadcastClusterLeaderDataSync::~VistaBroadcastClusterLeaderDataSync()
{
	for( std::size_t i = 0; i < m_vecBCSenderSockets.size(); ++i )
	{
		if( m_vecDeleteBroadcastSocket[i] )
			delete m_vecBCSenderSockets[i];
	}
}
bool VistaBroadcastClusterLeaderDataSync::GetIsValid() const
{
	return ( m_vecBCSenderSockets.empty() == false );
}

bool VistaBroadcastClusterLeaderDataSync::DoSendMessage()
{
	std::vector<VistaUDPSocket*> vecDiedSocket;

	VistaType::sint32 nSize[2]; 
	nSize[0] = m_oMessage.GetBufferSize();
	if( m_pExtBuffer )
		nSize[1] = m_nExtBufferSize;
	else
		nSize[1] = 0;

	// we never swap bytes - leave that to the slaves
	for( std::vector<VistaUDPSocket*>::iterator itSocket = m_vecBCSenderSockets.begin();
			itSocket != m_vecBCSenderSockets.end(); ++itSocket )
	{
		try
		{
			int nReturn = (*itSocket)->SendRaw( nSize, 2 * sizeof(VistaType::sint32) );
			if( nReturn != 2 * sizeof(VistaType::sint32) )
			{
				if( GetIsVerbose() )
				{
					vstr::warnp() << "[VistaBroadcastClusterLeaderSync]: "
								<< "Writing data to broadcast socket"
								<< " failed - removing it from sync list" << std::endl;
				}
				vecDiedSocket.push_back( (*itSocket) );
				continue;
			}

			int nAccumSize = 0;
			while( nAccumSize < nSize[0] )
			{
				int nSendSize = std::min( S_nMaxPackageSize, nSize[0] - nAccumSize );
				nReturn = (*itSocket)->SendRaw( &m_oMessage.GetBuffer()[nAccumSize], nSendSize );
				if( nReturn != nSendSize )
				{
					if( GetIsVerbose() )
					{
						vstr::warnp() << "[VistaBroadcastClusterLeaderSync]: "
									<< "Writing data to broadcast socket"
									<< " failed - removing it from sync list" << std::endl;
					}
					vecDiedSocket.push_back( (*itSocket) );
					continue;
				}
				nAccumSize += nReturn;
			}
			
			nAccumSize = 0;
			while( nAccumSize < nSize[1] )
			{
				int nSendSize = std::min( S_nMaxPackageSize, nSize[1] - nAccumSize );
				nReturn = (*itSocket)->SendRaw( &m_oMessage.GetBuffer()[nAccumSize], nSendSize );
				if( nReturn != nSendSize )
				{
					if( GetIsVerbose() )
					{
						vstr::warnp() << "[VistaBroadcastClusterLeaderSync]: "
									<< "Writing data to broadcast socket"
									<< " failed - removing it from sync list" << std::endl;
					}
					vecDiedSocket.push_back( (*itSocket) );
					continue;
				}
				nAccumSize += nReturn;
			}
		}
		catch( VistaExceptionBase& )
		{
			if( GetIsVerbose() )
			{
				vstr::warnp() << "[VistaBroadcastClusterLeaderSync]: "
								<< "Exception while writing data to broadcast socket"
								<< " - removing it from sync list" << std::endl;
			}
			vecDiedSocket.push_back( (*itSocket) );
			continue;
		}

		VistaTimeUtils::Sleep(1);
	}

	// kill dead sockets
	for( std::vector<VistaUDPSocket*>::iterator itDied = vecDiedSocket.begin();
			itDied != vecDiedSocket.end(); ++itDied )
	{
		RemoveSocket( (*itDied) );
	}

	return GetIsValid();
}

std::string VistaBroadcastClusterLeaderDataSync::GetDataSyncType() const
{
	return "BroadcastLeaderSync";
}

bool VistaBroadcastClusterLeaderDataSync::RemoveSocket( VistaUDPSocket* pDeadSocket )
{
	std::vector<VistaUDPSocket*>::iterator itSocket = m_vecBCSenderSockets.begin();
	std::vector<bool>::iterator itDel = m_vecDeleteBroadcastSocket.begin();
	for( ; itSocket != m_vecBCSenderSockets.end(); ++itSocket, ++itDel )
	{
		if( pDeadSocket == (*itSocket ) )
		{
			if( (*itDel) )
				delete (*itSocket );
			m_vecBCSenderSockets.erase( itSocket );
			m_vecDeleteBroadcastSocket.erase( itDel );
			return true;
		}
	}
	return false;
}

bool VistaBroadcastClusterLeaderDataSync::AddBroadcast( const std::string& sBroadcastIP,
													  const int nBroadcastPort )
{
	VistaUDPSocket* pSocket = new VistaUDPSocket;
	VistaSocketAddress oAddress( sBroadcastIP, nBroadcastPort );

	if( (*pSocket).OpenSocket() == false )
	{
		if( GetIsVerbose() )
		{
			vstr::errp() << "[" << GetDataSyncType() << "]: "
						<< "Could not open BroadcastSocket - syncing on it will not work." << std::endl;
		}
		delete pSocket;
		return false;
	}

	if( oAddress.GetIsValid() )
	{
		if( GetIsVerbose() )
		{
			vstr::errp() << "[" << GetDataSyncType() << "]: "
					<< "BroadcastAddress [" << sBroadcastIP << ":" << nBroadcastPort
					<< " is invalid - syncing on it will not work." << std::endl;
		}
		delete pSocket;
		return false;
	}

	pSocket->SetPermitBroadcast(1);	
	pSocket->ConnectToAddress( oAddress );

	if( pSocket->GetIsConnected() == false )
	{
		if( GetIsVerbose() )
		{
			vstr::errp() << "[" << GetDataSyncType() << "]: "
					<< "Connection to [" << sBroadcastIP << ":" << nBroadcastPort
					<< " failed - syncing on it will not work" << std::endl;
		}
		return false;
	}

	m_vecBCSenderSockets.push_back( pSocket );
	m_vecDeleteBroadcastSocket.push_back( true );
	return true;
}

bool VistaBroadcastClusterLeaderDataSync::AddBroadcast( VistaUDPSocket* pBroadcastSocket,
														const bool bManageDeletion )
{
	if( pBroadcastSocket->GetIsConnected() == false )
	{
		
		if( GetIsVerbose() )
		{
			vstr::warnp() << "[" << GetDataSyncType() << "]: "
					<< "Received BroadcastSocket is not connected - syncing on it will not work" << std::endl;
		}
		return false;
	}

	pBroadcastSocket->SetPermitBroadcast(1);
	
	pBroadcastSocket->SetIsBuffering( false );
	pBroadcastSocket->SetIsBlocking( false );

	m_vecBCSenderSockets.push_back( pBroadcastSocket );
	m_vecDeleteBroadcastSocket.push_back( bManageDeletion );
	return true;
}

bool VistaBroadcastClusterLeaderDataSync::SetSendBlockingThreshold( const int nNumBytes )
{
	return false;
}

int VistaBroadcastClusterLeaderDataSync::GetSendBlockingThreshold() const
{
	return -1;
}

bool VistaBroadcastClusterLeaderDataSync::WaitForConnection( const std::vector<VistaConnectionIP*>& vecConnections )
{
	for( std::vector<VistaConnectionIP*>::const_iterator itConn = vecConnections.begin();
		itConn != vecConnections.end(); ++itConn )
	{
		(*itConn)->SetIsBlocking( true );
		assert( (*itConn)->PendingDataSize() == 0 );
	}
	
	std::vector<bool> vecConnectionSuccess( vecConnections.size(), false );
	VistaType::sint32 nCount = 0;
	bool bAllConnected = false;
	bool bReadValue;
	while( bAllConnected == false ) 
	{
		bAllConnected = true;
		
		for( std::vector<VistaUDPSocket*>::iterator itSocket = m_vecBCSenderSockets.begin();
			itSocket != m_vecBCSenderSockets.end(); ++itSocket )
		{
			int nReturn = (*itSocket)->SendRaw( &nCount, sizeof(VistaType::sint32) );
			if( nReturn != sizeof(VistaType::sint32) )
				VISTA_THROW( "BroadcastLeaderDataSync WaitForConnection fail", -1 );				
		}

		++nCount;
		VistaTimeUtils::Sleep( 1 );
		for( std::size_t nIndex = 0; nIndex < vecConnections.size(); ++nIndex )
		{
			if( vecConnectionSuccess[nIndex] )
				continue;
			if( vecConnections[nIndex]->HasPendingData() )
			{
				vecConnections[nIndex]->ReadBool( bReadValue );
				vecConnectionSuccess[nIndex] = true;
				assert( bReadValue );
			}
			else
			{
				bAllConnected = false;
			}				
		}
	}
	// send out finish message
	nCount = -1;
	for( std::vector<VistaUDPSocket*>::iterator itSocket = m_vecBCSenderSockets.begin();
			itSocket != m_vecBCSenderSockets.end(); ++itSocket )
	{
		int nReturn = (*itSocket)->SendRaw( &nCount, sizeof(VistaType::sint32) );
		if( nReturn != sizeof(VistaType::sint32) )
			VISTA_THROW( "BroadcastLeaderDataSync WaitForConnection fail", -1 );				
	}

	return true;
}


/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaBroadcastClusterFollowerDataSync::VistaBroadcastClusterFollowerDataSync(
									const std::string& sBroadcastAddress,
									const int nBroadcastPort,
									const bool bSwap,
									const bool bVerbose )
: VistaClusterBytebufferFollowerDataSyncBase( bSwap, bVerbose )
, m_bManageDeletion( true )
{
	m_pBCReceiverSocket = new VistaUDPSocket;

	VistaSocketAddress oAdress( sBroadcastAddress, nBroadcastPort );

	if( oAdress.GetIsValid() == false )
	{
		vstr::errp() << "[" << GetDataSyncType() << "]: "
					<< "Provided address [" << sBroadcastAddress << ":" << nBroadcastPort
					<< "] is invalid" << std::endl;
		delete m_pBCReceiverSocket;
		m_pBCReceiverSocket = NULL;
	}
	else if( m_pBCReceiverSocket->OpenSocket() == false )
	{
		vstr::errp() << "[" << GetDataSyncType() << "]: "
					<< "Could not open swap sync socket - syncing will not work." << std::endl;		
		delete m_pBCReceiverSocket;
		m_pBCReceiverSocket = NULL;
	}
	else
	{
		m_pBCReceiverSocket->SetPermitBroadcast(1);
		m_pBCReceiverSocket->ConnectToAddress( oAdress );
		if( m_pBCReceiverSocket->GetIsConnected() == false )
		{
			vstr::errp() << "[" << GetDataSyncType() << "]: "
						<< "Connection to [" << sBroadcastAddress << ":" << nBroadcastPort
						<< " failed - syncing will not work" << std::endl;
			delete m_pBCReceiverSocket;
			m_pBCReceiverSocket = NULL;
		}
		else
		{
			m_pBCReceiverSocket->SetIsBuffering( false );
		}
	}
}

VistaBroadcastClusterFollowerDataSync::VistaBroadcastClusterFollowerDataSync(
									VistaUDPSocket* pBroadcastSocket,
									const bool bSwap,
									const bool bManageDeletion,
									const bool bVerbose )
: VistaClusterBytebufferFollowerDataSyncBase( bSwap, bVerbose )
, m_bManageDeletion( true )
, m_pBCReceiverSocket( pBroadcastSocket )
{
	if( m_pBCReceiverSocket == NULL || m_pBCReceiverSocket->GetIsBoundToAddress() == false )
	{
		if( GetIsVerbose() )
		{
			vstr::warnp() << "[" << GetDataSyncType() << "]: "
					<< "Received socket is not connected - follower will "
					<< "report to leader, but not wait for go token" << std::endl;
		}
		if( m_bManageDeletion )
			delete m_pBCReceiverSocket;
		m_pBCReceiverSocket = NULL;
	}
	else
		m_pBCReceiverSocket->SetPermitBroadcast(1);
}


VistaBroadcastClusterFollowerDataSync::~VistaBroadcastClusterFollowerDataSync()
{
	if( m_bManageDeletion )
		delete m_pBCReceiverSocket;
}


bool VistaBroadcastClusterFollowerDataSync::GetIsValid() const
{	
	return ( m_pBCReceiverSocket != NULL );
}

bool VistaBroadcastClusterFollowerDataSync::DoReceiveMessage()
{
	if( GetIsValid() == false )
		return false;
	try
	{
		m_pBCReceiverSocket->SetIsBlocking( true );
		VistaType::sint32 nMessageSize[2];
		int nRet = m_pBCReceiverSocket->ReceiveRaw( nMessageSize, 2 * sizeof(VistaType::sint32) );
		if( nRet != 2 * sizeof(VistaType::sint32) )
		{
			if( GetIsVerbose() )
			{
				vstr::warnp() << "[VistaBroadcastClusterFollowerSync]: "
					<< "Error receiving message from leader" << std::endl;
			}
			ProcessError();
			return false;
		}

		if( m_bSwap )
		{
			VistaSerializingToolset::Swap( nMessageSize, sizeof(VistaType::sint32), 2 );
		}

		if( (int)m_vecMessageBuffer.size() < nMessageSize[0] + nMessageSize[1] )
			m_vecMessageBuffer.resize( nMessageSize[0] + nMessageSize[1] );

		// message might come in several chunks
		int nReadBytes = 0;
		while( nReadBytes < nMessageSize[0] )
		{
			int nReadSize = std::min( S_nMaxPackageSize, nMessageSize[0] - nReadBytes );
			nRet = m_pBCReceiverSocket->ReceiveRaw( &m_vecMessageBuffer[nReadBytes], nReadSize );
			if( nRet <= 0 )
			{
				if( GetIsVerbose() )
				{
					vstr::warnp() << "[VistaBroadcastClusterFollowerSync]: "
						<< "Error receiving message from leader" << std::endl;
				}
				ProcessError();
				return false;
			}
			nReadBytes += nRet;
		}
		int nReadExtBytes = 0;
		while( nReadExtBytes < nMessageSize[1] )
		{
			int nReadSize = std::min( S_nMaxPackageSize, nMessageSize[1] - nReadExtBytes );
			nRet = m_pBCReceiverSocket->ReceiveRaw( &m_vecMessageBuffer[nReadBytes + nReadExtBytes], nReadSize );
			if( nRet <= 0 )
			{
				if( GetIsVerbose() )
				{
					vstr::warnp() << "[VistaBroadcastClusterFollowerSync]: "
						<< "Error receiving message from leader" << std::endl;
				}
				ProcessError();
				return false;
			}
			nReadExtBytes += nRet;
		}

		m_oMessage.SetBuffer( &m_vecMessageBuffer[0], nMessageSize[0] + nMessageSize[1] );
		// read off inital size (we used our own size in a separate package )
		m_oMessage.ReadInt32( nMessageSize[0] );
	}
	catch( VistaExceptionBase& )
	{
		if( GetIsVerbose() )
		{
			vstr::warnp() << "[VistaBroadcastClusterFollowerSync]: "
					<< "Exception while receiving message from leader" << std::endl;
		}
		ProcessError();
		return false;
	}

	return true;
}

std::string VistaBroadcastClusterFollowerDataSync::GetDataSyncType() const
{
	return "BroadcastFollowerSync";
}

void VistaBroadcastClusterFollowerDataSync::ProcessError()
{
	if( m_bManageDeletion )
		delete m_pBCReceiverSocket;
	m_pBCReceiverSocket = NULL;
	Notify( MSG_FATAL_ERROR );
}

bool VistaBroadcastClusterFollowerDataSync::SetSendBlockingThreshold( const int nNumBytes )
{
	return false;
}

int VistaBroadcastClusterFollowerDataSync::GetSendBlockingThreshold() const
{
	return -1;
}
	
bool VistaBroadcastClusterFollowerDataSync::WaitForConnection( VistaConnectionIP* pConnection )
{
	pConnection->SetIsBlocking( true );
	pConnection->SetIsBuffering( false );
	
	VistaType::sint32 nReadValue;

	if( m_pBCReceiverSocket->ReceiveRaw( &nReadValue, sizeof(VistaType::sint32) ) != sizeof(VistaType::sint32) )
	{
		VISTA_THROW( "BroadcastFollowerDataSync WaitForConnection error", -1 );
	}
	
	pConnection->WriteBool( true );

	// read of all remaining messages
	do 
	{
		m_pBCReceiverSocket->ReceiveRaw( &nReadValue, sizeof(VistaType::sint32) );
		if( m_bSwap )
			VistaSerializingToolset::Swap( &nReadValue, sizeof(VistaType::sint32) );
	} while ( nReadValue != -1 );
	return true;
}
