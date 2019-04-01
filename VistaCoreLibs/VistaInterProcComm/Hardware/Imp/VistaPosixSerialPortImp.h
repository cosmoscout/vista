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


#if defined(LINUX) || defined(IRIX) || defined(HPUX) || defined(SUNOS) || defined(DARWIN)
#ifndef _VISTAPOSIXSERIALPORTIMP_H
#define _VISTAPOSIXSERIALPORTIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaSerialPortImp.h"


#ifdef IRIX
#include <termio.h>
#endif

// All UNIX flavors
#include <termios.h>

// for systems which do not define it (DARWIN)
#ifndef CBAUD
#define CBAUD 0010017 // taken from LINUX termios.h
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

class VistaPosixSerialPortImp : public VistaSerialPortImp
{
public:

	VistaPosixSerialPortImp();

	virtual ~VistaPosixSerialPortImp();


	virtual bool          OpenSerialPort() ;

	virtual bool          CloseSerialPort();

	virtual int           Receive(void *buffer, const int length, int iTimeout = 0) ;

	virtual int           Send(const void *buffer, const int length) ;

	virtual bool          SetBlockingMode ( unsigned long inReadInterval, unsigned long inReadMultiplyer, unsigned long inReadConstant );

	virtual unsigned long WaitForIncomingData(int timeout=0);

	virtual unsigned long PendingDataSize() const;
	virtual void          SetIsBlocking(bool bBlocking);

	virtual HANDLE GetDescriptor() const;

protected:
	/**
	 * tries to set the hardware as presented by the internals of this instance. If setting of this
	 * values fails, it is tried to gain the current state of the hardware by a call to GetHardwareState().
	 * @todo create a copy of the hardware structure and set this on failure
	 * @return false iff the hardware-state could not be set (e.g. when the port is not opened)
	 */
	bool SetHardwareState();

	/**
	 * Tries to gain the current set state from the hardware in order to create a valid state of this
	 * serialport representation.
	 * @true iff the hardwarestate could be set, false if the serialport is not open, yet
	 */
	bool GetHardwareState();

	/**
	 * Serialport-hardware is special (old stuff, you know...), so delays are useful when talking to
	 * the serial hardware. This method implements a delay in terms of millisecs without binding
	 * this module to the VistaTools-package
	 */
	void Delay(int iMsecs) const;

private:

	/**
	 * stores the filehandle for this implementation, -1 indicates an unallocated filehandle
	 * (state closed)
	 */
	int             m_iFileHandle;

	/**
	 * is used during getting/setting information from/to the hardware
	 */
	struct termios  m_sTermConf;

	char            m_cVMIN, /**< defaults to 0 */
					m_cVTIME; /**< defaults to 20 */
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTACONNECTIONSERIALWIN32

#endif // LINUX || IRIX || HPUX || SUNOS

