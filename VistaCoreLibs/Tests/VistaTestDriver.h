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

#ifndef _VISTATESTDRIVER
#define _VISTATESTDRIVER

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <gtest/gtest.h>

#include <memory>

#include <VistaAspects/VistaReflectionable.h>

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>


/*============================================================================*/
/* HELPERS                                                                    */
/*============================================================================*/

struct VistaTestDriverMeasure
{
	int m_nValue;
};

class VistaTestDriver : public IVistaDeviceDriver
{
public:
	VistaTestDriver( IVistaDriverCreationMethod* pCreationMethod );
	
	virtual bool DoSensorUpdate( VistaType::microtime dTs ) override;

	void PushValue( int nValue );

	virtual bool DoConnect() override;

	virtual bool DoDisconnect() override;

	class CreationMethod;
private:
	int m_nReadBuffer;
};


class VistaTestDriverWrapper
{
public:
	VistaTestDriverWrapper();
	~VistaTestDriverWrapper();

	VistaTestDriver* GetDriver();
	VistaDeviceSensor* GetSensor();
	const VistaMeasureHistory& GetHistory();
	IVistaMeasureTranscode::IntGet* GetTranscodeGet();
	
private:
	
	std::unique_ptr< IVistaTranscoderFactoryFactory > m_pMetaFactory;
	std::unique_ptr< VistaTestDriver::CreationMethod > m_pCreationMethod;
	std::unique_ptr< VistaTestDriver > m_pDriver;
	
	VistaDeviceSensor* m_pSensor;
};

#endif //_VISTATESTDRIVER

