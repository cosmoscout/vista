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
#pragma warning(disable : 4996)
#pragma warning(disable : 4267)
#endif

#include "VistaOpenSGGraphicsBridge.h"

#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>

#include <VistaBase/VistaStreamUtils.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#endif

#include <OpenSG/OSGFileGrabForeground.h>
#include <OpenSG/OSGImageForeground.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSimpleAttachments.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGSimpleMaterial.h>
#include <OpenSG/OSGSimpleStatisticsForeground.h>
#include <OpenSG/OSGSolidBackground.h>
//#include <OpenSG/OSGTextureManager.h>
#include <OpenSG/OSGColor.h>
#include <OpenSG/OSGFaceIterator.h>
#include <OpenSG/OSGGeoFunctions.h>
#include <OpenSG/OSGGeoPropPtrs.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGMaterialMergeGraphOp.h>
#include <OpenSG/OSGPointChunk.h>
#include <OpenSG/OSGPolygonChunk.h>
#include <OpenSG/OSGTextureChunk.h>
#include <OpenSG/OSGTriangleIterator.h>
#include <OpenSG/OSGViewport.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;
using namespace OSG;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaOpenSGGeometryData::VistaOpenSGGeometryData()
    : m_nCachedFaceType(VistaGeometry::VISTA_FACE_TYPE_UNDETERMINED) {
  m_ptrGeometry = Geometry::create();
  // addRefCP( m_ptrGeometry );
  m_ptrAttrChunk = NullFC;
}

///// debug helper
class FindOSGNode {
 public:
  Action::ResultE enter(NodePtr& node) {
    if (node == m_pNodeToFind) {
      m_bFound = true;
      return Action::Quit;
    }
    return Action::Continue;
  }

  bool find(NodePtr& n, NodePtr& root) {
    m_pNodeToFind = n;
    m_bFound      = false;
    traverse(root, osgTypedMethodFunctor1ObjPtrCPtrRef<Action::ResultE, FindOSGNode, NodePtr>(
                       this, &FindOSGNode::enter));

    return m_bFound;
  }

 private:
  NodePtr m_pNodeToFind;
  bool    m_bFound;
};
///////////////////

VistaOpenSGGeometryData::~VistaOpenSGGeometryData() {

#if 0
	if(m_pGeometry->getParents().size())
	{
		vstr::warnp() << "possibly dangling OpenSG Geometry core detected!!!!" << std::endl;
		vstr::warnp() << "parents: " << m_pGeometry->getParents().size() << std::endl;
		for(int i = 0; i < m_pGeometry->getParents().size(); ++i)
		{
			osg::NodePtr node = m_pGeometry->getParents()[i];

			// this produces instruction pointer corruptions on VC8 !!??

			//const char *nm = osg::getName(node);
			//std::cerr << nm << std::endl;
			// these, too...
			//std::cerr << osg::getName(node) << std::endl;
			//const char *nn = NULL;
			//std::cerr << nn << std::endl;

			// this is ok
			vstr::errp() << "\t" << node.getCPtr() << ": "
					  << (osg::getName(node)
							? osg::getName(node)
							: "[NO NAME]")
					  << std::endl;

			// trying to find corresponding nodes in scenegraph...
			VistaNode *pn = GetVistaSystem()->GetGraphicsManager()->GetSceneGraph()->GetRealRoot();
			osg::NodePtr root = ((VistaOpenSGNodeData*) pn->GetData())->GetNode();
			FindOSGNode finder;

			finder.find(node, root)
				? vstr::outi() << "FOUND NODE IN SCENEGRAPH" << std::endl
				: vstr::outi() << "DID NOT FIND NODE IN SCENEGRAPH...WE HAVE A ZOMBIE HERE!!!" << std::endl;

		}
	}
#endif
}

osg::PolygonChunkPtr VistaOpenSGGeometryData::GetAttrChunk() const {
  return m_ptrAttrChunk.get();
}

bool VistaOpenSGGeometryData::SetAttrChunk(osg::PolygonChunkPtr ptr) {
  osg::ChunkMaterialPtr mt = osg::ChunkMaterialPtr::dcast(GetGeometry()->getMaterial());
  if (mt == osg::NullFC) {
    vstr::errp() << "VistaOpenSGGraphicsBridge::SetAttrChunk(): unknown material type."
                 << std::endl;
    return false;
  }

  if (m_ptrAttrChunk != osg::NullFC) {
    // stm->setEnvMap(true);
    beginEditCP(mt);
    osg::MFStateChunkPtr::iterator it = mt->getChunks().find(m_ptrAttrChunk);
    if (it != mt->getChunks().end())
      mt->getChunks().erase(it);
    endEditCP(mt);
  }

  // ok, we killed the old chunk (hopefully)
  beginEditCP(mt);
  mt->addChunk(ptr);
  endEditCP(mt);

  m_ptrAttrChunk = ptr;

  return true;
}
osg::GeometryPtr VistaOpenSGGeometryData::GetGeometry() const {
  return m_ptrGeometry;
}

bool VistaOpenSGGeometryData::GetGeometry(osg::GeometryPtr& geom) const {
  geom = m_ptrGeometry;
  return true;
}

bool VistaOpenSGGeometryData::SetGeometry(const osg::GeometryPtr& geom) {
  if (m_ptrGeometry != osg::NullFC) {
    // exchange geometry cores on all parent nodes of the old core
    for (unsigned int i = 0; i < m_ptrGeometry->getParents().size(); ++i) {
      NodePtr ptrParent = m_ptrGeometry->getParents()[i];
      beginEditCP(ptrParent, Node::CoreFieldMask);
      ptrParent->setCore(geom);
      endEditCP(ptrParent, Node::CoreFieldMask);
    }
    // subRefCP(m_pGeometry);
  }
  m_ptrGeometry = geom;
  // addRefCP(m_pGeometry);
  return true;
}

VistaRenderingAttributes VistaOpenSGGeometryData::GetRenderingAttributes() const {
  return m_pRenderingAttributes;
}

bool VistaOpenSGGeometryData::GetRenderingAttributes(
    VistaRenderingAttributes& renderingattributes) const {
  renderingattributes = m_pRenderingAttributes;
  return true;
}

bool VistaOpenSGGeometryData::SetRenderingAttributes(
    const VistaRenderingAttributes& renderingattributes) {
  m_pRenderingAttributes = renderingattributes;
  return true;
}

osg::Int32 VistaOpenSGGeometryData::GetOsgFaceType() const {
  return m_nCachedFaceType;
}

