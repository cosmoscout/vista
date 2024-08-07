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

#include "VdfnUpdateThresholdNode.h"
#include "VdfnUtil.h"

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnUpdateThresholdNode::VdfnUpdateThresholdNode(double dThreshold)
    : m_dThreshold(dThreshold)
    , m_dSum(0.0)
    , m_pIn(NULL)
    , m_pOut(new TVdfnPort<double>) {
  RegisterInPortPrototype("in", new TVdfnPortTypeCompare<TVdfnPort<double>>);

  RegisterOutPort("out", m_pOut);
}

VdfnUpdateThresholdNode::~VdfnUpdateThresholdNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnUpdateThresholdNode::PrepareEvaluationRun() {
  m_pIn = dynamic_cast<TVdfnPort<double>*>(GetInPort("in"));
  return GetIsValid();
}

bool VdfnUpdateThresholdNode::DoEvalNode() {
  m_dSum += m_pIn->GetValue();

  if (m_dSum > m_dThreshold) {
    m_pOut->SetValue(m_dSum, GetUpdateTimeStamp());
    m_dSum = 0.0;
  }

  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
