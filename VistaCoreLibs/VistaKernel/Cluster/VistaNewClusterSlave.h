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

#ifndef _VISTANEWCLUSTERSLAVE_H
#define _VISTANEWCLUSTERSLAVE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaBaseTypes.h>
#include <VistaKernel/Cluster/Utils/VistaMasterSlaveUtils.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/VistaKernelConfig.h>

#include <list>
#include <ostream>
#include <string>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSystem;
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

class VISTAKERNELAPI VistaNewClusterSlave : public VistaClusterMode {
 public:
  VistaNewClusterSlave(VistaSystem* pVistaSystem, const std::string& sSlaveName);
  ~VistaNewClusterSlave();

  virtual bool Init(const std::string& sClusterSection, const VistaPropertyList& oConfig);
  virtual bool PostInit();

  virtual int         GetClusterMode() const;
  virtual std::string GetClusterModeName() const;

  virtual int         GetNodeType() const;
  virtual std::string GetNodeTypeName() const;
  virtual std::string GetNodeName() const;
  virtual std::string GetConfigSectionName() const;
  virtual int         GetNodeID() const;

  virtual bool GetIsLeader() const;
  virtual bool GetIsFollower() const;

  virtual bool StartFrame();
  virtual bool ProcessFrame();
  virtual bool EndFrame();

  virtual void SwapSync();
  bool         CreateDefaultSyncs();

  virtual bool CreateConnections(std::vector<VistaConnectionIP*>& vecConnections);
  virtual bool CreateNamedConnections(
      std::vector<std::pair<VistaConnectionIP*, std::string>>& vecConnections);
  virtual IVistaDataTunnel*         CreateDataTunnel(IDLVistaDataPacket* pPacketProto);
  virtual IVistaClusterDataSync*    CreateDataSync();
  virtual IVistaClusterDataSync*    GetDefaultDataSync();
  virtual IVistaClusterBarrier*     CreateBarrier();
  virtual IVistaClusterBarrier*     GetDefaultBarrier();
  virtual IVistaClusterDataCollect* CreateDataCollect();

  virtual void Debug(std::ostream& oStream) const;

  /**
   * returns the next free port that was specified in the slave's configuration
   * If no ports were specified, or all have been used already, -1 will be returned
   * If a valid port is returned, it will automatically be removed from the list
   * of available ports.
   */
  int GetNextFreePort();

 private:
  class SyncEntityObserver;
  friend class SyncEntityObserver;
  enum ForwardMode {
    DONT_FORWARD,
    FORWARD_MASTER,
    FORWARD_SLAVE,
  };

  bool WaitForClusterMaster(int nPort);
  bool InitSwapSync();

  bool ParseParameters(const VistaPropertyList& oSection);
  void ParseSyncForward(
      const VistaPropertyList& oSection, const std::string& sName, ForwardMode& bForwardVariable);

  VistaConnectionIP* CreateConnectionToMaster();

  IVistaClusterDataSync* CreateTypedDataSync(int nType, bool bUseDefaultConnection);
  IVistaClusterDataSync* CreateTCPIPDataSync(bool bUseDefaultConnection);
  IVistaClusterDataSync* CreateZeroMQDataSync();
  IVistaClusterDataSync* CreateInterProcDataSync();

  IVistaClusterBarrier* CreateTypedBarrier(
      int nType, bool bUseDefaultConnection, bool bIsSwapSyncBarrier);
  IVistaClusterBarrier* CreateTCPBarrier(bool bUseDefaultConnection);
  IVistaClusterBarrier* CreateBCBarrier(bool bUseDefaultConnection);
  IVistaClusterBarrier* CreateZeroMQBarrier(bool bUseDefaultConnection);
  IVistaClusterBarrier* CreateInterProcBarrier();

  void HandleMasterConnectionDrop();
  void PrintClusterSetupInfo() const;

  std::ostream& GetDebugStream();

 private:
  int         m_nOwnSlaveID;
  std::string m_sSlaveName;
  std::string m_sSlaveSectionName;
  int         m_iDataTunnelQueueSize;

  bool m_bDoByteSwap;
  bool m_bByteSwapSpecified;
  bool m_bDoOglFinish;

  VistaSystem* m_pVistaSystem;

  // the events that are supported
  VistaSystemEvent*      m_pSystemEvent;
  VistaExternalMsgEvent* m_pExternalMsgEvent;
  VistaMsg*              m_pMsg;
  VistaInteractionEvent* m_pInteractionEvent;

  std::string                    m_sOwnIP;
  int                            m_nServerPort;
  VistaMasterSlave::FreePortList m_vecFreePorts;

  VistaConnectionIP* m_pConnection;
  VistaConnectionIP* m_pSyncConnection;

  IVistaClusterDataSync* m_pDefaultDataSync;
  IVistaClusterBarrier*  m_pDefaultBarrier;
  IVistaClusterBarrier*  m_pSwapSyncBarrier;

  int m_nDataSyncMethod;
  int m_nSwapSyncMethod;
  int m_nSwapSyncTimeout;
  int m_nBarrierWaitMethod;
  int m_nBroadcastGroup;

  VistaMasterSlave::Message m_oClusterMessage;

  SyncEntityObserver* m_pSyncEntityObserver;

  ForwardMode m_eForwardBarrier;
  ForwardMode m_eForwardSwapBarrier;
  ForwardMode m_eForwardDataSync;

  std::string              m_sInterProcName;
  std::vector<std::string> m_vecInterProcNames;
  int                      m_nUsedInterProcs;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANEWCLUSTERSLAVE_H
