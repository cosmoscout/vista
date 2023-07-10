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


#ifndef _VISTAICAROSDRIVER_H
#define _VISTAICAROSDRIVER_H

/*
This class implements a driver for the ICAROS controller. More information on the ICAROS device itself can be found here:
https://www.icaros.com/de/produkte/icaros-home

The controller is visible to the operating system as serial COM device. It uses a FTDI chip and needs the following
driver to be installed. The device also needs to be connected using Bluetooth. The driver and some programming examples
can be found here: https://ftdichip.com/software-examples/code-examples/visual-c-examples/

In order to activate the driver in ViSTA you need to add the following to the Vista interaction ini file. 
DEVICEDRIVERS       = ..., ICAROS
INTERACTIONCONTEXTS = ..., ICAROSNAVIGATION

[ICAROS]
TYPE                = ICAROS
HISTORY             = 10
SENSORS             = ICAROS_CONTROLLER

[ICAROS_CONTROLLER]
RAWID               = 0

[ICAROSNAVIGATION]
ROLE                = ICAROSNAVIGATION
GRAPH               = xml/icaros_navigation.xml

The interation ini can then access buttons and orientation as follows
... icaros_navigation.xml ... 
        <node name="icaros" type="DriverSensor">
            <param name="sensor_index" value="0" />
            <param name="driver" value="ICAROS" />
        </node>

        <node name="history" type="HistoryProject">
            <param name="project">ORIENTATION, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4</param>
        </node>
... icaros_navigation.xml end ...
*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAICAROSDRIVER_STATIC) 
#ifdef VISTAICAROSDRIVER_EXPORTS
#define VISTAICAROSAPI __declspec(dllexport)
#else
#define VISTAICAROSAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAICAROSAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverWorkspaceAspect;
class IVistaDriverCreationMethod;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a driver for the ICAROS controller using the FTDI API to write a driver for serial devices. The device itself supports 6 degrees of freedom for orientation 
 * and has 4 buttons to control some application behaviour. The buttons and a quaternion for orientation are passed to the DFN nodes to configure interaction behaviour.
 */
class VISTAICAROSAPI VistaIcarosController : public IVistaDeviceDriver
{
public:
	VistaIcarosController(IVistaDriverCreationMethod *);
	virtual ~VistaIcarosController();

	struct VISTAICAROSAPI _sMeasure
	{
		_sMeasure()
			: 	m_fQuat1(0),
				m_fQuat2(0),
				m_fQuat3(0),
				m_fQuat4(0),
				m_NumberOfButtons(4)
		{
			m_nKeys[0] = m_nKeys[1] = m_nKeys[2] = m_nKeys[3];
		}

		double m_fQuat1,
			   m_fQuat2,
			   m_fQuat3,
			   m_fQuat4;

		long   m_NumberOfButtons;
		long   m_nKeys[4];
	};


protected:
	virtual bool DoSensorUpdate(VistaType::microtime dTs);

	bool DoConnect();
	bool DoDisconnect();
	bool PhysicalEnable( bool enable_flag );

public:
	struct _sPrivate;
private:
	friend class Parameters;
	_sPrivate *m_pPrivate;
	HANDLE 		hCommPort; 	//Handle to COM Port
	bool 		readSensor = true;
};


class VISTAICAROSAPI VistaIcarosCreateMethod : public IVistaDriverCreationMethod
{
public:
	VistaIcarosCreateMethod( IVistaTranscoderFactoryFactory * );
	virtual IVistaDeviceDriver *CreateDriver();
};


#endif //__VISTAICROSDRIVER_H
