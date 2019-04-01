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


#ifndef _VISTAIRMANDRIVERCONFIG_H_
#define _VISTAIRMANDRIVERCONFIG_H_

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
// Windows DLL build
#if defined(WIN32) && !defined(VISTAIRMANDRIVER_STATIC)
	#ifdef VISTAIRMANDRIVER_EXPORTS
		#define VISTAIRMANDRIVERAPI __declspec(dllexport)
	#else
		#define VISTAIRMANDRIVERAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTAIRMANDRIVERAPI
#endif
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

#endif //VISTAIRMANDRIVERCONFIG_H_
