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

#include "VistaCyberGloveDriver.h"

#include <cstring>
#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTACYBERGLOVEPLUGIN_STATIC)
#ifdef VISTACYBERGLOVEPLUGIN_EXPORTS
#define VISTACYBERGLOVEPLUGINAPI __declspec(dllexport)
#else
#define VISTACYBERGLOVEPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTACYBERGLOVEPLUGINAPI
#endif

namespace
{
	CreateCyberGloveDriver *SsCreationMethod  = NULL;
}


extern "C" VISTACYBERGLOVEPLUGINAPI IVistaDeviceDriver *CreateDevice(IVistaDriverCreationMethod *crm)
{
	return new VistaCyberGloveDriver(crm);
}

extern "C" VISTACYBERGLOVEPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod(IVistaTranscoderFactoryFactory *fac)
{
	if( SsCreationMethod == NULL )
		SsCreationMethod = new CreateCyberGloveDriver(fac);

	IVistaReferenceCountable::refup(SsCreationMethod);
	return SsCreationMethod;
}

extern "C" VISTACYBERGLOVEPLUGINAPI void DisposeCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SsCreationMethod == crm )
	{
		delete SsCreationMethod;
		SsCreationMethod = NULL;
	}
	else
		delete crm;
}

extern "C" VISTACYBERGLOVEPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod *crm)
{
	if( SsCreationMethod != NULL )
	{
		if(IVistaReferenceCountable::refdown(SsCreationMethod))
			SsCreationMethod = NULL;
	}
}


extern "C" VISTACYBERGLOVEPLUGINAPI const char *GetDeviceClassName()
{
	return "VISTACYBERGLOVE";
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/



