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

#include "VdfnGraph.h"
#include "VdfnActionNode.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnReEvalNode.h"

#include <VistaTools/VistaTopologyGraph.h>

#include <algorithm>
#include <iostream>
#include <stack>
#include <string>

using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnGraph::VdfnGraph()
    : m_bUpToDate(true)
    , m_bIsActive(true)
    , m_bCheckMasterSim(false) {
}

VdfnGraph::~VdfnGraph() {
  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    delete (*cit);
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CALCULATION
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool VdfnGraph::NeedsEvaluation() const {
  NodeVec::const_iterator begin = m_vecTraversal.begin();
  NodeVec::const_iterator end   = m_vecTraversal.end();
  for (NodeVec::const_iterator it = begin; it != end; ++it) {
    // iterate over source nodes ONLY
    // these are nodes with no attached input (even if they have one, in theory)
    if ((*it)->UnconditionalEvaluation())
      return true;

    // we need to call eval on *every* node. it is not sufficient to evaluate
    // only those nodes that have no in-port (source-nodes). Here is an
    // example:
    //  constant -> do_something_on_constant
    // if we evaluate only those with 0 inports, we would see that constant
    // has changed its value, but we will never call eval on do_some_constant
    // however, we could to evaluate only those with inport size == 0, iff we
    // use a stack traversal and move from iterative to recursive traversal of the graph
    // (pushing all the predecessors of constant to be evaluated next)
    if ((*it)->NeedsEval() == true)
      return true; // stop on first match (dirty / needs Eval == true)
  }

  // either no source node needs an update or no node?
  // whatever, we do not need to calculate anything here
  return false;
}

bool VdfnGraph::EvaluateGraph(double nTimeStamp) {
  // adding or removing a node toggles the dirty flag, but adjusting edges does not
  // so we accept the time stamp == 0 as trigger to re-check on the traversal vector
  if (!m_bUpToDate || nTimeStamp == 0) {
    if (UpdateTraversalVector() == false)
      return false;
    m_bUpToDate = true;
  }

  // in case the time-stamp was 0, we skip the evaluation step
  if (nTimeStamp == 0)
    return true; // system message, update only.

  if (m_bIsActive == false)
    return false;

  // no need to evaluate an empty graph
  if (m_vecTraversal.empty())
    return true;

  // First: Normal evaluation of whole graph
  if (EvaluateSubGraph(m_vecTraversal, nTimeStamp) == false)
    return false;

  // check if any node needs a reevaluation
  std::map<IVdfnReEvalNode*, NodeVec>::reverse_iterator ritReEvalNode;
  std::map<IVdfnReEvalNode*, NodeVec>::reverse_iterator rbegin = m_mpReEvalSubgraphs.rbegin();
  std::map<IVdfnReEvalNode*, NodeVec>::reverse_iterator rend   = m_mpReEvalSubgraphs.rend();
  for (ritReEvalNode = rbegin; ritReEvalNode != rend; /* no loop increment condition */) {
    // Determines if another evaluation run is required for the nodes
    // NeedsEval will usually true in the first, normal evaluation run,
    // but also indicated if a ReEvalNode needs another run afterwards
    // The tests and re-evaluations are performed back-to-front in order
    // to ensure that a further-down ReEvalNode is fully evaluated before
    // a ReEvalNode further-up can trigger and change the input
    if ((*ritReEvalNode).first->NeedsEval()) {
      // call evaluate on the subgraph starting with the current node
      if (EvaluateSubGraph((*ritReEvalNode).second, nTimeStamp) == false)
        return false;
      // reset iterator: check all prior nodes again, they may have changed
      ritReEvalNode = m_mpReEvalSubgraphs.rbegin();
    } else
      ++ritReEvalNode;
  }

  return true;
}

bool VdfnGraph::EvaluateSubGraph(const NodeVec& vecSubGraph, const double nTimeStamp) {
  NodeVec::const_iterator begin = vecSubGraph.begin();
  NodeVec::const_iterator end   = vecSubGraph.end();

  for (NodeVec::const_iterator citNode = begin; citNode != end; ++citNode) {
    // mast-sim nodes are only evaluated on the master node
    // this is reflected by the state of the CheckMasterSim flag on this graph
    if (m_bCheckMasterSim && (*citNode)->GetIsMasterSim())
      continue;

    // only check on valid nodes
    if ((*citNode)->GetIsValid() == false) {
      // this node is not valid... hmm...
      // only check on enabled nodes
      if ((*citNode)->GetIsEnabled() == false)
        continue; // skip
#if defined(_DEBUG)
      vstr::warnp() << "[VdfnGraph::EvaluateGraph] Node [" << (*citNode)->GetNameForNameable()
                    << "] is not valid..." << std::endl;
#endif

      // try to make it valid by calling PrepareEvaluationRun()
      // maybe the node was not ready on the first try...
      // we call this 'Recover'
      if ((*citNode)->PrepareEvaluationRun() == false) {
#if defined(_DEBUG)
        vstr::warnp() << "recover did not work... continue..." << std::endl;
#endif
        (*citNode)->SetIsEnabled(false); // failed, mark disabled
        continue;
      } else {
        (*citNode)->SetIsEnabled(true); // recovered

        // Do the evaluation
        if ((*citNode)->EvalNode(nTimeStamp) == false)
          return false;
      }
    } else {
      // Do the evaluation
      if ((*citNode)->EvalNode(nTimeStamp) == false) {
#ifdef DEBUG
        vstr::warnp() << "[VdfnGraph::EvaluateGraph] Node [" << (*citNode)->GetNameForNameable()
                      << "] failed on EvalNode()" << std::endl;
#endif
        return false;
      }
    }
  } // for citNode: node iteration
  return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NODE MANAGEMENT
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool VdfnGraph::AddNode(IVdfnNode* pNode) {
  m_liNodes.push_back(pNode);
  m_bUpToDate = false;

  return true;
}

bool VdfnGraph::RemNode(IVdfnNode* pNode) {
  m_liNodes.remove(pNode);
  m_bUpToDate = false;
  return true;
}

bool VdfnGraph::GetIsNode(IVdfnNode* pNode) const {
  return (std::find(m_liNodes.begin(), m_liNodes.end(), pNode) != m_liNodes.end());
}

IVdfnNode* VdfnGraph::GetNodeByName(const std::string& strName) const {
  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    if ((*cit)->GetNameForNameable() == strName)
      return (*cit);
  }
  return NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// EDGE MANAGEMENT
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool VdfnGraph::GetIsConnectedFromTo(IVdfnNode* pFrom, IVdfnNode* pTo) const {
  // PPV: GetIsNode(pFrom) == true

  Edges::const_iterator cit = m_mpEdges.find(pFrom);
  if (cit == m_mpEdges.end())
    return false;

  const std::list<Connect>& liPrts = (*cit).second;
  for (std::list<Connect>::const_iterator t = liPrts.begin(); t != liPrts.end(); ++t) {
    const ConInfo& edges = (*t).second;
    for (ConInfo::const_iterator nit = edges.begin(); nit != edges.end(); ++nit) {
      if ((*nit).first == pTo)
        return true;
    }
  }
  return false;
}

void VdfnGraph::PrepareAllPorts() {
  for (Nodes::iterator uit = m_liNodes.begin(); uit != m_liNodes.end(); ++uit) {
    if ((*uit)->PreparePorts() == false)
      (*uit)->SetIsEnabled(false);
  }
}

bool VdfnGraph::ReloadActionObjects() {
  for (Nodes::iterator uit = m_liNodes.begin(); uit != m_liNodes.end(); ++uit) {
    VdfnActionNode* pActionNode = dynamic_cast<VdfnActionNode*>((*uit));
    if (pActionNode == NULL)
      continue;

    if (pActionNode->ReloadActionObject() == false) {
      pActionNode->SetIsEnabled(false);
      continue;
    }

    // get all ports of this node
    std::list<std::string> liOutPorts = pActionNode->GetOutPortNames();
    for (std::list<std::string>::const_iterator sit = liOutPorts.begin(); sit != liOutPorts.end();
         ++sit) {
      // insert an entry into the port lookup for each port,
      // pointing to its containing node
      IVdfnPort* pPort      = pActionNode->GetOutPort(*sit);
      m_mpPortLookup[pPort] = pActionNode;
    }
  }

  m_mpEdges.clear();

  // go through all nodes, check the in ports, lookup the parent node
  // of the inports and add an edge from parent node to current node
  // to the edge map
  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    std::list<std::string> liInPorts = (*cit)->GetInPortNames();
    for (std::list<std::string>::const_iterator sit = liInPorts.begin(); sit != liInPorts.end();
         ++sit) {
      // look up the parent node in the port-map
      IVdfnPort* pPort = (*cit)->GetInPort(*sit);
      if (pPort) // maybe NULL iff not connected
      {
        // ok, lookup in portmap
        PortLookup::const_iterator pit = m_mpPortLookup.find(pPort);
        if (pit != m_mpPortLookup.end()) {
          // ok, we found a parent node
          // add an edge to the edge-list
          Connect& edges = GetOrCreateAdjacencyListForNode((*pit).second, pit->first);
          // add edge from (in_p) -> (sink_p)
          edges.second.push_back(
              std::pair<IVdfnNode*, ConPortInfo>(*cit, ConPortInfo(*sit, pPort)));
        }
      }
    }
  }

  return true;
}

bool VdfnGraph::UpdateTraversalVector() {
  PrepareAllPorts();
  UpdateOutportLookupMap();
  UpdateEdgeMap();

  m_vecTraversal.clear();

  VistaTopologyGraph<IVdfnNode*> topology;
  for (Edges::iterator cit = m_mpEdges.begin(); cit != m_mpEdges.end(); ++cit) {
    const std::list<Connect>& liPrts = (*cit).second;
    for (std::list<Connect>::const_iterator t = liPrts.begin(); t != liPrts.end(); ++t) {
      const ConInfo& liChildren = (*t).second;
      // add node with children to topology graph
      for (ConInfo::const_iterator nit = liChildren.begin(); nit != liChildren.end(); ++nit) {
        // child depends on parent
        topology.AddDependency((*cit).first, (*nit).first);
      }
    }
  }

  std::list<VistaTopologyGraph<IVdfnNode*>::Node*> liNodes = topology.CalcTopology();

  for (std::list<VistaTopologyGraph<IVdfnNode*>::Node*>::const_iterator tcit = liNodes.begin();
       tcit != liNodes.end(); ++tcit) {
    m_vecTraversal.push_back((*tcit)->m_oElement);
    // check if node is a re-evalnode. if so, create its subgraph
    IVdfnReEvalNode* pReEvalNode = dynamic_cast<IVdfnReEvalNode*>((*tcit)->m_oElement);
    // If a node is a ReEvalNode, it may become necessary to evaluate it multiple times per
    // frame. While the full graph is evaluated on the first run, a reevaluation should
    // only process the ReEvalNode and the subgraph spanned by it, so that other nodes
    // (Especially other ReEvalNodes) are not influenced
    // Therefore, we construct a subgraph by breadth-first-search from the node,
    // and store it in a map for later use
    if (pReEvalNode) {
      // create subgraph
      std::list<VistaTopologyGraph<IVdfnNode*>::Node*> liSubgraph =
          topology.CalcBFSSubgraph((*tcit));

      std::vector<IVdfnNode*>& vecSubgraph = m_mpReEvalSubgraphs[pReEvalNode];
      vecSubgraph.resize(liSubgraph.size());

      std::list<VistaTopologyGraph<IVdfnNode*>::Node*>::const_iterator citOrigNode =
          liSubgraph.begin();
      std::vector<IVdfnNode*>::iterator itCopyNode                         = vecSubgraph.begin();
      std::list<VistaTopologyGraph<IVdfnNode*>::Node*>::const_iterator end = liSubgraph.end();

      for (; citOrigNode != end; ++citOrigNode, ++itCopyNode)
        (*itCopyNode) = (*citOrigNode)->m_oElement;
    }

    if ((*tcit)->m_oElement->PrepareEvaluationRun() == false) {
      std::string strUserTag;
      (*tcit)->m_oElement->GetUserTag(strUserTag);
      vstr::warnp() << "[VdfnGraph--" << strUserTag << "]: PrepareEvaluationRun() on node ["
                    << (*tcit)->m_oElement->GetNameForNameable() << "] failed to initialize."
                    << std::endl;
    }
  }

  // collect unconnected nodes
  // ok, this is not the most optimal method for finding nodes without edges to other
  // nodes, but at this point, we have quite some time to take for the calculation
  // and it works, so... what the heck...
  Nodes::const_iterator begin = m_liNodes.begin();
  Nodes::const_iterator end   = m_liNodes.end();

  for (Nodes::const_iterator ucit = begin; ucit != end; ++ucit) {
    // not in topology graph, so it is not in the edge list
    if (!topology.GetNodeByValue(*ucit)) {
      // while we are at it: call initialize on these nodes :)
      if ((*ucit)->PrepareEvaluationRun() == true)
        // order is not important, could be anywhere in the list
        m_vecTraversal.push_back(*ucit);
      else {
        // omit broken nodes
        vstr::warnp() << "[VdfnGraph]: PrepareEvaluationRun() on node ["
                      << (*ucit)->GetNameForNameable() << "] failed to initialize." << std::endl;
      }
    }
  }
  return true;
}

bool VdfnGraph::UpdateOutportLookupMap() {
  // clear old values
  m_mpPortLookup.clear();

  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    // get all ports of this node
    std::list<std::string> liOutPorts = (*cit)->GetOutPortNames();
    for (std::list<std::string>::const_iterator sit = liOutPorts.begin(); sit != liOutPorts.end();
         ++sit) {
      // insert an entry into the port lookup for each port,
      // pointing to its containing node
      IVdfnPort* pPort      = (*cit)->GetOutPort(*sit);
      m_mpPortLookup[pPort] = *cit;
    }
  }
  return true;
}

