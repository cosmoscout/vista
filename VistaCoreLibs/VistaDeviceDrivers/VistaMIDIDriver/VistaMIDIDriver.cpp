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


#include "VistaMIDIDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <cstring>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaMIDIDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
  virtual bool operator()(VistaConnection *pCon)
	{
	  pCon->Close();
	  return !pCon->GetIsOpen();
	}
};

class VistaMIDIAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
  virtual bool operator()(VistaConnection *pCon)
	{
	  if(!pCon->GetIsOpen())
		  return pCon->Open();

	  return true;
	}
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMIDIDriver::VistaMIDIDriver(IVistaDriverCreationMethod *crm) :
  IVistaDeviceDriver(crm),
  m_pConnection(new VistaDriverConnectionAspect)
{
  SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);

  m_pConnection->SetConnection(0, NULL, "MAIN", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT );
  RegisterAspect(m_pConnection);

  m_pConnection->SetDetachSequence(0, new VistaMIDIDetachSequence);
  m_pConnection->SetAttachSequence(0, new VistaMIDIAttachSequence);

  VistaDeviceSensor *pSen = new VistaDeviceSensor();

  pSen->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );
  pSen->SetTypeHint( "" );
  AddDeviceSensor( pSen );
}


VistaMIDIDriver::~VistaMIDIDriver()
{
  VistaDeviceSensor *pSen = GetSensorByIndex(0);
  RemDeviceSensor(pSen);

  delete pSen->GetMeasureTranscode();
  pSen->SetMeasureTranscode(NULL);

  delete pSen;
  delete m_pConnection;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaMIDIDriver::DoConnect()
{
	return true;
}

bool VistaMIDIDriver::DoDisconnect()
{
	return true;
}

bool VistaMIDIDriver::DoSensorUpdate(VistaType::microtime dTs)
{
  unsigned char bMIDIEvent[3];

  VistaMIDIMeasures::sMIDIMeasure sMIDIMeasure;

  VistaDeviceSensor *pSensor = GetSensorByIndex(0);
  VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);


  bool oldstate = m_pConnection->GetConnection(0)->GetIsBlocking();
  m_pConnection->GetConnection(0)->SetIsBlocking(true);

  while( m_pConnection->GetConnection(0)->HasPendingData() )
  {
	MeasureStart( 0, dTs );

	m_pConnection->GetConnection(0)->ReadRawBuffer( bMIDIEvent, 3 );

	sMIDIMeasure.status = bMIDIEvent[0];
	sMIDIMeasure.data0  = bMIDIEvent[1];
	sMIDIMeasure.data1  = bMIDIEvent[2];

	std::memcpy( &(*pM).m_vecMeasures[0], &sMIDIMeasure, sizeof(VistaMIDIMeasures::sMIDIMeasure) );

	MeasureStop( 0 );
  }

  m_pConnection->GetConnection(0)->SetIsBlocking(oldstate);


  return true;
}

