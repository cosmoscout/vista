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


#include "OglDemoAppl.h"
#include "OglDrawObject.h"

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaBase/VistaExceptionBase.h>

#include <cassert>

using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
OglDemoAppl::OglDemoAppl( int argc, char * argv[] ) : m_vistaSystem( VistaSystem() )
{

	std::list<std::string> liSearchPaths;
	liSearchPaths.push_back( "../configfiles/" );
	m_vistaSystem.SetIniSearchPaths( liSearchPaths );
	
	m_vistaSystem.IntroMsg ();

	// init system (the above created handler is now known by the system)
	bool initOK = m_vistaSystem.Init ( argc, argv );
	
	if( initOK )
		CreateScene();

	assert(initOK && m_vistaSystem.GetKeyboardSystemControl());

	if(m_vistaSystem.GetDisplayManager()->GetDisplaySystem(0)== 0 )
		VISTA_THROW( "No DisplaySystem found" , 1);

	std::map<std::string,VistaWindow*> wmap=m_vistaSystem.GetDisplayManager()->GetWindows();
	if (wmap.empty())
		VISTA_THROW( "No Window found" , 1);

	(*wmap.begin()).second->GetWindowProperties()->SetTitle(argv[0]);
}

OglDemoAppl::~OglDemoAppl()
{

}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Run                                                         */
/*                                                                            */
/*============================================================================*/
void OglDemoAppl::Run ()
{
	// Start Universe
	m_vistaSystem.Run();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateScene                                                 */
/*                                                                            */
/*============================================================================*/
void OglDemoAppl::CreateScene ()
{
	VistaGraphicsManager* pGraphicsManager = m_vistaSystem.GetGraphicsManager();
	
	assert(pGraphicsManager && "You have to have a graphics manager here!");

	VistaSceneGraph* pSG = pGraphicsManager->GetSceneGraph();

	pSG->GetRoot()->SetName("ViSTA-ROOT");
	
	// an OGL draw object
	m_pOglDrawObj = new OglDrawObject;

	// ask SG to generate an OGL node
	m_pOglRootNode = pSG->NewOpenGLNode(pSG->GetRoot(), m_pOglDrawObj);
    m_pOglRootNode->SetName("OpenGL demo node");
}

