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


#ifndef _VISTACLUSTERSLAVE_H
#define _VISTACLUSTERSLAVE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaBase/VistaBaseTypes.h>

#include <string>
#include <list>
#include <vector>
#include <ostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaClusterMode;

class VistaConnectionIP;
class IVistaSocket;
class VistaInteractionEvent;
class VistaInteractionManager;
class VistaSystemEvent;
class VistaExternalMsgEvent;
class VistaMsg;
class VistaWeightedAverageTimer;
class VistaByteBufferDeSerializer;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaClusterSlave : public VistaClusterMode
{
public:
	VistaClusterSlave( VistaEventManager* pEventManager,
			           const std::string& sSlaveName,
					   VistaInteractionManager* pInteractionMngr );
	~VistaClusterSlave();

	virtual bool Init( const std::string& sClusterSection,
						const VistaPropertyList& oConfig );
	virtual bool PostInit();

	virtual int GetClusterMode() const;
	virtual std::string GetClusterModeName() const;
	
	virtual int GetNodeType() const;
	virtual std::string GetNodeTypeName() const;
	virtual std::string GetNodeName() const;
	virtual std::string GetConfigSectionName() const;
	virtual int GetNodeID() const;

	virtual bool GetIsLeader() const;
	virtual bool GetIsFollower() const;	

	virtual int GetNumberOfNodes() const;	
	virtual std::string GetNodeName( const int iNodeID ) const;
	virtual bool GetNodeInfo( const int iNodeID, NodeInfo& oInfo ) const;

	virtual bool StartFrame();
	virtual bool ProcessFrame();
	virtual bool EndFrame();

	virtual void SwapSync();

	virtual bool CreateConnections( std::vector<VistaConnectionIP*>& vecConnections );
	virtual bool CreateNamedConnections( std::vector<std::pair<VistaConnectionIP*, std::string> >&
															vecConnections );
	virtual IVistaDataTunnel* CreateDataTunnel( IDLVistaDataPacket* pPacketProto );
	virtual IVistaClusterDataSync* CreateDataSync();
	virtual IVistaClusterDataSync* GetDefaultDataSync();
	virtual IVistaClusterBarrier* CreateBarrier();
	virtual IVistaClusterBarrier* GetDefaultBarrier();
	virtual IVistaClusterDataCollect* CreateDataCollect();

	virtual void Debug( std::ostream& oStream ) const;

	/**
	 * returns the next free port that was specified in the slave's configuration
	 * If no ports were specified, or all have been used already, -1 will be returned
	 * If a valid port is returned, it will automatically be removed from the list
	 * of available ports.
	 */
	int GetNextFreePort();

	VistaType::microtime GetAvgUpdateTime() const;
	VistaType::microtime GetAvgSwapTime() const;	

private:
	bool ReceiveClusterSetupInformation();
	bool WaitForClusterMaster( const std::string &strIp, int nPort, int nAckPort );

	VistaConnectionIP* CreateConnectionToMaster();	

	void ReceiveDataFromMaster();
	// checked reads - throw exception on failure
	bool ReadBoolFromMsg();
	VistaType::sint32 ReadIntFromMsg();
	double ReadDoubleFromMsg();
	void ReadStringFromMsg( std::string& sTarget );

private:
	std::string					m_sSlaveName;
	std::string					m_sSlaveSectionName;
	std::string					m_sSlaveHostName;
	unsigned int				m_nSlaveIndex;
	unsigned int				m_nBeforeAckDelay;
	unsigned int				m_nBeforeSwapDelay;
	unsigned int				m_nAfterSwapSyncDelay;
	int							m_iDataTunnelQueueSize;
	int							m_nSyncTimeout;

	bool						m_bDoSwap;
	bool						m_bDoOglFinish;
	VistaEventManager*			m_pEventManager;

	VistaConnectionIP*			m_pConnection;
	VistaConnectionIP*			m_pAckConnection;
	IVistaSocket*				m_pSyncSocket;
	VistaWeightedAverageTimer*	m_pAvgUpd;
	VistaWeightedAverageTimer*	m_pAvgSwap;	

	std::vector<VistaType::byte>			m_vecMessage;
	VistaByteBufferDeSerializer*	m_pMessageDeSer;

	// the events to be thrown
	VistaSystemEvent*			m_pSystemEvent;
	VistaExternalMsgEvent*		m_pExternalMsgEvent;
	VistaMsg*					m_pMsg;
	VistaInteractionEvent*		m_pInteractionEvent;

	int							m_iOwnSlaveID;
	NodeInfo					m_oMasterInfo;
	std::vector<NodeInfo>		m_vecSlaveInfo;

	std::list<std::pair<int,int> >	m_liFreePorts;

	IVistaClusterDataSync*		m_pDefaultDataSync;
	IVistaClusterBarrier*		m_pDefaultBarrier;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERSLAVE_H
