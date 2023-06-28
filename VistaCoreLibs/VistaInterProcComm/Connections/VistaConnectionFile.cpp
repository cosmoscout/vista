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
#pragma warning(disable : 4996)
#else
#include <cerrno>
#include <poll.h>
#include <unistd.h>
#endif

//#include <sys/ioctl.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <ctime>
#include <iostream>

#include "VistaConnectionFile.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaUtilityMacros.h>

VistaConnectionFile::VistaConnectionFile(const std::string& sFilename, int iMode) {
  m_sFilename = sFilename;

  /** @todo check whether mode is valid */
  m_iMode      = iMode;
  m_sStream    = NULL;
  m_bBuffering = false;

  SetIsOpen(false);
}

VistaConnectionFile::~VistaConnectionFile() {
  Close();
}

bool VistaConnectionFile::Open() {
  if (GetIsOpen())
    return true;

  const char* mode = NULL;
  switch (m_iMode) {
  case READ:
    mode = "rb";
    break;

  case WRITE:
    mode = "wb";
    break;

  case READWRITE:
    mode = "w+b";
    break;

  case APPEND:
    mode = "a+b";
    break;
  default:
    return false;
  }
  m_sStream = fopen(m_sFilename.c_str(), mode);
  if (m_sStream == NULL)
    return false;

  if (m_bBuffering == false)
    setvbuf(m_sStream, NULL, _IONBF, 0);

  SetIsOpen(true);
  return true;
}

void VistaConnectionFile::Close() {
  if (!GetIsOpen())
    return;

  if (m_sStream != NULL) {
    if (fclose(m_sStream) != 0) {
      vstr::errp() << "VistaConnectionFile::Close() -- Unable to close file: " << m_sFilename
                   << std::endl;
    }
    m_sStream = NULL;
  }

  SetIsOpen(false);
}

int VistaConnectionFile::Receive(void* buffer, const int length, int iTimeout) {
  if (!GetIsOpen())
    return -1;

  /* check whether in 'Receiving' mode */
  if (m_iMode == READ || m_iMode == READWRITE) {
    int read_bytes = (int)fread(buffer, 1, length, m_sStream);
    if (read_bytes != length) {
      if (feof(m_sStream))
        return 0;
      /* if unable to read requested bytes but not at EOF -> return error */
      return -1;
    }
    return read_bytes;
  } else
    return -1;
}

int VistaConnectionFile::Send(const void* buffer, const int length) {
  if (!GetIsOpen())
    return -1;

  /* check whether in 'Sending' mode */
  if (m_iMode == WRITE || m_iMode == READWRITE || m_iMode == APPEND) {
    int sent_bytes = (int)fwrite(buffer, 1, length, m_sStream);
    if (sent_bytes != length) {
      /* if unable to write all data out -> error */
      return -1;
    }
    return sent_bytes;
  } else
    return -1;
}

VistaType::uint64 VistaConnectionFile::GetFileSize() const {
  if (!GetIsOpen())
    return ~0;

  struct stat attributes;

  if (stat(m_sFilename.c_str(), &attributes) != 0)
    return 0;

  return ((VistaType::uint64)attributes.st_size);
}

unsigned long VistaConnectionFile::PendingDataSize() const {
  /** @todo implement me! */
#if !defined(WIN32)
  VISTA_COMPILATION_WARNING(
      "Function VistaConnectionFile::PendingDataSize() not implemented for this architecture");
  return ~0; // (0xFFFFFF)
#else
  return ((unsigned long)GetFileSize() - (unsigned long)ftell(m_sStream));
#endif
}

bool VistaConnectionFile::HasPendingData() const {
#if !defined(WIN32)
  struct pollfd fds;
  int           timeout_msecs = 0;

  fds.fd     = fileno(m_sStream);
  fds.events = POLLIN;

  TEMP_FAILURE_RETRY(poll(&fds, 1, timeout_msecs));

  return (fds.revents & POLLIN);
#else
  /**
   * @todo native implementation
   */

  return (ftell(m_sStream) != GetFileSize());
#endif
}

HANDLE VistaConnectionFile::GetConnectionDescriptor() const {
  if (!m_sStream)
    return HANDLE(~0);

  return HANDLE(fileno(m_sStream));
}

HANDLE VistaConnectionFile::GetConnectionWaitForDescriptor() {
  if (!m_sStream)
    return HANDLE(~0);

  return HANDLE(fileno(m_sStream));
}

bool VistaConnectionFile::Flush() {
  return (fflush(m_sStream) != -1);
}

bool VistaConnectionFile::GetIsBuffering() const {
  return m_bBuffering;
}

void VistaConnectionFile::SetIsBuffering(bool bBuffering) {
  if (m_bBuffering == bBuffering)
    return;

  m_bBuffering = bBuffering;

  if (GetIsOpen()) {
    if (bBuffering)
      setvbuf(m_sStream, NULL, _IOFBF, 1024);
    else
      setvbuf(m_sStream, NULL, _IONBF, 0);
  }
}
