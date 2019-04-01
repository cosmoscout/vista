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


#ifndef _VISTAASPECTSCONFIG_H
#define _VISTAASPECTSCONFIG_H

// Windows DLL build
#if (defined WIN32) && !(defined VISTAASPECTS_STATIC)  
	#ifdef VISTAASPECTS_EXPORTS
		#define VISTAASPECTSAPI __declspec(dllexport)
	#else
		#define VISTAASPECTSAPI __declspec(dllimport)
		//#ifndef VISTA_NO_AUTOLINKING
		//	// auto-linking for msvc8
		//	#if _MSC_VER == 1400
		//		#if defined(_DEBUG) || defined(DEBUG)
		//			#pragma comment(lib, "VistaAspectsD.vc8.lib")
		//		#else
		//			#pragma comment(lib, "VistaAspects.vc8.lib")
		//		#endif
		//	#endif
		//	// auto-linking for msvc9
		//	#if _MSC_VER == 1500
		//		#if defined(_DEBUG) || defined(DEBUG)
		//			#pragma comment(lib, "VistaAspectsD.vc9.lib")
		//		#else
		//			#pragma comment(lib, "VistaAspects.vc9.lib")
		//		#endif
		//	#endif
		//#endif
	#endif
#else // no Windows or static build
	#define VISTAASPECTSAPI
#endif

#endif //_VISTAASPECTSCONFIG_H
