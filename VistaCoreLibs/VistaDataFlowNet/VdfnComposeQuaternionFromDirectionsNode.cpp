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

#include "VdfnComposeQuaternionFromDirectionsNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnComposeQuaternionFromDirectionsNode::VdfnComposeQuaternionFromDirectionsNode()
    : m_pInFrom(NULL)
    , m_pInTo(NULL)
    , m_pOut(new TVdfnPort<VistaQuaternion>()) {
  RegisterInPortPrototype("from", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("to", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterOutPort("out", m_pOut);
}
VdfnComposeQuaternionFromDirectionsNode::~VdfnComposeQuaternionFromDirectionsNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnComposeQuaternionFromDirectionsNode::PrepareEvaluationRun() {
  m_pInFrom = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("from"));
  m_pInTo   = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("to"));
  return GetIsValid();
}

bool VdfnComposeQuaternionFromDirectionsNode::DoEvalNode() {
  VistaQuaternion& qOut = m_pOut->GetValueRef();

  qOut = VistaQuaternion(m_pInFrom->GetValueConstRef(), m_pInTo->GetValueConstRef());
  qOut.Normalize();

  m_pOut->SetUpdateTimeStamp(GetUpdateTimeStamp());
  m_pOut->IncUpdateCounter();
  return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
