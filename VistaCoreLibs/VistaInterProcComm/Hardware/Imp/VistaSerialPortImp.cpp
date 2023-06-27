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

#include "VistaSerialPortImp.h"

#if defined(WIN32)
#include "VistaWin32SerialPortImp.h"
#else
#include "VistaPosixSerialPortImp.h"
#endif

#include <cassert>
#include <string>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSerialPortImp::VistaSerialPortImp() {
  m_ePortIndex = VistaSerialPortStateDesc::CSP_NA;

  m_ePortSpeed    = VistaSerialPortStateDesc::CSSP_9600;
  m_ePortParity   = VistaSerialPortStateDesc::CSPA_NONE;
  m_ePortDataBits = VistaSerialPortStateDesc::CSD_8;
  m_ePortStopBits = VistaSerialPortStateDesc::CSST_1;

  m_bIsHandshaking = true;
  m_bIsxOnxOff     = false;

  m_cXonChar  = 021; // this is POSIX standard
  m_cXoffChar = 023; // this is POSIX standard

  m_cErrorChar = m_cEofChar = 0x00;

  AddPortName("UNDEFINED");         // index 0
  m_sPortName   = m_vePortNames[0]; // set port-name to "UNDEFINED"
  m_bIsOpen     = false;
  m_bIsBlocking = false;
}

VistaSerialPortImp::~VistaSerialPortImp() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaSerialPortImp::SetPortByName(const string& sPortName) {
  VistaSerialPortStateDesc::VistaPortIndex eIndex = VistaSerialPortStateDesc::CSP_NA;
  if ((eIndex = HasPortName(sPortName)) != VistaSerialPortStateDesc::CSP_NA) {
    m_ePortIndex = eIndex; // this is legal, we do not want to call SetPortByIndex() here.
  } else {
    // ok, we do not have this port, but maybe the default values do not match
    // the OSes name for that port, so we will not set an index, but set a
    // name on user-demand
    m_ePortIndex = VistaSerialPortStateDesc::CSP_USER;
  }

  m_sPortName = sPortName;
  return SetHardwareState();
}

void VistaSerialPortImp::AddPortName(const string& sPortName) {
  if (m_vePortNames.size() < VistaSerialPortStateDesc::CSP_MAX)
    m_vePortNames.push_back(sPortName); // do not check for duplicates
}

VistaSerialPortStateDesc::VistaPortIndex VistaSerialPortImp::HasPortName(
    const string& sPortName) const {
  for (unsigned int i = 0; i < m_vePortNames.size(); ++i) {

    if (m_vePortNames[i] == sPortName)
      return (VistaSerialPortStateDesc::VistaPortIndex)i;
  }

  return VistaSerialPortStateDesc::CSP_NA;
}

string VistaSerialPortImp::GetPortName() const {
  return m_sPortName;
}

bool VistaSerialPortImp::SetPortByIndex(VistaSerialPortStateDesc::VistaPortIndex iIndex) {
  assert(m_vePortNames.size() == 5); // four port names + UNDEFINED
  switch (iIndex) {
  case VistaSerialPortStateDesc::CSP_1:
  case VistaSerialPortStateDesc::CSP_2:
  case VistaSerialPortStateDesc::CSP_3:
  case VistaSerialPortStateDesc::CSP_4: {
    m_ePortIndex = (VistaSerialPortStateDesc::VistaPortIndex)iIndex;
    m_sPortName  = m_vePortNames[iIndex];
    break;
  }
  case VistaSerialPortStateDesc::CSP_USER: {
    m_ePortIndex = (VistaSerialPortStateDesc::VistaPortIndex)
        iIndex; // user is different, but not N/A! so simply set this without changing the name
    break;
  }
  default: {
    m_ePortIndex = VistaSerialPortStateDesc::CSP_NA;
    m_sPortName  = m_vePortNames[0]; // use "UNDEFINED"
    return false;
  }
  }

  return SetHardwareState();
}

