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
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/VistaSystem.h>

#include "KeyboardInteraction.h"

void KeyboardInteraction::RegisterSomeButtons(VistaSystem* pSystem) {
  std::cout << "[KeyboardInteraction] SomeButtons are enabled" << std::endl;

  // this class registers the key 'B' on an inner class inherited from
  // IVistaExplicitCallbackInterface in the callback is only a counter incremented and printed to
  // the commandline

  // first get the keyboardsystemcontrol object
  VistaKeyboardSystemControl* pCtrl = pSystem->GetKeyboardSystemControl();

  // second register callback on a button
  pCtrl->BindAction('B',        // the button NOTE: it is case sensitive
      new SomeButtonCallback(), // create a new instance of my callback
      "its a counter"           // descriptive text
  );
}

SomeButtonCallback::SomeButtonCallback()
    : iCount(0) {
}

bool SomeButtonCallback::Do() {
  std::cout << "[SomeButtonCallback] " << ++iCount << std::endl;
  return true;
}
