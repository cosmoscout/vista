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


#ifndef _VISTACLUSTERBYTEBUFFERDATASYNCBASE_H
#define _VISTACLUSTERBYTEBUFFERDATASYNCBASE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterDataSync.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>

#include <vector>
#include <string>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaClusterBytebufferLeaderDataSyncBase : public IVistaClusterDataSync
{
public:
	explicit VistaClusterBytebufferLeaderDataSyncBase( const bool bVerbose = true );
	virtual ~VistaClusterBytebufferLeaderDataSyncBase();

	virtual bool SyncTime( VistaType::systemtime& nTime );
	virtual bool SyncData( std::string& sData );
	virtual bool SyncData( VistaPropertyList& oList );
	virtual bool SyncData( IVistaSerializable& oSerializable );
	virtual bool SyncData( VistaType::byte* pData, 
							const int iDataSize );
	virtual bool SyncData( VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							int& iDataSize );
	virtual bool SyncData( std::vector<VistaType::byte>& vecData );
protected:
	bool SendMessage();
	virtual bool DoSendMessage() = 0;

protected:
	VistaType::uint64		m_nSyncCount;
	VistaByteBufferSerializer m_oMessage;
	const VistaType::byte*	m_pExtBuffer;
	VistaType::sint32		m_nExtBufferSize;
};

class VISTAINTERPROCCOMMAPI VistaClusterBytebufferFollowerDataSyncBase : public IVistaClusterDataSync
{
public:
	VistaClusterBytebufferFollowerDataSyncBase( const bool bSwap,
									const bool bVerbose = true );
	virtual ~VistaClusterBytebufferFollowerDataSyncBase();

	virtual bool SyncTime( VistaType::systemtime& nTime );
	virtual bool SyncData( std::string& sData );
	virtual bool SyncData( VistaPropertyList& oList );
	virtual bool SyncData( IVistaSerializable& oSerializable );
	virtual bool SyncData( VistaType::byte* pData, 
							const int iDataSize );
	virtual bool SyncData( VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							int& iDataSize );
	virtual bool SyncData( std::vector<VistaType::byte>& vecData );

	const VistaType::byte* GetMessageBuffer() const;
	int GetMessageSize() const;

protected:
	bool ReceiveMessage( VistaType::sint32 nExpectedMessageType );
	virtual bool DoReceiveMessage() = 0;

protected:
	VistaType::uint64	m_nSyncCount;
	bool				m_bSwap;
	std::vector<VistaType::byte>	m_vecMessageBuffer;
	VistaByteBufferDeSerializer		m_oMessage;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERBYTEBUFFERDATASYNCBASE_H
