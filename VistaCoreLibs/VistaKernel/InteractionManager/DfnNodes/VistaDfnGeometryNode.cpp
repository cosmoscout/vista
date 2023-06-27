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

#include "VistaDfnGeometryNode.h"

#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaDfnGeometryNode::VistaDfnGeometryNode(VistaGeometry* pGeometry, VistaSceneGraph* pSceneGraph)
    : IVdfnNode()
    , m_pOffsetNode(NULL)
    , m_bDeleteSubtree(true)
    , m_nTransformUpdate(0)
    , m_nPositionPortUpdate(0)
    , m_nOrientationPortUpdate(0)
    , m_nScalePortUpdate(0)
    , m_nUniformScalePortUpdate(0)
    , m_nEnableUpdate(0)
    , m_pGeometry(pGeometry)
    , m_pPositionPort(NULL)
    , m_pScalePort(NULL)
    , m_pUniformScalePort(NULL)
    , m_pOrientationPort(NULL)
    , m_pTransformPort(NULL)
    , m_pEnablePort(NULL) {
  RegisterInPortPrototype("position", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("orientation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
  RegisterInPortPrototype("scale", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("transform", new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix>>);
  RegisterInPortPrototype("enable", new TVdfnPortTypeCompare<TVdfnPort<bool>>);

  m_pTransformNode = pSceneGraph->NewTransformNode(pSceneGraph->GetRoot());
  m_pTransformNode->SetName("DfnGeomNode-" + GetNameForNameable() + "-transform");
  m_pOffsetNode = pSceneGraph->NewTransformNode(m_pTransformNode);
  m_pOffsetNode->SetName("DfnGeomNode-" + GetNameForNameable() + "-offset");
  VistaGeomNode* pGeomNode = pSceneGraph->NewGeomNode(m_pOffsetNode, m_pGeometry);
  pGeomNode->SetName("DfnGeomNode-" + GetNameForNameable() + "-geometry");
}

VistaDfnGeometryNode::VistaDfnGeometryNode(
    IVistaNode* pSubtreeRoot, VistaSceneGraph* pSceneGraph, bool bDeleteSubtree)
    : IVdfnNode()
    , m_pOffsetNode(NULL)
    , m_bDeleteSubtree(bDeleteSubtree)
    , m_nTransformUpdate(0)
    , m_nPositionPortUpdate(0)
    , m_nOrientationPortUpdate(0)
    , m_nScalePortUpdate(0)
    , m_nUniformScalePortUpdate(0)
    , m_nEnableUpdate(0)
    , m_pGeometry(NULL)
    , m_pPositionPort(NULL)
    , m_pScalePort(NULL)
    , m_pUniformScalePort(NULL)
    , m_pOrientationPort(NULL)
    , m_pTransformPort(NULL)
    , m_pEnablePort(NULL) {
  RegisterInPortPrototype("position", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("orientation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
  RegisterInPortPrototype("scale", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("uniform_scale", new TVdfnPortTypeCompare<TVdfnPort<float>>);
  RegisterInPortPrototype("transform", new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix>>);
  RegisterInPortPrototype("enable", new TVdfnPortTypeCompare<TVdfnPort<bool>>);

  m_pTransformNode = pSceneGraph->NewTransformNode(pSceneGraph->GetRoot());
  m_pOffsetNode    = pSceneGraph->NewTransformNode(m_pTransformNode);
  m_pOffsetNode->SetName("DfnGeomNode-" + GetNameForNameable() + "-offset");
  m_pOffsetNode->AddChild(pSubtreeRoot);
}

VistaDfnGeometryNode::VistaDfnGeometryNode(
    const VistaPropertyList& oConfig, VistaSceneGraph* pSceneGraph)
    : IVdfnNode()
    , m_pTransformNode(NULL)
    , m_pOffsetNode(NULL)
    , m_bDeleteSubtree(true)
    , m_nTransformUpdate(0)
    , m_nPositionPortUpdate(0)
    , m_nOrientationPortUpdate(0)
    , m_nScalePortUpdate(0)
    , m_nUniformScalePortUpdate(0)
    , m_nEnableUpdate(0) {
  RegisterInPortPrototype("position", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("orientation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
  RegisterInPortPrototype("scale", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("uniform_scale", new TVdfnPortTypeCompare<TVdfnPort<float>>);
  RegisterInPortPrototype("transform", new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix>>);
  RegisterInPortPrototype("enable", new TVdfnPortTypeCompare<TVdfnPort<bool>>);

  VistaGeometryFactory oFactory(pSceneGraph);
  m_pGeometry = oFactory.CreateFromPropertyList(oConfig);
  if (m_pGeometry != NULL) {
    m_pTransformNode = pSceneGraph->NewTransformNode(pSceneGraph->GetRoot());
    m_pTransformNode->SetName("DfnGeomNode-" + GetNameForNameable() + "-transform");
    m_pOffsetNode = pSceneGraph->NewTransformNode(m_pTransformNode);
    m_pOffsetNode->SetName("DfnGeomNode-" + GetNameForNameable() + "-offset");
    VistaGeomNode* pGeomNode = pSceneGraph->NewGeomNode(m_pOffsetNode, m_pGeometry);
    pGeomNode->SetName("DfnGeomNode-" + GetNameForNameable() + "-geometry");
  }
}
VistaDfnGeometryNode::~VistaDfnGeometryNode() {
  if (m_bDeleteSubtree == false) {
    while (m_pOffsetNode->GetNumChildren() > 0)
      m_pOffsetNode->DisconnectChild((unsigned int)0);
  }
  delete m_pTransformNode;
}

bool VistaDfnGeometryNode::PrepareEvaluationRun() {
  m_pPositionPort     = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("position"));
  m_pOrientationPort  = dynamic_cast<TVdfnPort<VistaQuaternion>*>(GetInPort("orientation"));
  m_pScalePort        = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("scale"));
  m_pUniformScalePort = dynamic_cast<TVdfnPort<float>*>(GetInPort("uniform_scale"));
  m_pTransformPort    = dynamic_cast<TVdfnPort<VistaTransformMatrix>*>(GetInPort("transform"));
  m_pEnablePort       = dynamic_cast<TVdfnPort<bool>*>(GetInPort("enable"));

  return GetIsValid();
}

bool VistaDfnGeometryNode::GetIsValid() const {
  // we dont need any inports, geometries may exist for themselves
  return (m_pTransformNode != NULL);
}

bool VistaDfnGeometryNode::DoEvalNode() {
  VistaTransformMatrix oTransform;
  if (m_pScalePort && m_pScalePort->GetUpdateCounter() > m_nScalePortUpdate) {
    m_pTransformNode->SetScale(m_pScalePort->GetValue());
    m_nScalePortUpdate = m_pScalePort->GetUpdateCounter();
  }
  if (m_pUniformScalePort && m_pUniformScalePort->GetUpdateCounter() > m_nUniformScalePortUpdate) {
    m_pTransformNode->SetScale(m_pUniformScalePort->GetValue(), m_pUniformScalePort->GetValue(),
        m_pUniformScalePort->GetValue());
    m_nScalePortUpdate = m_pUniformScalePort->GetUpdateCounter();
  }
  if (m_pOrientationPort && m_pOrientationPort->GetUpdateCounter() > m_nOrientationPortUpdate) {
    m_pTransformNode->SetRotation(m_pOrientationPort->GetValue());
    m_nOrientationPortUpdate = m_pOrientationPort->GetUpdateCounter();
  }
  if (m_pPositionPort && m_pPositionPort->GetUpdateCounter() > m_nPositionPortUpdate) {
    m_pTransformNode->SetTranslation(m_pPositionPort->GetValue());
    m_nPositionPortUpdate = m_pPositionPort->GetUpdateCounter();
  }
  if (m_pTransformPort && m_pTransformPort->GetUpdateCounter() > m_nTransformUpdate) {
    m_pTransformNode->SetTransform(m_pTransformPort->GetValue());
    m_nTransformUpdate = m_pTransformPort->GetUpdateCounter();
  }
  if (m_pEnablePort && m_pEnablePort->GetUpdateCounter() > m_nEnableUpdate) {
    m_pTransformNode->SetIsEnabled(m_pEnablePort->GetValue());
    m_nEnableUpdate = m_pEnablePort->GetUpdateCounter();
  }

  return true;
}

VistaTransformNode* VistaDfnGeometryNode::GetOffsetNode() const {
  return m_pOffsetNode;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
