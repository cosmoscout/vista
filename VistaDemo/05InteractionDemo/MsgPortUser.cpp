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

// STL stuff
#include <iostream>

// Vista stuff
#include <VistaKernel/EventManager/VistaCommandEvent.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/VistaSystem.h>

// Demo stuff
#include "MsgPortUser.h"

class CommandEventHandler : public VistaEventHandler {
  void HandleEvent(VistaEvent* pEvent) {
    // cast the event to a command event
    // those come i.e. from the messageport
    // so you see the benefit is that you do not
    // have NOT to mention about where the command
    // comes  from.
    VistaCommandEvent* pCmdEv = static_cast<VistaCommandEvent*>(pEvent);
    // in this simple case we only print out the token
    // use pCmdEv->GetProperty* to get the PropertyList
    // which comes with the command event
    std::cout << "TOKEN: " << pCmdEv->GetMethodToken() << std::endl;
    // now the command is handled
    // set this flag only if you want only
    // this eventhandler to handle the event
    pCmdEv->SetHandled(true);
  }
};

void MsgPortUser::EchoToCout(VistaSystem* pSystem) {
  std::cout << "[MsgPortUser] init echoToCout" << std::endl;
  // add the eventhandler which does the `real` job
  // in the vista.ini is port an hostname given
  // so you do NOT have to mention about the connection
  pSystem->GetEventManager()->AddEventHandler(
      new CommandEventHandler(), VistaCommandEvent::GetTypeId(), VistaCommandEvent::VEIDC_CMD);
}
