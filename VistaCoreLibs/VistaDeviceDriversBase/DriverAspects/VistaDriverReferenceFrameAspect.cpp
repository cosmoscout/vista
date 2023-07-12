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

#include "VistaDriverReferenceFrameAspect.h"
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

int IVistaDriverReferenceFrameAspect::m_nAspectId = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaDriverReferenceFrameAspect::IVistaDriverReferenceFrameAspect()
    : IVistaDeviceDriver::IVistaDeviceDriverAspect(false) {
  if (IVistaDriverReferenceFrameAspect::GetAspectId() == -1) // unregistered
    IVistaDriverReferenceFrameAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("REFFRAME"));

  SetId(IVistaDriverReferenceFrameAspect::GetAspectId());
}

IVistaDriverReferenceFrameAspect::~IVistaDriverReferenceFrameAspect() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int IVistaDriverReferenceFrameAspect::GetAspectId() {
  return IVistaDriverReferenceFrameAspect::m_nAspectId;
}

void IVistaDriverReferenceFrameAspect::SetAspectId(int nId) {
  assert(m_nAspectId == -1);
  m_nAspectId = nId;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
