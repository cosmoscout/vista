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

#if defined(WIN32)
#pragma warning(disable : 4996)
#endif

#include <cstdio>
#include <iostream>

#include "VistaDisplayEntity.h"
#include <VistaAspects/VistaAspectsUtils.h>

#include <cstdio>
#include <iostream>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaDisplayEntity::VistaDisplayEntity(IVistaDisplayEntityData* pData, IVistaDisplayBridge* pBridge)
    : IVistaNameable()
    , m_pData(pData)
    , m_pBridge(pBridge)
    , m_pProperties(NULL) {
}

VistaDisplayEntity::~VistaDisplayEntity() {
  DeleteProperties();
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetData                                                     */
/*                                                                            */
/*============================================================================*/
IVistaDisplayEntityData* VistaDisplayEntity::GetData() const {
  return m_pData;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaDisplayEntity::Debug(std::ostream& out) const {
  out << " [VistaDisplayEntity] - name:         " << GetNameForNameable() << std::endl;
  out << " [VistaDisplayEntity] - id:           " << GetNameableId() << std::endl;
  out << " [VistaDisplayEntity] - data pointer: " << m_pData << std::endl;
  out << " [VistaDisplayEntity] - bridge:       " << m_pBridge << std::endl;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNameForNameable                                          */
/*                                                                            */
/*============================================================================*/
std::string VistaDisplayEntity::GetNameForNameable() const {
  return m_strName;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetNameForNameable                                          */
/*                                                                            */
/*============================================================================*/
void VistaDisplayEntity::SetNameForNameable(const std::string& strNewName) {
  m_strName = strNewName;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNameableIdAsString                                       */
/*                                                                            */
/*============================================================================*/
std::string VistaDisplayEntity::GetNameableIdAsString() const {
  char buf[128];
  sprintf(buf, "%lx", long(GetNameableId()));
  return string(buf);
}

bool VistaDisplayEntity::DeleteProperties() {
  delete m_pProperties;
  m_pProperties = NULL;
  return true;
}

IVistaDisplayBridge* VistaDisplayEntity::GetDisplayBridge() const {
  return m_pBridge;
}

void VistaDisplayEntity::InitProperties() {
  if (!m_pProperties)
    m_pProperties = CreateProperties();
}

// ############################################################################

namespace {
std::string sSReflectionTypeVSE("VistaDisplayEntity");

IVistaPropertyGetFunctor* aCgFunctorsVSE[] = {
    new TVistaPropertyGet<string, VistaDisplayEntity::IVistaDisplayEntityProperties>(
        "NAME", sSReflectionTypeVSE, &VistaDisplayEntity::IVistaDisplayEntityProperties::GetName),
    NULL};

/*	IVistaPropertySetFunctor *aCsFunctors[] =
        {
                NULL
        };*/
} // namespace

std::string VistaDisplayEntity::IVistaDisplayEntityProperties::GetName() const {
  VistaDisplayEntity* pDE = static_cast<VistaDisplayEntity*>(GetParent());
  return pDE->GetNameForNameable();
}

bool VistaDisplayEntity::IVistaDisplayEntityProperties::SetName(const std::string& sName) {
  std::string sOldName = GetParent()->GetNameForNameable();
  if (sOldName != sName) {
    GetParent()->SetNameForNameable(sName);
    return true;
  }

  return false;
}

VistaDisplayEntity::IVistaDisplayEntityProperties* VistaDisplayEntity::GetProperties() const {
  return m_pProperties;
}

VistaDisplayEntity* VistaDisplayEntity::IVistaDisplayEntityProperties::GetParent() const {
  return m_pParent;
}

IVistaDisplayBridge* VistaDisplayEntity::IVistaDisplayEntityProperties::GetDisplayBridge() const {
  return m_pBridge;
}

VistaDisplayEntity::IVistaDisplayEntityProperties::IVistaDisplayEntityProperties(
    VistaDisplayEntity* pParent, IVistaDisplayBridge* pBridge)
    : m_pParent(pParent)
    , m_pBridge(pBridge) {
}

VistaDisplayEntity::IVistaDisplayEntityProperties::~IVistaDisplayEntityProperties() {
}

string VistaDisplayEntity::IVistaDisplayEntityProperties::GetReflectionableType() const {
  return sSReflectionTypeVSE;
}

int VistaDisplayEntity::IVistaDisplayEntityProperties::AddToBaseTypeList(
    list<string>& rBtList) const {
  int nSize = IVistaReflectionable::AddToBaseTypeList(rBtList);
  rBtList.push_back(sSReflectionTypeVSE);
  return nSize + 1;
}

/*============================================================================*/
/*  STREAMING OPERATOR                                                        */
/*============================================================================*/
std::ostream& operator<<(std::ostream& os, const VistaDisplayEntity& refEntity) {
  refEntity.Debug(os);

  return os;
}
