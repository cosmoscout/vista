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

#ifndef _VISTAOPENSGGEOMETRYTOOLS_H
#define _VISTAOPENSGGEOMETRYTOOLS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaKernelOpenSGExtConfig.h"
#include <VistaKernel/GraphicsManager/VistaNodeInterface.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif
#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGNode.h>
#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

#include <list>
#include <set>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaGeometry;
class VistaDisplayManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace VistaOpenSGGeometryTools {
/**
 * COpenSGGeometryGrabber traverses a subtree and collects all geometries
 */
class VISTAKERNELOPENSGEXTAPI GeometryGrabber {
 public:
  GeometryGrabber();
  virtual ~GeometryGrabber();

  void Traverse(OSG::NodePtr node);

  typedef std::list<OSG::NodePtr>    NodeList;
  typedef std::set<OSG::GeometryPtr> GeoSet;

  const NodeList* GetNodes() const;
  const GeoSet*   GetGeometries() const;

 private:
  std::list<OSG::NodePtr>    m_lstAllGeoNodes;
  std::set<OSG::GeometryPtr> m_setAllGeos;

  OSG::Action::ResultE Enter(OSG::NodePtr& node);
  OSG::Action::ResultE Leave(OSG::NodePtr& node, OSG::Action::ResultE res);
};

VISTAKERNELOPENSGEXTAPI
bool CalcVertexNormalsOnSubtree(IVistaNode* pNode,
    const float&                            fCreaseAngle = 0.524f); // 30 degrees
VISTAKERNELOPENSGEXTAPI
bool CalcFaceNormalsOnSubtree(IVistaNode* pNode);

VISTAKERNELOPENSGEXTAPI
bool CalcVertexNormals(VistaGeometry* pGeo,
    const float&                      fCreaseAngle = 0.524f); // 30 degrees
VISTAKERNELOPENSGEXTAPI
bool CalcFaceNormals(VistaGeometry* pGeo);

VISTAKERNELOPENSGEXTAPI
bool CalcVertexNormalsGeo(VistaGeometry* pGeo,
    const float&                         fCreaseAngle = 0.524f, // 30 degrees
    bool                                 pCalc = false); // if true do CalcVertexNormals before
VISTAKERNELOPENSGEXTAPI
bool CalcFaceNormalsGeo(VistaGeometry* pGeo,
    bool                               pCalc = false); // if true do CalcFaceNormals before

VISTAKERNELOPENSGEXTAPI
bool SetUseVBOForGeometry(VistaGeometry* pGeo, const bool bSet);

VISTAKERNELOPENSGEXTAPI
bool SetUseVBOOnSubtree(IVistaNode* pNode, const bool bSet);

VISTAKERNELOPENSGEXTAPI
bool PreloadRenderData(IVistaNode* pNode, VistaDisplayManager* pDispManager);
VISTAKERNELOPENSGEXTAPI
bool PreloadRenderData(VistaGeometry* pGeom, VistaDisplayManager* pDispManager);
VISTAKERNELOPENSGEXTAPI
bool PreloadRenderData(osg::GeometryPtr pGeom, osg::WindowPtr pWindow);
VISTAKERNELOPENSGEXTAPI
bool PreloadRenderData(osg::ChunkMaterialPtr pMaterial, osg::WindowPtr pWindow);

VISTAKERNELOPENSGEXTAPI
bool PreloadAllRenderData(VistaDisplayManager* pDispManager);
} // namespace VistaOpenSGGeometryTools

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGGEOMETRYTOOLS_H
