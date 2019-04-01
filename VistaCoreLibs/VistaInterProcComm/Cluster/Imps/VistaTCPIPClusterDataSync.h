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


#ifndef _VISTATCPIPCLUSTERSYNC_H
#define _VISTATCPIPCLUSTERSYNC_H

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
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaTCPIPClusterLeaderDataSync : public VistaClusterBytebufferLeaderDataSyncBase
{
public:
	explicit VistaTCPIPClusterLeaderDataSync( const bool bVerbose = true );

	virtual ~VistaTCPIPClusterLeaderDataSync();
	
	int AddFollower( const std::string& sName,
						VistaConnectionIP* pConnection,
						const bool bManageDeletion = false );


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
	virtual bool DoSendMessage();

	struct Follower;
	bool RemoveFollower( Follower* pFollower );

private:
	std::vector<Follower*>	m_vecFollowers;
	std::vector<Follower*>	m_vecAliveFollowers;
	int						m_nLastChangedFollower;

	int						m_nBlockByteCount;
};

class VISTAINTERPROCCOMMAPI VistaTCPIPClusterFollowerDataSync : public VistaClusterBytebufferFollowerDataSyncBase
{
public:
	VistaTCPIPClusterFollowerDataSync( VistaConnectionIP* pLeaderConnection,
									const bool bSwap,
									const bool bManageDeletion = false,
									const bool bVerbose = true );

	virtual ~VistaTCPIPClusterFollowerDataSync();

	virtual bool GetIsValid() const;

	virtual std::string GetDataSyncType() const;

	virtual int GetSendBlockingThreshold() const;
	virtual bool SetSendBlockingThreshold( const int nNumBytes );

private:
	bool DoReceiveMessage();
	void Cleanup();

private:
	VistaConnectionIP*	m_pLeaderConn;
	bool				m_bManageDeletion;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATCPIPCLUSTERSYNC_H
