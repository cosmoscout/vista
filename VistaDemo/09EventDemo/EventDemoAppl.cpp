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


#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaCentralEventHandler.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaBase/VistaExceptionBase.h>

#include "EventDemoAppl.h"
#include "ProcessCallback.h"
#include "ObserverRegisterCallback.h"
#include "DemoEvent.h"
#include "GreedyObserver.h"
#include "DemoHandler.h"

using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
EventDemoAppl::EventDemoAppl( int argc, char * argv[] )
	:	m_pEventManager( NULL ),
		m_pGreedyObserver( NULL ),
		m_pEventHandler( NULL )
{

	// Initialize VistaSystem

	std::list<std::string> liSearchPath;
	liSearchPath.push_back( "../configfiles/" );

	m_pVistaSystem.SetIniSearchPaths( liSearchPath );
	m_pVistaSystem.IntroMsg ();
	m_pVistaSystem.Init (argc, argv);
	CreateScene();

	// Get the EventManager from the system
	m_pEventManager = m_pVistaSystem.GetEventManager();

	// register a new event type with a unique string (for debugging)
	// the important information is the return value, which is the
	// key to items of type "DEMOEVENT"
	int iDemoEventType = m_pEventManager->RegisterEventType( "DEMOEVENT" );
	int iDemoEventId = m_pEventManager->RegisterEventId( iDemoEventType, "DEMOEVENT");

	// this key is dynamically stored as a static member of CDemoEvent
	// *HAS* to be done *before* creating an instance of CDemoEvent
	DemoEvent::SetTypeId( iDemoEventType );

	// Register some keys to fire events
	VistaKeyboardSystemControl *pCtrl = m_pVistaSystem.GetKeyboardSystemControl();

	pCtrl->BindAction(' ', new ProcessCallback( ProcessCallback::NULL_EVENT, m_pEventManager ),
		"Send a NULL pointer to the event manager." );

	pCtrl->BindAction('g', new ProcessCallback( ProcessCallback::GRAPHICS_EVENT, m_pEventManager ),
		"create, send, and destroy an (invalid) graphics event..." );

	pCtrl->BindAction('d', new ProcessCallback( ProcessCallback::DEMO_EVENT, m_pEventManager ),
		"sending a demo event..." );

	m_pEventHandler = new DemoHandler();
	m_pEventManager->AddEventHandler( m_pEventHandler, iDemoEventType, iDemoEventId );

	pCtrl->BindAction('t', new ObserverRegisterCallback(
		m_pEventManager, ObserverRegisterCallback::TIME_OBSERVER,
		VistaSystemEvent::GetTypeId() ),
		"toggle time observer" );

	// Register the GreedyObserver which is always active
	// NOTE: this observer fetches all events but do NOT print
	// out a line for each event because this will be too much
	m_pGreedyObserver = new GreedyObserver();
	m_pEventManager->RegisterObserver(m_pGreedyObserver, VistaEventManager::NVET_ALL);

	if (m_pVistaSystem.GetDisplayManager()->GetDisplaySystem(0)==0)
		VISTA_THROW("No DisplaySystem found",1);

	m_pVistaSystem.GetDisplayManager()->GetWindowByName("MAIN_WINDOW")->GetWindowProperties()->SetTitle(argv[0]);
}

EventDemoAppl::~EventDemoAppl()
{
    delete m_pEventHandler;
    m_pEventHandler= NULL;

    delete m_pGreedyObserver;
    m_pGreedyObserver = NULL;

    // do NOT delete the eventManager this will be done by the system
    m_pEventManager = NULL;

}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Run                                                         */
/*                                                                            */
/*============================================================================*/
void EventDemoAppl::Run ()
{
    // Start Universe
    m_pVistaSystem.Run();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateScene                                                 */
/*                                                                            */
/*============================================================================*/
void EventDemoAppl::CreateScene ()
{
	VistaGraphicsManager* pGrMng = m_pVistaSystem.GetGraphicsManager();
	if (pGrMng != NULL)
    {
		// creating something to be displayed...
        VistaSceneGraph* pSG = pGrMng->GetSceneGraph();
		pSG->GetRoot()->SetName( "ViSTA-root-node" );

		VistaGeometryFactory oGeometryFactory(pSG);
		VistaGeometry *pGeomSphere = oGeometryFactory.CreateSphere();
		pSG->NewGeomNode( pSG->GetRoot() ,pGeomSphere);
		
		std::cout << "sphere done" << std::endl;

		pGeomSphere->SetColor(VistaColor(1.0f, 0.0f, 0.0f));
    }
    else
    {
	    // =======================
	    // legacy graphics manager
	    // =======================
		cout << "WARNING! Unable to find new graphics manager -> drawing nothing..." << endl;
    }
    
}