void VistaOpenSGGeometryData::SetOsgFaceType(osg::Int32 nType) {
  m_nCachedFaceType = nType;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaOpenSGGraphicsBridge::VistaOpenSGGraphicsBridge(
    osg::RenderAction* pRenderAction, VistaOpenSGSystemClassFactory* pFactory)
    : m_pRenderAction(pRenderAction)
    , m_pFactory(pFactory) {
}

VistaOpenSGGraphicsBridge::~VistaOpenSGGraphicsBridge() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

// ============================================================================

// ============================================================================

/**
 * @todo cursor stuff should go into DisplayBridge!!!
 */

bool VistaOpenSGGraphicsBridge::CalcVertexNormals(
    IVistaGeometryData* pData, const float& fCreaseAngle) {
  OSG::calcVertexNormals(
      dynamic_cast<VistaOpenSGGeometryData*>(pData)->GetGeometry(), fCreaseAngle);

  return true;
}

bool VistaOpenSGGraphicsBridge::CalcFaceNormals(IVistaGeometryData* pData) {
  OSG::calcFaceNormals(dynamic_cast<VistaOpenSGGeometryData*>(pData)->GetGeometry());

  return true;
}

bool VistaOpenSGGraphicsBridge::GetFrustumCullingEnabled() const {
  assert(GetRenderAction());
  return GetRenderAction()->getFrustumCulling();
}

void VistaOpenSGGraphicsBridge::SetFrustumCullingEnabled(bool bCullingEnabled) {
  assert(GetRenderAction());
  GetRenderAction()->setFrustumCulling(bCullingEnabled);
}

bool VistaOpenSGGraphicsBridge::GetOcclusionCullingEnabled() const {
  assert(GetRenderAction());
  return GetRenderAction()->getOcclusionCulling();
}

void VistaOpenSGGraphicsBridge::SetOcclusionCullingEnabled(bool bOclCullingEnabled) {
  GetRenderAction()->setOcclusionCulling(bOclCullingEnabled);
}

bool VistaOpenSGGraphicsBridge::GetBBoxDrawingEnabled() const {
  assert(GetRenderAction());
  return GetRenderAction()->getVolumeDrawing();
}

void VistaOpenSGGraphicsBridge::SetBBoxDrawingEnabled(bool bState) {
  assert(GetRenderAction());
  GetRenderAction()->setVolumeDrawing(bState);
}

//  little helper to manage OSG images
osg::ImagePtr VistaOpenSGGraphicsBridge::GetCachedOrLoadImage(const string& sFileName) {
  return osg::NullFC;
}

VistaColor VistaOpenSGGraphicsBridge::GetBackgroundColor() const {
  // @todo: remove use of static function
  if (GetVistaSystem()->GetDisplayManager()->GetViewports().empty()) {
    vstr::warnp() << " VistaOpenSGGraphicsBridge::GetBackgroundColor(): no viewports defined yet "
                  << std::endl;
    return VistaColor(0, 0, 0); // returning BLACK
  }

  VistaOpenSGDisplayBridge::ViewportData* pData =
      static_cast<VistaOpenSGDisplayBridge::ViewportData*>(
          GetVistaSystem()->GetDisplayManager()->GetViewports().begin()->second->GetData());
  osg::SolidBackgroundPtr bg = pData->GetSolidBackground();
  if (bg == osg::NullFC) {
    vstr::warnp() << "VistaOpenSGGraphicsBridge::GetBackgroundColor(): background is not solid "
                  << std::endl;
    return VistaColor(0, 0, 0); // returning BLACK if the background isn't solid
  }
  osg::Color3f oColor = bg->getColor();

  return VistaColor(oColor[0], oColor[1], oColor[2]);
}

void VistaOpenSGGraphicsBridge::DebugOSG() {

  //      cout << std::endl << std::endl;
  //      cout << "OpenSG nodes:";
  //      cout << std::endl << std::endl;

  // unfortunately we have to go over the scene graph afterwards
  // to check if the same texture wasn't already loaded
  // to avoid keeping the same thing in memory twice
  // CAUTION this may become inefficient with frequent texture switching
  //         or when using large textures
  //      MaterialMergeGraphOp * TextureCleaner = new MaterialMergeGraphOp;
  //      VistaGroupNode * TheRootOfAllEvil =
  //      GetVistaSystem()->GetGraphicsManager()->GetSceneGraph()->GetRoot(); if(TheRootOfAllEvil !=
  //      NULL)
  //      {
  //              VistaOpenSGNodeData * pOpenSGNode =
  //              static_cast<VistaOpenSGNodeData*>(TheRootOfAllEvil->GetData()); NodePtr
  //              contentNode = pOpenSGNode->GetContentNode() ;
  //              TextureCleaner->traverse(contentNode);
  //      }

  // failed with segfault on cluster-linux32, temporarily commented out
  //      traverse( m_pRenderAction->getViewport()->getRoot(),
  //              osgTypedFunctionFunctor1CPtrRef<Action::ResultE, NodePtr>(enter) );
  //      cout << std::endl << std::endl;
}

void VistaOpenSGGraphicsBridge::SetBackgroundColor(const VistaColor& color) {
  if (std::abs(color.GetAlpha() - 1.0f) > Vista::Epsilon) {
    vstr::warnp() << "VistaOpenSGGraphicsBridge::SetBackgroundColor( " << color
                  << " ) - transparency not supported, ignoring alpha value" << std::endl;
  }

  if (GetVistaSystem()->GetDisplayManager() == NULL ||
      GetVistaSystem()->GetDisplayManager()->GetViewports().empty()) {
    vstr::warnp() << "VistaOpenSGGraphicsBridge::SetBackgroundColor() no viewports yet "
                  << std::endl;
    return;
  }

  osg::Color3f oOsgColor(color[0], color[1], color[2]);

  const std::map<std::string, VistaViewport*>& mapViewports =
      GetVistaSystem()->GetDisplayManager()->GetViewportsConstRef();
  for (std::map<std::string, VistaViewport*>::const_iterator itViewport = mapViewports.begin();
       itViewport != mapViewports.end(); ++itViewport) {
    VistaOpenSGDisplayBridge::ViewportData* pData =
        static_cast<VistaOpenSGDisplayBridge::ViewportData*>((*itViewport).second->GetData());
    pData->GetSolidBackground()->setColor(oOsgColor);
  }
}

VistaVertexFormat VistaOpenSGGraphicsBridge::GetVertexFormat(
    const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  VistaVertexFormat vFormat;
  osg::GeometryPtr  geo = pOpenSGData->GetGeometry();

  vFormat.coordinate = VistaVertexFormat::COORDINATE;

  if (geo->calcMappingIndex(osg::Geometry::MapNormal) >= 0)
    vFormat.normal = VistaVertexFormat::NORMAL;
  else
    vFormat.normal = VistaVertexFormat::NORMAL_NONE;

  if (geo->calcMappingIndex(osg::Geometry::MapColor) >= 0)
    vFormat.color = VistaVertexFormat::COLOR_RGB;
  else
    vFormat.color = VistaVertexFormat::COLOR_NONE;

  if (geo->calcMappingIndex(osg::Geometry::MapTexCoords) >= 0)
    vFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;
  else
    vFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_NONE;

  return vFormat;
}

VistaGeometry::FaceType VistaOpenSGGraphicsBridge::GetFaceType(
    const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  if (geo->getTypes()->size() > 1) {
    if (pOpenSGData->GetOsgFaceType() == -1) {
      // try to resolve the face types
      // vstr::outi() << "Geometry possibly mixes different primitive types." << std::endl;
      std::set<osg::UInt32> sTypes;
      osg::GeometryPtr      pOsgGeo = pOpenSGData->GetGeometry();

      osg::PrimitiveIterator it(pOsgGeo);
      for (; !it.isAtEnd(); ++it) {
        sTypes.insert(it.getType());
      }

      if (sTypes.size() == 1) {
        const_cast<VistaOpenSGGeometryData*>(pOpenSGData)
            ->SetOsgFaceType(*sTypes.begin()); // cache type
      } else
        return VistaGeometry::VISTA_FACE_TYPE_UNKNOWN;
    }

    switch (pOpenSGData->GetOsgFaceType()) {
    case GL_POINTS:
      return VistaGeometry::VISTA_FACE_TYPE_POINTS;
    case GL_LINES:
      return VistaGeometry::VISTA_FACE_TYPE_LINES;
    case GL_LINE_STRIP:
      return VistaGeometry::VISTA_FACE_TYPE_POLYLINE;
    case GL_LINE_LOOP:
      return VistaGeometry::VISTA_FACE_TYPE_POLYLINE_LOOP;
    case GL_TRIANGLES:
      return VistaGeometry::VISTA_FACE_TYPE_TRIANGLES;
    case GL_QUADS:
      return VistaGeometry::VISTA_FACE_TYPE_QUADS;
    default:
      return VistaGeometry::VISTA_FACE_TYPE_UNKNOWN;
    }

  } else { // only one element in types

    switch (geo->getTypes()->getValue(0)) {
    case GL_POINTS:
      return VistaGeometry::VISTA_FACE_TYPE_POINTS;
    case GL_LINES:
      return VistaGeometry::VISTA_FACE_TYPE_LINES;
    case GL_LINE_STRIP:
      return VistaGeometry::VISTA_FACE_TYPE_POLYLINE;
    case GL_LINE_LOOP:
      return VistaGeometry::VISTA_FACE_TYPE_POLYLINE_LOOP;
    case GL_TRIANGLES:
      return VistaGeometry::VISTA_FACE_TYPE_TRIANGLES;
    case GL_QUADS:
      return VistaGeometry::VISTA_FACE_TYPE_QUADS;
    default:
      return VistaGeometry::VISTA_FACE_TYPE_UNKNOWN;
    }
  }
}

int VistaOpenSGGraphicsBridge::GetNumberOfVertices(const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  if (geo->getLengths() != osg::NullFC && geo->getLengths()->size() == 1)
    return geo->getLengths()->getValue(0);
  else if (geo->getIndices() != osg::NullFC) {
    if (geo->getIndexMapping().size() > 0)
      return (geo->getIndices()->size() / geo->getIndexMapping().size());
    else
      return (geo->getIndices()->size());
  } else
    return 0;
}

int VistaOpenSGGraphicsBridge::GetNumberOfFaces(const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  if (pOpenSGData->GetGeometry()->getIndices() == osg::NullFC) {
    osg::GeometryPtr       geo = pOpenSGData->GetGeometry();
    osg::PrimitiveIterator it(geo);
    unsigned int           n = 0;
    for (; !it.isAtEnd(); ++it)
      ++n;
    return n;
  }

  osg::UInt32 idxSize     = pOpenSGData->GetGeometry()->getIndices()->size();
  osg::UInt32 mappingSize = pOpenSGData->GetGeometry()->getIndexMapping().size();

  switch (GetFaceType(pData)) {
  case VistaGeometry::VISTA_FACE_TYPE_POINTS:
    return idxSize / mappingSize;
  case VistaGeometry::VISTA_FACE_TYPE_LINES:
    return idxSize / (2 * mappingSize);
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE:
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE_LOOP:
    return 1;
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLES:
    return idxSize / (3 * mappingSize);
  case VistaGeometry::VISTA_FACE_TYPE_QUADS:
    return idxSize / (4 * mappingSize);
  case VistaGeometry::VISTA_FACE_TYPE_UNKNOWN:
  case VistaGeometry::VISTA_FACE_TYPE_UNDETERMINED:
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLE_STRIP:
  default: {
    // try to calculate the number of faces
    osg::GeometryPtr       geo = pOpenSGData->GetGeometry();
    osg::PrimitiveIterator it(geo);
    unsigned int           n = 0;
    for (; !it.isAtEnd(); ++it)
      ++n;
    return n;
  } break;
  }
}

int VistaOpenSGGraphicsBridge::GetNumberOfColors(const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  return pOpenSGData->GetGeometry()->getColors() == NULL
             ? 0
             : pOpenSGData->GetGeometry()->getColors()->size();
}

int VistaOpenSGGraphicsBridge::GetNumberOfCoords(const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  return pOpenSGData->GetGeometry()->getPositions() == NULL
             ? 0
             : pOpenSGData->GetGeometry()->getPositions()->size();
}

int VistaOpenSGGraphicsBridge::GetNumberOfNormals(const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  return pOpenSGData->GetGeometry()->getNormals() == NULL
             ? 0
             : pOpenSGData->GetGeometry()->getNormals()->size();
}

int VistaOpenSGGraphicsBridge::GetNumberOfTextureCoords(const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  return pOpenSGData->GetGeometry()->getTexCoords() == NULL
             ? 0
             : pOpenSGData->GetGeometry()->getTexCoords()->size();
}

int VistaOpenSGGraphicsBridge::GetCoordinateIndex(
    const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  osg::GeometryPtr geo    = pOpenSGData->GetGeometry();
  osg::UInt32      stride = geo->getIndexMapping().size();
  osg::Int16       inc    = geo->calcMappingIndex(osg::Geometry::MapPosition);

  if (inc < 0)
    return -1;
  else
    return pOpenSGData->GetGeometry()->getIndices()->getValue(idx * stride + inc);
}

VistaVector3D VistaOpenSGGraphicsBridge::GetCoordinate(
    const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::Pnt3f                     p(pOpenSGData->GetGeometry()->getPositions()->getValue(idx));
  return VistaVector3D(p[0], p[1], p[2]);
}

bool VistaOpenSGGraphicsBridge::GetCoordinate(
    const int idx, float fCoord[3], const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::Pnt3f                     p(pOpenSGData->GetGeometry()->getPositions()->getValue(idx));
  fCoord[0] = p[0];
  fCoord[1] = p[1];
  fCoord[2] = p[2];
  return true;
}

int VistaOpenSGGraphicsBridge::GetNormalIndex(
    const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  osg::GeometryPtr geo    = pOpenSGData->GetGeometry();
  osg::UInt32      stride = geo->getIndexMapping().size();
  osg::Int16       inc    = geo->calcMappingIndex(osg::Geometry::MapNormal);

  if (inc < 0)
    return -1;
  else
    return pOpenSGData->GetGeometry()->getIndices()->getValue(idx * stride + inc);
}

VistaVector3D VistaOpenSGGraphicsBridge::GetNormal(
    const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::Vec3f                     normal(pOpenSGData->GetGeometry()->getNormals()->getValue(idx));
  return VistaVector3D(normal[0], normal[1], normal[2]);
}

bool VistaOpenSGGraphicsBridge::GetNormal(
    const int idx, float fNormal[3], const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::Vec3f                     p(pOpenSGData->GetGeometry()->getNormals()->getValue(idx));
  fNormal[0] = p[0];
  fNormal[1] = p[1];
  fNormal[2] = p[2];
  return true;
}

int VistaOpenSGGraphicsBridge::GetColorIndex(const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  osg::GeometryPtr geo    = pOpenSGData->GetGeometry();
  osg::UInt32      stride = geo->getIndexMapping().size();
  osg::Int16       inc    = geo->calcMappingIndex(osg::Geometry::MapColor);

  if (inc < 0)
    return -1;
  else
    return pOpenSGData->GetGeometry()->getIndices()->getValue(idx * stride + inc);
}

VistaColor VistaOpenSGGraphicsBridge::GetColor(
    const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  if (pOpenSGData->GetGeometry()->getColors() == NULL) {
    // no vertex colors, get material color instead
    GeometryPtr      ptrGeo = pOpenSGData->GetGeometry();
    ChunkMaterialPtr ptrMat = ChunkMaterialPtr::dcast(ptrGeo->getMaterial());
    if (!ptrMat) {
#ifdef DEBUG
      vstr::errp() << "VistaOpenSGGraphicsBridge::GetColor() -- "
                   << "found no ChunkMaterial in OpenSG Geometry-Core!? MultiPassMaterial?"
                   << std::endl;
#endif
      return VistaColor(VistaColor::RED);
    }

    MaterialChunkPtr ptrChunk =
        MaterialChunkPtr::dcast(ptrMat->find(MaterialChunk::getClassType()));
    if (ptrChunk == osg::NullFC) {
      osg::SimpleMaterialPtr pSimpleMaterial = osg::SimpleMaterialPtr::dcast(ptrMat);
      if (ptrMat == osg::NullFC) {
#ifdef DEBUG
        vstr::errp() << "VistaOpenSGGraphicsBridge::GetColor() -- "
                     << " found no MaterialChunk and no SimpleMaterial - cannot retrieve color"
                     << std::endl;
#endif
        return VistaColor(VistaColor::RED);
      }
      Color3f colDiffuse = pSimpleMaterial->getDiffuse();
      return VistaColor(colDiffuse.red(), colDiffuse.green(), colDiffuse.blue(), 1.0f);
    }
    Color4f colDiffuse = ptrChunk->getDiffuse();
    return VistaColor(colDiffuse.red(), colDiffuse.green(), colDiffuse.blue(), colDiffuse.alpha());
  }

  if (0 > idx || (unsigned int)idx >= pOpenSGData->GetGeometry()->getColors()->size())
    return VistaColor(VistaColor::WHITE);

  osg::Color3f color(pOpenSGData->GetGeometry()->getColors()->getValue(idx));
  return VistaColor(color[0], color[1], color[2]);
}

int VistaOpenSGGraphicsBridge::GetTextureCoordIndex(
    const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  osg::GeometryPtr geo    = pOpenSGData->GetGeometry();
  osg::UInt32      stride = geo->getIndexMapping().size();
  osg::Int16       inc    = geo->calcMappingIndex(osg::Geometry::MapTexCoords);

  if (inc < 0)
    return -1;
  else
    return pOpenSGData->GetGeometry()->getIndices()->getValue(idx * stride + inc);
}

bool VistaOpenSGGraphicsBridge::GetTextureCoord(
    const int idx, float fTexCoord[3], const IVistaGeometryData* pData) const {
  /** @todo vec3f? */
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeoTexCoordsPtr           pTexCoords  = pOpenSGData->GetGeometry()->getTexCoords();
  if (pTexCoords != osg::NullFC && (unsigned int)idx < pTexCoords->getContainerSize()) {
    osg::Vec3f texcoords(pTexCoords->getValue(idx));
    fTexCoord[0] = texcoords[0];
    fTexCoord[1] = texcoords[1];
    fTexCoord[2] = texcoords[2];
    return true;
  } else {
    fTexCoord[0] = 0.0f;
    fTexCoord[1] = 0.0f;
    fTexCoord[2] = 0.0f;
    return false;
  }
}

VistaVector3D VistaOpenSGGraphicsBridge::GetTextureCoord(
    const int idx, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeoTexCoordsPtr           pTexCoords  = pOpenSGData->GetGeometry()->getTexCoords();
  if (pTexCoords != osg::NullFC && (unsigned int)idx < pTexCoords->getContainerSize()) {
    osg::Vec3f texcoords(pTexCoords->getValue(idx));
    return VistaVector3D(texcoords[0], texcoords[1], texcoords[2]);
  } else
    return VistaVector3D();
}

bool VistaOpenSGGraphicsBridge::SetCoordinate(
    const int idx, const VistaVector3D& coord, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0 || (unsigned int)idx >= pOpenSGData->GetGeometry()->getPositions()->size())
    return false;

  beginEditCP(pOpenSGData->GetGeometry()->getPositions());
  pOpenSGData->GetGeometry()->getPositions()->setValue(
      osg::Pnt3f(coord[0], coord[1], coord[2]), idx);
  endEditCP(pOpenSGData->GetGeometry()->getPositions());

  return true;
}

bool VistaOpenSGGraphicsBridge::SetCoordinate(
    const int idx, const float coord[], IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0 || (unsigned int)idx >= pOpenSGData->GetGeometry()->getPositions()->size())
    return false;

  beginEditCP(pOpenSGData->GetGeometry()->getPositions());
  pOpenSGData->GetGeometry()->getPositions()->setValue(
      osg::Pnt3f(coord[0], coord[1], coord[2]), idx);
  endEditCP(pOpenSGData->GetGeometry()->getPositions());

  return true;
}

bool VistaOpenSGGraphicsBridge::SetCoordinates(
    const int idx, const vector<VistaVector3D>& coords, IVistaGeometryData* pData) {
  unsigned int             n(idx + coords.size());
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  osg::GeometryPtr     p    = pOpenSGData->GetGeometry();
  osg::GeoPositionsPtr pnts = p->getPositions();

  if (p == osg::NullFC || pnts == osg::NullFC || n > pnts->size() || 0 > idx)
    return false;

  beginEditCP(pnts);
  {
    for (register unsigned int i = idx; i < n; ++i) {
      // const VistaVector3D &v = coords[i-idx];
      // const float *f = &v[0];

      // pnt[0] = f[0];
      // pnt[1] = f[1];
      // pnt[2] = f[2];
      // pnts->setValue(osg::Pnt3f(f[0],f[1], f[2]), i);
      pnts->setValue(osg::Pnt3f(coords[i - idx][0], coords[i - idx][1], coords[i - idx][2]), i);
    }
  }
  endEditCP(pnts);
  return true;
}

bool VistaOpenSGGraphicsBridge::SetCoordinates(
    const int idx, const vector<float>& coords, IVistaGeometryData* pData) {
  unsigned int             n           = idx + coords.size() / 3;
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0)
    return false;

  if (n > pOpenSGData->GetGeometry()->getPositions()->size())
    n = pOpenSGData->GetGeometry()->getPositions()->size();

  const float* c = &coords[3 * idx];
  beginEditCP(pOpenSGData->GetGeometry()->getPositions());
  {
    for (unsigned int i = idx; i < n; ++i, c += 3)
      pOpenSGData->GetGeometry()->getPositions()->setValue(osg::Pnt3f(*c, *(c + 1), *(c + 2)), i);
  }
  endEditCP(pOpenSGData->GetGeometry()->getPositions());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetTextureCoord2D(
    const int idx, const VistaVector3D& coord, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0 || (unsigned int)idx >= pOpenSGData->GetGeometry()->getTexCoords()->size())
    return false;

  beginEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  pOpenSGData->GetGeometry()->getTexCoords()->setValue(
      osg::Vec2f(coord[0], coord[1]), (unsigned int)idx);
  endEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetTextureCoord2D(
    const int idx, const float coord[], IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0 || (unsigned int)idx >= pOpenSGData->GetGeometry()->getTexCoords()->size())
    return false;

  beginEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  pOpenSGData->GetGeometry()->getTexCoords()->setValue(
      osg::Vec2f(coord[0], coord[1]), (unsigned int)idx);
  endEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetTextureCoords2D(
    const int idx, const vector<VistaVector3D>& textureCoords2D, IVistaGeometryData* pData) {
  unsigned                 n           = idx + textureCoords2D.size();
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0)
    return false;

  if (n > pOpenSGData->GetGeometry()->getTexCoords()->size())
    n = pOpenSGData->GetGeometry()->getTexCoords()->size();

  beginEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  {
    for (unsigned int i = idx; i < n; ++i)
      pOpenSGData->GetGeometry()->getTexCoords()->setValue(
          osg::Vec2f(textureCoords2D[i - idx][0], textureCoords2D[i - idx][1]), i);
  }
  endEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetTextureCoords2D(
    const int idx, const vector<float>& textureCoords2D, IVistaGeometryData* pData) {
  unsigned int             n           = idx + (textureCoords2D.size() / 2);
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0)
    return false;

  if (n > pOpenSGData->GetGeometry()->getTexCoords()->size())
    n = pOpenSGData->GetGeometry()->getTexCoords()->size();

  beginEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  {
    for (unsigned int i = idx; i < n; ++i)
      pOpenSGData->GetGeometry()->getTexCoords()->setValue(
          osg::Vec2f(textureCoords2D[(i - idx) * 2], textureCoords2D[(i - idx) * 2 + 1]), i);
  }
  endEditCP(pOpenSGData->GetGeometry()->getTexCoords());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetNormal(
    const int idx, const VistaVector3D& normal, IVistaGeometryData* pData) {
  return SetNormal(idx, &normal[0], pData);
}

