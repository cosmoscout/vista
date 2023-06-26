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

#include "VistaDriverSensorMappingAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h> // needed for the deletion of the factory
          // maybe shift the deletion to another instance

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <algorithm>
#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int          VistaDriverSensorMappingAspect::m_nAspectId  = -1;
unsigned int VistaDriverSensorMappingAspect::INVALID_TYPE = ~0u;
unsigned int VistaDriverSensorMappingAspect::INVALID_ID   = ~0u;

int VistaDriverSensorMappingAspect::GetAspectId() {
  return VistaDriverSensorMappingAspect::m_nAspectId;
}

void VistaDriverSensorMappingAspect::SetAspectId(int nId) {
  if (VistaDriverSensorMappingAspect::m_nAspectId == -1)
    VistaDriverSensorMappingAspect::m_nAspectId = nId;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverSensorMappingAspect::VistaDriverSensorMappingAspect(IVistaDriverCreationMethod* crm) {
  if (VistaDriverSensorMappingAspect::GetAspectId() == -1) // unregistered
    VistaDriverSensorMappingAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("SENSORMAPPING"));

  SetId(VistaDriverSensorMappingAspect::GetAspectId());

  std::list<unsigned int> liTypes = crm->GetTypes();
  std::string             sTypeName;
  for (std::list<unsigned int>::const_iterator itIndex = liTypes.begin(); itIndex != liTypes.end();
       ++itIndex) {
    crm->GetTypeNameFor((*itIndex), sTypeName);
    VISTA_VERIFY(RegisterType(sTypeName), (*itIndex));
  }
}

VistaDriverSensorMappingAspect::~VistaDriverSensorMappingAspect() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
unsigned int VistaDriverSensorMappingAspect::RegisterType(const std::string& strTypeName) {
  if (!GetIsType(strTypeName)) {
    // ok, proceed
    m_vecTypeMapping.push_back(_sL(strTypeName));
    return (unsigned int)m_vecTypeMapping.size() - 1;
  }
  return INVALID_TYPE;
}

unsigned int VistaDriverSensorMappingAspect::GetNumberOfRegisteredTypes() const {
  return (unsigned int)m_vecTypeMapping.size();
}

bool VistaDriverSensorMappingAspect::UnregisterType(const std::string& strType) {
  if (!GetIsType(strType))
    return false;

  for (MAPVEC::iterator it = m_vecTypeMapping.begin(); it != m_vecTypeMapping.end(); ++it) {
    if (VistaAspectsComparisonStuff::StringEquals((*it).m_strTypeName, strType, false)) {
      m_vecTypeMapping.erase(it);
      return true;
    }
  }
  return false;
}

std::list<std::string> VistaDriverSensorMappingAspect::GetTypeNames() const {
  std::list<std::string> liRet;
  for (MAPVEC::const_iterator it = m_vecTypeMapping.begin(); it != m_vecTypeMapping.end(); ++it) {
    liRet.push_back((*it).m_strTypeName);
  }
  return liRet;
}

bool VistaDriverSensorMappingAspect::GetIsType(const std::string& sTypeName) const {
  for (MAPVEC::const_iterator it = m_vecTypeMapping.begin(); it != m_vecTypeMapping.end(); ++it) {
    if (VistaAspectsComparisonStuff::StringEquals((*it).m_strTypeName, sTypeName, false)) {
      // found it
      return true;
    }
  }
  return false;
}

unsigned int VistaDriverSensorMappingAspect::GetTypeId(const std::string& sTypeName) const {
  unsigned int nRet = 0;
  for (MAPVEC::const_iterator it = m_vecTypeMapping.begin(); it != m_vecTypeMapping.end();
       ++it, ++nRet) {
    if (VistaAspectsComparisonStuff::StringEquals((*it).m_strTypeName, sTypeName, false)) {
      // found it
      return nRet;
    }
  }

  return INVALID_TYPE;
}

unsigned int VistaDriverSensorMappingAspect::GetSensorIdByRawId(unsigned int nRawSensorId) const {
  for (MAPVEC::size_type n = 0; n < m_vecTypeMapping.size(); ++n) {
    unsigned int id = GetSensorId((unsigned int)n, nRawSensorId);
    if (id != INVALID_ID)
      return id;
  }
  return INVALID_ID;
}

unsigned int VistaDriverSensorMappingAspect::GetSensorId(
    unsigned int nSensorType, unsigned int nRawSensorId) const {
  if (nSensorType >= m_vecTypeMapping.size())
    return INVALID_ID;

  const _sL&                                           l   = m_vecTypeMapping[nSensorType];
  std::map<unsigned int, unsigned int>::const_iterator cit = l.m_mapIds.find(nRawSensorId);
  if (cit == l.m_mapIds.end())
    return INVALID_ID;
  else
    return (*cit).second;
}

unsigned int VistaDriverSensorMappingAspect::GetRawIdByMappedId(
    unsigned int nSensorType, unsigned int nMappedId) const {
  if (nSensorType >= m_vecTypeMapping.size())
    return INVALID_TYPE;

  const _sL& l = m_vecTypeMapping[nSensorType];
  // will have to iterate over the entire list :(
  for (std::map<unsigned int, unsigned int>::const_iterator cit = l.m_mapIds.begin();
       cit != l.m_mapIds.end(); ++cit) {
    if ((*cit).second == nMappedId)
      return (*cit).first;
  }
  return INVALID_ID; // not found
}

bool VistaDriverSensorMappingAspect::SetSensorId(
    unsigned int nSensorType, unsigned int nRawSensorId, unsigned int nSensorId) {
  if (nSensorType >= m_vecTypeMapping.size())
    return false;

  _sL& l                   = m_vecTypeMapping[nSensorType];
  l.m_mapIds[nRawSensorId] = nSensorId;

  vstr::outi() << "VistaDriverSensorMappingAspect::SetSensorId(" << nSensorType << " ["
               << l.m_strTypeName << "], " << nRawSensorId << ", " << nSensorId << ")" << std::endl;
  return true;
}

unsigned int VistaDriverSensorMappingAspect::GetNumRegisteredSensorsForType(
    unsigned int nType) const {
  if (nType >= m_vecTypeMapping.size())
    return 0;

  const _sL& l = m_vecTypeMapping[nType];
  return (unsigned int)l.m_mapIds.size();
}

bool VistaDriverSensorMappingAspect::ClearSensorMapping() {
  for (MAPVEC::iterator it = m_vecTypeMapping.begin(); it != m_vecTypeMapping.end(); ++it) {
    (*it).m_mapIds.clear();
  }
  return true;
}

bool VistaDriverSensorMappingAspect::ClearTypeMapping() {
  m_vecTypeMapping.clear();
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
