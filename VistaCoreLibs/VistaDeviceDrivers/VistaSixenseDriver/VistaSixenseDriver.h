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


#ifndef _VISTASIXENSEDRIVER_H
#define _VISTASIXENSEDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include "VistaSixenseCommonShare.h"

//Creation Method
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <vector>
#include <map>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//Windows DLL build
#if defined(WIN32) && !defined(VISTASIXENSEDRIVER_STATIC) 
	#ifdef VISTASIXENSEDRIVER_EXPORTS
		#define VISTASIXENSEDRIVERAPI __declspec(dllexport)
	#else
		#define VISTASIXENSEDRIVERAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTASIXENSEDRIVERAPI
#endif
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverThreadAspect;
class VistaDriverSensorMappingAspect;
struct _sixenseAllControllerData;
typedef _sixenseAllControllerData sixenseAllControllerData;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 *
 */
class VISTASIXENSEDRIVERAPI VistaSixenseDriver : public IVistaDeviceDriver
{
public:
	VistaSixenseDriver( IVistaDriverCreationMethod *crm );
	virtual ~VistaSixenseDriver();


protected:
	virtual bool DoSensorUpdate( VistaType::microtime dTs );
	virtual bool PhysicalEnable( bool bEnable );



	virtual bool UpdateSensor( const int nSensorIndex, const VistaType::microtime nTimestamp );
protected:
	virtual bool DoConnect();
	virtual bool DoDisconnect();
private:
	VistaDriverThreadAspect* m_pThreadAspect;
	VistaDriverSensorMappingAspect* m_pMappingAspect;
	sixenseAllControllerData* m_pData;
	unsigned char m_anLastUpdateIndices[4];
};


//CREATION METHOD

class VISTASIXENSEDRIVERAPI VistaSixenseCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaSixenseCreationMethod( IVistaTranscoderFactoryFactory* metaFac );
	virtual IVistaDeviceDriver* CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASIXENSEDRIVER_H
