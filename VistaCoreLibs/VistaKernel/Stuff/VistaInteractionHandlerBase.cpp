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


#include "VistaInteractionHandlerBase.h" 

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaInteractionHandlerBase::IVistaInteractionHandlerBase(VistaEventManager *pEvMgr,
														   unsigned int nRoleId,
							 bool bNeedsTime)
							 : VistaEventHandler(),
							   m_pEvMgr(pEvMgr),
							   m_bNeedsTime(bNeedsTime),
							   m_dTs(0.0),
							   m_nRoleId(nRoleId)
{
	m_pEvMgr->AddEventHandler(this, VistaInteractionEvent::GetTypeId(), VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE);
	m_pEvMgr->AddEventHandler(this, VistaInteractionEvent::GetTypeId(), VistaInteractionEvent::VEID_CONTEXT_CHANGE);

	if(bNeedsTime)
	{
		m_pEvMgr->AddEventHandler(this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_POSTAPPLICATIONLOOP);
	}
}

IVistaInteractionHandlerBase::~IVistaInteractionHandlerBase()
{
	m_pEvMgr->RemEventHandler(this, VistaInteractionEvent::GetTypeId(), VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE);
	m_pEvMgr->RemEventHandler(this, VistaInteractionEvent::GetTypeId(), VistaInteractionEvent::VEID_CONTEXT_CHANGE);

	if(m_bNeedsTime)
		m_pEvMgr->RemEventHandler(this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_POSTAPPLICATIONLOOP);
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void IVistaInteractionHandlerBase::HandleEvent(VistaEvent *pEvent)
{
	if(pEvent->GetType() == VistaSystemEvent::GetTypeId())
	{
		if(m_dTs == 0.0)
		{
			m_dTs = pEvent->GetTime();
		}
		else
		{
			double dTs = pEvent->GetTime();
			if(dTs - m_dTs > 0.0)
				HandleTimeUpdate( dTs, m_dTs );
			m_dTs = dTs;
		}

	}
	else if(pEvent->GetType() == VistaInteractionEvent::GetTypeId())
	{
		VistaInteractionEvent *pInEv = static_cast<VistaInteractionEvent*>(pEvent);
		if(pInEv->GetRoleId() == m_nRoleId)
		{
			switch(pInEv->GetId())
			{
			case VistaInteractionEvent::VEID_CONTEXT_CHANGE:
				{
					HandleContextChange(pInEv);
					break;
				}
			case VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE:
				{
					HandleGraphUpdate(pInEv);
					break;
				}
			default:
				break; // should not happen
			}
		}
	}
}


bool IVistaInteractionHandlerBase::HandleTimeUpdate( double dTs, double dLastTs )
{
	return true; // default action
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


