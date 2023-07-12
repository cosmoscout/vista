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

#include "VistaMIDIDriver.h"

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <cstring>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTAMIDIDRIVERPLUGIN_STATIC)
#ifdef VISTAMIDIPLUGIN_EXPORTS
#define VISTAMIDIPLUGINAPI __declspec(dllexport)
#else
#define VISTAMIDIPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAMIDIPLUGINAPI
#endif

namespace {
VistaMIDIDriverCreationMethod* SpFactory = NULL;
}

extern "C" VISTAMIDIPLUGINAPI IVistaDeviceDriver* CreateDevice(IVistaDriverCreationMethod* crm) {
  return new VistaMIDIDriver(crm);
}

extern "C" VISTAMIDIPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (SpFactory == NULL)
    SpFactory = new VistaMIDIDriverCreationMethod(fac);

  IVistaReferenceCountable::refup(SpFactory);
  return SpFactory;
}

extern "C" VISTAMIDIPLUGINAPI void DisposeCreationMethod(IVistaDriverCreationMethod* crm) {
  if (SpFactory == crm) {
    delete SpFactory;
    SpFactory = NULL;
  } else
    delete crm;
}

extern "C" VISTAMIDIPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod* crm) {
  if (SpFactory != NULL) {
    if (IVistaReferenceCountable::refdown(SpFactory))
      SpFactory = NULL;
  }
}

extern "C" VISTAMIDIPLUGINAPI const char* GetDeviceClassName() {
  return "MIDI";
}
