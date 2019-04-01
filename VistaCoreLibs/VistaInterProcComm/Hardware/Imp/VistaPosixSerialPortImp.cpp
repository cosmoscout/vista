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


#include "VistaPosixSerialPortImp.h"


#include <cstdio>
#include <string>

using namespace std;


#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

#if defined(SUNOS)
#include <sys/ioctl.h>
#include <sys/stream.h> 
#include <sys/stropts.h> 
#include <sys/ttold.h> 
#include <sys/ttcompat.h> 
#include <sys/filio.h>
#endif

#if defined(LINUX) || defined(IRIX) || defined(DARWIN)
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
static void PrintErrorMessage(int iErrno, const char *pcMsg)
{
	printf("[%s]: SerialPort-Error: %d\n", pcMsg, iErrno);
	switch(iErrno)
	{
		case EBADF:
		{
			printf("EBADF\n");
			break;
		}
		case ENOTTY:
		{
			printf("ENOTTY\n");
			break;
		}
		case EINVAL:
		{
			printf("EINVAL\n");
			break;
		}
		case ENXIO:
		{
			printf("driver said ENXIO\n");
			break;
		}
		default:
		{
			printf("Hmm...\n");
			break;
		}
	}
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPosixSerialPortImp::VistaPosixSerialPortImp()
{
	memset(&m_sTermConf, 0, sizeof(struct termios));
	m_iFileHandle = -1; // -1 is invalid and the standard "indicate-error"-return value, so we use this as default

#if defined(LINUX)
	AddPortName("/dev/ttyS0");
	AddPortName("/dev/ttyS1");
	AddPortName("/dev/ttyS2");
	AddPortName("/dev/ttyS3");
#elif defined(IRIX)
	AddPortName("/dev/ttyd1");
	AddPortName("/dev/ttyd2");
	AddPortName("/dev/ttyd3");
	AddPortName("/dev/ttyd4");
#elif defined(HPUX)
	AddPortName("/dev/tty1p0");
	AddPortName("/dev/tty2p0");
	AddPortName("/dev/tty3p0");
	AddPortName("/dev/tty4p0");
#elif defined(SUNOS)
	AddPortName("/dev/cua/a");
	AddPortName("/dev/cua/b");
	AddPortName("/dev/cua/c");
	AddPortName("/dev/cua/d");
#endif


	// these are empirical values that are observed to work with the devices we have here
	m_cVMIN = 0;
	m_cVTIME = 20;
}

VistaPosixSerialPortImp::~VistaPosixSerialPortImp()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaPosixSerialPortImp::SetHardwareState()
{
	/**
	 * we do use switch-case constructions here, as this does allow easier debugging
	 * (follow steps in single-step-mode)
	 */


	// ViSTA-standard: sanity checks
	if(m_iFileHandle < 0)
		return false;

	m_sTermConf.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG ); // use RAW INPUT
	
	m_sTermConf.c_oflag &= ~OPOST; // use RAW OUTPUT
	m_sTermConf.c_oflag &= ~ONLCR;
  
	m_sTermConf.c_cflag |= ( CREAD | CLOCAL); // enable receiver, and do not change ownership of file-handle


	switch(m_ePortSpeed)
	{
		case VistaSerialPortStateDesc::CSSP_9600:
		{
			cfsetispeed(&m_sTermConf, B9600);
			cfsetospeed(&m_sTermConf, B9600);
			break;
		}
		case VistaSerialPortStateDesc::CSSP_19200:
		{
				
			cfsetispeed(&m_sTermConf, B19200);
			cfsetospeed(&m_sTermConf, B19200);
			break;
		}
		case VistaSerialPortStateDesc::CSSP_38400:
		{
				
			cfsetispeed(&m_sTermConf, B38400);
			cfsetospeed(&m_sTermConf, B38400);
			break;
		}
		case VistaSerialPortStateDesc::CSSP_57600:
		{
				
			cfsetispeed(&m_sTermConf, B57600);
			cfsetospeed(&m_sTermConf, B57600);
			break;
		}
		case VistaSerialPortStateDesc::CSSP_115200:
		{

			cfsetispeed(&m_sTermConf, B115200);
			cfsetospeed(&m_sTermConf, B115200);
			break;
		}
		default:
			// we should prepare for error here?
			return false;
	}

	/**
	 * POSIX does not support 1.5 stop-bits, does it?
	 */
	switch(m_ePortStopBits)
	{
	case VistaSerialPortStateDesc::CSST_2:
		m_sTermConf.c_cflag |= CSTOPB;
		break;
	case VistaSerialPortStateDesc::CSST_1:
	case VistaSerialPortStateDesc::CSPA_NONE:
	default:
		// this might look as an error here... but POSIX does only seem
		// to support EITHER 1 OR 2 stopbits
		// so to be sure, we set one stopbit as default
		m_sTermConf.c_cflag  &= ~CSTOPB; // default = 1 stop-bit
		break;
	}

	m_sTermConf.c_cflag &= ~(CSIZE); // set mask for bit-sizes

	switch(m_ePortDataBits)
	{
	case VistaSerialPortStateDesc::CSD_5:
		m_sTermConf.c_cflag |= CS5;
		break;
	case VistaSerialPortStateDesc::CSD_6:
		m_sTermConf.c_cflag |= CS6;
		break;
	case VistaSerialPortStateDesc::CSD_7:
		m_sTermConf.c_cflag |= CS7;
		break;
	case VistaSerialPortStateDesc::CSD_8:
		m_sTermConf.c_cflag |= CS8;
		break;
	default:
		// this is regarded to be a mistake!
		return false;
	}

	switch(m_ePortParity)
	{
	case VistaSerialPortStateDesc::CSPA_EVEN:
		m_sTermConf.c_cflag |= PARENB;
		m_sTermConf.c_cflag &= ~PARODD;
		m_sTermConf.c_iflag |= INPCK;
		break;
	case VistaSerialPortStateDesc::CSPA_ODD:
		m_sTermConf.c_cflag |= PARENB;
		m_sTermConf.c_cflag |= PARODD;
		m_sTermConf.c_iflag |= INPCK;
		break;
	case VistaSerialPortStateDesc::CSPA_NONE:
		m_sTermConf.c_cflag &= ~PARENB;
		m_sTermConf.c_iflag &= ~INPCK;

		break;
	default:
		// this is regarded to be a mistake!
		return false;
	}

	/**
	 * handshaking is hardware-flow
	 */

#if !defined(HPUX)    // it is not clear where, how and if HPUX defines CRTSCTS, this is a todo!
	if(m_bIsHandshaking)
	{
		// there are new and old-style flags for CRTSCTS, IRIX uses new-style
		#if !defined(IRIX)
		m_sTermConf.c_cflag |= CRTSCTS;
		#else
		m_sTermConf.c_cflag |= CNEW_RTSCTS;
		#endif
	}
	else
	{
		// there are new and old-style flags for CRTSCTS, IRIX uses new-style
		#if !defined(IRIX)
		m_sTermConf.c_cflag &= ~CRTSCTS;
		#else
		m_sTermConf.c_cflag &= ~CNEW_RTSCTS;
		#endif
	}
#endif

	m_sTermConf.c_iflag &= ~IGNBRK; // always ignore breaks

	/**
	 * xOnxOff is software-flow
	 */
	if(m_bIsxOnxOff)
	{
		m_sTermConf.c_iflag |= (IXON | IXOFF | IXANY );
	}
	else
	{
		m_sTermConf.c_iflag &= ~(IXON | IXOFF | IXANY);
	}

	/**
	 * in case software-flow is off, these should be ignored by driver, so it is safe to set them here
	 */
	m_sTermConf.c_cc[VSTART] = m_cXonChar;
	m_sTermConf.c_cc[VSTOP]   = m_cXoffChar;
	m_sTermConf.c_cc[VEOF]   = m_cEofChar;

   if(tcsetattr( m_iFileHandle, TCSANOW, &m_sTermConf) < 0)
   {
	   PrintErrorMessage(errno, "setstate");
	   // we could not set hardware state!!, so try to get current state
	   GetHardwareState();
	   return false;
   }

   // allow device to take some time to settle things
   // 1000msecs is an empirical value, and was tested to work for the machines
   // we use...
   Delay(1000); 
   return true;
}

