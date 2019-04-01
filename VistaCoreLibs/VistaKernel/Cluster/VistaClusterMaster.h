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


#ifndef _VISTACLUSTERMASTER_H
#define _VISTACLUSTERMASTER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

#include <string>
#include <list>
#include <iostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaWeightedAverageTimer;
//class VistaMasterNetworkSync;
class VistaPropertyList;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaClusterMaster : public VistaClusterMode
{
public:
	VistaClusterMaster( VistaEventManager* pEventManager,
			            const std::string& sMasterName );
	~VistaClusterMaster();

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
	virtual bool GetNodeInfo( const int iNodeID, NodeInfo& oInfo ) const;

	virtual bool GetIsLeader() const;
	virtual bool GetIsFollower() const;	

	virtual int GetNumberOfNodes() const;	
	virtual std::string GetNodeName( const int iNodeID ) const;

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

private:
	bool AddSlave( const std::string& sName,
					const std::string &sIP, int iPort,
					bool bDoSwap, bool bDoSync );
	void WaitForSlaveInit();


private:
	VistaEventManager*		m_pEventManager;
	std::string				m_sMasterName; 
	std::string				m_sMasterSectionName;

	VistaWeightedAverageTimer *m_pAvgBc;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// internal API
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	class EventObserver;
	EventObserver*			m_pEventObserver;

	IVistaClusterDataSync*		m_pDefaultDataSync;
	IVistaClusterBarrier*	m_pDefaultBarrier;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACLUSTERMASTER_H

