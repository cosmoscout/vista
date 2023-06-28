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

#include "VistaConnectionNamedPipe.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#if !defined(WIN32)
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#else
#include <Windows.h>
#endif

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

#ifndef WIN32
namespace {
const std::string S_sFifoFolder = "/tmp/VistaFifos";

int OpenFIFOWrite(const std::string& sName) {
  int nFile = TEMP_FAILURE_RETRY(open(sName.c_str(), O_WRONLY));

  if (nFile < 0) {
    vstr::warnp() << "Opening Named Pipe [" << sName << "] for writing failed with error [" << errno
                  << "] (" << strerror(errno) << ")" << std::endl;
  }
  return nFile;
}
int OpenFIFORead(const std::string& sName, int m_nMaxWait = 10000) {
  VistaTimer oTimer;
  double     nMaxWaitSecs = m_nMaxWait * 1e-3;
  int        nFile        = -1;
  for (;;) {
    nFile = TEMP_FAILURE_RETRY(open(sName.c_str(), O_RDONLY));
    if (nFile >= 0)
      break;
    VistaTimeUtils::Sleep(1);
    if (oTimer.GetLifeTime() < nMaxWaitSecs) {
      vstr::warnp() << "Opening named pipe [" << sName << "] for reading failed with error ["
                    << errno << "] (" << strerror(errno) << ")" << std::endl;
      return -1;
    }
  }
  return nFile;
}
bool CreateFifo(const std::string& sName) {
  struct stat oAttributes;
  if (stat(S_sFifoFolder.c_str(), &oAttributes) != 0 ||
      ((oAttributes.st_mode & S_IFDIR) == false)) {
    if (mkdir(S_sFifoFolder.c_str(), S_IRWXU | S_IRGRP | S_IXGRP) != 0)
      return false;
  }

  int nRes = mkfifo(sName.c_str(), 0600);
  if (nRes != 0 && errno == EEXIST) {
    // it's okay, we just claim it
    return true;
  }
  if (nRes == 0) {
    return true;
  }
  vstr::warnp() << "Creating named pipe [" << sName << "] failed with error [" << errno << "] ("
                << strerror(errno) << ")" << std::endl;
  return false;
}
} // namespace
#else
namespace vcnamedpipe {
std::string GetErrorText(int nCode = -1) {
  std::string sReturn(1024, '\0');
  if (nCode == -1)
    nCode = GetLastError();
  int nSize = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, nCode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&sReturn[0], 1024, NULL);
  sReturn.resize(nSize);
  return sReturn;
}
} // namespace vcnamedpipe
#endif
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaConnectionNamedPipe::VistaConnectionNamedPipe(
    const std::string& sPipeName, const bool bIsServer)
    : m_sPipeName(sPipeName)
    , m_bIsServer(bIsServer) {
#ifdef WIN32
  if (m_bIsServer) {
    std::string sActualName = "\\\\.\\pipe\\" + m_sPipeName;
    m_oPipe                 = CreateNamedPipe(sActualName.c_str(), PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 512, 512, 60000, NULL);
    if (m_oPipe == INVALID_HANDLE_VALUE) {
      vstr::warnp() << "[VistaConnectionNamedPipe]: Could not create event [" << m_sPipeName
                    << "] - Error: " << vcnamedpipe::GetErrorText() << std::endl;
      m_oPipe = NULL;
    }
  }
#else
  std::string sLeftName  = S_sFifoFolder + "/.VistaPipe_" + m_sPipeName + "_left";
  std::string sRightName = S_sFifoFolder + "/.VistaPipe_" + m_sPipeName + "_right";

  CreateFifo(sLeftName.c_str());
  CreateFifo(sRightName.c_str());

  m_nReadFifo             = -1;
  m_nWriteFifo            = -1;
#endif
  SetIsOpen(false);
}