bool VistaPosixSerialPortImp::GetHardwareState()
{
	/**
	 * we do use switch-case constructions here, as this does allow easier debugging
	 * (follow steps in single-step-mode)
	 */


	// ViSTA-standard: sanity checks
	if(m_iFileHandle < 0)
		return false;

	// try to get state from hardware
	if(tcgetattr ( m_iFileHandle, &m_sTermConf) < 0)
	{
		PrintErrorMessage(errno, "getstate");
		// failed to get comm-state
		return false;
	}

	tcflag_t isb = m_sTermConf.c_cflag & CBAUD;
	switch(isb)
	{
	case B9600:
		{
			m_ePortSpeed = VistaSerialPortStateDesc::CSSP_9600;
			break;
		}
	case B19200:
		{
			m_ePortSpeed = VistaSerialPortStateDesc::CSSP_19200;
			break;
		}
	case B38400:
		{
			m_ePortSpeed = VistaSerialPortStateDesc::CSSP_38400;
			break;
		}
	case B57600:
		{
			m_ePortSpeed = VistaSerialPortStateDesc::CSSP_57600;
			break;
		}
	case B115200:
		{
			m_ePortSpeed = VistaSerialPortStateDesc::CSSP_115200;
			break;
		}
	default:
		m_ePortSpeed = VistaSerialPortStateDesc::CSSP_NONE;
		break;
	}


	isb = m_sTermConf.c_cflag & CSTOPB;

	// POSIX does not support 1.5 stopbits
	if(isb)
		m_ePortStopBits = VistaSerialPortStateDesc::CSST_2;
	else
		m_ePortStopBits = VistaSerialPortStateDesc::CSST_1;


	isb = m_sTermConf.c_cflag & CSIZE;


	switch(isb)
	{
	case CS5:
		m_ePortDataBits = VistaSerialPortStateDesc::CSD_5;
		break;
	case CS6:
		m_ePortDataBits = VistaSerialPortStateDesc::CSD_6;
		break;
	case CS7:
		m_ePortDataBits = VistaSerialPortStateDesc::CSD_7;
		break;
	case CS8:
		m_ePortDataBits = VistaSerialPortStateDesc::CSD_8;
		break;
	default:
		m_ePortDataBits = VistaSerialPortStateDesc::CSD_NONE;
		break;
	}

	isb = m_sTermConf.c_cflag & PARENB;

	if(!isb)
		m_ePortParity = VistaSerialPortStateDesc::CSPA_NONE;
	else
	{
		isb = m_sTermConf.c_cflag & PARODD;
		// check for odd or even
		if(isb)
		{
			// odd
			m_ePortParity = VistaSerialPortStateDesc::CSPA_ODD;
		}
		else
			m_ePortParity = VistaSerialPortStateDesc::CSPA_EVEN;
	}
#if !defined(HPUX) // is is not clear where, how and if HPUX does define CRTSCTS, this is a todo!
#if !defined(IRIX)
	isb = m_sTermConf.c_cflag & CRTSCTS;
#else
	isb = m_sTermConf.c_cflag & CNEW_RTSCTS;
#endif
#endif // HPUX

	if(isb)
	{
		// handshake enabled
		m_bIsHandshaking = true;
	}
	else
		m_bIsHandshaking = false;


	isb = m_sTermConf.c_cflag & ( IXON | IXOFF | IXANY );

	if(isb)
		m_bIsxOnxOff = true;
	else
		m_bIsxOnxOff = false;

	

	m_cXonChar = m_sTermConf.c_cc[VSTART];
	m_cXoffChar = m_sTermConf.c_cc[VSTOP];

	m_cEofChar = m_sTermConf.c_cc[VEOF];

	return true;
}

