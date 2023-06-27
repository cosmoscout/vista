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

#include "VistaEvent.h"

#include <VistaBase/VistaStreamUtils.h>

#include <VistaAspects/VistaDeSerializer.h>
#include <VistaAspects/VistaSerializer.h>

#include <cassert>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

int VistaEvent::m_nEventId = VistaEvent::VET_INVALID;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaEvent::VistaEvent(const int iEventType, const int iEventID)
    : m_iType(iEventType)
    , m_iId(iEventID)
    , m_bHandled(false)
    , m_nTime(0)
    , m_nCount(0) {
}

VistaEvent::~VistaEvent() {
  // vstr::debugi() << " [VistaEvent] >> DESTRUCTOR <<" << std::endl;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

VistaType::microtime VistaEvent::GetTime() const {
  return m_nTime;
}

bool VistaEvent::IsHandled() const {
  return m_bHandled;
}

void VistaEvent::SetHandled(bool bHandled) {
  m_bHandled = bHandled;
}

int VistaEvent::GetId() const {
  return m_iId;
}

bool VistaEvent::SetId(int iId) {
  m_iId = iId;
  return true;
};

std::string VistaEvent::GetName() const {
  return "VistaEvent";
}

void VistaEvent::Debug(std::ostream& oOut) const {
  oOut << " [ViEv]     Name:      " << GetName() << std::endl;
  oOut << " [ViEv]     TypeId:    " << m_iType << std::endl;

  std::streamsize iPrecision = oOut.precision(12);
  oOut << " [ViEv]     TimeStamp: " << m_nTime << std::endl;
  oOut.precision(iPrecision);

  oOut << " [ViEv]     Handled:   " << m_bHandled << std::endl;
}

int VistaEvent::Serialize(IVistaSerializer& oSerializer) const {
  int iSize = 0;
  iSize += oSerializer.WriteInt32(GetType());
  iSize += oSerializer.WriteInt32(GetId());
  iSize += oSerializer.WriteBool(IsHandled());
  iSize += oSerializer.WriteDouble(GetTime());
  return iSize;
}

int VistaEvent::DeSerialize(IVistaDeSerializer& oDeSerializer) {
  int iSize = 0;
  iSize += oDeSerializer.ReadInt32(m_iType);
  iSize += oDeSerializer.ReadInt32(m_iId);
  iSize += oDeSerializer.ReadBool(m_bHandled);
  iSize += oDeSerializer.ReadDouble(m_nTime);
  return iSize;
}

std::string VistaEvent::GetSignature() const {
  return GetName();
}

void VistaEvent::SetType(int iType) {
  m_iType = iType;
  assert(m_iType >= 0);
}

int VistaEvent::GetType() const {
  return m_iType;
}

int VistaEvent::GetCount() const {
  return m_nCount;
}

std::string VistaEvent::GetIdString(int nId) {
  switch (nId) {
  case VEID_NONE:
    return "VEID_NONE";
  case VEID_LAST:
    return "VEID_LAST";
  default:
    return "<unknown>";
  }
}

int VistaEvent::GetTypeId() {
  return m_nEventId;
}

void VistaEvent::SetTypeId(int nId) {
  if (m_nEventId == VistaEvent::VET_INVALID)
    m_nEventId = nId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
std::ostream& operator<<(std::ostream& oOut, const VistaEvent& oEvent) {
  oEvent.Debug(oOut);
  return oOut;
}
