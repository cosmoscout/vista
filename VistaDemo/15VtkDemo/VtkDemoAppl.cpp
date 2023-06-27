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

#include "VtkDemoAppl.h"
#include "VtkDrawObject.h"

// new graphics manager stuff
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

// old graphics manager stuff
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>

#include <vtkActorCollection.h>

#include <VistaBase/VistaExceptionBase.h>

using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VtkDemoAppl::VtkDemoAppl(int argc, char* argv[])
    : m_pVtkRoot(NULL)
    , m_pVtkDrawObj(NULL) {

  std::list<std::string> liSearchPaths;
  liSearchPaths.push_back("../configfiles/");
  mVistaSystem.SetIniSearchPaths(liSearchPaths);
  mVistaSystem.IntroMsg();

  if (!mVistaSystem.Init(argc, argv))
    return;

  CreateScene();
  if (mVistaSystem.GetDisplayManager()->GetDisplaySystem(0) == 0)
    VISTA_THROW("No DisplaySystem found", 1);

  mVistaSystem.GetDisplayManager()
      ->GetWindowByName("MAIN_WINDOW")
      ->GetWindowProperties()
      ->SetTitle(argv[0]);
}

VtkDemoAppl::~VtkDemoAppl() {
  //  if (m_pVtkDrawObj)
  // m_pVtkDrawObj->Delete();	// avoid reference count problem (ms 030120)
  m_pVtkDrawObj = NULL;

  if (m_pVtkRoot)
    delete m_pVtkRoot;
  m_pVtkRoot = NULL;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Run                                                         */
/*                                                                            */
/*============================================================================*/
bool VtkDemoAppl::Run() {
  // Start Universe
  mVistaSystem.Run();

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateScene                                                 */
/*                                                                            */
/*============================================================================*/
bool VtkDemoAppl::CreateScene() {
  std::cout << "VtkDemoAppl - using new graphics manager..." << std::endl;

  VistaGraphicsManager* pGraphicsManager = mVistaSystem.GetGraphicsManager();
  VistaSceneGraph*      pSG              = pGraphicsManager->GetSceneGraph();

  m_pVtkDrawObj = new VtkDrawObject;
  pSG->GetRoot()->SetName("ViSTA-ROOT");
  m_pVtkRoot = pSG->NewOpenGLNode(pSG->GetRoot(), m_pVtkDrawObj);
  m_pVtkRoot->SetName("VTK demo node");

  return true;
}
