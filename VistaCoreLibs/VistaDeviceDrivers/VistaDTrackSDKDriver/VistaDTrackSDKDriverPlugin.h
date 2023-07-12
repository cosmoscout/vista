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

#ifndef __VISTADTRACKSDKDRIVERPLUGINPLUGIN_H
#define __VISTADTRACKSDKDRIVERPLUGINPLUGIN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning(disable : 4786)

#define VISTADTRACKSDKDRIVERPLUGINEXPORT __declspec(dllexport)
#define VISTADTRACKSDKDRIVERPLUGINIMPORT __declspec(dllimport)

#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE_EXPORT
#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTADTRACKSDKDRIVERPLUGINEXPORT
#define VISTADTRACKSDKDRIVERPLUGINIMPORT
#endif

// Define VISTADTRACKSDKDRIVERPLUGINAPI for DLL builds
#ifdef VISTADTRACKSDKDRIVERPLUGINDLL
#ifdef VISTADTRACKSDKDRIVERPLUGINDLL_EXPORTS
#define VISTADTRACKSDKDRIVERPLUGINAPI VISTADTRACKSDKDRIVERPLUGINEXPORT
#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE
#else
#define VISTADTRACKSDKDRIVERPLUGINAPI VISTADTRACKSDKDRIVERPLUGINIMPORT
#define VISTADTRACKSDKDRIVERPLUGIN_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTADTRACKSDKDRIVERPLUGINAPI
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;

extern "C" VISTADTRACKSDKDRIVERPLUGINAPI IVistaDeviceDriver* CreateDevice(unsigned int);
extern "C" VISTADTRACKSDKDRIVERPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod();
extern "C" VISTADTRACKSDKDRIVERPLUGINAPI const char*                 GetDeviceClassName();
extern "C" VISTADTRACKSDKDRIVERPLUGINAPI void Unload(IVistaDriverCreationMethod*);

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTADTRACKSDKDRIVERPLUGINPLUGIN_H
