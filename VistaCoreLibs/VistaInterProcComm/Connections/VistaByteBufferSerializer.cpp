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

#include "VistaByteBufferSerializer.h"
#include <VistaAspects/VistaSerializable.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaByteBufferSerializer::VistaByteBufferSerializer(unsigned int uiInitialBufferSize)
    : IVistaSerializer()
    , m_iWriteHead(0)
    , m_iOverwritePosition(-1)
    ,
    // m_uiCapacity(0),
    m_bRetrimSize(true)
    , m_bDoSwap(VistaSerializingToolset::GetDefaultPlatformSwapBehavior()) {
  m_vecBuffer.resize(uiInitialBufferSize);
  m_uiCapacity = (unsigned int)m_vecBuffer.capacity();
  if (uiInitialBufferSize > 0)
    m_pHead = &(m_vecBuffer[0]);
  else
    m_pHead = NULL;
}

VistaByteBufferSerializer::~VistaByteBufferSerializer() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
int VistaByteBufferSerializer::WriteValue(const VistaType::byte* pValue, int iLength) {
  VistaType::byte* pTarget;
  if (m_iOverwritePosition < 0) // no overwrite requested
  {
    int iSum = m_iWriteHead + iLength;
    if (m_uiCapacity < (unsigned int)iSum) {
      if (!m_bRetrimSize) {
        // we shall not retrim the size -> fail
        return -1;
      } else {
        // resize, make some space
        // 2*oldCapacity+oldSpace+newNeed (iSum)
        m_vecBuffer.resize((2 * m_uiCapacity) + iSum);

        // cache capacity
        m_uiCapacity = (unsigned int)m_vecBuffer.capacity();

        // STL vectors might copy memory to
        // a new region if necessary, se recache the head pointer
        m_pHead = &(m_vecBuffer[0]);
      }
    }
    pTarget = m_pHead + m_iWriteHead;
    m_iWriteHead += iLength;
  } else // overwrite flag has been set
  {
    int iSum = m_iOverwritePosition + iLength;
    if (m_uiCapacity < (unsigned int)iSum) {
      if (!m_bRetrimSize) {
        // we shall not retrim the size -> fail
        return -1;
      } else {
        // resize, make some space
        // 2*oldCapacity+oldSpace+newNeed (iSum)
        m_vecBuffer.resize((2 * m_uiCapacity) + iSum);

        // cache capacity
        m_uiCapacity = (unsigned int)m_vecBuffer.capacity();

        // STL vectors might copy memory to
        // a new region if necessary, se recache the head pointer
        m_pHead = &(m_vecBuffer[0]);
      }
    }
    pTarget              = m_pHead + m_iOverwritePosition;
    m_iOverwritePosition = -1;
  }

  memcpy(pTarget, pValue, iLength * sizeof(VistaType::byte));

  return iLength;
}

VistaSerializingToolset::ByteOrderSwapBehavior
VistaByteBufferSerializer::GetByteorderSwapFlag() const {
  return m_bDoSwap;
}

void VistaByteBufferSerializer::SetByteorderSwapFlag(
    VistaSerializingToolset::ByteOrderSwapBehavior bDoesIt) {
  m_bDoSwap = bDoesIt;
}

int VistaByteBufferSerializer::WriteShort16(VistaType::ushort16 us16Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&us16Val, sizeof(VistaType::ushort16));
  return WriteValue(
      reinterpret_cast<const VistaType::byte*>(&us16Val), sizeof(VistaType::ushort16));
}

int VistaByteBufferSerializer::WriteInt32(VistaType::sint32 si32Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si32Val, sizeof(VistaType::sint32));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&si32Val), sizeof(si32Val));
}

int VistaByteBufferSerializer::WriteInt32(VistaType::uint32 si32Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si32Val, sizeof(VistaType::uint32));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&si32Val), sizeof(si32Val));
}

int VistaByteBufferSerializer::WriteInt64(VistaType::sint64 si64Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&si64Val, sizeof(VistaType::sint64));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&si64Val), sizeof(si64Val));
}

int VistaByteBufferSerializer::WriteUInt64(VistaType::uint64 ui64Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&ui64Val, sizeof(VistaType::uint64));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&ui64Val), sizeof(ui64Val));
}

int VistaByteBufferSerializer::WriteFloat32(VistaType::float32 fVal) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&fVal, sizeof(VistaType::float32));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&fVal), sizeof(fVal));
}

