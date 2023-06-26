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

#include "VistaDfnReferenceFrameTransformNode.h"

#include <VistaAspects/VistaConversion.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnUtil.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaDfnReferenceFrameTransformNode::VistaDfnReferenceFrameTransformNode(
    VistaReferenceFrame* pFrame, bool bTransformToFrame, int iNrPositionPorts,
    int iNrOrientationPorts, int iNrMatrixPorts)
    : IVdfnNode()
    , m_pFrame(pFrame)
    , m_bTransformFromFrame(bTransformToFrame) {
  m_vecInPortsPos.resize(iNrPositionPorts);
  m_vecOutPortsPos.resize(iNrPositionPorts);
  for (int i = 0; i < iNrPositionPorts; ++i) {
    m_vecOutPortsPos[i] = new TVdfnPort<VistaVector3D>;
    RegisterInPortPrototype("in_position" + VistaConversion::ToString(i),
        new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
    RegisterOutPort("out_position" + VistaConversion::ToString(i), m_vecOutPortsPos[i]);
  }

  m_vecInPortsOri.resize(iNrOrientationPorts);
  m_vecOutPortsOri.resize(iNrOrientationPorts);
  for (int i = 0; i < iNrOrientationPorts; ++i) {
    m_vecOutPortsOri[i] = new TVdfnPort<VistaQuaternion>;
    RegisterInPortPrototype("in_orientation" + VistaConversion::ToString(i),
        new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
    RegisterOutPort("out_orientation" + VistaConversion::ToString(i), m_vecOutPortsOri[i]);
  }

  m_vecInPortsMat.resize(iNrMatrixPorts);
  m_vecOutPortsMat.resize(iNrMatrixPorts);
  for (int i = 0; i < iNrMatrixPorts; ++i) {
    m_vecOutPortsMat[i] = new TVdfnPort<VistaTransformMatrix>;
    RegisterInPortPrototype("in_matrix" + VistaConversion::ToString(i),
        new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix>>);
    RegisterOutPort("out_matrix" + VistaConversion::ToString(i), m_vecOutPortsMat[i]);
  }
}

VistaDfnReferenceFrameTransformNode::~VistaDfnReferenceFrameTransformNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDfnReferenceFrameTransformNode::GetIsValid() const {
  return (m_pFrame != NULL);
}

bool VistaDfnReferenceFrameTransformNode::PrepareEvaluationRun() {
  int                                              iMax      = (int)m_vecOutPortsPos.size();
  std::vector<TVdfnPort<VistaVector3D>*>::iterator itPortPos = m_vecInPortsPos.begin();
  for (int i = 0; i < iMax; ++i, ++itPortPos) {
    (*itPortPos) = VdfnUtil::GetInPortTyped<TVdfnPort<VistaVector3D>*>(
        "in_position" + VistaConversion::ToString(i), this);
  }
  for (int i = (int)m_vecInPortsPos.size() - 1; i >= 0; --i) {
    if (m_vecInPortsPos[i] == NULL) {
      m_vecInPortsPos.erase(m_vecInPortsPos.begin() + i);
      m_vecOutPortsPos.erase(m_vecOutPortsPos.begin() + i);
    }
  }

  iMax                                                         = (int)m_vecOutPortsOri.size();
  std::vector<TVdfnPort<VistaQuaternion>*>::iterator itPortOri = m_vecInPortsOri.begin();
  for (int i = 0; i < iMax; ++i, ++itPortOri) {
    (*itPortOri) = VdfnUtil::GetInPortTyped<TVdfnPort<VistaQuaternion>*>(
        "in_orientation" + VistaConversion::ToString(i), this);
  }
  for (int i = (int)m_vecInPortsOri.size() - 1; i >= 0; --i) {
    if (m_vecInPortsOri[i] == NULL) {
      m_vecInPortsOri.erase(m_vecInPortsOri.begin() + i);
      m_vecOutPortsOri.erase(m_vecOutPortsOri.begin() + i);
    }
  }

  iMax                                                              = (int)m_vecOutPortsMat.size();
  std::vector<TVdfnPort<VistaTransformMatrix>*>::iterator itPortMat = m_vecInPortsMat.begin();
  for (int i = 0; i < iMax; ++i, ++itPortMat) {
    (*itPortMat) = VdfnUtil::GetInPortTyped<TVdfnPort<VistaTransformMatrix>*>(
        "in_matrix" + VistaConversion::ToString(i), this);
  }
  for (int i = (int)m_vecInPortsMat.size() - 1; i >= 0; --i) {
    if (m_vecInPortsMat[i] == NULL) {
      m_vecInPortsMat.erase(m_vecInPortsMat.begin() + i);
      m_vecOutPortsMat.erase(m_vecOutPortsMat.begin() + i);
    }
  }

  return GetIsValid();
}

bool VistaDfnReferenceFrameTransformNode::DoEvalNode() {
  VistaTransformMatrix matTransform;
  if (m_bTransformFromFrame)
    m_pFrame->GetMatrixInverse(matTransform);
  else
    m_pFrame->GetMatrix(matTransform);

  if (m_vecInPortsPos.empty() == false) {
    std::vector<TVdfnPort<VistaVector3D>*>::const_iterator citPosIn = m_vecInPortsPos.begin();
    std::vector<TVdfnPort<VistaVector3D>*>::iterator       itPosOut = m_vecOutPortsPos.begin();
    VistaVector3D                                          v3Pos;
    for (; citPosIn != m_vecInPortsPos.end(); ++citPosIn, ++itPosOut) {
      (*citPosIn)->GetValue(v3Pos);
      v3Pos = matTransform.Transform(v3Pos);
      (*itPosOut)->SetValue(v3Pos, GetUpdateTimeStamp());
    }
  }

  if (m_vecInPortsOri.empty() == false) {
    std::vector<TVdfnPort<VistaQuaternion>*>::const_iterator citOriIn = m_vecInPortsOri.begin();
    std::vector<TVdfnPort<VistaQuaternion>*>::iterator       itOriOut = m_vecOutPortsOri.begin();
    VistaQuaternion                                          qOri;
    for (; citOriIn != m_vecInPortsOri.end(); ++citOriIn, ++itOriOut) {
      (*citOriIn)->GetValue(qOri);
      qOri = VistaQuaternion(matTransform * VistaTransformMatrix(qOri));
      (*itOriOut)->SetValue(qOri, GetUpdateTimeStamp());
    }
  }

  if (m_vecInPortsMat.empty() == false) {
    std::vector<TVdfnPort<VistaTransformMatrix>*>::const_iterator citMatIn =
        m_vecInPortsMat.begin();
    std::vector<TVdfnPort<VistaTransformMatrix>*>::iterator itMatOut = m_vecOutPortsMat.begin();
    VistaTransformMatrix                                    matMat;
    for (; citMatIn != m_vecInPortsMat.end(); ++citMatIn, ++itMatOut) {
      (*citMatIn)->GetValue(matMat);
      matMat = matTransform * matMat;
      (*itMatOut)->SetValue(matMat, GetUpdateTimeStamp());
    }
  }
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
