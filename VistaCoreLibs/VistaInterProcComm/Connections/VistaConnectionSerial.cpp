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


#include "VistaConnectionSerial.h"

#include <VistaInterProcComm/Hardware/VistaSerialPort.h>
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
VistaConnectionSerial::VistaConnectionSerial()
{
	m_pSerialPort = new VistaSerialPort;
}

VistaConnectionSerial::VistaConnectionSerial(const string &sPortName)
{
	m_pSerialPort = new VistaSerialPort(sPortName);
}


VistaConnectionSerial::~VistaConnectionSerial()
{
	Close();
	delete m_pSerialPort;
	m_pSerialPort = (VistaSerialPort*)0xDEADBEEF;
}

bool VistaConnectionSerial::Open()
{
	SetIsOpen( m_pSerialPort->OpenSerialPort() );
	return GetIsOpen();
}

void VistaConnectionSerial::Close(  )
{
	SetIsOpen(!m_pSerialPort->CloseSerialPort());
}

int VistaConnectionSerial::Receive(void *buffer, const int length, int iTimeout)
{
	return m_pSerialPort->Receive(buffer, length, iTimeout);
}

int VistaConnectionSerial::Send(const void *buffer, const int length)
{
	return m_pSerialPort->Send(buffer, length);
}

bool VistaConnectionSerial::SetPortByName(const string &sPortName)
{
	return m_pSerialPort->SetPortByName(sPortName);
}

string   VistaConnectionSerial::GetPortName() const
{
	return m_pSerialPort->GetPortName();
}

bool          VistaConnectionSerial::SetPortByIndex(int iIndex)
{
	return m_pSerialPort->SetPortByIndex(iIndex);
}

int           VistaConnectionSerial::GetPortIndex() const
{
	return m_pSerialPort->GetPortIndex();
}


bool          VistaConnectionSerial::SetSpeed( int eSpeed)
{
	return m_pSerialPort->SetSpeed(eSpeed);
}

int           VistaConnectionSerial::GetSpeed() const
{
	return m_pSerialPort->GetSpeed();
}

unsigned long VistaConnectionSerial::GetMaximumSpeed () const
{
	return m_pSerialPort->GetMaximumSpeed();
}

bool          VistaConnectionSerial::SetParity ( int eParam )
{
	return m_pSerialPort->SetParity( eParam );
}

int           VistaConnectionSerial::GetParity () const
{
	return m_pSerialPort->GetParity();
}

bool          VistaConnectionSerial::SetDataBits ( int eParam )
{
	return m_pSerialPort->SetDataBits( eParam);
}

int           VistaConnectionSerial::GetDataBits () const
{
	return m_pSerialPort->GetDataBits();
}

bool          VistaConnectionSerial::SetStopBits ( float eParam )
{
	if(eParam == 1.5f)
		return m_pSerialPort->SetStopBits(15);
	else
		return m_pSerialPort->SetStopBits((float)((int)eParam));
}


float         VistaConnectionSerial::GetStopBits () const
{
	return m_pSerialPort->GetStopBits();
}

bool          VistaConnectionSerial::SetHardwareFlow( bool bInHandshaking )
{
	return m_pSerialPort->SetHardwareFlow(bInHandshaking);
}

bool VistaConnectionSerial::GetIsHardwareFlow() const
{
	return m_pSerialPort->GetIsHardwareFlow();
}

bool          VistaConnectionSerial::SetSoftwareFlow ( bool bParam )
{
	return m_pSerialPort->SetSoftwareFlow(bParam);
}

bool          VistaConnectionSerial::GetIsSoftwareFlow () const
{
	return m_pSerialPort->GetIsSoftwareFlow();
}

bool          VistaConnectionSerial::SetBlockingMode ( unsigned long inReadInterval, unsigned long inReadMultiplyer, unsigned long inReadConstant )
{
	return m_pSerialPort->SetBlockingMode(inReadInterval, inReadMultiplyer, inReadConstant);
}

unsigned long VistaConnectionSerial::WaitForIncomingData(int timeout)
{
	return m_pSerialPort->WaitForIncomingData(timeout);
}

bool VistaConnectionSerial::HasPendingData() const
{
	return (m_pSerialPort->PendingDataSize() > 0);
}


unsigned long VistaConnectionSerial::PendingDataSize() const
{
	return m_pSerialPort->PendingDataSize();
}

void VistaConnectionSerial::SetIsBlocking(bool bBlocking)
{
	m_pSerialPort->SetIsBlocking(bBlocking);
}

bool VistaConnectionSerial::GetIsBlocking() const
{
	return m_pSerialPort->GetIsBlocking();
}

HANDLE VistaConnectionSerial::GetConnectionDescriptor() const
{
	return m_pSerialPort->GetDescriptor();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


