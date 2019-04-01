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


#if defined(WIN32) && !defined(VISTAIRMANPLUGIN_STATIC)
	#ifdef VISTAIRMANPLUGIN_EXPORTS
		#define VISTAIRMANDRIVERPLUGINAPI __declspec(dllexport)
	#else
		#define VISTAIRMANDRIVERPLUGINAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTAIRMANDRIVERPLUGINAPI
#endif

#include "VistaIRManDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

namespace
{
	VistaIRManDriverCreationMethod *SpFactory = NULL;
}


#if defined(WIN32)

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif //__VISTAVISTAIRMANDRIVERCONFIG_H

extern "C" VISTAIRMANDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if(SpFactory == NULL)
		SpFactory = new VistaIRManDriverCreationMethod(fac);
	IVistaReferenceCountable::refup( SpFactory );
	return SpFactory;
}

extern "C" VISTAIRMANDRIVERPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( crm == SpFactory )
		if(IVistaReferenceCountable::refdown(SpFactory))
			SpFactory = NULL;
}

extern "C" VISTAIRMANDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "IRMAN";
}
