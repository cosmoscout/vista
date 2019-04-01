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


#ifndef _VISTAOPTITRACKDRIVER_H
#define _VISTAOPTITRACKDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaOptitrackCommonShare.h"
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include "VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h"
#include "NatNetTypes.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//Windows DLL build
#if defined(WIN32) && !defined(VISTAOPTITRACKDRIVER_STATIC) 
	#ifdef VISTAOPTITRACKDRIVER_EXPORTS
		#define VISTAOPTITRACKDRIVERAPI __declspec(dllexport)
	#else
		#define VISTAOPTITRACKDRIVERAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTAOPTITRACKDRIVERAPI
#endif
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverSensorMappingAspect;
class NatNetClient;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * For the Optitrack Tracking System, using Optitrack's NatNetSDK
 */
class VISTAOPTITRACKDRIVERAPI VistaOptitrackDriver : public IVistaDeviceDriver
{
public:
	class VISTAOPTITRACKDRIVERAPI Parameters : public VistaDriverGenericParameterAspect::IParameterContainer
	{
		REFL_DECLARE
	public:
		Parameters( IVistaDeviceDriver* pDriver );

		bool m_bReportMessages;
		bool m_bServerUsesMulticast;
		std::string m_sMulticastAddress;
		std::string m_sOwnHostName;
		std::string m_sServerHostName;
		int m_nServerCommandPort;
		int m_nServerDataPort;
	};
public:
	VistaOptitrackDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaOptitrackDriver();

	void SetCurrentFrameData( sFrameOfMocapData* pValue );

protected:
	virtual bool DoSensorUpdate(VistaType::microtime dTs);
	virtual bool PhysicalEnable(bool bEnable);

	virtual bool DoConnect();
	virtual bool DoDisconnect();

	const Parameters* GetParameters() const;
	Parameters* GetParameters();

private:
	VistaDriverSensorMappingAspect* m_pSensors;
	VistaDriverGenericParameterAspect* m_pParamAspect;
	NatNetClient* m_pClient;
	sFrameOfMocapData* m_pCurrentFrameData;
};


//CREATION METHOD

class VISTAOPTITRACKDRIVERAPI VistaOptitrackCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaOptitrackCreationMethod( IVistaTranscoderFactoryFactory* PMetaFactory );
	virtual IVistaDeviceDriver* CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPTITRACKDRIVER_H
