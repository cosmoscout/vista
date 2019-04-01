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


#include "ProcessCallback.h"
#include <cmath>
#include "DemoEvent.h"
#include <VistaTools/VistaMemoryInfo.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>

#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaCentralEventHandler.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
ProcessCallback::ProcessCallback(int iEventType, VistaEventManager* pEventManager)
: m_iEventType(iEventType)
, m_pEventManager(pEventManager)
{
}

ProcessCallback::~ProcessCallback()
{
	// do NOT delete the EventManager
	m_pEventManager = NULL;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool ProcessCallback::PrepareCallback()
{
	return true;
}

bool ProcessCallback::Do()
{	
	switch( m_iEventType )
	{
	case NULL_EVENT:
		m_pEventManager->ProcessEvent( NULL );
		break;
	case DEMO_EVENT:
		VistaEvent* pEvent = new DemoEvent();
		m_pEventManager->ProcessEvent( pEvent );
		delete pEvent;
		break;
	}
	
	return true;
}

bool ProcessCallback::PostCallback()
{
	return true;
}

