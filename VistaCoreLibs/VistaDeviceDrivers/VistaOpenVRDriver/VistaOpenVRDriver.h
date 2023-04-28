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


#ifndef __VISTAOPENVRDRIVERDRIVER_H
#define __VISTAOPENVRDRIVERDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaOpenVRDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
// Shared library support

//Windows DLL build
#if defined(WIN32) && !defined(VISTAOPENVR_STATIC) 
#ifdef VISTAOPENVR_EXPORTS
#define VISTAOPENVRDRIVERAPI __declspec(dllexport)
#else
#define VISTAOPENVRDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAOPENVRDRIVERAPI
#endif

/*
#ifdef WIN32
#define VISTAOPENVRDRIVEREXPORT __declspec(dllexport)
#define VISTAOPENVRDRIVERIMPORT __declspec(dllimport)
#define VISTAOPENVRDRIVER_EXPLICIT_TEMPLATE_EXPORT
#define VISTAOPENVRDRIVER_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAOPENVRDRIVEREXPORT
#define VISTAOPENVRDRIVERIMPORT
#endif

// Define VISTAOPENVRDRIVERAPI for DLL builds
#ifdef VISTAOPENVRDRIVERDLL
#ifdef VISTAOPENVRDRIVERDLL_EXPORTS
#define VISTAOPENVRDRIVERAPI VISTAOPENVRDRIVEREXPORT
#define VISTAOPENVRDRIVER_EXPLICIT_TEMPLATE
#else
#define VISTAOPENVRDRIVERAPI VISTAOPENVRDRIVERIMPORT
#define VISTAOPENVRDRIVER_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAOPENVRDRIVERAPI
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
 * this is a driver for transmitting OpenVR tracking data provided by openVR.
 *
 */
class VISTAOPENVRDRIVERAPI VistaOpenVRDriver : public IVistaDeviceDriver
{
public:
	VistaOpenVRDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaOpenVRDriver();

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
#endif //__VISTAOPENVRDRIVERDRIVER_H
