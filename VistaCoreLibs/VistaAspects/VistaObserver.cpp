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


#include "VistaObserver.h"
#include <algorithm>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


IVistaObserver::IVistaObserver(IVistaObserver &)
{}

IVistaObserver::IVistaObserver()
: m_bObserverEnabled(true)
{}

IVistaObserver::~IVistaObserver()
{
	// copy vector of observables, because the original (member) version
	// is altered during the ReleaseObserveable call
	std::vector<IVistaObserveable *> vecObs = m_vecObserveables;

	std::vector<IVistaObserveable *>::iterator it;
	for (it = vecObs.begin(); it != vecObs.end(); ++it)
	{
		ReleaseObserveable(*it, IVistaObserveable::TICKET_NONE);
	}

	m_vecObserveables.clear();

}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool IVistaObserver::GetIsObserverEnabled() const
{
	return m_bObserverEnabled;
}

void IVistaObserver::SetIsObserverEnabled(bool bEnabled)
{
	m_bObserverEnabled = bEnabled;
}

void IVistaObserver::UpdateRequest(IVistaObserveable *pObserveable, int msg, int nTicket)
{
	if(m_bObserverEnabled)
		ObserverUpdate(pObserveable, msg, nTicket);
}

bool IVistaObserver::ObserveableDeleteRequest(
	IVistaObserveable* pObserveable, 
	int nTicket /*= IVistaObserveable::TICKET_NONE */
	)
{
	return true;
}

void IVistaObserver::ObserveableDelete(
	IVistaObserveable* pObserveable, 
	int nTicket /*= IVistaObserveable::TICKET_NONE */
	)
{
	// just delete pObserveable from list

	std::vector<IVistaObserveable*>::iterator it =
		std::find(m_vecObserveables.begin(), m_vecObserveables.end(), pObserveable);

	if (it != m_vecObserveables.end())
	{
		m_vecObserveables.erase(it);
	}
}

void IVistaObserver::ReleaseObserveable(
	IVistaObserveable* pObserveable, 
	int nTicket /*= IVistaObserveable::TICKET_NONE */
	)
{
	if (Observes(pObserveable))
	{
		pObserveable->DetachObserver(this);

		std::vector<IVistaObserveable*>::iterator it =
			std::find(m_vecObserveables.begin(), m_vecObserveables.end(), pObserveable);

		if (it != m_vecObserveables.end())
		{
			m_vecObserveables.erase(it);
		}
	}
}

bool IVistaObserver::Observes(IVistaObserveable* pObserveable)
{
	std::vector<IVistaObserveable *>::iterator it =
		std::find(m_vecObserveables.begin(), m_vecObserveables.end(), pObserveable);

	return (it != m_vecObserveables.end());
}

void IVistaObserver::Observe(
	IVistaObserveable* pObserveable, 
	int nTicket /*= IVistaObserveable::TICKET_NONE */
	)
{
	if (!Observes(pObserveable))
	{
		if (pObserveable->AttachObserver(this, nTicket))
			m_vecObserveables.push_back(pObserveable);
	}
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

