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


#ifndef _VISTACONNECTIONSERIAL_H
#define _VISTACONNECTIONSERIAL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaConnection.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSerialPort;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTAINTERPROCCOMMAPI VistaConnectionSerial : public VistaConnection
{
public:
	VistaConnectionSerial();

	VistaConnectionSerial(const std::string &sPortName);

	virtual ~VistaConnectionSerial();


	virtual bool Open();
	virtual void Close();

	virtual int Receive(void *buffer, const int length, int iTimeout = 0);
	virtual int Send(const void *buffer, const int length);


	bool          SetPortByName(const std::string &sPortName);
	std::string   GetPortName() const;

	bool          SetPortByIndex(int iIndex);
	int           GetPortIndex() const;

	bool          SetSpeed( int eSpeed);
	int           GetSpeed() const;
	unsigned long GetMaximumSpeed () const;

	bool          SetParity ( int eParam );
	int           GetParity () const;

	bool          SetDataBits ( int eParam );
	int           GetDataBits () const;

	bool          SetStopBits ( float eParam );
	float         GetStopBits () const;

	bool          SetHardwareFlow ( bool bInHandshaking );
	bool          GetIsHardwareFlow() const;

	bool          SetSoftwareFlow ( bool bParam );
	bool          GetIsSoftwareFlow () const;

	bool          SetBlockingMode ( unsigned long inReadInterval,
									unsigned long inReadMultiplyer,
									unsigned long inReadConstant );

	virtual bool HasPendingData() const;
	virtual unsigned long WaitForIncomingData(int timeout=0);
	virtual unsigned long PendingDataSize() const;

	virtual void SetIsBlocking(bool bBlocking);
	virtual bool GetIsBlocking() const;

	virtual HANDLE GetConnectionDescriptor() const;

private:
	VistaSerialPort *m_pSerialPort;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACONNECTIONSERIAL_H