bool VdfnGraph::UpdateEdgeMap() {
  m_mpEdges.clear();

  // go through all nodes, check the in ports, lookup the parent node
  // of the inports and add an edge from parent node to current node
  // to the edge map
  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    std::list<std::string> liInPorts = (*cit)->GetInPortNames();
    for (std::list<std::string>::const_iterator sit = liInPorts.begin(); sit != liInPorts.end();
         ++sit) {
      // look up the parent node in the port-map
      IVdfnPort* pPort = (*cit)->GetInPort(*sit);
      if (pPort) // maybe NULL iff not connected
      {
        // ok, lookup in portmap
        PortLookup::const_iterator pit = m_mpPortLookup.find(pPort);
        if (pit != m_mpPortLookup.end()) {
          // ok, we found a parent node
          // add an edge to the edge-list
          Connect& edges = GetOrCreateAdjacencyListForNode((*pit).second, pit->first);
          // add edge and note the name as well
          edges.second.push_back(
              std::pair<IVdfnNode*, ConPortInfo>(*cit, ConPortInfo(*sit, pPort)));
        }
      }
    }
  }

  return true;
}

IVdfnNode* VdfnGraph::GetNodeForPort(IVdfnPort* pPort) const {
  PortLookup::const_iterator cit = m_mpPortLookup.find(pPort);
  if (cit == m_mpPortLookup.end())
    return NULL;
  return (*cit).second;
}

VdfnGraph::Connect& VdfnGraph::GetOrCreateAdjacencyListForNode(IVdfnNode* pNode, IVdfnPort* pPort) {
  // lookup node in edge map
  Edges::iterator it = m_mpEdges.find(pNode);
  if (it == m_mpEdges.end()) {
    // add a new entry

    it = m_mpEdges.insert(Edges::value_type(pNode, std::list<Connect>())).first;
    (*it).second.push_back(Connect(pPort, ConInfo()));
  } else {
    // lookup the list for the port
    for (std::list<Connect>::iterator cit = (*it).second.begin(); cit != (*it).second.end();
         ++cit) {
      // FIXME this is problematic, if a port is shared multiple times
      // (as it only searches for the first match, it may skip at least
      // one edge).
      if ((*cit).first == pPort) {
        return (*cit);
      }
    }

    // iff we are here, we did not find it,
    // so insert a new connect to the back of the list
    (*it).second.push_back(Connect(pPort, ConInfo()));
  }

  // and give this back
  return (*it).second.back();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// COMPOSITE/EXPORTS MANAGEMENT
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void VdfnGraph::GetExports(ExportList& liExports) const {
  liExports = m_liExports;
}

void VdfnGraph::SetExports(const ExportList& liExports) {
  for (ExportList::const_iterator cit = liExports.begin(); cit != liExports.end(); ++cit) {
    const VdfnGraph::ExportData& exp   = *cit;
    IVdfnNode*                   pNode = GetNodeByName(exp.m_strNodeName);
    if (!pNode) {
      vstr::warnp() << "VdfnGraph::SetExports() -- Node [" << exp.m_strNodeName
                    << "] not found - skipping" << std::endl;
      continue;
    }
    if (exp.m_nDirection == VdfnGraph::ExportData::INPORT) {
      // verify on availability of inport
      if (pNode->GetHasInPort(exp.m_strPortName) == false) {
        vstr::warnp() << "VdfnGraph::SetExports() -- inport [" << exp.m_strPortName
                      << "] NOT FOUND on node " << exp.m_strNodeName << std::endl;
        vstr::warni() << "the node has the following in-ports:" << std::endl;
        vstr::IndentObject     oIndent;
        std::list<std::string> liInPorts = pNode->GetInPortNames();
        for (std::list<std::string>::const_iterator it = liInPorts.begin(); it != liInPorts.end();
             ++it) {
          vstr::warni() << *it << std::endl;
        }
      }
    } else if (exp.m_nDirection == VdfnGraph::ExportData::OUTPORT) {
      if (pNode->GetOutPort(exp.m_strPortName) == NULL) {
        vstr::warnp() << "VdfnGraph::SetExports() -- outport [" << exp.m_strPortName
                      << "] NOT FOUND on node " << exp.m_strNodeName << std::endl;
      }
    } else {
      vstr::errp() << "VdfnGraph::SetExports() -- port direction not"
                   << " set (UNDEFINED), skipping this node declaration.";
      continue; // for-loop
    }
    m_liExports.push_back(exp);
  }
}

int VdfnGraph::Serialize(IVistaSerializer& oSer) const {
  int nRet = 0;
  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    IVdfnNode*             pNode      = (*cit);
    std::list<std::string> liOutPorts = pNode->GetOutPortNames();

    for (std::list<std::string>::const_iterator lit = liOutPorts.begin(); lit != liOutPorts.end();
         ++lit) {
      IVdfnPort*                pPort = pNode->GetOutPort(*lit);
      VdfnPortSerializeAdapter* pAdp  = pPort->GetSerializeAdapter();
      oSer.WriteSerializable(*pAdp);
    }
  }
  return nRet;
}

int VdfnGraph::DeSerialize(IVistaDeSerializer& deSer) {
  int nRet = 0;
  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    IVdfnNode*             pNode      = (*cit);
    std::list<std::string> liOutPorts = pNode->GetOutPortNames();

    for (std::list<std::string>::const_iterator lit = liOutPorts.begin(); lit != liOutPorts.end();
         ++lit) {
      IVdfnPort*                pPort = pNode->GetOutPort(*lit);
      VdfnPortSerializeAdapter* pAdp  = pPort->GetSerializeAdapter();
      deSer.ReadSerializable(*pAdp);
    }
  }
  return nRet;
}

string VdfnGraph::GetSignature() const {
  return "VdfnGraph";
}

const VdfnGraph::Nodes& VdfnGraph::GetNodes() const {
  return m_liNodes;
}

const VdfnGraph::Edges& VdfnGraph::GetEdges() const {
  return m_mpEdges;
}

VdfnGraph::Nodes VdfnGraph::GetNodesWithTag() const {
  Nodes liRet;

  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    if ((*cit)->GetHasUserTag())
      liRet.push_back(*cit);
  }

  return liRet;
}

VdfnGraph::Nodes VdfnGraph::GetNodesByGroupTag(const std::string& strGroupTag) const {
  Nodes liRet;

  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    if ((*cit)->GetGroupTag() == strGroupTag)
      liRet.push_back(*cit);
  }

  return liRet;
}

VdfnGraph::Nodes VdfnGraph::GetSourceNodes() const {
  Nodes liRet;
  if (!m_vecTraversal.empty()) {
    for (NodeVec::const_iterator cit = m_vecTraversal.begin(); cit != m_vecTraversal.end(); ++cit) {
      if ((*cit)->GetInPortCount() == 0) {
        liRet.push_back((*cit));
      } else {
        // found an instance in the traversal vec
        // with an inport set -> no more source nodes
        // can be found! skip the rest
        return liRet;
      }
    }
  } else {
    // no traversal vec yet, iterate over all nodes in this graph
    for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
      if ((*cit)->GetInPortCount() == 0)
        liRet.push_back(*cit);
    }
  }
  return liRet;
}

unsigned int VdfnGraph::GetMasterSimCount() const {
  unsigned int nCnt = 0;
  for (Nodes::const_iterator cit = m_liNodes.begin(); cit != m_liNodes.end(); ++cit) {
    if ((*cit)->GetIsMasterSim())
      ++nCnt;
  }
  return nCnt;
}

void VdfnGraph::SetIsActive(bool bIsActive, double dTimeStamp) {
  if (dTimeStamp != 0 && (bIsActive == m_bIsActive))
    return; // skip, all nodes are active and it is no system message

  m_bIsActive = bIsActive;

  Nodes::iterator begin = m_liNodes.begin();
  Nodes::iterator end   = m_liNodes.end();

  for (Nodes::iterator it = begin; it != end; ++it) {
    if (!(*it)->GetIsValid())
      continue;

    if (m_bIsActive)
      (*it)->OnActivation(dTimeStamp);
    else
      (*it)->OnDeactivation(dTimeStamp);
  }
}

bool VdfnGraph::GetIsActive() const {
  return m_bIsActive;
}

bool VdfnGraph::GetCheckMasterSim() const {
  return m_bCheckMasterSim;
}

void VdfnGraph::SetCheckMasterSim(bool bCheck) {
  m_bCheckMasterSim = bCheck;
}

void VdfnGraph::PrintInfo(std::ostream& oStream) const {
  oStream << "CheckMasterSim         :" << (m_bCheckMasterSim ? "true" : "false") << "\n";
  oStream << "Nodes:\n";
  for (NodeVec::const_iterator itNode = m_vecTraversal.begin(); itNode != m_vecTraversal.end();
       ++itNode) {
    (*itNode)->PrintInfo(oStream);
  }
  oStream.flush();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
