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

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(disable : 4231)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#include "VistaOpenSGNodeBridge.h"

#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/OpenSG/OSGVistaOpenGLDrawCore.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>

#include <VistaKernel/GraphicsManager/Vista3DText.h>
#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaLODNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSwitchNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaFileSystemFile.h>
#include <VistaTools/VistaIniFileParser.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <VistaAspects/VistaConversion.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSwitch.h>

#include <OpenSG/OSGDirectionalLight.h>
#include <OpenSG/OSGPointLight.h>
#include <OpenSG/OSGSpotLight.h>

#include <OpenSG/OSGAttachment.h>
#include <OpenSG/OSGSimpleAttachments.h>

#include <OpenSG/OSGDistanceLOD.h>
#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGSceneFileHandler.h> // used for VRML File IO
#include <OpenSG/OSGSimpleMaterial.h>

// scene graph optimization
#include <OpenSG/OSGGraphOpSeq.h>
#include <OpenSG/OSGMaterialMergeGraphOp.h>
#include <OpenSG/OSGMergeGraphOp.h>
#include <OpenSG/OSGPruneGraphOp.h>
#include <OpenSG/OSGSharePtrGraphOp.h>
#include <OpenSG/OSGSplitGraphOp.h>
#include <OpenSG/OSGStripeGraphOp.h>
#include <OpenSG/OSGVerifyGraphOp.h>

#include <OpenSG/OSGTextLayoutParam.h>
#include <OpenSG/OSGTextLayoutResult.h>
#include <OpenSG/OSGTextVectorFace.h>

/*============================================================================*/
/*  MAKROS AND DEFINES AND STATICS                                            */
/*============================================================================*/
using namespace std;

/** @todo eliminate defines!!!! */
#define OSG_LIGHTALPHA_DEFAULT 1

// OpenSG traverser
class LightOfBeaconFinder {
 public:
  LightOfBeaconFinder()
      : m_ptrBeacon(osg::NullFC)
      , m_ptrLightNode(osg::NullFC) {
  }

  osg::NodePtr find(osg::NodePtr& ptrBeacon, osg::NodePtr& ptrRoot) {
    m_ptrBeacon    = ptrBeacon;
    m_ptrLightNode = osg::NullFC;

    // and now welcome that very intuitive OpenSG traversal call...
    osg::traverse(ptrRoot,
        osg::osgTypedMethodFunctor1ObjPtrCPtrRef<osg::Action::ResultE, LightOfBeaconFinder,
            osg::NodePtr>(this, &LightOfBeaconFinder::enter),
        osg::osgTypedMethodFunctor2ObjPtrCPtrRef<osg::Action::ResultE, LightOfBeaconFinder,
            osg::NodePtr, osg::Action::ResultE>(this, &LightOfBeaconFinder::leave));

    return m_ptrLightNode;
  }

 private:
  osg::NodePtr m_ptrBeacon;
  osg::NodePtr m_ptrLightNode;

  osg::Action::ResultE enter(osg::NodePtr& node) {
    if (node->getCore()->getType().isDerivedFrom(osg::Light::getClassType())) {
      osg::LightPtr light = osg::LightPtr::dcast(node->getCore());
      if (m_ptrBeacon == light->getBeacon()) {
        m_ptrLightNode = node;
        return osg::Action::Quit;
      }
    }
    return osg::Action::Continue;
  }

  osg::Action::ResultE leave(osg::NodePtr& node, osg::Action::ResultE res) {
    return res;
  }
};

static IVistaNode* dive(VistaOpenSGNodeBridge* pNodeBridge,
    VistaOpenSGGraphicsBridge* pGraphicsBridge, const osg::NodePtr& node) {
  IVistaNode* pNode = NULL;

  // cout << node->getCore()->getTypeName() << std::endl;

  if (node->getCore()->getType().isDerivedFrom(osg::Transform::getClassType())) {
    VistaOpenSGNodeData* pd =
        static_cast<VistaOpenSGNodeData*>(pNodeBridge->NewTransformNodeData());
    const osg::Char8* name = osg::getName(node);
    pd->SetCore(node->getCore());

    VistaTransformNode* group = pNodeBridge->NewTransformNode(NULL, pd, name ? name : "<none>");

    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }
    // osg::TransformPtr trans = osg::TransformPtr::dcast(node->getCore());
    // pd->SetTransform(trans->getMatrix());

    pNode = group;

  } else if (node->getCore()->getType().isDerivedFrom(osg::DistanceLOD::getClassType())) {
    VistaOpenSGNodeData* pd = dynamic_cast<VistaOpenSGNodeData*>(pNodeBridge->NewLODNodeData());
    pd->SetCore(node->getCore());
    const osg::Char8* name  = osg::getName(node);
    VistaLODNode*     group = pNodeBridge->NewLODNode(NULL, pd, name ? name : "<none>");
    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }
    pNode = group;
  } else if (node->getCore()->getType().isDerivedFrom(osg::Switch::getClassType())) {
    VistaOpenSGNodeData* pd = dynamic_cast<VistaOpenSGNodeData*>(pNodeBridge->NewSwitchNodeData());
    pd->SetCore(node->getCore());
    const osg::Char8* name  = osg::getName(node);
    VistaSwitchNode*  group = pNodeBridge->NewSwitchNode(NULL, pd, name ? name : "<none>");
    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }

    pNode = group;
  } else if (node->getCore()->getType().isDerivedFrom(osg::Group::getClassType())) {
    // its a group
    // 			cout << "node has [" << node->getNChildren() << "] children\n";
    VistaOpenSGNodeData* pd = static_cast<VistaOpenSGNodeData*>(pNodeBridge->NewGroupNodeData());
    pd->SetCore(node->getCore());
    const osg::Char8* name  = osg::getName(node);
    VistaGroupNode*   group = pNodeBridge->NewGroupNode(NULL, pd, name ? name : "<none>");

    // 			cout << "node has [" << node->getNChildren() << "] children\n";
    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }

    pNode = group;
  } else if (node->getCore()->getType().isDerivedFrom(osg::Geometry::getClassType())) {
    VistaOpenSGGeometryData* pd =
        static_cast<VistaOpenSGGeometryData*>(pGraphicsBridge->NewGeometryData());
    pd->SetGeometry(osg::GeometryPtr::dcast(node->getCore()));
    IVistaNodeData*   nd   = pNodeBridge->NewGeomNodeData(pd);
    VistaGeometry*    geom = pGraphicsBridge->NewGeometry(pd);
    const osg::Char8* name = osg::getName(node);
    pNode                  = pNodeBridge->NewGeomNode(NULL, geom, nd, name ? name : "<none>");
  } else if (node->getCore()->getType().isDerivedFrom(osg::Light::getClassType())) {
  } else {
    VistaOpenSGExtensionNodeData* nd =
        dynamic_cast<VistaOpenSGExtensionNodeData*>(pNodeBridge->NewExtensionNodeData());
    if (nd)
      nd->SetNode(node);
    // attach to parent
    const osg::Char8* name = osg::getName(node);
    pNode                  = pNodeBridge->NewExtensionNode(NULL, NULL, nd, name ? name : "<none>");
  }

  return pNode;
}