bool VistaPosixSerialPortImp::OpenSerialPort()
{
	if(m_iFileHandle >= 0)
		return true; // we return true for convinience, alas... this port IS open ;)

	
	printf("Trying to open POSIX-serial port \"%s\"\n", GetPortName().c_str());

	m_iFileHandle = TEMP_FAILURE_RETRY( open(GetPortName().c_str(), O_RDWR|O_NOCTTY) );
	if( m_iFileHandle != -1)
	{
		SetIsOpen(true);
		// this IS important, set stored timeout values
		// ok, created port, see if we can get config
		if(!SetHardwareState()) // set the current state
			return GetHardwareState(); // setting failed, so lets see what is set on the hw

		SetBlockingMode((int)m_cVTIME,0, (int)m_cVMIN);
		return true; // ok, this worked
	}
	else
		printf( "OPEN FAILED (erro=%d).\n", errno);
	return false;
}

bool VistaPosixSerialPortImp::CloseSerialPort()
{
	if(m_iFileHandle < 0)
		return true; // we return true for convinience, alas... this port IS closed ;)

	if(close(m_iFileHandle) >= 0)
	{
		m_iFileHandle = -1;
		SetIsOpen(false);
		return true;
	}

	return false;
}

int  VistaPosixSerialPortImp::Receive(void *buffer, const int length, int iTimeout ) 
{
	if(m_iFileHandle < 0)
		return -1;

	if(iTimeout)
	{
		if(WaitForIncomingData(iTimeout) == ~0u)
			return 0; // timeout!
	}

	int charsRead = TEMP_FAILURE_RETRY( read( m_iFileHandle, buffer, length ) );
	if ( charsRead < 0  )
	{
		// this failed.
		return -1;
	}
	return charsRead;
}

