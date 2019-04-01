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


#include "GreedyObserver.h"
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <iostream>
#include <fstream>

using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

GreedyObserver::GreedyObserver()
{
	cout << "[GreedyObserver] I might listen to all events" << endl;
	cout << "                 and log them to disk ;-)" << endl;

// the log can get very big
#ifdef INSANE
	m_pOuputFileStream = new ofstream( "events.log" );
#else
	m_pOuputFileStream = new ofstream( "/dev/null" );
#endif

	*m_pOuputFileStream << "event log file:" << endl;
}

GreedyObserver::~GreedyObserver()
{
	m_pOuputFileStream->close();

	delete m_pOuputFileStream;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Notify                                                      */
/*                                                                            */
/*============================================================================*/
void GreedyObserver::Notify(const VistaEvent *pEvent)
{
	if (pEvent->GetType() == VistaInteractionEvent::GetTypeId())
	{
		//std::cout << "Got an InteractionEvent!" << std::endl;
		*m_pOuputFileStream << *pEvent;
		*m_pOuputFileStream << "----" << endl;
	}
}

