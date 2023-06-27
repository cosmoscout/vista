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

#include "VdfnOutstreamNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VdfnOutstreamNode::VdfnOutstreamNode(std::ostream& oOutstream)
    : IVdfnNode()
    , m_oOutstream(oOutstream)
    , m_pTextPort(NULL)
    , m_sPostfix("\n") {
  RegisterInPortPrototype("text", new TVdfnPortTypeCompare<TVdfnPort<std::string>>);
}

VdfnOutstreamNode::~VdfnOutstreamNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnOutstreamNode::PrepareEvaluationRun() {
  m_pTextPort = dynamic_cast<TVdfnPort<std::string>*>(GetInPort("text"));
  return GetIsValid();
}

bool VdfnOutstreamNode::DoEvalNode() {
  m_oOutstream << m_sPrefix << m_pTextPort->GetValueConstRef() << m_sPostfix << std::flush;
  return true;
}

std::string VdfnOutstreamNode::GetPrefix() const {
  return m_sPrefix;
}

void VdfnOutstreamNode::SetPrefix(const std::string& oValue) {
  m_sPrefix = oValue;
}

std::string VdfnOutstreamNode::GetPostfix() const {
  return m_sPostfix;
}

void VdfnOutstreamNode::SetPostfix(const std::string& oValue) {
  m_sPostfix = oValue;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
