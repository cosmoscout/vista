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

#include "VistaCHAI3DHapticDevicesCommonShare.h"
#include "VistaCHAI3DHapticDevicesDriver.h"

#if defined(WIN32) && !defined(VISTACHAI3DHAPTICDEVICESDRIVERPLUGIN_STATIC)
#ifdef VISTACHAI3DHAPTICDEVICESPLUGIN_EXPORTS
#define VISTACHAI3DHAPTICDEVICESPLUGINAPI __declspec(dllexport)
#else
#define VISTACHAI3DHAPTICDEVICESPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTACHAI3DHAPTICDEVICESPLUGINAPI
#endif

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

#endif //__VISTACHAI3DHapticDevicesDRIVERCONFIG_H

namespace {
VistaCHAI3DHapticDevicesDriverFactory* SpFactory = NULL;

/*
IVistaDriverCreationMethod *VistaCHAI3DHapticDevicesDriver::GetDriverFactoryMethod()
{
if(SpFactory == NULL)
{
        SpFactory = new VistaCHAI3DHapticDevicesDriverFactory;
        SpFactory->RegisterSensorType( "",
sizeof(VistaCHAI3DHapticDevicesDriverHelper::_sCHAI3DHapticDevicesMeasure), 1000, new
VistaCHAI3DHapticDevicesTranscoderFactory,
                VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString() );
}
return SpFactory;
}
*/
} // namespace

extern "C" VISTACHAI3DHAPTICDEVICESPLUGINAPI IVistaDeviceDriver* CreateDevice(
    IVistaDriverCreationMethod* crm) {
  return new VistaCHAI3DHapticDevicesDriver(crm);
}

extern "C" VISTACHAI3DHAPTICDEVICESPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (SpFactory == NULL)
    SpFactory = new VistaCHAI3DHapticDevicesDriverFactory(fac);

  IVistaReferenceCountable::refup(SpFactory);
  return SpFactory;
}

extern "C" VISTACHAI3DHAPTICDEVICESPLUGINAPI const char* GetDeviceClassName() {
  return "CHAI3DHapticDevices";
}

extern "C" VISTACHAI3DHAPTICDEVICESPLUGINAPI void UnloadCreationMethod(
    IVistaDriverCreationMethod* crm) {
  if (SpFactory == crm) {
    if (IVistaReferenceCountable::refdown(SpFactory))
      SpFactory = NULL;
  }
}

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/