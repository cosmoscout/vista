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

#include "VistaProperty.h"

#include "VistaAspectsUtils.h"
#include "VistaDeSerializer.h"
#include "VistaSerializer.h"
#include <VistaAspects/VistaPropertyList.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cstdlib>
#include <iostream>
#include <list>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

std::string VistaProperty::GetPropTypeName(VistaProperty::ePropType eTypeId) {
  switch (eTypeId) {
  case PROPT_NIL:
    return "NIL";
  case PROPT_STRING:
    return "STRING";
  case PROPT_INT:
    return "INT";
  case PROPT_BOOL:
    return "BOOL";
  case PROPT_DOUBLE:
    return "DOUBLE";
  case PROPT_LIST:
    return "LIST";
  case PROPT_ID:
    return "ID";
  case PROPT_PROPERTYLIST:
    return "PROPERTYLIST";
  default:
    return "<unknown>";
  }
}

VistaProperty::ePropType VistaProperty::GetPropTypeEnum(int iPropType) {
  switch (iPropType) {
  case PROPT_STRING:
    return PROPT_STRING;
  case PROPT_INT:
    return PROPT_INT;
  case PROPT_BOOL:
    return PROPT_BOOL;
  case PROPT_DOUBLE:
    return PROPT_DOUBLE;
  case PROPT_LIST:
    return PROPT_LIST;
  case PROPT_ID:
    return PROPT_ID;
  case PROPT_PROPERTYLIST:
    return PROPT_PROPERTYLIST;
  default:
    break;
  }
  return PROPT_NIL;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaProperty::VistaProperty()
    : m_sName("")
    , m_ePropType(PROPT_NIL)
    , m_eListSubType(PROPT_NIL)
    , m_pSubProps(NULL) {
}

VistaProperty::VistaProperty(const std::string& sName)
    : m_sName(sName)
    , m_ePropType(PROPT_NIL)
    , m_eListSubType(PROPT_NIL)
    , m_pSubProps(NULL) {
}

VistaProperty::VistaProperty(
    const std::string& sName, const std::string& sValue, ePropType eTp, ePropType eListTp)
    : m_sName(sName)
    , m_sValue(sValue)
    , m_ePropType(eTp)
    , m_eListSubType(eListTp)
    , m_pSubProps(NULL) {
}

VistaProperty::~VistaProperty() {
  delete m_pSubProps;
}

VistaProperty::VistaProperty(const VistaProperty& rProp)
    : m_sName(rProp.m_sName)
    , m_sValue(rProp.m_sValue)
    , m_ePropType(rProp.m_ePropType)
    , m_eListSubType(rProp.m_eListSubType) {

  if (rProp.m_pSubProps) {
    m_pSubProps = new VistaPropertyList(*rProp.m_pSubProps);
  } else {
    m_pSubProps = NULL;
  }
}

VistaProperty& VistaProperty::operator=(const VistaProperty& rProp) {
  if (&rProp == this)
    return *this;

  m_sName        = rProp.m_sName;
  m_sValue       = rProp.m_sValue;
  m_ePropType    = rProp.m_ePropType;
  m_eListSubType = rProp.m_eListSubType;

  if (rProp.m_pSubProps) {
    m_pSubProps = new VistaPropertyList;
    // assign by copy
    (*m_pSubProps) = *rProp.m_pSubProps;
  } else {
    m_pSubProps = NULL;
  }

  return *this;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

std::string VistaProperty::GetNameForNameable() const {
  return m_sName;
}

std::string VistaProperty::GetValue() const {
  return m_sValue;
}
const std::string& VistaProperty::GetValueConstRef() const {
  return m_sValue;
}

void VistaProperty::SetValue(const std::string& sValue) {
  m_sValue = sValue;
  if (m_ePropType == PROPT_NIL)
    m_ePropType = PROPT_STRING; // switch to std::string-value
}

void VistaProperty::SetNameForNameable(const std::string& sNewName) {
  m_sName = sNewName;
}

VistaProperty::ePropType VistaProperty::GetPropertyType() const {
  return m_ePropType;
}

VistaProperty::ePropType VistaProperty::GetPropertyListSubType() const {
  return m_eListSubType;
}

void VistaProperty::SetPropertyType(ePropType eType) {
  m_ePropType = eType;
  if ((m_ePropType != PROPT_PROPERTYLIST) && (m_pSubProps)) {
    delete m_pSubProps;
    m_pSubProps = NULL;
  }
  if ((m_ePropType == PROPT_PROPERTYLIST) && (m_pSubProps == NULL)) {
    m_pSubProps = new VistaPropertyList;
  }
}

void VistaProperty::SetPropertyListSubType(ePropType eType) {
  m_eListSubType = eType;
}

bool VistaProperty::GetIsNilProperty() const {
  return (m_ePropType == PROPT_NIL);
}

void VistaProperty::SetPropertyListValue(const VistaPropertyList& refProp) {
  if (!m_pSubProps)
    m_pSubProps = new VistaPropertyList;

  (*m_pSubProps) = refProp;

  m_ePropType = PROPT_PROPERTYLIST;
}

VistaPropertyList VistaProperty::GetPropertyListValue() const {
  if (m_pSubProps)
    return *m_pSubProps;

  return VistaPropertyList();
}

VistaPropertyList& VistaProperty::GetPropertyListRef() {
  if (!m_pSubProps) {
    // should create a PropertyList pointer as a side effect
    // and treat the type
    SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
  }

  return *m_pSubProps;
}

const VistaPropertyList& VistaProperty::GetPropertyListConstRef() const {
  if (!m_pSubProps)
    VISTA_THROW("VistaProperty::GetPropertyListConstRef -- Exception (no PropertyList present)",
        0x00000001);

  return *m_pSubProps;
}

bool VistaProperty::operator==(const std::string& rValue) const {
  return (m_sValue == rValue);
}

bool VistaProperty::operator==(const VistaPropertyList& refList) const {
  if (!m_pSubProps)
    VISTA_THROW("VistaProperty::operator== -- Exception (no PropertyList present)", 0x00000001);

  return (*m_pSubProps) == refList;
}

bool VistaProperty::operator==(const VistaProperty& rProp) const {
  if (m_ePropType != rProp.GetPropertyType()) {
    vstr::warnp() << "VistaProperty::operator==(const VistaProperty &) -- "
                  << "Comparing on values of different types (" << GetPropertyType() << ") vs. ("
                  << rProp.GetPropertyType() << ")" << std::endl;
  }

  if ((m_ePropType == VistaProperty::PROPT_PROPERTYLIST) &&
      (rProp.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST)) {
    if (!m_pSubProps)
      VISTA_THROW("VistaProperty::operator== -- Exception (no PropertyList present)", 0x00000001);

    return ((*m_pSubProps) == rProp.GetPropertyListConstRef());
  }

  return (m_sValue == rProp.m_sValue);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
