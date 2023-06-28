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

#include "VdfnDecomposeQuaternionNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnDecomposeQuaternionNode::VdfnDecomposeQuaternionNode()
    : m_pOutX(new TVdfnPort<float>())
    , m_pOutY(new TVdfnPort<float>())
    , m_pOutZ(new TVdfnPort<float>())
    , m_pOutW(new TVdfnPort<float>())
    , m_pIn(NULL) {
  RegisterInPortPrototype("in", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>());

  RegisterOutPort("x", m_pOutX);
  RegisterOutPort("y", m_pOutY);
  RegisterOutPort("z", m_pOutZ);
  RegisterOutPort("w", m_pOutW);
}
VdfnDecomposeQuaternionNode::~VdfnDecomposeQuaternionNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnDecomposeQuaternionNode::PrepareEvaluationRun() {
  m_pIn = dynamic_cast<TVdfnPort<VistaQuaternion>*>(GetInPort("in"));
  return GetIsValid();
}

bool VdfnDecomposeQuaternionNode::DoEvalNode() {
  const VistaQuaternion& qQuat = m_pIn->GetValueConstRef();
  m_pOutX->SetValue(qQuat[0], GetUpdateTimeStamp());
  m_pOutY->SetValue(qQuat[1], GetUpdateTimeStamp());
  m_pOutZ->SetValue(qQuat[2], GetUpdateTimeStamp());
  m_pOutW->SetValue(qQuat[3], GetUpdateTimeStamp());
  return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
