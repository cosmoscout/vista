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


#ifndef _VISTACLUSTERMODE_H
#define _VISTACLUSTERMODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaSerializable.h>

#include <string>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPropertyList;
class VistaInteractionContext;
class IVistaDataTunnel;
class VistaConnectionIP;
class IVistaClusterDataSync;
class IVistaClusterBarrier;
class IVistaClusterDataCollect;
class IDLVistaDataPacket;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaClusterMode
{
public:
	virtual ~VistaClusterMode();

	virtual bool Init( const std::string& sClusterSection,
						const VistaPropertyList& oConfig ) = 0;
	virtual bool PostInit() = 0;

	enum CLUSTER_MODE
	{
		CM_INVALID = -1,
		CM_UNKNOWN = 0,
		CM_STANDALONE,
		CM_MASTER_SLAVE,
		CM_REPLAY,
	};
	virtual int GetClusterMode() const = 0;
	virtual std::string GetClusterModeName() const = 0;
	
	enum CLUSTER_NODE_TYPE
	{
		NT_INVALID = -1,
		NT_UNKNOWN = 0,
		NT_STANDALONE,
		NT_MASTER,
		NT_SLAVE,
		NT_REPLAY_SLAVE,
	};
	virtual int GetNodeType() const = 0;
	virtual std::string GetNodeTypeName() const = 0;
	virtual std::string GetNodeName() const = 0;
	virtual std::string GetConfigSectionName() const = 0;
	virtual int GetNodeID() const = 0;

	virtual bool GetIsLeader() const = 0;
	virtual bool GetIsFollower() const = 0;		

	// Functions mainly to be called from the VistaFrameloop
	virtual bool StartFrame() = 0;
	virtual bool ProcessFrame() = 0;
	virtual bool EndFrame() = 0;

	virtual void SwapSync() = 0;
	
	/**
	 * Creates a list of connections, which can be either zero (unconnected or standalone),
	 * one for followers (conn to the leader), or multiple conns for leaders (one per follower)
	 */
	virtual bool CreateConnections( std::vector<VistaConnectionIP*>& vecConnections ) = 0;
	virtual bool CreateNamedConnections( std::vector<std::pair<VistaConnectionIP*, std::string> >&
															vecConnections ) = 0;

	virtual IVistaDataTunnel* CreateDataTunnel( IDLVistaDataPacket* pPacketProto ) = 0;
	/**
	 * Creates a fully configured ClusterDataSync object. Always returns a non-NULL instance,
	 * but it may not be valid if connections failed or the configuration is wrong
	 */
	virtual IVistaClusterDataSync* CreateDataSync() = 0;	
	/**
	 * Returns the default DataSync. Since this object may also be used by the clustermode itself,
	 * take care that it is only used in the main thread!
	 */
	virtual IVistaClusterDataSync* GetDefaultDataSync() = 0;
	/**
	 * Creates a fully configured ClusterBarrier object. Always returns a non-NULL instance,
	 * but it may not be valid if connections failed or the configuration is wrong
	 */
	virtual IVistaClusterBarrier* CreateBarrier() = 0;
	/**
	 * Returns the default barrier. Since this object may also be used by the clustermode itself,
	 * take care that it is only used in the main thread!
	 */
	virtual IVistaClusterBarrier* GetDefaultBarrier() = 0;
	/**
	 * Creates a fully configured ClusterDataCollect object. Always returns a non-NULL instance,
	 * but it may not be valid if connections failed or the configuration is wrong
	 */
	virtual IVistaClusterDataCollect* CreateDataCollect() = 0;

	/**
	 * Print debug output for the CLusterMode to the passed stream
	 */
	virtual void Debug( std::ostream& oStream ) const = 0;
	

	virtual VistaType::systemtime GetFrameClock() const;
	virtual int GetFrameCount() const;

	struct NodeInfo
	{
		std::string			m_sNodeName;
		CLUSTER_NODE_TYPE	m_eNodeType;
		int					m_iNodeID;
		bool				m_bIsActive;
	};
	int GetNumberOfNodes() const;
	int GetNumberOfActiveNodes() const;
	int GetNumberOfDeadNodes() const;
	std::string GetNodeName( const int iNodeID ) const;
	bool GetNodeIsActive( const int iNodeID ) const;
	CLUSTER_NODE_TYPE GetNodeType( const int iNodeID ) const;
	NodeInfo GetNodeInfo( const int iNodeID ) const;
	bool GetNodeInfo( const int iNodeID, NodeInfo& oInfo ) const;

	
protected:
	VistaClusterMode();

protected:
	VistaType::systemtime	m_dFrameClock;
	int						m_iFrameCount;
	IVistaClusterDataSync*	m_pDefaultDataSync;
	IVistaClusterBarrier*	m_pDefaultBarrier;

	class ClusterInfo : public IVistaSerializable
	{
	public:
		ClusterInfo();
		virtual int Serialize( IVistaSerializer & ) const;
		virtual int DeSerialize( IVistaDeSerializer & );
		virtual std::string GetSignature() const;

	public:
		std::vector<NodeInfo>	m_vecNodeInfos;
		
		bool					m_bClusterDebuggingActive;
		bool					m_bClusterDebuggingrependTimestamp;
		std::string				m_sClusterDebuggingStreamName;
		std::string				m_sClusterDebuggingFile;
	};
	ClusterInfo			m_oClusterInfo;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERMODE_H
