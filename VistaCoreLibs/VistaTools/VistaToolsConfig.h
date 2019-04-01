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


#ifndef _VISTATOOLSCONFIG_H
#define _VISTATOOLSCONFIG_H

// Windows DLL build
#if defined(WIN32) && !defined(VISTATOOLS_STATIC) 
	#ifdef VISTATOOLS_EXPORTS
		#define VISTATOOLSAPI __declspec(dllexport)
	#else
		#define VISTATOOLSAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTATOOLSAPI
#endif

#endif //_VISTATOOLSCONFIG_H
