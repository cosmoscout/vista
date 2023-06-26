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

#include "VdfnDecompose3DVectorNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnDecompose3DVectorNode::VdfnDecompose3DVectorNode()
    : m_pOutX(new TVdfnPort<float>)
    , m_pOutY(new TVdfnPort<float>)
    , m_pOutZ(new TVdfnPort<float>)
    , m_pOutW(new TVdfnPort<float>)
    , m_pIn(NULL) {
  RegisterInPortPrototype("in", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);

  RegisterOutPort("x", m_pOutX);
  RegisterOutPort("y", m_pOutY);
  RegisterOutPort("z", m_pOutZ);
  RegisterOutPort("w", m_pOutW);
}
VdfnDecompose3DVectorNode::~VdfnDecompose3DVectorNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnDecompose3DVectorNode::GetIsValid() const {
  return (m_pIn != NULL);
}

bool VdfnDecompose3DVectorNode::PrepareEvaluationRun() {
  m_pIn = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("in"));
  return GetIsValid();
}

bool VdfnDecompose3DVectorNode::DoEvalNode() {
  const VistaVector3D& v3Vector = m_pIn->GetValueConstRef();
  m_pOutX->SetValue(v3Vector[0], GetUpdateTimeStamp());
  m_pOutY->SetValue(v3Vector[1], GetUpdateTimeStamp());
  m_pOutZ->SetValue(v3Vector[2], GetUpdateTimeStamp());
  m_pOutW->SetValue(v3Vector[3], GetUpdateTimeStamp());
  return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
