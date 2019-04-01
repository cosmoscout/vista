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


#include "VistaZeroMQClusterBarrier.h"

#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaAspects/VistaPropertyList.h>

#include <zmq.hpp>

#include <cassert>
#include "VistaZeroMQCommon.h"

#ifdef SendMessage
#undef SendMessage
#endif

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/


VistaZeroMQClusterLeaderBarrier::VistaZeroMQClusterLeaderBarrier( const std::string& sZeroMQAddress,
																const bool bVerbose )
: VistaClusterLeaderBarrierIPBase( bVerbose )
{
	VistaZeroMQCommon::RegisterZeroMQUser();

	try
	{	
		m_pSocket = new zmq::socket_t( VistaZeroMQCommon::GetContext(), ZMQ_PUB );
		m_pSocket->bind( sZeroMQAddress.c_str() );
	}
	catch( zmq::error_t& oZmqException )
	{
		vstr::warnp() << "[ZeroMQLeaderBarrier]: Could not create/bind zmq socket at address ["
						<< sZeroMQAddress << "] with exception:\n";
		vstr::warni() << oZmqException.what() << std::endl;
		delete m_pSocket;
		m_pSocket = NULL;
	}	
}

VistaZeroMQClusterLeaderBarrier::~VistaZeroMQClusterLeaderBarrier()
{
	delete m_pSocket;
	VistaZeroMQCommon::UnregisterZeroMQUser();
}

bool VistaZeroMQClusterLeaderBarrier::BarrierWait( int iTimeOut )
{
	if( VistaClusterLeaderBarrierIPBase::WaitForAllFollowers( iTimeOut ) == false )
		return false;

	// send out go token
	VistaType::sint32 nCount = GetBarrierWaitCount();
	zmq::message_t oZmqMsg( sizeof( VistaType::sint32 ) );
	memcpy( oZmqMsg.data(), &nCount, sizeof( VistaType::sint32 ) );
	for( ;; )
	{
		try
		{
			if( m_pSocket->send( oZmqMsg ) )
				break;
		}
		catch( zmq::error_t& oZmqException )
		{
			if( oZmqException.num() == EINTR ) // EINTR are okay, we just retry
				continue;
			vstr::warnp() << "[ZeroMQLeaderDataSync]: Sending ZeroMQ message failed with exception:\n";
			vstr::warni() << oZmqException.what() << std::endl;
			return false;
		}
	}
	return true;
}

bool VistaZeroMQClusterLeaderBarrier::GetIsValid() const
{
	return ( m_pSocket != NULL );
}

std::string VistaZeroMQClusterLeaderBarrier::GetBarrierType() const
{
	return "ZeroMQLeaderBarrier";
}

bool VistaZeroMQClusterLeaderBarrier::WaitForConnection(const std::vector<VistaConnectionIP*>& vecConnections)
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
	try
	{
		while( bAllConnected == false ) 
		{
			bAllConnected = true;
			zmq::message_t oMessage( sizeof(VistaType::sint32) );
			memcpy( oMessage.data(), &nCount, sizeof(VistaType::sint32) );
			for( ;; )
			{
				try
				{
					if( m_pSocket->send( oMessage ) )
						break;
				}
				catch( zmq::error_t& oZmqException )
				{
					if( oZmqException.num() == EINTR ) // EINTR are okay, we just retry
						continue;
					vstr::warnp() << "[ZeroMQLeaderBarrier]: Sending ZeroMQ message failed with exception:\n";
					vstr::warni() << oZmqException.what() << std::endl;
					return false;
				}
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
		zmq::message_t oMessage( sizeof(VistaType::sint32) );
		memcpy( oMessage.data(), &nCount, sizeof(VistaType::sint32) );
		for( ;; )
		{
			try
			{
				if( m_pSocket->send( oMessage ) )
					break;
			}
			catch( zmq::error_t& oZmqException )
			{
				if( oZmqException.num() == EINTR ) // EINTR are okay, we just retry
					continue;
				vstr::warnp() << "[ZeroMQLeaderBarrier]: Sending ZeroMQ message failed with exception:\n";
				vstr::warni() << oZmqException.what() << std::endl;
				return false;
			}
		}
	}
	catch( zmq::error_t& oZmqException )
	{
		vstr::warnp() << "[ZeroMQLeaderBarrier]: WaitForConnection failed:\n";
		vstr::warni() << oZmqException.what() << std::endl;
		return false;
	}	

	return true;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaZeroMQClusterFollowerBarrier::VistaZeroMQClusterFollowerBarrier( const std::string& sZeroMQAddress,
																VistaConnectionIP* pLeaderConnection,
																const bool bManageConnection, 
																const bool bVerbose )
: VistaClusterFollowerBarrierIPBase( pLeaderConnection, bManageConnection, bVerbose )
{
	VistaZeroMQCommon::RegisterZeroMQUser();

	try
	{
		m_pZmqMessage = new zmq::message_t;
		m_pSocket = new zmq::socket_t( VistaZeroMQCommon::GetContext(), ZMQ_SUB );
		m_pSocket->connect( sZeroMQAddress.c_str() );
		m_pSocket->setsockopt( ZMQ_SUBSCRIBE, "", 0 );
	}
	catch( zmq::error_t& oZmqException )
	{
		vstr::warnp() << "[ZeroMQFollowerDataSync]: Could not create/connect zmq socket to address ["
				<< sZeroMQAddress << "] with exception:\n";
		vstr::warni() << oZmqException.what() << std::endl;
		delete m_pSocket;
		m_pSocket = NULL;
	}
}
VistaZeroMQClusterFollowerBarrier::~VistaZeroMQClusterFollowerBarrier()
{
	delete m_pSocket;
	VistaZeroMQCommon::UnregisterZeroMQUser();
}

bool VistaZeroMQClusterFollowerBarrier::BarrierWait( int iTimeOut )
{
	if( VistaClusterFollowerBarrierIPBase::SendReadyTokenToLeader( iTimeOut ) == false )
		return false;

	for( ;; )
	{
		try
		{
			if( m_pSocket->recv( m_pZmqMessage ) )
				break;
		}
		catch( zmq::error_t& oZmqException )
		{
			if( oZmqException.num() == EINTR ) // EINTR are okay, we just retry
				continue;
			vstr::warnp() << "[ZeroMQLeaderDataSync]: receiving ZeroMQ message failed with exception:\n";
			vstr::warni() << oZmqException.what() << std::endl;
			return false;
		}
	}

	VistaType::sint32 nCount;
	memcpy( &nCount, m_pZmqMessage->data(), sizeof(VistaType::sint32) );

	if( GetDoesSwap() )
		VistaSerializingToolset::Swap( &nCount, sizeof(VistaType::sint32) );

	if( nCount != GetBarrierWaitCount() )
	{
		if( GetIsVerbose() )
		{
			vstr::warnp() << "[" << GetBarrierType() << "]: "
						<< "Received BarrierCounter is [" << nCount <<"], but expected ["
						<< GetBarrierWaitCount() << "]" << std::endl;
		}
		SetBarrierWaitCount( nCount );
	}
	return true;	
}

bool VistaZeroMQClusterFollowerBarrier::GetIsValid() const
{
	return ( m_pSocket != NULL );
}

std::string VistaZeroMQClusterFollowerBarrier::GetBarrierType() const
{
	return "ZeroMQFollowerBarrier";
}

bool VistaZeroMQClusterFollowerBarrier::WaitForConnection( VistaConnectionIP* pConnection )
{
	pConnection->SetIsBlocking( true );
	pConnection->SetIsBuffering( false );

	zmq::message_t oMessage;
	for( ;; )
	{
		try
		{
			if( m_pSocket->recv( &oMessage ) )
				break;
		}
		catch( zmq::error_t& oZmqException )
		{
			if( oZmqException.num() == EINTR ) // EINTR are okay, we just retry
				continue;
			vstr::warnp() << "[ZeroMQLeaderDataSync]: receiving ZeroMQ message failed with exception:\n";
			vstr::warni() << oZmqException.what() << std::endl;
			return false;
		}
	}
	VistaType::sint32 nCount = 666;
	memcpy( &nCount, oMessage.data(), sizeof(VistaType::sint32) );
	pConnection->WriteBool( true );
	// read of all remaining messages
	VistaType::sint32 nRead;
	do 
	{
		for( ;; )
		{
			try
			{
				if( m_pSocket->recv( &oMessage ) )
					break;
			}
			catch( zmq::error_t& oZmqException )
			{
				if( oZmqException.num() == EINTR ) // EINTR are okay, we just retry
					continue;
				vstr::warnp() << "[ZeroMQLeaderDataSync]: receiving ZeroMQ message failed with exception:\n";
				vstr::warni() << oZmqException.what() << std::endl;
				return false;
			}
		}
		memcpy( &nRead, oMessage.data(), sizeof(VistaType::sint32) );
		if( GetDoesSwap() )
			VistaSerializingToolset::Swap( &nRead, sizeof(VistaType::sint32) );
	} while ( nRead != -1 );
	return true;
}
