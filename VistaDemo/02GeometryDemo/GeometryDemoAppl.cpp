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

#include "GeometryDemoAppl.h"

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
GeometryDemoAppl::GeometryDemoAppl()
    : m_pVistaSystem(NULL)
    , m_pPlane(NULL)
    , m_pBox(NULL)
    , m_pDisk(NULL)
    , m_pCone(NULL)
    , m_pTorus(NULL)
    , m_pEllipsoid(NULL)
    , m_pSphere(NULL)
    , m_pTriangle(NULL) {
}

GeometryDemoAppl::~GeometryDemoAppl() {
  delete m_pVistaSystem;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool GeometryDemoAppl::Init(int argc, char* argv[]) {
  m_pVistaSystem = new VistaSystem;

  std::list<std::string> liSearchPaths;

  // here is a shared configfile used for the demos
  liSearchPaths.push_back("../configfiles/");
  m_pVistaSystem->SetIniSearchPaths(liSearchPaths);

  // ViSTA's standard intro message
  m_pVistaSystem->IntroMsg();

  if (!m_pVistaSystem->Init(argc, argv))
    return false;

  if (m_pVistaSystem->GetDisplayManager()->GetDisplaySystem() == 0)
    VISTA_THROW("No DisplaySystem found", 1);

  // Get the scenegraph, which manages the drawable objects
  VistaSceneGraph* pSceneGraph = m_pVistaSystem->GetGraphicsManager()->GetSceneGraph();

  // First, we need some geometry to draw. For simple shapes, we can
  // use the VistaGeometryFactory to create them.  Since Geometries
  // can be reused multiple times, they are reference-counted
  // internally for eafch GeomNode that uses them, and are
  // automatically deleted once the last GeomNode referencing them
  // is deleted. Take care, however, that this means that a Geometry
  // you created will become invalid once all referencing nodes are
  // deleted, so you cannot reuse it after cleaning the whole
  // scenegraph.
  VistaGeometryFactory oGeometryFactory(pSceneGraph);

  // Create a plane
  m_pPlane = oGeometryFactory.CreatePlane(1.0f, 1.0f, 1, 1, Vista::Z, VistaColor::MAROON);

  // Create a box
  m_pBox = oGeometryFactory.CreateBox();

  // Create a disk
  m_pDisk = oGeometryFactory.CreateDisk(0.7f, 16, 4, Vista::Z, VistaColor::DARK_TURQUOISE);
  // we set rendering attributes to wireframe to make the topology
  // visible as an example.
  VistaRenderingAttributes raWireFrame(VistaRenderingAttributes::SHADING_DEFAULT,
      VistaRenderingAttributes::CULL_DEFAULT, VistaRenderingAttributes::STYLE_WIREFRAME);
  m_pDisk->SetRenderingAttributes(raWireFrame);

  // Create a cone
  m_pCone = oGeometryFactory.CreateCone();

  // Create a torus
  m_pTorus = oGeometryFactory.CreateTorus(0.3f, 0.4f, 30, 20, VistaColor::ROYAL_BLUE);

  // Create an ellipsoid
  m_pEllipsoid = oGeometryFactory.CreateEllipsoid(0.3f, 1.2f, 0.2f);

  // Create a sphere
  m_pSphere = oGeometryFactory.CreateSphere(1, 300, VistaColor::WHITE);
  // set a nice texture
  m_pSphere->SetTexture("../data/bus.jpg");

  m_pTriangle = oGeometryFactory.CreateTriangle();

  // We now have created the geometries, but still need to tell the
  // scenegraph to render them.  Therefore, we first will use to
  // types of nodes here: TransformNodes and GeomNodes GeomNodes
  // tell the scenegraph to actually draw the Geometry linked to
  // them TransformNodes build a hierarchy that determines the
  // position of the objects in GeomNodes Note that if the nodes are
  // added to the SceneGraph's tree (i.e. they can be reached from
  // the SceneGraphs root node), they will automatically be deleted,
  // so we don't have to care about that

  // we will first create two TransformNodes: One that moves the
  // whole scenery, and one for each primitive which moves the them
  // relative to each other.
  VistaTransformNode* pSceneryTrans = pSceneGraph->NewTransformNode(pSceneGraph->GetRoot());
  pSceneryTrans->Translate(0, 0, -10.0);

  // notice that we now add the primitive TransformNodes as
  // childnode of the SceneryTrans. Thus, in the end, they
  // will be affected by both transformnodes
  VistaTransformNode* pPlaneTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pPlaneTrans->Translate(-3, 1, -1);

  VistaTransformNode* pBoxTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pBoxTrans->Translate(-1, 1, -1);

  VistaTransformNode* pDiskTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pDiskTrans->Translate(1, 1, -1);

  VistaTransformNode* pConeTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pConeTrans->Translate(3, 1, -1);

  VistaTransformNode* pTorusTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pTorusTrans->Translate(-3, -1, -1);

  VistaTransformNode* pEllipsoidTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pEllipsoidTrans->Translate(-1, -1, -1);

  VistaTransformNode* pSphereTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pSphereTrans->Translate(1, -1, -1);

  VistaTransformNode* pTriangleTrans = pSceneGraph->NewTransformNode(pSceneryTrans);
  pTriangleTrans->Translate(3, -1, -1);

  // We now add the GeomNodes holding our primitives
  pSceneGraph->NewGeomNode(pPlaneTrans, m_pPlane);
  pSceneGraph->NewGeomNode(pBoxTrans, m_pBox);
  pSceneGraph->NewGeomNode(pDiskTrans, m_pDisk);
  pSceneGraph->NewGeomNode(pConeTrans, m_pCone);
  pSceneGraph->NewGeomNode(pTorusTrans, m_pTorus);
  pSceneGraph->NewGeomNode(pEllipsoidTrans, m_pEllipsoid);
  pSceneGraph->NewGeomNode(pSphereTrans, m_pSphere);
  pSceneGraph->NewGeomNode(pTriangleTrans, m_pTriangle);

  m_pVistaSystem->GetDisplayManager()
      ->GetWindowByName("MAIN_WINDOW")
      ->GetWindowProperties()
      ->SetTitle(argv[0]);

  return true;
}

void GeometryDemoAppl::Run() {
  // Start Universe
  m_pVistaSystem->Run();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
