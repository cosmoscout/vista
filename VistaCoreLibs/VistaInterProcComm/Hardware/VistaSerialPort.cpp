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

#include "VistaSerialPort.h"
#include "Imp/VistaSerialPortImp.h"

#include <string>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaSerialPort::VistaSerialPort() {
  m_pImp = VistaSerialPortImp::CreateSerialPortImp();
}

VistaSerialPort::VistaSerialPort(const string& sPortName) {
  m_pImp = VistaSerialPortImp::CreateSerialPortImp();
  SetPortByName(sPortName);
}

VistaSerialPort::~VistaSerialPort() {
  CloseSerialPort();
  VistaSerialPortImp::DestroySerialPortImp(m_pImp);
  m_pImp = (VistaSerialPortImp*)0xDEADBEEF;
}

bool VistaSerialPort::OpenSerialPort() {
  return m_pImp->OpenSerialPort();
}

bool VistaSerialPort::CloseSerialPort() {
  return m_pImp->CloseSerialPort();
}

int VistaSerialPort::Receive(void* buffer, const int length, int iTimeout) {
  return m_pImp->Receive(buffer, length, iTimeout);
}

int VistaSerialPort::Send(const void* buffer, const int length) {
  return m_pImp->Send(buffer, length);
}

bool VistaSerialPort::SetPortByName(const string& sPortName) {
  return m_pImp->SetPortByName(sPortName);
}

string VistaSerialPort::GetPortName() const {
  return m_pImp->GetPortName();
}

bool VistaSerialPort::SetPortByIndex(int iIndex) {
  return m_pImp->SetPortByIndex((VistaSerialPortStateDesc::VistaPortIndex)iIndex);
}

int VistaSerialPort::GetPortIndex() const {
  return (int)m_pImp->GetPortIndex();
}

bool VistaSerialPort::SetSpeed(int eSpeed) {
  return m_pImp->SetSpeed((VistaSerialPortStateDesc::VistaSpeed)eSpeed);
}

int VistaSerialPort::GetSpeed() const {
  return m_pImp->GetSpeed();
}

unsigned long VistaSerialPort::GetMaximumSpeed() const {
  return m_pImp->GetMaximumSpeed();
}

bool VistaSerialPort::SetParity(int eParam) {
  return m_pImp->SetParity((VistaSerialPortStateDesc::VistaParity)eParam);
}

int VistaSerialPort::GetParity() const {
  return m_pImp->GetParity();
}

bool VistaSerialPort::SetDataBits(int eParam) {
  return m_pImp->SetDataBits((VistaSerialPortStateDesc::VistaDataBits)eParam);
}

int VistaSerialPort::GetDataBits() const {
  return (int)m_pImp->GetDataBits();
}

bool VistaSerialPort::SetStopBits(float eParam) {
  if (eParam == 1.5f)
    return m_pImp->SetStopBits(VistaSerialPortStateDesc::CSST_15);
  else
    return m_pImp->SetStopBits((VistaSerialPortStateDesc::VistaStopBits)((int)eParam));
}

float VistaSerialPort::GetStopBits() const {
  VistaSerialPortStateDesc::VistaStopBits e = m_pImp->GetStopBits();
  if (e == VistaSerialPortStateDesc::CSST_15)
    return 1.5;

  return (float)e;
}

bool VistaSerialPort::SetHardwareFlow(bool bInHandshaking) {
  return m_pImp->SetHardwareFlow(bInHandshaking);
}

bool VistaSerialPort::GetIsHardwareFlow() const {
  return m_pImp->GetIsHardwareFlow();
}

bool VistaSerialPort::SetSoftwareFlow(bool bParam) {
  return m_pImp->SetSoftwareFlow(bParam);
}

bool VistaSerialPort::GetIsSoftwareFlow() const {
  return m_pImp->GetIsSoftwareFlow();
}

bool VistaSerialPort::SetBlockingMode(
    unsigned long inReadInterval, unsigned long inReadMultiplyer, unsigned long inReadConstant) {
  return m_pImp->SetBlockingMode(inReadInterval, inReadMultiplyer, inReadConstant);
}

unsigned long VistaSerialPort::WaitForIncomingData(int timeout) {
  return m_pImp->WaitForIncomingData(timeout);
}

unsigned long VistaSerialPort::PendingDataSize() const {
  return m_pImp->PendingDataSize();
}

void VistaSerialPort::SetIsBlocking(bool bBlocking) {
  m_pImp->SetIsBlocking(bBlocking);
}

bool VistaSerialPort::GetIsBlocking() const {
  return m_pImp->GetIsBlocking();
}

HANDLE VistaSerialPort::GetDescriptor() const {
  return m_pImp->GetDescriptor();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
