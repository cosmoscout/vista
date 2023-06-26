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

// include header here

#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaKernel/GraphicsManager/VistaAxes.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

void FitToScreen(VistaVirtualPlatform* pPlatform, const IVistaNode* pRoot) {
  // this is correct for a fov of 90 degrees  only.

  VistaVector3D v3Diagonal, v3Center, v3Min, v3Max;
  pRoot->GetBoundingBox(v3Min, v3Max);
  v3Diagonal = v3Min - v3Max;

  float fDiameter = v3Diagonal.GetLength();

  v3Center = v3Min - 0.5f * v3Diagonal;

  pPlatform->SetTranslation(VistaVector3D(v3Center[0], v3Center[1], 2 * fDiameter));
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int main(int argc, char** argv) {
  try {
    // Loading shared configfile.
    std::list<std::string> liSearchPath;
    liSearchPath.push_back("../configfiles/");

    VistaSystem* pVistaSystem = new VistaSystem;

    pVistaSystem->SetIniSearchPaths(liSearchPath);
    if (pVistaSystem->Init(argc, argv)) {
      // load a model, so we have something to look at
      VistaSceneGraph* pSG = pVistaSystem->GetGraphicsManager()->GetSceneGraph();

      std::string filename = "../data/cow.obj";

      IVistaNode* pNode = pSG->LoadNode(filename);

      if (pNode) {
        VistaTransformNode* pTransformNode = pSG->NewTransformNode(pSG->GetRoot());
        pTransformNode->SetTranslation(5, 5, 5);
        // model loaded, add to root node.
        pTransformNode->AddChild(pNode);
      } else {
        std::cerr << "Error: Modelfile [" << filename << "] not found!" << std::endl;
      }

      // Adding coordination axes to the scene.
      VistaAxes coordAxes(pSG);
      pSG->GetRoot()->AddChild(coordAxes.GetVistaNode());

      // now determine the reference frame from a given display system
      // we query that from the display manager here
      // for this demo, we simply take the first display system (index = 0)
      VistaDisplaySystem* pDispSys = pVistaSystem->GetDisplayManager()->GetDisplaySystem(0);

      if (pDispSys == NULL)
        VISTA_THROW("No DisplaySystem found", 1);

      // each display system has a reference frame, which represents a virtual
      // camera, so we claim that.
      VistaVirtualPlatform* pVirtualPlatform = pDispSys->GetReferenceFrame();

      // move the camera to show the model
      FitToScreen(pVirtualPlatform, pSG->GetRealRoot());

      pVistaSystem->GetDisplayManager()
          ->GetWindowByName("MAIN_WINDOW")
          ->GetWindowProperties()
          ->SetTitle(argv[0]);

      // start the simulation.
      pVistaSystem->Run();

      delete (pVistaSystem);
    }
  } catch (VistaExceptionBase& e) { e.PrintException(); } catch (std::exception& e) {
    std::cerr << "Exception:" << e.what() << std::endl;
  }

  return 0;
}
