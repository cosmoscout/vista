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
/*                                 DLR/utzi_se                                */
/*============================================================================*/


#ifndef __VISTAVIVEDRIVERDRIVER_H
#define __VISTAVIVEDRIVERDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaViveDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
// Shared library support

//Windows DLL build
#if defined(WIN32) && !defined(VISTAVIVE_STATIC) 
#ifdef VISTAVIVE_EXPORTS
#define VISTAVIVEDRIVERAPI __declspec(dllexport)
#else
#define VISTAVIVEDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAVIVEDRIVERAPI
#endif

/*
#ifdef WIN32
#define VISTAVIVEDRIVEREXPORT __declspec(dllexport)
#define VISTAVIVEDRIVERIMPORT __declspec(dllimport)
#define VISTAVIVEDRIVER_EXPLICIT_TEMPLATE_EXPORT
#define VISTAVIVEDRIVER_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAVIVEDRIVEREXPORT
#define VISTAVIVEDRIVERIMPORT
#endif

// Define VISTAVIVEDRIVERAPI for DLL builds
#ifdef VISTAVIVEDRIVERDLL
#ifdef VISTAVIVEDRIVERDLL_EXPORTS
#define VISTAVIVEDRIVERAPI VISTAVIVEDRIVEREXPORT
#define VISTAVIVEDRIVER_EXPLICIT_TEMPLATE
#else
#define VISTAVIVEDRIVERAPI VISTAVIVEDRIVERIMPORT
#define VISTAVIVEDRIVER_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAVIVEDRIVERAPI
#endif
*/
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

namespace vr{
	class IVRSystem;
}
class VistaDriverThreadAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver for transmitting Vive tracking data provided by openVR.
 *
 */
class VISTAVIVEDRIVERAPI VistaViveDriver : public IVistaDeviceDriver
{
public:
	VistaViveDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaViveDriver();

protected:
	virtual bool PhysicalEnable(bool bEnabled);
	virtual bool DoSensorUpdate(VistaType::microtime dTs);

	bool DoConnect();
	bool DoDisconnect();
private:
	vr::IVRSystem* m_pVRSystem;

	VistaDriverThreadAspect *m_pThread;

	bool UpdateStickSensor( VistaType::microtime dTs);
	bool UpdateHeadSensor( VistaType::microtime dTs);

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTAVIVEDRIVERDRIVER_H
