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


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaCyberGloveDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaInterProcComm/Connections/VistaConnectionSerial.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>

#include <VistaBase/VistaStreamUtils.h>

#include <cstring>
#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
#define CG_SLEEP_TIME 1000

class VistaCyberGloveProtocol
{
public:

	// Retrieves the DataGlove Info String (p. 32)
	static bool CmdGetCyberGloveInfo(char * pInfoText, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = {'?','i',0};

		// Send Command
		if(!pConnectionAspect->SendCommand(0, pCommand, 2, CG_SLEEP_TIME))
			return false;

		// Retrieve Respone
		if( !(pConnectionAspect->GetTerminatedCommand(0, pInfoText,~0,0) > 0) )
			return false;

#ifdef _DEBUG
		vstr::outi() << "[VistaCyberGlove] CyberGloveInfo:" << endl;
		vstr::outi() << pInfoText << endl;
#endif

		return true;
	}

	// Request DataGlove Connection Info (p. 31)
	// The returned status byte has the following meaning:
	// Bit 0: DataGlove properly intitialised
	// Bit 1: DataGlove Connected to CGIU
	static int CmdGetConnectionInfo(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = {'?','G',0};
		char pResponse[5] = {0,0,0,0,0};
		int nReturn =0;

		// Send Command
		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		// Retrieve Respone
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		// Test if valid data packet
		if(pResponse[1] != 'G')
		{
#ifdef _DEBUG
			vstr::errp() << "[VistaCyberGlove] Trashed Response Received (CmdGetConnectionInfo)" << endl;
			vstr::erri() << vstr::singleindent << "Command Bytes: " << pResponse[0] << pResponse[1] << endl;
#endif
			return -1;
		}

		// Extract Connection Info
		nReturn = (int)pResponse[2];

#ifdef _DEBUG
		if(nReturn > 3)
			vstr::errp() << "[VistaCyberGlove] Connection Info Byte Corrupted" << endl;
#endif

		return nReturn;
	}