static IVistaNode* NameCopyDive(VistaOpenSGNodeBridge* pNodeBridge,
    VistaOpenSGGraphicsBridge* pGraphicsBridge, const osg::NodePtr& node,
    const osg::NodePtr& sourcenode) {
  IVistaNode* pNode = NULL;

  // cout << node->getCore()->getTypeName() << std::endl;
#ifdef DEBUG
  if (node->getType() != sourcenode->getType()) {
    vstr::warnp() << "[OpenSG::CloneNode]: source and copy have different types!" << std::endl;
  }
#endif

  if (node->getCore()->getType().isDerivedFrom(osg::Transform::getClassType())) {
    VistaOpenSGNodeData* pd =
        static_cast<VistaOpenSGNodeData*>(pNodeBridge->NewTransformNodeData());
    const osg::Char8* name = osg::getName(sourcenode);
    osg::setName(node, name);
    pd->SetCore(node->getCore());

    VistaTransformNode* group = pNodeBridge->NewTransformNode(NULL, pd, name ? name : "<none>");

    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode;
      if (sourcenode->getNChildren() < n + 1) {
        vstr::warnp() << "[OpenSG::CloneNode]: source and copy have different children count!"
                      << std::endl;
        subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      } else
        subnode =
            NameCopyDive(pNodeBridge, pGraphicsBridge, node->getChild(n), sourcenode->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }
    // osg::TransformPtr trans = osg::TransformPtr::dcast(node->getCore());
    // pd->SetTransform(trans->getMatrix());

    pNode = group;

  } else if (node->getCore()->getType().isDerivedFrom(osg::DistanceLOD::getClassType())) {
    VistaOpenSGNodeData* pd = dynamic_cast<VistaOpenSGNodeData*>(pNodeBridge->NewLODNodeData());
    pd->SetCore(node->getCore());
    const osg::Char8* name = osg::getName(sourcenode);
    osg::setName(node, name);
    VistaLODNode* group = pNodeBridge->NewLODNode(NULL, pd, name ? name : "<none>");
    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode;
      if (sourcenode->getNChildren() < n + 1) {
        vstr::warnp() << "[OpenSG::CloneNode]: source and copy have different children count!"
                      << std::endl;
        subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      } else
        subnode =
            NameCopyDive(pNodeBridge, pGraphicsBridge, node->getChild(n), sourcenode->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }
    pNode = group;
  } else if (node->getCore()->getType().isDerivedFrom(osg::Switch::getClassType())) {
    VistaOpenSGNodeData* pd = dynamic_cast<VistaOpenSGNodeData*>(pNodeBridge->NewSwitchNodeData());
    pd->SetCore(node->getCore());
    const osg::Char8* name = osg::getName(sourcenode);
    osg::setName(node, name);
    VistaSwitchNode* group = pNodeBridge->NewSwitchNode(NULL, pd, name ? name : "<none>");
    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode;
      if (sourcenode->getNChildren() < n + 1) {
        vstr::warnp() << "[OpenSG::CloneNode]: source and copy have different children count!"
                      << std::endl;
        subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      } else
        subnode =
            NameCopyDive(pNodeBridge, pGraphicsBridge, node->getChild(n), sourcenode->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }

    pNode = group;
  } else if (node->getCore()->getType().isDerivedFrom(osg::Group::getClassType())) {
    // its a group
    // 			cout << "node has [" << node->getNChildren() << "] children\n";
    VistaOpenSGNodeData* pd = static_cast<VistaOpenSGNodeData*>(pNodeBridge->NewGroupNodeData());
    pd->SetCore(node->getCore());
    const osg::Char8* name = osg::getName(sourcenode);
    osg::setName(node, name);
    VistaGroupNode* group = pNodeBridge->NewGroupNode(NULL, pd, name ? name : "<none>");

    // 			cout << "node has [" << node->getNChildren() << "] children\n";
    for (osg::UInt32 n = 0; n < node->getNChildren(); ++n) {
      IVistaNode* subnode;
      if (sourcenode->getNChildren() < n + 1) {
        vstr::warnp() << "[OpenSG::CloneNode]: source and copy have different children count!"
                      << std::endl;
        subnode = dive(pNodeBridge, pGraphicsBridge, node->getChild(n));
      } else
        subnode =
            NameCopyDive(pNodeBridge, pGraphicsBridge, node->getChild(n), sourcenode->getChild(n));
      if (subnode)
        group->AddChild(subnode);
    }

    pNode = group;
  } else if (node->getCore()->getType().isDerivedFrom(osg::Geometry::getClassType())) {
    VistaOpenSGGeometryData* pd =
        static_cast<VistaOpenSGGeometryData*>(pGraphicsBridge->NewGeometryData());
    pd->SetGeometry(osg::GeometryPtr::dcast(node->getCore()));
    IVistaNodeData*   nd   = pNodeBridge->NewGeomNodeData(pd);
    VistaGeometry*    geom = pGraphicsBridge->NewGeometry(pd);
    const osg::Char8* name = osg::getName(sourcenode);
    osg::setName(node, name);
    pNode = pNodeBridge->NewGeomNode(NULL, geom, nd, name ? name : "<none>");
  } else if (node->getCore()->getType().isDerivedFrom(osg::Light::getClassType())) {
  } else {
    VistaOpenSGExtensionNodeData* nd =
        dynamic_cast<VistaOpenSGExtensionNodeData*>(pNodeBridge->NewExtensionNodeData());
    if (nd)
      nd->SetNode(node);
    // attach to parent
    const osg::Char8* name = osg::getName(sourcenode);
    osg::setName(node, name);
    pNode = pNodeBridge->NewExtensionNode(NULL, NULL, nd, name ? name : "<none>");
  }

  return pNode;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaOpenSGExtensionNodeData::VistaOpenSGExtensionNodeData() {
}

VistaOpenSGExtensionNodeData::~VistaOpenSGExtensionNodeData() {
}

VistaOpenSGNodeData::VistaOpenSGNodeData() {
  m_ptrNode = osg::Node::create();
  if (!m_ptrNode)
    VISTA_THROW("COULD NOT CREATE OPENSG-NODE", 0xDEAD);
}

VistaOpenSGNodeData::~VistaOpenSGNodeData() {
}

osg::NodePtr VistaOpenSGNodeData::GetNode() const {
  return m_ptrNode;
}

osg::NodeCorePtr VistaOpenSGNodeData::GetCore() const {
  return m_ptrNode->getCore();
}

bool VistaOpenSGNodeData::SetNode(const osg::NodePtr& node) {
  m_ptrNode = node;
  return true;
}

bool VistaOpenSGNodeData::SetCore(const osg::NodeCorePtr& core) {
  beginEditCP(m_ptrNode, osg::Node::CoreFieldMask);
  m_ptrNode->setCore(core);
  endEditCP(m_ptrNode, osg::Node::CoreFieldMask);

  return true;
}

VistaOpenSGGeomNodeData::VistaOpenSGGeomNodeData()
    : VistaOpenSGNodeData() {
}

VistaOpenSGGeomNodeData::~VistaOpenSGGeomNodeData() {
}

osg::NodeCorePtr VistaOpenSGGeomNodeData::GetCore() const {
  // for debugging
  // return osg::NullFC;

  return m_ptrNode->getCore();
}

bool VistaOpenSGGeomNodeData::SetCore(const osg::NodeCorePtr& core) {
  beginEditCP(m_ptrNode, osg::Node::CoreFieldMask);
  m_ptrNode->setCore(core);
  endEditCP(m_ptrNode, osg::Node::CoreFieldMask);
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::NewRootNodeData                       */
/*                                                                            */
/*============================================================================*/

VistaOpenSGNodeBridge::VistaOpenSGNodeBridge()
    : IVistaNodeBridge()
    , m_afAmbientLight(0.0f, 0.0f, 0.0f, 1.0f)
    , m_bAmbientLightState(false) {
}

VistaOpenSGNodeBridge::~VistaOpenSGNodeBridge() {
}

IVistaNodeData* VistaOpenSGNodeBridge::NewRootNodeData() {
  VistaOpenSGNodeData* pNewData = new VistaOpenSGNodeData;
  if (!pNewData) {
    printf("Creation of VistaOpenSGNodeData failed!");
    return NULL;
  }

  pNewData->SetCore(osg::Transform::create());

  return pNewData;
}
/*=============================================================================*/
/*                                                                             */
/*  NAME    :   VistaOpenSGNodeBridge::NewGroupNodeData                       */
/*                                                                             */
/*=============================================================================*/
IVistaNodeData* VistaOpenSGNodeBridge::NewGroupNodeData() {
  VistaOpenSGNodeData* pNewData = new VistaOpenSGNodeData;
  if (!pNewData) {
    printf("Creation of VistaOpenSGNodeData failed!");
    return NULL;
  }

  pNewData->SetCore(osg::Group::create());

  return pNewData;
}

IVistaNodeData* VistaOpenSGNodeBridge::NewTransformNodeData() {
  VistaOpenSGNodeData* pNewData = new VistaOpenSGNodeData;
  if (!pNewData) {
    printf("Creation of VistaOpenSGNodeData failed!");
    return NULL;
  }

  pNewData->SetCore(osg::Transform::create());

  return pNewData;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::NewSwitchNodeData                     */
/*                                                                            */
/*============================================================================*/
IVistaNodeData* VistaOpenSGNodeBridge::NewSwitchNodeData() {
  VistaOpenSGNodeData* pNewData = new VistaOpenSGNodeData;
  if (!pNewData) {
    printf("Creation of VistaOpenSGNodeData failed!");
    return NULL;
  }

  pNewData->SetCore(osg::Switch::create());

  return pNewData;
}

IVistaNodeData* VistaOpenSGNodeBridge::NewExtensionNodeData() {
  return new VistaOpenSGExtensionNodeData;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::NewLODNodeData                        */
/*                                                                            */
/*============================================================================*/
IVistaNodeData* VistaOpenSGNodeBridge::NewLODNodeData() {
  VistaOpenSGNodeData* pNewData = new VistaOpenSGNodeData;

  if (!pNewData) {
    printf("Creation of VistaOpenSGNodeData failed!");
    return NULL;
  }

  pNewData->SetCore(osg::DistanceLOD::create());

  return pNewData;
}

bool VistaOpenSGNodeBridge::SetCenter(const VistaVector3D& center, IVistaNodeData* pData) {
  VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  osg::DistanceLODPtr  lod         = osg::DistanceLODPtr::dcast(pOpenSGData->GetCore());
  if (lod == osg::NullFC)
    return false;

  beginEditCP(lod);
  lod->setCenter(osg::Pnt3f(center[0], center[1], center[2]));
  endEditCP(lod);
  return true;
}

bool VistaOpenSGNodeBridge::GetCenter(VistaVector3D& center, const IVistaNodeData* pData) const {
  const VistaOpenSGNodeData* pOpenSGData = static_cast<const VistaOpenSGNodeData*>(pData);
  osg::DistanceLODPtr        lod         = osg::DistanceLODPtr::dcast(pOpenSGData->GetCore());
  if (lod == osg::NullFC)
    return false;

  osg::Pnt3f cen = lod->getCenter();

  center[0] = cen[0];
  center[1] = cen[1];
  center[2] = cen[2];

  return true;
}

bool VistaOpenSGNodeBridge::SetRange(const std::vector<float>& rangeList, IVistaNodeData* pData) {
  VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  osg::DistanceLODPtr  lod         = osg::DistanceLODPtr::dcast(pOpenSGData->GetCore());
  if (lod == osg::NullFC)
    return false;

  if (rangeList.empty()) {
    lod->getMFRange()->clear(); // simply clear list
  } else {
    unsigned int i, n(rangeList.size());
    beginEditCP(lod);
    {
      lod->getMFRange()->clear();
      for (i = 0; i < n; ++i)
        lod->getMFRange()->push_back(rangeList[i]);
    }
    endEditCP(lod);
  }
  return true;
}

bool VistaOpenSGNodeBridge::GetRange(
    std::vector<float>& rangeList, const IVistaNodeData* pData) const {
  const VistaOpenSGNodeData* pOpenSGData = static_cast<const VistaOpenSGNodeData*>(pData);
  osg::DistanceLODPtr        lod         = osg::DistanceLODPtr::dcast(pOpenSGData->GetCore());

  if (lod == osg::NullFC)
    return false;

  rangeList.clear();

  unsigned int i, n(lod->getMFRange()->size());
  rangeList.resize(n);

  for (i = 0; i < n; ++i)
    rangeList[i] = (*lod->getMFRange())[i];

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::NewGeomNodeData                       */
/*                                                                            */
/*============================================================================*/
IVistaNodeData* VistaOpenSGNodeBridge::NewGeomNodeData(IVistaGeometryData* pData) {
  if (!pData) {
    printf("VistaOpenSGNodeBridge::NewGeomNodeData: Got NULL as argument!");
    return NULL;
  }

  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  VistaOpenSGNodeData* pNewData = new VistaOpenSGGeomNodeData;

  pNewData->SetCore(pOpenSGData->GetGeometry());

  return pNewData;
}

void VistaOpenSGNodeBridge::DeleteNode(IVistaNode* pNode) {
  IVistaNodeBridge::DeleteNode(pNode);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::NewLightNodeData                      */
/*                                                                            */
/*============================================================================*/
IVistaNodeData* VistaOpenSGNodeBridge::NewLightNodeData(VISTA_LIGHTTYPE eLightType) {
  VistaOpenSGLightNodeData* pLightData = new VistaOpenSGLightNodeData;
  if (!pLightData) {
    printf("Creation of VistaOpenSGNodeData failed!");
    return NULL;
  }

  pLightData->m_nLightType = (int)eLightType;

  osg::LightRefPtr lightCore(osg::NullFC);
  osg::NodeRefPtr  lightNode(osg::Node::create());

  // the ViSTA-node part (light's beacon)
  pLightData->SetCore(osg::Group::create());
  pLightData->m_ptrLightNode = lightNode;

  switch (eLightType) {
    // there seems to be no AMBIENT_LIGHT in OpenSG,
  case VISTA_AMBIENT_LIGHT: {
    if (m_bAmbientLightState) {
      delete pLightData;
      return NULL; // only one ambient light allowed!
    }
    m_bAmbientLightState = true;
    beginEditCP(lightNode, osg::Node::CoreFieldId);
    lightNode->setCore(osg::Group::create());
    endEditCP(lightNode, osg::Node::CoreFieldId);
    break;
  }
  case VISTA_DIRECTIONAL_LIGHT: {
    lightCore = osg::DirectionalLight::create();
    break;
  }

  case VISTA_POINT_LIGHT: {
    lightCore = osg::PointLight::create();
    break;
  }
  case VISTA_SPOT_LIGHT: {
    /*
     * Rationale behind the default values for the spot light
     * - we set 45.0f as default for cut off spot lights, resulting
             in nice torch-like spot light, which is usually what a
             user wants when creating spot lights
     * - on the contrary, we use 0 for the distribution, resulting
             in a uniform distribution of the light in the spot
     * - we set the default direction to -z, which might be convinient
             for graphic users.
    */
    osg::SpotLightRefPtr spotLightPtr(osg::SpotLight::create());
    beginEditCP(spotLightPtr);
    spotLightPtr->setSpotExponent(0);
    spotLightPtr->setSpotDirection(0, 0, -1);
    spotLightPtr->setSpotCutOffDeg(45.0f);
    endEditCP(spotLightPtr);

    lightCore = spotLightPtr;

    break;
  }
  default: {
    vstr::errp() << "[VistaOpenSGNodeBridge::NewLightNodeData] Unsupported light type ("
                 << eLightType << ")" << std::endl;
    delete pLightData;
    pLightData = NULL;
    return NULL;
  }
  }

  if (lightCore) {
    // set light beacon (the ViSTA-Node)
    beginEditCP(lightCore, osg::Light::BeaconFieldMask);
    lightCore->setBeacon(pLightData->GetNode());
    endEditCP(lightCore, osg::Light::BeaconFieldMask);

    // set light core
    beginEditCP(lightNode, osg::Node::CoreFieldMask);
    lightNode->setCore(lightCore);
    endEditCP(lightNode, osg::Node::CoreFieldMask);

    // set light shadow intensity
#if OSG_MAJOR_VERSION == 1 && OSG_MINOR_VERSION >= 8
    beginEditCP(lightCore, OSG::Light::ShadowIntensityFieldMask);
    lightCore->setShadowIntensity(pLightData->m_fShadowIntensity);
    endEditCP(lightCore, OSG::Light::ShadowIntensityFieldMask);
#endif
  }

  // move light node down to the root (...to enlighten the whole world...)
  //----------------------------------------------------------------------

  VistaOpenSGNodeData* pRootData =
      (VistaOpenSGNodeData*)(GetVistaSceneGraph()->GetRealRoot()->GetData());
  osg::NodePtr rootNode = pRootData->GetNode();

  // re-parent the existing scenegraph under the light node
  if (rootNode->getNChildren() > 0) {
    // reparent all children under the light node
    while (rootNode->getNChildren() > 0) {
      // this auto-removes the node from the old root node, too
      osg::beginEditCP(lightNode, osg::Node::TravMaskFieldMask);
      lightNode->addChild(rootNode->getChild(0));
      osg::endEditCP(lightNode, osg::Node::TravMaskFieldMask);
    }
  }

  // now add/reparent the light node
  beginEditCP(rootNode, osg::Node::TravMaskFieldMask);
  rootNode->addChild(lightNode);
  endEditCP(rootNode, osg::Node::TravMaskFieldMask);

  /**************************************************************
   * @todo how/where to handle removal/disconnect of light nodes!?
   */

  return pLightData;
}
/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::NewOpenGLNodeData                     */
/*                                                                            */
/*============================================================================*/
IVistaNodeData* VistaOpenSGNodeBridge::NewOpenGLNodeData() {
  VistaOpenSGOpenGLNodeData* pNewData = new VistaOpenSGOpenGLNodeData;

  if (!pNewData) {
    printf("Creation of VistaOpenSGOpenGLNodeData failed!");
    return NULL;
  }

  return pNewData;
}

// VistaOpenSGTextNodeDraw::VistaOpenSGTextNodeDraw()
//{
//	m_pText = NULL;
//}
//
// VistaOpenSGTextNodeDraw::~VistaOpenSGTextNodeDraw()
//{
//}
//
//
// bool VistaOpenSGTextNodeDraw::Do ()
//{
//	if(!m_pText)
//		return false;
//
//	return true;
//}
//
// bool VistaOpenSGTextNodeDraw::GetBoundingBox(VistaBoundingBox &bb) const
//{
//	/**
//	 * @todo Implement!
//	 */
//	return false;
//}

class VistaOpenSG3DText : public IVista3DText {
 public:
  VistaOpenSG3DText()
      : m_pface(0)
      , m_fScale(1.0f)
      , m_fDepth(0.0f)
      , m_fSize(1.0f)
      , m_colFontColor(VistaColor::WHITE)
      , m_sText("")
      , m_pData(NULL)
      , m_bLit(false) {
  }

  virtual ~VistaOpenSG3DText() {
    /// @test @bug @todo as m_pface is no FieldContainer, does this work at all??? CHECK THIS!
    if (m_pface)
      subRefP(m_pface); // should/could kill it
  }

  virtual float GetFontDepth() const {
    return m_fDepth;
  }

  virtual void SetFontDepth(float fDepth) {
    m_fDepth = fDepth;
    if (m_ptrGeo)
      GenerateTextGeoemtry();
  }

  virtual void SetText(const std::string& Text) {
    if (m_sText != Text || !m_ptrGeo) {
      m_sText = Text;
      GenerateTextGeoemtry();
    }
  }

  virtual std::string GetText() const {
    return m_sText;
  }

  virtual float GetFontSize() const {
    return m_fSize;
  }

  virtual void SetFontSize(float fFontSize) {
    m_fSize = fFontSize;
  }

  virtual bool FontHasChar(char c) const {
    return true;
  }

  virtual bool SetFontName(const std::string& sFontName) {
    osg::TextVectorFace* face = osg::TextVectorFace::create(sFontName);
    if (face == 0) {
      return false;
    }

    if (m_pface != 0)
      subRefP(m_pface);

    m_pface = face;
    addRefP(m_pface);

    m_sFontName = sFontName;
    return true;
  }

  virtual std::string GetFontName() const {
    return m_sFontName;
  }

  virtual VistaColor GetFontColor() const {
    return m_colFontColor;
  }

  virtual void SetFontColor(const VistaColor& rgb) {
    m_colFontColor = rgb;
    UpdateColor();
  }

  const osg::GeometryPtr GetGeometry() const {
    return m_ptrGeo;
  }

  virtual bool GetIsLit() const {
    return m_bLit;
  }

  virtual bool SetIsLit(bool bSet) {
    m_bLit = bSet;
    return UpdateColor();
  }

  VistaOpenSGTextNodeData* m_pData;

 private:
  void GenerateTextGeoemtry() {
    osg::TextLayoutParam  layoutParam;
    osg::TextLayoutResult layoutResult;
    m_pface->layout(m_sText.c_str(), layoutParam, layoutResult);
    m_ptrGeo = m_pface->makeGeo(layoutResult, m_fSize * m_fScale, m_fDepth);
    m_ptrGeo->setDlistCache(false);
    SetFontColor(m_colFontColor);
    if (m_pData) {
      m_pData->SetCore(m_ptrGeo);
    }
  }

  bool UpdateColor() {
    if (m_ptrGeo) {
      osg::SimpleMaterialPtr mat = osg::SimpleMaterialPtr::dcast(m_ptrGeo->getMaterial());

      if (mat == OSG::NullFC) {
        mat = osg::SimpleMaterial::create();
        beginEditCP(m_ptrGeo, osg::Geometry::MaterialFieldMask);
        m_ptrGeo->setMaterial(mat);
        endEditCP(m_ptrGeo, osg::Geometry::MaterialFieldMask);
      }

      beginEditCP(mat, osg::SimpleMaterial::DiffuseFieldMask |
                           osg::SimpleMaterial::TransparencyFieldMask |
                           osg::SimpleMaterial::ColorMaterialFieldMask);

      mat->setLit(m_bLit);
      mat->setDiffuse(osg::Color3f(m_colFontColor[0], m_colFontColor[1], m_colFontColor[2]));
      mat->setTransparency(1.0f - osg::Real32(m_colFontColor[3]));

      endEditCP(mat, osg::SimpleMaterial::DiffuseFieldMask |
                         osg::SimpleMaterial::TransparencyFieldMask |
                         osg::SimpleMaterial::ColorMaterialFieldMask);

      return true;
    }
    return false;
  }

  std::string m_sText, m_sFontName;
  VistaColor  m_colFontColor;
  float       m_fScale;
  float       m_fSize;
  float       m_fDepth;
  bool        m_bLit;

  // OSG specific
  osg::TextVectorFace* m_pface;
  osg::GeometryRefPtr  m_ptrGeo;
};

IVistaNodeData* VistaOpenSGNodeBridge::NewTextNodeData(IVista3DText* pT) {
  if (pT == NULL)
    pT = new VistaOpenSG3DText;

  VistaOpenSG3DText* tx = dynamic_cast<VistaOpenSG3DText*>(pT);
  if (tx) {

    // this is a special instance that provides us with geometry
    VistaOpenSGTextNodeData* pNewData = new VistaOpenSGTextNodeData(tx);
    tx->m_pData                       = pNewData;
    pNewData->SetCore(tx->GetGeometry());
    return pNewData;
  }
  // else
  //{
  //	// can this happen at all?
  //}

  return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::LoadNode                              */
/*                                                                            */
/*============================================================================*/
// av006ce
IVistaNode* VistaOpenSGNodeBridge::LoadNode(const std::string& strFileName, float fScale,
    VistaSceneGraph::eOptFlags flags, const bool bVerbose) {
  return NULL;
}

bool VistaOpenSGNodeBridge::ApplyOptimizationToNode(
    IVistaNode* pNode, VistaSceneGraph::eOptFlags eFlags, const bool bVerbose) {
  if (eFlags != VistaSceneGraph::OPT_NONE) {

    {
      vstr::outi() << "VistaOpenSGNodeBridge::ApplyOptimizationToNode(" << pNode->GetName()
                   << ") -- applying optimizations" << std::endl;
    }

    osg::GraphOpSeq* graphOperator = new osg::GraphOpSeq;
    // removes redundant geometry information
    // and merges textures

    if (eFlags & VistaSceneGraph::OPT_MEMORY_HIGH) {
      graphOperator->addGraphOp(new osg::MaterialMergeGraphOp);
      graphOperator->addGraphOp(new osg::SharePtrGraphOp);
      if (bVerbose)
        vstr::outi() << "# MEMORY (HIGH):\nMaterialMerge\nSharePtr" << std::endl;
    } else if (eFlags & VistaSceneGraph::OPT_MEMORY_LOW) {
      graphOperator->addGraphOp(new osg::MaterialMergeGraphOp);
      if (bVerbose)
        vstr::outi() << "# MEMORY (LOW):\nMaterialMerge" << std::endl;
    }

    if (eFlags & VistaSceneGraph::OPT_GEOMETRY_LOW) {
      graphOperator->addGraphOp(new osg::MergeGraphOp);
      if (bVerbose)
        vstr::outi() << "# GEOMETRY (LOW):\nMerge" << std::endl;

    } else if (eFlags & VistaSceneGraph::OPT_GEOMETRY_MID) {
      graphOperator->addGraphOp(new osg::MergeGraphOp);
      graphOperator->addGraphOp(new osg::StripeGraphOp);
      if (bVerbose)
        vstr::outi() << "# GEOMETRY (MID):\nMerge\nStripe" << std::endl;

    } else if (eFlags & VistaSceneGraph::OPT_GEOMETRY_HIGH) {
      graphOperator->addGraphOp(new osg::MergeGraphOp);
      graphOperator->addGraphOp(new osg::StripeGraphOp);
      if (bVerbose)
        vstr::outi() << "# GEOMETRY (HIGH):\nMerge\nStripe\nPrune" << std::endl;

      std::string               sOpts = std::string("opt_") + pNode->GetName();
      float                     nSize = 1.0f;
      osg::PruneGraphOp::Method m     = osg::PruneGraphOp::SUM_OF_DIMENSIONS;

      VistaIniFileParser oFile;
      if (oFile.ReadFile(sOpts)) {
        if (bVerbose)
          vstr::outi() << "\tResolving PRUNE options from [" << sOpts << "]" << std::endl;
        nSize = oFile.GetPropertyList().GetValueInSubListOrDefault<int>("SIZE", "PRUNE", 1);

        string s = oFile.GetPropertyList().GetValueInSubListOrDefault<std::string>(
            "METHOD", "PRUNE", "SUM_OF_DIMENSIONS");
        VistaConversion::StringToLower(s);
        if (s == "sum_of_dimensions") {
          m = osg::PruneGraphOp::SUM_OF_DIMENSIONS;
        } else if (s == "volume") {
          m = osg::PruneGraphOp::VOLUME;
        }
        if (bVerbose) {
          vstr::outi() << "Using: Prune.SIZE = " << nSize << "\nand: method = " << s << std::endl;
        }
      } else {
        if (bVerbose)
          vstr::outi() << "Using default options (1.0f and SUM_OF_DIMENSIONS)" << std::endl;
      }

      graphOperator->addGraphOp(new osg::PruneGraphOp(nSize, m));
    }

    if (eFlags & VistaSceneGraph::OPT_CULLING) {
      if (bVerbose)
        vstr::outi() << "# CULLING:\nSplit" << std::endl;
      osg::UInt16        nSize = 1000;
      std::string        sOpts = std::string("opt_") + pNode->GetName();
      VistaIniFileParser oFile;
      if (oFile.ReadFile(sOpts)) {
        if (bVerbose)
          vstr::outi() << "\tResolving SPLIT options from [" << sOpts << "]" << std::endl;
        nSize = (osg::UInt16)oFile.GetPropertyList().GetValueInSubListOrDefault<int>(
            "MAXPOLYGONS", "SPLIT", 1000);
      }
      if (bVerbose)
        vstr::outi() << "Using nSize = " << nSize << std::endl;
      graphOperator->addGraphOp(new osg::SplitGraphOp("Split", nSize));
    }

#if defined(DEBUG)
    // inspects consitency of the graph loaded, always useful
    // and does not harm
    graphOperator->addGraphOp(new osg::VerifyGraphOp);
#endif

    // get data from node
    VistaNode* p1Node = static_cast<VistaNode*>(pNode);

    VistaOpenSGNodeData* pOpenSGNode = static_cast<VistaOpenSGNodeData*>(p1Node->GetData());

    OSG::NodePtr nd = pOpenSGNode->GetNode();
    graphOperator->run(nd);

    delete graphOperator;
  }
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   VistaOpenSGNodeBridge::SaveNode                              */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::SaveNode(const std::string& strFName, IVistaNode* pNode) {
  return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetName  av006ce                                              */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::SetName(const std::string& strName, IVistaNodeData* pData) {
  VistaOpenSGNodeData* pOpenSGNodeData = static_cast<VistaOpenSGNodeData*>(pData);
  //	setName( pOpenSGNodeData->GetBaseNode(), (strName + " transform").c_str() );
  osg::setName(pOpenSGNodeData->GetNode(), strName.c_str());
  if (VistaOpenSGLightNodeData* pOpenSGLightNodeData =
          dynamic_cast<VistaOpenSGLightNodeData*>(pOpenSGNodeData)) {
    osg::setName(pOpenSGLightNodeData->GetLightNode(), (strName + " light").c_str());
  }
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetName  av006ce                                              */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetName(std::string& strName, const IVistaNodeData* pData) const {
  const VistaOpenSGNodeData* pOpenSGNodeData = static_cast<const VistaOpenSGNodeData*>(pData);
  const char*                pcName          = getName(pOpenSGNodeData->GetNode());

  strName = pcName ? std::string(pcName) : "";
  return (pcName != NULL);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetIsEnabled                                                  */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetIsEnabled(const IVistaNodeData* pData) const {
  const VistaOpenSGNodeData* pOpenSGNodeData = static_cast<const VistaOpenSGNodeData*>(pData);
  return pOpenSGNodeData->GetNode()->getActive();
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetIsEnabled                                                  */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGNodeBridge::SetIsEnabled(bool bEnabled, IVistaNodeData* pData) {
  const VistaOpenSGNodeData* pOpenSGNodeData = static_cast<const VistaOpenSGNodeData*>(pData);
  beginEditCP(pOpenSGNodeData->GetNode(), osg::Node::TravMaskFieldMask);
  pOpenSGNodeData->GetNode()->setActive(bEnabled);
  endEditCP(pOpenSGNodeData->GetNode(), osg::Node::TravMaskFieldMask);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetBoundingBox                                                */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetBoundingBox(
    VistaVector3D& pMin, VistaVector3D& pMax, const IVistaNodeData* pData) const {
  osg::Pnt3f                 pMinPnt, pMaxPnt;
  const VistaOpenSGNodeData* pOpenSGNodeData = static_cast<const VistaOpenSGNodeData*>(pData);

  // get the local bounding box of this node.
  // beware: transform nodes report the transformed bbox of their children here.
  pOpenSGNodeData->GetNode()->getVolume(true).getBounds(pMinPnt, pMaxPnt);

  pMin[0] = pMinPnt[0];
  pMin[1] = pMinPnt[1];
  pMin[2] = pMinPnt[2];

  pMax[0] = pMaxPnt[0];
  pMax[1] = pMaxPnt[1];
  pMax[2] = pMaxPnt[2];

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetTranslation                                                */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetTranslation(
    VistaVector3D& pTrans, const IVistaNodeData* pData) const {

  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // osg::Matrix m(pOpenSGData->GetTransform());
  // OSG::Pnt3f _from = (OSG::Pnt3f)m[3];
  // pTrans[0] = _from[0];
  // pTrans[1] = _from[1];
  // pTrans[2] = _from[2];

  osg::TransformPtr ptrTrans = GetTransformCore(pData);
  if (!ptrTrans) {
    return false;
  }

  const osg::Matrix& m = ptrTrans->getMatrix();
  pTrans[0]            = m[3][0];
  pTrans[1]            = m[3][1];
  pTrans[2]            = m[3][2];

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetTranslation                                                */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::SetTranslation(const VistaVector3D& oTrans, IVistaNodeData* pData) {

  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // osg::Matrix m(pOpenSGData->GetTransform());
  // OSG::Pnt3f _from( pTrans[0], pTrans[1], pTrans[2] );
  // m.setTranslate( _from );

  /**********************************************************************/
  // osg::Vec3f trans, scale;
  // osg::Quaternion ori, scale_ori;
  // m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
  // trans = osg::Vec3f( pTrans[0],pTrans[1],pTrans[2] );
  // m.setTransform( trans, ori, scale, scale_ori );
  /**********************************************************************/
  // pOpenSGData->SetTransform(m);

  osg::TransformPtr ptrTrans = GetTransformCore(pData);
  if (!ptrTrans)
    return false;

  beginEditCP(ptrTrans, osg::Transform::MatrixFieldMask);
  osg::Matrix& m = ptrTrans->getMatrix();
  m[3][0]        = oTrans[0];
  m[3][1]        = oTrans[1];
  m[3][2]        = oTrans[2];
  endEditCP(ptrTrans, osg::Transform::MatrixFieldMask);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetRotation                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetRotation(
    VistaTransformMatrix& oMatrix, const IVistaNodeData* pData) const {
  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // osg::Matrix m(pOpenSGData->GetTransform());
  // osg::Quaternion q(m);
  // VistaQuaternion qv(q[0], q[1], q[2], q[3]);
  // pMatrix = VistaTransformMatrix(qv);

  osg::TransformPtr ptrTrans = GetTransformCore(pData);
  if (!ptrTrans)
    return false;

  const osg::Matrix& m = ptrTrans->getMatrix();
  osg::Quaternion    q, q1;
  osg::Vec3f         t1, t2;
  m.getTransform(t1, q, t2, q1); // clean but slow
  osg::Vec3f vec;
  float      ang;
  q.getValueAsAxisRad(vec, ang);
  oMatrix = VistaTransformMatrix(VistaAxisAndAngle(VistaVector3D(vec[0], vec[1], vec[2]), ang));
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetRotation                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::SetRotation(
    const VistaTransformMatrix& pMatrix, IVistaNodeData* pData) {
  return SetRotation(VistaQuaternion(pMatrix), pData);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetRotation                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetRotation(VistaQuaternion& pOri, const IVistaNodeData* pData) const {

  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // osg::Matrix m(pOpenSGData->GetTransform());
  // osg::Quaternion q(m);
  // pOri[0] = q[0];
  // pOri[1] = q[1];
  // pOri[2] = q[2];
  // pOri[3] = q[3];

  osg::TransformPtr ptrTrans = GetTransformCore(pData);
  if (!ptrTrans)
    return false;

  const osg::Matrix& m = ptrTrans->getMatrix();

  osg::Quaternion q, q1;
  osg::Vec3f      t1, t2;
  m.getTransform(t1, q, t2, q1); // clean but slow
  osg::Vec3f vec;
  float      ang;
  q.getValueAsAxisRad(vec, ang);
  pOri = VistaQuaternion(VistaAxisAndAngle(VistaVector3D(vec[0], vec[1], vec[2]), ang));
  return true;
}
/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetRotation                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::SetRotation(const VistaQuaternion& pOri, IVistaNodeData* pData) {
  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // osg::Matrix m(pOpenSGData->GetTransform());
  // osg::Vec3f trans, scale;
  // osg::Quaternion ori, scale_ori;
  // m.getTransform( trans, ori, scale, scale_ori ); // clean but slow

  // VistaAxisAndAngle aaa(pOri.GetAxisAndAngle());

  // ori = osg::Quaternion( osg::Vec3f(aaa.m_v3Axis[0],aaa.m_v3Axis[1],aaa.m_v3Axis[2]),
  // aaa.m_fAngle ); m.setTransform( trans, ori, scale, scale_ori ); pOpenSGData->SetTransform(m);

  osg::TransformPtr pTrans = GetTransformCore(pData);
  if (!pTrans)
    return false;

  osg::Matrix     m = pTrans->getMatrix();
  osg::Vec3f      trans, scale;
  osg::Quaternion ori, scale_ori;
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow

  VistaAxisAndAngle aaa(pOri.GetAxisAndAngle());

  ori =
      osg::Quaternion(osg::Vec3f(aaa.m_v3Axis[0], aaa.m_v3Axis[1], aaa.m_v3Axis[2]), aaa.m_fAngle);
  m.setTransform(trans, ori, scale, scale_ori);

  osg::beginEditCP(pTrans, osg::Transform::MatrixFieldMask);
  pTrans->setMatrix(m);
  osg::endEditCP(pTrans, osg::Transform::MatrixFieldMask);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetWorldTransform                                       */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetWorldTransform(
    VistaTransformMatrix& pTrans, const IVistaNodeData* pData) const {
  /**
   * @todo simply get the transform from the toolkit...as the "real-real-root"
   * should not be transformable at all (-> see VistaSceneGraph::Init - do we need the
   * real-root, the multiplication with
   * inv(modelroot-transform) should not be necessary...
   */

  const VistaOpenSGNodeData* pOpenSGData = static_cast<const VistaOpenSGNodeData*>(pData);
  VistaGroupNode*            pSceneRoot  = GetVistaSceneGraph()->GetRoot();

  // returns the transformation that transforms local coordinates into world space
  // old, wrong: osg::NodePtr nd = pOpenSGData->GetNode()->getParent();
  osg::NodePtr nd = pOpenSGData->GetNode();
  if (nd == osg::NullFC)
    return false;

  osg::Matrix m(nd->getToWorld());

  // get model-root transform
  VistaOpenSGNodeData* pRt = static_cast<VistaOpenSGNodeData*>(pSceneRoot->GetData());
  osg::Matrix          mr(pRt->GetNode()->getToWorld());

  if (mr != osg::Matrix::identity()) {
    // invert transform
    mr.invert();

    // make mr*m
    m.multLeft(mr);
  }
  // get results from m and store
  osg::Real32* val = m.getValues();
  pTrans = VistaTransformMatrix(val[0], val[4], val[8], val[12], val[1], val[5], val[9], val[13],
      val[2], val[6], val[10], val[14], val[3], val[7], val[11], val[15]);

  return true;
}
/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetWorldPosition                                             */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetWorldPosition(
    VistaVector3D& v3Pos, const IVistaNodeData* pData) const {
  VistaTransformMatrix m;
  if (GetWorldTransform(m, pData) == false)
    return false;

  VistaVector3D trans;
  m.GetTranslation(v3Pos);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetWorldOrientation                                           */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetWorldOrientation(
    VistaQuaternion& qOri, const IVistaNodeData* pData) const {
  VistaTransformMatrix m;
  if (GetWorldTransform(m, pData) == false)
    return false;

  qOri = VistaQuaternion(m);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetTransform                                                  */
/*                                                                            */
/*============================================================================*/
// av006ce eventually skip the transpose
bool VistaOpenSGNodeBridge::SetTransform(
    const VistaTransformMatrix& matrix, IVistaNodeData* pData) {
  // the osg::Matrix class is row major, just
  // as the VistaTransformMatrix
  // osg::Matrix m
  //	( pTrans[0][0], pTrans[0][1], pTrans[0][2], pTrans[0][3],
  //	  pTrans[1][0], pTrans[1][1], pTrans[1][2], pTrans[1][3],
  //	  pTrans[2][0], pTrans[2][1], pTrans[2][2], pTrans[2][3],
  //	  pTrans[3][0], pTrans[3][1], pTrans[3][2], pTrans[3][3] );
  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // pOpenSGData->SetTransform(m);

  osg::TransformPtr pTrans = GetTransformCore(pData);
  if (!pTrans)
    return false;

  // the osg::Matrix class is row major, just
  // as the VistaTransformMatrix
  osg::Matrix m(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3], matrix[1][0], matrix[1][1],
      matrix[1][2], matrix[1][3], matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
      matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);

  beginEditCP(pTrans, osg::Transform::MatrixFieldMask);
  pTrans->setMatrix(m);
  endEditCP(pTrans, osg::Transform::MatrixFieldMask);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetTransform                                                  */
/*                                                                            */
/*============================================================================*/
// av006ce eventually skip the transpose
bool VistaOpenSGNodeBridge::GetTransform(
    VistaTransformMatrix& oTrans, const IVistaNodeData* pData) const {
  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // osg::Matrix m( pOpenSGData->GetTransform() );
  // osg::Real32 * val = m.getValues();
  // pTrans = VistaTransformMatrix
  //	( val[0], val[4], val[8],  val[12],
  //	  val[1], val[5], val[9],  val[13],
  //	  val[2], val[6], val[10], val[14],
  //	  val[3], val[7], val[11], val[15] );

  osg::TransformPtr pTrans = GetTransformCore(pData);
  if (!pTrans)
    return false;

  osg::Real32* val = pTrans->getMatrix().getValues();
  oTrans = VistaTransformMatrix(val[0], val[4], val[8], val[12], val[1], val[5], val[9], val[13],
      val[2], val[6], val[10], val[14], val[3], val[7], val[11], val[15]);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   Rotate                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::Rotate(float x, float y, float z, IVistaNodeData* pData) {
  return Rotate(VistaQuaternion(VistaEulerAngles(x, y, z)), pData);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   Rotate                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::Rotate(const VistaQuaternion& pQuat, IVistaNodeData* pData) {

  // VistaAxisAndAngle aaa(pQuat.GetAxisAndAngle());

  // osg::Matrix transMatrix;
  // transMatrix.setIdentity();
  // transMatrix.setRotate( osg::Quaternion(
  // osg::Vec3f(aaa.m_v3Axis[0],aaa.m_v3Axis[1],aaa.m_v3Axis[2]), aaa.m_fAngle ) );

  // VistaOpenSGNodeData * pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  // transMatrix.mult(((osg::TransformPtr)pOpenSGData->GetTransformCore())->getMatrix());

  // pOpenSGData->SetTransform(transMatrix);

  osg::TransformPtr ptrTrans = GetTransformCore(pData);
  if (!ptrTrans)
    return false;

  VistaAxisAndAngle aaa(pQuat.GetAxisAndAngle());

  osg::Matrix transMatrix;
  transMatrix.setIdentity();
  transMatrix.setRotate(
      osg::Quaternion(osg::Vec3f(aaa.m_v3Axis[0], aaa.m_v3Axis[1], aaa.m_v3Axis[2]), aaa.m_fAngle));
  transMatrix.mult(ptrTrans->getMatrix());

  osg::beginEditCP(ptrTrans, osg::Transform::MatrixFieldMask);
  ptrTrans->setMatrix(transMatrix);
  osg::endEditCP(ptrTrans, osg::Transform::MatrixFieldMask);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   Translate                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::Translate(const VistaVector3D& vTrans, IVistaNodeData* pData) {
  // VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  osg::TransformPtr pTrans = GetTransformCore(pData);
  if (!pTrans)
    return false;

  beginEditCP(pTrans, osg::Transform::MatrixFieldMask);
  osg::Matrix& m = pTrans->getMatrix();
  m[3][0] += vTrans[0];
  m[3][1] += vTrans[1];
  m[3][2] += vTrans[2];
  endEditCP(pTrans, osg::Transform::MatrixFieldMask);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetTransformCore                                              */
/*                                                                            */
/*============================================================================*/
osg::TransformPtr VistaOpenSGNodeBridge::GetTransformCore(const IVistaNodeData* pData) const {
  const VistaOpenSGNodeData* pOpenSGData = static_cast<const VistaOpenSGNodeData*>(pData);
#ifdef _DEBUG
  osg::TransformPtr pTrans = osg::TransformPtr::dcast(pOpenSGData->GetCore());
  if (!pTrans) {
    std::string sName;
    GetName(sName, pData);
    vstr::errp() << "GetTransformCore called on non-transform node '" << sName << "'!" << std::endl;
  }
  return pTrans;
#else
  return osg::TransformPtr::dcast(pOpenSGData->GetCore());
#endif
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   CanAddChild                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::CanAddChild(
    IVistaNodeData* pChildData, const IVistaNodeData* pData) const {
  return true; // always succeeds in OpenSG
}
/*============================================================================*/
/*                                                                            */
/*  NAME    :   AddChild                                                      */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::AddChild(IVistaNodeData* pChildData, IVistaNodeData* pData) {
  VistaOpenSGNodeData* pOpenSGParentData = static_cast<VistaOpenSGNodeData*>(pData);
  VistaOpenSGNodeData* pOpenSGChildData  = static_cast<VistaOpenSGNodeData*>(pChildData);
  beginEditCP(pOpenSGParentData->GetNode(), osg::Node::ChildrenFieldMask);
  //	pOpenSGParentData->m_pNode->addChild(pOpenSGChildData->m_pBaseNode );
  pOpenSGParentData->GetNode()->addChild(pOpenSGChildData->GetNode());
  endEditCP(pOpenSGParentData->GetNode(), osg::Node::ChildrenFieldMask);

  // light nodes also need a light under the real root
  VistaOpenSGLightNodeData* pLightNodeData = dynamic_cast<VistaOpenSGLightNodeData*>(pChildData);
  if (pLightNodeData) {
    VistaOpenSGNodeData* pRootData =
        static_cast<VistaOpenSGNodeData*>(GetVistaSceneGraph()->GetRealRoot()->GetData());
    osg::NodePtr rootNode = pRootData->GetNode();

    // re-parent the existing scenegraph under the light node
    if (rootNode->getNChildren() > 0) {
      // reparent all children under the light node
      while (rootNode->getNChildren() > 0) {
        // this auto-removes the node from the old root node, too
        osg::beginEditCP(pLightNodeData->m_ptrLightNode, osg::Node::TravMaskFieldMask);
        pLightNodeData->m_ptrLightNode->addChild(rootNode->getChild(0));
        osg::endEditCP(pLightNodeData->m_ptrLightNode, osg::Node::TravMaskFieldMask);
      }
    }

    // now add/reparent the light node
    beginEditCP(rootNode, osg::Node::TravMaskFieldMask);
    rootNode->addChild(pLightNodeData->m_ptrLightNode);
    endEditCP(rootNode, osg::Node::TravMaskFieldMask);
  }
  return true;
}
/*============================================================================*/
/*                                                                            */
/*  NAME    :   InsertChild                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::InsertChild(
    IVistaNodeData* pChildData, int nIndex, IVistaNodeData* pData) {
  VistaOpenSGNodeData* pOpenSGParentData = static_cast<VistaOpenSGNodeData*>(pData);
  VistaOpenSGNodeData* pOpenSGChildData  = static_cast<VistaOpenSGNodeData*>(pChildData);
  beginEditCP(pOpenSGParentData->GetNode(), osg::Node::ChildrenFieldMask);
  //	pOpenSGParentData->m_pNode->insertChild(nIndex, pOpenSGChildData->m_pBaseNode);
  pOpenSGParentData->GetNode()->insertChild(nIndex, pOpenSGChildData->GetNode());
  endEditCP(pOpenSGParentData->GetNode(), osg::Node::ChildrenFieldMask);
  // light nodes also need a light under the real root
  VistaOpenSGLightNodeData* pLightNodeData = dynamic_cast<VistaOpenSGLightNodeData*>(pChildData);
  if (pLightNodeData) {
    VistaOpenSGNodeData* pRootData =
        static_cast<VistaOpenSGNodeData*>(GetVistaSceneGraph()->GetRealRoot()->GetData());
    osg::NodePtr rootNode = pRootData->GetNode();

    // re-parent the existing scenegraph under the light node
    if (rootNode->getNChildren() > 0) {
      // reparent all children under the light node
      while (rootNode->getNChildren() > 0) {
        // this auto-removes the node from the old root node, too
        osg::beginEditCP(pLightNodeData->m_ptrLightNode, osg::Node::TravMaskFieldMask);
        pLightNodeData->m_ptrLightNode->addChild(rootNode->getChild(0));
        osg::endEditCP(pLightNodeData->m_ptrLightNode, osg::Node::TravMaskFieldMask);
      }
    }

    // now add/reparent the light node
    beginEditCP(rootNode, osg::Node::TravMaskFieldMask);
    rootNode->addChild(pLightNodeData->m_ptrLightNode);
    endEditCP(rootNode, osg::Node::TravMaskFieldMask);
  }
  return true;
}
/*============================================================================*/
/*                                                                            */
/*  NAME    :   DisconnectChild                                               */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::DisconnectChild(int nChildindex, IVistaNodeData* pData) {
  VistaOpenSGNodeData* pOpenSGParentData = static_cast<VistaOpenSGNodeData*>(pData);
  if (pOpenSGParentData->GetNode() == osg::NullFC ||
      pOpenSGParentData->GetNode()->getNChildren() < 1)
    return false;

  // special handling for lights
  // -> how can we determine if child is a VistaLight node!? We will just see the beacon here...
  // -> we have to traverse the whole graph to find the matching light-node!
  /** @todo is there a better way to solve this!? */

  osg::NodePtr ptrChildNode = pOpenSGParentData->GetNode()->getChild(nChildindex);

  // ViSTA-created light-beacons are always osg::Groups, bail out otherwise
  if (ptrChildNode->getCore()->getType() == osg::Group::getClassType()) {
    osg::NodePtr ptrRealRoot =
        static_cast<VistaOpenSGNodeData*>(GetVistaSceneGraph()->GetRealRoot()->GetData())
            ->GetNode();

    LightOfBeaconFinder lbf;
    osg::NodePtr        ptrLightNode = lbf.find(ptrChildNode, ptrRealRoot);

    if (ptrLightNode) {
      // remove the light node and re-parent the subgraph
      osg::NodePtr ptrLightParent = ptrLightNode->getParent();

      osg::beginEditCP(ptrLightParent, osg::Node::ChildrenFieldMask);
      while (ptrLightNode->getNChildren()) {
        ptrLightParent->addChild(ptrLightNode->getChild(0));
      }

      // now remove the light node itself. it should get deleted by opensg mechanisms.
      ptrLightParent->subChild(ptrLightNode);
      osg::endEditCP(ptrLightParent, osg::Node::ChildrenFieldMask);
    }
  }
  beginEditCP(pOpenSGParentData->m_ptrNode, osg::Node::ChildrenFieldMask);
  pOpenSGParentData->m_ptrNode->subChild(nChildindex);
  endEditCP(pOpenSGParentData->m_ptrNode, osg::Node::ChildrenFieldMask);

  return true;
}
/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetActiveChild                                                */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::SetActiveChild(int i, IVistaNodeData* pData) {
  VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*>(pData);
  osg::SwitchPtr       pSwitch     = osg::SwitchPtr::dcast(pOpenSGData->GetCore());
  if (pSwitch == osg::NullFC)
    return false;
  beginEditCP(pSwitch, osg::Switch::ChoiceFieldMask);
  pSwitch->setChoice(i);
  endEditCP(pSwitch, osg::Switch::ChoiceFieldMask);
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetActiveChild                                                */
/*                                                                            */
/*============================================================================*/
int VistaOpenSGNodeBridge::GetActiveChild(const IVistaNodeData* pData) const {
  const VistaOpenSGNodeData* pOpenSGData = static_cast<const VistaOpenSGNodeData*>(pData);
  osg::SwitchPtr             pSwitch     = osg::SwitchPtr::dcast(pOpenSGData->GetCore());
  if (pSwitch == osg::NullFC)
    return -1;
  return pSwitch->getChoice();
}

bool VistaOpenSGNodeBridge::InitExtensionNode(VistaExtensionNode* pNode, IVistaNodeData*) {
  // why was this set to false!?
  return true;
}

IVista3DText* VistaOpenSGNodeBridge::CreateFontRepresentation(const std::string& sFontName) {
  VistaOpenSG3DText* pT = new VistaOpenSG3DText;
  if (pT->SetFontName(sFontName)) {
    return pT;
  } else {
    vstr::errp() << "Failed to initialize OpenSG font face for font \"" << sFontName
                 << "\". Not creating Vista3DText object!" << std::endl;
    delete pT;
    return NULL;
  }
}

bool VistaOpenSGNodeBridge::DestroyFontRepresentation(IVista3DText* pText) {
  delete pText;
  return true;
}

// ############################################################################
// OPENGL NODE METHODS
// ############################################################################
bool VistaOpenSGNodeBridge::InitOpenGLNode(IVistaNodeData* pData, VistaOpenGLNode* pNode) {
  VistaOpenSGOpenGLNodeData* pOpenSGNodeData = static_cast<VistaOpenSGOpenGLNodeData*>(pData);

  IVistaExplicitCallbackInterface* pd = pNode->GetExtension();
  if (pd == NULL)
    return false;
  IVistaOpenGLDraw*           pOgl   = dynamic_cast<IVistaOpenGLDraw*>(pd);
  osg::VistaOpenGLDrawCorePtr oglPtr = osg::VistaOpenGLDrawCore::create();
  // osg::beginEditCP(oglPtr); // not needed as we don't set a FiledContainer
  oglPtr->SetOpenGLDraw(pOgl);
  // osg::endEditCP(oglPtr);

  pOpenSGNodeData->SetCore(oglPtr);

  return true;
}

// ############################################################################
// GEOMETRY NODE METHODS
// ############################################################################

bool VistaOpenSGNodeBridge::SetGeometry(IVistaGeometryData* pGeom, IVistaNodeData* pNode) {
  VistaOpenSGGeometryData* pOpenSGData     = static_cast<VistaOpenSGGeometryData*>(pGeom);
  VistaOpenSGNodeData*     pOpenSGNodeData = static_cast<VistaOpenSGNodeData*>(pNode);

  // beginEditCP(pOpenSGNodeData->m_pNode);
  // pOpenSGNodeData->m_pNode->setCore(pOpenSGData->GetGeometry());
  // endEditCP  (pOpenSGNodeData->m_pNode);

  pOpenSGNodeData->SetCore(pOpenSGData->GetGeometry());

  return true;
}

void VistaOpenSGNodeBridge::ClearGeomNodeData(VistaGeomNode* pGeomNode) const {
  VistaOpenSGGeomNodeData* pData = dynamic_cast<VistaOpenSGGeomNodeData*>(pGeomNode->GetData());
  pData->SetCore(osg::Geometry::create());
}

// ############################################################################
// LIGHT NODE METHODS
// ############################################################################

bool VistaOpenSGNodeBridge::SetLightColor(
    VISTA_LIGHTTYPE t, float r, float g, float b, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);

  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    return false;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;

    beginEditCP(pLight, osg::Light::AmbientFieldMask | osg::Light::DiffuseFieldMask |
                            osg::Light::SpecularFieldMask);

    pLight->setAmbient(osg::Real32(r), osg::Real32(g), osg::Real32(b), OSG_LIGHTALPHA_DEFAULT);
    pLight->setDiffuse(
        osg::Real32(1.0f), osg::Real32(1.0f), osg::Real32(1.0f), OSG_LIGHTALPHA_DEFAULT);
    pLight->setSpecular(
        osg::Real32(1.0f), osg::Real32(1.0f), osg::Real32(1.0f), OSG_LIGHTALPHA_DEFAULT);
    endEditCP(pLight, osg::Light::AmbientFieldMask | osg::Light::DiffuseFieldMask |
                          osg::Light::SpecularFieldMask);

    pOpenSGData->m_cAmbient  = pLight->getAmbient();
    pOpenSGData->m_cDiffuse  = pLight->getDiffuse();
    pOpenSGData->m_cSpecular = pLight->getSpecular();
    return true;
  }
  default:
    break;
  }

  return false;
}

bool VistaOpenSGNodeBridge::SetLightAmbientColor(
    VISTA_LIGHTTYPE t, float r, float g, float b, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);

  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    m_afAmbientLight.setValuesRGBA(osg::Real32(r), osg::Real32(g), osg::Real32(b), 1.0f);
    return true;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;
    pOpenSGData->m_cAmbient =
        osg::Color4f(osg::Real32(r), osg::Real32(g), osg::Real32(b), OSG_LIGHTALPHA_DEFAULT);

    beginEditCP(pLight, osg::Light::AmbientFieldMask);
    pLight->setAmbient(pOpenSGData->m_cAmbient * pOpenSGData->m_fIntensity);
    endEditCP(pLight, osg::Light::AmbientFieldMask);

    return true;
  }
  default:
    break;
  }

  return false;
}

bool VistaOpenSGNodeBridge::SetLightDiffuseColor(
    VISTA_LIGHTTYPE t, float r, float g, float b, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);

  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    break;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {

    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;
    pOpenSGData->m_cDiffuse =
        osg::Color4f(osg::Real32(r), osg::Real32(g), osg::Real32(b), OSG_LIGHTALPHA_DEFAULT);

    beginEditCP(pLight, osg::Light::DiffuseFieldMask);
    pLight->setDiffuse(pOpenSGData->m_cDiffuse * pOpenSGData->m_fIntensity);
    endEditCP(pLight, osg::Light::DiffuseFieldMask);
    return true;
  }
  default:
    break;
  }

  return false;
}

bool VistaOpenSGNodeBridge::SetLightSpecularColor(
    VISTA_LIGHTTYPE t, float r, float g, float b, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);
  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    break;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;
    pOpenSGData->m_cSpecular =
        osg::Color4f(osg::Real32(r), osg::Real32(g), osg::Real32(b), OSG_LIGHTALPHA_DEFAULT);
    beginEditCP(pLight, osg::Light::SpecularFieldMask);
    pLight->setSpecular(pOpenSGData->m_cSpecular * pOpenSGData->m_fIntensity);
    endEditCP(pLight, osg::Light::SpecularFieldMask);
    return true;
  }
  default:
    break;
  }

  return false;
}

bool VistaOpenSGNodeBridge::SetLightAttenuation(
    VISTA_LIGHTTYPE t, const VistaVector3D& att, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);
  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    break;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;

    beginEditCP(pLight, osg::Light::ConstantAttenuationFieldMask |
                            osg::Light::LinearAttenuationFieldMask |
                            osg::Light::QuadraticAttenuationFieldMask);

    pLight->setConstantAttenuation(osg::Real32(att[0]));
    pLight->setLinearAttenuation(osg::Real32(att[1]));
    pLight->setQuadraticAttenuation(osg::Real32(att[2]));

    endEditCP(pLight, osg::Light::ConstantAttenuationFieldMask |
                          osg::Light::LinearAttenuationFieldMask |
                          osg::Light::QuadraticAttenuationFieldMask);

    return true;
  }
  default:
    break;
  }

  return false;
}

bool VistaOpenSGNodeBridge::SetLightPosition(
    VISTA_LIGHTTYPE t, const VistaVector3D& pos, IVistaNodeData* pData) {
  switch (t) {
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);
    osg::PointLightPtr        pLight      = osg::PointLightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;
    else {
      beginEditCP(pLight, osg::PointLight::PositionFieldMask);
      pLight->setPosition(osg::Real32(pos[0]), osg::Real32(pos[1]), osg::Real32(pos[2]));
      endEditCP(pLight, osg::PointLight::PositionFieldMask);
      return true;
    }
  }
  case VISTA_AMBIENT_LIGHT:
  case VISTA_DIRECTIONAL_LIGHT:
  default:
    break;
  }

  return false;
}

bool VistaOpenSGNodeBridge::SetLightDirection(
    VISTA_LIGHTTYPE t, const VistaVector3D& dir, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);
  if (!pOpenSGData)
    return false;

  switch (t) {
  case VISTA_DIRECTIONAL_LIGHT: {
    osg::DirectionalLightPtr pLight = osg::DirectionalLightPtr::dcast(pOpenSGData->GetLightCore());

    if (pLight == osg::NullFC)
      return false;

    beginEditCP(pLight, osg::DirectionalLight::DirectionFieldMask);
    pLight->setDirection(osg::Real32(dir[0]), osg::Real32(dir[1]), osg::Real32(dir[2]));
    endEditCP(pLight, osg::DirectionalLight::DirectionFieldMask);

    return true;
  }
  case VISTA_SPOT_LIGHT: {
    osg::SpotLightPtr pLight = osg::SpotLightPtr::dcast(pOpenSGData->GetLightCore());

    if (pLight == osg::NullFC)
      return false;

    beginEditCP(pLight, osg::SpotLight::DirectionFieldMask);
    pLight->setSpotDirection(osg::Real32(dir[0]), osg::Real32(dir[1]), osg::Real32(dir[2]));
    endEditCP(pLight, osg::SpotLight::DirectionFieldMask);
    return true;
  }
  case VISTA_AMBIENT_LIGHT:
  case VISTA_POINT_LIGHT:
  default:
    break;
  }

  return false;
}

bool VistaOpenSGNodeBridge::GetLightAmbientColor(
    VISTA_LIGHTTYPE t, float& r, float& g, float& b, const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);
  bool                            bRet        = false;
  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    r = m_afAmbientLight.red();
    g = m_afAmbientLight.green();
    b = m_afAmbientLight.blue();

    // osg::MaterialGroupPtr mtg = osg::MaterialGroupPtr::dcast( pOpenSGData->GetLightCore() );
    // if(mtg == osg::NullFC)
    //	return false;

    //// get light chunk
    // osg::ChunkMaterialPtr cm = osg::ChunkMaterialPtr::dcast( mtg->getMaterial() );
    // osg::StateChunkPtr sc = cm->find( osg::LightModelChunk::getClassType() );

    // if(sc == osg::NullFC)
    //	return false;

    // osg::LightModelChunkPtr lm = osg::LightModelChunkPtr::dcast( sc );

    // if(lm == osg::NullFC)
    //	return false;

    // osg::Color4f &col(lm->getAmbient());
    // r = col.red();
    // g = col.green();
    // b = col.blue();
    return true;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;

    osg::Color4f& col(pLight->getAmbient());
    r = col.red();
    g = col.green();
    b = col.blue();
    return true;
  }
  default:
    break;
  }

  return bRet;
}

bool VistaOpenSGNodeBridge::GetLightDiffuseColor(
    VISTA_LIGHTTYPE t, float& r, float& g, float& b, const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  bool bRet = false;
  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    break;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;
    osg::Color4f& col = pLight->getDiffuse();
    r                 = col.red();
    g                 = col.green();
    b                 = col.blue();
    return true;
  }
  default:
    break;
  }

  return bRet;
}

bool VistaOpenSGNodeBridge::GetLightSpecularColor(
    VISTA_LIGHTTYPE t, float& r, float& g, float& b, const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  bool bRet = false;
  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    break;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {

    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;
    osg::Color4f& col = pLight->getSpecular();
    r                 = col.red();
    g                 = col.green();
    b                 = col.blue();
    break;
  }
  default:
    break;
  }

  return bRet;
}

bool VistaOpenSGNodeBridge::GetLightAttenuation(
    VISTA_LIGHTTYPE t, VistaVector3D& att, const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  bool bRet = false;
  switch (t) {
  case VISTA_AMBIENT_LIGHT: {
    break;
  }
  case VISTA_DIRECTIONAL_LIGHT:
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
    if (pLight == osg::NullFC)
      return false;
    att[0] = float(pLight->getConstantAttenuation());
    att[1] = float(pLight->getLinearAttenuation());
    att[2] = float(pLight->getQuadraticAttenuation());
    return true;
  }
  default:
    break;
  }

  return bRet;
}

bool VistaOpenSGNodeBridge::GetLightPosition(
    VISTA_LIGHTTYPE t, VistaVector3D& pos, const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  switch (t) {
  case VISTA_POINT_LIGHT:
  case VISTA_SPOT_LIGHT: {
    osg::PointLightPtr pLight = osg::PointLightPtr::dcast(pOpenSGData->GetLightCore());

    if (pLight == osg::NullFC)
      return false;

    osg::Pnt3f& opos = pLight->getPosition();
    pos[0]           = opos[0];
    pos[1]           = opos[1];
    pos[2]           = opos[2];
    return true;
  }
  case VISTA_AMBIENT_LIGHT:
  case VISTA_DIRECTIONAL_LIGHT:
  default:
    break;
  }
  return false;
}

bool VistaOpenSGNodeBridge::GetLightDirection(
    VISTA_LIGHTTYPE t, VistaVector3D& dir, const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  switch (t) {
  case VISTA_DIRECTIONAL_LIGHT: {
    osg::DirectionalLightPtr pLight = osg::DirectionalLightPtr::dcast(pOpenSGData->GetLightCore());

    if (pLight == osg::NullFC)
      return false;

    osg::Pnt3f& p = pLight->getDirection();
    dir[0]        = p[0];
    dir[1]        = p[1];
    dir[2]        = p[2];
    return true;
  }

  case VISTA_SPOT_LIGHT: {
    osg::SpotLightPtr pLight = osg::SpotLightPtr::dcast(pOpenSGData->GetLightCore());

    if (pLight == osg::NullFC)
      return false;

    osg::Pnt3f& p = pLight->getDirection();
    dir[0]        = p[0];
    dir[1]        = p[1];
    dir[2]        = p[2];
    return true;
  }
  case VISTA_AMBIENT_LIGHT:
  case VISTA_POINT_LIGHT:
  default:
    break;
  }

  return false;
}

float VistaOpenSGNodeBridge::GetLightIntensity(const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  return pOpenSGData->m_fIntensity;
}

bool VistaOpenSGNodeBridge::SetLightIntensity(float i, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);

  osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
  if (pLight == osg::NullFC)
    return false;

  if (i < 0)
    i = 0;
  if (i > 1.0f)
    i = 1.0f;

  pOpenSGData->m_fIntensity = i;

  beginEditCP(pLight,
      osg::Light::AmbientFieldMask | osg::Light::DiffuseFieldMask | osg::Light::SpecularFieldMask);
  pLight->setAmbient(pOpenSGData->m_cAmbient * pOpenSGData->m_fIntensity);
  pLight->setDiffuse(pOpenSGData->m_cDiffuse * pOpenSGData->m_fIntensity);
  pLight->setSpecular(pOpenSGData->m_cSpecular * pOpenSGData->m_fIntensity);
  endEditCP(pLight,
      osg::Light::AmbientFieldMask | osg::Light::DiffuseFieldMask | osg::Light::SpecularFieldMask);
  return true;
}

bool VistaOpenSGNodeBridge::SetSpotCharacter(float eCharacter, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);
  osg::SpotLightPtr         pLight      = osg::SpotLightPtr::dcast(pOpenSGData->GetLightCore());
  if (pLight == osg::NullFC)
    return false;

  // clamp values
  if (eCharacter < 0.0f)
    eCharacter = 0;
  if (eCharacter > 180.0f)
    eCharacter = 180.0f;

  beginEditCP(pLight);
  pLight->setSpotCutOffDeg(eCharacter);
  endEditCP(pLight);

  return true;
}

float VistaOpenSGNodeBridge::GetSpotCharacter(const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);
  osg::SpotLightPtr               pLight = osg::SpotLightPtr::dcast(pOpenSGData->GetLightCore());
  if (pLight == osg::NullFC)
    return 0.0f;

  return float(pLight->getSpotCutOffDeg());
}

