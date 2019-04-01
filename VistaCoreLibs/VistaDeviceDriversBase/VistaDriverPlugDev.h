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


#ifndef _VISTADRIVERPLUGDEV_H
#define _VISTADRIVERPLUGDEV_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"

#if !defined(WIN32)
#define DEFTRANSCODERPLUG_FUNC_EXPORTS( facsym ) \
		namespace \
		{ \
			facsym *SpFactory=NULL; \
		} \
		\
		extern "C" IVistaTranscoderFactoryFactory *CreateTranscoderFactoryFactory() \
		{ \
			if( !SpFactory ) \
				SpFactory = new #facsym; \
			\
			IVistaReferenceCountable::refup(SpFactory); \
			\
			return SpFactory; \
		} \
		\
		extern "C" bool OnUnloadTranscoderFactoryFactory( IVistaTranscoderFactoryFactory *fac ) \
		\
		{ \
			if(!SpFactory) \
				return false; \
		\
			if(IVistaReferenceCountable::refdown(SpFactory)) \
			{ \
				SpFactory = NULL; \
				return true; \
			} \
			return false; \
		}

	#define DEFTRANSCODERPLUG_FUNC_IMPORTS( facsym ) \
		namespace \
		{ \
			facsym *SpFactory = NULL; \
		} \
		\
		extern "C" IVistaTranscoderFactoryFactory *CreateTranscoderFactoryFactory() \
		{ \
			if( !SpFactory ) \
				SpFactory = new facsym; \
			\
			IVistaReferenceCountable::refup(SpFactory); \
			\
			return SpFactory; \
		} \
		\
		extern "C" bool OnUnloadTranscoderFactoryFactory( IVistaTranscoderFactoryFactory *fac ) \
		\
		{ \
			if(!SpFactory) \
				return false; \
		\
			if(IVistaReferenceCountable::refdown(SpFactory)) \
			{ \
				SpFactory = NULL; \
				return true; \
			} \
			return false; \
		}

#else
#define DEFTRANSCODERPLUG_FUNC_EXPORTS( facsym ) \
		namespace \
		{ \
			facsym *SpFactory = NULL; \
		} \
		\
		extern "C" __declspec(dllexport) IVistaTranscoderFactoryFactory *CreateTranscoderFactoryFactory() \
		{ \
			if( !SpFactory ) \
				SpFactory = new facsym; \
			\
			IVistaReferenceCountable::refup(SpFactory); \
			\
			return SpFactory; \
		} \
		\
		extern "C" __declspec(dllexport) bool OnUnloadTranscoderFactoryFactory( IVistaTranscoderFactoryFactory *fac ) \
		\
		{ \
			if(!SpFactory) \
				return false; \
		\
			if(IVistaReferenceCountable::refdown(SpFactory)) \
			{ \
				SpFactory = NULL; \
				return true; \
			} \
			return false; \
		}

	#define DEFTRANSCODERPLUG_FUNC_IMPORTS( facsym ) \
		namespace \
		{ \
			facsym *SpFactory = NULL; \
		} \
		\
		extern "C" __declspec(dllimport) IVistaTranscoderFactoryFactory *CreateTranscoderFactoryFactory() \
		{ \
			if( !SpFactory ) \
				SpFactory = new facsym; \
			\
			IVistaReferenceCountable::refup(SpFactory); \
			\
			return SpFactory; \
		} \
		\
		extern "C" __declspec(dllimport) bool OnUnloadTranscoderFactoryFactory( IVistaTranscoderFactoryFactory *fac ) \
		\
		{ \
			if(!SpFactory) \
				return false; \
		\
			if(IVistaReferenceCountable::refdown(SpFactory)) \
			{ \
				SpFactory = NULL; \
				return true; \
			} \
			return false; \
		}
#endif



#if defined(WIN32)
	#define DEFTRANSCODERPLUG_CLEANUP static void releaseTranscoders();

	#include <windows.h>
	#define IMPTRANSCODERPLUG_CLEANUP(facsym) \
		static void releaseTranscoders() { facsym::OnUnload(); } \
		\
		BOOL APIENTRY DllMain( HANDLE hModule, \
							   DWORD  ul_reason_for_call, \
							   LPVOID lpReserved \
							 ) \
		{ \
			switch (ul_reason_for_call) \
			{ \
			case DLL_PROCESS_ATTACH: \
			case DLL_THREAD_ATTACH: \
			case DLL_THREAD_DETACH: \
				break; \
			case DLL_PROCESS_DETACH: \
				if( lpReserved == 0 ) \
					releaseTranscoders(); \
				break; \
			} \
			return TRUE; \
		}

#else
	#define DEFTRANSCODERPLUG_CLEANUP static void releaseTranscoders() __attribute__ ((destructor))

	#define IMPTRANSCODERPLUG_CLEANUP(facsym) \
		static void releaseTranscoders() { facsym::OnUnload(); }
#endif




#endif // _VISTADRIVERPLUGDEV_H

