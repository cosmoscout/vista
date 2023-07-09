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

#include "VistaSDL2MouseDriver.h"

#if defined(WIN32) && !defined(VISTASDL2MOUSEPLUGIN_STATIC)
#ifdef VISTASDL2MOUSEPLUGIN_EXPORTS
#define VISTASDL2MOUSEPLUGINAPI __declspec(dllexport)
#else
#define VISTASDL2MOUSEPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2MOUSEPLUGINAPI
#endif

namespace {
VistaSDL2MouseDriverCreationMethod* SpFactory = nullptr;
}

#if defined(WIN32)

#include <windows.h>

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

#endif

extern "C" VISTASDL2MOUSEPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (SpFactory == NULL)
    SpFactory = new VistaSDL2MouseDriverCreationMethod(fac);

  IVistaReferenceCountable::refup(SpFactory);
  return SpFactory;
}

extern "C" VISTASDL2MOUSEPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod* crm) {
  if (crm == SpFactory)
    if (IVistaReferenceCountable::refdown(SpFactory))
      SpFactory = nullptr;
}

extern "C" VISTASDL2MOUSEPLUGINAPI const char* GetDeviceClassName() {
  return "SDL2MOUSE";
}
