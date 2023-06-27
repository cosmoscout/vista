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

#include "VistaDeviceIdentificationAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDeviceIdentificationAspect::m_nAspectId = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDeviceIdentificationAspect::VistaDeviceIdentificationAspect()
    : IVistaDeviceDriver::IVistaDeviceDriverAspect(false)
    , m_nVendorID(0)
    , m_nProductID(0)
    , m_nSerialNumber(0)
    , m_sDeviceName("") {
  if (VistaDeviceIdentificationAspect::GetAspectId() == -1) // unregistered
    VistaDeviceIdentificationAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("IDENTIFICATION"));

  SetId(VistaDeviceIdentificationAspect::GetAspectId());
}

VistaDeviceIdentificationAspect::~VistaDeviceIdentificationAspect() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
int VistaDeviceIdentificationAspect::GetVendorID() const {
  return m_nVendorID;
}

void VistaDeviceIdentificationAspect::SetVendorID(int nVendorID) {
  m_nVendorID = nVendorID;
}

int VistaDeviceIdentificationAspect::GetProductID() const {
  return m_nProductID;
}

void VistaDeviceIdentificationAspect::SetProductID(int nProductID) {
  m_nProductID = nProductID;
}

int VistaDeviceIdentificationAspect::GetSerialNumber() const {
  return m_nSerialNumber;
}

void VistaDeviceIdentificationAspect::SetSerialNumber(int nSerialNumber) {
  m_nSerialNumber = nSerialNumber;
}

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int VistaDeviceIdentificationAspect::GetAspectId() {
  return VistaDeviceIdentificationAspect::m_nAspectId;
}

void VistaDeviceIdentificationAspect::SetAspectId(int nId) {
  assert(m_nAspectId == -1);
  m_nAspectId = nId;
}

std::string VistaDeviceIdentificationAspect::GetDeviceName() const {
  return m_sDeviceName;
}

void VistaDeviceIdentificationAspect::SetDeviceName(const std::string& sName) {
  m_sDeviceName = sName;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
