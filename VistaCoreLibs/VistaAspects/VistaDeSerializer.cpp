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

#include <string>
using std::string;

#include "VistaDeSerializer.h"

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

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::ushort16& val) {
  ReadShort16(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::sint32& val) {
  ReadInt32(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::uint32& val) {
  ReadInt32(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::sint64& val) {
  ReadInt64(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::uint64& val) {
  ReadUInt64(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::float32& val) {
  ReadFloat32(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::float64& val) {
  ReadFloat64(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(string& val) {
  ReadEncodedString(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(bool& val) {
  ReadBool(val);
  return *this;
}

IVistaDeSerializer& IVistaDeSerializer::operator>>(VistaType::byte& val) {
  ReadRawBuffer(&val, sizeof(VistaType::byte));
  return *this;
}

void IVistaDeSerializer::SetByteorderSwapFlag(bool bDoesIt) {
  SetByteorderSwapFlag(bDoesIt == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                                       : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
}