bool VistaOpenSGGraphicsBridge::SetNormal(
    const int idx, const float normal[3], IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0)
    return false;

  osg::GeoNormalsPtr normals = pOpenSGData->GetGeometry()->getNormals();

  if (normals == osg::NullFC)
    return false;

  beginEditCP(normals);
  if ((unsigned int)idx >= normals->size()) {
    // add a new normal to the normals field
    normals->resize(idx + 1);
  }
  normals->setValue(osg::Vec3f(normal[0], normal[1], normal[2]), idx);
  endEditCP(normals);

  return true;
}

bool VistaOpenSGGraphicsBridge::SetNormals(
    const int idx, const vector<float>& normals, IVistaGeometryData* pData) {
  unsigned int             n           = idx + normals.size() / 3;
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0)
    return false;

  if (n > pOpenSGData->GetGeometry()->getNormals()->size())
    n = pOpenSGData->GetGeometry()->getNormals()->size();

  const float* c = &normals[3 * idx];
  beginEditCP(pOpenSGData->GetGeometry()->getNormals());
  {
    for (unsigned int i = idx; i < n; ++i, c += 3)
      pOpenSGData->GetGeometry()->getNormals()->setValue(osg::Vec3f(*c, *(c + 1), *(c + 2)), i);
  }
  endEditCP(pOpenSGData->GetGeometry()->getNormals());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetNormals(
    const int idx, const vector<VistaVector3D>& normals, IVistaGeometryData* pData) {
  unsigned int             n           = idx + normals.size();
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0)
    return false;

  if (n > pOpenSGData->GetGeometry()->getNormals()->size())
    n = pOpenSGData->GetGeometry()->getNormals()->size();

  beginEditCP(pOpenSGData->GetGeometry()->getNormals());
  {
    for (unsigned int i = idx; i < n; ++i)
      pOpenSGData->GetGeometry()->getNormals()->setValue(
          osg::Vec3f(normals[i - idx][0], normals[i - idx][1], normals[i - idx][2]), i);
  }
  endEditCP(pOpenSGData->GetGeometry()->getNormals());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetColor(
    const int idx, const VistaColor& color, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0)
    return false;

  osg::GeometryPtr  geo    = pOpenSGData->GetGeometry();
  osg::GeoColorsPtr colors = geo->getColors();

  if (colors == osg::NullFC) {
    beginEditCP(geo);
    geo->setColors(osg::GeoColors3f::create());
    colors = geo->getColors();
    endEditCP(geo);

    // set the first color to to be equal to the material diffuse component
    beginEditCP(colors);
    colors->resize(1);
    osg::MaterialPtr pMt = geo->getMaterial();
    if (pMt != osg::NullFC) {
      osg::ChunkMaterialPtr mt = osg::ChunkMaterialPtr::dcast(pMt);
      if (mt != osg::NullFC) {
        osg::MaterialChunkPtr matchunk =
            osg::MaterialChunkPtr::dcast(mt->find(osg::MaterialChunk::getClassType()));
        if (matchunk != osg::NullFC) {
          osg::Color4f& clr = matchunk->getDiffuse();
          colors->setValue(osg::Color3f(clr.red(), clr.green(), clr.blue()), 0);
        }
      }
    }
    endEditCP(colors);
  }

  float c[3];
  color.GetValues(c);
  beginEditCP(colors);
  if ((unsigned int)idx >= colors->size()) {
    // add a new color to the colors field
    colors->resize(idx + 1);
  }
  colors->setValue(osg::Color3f(c[0], c[1], c[2]), idx);
  endEditCP(colors);

  return true;
}

bool VistaOpenSGGraphicsBridge::SetColors(
    const int idx, const vector<VistaColor>& colors, IVistaGeometryData* pData) {
  unsigned int             n           = idx + colors.size();
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0)
    return false;

  if (n > pOpenSGData->GetGeometry()->getColors()->size())
    n = pOpenSGData->GetGeometry()->getColors()->size();

  float c[3];
  beginEditCP(pOpenSGData->GetGeometry()->getColors());
  {
    for (unsigned int i = idx; i < n; ++i) {
      colors[i - idx].GetValues(c);
      pOpenSGData->GetGeometry()->getColors()->setValue(osg::Color3f(c[0], c[1], c[2]), i);
    }
  }
  endEditCP(pOpenSGData->GetGeometry()->getColors());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetColors(
    const int idx, const int bufferLength, float* colors, IVistaGeometryData* pData) {
  unsigned int             n           = idx + (bufferLength / 3);
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  if (idx < 0)
    return false;

  if (n > pOpenSGData->GetGeometry()->getColors()->size())
    n = pOpenSGData->GetGeometry()->getColors()->size();

  beginEditCP(pOpenSGData->GetGeometry()->getColors());
  {
    for (unsigned int i = idx; i < n; ++i)
      pOpenSGData->GetGeometry()->getColors()->setValue(
          osg::Color3f(colors[(i - idx) * 3], colors[(i - idx) * 3 + 1], colors[(i - idx) * 3 + 2]),
          i);
  }
  endEditCP(pOpenSGData->GetGeometry()->getColors());
  return true;
}

bool VistaOpenSGGraphicsBridge::SetCoordIndex(
    const int idx, const int value, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0 || value < 0)
    return false;

  osg::GeometryPtr   geo     = pOpenSGData->GetGeometry();
  osg::MFUInt16&     im      = geo->getIndexMapping();
  osg::Int16         pi      = geo->calcMappingIndex(osg::Geometry::MapPosition);
  osg::GeoIndicesPtr indices = geo->getIndices();

  if (pi < 0)
    return false;

  beginEditCP(indices);
  indices->setValue(value, im.size() * idx + pi);
  endEditCP(indices);

  return true;
}

bool VistaOpenSGGraphicsBridge::SetTextureCoordIndex(
    const int idx, const int value, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0 || value < 0)
    return false;

  osg::GeometryPtr   geo       = pOpenSGData->GetGeometry();
  osg::MFUInt16&     im        = geo->getIndexMapping();
  osg::Int16         pi        = geo->calcMappingIndex(osg::Geometry::MapPosition);
  osg::Int16         ti        = geo->calcMappingIndex(osg::Geometry::MapTexCoords);
  osg::GeoIndicesPtr indices   = geo->getIndices();
  osg::UInt32        nVertices = indices->size() / im.size();

  if (ti < 0) {
    // create new indices for TexCoord
    ti = im.size();
    im.push_back(osg::Geometry::MapTexCoords);

    beginEditCP(indices);
    indices->resize(indices->size() + nVertices);
    for (int i = nVertices - 1; i >= 0; --i) {
      indices->setValue(0, (ti + 1) * i + ti);
      for (int j = ti - 1; j >= 0; --j) {
        indices->setValue(indices->getValue(ti * i + j), (ti + 1) * i + j);
      }
    }
    endEditCP(indices);
  } else if (ti == pi) {
    // use a separate index for normal
    im[pi] &= ~osg::Geometry::MapNormal;
    ti = im.size();
    im.push_back(osg::Geometry::MapNormal);

    beginEditCP(indices);
    indices->resize(indices->size() + nVertices);
    for (int i = nVertices - 1; i >= 0; --i) {
      // the new normal index is a copy of the position index
      indices->setValue(indices->getValue(ti * i + pi), (ti + 1) * i + ti);
      for (int j = ti - 1; j >= 0; --j) {
        indices->setValue(indices->getValue(ti * i + j), (ti + 1) * i + j);
      }
    }
    endEditCP(indices);
  }

  beginEditCP(indices);
  indices->setValue(value, im.size() * idx + ti);
  endEditCP(indices);

  return true;
}

bool VistaOpenSGGraphicsBridge::SetNormalIndex(
    const int idx, const int value, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0 || value < 0)
    return false;

  osg::GeometryPtr   geo       = pOpenSGData->GetGeometry();
  osg::MFUInt16&     im        = geo->getIndexMapping();
  osg::Int16         pi        = geo->calcMappingIndex(osg::Geometry::MapPosition);
  osg::Int16         ni        = geo->calcMappingIndex(osg::Geometry::MapNormal);
  osg::GeoIndicesPtr indices   = geo->getIndices();
  osg::UInt32        nVertices = indices->size() / im.size();

  if (ni < 0) {
    // create new indices for normal
    ni = im.size();
    im.push_back(osg::Geometry::MapNormal);

    beginEditCP(indices);
    indices->resize(indices->size() + nVertices);
    for (int i = nVertices - 1; i >= 0; --i) {
      indices->setValue(0, (ni + 1) * i + ni);
      for (int j = ni - 1; j >= 0; --j) {
        indices->setValue(indices->getValue(ni * i + j), (ni + 1) * i + j);
      }
    }
    endEditCP(indices);
  } else if (ni == pi) {
    // use a separate index for normal
    im[pi] &= ~osg::Geometry::MapNormal;
    ni = im.size();
    im.push_back(osg::Geometry::MapNormal);

    beginEditCP(indices);
    indices->resize(indices->size() + nVertices);
    for (int i = nVertices - 1; i >= 0; --i) {
      // the new normal index is a copy of the position index
      indices->setValue(indices->getValue(ni * i + pi), (ni + 1) * i + ni);
      for (int j = ni - 1; j >= 0; --j) {
        indices->setValue(indices->getValue(ni * i + j), (ni + 1) * i + j);
      }
    }
    endEditCP(indices);
  }

  beginEditCP(indices);
  indices->setValue(value, im.size() * idx + ni);
  endEditCP(indices);

  return true;
}

bool VistaOpenSGGraphicsBridge::SetColorIndex(
    const int idx, const int value, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  if (idx < 0 || value < 0)
    return false;

  osg::GeometryPtr   geo       = pOpenSGData->GetGeometry();
  osg::MFUInt16&     im        = geo->getIndexMapping();
  osg::Int16         pi        = geo->calcMappingIndex(osg::Geometry::MapPosition);
  osg::Int16         ci        = geo->calcMappingIndex(osg::Geometry::MapColor);
  osg::GeoIndicesPtr indices   = geo->getIndices();
  osg::UInt32        nVertices = indices->size() / im.size();

  if (ci < 0) {
    // create new indices for color
    ci = im.size();
    im.push_back(osg::Geometry::MapColor);

    beginEditCP(indices);
    indices->resize(indices->size() + nVertices);
    for (int i = nVertices - 1; i >= 0; --i) {
      indices->setValue(0, (ci + 1) * i + ci);
      for (int j = ci - 1; j >= 0; --j) {
        indices->setValue(indices->getValue(ci * i + j), (ci + 1) * i + j);
      }
    }
    endEditCP(indices);
  } else if (ci == pi) {
    // use a separate index for color
    im[pi] &= ~osg::Geometry::MapColor;
    ci = im.size();
    im.push_back(osg::Geometry::MapColor);

    beginEditCP(indices);
    indices->resize(indices->size() + nVertices);
    for (int i = nVertices - 1; i >= 0; --i) {
      // the new normal index is a copy of the position index
      indices->setValue(indices->getValue(ci * i + pi), (ci + 1) * i + ci);
      for (int j = ci - 1; j >= 0; --j) {
        indices->setValue(indices->getValue(ci * i + j), (ci + 1) * i + j);
      }
    }
    endEditCP(indices);
  }

  beginEditCP(indices);
  indices->setValue(value, im.size() * idx + ci);
  endEditCP(indices);

  return true;
}

bool VistaOpenSGGraphicsBridge::SetColor(const VistaColor& color, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  float                    col[3];
  color.GetValues(col);
  osg::Color3f colour(col[0], col[1], col[2]);

  if (pOpenSGData->GetGeometry()->getColors() != NULL) {
    unsigned int i, n(pOpenSGData->GetGeometry()->getColors()->size());
    beginEditCP(pOpenSGData->GetGeometry()->getColors());
    {
      for (i = 0; i < n; ++i)
        pOpenSGData->GetGeometry()->getColors()->setValue(colour, i);
    }
    endEditCP(pOpenSGData->GetGeometry()->getColors());
  } else {
    // this was no vertex colored object, so maybe it's a material colored
    // object.
    osg::ChunkMaterialPtr stm =
        osg::ChunkMaterialPtr::dcast(pOpenSGData->GetGeometry()->getMaterial());
    if (stm == osg::NullFC) {
      // no, but let's create one
      stm = osg::ChunkMaterial::create();
      pOpenSGData->GetGeometry()->setMaterial(stm); // add as material
    }

    // ok, we have a valid material in stm

    // see iff this material has a blend chunk (means: is transparent to some extend)
    osg::BlendChunkPtr bc = osg::BlendChunkPtr::dcast(stm->find(osg::BlendChunk::getClassType()));
    float              opacity = 1.0f;
    if (bc != osg::NullFC) {
      // yes, so we capture the transparency value as alpha value
      /** @todo should depned on this chunk's src/dest factor? */
      opacity = 1.0f - float(bc->getAlphaValue());
    }

    osg::StateChunkPtr matChunk = stm->find(osg::MaterialChunk::getClassType());
    if (matChunk == osg::NullFC) {
      osg::Color4f colAmbDiff;
      colAmbDiff = osg::Color4f(colour[0], colour[1], colour[2], opacity);

      // create a state chunk
      osg::MaterialChunkPtr pNewMatChunk = osg::MaterialChunk::create();
      beginEditCP(pNewMatChunk);
      pNewMatChunk->setDiffuse(colAmbDiff);
      pNewMatChunk->setAmbient(colAmbDiff);
      pNewMatChunk->setSpecular(osg::Color4f(1, 1, 1, opacity));
      pNewMatChunk->setEmission(osg::Color4f(0, 0, 0, opacity));
      pNewMatChunk->setShininess(20.0f);
      pNewMatChunk->setLit(GL_TRUE);
      endEditCP(pNewMatChunk);

      beginEditCP(stm);
      stm->addChunk(pNewMatChunk); // add to material
      endEditCP(stm);
    } else {
      // should work, as we searched for MaterialChunk
      osg::MaterialChunkPtr mat = osg::MaterialChunkPtr::dcast(matChunk);
      osg::Color4f          c(colour[0], colour[1], colour[2], opacity);
      osg::Color4f          amb(c);
      osg::Color4f          dif(c);
      osg::Color4f          emi(mat->getEmission());
      osg::Color4f          spe(mat->getSpecular());

      emi[3] = spe[3] = opacity;
      beginEditCP(mat);
      mat->setAmbient(amb);
      mat->setDiffuse(dif);
      mat->setSpecular(spe);
      mat->setEmission(emi);
      endEditCP(mat);
    }
  }
  return true;
}

