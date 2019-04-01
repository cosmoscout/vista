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


#ifndef _VISTATCPIPCLUSTERCOLLECT_H
#define _VISTATCPIPCLUSTERCOLLECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaInterProcComm/Cluster/VistaClusterDataCollect.h>
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

class VISTAINTERPROCCOMMAPI VistaTCPIPClusterLeaderDataCollect : public IVistaClusterDataCollect
{
public:
	explicit VistaTCPIPClusterLeaderDataCollect( const bool bVerbose = true );

	virtual ~VistaTCPIPClusterLeaderDataCollect();
	
	int AddFollower( const std::string& sName,
						VistaConnectionIP* pConnection,
						const bool bManageDeletion = false );

	virtual bool CollectTime( const VistaType::systemtime nOwnTime,
							std::vector<VistaType::systemtime>& vecCollected );
	virtual bool CollectData( const VistaPropertyList& oList,
							std::vector<VistaPropertyList>& vecCollected );
	virtual bool CollectData( const VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							std::vector<std::vector<VistaType::byte> >& vecCollected );

	virtual bool GetIsValid() const;


	virtual std::string GetDataCollectType() const;

	virtual int GetNumberOfFollowers() const;
	virtual int GetNumberOfActiveFollowers() const;
	virtual int GetNumberOfDeadFollowers() const;
	virtual std::string GetFollowerNameForId( const int nID ) const;
	virtual int GetFollowerIdForName( const std::string& sName ) const;
	virtual bool GetFollowerIsAlive( const int nID ) const;
	virtual int GetLastChangedFollower();
	virtual bool DeactivateFollower( const std::string& sName );
	virtual bool DeactivateFollower( const int nID );

	virtual bool SetSendBlockingThreshold( const int nNumBytes );
	virtual int GetSendBlockingThreshold() const;

private:
	struct Follower;
	bool RemoveFollower( Follower* pFollower );

	bool ReceivePartFromFollower( int nExpectedType, Follower* pSlave,
									const int nTimeout = 0 );
	bool SendMessage();

private:
	VistaType::uint64		m_nCollectCount;
	VistaByteBufferDeSerializer m_oDeSer;
	std::vector<VistaType::byte> m_vecDeSerData;

	std::vector<Follower*>	m_vecFollowers;
	std::vector<Follower*>	m_vecAliveFollowers;
	int						m_nLastChangedFollower;
};

class VISTAINTERPROCCOMMAPI VistaTCPIPClusterFollowerDataCollect : public IVistaClusterDataCollect
{
public:
	VistaTCPIPClusterFollowerDataCollect( VistaConnectionIP* pLeaderConnection,
									const bool bSwap,
									const bool bManageDeletion = false,
									const bool bVerbose = true );

	virtual ~VistaTCPIPClusterFollowerDataCollect();

	virtual bool GetIsValid() const;

	virtual bool CollectTime( const VistaType::systemtime nOwnTime,
							std::vector<VistaType::systemtime>& vecCollected );
	virtual bool CollectData( const VistaPropertyList& oList,
							std::vector<VistaPropertyList>& vecCollected );
	virtual bool CollectData( const VistaType::byte* pDataBuffer, 
							const int iBufferSize,
							std::vector<std::vector<VistaType::byte> >& vecCollected );

	virtual std::string GetDataCollectType() const;

	virtual bool SetSendBlockingThreshold( const int nNumBytes );
	virtual int GetSendBlockingThreshold() const;

private:
	bool SendMessage();
	void Cleanup();

private:
	VistaType::uint64			m_nCollectCount;
	VistaConnectionIP*			m_pLeaderConn;
	bool						m_bManageDeletion;
	VistaByteBufferSerializer	m_oMessage;
	int							m_nBlockByteCount;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATCPIPCLUSTERCOLLECT_H
