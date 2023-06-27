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

#include "VistaDriverProtocolAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#if defined(SUNOS)
#include <typeinfo.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int IVistaDriverProtocolAspect::m_nAspectId = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaDriverProtocolAspect::IVistaDriverProtocolAspect()
    : IVistaDeviceDriver::IVistaDeviceDriverAspect(false) {
  if (IVistaDriverProtocolAspect::GetAspectId() == -1) // unregistered
    IVistaDriverProtocolAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("PROTOCOL"));

  SetId(IVistaDriverProtocolAspect::GetAspectId());
}

IVistaDriverProtocolAspect::~IVistaDriverProtocolAspect() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool IVistaDriverProtocolAspect::RegisterProtocol(const _cVersionTag& oTag) {
  if (GetHasProtocol(oTag) == false) {
    m_liProtocols.push_back(oTag);
    return true;
  }
  return false;
}

bool IVistaDriverProtocolAspect::GetHasProtocol(const _cVersionTag& oTag) const {
  return (std::find(m_liProtocols.begin(), m_liProtocols.end(), oTag) != m_liProtocols.end());
}

bool IVistaDriverProtocolAspect::UnregisterProtocol(const _cVersionTag& oTag) {
  m_liProtocols.remove(oTag);
  return true;
}

bool IVistaDriverProtocolAspect::SetProtocol(const _cVersionTag& oTag) {
  m_oCurrent = std::find(m_liProtocols.begin(), m_liProtocols.end(), oTag);
  return (m_oCurrent != m_liProtocols.end());
}

bool IVistaDriverProtocolAspect::GetProtocol(_cVersionTag& oTag) const {
  if (m_liProtocols.empty())
    return false;
  if (m_oCurrent == m_liProtocols.end())
    return false;

  oTag = *m_oCurrent;
  return true;
}

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int IVistaDriverProtocolAspect::GetAspectId() {
  return IVistaDriverProtocolAspect::m_nAspectId;
}

void IVistaDriverProtocolAspect::SetAspectId(int nId) {
  assert(m_nAspectId == -1);
  m_nAspectId = nId;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
