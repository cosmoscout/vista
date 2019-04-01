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


#ifndef __VISTADTRACKSDKDRIVERDRIVER_H
#define __VISTADTRACKSDKDRIVERDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDTrackSDKDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
// Shared library support

//Windows DLL build
#if defined(WIN32) && !defined(VISTADTRACKSDK_STATIC) 
#ifdef VISTADTRACKSDK_EXPORTS
#define VISTADTRACKSDKDRIVERAPI __declspec(dllexport)
#else
#define VISTADTRACKSDKDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTADTRACKSDKDRIVERAPI
#endif

/*
#ifdef WIN32
#define VISTADTRACKSDKDRIVEREXPORT __declspec(dllexport)
#define VISTADTRACKSDKDRIVERIMPORT __declspec(dllimport)
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE_EXPORT
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTADTRACKSDKDRIVEREXPORT
#define VISTADTRACKSDKDRIVERIMPORT
#endif

// Define VISTADTRACKSDKDRIVERAPI for DLL builds
#ifdef VISTADTRACKSDKDRIVERDLL
#ifdef VISTADTRACKSDKDRIVERDLL_EXPORTS
#define VISTADTRACKSDKDRIVERAPI VISTADTRACKSDKDRIVEREXPORT
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE
#else
#define VISTADTRACKSDKDRIVERAPI VISTADTRACKSDKDRIVERIMPORT
#define VISTADTRACKSDKDRIVER_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTADTRACKSDKDRIVERAPI
#endif
*/
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class DTrack;
class VistaDriverThreadAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver for the simple SpaceNav device vom 3DC. It uses the HID
 * event layer on Linux and the 3DC specific driver shipped with windows.
 * On windows, the driver raises a dependency to the installed DCOM drivers,
 * even on compile time.
 * There is currently only support for a two-button device having one puck
 * and no device backwards communication.
 *
 */
class VISTADTRACKSDKDRIVERAPI VistaDTrackSDKDriver : public IVistaDeviceDriver
{
public:
	VistaDTrackSDKDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaDTrackSDKDriver();





protected:
	virtual bool PhysicalEnable(bool bEnabled);
	virtual bool DoSensorUpdate(VistaType::microtime dTs);

	bool DoConnect();
	bool DoDisconnect();
private:
	DTrack *m_pDriver;

	VistaDriverThreadAspect *m_pThread;

	bool UpdateBodySensor( VistaType::microtime dTs, int nIdx = 0 );
	bool UpdateMarker( VistaType::microtime dTs, int nIdx = 0 );
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTADTRACKSDKDRIVERDRIVER_H
