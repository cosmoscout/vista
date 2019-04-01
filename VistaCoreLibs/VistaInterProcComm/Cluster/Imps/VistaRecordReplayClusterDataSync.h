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


#ifndef _VISTARECORDREPLAYCLUSTERSYNC_H
#define _VISTARECORDREPLAYCLUSTERSYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaBase/VistaTimer.h>

#include <VistaInterProcComm/Cluster/VistaClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaClusterBytebufferDataSyncBase.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>

#include <vector>
#include <string>
#include <fstream>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaConnectionFile;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaRecordClusterLeaderDataSync : public IVistaClusterDataSync
{
public:
	VistaRecordClusterLeaderDataSync( const std::string& sFilename,
												IVistaClusterDataSync* pOriginalSync,
												const bool bManageOriginalSyncDeletion = false,
												const bool bVerbose = true );
	VistaRecordClusterLeaderDataSync( VistaConnectionFile* pRecordFile,
												IVistaClusterDataSync* pOriginalSync,
												const bool bManageOriginalSyncDeletion = false,
												const bool bVerbose = true );

	virtual ~VistaRecordClusterLeaderDataSync();

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

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual int GetNumberOfFollowers() const;
	virtual int GetNumberOfActiveFollowers() const;
	virtual int GetNumberOfDeadFollowers() const;
	virtual std::string GetFollowerNameForId( const int nID ) const;
	virtual int GetFollowerIdForName( const std::string& sName ) const;
	virtual bool GetFollowerIsAlive( const int nID ) const;
	virtual int GetLastChangedFollower();
	virtual bool DeactivateFollower( const std::string& sName );
	virtual bool DeactivateFollower( const int nID );


	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );

private:
	IVistaClusterDataSync* m_pOriginalSync;
	bool m_bManageOriginalSyncDeletion;
	VistaConnectionFile* m_pRecordFile;
	VistaTimer m_oTimer;
};

class VISTAINTERPROCCOMMAPI VistaReplayClusterFollowerDataSync : public IVistaClusterDataSync
{
public:
	VistaReplayClusterFollowerDataSync( const std::string& sFilename,
												const bool bVerbose = true );
	VistaReplayClusterFollowerDataSync( VistaConnectionFile* pRecordFile,
												const bool bVerbose = true );

	virtual ~VistaReplayClusterFollowerDataSync();

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

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual int GetNumberOfFollowers() const;
	virtual int GetNumberOfActiveFollowers() const;
	virtual int GetNumberOfDeadFollowers() const;
	virtual std::string GetFollowerNameForId( const int nID ) const;
	virtual int GetFollowerIdForName( const std::string& sName ) const;
	virtual bool GetFollowerIsAlive( const int nID ) const;
	virtual int GetLastChangedFollower();
	virtual bool DeactivateFollower( const std::string& sName );
	virtual bool DeactivateFollower( const int nID );


	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );

	// special for the replay sync: if WaitForSyncTime is true,
	// emitting the next sync will not occur until the same time has
	// been passed as when recording the session.
	// This will play back the recording in real-time if possible
	// (it will not run faster, but potentially slower)
	bool GetWaitForSyncTime() const { return m_bWaitForSyncTime; }
	void SetWaitForSyncTime( const bool& oValue ) { m_bWaitForSyncTime = oValue; }

private:
	void Wait();

	VistaConnectionFile* m_pReplayFile;
	VistaTimer m_oTimer;

	bool m_bWaitForSyncTime;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTARECORDREPLAYCLUSTERSYNC_H
