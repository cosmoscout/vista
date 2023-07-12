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

#include <GL/glew.h>

#include "VistaReplaySlave.h"

#include <VistaInterProcComm/Cluster/Imps/VistaBroadcastClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaBroadcastClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaInterProcClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaInterProcClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataCollect.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataSync.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>
#include <VistaInterProcComm/IPNet/VistaIPAddress.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaObserver.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <VistaTools/VistaFileSystemDirectory.h>

#include <VistaKernel/Cluster/Utils/VistaGSyncSwapBarrier.h>
#include <VistaKernel/Cluster/Utils/VistaSlaveDataTunnel.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/Cluster/ZeroMQExt/VistaZeroMQClusterDataSync.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>
#include <VistaKernel/VistaSystem.h>

#include <cassert>

#if defined(WIN32)
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif
#include "VistaInterProcComm/Cluster/Imps/VistaDummyClusterDataCollect.h"
#include "VistaInterProcComm/Cluster/Imps/VistaRecordReplayClusterDataSync.h"
#include "VistaInterProcComm/Connections/VistaConnectionFile.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaReplaySlave::VistaReplaySlave(VistaSystem* pVistaSystem, const std::string& sSlaveName)
    : m_pVistaSystem(pVistaSystem)
    , m_nReplaySyncCounter(0)
    , m_sReplayDataFolder(sSlaveName)
    , m_pSystemEvent(new VistaSystemEvent)
    , m_pExternalMsgEvent(new VistaExternalMsgEvent)
    , m_pMsg(new VistaMsg)
    , m_pInteractionEvent(NULL) {
  m_pExternalMsgEvent->SetThisMsg(m_pMsg);
  m_pInteractionEvent = new VistaInteractionEvent(m_pVistaSystem->GetInteractionManager());

  // Register Events
  m_oClusterMessage.RegisterEventType(m_pSystemEvent);
  m_oClusterMessage.RegisterEventType(m_pInteractionEvent);
  m_oClusterMessage.RegisterEventType(m_pExternalMsgEvent);
}

