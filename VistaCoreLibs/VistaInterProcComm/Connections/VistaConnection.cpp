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

#include "VistaConnection.h"

#include <VistaAspects/VistaSerializable.h>

#include <VistaBase/VistaStreamUtils.h>

#include <vector>

#if !defined(VISTA_USE_POSIX)
#ifndef WIN32
#if defined(IRIX)
// we prefer AT&T System-V Rel.4 (SVR4)
#define VISTA_USE_SYSTEMV
#else
// we prefer POSIX
#define VISTA_USE_POSIX
#ifdef SUNOS
#define __EXTENSIONS__
#define _POSIX_C_SOURCE 199506L
#endif
#endif
#endif
#endif

// system headers

#ifdef WIN32
#include <Windows.h>
#include <stdlib.h>
#include <winsock2.h>
//#   include <winsock.h>
#else
#include <sys/types.h> // socket(),gethostbyname(),htons(),connect()
#if (!defined(LINUX) && !defined(HPUX))
#include <sys/conf.h> // ioctl ( FIONREAD )
#endif
#include <arpa/inet.h>
#include <netinet/in.h> // gethostbyname(),htons() libc-Version!
#include <sys/socket.h> // socket(),gethostbyname(),connect()
#include <unistd.h>     // ioctl(), FIONREAD
//#   include <stropts.h>         // ioctl(), FIONREAD
#include <sys/ioctl.h>  // ioctl(), FIONREAD
#if (!defined(LINUX) && !defined(HPUX))
#include <sys/filio.h>
#endif
#include <netdb.h> // getprotoent() libc-Version!,gethostbyname()
#ifdef IRIX
#include <sys/ipc.h>
#include <sys/prctl.h> // threads
#include <sys/sem.h>
#endif
#ifdef VISTA_USE_POSIX
#include <pthread.h>
#include <semaphore.h>
#endif
#include <sched.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#endif

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

//******************************************************************************
//
// CONSTRUCTOR AND DESTRUCTOR
//
//******************************************************************************

VistaConnection::VistaConnection()
    : m_bIsOpen(false)
    , m_bDoesSwapByteOrder(VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES)
    , m_bIsBuffering(false)
    , m_bIsBlocking(false)
    , m_nReadTimeout(0) {
}

VistaConnection::~VistaConnection() {
}

/*============================================================================*/

void VistaConnection::SetIsOpen(bool bOpen) {
  m_bIsOpen = bOpen;
}

int VistaConnection::WriteShort16(VistaType::ushort16 us16Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&us16Val, sizeof(VistaType::ushort16));
  return Send(&us16Val, sizeof(VistaType::ushort16));
}

int VistaConnection::WriteInt32(VistaType::sint32 si32Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si32Val, sizeof(VistaType::sint32));
  return Send(&si32Val, sizeof(VistaType::sint32));
}

int VistaConnection::WriteInt32(VistaType::uint32 si32Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si32Val, sizeof(VistaType::uint32));
  return Send(&si32Val, sizeof(VistaType::uint32));
}

int VistaConnection::WriteInt64(VistaType::sint64 si64Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si64Val, sizeof(VistaType::sint64));
  return Send(&si64Val, sizeof(VistaType::sint64));
}

int VistaConnection::WriteUInt64(VistaType::uint64 ui64Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&ui64Val, sizeof(VistaType::uint64));
  return Send(&ui64Val, sizeof(VistaType::uint64));
}

int VistaConnection::WriteFloat32(VistaType::float32 fVal32) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&fVal32, sizeof(VistaType::float32));
  return Send(&fVal32, sizeof(VistaType::float32));
}

int VistaConnection::WriteFloat64(VistaType::float64 f64Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&f64Val, sizeof(VistaType::float64));
  return Send(&f64Val, sizeof(VistaType::float64));
}

int VistaConnection::WriteDouble(double dVal) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&dVal, sizeof(double));
  return Send(&dVal, sizeof(double));
}

int VistaConnection::WriteString(const string& sString) {
  return Send((const void*)sString.data(), (int)sString.length());
}

int VistaConnection::WriteEncodedString(const std::string& sString) {
  int iSize = WriteInt32((VistaType::sint32)sString.length());
  if (sString.size() > 0)
    iSize += Send((const void*)sString.data(), (int)sString.length());
  return iSize;
}

int VistaConnection::WriteDelimitedString(const string& sString, char cDelim) {
  int iSize = Send((const void*)sString.data(), (int)sString.length());
  iSize += Send(&cDelim, sizeof(char));
  return iSize;
}

int VistaConnection::WriteRawBuffer(const void* pBuffer, const int iLen) {
  return Send(pBuffer, iLen);
}

int VistaConnection::WriteBool(bool bVal) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&bVal, sizeof(bool));
  return Send(&bVal, sizeof(bool));
}

int VistaConnection::WriteSerializable(const IVistaSerializable& rObj) {
  return rObj.Serialize(*this);
}

int VistaConnection::ReadShort16(VistaType::ushort16& us16Val) {
  int iRet = Receive(&us16Val, sizeof(VistaType::ushort16), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&us16Val, sizeof(VistaType::ushort16));
  return iRet;
}

