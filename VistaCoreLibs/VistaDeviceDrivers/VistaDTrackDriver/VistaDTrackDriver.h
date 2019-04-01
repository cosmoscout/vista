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


#ifndef _VISTADTRACKDRIVER_H
#define _VISTADTRACKDRIVER_H



/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include "VistaDTrackCommonShare.h"

//Creation Method
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <vector>
#include <map>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//Windows DLL build
#if defined(WIN32) && !defined(VISTADTRACKDRIVER_STATIC) 
	#ifdef VISTADTRACKDRIVER_EXPORTS
		#define VISTADTRACKDRIVERAPI __declspec(dllexport)
	#else
		#define VISTADTRACKDRIVERAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTADTRACKDRIVERAPI
#endif
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverConnectionAspect;
class VistaByteBufferDeSerializer;
class ILineDecode;
class VistaDriverSensorMappingAspect;
class VistaDTrackProtocolAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a driver for A.R.T. optical tracking systems. It supports almost
 * all available features (2009), protocol 1 and 2 of the A.R.T. protocol,
 * measure devices and single markers.
 * It can be a beast to setup, especially, as the A.R.T. device is capable of
 * producing almost any kind of weird mapping from the bodies on the device
 * into the application. So this driver heavily relies on a proper setup mapping
 * aspect. It does, by itself, not create any sensors, but the sensors have
 * to be given from the outside to the driver. The update code merely decodes
 * incoming packets, filters sensors that are present and checks whether the
 * user has wanted them or not (indicated by: a sensor for an A.R.T. sensor id
 * is present in the set of sensors, e.g., the sensor mapping).
 * The driver is capable of attaching to a stream, avoiding to open the line
 * again. As the transfer protocol is UDP, this can be used to sniff on an active
 * transmission, or to put up a logging client.
 *
 * @todo the decoding can be somewhat slow (it uses stream classes...)
 */
class VISTADTRACKDRIVERAPI VistaDTrackDriver : public IVistaDeviceDriver
{
public:
	VistaDTrackDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaDTrackDriver();

	//static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	virtual bool GetAttachOnly() const;
	virtual void SetAttachOnly(bool bAttach);

protected:
	virtual bool DoSensorUpdate(VistaType::microtime dTs);
	virtual bool PhysicalEnable(bool bEnable);
	virtual bool DoConnect();
	virtual bool DoDisconnect();

private:
	VistaDriverConnectionAspect     *m_pConnection;
	VistaDTrackProtocolAspect       *m_pProtocol;
	VistaDriverSensorMappingAspect  *m_pSensors;
	VistaByteBufferDeSerializer     *m_pDeSerializer,
									*m_pLine;
	std::vector<VistaType::byte>	m_vecPacketBuffer;


	typedef std::map<std::string, ILineDecode*> DECODEMAP;
	DECODEMAP m_mapDecoder;
	unsigned int m_nGlobalType,
				 m_nMarkerType;

	bool m_bAttachOnly;
};


//CREATION METHOD

class VISTADTRACKDRIVERAPI VistaDTrackCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaDTrackCreationMethod(IVistaTranscoderFactoryFactory *metaFac);
	virtual IVistaDeviceDriver *CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADTRACKDRIVER_H
