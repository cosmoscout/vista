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

#include "VistaSpaceMouseDriver.h"
#include "VistaSpaceMouseCommonShare.h"

#include "VistaDeviceDriversBase/VistaDeviceSensor.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <cassert>
#include <cstring>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
class VistaSpaceMouseDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence {
 public:
  virtual bool operator()(VistaConnection* pCon) {
    pCon->Close();
    return !pCon->GetIsOpen();
  }
};

class VistaSpaceMouseAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence {
 public:
  VistaSpaceMouseAttachSequence(VistaSpaceMouseDriver* pDriver)
      : m_pDriver(pDriver) {
  }

  virtual bool operator()(VistaConnection* pCon) {
    if (pCon->GetIsOpen())
      return true;

    if (pCon->Open()) {
      return true;
    }
    return false;
  }

  VistaSpaceMouseDriver* m_pDriver;
};

typedef TVistaDriverEnableAspect<VistaSpaceMouseDriver> VistaSpaceMouseEnableAspect;

namespace {
static char SCodeTable[16] = {
    '0', 'A', 'B', '3', 'D', '5', '6', 'G', 'H', '9', ':', 'K', '<', 'M', 'N', '?'};
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaSpaceMouseDriver::VistaSpaceMouseDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pConnection(new VistaDriverConnectionAspect)
    , m_nVecButtonStates(9) {
  RegisterAspect(new VistaSpaceMouseEnableAspect(this, &VistaSpaceMouseDriver::PhysicalEnable));

  SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);
  VistaDeviceSensor* pSen = new VistaDeviceSensor;

  //	pSen->SetMeasureTranscode(new VistaSpaceMouseMeasureTranscode);
  pSen->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder());
  pSen->SetTypeHint("");

  AddDeviceSensor(pSen);

  // m_pHistoryAspect->RegisterSensor(pSen);
  // m_pHistoryAspect->SetHistorySize(pSen, 5, 17);

  // register connection aspect
  m_pConnection->SetConnection(
      0, NULL, "MAIN", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT);

  m_pConnection->SetDetachSequence(0, new VistaSpaceMouseDetachSequence);
  m_pConnection->SetAttachSequence(0, new VistaSpaceMouseAttachSequence(this));

  RegisterAspect(m_pConnection);
}

VistaSpaceMouseDriver::~VistaSpaceMouseDriver() {
  VistaDeviceSensor* pSen = GetSensorByIndex(0);
  m_pHistoryAspect->UnregisterSensor(pSen);

  // VistaSpaceMouseMeasureTranscode *pTC
  //	= dynamic_cast<VistaSpaceMouseMeasureTranscode*>(pSen->GetMeasureTranscode());
  // delete pTC;
  pSen->SetMeasureTranscode(NULL);

  RemDeviceSensor(pSen);
  delete pSen;

  UnregisterAspect(m_pConnection, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pConnection;

  VistaDriverEnableAspect* enabler =
      GetAspectAs<VistaDriverEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;
}

bool VistaSpaceMouseDriver::DoSensorUpdate(VistaType::microtime dTs) {
  char ucRead[8192];
  bool bTimeout = false;
  std::memset(ucRead, 0, 8192);

  // std::cout << "pending: " << m_pConnection->GetConnection(0)->PendingDataSize() << std::endl;
  // we try to read a packet of max. 8k until an endline is coming
  // with a timeout of 5secs, otherwise, the spacemouse might block
  // until moved.
  int nRead = m_pConnection->GetTerminatedCommand(0, ucRead, 8192, 0x0d, 5000, &bTimeout);

  if (bTimeout)
    return false; // hmm... what about the global state of this read?

  if (nRead == -1)
    return false; // something is wrong

  if (nRead == 0) // blocking read of 0, can indicate another form of timeout?
    return false;

  ucRead[nRead - 1] = '\0';

  // uncomment the following iff you like to see what was
  // received
  // cout << "c: " << ucRead << endl;

  float fRawRotAxis[3] = {0, 0, 0};

  switch (ucRead[0]) {
  case 'd': {
    float fCoords[6];

    fCoords[0] = BuildCoordinate((unsigned char*)&ucRead[1]); // Build X / Translation
    fCoords[1] = BuildCoordinate((unsigned char*)&ucRead[5]); // Build Y / Translation
    fCoords[2] = BuildCoordinate((unsigned char*)&ucRead[9]); // Build Z / Translation

    fRawRotAxis[0] = BuildCoordinate((unsigned char*)&ucRead[13]); // Build X / Rotation
    fRawRotAxis[1] = BuildCoordinate((unsigned char*)&ucRead[17]); // Build Y / Rotation
    fRawRotAxis[2] = BuildCoordinate((unsigned char*)&ucRead[21]); // Build Z / Rotation

    fCoords[3] = fRawRotAxis[0] * 180.0f / 500.0f;
    fCoords[4] = fRawRotAxis[1] * 180.0f / 500.0f;
    fCoords[5] = fRawRotAxis[2] * 180.0f / 500.0f;

    m_v3Pos[0] = fCoords[0];
    m_v3Pos[1] = fCoords[1];
    m_v3Pos[2] = fCoords[2];

    m_qOri = VistaQuaternion(VistaEulerAngles((Vista::Pi / 180) * (fCoords[3]),
        (Vista::Pi / 180) * (fCoords[4]), (Vista::Pi / 180) * (fCoords[5])));
    break;
  }

  case 'k': {
    int iButtons = DecodeValue(ucRead[1]);    // Buttons 1-4
    iButtons += DecodeValue(ucRead[2]) * 16;  // Buttons 5-8
    iButtons += DecodeValue(ucRead[3]) * 256; // Button *

    for (unsigned int n = 0; n < 9; ++n) {
      m_nVecButtonStates[n] = (iButtons & (1 << n) ? 1.0 : 0.0);
    }
    break;
  }
  default: // Unknown Event
    break;
  }

  VistaDeviceSensor* pSen = GetSensorByIndex(0);
  m_pHistoryAspect->MeasureStart(pSen, dTs);
  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSen);
  assert(pM);

  VistaSpaceMouseMeasures::sSpaceMouseMeasure* m =
      pM->getWrite<VistaSpaceMouseMeasures::sSpaceMouseMeasure>();

  m_v3Pos.GetValues((*m).m_nPos);
  m_qOri.GetValues((*m).m_nOri);
  (*m).m_nRotAx[0] = fRawRotAxis[0];
  (*m).m_nRotAx[1] = fRawRotAxis[1];
  (*m).m_nRotAx[2] = fRawRotAxis[2];
  (*m).m_nRotAx[3] = 1.0f; // homogenous coordinate

  memcpy((*m).m_nButtonStates, &m_nVecButtonStates[0], 9 * sizeof(double));

  m_pHistoryAspect->MeasureStop(pSen);

  return true;
}