	// Indicates whether the glove status byte should be included in each data packet (p. 50)
	static bool CmdIncludeStatusByte(bool bMode, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Ux";
		char pResponse[5] = {0,0,0,0,0};

		// fill-in parameters
		if(bMode)
			pCommand[1] = 1;
		else
			pCommand[1] = 0;

		// Send Command
		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		// Retrieve Respone
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	// Indicates whether a timestamp should be appended to each data packet (p. 28)
	static bool CmdIncludeTimestamp(bool bMode, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Dx";
		//unsigned char pCommand[] = {'D','x'};
		char pResponse[5] = {0,0,0,0,0};

		// Fill-in parameters
		if(bMode)
			pCommand[1] = 1;
		else
			pCommand[1] = 0;

		// Send Command
		/* problem: strlen returns size of string till null byte. if bMode == false, returned length is 1, but needs to be 2.
		if(!GetConnection()->Send(pCommand,strlen((char*)pCommand)))
			return false;
		*/
		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		// Retrieve Respone
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}


	// Restart the CGIU firmware (reset). This command returns nul only AFTER the restart
	// is complete (p. 45)
	static bool CmdRestartFirmware(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = {18};    // CTRL-R = ^r = 18F
		char pResponse[5] = {0,0,0,0,0};

		// Send Command
		/* Note: "It takes about 1.75 seconds before this command is completed and the nul is returned." (p. 45)
		 * Hence, a post-send-timeout of 2.5s should be sufficient.
		 */

		if(!pConnectionAspect->SendCommand(0,pCommand,1,2500))
			return false;

		// Retrieve Response
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0,1000) > 0) )
			return false;

		// Check if reset was successful.
		if( pResponse[0] == pCommand[0] )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// Enabling digital filtering results in a more stable signal from the sensors
	// It adds about 0.126ms of lag
	// See also p.29
	static bool CmdSetDigitalFilter(bool bMode, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Fx";
		char pResponse[5] = {0,0,0,0,0};

		// Fill-in parameters
		if(bMode)
			pCommand[1] = 1;
		else
			pCommand[1] = 0;

		// Send Command
		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		// Retrieve Response
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	// You can choose whether the switch should control the light
	// (switch pressed -> light on) or whether they should be independent (bMode = 0)
	// See also p. 35
	static bool CmdSetSwitchLightMode(bool bMode, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Jx";
		char pResponse[5] = {0,0,0,0,0};

		// Set Paramaters
		// bMode = 0: Light / Switch independent
		// bMode = 1: Switch controls Light
		if(bMode)
			pCommand[1] = 1;
		else
			pCommand[1] = 0;

		// Send Command
		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		// Retrieve Response
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	// Returns the state of the switch on the wrist of the DataGlove
	// A return value of true means that the switch is pressed (p.52)
	static bool CmdGetSwitchState(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "?W";
		char pResponse[5] = {0,0,0,0,0};

		// Send Command
		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		// Retrieve Response
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		// Response Format "?Wx" with x=0 Switch Off x=1 Switch On
		if(pResponse[1] == 'W')
			return pResponse[2] ? true : false;
		else
			return false;
	}

	// Sets the state of the switch on the DataGlove wrist
	// true = pressed / false = released (p. 52)
	static bool CmdSetSwitchState(bool bState, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Wx";
		char pResponse[5] = {0,0,0,0,0};

		// Set parameters
		if(bState)
			pCommand[1] = 1;
		else
			pCommand[1] = 0;

		// Send Command
		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		// Retrieve Response
		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	static bool CmdSetLightState(bool bState, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Lx";
		char pResponse[5] = {0,0,0,0,0};

		if(bState)
			pCommand[1] = 1;
		else
			pCommand[1] = 0;

		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	static bool CmdGetLightState(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "?L";
		char pResponse[5] = {0,0,0,0,0};

		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		// Response Format "?Lx" with x=0 Light Off x=1 Light On
		if(pResponse[1] == 'L')
			return pResponse[2] ? true : false;
		else
			return false;
	}

	static int CmdGetSingleRecord(VistaCyberGloveCommonShare::sCyberGloveSample * pRecord, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "G";

		if(!pConnectionAspect->SendCommand(0,pCommand,1,CG_SLEEP_TIME))
			return -1;

		return pConnectionAspect->GetTerminatedCommand(0,pRecord,~0,0);
	}

	static bool CmdStartStreamingMode(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "S";

		if(!pConnectionAspect->SendCommand(0,pCommand,1,CG_SLEEP_TIME))
			return false;

		// In streaming mode the data packets are evaluated by the DoSensorUpdate() method
		// so no packets are read here
		return true;
	}

	static bool CmdEndStreamingMode(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = {3}; // CTRL-C = ^c = 3F
		char pResponse[5] = {0,0,0,0,0};

		if(!pConnectionAspect->SendCommand(0,pCommand,1,CG_SLEEP_TIME))
			return false;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	static bool CmdSetSensorMask(char b1,char b2,char b3, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Mxxx";
		char pResponse[5] = {0,0,0,0,0};

		pCommand[1] = b1;
		pCommand[2] = b2;
		pCommand[3] = b3;

		if(!pConnectionAspect->SendCommand(0,pCommand,4,CG_SLEEP_TIME))
			return false;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	static bool CmdGetSensorMask(char * cResult, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "?M";
		char pResponse[5] = {0,0,0,0,0};

		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		if(pResponse[1] != 'M')
		{
#ifdef _DEBUG
			vstr::warnp() << "[VistaCyberGlove] CmdGetSensorMask received invalid packet (id "
					<< pResponse[1] << ")" << endl;
#endif
			return false;
		}

		cResult[0] = pResponse[2];
		cResult[1] = pResponse[3];
		cResult[2] = pResponse[4];

#ifdef _DEBUG
		vstr::outi() << "[VistaCyberGlove] SensorMask : " << (int)pResponse[2] << " " << (int)pResponse[3]
					 << " "  << (int)pResponse[4] << endl;
#endif

		return true;
	}

	static bool CmdSetNumberOfSensors(int nSensors, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Nx";
		char pResponse[5] = {0,0,0,0,0};

		if(nSensors > 24)
			return false;

		pCommand[1] = (char)nSensors;

		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return false;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}

	static int CmdGetMaxNumberOfSensors(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "?S";
		char pResponse[5] = {0,0,0,0,0};

		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return -1;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return -1;

		if( pResponse[1] != 'S' )
		{
			vstr::warnp() << "[VistaCyberGlove] CmdGetMaxNumberOfSensors received invalid packet (id " 
					<< pResponse[1] << ")" << endl;
			return -1;
		}

		if( pResponse[2] > 24 )
			return -1;

		return (int)(pResponse[2]);
	}

	static int CmdGetNumberOfSensors(VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "?N";
		char pResponse[5] = {0,0,0,0,0};

		if(!pConnectionAspect->SendCommand(0,pCommand,2,CG_SLEEP_TIME))
			return -1;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return -1;

		if( pResponse[1] != 'N' )
		{
			vstr::warnp() << "[VistaCyberGlove] CmdGetNumberOfSensors received invalid packet (id " 
							<< pResponse[1] << ")" << endl;
			return -1;
		}

		if( pResponse[2] > 24 )
			return -1;

		return (int)(pResponse[2]);
	}

	static bool CmdSetSamplingPeriod(int w1,int w2, VistaDriverConnectionAspect* pConnectionAspect)
	{
		unsigned char pCommand[] = "Txxxx";
		char pResponse[5] = {0,0,0,0,0};

		// Little-Endian -> Big-Endian Conversion !!!!
		//********************************************
		pCommand[2] = (char)w1;
		pCommand[1] = (char)(w1<<8);
		pCommand[4] = (char)w2;
		pCommand[3] = (char)(w2<<8);


		if(!pConnectionAspect->SendCommand(0,pCommand,5,CG_SLEEP_TIME))
			return false;

		if( !(pConnectionAspect->GetTerminatedCommand(0,pResponse,5,0) > 0) )
			return false;

		return true;
	}
};

static int VCG_FrequencyMapping[]={1,2,10,15,30,45,60,75,100,0};
static int VCG_FrequencyW1[]={57600,57600,11520,7680,3840,2560,1920,1536,1152,0,0};
static int VCG_FrequencyW2[]={2,1,1,1,1,1,1,1,1,0,0};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaCyberGloveDriver::VistaCyberGloveDriver(IVistaDriverCreationMethod *crm)
	: IVistaDeviceDriver(crm),
	  m_pConnectionAspect(new VistaDriverConnectionAspect),
	  m_bInitDone(false),
	  m_bStreamingMode( false ),
	  m_bTempStreamingMode( false ),
	  m_bIncStatusByte( false ),
	  m_bIncTimestamp( false ),
	  m_bSwitchLightMode( false ),
	  m_bFilterMode( false ),
      m_nNumSensors( 0 ),
	  m_nSensorMask( 0 ),
	  m_nMaxSensors( 0 ),
      m_nSampleFreq( VCG_FREQ_MAX ),
      m_nSampleW1( 0 ),
      m_nSampleW2( 0 )
{
	// Invalid -> retrieve info from CGIU at first request
	m_nMaxSensors = -1;

	// CONNECTION ASPECT

	m_pConnectionAspect->SetConnection( 0, NULL, "MAIN", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT );
	m_pConnectionAspect->SetUpdateConnectionIndex(0);
	SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);
	RegisterAspect( m_pConnectionAspect );

	// DEVICE SENSOR

	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	AddDeviceSensor(pSensor);
	pSensor->SetTypeHint( "" );
	pSensor->SetMeasureTranscode(crm->GetTranscoderFactoryForSensor("")->CreateTranscoder());
}

VistaCyberGloveDriver::~VistaCyberGloveDriver()
{
	if( GetIsConnected() )
	{
		DoDisconnect();
	}

	UnregisterAspect( m_pConnectionAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pConnectionAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaCyberGloveDriver::DoConnect()
{
	// open and init the connection
	VistaConnection* pCon = m_pConnectionAspect->GetConnection(0);
	if (!pCon) // could not get connection. Possibly no serial port present.
	{
		vstr::errp() << "[VistaCyberGlove] Error opening connection" << std::endl;
		return false;
	}

	if(!pCon->GetIsOpen())
	{
		if(!pCon->Open())
		{
			vstr::errp() << "[VistaCyberGlove] Error opening connection" << std::endl;
			return false;
		}
	}

	// flush pending data
	pCon->SetIsBlocking(false);
	char c;
	while( pCon->ReadRawBuffer( &c, 1 ) > 0 ); // semicolon on purpose

	//---------------------
	// Configure CyberGlove
	//---------------------
	pCon->SetIsBuffering(true);
	pCon->SetIsBlocking(true);

	vstr::outi() << "[VistaCyberGlove] Connecting to CyberGlove... ";

	// do max three retries for RestartFirmware
	int nTries = 3;

	while ( (nTries>0) && (!VistaCyberGloveProtocol::CmdRestartFirmware(m_pConnectionAspect)) )
	{
		vstr::out().flush();
		vstr::warn() << "FAILED" << endl;
		nTries--;

		// on fail: disconnect, reconnect and try again
		pCon->Close();
		pCon->Open();
		pCon->SetIsBuffering(true);
		pCon->SetIsBlocking(true);

		if (nTries > 0)
		{
			vstr::outi() << "[VistaCyberGlove] CyberGlove connection retry... ";
		}
	}

	if (nTries > 0)
	{
		vstr::out() << "SUCCESS" << endl;
	}
	else
	{
		vstr::warnp() << "[VistaCyberGlove] Unable to connect to CyberGlove." << endl;
		return false;
	}

	char pInfoText[512];
	int nConInfo;

	// Get system Identification
	VistaCyberGloveProtocol::CmdGetCyberGloveInfo(pInfoText, m_pConnectionAspect);
	if(!strstr(pInfoText,"CyberGlove"))
	{
		vstr::warnp() << "[VistaCyberGlove] No CyberGlove found on this connection" << endl;
		return false;
	}
	vstr::outi() << "[VistaCyberGlove] CyberGlove detected" << endl;

	// Check if the equipment is properly connected
	// Returned Value: bit0 - CyberGlove initialized?
	//                 bit1 - CyberGlove connected?
	nConInfo = VistaCyberGloveProtocol::CmdGetConnectionInfo(m_pConnectionAspect);
	if(nConInfo != 3)
	{
		vstr::warnp() << "[VistaCyberGlove] Connection error (code "
				<< nConInfo << "): Make sure the glove is properly connected to the CyberGlove Interface Unit." << endl;
		return false;
	}

	VistaDeviceSensor *pSensor = new VistaDeviceSensor;

	pSensor->SetTypeHint( "" );
	pSensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor( "" )->CreateTranscoder());

	AddDeviceSensor(pSensor);

	// Init Done
	m_bInitDone = true;

	// Get the dataGlove Type (18 or 22 sensors)
	m_nMaxSensors = GetMaxNumberOfSensors();
	if(m_nMaxSensors == -1)
	{
		vstr::warnp() << "[VistaCyberGlove] Could not retrieve number of sensors in Glove." << endl;
		m_nMaxSensors = 18;
	}
	else
	{
		vstr::outi() << "[VistaCyberGlove] The device has " << m_nMaxSensors << " sensors." << endl;
	}

	// set configuration to driver default
	return RestoreDefaultSettings();
}

bool VistaCyberGloveDriver::DoDisconnect()
{
	// clean up: disable streaming mode flush pending data
	if (m_bInitDone && m_bStreamingMode)
	{
		SetStreamingMode(false);
	}

	VistaConnection *pCon = m_pConnectionAspect->GetConnection(0);

	if ( pCon )
	{
		pCon->Close();
		VistaDeviceSensor *pSensor = GetSensorByIndex(0);
		RemDeviceSensor(pSensor);
		m_pHistoryAspect->UnregisterSensor(pSensor);
		delete pSensor;

		return true;
	}

	return false;
}


//*****************************
// Public Configuration Methods
//*****************************

bool VistaCyberGloveDriver::RestoreDefaultSettings()
{
	if (!m_bInitDone)
	{
		return false;
	}

	m_bIncStatusByte		= true;
	m_bIncTimestamp			= false;
	m_bStreamingMode		= false;
	m_bTempStreamingMode	= true;
	m_bSwitchLightMode		= true;
	m_bFilterMode			= true;

	// Default is 18 sensors
	m_nNumSensors = 18;

	// Default Sensor Mask for 18 Sensor CyberGlove
	m_nSensorMask = 0x7bbb3F;

	// Default Sampling Frequency is Maximum (data packets are sent as fast as possible)
	m_nSampleFreq = VCG_FREQ_MAX;
	m_nSampleW1 = 0;
	m_nSampleW2 = 0;

	// Configure CyberGlove
	vstr::outi() << "[VistaCyberGlove] Setting CyberGlove configuration to driver default... ";

#ifdef WIN32
	_preCommunicate();
#endif

	// Note: Configuration stops after first encounter of an error due to way boolean conjunctions are evaluated
	bool noError = ( SetDigitalFilter(m_bFilterMode) &&
					 SetSwitchLightMode(m_bSwitchLightMode) &&
					 SetSensorMask(m_nSensorMask) &&
					 SetNumberOfSensors(m_nNumSensors) &&
					 SetPacketFormat(m_bIncStatusByte,m_bIncTimestamp) &&
					 SetSamplingFrequency(m_nSampleW1,m_nSampleW2) &&
					 SetStreamingMode(m_bTempStreamingMode) );

#ifdef WIN32
	_postCommunicate();
#endif

	if (noError)
	{
		vstr::out() << "SUCCESS" << endl;
		return true;
	}
	else
	{
		vstr::out().flush();
		vstr::warn() << "FAILED" << endl;
		return false;
	}
}
//--------------------
// Light Configuration
//--------------------

// Returns the current state of the small light on the wrist of the glove
// The State can either be set via software or using the switch on the wrist
//
// If in streaming mode this method always returns false. If status byte inclusion is enabled
// The light state can then be obtained by accessing the last recorded sample.
bool VistaCyberGloveDriver::GetLightState(void) const
{
	// Check if init was successful
	if(!m_bInitDone)
		return false;

	// If in Streaming mode
	if(m_bStreamingMode)
	{
		return false;
	}
	else
	{
#ifdef WIN32
		_preCommunicate();
#endif
		// Fetch light state
		bool noError = VistaCyberGloveProtocol::CmdGetLightState(m_pConnectionAspect);
#ifdef WIN32
		_postCommunicate();
#endif
		return noError;
	}
}

// Allows to set the state of the small light on the glove via software
// Using this command makes the light independent of the switch (see also Switch Configuration)
bool VistaCyberGloveDriver::SetLightState(bool bState)
{
	bool bReturn;

	// Check if init was successful
	if(!m_bInitDone)
		return false;

#ifdef WIN32
	_preCommunicate();
#endif
	// Set new light state
	bReturn = VistaCyberGloveProtocol::CmdSetLightState(bState, m_pConnectionAspect);
#ifdef WIN32
	_postCommunicate();
#endif

	// If the command was successful the light is now independent of the switch
	if(bReturn)
		m_bSwitchLightMode = false;

	// Return result
	return bReturn;
}

//---------------------
// Switch Configuration
//---------------------

// Tells the Cyberglove whether the light on the glove should be linked to the switch (bMode = true)
// or whether light and switch should be independent (bMode = false)
// Default Mode is bMode = true;
bool VistaCyberGloveDriver::SetSwitchLightMode(bool bMode)
{
	bool bReturn;

	// Check if initialistion was successful
	if(!m_bInitDone)
	{
		m_bSwitchLightMode = bMode;
		return true;
	}

#ifdef WIN32
	_preCommunicate();
#endif
	// Try to set new switch/light mode
	bReturn = VistaCyberGloveProtocol::CmdSetSwitchLightMode(bMode, m_pConnectionAspect);

#ifdef WIN32
	_postCommunicate();
#endif

	// Command successful -> Save new Mode
	if(bReturn)
	{
		m_bSwitchLightMode = bMode;
		return true;
	}
	// An Error occured
	else
		return false;
}

// Returns the current switch-light mode (see above)
bool VistaCyberGloveDriver::GetSwitchLightMode(void)
{
	return m_bSwitchLightMode;
}

// Returns the current state of the switch on the wrist of the glove
// The State can either be set via software or using the switch on the wrist
//
// If in streaming mode this method always returns false. If status byte inclusion is enabled
// The switch state can then be obtained by accessing the last recorded sample.
bool VistaCyberGloveDriver::GetSwitchState(void)
{
	// Check if init was successful
	if(!m_bInitDone)
		return false;

	if(m_bStreamingMode)
	{
		return false;
	}
	else
	{
#ifdef WIN32
		_preCommunicate();
#endif
		// Fetch switch state
		bool noError = VistaCyberGloveProtocol::CmdGetSwitchState(m_pConnectionAspect);
#ifdef WIN32
		_postCommunicate();
#endif
		return noError;
	}
}

// Allows to set the state of the switch on the glove via software
// This command can only be executed if streaming mode is disabled (sync problems)
bool VistaCyberGloveDriver::SetSwitchState(bool bState)
{
	// Check if init was successful
	if(!m_bInitDone || m_bStreamingMode)
		return false;

#ifdef WIN32
	_preCommunicate();
#endif
	// Set new switch state
	bool noError = VistaCyberGloveProtocol::CmdSetSwitchState(bState, m_pConnectionAspect);
#ifdef WIN32
	_postCommunicate();
#endif

	return noError;
}

//-----------------------
// Format of Data-Packets
//-----------------------

// Choose the format of a data packet
// Data Packet: <Sensor Values> [Timestamp] [Status]
// Use this method to include or exclude timestamp and status data
// The status byte contains information about the CyberGlove connection, the light
// and the switch state (see p. 50)
bool VistaCyberGloveDriver::SetPacketFormat(bool bStatus,bool bTimestamp)
{
	bool bReturn1,bReturn2;

	if(!m_bInitDone)
	{
		m_bIncStatusByte = bStatus;
		m_bIncTimestamp = bTimestamp;
		return true;
	}

#ifdef WIN32
	_preCommunicate();
#endif

	// Attempt to set status byte flag
	bReturn1 = VistaCyberGloveProtocol::CmdIncludeStatusByte(bStatus, m_pConnectionAspect);
	if(bReturn1)
		m_bIncStatusByte = bStatus;

	bReturn2 = VistaCyberGloveProtocol::CmdIncludeTimestamp(bTimestamp, m_pConnectionAspect);
	if(bReturn2)
		m_bIncTimestamp = bTimestamp;

#ifdef WIN32
	_postCommunicate();
#endif

	// Fail if one of the two calls reported an error
	if(!bReturn1 || !bReturn2)
		return false;
	else
		return true;
}

// The sensor mask indicates which sensors of the glove are reported
// This mask is combined internally with the hardware sensor mask, so
// no invalid sensors can be enabled. Bit 0 stands for Sensor #1 and
// Bit 23 for Sensor #24 (see p.20)
bool VistaCyberGloveDriver::SetSensorMask(int nMask)
{
	char b1,b2,b3;

	// Proceed only if init was successful
	if(!m_bInitDone)
	{
		m_nSensorMask = nMask;
		return true;
	}

	// Split Mask into 3 Byte Values
	b1 = (char)(nMask & 0xFF);
	b2 = (char)( (nMask >> 8) & 0xFF );
	b3 = (char)( (nMask >> 16) & 0xFF );

#ifdef WIN32
	_preCommunicate();
#endif
	// Set Software Mask
	bool noError = VistaCyberGloveProtocol::CmdSetSensorMask(b1,b2,b3, m_pConnectionAspect);
#ifdef WIN32
	_postCommunicate();
#endif

	if(noError)
	{
		m_nSensorMask = nMask;
		return true;
	}

	return false;
}

// Retrieves the current sensor mask from the CGIU
// If the CyberGlove is in use, the last value is returned
long VistaCyberGloveDriver::GetSensorMask()
{
	char pResponse[3];
	int nSensorMask = 0;

	// Proceed only if init was successful
	if(!m_bInitDone)
		return -1L;

#ifdef WIN32
	_preCommunicate();
#endif
	// Get sensor mask from CGIU
	bool noError = VistaCyberGloveProtocol::CmdGetSensorMask(pResponse, m_pConnectionAspect);
#ifdef WIN32
	_postCommunicate();
#endif

	if(!noError)
		return -1L;

	// Build Mask from Data Packet (b1,b2,b3)
	nSensorMask = (int)(pResponse[2]); // MSB
	nSensorMask = (nSensorMask << 8) | (int)(pResponse[1]);
	nSensorMask = (nSensorMask << 8) | (int)(pResponse[0]);    // LSB

	// Save Mask for future reference
	m_nSensorMask = nSensorMask;

	// Return Sensor Mask
	return nSensorMask;
}

// This method allows the user to include only the nNumSensors first sensor
// values in each data packet even if more sensors are enabled in the sensor mask
bool VistaCyberGloveDriver::SetNumberOfSensors(int nNumSensors)
{
	// Proceed only if init was successful
	if(!m_bInitDone)
	{
		m_nNumSensors = nNumSensors;
		return true;
	}

	// Only reconfigure CGIU if the glove is not in use
	if(m_bStreamingMode)
		return false;

#ifdef WIN32
	_preCommunicate();
#endif
	// Set the number of returned sensor values
	bool noError = VistaCyberGloveProtocol::CmdSetNumberOfSensors(nNumSensors, m_pConnectionAspect);
#ifdef WIN32
	_postCommunicate();
#endif

	if(!noError)
		return false;

	// Save current number of sensors
	m_nNumSensors = nNumSensors;

	// Everything is fine
	return true;
}

// Returns the number of sensor values that are included in each data packet
// If the CGIU is in use, the last known value is returned
int VistaCyberGloveDriver::GetNumberOfSensors()
{
	int nNumSensors;

	// Proceed only if init was successful
	if(!m_bInitDone)
		return -1;

#ifdef WIN32
	_preCommunicate();
#endif
	// Get sensor mask from CGIU
	nNumSensors = VistaCyberGloveProtocol::CmdGetNumberOfSensors(m_pConnectionAspect);
#ifdef WIN32
	_postCommunicate();
#endif

	if( nNumSensors == -1 )
		return -1;

	// Save current number of sensors
	m_nNumSensors = nNumSensors;

	// Everything is fine
	return nNumSensors;
}

// Returns the number of sensors of the DataGlove
// This value never changes, so it only needs to be retrieved from the CGIU once
int VistaCyberGloveDriver::GetMaxNumberOfSensors()
{
	if(!m_bInitDone)
		return -1;

	if(m_nMaxSensors == -1)
	{
#ifdef WIN32
		_preCommunicate();
#endif
		m_nMaxSensors = VistaCyberGloveProtocol::CmdGetMaxNumberOfSensors(m_pConnectionAspect);
#ifdef WIN32
		_postCommunicate();
#endif
	}

	return m_nMaxSensors;
}

//-----------------------
// Hardware Configuration
//-----------------------

// Enabling digital filtering results in a more stable signal from the sensors
// It adds about 0.126ms of lag
// See also p.29
bool VistaCyberGloveDriver::SetDigitalFilter(bool bMode)
{
	// Don't proceed if in streaming mode
	if(m_bStreamingMode)
		return false;

	// If not initialised yet, store the mode
	if(!m_bInitDone)
	{
		m_bFilterMode = bMode;
		return true;
	}

#ifdef WIN32
	_preCommunicate();
#endif
	bool noError = VistaCyberGloveProtocol::CmdSetDigitalFilter(bMode, m_pConnectionAspect);
#ifdef WIN32
	_postCommunicate();
#endif

	if(noError)
	{
		m_bFilterMode = bMode;
		return true;
	}

	return false;
}

// Restarts the CGIU Firmware and resets all attributes to
// driver default values. A restart takes about 2 seconds.
bool VistaCyberGloveDriver::Reset()
{
	// This command can be executed before init is finished
	// (e.g. in case an error occured during init), but the serial
	// port has to be open
	if(m_pConnectionAspect->GetConnection(0) == NULL)
		return false;

#ifdef WIN32
	_preCommunicate();
#endif
	vstr::outi() << "[VistaCyberGlove] Resetting Cyberglove Hardware... " << std::flush;
	bool bReturn = VistaCyberGloveProtocol::CmdRestartFirmware(m_pConnectionAspect);

	if (bReturn)
	{
		vstr::out() << "SUCCESS" << endl;
		bReturn = RestoreDefaultSettings();
	}
	else
	{
		vstr::warn() << "FAILED" << endl;
	}

#ifdef WIN32
	_postCommunicate();
#endif

	// Return result
	return bReturn;
}

//-------------------
// Sampling Frequency
//-------------------

// Sets the sampling frequency to one of the predefined values
// VCG_FREQ_CUSTOM is not allowed, if you want to specify your own freuqency
// use SetSamplingFrequency(int w1, int w2)
bool VistaCyberGloveDriver::SetSamplingFrequency(VCG_SAMPLE_FREQ nFreq)
{
	int w1,w2,i,nIndex = -1;
	bool bReturn = true;

	// Custom Frequency is not allowed
	if(nFreq == VCG_FREQ_CUSTOM)
		return false;

	for(i=0 ; i<10 ; i++)
	{
		if(VCG_FrequencyMapping[i] == (int)nFreq)
		{
			nIndex = i;
			break;
		}
	}

	if(nIndex < 0)
		return false;

	// Retrieve counter values
	w1 = VCG_FrequencyW1[nIndex];
	w2 = VCG_FrequencyW2[nIndex];

	// Set Sampling Period and store values
	if(m_bInitDone)
	{
#ifdef WIN32
		_preCommunicate();
#endif
		bReturn = VistaCyberGloveProtocol::CmdSetSamplingPeriod(w1,w2, m_pConnectionAspect);
#ifdef WIN32
		_postCommunicate();
#endif
	}

	if(bReturn)
	{
		m_nSampleFreq   = nFreq;
		m_nSampleW1     = w1;
		m_nSampleW2     = w2;

		return true;
	}
	else
		return false;
}

// If you need a sampling frequency that is not included in the list
// of predefined frequencies, you can can specify your own values for the counters
// w1 and w2. The CGIU counts from w1 to 0 and does so w2 times before the next data
// packet is sent out (see p. 47)
bool VistaCyberGloveDriver::SetSamplingFrequency(int w1, int w2)
{
	bool bReturn = true;

	// Set sampling Period and store values
	if(m_bInitDone)
	{
#ifdef WIN32
		_preCommunicate();
#endif
		bReturn = VistaCyberGloveProtocol::CmdSetSamplingPeriod(w1,w2, m_pConnectionAspect);
#ifdef WIN32
		_postCommunicate();
#endif
	}

	if(bReturn)
	{
		m_nSampleFreq   = VCG_FREQ_CUSTOM;
		m_nSampleW1     = w1;
		m_nSampleW2     = w2;

		return true;
	}
	else
		return false;
}

VistaCyberGloveDriver::VCG_SAMPLE_FREQ VistaCyberGloveDriver::GetSamplingFrequency(void) const
{
	return m_nSampleFreq;
}

bool VistaCyberGloveDriver::GetSamplingFrequency(int * w1, int * w2) const
{
	*w1 = m_nSampleW1;
	*w2 = m_nSampleW2;

	return true;
}

/* @todo 
//------------------------------
// Trigger Mode (Thread/Trigger)
//------------------------------

// You can choose between running the DataGlove in its own thread (independent of the main
// Vista loop) or updating the device each time the Vista main loop is executed
bool SetTriggerMode(VistaInputDevice::VISTA_TRIGGER_MODE Mode)
{

	VistaInputDevice::SetTriggerMode(Mode);
	return true;
}
*/

//-----------------------------
// Streaming Mode vs. Poll Mode
//-----------------------------

bool VistaCyberGloveDriver::SetStreamingMode(bool bMode)
{
	// Only store value if init has not been finished yet
	if(!m_bInitDone)
	{
		m_bTempStreamingMode = bMode;
		return true;
	}

	// Enter Streaming Mode
	if(bMode)
	{
		// If not already in Streaming Mode
		if(!m_bStreamingMode)
		{
#ifdef WIN32
			_preCommunicate();
#endif
			// Start Streaming
			m_bStreamingMode = VistaCyberGloveProtocol::CmdStartStreamingMode(m_pConnectionAspect);
#ifdef WIN32
			_postCommunicate();
#endif

#ifdef _DEBUG
			if(!m_bStreamingMode)
				printf("[CyberGlove] Unable to enter Streaming Mode\n");
#endif
		}
		else
		{
#ifdef _DEBUG
			printf("[CyberGlove] Already in Streaming Mode\n");
#endif
		}
	}
	// Set Poll Mode
	else
	{
		// Only proceed if in Streaming Mode
		if(m_bStreamingMode)
		{
			bool noError = VistaCyberGloveProtocol::CmdEndStreamingMode(m_pConnectionAspect);
			if(noError)
			{
				m_bStreamingMode = false;

				// flush pending data
				VistaConnection* pCon = m_pConnectionAspect->GetConnection(0);
				pCon->SetIsBlocking(false);
				char c;
				while( pCon->ReadRawBuffer( &c, 1 ) > 0 ); // semicolon on purpose
				pCon->SetIsBlocking(true);
			}
		}
		else
		{
#ifdef _DEBUG
			printf("[CyberGlove] Not in Streaming Mode\n");
#endif
		}
	}

	return true;
}

bool VistaCyberGloveDriver::DoSensorUpdate( VistaType::microtime nTs )
{
#ifdef WIN32
	_preCommunicate();
#endif

	// first of all, indicate the beginning of a sampling
	// for the sensor with index 0 at time nTs
	MeasureStart(0, nTs);

	// get the current place for the decoding for sensor 0
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(0));

	// for the sake of readability: interpret the memory blob
	// as a place to store a sample for the CyberGlove
	VistaCyberGloveCommonShare::sCyberGloveSample *s = pM->getWrite< VistaCyberGloveCommonShare::sCyberGloveSample >();

	int nRecordSize = -1;
	if(!m_bStreamingMode)
	{
		nRecordSize = VistaCyberGloveProtocol::CmdGetSingleRecord(s, m_pConnectionAspect);
	}
	else
	{
		nRecordSize = m_pConnectionAspect->GetTerminatedCommand(0,s,~0,0);
	}

	bool bDataReceived = nRecordSize > 0;
	if( bDataReceived )
	{
		// valid sensor sample?
		if ( s->m_cRecord[0] == 'S' || s->m_cRecord[0] == 'G' )
		{
			// In case of an error the returned data packet contains the char string 'eg' or 'es' before the trailing nul
			if (s->m_cRecord[nRecordSize - 2] == 'e')
			{
				char cErrorCode = s->m_cRecord[nRecordSize - 1];
				if (cErrorCode == 'g')
				{
					vstr::warnp() << "[VistaCyberGlove] Glove not plugged in." << endl;
					s->m_eErrorCode = VistaCyberGloveCommonShare::VCG_ERROR_GLOVE;
					return false;
				}
				else if (cErrorCode == 's')
				{
					vstr::warnp() << "[VistaCyberGlove] Sampling rate too fast." << endl;
					s->m_eErrorCode = VistaCyberGloveCommonShare::VCG_ERROR_SAMPLING;
					return false;
				}
			}

			// Extract Data from Packet
			// Packet Format: 'G', s1, s2, ..., sn, timestamp, status, 0

			// Calculate number of sensor values in data packet
			// Depends on the sensor mask (M) and the number of returned sensors (N)
			unsigned int nSensorsInPacket = (int) strlen(s->m_cRecord)-1;

			// CyberGlove status -> 1 extra byte
			if(m_bIncStatusByte)
			{
				--nSensorsInPacket;
			}

			// timestamp -> 5 extra bytes
			if(m_bIncTimestamp)
			{
				nSensorsInPacket -= 5;
			}

#ifdef _TALKATIVE
			// sensor values

			cout << "\r"; // invoke a carriage return
			for(unsigned int nSensor = 0 ; nSensor < nSensorsInPacket ; ++nSensor)
			{
				cout << (unsigned int)((unsigned char) s->m_cRecord[nSensor + 1]) << " ";
			}
			cout.flush();
#endif
			// store additional sample information:

			// sensors in sample
			s->m_nSensorsInSample = nSensorsInPacket;
			// sensor mask
			s->m_nSensorMask = m_nSensorMask;
			// include timestamp ?
			s->m_bIncTimestamp = m_bIncTimestamp;
			// include status byte ?
			s->m_bIncStatusByte = m_bIncStatusByte;
		}
		else
		{
			s->m_eErrorCode = VistaCyberGloveCommonShare::VCG_ERROR_UNKNOWN;
#ifdef _DEBUG
			printf("[CyberGlove] Invalid data packet (Command %c)\n", s->m_cRecord[0]);
#endif
		}

	}

	// we are done. Indicate that to the history
	MeasureStop(0);

#ifdef WIN32
	_postCommunicate();
#endif

	return true;
}

#ifdef WIN32
void VistaCyberGloveDriver::_preCommunicate() const
{
	// Before we communicate with the CyberGlove we have to enter blocking mode.
	m_pConnectionAspect->GetConnection(0)->SetIsBlocking(true);
}

void VistaCyberGloveDriver::_postCommunicate() const
{
	// After having communicated with the CyberGlove, we have to enter non-blocking mode.
	VistaConnection* pCon = m_pConnectionAspect->GetConnection(0);
	pCon->SetIsBlocking(false);
	pCon->WaitForIncomingData();
}

#endif



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/



