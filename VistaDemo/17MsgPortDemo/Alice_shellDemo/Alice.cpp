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
// include header here
#include "Alice.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaMsgChannel.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/VistaSystem.h>
#include <VistaTools/VistaIniFileParser.h>

// we use a cout in here...
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
Alice::Alice(VistaKeyboardSystemControl* pCtrl, const std::string& sIniPath) {
  // Fetch hostname and portnumber frominifile
  // Please note that we introduced the following section specially for this demo!
  // [ALICE_DEMO]
  // MSGCHANNELIP = 127.0.0.1
  // MSGCHANNELPORT = 6666
  VistaPropertyList oIniFile = VistaIniFileParser::ReadProplistFromFile(sIniPath);
  std::string       sHostname =
      oIniFile.GetValueInSubListOrDefault<std::string>("MSGCHANNELIP", "ALICE_DEMO", "127.0.0.1");
  int nPortNumber = oIniFile.GetValueInSubListOrDefault<int>("MSGCHANNELPORT", "ALICE_DEMO", 6666);

  // Create MessageChannel via IP Connection.
  m_pChannel               = new VistaMsgChannel;
  VistaConnectionIP* pConn = new VistaConnectionIP(VistaConnectionIP::CT_TCP);
  pConn->SetHostNameAndPort(sHostname, nPortNumber);
  if (pConn->Open() == false) {
    std::cout << "Alice could not connect to " << sHostname << " on port " << nPortNumber
              << std::endl;
    VISTA_THROW("No connection", -1);
  } else {
    std::cout << "Alice connected to " << sHostname << " on port " << nPortNumber << std::endl;
    pConn->SetIsBlocking(true);
    m_pChannel->SetConnection(pConn);
  }

  // Create Keyboard assignments for movements. These will get transferred to Bob ;)
  pCtrl->BindAction(
      'w', new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_ZOOM_IN), "ZOOM IN");
  pCtrl->BindAction(
      's', new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_ZOOM_OUT), "ZOOM OUT");
  pCtrl->BindAction(
      'a', new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_TURN_LEFT), "TURN LEFT");
  pCtrl->BindAction(
      'd', new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_TURN_RIGHT), "TURN RIGHT");

  pCtrl->BindAction(VISTA_KEY_UPARROW,
      new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_MOVE_UP), "MOVE UP");
  pCtrl->BindAction(VISTA_KEY_DOWNARROW,
      new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_MOVE_DOWN), "MOVE DOWN");
  pCtrl->BindAction(VISTA_KEY_RIGHTARROW,
      new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_MOVE_RIGHT), "MOVE RIGHT");
  pCtrl->BindAction(VISTA_KEY_LEFTARROW,
      new AliceInitiateAction(m_pChannel, AliceInitiateAction::CMD_MOVE_LEFT), "MOVE LEFT");
};

Alice::~Alice() {
  m_pChannel->DisconnectL();
  delete m_pChannel;
};

AliceInitiateAction::AliceInitiateAction(
    VistaMsgChannel* pChannel, ALICE_COMMANDS eExecuteCommand) {
  m_pChannel        = pChannel;
  m_eExecuteCommand = eExecuteCommand;
}

// This gets executed each time Alice transmits a message to Bob. He will answer and Alice will
// print! More in detail: This gets executed each time we press a key ;)
bool AliceInitiateAction::Do() {
  // This is used to pass information to Bob and to receive Bob's answer
  VistaPropertyList oAnswer, oProps;

  // This sends the actual command: If everything was sent fine, MessageSuccessful == true :-)
  // Bob's CommandHandler will receive an Event on receive :-)
  bool MessageSuccessful = m_pChannel->WriteMsg(m_eExecuteCommand, oProps, oAnswer);

  // Bob must have sent us a reply. Let's see...
  if (MessageSuccessful) {
    std::cout << "Got a message from Bob: " << oAnswer.GetValue<std::string>("message")
              << std::endl;
  } else {
    std::cout << "Oops! Didn't get response from Bob!" << std::endl;
  }

  return 1;
}
