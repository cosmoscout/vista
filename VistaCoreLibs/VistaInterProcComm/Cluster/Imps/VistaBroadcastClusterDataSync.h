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


#ifndef _VISTABROADCASTCLUSTERSYNC_H
#define _VISTABROADCASTCLUSTERSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaClusterBytebufferDataSyncBase.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>

#include <vector>
#include <string>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaUDPSocket;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * EXPERIMENTAL - use with care
 * This is a ClusterDataSync implementation that uses broadcasting to send
 * packages. However, tests showed that this only works for small packages
 * and with a low bytes/sec ratio, where packages/data was lost.
 * If possible, use the ZeroMq-Datasync (if available) or the 
 * TCP/IP DataSync (for small numbers of slaves)
 */

class VISTAINTERPROCCOMMAPI VistaBroadcastClusterLeaderDataSync : public VistaClusterBytebufferLeaderDataSyncBase
{
public:
	VistaBroadcastClusterLeaderDataSync( const bool bVerbose = true );
	VistaBroadcastClusterLeaderDataSync( const std::string& sBroadcastIP,
									const int nBroadcastPort,
									const bool bVerbose = true );
	VistaBroadcastClusterLeaderDataSync( VistaUDPSocket* pBroadcastSocket,
									const bool bManageDeletion = true,									
									const bool bVerbose = true );

	virtual ~VistaBroadcastClusterLeaderDataSync();

	bool WaitForConnection( const std::vector<VistaConnectionIP*>& vecConnections );
	
	bool AddBroadcast( const std::string& sBroadcastIP,
						const int nBroadcastPort );
	bool AddBroadcast( VistaUDPSocket* pBroadcastSocket,
						const bool bManageDeletion = true );


	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual bool SetSendBlockingThreshold( const int nNumBytes );
	virtual int GetSendBlockingThreshold() const;

private:
	virtual bool DoSendMessage();
	bool RemoveSocket( VistaUDPSocket* pDeadSocket );

private:
	std::vector<VistaUDPSocket*>	m_vecBCSenderSockets;
	std::vector<bool>				m_vecDeleteBroadcastSocket;
};

class VISTAINTERPROCCOMMAPI VistaBroadcastClusterFollowerDataSync : public VistaClusterBytebufferFollowerDataSyncBase
{
public:
	VistaBroadcastClusterFollowerDataSync( const std::string& sBroadcastAddress,
									const int nBroadcastPort,
									const bool bSwap,
									const bool bVerbose = true );
	VistaBroadcastClusterFollowerDataSync( VistaUDPSocket* pBroadcastSocket,
									const bool bSwap,
									const bool bManageDeletion = true,
									const bool bVerbose = true );
	virtual ~VistaBroadcastClusterFollowerDataSync();

	bool WaitForConnection( VistaConnectionIP* pConnection );

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual bool SetSendBlockingThreshold( const int nNumBytes );
	virtual int GetSendBlockingThreshold() const;

private:
	virtual bool DoReceiveMessage();
	void ProcessError();
private:
	VistaUDPSocket*		m_pBCReceiverSocket;
	bool				m_bManageDeletion;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTABROADCASTCLUSTERSYNC_H
