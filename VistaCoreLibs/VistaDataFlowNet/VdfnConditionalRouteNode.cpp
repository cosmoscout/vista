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

#include "VdfnConditionalRouteNode.h"

#include <VistaDataFlowNet/VdfnUtil.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnConditionalRouteNode::VdfnConditionalRouteNode()
    : VdfnShallowNode()
    , m_pTrigger(NULL)
    , m_nUpdateScore(0) {
  RegisterInPortPrototype("trigger", new TVdfnPortTypeCompare<TVdfnPort<bool>>);
}

VdfnConditionalRouteNode::~VdfnConditionalRouteNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VdfnConditionalRouteNode::SetInPort(const std::string& sName, IVdfnPort* pPort) {
  if (VdfnShallowNode::SetInPort(sName, pPort)) {
    // register out port
    IVdfnPort* pClonedPort = pPort->Clone();
    if (pClonedPort) {
      RegisterOutPort(sName, pClonedPort);
    } else {
      // should we unset the inport?
      return false;
    }
    return true;
  }
  return false;
}

bool VdfnConditionalRouteNode::PrepareEvaluationRun() {
  m_pTrigger = VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>("trigger", this);

  // prepare trigger map in m_pHlp
  for (IVdfnNode::PortMap::const_iterator cit = this->m_mpInPorts.begin(); cit != m_mpInPorts.end();
       ++cit) {
    IVdfnPort* pPort = (*cit).second;
    if (pPort) {
      // store initial revision for this port in the portmap
      m_mpRevisions[pPort] = pPort->GetUpdateCounter();
    }
  }

  return (m_pTrigger != NULL);
}

bool VdfnConditionalRouteNode::DoEvalNode() {
  if (m_pTrigger->GetValue()) {
    // copy in-ports to outports
    for (IVdfnNode::PortMap::const_iterator cit = this->m_mpInPorts.begin();
         cit != m_mpInPorts.end(); ++cit) {
      IVdfnPort* pIn  = (*cit).second;
      IVdfnPort* pOut = GetOutPort((*cit).first);
      if (pIn && pOut) {
        if (pIn->GetUpdateCounter() > m_mpRevisions[pIn]) {
          m_mpRevisions[pIn] = pIn->GetUpdateCounter();
          pOut->AssignFrom(pIn);
        }
      }
    }
  }

  return true;
}

unsigned int VdfnConditionalRouteNode::CalcUpdateNeededScore() const {
  if (m_pTrigger->GetValue()) {
    return (++m_nUpdateScore);
  }
  return m_nUpdateScore;
}

// ############################################################################

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