bool VistaOpenSGNodeBridge::SetSpotDistribution(int nExponent, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);
  osg::SpotLightPtr         pLight      = osg::SpotLightPtr::dcast(pOpenSGData->GetLightCore());
  if (pLight == osg::NullFC)
    return false;

  if (nExponent < 0)
    nExponent = 0;
  if (nExponent > 128)
    nExponent = 128;

  beginEditCP(pLight);
  pLight->setSpotExponent((osg::Real32)nExponent);
  endEditCP(pLight);
  return true;
}

int VistaOpenSGNodeBridge::GetSpotDistribution(const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  osg::SpotLightPtr pLight = osg::SpotLightPtr::dcast(pOpenSGData->GetLightCore());

  if (pLight == osg::NullFC)
    return -1;

  return int(pLight->getSpotExponent());
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetLightIsEnabled                                             */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGNodeBridge::GetLightIsEnabled(const IVistaNodeData* pData) const {
  const VistaOpenSGLightNodeData* pOpenSGData = static_cast<const VistaOpenSGLightNodeData*>(pData);

  OSG::LightPtr pLight = OSG::LightPtr::dcast(pOpenSGData->GetLightCore());

  if (pLight == osg::NullFC)
    return false;
  else
    return pLight->getOn();
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetLightIsEnabled                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGNodeBridge::SetLightIsEnabled(bool bEnabled, IVistaNodeData* pData) {
  VistaOpenSGLightNodeData* pOpenSGData = static_cast<VistaOpenSGLightNodeData*>(pData);
  OSG::LightPtr             pLight      = OSG::LightPtr::dcast(pOpenSGData->GetLightCore());
  if (pLight == osg::NullFC)
    return;
  beginEditCP(pLight, OSG::Light::OnFieldMask);
  pLight->setOn(bEnabled);
  endEditCP(pLight, OSG::Light::OnFieldMask);

  if (pOpenSGData->m_nLightType == VISTA_AMBIENT_LIGHT) {
    m_bAmbientLightState = bEnabled;
  }
}

bool VistaOpenSGNodeBridge::GetAmbientLightState() const {
  return m_bAmbientLightState;
}

IVistaNode* VistaOpenSGNodeBridge::CloneSubtree(IVistaNodeData* pNodeData) {
  VistaOpenSGNodeData*     pOpenSGData = static_cast<VistaOpenSGNodeData*>(pNodeData);
  std::vector<osg::UInt16> vecExcludes;
  vecExcludes.push_back(osg::Geometry::getClassTypeId());
  osg::NodePtr pNewOSGNode = deepCloneTree(pOpenSGData->GetNode(), vecExcludes);
  IVistaNode*  pRet        = NameCopyDive(this,
      static_cast<VistaOpenSGGraphicsBridge*>(GetVistaSceneGraph()->GetGraphicsBridge()),
      pNewOSGNode, pOpenSGData->GetNode());
  return pRet;
}

const std::vector<VistaLightNode*>& VistaOpenSGNodeBridge::GetAllLightNodes() const {
  return m_vecLightNodes;
}

void VistaOpenSGNodeBridge::RegisterLightNode(VistaLightNode* pLight) {
  m_vecLightNodes.push_back(pLight);
}

void VistaOpenSGNodeBridge::UnregisterLightNode(VistaLightNode* pLight) {
  std::vector<VistaLightNode*>::iterator itStoredLight =
      std::find(m_vecLightNodes.begin(), m_vecLightNodes.end(), pLight);
  if (itStoredLight == m_vecLightNodes.end()) {
    vstr::warnp() << "[VistaOpenSGNodeBridge::DeleteNode]: "
                  << "Deleting LightNode which was not registered before" << std::endl;
  } else {
    m_vecLightNodes.erase(itStoredLight);
  }

  if (pLight->GetLightType() == VISTA_AMBIENT_LIGHT) {
    m_bAmbientLightState = false;
  }
}
