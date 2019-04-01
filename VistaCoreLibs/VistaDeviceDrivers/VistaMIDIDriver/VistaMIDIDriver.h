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


#ifndef _VISTAMIDIDRIVER_H__
#define _VISTAMIDIDRIVER_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include "VistaMIDICommonShare.h"
#include <string>

//CRM
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAMIDIDRIVER_STATIC) 
	#ifdef VISTAMIDIDRIVER_EXPORTS
		#define VISTAMIDIDRIVERAPI __declspec(dllexport)
	#else
		#define VISTAMIDIDRIVERAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTAMIDIDRIVERAPI
#endif

// Shared library support
/*
**OLD!
#ifdef WIN32
#define VISTAMIDIEXPORT __declspec(dllexport)
#define VISTAMIDIIMPORT __declspec(dllimport)
#define VISTAMIDI_EXPLICIT_TEMPLATE_EXPORT
#define VISTAMIDI_EXPLICIT_TEMPLATE_IMPORT
#else
#define VISTAMIDIEXPORT
#define VISTAMIDIIMPORT
#endif

// Define VISTAMIDIAPI for DLL builds
#ifdef VISTAMIDIDLL
#ifdef VISTAMIDIDLL_EXPORTS
#define VISTAMIDIAPI VISTAMIDIEXPORT
#define VISTAMIDI_EXPLICIT_TEMPLATE
#else
#define VISTAMIDIAPI VISTAMIDIIMPORT
#define VISTAMIDI_EXPLICIT_TEMPLATE extern
#endif
#else
#define VISTAMIDIAPI __declspec(dllexport)
#endif
*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverConnectionAspect ;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * standard Linux midi driver. This driver is not known to work under windows.
 *
 *
 * @todo update documentation, especially on setting up the connection
 * @todo check windows implementation
 * @todo see whether this driver can benefit from the new Connect() API
 */
class VISTAMIDIDRIVERAPI VistaMIDIDriver : public IVistaDeviceDriver
{
public:
	VistaMIDIDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaMIDIDriver();

protected:
	bool DoConnect();
	bool DoDisconnect();

	bool DoSensorUpdate(VistaType::microtime dTs);

private:
	// the file descriptor for the device file
	VistaDriverConnectionAspect* m_pConnection ;
};



class VISTAMIDIDRIVERAPI VistaMIDIDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaMIDIDriverCreationMethod(IVistaTranscoderFactoryFactory *metaFac)
		:IVistaDriverCreationMethod(metaFac)
	{
		RegisterSensorType( "",
			sizeof(VistaMIDIMeasures::sMIDIMeasure),
			100, metaFac->CreateFactoryForType("VistaMIDITranscode") );
	}

	virtual IVistaDeviceDriver *CreateDriver()
	{
		return new VistaMIDIDriver(this);
	}
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTAMOUSEDRIVER_H
