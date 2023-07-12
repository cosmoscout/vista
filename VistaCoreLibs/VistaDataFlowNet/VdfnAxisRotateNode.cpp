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

#include "VdfnAxisRotateNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnAxisRotateNode::VdfnAxisRotateNode()
    : IVdfnNode()
    , m_pAngle(NULL)
    , m_pAxis(NULL)
    , m_pAxisAndAngle(NULL)
    , m_pOut(new TVdfnPort<VistaQuaternion>) {
  RegisterInPortPrototype("angle", new TVdfnPortTypeCompare<TVdfnPort<float>>);
  RegisterInPortPrototype("axis", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("axisandangle", new TVdfnPortTypeCompare<TVdfnPort<VistaAxisAndAngle>>);
  RegisterOutPort("out", m_pOut);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnAxisRotateNode::GetIsValid() const {
  return (((m_pAxisAndAngle == NULL) && (m_pAxis && m_pAngle)) ||
          ((m_pAxisAndAngle) && (!m_pAxis && !m_pAngle)));
}

bool VdfnAxisRotateNode::PrepareEvaluationRun() {
  m_pAngle        = dynamic_cast<TVdfnPort<float>*>(GetInPort("angle"));
  m_pAxis         = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("axis"));
  m_pAxisAndAngle = dynamic_cast<TVdfnPort<VistaAxisAndAngle>*>(GetInPort("axisandangle"));

  return GetIsValid();
}

bool VdfnAxisRotateNode::DoEvalNode() {
  if (m_pAxisAndAngle) {
    VistaAxisAndAngle aaa = m_pAxisAndAngle->GetValue();
    m_pOut->SetValue(VistaQuaternion(aaa), GetUpdateTimeStamp());
  } else {
    VistaAxisAndAngle aaa(m_pAxis->GetValue(), m_pAngle->GetValue());
    m_pOut->SetValue(VistaQuaternion(aaa), GetUpdateTimeStamp());
  }
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
