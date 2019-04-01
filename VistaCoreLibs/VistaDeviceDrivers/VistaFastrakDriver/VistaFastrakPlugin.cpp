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


#include "VistaFastrakDriver.h"
#include "VistaFastrakCommonShare.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>


#include <cstring>
#include <cstdio>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTAFASTRAKDRIVERPLUGIN_STATIC)
#ifdef VISTAFASTRAKPLUGIN_EXPORTS
#define VISTAFASTRAKDRIVERPLUGINAPI __declspec(dllexport)
#else
#define VISTAFASTRAKDRIVERPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAFASTRAKDRIVERPLUGINAPI
#endif

// ############################################################################
// FASTRAK COMMANDSET VARIANTS INTERFACE
// ############################################################################

namespace
{
	FastrackCreationMethod *SpFactory = NULL;

	// SHADOWED FROM DRIVER API
	//IVistaDriverCreationMethod *VistaFastrakDriver::GetDriverFactoryMethod()
	//{
	//	if(SpFactory == NULL)
	//	{
	//		SpFactory = new FastrackCreationMethod;
	//		SpFactory->RegisterSensorType( "BODY", sizeof(_sFastrakButtonSample),
	//			120,
	//			new VistaFastrakBodyFactory,
	//			VistaFastrakBodyTranscoder::GetTypeString() );
	//		SpFactory->RegisterSensorType( "WAND",
	//			sizeof(_sFastrakSample),
	//			120,
	//			new VistaFastrakWandFactory,
	//			VistaFastrakWandTranscoder::GetTypeString() );
	//	}
	//	return SpFactory;
	//}
}

extern "C" VISTAFASTRAKDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaFastrakDriver(crm);
}

extern "C" VISTAFASTRAKDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SpFactory == NULL )
		SpFactory = new FastrackCreationMethod(fac);

	IVistaReferenceCountable::refup(SpFactory);
	return SpFactory;
}

extern "C" VISTAFASTRAKDRIVERPLUGINAPI void DisposeCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SpFactory == crm )
	{
		delete SpFactory;
		SpFactory = NULL;
	}
	else
		delete crm;
}

extern "C" VISTAFASTRAKDRIVERPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SpFactory != NULL )
	{
		if(IVistaReferenceCountable::refdown(SpFactory))
			SpFactory = NULL;
	}
}


extern "C" VISTAFASTRAKDRIVERPLUGINAPI const char *GetDeviceClassName()
{
	return "FASTRAK";
}

