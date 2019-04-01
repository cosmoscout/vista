/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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

#include "VistaViveDriver.h"
#include "VistaViveDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VISTAVIVEDRIVERPLUGINEXPORT __declspec(dllexport)
#define VISTAVIVEDRIVERPLUGINIMPORT __declspec(dllimport)

#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAVIVEDRIVERPLUGINEXPORT
#define VISTAVIVEDRIVERPLUGINIMPORT
#endif

// Define VISTAVIVEDRIVERPLUGINAPI for DLL builds
#ifdef VISTAVIVEDRIVERPLUGINDLL
#ifdef VISTAVIVEDRIVERPLUGINDLL_EXPORTS
#define VISTAVIVEDRIVERPLUGINAPI VISTAVIVEDRIVERPLUGINEXPORT
#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTAVIVEDRIVERPLUGINAPI VISTAVIVEDRIVERPLUGINIMPORT
#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAVIVEDRIVERPLUGINAPI
#endif

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

#endif

/*
IVistaDriverCreationMethod *VistaViveDriver::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaViveDriverCreateMethod;

		// we assume an update rate of 20Hz at max. Devices we inspected reported
		// an update rate of about 17Hz, which seem reasonable.
		SpFactory->RegisterSensorType( "BODY", sizeof(dtrack_body_type),
			                           60,
									   new VistaViveBodyTranscodeFactory,
									   VistaViveBodyTranscode::GetTypeString() );
		SpFactory->RegisterSensorType( "MARKER", sizeof(dtrack_marker_type),
			                           60,
									   new VistaViveMarkerTranscodeFactory,
									   VistaViveMarkerTranscode::GetTypeString() );

	}

	return SpFactory;
} */

namespace
{
	class VistaViveDriverCreateMethod : public IVistaDriverCreationMethod
	{
	public:
		VistaViveDriverCreateMethod(IVistaTranscoderFactoryFactory *metaFac)
			: IVistaDriverCreationMethod(metaFac)
		{
			RegisterSensorType( "STICK", sizeof(VistaViveConfig::VISTA_vive_stick_type),
				60,
				metaFac->CreateFactoryForType("STICK") );
			RegisterSensorType( "HEAD", sizeof(VistaViveConfig::VISTA_vive_head_type),
				60,
				metaFac->CreateFactoryForType("HEAD") );
		}

		virtual IVistaDeviceDriver *CreateDriver()
		{
			return new VistaViveDriver(this);
		}
	};



	VistaViveDriverCreateMethod *SpFactory = NULL;
}

// /* definition to expand macro then apply to pragma message */
// #define VALUE_TO_STRING(x) #x
// #define VALUE(x) VALUE_TO_STRING(x)
// #define VAR_NAME_VALUE(var) #var "="  VALUE(var)

// /* Some example here */
// #pragma message(VAR_NAME_VALUE(VISTAVIVEDRIVERPLUGINAPI))

extern "C" VISTAVIVEDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaViveDriver(crm);
}

extern "C" VISTAVIVEDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SpFactory == NULL )
		SpFactory = new VistaViveDriverCreateMethod(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTAVIVEDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "VIVEDRIVER";
}

extern "C" VISTAVIVEDRIVERPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	//@TODO: What?
	if( SpFactory == NULL )
	{
		if(IVistaReferenceCountable::refdown(SpFactory))
			SpFactory = NULL;
	}
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





