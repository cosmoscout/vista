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


#ifndef _VISTACLUSTERBARRIERIPBASE_H
#define _VISTACLUSTERBARRIERIPBASE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterBarrier.h>

#include <vector>
#include <string>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
class VistaUDPSocket;
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaClusterLeaderBarrierIPBase : public IVistaClusterBarrier
{
public:
	VistaClusterLeaderBarrierIPBase( const bool bVerbose );
	virtual ~VistaClusterLeaderBarrierIPBase();
	
	int AddFollower( const std::string& sName,
						VistaConnectionIP* pConnection,
						const bool bManageConnectionDeletion = true );

	virtual int GetNumberOfFollowers() const;
	virtual int GetNumberOfActiveFollowers() const;
	virtual int GetNumberOfDeadFollowers() const;
	virtual std::string GetFollowerNameForId( const int nID ) const;
	virtual int GetFollowerIdForName( const std::string& sName ) const;
	virtual bool GetFollowerIsAlive( const int nID ) const;
	virtual int GetLastChangedFollower();
	virtual bool DeactivateFollower( const std::string& sName );
	virtual bool DeactivateFollower( const int nID );

	virtual bool GetIsValid() const;

	virtual bool SetSendBlockingThreshold( const int nNumBytes );
	virtual int GetSendBlockingThreshold() const;
	
	int GetAllowedConsecutiveSyncFailures() const;
	void SetAllowedConsecutiveSyncFailures( const int oValue );

protected:
	bool WaitForAllFollowers( int iTimeOut );

	struct Follower
	{
		Follower( const std::string& sName, VistaConnectionIP* pConn,
					bool bDelete, int nID );
		std::string			m_sName;
		int					m_nID;
		VistaConnectionIP*	m_pLeaderConn;
		bool				m_bManageConnDeletion;
		int					m_nSyncFailures;
	};
	bool RemoveFollower( Follower* pFollower );

	std::vector<Follower*>& GetFollowersRef();

	VistaType::sint32 GetBarrierWaitCount();

private:
	VistaType::sint32		m_nBarrierWaitCount;

	std::vector<Follower*>	m_vecFollowers;
	std::vector<Follower*>	m_vecAliveFollowers;
	int						m_nLastChangedFollower;
	int						m_nAllowedConsecutiveSyncFailures;
};

class VISTAINTERPROCCOMMAPI VistaClusterFollowerBarrierIPBase : public IVistaClusterBarrier
{
public:
	VistaClusterFollowerBarrierIPBase( VistaConnectionIP* pLeaderConnection,
									const bool bManageConnection = true,
									const bool bVerbose = true );

	virtual ~VistaClusterFollowerBarrierIPBase();

	virtual bool GetIsValid() const;

protected:
	bool SendReadyTokenToLeader( int iTimeOut );
	void ProcessError();

	VistaType::sint32 GetBarrierWaitCount() const;
	void SetBarrierWaitCount( VistaType::sint32 nCount );

	bool GetDoesSwap() const;

	VistaConnectionIP* GetConnection();

	virtual bool SetSendBlockingThreshold( const int nNumBytes );
	virtual int GetSendBlockingThreshold() const;

private:	
	VistaType::sint32	m_nBarrierWaitCount;

	VistaConnectionIP*	m_pLeaderConn;
	bool				m_bManageConnection;
	bool				m_bSwap;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERBARRIERIPBASE_H
