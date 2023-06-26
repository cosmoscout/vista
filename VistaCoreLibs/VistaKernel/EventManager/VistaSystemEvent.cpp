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

#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

int VistaSystemEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaSystemEvent::VistaSystemEvent() {
  SetType(VistaSystemEvent::GetTypeId());
}

VistaSystemEvent::VistaSystemEvent(const EVENT_ID eEventID) {
  SetType(VistaSystemEvent::GetTypeId());
  SetId(eEventID);
}

VistaSystemEvent::~VistaSystemEvent() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetId                                                       */
/*                                                                            */
/*============================================================================*/
bool VistaSystemEvent::SetId(int iId) {
  if (iId > VSE_INVALID && iId < VSE_UPPER_BOUND) {
    VistaEvent::SetId(iId);
    return true;
  }
  VistaEvent::SetId(VSE_INVALID);
  return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetName                                                     */
/*                                                                            */
/*============================================================================*/
std::string VistaSystemEvent::GetName() const {
  return VistaEvent::GetName() + "::VistaSystemEvent";
}

std::string VistaSystemEvent::GetIdString(int nId) {
  switch (nId) {
  case VSE_INVALID:
    return "VSE_INVALID";
  case VSE_INIT:
    return "VSE_INIT";
  case VSE_QUIT:
    return "VSE_QUIT";
  case VSE_EXIT:
    return "VSE_EXIT";
  case VSE_PREGRAPHICS:
    return "VSE_PREGRAPHICS";
  case VSE_POSTGRAPHICS:
    return "VSE_POSTGRAPHICS";
  case VSE_UPDATE_INTERACTION:
    return "VSE_UPDATE_INTERACTION";
  case VSE_UPDATE_DELAYED_INTERACTION:
    return "VSE_UPDATE_DELAYED_INTERACTION";
  case VSE_UPDATE_DISPLAYS:
    return "VSE_UPDATE_DISPLAYS";
  case VSE_PREAPPLICATIONLOOP:
    return "VSE_PREAPPLICATIONLOOP";
  case VSE_POSTAPPLICATIONLOOP:
    return "VSE_POSTAPPLICATIONLOOP";
  default:
    return VistaEvent::GetIdString(nId);
  }
}

int VistaSystemEvent::GetTypeId() {
  return VistaSystemEvent::m_nEventId;
}

void VistaSystemEvent::SetTypeId(int nId) {
  if (VistaSystemEvent::m_nEventId == VistaEvent::VET_INVALID)
    VistaSystemEvent::m_nEventId = nId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaSystemEvent::Debug(std::ostream& out) const {
  VistaEvent::Debug(out);
  out << " [ViSyEv]   EventId: ";
  out << GetIdString(GetId());
  out << std::endl;
}
