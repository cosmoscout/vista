
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

#include "VistaClusterMode.h"

#include <VistaAspects/VistaDeSerializer.h>
#include <VistaAspects/VistaSerializer.h>

#include <VistaInterProcComm/Cluster/VistaClusterBarrier.h>
#include <VistaInterProcComm/Cluster/VistaClusterDataSync.h>

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterMode::VistaClusterMode()
    : m_iFrameCount(-1)
    , m_dFrameClock(0)
    , m_pDefaultDataSync(NULL) {
}

VistaClusterMode::~VistaClusterMode() {
  delete m_pDefaultDataSync;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaType::systemtime VistaClusterMode::GetFrameClock() const {
  return m_dFrameClock;
}

int VistaClusterMode::GetFrameCount() const {
  return m_iFrameCount;
}

int VistaClusterMode::GetNumberOfNodes() const {
  return (int)m_oClusterInfo.m_vecNodeInfos.size();
}

std::string VistaClusterMode::GetNodeName(const int iNodeID) const {
  assert(iNodeID >= 0 && iNodeID < (int)m_oClusterInfo.m_vecNodeInfos.size());
  return m_oClusterInfo.m_vecNodeInfos[iNodeID].m_sNodeName;
}

bool VistaClusterMode::GetNodeIsActive(const int iNodeID) const {
  assert(iNodeID >= 0 && iNodeID < (int)m_oClusterInfo.m_vecNodeInfos.size());
  return m_oClusterInfo.m_vecNodeInfos[iNodeID].m_bIsActive;
}

VistaClusterMode::CLUSTER_NODE_TYPE VistaClusterMode::GetNodeType(const int iNodeID) const {
  assert(iNodeID >= 0 && iNodeID < (int)m_oClusterInfo.m_vecNodeInfos.size());
  return m_oClusterInfo.m_vecNodeInfos[iNodeID].m_eNodeType;
}

VistaClusterMode::NodeInfo VistaClusterMode::GetNodeInfo(const int iNodeID) const {
  assert(iNodeID >= 0 && iNodeID < (int)m_oClusterInfo.m_vecNodeInfos.size());
  return m_oClusterInfo.m_vecNodeInfos[iNodeID];
}

bool VistaClusterMode::GetNodeInfo(const int iNodeID, NodeInfo& oInfo) const {
  if (iNodeID >= 0 && iNodeID < (int)m_oClusterInfo.m_vecNodeInfos.size())
    return false;
  oInfo = m_oClusterInfo.m_vecNodeInfos[iNodeID];
  return true;
}

int VistaClusterMode::GetNumberOfActiveNodes() const {
  int nCount = 0;
  for (std::vector<NodeInfo>::const_iterator itNode = m_oClusterInfo.m_vecNodeInfos.begin();
       itNode != m_oClusterInfo.m_vecNodeInfos.end(); ++itNode) {
    if ((*itNode).m_bIsActive)
      ++nCount;
  }
  return nCount;
}

int VistaClusterMode::GetNumberOfDeadNodes() const {
  int nCount = 0;
  for (std::vector<NodeInfo>::const_iterator itNode = m_oClusterInfo.m_vecNodeInfos.begin();
       itNode != m_oClusterInfo.m_vecNodeInfos.end(); ++itNode) {
    if ((*itNode).m_bIsActive == false)
      ++nCount;
  }
  return nCount;
}

/*============================================================================*/
/* VistaClusterInfo                                                       */
/*============================================================================*/

VistaClusterMode::ClusterInfo::ClusterInfo()
    : m_bClusterDebuggingActive(false)
    , m_bClusterDebuggingrependTimestamp(false) {
}

int VistaClusterMode::ClusterInfo::Serialize(IVistaSerializer& oSer) const {
  int nRet = 0;
  nRet += oSer.WriteInt32((VistaType::sint32)m_vecNodeInfos.size());
  for (std::size_t i = 0; i < m_vecNodeInfos.size(); ++i) {
    nRet += oSer.WriteInt32((VistaType::sint32)m_vecNodeInfos[i].m_iNodeID);
    nRet += oSer.WriteInt32((VistaType::sint32)m_vecNodeInfos[i].m_eNodeType);
    nRet += oSer.WriteBool(m_vecNodeInfos[i].m_bIsActive);
    nRet += oSer.WriteEncodedString(m_vecNodeInfos[i].m_sNodeName);
  }

  nRet += oSer.WriteBool(m_bClusterDebuggingActive);
  nRet += oSer.WriteBool(m_bClusterDebuggingrependTimestamp);
  nRet += oSer.WriteEncodedString(m_sClusterDebuggingFile);
  nRet += oSer.WriteEncodedString(m_sClusterDebuggingStreamName);
  return nRet;
}

int VistaClusterMode::ClusterInfo::DeSerialize(IVistaDeSerializer& oDeSer) {
  int               nRet = 0;
  VistaType::sint32 nDummy;
  nRet += oDeSer.ReadInt32(nDummy);
  m_vecNodeInfos.resize(nDummy);
  for (std::size_t i = 0; i < m_vecNodeInfos.size(); ++i) {
    nRet += oDeSer.ReadInt32(nDummy);
    m_vecNodeInfos[i].m_iNodeID = nDummy;
    nRet += oDeSer.ReadInt32(nDummy);
    m_vecNodeInfos[i].m_eNodeType = (CLUSTER_NODE_TYPE)nDummy;
    nRet += oDeSer.ReadBool(m_vecNodeInfos[i].m_bIsActive);
    nRet += oDeSer.ReadEncodedString(m_vecNodeInfos[i].m_sNodeName);
  }

  nRet += oDeSer.ReadBool(m_bClusterDebuggingActive);
  nRet += oDeSer.ReadBool(m_bClusterDebuggingrependTimestamp);
  nRet += oDeSer.ReadEncodedString(m_sClusterDebuggingFile);
  nRet += oDeSer.ReadEncodedString(m_sClusterDebuggingStreamName);
  return nRet;
}

std::string VistaClusterMode::ClusterInfo::GetSignature() const {
  return "VistaClusterMode::ClusterInfo";
}
