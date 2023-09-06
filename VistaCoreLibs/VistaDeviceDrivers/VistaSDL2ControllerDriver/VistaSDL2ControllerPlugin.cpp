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

#if defined(WIN32) && !defined(VISTASDL2CONTROLLERPLUGIN_STATIC)
#ifdef VISTASDL2CONTROLLERPLUGIN_EXPORTS
#define VISTASDL2CONTROLLERPLUGINAPI __declspec(dllexport)
#else
#define VISTASDL2CONTROLLERPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2CONTROLLERPLUGINAPI
#endif

#include "VistaSDL2ControllerDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

namespace {
VistaSDL2ControllerCreationMethod* Factory = nullptr;
}

extern "C" VISTASDL2CONTROLLERPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (Factory == nullptr) {
    Factory = new VistaSDL2ControllerCreationMethod(fac);
  }
  return Factory;
}

extern "C" VISTASDL2CONTROLLERPLUGINAPI void DisposeCreationMethod(
    IVistaDriverCreationMethod* crm) {
  if (Factory == crm) {
    delete Factory;
    Factory = nullptr;
  } else
    delete crm;
}

extern "C" VISTASDL2CONTROLLERPLUGINAPI const char* GetDeviceClassName() {
  return "SDL2CONTROLLER";
}
