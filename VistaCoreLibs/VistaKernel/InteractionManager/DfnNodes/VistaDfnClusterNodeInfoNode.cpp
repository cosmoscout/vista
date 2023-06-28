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

#include "VistaDfnClusterNodeInfoNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

#include <VistaDataFlowNet/VdfnUtil.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnClusterNodeInfoNode::VdfnClusterNodeInfoNode(VistaClusterMode* pClAux)
    : IVdfnNode()
    , m_pClusterMode(pClAux)
    , m_pClusterNodeName(new TVdfnPort<std::string>)
    , m_pClusterNodeType(new TVdfnPort<std::string>)
    , m_pClusterClock(new TVdfnPort<double>) {
  RegisterOutPort("node_name", m_pClusterNodeName);
  RegisterOutPort("cluster_mode", m_pClusterNodeType);
  RegisterOutPort("node_clock", m_pClusterClock);

  // we mark this mode as unconditional eval for the frameclock
  SetEvaluationFlag(true);
}

VdfnClusterNodeInfoNode::~VdfnClusterNodeInfoNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnClusterNodeInfoNode::DoEvalNode() {
  if (m_pClusterNodeName->GetValueConstRef().empty())
    m_pClusterNodeName->SetValue(m_pClusterMode->GetNodeName(), GetUpdateTimeStamp());

  if (m_pClusterNodeType->GetValueConstRef().empty())
    m_pClusterNodeType->SetValue(m_pClusterMode->GetClusterModeName(), GetUpdateTimeStamp());

  m_pClusterClock->SetValue(m_pClusterMode->GetFrameClock(), GetUpdateTimeStamp());

  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
