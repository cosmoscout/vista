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


#include "VistaHapticDeviceEmulatorDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>

#include <VistaBase/VistaExceptionBase.h>

#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTAHAPTICDEVICEEMULATORDRIVERPLUGIN_STATIC)
#ifdef VISTAHAPTICDEVICEEMULATORPLUGIN_EXPORTS
#define VISTAHAPTICDEVICEEMULATORPLUGINAPI __declspec(dllexport)
#else
#define VISTAHAPTICDEVICEEMULATORPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAHAPTICDEVICEEMULATORPLUGINAPI
#endif

namespace
{
	VistaHapticDeviceEmulatorDriverFactory *SpFactory = NULL;
}

/*
IVistaDriverCreationMethod *VistaHapticDeviceEmulatorDriver::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
	{
		SpFactory = new VistaHapticDeviceEmulatorDriverFactory;
		SpFactory->RegisterSensorType( "",
			sizeof(_sPhantomMeasure),
			1000,
			new VistaHapticDeviceEmulatorTranscoderFactory,
			VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString());
	}
	return SpFactory;
}*/

extern "C" VISTAHAPTICDEVICEEMULATORPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaHapticDeviceEmulatorDriver(crm);
}

extern "C" VISTAHAPTICDEVICEEMULATORPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SpFactory == NULL )
		SpFactory = new VistaHapticDeviceEmulatorDriverFactory(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTAHAPTICDEVICEEMULATORPLUGINAPI const char *GetDeviceClassName()
{
	return "HAPTICDEVICEEMULATOR";
}