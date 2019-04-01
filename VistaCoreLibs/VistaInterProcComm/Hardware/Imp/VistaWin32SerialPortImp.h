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


#if defined(WIN32)
#ifndef _VISTAWIN32SERIALPORTIMP_H
#define _VISTAWIN32SERIALPORTIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaSerialPortImp.h"


#if !defined(_WINDOWS_)
#include <Windows.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaWin32SerialPortImp : public VistaSerialPortImp
{
public:
	VistaWin32SerialPortImp();
	virtual ~VistaWin32SerialPortImp();


	virtual bool          OpenSerialPort() ;
	virtual bool          CloseSerialPort();

	/**
	 * Timeout-feature is currently NOT supported on win32
	 */
	virtual int           Receive(void *buffer, const int length, int iTimeout = 0) ;
	virtual int           Send(const void *buffer, const int length) ;


	virtual bool          SetBlockingMode ( unsigned long inReadInterval, unsigned long inReadMultiplyer, unsigned long inReadConstant );

	/**
	 * Timeout-feature is currently NOT supported on win32
	 * THIS CALL (WIN32) WILL BLOCK FOREVER IF THERE IS NO INCOMING DATA ON A VALID PORT!
	 * @todo fix timout feature on win32
	 */
	virtual unsigned long WaitForIncomingData(int timeout=0);

	virtual unsigned long PendingDataSize() const;

	virtual void          SetIsBlocking(bool bBlocking);

	virtual HANDLE        GetDescriptor() const;

protected:
	bool SetHardwareState();
	bool GetHardwareState();

	virtual void Delay(int iMsecs) const;
private:
	DCB    m_myDcb;
	HANDLE m_hanPort;
	DWORD    m_iReadMultiplier,
			 m_iReadTimeout,
			 m_iReadConstant;
	OVERLAPPED m_hOverlap;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTACONNECTIONSERIALWIN32

#endif // WIN32

