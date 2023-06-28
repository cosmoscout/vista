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

/*============================================================================*/
/* INCLUDES & DEFINES                                                         */
/*============================================================================*/
#include "CommandHandler.h"
#include "Bob.h"

#include <VistaKernel/EventManager/VistaCommandEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/VistaSystem.h>

/*============================================================================*/
/* Function definitions                                                       */
/*============================================================================*/
CommandHandler::CommandHandler(VistaEventManager* pEvMgr, Bob* pBob)
    : m_pEvMgr(pEvMgr)
    , m_pBob(pBob) {
  pEvMgr->AddEventHandler(this, VistaCommandEvent::GetTypeId(), VistaCommandEvent::VEIDC_CMD);

  std::cout << "CommandHandler running..." << std::endl;
}

CommandHandler::~CommandHandler() {
}

// Here we go. If this is called, the EventHandler has triggered because Alice sent an
// event/command.
void CommandHandler::HandleEvent(VistaEvent* pEvent) {
  VistaCommandEvent* pCmdEv = static_cast<VistaCommandEvent*>(pEvent);

  // One PropList for the Message, one for the Answer
  VistaPropertyList oProps, oAnswer;

  // Get the message
  pCmdEv->GetPropertiesByList(oProps);

  // Switch over the MethodToken.

  // Depending on what Message has been sent, we execute different stuff.
  // This calls Bobs OnCmdAction with different agruments. Theoretically we could
  // merge Bob and this CommandHandler. But that would make it a bit difficult.
  // GetMethodToken fetches the sent command-ID
  switch (pCmdEv->GetMethodToken()) {
    // change resolution
  case Bob::CMD_RESOLUTION:
    std::cout << "Received a ChangeResolutionEvent! Adjusting properties as \
			 follows:"
              << std::endl;
    oProps.Print();

    // set up a nonsensical message to Alice.
    oAnswer.SetValue<std::string>("message", "Hello Alice!");

    // try to change the resolution - we have a function for this
    // return false if the value is lower then 3
    if (!m_pBob->OnCmdAction(Bob::CMD_RESOLUTION, (float)(oProps.GetValue<int>("resolution")))) {
      oAnswer.SetValue<int>("resolution", 3);
    }

    // send the message to Alice. Just to see how it's done...
    pCmdEv->SetPropertiesByList(oAnswer);

    // mark the Event as handled, so it will not be passed to other
    // EventHandlers (if any) and there won't be a warning.
    pEvent->SetHandled(true);
    break;

    // zoom
  case Bob::CMD_ZOOM_IN:
    std::cout << "Received a ZoomIn Event!" << std::endl;
    // again we have a function for this. negative is zoomIn...
    m_pBob->OnCmdAction(Bob::CMD_ZOOM_IN, -1.0f);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, ZoomIn Event executed!");
    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;
  case Bob::CMD_ZOOM_OUT:
    std::cout << "Received a ZoomOut Event!" << std::endl;
    // positive is zoomOut
    m_pBob->OnCmdAction(Bob::CMD_ZOOM_OUT, 1.0f);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, ZoomOut Event executed!");

    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;

  case Bob::CMD_TURN_LEFT:
    std::cout << "Received a TurnLeft Event!" << std::endl;
    // negative is zoomOut
    m_pBob->OnCmdAction(Bob::CMD_TURN_LEFT);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, TurnLeft Event executed!");
    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;

  case Bob::CMD_TURN_RIGHT:
    std::cout << "Received a TurnRight Event!" << std::endl;
    // negative is zoomOut
    m_pBob->OnCmdAction(Bob::CMD_TURN_RIGHT);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, TurnRight Event executed!");
    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;
  case Bob::CMD_MOVE_LEFT:
    std::cout << "Received a MoveLeft Event!" << std::endl;
    // negative is zoomOut
    m_pBob->OnCmdAction(Bob::CMD_MOVE_LEFT);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, MoveLeft Event executed!");
    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;
  case Bob::CMD_MOVE_RIGHT:
    std::cout << "Received a MoveRight Event!" << std::endl;
    // negative is zoomOut
    m_pBob->OnCmdAction(Bob::CMD_MOVE_RIGHT);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, MoveRight Event executed!");
    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;
  case Bob::CMD_MOVE_UP:
    std::cout << "Received a MoveUp Event!" << std::endl;
    // negative is zoomOut
    m_pBob->OnCmdAction(Bob::CMD_MOVE_UP);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, MoveUp Event executed!");
    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;
  case Bob::CMD_MOVE_DOWN:
    std::cout << "Received a MoveDown Event!" << std::endl;
    // negative is zoomOut
    m_pBob->OnCmdAction(Bob::CMD_MOVE_DOWN);

    // send a nonsensical message to Alice. Just to see how it's done...
    oAnswer.SetValue<std::string>("message", "Hello Alice, MoveDown Event executed!");
    pCmdEv->SetPropertiesByList(oAnswer);

    pEvent->SetHandled(true);
    break;

    // and if there is something we can't handle...
  default:
    std::cout << "Received an unknown Event: " << pCmdEv->GetMethodToken() << std::endl;

    // if there are other handlers, they will still get the event.
    pEvent->SetHandled(false);
    break;
  }

  std::cout << "Message for Alice: " << oAnswer.GetValue<std::string>("message") << std::endl;
}