bool VistaOpenSGGraphicsBridge::SetTexture(osg::ImagePtr image, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  osg::ChunkMaterialPtr mt  = osg::NullFC;
  osg::MaterialPtr      pMt = pOpenSGData->GetGeometry()->getMaterial();
  if (pMt == osg::NullFC) {
    // no material, yet?
    // create a new one

    mt = osg::ChunkMaterial::create();

    osg::MaterialChunkPtr matchunk = osg::MaterialChunk::create();
    beginEditCP(matchunk);
    matchunk->setDiffuse(osg::Color4f(0.55f, 0.55f, 0.55f, 1.0f));
    matchunk->setAmbient(osg::Color4f(0.1f, 0.1f, 0.1f, 1.0f));
    matchunk->setSpecular(osg::Color4f(0.70f, 0.70f, 0.70f, 1.0f));
    matchunk->setShininess(20.0f);
    matchunk->setLit(GL_TRUE);
    endEditCP(matchunk);

    beginEditCP(mt);
    mt->addChunk(matchunk);
    endEditCP(mt);

    pOpenSGData->GetGeometry()->setMaterial(mt);
  } else {
    mt = osg::ChunkMaterialPtr::dcast(pMt);
    if (mt == osg::NullFC)
      return false; // ok... give up
  }

  osg::StateChunkPtr stateChunk = osg::NullFC;
  // search for an existing TextureChunk, the 2nd paramter is important for multitexures
  osg::TextureChunkPtr texchunk =
      osg::TextureChunkPtr::dcast(mt->find(osg::TextureChunk::getClassType(), 0));
  if (texchunk == osg::NullFC) {
    texchunk = osg::TextureChunk::create();
    beginEditCP(
        texchunk, osg::TextureChunk::MinFilterFieldMask | osg::TextureChunk::MagFilterFieldMask |
                      osg::TextureChunk::WrapSFieldMask | osg::TextureChunk::WrapTFieldMask |
                      osg::TextureChunk::EnvModeFieldMask);

    texchunk->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texchunk->setMagFilter(GL_LINEAR);
    texchunk->setWrapS(GL_REPEAT);
    texchunk->setWrapT(GL_REPEAT);
    texchunk->setEnvMode(GL_MODULATE);

    endEditCP(texchunk, osg::TextureChunk::MinFilterFieldMask |
                            osg::TextureChunk::MagFilterFieldMask |
                            osg::TextureChunk::WrapSFieldMask | osg::TextureChunk::WrapTFieldMask |
                            osg::TextureChunk::EnvModeFieldMask);

    beginEditCP(mt, osg::ChunkMaterial::ChunksFieldMask);
    mt->addChunk(texchunk);
    endEditCP(mt, osg::ChunkMaterial::ChunksFieldMask);
  }

  beginEditCP(texchunk, osg::TextureChunk::ImageFieldMask);
  texchunk->setImage(image);
  endEditCP(texchunk, osg::TextureChunk::ImageFieldMask);

  // TBR add a BlendChunk if image has an alpha channel
  if (image->hasAlphaChannel()) {
    // check if there already is a BlendChunk
    osg::BlendChunkPtr bc = osg::BlendChunkPtr::dcast(mt->find(osg::BlendChunk::getClassType()));
    if (!bc) {
      bc = osg::BlendChunk::create();
      beginEditCP(bc);
      bc->setSrcFactor(GL_SRC_ALPHA);
      bc->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
      endEditCP(bc);

      beginEditCP(mt, osg::ChunkMaterial::ChunksFieldMask);
      mt->addChunk(bc);
      endEditCP(mt, osg::ChunkMaterial::ChunksFieldMask);
    }
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::SetTexture(const string& strNewFName, IVistaGeometryData* pData) {
  // this call may return osg::NullFC, which is ok (clear image)
  osg::ImagePtr image = GetCachedOrLoadImage(strNewFName);
  if (image == osg::NullFC)
    return false;

  return SetTexture(image, pData);
}

bool VistaOpenSGGraphicsBridge::SetTexture(const int id, const int width, const int height,
    const int bpp, VistaType::byte* pBuffer, bool bHasAlpha, IVistaGeometryData* pData) {
  if (bpp != 8 && bpp != 32) {
    vstr::warnp()
        << "[VistaOpenSGGraphicsBridge::SetTexture] only 8/32bits per color channel supported atm"
        << std::endl;
    return false;
  }

  osg::ImagePtr image = osg::Image::create();
  beginEditCP(image);
  image->set((bHasAlpha ? osg::Image::OSG_RGBA_PF : osg::Image::OSG_RGB_PF), width, height, 1, 1, 1,
      0, pBuffer, (bpp == 8 ? osg::Image::OSG_UINT8_IMAGEDATA : osg::Image::OSG_UINT32_IMAGEDATA),
      true);
  endEditCP(image);

  return SetTexture(image, pData);
}

/**
 * @todo is "Delete"Texture the right meaning? It's more a "RemoveTextureFromGeometry"
 */
bool VistaOpenSGGraphicsBridge::DeleteTexture(IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::ChunkMaterialPtr    stm =
      osg::ChunkMaterialPtr::dcast(pOpenSGData->GetGeometry()->getMaterial());
  if (stm.getCPtr() == NULL)
    return false;

  // search for the TextureChunk
  /**
   * @todo BEE there may be more then ONE texture chunk!
   */
  osg::StateChunkPtr stateChunk = stm->find(osg::TextureChunk::getClassType());
  if (stateChunk != osg::NullFC) {
    beginEditCP(stm);
    stm->subChunk(stateChunk);
    endEditCP(stm);
  }
  return true;
}

bool VistaOpenSGGraphicsBridge::ScaleGeometry(const float sc, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::Vec3f               cog(0, 0, 0), pos;
  unsigned int             i, n(pOpenSGData->GetGeometry()->getPositions()->size());
  // compute center of gravity first
  for (i = 0; i < n; ++i)
    cog += osg::Vec3f(pOpenSGData->GetGeometry()->getPositions()->getValue(i));
  cog /= (float)n;
  // recenter points then scale them before translating them back
  beginEditCP(pOpenSGData->GetGeometry()->getPositions());
  {
    for (i = 0; i < n; ++i) {
      pos = osg::Vec3f(pOpenSGData->GetGeometry()->getPositions()->getValue(i));
      pos -= cog;
      pos *= sc;
      pos += cog;
      pOpenSGData->GetGeometry()->getPositions()->setValue(pos, i);
    }
  }
  endEditCP(pOpenSGData->GetGeometry()->getPositions());
  return true;
}

bool VistaOpenSGGraphicsBridge::ScaleGeometry(const float sc[3], IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::Vec3f               cog(0, 0, 0), pos;
  unsigned int             i, n(pOpenSGData->GetGeometry()->getPositions()->size());
  // compute center of gravity first
  for (i = 0; i < n; ++i)
    cog += osg::Vec3f(pOpenSGData->GetGeometry()->getPositions()->getValue(i));
  cog /= (float)n;
  // recenter points then scale them before translating them back
  beginEditCP(pOpenSGData->GetGeometry()->getPositions());
  {
    for (i = 0; i < n; ++i) {
      pos = osg::Vec3f(pOpenSGData->GetGeometry()->getPositions()->getValue(i));
      pos -= cog;
      pos[0] *= sc[0];
      pos[1] *= sc[1];
      pos[2] *= sc[2];
      pos += cog;
      pOpenSGData->GetGeometry()->getPositions()->setValue(pos, i);
    }
  }
  endEditCP(pOpenSGData->GetGeometry()->getPositions());
  // don't forget to correctly transform all normals
  osg::Vec3f norm;
  n = pOpenSGData->GetGeometry()->getNormals()->size();
  beginEditCP(pOpenSGData->GetGeometry()->getNormals());
  {
    for (i = 0; i < n; ++i) {
      norm = pOpenSGData->GetGeometry()->getNormals()->getValue(i);
      norm[0] *= sc[0];
      norm[1] *= sc[1];
      norm[2] *= sc[2];
      norm.normalize();
      pOpenSGData->GetGeometry()->getNormals()->setValue(norm, i);
    }
  }
  endEditCP(pOpenSGData->GetGeometry()->getNormals());
  return true;
}

bool VistaOpenSGGraphicsBridge::ScaleGeometry(
    const float sx, const float sy, const float sz, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::Vec3f               cog(0, 0, 0), pos;
  unsigned int             i, n(pOpenSGData->GetGeometry()->getPositions()->size());
  // compute center of gravity first
  for (i = 0; i < n; ++i)
    cog += osg::Vec3f(pOpenSGData->GetGeometry()->getPositions()->getValue(i));
  cog /= (float)n;
  // recenter points then scale them before translating them back
  beginEditCP(pOpenSGData->GetGeometry()->getPositions());
  {
    for (i = 0; i < n; ++i) {
      pos = osg::Vec3f(pOpenSGData->GetGeometry()->getPositions()->getValue(i));
      pos -= cog;
      pos[0] *= sx;
      pos[1] *= sy;
      pos[2] *= sz;
      pos += cog;
      pOpenSGData->GetGeometry()->getPositions()->setValue(pos, i);
    }
  }
  endEditCP(pOpenSGData->GetGeometry()->getPositions());
  // don't forget to correctly transform all normals
  osg::Vec3f norm;
  n = pOpenSGData->GetGeometry()->getNormals()->size();
  beginEditCP(pOpenSGData->GetGeometry()->getNormals());
  {
    for (i = 0; i < n; ++i) {
      norm = pOpenSGData->GetGeometry()->getNormals()->getValue(i);
      norm[0] *= sx;
      norm[1] *= sy;
      norm[2] *= sz;
      norm.normalize();
      pOpenSGData->GetGeometry()->getNormals()->setValue(norm, i);
    }
  }
  endEditCP(pOpenSGData->GetGeometry()->getNormals());
  return true;
}

bool VistaOpenSGGraphicsBridge::GetBoundingBox(
    VistaVector3D& pMin, VistaVector3D& pMax, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  int                            i, n(pOpenSGData->GetGeometry()->getPositions()->size());
  if (n == 0)
    return false;
  osg::Pnt3f coordinate(pOpenSGData->GetGeometry()->getPositions()->getValue(0));
  pMin[0] = coordinate[0];
  pMin[1] = coordinate[1];
  pMin[2] = coordinate[2];
  pMax[0] = coordinate[0];
  pMax[1] = coordinate[1];
  pMax[2] = coordinate[2];
  for (i = 1; i < n; ++i) {
    coordinate = pOpenSGData->GetGeometry()->getPositions()->getValue(i);
    if (coordinate[0] < pMin[0])
      pMin[0] = coordinate[0];
    if (coordinate[1] < pMin[1])
      pMin[1] = coordinate[1];
    if (coordinate[2] < pMin[2])
      pMin[2] = coordinate[2];
    if (coordinate[0] > pMax[0])
      pMax[0] = coordinate[0];
    if (coordinate[1] > pMax[1])
      pMax[1] = coordinate[1];
    if (coordinate[2] > pMax[2])
      pMax[2] = coordinate[2];
  }
  return true;
}

bool VistaOpenSGGraphicsBridge::GetFaceBoundingBox(const int idx, VistaVector3D& pmin,
    VistaVector3D& pmax, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();
  osg::UInt32                    mappingSize = geo->getIndexMapping().size();
  osg::GeoIndicesPtr             indices     = geo->getIndices();

  osg::UInt32 verticesPerFace;
  switch (GetFaceType(pData)) {
  case VistaGeometry::VISTA_FACE_TYPE_POINTS:
    verticesPerFace = 1;
    break;
  case VistaGeometry::VISTA_FACE_TYPE_LINES:
    verticesPerFace = 2;
    break;
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLES:
    verticesPerFace = 3;
    break;
  case VistaGeometry::VISTA_FACE_TYPE_QUADS:
    verticesPerFace = 4;
    break;
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE:
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE_LOOP:
  case VistaGeometry::VISTA_FACE_TYPE_UNKNOWN: // mixed face type
  {
    osg::FaceIterator it(geo);
    // this is similar to ir.seek, but it breaks up when the end is found
    for (it.setToBegin(); it.getIndex() != idx; ++it) {
      if (it.isAtEnd()) {
        vstr::warnp() << "[VistaOpenSGGraphicsBridge::GetFaceBoundingBox] Could not find face "
                      << idx << std::endl;
        return false;
      }
    }
    verticesPerFace = it.getLength();
    osg::Pnt3f pos(it.getPosition(0));
    pmin[0] = pos[0];
    pmin[1] = pos[1];
    pmin[2] = pos[2];
    pmax[0] = pos[0];
    pmax[1] = pos[1];
    pmax[2] = pos[2];
    for (osg::UInt32 i = 1; i < verticesPerFace; ++i) {
      pos = it.getPosition(i);
      if (pos[0] < pmin[0])
        pmin[0] = pos[0];
      if (pos[1] < pmin[1])
        pmin[1] = pos[1];
      if (pos[2] < pmin[2])
        pmin[2] = pos[2];
      if (pos[0] > pmax[0])
        pmax[0] = pos[0];
      if (pos[1] > pmax[1])
        pmax[1] = pos[1];
      if (pos[2] > pmax[2])
        pmax[2] = pos[2];
    }
    return true;
  }
  default:
    vstr::warnp() << "[VistaOpenSGGraphicsBridge::GetFaceBoundingBox] Unknown face type."
                  << std::endl;
    return false;
    break;
  }

  // this is MUCH faster than using FaceIterator.seek()
  osg::Pnt3f pos(geo->getPositions()->getValue(indices->getValue(
      verticesPerFace * idx * mappingSize + geo->calcMappingIndex(osg::Geometry::MapPosition))));
  pmin[0] = pos[0];
  pmin[1] = pos[1];
  pmin[2] = pos[2];
  pmax[0] = pos[0];
  pmax[1] = pos[1];
  pmax[2] = pos[2];

  for (osg::UInt32 i = 1; i < verticesPerFace; ++i) {
    pos = geo->getPositions()->getValue(
        indices->getValue((verticesPerFace * idx + i) * mappingSize +
                          geo->calcMappingIndex(osg::Geometry::MapPosition)));
    if (pos[0] < pmin[0])
      pmin[0] = pos[0];
    if (pos[1] < pmin[1])
      pmin[1] = pos[1];
    if (pos[2] < pmin[2])
      pmin[2] = pos[2];
    if (pos[0] > pmax[0])
      pmax[0] = pos[0];
    if (pos[1] > pmax[1])
      pmax[1] = pos[1];
    if (pos[2] > pmax[2])
      pmax[2] = pos[2];
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetFaceCoords(const int idx, VistaVector3D& a, VistaVector3D& b,
    VistaVector3D& c, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::UInt32        mappingSize = geo->getIndexMapping().size();
  osg::GeoIndicesPtr indices     = geo->getIndices();

  a = VistaVector3D(
      &(geo->getPositions()->getValue(indices->getValue((3 * idx + 0) * mappingSize)))[0]);
  b = VistaVector3D(
      &(geo->getPositions()->getValue(indices->getValue((3 * idx + 1) * mappingSize)))[0]);
  c = VistaVector3D(
      &(geo->getPositions()->getValue(indices->getValue((3 * idx + 2) * mappingSize)))[0]);
  return true;
}

bool VistaOpenSGGraphicsBridge::GetFaces(
    vector<int>& faces, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  unsigned int nFaceSize = 0;
  int          nFaces    = -1;
  switch (GetFaceType(pData)) {
  case VistaGeometry::VISTA_FACE_TYPE_QUADS: {
    nFaceSize = 4;
    break;
  }
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLES: {
    nFaceSize = 3;
    break;
  }
  case VistaGeometry::VISTA_FACE_TYPE_POINTS: {
    //			nFaces = nFaceSize = GetNumberOfFaces(pData); // did this ever work?

    nFaceSize = 1;
    break;
  }
  default: {
    vstr::warnp() << "[VistaOpenSGGraphicsBridge::GetFaces] does not work for this face-type."
                  << std::endl;
    return false;
  }
  }

  if (nFaces == -1)
    nFaces = GetNumberOfFaces(pData);

  if (faces.size() != nFaceSize * nFaces)
    faces.resize(nFaceSize * nFaces);

  vector<int>::iterator fit = faces.begin();
  osg::FaceIterator     it(pOpenSGData->GetGeometry());
  for (it.setToBegin(); !it.isAtEnd(); ++it) {
    for (unsigned int n = 0; n < nFaceSize; ++n) {
      *(fit++) = it.getPositionIndex(n);
    }
  }
  return true;
}

bool VistaOpenSGGraphicsBridge::GetTrianglesVertexIndices(
    vector<int>& vecVertexIndices, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               pGeom       = pOpenSGData->GetGeometry();
  osg::TriangleIterator          pTriIter    = pGeom->beginTriangles();
  int                            iLength     = pTriIter.getLength();
  vecVertexIndices.resize(3 * iLength);
  vector<int>::iterator pCoordIter = vecVertexIndices.begin();
  int                   iTriA, iTriB, iTriC;
  int                   iNrOfTris = 0;

  for (; pTriIter != pGeom->endTriangles(); ++pTriIter) {
    /** @todo Length? Why? What are those degenerated faces */
    iTriA = pTriIter.getPositionIndex(0);
    iTriB = pTriIter.getPositionIndex(1);
    iTriC = pTriIter.getPositionIndex(2);
    if (iTriA != iTriB && iTriA != iTriC && iTriB != iTriC) {
      iNrOfTris++;
      if (iNrOfTris > iLength) {
        vecVertexIndices.resize(6 * iLength);
        pCoordIter = vecVertexIndices.begin() + 3 * iLength;
        iLength *= 2;
      }
      *(pCoordIter++) = iTriA;
      *(pCoordIter++) = iTriB;
      *(pCoordIter++) = iTriC;
    }
  }
  vecVertexIndices.resize(3 * iNrOfTris);
  return true;
}

bool VistaOpenSGGraphicsBridge::GetTrianglesNormalIndices(
    vector<int>& vecNormalIndices, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               pGeom       = pOpenSGData->GetGeometry();
  osg::TriangleIterator          pTriIter    = pGeom->beginTriangles();
  int                            iLength     = pTriIter.getLength();
  vecNormalIndices.resize(3 * iLength);
  vector<int>::iterator pNormalIter = vecNormalIndices.begin();
  int                   iNormalA, iNormalB, iNormalC;
  int                   iNrOfTris = 0;

  for (; pTriIter != pGeom->endTriangles(); ++pTriIter) {
    /** @todo Length? Why? What are those degenerated faces */
    iNormalA = pTriIter.getNormalIndex(0);
    iNormalB = pTriIter.getNormalIndex(1);
    iNormalC = pTriIter.getNormalIndex(2);
    if (!(iNormalA == -1 && iNormalB == -1 && iNormalC == -1)) {
      iNrOfTris++;
      if (iNrOfTris > iLength) {
        vecNormalIndices.resize(6 * iLength);
        pNormalIter = vecNormalIndices.begin() + 3 * iLength;
        iLength *= 2;
      }
      *(pNormalIter++) = iNormalA;
      *(pNormalIter++) = iNormalB;
      *(pNormalIter++) = iNormalC;
    }
  }
  vecNormalIndices.resize(3 * iNrOfTris);
  return true;
}

bool VistaOpenSGGraphicsBridge::GetTrianglesTextureCoordinateIndices(
    std::vector<int>& vecTexCoordIndices, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               pGeom       = pOpenSGData->GetGeometry();
  osg::TriangleIterator          pTriIter    = pGeom->beginTriangles();
  int                            iLength     = pTriIter.getLength();
  vecTexCoordIndices.resize(3 * iLength);
  vector<int>::iterator pTexCoordIter = vecTexCoordIndices.begin();
  int                   iTexCoordA, iTexCoordB, iTexCoordC;
  int                   iNrOfTris = 0;

  for (; pTriIter != pGeom->endTriangles(); ++pTriIter) {
    /** @todo Length? Why? What are those degenerated faces */
    iTexCoordA = pTriIter.getTexCoordsIndex(0);
    iTexCoordB = pTriIter.getTexCoordsIndex(1);
    iTexCoordC = pTriIter.getTexCoordsIndex(2);
    if (!(iTexCoordA == -1 && iTexCoordB == -1 && iTexCoordC == -1)) {
      iNrOfTris++;
      if (iNrOfTris > iLength) {
        vecTexCoordIndices.resize(6 * iLength);
        pTexCoordIter = vecTexCoordIndices.begin() + 3 * iLength;
        iLength *= 2;
      }
      *(pTexCoordIter++) = iTexCoordA;
      *(pTexCoordIter++) = iTexCoordB;
      *(pTexCoordIter++) = iTexCoordC;
    }
  }
  vecTexCoordIndices.resize(3 * iNrOfTris);
  return true;
}

bool VistaOpenSGGraphicsBridge::GetTrianglesVertexAndNormalIndices(vector<int>& vecVertexIndices,
    vector<int>& vecNormalIndices, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               pGeom       = pOpenSGData->GetGeometry();
  osg::TriangleIterator          pTriIter    = pGeom->beginTriangles();
  int                            iLength     = pTriIter.getLength();
  vecVertexIndices.resize(3 * iLength);
  vector<int>::iterator pCoordIter = vecVertexIndices.begin();
  vecNormalIndices.resize(3 * iLength);
  vector<int>::iterator pNormalIter = vecNormalIndices.begin();
  int                   iTriA, iTriB, iTriC;
  int                   iNormalA, iNormalB, iNormalC;
  int                   iNrOfTris = 0;

  for (; pTriIter != pGeom->endTriangles(); ++pTriIter) {
    /** @todo Length? Why? What are those degenerated faces */
    iTriA    = pTriIter.getPositionIndex(0);
    iTriB    = pTriIter.getPositionIndex(1);
    iTriC    = pTriIter.getPositionIndex(2);
    iNormalA = pTriIter.getNormalIndex(0);
    iNormalB = pTriIter.getNormalIndex(1);
    iNormalC = pTriIter.getNormalIndex(2);
    if (iTriA != iTriB && iTriA != iTriC && iTriB != iTriC) {
      iNrOfTris++;
      if (iNrOfTris > iLength) {
        vecVertexIndices.resize(6 * iLength);
        pCoordIter = vecVertexIndices.begin() + 3 * iLength;
        vecNormalIndices.resize(6 * iLength);
        pNormalIter = vecNormalIndices.begin() + 3 * iLength;
        iLength *= 2;
      }
      *(pCoordIter++)  = iTriA;
      *(pCoordIter++)  = iTriB;
      *(pCoordIter++)  = iTriC;
      *(pNormalIter++) = iNormalA;
      *(pNormalIter++) = iNormalB;
      *(pNormalIter++) = iNormalC;
    }
  }
  vecVertexIndices.resize(3 * iNrOfTris);
  vecNormalIndices.resize(3 * iNrOfTris);
  return true;
}

bool VistaOpenSGGraphicsBridge::GetCoordinates(
    vector<float>& coords, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoPositionsPtr pos  = geo->getPositions();
  osg::UInt32          npos = pos->size();
  osg::Pnt3f           pnt;

  if (coords.size() != 3 * npos)
    coords.resize(3 * npos, 0.0f);

  osg::UInt32 i   = 0;
  int         idx = -1;
  while (i < npos) {
    pos->getValue(pnt, i++);
    coords[++idx] = pnt[0];
    coords[++idx] = pnt[1];
    coords[++idx] = pnt[2];
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetCoordinates(
    vector<VistaVector3D>& coords, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoPositionsPtr pos  = geo->getPositions();
  osg::UInt32          npos = pos->size();
  osg::Pnt3f           pnt;

  if (coords.size() != npos)
    coords.resize(npos);

  osg::UInt32 i = 0;
  while (i < npos) {
    pos->getValue(pnt, i);
    coords[i][0] = pnt[0];
    coords[i][1] = pnt[1];
    coords[i][2] = pnt[2];
    coords[i][3] = 1.0f;
    ++i;
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetTextureCoords2D(
    vector<VistaVector3D>& textureCoords2D, const IVistaGeometryData* pData) const {
  /** @todo why 2D TexCoords into VistaVector3D? */
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoTexCoordsPtr tex  = geo->getTexCoords();
  osg::UInt32          ntex = tex->size();
  osg::Vec2f           vec;

  textureCoords2D.resize(ntex);

  for (osg::UInt32 i = 0; i < ntex; ++i) {
    tex->getValue(vec, i);
    textureCoords2D[i][0] = vec[0];
    textureCoords2D[i][1] = vec[1];
    textureCoords2D[i][2] = 0.0f;
    textureCoords2D[i][3] = 0.0f;
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetTextureCoords2D(
    std::vector<float>& vecTexCoords, const IVistaGeometryData* pData) const {
  /** @todo why 2D TexCoords into VistaVector3D? */
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoTexCoordsPtr tex        = geo->getTexCoords();
  osg::UInt32          nNumCoords = tex->size();
  osg::Vec2f           vec;

  vecTexCoords.resize(2 * nNumCoords);

  for (osg::UInt32 i = 0; i < nNumCoords; ++i) {
    tex->getValue(vec, i);
    vecTexCoords[2 * i]     = vec[0];
    vecTexCoords[2 * i + 1] = vec[1];
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetNormals(
    vector<VistaVector3D>& normals, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoNormalsPtr norm  = geo->getNormals();
  osg::UInt32        nnorm = norm->size();
  osg::Vec3f         vec;

  if (normals.size() != nnorm)
    normals.resize(nnorm);

  osg::UInt32 i = 0;
  while (i < nnorm) {
    norm->getValue(vec, i);
    normals[i][0] = vec[0];
    normals[i][1] = vec[1];
    normals[i][2] = vec[2];
    normals[i][3] = 0.0f;
    ++i;
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetNormals(
    vector<float>& normals, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoNormalsPtr norm  = geo->getNormals();
  osg::UInt32        nnorm = norm->size();
  osg::Vec3f         vec;

  if (normals.size() != 3 * nnorm)
    normals.resize(3 * nnorm);

  osg::UInt32 i   = 0;
  int         idx = -1;
  while (i < nnorm) {
    norm->getValue(vec, i++);
    normals[++idx] = vec[0];
    normals[++idx] = vec[1];
    normals[++idx] = vec[2];
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetColors(
    vector<VistaColor>& colorsRGB, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoColorsPtr col  = geo->getColors();
  osg::UInt32       ncol = col->size();
  osg::Color3f      rgb;

  colorsRGB.resize(ncol);

  for (osg::UInt32 i = 0; i < ncol; ++i) {
    col->getValue(rgb, i);
    colorsRGB[i] = VistaColor(rgb[0], rgb[1], rgb[2]);
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetColors(
    std::vector<float>& vecColors, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               geo         = pOpenSGData->GetGeometry();

  osg::GeoColorsPtr col  = geo->getColors();
  osg::UInt32       ncol = col->size();
  osg::Color3f      rgb;

  vecColors.resize(3 * ncol);

  for (osg::UInt32 i = 0; i < ncol; ++i) {
    col->getValue(rgb, i);
    for (int j = 0; j < 3; ++j)
      vecColors[3 * i + j] = rgb[j];
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::AddFace(const int vertexId0, const int vertexId1,
    const int vertexId2, int& faceId, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr         geo         = pOpenSGData->GetGeometry();

  if (GetFaceType(pData) != VistaGeometry::VISTA_FACE_TYPE_TRIANGLES) {
    vstr::warnp() << "[VistaOpenSGGraphicsBridge::GetFaceCoords] only works for triangles."
                  << std::endl;
    return false;
  }

  if (faceId < 0 || faceId >= this->GetNumberOfFaces(pData)) {
    // append a new face
    osg::UInt32 mappingSize = geo->getIndexMapping().size();
    osg::UInt32 i           = 0;
    beginEditCP(geo->getIndices());
    for (i = 0; i < mappingSize; ++i) {
      if (geo->getIndexMapping()[i] & osg::Geometry::MapPosition)
        geo->getIndices()->push_back(vertexId0);
      else
        geo->getIndices()->push_back(0);
    }
    for (i = 0; i < mappingSize; ++i) {
      if (geo->getIndexMapping()[i] & osg::Geometry::MapPosition)
        geo->getIndices()->push_back(vertexId1);
      else
        geo->getIndices()->push_back(0);
    }
    for (i = 0; i < mappingSize; ++i) {
      if (geo->getIndexMapping()[i] & osg::Geometry::MapPosition)
        geo->getIndices()->push_back(vertexId2);
      else
        geo->getIndices()->push_back(0);
    }
    endEditCP(geo->getIndices());

    beginEditCP(geo->getLengths());
    geo->getLengths()->setValue(geo->getLengths()->getValue(0) + 3, 0);
    endEditCP(geo->getLengths());
  } else {
    osg::UInt32        mappingSize = geo->getIndexMapping().size();
    osg::GeoIndicesPtr indices     = geo->getIndices();
    // replace an existing face
    beginEditCP(indices);
    indices->setValue(vertexId0, (3 * faceId + 0) * mappingSize);
    indices->setValue(vertexId1, (3 * faceId + 1) * mappingSize);
    indices->setValue(vertexId2, (3 * faceId + 2) * mappingSize);
    endEditCP(indices);
  }

  // don't forget to correctly transform all normals
  /*
    osg::Vec3f norm;
    n = pOpenSGData->GetGeometry()->getNormals()->size();
    beginEditCP(pOpenSGData->GetGeometry()->getNormals());
    {
    for( i = 0; i < n; ++i )
    {
    norm = pOpenSGData->GetGeometry()->getNormals()->getValue(i);
    norm[0] *= sx;
    norm[1] *= sy;
    norm[2] *= sz;
    norm.normalize();
    pOpenSGData->GetGeometry()->getNormals()->setValue(norm,i);
    }
    }
    endEditCP(pOpenSGData->GetGeometry()->getNormals());
  */
  return false;
}

bool VistaOpenSGGraphicsBridge::DeleteFace(
    const int faceId, bool deleteVertices, IVistaGeometryData* pData) {
  return false;
}

int VistaOpenSGGraphicsBridge::AddVertex(const VistaVector3D& pos, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  osg::Vec3f position(pos[0], pos[1], pos[2]);
  beginEditCP(pOpenSGData->GetGeometry()->getPositions());
  pOpenSGData->GetGeometry()->getPositions()->push_back(position);
  endEditCP(pOpenSGData->GetGeometry()->getPositions());

  return pOpenSGData->GetGeometry()->getPositions()->size() - 1;
}

bool VistaOpenSGGraphicsBridge::DeleteVertex(const int vertexId, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr         geo         = pOpenSGData->GetGeometry();
  osg::FaceIterator        it;
  unsigned int             i;
  vector<int>              mark;
  mark.clear();
  for (it = geo->beginFaces(); it != geo->endFaces(); ++it) {
    for (i = 0; i < it.getLength(); ++i)
      if (it.getPositionIndex(i) == vertexId) {
        mark.push_back(it.getIndex()); // Face Index
      }
  }

  return false;
}

bool VistaOpenSGGraphicsBridge::GetFaceVertices(const int idx, int& vertexId0, int& vertexId1,
    int& vertexId2, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  if (GetFaceType(pData) != VistaGeometry::VISTA_FACE_TYPE_TRIANGLES) {
    vstr::warnp() << "[VistaOpenSGGraphicsBridge::GetFaceCoords] only works for triangles."
                  << std::endl;
    return false;
  }

  osg::GeometryPtr   geo         = pOpenSGData->GetGeometry();
  osg::UInt32        mappingSize = geo->getIndexMapping().size();
  osg::GeoIndicesPtr indices     = geo->getIndices();

  vertexId0 = indices->getValue((3 * idx + 0) * mappingSize);
  vertexId1 = indices->getValue((3 * idx + 1) * mappingSize);
  vertexId2 = indices->getValue((3 * idx + 2) * mappingSize);
  return true;
}

bool VistaOpenSGGraphicsBridge::GetFaceCoords(
    const int idx, std::vector<int>& coords, int& nMod, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);

  int verticesPerFace = -1;
  switch (GetFaceType(pOpenSGData)) {
  case VistaGeometry::VISTA_FACE_TYPE_POINTS:
    verticesPerFace = 1;
    break;
  case VistaGeometry::VISTA_FACE_TYPE_LINES:
    verticesPerFace = 2;
    break;
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLES:
    verticesPerFace = 3;
    break;
  case VistaGeometry::VISTA_FACE_TYPE_QUADS:
    verticesPerFace = 4;
    break;
  default:
    break;
  };

  if (verticesPerFace == -1) {
    nMod = -1;
    return false;
  } else
    nMod = verticesPerFace;

  osg::GeometryPtr   geo         = pOpenSGData->GetGeometry();
  osg::UInt32        mappingSize = geo->getIndexMapping().size();
  osg::GeoIndicesPtr indices     = geo->getIndices();

  for (int n = 0; n < verticesPerFace; ++n) {
    coords.push_back(indices->getValue((verticesPerFace * idx + n) * mappingSize));
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::GetRenderingAttributes(
    VistaRenderingAttributes& attr, const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  return pOpenSGData->GetRenderingAttributes(attr);
}

bool VistaOpenSGGraphicsBridge::SetRenderingAttributes(
    const VistaRenderingAttributes& attr, IVistaGeometryData* pData) {
  bool return_state = true;

  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  pOpenSGData->SetRenderingAttributes(attr);

  // PolygonChunkPtr ChunkNorris = pOpenSGData->GetAttrChunk();
  // if( ChunkNorris == osg::NullFC )
  //{
  //  ChunkNorris = PolygonChunk::create();
  //  pOpenSGData->SetAttrChunk(ChunkNorris);
  //}

  // brute force using a new Chunk
  osg::PolygonChunkPtr ChunkNorris = osg::PolygonChunk::create();
  // SetAttrChunk will delete any PolygonChunk
  pOpenSGData->SetAttrChunk(ChunkNorris);

  beginEditCP(ChunkNorris);
  switch (attr.shading) {
  case VistaRenderingAttributes::SHADING_DEFAULT:
  case VistaRenderingAttributes::SHADING_GOURAUD:
  case VistaRenderingAttributes::SHADING_NICEST:
    ChunkNorris->setSmooth(true);
    break;
  case VistaRenderingAttributes::SHADING_FLAT:
    ChunkNorris->setSmooth(false);
    break;
  }

  switch (attr.culling) {
  case VistaRenderingAttributes::CULL_DEFAULT:
  case VistaRenderingAttributes::CULL_BACK:
    ChunkNorris->setCullFace(GL_BACK);
    break;
  case VistaRenderingAttributes::CULL_FRONT:
    ChunkNorris->setCullFace(GL_FRONT);
    break;
  case VistaRenderingAttributes::CULL_NONE:
    ChunkNorris->setCullFace(GL_NONE);
    // didn't find the equivalent in OSGPolygonChunk
    //              glDisable(GL_CULL_FACE);
    break;
  }

  switch (attr.style) {
  case VistaRenderingAttributes::STYLE_DEFAULT:
  case VistaRenderingAttributes::STYLE_FILLED:
    ChunkNorris->setFrontMode(GL_FILL);
    ChunkNorris->setBackMode(GL_FILL);
    break;
  case VistaRenderingAttributes::STYLE_WIREFRAME:
    ChunkNorris->setFrontMode(GL_LINE);
    ChunkNorris->setBackMode(GL_LINE);
    break;
  case VistaRenderingAttributes::STYLE_POINT:

    ChunkNorris->setFrontMode(GL_POINT);
    ChunkNorris->setBackMode(GL_POINT);
    ChunkNorris->setOffsetPoint(GL_TRUE);
    break;
  }
  endEditCP(ChunkNorris);

  osg::ChunkMaterialPtr stm =
      osg::ChunkMaterialPtr::dcast(pOpenSGData->GetGeometry()->getMaterial());
  if (stm.getCPtr() == NULL) {
    vstr::warnp() << "VistaOpenSGGraphicsBridge::SetRenderingAttributes() -- "
                  << "unknown material type -> omitting texture mode. " << std::endl;
    return_state &= false;
  } else {
    // search for an existing TextureChunk
    osg::StateChunkPtr stateChunk = stm->find(osg::TextureChunk::getClassType());
    if (stateChunk == osg::NullFC) {
      //                      cerr << " Warning VistaOpenSGGraphicsBridge::SetRenderingAttributes():
      //                      no texture found. " << std::endl; return_state &= false;
    } else {
      osg::TextureChunkPtr texchunk = osg::TextureChunkPtr::dcast(stateChunk);
      // stm->setEnvMap(true);
      beginEditCP(
          texchunk, OSG::TextureChunk::MinFilterFieldMask | OSG::TextureChunk::MagFilterFieldMask);

      // stm->addChunk(ChunkNorris);
      switch (attr.texFilter) {
      case VistaRenderingAttributes::TEXTURE_FILTER_DEFAULT:
      case VistaRenderingAttributes::TEXTURE_FILTER_NICEST:
      case VistaRenderingAttributes::TEXTURE_FILTER_LINEAR: {
        texchunk->setMinFilter(GL_LINEAR);
        texchunk->setMagFilter(GL_LINEAR);
      } break;
      case VistaRenderingAttributes::TEXTURE_FILTER_NEAREST: {
        texchunk->setMinFilter(GL_NEAREST);
        texchunk->setMagFilter(GL_NEAREST);
      } break;
      case VistaRenderingAttributes::TEXTURE_FILTER_MIPMAP: {
        texchunk->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
        texchunk->setMagFilter(GL_LINEAR_MIPMAP_LINEAR);
      } break;
      default: {
        vstr::warnp() << "VistaOpenSGGraphicsBridge::SetRenderingAttributes() -- "
                      << "unknown texture mode -> swithing to linear interpolation. " << std::endl;
        texchunk->setMinFilter(GL_LINEAR);
        texchunk->setMagFilter(GL_LINEAR);
        return_state &= false;
      } break;
      }
      endEditCP(
          texchunk, OSG::TextureChunk::MinFilterFieldMask | OSG::TextureChunk::MagFilterFieldMask);
    }
  }

  return return_state;
}

float VistaOpenSGGraphicsBridge::GetTransparency(const IVistaGeometryData* pData) const {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::ChunkMaterialPtr          stm =
      osg::ChunkMaterialPtr::dcast(pOpenSGData->GetGeometry()->getMaterial());

  /** @todo think about this */
  // -> should happen only with errorneous geometries
  if (stm == osg::NullFC)
    return 0.0f;

  osg::StateChunkPtr stateChunk = stm->find(osg::BlendChunk::getClassType());
  if (stateChunk == osg::NullFC)
    return 0.0f;

  // this should work, as we searched explicitely for a blend chunk above
  osg::BlendChunkPtr blendchunk = osg::BlendChunkPtr::dcast(stateChunk);
  return float(blendchunk->getAlphaValue());
}

bool VistaOpenSGGraphicsBridge::SetTransparency(
    const float transparency, IVistaGeometryData* pData) {
  if (transparency < 0 || 1 < transparency)
    return false;

  float opacity = 1.0f - transparency;

  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::ChunkMaterialPtr    stm =
      osg::ChunkMaterialPtr::dcast(pOpenSGData->GetGeometry()->getMaterial());
  if (stm == osg::NullFC)
    return false;

  osg::SimpleMaterialPtr sm = osg::SimpleMaterialPtr::dcast(stm);
  if (sm != osg::NullFC) {
    beginEditCP(sm);
    sm->setTransparency(transparency);
    sm->setColorMaterial(GL_NONE);
    endEditCP(sm);
  } else {
    // first: see if we want to create an opaque object
    // this results in: NO BLENDCHUNK AT ALL

    osg::StateChunkPtr stateChunk = stm->find(osg::BlendChunk::getClassType());

    // TBR find texture chunk / image and find out if it hasAlphaChannel
    osg::TextureChunkPtr tc =
        osg::TextureChunkPtr::dcast(stm->find(osg::TextureChunk::getClassType()));
    bool hasAlpha = false;
    if (tc) {
      hasAlpha = tc->getImage()->hasAlphaChannel();
    }

    // search for an existing BlendChunk
    if (opacity == 1.0f && !hasAlpha) {
      if (stateChunk != osg::NullFC) {
        // ok, we found a blend chunk, so we better remove it
        beginEditCP(stm);
        stm->subChunk(stateChunk);
        endEditCP(stm);
      }
    } else {
      // ok, so the object should be transparent, see iff we have a blend chunk
      osg::BlendChunkPtr blendchunk = osg::BlendChunkPtr::dcast(stateChunk);
      if (blendchunk == osg::NullFC) {
        // no, let's create one
        // create one
        blendchunk = osg::BlendChunk::create();
        beginEditCP(blendchunk);
        blendchunk->setSrcFactor(GL_SRC_ALPHA);
        blendchunk->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
        endEditCP(blendchunk);

        // add to material
        beginEditCP(stm);
        stm->addChunk(blendchunk);
        endEditCP(stm);
      }

      beginEditCP(blendchunk);
      blendchunk->setAlphaValue(transparency);
      endEditCP(blendchunk);
    }

    // ok, we reached this point, so we finally have to set the resulting
    // transparency value in the material
    // we first check for a material that is needed
    // in order to reflect the transparency settings.
    // note that we store the opacity value in the blendchunk
    // itself, according to OSG 1.6, this flag does not seem to
    // be used by any other class, so hey...
    osg::MaterialChunkPtr matChunk =
        osg::MaterialChunkPtr::dcast(stm->find(osg::MaterialChunk::getClassType()));
    if (matChunk == osg::NullFC) {
      // hmm... no material chunk, let's create one
      matChunk = osg::MaterialChunk::create();

      // set the default values
      beginEditCP(matChunk);
      matChunk->setLit(GL_TRUE);
      matChunk->setAmbient(osg::Color4f(0.01f, 0.01f, 0.01f, opacity));
      matChunk->setDiffuse(osg::Color4f(0.55f, 0.55f, 0.55f, opacity));
      matChunk->setSpecular(osg::Color4f(0.70f, 0.70f, 0.70f, opacity));
      matChunk->setShininess(20.0f);
      endEditCP(matChunk);

      beginEditCP(stm);
      stm->addChunk(matChunk);
      endEditCP(stm);
    } else {
      // we already have a matchunk
      osg::Color4f amb(matChunk->getAmbient());
      osg::Color4f dif(matChunk->getDiffuse());
      osg::Color4f emi(matChunk->getEmission());
      osg::Color4f spe(matChunk->getSpecular());

      amb[3] = dif[3] = emi[3] = spe[3] = opacity;
      beginEditCP(matChunk);
      matChunk->setAmbient(amb);
      matChunk->setDiffuse(dif);
      matChunk->setSpecular(spe);
      matChunk->setEmission(emi);
      endEditCP(matChunk);
    }
  }

  return true;
}

bool VistaOpenSGGraphicsBridge::CreateIndexedGeometry(const vector<VistaIndexedVertex>& vertices,
    const vector<VistaVector3D>& coords, const vector<VistaVector3D>& textureCoords,
    const vector<VistaVector3D>& normals, const vector<VistaColor>& colorsRGB,
    const VistaVertexFormat& vFormat, const VistaGeometry::FaceType fType,
    IVistaGeometryData* pData) {
  /*
    osg::Geometry data in OpenSG is stored in several separate vectors.

    These vectors are not a direct part of the osg::Geometry Core but
    rather split up into multiple separate classes.

    These classes, the GeoProperties, contain a single field containg
    their values, which can be accessed directly, see the docs for
    GeoProperty for the whole interface.
  */

  /*
    The first part: the primtive types.
    These are taken from OpenGL, any values that can be passed to
    glBegin(); are legal. Different types can be freely mixed.

    All properties have only one field, which has the same name for every
    property, thus the mask is also called the same for each property.
  */
  osg::GeoPTypesPtr   type = osg::GeoPTypesUI8::create();
  osg::GeoPLengthsPtr lens = osg::GeoPLengthsUI32::create();

  beginEditCP(type, osg::GeoPTypesUI8::GeoPropDataFieldMask);
  beginEditCP(lens, osg::GeoPLengthsUI32::GeoPropDataFieldMask);
  switch (fType) {
  case VistaGeometry::VISTA_FACE_TYPE_POINTS:
    type->addValue(GL_POINTS);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_LINES:
    type->addValue(GL_LINES);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE:
    type->addValue(GL_LINE_STRIP);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE_LOOP:
    type->addValue(GL_LINE_LOOP);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLES:
    type->addValue(GL_TRIANGLES);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_QUADS:
    type->addValue(GL_QUADS);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLE_STRIP:
    type->addValue(GL_TRIANGLE_STRIP);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_UNDETERMINED:
  case VistaGeometry::VISTA_FACE_TYPE_UNKNOWN:
    VISTA_THROW("CreateIndexedGeometry called with invalid face type", -1);
  }
  lens->addValue(vertices.size());
  endEditCP(type, osg::GeoPTypesUI8::GeoPropDataFieldMask);
  endEditCP(lens, osg::GeoPLengthsUI32::GeoPropDataFieldMask);

  unsigned int i, n(vertices.size());
  /*
    To use different indices for different attributes they have to be
    specified. This is done within the single index property, by using more
    than one index per vertex.

    In this case every vertex reads multiple indices from the Indices
    property. The meaning of every index is defined by the indexMapping
    given below.
  */
  osg::GeoIndicesUI32Ptr indices = osg::GeoIndicesUI32::create();
  beginEditCP(indices, osg::GeoIndicesUI32::GeoPropDataFieldMask);
  {
    // indices for the polygon
    for (i = 0; i < n; ++i) {
      if (vFormat.coordinate == VistaVertexFormat::COORDINATE)
        indices->push_back(vertices[i].GetCoordinateIndex());

      if (vFormat.color == VistaVertexFormat::COLOR_RGB)
        indices->push_back(vertices[i].GetColorIndex());

      if (vFormat.normal == VistaVertexFormat::NORMAL)
        indices->push_back(vertices[i].GetNormalIndex());

      if (vFormat.textureCoord == VistaVertexFormat::TEXTURE_COORD_2D)
        indices->push_back(vertices[i].GetTextureCoordinateIndex());
    }
  }
  endEditCP(indices, osg::GeoIndicesUI32::GeoPropDataFieldMask);

  /*
    The third part: the vertex positions.

    OpenSG uses different types for vectors and points.

    Points (e.g. Pnt3f) are just positions in space, they have a limited
    set of operations they can handle. Vectors (e.g. osg::Vec3f) are the more
    general kind.
  */
  n                           = coords.size();
  osg::GeoPositions3fPtr pnts = osg::GeoPositions3f::create();
  if (vFormat.coordinate == VistaVertexFormat::COORDINATE) {
    beginEditCP(pnts, osg::GeoPositions3f::GeoPropDataFieldMask);
    {
      for (i = 0; i < n; ++i)
        pnts->addValue(osg::Pnt3f(coords[i][0], coords[i][1], coords[i][2]));
    }
    endEditCP(pnts, osg::GeoPositions3f::GeoPropDataFieldMask);
  }

  n = colorsRGB.size();
  osg::RefPtr<osg::GeoColors3fPtr> colors(osg::GeoColors3f::create());
  if (vFormat.color == VistaVertexFormat::COLOR_RGB) {
    beginEditCP(colors, osg::GeoColors3f::GeoPropDataFieldMask);
    {
      float col[3];
      for (i = 0; i < n; ++i) {
        colorsRGB[i].GetValues(col);
        colors->push_back(osg::Color3f(col[0], col[1], col[2]));
      }
    }
    endEditCP(colors, osg::GeoColors3f::GeoPropDataFieldMask);
  }

  n = normals.size();
  osg::RefPtr<osg::GeoNormals3fPtr> norms(osg::GeoNormals3f::create());
  if (vFormat.normal == VistaVertexFormat::NORMAL) {
    beginEditCP(norms, osg::GeoNormals3f::GeoPropDataFieldMask);
    {
      for (i = 0; i < n; ++i) {
        // osg::osg::Vec3f v(normals[i][0], normals[i][1], normals[i][2]);
        // cout << "normal [" << i << "] = " << normals[i] << std::endl;
        // cout << "vec = [" << v[0] << ", " << v[1] << ", " << v[2] << std::endl;

        norms->push_back(osg::Vec3f(normals[i][0], normals[i][1], normals[i][2]));
      }
    }
    endEditCP(norms, osg::GeoNormals3f::GeoPropDataFieldMask);
  }

  n = textureCoords.size();
  osg::RefPtr<osg::GeoTexCoords2fPtr> texcoords(osg::GeoTexCoords2f::create());
  if (vFormat.textureCoord == VistaVertexFormat::TEXTURE_COORD_2D) {
    beginEditCP(texcoords, osg::GeoTexCoords2f::GeoPropDataFieldMask);
    {
      for (i = 0; i < n; ++i)
        texcoords->push_back(osg::Vec2f(textureCoords[i][0], textureCoords[i][1]));
    }
    endEditCP(texcoords, osg::GeoTexCoords2f::GeoPropDataFieldMask);
  }
  osg::MaterialChunkRefPtr matchunk(osg::MaterialChunk::create());
  beginEditCP(matchunk);
  {
    matchunk->setLit(GL_TRUE);
    matchunk->setAmbient(osg::Color4f(0.01f, 0.01f, 0.01f, 1.0f));
    matchunk->setDiffuse(osg::Color4f(0.55f, 0.55f, 0.55f, 1.0f));
    matchunk->setSpecular(osg::Color4f(0.70f, 0.70f, 0.70f, 1.0f));
    matchunk->setShininess(20.0f);
  }
  endEditCP(matchunk);

  osg::ChunkMaterialRefPtr sm(osg::ChunkMaterial::create());
  beginEditCP(sm, osg::ChunkMaterial::ChunksFieldMask);
  { sm->addChunk(matchunk); }
  endEditCP(sm, osg::ChunkMaterial::ChunksFieldMask);

  /*
    Put it all together into a osg::Geometry NodeCore.
  */
  osg::GeometryRefPtr geo(osg::Geometry::create());
  beginEditCP(geo, osg::Geometry::TypesFieldMask | osg::Geometry::LengthsFieldMask |
                       osg::Geometry::IndicesFieldMask | osg::Geometry::PositionsFieldMask |
                       osg::Geometry::ColorsFieldMask | osg::Geometry::NormalsFieldMask |
                       osg::Geometry::TexCoords2FieldMask | osg::Geometry::TexCoords3FieldMask |
                       osg::Geometry::MaterialFieldMask);
  {
    geo->setTypes(type);

    geo->setLengths(lens);
    geo->setIndices(indices);

    /*
      The meaning of the different indices is given by the indexMapping
      field.

      It contains an entry that defines which index for a vertex
      selects which attribute. In this example the first index selects
      the positions, the second is used for colors and normals.

      The number of elements in the indexMapping field defines the
      number of indices used for every vertex.
    */
    if (vFormat.coordinate == VistaVertexFormat::COORDINATE) {
      geo->getIndexMapping().push_back(osg::Geometry::MapPosition);
      geo->setPositions(pnts);
    }

    if (vFormat.color == VistaVertexFormat::COLOR_RGB) {
      geo->getIndexMapping().push_back(osg::Geometry::MapColor);
      geo->setColors(colors);
    }

    if (vFormat.normal == VistaVertexFormat::NORMAL) {
      geo->getIndexMapping().push_back(osg::Geometry::MapNormal);
      geo->setNormals(norms);
    }

    if (vFormat.textureCoord == VistaVertexFormat::TEXTURE_COORD_2D) {
      geo->getIndexMapping().push_back(osg::Geometry::MapTexCoords);
      geo->setTexCoords(texcoords);
    }

    // assign a material to the geometry to make it visible
    geo->setMaterial(sm); // osg::getDefaultMaterial());
  }
  endEditCP(geo, osg::Geometry::TypesFieldMask | osg::Geometry::LengthsFieldMask |
                     osg::Geometry::IndicesFieldMask | osg::Geometry::PositionsFieldMask |
                     osg::Geometry::ColorsFieldMask | osg::Geometry::NormalsFieldMask |
                     osg::Geometry::TexCoords2FieldMask | osg::Geometry::TexCoords3FieldMask |
                     osg::Geometry::MaterialFieldMask);

  //      osg::GeometryPtr p = osg::calcVertexNormalsGeo(geo, 1.0f);

  if (vFormat.normal == VistaVertexFormat::NORMAL_AUTO)
    osg::calcVertexNormals(geo, 45.0f);

  // osg::createSharedIndex(geo);
  // osg::calcFaceNormals(geo);

  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  pOpenSGData->SetGeometry(geo);
  //      pOpenSGData->SetIndexed( true );
  //      pOpenSGData->SetFaceType( fType );

  return true;
}

bool VistaOpenSGGraphicsBridge::CreateIndexedGeometry(const vector<VistaIndexedVertex>& vertices,
    const vector<float>& coords, const vector<float>& textureCoords, const vector<float>& normals,
    const vector<VistaColor>& colorsRGB, const VistaVertexFormat& vFormat,
    const VistaGeometry::FaceType fType, IVistaGeometryData* pData) {
  /*
    osg::Geometry data in OpenSG is stored in several separate vectors.

    These vectors are not a direct part of the osg::Geometry Core but
    rather split up into multiple separate classes.

    These classes, the GeoProperties, contain a single field containg
    their values, which can be accessed directly, see the docs for
    GeoProperty for the whole interface.
  */

  /*
    The first part: the primtive types.
    These are taken from OpenGL, any values that can be passed to
    glBegin(); are legal. Different types can be freely mixed.

    All properties have only one field, which has the same name for every
    property, thus the mask is also called the same for each property.
  */

  osg::GeoPTypesRefPtr   type(osg::GeoPTypesUI8::create());
  osg::GeoPLengthsRefPtr lens(osg::GeoPLengthsUI32::create());

  beginEditCP(type, osg::GeoPTypesUI8::GeoPropDataFieldMask);
  beginEditCP(lens, osg::GeoPLengthsUI32::GeoPropDataFieldMask);
  switch (fType) {
  case VistaGeometry::VISTA_FACE_TYPE_POINTS:
    type->addValue(GL_POINTS);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_LINES:
    type->addValue(GL_LINES);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE:
    type->addValue(GL_LINE_STRIP);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_POLYLINE_LOOP:
    type->addValue(GL_LINE_LOOP);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLES:
    type->addValue(GL_TRIANGLES);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_QUADS:
    type->addValue(GL_QUADS);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_TRIANGLE_STRIP:
    type->addValue(GL_TRIANGLE_STRIP);
    break;
  case VistaGeometry::VISTA_FACE_TYPE_UNDETERMINED:
  case VistaGeometry::VISTA_FACE_TYPE_UNKNOWN:
    VISTA_THROW("CreateIndexedGeometry called with invalid face type", -1);
  }
  lens->addValue(vertices.size());
  endEditCP(type, osg::GeoPTypesUI8::GeoPropDataFieldMask);
  endEditCP(lens, osg::GeoPLengthsUI32::GeoPropDataFieldMask);

  unsigned int i, n(vertices.size());

  /*
    To use different indices for different attributes they have to be
    specified. This is done within the single index property, by using more
    than one index per vertex.

    In this case every vertex reads multiple indices from the Indices
    property. The meaning of every index is defined by the indexMapping
    given below.
  */
  osg::RefPtr<osg::GeoIndicesUI32Ptr> indices(osg::GeoIndicesUI32::create());
  beginEditCP(indices, osg::GeoIndicesUI32::GeoPropDataFieldMask);
  {
    // indices for the polygon
    for (i = 0; i < n; ++i) {
      if (vFormat.coordinate == VistaVertexFormat::COORDINATE)
        indices->push_back(vertices[i].GetCoordinateIndex());
      if (vFormat.color == VistaVertexFormat::COLOR_RGB)
        indices->push_back(vertices[i].GetColorIndex());
      if (vFormat.normal == VistaVertexFormat::NORMAL)
        indices->push_back(vertices[i].GetNormalIndex());
      if (vFormat.textureCoord == VistaVertexFormat::TEXTURE_COORD_2D)
        indices->push_back(vertices[i].GetTextureCoordinateIndex());
    }
  }
  endEditCP(indices, osg::GeoIndicesUI32::GeoPropDataFieldMask);

  /*
    The third part: the vertex positions.

    OpenSG uses different types for vectors and points.

    Points (e.g. Pnt3f) are just positions in space, they have a limited
    set of operations they can handle. Vectors (e.g. Vec3f) are the more
    general kind.
  */
  n = coords.size();
  if (n % 3 != 0)
    vstr::errp() << "[VistaOpenSGGraphicsBridge::CreateIndexedGeometry] - Bad size"
                    "of coordinates vector!"
                 << std::endl;
  osg::RefPtr<osg::GeoPositions3fPtr> pnts(osg::GeoPositions3f::create());
  if (vFormat.coordinate == VistaVertexFormat::COORDINATE) {
    beginEditCP(pnts, osg::GeoPositions3f::GeoPropDataFieldMask);
    {
      for (i = 0; i < n; i += 3)
        pnts->addValue(osg::Pnt3f(coords[i], coords[i + 1], coords[i + 2]));
    }
    endEditCP(pnts, osg::GeoPositions3f::GeoPropDataFieldMask);
  }

  n = colorsRGB.size();
  osg::RefPtr<osg::GeoColors3fPtr> colors(osg::GeoColors3f::create());
  if (vFormat.color == VistaVertexFormat::COLOR_RGB) {
    beginEditCP(colors, osg::GeoColors3f::GeoPropDataFieldMask);
    {
      float col[3];
      for (i = 0; i < n; ++i) {
        colorsRGB[i].GetValues(col);
        colors->push_back(osg::Color3f(col[0], col[1], col[2]));
      }
    }
    endEditCP(colors, osg::GeoColors3f::GeoPropDataFieldMask);
  }

  n = normals.size();
  osg::RefPtr<osg::GeoNormals3fPtr> norms(osg::GeoNormals3f::create());
  if (vFormat.normal == VistaVertexFormat::NORMAL) {
    beginEditCP(norms, osg::GeoNormals3f::GeoPropDataFieldMask);
    {
      for (i = 0; i < n; i += 3)
        norms->push_back(osg::Vec3f(normals[i], normals[i + 1], normals[i + 2]));
    }
    endEditCP(norms, osg::GeoNormals3f::GeoPropDataFieldMask);
  }

  n = textureCoords.size();
  osg::RefPtr<osg::GeoTexCoords2fPtr> texcoords(osg::GeoTexCoords2f::create());
  if (vFormat.textureCoord == VistaVertexFormat::TEXTURE_COORD_2D) {
    beginEditCP(texcoords, osg::GeoTexCoords2f::GeoPropDataFieldMask);
    {
      for (i = 0; i < n; i += 2)
        texcoords->push_back(osg::Vec2f(textureCoords[i], textureCoords[i + 1]));
    }
    endEditCP(texcoords, osg::GeoTexCoords2f::GeoPropDataFieldMask);
  }

  osg::MaterialChunkRefPtr matchunk(osg::MaterialChunk::create());
  beginEditCP(matchunk);
  {
    matchunk->setLit(true);
    matchunk->setAmbient(osg::Color4f(0.01f, 0.01f, 0.01f, 1.0f));
    matchunk->setDiffuse(osg::Color4f(0.55f, 0.55f, 0.55f, 1.0f));
    matchunk->setSpecular(osg::Color4f(0.70f, 0.70f, 0.70f, 1.0f));
    matchunk->setShininess(20.0f);
  }
  endEditCP(matchunk);

  osg::ChunkMaterialRefPtr sm(osg::ChunkMaterial::create());
  beginEditCP(sm);
  { sm->addChunk(matchunk); }
  endEditCP(sm);

  /*
    Put it all together into a osg::Geometry NodeCore.
  */
  osg::GeometryRefPtr geo(osg::Geometry::create());
  beginEditCP(geo, osg::Geometry::TypesFieldMask | osg::Geometry::LengthsFieldMask |
                       osg::Geometry::IndicesFieldMask | osg::Geometry::PositionsFieldMask |
                       osg::Geometry::ColorsFieldMask | osg::Geometry::NormalsFieldMask |
                       osg::Geometry::TexCoords1FieldMask |
                       // osg::Geometry::TexCoords2FieldMask |
                       // osg::Geometry::TexCoords3FieldMask |
                       osg::Geometry::MaterialFieldMask);
  {
    geo->setTypes(type);
    geo->setLengths(lens);
    geo->setIndices(indices);
    /*
      The meaning of the different indices is given by the indexMapping
      field.

      If contains an entry that defines which index for a vertex
      selects which attribute. In this example the first index selects
      the positions, the second is used for colors and normals.

      The number of elements in the indexMapping field defines the
      number of indices used for every vertex.
    */
    if (vFormat.coordinate == VistaVertexFormat::COORDINATE) {
      geo->getIndexMapping().push_back(osg::Geometry::MapPosition);
      geo->setPositions(pnts);
    }

    if (vFormat.color == VistaVertexFormat::COLOR_RGB) {
      geo->getIndexMapping().push_back(osg::Geometry::MapColor);
      geo->setColors(colors);
    }

    if (vFormat.normal == VistaVertexFormat::NORMAL) {
      geo->getIndexMapping().push_back(osg::Geometry::MapNormal);
      geo->setNormals(norms);
    }

    if (vFormat.textureCoord == VistaVertexFormat::TEXTURE_COORD_2D) {
      geo->getIndexMapping().push_back(osg::Geometry::MapTexCoords);
      geo->setTexCoords(texcoords);
    }

    // assign a material to the geometry to make it visible
    geo->setMaterial(sm); // osg::getDefaultMaterial());
  }
  endEditCP(geo, osg::Geometry::TypesFieldMask | osg::Geometry::LengthsFieldMask |
                     osg::Geometry::IndicesFieldMask | osg::Geometry::PositionsFieldMask |
                     osg::Geometry::ColorsFieldMask | osg::Geometry::NormalsFieldMask |
                     osg::Geometry::TexCoords1FieldMask | osg::Geometry::MaterialFieldMask);

  if (vFormat.normal == VistaVertexFormat::NORMAL_AUTO)
    osg::calcVertexNormals(geo, 45.0f);

  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  pOpenSGData->SetGeometry(geo);

  return true;
}

bool VistaOpenSGGraphicsBridge::DeleteGeometryData(IVistaGeometryData* pData) const {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  delete pOpenSGData;
  return true;
}

// ============================================================================

IVistaGeometryData* VistaOpenSGGraphicsBridge::NewGeometryData() const {
  return new VistaOpenSGGeometryData;
}

bool VistaOpenSGGraphicsBridge::GetMaterialByIndex(int iIndex, VistaMaterial& oIn) const {
  //      oIn = m_vMaterialTable[iIndex];

  unsigned int i, n(m_vMaterialTable.size());
  for (i = 0; i < n; ++i)
    if (m_vMaterialTable[i].GetMaterialIndex() == iIndex)
      oIn = m_vMaterialTable[i];
  return true;
}

bool VistaOpenSGGraphicsBridge::GetOSGMaterialByIndex(int iIndex, osg::ChunkMaterialPtr& stm) {
  unsigned int i, n(m_vMaterialTable.size());
  for (i = 0; i < n; ++i)
    if (m_vMaterialTable[i].GetMaterialIndex() == iIndex) {
      stm = m_vOSGMatTab[i];
      return true;
    }
  return false;
}

bool VistaOpenSGGraphicsBridge::GetMaterialByName(
    const std::string& sName, VistaMaterial& oIn) const {
  string       sCurrentName;
  unsigned int i, n(m_vMaterialTable.size());
  for (i = 0; i < n; ++i) {
    sCurrentName = m_vMaterialTable[i].GetName();
    if (sCurrentName.compare(sName) == 0) {
      oIn = m_vMaterialTable[i];
      return true;
    }
  }
  return false;
}

IVistaGeometryData* VistaOpenSGGraphicsBridge::CloneGeometryData(
    const IVistaGeometryData* pGeomData) const {
  const VistaOpenSGGeometryData* pOpenSGData =
      static_cast<const VistaOpenSGGeometryData*>(pGeomData);
  VistaOpenSGGeometryData* pNewData = static_cast<VistaOpenSGGeometryData*>(NewGeometryData());
  pNewData->SetGeometry(osg::GeometryPtr::dcast(osg::deepClone(pOpenSGData->GetGeometry())));
  return pNewData;
}

// convenience function in/for the OpenSG bridge
bool VistaOpenSGGraphicsBridge::SetMaterial(
    const VistaMaterial& oMaterial, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);

  float amb[3], dif[3], spe[3], emi[3];
  oMaterial.GetAmbientColor(amb);
  oMaterial.GetDiffuseColor(dif);
  oMaterial.GetSpecularColor(spe);
  oMaterial.GetEmissionColor(emi);
  float nOpacity   = oMaterial.GetOpacity();
  float nShininess = oMaterial.GetShininess();

#if 0
	/*
	 * @todo do we need to preserve the whole material state on "SetMaterial?"
	 * the vista interface only offers "material" and "texture"...
	 */
	osg::SimpleMaterialPtr ptrMat = osg::SimpleMaterial::create();
	beginEditCP(ptrMat);
	{
		ptrMat->setAmbient (osg::Color3f(amb[0], amb[1], amb[2]));
		ptrMat->setDiffuse (osg::Color3f(dif[0], dif[1], dif[2]));
		ptrMat->setSpecular(osg::Color3f(spe[0], spe[1], spe[2]));
		ptrMat->setEmission(osg::Color3f(emi[0], emi[1], emi[2]));
		ptrMat->setTransparency(1.0f - opa);
		ptrMat->setShininess(shi);
		ptrMat->setLit(true);
	}
	endEditCP  (ptrMat);

	osg::GeometryPtr ptrGeo = pOpenSGData->GetGeometry();
	beginEditCP(ptrGeo, osg::Geometry::MaterialFieldMask);
	{
		ptrGeo->setMaterial(ptrMat);
	}
	endEditCP  (ptrGeo, osg::Geometry::MaterialFieldMask);

#else

  osg::ChunkMaterialRefPtr stm(
      osg::ChunkMaterialPtr::dcast(pOpenSGData->GetGeometry()->getMaterial()));
  if (stm == osg::NullFC)
    return false;

  osg::StateChunkPtr stateChunk(osg::NullFC);
  // search for an existing MaterialChunk
  stateChunk = stm->find(osg::MaterialChunk::getClassType());

  osg::MaterialChunkRefPtr matchunk(osg::NullFC);

  osg::BlendChunkRefPtr blendchunk(osg::BlendChunk::create());
  beginEditCP(blendchunk);
  blendchunk->setSrcFactor(GL_SRC_ALPHA);
  blendchunk->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
  endEditCP(blendchunk);

  if (stateChunk != osg::NullFC) {
    matchunk = osg::MaterialChunkPtr::dcast(stateChunk);
    if (nOpacity != 1.0f) {
      beginEditCP(stm);
      stm->addChunk(blendchunk);
      endEditCP(stm);
    } else {
      stateChunk = stm->find(osg::BlendChunk::getClassType());
      if (stateChunk != osg::NullFC) {
        beginEditCP(stm);
        stm->subChunk(stateChunk);
        endEditCP(stm);
      }
    }
  } else {
    matchunk = osg::MaterialChunk::create();
    beginEditCP(stm);
    {
      if (nOpacity != 1.0f)
        stm->addChunk(blendchunk);
      else {
        stateChunk = stm->find(osg::BlendChunk::getClassType());
        if (stateChunk != osg::NullFC) {
          beginEditCP(stm);
          stm->subChunk(stateChunk);
          endEditCP(stm);
        }
      }
      stm->addChunk(matchunk);
    }
    endEditCP(stm);
  }

  beginEditCP(matchunk);
  {
    matchunk->setAmbient(osg::Color4f(amb[0], amb[1], amb[2], 1.0f));
    matchunk->setDiffuse(osg::Color4f(dif[0], dif[1], dif[2], nOpacity));
    matchunk->setSpecular(osg::Color4f(spe[0], spe[1], spe[2], 1.0f));
    matchunk->setEmission(osg::Color4f(emi[0], emi[1], emi[2], 1.0f));
    matchunk->setShininess(nShininess);
    matchunk->setLit(true);
  }
  endEditCP(matchunk);

#endif

  return true;
}

bool VistaOpenSGGraphicsBridge::SetMaterialIndex(
    const int& materialIndex, IVistaGeometryData* pData) {
  VISTA_THROW_NOT_IMPLEMENTED

  // IAR: to be renewed

  // osg::ChunkMaterialPtr stm = osg::NullFC;

  // if( GetOSGMaterialByIndex(materialIndex,stm) )
  //{
  //	osg::MaterialChunkPtr matchunk =
  //osg::MaterialChunkPtr::dcast(stm->find(MaterialChunk::getClassType()));

  //	if(matchunk == osg::NullFC)
  //	{
  //		/** @todo look at this */
  //		return false;
  //	}

  //	// check for an existing blend chunk

  //	osg::BlendChunkPtr blendchunk =
  //osg::BlendChunkPtr::dcast(stm->find(osg::BlendChunk::getClassType())); 	osg::Real32 opacity
  //= 1.0f; 	if(blendchunk == osg::NullFC)
  //	{
  //		/** @todo we should check this against the chosen model */
  //		opacity = 1.0f - blendchunk->getAlphaValue();
  //	}
  //}

  // IAR: OLD CODE

  ////	VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  ////	osg::ChunkMaterialPtr mt = osg::NullFC;
  ////	osg::MaterialPtr pMt = pOpenSGData->GetGeometry()->getMaterial();
  ////	//static int iSortKey = 0;
  ////	if( pMt == osg::NullFC )
  ////	{
  ////		// no material, yet?
  ////		// create a new one
  ////		mt = osg::ChunkMaterial::create();

  ////		beginEditCP(mt);
  ////		mt->addChunk(matchunk);
  ////		if(matchunk->getAmbient()[3]!=1.0f)
  ////			mt->addChunk(blendchunk);
  ////		else
  ////		{
  ////			stateChunk = osg::NullFC;
  ////			stateChunk = mt->find(BlendChunk::getClassType());
  ////			if( stateChunk != osg::NullFC )
  ////			{
  ////				beginEditCP(mt);
  ////						mt->subChunk(stateChunk);
  ////				endEditCP(mt);
  ////			}
  ////		}
  ////		//mt->setSortKey(++iSortKey);
  ////		endEditCP(mt);

  ////		pOpenSGData->GetGeometry()->setMaterial(mt);
  ////	}
  ////	else
  ////	{
  ////		mt = osg::ChunkMaterialPtr::dcast(pMt);
  ////		if(mt == osg::NullFC)
  ////			return false; // ok... give up

  ////		stateChunk = osg::NullFC;
  ////		//search for an existing MaterialChunk
  ////		stateChunk = mt->find(MaterialChunk::getClassType());

  ////		beginEditCP(mt);
  ////			if(stateChunk != osg::NullFC)
  ////				mt->subChunk(stateChunk);
  ////			if(matchunk->getAmbient()[3]!=1.0f)
  ////				mt->addChunk(blendchunk);
  ////			else
  ////			{
  ////				stateChunk = osg::NullFC;
  ////				stateChunk = mt->find(BlendChunk::getClassType());
  ////				if( stateChunk != osg::NullFC )
  ////				{
  ////					beginEditCP(mt);
  ////							mt->subChunk(stateChunk);
  ////					endEditCP(mt);
  ////				}
  ////			}
  ////			mt->addChunk(matchunk);
  ////			//mt->setSortKey(++iSortKey);
  ////		endEditCP(mt);
  ////	}
  ////	return true;
  ////}

  ////return false;
}

bool VistaOpenSGGraphicsBridge::GetIsStatic(const IVistaGeometryData* pData) {
  const VistaOpenSGGeometryData* pOpenSGData = static_cast<const VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr               p           = pOpenSGData->GetGeometry();
  return p->getDlistCache();
}

void VistaOpenSGGraphicsBridge::SetIsStatic(bool bIsStatic, IVistaGeometryData* pData) {
  VistaOpenSGGeometryData* pOpenSGData = static_cast<VistaOpenSGGeometryData*>(pData);
  osg::GeometryPtr         p           = pOpenSGData->GetGeometry();

  beginEditCP(p, osg::Geometry::DlistCacheFieldMask);
  p->setDlistCache(bIsStatic);
  endEditCP(p, osg::Geometry::DlistCacheFieldMask);
}

bool VistaOpenSGGraphicsBridge::CreateMaterialTable(void) {
  m_vMaterialTable.clear();
  return true;
}

bool VistaOpenSGGraphicsBridge::GetVertices(
    std::vector<VistaIndexedVertex>& vertices, const IVistaGeometryData*) const {
  VISTA_THROW_NOT_IMPLEMENTED; /** @todo implment...or not...gonna cut the graphics-backend
                                  abstraction down anyway!*/
}

int VistaOpenSGGraphicsBridge::AddMaterial(const VistaMaterial& oMaterial) {
  // get material properties from ViSTA object
  float amb[3], dif[3], spe[3], emi[3];
  oMaterial.GetAmbientColor(amb);
  oMaterial.GetDiffuseColor(dif);
  oMaterial.GetSpecularColor(spe);
  oMaterial.GetEmissionColor(emi);
  float nOpacity   = oMaterial.GetOpacity();
  float nShininess = oMaterial.GetShininess();
  float nAlpha     = nOpacity;

  osg::MaterialChunkPtr matchunk = osg::MaterialChunk::create();
  beginEditCP(matchunk);
  {
    matchunk->setAmbient(osg::Color4f(amb[0], amb[1], amb[2], nAlpha));
    matchunk->setDiffuse(osg::Color4f(dif[0], dif[1], dif[2], nAlpha));
    matchunk->setSpecular(osg::Color4f(spe[0], spe[1], spe[2], nAlpha));
    matchunk->setEmission(osg::Color4f(emi[0], emi[1], emi[2], nAlpha));
    matchunk->setShininess(nShininess);
    matchunk->setLit(true);
  }
  endEditCP(matchunk);

  // store the retrieved Information in an OpenSG object
  osg::ChunkMaterialPtr stm = osg::ChunkMaterial::create();
  beginEditCP(stm);
  { stm->addChunk(matchunk); }
  endEditCP(stm);

  // cache all OpenSG data to prevent inefficient material-merge-scene-graph-operations
  m_vOSGMatTab.push_back(stm);

  m_vMaterialTable.push_back(oMaterial);
  m_vMaterialTable.back().SetMaterialIndex((int)m_vMaterialTable.size() - 1);
  return (int)m_vMaterialTable.size() - 1;
}

bool VistaOpenSGGraphicsBridge::SetCoordIndices(
    const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) {
  VISTA_THROW_NOT_IMPLEMENTED; /** @todo */
}

bool VistaOpenSGGraphicsBridge::SetTextureCoordsIndices(
    const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) {
  VISTA_THROW_NOT_IMPLEMENTED; /** @todo */
}

bool VistaOpenSGGraphicsBridge::SetNormalIndices(
    const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) {
  VISTA_THROW_NOT_IMPLEMENTED; /** @todo */
}

bool VistaOpenSGGraphicsBridge::SetColorIndices(
    const int startIdx, const std::vector<int>& indices, IVistaGeometryData*) {
  VISTA_THROW_NOT_IMPLEMENTED; /** @todo */
}

int VistaOpenSGGraphicsBridge::GetNumberOfMaterials() {
  return (int)m_vMaterialTable.size();
}

bool VistaOpenSGGraphicsBridge::BindToVistaMaterialTable(IVistaGeometryData* pData) {
  return true;
}
