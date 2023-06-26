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

#ifndef __COMMANDHANDLER_H
#define __COMMANDHANDLER_H

/*============================================================================*/
/* INCLUDES & DEFINES                                                         */
/*============================================================================*/
#include <VistaKernel/EventManager/VistaEventHandler.h>

/*============================================================================*/
/* FORWARD DECLARATION                                                        */
/*============================================================================*/
class Bob;

/*============================================================================*/
/* CLASS DEFINITION                                                           */
/*============================================================================*/
class CommandHandler : public VistaEventHandler {
 public:
  // Constructor, Destructor. boring.
  CommandHandler(VistaEventManager* pEvMgr, Bob* pBob);
  virtual ~CommandHandler();

  // HandleEvent is called by the VistaEventManager.
  // If the Event is triggered by Alice, we react on her command here!
  virtual void HandleEvent(VistaEvent* pEvent);

 protected:
 private:
  VistaEventManager* m_pEvMgr;
  Bob*               m_pBob;
};

#endif //__COMMANDHANDLER_H
