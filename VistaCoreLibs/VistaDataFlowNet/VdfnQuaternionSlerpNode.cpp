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
#include "VdfnQuaternionSlerpNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
const std::string VdfnQuaternionSlerpNode::S_sFirstQuaternionInPortName("first");
const std::string VdfnQuaternionSlerpNode::S_sSecondQuaternionInPortName("second");
const std::string VdfnQuaternionSlerpNode::S_sFractionInPortName("fraction");

const std::string VdfnQuaternionSlerpNode::S_sQuaternionOutPortName("out");

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnQuaternionSlerpNode::VdfnQuaternionSlerpNode()
    : IVdfnNode()
    , m_pInFirstQuaternion(NULL)
    , m_pInSecondQuaternion(NULL)
    , m_pInFraction(NULL)
    , m_pOutQuaternion(new TVdfnPort<VistaQuaternion>) {
  // define Inports
  RegisterInPortPrototype(
      S_sFirstQuaternionInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
  RegisterInPortPrototype(
      S_sSecondQuaternionInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
  RegisterInPortPrototype(S_sFractionInPortName, new TVdfnPortTypeCompare<TVdfnPort<float>>);

  // define Outports
  RegisterOutPort(S_sQuaternionOutPortName, m_pOutQuaternion);
}

VdfnQuaternionSlerpNode::~VdfnQuaternionSlerpNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VdfnQuaternionSlerpNode::GetIsValid() const {
  // All ports are optional. Thus we do not have to check something here.
  return true;
}

bool VdfnQuaternionSlerpNode::PrepareEvaluationRun() {
  // prepare Inports for first usage
  m_pInFirstQuaternion =
      dynamic_cast<TVdfnPort<VistaQuaternion>*>(GetInPort(S_sFirstQuaternionInPortName));
  m_pInSecondQuaternion =
      dynamic_cast<TVdfnPort<VistaQuaternion>*>(GetInPort(S_sSecondQuaternionInPortName));
  m_pInFraction = dynamic_cast<TVdfnPort<float>*>(GetInPort(S_sFractionInPortName));

  // check for validity
  return GetIsValid();
}

bool VdfnQuaternionSlerpNode::DoEvalNode() {
  VistaQuaternion first    = VistaQuaternion();
  VistaQuaternion second   = VistaQuaternion();
  float           fraction = 0.0;

  // check for used Inports and set the values if available
  if (m_pInFirstQuaternion)
    first = m_pInFirstQuaternion->GetValueConstRef();
  if (m_pInSecondQuaternion)
    second = m_pInSecondQuaternion->GetValueConstRef();
  if (m_pInFraction)
    fraction = m_pInFraction->GetValueConstRef();

  // calculate the slerp
  m_pOutQuaternion->GetValueRef() = first.Slerp(second, fraction);

  // apply quaternion to the Outport
  m_pOutQuaternion->SetUpdateTimeStamp(GetUpdateTimeStamp());
  m_pOutQuaternion->IncUpdateCounter();

  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
