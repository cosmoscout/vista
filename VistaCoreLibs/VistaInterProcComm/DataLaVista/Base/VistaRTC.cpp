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

#include "VistaRTC.h"

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

IDLVistaRTC* IDLVistaRTC::m_pSingleton = 0;

IDLVistaRTC::IDLVistaRTC() {
  SetRTCSingleton(this);
}

IDLVistaRTC::IDLVistaRTC(IDLVistaRTC&) {

  // throw;
}

IDLVistaRTC::~IDLVistaRTC() {
  m_pSingleton = 0;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void IDLVistaRTC::SetRTCSingleton(IDLVistaRTC* pSingleton) {
  // this can only be done once!
  if (!m_pSingleton)
    m_pSingleton = pSingleton;
}

IDLVistaRTC* IDLVistaRTC::GetRTCSingleton() {
  return m_pSingleton;
}

const char* IDLVistaRTC::GetPrintfFormatSpecifier() {
#if defined(WIN32)
  return "%I64d";
#elif defined(LINUX)
  return "%lld";
#else
  return "%d";
#endif
}
