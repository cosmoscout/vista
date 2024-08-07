/*============================================================================*/
/*                    ViSTA VR toolkit - OpenCV2 driver                  */
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

#include "VistaDepthSenseDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#if defined(WIN32) && !defined(VISTADEPTHSENSEPLUGIN_STATIC)
#ifdef VISTADEPTHSENSEPLUGIN_EXPORTS
#define VISTADEPTHSENSEPLUGINAPI __declspec(dllexport)
#else
#define VISTADEPTHSENSEPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTADEPTHSENSEPLUGINAPI
#endif

namespace {
VistaDepthSenseDriverCreationMethod* g_SpFactory = NULL;
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

#endif //__VISTAVISTASPACENAVIGATORCONFIG_H

extern "C" VISTADEPTHSENSEPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (g_SpFactory == NULL)
    g_SpFactory = new VistaDepthSenseDriverCreationMethod(fac);

  IVistaReferenceCountable::refup(g_SpFactory);
  return g_SpFactory;
}

extern "C" VISTADEPTHSENSEPLUGINAPI const char* GetDeviceClassName() {
  return "DEPTHSENSE";
}

extern "C" VISTADEPTHSENSEPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod* crm) {
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