VistaSerialPortStateDesc::VistaPortIndex VistaSerialPortImp::GetPortIndex() const {
  return m_ePortIndex;
}

bool VistaSerialPortImp::SetSpeed(VistaSerialPortStateDesc::VistaSpeed eSpeed) {
  m_ePortSpeed = eSpeed;
  return SetHardwareState();
}

VistaSerialPortStateDesc::VistaSpeed VistaSerialPortImp::GetSpeed() const {
  return m_ePortSpeed;
}

unsigned long VistaSerialPortImp::GetMaximumSpeed() const {
  return VistaSerialPortStateDesc::CSSP_115200; // this can be overridden by subclass
}

bool VistaSerialPortImp::SetParity(VistaSerialPortStateDesc::VistaParity eParam) {
  m_ePortParity = eParam;
  return SetHardwareState();
}

VistaSerialPortStateDesc::VistaParity VistaSerialPortImp::GetParity() const {
  return m_ePortParity;
}

bool VistaSerialPortImp::SetDataBits(VistaSerialPortStateDesc::VistaDataBits eParam) {
  m_ePortDataBits = eParam;
  return SetHardwareState();
}

VistaSerialPortStateDesc::VistaDataBits VistaSerialPortImp::GetDataBits() const {
  return m_ePortDataBits;
}

bool VistaSerialPortImp::SetStopBits(VistaSerialPortStateDesc::VistaStopBits eParam) {
  m_ePortStopBits = eParam;
  return SetHardwareState();
}

VistaSerialPortStateDesc::VistaStopBits VistaSerialPortImp::GetStopBits() const {
  return m_ePortStopBits;
}

bool VistaSerialPortImp::SetHardwareFlow(bool bInHandshaking) {
  m_bIsHandshaking = bInHandshaking;
  return SetHardwareState();
}

bool VistaSerialPortImp::GetIsHardwareFlow() const {
  return m_bIsHandshaking;
}

bool VistaSerialPortImp::SetSoftwareFlow(bool bParam) {
  m_bIsxOnxOff = bParam;
  return SetHardwareState();
}

bool VistaSerialPortImp::GetIsSoftwareFlow() const {
  return m_bIsxOnxOff;
}

bool VistaSerialPortImp::SetSoftwareFlowOnOffChars(char cXonChar, char cXoffChar) {
  m_cXonChar  = cXonChar;
  m_cXoffChar = cXoffChar;
  return SetHardwareState();
}

char VistaSerialPortImp::GetXonChar() const {
  return m_cXonChar;
}

char VistaSerialPortImp::GetXoffChar() const {
  return m_cXoffChar;
}

bool VistaSerialPortImp::SetCharForParityError(char cErrorChar) {
  m_cErrorChar = cErrorChar;
  return SetHardwareState();
}

char VistaSerialPortImp::GetCharForParityError() const {
  return m_cErrorChar;
}

char VistaSerialPortImp::GetEofChar() const {
  return m_cEofChar;
}

bool VistaSerialPortImp::SetEofChar(char cEofChar) {
  m_cEofChar = cEofChar;
  return SetHardwareState();
}

VistaSerialPortImp* VistaSerialPortImp::CreateSerialPortImp() {
#if defined(WIN32)
  return new VistaWin32SerialPortImp;
#else
  return new VistaPosixSerialPortImp;
#endif
}

void VistaSerialPortImp::DestroySerialPortImp(VistaSerialPortImp* pImp) {
  if (pImp->IsOpen())
    pImp->CloseSerialPort();
  delete pImp;
}

bool VistaSerialPortImp::IsOpen() const {
  return m_bIsOpen;
}

void VistaSerialPortImp::SetIsOpen(bool bIsOpen) {
  m_bIsOpen = bIsOpen;
}

void VistaSerialPortImp::SetIsBlocking(bool bBlocking) {
  m_bIsBlocking = bBlocking;
}

bool VistaSerialPortImp::GetIsBlocking() const {
  return m_bIsBlocking;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
