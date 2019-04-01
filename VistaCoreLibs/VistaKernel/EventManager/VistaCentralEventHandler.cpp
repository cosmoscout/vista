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


#include <VistaKernel/EventManager/VistaCentralEventHandler.h>

// avoid include file dependencies - we don't like excessive compile times, right?! (ms)
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaCommandEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
#ifdef DEBUG
//#define EXCESSIVE_DEBUG		// makes the system pretty talkative...
#endif

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaCentralEventHandler::VistaCentralEventHandler(VistaEventManager *pMgr)
: m_pEvMgr(pMgr)
{
	m_pEvMgr->AddEventHandler(this, VistaEventManager::NVET_ALL, 
								VistaEventManager::NVET_ALL, 
								VistaEventManager::PRIO_MID);
}

VistaCentralEventHandler::~VistaCentralEventHandler()
{
	m_pEvMgr->RemEventHandler(this, VistaEventManager::NVET_ALL,
							  VistaEventManager::NVET_ALL);
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaCentralEventHandler::HandleEvent(VistaEvent *pEvent)
{
	GeneralEventHandler(pEvent);

	if (pEvent->IsHandled())
		return;

	int nTypeID = pEvent->GetType();
	if(nTypeID == VistaSystemEvent::GetTypeId())
		DispatchSystemEvent(static_cast<VistaSystemEvent*>(pEvent));
	else if(nTypeID == VistaCommandEvent::GetTypeId())
		DispatchCommandEvent(static_cast<VistaCommandEvent*>(pEvent));
	else
		ExternalEventHandler(pEvent);
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GeneralEventHandler                                         */
/*                                                                            */
/*============================================================================*/
void VistaCentralEventHandler::GeneralEventHandler(VistaEvent *pEvent)
{
}

//============================================================================
// SYSTEM Event Handling, i.e. dispatcher and user-defineable(??) callback
//
// Call Order:
// 1. SystemEventHandler
// 2. Special Handlers (init, quit, exit, predraw, postdraw)
//============================================================================
void VistaCentralEventHandler::DispatchSystemEvent(VistaSystemEvent *pEvent)
{
	if (pEvent->GetId() == VistaSystemEvent::VSE_INVALID)
		return;

	// call user-defined general system event handler
	SystemEventHandler(pEvent);

	if (pEvent->IsHandled())
		return;

	// if the event is not handled yet, call the specialized handlers
	switch (pEvent->GetId())
	{
	case VistaSystemEvent::VSE_INIT:
		InitVistaEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_QUIT:
		QuitVistaEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_EXIT:
		ExitVistaEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_PREGRAPHICS:
		PreDrawGraphicsEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_POSTGRAPHICS:
		PostDrawGraphicsEvent(pEvent);
		break;
	case VistaSystemEvent::VSE_POSTAPPLICATIONLOOP:
		PostAppEventHandler(pEvent);
		break;
	case VistaSystemEvent::VSE_PREAPPLICATIONLOOP:
		PreAppEventHandler(pEvent);
		break;
	default:
		break;
	}
}

void VistaCentralEventHandler::SystemEventHandler(VistaSystemEvent *pEvent)
{
}

void VistaCentralEventHandler::CommandEventHandler(VistaCommandEvent *pEvent)
{
}

void VistaCentralEventHandler::DispatchCommandEvent(VistaCommandEvent *pEvent)
{
	if (pEvent->GetId() >= VistaCommandEvent::VEIDC_LAST)
		return;

	CommandEventHandler(pEvent);
}

void VistaCentralEventHandler::InitVistaEvent(VistaSystemEvent *pEvent)
{
	vstr::warnp() << "[VistaCentralEventHandler] Application didn't define a system event handler for initialization!" << std::endl;
}

void VistaCentralEventHandler::QuitVistaEvent(VistaSystemEvent *pEvent)
{
}

void VistaCentralEventHandler::ExitVistaEvent(VistaSystemEvent *pEvent)
{
}

void VistaCentralEventHandler::PreDrawGraphicsEvent(VistaSystemEvent *pEvent)
{
}

void VistaCentralEventHandler::PostDrawGraphicsEvent(VistaSystemEvent *pEvent)
{
}

void VistaCentralEventHandler::PreAppEventHandler(VistaSystemEvent *pEvent)
{
}

void VistaCentralEventHandler::PostAppEventHandler(VistaSystemEvent *pEvent)
{
}

//============================================================================
// INPUT Event Handling, i.e. dispatcher and user-defineable(??) callback
//
// Order of Calls:
// 1. InputEventHandler
// 2. Special Event Handler(s) (Keyboard)
//============================================================================
//void VistaCentralEventHandler::DispatchInputEvent(VistaInputEvent *pEvent)
//{
//	// make sure, we have a valid event
//	if (pEvent->GetId() == VistaInputEvent::VIE_INVALID)
//		return;
//
//	// call user-defined general input event handler
//	InputEventHandler(pEvent);
//
//	if (pEvent->IsHandled())
//		return;
//
//	switch(pEvent->GetInputDevice()->WhoAmI())
//	{
//	case VID_KEYBOARD:
//
//		if ( pEvent->GetId() == VistaInputEvent::VIE_BUTTON_PRESSED &&
//			 !ProcessKeyPress(((VistaKeyboard*)(pEvent->GetInputDevice()))->GetCurrentKey() ) )
//			return;
//		break;
//	default:
//		return;
//	}
//	pEvent->SetHandled(true);
//}

//void VistaCentralEventHandler::InputEventHandler (VistaInputEvent *pEvent)
//{
////#ifdef DEBUG
//#ifdef EXCESSIVE_DEBUG
//	cout << "ViCeEvHa::InputEventHandler - received event:" << std::endl << *pEvent;
//#endif
//}

bool VistaCentralEventHandler::ProcessKeyPress(int keyCode)
{
	return false;
}

//============================================================================
// EXTERNAL Event Handling, i.e. user-defineable(??) callback
//============================================================================
void VistaCentralEventHandler::ExternalEventHandler(VistaEvent *pEvent)
{
}

