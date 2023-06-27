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

#include "VistaDriverInfoAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverInfoAspect::m_nAspectId = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverInfoAspect::VistaDriverInfoAspect()
    : IVistaDeviceDriver::IVistaDeviceDriverAspect(false) {
  if (VistaDriverInfoAspect::GetAspectId() == -1) // unregistered
    VistaDriverInfoAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("INFO"));

  SetId(VistaDriverInfoAspect::GetAspectId());
}

VistaDriverInfoAspect::~VistaDriverInfoAspect() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaPropertyList& VistaDriverInfoAspect::GetInfoPropsWrite() {
  return m_oProps;
}

VistaPropertyList VistaDriverInfoAspect::GetInfoProps() const {
  return m_oProps;
}

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int VistaDriverInfoAspect::GetAspectId() {
  return VistaDriverInfoAspect::m_nAspectId;
}

void VistaDriverInfoAspect::SetAspectId(int nId) {
  assert(m_nAspectId == -1);
  m_nAspectId = nId;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
