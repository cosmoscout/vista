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


#ifndef _EVENTDEMOAPPL_H
#define _EVENTDEMOAPPL_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaSystem.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class GreedyObserver;
class DemoHandler;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class EventDemoAppl
{
public:
	EventDemoAppl( int argc = 0, char* argv[] = NULL );
	virtual ~EventDemoAppl();

	void Run();
	void CreateScene();
	
private:
	VistaSystem					m_pVistaSystem;
	DemoHandler*				m_pEventHandler;
	VistaEventManager*			m_pEventManager;
	GreedyObserver*				m_pGreedyObserver;
};


#endif // _EVENTDEMOAPPL_H
