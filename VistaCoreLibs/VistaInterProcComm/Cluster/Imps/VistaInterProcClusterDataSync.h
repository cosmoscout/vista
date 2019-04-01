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


#ifndef _VISTAINTERPROCCLUSTERDATASYNC_H
#define _VISTAINTERPROCCLUSTERDATASYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaClusterBytebufferDataSyncBase.h>

#include <vector>
#include <string>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaConnectionNamedPipe;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaInterProcClusterLeaderDataSync : public VistaClusterBytebufferLeaderDataSyncBase
{
public:
	VistaInterProcClusterLeaderDataSync( const bool bActAsInterProcServer = false,
										const bool bVerbose = true );

	virtual ~VistaInterProcClusterLeaderDataSync();

	bool AddInterProc( const std::string& sInterProcName );
	bool ConnectPipes();

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );

protected:
	virtual bool DoSendMessage();

private:
	bool m_bActAsInterProcServer;
	std::vector<VistaConnectionNamedPipe*> m_vecConnections;
};

class VISTAINTERPROCCOMMAPI VistaInterProcClusterFollowerDataSync : public VistaClusterBytebufferFollowerDataSyncBase
{
public:
	VistaInterProcClusterFollowerDataSync( const std::string& sInterProcName,
										const bool bActAsInterProcServer = false,
										const bool bVerbose = true );
	virtual ~VistaInterProcClusterFollowerDataSync();

	bool ConnectPipe();

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );

private:
	virtual bool DoReceiveMessage();

private:
	VistaConnectionNamedPipe* m_pConnection;
};

class VISTAINTERPROCCOMMAPI VistaInterProcClusterRedirectDataSync : public VistaInterProcClusterLeaderDataSync
{
public:
	VistaInterProcClusterRedirectDataSync( const bool bVerbose = true );
	VistaInterProcClusterRedirectDataSync( IVistaClusterDataSync* pOriginalClusterSync,
											const bool bVerbose = true );

	virtual ~VistaInterProcClusterRedirectDataSync();

	
	IVistaClusterDataSync* GetDependentDataSync() const;
	void SetDependentDataSync( IVistaClusterDataSync* pSync );

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

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

private:
	bool SendRedirectedMessage();

private:
	std::vector<VistaConnectionNamedPipe*> m_vecConnections;
	
	IVistaClusterDataSync*						m_pDependentDataSync;
	VistaClusterBytebufferFollowerDataSyncBase*	m_pDependentDataSyncMessage;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINTERPROCCLUSTERDATASYNC_H
