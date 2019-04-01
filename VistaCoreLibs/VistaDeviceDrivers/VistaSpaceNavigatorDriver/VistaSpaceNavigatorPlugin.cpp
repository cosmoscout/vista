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

#if defined(WIN32) && !defined(VISTASPACENAVIGATORDRIVERPLUGIN_STATIC)
#ifdef VISTASPACENAVIGATORPLUGIN_EXPORTS
#define VISTASPACENAVIGATORDRIVERPLUGINAPI __declspec(dllexport)
#else
#define VISTASPACENAVIGATORDRIVERPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASPACENAVIGATORDRIVERPLUGINAPI
#endif

#include "VistaSpaceNavigatorDriver.h"


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

#endif //__VISTAVISTASPACENAVIGATORCONFIG_H

namespace
{
	/**
	 * creates a VistaSpaceMouse driver
	 */
	VistaSpaceNavigatorCreationMethod *SpFactory = NULL;

}

//IVistaDriverCreationMethod *VistaSpaceNavigator::GetDriverFactoryMethod()
//{
//	if(SpFactory == NULL)
//	{
//		SpFactory = new VistaSpaceNavigatorCreationMethod;
//
//		// TODO: check update rate of actual device!
//		// TODO: VistaTranscode is a stoopid name
//		SpFactory->RegisterSensorType( "", sizeof(VistaSpaceNavigator::_sMeasure),
//			20,
//			new VistaSpaceNavigatorTranscodeFactory,
//			VistaTranscode::GetTypeString() );
//
//	}
//
//	return SpFactory;
//}

extern "C" VISTASPACENAVIGATORDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaSpaceNavigator(crm);
}

extern "C" VISTASPACENAVIGATORDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SpFactory == NULL )
		SpFactory = new VistaSpaceNavigatorCreationMethod(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTASPACENAVIGATORDRIVERPLUGINAPI void DisposeCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SpFactory == crm )
	{
		delete SpFactory;
		SpFactory = NULL;
	}
	else
		delete crm;
}


extern "C" VISTASPACENAVIGATORDRIVERPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SpFactory != NULL )
	{
		if(IVistaReferenceCountable::refdown(SpFactory))
			SpFactory = NULL;
	}
}


extern "C" VISTASPACENAVIGATORDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "VISTASPACENAVIGATOR";
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