bool VistaSpaceMouseDriver::PostUpdate() {
#if defined(WIN32)
  m_pConnection->GetConnection()->WaitForIncomingData();
#endif
  return true;
}

// ########################################################################
// COMMANDTABLE
// ########################################################################

bool VistaSpaceMouseDriver::CmdBeep(int nDuration) {
  unsigned char cCommand[] = "bX\r";
  cCommand[1]              = EncodeValue(nDuration + 8); // Encode Number into Magellan Format
  if (m_pConnection->SendCommand(0, cCommand, 3, 0)) {
    char ret[256];
    return (m_pConnection->GetTerminatedCommand(0, ret, 256, '\r') > 0);
  }
  return false;
}

bool VistaSpaceMouseDriver::CmdKeyboard() {
  unsigned char cCommand[] = "kQ\r"; // Command sequence
  return m_pConnection->SendCommand(0, cCommand, 3, 300);
}

bool VistaSpaceMouseDriver::CmdSetMode(int nDominant, int nTranslation, int nRotation) {
  unsigned char cCommand[] = "mX\r";
  int           iValue     = nDominant * 4;

  iValue += nTranslation * 2;
  iValue += nRotation; // Build nibble1

  cCommand[1] = EncodeValue(iValue);

  return m_pConnection->SendCommand(0, cCommand, 3, 300); // Send Command
}

bool VistaSpaceMouseDriver::CmdRequestData() {
  unsigned char cCommand[] = "dQ\r";
  return m_pConnection->SendCommand(0, cCommand, 3, 300);
}

bool VistaSpaceMouseDriver::CmdSetNullRadius(int nRadius) {
  unsigned char cCommand[] = "nX\r";
  cCommand[1]              = EncodeValue(nRadius);
  return m_pConnection->SendCommand(0, cCommand, 3, 100);
}

bool VistaSpaceMouseDriver::CmdReset(void) {
  unsigned char cResetCommand[] = "\rz\rz\r";
  return m_pConnection->SendCommand(
      0, cResetCommand, (unsigned int)strlen((char*)cResetCommand), 300);
}

bool VistaSpaceMouseDriver::CmdGetDeviceName(std::string& sName) {
  unsigned char cDeviceCommand[] = "\r\rvQ\r";
  bool          bResult          = m_pConnection->SendCommand(
      0, cDeviceCommand, (unsigned int)strlen((char*)cDeviceCommand), 1000);

  if (!bResult)
    return false;

  unsigned char ucName[256];

  int nRead = m_pConnection->GetTerminatedCommand(0, ucName, 256, '\r'); // GetCommand(ucName,256);

  if (nRead > 0) {
    ucName[nRead - 1] = '\0';
    sName.assign((const char*)&ucName[0], nRead);
    return true;
  }

  return false;
}

// ########################################################################

unsigned char VistaSpaceMouseDriver::EncodeValue(unsigned int nValue) {
  if (nValue > 15) // Value out of bounds
    return 0;

  return SCodeTable[nValue];
}

unsigned int VistaSpaceMouseDriver::DecodeValue(unsigned char cKey) {
  for (unsigned int i = 0; i < 16; ++i) {
    if (SCodeTable[i] == cKey)
      return i;
  }
  return 0;
}

float VistaSpaceMouseDriver::BuildCoordinate(unsigned char* cBuffer) {
  int nResult = DecodeValue(cBuffer[0]) * 4096 + DecodeValue(cBuffer[1]) * 256 +
                DecodeValue(cBuffer[2]) * 16 + DecodeValue(cBuffer[3]) * 1 - 32768;

  return float(nResult);
}

bool VistaSpaceMouseDriver::PhysicalEnable(bool bEnable) {
  if (bEnable) {

    if (m_pConnection->GetConnection()) {
      CmdBeep(3);
      CmdSetMode(true, true, true);
      m_pConnection->GetConnection()->WaitForSendFinish();
      // request data block in non-blocking state
      m_pConnection->GetConnection()->SetIsBlocking(false);

#if defined(WIN32)
      m_pConnection->GetConnection()->WaitForIncomingData();
#endif
      return true;
    }
    return false;
  }
  return true;
}

bool VistaSpaceMouseDriver::DoConnect() {
  return true;
}

bool VistaSpaceMouseDriver::DoDisconnect() {
  return true;
}