int VistaByteBufferSerializer::WriteFloat64(VistaType::float64 f64Val) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&f64Val, sizeof(VistaType::float64));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&f64Val), sizeof(f64Val));
}

int VistaByteBufferSerializer::WriteDouble(double dVal) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&dVal, sizeof(double));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&dVal), sizeof(dVal));
}

int VistaByteBufferSerializer::WriteDelimitedString(const string& sString, char cDelim) {
  int iLength = (int)sString.size();
  int iSize   = WriteValue(reinterpret_cast<const VistaType::byte*>(sString.c_str()), iLength);
  iSize += WriteValue(reinterpret_cast<const VistaType::byte*>(&cDelim), 1);
  return iSize;
}

int VistaByteBufferSerializer::WriteString(const string& sString) {
  int iLength = (int)sString.size();
  int iSize   = WriteValue(reinterpret_cast<const VistaType::byte*>(sString.c_str()), iLength);
  return iSize;
}

int VistaByteBufferSerializer::WriteEncodedString(const std::string& sString) {
  VistaType::sint32 iLength = (VistaType::sint32)sString.size();
  int               iSize   = WriteInt32(iLength);
  if (iSize != sizeof(VistaType::sint32))
    return iSize;
  iSize += WriteValue(reinterpret_cast<const VistaType::byte*>(sString.c_str()), iLength);
  return iSize;
}

int VistaByteBufferSerializer::WriteRawBuffer(const void* pBuffer, const int iLen) {
  return WriteValue(reinterpret_cast<const VistaType::byte*>(pBuffer), iLen);
}

int VistaByteBufferSerializer::WriteBool(bool bVal) {
  if (GetByteorderSwapFlag())
    VistaSerializingToolset::Swap((void*)&bVal, sizeof(bool));
  return WriteValue(reinterpret_cast<const VistaType::byte*>(&bVal), sizeof(bVal));
}

int VistaByteBufferSerializer::WriteSerializable(const IVistaSerializable& obj) {
  return obj.Serialize(*this);
}

const VistaType::byte* VistaByteBufferSerializer::GetBuffer() const {
  return m_pHead;
}

int VistaByteBufferSerializer::GetBufferSize() const {
  return m_iWriteHead;
  //    return m_vecBuffer.size();
}

void VistaByteBufferSerializer::GetBuffer(vector<VistaType::byte>& vecStore) {
  vecStore.resize(m_iWriteHead);
  memcpy(&vecStore[0], m_pHead, m_iWriteHead);
}

void VistaByteBufferSerializer::ClearBuffer() {
  // m_vecBuffer.clear();
  // m_pHead = &m_vecBuffer[0];
  m_iWriteHead = 0;
}

void VistaByteBufferSerializer::SetBufferCapacity(int iNewSize) {
  if (m_bRetrimSize) {
    m_vecBuffer.resize(iNewSize);
    m_uiCapacity = (unsigned int)m_vecBuffer.capacity();
    m_pHead      = &(m_vecBuffer[0]);
  }
}

int VistaByteBufferSerializer::GetBufferCapacity() const {
  return m_uiCapacity; // this is always correct, even for adopted buffers
}

void VistaByteBufferSerializer::SetBuffer(
    VistaType::byte* pBuffer, int iBufferSize, int iWriteHead) {
  assert(iBufferSize > 0);
  if (pBuffer == NULL) {
    // relink to internal buffer
    m_vecBuffer.resize(iBufferSize);
    m_pHead       = &(m_vecBuffer[0]);
    m_uiCapacity  = (int)m_vecBuffer.capacity();
    m_iWriteHead  = 0;
    m_bRetrimSize = true;
  } else {
    // link to external buffer
    m_bRetrimSize = false;
    m_pHead       = pBuffer;
    m_iWriteHead  = iWriteHead;
    m_uiCapacity  = iBufferSize;
  }
}

void VistaByteBufferSerializer::DumpCurrentBuffer() const {
  printf("VistaByteBufferSerializer::DumpCurrentBuffer() -- size = %d\n", int(m_vecBuffer.size()));

  for (int i = 0; i < m_iWriteHead; ++i) {
    printf("%02x ", m_vecBuffer[i]);
    if (i && (i % 16 == 0))
      printf("\n");
  }
  printf("\n");
}

bool VistaByteBufferSerializer::SetBufferRewritePosition(const int nOverridePosition) {
  if (nOverridePosition >= m_iWriteHead)
    return false;
  m_iOverwritePosition = nOverridePosition;
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
