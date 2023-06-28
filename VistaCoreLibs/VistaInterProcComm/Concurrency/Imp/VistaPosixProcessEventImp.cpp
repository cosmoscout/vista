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

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#if !defined(VISTA_THREADING_WIN32)

#include "VistaPosixProcessEventImp.h"

#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const std::string sFifoFolder = ".VistaFifos";

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

int OpenFIFOWrite(const std::string& sName) {
  int nFile = TEMP_FAILURE_RETRY(open(sName.c_str(), O_WRONLY));

  if (nFile < 0) {
    vstr::warnp() << "Opening FIFO [" << sName << "] failed with error [" << errno << "] ("
                  << strerror(errno) << ")" << std::endl;
  }
  return nFile;
}

int OpenFIFORead(const std::string& sName, int m_nMaxWait) {
  VistaTimer oTimer;
  double     nMaxWaitSecs = m_nMaxWait * 1e-3;
  int        nFile        = -1;
  while (oTimer.GetLifeTime() < nMaxWaitSecs) {
    nFile = TEMP_FAILURE_RETRY(open(sName.c_str(), O_RDONLY));
    if (nFile >= 0)
      break;
    VistaTimeUtils::Sleep(5);
  }
  if (nFile < 0) {
    vstr::warnp() << "Opening FIFO [" << sName << "] failed with error [" << errno << "] ("
                  << strerror(errno) << ")" << std::endl;
  }
  return nFile;
}

bool CreateFifo(const std::string& sName) {
  struct stat oAttributes;
  if (stat(sFifoFolder.c_str(), &oAttributes) != 0 || ((oAttributes.st_mode & S_IFDIR) == false)) {
    if (mkdir(sFifoFolder.c_str(), S_IRWXU | S_IRGRP | S_IXGRP) != 0)
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
  vstr::warnp() << "Creating FIFO [" << sName << "] failed with error [" << errno << "] ("
                << strerror(errno) << ")" << std::endl;
  return false;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaPosixProcessEventImp::VistaPosixProcessEventImp(const std::string& sEventName)
    : IVistaProcessEventImp(sEventName, true)
    , m_oReadSet(NULL)
    , m_bManagesFiles(false)
    , m_nFifo(-1) {
  std::string sFifoName = sFifoFolder + "/VISTAPIPE_" + sEventName;
  if (CreateFifo(sFifoName) == false)
    return; // fatal failure

  m_nFifo = OpenFIFOWrite(sFifoName);
}

VistaPosixProcessEventImp::VistaPosixProcessEventImp(
    const std::string& sEventName, const int m_nMaxWait)
    : IVistaProcessEventImp(sEventName, false)
    , m_oReadSet(NULL)
    , m_bManagesFiles(false)
    , m_nFifo(-1) {
  std::string sFifoName = "VISTAPIPE_" + sEventName;

  m_nFifo = OpenFIFORead(sFifoName, m_nMaxWait);
  if (m_nFifo == -1)
    return;
  m_oReadSet = new fd_set;
  FD_ZERO(m_oReadSet);
  FD_SET(m_nFifo, m_oReadSet);

  // be sure to clean any remaining data on reading
  char           nDummy;
  struct timeval nDelta;
  nDelta.tv_usec = 0;
  nDelta.tv_sec  = 0;
  while (TEMP_FAILURE_RETRY(select(1, m_oReadSet, NULL, NULL, &nDelta))) {
    TEMP_FAILURE_RETRY(read(m_nFifo, &nDummy, sizeof(char)));
  }
}

VistaPosixProcessEventImp::~VistaPosixProcessEventImp() {
  if (m_nFifo >= 0) {
    close(m_nFifo);
  }

  // if( GetIsSignaller() )
  { remove(("VISTAPIPE_" + GetEventName()).c_str()); }

  delete m_oReadSet;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaPosixProcessEventImp::GetIsValid() const {
  return (m_nFifo >= 0);
}

bool VistaPosixProcessEventImp::SignalEvent() {
  if (GetIsValid() == false)
    return false;
  assert(GetIsSignaller() == true);
  char nDummy = 0;
  TEMP_FAILURE_RETRY(write(m_nFifo, &nDummy, sizeof(char)));
  return true;
}

bool VistaPosixProcessEventImp::WaitForEvent(int iBlockTime) {
  if (GetIsValid() == false)
    return false;
  assert(GetIsSignaller() == false);

  struct timeval nDelta;
  nDelta.tv_usec = iBlockTime % 1000;
  nDelta.tv_sec  = (int)((double)iBlockTime / 1000.0);

  int nRet = TEMP_FAILURE_RETRY(select(1, m_oReadSet, NULL, NULL, &nDelta));
  if (nRet == 0) // timeout
    return false;
  else if (nRet == -1) {
    vstr::warnp() << "[VistaPosixProcessEventImp]: "
                  << "Error while waiting for fifo: " << strerror(errno) << std::endl;
    return false;
  }

  char nTarget;
  if (TEMP_FAILURE_RETRY(read(m_nFifo, &nTarget, sizeof(char))) == -1) {
    vstr::warnp() << "[VistaPosixProcessEventImp]: "
                  << "Error while reading from fifo: " << strerror(errno) << std::endl;
    return false;
  }

  return true;
}

bool VistaPosixProcessEventImp::WaitForEvent(bool bBlock) {
  if (GetIsValid() == false)
    return false;
  assert(GetIsSignaller() == false);

  if (bBlock == false) {
    struct timeval nDelta;
    nDelta.tv_usec = 0;
    nDelta.tv_sec  = 0;

    if (TEMP_FAILURE_RETRY(select(1, m_oReadSet, NULL, NULL, &nDelta)) == 0)
      return false;
  }

  char nTarget;
  if (TEMP_FAILURE_RETRY(read(m_nFifo, &nTarget, sizeof(char))) == -1) {
    vstr::warnp() << "[VistaPosixProcessEventImp]: "
                  << "Error while reading from fifo: " << strerror(errno) << std::endl;
    return false;
  }

  return true;
}

#endif

// ============================================================================
// ============================================================================