VistaReplaySlave::~VistaReplaySlave() {
  delete m_pSystemEvent;
  delete m_pInteractionEvent;
  delete m_pExternalMsgEvent;
  delete m_pMsg;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaReplaySlave::Init(const std::string& sClusterSection, const VistaPropertyList& oConfig) {
  VistaFileSystemDirectory oFile(m_sReplayDataFolder);
  if (oFile.Exists() == false) {
    vstr::err() << "VistaReplaySlave::Init - cannot find recording folder \"" << m_sReplayDataFolder
                << "\" - no replay available" << std::endl;
    VISTA_THROW("VistaReplaySlave non-existent recird folder", -1);
  }

  std::string sFilename =
      m_sReplayDataFolder + "/record_" + VistaConversion::ToString(m_nReplaySyncCounter++);
  m_pDefaultDataSync = new VistaReplayClusterFollowerDataSync(sFilename);

  // sync first frame clock
  m_pDefaultDataSync->SyncTime(m_dFrameClock);

  m_pDefaultDataSync->SyncData(m_oClusterInfo);

  return true;
}

bool VistaReplaySlave::PostInit() {
  return true;
}

int VistaReplaySlave::GetClusterMode() const {
  return CM_REPLAY;
}

std::string VistaReplaySlave::GetClusterModeName() const {
  return "REPLAY";
}

int VistaReplaySlave::GetNodeType() const {
  return NT_REPLAY_SLAVE;
}

std::string VistaReplaySlave::GetNodeTypeName() const {
  return "REPLAY_SLAVE";
}

std::string VistaReplaySlave::GetNodeName() const {
  return "replay";
}

std::string VistaReplaySlave::GetConfigSectionName() const {
  return "SYSTEM";
}

int VistaReplaySlave::GetNodeID() const {
  return 0;
}

bool VistaReplaySlave::GetIsLeader() const {
  return false;
}

bool VistaReplaySlave::GetIsFollower() const {
  return true;
}

bool VistaReplaySlave::StartFrame() {
  ++m_iFrameCount;

  if (m_pDefaultDataSync == NULL || !m_pDefaultDataSync->GetIsValid())
    return false;

  if (m_pDefaultDataSync->SyncData(m_oClusterMessage) == false) {
    vstr::errp() << "[VistaCluserSlave]: Fatal communication error - aborting" << std::endl;
    VISTA_THROW("ClusterSlave Error: Communication failure", -1);
  }

  if (m_oClusterMessage.GetType() != VistaMasterSlave::Message::CMSG_STARTFRAME) {
    VISTA_THROW("ClusterSlave Protocol Error: received unexpected message type", -1);
  }

  if (m_oClusterMessage.GetFrameCount() != m_iFrameCount) {
    VISTA_THROW("ClusterSlave Error: received unexpected frame count", -1);
  }

  m_dFrameClock = m_oClusterMessage.GetClock();
  return true;
}

bool VistaReplaySlave::ProcessFrame() {
  while (m_pDefaultDataSync && m_pDefaultDataSync->GetIsValid()) {
    if (m_pDefaultDataSync->SyncData(m_oClusterMessage) == false) {
      VISTA_THROW("ClusterSlave Error: Communication failure", -1);
    }
    switch (m_oClusterMessage.GetType()) {
    case VistaMasterSlave::Message::CMSG_ENDFRAME: {
      return true;
    }
    case VistaMasterSlave::Message::CMSG_EVENT: {
      // special distinction for profiling of system events
      // @todo this is somewhat hacked currently
      VistaSystemEvent* pSysEvent = dynamic_cast<VistaSystemEvent*>(m_oClusterMessage.GetEvent());
      if (pSysEvent) {
        VistaKernelProfileScope(VistaSystemEvent::GetIdString(pSysEvent->GetId()));
        m_pVistaSystem->GetEventManager()->ProcessEvent(pSysEvent);
      } else {
        m_pVistaSystem->GetEventManager()->ProcessEvent(m_oClusterMessage.GetEvent());
      }
      break;
    }
    case VistaMasterSlave::Message::CMSG_QUIT: {
      // @todo
      break;
    }
    case VistaMasterSlave::Message::CMSG_INVALID:
    case VistaMasterSlave::Message::CMSG_STARTFRAME:
    default: {
      VISTA_THROW("VistaClusterSlave unexpected event type", -1);
    }
    }
  }

  return false;
}

bool VistaReplaySlave::EndFrame() {
  return true;
}

bool VistaReplaySlave::CreateConnections(std::vector<VistaConnectionIP*>& vecConnections) {
  return true;
}
bool VistaReplaySlave::CreateNamedConnections(
    std::vector<std::pair<VistaConnectionIP*, std::string>>& vecConnections) {
  return true;
}

IVistaDataTunnel* VistaReplaySlave::CreateDataTunnel(IDLVistaDataPacket* pPacketProto) {
  VISTA_THROW("Usage of DataTunnel not supported for replay mode", -1);
}

void VistaReplaySlave::Debug(std::ostream& oStream) const {
  oStream << "[VistaReplaySlave]\n"
          << "    ReplayFolder     : " << m_sReplayDataFolder << std::endl;
}

void VistaReplaySlave::SwapSync() {
  return;
}

IVistaClusterDataSync* VistaReplaySlave::CreateDataSync() {

  std::string sFilename =
      m_sReplayDataFolder + "/record_" + VistaConversion::ToString(m_nReplaySyncCounter++);
  return new VistaReplayClusterFollowerDataSync(sFilename);
}

IVistaClusterDataSync* VistaReplaySlave::GetDefaultDataSync() {
  return m_pDefaultDataSync;
}

IVistaClusterBarrier* VistaReplaySlave::CreateBarrier() {
  return new VistaDummyClusterBarrier;
}

IVistaClusterBarrier* VistaReplaySlave::GetDefaultBarrier() {
  if (m_pDefaultBarrier == NULL)
    m_pDefaultBarrier = new VistaDummyClusterBarrier;
  return m_pDefaultBarrier;
}

IVistaClusterDataCollect* VistaReplaySlave::CreateDataCollect() {
  return new VistaDummyClusterDataCollect;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
