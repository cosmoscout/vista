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

#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/GraphicsManager/VistaAxes.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaMath/VistaVector.h>

#include <VistaDataFlowNet/VdfnActionNode.h>
#include <VistaDataFlowNet/VdfnObjectRegistry.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>

#include <VistaBase/VistaExceptionBase.h>

#include "PhantomWorkspaceHandler.h"
#include "VdfnPlaneConstraintNode.h"
#include "WorkspaceActionObject.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

void CreateScene(VistaSystem&);
void CreateCallbacks(VistaSystem&);

VistaTransformNode*      m_pHandleParent;
VistaTransformNode*      m_pHandle;
VdfnActionNode*          m_pWorkspaceNode;
WorkspaceActionObject*   m_pWorkspaceActionObject;
PhantomWorkspaceHandler* m_pWorkspaceHandler;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/**
 * This demo does *NOT* use the shared configfiles cause
 * of the phantom driver.
 */
int main(int argc, char** argv) {
  int ret = 0xdead;

  try {

    // Create the system
    VistaSystem pVistaSystem;

    // add path to configfiles
    std::list<std::string> liPathes;
    liPathes.push_back("configfiles/");
    pVistaSystem.SetIniSearchPaths(liPathes);

    // initialize the VistaSystem
    if (pVistaSystem.Init(argc, argv)) {
      // Place the visible elements in the scene
      CreateScene(pVistaSystem);
      // Register callbacks
      CreateCallbacks(pVistaSystem);
      // Register the constraint node in the dfn
      VdfnNodeFactory* pFac = VdfnNodeFactory::GetSingleton();
      pFac->SetNodeCreator(
          "planeconstraint", new VdfnPlaneConstraintNodeCreate(pVistaSystem.GetDriverMap()));

      if (pVistaSystem.GetDisplayManager()->GetDisplaySystem(0) == 0)
        VISTA_THROW("No DisplaySystem found", 1);

      // Set the title
      pVistaSystem.GetDisplayManager()
          ->GetWindowByName("MAIN_WINDOW")
          ->GetWindowProperties()
          ->SetTitle(argv[0]);
      // Run the stuff
      ret = pVistaSystem.Run();
    }
  } catch (VistaExceptionBase& e) {
    e.PrintException();
    ret = -1;
  }

  // clean up
  delete m_pHandleParent;
  delete m_pHandle;
  delete m_pWorkspaceNode;
  delete m_pWorkspaceActionObject;
  delete m_pWorkspaceHandler;

  return ret;
}

/**
 * This methods adds the visible parts to the scene
 */
void CreateScene(VistaSystem& vistaSystem) {
  // getting the scenegraph
  VistaSceneGraph* vistaSG = vistaSystem.GetGraphicsManager()->GetSceneGraph();

  // factory to create geometries
  VistaGeometryFactory gf(vistaSG);

  // create handle to move around
  // note: This node is attached to the root but it will get transformed
  // in the PhantomWorkspaceHandler cause the cam is not part of the
  // VistaSceneGraph.
  // @todo as soon as possible hang this node under the cam
  m_pHandleParent = vistaSG->NewTransformNode(vistaSG->GetRoot());
  // m_pHandleParent->SetTranslation(0,0,10);

  VistaGeometry* plane = gf.CreatePlane(3, 3, 1, 1, Vista::Y, VistaColor::YELLOW);
  vistaSG->NewGeomNode(vistaSG->GetRoot(), plane);

  // group the handle's elements
  m_pHandle = vistaSG->NewTransformNode(m_pHandleParent);
  m_pHandle->SetName("HANDLE");

  VistaGeometry* handleBall = gf.CreateSphere(0.05f);
  vistaSG->NewGeomNode(m_pHandle, handleBall);

  VistaGeometry* x = gf.CreateCone(0.01f, 0.01f, 0.15f);
  x->SetColor(VistaColor::RED);
  VistaTransformNode* xNode = vistaSG->NewTransformNode(m_pHandle);
  // turn around 90 degrees
  xNode->SetRotation(
      VistaQuaternion(VistaAxisAndAngle(VistaVector3D(0, 0, 1), Vista::DegToRad(90))));
  vistaSG->NewGeomNode(xNode, x);

  VistaGeometry* y = gf.CreateCone(0.01f, 0.01f, 0.15f);
  y->SetColor(VistaColor::GREEN);
  vistaSG->NewGeomNode(m_pHandle, y);

  VistaGeometry* z = gf.CreateCone(0.01f, 0.01f, 0.15f);
  z->SetColor(VistaColor::BLUE);
  VistaTransformNode* zNode = vistaSG->NewTransformNode(m_pHandle);
  // turn around 90 degrees
  zNode->SetRotation(
      VistaQuaternion(VistaAxisAndAngle(VistaVector3D(1, 0, 0), Vista::DegToRad(90))));
  vistaSG->NewGeomNode(zNode, z);

  // Create axes, to see how are you moving
  VistaAxes axes(vistaSG);
  vistaSG->GetRoot()->AddChild(axes.GetVistaNode());
}

/**
 * This node registers the callbacks and inits the stuff which has to be done
 * at runtime.
 */
void CreateCallbacks(VistaSystem& vistaSystem) {

  VistaEventManager* eventMgr = vistaSystem.GetEventManager();
  // Register the handler for system events, here the pregraphics event
  // this handler gets an update *before* each frame and reads the transformation
  // from the virtual platform (CAM:MAIN) and applies this to the transformnode above the
  // handle and the workspace boundingbox
  m_pWorkspaceHandler =
      new PhantomWorkspaceHandler(vistaSystem.GetDfnObjectRegistry(), m_pHandleParent);
  eventMgr->AddEventHandler(
      m_pWorkspaceHandler, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

  VistaSceneGraph* pVistaSceneGraph = vistaSystem.GetGraphicsManager()->GetSceneGraph();
  // create a new WorkspaceActionObject with all needed data
  m_pWorkspaceActionObject =
      new WorkspaceActionObject(m_pHandleParent, m_pHandle, pVistaSceneGraph);
  // set the name for the nameableinterface
  // This is not the name to reference it in the DFN
  m_pWorkspaceActionObject->SetNameForNameable("WorkspaceActionObject");
  // Register it in the DFN
  // This creates automagic an VdfnActionNode
  // The name should be the same as the nameable but it not mandatory
  // with this name you can reference the node  in the DFN
  VdfnObjectRegistry* pDfnRegistry = vistaSystem.GetDfnObjectRegistry();
  pDfnRegistry->SetObject("WorkspaceActionObject", m_pWorkspaceActionObject, NULL);
}
