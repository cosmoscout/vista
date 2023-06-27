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

#include "VistaDfnEventSourceNode.h"

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaDataFlowNet/VdfnPort.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnEventSourceNode::VistaDfnEventSourceNode(
    VistaEventManager* pEvMgr, VistaInteractionManager* pInMa, const std::string& strSourceTag)
    : VdfnShallowNode()
    , m_pEvMgr(pEvMgr)
    , m_pInMa(pInMa)
    , m_pEvent(new VistaInteractionEvent(pInMa))
    , m_strSourceTag(strSourceTag) {
  (*m_pEvent).SetId(VistaInteractionEvent::VEID_GRAPH_INPORT_CHANGE);
  (*m_pEvent).SetInteractionContext(
      pInMa->GetInteractionContextByRoleId(pInMa->GetRoleId(m_strSourceTag)));
  (*m_pEvent).SetEventNode(this);
}

VistaDfnEventSourceNode::~VistaDfnEventSourceNode() {
  delete m_pEvent;
}

bool VistaDfnEventSourceNode::PrepareEvaluationRun() {
  m_mpRevision.clear();
  for (PortMap::const_iterator it = m_mpInPorts.begin(); it != m_mpInPorts.end(); ++it) {
    IVdfnPort* pPort    = (*it).second;
    m_mpRevision[pPort] = pPort->GetUpdateCounter();
  }
  return true;
}

bool VistaDfnEventSourceNode::DoEvalNode() {
  VistaInteractionEvent::PortList& liChangedPorts = (*m_pEvent).GetPortMapWrite();
  liChangedPorts.clear();

  for (PortMap::const_iterator it = m_mpInPorts.begin(); it != m_mpInPorts.end(); ++it) {
    IVdfnPort*            pCheckPort = (*it).second;
    RevisionMap::iterator rit        = m_mpRevision.find(pCheckPort);
    if (rit != m_mpRevision.end()) {
      if (pCheckPort->GetUpdateCounter() > (*rit).second) {
        // note port
        liChangedPorts.push_back((*it).first);
        // note current revision
        (*rit).second = pCheckPort->GetUpdateCounter();
      }
    }
  }

  if (!liChangedPorts.empty()) {
    if (m_pEvent->GetInteractionContext() == NULL)
      (*m_pEvent).SetInteractionContext(
          m_pInMa->GetInteractionContextByRoleId(m_pInMa->GetRoleId(m_strSourceTag)));

    // utter event!
    (*m_pEvMgr).ProcessEvent(m_pEvent);
  }
  return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
