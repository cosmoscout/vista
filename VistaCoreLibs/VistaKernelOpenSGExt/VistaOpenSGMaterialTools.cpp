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

#if defined(WIN32)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4312)
#pragma warning(disable : 4267)
#pragma warning(disable : 4275)
#endif

#include "VistaOpenSGMaterialTools.h"

#include <VistaBase/VistaVersion.h>

#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>

#include <OpenSG/OSGColorMaskChunk.h>
#include <OpenSG/OSGMaterial.h>
#include <OpenSG/OSGMaterialDrawable.h>
#include <OpenSG/OSGNode.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace {
class FindMaterialDrawables {
 public:
  void Traverse(osg::NodePtr oNode) {
    m_setAllMaterials.clear();

    // Edit by Markus Flatken and Simon Schneegans:
    // originally, these functors have been passed by value to osg::traverse
    // this method expects a base class and therfore the methods Enter() and
    // Leave() have never been called... this change was necessary since we
    // switched to gcc-7.2.0. We have no idea how this used to work before...
    auto enterFunctor = osg::osgTypedMethodFunctor1ObjPtrCPtrRef<osg::Action::ResultE,
        FindMaterialDrawables, osg::NodePtr>(this, &FindMaterialDrawables::Enter);
    auto leaveFunctor =
        osg::osgTypedMethodFunctor2ObjPtrCPtrRef<osg::Action::ResultE, FindMaterialDrawables,
            osg::NodePtr, osg::Action::ResultE>(this, &FindMaterialDrawables::Leave);

    osg::traverse(oNode, &enterFunctor, &leaveFunctor);
  }

  std::set<osg::MaterialDrawablePtr> m_setAllMaterials;

  OSG::Action::ResultE Enter(osg::NodePtr& oNode) {
    if (oNode->getCore()->getType().isDerivedFrom(osg::MaterialDrawable::getClassType())) {
      OSG::MaterialDrawablePtr oMat = osg::MaterialDrawablePtr::dcast(oNode->getCore());
      if (oMat != osg::NullFC)
        m_setAllMaterials.insert(oMat);
    }
    return OSG::Action::Continue;
  }
  OSG::Action::ResultE Leave(osg::NodePtr& oNode, osg::Action::ResultE oRes) {
    return oRes;
  }
};

bool ApplySortKey(osg::MaterialDrawablePtr pDrawable, const int nSortKey) {
  osg::ChunkMaterialPtr pMaterial = osg::ChunkMaterialPtr::dcast(pDrawable->getMaterial());
  if (pMaterial == osg::NullFC) {
    // create new one
    pMaterial = osg::ChunkMaterial::create();
    pDrawable->setMaterial(pMaterial);
  }
  beginEditCP(pMaterial);
  pMaterial->setSortKey(nSortKey);
  endEditCP(pMaterial);

  return true;
}

bool ApplyColorMask(osg::MaterialDrawablePtr pDrawable, const bool bDrawRed, const bool bDrawGreen,
    const bool bDrawBlue, const bool bDrawAlpha) {
  osg::ChunkMaterialPtr pMaterial = osg::ChunkMaterialPtr::dcast(pDrawable->getMaterial());
  beginEditCP(pMaterial);
  if (pMaterial == osg::NullFC) {
    // create new one
    pMaterial = osg::ChunkMaterial::create();
    pDrawable->setMaterial(pMaterial);
  }
  osg::ColorMaskChunkPtr pChunk =
      osg::ColorMaskChunkPtr::dcast(pMaterial->find(osg::ColorMaskChunk::getClassType()));
  if (pChunk == osg::NullFC) {
    pChunk = osg::ColorMaskChunk::create();
    pMaterial->addChunk(pChunk);
  }

  beginEditCP(pChunk);
  pChunk->setMask(bDrawRed, bDrawGreen, bDrawBlue, bDrawAlpha);
  endEditCP(pChunk);

  endEditCP(pMaterial);
  return true;
}

} // namespace

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaOpenSGMaterialTools::SetSortKey(VistaGeometry* pGeom, const int nSortKey) {
  osg::MaterialDrawablePtr pDrawable =
      static_cast<VistaOpenSGGeometryData*>(pGeom->GetData())->GetGeometry();
  return ApplySortKey(pDrawable, nSortKey);
}

bool VistaOpenSGMaterialTools::SetSortKeyOnSubtree(IVistaNode* pNode, const int nSortKey) {
  bool                  bReturn = true;
  FindMaterialDrawables oGrabber;
  oGrabber.Traverse(
      static_cast<VistaOpenSGNodeData*>(static_cast<VistaNode*>(pNode)->GetData())->GetNode());
  for (std::set<osg::MaterialDrawablePtr>::iterator itMat = oGrabber.m_setAllMaterials.begin();
       itMat != oGrabber.m_setAllMaterials.end(); ++itMat) {
    if (ApplySortKey((*itMat), nSortKey) == false)
      bReturn = false;
  }
  return bReturn;
}

bool VistaOpenSGMaterialTools::SetColorMask(VistaGeometry* pGeom, const bool bDrawRed,
    const bool bDrawGreen, const bool bDrawBlue, const bool bDrawAlpha) {
  osg::MaterialDrawablePtr pDrawable =
      static_cast<VistaOpenSGGeometryData*>(pGeom->GetData())->GetGeometry();
  return ApplyColorMask(pDrawable, bDrawRed, bDrawGreen, bDrawBlue, bDrawAlpha);
}

bool VistaOpenSGMaterialTools::SetColorMaskOnSubtree(IVistaNode* pNode, const bool bDrawRed,
    const bool bDrawGreen, const bool bDrawBlue, const bool bDrawAlpha) {
  bool                  bReturn = true;
  FindMaterialDrawables oGrabber;
  oGrabber.Traverse(
      static_cast<VistaOpenSGNodeData*>(static_cast<VistaNode*>(pNode)->GetData())->GetNode());
  for (std::set<osg::MaterialDrawablePtr>::iterator itMat = oGrabber.m_setAllMaterials.begin();
       itMat != oGrabber.m_setAllMaterials.end(); ++itMat) {
    if (ApplyColorMask((*itMat), bDrawRed, bDrawGreen, bDrawBlue, bDrawAlpha) == false)
      bReturn = false;
  }
  return bReturn;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
