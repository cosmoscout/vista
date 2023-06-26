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
/*
 * $Id$
 */

#ifndef _ALICE_H
#define _ALICE_H

/*============================================================================*/
/* INCLUDES & DEFINES                                                         */
/*============================================================================*/

#if defined(WIN32)
#include <winsock2.h>
#endif

//#include <fx.h>
#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaInterProcComm/Connections/VistaMsgChannel.h>
#include <VistaKernel/VistaSystem.h>

#include <iostream>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

//
class AliceInitiateAction : public IVistaExplicitCallbackInterface {
 public:
  // This enum should also be available to Bob. At least he should have an equivalent one ;)
  //@TODO. CMD Resolution not implemented!
  enum ALICE_COMMANDS {
    CMD_RESOLUTION = 0,
    CMD_ZOOM_IN,
    CMD_ZOOM_OUT,
    CMD_TURN_LEFT,
    CMD_TURN_RIGHT,
    CMD_MOVE_LEFT,
    CMD_MOVE_RIGHT,
    CMD_MOVE_UP,
    CMD_MOVE_DOWN,
  };

  // This creates an object containing the move to do (as ALICE COMMANDS)
  AliceInitiateAction(VistaMsgChannel* pChannel, ALICE_COMMANDS eExecuteCommand);

  // This gets called once after the key got pressed and sends commands to Bob.
  bool Do();

 private:
  VistaMsgChannel* m_pChannel;
  ALICE_COMMANDS   m_eExecuteCommand;
};

// Actually Alice only opens the MessageChannel and creates keyboard assignments.
// Sending commands is done by AliceInitiateAction!
class Alice {
 public:
  Alice(VistaKeyboardSystemControl* pCtrl, const std::string& sIniPath);
  ~Alice();

 private:
  VistaMsgChannel* m_pChannel;
};

#endif //_ALICE_H

/*============================================================================*/
/* END OF FILE */
/*============================================================================*/
