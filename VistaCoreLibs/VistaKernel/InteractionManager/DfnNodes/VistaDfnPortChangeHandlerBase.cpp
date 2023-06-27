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

#include "VistaDfnPortChangeHandlerBase.h"

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnPortChangeHandlerBase::VistaDfnPortChangeHandlerBase(VistaEventManager* pEvMgr)
    : VistaEventHandler()
    , m_pEvMgr(pEvMgr) {
  m_pEvMgr->AddEventHandler(
      this, VistaInteractionEvent::GetTypeId(), VistaInteractionEvent::VEID_GRAPH_INPORT_CHANGE);
}

VistaDfnPortChangeHandlerBase::~VistaDfnPortChangeHandlerBase() {
  m_pEvMgr->RemEventHandler(
      this, VistaInteractionEvent::GetTypeId(), VistaInteractionEvent::VEID_GRAPH_INPORT_CHANGE);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaDfnPortChangeHandlerBase::HandleEvent(VistaEvent* pEvent) {
  VistaInteractionEvent* pInEv = dynamic_cast<VistaInteractionEvent*>(pEvent);
  pEvent->SetHandled(HandlePortChange(pInEv, pInEv->GetPortMapRead()));
}

bool VistaDfnPortChangeHandlerBase::HandlePortChange(
    VistaInteractionEvent* pInEv, const VistaInteractionEvent::PortList& liPorts) {
  bool bRet = false; // do not swallow by default
  for (VistaInteractionEvent::PortList::const_iterator cit = liPorts.begin(); cit != liPorts.end();
       ++cit) {
    bRet = bRet || HandlePortChange(pInEv, *cit);
  }
  return bRet;
}

bool VistaDfnPortChangeHandlerBase::HandlePortChange(
    VistaInteractionEvent* pInEv, const std::string& strPort) {
  return false; // do not swallow by default
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
