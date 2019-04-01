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

#include "VistaDTrackSDKDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include <DTrack.hpp>

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VISTADTRACKSDKDRIVERPLUGINEXPORT __declspec(dllexport)
#define VISTADTRACKSDKDRIVERPLUGINIMPORT __declspec(dllimport)

#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTADTRACKSDKDRIVERPLUGINEXPORT
#define VISTADTRACKSDKDRIVERPLUGINIMPORT
#endif

// Define VISTADTRACKSDKDRIVERPLUGINAPI for DLL builds
#ifdef VISTADTRACKSDKDRIVERPLUGINDLL
#ifdef VISTADTRACKSDKDRIVERPLUGINDLL_EXPORTS
#define VISTADTRACKSDKDRIVERPLUGINAPI VISTADTRACKSDKDRIVERPLUGINEXPORT
#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTADTRACKSDKDRIVERPLUGINAPI VISTADTRACKSDKDRIVERPLUGINIMPORT
#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTADTRACKSDKDRIVERPLUGINAPI
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
IVistaDriverCreationMethod *VistaDTrackSDKDriver::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaDTrackSDKDriverCreateMethod;

		// we assume an update rate of 20Hz at max. Devices we inspected reported
		// an update rate of about 17Hz, which seem reasonable.
		SpFactory->RegisterSensorType( "BODY", sizeof(dtrack_body_type),
			                           60,
									   new VistaDTrackSDKBodyTranscodeFactory,
									   VistaDTrackSDKBodyTranscode::GetTypeString() );
		SpFactory->RegisterSensorType( "MARKER", sizeof(dtrack_marker_type),
			                           60,
									   new VistaDTrackSDKMarkerTranscodeFactory,
									   VistaDTrackSDKMarkerTranscode::GetTypeString() );

	}

	return SpFactory;
} */

namespace
{
	class VistaDTrackSDKDriverCreateMethod : public IVistaDriverCreationMethod
	{
	public:
		VistaDTrackSDKDriverCreateMethod(IVistaTranscoderFactoryFactory *metaFac)
			: IVistaDriverCreationMethod(metaFac)
		{
			RegisterSensorType( "BODY", sizeof(dtrack_body_type),
				60,
				metaFac->CreateFactoryForType("BODY") );
			RegisterSensorType( "MARKER", sizeof(dtrack_marker_type),
				60,
				metaFac->CreateFactoryForType("MARKER") );
		}

		virtual IVistaDeviceDriver *CreateDriver()
		{
			return new VistaDTrackSDKDriver(this);
		}
	};



	VistaDTrackSDKDriverCreateMethod *SpFactory = NULL;
}

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaDTrackSDKDriver(crm);
}

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SpFactory == NULL )
		SpFactory = new VistaDTrackSDKDriverCreateMethod(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "DTRACKSDKDRIVER";
}

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
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





