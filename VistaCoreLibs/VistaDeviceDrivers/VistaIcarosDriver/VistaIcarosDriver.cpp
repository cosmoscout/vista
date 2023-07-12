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

// Header to implement FTDI driver functionalities
#include <ftd2xx.h>

#include <map>
#include <set>
#include <string>
#include <thread>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimerImp.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include "VistaIcarosDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace {
const char* g_sLogPrefix = "[ICAROS] ";
}

struct VistaIcarosController::_sPrivate {
  _sPrivate()
      : m_nKeyStates(0)
      , m_bLastZero(false)
      , m_pWorkspace(NULL) {
    m_nOldKeys[0] = 0;
    m_nOldKeys[1] = 0;
    m_nOldKeys[2] = 0;
    m_nOldKeys[3] = 0;
  }

  __int64   m_nKeyStates;
  _sMeasure m_measure;
  bool      m_bLastZero;

  long                        m_nOldKeys[4];
  VistaDriverWorkspaceAspect* m_pWorkspace;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaIcarosController::VistaIcarosController(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pPrivate(new _sPrivate) {
  SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);

  VistaDeviceSensor* pSensor = new VistaDeviceSensor;
  AddDeviceSensor(pSensor);
  pSensor->SetTypeHint("");
  pSensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder());

  VistaBoundingBox bb;

  bb.m_v3Min[0] = bb.m_v3Min[1] = bb.m_v3Min[2] = -400;
  bb.m_v3Max[0] = bb.m_v3Max[1] = bb.m_v3Max[2] = 400;

  m_pPrivate->m_pWorkspace = new VistaDriverWorkspaceAspect;
  m_pPrivate->m_pWorkspace->SetWorkspace("ICAROS", bb);

  RegisterAspect(m_pPrivate->m_pWorkspace);
}

VistaIcarosController::~VistaIcarosController() {
  VistaDeviceSensor*      pSensor = GetSensorByIndex(0);
  IVistaMeasureTranscode* pTr     = pSensor->GetMeasureTranscode();
  pSensor->SetMeasureTranscode(NULL);
  GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(pTr);

  RemDeviceSensor(pSensor);
  delete pSensor;

  if (GetIsConnected())
    Disconnect();

  UnregisterAspect(m_pPrivate->m_pWorkspace, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pPrivate->m_pWorkspace;

  delete m_pPrivate;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaIcarosController::DoConnect() {
	std::thread mThread([&](){
	FT_STATUS 	res;    	//Result handling of FTDI calls
	DCB 		dcb;		//Communication paramters (baudrate, etc.)
	char 		COMx[5];	//

	/********************************************************/
	// Send an "ICROS" string to all devices and see if one answers. Use that COMPort then
	/********************************************************/
	//TODO: Find the ICAROS device
	int COMPORT = 6;

	/********************************************************/
	// Open the com port assigned to your device
	/********************************************************/		
	int n = sprintf(COMx, "COM%d",COMPORT);

		hCommPort = CreateFile(
		COMx,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (hCommPort == INVALID_HANDLE_VALUE) 
	{
		vstr::err() << "[IcarosDriver] IcarosDriver: failed to open com port! Error code: "  << GetLastError() << std::endl;
		return false;
	}

	/********************************************************/
	// Configure the UART interface parameters
	/********************************************************/
	res = GetCommState(hCommPort, &dcb);
	if (!res) 
	{
		vstr::err() << "[IcarosDriver] GetCommStateFailed! Error code: " << GetLastError() << std::endl;
		return false;
	}

	//Configure communication parameters
	dcb.BaudRate 	= 115200;
	dcb.ByteSize 	= 8;
	dcb.Parity 		= NOPARITY;
	dcb.StopBits 	= ONESTOPBIT;
	dcb.fDtrControl = FT_FLOW_RTS_CTS;
	dcb.XonChar 	= 0x11;
	dcb.XoffChar 	= 0x13;

	res = SetCommState(hCommPort, &dcb);
	if (!res)
	{
		vstr::err() << "[IcarosDriver] SetCommState failed! Error code: " << GetLastError() << std::endl;
		return false;
	}
	vstr::outi() << "[IcarosDriver] Port configured successfull " << std::endl;

	/********************************************************/
	// Send the start sensoring command to ICAROS
	/********************************************************/
	DWORD dwwritten = 0;
	std::string startCMD = "START\r\n";
	res = WriteFile(hCommPort, startCMD.c_str(), startCMD.length(), &dwwritten, NULL);
	if (!res)
	{
		vstr::err() << "[IcarosDriver] WriteFile failed! Error code: " << GetLastError() << std::endl;
		return false;
	}
	vstr::outi() << "[IcarosDriver] Start command send to device. The device should now sends the sensor updates ... " << std::endl;

	/********************************************************/
	// Reading sensor values from ICAROS
	/********************************************************/
  	char buf[20];
	DWORD dwRead;
	memset(buf,0,20);
	
	while(readSensor)
	{
		if (ReadFile(hCommPort, buf, sizeof(buf), &dwRead, NULL))
		{
			float* current = (float*) &buf[2];

			m_pPrivate->m_measure.m_nKeys[0] = (buf[1] & 1) > 0;
			m_pPrivate->m_measure.m_nKeys[1] = (buf[1] & 2) > 0;
			m_pPrivate->m_measure.m_nKeys[2] = (buf[1] & 4) > 0;
			m_pPrivate->m_measure.m_nKeys[3] = (buf[1] & 8) > 0;

			m_pPrivate->m_measure.m_fQuat1 = *current; ++current;
			m_pPrivate->m_measure.m_fQuat2 = *current; ++current;
			m_pPrivate->m_measure.m_fQuat3 = *current; ++current;
			m_pPrivate->m_measure.m_fQuat4 = *current; ++current;
		}
	}
	});
	mThread.detach();
  	return true;
}

bool VistaIcarosController::DoDisconnect() {
  readSensor = false;
  CloseHandle(hCommPort);
  vstr::outi() << "[IcarosDriver] Device disconnected " << GetLastError() << std::endl;
  return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaIcarosCreateMethod::VistaIcarosCreateMethod(IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {

  // we assume an update rate of 60Hz at max.
  RegisterSensorType(
      "", sizeof(VistaIcarosController::_sMeasure), 30, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaIcarosCreateMethod::CreateDriver() {
  return new VistaIcarosController(this);
}

bool VistaIcarosController::DoSensorUpdate(VistaType::microtime dTs) {
	
	
    VistaSensorMeasure* pM = MeasureStart(0, dTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
    if (pM == NULL) {
      MeasureStop(0);
      return false;
    }

    _sMeasure* m = pM->getWrite<_sMeasure>();
    memcpy(m, &m_pPrivate->m_measure, sizeof(_sMeasure));

    MeasureStop(0);
  	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
