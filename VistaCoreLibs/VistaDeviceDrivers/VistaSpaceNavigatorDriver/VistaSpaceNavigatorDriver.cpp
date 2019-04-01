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


/*
#if !defined(_isnan)
#define _isnan(A) (A != A)
#endif
*/
#include <cstring>

#include "VistaSpaceNavigatorDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>

#include <VistaInterProcComm/Connections/VistaConnection.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/



/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSpaceNavigator::VistaSpaceNavigator(IVistaDriverCreationMethod *crm)
	: IVistaDeviceDriver(crm)
{
	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	AddDeviceSensor( pSensor );
	pSensor->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );

	// using the USB connection we have to poll explicitly,
	// since there is no activity on the file handle, but we
	// use ioctls to make the read/write requests.
	SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);
	m_pConAsp = new VistaDriverConnectionAspect;
	RegisterAspect( m_pConAsp );

	m_pConAsp->SetConnection( 0, NULL, "MAIN", true );
}

VistaSpaceNavigator::~VistaSpaceNavigator()
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	RemDeviceSensor( pSensor );
	delete pSensor;

	delete m_pConAsp;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaSpaceNavigator::DoSensorUpdate(VistaType::microtime dTs)
{
	// TODO: check
	bool bNewState = true;

	static _sMeasure measure;

	VistaConnection *pCon = m_pConAsp->GetConnection(0);

	int read = 1;
	unsigned char report[7];
	while( read > 0 )
	{
		// read one full report of 7 bytes
		read = pCon->Receive( report, 7, 0 );

		std::cout << "bytes read: " << read << std::endl;
	}



	if(bNewState)
	{
		MeasureStart( 0, dTs );
		VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(0));

		if(pM == NULL)
		{
			MeasureStop(0);
			return false;
		}

		_sMeasure *m = (_sMeasure*)&(*pM).m_vecMeasures[0];
		std::memcpy( m, &measure, sizeof(_sMeasure) );

		MeasureStop(0);

		GetSensorByIndex(0)->SetUpdateTimeStamp( dTs );
	}


	return true;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

