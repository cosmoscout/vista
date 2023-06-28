/*============================================================================*/
/*                    ViSTA VR toolkit - Jsw Joystick driver                  */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaJswJoystickDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

namespace {
VistaJswJoystickDriverCreationMethod* g_SpFactory = NULL;
}

extern "C" IVistaDriverCreationMethod* GetCreationMethod(IVistaTranscoderFactoryFactory* fac) {
  if (g_SpFactory == NULL)
    g_SpFactory = new VistaJswJoystickDriverCreationMethod(fac);

  IVistaReferenceCountable::refup(g_SpFactory);
  return g_SpFactory;
}

extern "C" const char* GetDeviceClassName() {
  return "JSWJOYSTICK";
}

extern "C" void UnloadCreationMethod(IVistaDriverCreationMethod* crm) {
  if (crm == g_SpFactory)
    if (IVistaReferenceCountable::refdown(g_SpFactory))
      g_SpFactory = NULL;
}
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
