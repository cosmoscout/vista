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

#include "VdfnSetTransformNode.h"

#include "VdfnObjectRegistry.h"

#include <VistaAspects/VistaTransformable.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

const std::string VdfnSetTransformNode::STransformInPortName("in");

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VdfnSetTransformNode::VdfnSetTransformNode(VdfnObjectRegistry* pObjReg, const std::string& strKey)
    : IVdfnNode()
    , m_pInTransform(NULL)
    , m_pTransform(NULL)
    , m_nTCount(0)
    , m_pObjRegistry(pObjReg)
    , m_strKey(strKey) {
  RegisterInPrototypes();
}

VdfnSetTransformNode::VdfnSetTransformNode()
    : IVdfnNode()
    , m_pInTransform(NULL)
    , m_pTransform(NULL)
    , m_pObjRegistry(NULL) {
  RegisterInPrototypes();
}

VdfnSetTransformNode::VdfnSetTransformNode(IVistaTransformable* pObj)
    : IVdfnNode()
    , m_pInTransform(NULL)
    , m_pTransform(pObj)
    , m_pObjRegistry(NULL) {
  RegisterInPrototypes();
}

VdfnSetTransformNode::~VdfnSetTransformNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VdfnSetTransformNode::RegisterInPrototypes() {
  RegisterInPortPrototype(
      STransformInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix>>);
}

bool VdfnSetTransformNode::GetIsValid() const {
  return m_pTransform && IVdfnNode::GetIsValid();
}

bool VdfnSetTransformNode::PrepareEvaluationRun() {
  if (m_pObjRegistry && (m_pTransform == NULL) && !m_strKey.empty())
    m_pTransform = m_pObjRegistry->GetObjectTransform(m_strKey);

  m_pInTransform = dynamic_cast<TVdfnPort<VistaTransformMatrix>*>(GetInPort(STransformInPortName));
  return GetIsValid();
}

bool VdfnSetTransformNode::DoEvalNode() {
  m_nTCount                     = m_pInTransform->GetUpdateCounter();
  const VistaTransformMatrix& m = (*m_pInTransform).GetValueConstRef();

  float m4x4[16];
  m.GetValues(m4x4);

  m_pTransform->SetTransform(m4x4);
  return true;
}

IVistaTransformable* VdfnSetTransformNode::GetTransformTarget() const {
  return m_pTransform;
}

void VdfnSetTransformNode::SetTransformTarget(IVistaTransformable* pTransform) {
  m_pTransform = pTransform;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
