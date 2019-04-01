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


#include "VistaClusterBytebufferDataSyncBase.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaAspects/VistaPropertyList.h>

#include <cassert>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

enum
{
	SYNC_TIME = 42030,
	SYNC_PROPLIST,
	SYNC_SERIALIZABLE,
	SYNC_FIXEDSIZE_DATA,
	SYNC_VARSIZE_DATA,
	SYNC_STRING,
};

#define VERIFY_READ( readcall ) \
{ \
	if( readcall <= 0 ) \
		VISTA_THROW( "TCPIPCLusterDataSync Protocol Error", -1 ); \
}
#define VERIFY_READ_SIZE( readcall, size ) \
{ \
	if( readcall != size ) \
		VISTA_THROW( "TCPIPCLusterDataSync Protocol Error", -1 ); \
}

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaClusterBytebufferLeaderDataSyncBase::VistaClusterBytebufferLeaderDataSyncBase(
												const bool bVerbose )
: IVistaClusterDataSync( bVerbose, true )
, m_nSyncCount( 0 )
, m_pExtBuffer( NULL )
{
	m_oMessage.SetByteorderSwapFlag( VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES ); // we never swap
	m_oMessage.WriteInt32( 0 ); // dummy for size
	m_oMessage.WriteUInt64( (VistaType::sint32)m_nSyncCount );	
}

VistaClusterBytebufferLeaderDataSyncBase::~VistaClusterBytebufferLeaderDataSyncBase()
{
}

bool VistaClusterBytebufferLeaderDataSyncBase::SyncTime( VistaType::systemtime& nTime )
{
	m_oMessage.WriteInt32( SYNC_TIME );
	m_oMessage.WriteDouble( nTime );
	return SendMessage();
}

bool VistaClusterBytebufferLeaderDataSyncBase::SyncData( VistaPropertyList& oList )
{
	m_oMessage.WriteInt32( SYNC_PROPLIST );
	/* int nRet = */ VistaPropertyList::SerializePropertyList( m_oMessage, oList, "" );
	//assert( nRet == m_oMessage.GetBufferSize() - 3*sizeof(VistaType::sint32) );
	return SendMessage();
}

bool VistaClusterBytebufferLeaderDataSyncBase::SyncData( IVistaSerializable& oSerializable )
{
	m_oMessage.WriteInt32( SYNC_SERIALIZABLE );
	/* int nRet = */ m_oMessage.WriteSerializable( oSerializable );
	//assert( nRet == m_oMessage.GetBufferSize() - 3*sizeof(VistaType::sint32) );
	return SendMessage();
}

bool VistaClusterBytebufferLeaderDataSyncBase::SyncData( VistaType::byte* pData, const int iDataSize )
{
	m_oMessage.WriteInt32( SYNC_FIXEDSIZE_DATA );
	m_oMessage.WriteInt32( (VistaType::sint32)iDataSize );
	m_pExtBuffer = pData;
	m_nExtBufferSize = iDataSize;
	return SendMessage();
}

bool VistaClusterBytebufferLeaderDataSyncBase::SyncData( VistaType::byte* pDataBuffer,
										   const int iBufferSize, int& iDataSize )
{
	m_oMessage.WriteInt32( SYNC_VARSIZE_DATA );
	m_oMessage.WriteInt32( (VistaType::sint32)iDataSize );
	m_pExtBuffer = pDataBuffer;
	m_nExtBufferSize = iDataSize;
	if( SendMessage() == false )
		return false;
	return true;
}

bool VistaClusterBytebufferLeaderDataSyncBase::SyncData( std::vector<VistaType::byte>& vecData )
{
	m_oMessage.WriteInt32( SYNC_VARSIZE_DATA );
	m_oMessage.WriteInt32( (VistaType::sint32)vecData.size() );
	m_pExtBuffer = &vecData[0];
	m_nExtBufferSize = (VistaType::sint32)vecData.size();
	if( SendMessage() == false )
		return false;
	return true;
}

bool VistaClusterBytebufferLeaderDataSyncBase::SyncData( std::string& sData )
{
	m_oMessage.WriteInt32( SYNC_STRING );
	m_oMessage.WriteInt32( (VistaType::sint32)sData.size() );
	m_pExtBuffer = reinterpret_cast<VistaType::byte*>( &sData[0] );
	m_nExtBufferSize = (VistaType::sint32)sData.size();
	if( SendMessage() == false )
		return false;
	return true;
}