VistaConnectionNamedPipe::~VistaConnectionNamedPipe() {
  Close();
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaConnectionNamedPipe::Open() {
  if (GetIsOpen())
    return true;

#if !defined(WIN32)
  std::string sLeftName  = S_sFifoFolder + "/.VistaPipe_" + m_sPipeName + "_left";
  std::string sRightName = S_sFifoFolder + "/.VistaPipe_" + m_sPipeName + "_right";

  if (m_bIsServer) {
    m_nWriteFifo = OpenFIFOWrite(sLeftName);
    m_nReadFifo  = OpenFIFORead(sRightName);
  } else {
    m_nReadFifo  = OpenFIFORead(sLeftName);
    m_nWriteFifo = OpenFIFOWrite(sRightName);
  }

#else
  std::string sActualName = "\\\\.\\pipe\\" + m_sPipeName;
  if (m_bIsServer) {
    if (m_oPipe == NULL) {
      vstr::warnp() << "[VistaConnectionNamedPipe]: pipe could not be created" << std::endl;
      return false;
    }
    if (ConnectNamedPipe(m_oPipe, NULL) == false) {
      vstr::warnp() << "[VistaConnectionNamedPipe]: pipe client did not connect to pipe ["
                    << m_sPipeName << "] - Error: " << vcnamedpipe::GetErrorText() << std::endl;
      SetIsOpen(false);
      return false;
    }
  } else {
    //@todo: timeout
    if (WaitNamedPipe(sActualName.c_str(), 60000) == 0) {
      vstr::warnp() << "[VistaConnectionNamedPipe]: Could not find server's pipe [" << m_sPipeName
                    << "] - Error: " << vcnamedpipe::GetErrorText() << std::endl;
      SetIsOpen(false);
      return false;
    }
    m_oPipe = CreateFile(
        sActualName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (m_oPipe == INVALID_HANDLE_VALUE) {
      vstr::warnp() << "[VistaConnectionNamedPipe]: Could not create event [" << m_sPipeName
                    << "] - Error: " << vcnamedpipe::GetErrorText() << std::endl;
      SetIsOpen(false);
      return false;
    }
  }
#endif

  SetIsOpen(true);
  return true;
}

void VistaConnectionNamedPipe::Close() {
  if (!GetIsOpen())
    return;

#if !defined(WIN32)
  close(m_nReadFifo);
  close(m_nWriteFifo);

  std::string sLeftFile  = S_sFifoFolder + "/.VistaPipe_" + m_sPipeName + "_left";
  std::string sRightFile = S_sFifoFolder + "/.VistaPipe_" + m_sPipeName + "_right";

  remove(sLeftFile.c_str());
  remove(sRightFile.c_str());

#else
  CloseHandle(m_oPipe);
#endif
}

int VistaConnectionNamedPipe::Receive(void* pBuffer, const int nLength, int nTimeout) {
  if (!GetIsOpen())
    return -1;

  if (nTimeout != 0) {
    VistaTimer oTimer;
    int        nRet = 0;
    while (nRet != nLength) {
      int nRealTimeout = nTimeout - (int)(oTimer.GetLifeTime() * 1000);
      if (nRealTimeout <= 0)
        return 0; // timeout
      nRet = WaitForIncomingData(nRealTimeout);
      if (nRet == ~0)
        return 0; // timeout
    }
  }

#ifndef WIN32

  int nRet = TEMP_FAILURE_RETRY(read(m_nReadFifo, pBuffer, nLength));
  if (nRet < 0) {
    vstr::warnp() << "VistaConnectionNamedPipe::Receive() -- error during read call: "
                  << strerror(errno) << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("VistaConnectionNamedPipe::Receive -- Exception", -1)
#endif
  }

  return nRet;
#else
  DWORD nReadbytes = 0;
  if (ReadFile(m_oPipe, pBuffer, nLength, &nReadbytes, NULL) == false) {
    vstr::warnp() << "VistaConnectionNamedPipe::Receive() -- error during read call: "
                  << vcnamedpipe::GetErrorText() << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("VistaConnectionNamedPipe::Receive -- Exception", -1)
#endif
  }
  return (int)nReadbytes;
#endif
}

int VistaConnectionNamedPipe::Send(const void* pBuffer, const int nLength) {
  if (!GetIsOpen())
    return -1;

#if !defined(WIN32)
  int nRet = TEMP_FAILURE_RETRY(write(m_nWriteFifo, pBuffer, nLength));
  if (nRet != nLength) {
    vstr::warnp() << "VistaConnectionNamedPipe::Send() -- error during end call: "
                  << strerror(errno) << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("VistaConnectionNamedPipe::Send -- Exception", -1)
#endif
  }
  return nRet;
#else
  DWORD nWrittenbytes = 0;
  if (WriteFile(m_oPipe, pBuffer, nLength, &nWrittenbytes, NULL) == false) {
    vstr::warnp() << "VistaConnectionNamedPipe::send() -- error during write call: "
                  << vcnamedpipe::GetErrorText() << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("VistaConnectionNamedPipe::Send -- Exception", -1)
#endif
  }
  return (int)nWrittenbytes;
#endif
}

bool VistaConnectionNamedPipe::HasPendingData() const {
  return (PendingDataSize() > 0);
}

unsigned long VistaConnectionNamedPipe::PendingDataSize() const {
#ifndef WIN32
  int nResult = 0;
  if (ioctl(int(m_nReadFifo), FIONREAD, &nResult) != -1)
    return nResult;
  else
    return 0;
#else
  DWORD nBytes = 0;
  if (PeekNamedPipe(m_oPipe, NULL, 0, NULL, &nBytes, NULL) == false) {
    vstr::warnp()
        << "VistaConnectionNamedPipe::PendingDataSize() -- PeekNamedPipe failed with Error "
        << GetLastError() << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("IVistaSocket::PendingDataSize -- Exception", 0x00000104)
#endif
  }
  return (unsigned long)nBytes;
#endif
}

std::string VistaConnectionNamedPipe::GetPipeName() const {
  return m_sPipeName;
}

unsigned long VistaConnectionNamedPipe::WaitForIncomingData(int nTimeout) {
#ifndef WIN32
  struct timeval  tv;
  struct timeval* pTime = NULL;
  if (nTimeout != 0) {
    tv.tv_sec  = nTimeout / 1000;
    tv.tv_usec = (nTimeout % 1000) * 1000;
    pTime      = &tv;
  }
  fd_set oReadSet;
  fd_set oExceptionSet;
  FD_ZERO(&oReadSet);
  FD_ZERO(&oExceptionSet);
  FD_SET(m_nReadFifo, &oReadSet);
  FD_SET(m_nReadFifo, &oExceptionSet);
  int iRet = TEMP_FAILURE_RETRY(select(m_nReadFifo + 1, &oReadSet, NULL, &oExceptionSet, pTime));
  if (iRet == 0) {
    // timeout
    return ~0;
  }
  if (iRet == -1) {
    vstr::warnp() << "VistaConnectionNamedPipe::WaitForIncomingData() -- error during select call: "
                  << strerror(errno) << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("VistaConnectionNamedPipe::WaitForIncomingData -- Exception", -1)
#endif
  }

  if (FD_ISSET(m_nReadFifo, &oExceptionSet)) {
    vstr::warnp()
        << "VistaConnectionNamedPipe::WaitForIncomingData() -- socket is in exception array"
        << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000104)
#endif
  }

  return PendingDataSize();
#else
  int nRet = WaitForSingleObject(m_oPipe, nTimeout);
  if (nRet == WAIT_TIMEOUT) {
    return 0;
  } else if (nRet == WAIT_FAILED) {
    vstr::warnp() << "VistaConnectionNamedPipe::WaitForIncomingData() -- WaitForSingleObject "
                     "failed with Error "
                  << GetLastError() << std::endl;
#ifdef VISTA_IPC_USE_EXCEPTIONS
    VISTA_THROW("IVistaSocket::WaitForIncomingData -- Exception", 0x00000104)
#endif
  }
  return PendingDataSize();
#endif
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
