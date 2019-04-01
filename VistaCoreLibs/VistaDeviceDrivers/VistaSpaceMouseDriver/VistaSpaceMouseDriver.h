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


#ifndef _VISTASPACEMOUSEDRIVER_H
#define _VISTASPACEMOUSEDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaSpaceMouseCommonShare.h"

#include "VistaDeviceDriversBase/VistaDeviceDriver.h"
#include <VistaBase/VistaVectorMath.h>

#include <string>

//CRM
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Shared library support
// Windows DLL build
#if defined(WIN32) && !defined(VISTASPACEMOUSE_STATIC) 
#ifdef VISTASPACEMOUSEDRIVER_EXPORTS
#define VISTASPACEMOUSEDRIVERAPI __declspec(dllexport)
#else
#define VISTASPACEMOUSEDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASPACEMOUSEDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverConnectionAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver for serial line connection style space mice. Use it when
 * you have such an old device. The device driver expects a serial line connection,
 * as it has to twiddle a bit with timeouts due to serial transmission, so be
 * aware of that. The proper settings can be found in the manual of the
 * device.
 *
 * @todo more documentation on this
 */
class VISTASPACEMOUSEDRIVERAPI VistaSpaceMouseDriver : public IVistaDeviceDriver
{

public:
	VistaSpaceMouseDriver(IVistaDriverCreationMethod *crm);
	~VistaSpaceMouseDriver();

	virtual bool PostUpdate();

public:
	// ########################################################################
	// COMMANDTABLE
	// ########################################################################
	bool CmdBeep(int nDuration);
	bool CmdKeyboard();
	bool CmdSetMode(int nDominant,int nTranslation,int nRotation);
	bool CmdRequestData();
	bool CmdSetNullRadius(int nRadius);
	bool CmdReset(void);
	bool CmdGetDeviceName(std::string &sName);

protected:
	bool DoConnect();
	bool DoDisconnect();
	bool DoSensorUpdate(VistaType::microtime dTs);
	virtual bool PhysicalEnable(bool bEnable);
private:
	// ########################################################################
	// HELPERS
	// ########################################################################
	unsigned char EncodeValue(unsigned int nValue);
	unsigned int  DecodeValue(unsigned char cKey);
	float         BuildCoordinate(unsigned char *cBuffer);

	VistaDriverConnectionAspect *m_pConnection;
	VistaVector3D m_v3Pos;
	VistaQuaternion m_qOri;
	std::vector<double> m_nVecButtonStates;

};

class VISTASPACEMOUSEDRIVERAPI VistaSpaceMouseCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaSpaceMouseCreationMethod(IVistaTranscoderFactoryFactory *metaFac)
		:IVistaDriverCreationMethod(metaFac)
	{
		RegisterSensorType( "",
			sizeof(VistaSpaceMouseMeasures::sSpaceMouseMeasure),
			20, metaFac->CreateFactoryForType("VistaSpaceMouseTranscode") );
	}

	virtual IVistaDeviceDriver *CreateDriver()
	{
		return new VistaSpaceMouseDriver(this);
	}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASPACEMOUSEDRIVER_H
