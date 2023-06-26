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
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTAWIIMOTEDRIVERPLUGIN_STATIC)
#ifdef VISTAWIIMOTEPLUGIN_EXPORTS
#define VISTAWIIMOTEPLUGINAPI __declspec(dllexport)
#else
#define VISTAWIIMOTEPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAWIIMOTEPLUGINAPI
#endif

#include "VistaWiimoteDriver.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

namespace {
VistaWiimoteDriverCreationMethod* g_SpFactory = NULL;
}

extern "C" VISTAWIIMOTEPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (g_SpFactory == NULL)
    g_SpFactory = new VistaWiimoteDriverCreationMethod(fac);

  IVistaReferenceCountable::refup(g_SpFactory);
  return g_SpFactory;
}

extern "C" VISTAWIIMOTEPLUGINAPI const char* GetDeviceClassName() {
  return "WIIMOTE";
}

extern "C" VISTAWIIMOTEPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod* crm) {
  if (g_SpFactory == NULL) {
    if (IVistaReferenceCountable::refdown(g_SpFactory))
      g_SpFactory = NULL;
  }
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
