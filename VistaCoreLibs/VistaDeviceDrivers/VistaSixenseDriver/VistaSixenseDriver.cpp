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


#include "VistaSixenseDriver.h"

#include "VistaSixenseCommonShare.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>

#include <assert.h>

#include <sixense.h>
#include <sixense_utils/controller_manager/controller_manager.hpp>
#include "VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h"
#include "VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h"
#include "VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h"

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaSixenseDriver::VistaSixenseDriver( IVistaDriverCreationMethod *crm )
: IVistaDeviceDriver( crm )
, m_pData( new sixenseAllControllerData )
{
	SetUpdateType( IVistaDeviceDriver::UPDATE_EXPLICIT_POLL );

	m_pMappingAspect = new VistaDriverSensorMappingAspect( crm );
	RegisterAspect( m_pMappingAspect );

	for( int i = 0; i < 4; ++i )
		m_anLastUpdateIndices[i] = 255;
}

VistaSixenseDriver::~VistaSixenseDriver()
{
	sixenseExit();
	delete m_pData;
}

bool VistaSixenseDriver::DoConnect()
{
	if( sixenseInit() == SIXENSE_FAILURE )
	{
		vstr::warnp() << "[SixenseDriver]: Initialization failed";
		return false;
	}

	sixenseSetActiveBase( 0 ); // @todo: configurable

	//int nNumControllers = sixenseGetNumActiveControllers();

	return true;
}

bool VistaSixenseDriver::DoDisconnect()
{
	return ( sixenseExit() == SIXENSE_SUCCESS );
}

bool VistaSixenseDriver::DoSensorUpdate( VistaType::microtime dTs )
{
	if( sixenseGetAllNewestData( m_pData ) == SIXENSE_FAILURE )
		return false;

	bool bSuccess = false;
	for( int i = 0; i < 4; ++i )
		bSuccess |= UpdateSensor( i, dTs );

	return bSuccess;
}

bool VistaSixenseDriver::PhysicalEnable( bool bEnable )
{
	return true;
}

bool VistaSixenseDriver::UpdateSensor( const int nSensorIndex, const VistaType::microtime nTimestamp )
{
	if( m_pData->controllers[nSensorIndex].enabled == false
		|| m_pData->controllers[nSensorIndex].sequence_number == m_anLastUpdateIndices[nSensorIndex] )
		return false;

	VistaDeviceSensor* pSensor = GetSensorByIndex( nSensorIndex );
	if( pSensor == NULL )
		return false;

	MeasureStart( nSensorIndex, nTimestamp );
	// get the current place for the decoding for sensor 0
	VistaSixenseMeasures::Measure* pMeasure = m_pHistoryAspect->GetCurrentSlot(pSensor)->getWrite<VistaSixenseMeasures::Measure>();

	memcpy( pMeasure, &m_pData->controllers[nSensorIndex], sizeof( VistaSixenseMeasures::Measure ) );

	// we are done. Indicate that to the history
	MeasureStop( nSensorIndex );


	m_anLastUpdateIndices[nSensorIndex] = m_pData->controllers[nSensorIndex].sequence_number;

	return true;
}

/*============================================================================*/
/* CREATION METHOD                                                            */
/*============================================================================*/


VistaSixenseCreationMethod::VistaSixenseCreationMethod( IVistaTranscoderFactoryFactory *metaFac )
: IVistaDriverCreationMethod( metaFac )
{
	RegisterSensorType( "",	sizeof( VistaSixenseMeasures::Measure ), 120, metaFac->CreateFactoryForType( "" ) );
}

IVistaDeviceDriver* VistaSixenseCreationMethod::CreateDriver()
{
	return new VistaSixenseDriver( this );
}

