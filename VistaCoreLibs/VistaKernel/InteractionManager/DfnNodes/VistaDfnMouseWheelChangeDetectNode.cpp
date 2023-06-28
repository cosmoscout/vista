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

#include "VistaDfnMouseWheelChangeDetectNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaDataFlowNet/VdfnUtil.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaDfnMouseWheelChangeDetectNode::VistaDfnMouseWheelChangeDetectNode()
    : IVdfnNode()
    , m_iLastState(0)
    , m_pWheelChange(new TVdfnPort<int>)
    , m_pWheelState(NULL) {
  RegisterInPortPrototype("wheel_state", new TVdfnPortTypeCompare<TVdfnPort<int>>);
  RegisterOutPort("wheel_change", m_pWheelChange);
}

VistaDfnMouseWheelChangeDetectNode::~VistaDfnMouseWheelChangeDetectNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDfnMouseWheelChangeDetectNode::PrepareEvaluationRun() {
  m_pWheelState = VdfnUtil::GetInPortTyped<TVdfnPort<int>*>("wheel_state", this);
  return GetIsValid();
}

bool VistaDfnMouseWheelChangeDetectNode::DoEvalNode() {
  int iNewState = m_pWheelState->GetValue();
  int iChange   = iNewState - m_iLastState;
  m_iLastState  = iNewState;
  m_pWheelChange->SetValue(iChange, GetUpdateTimeStamp());
  return true;
}
// #############################################################################

VistaDfnMouseWheelChangeDetectNodeCreate::VistaDfnMouseWheelChangeDetectNodeCreate() {
}

IVdfnNode* VistaDfnMouseWheelChangeDetectNodeCreate::CreateNode(
    const VistaPropertyList& oParams) const {
  return new VistaDfnMouseWheelChangeDetectNode();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