int VistaConnection::ReadInt32(VistaType::sint32& si32Val) {
  int iRet = Receive(&si32Val, sizeof(VistaType::sint32), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si32Val, sizeof(VistaType::sint32));
  return iRet;
}

int VistaConnection::ReadInt32(VistaType::uint32& si32Val) {
  int iRet = Receive(&si32Val, sizeof(VistaType::uint32), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si32Val, sizeof(VistaType::uint32));
  return iRet;
}

int VistaConnection::ReadInt64(VistaType::sint64& si64Val) {
  int iRet = Receive(&si64Val, sizeof(VistaType::sint64), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si64Val, sizeof(VistaType::sint64));
  return iRet;
}

int VistaConnection::ReadUInt64(VistaType::uint64& ui64Val) {
  int iRet = Receive(&ui64Val, sizeof(VistaType::uint64), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&ui64Val, sizeof(VistaType::uint64));
  return iRet;
}

int VistaConnection::ReadFloat32(VistaType::float32& fVal) {
  int iRet = Receive(&fVal, sizeof(VistaType::float32), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&fVal, sizeof(VistaType::float32));
  return iRet;
}

int VistaConnection::ReadFloat64(VistaType::float64& f64Val) {
  int iRet = Receive(&f64Val, sizeof(VistaType::float64), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&f64Val, sizeof(VistaType::float64));
  return iRet;
}

int VistaConnection::ReadDouble(double& dVal) {
  int iRet = Receive(&dVal, sizeof(double), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&dVal, sizeof(double));
  return iRet;
}

int VistaConnection::ReadString(string& sString, const int iMaxLen) {
  if (iMaxLen == 0)
    return 0;
  vector<char> ve(iMaxLen);
  int          iRet = Receive(&ve[0], iMaxLen, m_nReadTimeout);
  sString.assign(&(ve[0]), iRet);
  return iRet;
}

int VistaConnection::ReadDelimitedString(string& sString, char cDelim) {
  sString.erase();

  char pcTmp[2];
  pcTmp[0] = 0x00;
  pcTmp[1] = 0x00;

  int iLength     = 1;
  int iReadLength = 0; /**< measure length */
  for (;;) {
    int iRead = ReadRawBuffer((void*)&pcTmp[0], iLength);
    if (iRead == iLength) {
      if (pcTmp[0] == cDelim) {
        break; // leave loop
      } else {
        sString.append(string(&pcTmp[0]));
        ++iReadLength;
      }
    } else {
      if (GetIsBlocking()) {
        vstr::errp() << " VistaConnection::ReadDelimitedString() -- "
                     << "Should read: " << iLength << ", but read: " << iRead
                     << " on a BLOCKING connection?\n"
                     << std::endl;
        break;
      }
    }
  }

  return iReadLength;
}

int VistaConnection::ReadEncodedString(std::string& sTarget) {
  VistaType::sint32 nSize;
  int               nRet = ReadInt32(nSize);
  if (nRet != sizeof(VistaType::sint32))
    return -1;
  int nStringRead = ReadString(sTarget, nSize);
  if (nStringRead != nSize) {
    return false;
  }
  return nRet + nStringRead;
}

int VistaConnection::ReadRawBuffer(void* pBuffer, int iLen) {
  return Receive(pBuffer, iLen, m_nReadTimeout);
}

int VistaConnection::ReadBool(bool& bVal) {
  int iRet = Receive(&bVal, sizeof(bool), m_nReadTimeout);
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&bVal, sizeof(bool));
  return iRet;
}

int VistaConnection::ReadSerializable(IVistaSerializable& rObj) {
  return rObj.DeSerialize(*this);
}

VistaSerializingToolset::ByteOrderSwapBehavior VistaConnection::GetByteorderSwapFlag() const {
  return m_bDoesSwapByteOrder;
}

void VistaConnection::SetByteorderSwapFlag(VistaSerializingToolset::ByteOrderSwapBehavior bDoesIt) {
  m_bDoesSwapByteOrder = bDoesIt;
}

bool VistaConnection::GetIsBlocking() const {
  return m_bIsBlocking;
}

HANDLE VistaConnection::GetConnectionDescriptor() const {
  return 0;
}

HANDLE VistaConnection::GetConnectionWaitForDescriptor() {
  return GetConnectionDescriptor();
}

bool VistaConnection::GetWaitForDescriptorEventSelectIsEnabled() const {
  return true;
}

bool VistaConnection::SetWaitForDescriptorEventSelectIsEnabled(const bool bSet) {
  return (bSet == true);
}

VistaConnection::eConState VistaConnection::DetermineConState() const {
  return CON_NONE;
}

bool VistaConnection::GetIsFine() const {
  return GetIsOpen();
}

/*============================================================================*/
void VistaConnection::Debug(ostream& out) const {
  out << " [VistaConnection]  ==BEGIN==>" << std::endl;
  out << " [VistaConnection]  <==END==" << std::endl;
}

int VistaConnection::GetReadTimeout() const {
  return m_nReadTimeout;
}

bool VistaConnection::SetReadTimeout(int nReadTimeout) {
  m_nReadTimeout = nReadTimeout;
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream& operator<<(ostream& out, const VistaConnection& device) {
  device.Debug(out);
  return out;
}