bool VistaClusterBytebufferLeaderDataSyncBase::SendMessage()
{
	// rewrite message size
	VistaType::sint32 nSize = m_oMessage.GetBufferSize() - sizeof(VistaType::sint32);
	if( m_pExtBuffer )
	{
		if( m_nExtBufferSize == 0 )
			m_pExtBuffer = NULL;
		else
			nSize += m_nExtBufferSize;
	}
	// rewrite first dummy bite with size
	m_oMessage.SetBufferRewritePosition( 0 );
	m_oMessage.WriteInt32( nSize );

	bool bRet = DoSendMessage();

	++m_nSyncCount;

	// prepare message for next send
	m_oMessage.ClearBuffer();
	m_oMessage.WriteInt32( 0 ); // dummy for size
	m_oMessage.WriteUInt64( m_nSyncCount );
	m_pExtBuffer = NULL;

	return bRet;
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaClusterBytebufferFollowerDataSyncBase::VistaClusterBytebufferFollowerDataSyncBase(
												const bool bSwap,
												const bool bVerbose )
: IVistaClusterDataSync( bVerbose, false )
, m_nSyncCount( 0 )
, m_bSwap( bSwap )
{
}


VistaClusterBytebufferFollowerDataSyncBase::~VistaClusterBytebufferFollowerDataSyncBase()
{
}

bool VistaClusterBytebufferFollowerDataSyncBase::SyncTime( VistaType::systemtime& nTime )
{
	if( ReceiveMessage( SYNC_TIME ) == false )
		return false;

	VERIFY_READ_SIZE( m_oMessage.ReadDouble( nTime ), sizeof(double) );
	return true;
}

bool VistaClusterBytebufferFollowerDataSyncBase::SyncData( VistaPropertyList& oList )
{
	if( ReceiveMessage( SYNC_PROPLIST ) == false )
		return false;

	std::string sDummy;
	VistaPropertyList::DeSerializePropertyList( m_oMessage, oList, sDummy );
	return true;
}

bool VistaClusterBytebufferFollowerDataSyncBase::SyncData( IVistaSerializable& oSerializable )
{
	if( ReceiveMessage( SYNC_SERIALIZABLE ) == false )
		return false;

	VERIFY_READ( m_oMessage.ReadSerializable( oSerializable ) );
	return true;
}

bool VistaClusterBytebufferFollowerDataSyncBase::SyncData( VistaType::byte* pData, const int iDataSize )
{
	if( ReceiveMessage( SYNC_FIXEDSIZE_DATA ) == false )
		return false;

	VistaType::sint32 nReceivedDataSize;
	VERIFY_READ_SIZE( m_oMessage.ReadInt32( nReceivedDataSize ), sizeof(VistaType::sint32) )
	assert( nReceivedDataSize == iDataSize );
	if( iDataSize != 0 )
		VERIFY_READ_SIZE( m_oMessage.ReadRawBuffer( pData, iDataSize ), iDataSize )
	return true;
}

bool VistaClusterBytebufferFollowerDataSyncBase::SyncData( VistaType::byte* pDataBuffer, const int iBufferSize, int& iDataSize )
{
	if( ReceiveMessage( SYNC_VARSIZE_DATA ) == false )
		return false;

	VistaType::sint32 nReceivedDataSize;
	VERIFY_READ_SIZE( m_oMessage.ReadInt32( nReceivedDataSize ), sizeof(VistaType::sint32) )
	if( iBufferSize < nReceivedDataSize )
		return false;
	if( nReceivedDataSize != 0 )
		VERIFY_READ_SIZE( m_oMessage.ReadRawBuffer( pDataBuffer, nReceivedDataSize ), nReceivedDataSize );
	iDataSize = nReceivedDataSize;
	return true;
}

bool VistaClusterBytebufferFollowerDataSyncBase::SyncData( std::vector<VistaType::byte>& vecData )
{
	if( ReceiveMessage( SYNC_VARSIZE_DATA ) == false )
		return false;

	VistaType::sint32 nReceivedDataSize;
	VERIFY_READ_SIZE( m_oMessage.ReadInt32( nReceivedDataSize ), sizeof(VistaType::sint32) )
	vecData.resize( nReceivedDataSize );
	if( nReceivedDataSize != 0 )
		VERIFY_READ_SIZE( m_oMessage.ReadRawBuffer( &vecData[0], nReceivedDataSize ), nReceivedDataSize );
	return true;
}

bool VistaClusterBytebufferFollowerDataSyncBase::SyncData( std::string& sData )
{
	if( ReceiveMessage( SYNC_STRING ) == false )
		return false;

	VistaType::sint32 nReceivedDataSize;
	VERIFY_READ_SIZE( m_oMessage.ReadInt32( nReceivedDataSize ), sizeof(VistaType::sint32) )
	sData.resize( nReceivedDataSize );
	if( nReceivedDataSize != 0 )
		VERIFY_READ_SIZE( m_oMessage.ReadRawBuffer( &sData[0], nReceivedDataSize ), nReceivedDataSize );
	return true;
}

bool VistaClusterBytebufferFollowerDataSyncBase::ReceiveMessage( VistaType::sint32 nExpectedMessageType )
{
	m_oMessage.ClearBuffer();
	m_oMessage.SetByteorderSwapFlag( m_bSwap ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );
	if( GetIsValid() == false )
		return false;

	if( DoReceiveMessage() == false )
		return false;

	VistaType::uint64 nCounter = ~0;
	VERIFY_READ_SIZE( m_oMessage.ReadUInt64( nCounter ), sizeof(VistaType::uint64) )
	if( nCounter != m_nSyncCount )
	{
		VISTA_THROW( "ClusterSync Protocol Error - Message Counter does not match", -1 );
	}

	VistaType::sint32 nMessageType = -1;
	VERIFY_READ_SIZE( m_oMessage.ReadInt32( nMessageType ), sizeof(VistaType::sint32) )
	if( nMessageType != nExpectedMessageType )
	{
		VISTA_THROW( "ClusterSync Protocol Error - Message Types do not match", -1 );
	}

	++m_nSyncCount;
	return true;
}

const VistaType::byte* VistaClusterBytebufferFollowerDataSyncBase::GetMessageBuffer() const
{
	return &m_vecMessageBuffer[0];
}

