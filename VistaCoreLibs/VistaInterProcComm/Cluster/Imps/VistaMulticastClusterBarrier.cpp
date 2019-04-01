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


#include "VistaMulticastClusterBarrier.h"

#include <VistaInterProcComm/IPNet/VistaMcastSocket.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaSocketAddress.h>
#include <VistaInterProcComm/IPNet/VistaIPAddress.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaMulticastClusterLeaderBarrier::VistaMulticastClusterLeaderBarrier( 
												const std::string& sOwnIP,
												const std::string& sMulticastIP,
												const int nMulticastPort,
												const bool bVerbose )
: VistaClusterLeaderBarrierIPBase( bVerbose )
, m_pMulticastSocket( new VistaMcastSocket )
, m_bDeleteMulticastSocket( true )
{
	m_pMulticastSocket->OpenSocket();
	if( m_pMulticastSocket->ActiveMulticast( 0, sOwnIP, sMulticastIP, nMulticastPort, 1, false ) )
	{
		vstr::warnp() << "[" << GetBarrierType() << "]: "
					<< "could not set up sender to multicast group [" << sMulticastIP
					<< ":" << nMulticastPort << "]" << std::endl;
		delete m_pMulticastSocket;
		m_pMulticastSocket = NULL;
	}
}

VistaMulticastClusterLeaderBarrier::VistaMulticastClusterLeaderBarrier(
												VistaMcastSocket* pMulticastSocket,
												const bool bManageSocketDeletion,
												const bool bVerbose )
: VistaClusterLeaderBarrierIPBase( bVerbose )
{
	if( m_pMulticastSocket->GetIsOpen() == false
		|| m_pMulticastSocket->GetIsSetMulticast() == false )
	{
		vstr::warnp() << "[" << GetBarrierType() << "]: "
					<< "received invalid multicast socket" << std::endl;
		delete m_pMulticastSocket;
		m_pMulticastSocket = NULL;
	}
}



VistaMulticastClusterLeaderBarrier::~VistaMulticastClusterLeaderBarrier()
{
	if( m_bDeleteMulticastSocket )
	{
		m_pMulticastSocket->WaitForSendFinish( 5 );
		delete m_pMulticastSocket;
	}
}

bool VistaMulticastClusterLeaderBarrier::BarrierWait( int iTimeOut )
{
	if( GetIsValid() == false )
		return false;

	if( WaitForAllFollowers( iTimeOut ) == false )
		return false;	

	// send out go token to every follower
	try
	{
		VistaType::sint32 nCount = GetBarrierWaitCount();
		int nRes = m_pMulticastSocket->SendtoMulticast( &nCount, sizeof(VistaType::sint32) );
		if( nRes != sizeof(VistaType::sint32) )
		{
			vstr::warnp() << "[" << GetBarrierType() << "]: "
				<< "Sending to Multicast group failed!" << std::endl;
			// @TODO: cleanup
			return false;
		}
	}
	catch( VistaExceptionBase& )
	{
		vstr::warnp() << "[" << GetBarrierType() << "]: "
				<< "Sending to Multicast group failed!" << std::endl;
		// @TODO: cleanup
		return false;
	}			

	return true;
}

bool VistaMulticastClusterLeaderBarrier::GetIsValid() const
{
	// we are valid as long as we have at least one valid slave and one valid Multicast socket
	if( VistaClusterLeaderBarrierIPBase::GetIsValid() == false )
		return false;
	if( m_pMulticastSocket == NULL )
		return false;
	return true;
}


std::string VistaMulticastClusterLeaderBarrier::GetBarrierType() const
{
	return "MulticastLeaderBarrier";
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaMulticastClusterFollowerBarrier::VistaMulticastClusterFollowerBarrier( 
												const std::string& sMulticastIP,
												const int nMulticastPort,
												VistaConnectionIP* pLeaderConnection,
												const bool bManageConnection,
												const bool bVerbose )
: VistaClusterFollowerBarrierIPBase( pLeaderConnection, bManageConnection, bVerbose )
, m_bManageSocket( true )
{
	m_pMulticastSocket = new VistaMcastSocket;

	std::string sOwnIP = pLeaderConnection->GetLocalAddress().GetIPAddress().GetIPAddressC();	

	m_pMulticastSocket->OpenSocket();
	if( m_pMulticastSocket->ActiveMulticast( 1, sOwnIP, sMulticastIP, nMulticastPort ) == false )
	{
		vstr::warnp() << "[" << GetBarrierType() << "]: "
					<< "setting up receiver socket for multicast group [" << sMulticastIP
					<< ":" << nMulticastPort << "]" << std::endl;
		delete m_pMulticastSocket;
		m_pMulticastSocket = NULL;
	}	
}

VistaMulticastClusterFollowerBarrier::VistaMulticastClusterFollowerBarrier(
												VistaMcastSocket* pMulticastSocket,
												VistaConnectionIP* pLeaderConn,
												const bool bManageSocket,
												const bool bManageConnection,
												const bool bVerbose )
: VistaClusterFollowerBarrierIPBase( pLeaderConn, bManageConnection, bVerbose )
, m_nBarrierCount( 0 )
, m_pMulticastSocket( pMulticastSocket )
, m_bManageSocket( bManageSocket )
{	
	if( m_pMulticastSocket->GetIsOpen() == false
		|| m_pMulticastSocket->GetIsJoinedMulticast() == false )
	{
		vstr::warnp() << "[" << GetBarrierType() << "]: "
					<< "received invalid multicast socket" << std::endl;
		delete m_pMulticastSocket;
		m_pMulticastSocket = NULL;
	}
}


VistaMulticastClusterFollowerBarrier::~VistaMulticastClusterFollowerBarrier()
{
	if( m_bManageSocket )
		delete m_pMulticastSocket;
}


bool VistaMulticastClusterFollowerBarrier::GetIsValid() const
{
	return ( VistaClusterFollowerBarrierIPBase::GetIsValid() && m_pLeaderConn != NULL );
}

bool VistaMulticastClusterFollowerBarrier::BarrierWait( int iTimeOut )
{
	if( GetIsValid() == false )
		return false;

	if( VistaClusterFollowerBarrierIPBase::SendReadyTokenToLeader( iTimeOut ) == false )
		return false;

	VistaType::sint32 nReceive;

	try
	{
		int nRet = m_pMulticastSocket->ReceivefromMulticast( &nReceive, sizeof(VistaType::sint32), iTimeOut );
		if( nRet == 0 )
		{
			if( GetIsVerbose() )
			{
				vstr::warnp() << "[" << GetBarrierType() << "]: "
						<< "Timeout while waiting for Multicast token" << std::endl;
			}
		}
		else if( nRet != sizeof(VistaType::sint32) )
		{
			if( GetIsVerbose() )
			{
				vstr::warnp() << "[" << GetBarrierType() << "]: "
						<< "Could not receive go token from leader" << std::endl;
			}
			ProcessError();
			return false;
		}			

		if( m_bSwap )
			VistaSerializingToolset::Swap( &nReceive, sizeof(VistaType::sint32) );

		if( nReceive != m_nBarrierCount )
		{
			if( GetIsVerbose() )
			{
				vstr::warnp() << "[" << GetBarrierType() << "]: "
						<< "Received BarrierCounter is [" << nReceive <<"], but expected ["
						<< m_nBarrierCount << "]" << std::endl;
			}
			m_nBarrierCount = nReceive;
		}
	}
	catch( VistaExceptionBase& )
	{
		if( GetIsVerbose() )
		{
			vstr::errp() << "[" << GetBarrierType() << "]: "
						<< "Exception during BarrierWait" << std::endl;
		}
		ProcessError();
		return false;
	}	

	return true;
}

std::string VistaMulticastClusterFollowerBarrier::GetBarrierType() const
{
	return "MulticastFollowerBarrier";
}
