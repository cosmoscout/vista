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

#include "VistaPhantomCommonShare.h"
#include "VistaPhantomDriver.h"

#if defined(WIN32) && !defined(VISTAPHANTOMDRIVERPLUGIN_STATIC)
#ifdef VISTAPHANTOMPLUGIN_EXPORTS
#define VISTAPHANTOMPLUGINAPI __declspec(dllexport)
#else
#define VISTAPHANTOMPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAPHANTOMPLUGINAPI
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

#endif //__VISTAPHANTOMDRIVERCONFIG_H

namespace {

VistaPhantomDriverFactory* SpFactory = NULL;

/*
IVistaDriverCreationMethod *VistaPhantomDriver::GetDriverFactoryMethod()
{
if(SpFactory == NULL)
{
        SpFactory = new VistaPhantomDriverFactory;
        SpFactory->RegisterSensorType( "", sizeof(VistaPhantomDriverHelper::_sPhantomMeasure), 1000,
                new VistaPhantomTranscoderFactory,
                VistaPhantomDriverMeasureTranscode::GetTypeString() );
}
return SpFactory;
}
*/
} // namespace

extern "C" VISTAPHANTOMPLUGINAPI IVistaDeviceDriver* CreateDevice(IVistaDriverCreationMethod* crm) {
  return new VistaPhantomDriver(crm);
}

extern "C" VISTAPHANTOMPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (SpFactory == NULL)
    SpFactory = new VistaPhantomDriverFactory(fac);

  IVistaReferenceCountable::refup(SpFactory);
  return SpFactory;
}

extern "C" VISTAPHANTOMPLUGINAPI const char* GetDeviceClassName() {
  return "PHANTOM";
}

extern "C" VISTAPHANTOMPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod* crm) {
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
