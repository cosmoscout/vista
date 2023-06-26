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

// App stuff
#include "DisplayDemoAppl.h"

// Vista stuff
#include <VistaKernel/GraphicsManager/VistaAxes.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaExceptionBase.h>

// STL stuff
#include <list>
#include <string>

class ScreenshotCallback : public IVistaExplicitCallbackInterface {
 public:
  ScreenshotCallback(VistaDisplayManager* pDisplayManager)
      : IVistaExplicitCallbackInterface()
      , m_pDisplayManager(pDisplayManager) {
  }

  bool Do() {
    m_pDisplayManager->MakeScreenshot("MAIN_WINDOW", "dbg_");
    return true;
  }

 private:
  VistaDisplayManager* m_pDisplayManager;
};

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
DisplayDemoAppl::DisplayDemoAppl(int argc, char* argv[])
    : m_vistaSystem(VistaSystem()) {

  m_vistaSystem.IntroMsg();

  std::list<std::string> searchpath;
  searchpath.push_back("configfiles/");
  searchpath.push_back("../10DisplayDemo/configfiles/"); // for bin folder
  m_vistaSystem.SetIniSearchPaths(searchpath);

  m_vistaSystem.Init(argc, argv);

  CreateScene();

  m_vistaSystem.GetKeyboardSystemControl()->BindAction(
      's', new ScreenshotCallback(m_vistaSystem.GetDisplayManager()), "Take screenshot");

  if (m_vistaSystem.GetDisplayManager()->GetDisplaySystem(0) == 0)
    VISTA_THROW("No DisplaySystem found", 1);

  m_vistaSystem.GetDisplayManager()
      ->GetWindowByName("MAIN_WINDOW")
      ->GetWindowProperties()
      ->SetTitle(argv[0]);
}

DisplayDemoAppl::~DisplayDemoAppl() {
  delete pAxes;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Run                                                         */
/*                                                                            */
/*============================================================================*/
void DisplayDemoAppl::Run() {
  // Start Universe
  m_vistaSystem.Run();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateScene                                                 */
/*                                                                            */
/*============================================================================*/
void DisplayDemoAppl::CreateScene() {
  // get OpenGL node dispatcher
  VistaGraphicsManager* pGraphicsMananager = m_vistaSystem.GetGraphicsManager();
  if (pGraphicsMananager) {
    std::cout << "DisplayDemoAppl - using new graphics manager..." << std::endl;

    VistaSceneGraph* pSG = pGraphicsMananager->GetSceneGraph();
    // Get root...
    VistaGroupNode* pRoot = pSG->GetRoot();
    // ... and attach a new groupnode
    VistaGroupNode* pBoxGroup = pSG->NewGroupNode(pRoot);

    // draw axes
    pAxes = new VistaAxes(pSG);
    pBoxGroup->AddChild(pAxes->GetVistaNode());
    // pSG->GetRoot()->AddChild(axes->GetVistaNode());

    // create geometry factory
    VistaGeometryFactory geometryFactory(pSG);

    std::cout << " [DisplayDemoAppl] - creating demo box(es)..." << std::endl;

    VistaGeometry*      pBox      = geometryFactory.CreateBox();
    VistaTransformNode* pBoxTrans = pSG->NewTransformNode(pBoxGroup);
    VistaVector3D       v3Temp(0.0f, 1.0f, -1.8f);
    pBoxTrans->SetTranslation(v3Temp);
    pSG->NewGeomNode(pBoxTrans, pBox);

    pBox      = geometryFactory.CreateBox();
    pBoxTrans = pSG->NewTransformNode(pBoxGroup);
    v3Temp    = VistaVector3D(-1.8f, 1.0f, 0.0f);
    pBoxTrans->SetTranslation(v3Temp);
    pSG->NewGeomNode(pBoxTrans, pBox);

    pBox      = geometryFactory.CreateBox();
    pBoxTrans = pSG->NewTransformNode(pBoxGroup);
    v3Temp    = VistaVector3D(1.8f, 1.0f, 0.0f);
    pBoxTrans->SetTranslation(v3Temp);
    pSG->NewGeomNode(pBoxTrans, pBox);

    pBox      = geometryFactory.CreateBox();
    pBoxTrans = pSG->NewTransformNode(pBoxGroup);
    v3Temp    = VistaVector3D(0.0f, 0.0f, 0.0f);
    pBoxTrans->SetTranslation(v3Temp);
    pSG->NewGeomNode(pBoxTrans, pBox);

    Vista2DText* pText2D = m_vistaSystem.GetDisplayManager()->New2DText("MOVEABLE_VIEWPORT");
    // !!!IMPORTANT!!!
    // Call init here and do NOT set text and position seperately
    if (pText2D)
      pText2D->Init("MOVEABLE", 0, 0);

    pText2D = m_vistaSystem.GetDisplayManager()->New2DText("MAIN_VIEWPORT");
    // !!!IMPORTANT!!!
    // Call init here and do NOT set text and position seperately
    if (pText2D)
      pText2D->Init("MAIN", 0, 0);

  } else {
    std::cout << "DisplayDemoAppl - unable to find graphics manager..." << std::endl;
  }
}
