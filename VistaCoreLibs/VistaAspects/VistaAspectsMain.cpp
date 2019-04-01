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
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
#include "VistaReflectionable.h"
#include "VistaPropertyFunctorRegistry.h"
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
#if defined(WIN32)

#include <windows.h>

static void ReleaseProperties();
static void ClaimRegistry();


BOOL APIENTRY DllMain( HANDLE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
	{
		if( lpReserved == 0 )
			ClaimRegistry();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if( lpReserved == 0 )
			ReleaseProperties();
		break;
	}
	return TRUE;
}

#else
	static void ReleaseProperties() __attribute__ ((destructor));
	static void ClaimRegistry() __attribute__ ((constructor));
#endif

static void ClaimRegistry()
{
	IVistaReferenceCountable::refup(VistaPropertyFunctorRegistry::GetSingleton());
}

static void ReleaseProperties()
{
	VistaReflectionableUtil::ReleaseProperties();

#if defined(ASPECTS_FUNCTORREGISTRY_DEBUG)
	VistaPropertyFunctorRegistry *reg = VistaPropertyFunctorRegistry::GetSingleton();
	if( reg && reg->getcount() > 1 )
	{
		VistaAspectsOut out;
		reg->Show( out );
	}
#endif

	IVistaReferenceCountable::refdown(VistaPropertyFunctorRegistry::GetSingleton());
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

