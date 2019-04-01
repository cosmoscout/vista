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


#ifndef _COBSERVERREGISTERCALLBACK_H
#define _COBSERVERREGISTERCALLBACK_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaExplicitCallbackInterface.h>


class VistaEventManager;
class VistaEventObserver;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class ObserverRegisterCallback : public IVistaExplicitCallbackInterface
{
public:

	static const int NULL_OBSERVER				= 0;
	static const int DEMO_OBSERVER				= 1;
	static const int TIME_OBSERVER				= 2;
	ObserverRegisterCallback(VistaEventManager* pEventManager, int iObserverType,
		int iEventType);
	~ObserverRegisterCallback();

	// Interface
	bool PrepareCallback();
	bool Do();
	bool PostCallback();


private:
	int					m_iEventType;
	int					m_iObserverType;
	VistaEventManager*	m_pEventManager;

	// some observers
	VistaEventObserver*	m_pObserver;
};

#endif
