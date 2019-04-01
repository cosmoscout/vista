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


/* The Windows-specific code (especially the InitDriver routine) has been 
 * copied from the VRPN toolkit, which is released in the public domain.
 * Those program parts stay in the public domain, eventually to be replaced by
 * an own implementation.
 * All other parts are Copyright of RWTH Aachen university and covered under the
 * terms of the GNU Lesser General Public License, as stated above.
 */

#include "VistaHIDCommonShare.h"
#include "VistaHIDDriver.h"

#include <iostream>
#include <iomanip>
#include <string>

#include <cstdio>

#if defined(WIN32) && !defined(VISTAHIDPLUGIN_STATIC)
#ifdef VISTAHIDPLUGIN_EXPORTS
#define VISTAHIDDRIVERPLUGINAPI __declspec(dllexport)
#else
#define VISTAHIDDRIVERPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAHIDDRIVERPLUGINAPI
#endif

namespace
{
	VistaHIDDriverCreationMethod *g_SpFactory = NULL;

/*
	IVistaDriverCreationMethod *VistaHIDDriver::GetDriverFactoryMethod()
	{
		if( g_SpFactory == NULL )
		{
			g_SpFactory = new VistaHIDDriverCreationMethod;

			// register sensor type 3DCONNEXION_SPACENAVIGATOR
			g_SpFactory->RegisterSensorType("3DCONNEXION_SPACENAVIGATOR",
				sizeof(_sHIDMeasure),
				100, new VistaHIDDriverSpaceNavigatorFactory,
				VistaHIDDriverSpaceNavigatorCmdTranscode::GetTypeString());

			// register sensor type 3DCONNEXION_SPACENAVIGATOR
			g_SpFactory->RegisterSensorType("3DCONNEXION_SPACENAVIGATOR_CMD",
				sizeof(_sHIDMeasure),
				100, new VistaHIDDriverSpaceNavigatorCmdFactory,
				VistaHIDDriverSpaceNavigatorCmdTranscode::GetTypeString());

			// register sensor type LOGITECH_RUMBLEPAD2
			g_SpFactory->RegisterSensorType("LOGITECH_RUMBLEPAD2",
				sizeof(_sHIDMeasure),
				100, new VistaHIDDriverLogitechPadFactory,
				VistaHIDDriverLogitechPadTranscode::GetTypeString());

			// register sensor type GRIFFIN_POWERMATE
			g_SpFactory->RegisterSensorType("GRIFFIN_POWERMATE",
				sizeof(_sHIDMeasure),
				100, new VistaHIDDriverPowerMateFactory,
				VistaHIDDriverPowerMateTranscode::GetTypeString());

			// register sensor type HID_RAW
			g_SpFactory->RegisterSensorType("HID_RAW",
				sizeof(_sHIDMeasure),
				100, new VistaHIDDriverRawTranscodeFactory,
				VistaHIDDriverRawTranscode::GetTypeString());
		}
		return g_SpFactory;
	}
	*/
}

extern "C" VISTAHIDDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaHIDDriver(crm);
}

extern "C" VISTAHIDDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( g_SpFactory == NULL )
		g_SpFactory = new VistaHIDDriverCreationMethod(fac);

	IVistaReferenceCountable::refup(g_SpFactory);
	return g_SpFactory;
}

extern "C" VISTAHIDDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "HIDDRIVER";
}


extern "C" VISTAHIDDRIVERPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( g_SpFactory != NULL )
	{
		if(IVistaReferenceCountable::refdown(g_SpFactory))
			g_SpFactory = NULL;
	}
}