int  VistaPosixSerialPortImp::Send(const void *buffer, const int length) 
{
	if(m_iFileHandle < 0)
		return -1;

	int charsWritten = TEMP_FAILURE_RETRY( write( m_iFileHandle, buffer, length ) );
	if( charsWritten < 0 )
	{
		// this failed
		return -1;
	}

	return charsWritten;
}

bool VistaPosixSerialPortImp::SetBlockingMode ( unsigned long inReadInterval, unsigned long inReadMultiplyer, unsigned long inReadConstant )
{

	// we store those constants to set them later, iff this port is not opened
	m_cVMIN = (char)inReadConstant;
	m_cVTIME = (char)inReadInterval;


	if(!m_iFileHandle)
		return false;
	
	if(inReadInterval || inReadConstant)
	{
	// enable non-blocking mode with timeout
		m_sTermConf.c_cc[VTIME] = m_cVTIME; // set timeout
		m_sTermConf.c_cc[VMIN] = m_cVMIN;   // set minimium of characters to read before timeout
#if defined(IRIX)
		fcntl(m_iFileHandle, F_SETFL, FNDELAY); // return after timeout or when bytes are ready
#elif defined(SUNOS)
		fcntl(m_iFileHandle, F_SETFL, O_NONBLOCK); // return after timeout or when bytes are ready
#endif

		// we do set this directly via tcssettr, as we do not want to get/set hardware state now

	}
	else
	{
		m_sTermConf.c_cc[VTIME] = 0; // no timout, block
		m_sTermConf.c_cc[VMIN] = 1;  // blocking mode, wait at least for 1 char
#if defined(IRIX)
	fcntl(m_iFileHandle, F_SETFL, 0); // return immediately, when no bytes are ready
#elif defined(SUNOS)
	fcntl(m_iFileHandle, F_SETFL, 0); // reset any O_NONBLOCK flag
#endif
	}
	
	if (tcsetattr(m_iFileHandle, TCSANOW, &m_sTermConf) >= 0)
			return true;
	else
	{
				printf("error setting blocking mode.\n");
				return false;
	}
}

unsigned long VistaPosixSerialPortImp::WaitForIncomingData(int timeout)
{
	timeout *= 1000;
	struct timeval tv;
	tv.tv_sec  = (timeout != 0) ? (timeout / 1000000) : 0;
	tv.tv_usec = (timeout != 0) ? (timeout % 1000000) : 0;

	fd_set fdset;
	FD_ZERO(&fdset);

	FD_SET(m_iFileHandle, &fdset);

	
	int iRet = TEMP_FAILURE_RETRY( select(m_iFileHandle+1, &fdset, NULL, NULL, ((tv.tv_sec!=0 || tv.tv_usec!=0) ? &tv : NULL) ) );
	if(iRet == 0)
	{
		//printf("timeout?\n");
		return ~0;
	}
	return PendingDataSize();

}

unsigned long VistaPosixSerialPortImp::PendingDataSize() const
{
//#if defined(SUNOS)
//    return (unsigned long)( ioctl(m_iFileHandle,FIORDCHK,NULL));
//#else
	int nchars=0;
	ioctl(m_iFileHandle,FIONREAD,&nchars);
	return (unsigned long)nchars;
//#endif
}


void VistaPosixSerialPortImp::Delay(int msecs) const
{
	msecs = msecs * 1000;
	struct timeval tv;
	tv.tv_sec  = (msecs != 0) ? (msecs / 1000000) : 0;
	tv.tv_usec = (msecs != 0) ? (msecs % 1000000) : 0;

	// simply poll, but in microwait!
	select(0, NULL, NULL, NULL, ((tv.tv_sec!=0 || tv.tv_usec!=0) ? &tv : NULL));
}

void VistaPosixSerialPortImp::SetIsBlocking(bool bBlocking)
{
	if(bBlocking)
		{
			if(!SetBlockingMode(0,0,0))
				return;
	}
	else
		{
			if(!SetBlockingMode(20,0,0))
				return;
	}
	VistaSerialPortImp::SetIsBlocking(bBlocking);
}

HANDLE VistaPosixSerialPortImp::GetDescriptor() const
{
	return m_iFileHandle;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // #if defined(LINUX) || defined(IRIX) || defined(HPUX) || defined(SUNOS)

