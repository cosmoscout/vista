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


#ifndef _VISTAHIDDRIVER_H__
#define _VISTAHIDDRIVER_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <string>
#include <vector>

//CRM
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#ifdef WIN32
#include <Windows.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Shared library support
#ifdef WIN32
	#define VISTAHIDDRIVEREXPORT __declspec(dllexport)
	#define VISTAHIDDRIVERIMPORT __declspec(dllimport)
#else
	#define VISTAHIDDRIVEREXPORT
	#define VISTAHIDDRIVERIMPORT
#endif

// Define VISTAHIDDRIVERAPI for DLL builds
#ifdef VISTAHIDDDRIVERDLL
	#ifdef VISTAHIDDDRIVERDLL_EXPORT
	#define VISTAHIDDDRIVERAPI VISTAHIDDDRIVEREXPORT
	#else
	#define VISTAHIDDDRIVERAPI VISTAHIDDDRIVERIMPORT
	#endif
#else
	#define VISTAHIDDDRIVERAPI VISTAHIDDRIVEREXPORT
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
#ifdef LINUX
struct input_event ;
#endif
#ifdef WIN32
class VistaDriverThreadAspect;
#endif

class VistaDriverConnectionAspect;
class VistaDriverSensorMappingAspect;
class VistaDeviceIdentificationAspect;
class VistaHIDDriverForceFeedbackAspect;
class VistaHIDDetachSequence;

struct _sHIDMeasure;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver working on top of the event subsystem of the Linux kernel.
 * or on top of the HID layer under windows.
 * @todo add documentation for this driver, PLEAZE
 */
class VISTAHIDDDRIVERAPI VistaHIDDriver : public IVistaDeviceDriver
{
public:
	VistaHIDDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaHIDDriver();


	static bool ScanAvailableHIDDevices(std::list<std::string> &liResult);

	

protected:
	bool DoConnect();
	bool DoDisconnect();

	bool DoSensorUpdate(VistaType::microtime dTs);
	virtual bool PhysicalEnable(bool bEnable);
private:
	bool InitDriver(int nVendor, int nDevId);

#ifdef LINUX
	VistaDriverConnectionAspect        *m_pConnection;
	VistaHIDDriverForceFeedbackAspect  *m_pForce;
	std::vector<input_event> m_vEvents ;
#endif
#ifdef WIN32
	bool StartWinRead();

	VistaDriverThreadAspect			*m_pThreadAspect;

	HANDLE m_hDevice;
	HANDLE m_hReadEvent;
	BYTE m_readBuffer[512];
	OVERLAPPED m_readOverlap;
#endif

	VistaDriverSensorMappingAspect     *m_pSensorMappingAsp;
	VistaDeviceIdentificationAspect    *m_pIdentAsp;

	VistaHIDCommonShare::sHIDMeasure *m_pMeasure;
};

class VISTAHIDDDRIVERAPI VistaHIDDriverCreationMethod : public IVistaDriverCreationMethod
{
public:

	VistaHIDDriverCreationMethod(IVistaTranscoderFactoryFactory *metaFac)
		: IVistaDriverCreationMethod(metaFac)
	{
		// register sensor type 3DCONNEXION_SPACENAVIGATOR
		RegisterSensorType("3DCONNEXION_SPACENAVIGATOR",
			sizeof(VistaHIDCommonShare::sHIDMeasure),
			100, metaFac->CreateFactoryForType("3DCONNEXION_SPACENAVIGATOR"));

		// register sensor type 3DCONNEXION_SPACENAVIGATOR
		RegisterSensorType("3DCONNEXION_SPACENAVIGATOR_CMD",
			sizeof(VistaHIDCommonShare::sHIDMeasure),
			100, metaFac->CreateFactoryForType("3DCONNEXION_SPACENAVIGATOR_CMD"));

		// register sensor type LOGITECH_RUMBLEPAD2
		RegisterSensorType("LOGITECH_RUMBLEPAD2",
			sizeof(VistaHIDCommonShare::sHIDMeasure),
			100, metaFac->CreateFactoryForType("LOGITECH_RUMBLEPAD2"));

		// register sensor type GRIFFIN_POWERMATE
		RegisterSensorType("GRIFFIN_POWERMATE",
			sizeof(VistaHIDCommonShare::sHIDMeasure),
			100, metaFac->CreateFactoryForType("GRIFFIN_POWERMATE"));

		// register sensor type HID_RAW
		RegisterSensorType("HID_RAW",
			sizeof(VistaHIDCommonShare::sHIDMeasure),
			100, metaFac->CreateFactoryForType("HID_RAW"));
	}

	virtual IVistaDeviceDriver *CreateDriver()
	{
		return new VistaHIDDriver(this);
	}
protected:
private:
};

/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMOUSEDRIVER_H
