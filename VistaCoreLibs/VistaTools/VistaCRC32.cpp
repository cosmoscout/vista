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

#include "VistaCRC32.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaCRC32::VistaCRC32(unsigned long ulKey)
    : m_ulRegister(0) {
  Init(ulKey);
}

VistaCRC32::~VistaCRC32() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaCRC32::Init(unsigned long ulKey) {
  // for all possible byte values
  int i, j;
  for (i = 0; i < 256; ++i) {
    unsigned long ulReg = i << 24;
    // for all bits in a byte
    for (j = 0; j < 8; ++j) {
      bool bTopBit = (ulReg & 0x80000000) != 0;
      ulReg <<= 1;
      if (bTopBit)
        ulReg ^= ulKey;
    }
    m_ulLUTElements[i] = ulReg;
  }
}

void VistaCRC32::AddString(const std::string& sText) {
  for (size_t i = 0; i < sText.length(); ++i)
    AddByte(sText[i]);
}

unsigned long VistaCRC32::ProcessString(const std::string& sText) {
  m_ulRegister = 0;
  AddString(sText);
  return GetResult();
}

void VistaCRC32::AddByteBuffer(VistaType::byte* pByteBuffer, int iLength) {
  for (int i = 0; i < iLength; ++i) {
    AddByte(*pByteBuffer);
    ++pByteBuffer;
  }
}

unsigned long VistaCRC32::ProcessByteBuffer(VistaType::byte* pByteBuffer, int iLength) {
  m_ulRegister = 0;
  AddByteBuffer(pByteBuffer, iLength);
  return GetResult();
}

void VistaCRC32::AddByte(VistaType::byte cByte) {
  VistaType::byte cTop = (VistaType::byte)(m_ulRegister >> 24);
  cTop ^= cByte;
  m_ulRegister = (m_ulRegister << 8) ^ m_ulLUTElements[cTop];
}

unsigned long VistaCRC32::GetResult() {
  unsigned long tmp = m_ulRegister;
  m_ulRegister      = 0;
  return tmp;
}
