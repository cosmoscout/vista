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

#include "VistaWin32SerialPortImp.h"

#include <VistaBase/VistaStreamUtils.h>

#pragma warning(disable : 4996)

#include <iostream>
using namespace std;
#include <cstdio>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static string _build_dcb_string(const VistaWin32SerialPortImp& con) {
  string sRet;
  sRet.resize(100);

  if (con.GetStopBits() != VistaSerialPortStateDesc::CSST_15) {
    sprintf((char*)sRet.data(), "%d,%c,%d,%d", con.GetSpeed(),
        (con.GetParity() == VistaSerialPortStateDesc::CSPA_EVEN ? 'n' : 'y'), con.GetDataBits(),
        con.GetStopBits());
  } else {
    sprintf((char*)sRet.data(), "%d,%c,%d,1.5", con.GetSpeed(),
        (con.GetParity() == VistaSerialPortStateDesc::CSPA_EVEN ? 'n' : 'y'), con.GetDataBits());
  }

  return sRet;
}

static void PrintSerialPortErrorMessage(DWORD error) {
  switch (error) {
  case CE_RXOVER: {
    vstr::warnp() << "Receive Queue overflow" << std::endl;
    break;
  }
  case CE_OVERRUN: {
    vstr::warnp() << "Receive Overrun Error" << std::endl;
    break;
  }
  case CE_RXPARITY: {
    vstr::warnp() << "Receive Parity Error" << std::endl;
    break;
  }
  case CE_FRAME: {
    vstr::warnp() << "Receive Framing error" << std::endl;
    break;
  }
  case CE_BREAK: {
    vstr::warnp() << "Break Detected" << std::endl;
    break;
  }
  case CE_TXFULL: {
    vstr::warnp() << "TX Queue is full" << std::endl;
    break;
  }
  case CE_PTO: {
    vstr::warnp() << "LPTx Timeout" << std::endl;
    break;
  }
  case CE_IOE: {
    vstr::warnp() << "LPTx I/O Error" << std::endl;
    break;
  }
  case CE_DNS: {
    vstr::warnp() << "LPTx Device not selected" << std::endl;
    break;
  }
  case CE_OOP: {
    vstr::warnp() << "LPTx Out-Of-Paper" << std::endl;
    break;
  }
  case CE_MODE: {
    vstr::warnp() << "Requested mode unsupported" << std::endl;
    break;
  }
  case ERROR_IO_PENDING: {
    vstr::errp() << "IO Pending" << std::endl;
    break;
  }
  default: {
    vstr::warnp() << "Undocumented win32-serialport error (" << error << ")" << std::endl;
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

    vstr::warnp() << lpMsgBuf << std::endl;
    LocalFree(lpMsgBuf);
    break;
  }
  }
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaWin32SerialPortImp::VistaWin32SerialPortImp() {
  m_hanPort = NULL;
  memset(&m_hOverlap, 0, sizeof(OVERLAPPED));

  m_hOverlap.hEvent = ::CreateEvent(NULL, FALSE, FALSE, "Overlapped_Event_Serial_Port_");

  memset(&m_myDcb, 0, sizeof(m_myDcb));
  string dcbString = _build_dcb_string(*this);

  if (!BuildCommDCB(dcbString.c_str(), &m_myDcb)) {
    // this IS a problem
    // I'd love to throw an exception here... but we do not
    // for several reasons
  }

  AddPortName("\\\\.\\COM1");
  AddPortName("\\\\.\\COM2");
  AddPortName("\\\\.\\COM3");
  AddPortName("\\\\.\\COM4");

  // these are emirpircal values that work with the devices we have
  m_iReadMultiplier = 0;
  m_iReadTimeout    = MAXDWORD;
  m_iReadConstant   = 2000;
}

VistaWin32SerialPortImp::~VistaWin32SerialPortImp() {
  ::CloseHandle(m_hOverlap.hEvent);
  if (m_hanPort)
    ::CloseHandle(m_hanPort);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaWin32SerialPortImp::SetHardwareState() {
  if (!m_hanPort)
    return false;

  m_myDcb.BaudRate = (int)m_ePortSpeed;

  switch (m_ePortStopBits) {
  case VistaSerialPortStateDesc::CSST_2:
    m_myDcb.StopBits = TWOSTOPBITS;
    break;
  case VistaSerialPortStateDesc::CSST_15:
    m_myDcb.StopBits = ONE5STOPBITS;
  case VistaSerialPortStateDesc::CSST_1:
    m_myDcb.StopBits = ONESTOPBIT;
    break;
  default:
    // this is regarded to be a mistake
    return false;
  }

  switch (m_ePortDataBits) {
  case VistaSerialPortStateDesc::CSD_5:
    m_myDcb.ByteSize = 5;
    break;
  case VistaSerialPortStateDesc::CSD_7:
    m_myDcb.ByteSize = 7;
    break;
  case VistaSerialPortStateDesc::CSD_8:
    m_myDcb.ByteSize = 8;
    break;
  default:
    // this is regarded to be a mistake!
    return false;
  }

  switch (m_ePortParity) {
  case VistaSerialPortStateDesc::CSPA_EVEN:
    m_myDcb.Parity  = EVENPARITY;
    m_myDcb.fParity = TRUE;
    break;
  case VistaSerialPortStateDesc::CSPA_ODD:
    m_myDcb.Parity  = ODDPARITY;
    m_myDcb.fParity = TRUE;
    break;
  case VistaSerialPortStateDesc::CSPA_NONE:
    m_myDcb.Parity  = NOPARITY;
    m_myDcb.fParity = FALSE;
    break;
  default:
    // this is regarded to be a mistake!
    return false;
  }

  if (m_bIsHandshaking) {
    m_myDcb.fOutxCtsFlow = TRUE;
    // m_myDcb.fOutxDsrFlow = TRUE;
    m_myDcb.fRtsControl = RTS_CONTROL_ENABLE;
    // m_myDcb.fDtrControl  = DTR_CONTROL_HANDSHAKE;

  } else {
    m_myDcb.fOutxCtsFlow = FALSE;
    m_myDcb.fOutxDsrFlow = FALSE;

    m_myDcb.fRtsControl = RTS_CONTROL_ENABLE;
    m_myDcb.fDtrControl = DTR_CONTROL_ENABLE;
    m_myDcb.fInX = m_myDcb.fOutX = m_myDcb.fOutxDsrFlow = m_myDcb.fOutxCtsFlow = FALSE;
  }

  m_myDcb.XonChar   = m_cXonChar;
  m_myDcb.XoffChar  = m_cXoffChar;
  m_myDcb.EofChar   = m_cEofChar;
  m_myDcb.ErrorChar = m_cErrorChar;

  if (SetCommState(m_hanPort, &m_myDcb) == FALSE) {
    // we could not set hardware state, so try to get the current state
    GetHardwareState();
    return false; // return false to indicate failure
  }

  // allow device to take some time to settle things
  // 1000msecs is an empirical value, and was tested to work for the machines
  // we use...

  Delay(1000);

  return true; // ok, we could set hardware state
}

bool VistaWin32SerialPortImp::GetHardwareState() {
  if (!m_hanPort)
    return false;

  if (!GetCommState(m_hanPort, &m_myDcb)) {
    // failed to get comm-state
    return false;
  }

  m_ePortSpeed = (VistaSerialPortStateDesc::VistaSpeed)m_myDcb.BaudRate;

  switch (m_myDcb.StopBits) {
  case ONESTOPBIT:
    m_ePortStopBits = VistaSerialPortStateDesc::CSST_1;
    break;
  case ONE5STOPBITS:
    m_ePortStopBits = VistaSerialPortStateDesc::CSST_2;
    break;
  case TWOSTOPBITS:
    m_ePortStopBits = VistaSerialPortStateDesc::CSST_15;
  default:
    m_ePortStopBits = VistaSerialPortStateDesc::CSST_NONE;
  }

  switch (m_myDcb.ByteSize) {
  case 5:
    m_ePortDataBits = VistaSerialPortStateDesc::CSD_5;
    break;
  case 6:
    m_ePortDataBits = VistaSerialPortStateDesc::CSD_6;
    break;
  case 7:
    m_ePortDataBits = VistaSerialPortStateDesc::CSD_7;
    break;
  case 8:
    m_ePortDataBits = VistaSerialPortStateDesc::CSD_8;
    break;
  default:
    m_ePortDataBits = VistaSerialPortStateDesc::CSD_NONE;
    break;
  }

  switch (m_myDcb.Parity) {
  case EVENPARITY:
    m_ePortParity = VistaSerialPortStateDesc::CSPA_EVEN;
    break;
  case ODDPARITY:
    m_ePortParity = VistaSerialPortStateDesc::CSPA_ODD;
    break;
  case NOPARITY:
  default:
    m_ePortParity = VistaSerialPortStateDesc::CSPA_NONE;
    break;
  }

  if ((m_myDcb.fOutxCtsFlow == TRUE) && (m_myDcb.fOutxDsrFlow == FALSE) &&
      (m_myDcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
    m_bIsHandshaking = true;
  else
    m_bIsHandshaking = false;

  m_bIsxOnxOff = (m_myDcb.fOutX && m_myDcb.fInX);

  m_cXonChar   = m_myDcb.XonChar;
  m_cXoffChar  = m_myDcb.XoffChar;
  m_cEofChar   = m_myDcb.EofChar;
  m_cErrorChar = m_myDcb.ErrorChar;

  return true;
}

bool VistaWin32SerialPortImp::OpenSerialPort() {
  if (m_hanPort)
    return true; // we do this for convienence: it is already open

  if ((m_hanPort = CreateFile(GetPortName().c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
           OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, 0))) {
    SetIsOpen(true);

    // this IS important: set defaults for correct timeout settings
    SetBlockingMode(m_iReadTimeout, m_iReadMultiplier, m_iReadConstant);

    // ok, created port, see if we can get config
    if (!SetHardwareState())     // set current settings
      return GetHardwareState(); // this failed, so try to see what the current state is

    return true; // this worked
  }
  return false; // this failed
}

bool VistaWin32SerialPortImp::CloseSerialPort() {
  if (!m_hanPort)
    return true; // we do this for convienence: it is already closed

  if (CloseHandle(m_hanPort) == TRUE) {
    m_hanPort = NULL;
    SetIsOpen(false);
    return true;
  }

  return false;
}

int VistaWin32SerialPortImp::Receive(void* buffer, const int length, int iTimeout) {
  if (!m_hanPort)
    return -1;

  DWORD charsRead = 0;
  if (GetIsBlocking()) {
    if (!ReadFile((HANDLE(m_hanPort)), buffer, length, &charsRead, &m_hOverlap)) {
      // this failed.
      DWORD err = GetLastError();
      if (err != ERROR_IO_PENDING)
        return -1;
    }

    // wait for an overlapped event
    DWORD waitTime = INFINITE;
    // DWORD dwCause = 0;
    if (iTimeout > 0)
      waitTime = DWORD(iTimeout);

    DWORD retval = 0;
    if ((retval = ::WaitForSingleObject(HANDLE(m_hOverlap.hEvent), waitTime)) == WAIT_OBJECT_0)
      //::WaitCommEvent(HANDLE(m_hanPort), &dwCause, &m_hOverlap);
      ::GetOverlappedResult(HANDLE(m_hanPort), &m_hOverlap, &charsRead, true);
    else {
      if (retval == WAIT_TIMEOUT)
        return 0;
      return -1;
    }

  } else {

    DWORD retval = 0;
    if ((retval = ::WaitForSingleObject(HANDLE(m_hOverlap.hEvent), DWORD(iTimeout))) ==
        WAIT_OBJECT_0) {
      if (!ReadFile((HANDLE(m_hanPort)), buffer, length, &charsRead, &m_hOverlap)) {
        DWORD err = GetLastError();
        if (err == ERROR_IO_PENDING)
          return charsRead;
#if 0
				PrintSerialPortErrorMessage(err);
#endif

        // this failed.
        return -1;
      }
    }
  }
  return (int)charsRead;
}

int VistaWin32SerialPortImp::Send(const void* buffer, const int length) {
  if (!m_hanPort)
    return -1;

  DWORD charsWritten = 0;

  if (!WriteFile((HANDLE(m_hanPort)), buffer, length, &charsWritten, &m_hOverlap)) {
    // this failed
    DWORD err = GetLastError();
    if ((err == ERROR_IO_PENDING) && GetIsBlocking()) {
      ::WaitForSingleObject(m_hOverlap.hEvent, INFINITE);
      GetOverlappedResult(HANDLE(m_hanPort), &m_hOverlap, &charsWritten, true);
      return charsWritten;
    } else if (err == ERROR_IO_PENDING)
      return charsWritten;
    else
      PrintSerialPortErrorMessage(err);

    return -1;
  }

  return (int)charsWritten;
}

bool VistaWin32SerialPortImp::SetBlockingMode(
    unsigned long inReadInterval, unsigned long inReadMultiplyer, unsigned long inReadConstant) {
  // we store the incoming values, so they will be set once the port is opened
  m_iReadMultiplier = inReadMultiplyer;
  m_iReadTimeout    = inReadInterval;
  m_iReadConstant   = inReadConstant;

  if (!m_hanPort)
    return false;

  COMMTIMEOUTS commTimeout;
  if (!GetCommTimeouts((HANDLE(m_hanPort)), &commTimeout)) {
    return false;
  } // endif

  commTimeout.ReadIntervalTimeout = inReadInterval;

  commTimeout.ReadTotalTimeoutMultiplier = inReadMultiplyer;
  commTimeout.ReadTotalTimeoutConstant   = inReadConstant;

  commTimeout.WriteTotalTimeoutConstant   = inReadConstant;
  commTimeout.WriteTotalTimeoutMultiplier = inReadMultiplyer;

  if (!SetCommTimeouts((HANDLE(m_hanPort)), &commTimeout))
    return false;

  return true;
}

unsigned long VistaWin32SerialPortImp::WaitForIncomingData(int timeout) {
  DWORD oldMask, dwCurrentMask;
  GetCommMask(HANDLE(m_hanPort), &oldMask);

  SetCommMask(HANDLE(m_hanPort), EV_ERR | EV_RXCHAR);

  // we opened the file with OVERLAP flag, so we MUST pass a pointer
  // to an OVERLAP structure with a proper event set.
  if (!WaitCommEvent(HANDLE(m_hanPort), &dwCurrentMask, &m_hOverlap)) {
    if (!GetIsBlocking()) {
      if (GetLastError() != ERROR_IO_PENDING)
        return ~0UL; // error!
      // ok, pending request, we may wait if there is a time specified
      if (timeout) {
        DWORD chars = 0;
        if (::WaitForSingleObject(m_hOverlap.hEvent, DWORD(timeout)) != WAIT_TIMEOUT) {
          GetOverlappedResult(HANDLE(m_hanPort), &m_hOverlap, &chars, true);
        } else
          return ~0UL; // timeout
      }
    } else // blocking
    {
      //	reset mask
      SetCommMask(HANDLE(m_hanPort), oldMask);
      if (GetLastError() != ERROR_IO_PENDING)
        return ~0UL; // error!
    }
  }
  return PendingDataSize();
}

unsigned long VistaWin32SerialPortImp::PendingDataSize() const {
  DWORD   error;
  COMSTAT comstat;

  ClearCommError(HANDLE(m_hanPort), &error, &comstat);
#if 0
	if(error!=0)
	{
		PrintSerialPortErrorMessage(error);
	}
#endif
  unsigned long n = comstat.cbInQue;
  return n;
}

void VistaWin32SerialPortImp::Delay(int iMsecs) const {
  Sleep(iMsecs);
}

void VistaWin32SerialPortImp::SetIsBlocking(bool bBlocking) {
  /*    if(bBlocking)
                  if(!SetBlockingMode(0,0,0))
                          return;
          else
                  if(!SetBlockingMode(MAXDWORD,0,2000))
                          return;
                          */

  //    m_iReadMultiplier = inReadMultiplyer;
  //    m_iReadTimeout    = inReadInterval;
  //    m_iReadConstant   = inReadConstant;

  if (!m_hanPort)
    return;

  COMMTIMEOUTS commTimeout;
  if (!GetCommTimeouts((HANDLE(m_hanPort)), &commTimeout)) {
    vstr::errp() << "[VistaWin32SerialPortImp]: Could not get CommTimeouts" << std::endl;
    return;
  } // endif

  if (bBlocking) {
    /* Quoting the MSDN:
     // If an application sets ReadIntervalTimeout and
            ReadTotalTimeoutMultiplier to MAXDWORD and sets
            ReadTotalTimeoutConstant to a value greater than
            zero and less than MAXDWORD, one of the following
            occurs when the ReadFile function is called:
            * If there are any bytes in the input buffer,
              ReadFile returns immediately with the bytes in the buffer.
            * If there are no bytes in the input buffer,
              ReadFile waits until a byte arrives and then
              returns immediately.
            * If no bytes arrive within the time specified by
              ReadTotalTimeoutConstant, ReadFile times out.
       // Following from that, we make "blocking" a very late return
      */
    commTimeout.ReadIntervalTimeout        = 0;
    commTimeout.ReadTotalTimeoutMultiplier = MAXDWORD;
    commTimeout.ReadTotalTimeoutConstant   = MAXDWORD - 1;

    commTimeout.WriteTotalTimeoutConstant   = MAXDWORD;
    commTimeout.WriteTotalTimeoutMultiplier = MAXDWORD;
  } else {
    commTimeout.ReadIntervalTimeout        = MAXDWORD;
    commTimeout.ReadTotalTimeoutMultiplier = 0;
    commTimeout.ReadTotalTimeoutConstant   = 0;

    commTimeout.WriteTotalTimeoutConstant   = 0;
    commTimeout.WriteTotalTimeoutMultiplier = 0;
  }

  if (!SetCommTimeouts((HANDLE(m_hanPort)), &commTimeout)) {
    vstr::errp() << "[VistaWin32SerialPortImp]: Could not set comm timeouts" << std::endl;
    return;
  } else {
    VistaSerialPortImp::SetIsBlocking(bBlocking);
  }
}

HANDLE VistaWin32SerialPortImp::GetDescriptor() const {
  return m_hOverlap.hEvent;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // WIN32
