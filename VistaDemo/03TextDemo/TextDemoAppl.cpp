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

#include "TextDemoAppl.h"

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTextNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/DisplayManager/Vista3DTextOverlay.h>
#include <VistaKernel/DisplayManager/VistaSimpleTextOverlay.h>
#include <VistaKernel/DisplayManager/VistaTextEntity.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
TextDemoAppl::TextDemoAppl() {
}

TextDemoAppl::~TextDemoAppl() {
  delete m_pOverlay3D;
  delete m_pSimpleTextOverlay;
  delete m_pTextEntity1;
  delete m_pTextEntity2;
  delete m_pTextEntity3;
  delete m_pVistaSystem;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool TextDemoAppl::Init(int argc, char* argv[]) {
  m_pVistaSystem = new VistaSystem();

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

  // Add node to scenegraph
  VistaTransformNode* pTransform = pSceneGraph->NewTransformNode(pSceneGraph->GetRoot());
  pTransform->SetTranslation(-1, 0, -2);
  VistaTextNode* pTextNode3D = pSceneGraph->NewTextNode(pTransform, "SANS");

  // Fill with text
  if (pTextNode3D) {
    std::cout << "Setting 3D text." << std::endl;
    pTextNode3D->SetText("Hello world! (3D)");
  } else
    std::cerr << "3D Text cannot be initialized" << std::endl;

  VistaDisplayManager* pDispManager = m_pVistaSystem->GetDisplayManager();

  // you can create the VistaSimpleTextOverlay as a local or global Variable!
  // DO NOT forget to set the Position.
  VistaSimpleTextOverlay* pSimpleTOverlay = new VistaSimpleTextOverlay(pDispManager);
  m_pTextEntity1                          = pDispManager->CreateTextEntity();

  m_pTextEntity1->SetText("Hello World 1!");
  m_pTextEntity1->SetXPos(1.5f);
  m_pTextEntity1->SetYPos(1.5f);
  m_pTextEntity1->SetZPos(1.5f);
  m_pTextEntity1->SetColor(VistaColor::CYAN);

  pSimpleTOverlay->AddText(m_pTextEntity1);

  // All methods which start with "Create" will return a pointer
  // note the the method's caller is responsible for this pointer
  // and have to clean up those by themselves!
  m_pSimpleTextOverlay = new VistaSimpleTextOverlay(pDispManager);

  m_pTextEntity2 = pDispManager->CreateTextEntity();
  m_pTextEntity2->SetText("Hello World 2!");
  m_pTextEntity2->SetXPos(3.0f);
  m_pTextEntity2->SetYPos(3.0f);
  m_pTextEntity2->SetZPos(3.0f);

  m_pSimpleTextOverlay->AddText(m_pTextEntity2);

  //"Hello World 3!" is there, too! Can you find it?
  m_pTextEntity3 = pDispManager->CreateTextEntity();

  m_pTextEntity3->SetText("Hello World 3!");
  m_pTextEntity3->SetFont("SERIF", 30);
  m_pTextEntity3->SetColor(VistaColor::GREEN);
  m_pTextEntity3->SetXPos(0.0f);
  m_pTextEntity3->SetYPos(1.0f);
  m_pTextEntity3->SetZPos(0.0f);

  m_pOverlay3D = new Vista3DTextOverlay(pDispManager);
  m_pOverlay3D->AddText(m_pTextEntity3);

  if (!m_pVistaSystem->GetDisplayManager()->GetDisplaySystem())
    VISTA_THROW("No DisplaySystem found", 1);

  m_pVistaSystem->GetDisplayManager()
      ->GetWindowByName("MAIN_WINDOW")
      ->GetWindowProperties()
      ->SetTitle(argv[0]);

  return true;
}

void TextDemoAppl::Run() {
  // Start Universe
  m_pVistaSystem->Run();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
