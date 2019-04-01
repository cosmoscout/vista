/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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


#ifndef __VISTAVIVEDRIVERPLUGINPLUGIN_H
#define __VISTAVIVEDRIVERPLUGINPLUGIN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning (disable: 4786)

#define VISTAVIVEDRIVERPLUGINEXPORT __declspec(dllexport)
#define VISTAVIVEDRIVERPLUGINIMPORT __declspec(dllimport)

#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAVIVEDRIVERPLUGINEXPORT
#define VISTAVIVEDRIVERPLUGINIMPORT
#endif

// Define VISTAVIVEDRIVERPLUGINAPI for DLL builds
#ifdef VISTAVIVEDRIVERPLUGINDLL
#ifdef VISTAVIVEDRIVERPLUGINDLL_EXPORTS
#define VISTAVIVEDRIVERPLUGINAPI VISTAVIVEDRIVERPLUGINEXPORT
#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTAVIVEDRIVERPLUGINAPI VISTAVIVEDRIVERPLUGINIMPORT
#define VISTAVIVEDRIVERPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAVIVEDRIVERPLUGINAPI
#endif



/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;


extern "C" VISTAVIVEDRIVERPLUGINAPI IVistaDeviceDriver *CreateDevice(unsigned int);
extern "C" VISTAVIVEDRIVERPLUGINAPI IVistaDriverCreationMethod *GetCreationMethod();
extern "C" VISTAVIVEDRIVERPLUGINAPI const char *GetDeviceClassName();
extern "C" VISTAVIVEDRIVERPLUGINAPI void Unload(IVistaDriverCreationMethod *);


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTAVIVEDRIVERPLUGINPLUGIN_H





