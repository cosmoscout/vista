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

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

#include "VistaTimeStamp.h"

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaTimeStamp::DLVistaTimeStamp(const DLVistaTimeStamp& rStamp) {
  m_i64MicroStamp    = rStamp.m_i64MicroStamp;
  m_i32MacroStamp    = rStamp.m_i32MacroStamp;
  m_i32UnsafetyValue = rStamp.m_i32UnsafetyValue;
}

DLVistaTimeStamp::DLVistaTimeStamp(DLV_INT64 i64MicroStamp, DLV_INT32 i32MacroStamp) {
  m_i64MicroStamp    = i64MicroStamp;
  m_i32MacroStamp    = i32MacroStamp;
  m_i32UnsafetyValue = 0;
}

DLVistaTimeStamp::DLVistaTimeStamp() {
  m_i64MicroStamp    = -1; // invalid
  m_i32MacroStamp    = -1; // invalid
  m_i32UnsafetyValue = -1; // invalid
}

DLVistaTimeStamp::~DLVistaTimeStamp() {
  m_i64MicroStamp    = -1; // invalid
  m_i32MacroStamp    = -1; // invalid
  m_i32UnsafetyValue = -1; // invalid
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool DLVistaTimeStamp::Equals(const DLVistaTimeStamp& rStamp) {
  return false;
}

bool DLVistaTimeStamp::operator==(const DLVistaTimeStamp& rStamp) {
  return false;
}

bool DLVistaTimeStamp::operator>(const DLVistaTimeStamp& rStamp) {
  return false;
}

bool DLVistaTimeStamp::operator<(const DLVistaTimeStamp& rStamp) {
  return false;
}

bool DLVistaTimeStamp::operator<=(const DLVistaTimeStamp& rStamp) {
  return false;
}

bool DLVistaTimeStamp::operator>=(const DLVistaTimeStamp& rStamp) {
  return false;
}

DLVistaTimeStamp& DLVistaTimeStamp::operator=(const DLVistaTimeStamp& rStamp) {
  m_i64MicroStamp    = rStamp.GetMicroStamp();
  m_i32MacroStamp    = rStamp.GetMacroStamp();
  m_i32UnsafetyValue = rStamp.GetUnsafetyValue();
  return *this;
}
