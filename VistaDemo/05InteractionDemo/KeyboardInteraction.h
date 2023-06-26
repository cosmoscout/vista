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

#ifndef _KEYBOARDINTERACTION_H
#define _KEYBOARDINTERACTION_H

#include <VistaAspects/VistaExplicitCallbackInterface.h> //For SomeButtonCallback

class VistaSystem;
class IVistaExplicitCallbackInterface;

class SomeButtonCallback : public IVistaExplicitCallbackInterface {
 public:
  SomeButtonCallback();

  // override this method
  bool Do();

 private:
  int iCount;
};

class KeyboardInteraction {
 public:
  static void RegisterSomeButtons(VistaSystem* pSystem);
};

#endif /* _KEYBOARDINTERACTION_H */
