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

#if defined(WIN32) && !defined(VISTASPACEMOUSEDRIVERPLUGIN_STATIC)
#ifdef VISTASPACEMOUSEPLUGIN_EXPORTS
#define VISTASPACEMOUSEDRIVERPLUGINAPI __declspec(dllexport)
#else
#define VISTASPACEMOUSEDRIVERPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASPACEMOUSEDRIVERPLUGINAPI
#endif

#include "VistaSpaceMouseCommonShare.h"
#include "VistaSpaceMouseDriver.h"

namespace {
namespace {
VistaSpaceMouseCreationMethod* SpFactory = NULL;
}
} // namespace

extern "C" VISTASPACEMOUSEDRIVERPLUGINAPI IVistaDeviceDriver* CreateDevice(
    IVistaDriverCreationMethod* crm) {
  return new VistaSpaceMouseDriver(crm);
}

extern "C" VISTASPACEMOUSEDRIVERPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (SpFactory == NULL)
    SpFactory = new VistaSpaceMouseCreationMethod(fac);

  IVistaReferenceCountable::refup(SpFactory);
  return SpFactory;
}

extern "C" VISTASPACEMOUSEDRIVERPLUGINAPI void DisposeCreationMethod(
    IVistaDriverCreationMethod* crm) {
  if (SpFactory == crm) {
    delete SpFactory;
    SpFactory = NULL;
  } else
    delete crm;
}

extern "C" VISTASPACEMOUSEDRIVERPLUGINAPI void UnloadCreationMethod(
    IVistaDriverCreationMethod* crm) {
  if (SpFactory != NULL) {
    if (IVistaReferenceCountable::refdown(SpFactory))
      SpFactory = NULL;
  }
}

extern "C" VISTASPACEMOUSEDRIVERPLUGINAPI const char* GetDeviceClassName() {
  return "VISTASPACEMOUSE";
}
