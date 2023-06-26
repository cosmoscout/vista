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

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

// Vista stuff
#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

// Only for warning needed:
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTextNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaBase/VistaExceptionBase.h>

// Demo stuff
#include "KeyboardInteraction.h"
#include "MsgPortUser.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaMsgChannel.h>

/*============================================================================*/
/* DEFINES					                                                  */
/*============================================================================*/

// shows you how to register a
const static int KEYBOARD_SOMEBUTTONS = 1; // keyboard button - simple!
const static int MSGPORT_USER         = 2; // messageport, networking...

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

int main(int argc, char* argv[]) {
  try {
    VistaSystem* pVistaSystem = new VistaSystem;

    pVistaSystem->IntroMsg();

    std::list<std::string> liSearchPaths;
    liSearchPaths.push_back("configfiles/");
    liSearchPaths.push_back("../configfiles/");
    pVistaSystem->SetIniSearchPaths(liSearchPaths);

    if (pVistaSystem->Init(argc, argv)) {

      // Interaction can get very complex so you can use a bit mask to enable different
      // interaction stuff
      // Note: Simple keyboard Interaction is implemented per .ini file
      //		 type '?' for more information.
      //		 also the mouse is registered as trackball node.
      // Configure the bitmask like this here:
      // int nMask = KEYBOARD_SOMEBUTTONS | MESSAGEPORT
      // this enables some keyboard and messageport usage  at this demo
      // have a look at the different files for implementation

      int nMask = KEYBOARD_SOMEBUTTONS | MSGPORT_USER;

      if (nMask & KEYBOARD_SOMEBUTTONS) {
        KeyboardInteraction::RegisterSomeButtons(pVistaSystem);
      }
      if (nMask & MSGPORT_USER) {
        MsgPortUser::EchoToCout(pVistaSystem);
      }

      // To visualize the output from the CallStack we proceed in creating a visual CallStack :D
      // Therefore get the SceneGraph
      VistaSceneGraph* pSG = pVistaSystem->GetGraphicsManager()->GetSceneGraph();

      // Setting the scene a bit away so we can see everything from start screen :)
      VistaTransformNode* pTrans             = pSG->NewTransformNode(pSG->GetRoot());
      VistaTransformNode* pTextTransformNode = pSG->NewTransformNode(pTrans);
      pTextTransformNode->Translate(-6.8f, 0.0f, -15.0f);
      VistaTextNode* pText3D = pSG->NewTextNode(pTextTransformNode, "ARIAL");
      // Fill with text
      if (pText3D) {
        pText3D->SetText("Check the command line! Press B");
      }

      if (pVistaSystem->GetDisplayManager()->GetDisplaySystem(0) == 0)
        VISTA_THROW("No DisplaySystem found", 1);

      pVistaSystem->GetDisplayManager()
          ->GetWindowByName("MAIN_WINDOW")
          ->GetWindowProperties()
          ->SetTitle(argv[0]);

      pVistaSystem->Run();
    }

    // clean up - everything else is managed (and deleted) by the VistaSystem
    delete pVistaSystem;
  } catch (VistaExceptionBase& e) { e.PrintException(); }

  return 0;
}

/* tmp
old lib includes
ws2_32.lib
opengl32.lib
glu32.lib
glaux.lib
wsock32.lib
winmm.lib
OSGBaseD.lib
OSGSystemD.lib
OSGWindowGLUTD.lib
*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
