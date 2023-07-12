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

#include "VistaCSVSerializer.h"
#include <VistaAspects/VistaSerializable.h>

#include <cstdio>
#include <cstdlib>

using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaCSVSerializer::VistaCSVSerializer(char separator)
    : m_cSeparator(separator) {
}

VistaCSVSerializer::~VistaCSVSerializer() {
}

int VistaCSVSerializer::WriteShort16(VistaType::ushort16 us16Val) {
  m_streamOutput << us16Val << m_cSeparator;
  return sizeof(us16Val);
}

int VistaCSVSerializer::WriteInt32(VistaType::sint32 si32Val) {
  m_streamOutput << si32Val << m_cSeparator;
  return sizeof(si32Val);
}

int VistaCSVSerializer::WriteInt32(VistaType::uint32 si32Val) {
  m_streamOutput << si32Val << m_cSeparator;
  return sizeof(si32Val);
}

int VistaCSVSerializer::WriteInt64(VistaType::sint64 si64Val) {
#if defined(WIN32)
  const char* specifier = "%I64d";
#elif defined(LINUX)
  const char* specifier = "%lld";
#else
  const char* specifier = "%d";
#endif
  // 2^64 yields 20 digits + trailing zero
  // so we use 32byte, should be enough
  char buffer[32];

  sprintf(buffer, specifier, si64Val);

  m_streamOutput << buffer << m_cSeparator;
  return sizeof(si64Val);
}

int VistaCSVSerializer::WriteUInt64(VistaType::uint64 ui64Val) {
#if defined(WIN32)
  const char* specifier = "%I64u";
#elif defined(LINUX)
  const char* specifier = "%lld";
#else
  const char* specifier = "%d";
#endif
  // 2^64 yields 20 digits + trailing zero
  // so we use 32byte, should be enough
  char buffer[32];
  sprintf(buffer, specifier, ui64Val);

  m_streamOutput << buffer << m_cSeparator;
  return sizeof(ui64Val);
}

int VistaCSVSerializer::WriteFloat32(VistaType::float32 fVal) {
  m_streamOutput << fVal << m_cSeparator;
  return sizeof(fVal);
}

int VistaCSVSerializer::WriteFloat64(VistaType::float64 f64Val) {
  m_streamOutput << f64Val << m_cSeparator;
  return sizeof(f64Val);
}

int VistaCSVSerializer::WriteDouble(double dVal) {
  m_streamOutput << dVal << m_cSeparator;
  return sizeof(dVal);
}

int VistaCSVSerializer::WriteString(const string& sString) {
  m_streamOutput << sString << m_cSeparator;
  return (int)sString.length();
}

int VistaCSVSerializer::WriteDelimitedString(const string& sString, char cDelim) {
  int  iRet = WriteString(sString);
  char c[2];
  c[0] = cDelim;
  c[1] = 0x0;

  iRet += WriteString(std::string(c));

  return iRet;
}

int VistaCSVSerializer::WriteEncodedString(const std::string& sString) {
  int nRet = WriteInt32((VistaType::sint32)sString.size());
  nRet += WriteString(sString);
  return nRet;
}

int VistaCSVSerializer::WriteRawBuffer(const void* pBuffer, const int iLen) {
  m_streamOutput.write((const char*)pBuffer, iLen);
  m_streamOutput << m_cSeparator;
  return iLen;
}

int VistaCSVSerializer::WriteBool(bool bVal) {
  m_streamOutput << bVal << m_cSeparator;
  return sizeof(bVal);
}

int VistaCSVSerializer::WriteSerializable(const IVistaSerializable& obj) {
  return obj.Serialize(*this);
}

const char* VistaCSVSerializer::GetBuffer() const {
  return m_streamOutput.str().c_str();
}

void VistaCSVSerializer::GetBufferString(std::string& strBuffer) const {
  strBuffer = m_streamOutput.str();
}

int VistaCSVSerializer::GetBufferSize() const {
  return (int)m_streamOutput.str().length();
}

void VistaCSVSerializer::ClearBuffer() {
  m_streamOutput.flush();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
