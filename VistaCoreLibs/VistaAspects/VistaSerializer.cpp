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

#include "VistaSerializer.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

IVistaSerializer& IVistaSerializer::operator<<(VistaType::ushort16 val) {
  WriteShort16(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(VistaType::sint32 val) {
  WriteInt32(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(VistaType::uint32 val) {
  WriteInt32(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(VistaType::sint64 val) {
  WriteInt64(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(VistaType::uint64 val) {
  WriteUInt64(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(VistaType::float32 val) {
  WriteFloat32(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(VistaType::float64 val) {
  WriteFloat64(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(const std::string& val) {
  WriteEncodedString(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(bool val) {
  WriteBool(val);
  return *this;
}

IVistaSerializer& IVistaSerializer::operator<<(VistaType::byte val) {
  WriteRawBuffer(&val, sizeof(VistaType::byte));
  return *this;
}

void IVistaSerializer::SetByteorderSwapFlag(bool bDoesIt) {
  SetByteorderSwapFlag(bDoesIt == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                                       : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
}
