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


#ifndef _VISTAIRMANDRIVER_H
#define _VISTAIRMANDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaIRManDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

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

class VistaDriverConnectionAspect;
class IVistaDriverCreationMethod;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A driver for an infra-red receiver called IRMan and compatible devices.
 * Decodes keystrokes and collects them as byte-sequences as output of the
 * drivers's sensors. The device is simple in nature, it only needs a proper
 * connection setup (and yes... the device expects that to be a serial connection
 * setup at 8,n,1, 9600 baud.
 * The standard implementation offers to deliver the collected 6byte code.
 * The driver exports a single sensor at index 0.
 *
 * See http://www.intolect.com/irmandetail.htm for more details on the device
 * (and its successors).
 */
class VISTAIRMANDRIVERAPI VistaIRManDriver : public IVistaDeviceDriver
{
public:
	/**
	 * the memory layout for an IRMan sample, 6 bytes as collected by the device.
	 */
	struct _sIRManSample
	{
		unsigned char m_acKeyCode[6];
	};


	VistaIRManDriver( IVistaDriverCreationMethod * );
	virtual ~VistaIRManDriver();


protected:
	bool DoSensorUpdate( VistaType::microtime nTs );

	virtual bool DoConnect();
	virtual bool DoDisconnect();
private:
	VistaDriverConnectionAspect *m_pConnectionAspect;
};


class VISTAIRMANDRIVERAPI VistaIRManDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaIRManDriverCreationMethod(IVistaTranscoderFactoryFactory *fac);
	virtual IVistaDeviceDriver *CreateDriver();
protected:
private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAIRMANDRIVER_H

