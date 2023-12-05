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

#include "VdfnEulerRotateNode.h"
#include "VistaBase/VistaQuaternion.h"
#include "VistaBase/VistaVector3D.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnEulerRotateNode::VdfnEulerRotateNode()
    : IVdfnNode()
    , m_pEulerAngles(NULL)
    , m_pOut(new TVdfnPort<VistaQuaternion>) {
  RegisterInPortPrototype("angles", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterOutPort("out", m_pOut);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnEulerRotateNode::GetIsValid() const {
  return m_pEulerAngles != NULL;
}

bool VdfnEulerRotateNode::PrepareEvaluationRun() {
  m_pEulerAngles = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("angles"));
  return GetIsValid();
}

bool VdfnEulerRotateNode::DoEvalNode() {
  VistaVector3D a = m_pEulerAngles->GetValue();
  VistaEulerAngles euler(a[0], a[1], a[2]);
  m_pOut->SetValue(VistaQuaternion(euler), GetUpdateTimeStamp());
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
