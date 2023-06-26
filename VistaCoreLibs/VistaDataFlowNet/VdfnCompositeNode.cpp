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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <iostream>

#include "VdfnGraph.h"
#include "VdfnPort.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

#include "VdfnCompositeNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnCompositeNode::VdfnCompositeNode(VdfnGraph* pGraph)
    : IVdfnNode()
    , m_pGraph(pGraph)
    , m_bContainsMasterSim((m_pGraph->GetMasterSimCount() ? true : false)) {
  m_pGraph->GetExports(m_liExports);

  CreatePorts();
}

VdfnCompositeNode::~VdfnCompositeNode() {
  // important: we have the outports as POINTER-COPIES
  // from nodes within the m_pGraph,
  // so we purge the list of outports, *before* we
  // delete the graph, superclass's destructor might else
  // free the resources on the outports
  m_mpOutPorts.clear();

  delete m_pGraph;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VdfnCompositeNode::OnActivation(double dTs) {
  if (m_pGraph)
    m_pGraph->SetIsActive(true, dTs);
}

void VdfnCompositeNode::OnDeactivation(double dTs) {
  if (m_pGraph)
    m_pGraph->SetIsActive(false, dTs);
}

bool VdfnCompositeNode::GetIsValid() const {
  return (m_pGraph != NULL);
}

bool VdfnCompositeNode::SetInPort(const std::string& sName, IVdfnPort* pPort) {
  // ok
  // retrieve from inport map
  NAMEMAP::const_iterator cit = m_InportMap.find(sName);
  if (cit == m_InportMap.end()) {
    vstr::warnp() << "[VdfnCompositeNode::SetInPort]: Port [" << sName << "] not found"
                  << std::endl;
    return false;
  }

  std::string sMappedName = (*cit).second.m_strPortName;
  // if we found cit in the inport map, m_pTargetNode is not NULL!,
  // see CreatePorts(), so no need to test against NULL here.
  if ((*cit).second.m_pTargetNode->SetInPort(sMappedName, pPort)) {
    // register as mine...
    return DoSetInPort(sName, pPort);
  }

  return false;
}

bool VdfnCompositeNode::CreatePorts() {
  // should purge out old prototypes, as the update might run more than once...
  if (!m_mpOutPorts.empty())
    m_mpOutPorts.clear();

  if (!m_InportMap.empty())
    m_InportMap.clear();

  if (!m_mpInPrototypes.empty()) {
    for (TypeMap::iterator it2 = m_mpInPrototypes.begin(); it2 != m_mpInPrototypes.end(); ++it2) {
      delete (*it2).second;
    }

    m_mpInPrototypes.clear();
  }

  // create all inports
  for (std::list<VdfnGraph::ExportData>::const_iterator cit = m_liExports.begin();
       cit != m_liExports.end(); ++cit) {
    // register inport-prototype properly

    // first: find node in graph
    IVdfnNode* pNode = m_pGraph->GetNodeByName((*cit).m_strNodeName);
    if (!pNode)
      continue; /** @todo utter something here... */

    if ((*cit).m_nDirection == VdfnGraph::ExportData::INPORT) {

      const IVdfnPortTypeCompare& oPortCmp = pNode->GetPortTypeCompareFor((*cit).m_strPortName);

      // add prototype for this...
      RegisterInPortPrototype((*cit).m_strMapName, oPortCmp.Clone());
      //			std::cout << "Registering " << (*cit).m_strMapName << std::endl;
      // mark a target node for a later setport
      VdfnGraph::ExportData exp(*cit);
      exp.m_pTargetNode                = pNode;
      m_InportMap[(*cit).m_strMapName] = exp;
    } else {
      // claim port
      IVdfnPort* pOutport = pNode->GetOutPort((*cit).m_strPortName);
      if (!pOutport)
        continue; /** @todo utter something here */

      // register by pointer copy using the mapped name
      RegisterOutPort((*cit).m_strMapName, pOutport);
    }
  }

  return true;
}

bool VdfnCompositeNode::PrepareEvaluationRun() {
  if (m_pGraph)
    m_pGraph->EvaluateGraph(0); // first: update graph as a first timer...
  return GetIsValid();
}

bool VdfnCompositeNode::DoEvalNode() {
  return m_pGraph->EvaluateGraph(GetUpdateTimeStamp());
}

bool VdfnCompositeNode::GetIsMasterSim() const {
  return m_bContainsMasterSim;
}

const VdfnGraph* VdfnCompositeNode::GetGraph() const {
  return m_pGraph;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
