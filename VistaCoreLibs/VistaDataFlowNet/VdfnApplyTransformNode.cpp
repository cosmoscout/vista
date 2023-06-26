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

#include "VdfnApplyTransformNode.h"

#include "VdfnObjectRegistry.h"

#include <VistaAspects/VistaTransformable.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

const std::string VdfnApplyTransformNode::STransformInPortName("in");

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VdfnApplyTransformNode::VdfnApplyTransformNode(
    VdfnObjectRegistry* pObjReg, const std::string& strKey, const bool bApplyLocal)
    : IVdfnNode()
    , m_pInTransform(NULL)
    , m_pOutTransform(NULL)
    , m_nTCount(0)
    , m_pObjRegistry(pObjReg)
    , m_strKey(strKey)
    , m_bApplyLocal(bApplyLocal) {
  RegisterInPrototypes();
}

VdfnApplyTransformNode::VdfnApplyTransformNode()
    : IVdfnNode()
    , m_pInTransform(NULL)
    , m_pOutTransform(NULL)
    , m_pObjRegistry(NULL) {
  RegisterInPrototypes();
}

VdfnApplyTransformNode::VdfnApplyTransformNode(IVistaTransformable* pObj, const bool bApplyLocal)
    : IVdfnNode()
    , m_pInTransform(NULL)
    , m_pOutTransform(pObj)
    , m_pObjRegistry(NULL)
    , m_bApplyLocal(bApplyLocal) {
  RegisterInPrototypes();
}

VdfnApplyTransformNode::~VdfnApplyTransformNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VdfnApplyTransformNode::RegisterInPrototypes() {
  RegisterInPortPrototype(
      STransformInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix>>);
}

bool VdfnApplyTransformNode::GetIsValid() const {
  return (m_pOutTransform && m_pInTransform);
}

bool VdfnApplyTransformNode::PrepareEvaluationRun() {
  if (m_pObjRegistry && (m_pOutTransform == NULL) && !m_strKey.empty())
    m_pOutTransform = m_pObjRegistry->GetObjectTransform(m_strKey);

  m_pInTransform = dynamic_cast<TVdfnPort<VistaTransformMatrix>*>(GetInPort(STransformInPortName));
  return GetIsValid();
}

bool VdfnApplyTransformNode::DoEvalNode() {
  m_nTCount                     = m_pInTransform->GetUpdateCounter();
  const VistaTransformMatrix& m = (*m_pInTransform).GetValueConstRef();

  // Get the internal matrix from the transformable
  VistaTransformMatrix m2;
  m_pOutTransform->GetTransform(m2);

  // Multiply the matrices and put the values in a float
  // array again
  // Apply the matrix from right or left hand side
  VistaTransformMatrix matResult;
  if (m_bApplyLocal)
    matResult = m2 * m;
  else
    matResult = m * m2;

  // Set the matrix to the transformable
  m_pOutTransform->SetTransform(matResult);
  return true;
}

IVistaTransformable* VdfnApplyTransformNode::GetTransformTarget() const {
  return m_pOutTransform;
}

void VdfnApplyTransformNode::SetTransformTarget(IVistaTransformable* pTransform) {
  m_pOutTransform = pTransform;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
