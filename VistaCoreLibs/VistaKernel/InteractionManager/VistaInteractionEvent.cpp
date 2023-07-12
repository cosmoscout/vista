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

#include "VistaInteractionEvent.h"
#include "VistaInteractionContext.h"
#include "VistaInteractionManager.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaAspects/VistaDeSerializer.h>
#include <VistaAspects/VistaSerializer.h>

#include <VistaDataFlowNet/VdfnGraph.h>
#include <VistaDataFlowNet/VdfnNode.h>

#include <cassert>
#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
int VistaInteractionEvent::m_nEventId = -1;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaInteractionEvent::VistaInteractionEvent(VistaInteractionManager* pMgr)
    : VistaEvent()
    , m_pContext(NULL)
    , m_nUpdateMsg(0)
    , m_pMgr(pMgr)
    , m_pEventNode(NULL) {
  SetType(VistaInteractionEvent::GetTypeId());
}

VistaInteractionEvent::~VistaInteractionEvent() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaInteractionContext* VistaInteractionEvent::GetInteractionContext() const {
  return m_pContext;
}

void VistaInteractionEvent::SetInteractionContext(VistaInteractionContext* pCtx) {
  m_pContext = pCtx;
}

unsigned int VistaInteractionEvent::GetRoleId() const {
  if (!m_pContext)
    return ~0;

  return m_pContext->GetRoleId();
}

VistaInteractionEvent::PortList& VistaInteractionEvent::GetPortMapWrite() {
  return m_mpPortChangeMap;
}

const VistaInteractionEvent::PortList& VistaInteractionEvent::GetPortMapRead() const {
  return m_mpPortChangeMap;
}

const IVdfnNode* VistaInteractionEvent::GetEventNode() const {
  return m_pEventNode;
}

void VistaInteractionEvent::SetEventNode(IVdfnNode* pNode) {
  m_pEventNode = pNode;
}

void VistaInteractionEvent::SetTime(double dTs) {
  m_nTime = dTs; // set protected member.
}

int VistaInteractionEvent::Serialize(IVistaSerializer& ser) const {
  int nRet = VistaEvent::Serialize(ser);
  nRet += ser.WriteInt32(m_nUpdateMsg);
  nRet += ser.WriteBool((m_pContext) ? true : false);

  if (m_pContext) {
    nRet += ser.WriteInt32(m_pContext->GetRoleId());
    nRet += VistaInteractionContext::SerializeContext(ser, *m_pContext);

    nRet += ser.WriteBool(m_pEventNode ? true : false);
    if (m_pEventNode) {
      std::string strName = m_pEventNode->GetNameForNameable();
      nRet += ser.WriteEncodedString(strName);
    }
  }

  nRet += ser.WriteInt32((VistaType::sint32)m_mpPortChangeMap.size());
  for (PortList::const_iterator it = m_mpPortChangeMap.begin(); it != m_mpPortChangeMap.end();
       ++it) {
    nRet += ser.WriteEncodedString(*it);
  }

  return nRet;
}

int VistaInteractionEvent::DeSerialize(IVistaDeSerializer& deSer) {
  int nRet = VistaEvent::DeSerialize(deSer);

  bool bContext = false;

  nRet += deSer.ReadInt32(m_nUpdateMsg);
  nRet += deSer.ReadBool(bContext);

  if (bContext) {
    int nRoleId = 0;
    nRet += deSer.ReadInt32(nRoleId);

    VistaInteractionContext* pCtx = m_pMgr->GetInteractionContextByRoleId(nRoleId);
    assert(pCtx);
    nRet += VistaInteractionContext::DeSerializeContext(deSer, *pCtx);
    m_pContext = pCtx;

    bool bNode = false;
    nRet += deSer.ReadBool(bNode);
    if (bNode) {
      std::string strName;
      deSer.ReadEncodedString(strName);

      if (bNode && m_pContext->GetTransformGraph() != NULL)
        m_pEventNode = m_pContext->GetTransformGraph()->GetNodeByName(strName);
    } else
      m_pEventNode = NULL; // can, but should not happen

  } else
    m_pContext = NULL; // should not happen

  VistaType::sint32 nCount = 0;
  nRet += deSer.ReadInt32(nCount);
  m_mpPortChangeMap.clear();
  for (VistaType::sint32 n = 0; n < nCount; ++n) {
    std::string strString;
    deSer.ReadEncodedString(strString);
    m_mpPortChangeMap.push_back(strString);
  }
  return nRet;
}

std::string VistaInteractionEvent::GetSignature() const {
  return "VistaInteractionEvent";
}

int VistaInteractionEvent::GetTypeId() {
  return VistaInteractionEvent::m_nEventId;
}

void VistaInteractionEvent::SetTypeId(int nId) {
  if (VistaInteractionEvent::m_nEventId < 0)
    m_nEventId = nId;
}

std::string VistaInteractionEvent::GetIdString(int nId) {
  switch (nId) {
  case VEID_CONTEXT_CHANGE:
    return "VEID_CONTEXT_CHANGE";
  case VEID_CONTEXT_GRAPH_UPDATE:
    return "VEID_CONTEXT_GRAPH_UPDATE";
  case VEID_GRAPH_INPORT_CHANGE:
    return "VEID_GRAPH_INPORT_CHANGE";
  default:
    return VistaEvent::GetIdString(nId);
  }
}

void VistaInteractionEvent::Debug(std::ostream& out) const {
  VistaEvent::Debug(out);
  out << " [ViIn]     event signature: " << GetSignature() << "\n";
  // out << " [ViIn]     event role     : [" << m_pMgr->GetRoleForId(m_pContext->GetRoleId())
  //<< "]\n";
  out << " [ViIn]     event id       : [" << GetIdString(GetId()) << "]" << std::endl;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
