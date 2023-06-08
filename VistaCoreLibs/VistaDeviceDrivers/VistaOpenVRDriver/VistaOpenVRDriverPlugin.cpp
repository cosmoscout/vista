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

#include "VistaOpenVRDriver.h"
#include "VistaOpenVRDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VISTAOPENVRDRIVERPLUGINEXPORT __declspec(dllexport)
#define VISTAOPENVRDRIVERPLUGINIMPORT __declspec(dllimport)

#define VISTAOPENVRDRIVERPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTAOPENVRDRIVERPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAOPENVRDRIVERPLUGINEXPORT
#define VISTAOPENVRDRIVERPLUGINIMPORT
#endif

// Define VISTAOPENVRDRIVERPLUGINAPI for DLL builds
#ifdef VISTAOPENVRDRIVERPLUGINDLL
#ifdef VISTAOPENVRDRIVERPLUGINDLL_EXPORTS
#define VISTAOPENVRDRIVERPLUGINAPI VISTAOPENVRDRIVERPLUGINEXPORT
#define VISTAOPENVRDRIVERPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTAOPENVRDRIVERPLUGINAPI VISTAOPENVRDRIVERPLUGINIMPORT
#define VISTAOPENVRDRIVERPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAOPENVRDRIVERPLUGINAPI
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
IVistaDriverCreationMethod *VistaOpenVRDriver::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaOpenVRDriverCreateMethod;

		// we assume an update rate of 20Hz at max. Devices we inspected reported
		// an update rate of about 17Hz, which seem reasonable.
		SpFactory->RegisterSensorType( "BODY", sizeof(dtrack_body_type),
			                           60,
									   new VistaOpenVRBodyTranscodeFactory,
									   VistaOpenVRBodyTranscode::GetTypeString() );
		SpFactory->RegisterSensorType( "MARKER", sizeof(dtrack_marker_type),
			                           60,
									   new VistaOpenVRMarkerTranscodeFactory,
									   VistaOpenVRMarkerTranscode::GetTypeString() );

	}

	return SpFactory;
} */

namespace
{
	class VistaOpenVRDriverCreateMethod : public IVistaDriverCreationMethod
	{
	public:
		VistaOpenVRDriverCreateMethod(IVistaTranscoderFactoryFactory *metaFac)
			: IVistaDriverCreationMethod(metaFac)
		{
			RegisterSensorType( "STICK", sizeof(VistaOpenVRConfig::VISTA_openvr_stick_type),
				60,
				metaFac->CreateFactoryForType("STICK") );
			RegisterSensorType( "HEAD", sizeof(VistaOpenVRConfig::VISTA_openvr_head_type),
				60,
				metaFac->CreateFactoryForType("HEAD") );
		}

		virtual IVistaDeviceDriver *CreateDriver()
		{
			return new VistaOpenVRDriver(this);
		}
	};



	VistaOpenVRDriverCreateMethod *SpFactory = NULL;
}

// /* definition to expand macro then apply to pragma message */
// #define VALUE_TO_STRING(x) #x
// #define VALUE(x) VALUE_TO_STRING(x)
// #define VAR_NAME_VALUE(var) #var "="  VALUE(var)

// /* Some example here */
// #pragma message(VAR_NAME_VALUE(VISTAOPENVRDRIVERPLUGINAPI))

extern "C" VISTAOPENVRDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaOpenVRDriver(crm);
}

extern "C" VISTAOPENVRDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SpFactory == NULL )
		SpFactory = new VistaOpenVRDriverCreateMethod(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTAOPENVRDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "OPENVRDRIVER";
}

extern "C" VISTAOPENVRDRIVERPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
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





