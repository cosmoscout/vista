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


#include "VistaGlutKeyboardDriver.h"

#if defined(WIN32) && !defined(VISTAGLUTKEYBOARDPLUGIN_STATIC)
	#ifdef VISTAGLUTKEYBOARDPLUGIN_EXPORTS
		#define VISTAGLUTKEYBOARDPLUGINAPI __declspec(dllexport)
	#else
		#define VISTAGLUTKEYBOARDPLUGINAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTAGLUTKEYBOARDPLUGINAPI
#endif


namespace
{
	VistaGlutKeyboardDriverCreationMethod *SpFactory = NULL;
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

#endif //__VISTAVISTAGLUTKEYBOARDDRIVERCONFIG_H


extern "C" VISTAGLUTKEYBOARDPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if(SpFactory == NULL)
		SpFactory = new VistaGlutKeyboardDriverCreationMethod(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTAGLUTKEYBOARDPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( crm == SpFactory )
		if(IVistaReferenceCountable::refdown(SpFactory))
			SpFactory = NULL;
}

extern "C" VISTAGLUTKEYBOARDPLUGINAPI const char *GetDeviceClassName()
{
	return "GLUTKEYBOARD";
}
