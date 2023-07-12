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

#include "VistaPropertyList.h"

#include "VistaAspectsUtils.h"
#include "VistaDeSerializer.h"
#include "VistaSerializer.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cstdlib>
#include <iostream>
#include <list>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static bool ConvertPropList(const VistaPropertyList& oIn, VistaPropertyList& oOut,
    const bool bOutputCaseSensitive, const bool bConvertSubPropLists = true,
    const bool bFailOnNameClash = true) {
  oOut = VistaPropertyList(bOutputCaseSensitive);

  if (bOutputCaseSensitive) {
    // easier, we know that there can be no name clash
    for (VistaPropertyList::const_iterator itEntry = oIn.begin(); itEntry != oIn.end(); ++itEntry) {
      oOut.insert((*itEntry));
      if (bConvertSubPropLists &&
          (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
        oOut[(*itEntry).first].GetPropertyListRef().SetIsCaseSensitive(bOutputCaseSensitive);
      }
    }
  } else {
    // we need to check if insertions succeed
    for (VistaPropertyList::const_iterator itEntry = oIn.begin(); itEntry != oIn.end(); ++itEntry) {
      std::pair<VistaPropertyList::iterator, bool> oReturnPair = oOut.insert((*itEntry));
      if (oReturnPair.second == false) {
        // Name clash!
        if (bFailOnNameClash) {
          return false;
        } else {
          vstr::warnp() << "[VistaPropertyList] Name clash when converting "
                        << "PropertyList to CaseInsensitive: Entries ["
                        << (*oReturnPair.first).first << "] and [" << (*itEntry).first
                        << "] have the same key!" << std::endl;
        }
      }

      if (bConvertSubPropLists &&
          (*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
        if (oOut[(*itEntry).first].GetPropertyListRef().SetIsCaseSensitive(bOutputCaseSensitive) ==
                false &&
            bFailOnNameClash) {
          return false;
        }
      }
    }
  }
  return true;
}

IVistaSerializer& operator<<(IVistaSerializer& oSer, const VistaPropertyList& oList) {
  VistaPropertyList::SerializePropertyList(oSer, oList, "");
  return oSer;
}

IVistaDeSerializer& operator>>(IVistaDeSerializer& oDeSer, VistaPropertyList& oList) {
  std::string sDummy;
  VistaPropertyList::DeSerializePropertyList(oDeSer, oList, sDummy);
  return oDeSer;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaPropertyList::VistaPropertyList(const bool bCaseSensitive)
    : std::map<std::string, VistaProperty, VistaAspectsComparisonStuff::StringLessObject>(
          VistaAspectsComparisonStuff::StringLessObject(bCaseSensitive))
    , m_bIsCaseSensitive(bCaseSensitive) {
}

VistaPropertyList::VistaPropertyList(const VistaPropertyList& oCopy)
    : std::map<std::string, VistaProperty, VistaAspectsComparisonStuff::StringLessObject>(oCopy)
    , m_bIsCaseSensitive(oCopy.GetIsCaseSensitive()) {
}

VistaPropertyList::~VistaPropertyList() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaPropertyList::GetIsCaseSensitive() const {
  return m_bIsCaseSensitive;
}

bool VistaPropertyList::SetIsCaseSensitive(
    const bool bSet, const bool bConvertSubPropLists, const bool bFailOnNameClash) {
  if (m_bIsCaseSensitive == bSet)
    return true;

  // we need to remove and re-insert everything. For this, we first
  // copy ourselve, and then copy our content
  VistaPropertyList oTmpList;
  if (ConvertPropList(*this, oTmpList, bSet, bConvertSubPropLists, bFailOnNameClash)) {
    (*this) = oTmpList;
    return true;
  }
  return false;
}

int VistaPropertyList::DeSerializePropertyList(
    IVistaDeSerializer& rDeSer, VistaPropertyList& rPropertyList, std::string& sName) {
  rPropertyList.clear();
  int  iRet           = 0;
  int  iCount         = 0;
  bool bCaseSensitive = false;
  iRet += rDeSer.ReadInt32(iCount);
  iRet += rDeSer.ReadEncodedString(sName);
  iRet += rDeSer.ReadBool(bCaseSensitive);

  for (int i = 0; i < iCount; ++i) {
    VistaType::sint32 iType;
    iRet += rDeSer.ReadInt32(iType);
    if (iType == (int)VistaProperty::PROPT_PROPERTYLIST) {
      VistaPropertyList oList;
      std::string       subName;
      iRet += DeSerializePropertyList(rDeSer, oList, subName);
      rPropertyList.SetPropertyListValue(subName, oList);
    } else {
      VistaType::sint32 nListSubType;
      std::string       sKey, sValue;
      iRet += rDeSer.ReadEncodedString(sKey);
      iRet += rDeSer.ReadEncodedString(sValue);
      iRet += rDeSer.ReadInt32(nListSubType);
      rPropertyList.SetStringValueTyped(sKey, sValue, VistaProperty::GetPropTypeEnum(iType),
          VistaProperty::GetPropTypeEnum(nListSubType));
    }
  }

  return iRet;
}

int VistaPropertyList::SerializePropertyList(IVistaSerializer& rSer,
    const VistaPropertyList& rPropertyList, const std::string& sPropertyListName) {
  int iRet = 0;
  iRet += rSer.WriteInt32(int(rPropertyList.size()));
  iRet += rSer.WriteEncodedString(sPropertyListName);
  iRet += rSer.WriteBool(rPropertyList.GetIsCaseSensitive());

  for (VistaPropertyList::const_iterator cit = rPropertyList.begin(); cit != rPropertyList.end();
       ++cit) {
    std::string key = (*cit).first;
    iRet += rSer.WriteInt32((int)(*cit).second.GetPropertyType());
    if ((*cit).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
      iRet += SerializePropertyList(rSer, (*cit).second.GetPropertyListValue(), key);
    } else {
      // std::string value = (*cit).second;
      std::string sValue = (*cit).second.GetValue();
      iRet += rSer.WriteEncodedString(key);
      iRet += rSer.WriteEncodedString(sValue);
      iRet += rSer.WriteInt32((int)(*cit).second.GetPropertyListSubType());
    }
  }
  return iRet;
}

VistaProperty VistaPropertyList::GetPropertyCopy(const std::string& sPropName) const {
  VistaPropertyList::const_iterator cit = find(sPropName);

  if (cit == end())
    VISTA_THROW("VISTAPROPERTYLIST - REQUESTED NON_EXISTANT PROPERTY", -1)
  return (*cit).second;
}

VistaProperty& VistaPropertyList::GetPropertyRef(const std::string& sPropName) {
  VistaPropertyList::iterator it = find(sPropName);

  if (it == end())
    VISTA_THROW("VISTAPROPERTYLIST - REQUESTED NON_EXISTANT PROPERTY", -1)
  return (*it).second;
}

const VistaProperty& VistaPropertyList::GetPropertyConstRef(const std::string& sPropName) const {
  VistaPropertyList::const_iterator cit = find(sPropName);

  if (cit == end())
    VISTA_THROW("VISTAPROPERTYLIST - REQUESTED NON_EXISTANT PROPERTY", -1)
  return (*cit).second;
}

VistaPropertyList VistaPropertyList::GetSubListCopy(const std::string& sPropName) const {
  VistaPropertyList::const_iterator cit = find(sPropName);
  if (cit == end() || (*cit).second.GetPropertyType() != VistaProperty::PROPT_PROPERTYLIST)
    VISTA_THROW("VISTAPROPERTYLIST - REQUESTED NON_EXISTANT SUB_PROPLIST", -1)

  return (*cit).second.GetPropertyListValue();
}

VistaPropertyList& VistaPropertyList::GetSubListRef(const std::string& sPropName) {
  VistaPropertyList::iterator cit = find(sPropName);
  if (cit == end() || (*cit).second.GetPropertyType() != VistaProperty::PROPT_PROPERTYLIST)
    VISTA_THROW("VISTAPROPERTYLIST - REQUESTED NON_EXISTANT SUB_PROPLIST", -1)

  return (*cit).second.GetPropertyListRef();
}

const VistaPropertyList& VistaPropertyList::GetSubListConstRef(const std::string& sPropName) const {
  VistaPropertyList::const_iterator cit = find(sPropName);
  if (cit == end() || (*cit).second.GetPropertyType() != VistaProperty::PROPT_PROPERTYLIST)
    VISTA_THROW("VISTAPROPERTYLIST - REQUESTED NON_EXISTANT SUB_PROPLIST", -1)

  return (*cit).second.GetPropertyListConstRef();
}

void VistaPropertyList::SetStringValueTyped(const std::string& sPropName, const std::string& sValue,
    VistaProperty::ePropType eType, VistaProperty::ePropType eListType) {
  VistaProperty prop(sPropName);
  prop.SetValue(sValue);
  prop.SetPropertyType(eType);
  prop.SetPropertyListSubType(eListType);

  (*this)[sPropName] = prop;
}

void VistaPropertyList::SetProperty(const VistaProperty& propVal) {
  std::string strPropName = propVal.GetNameForNameable();
  (*this)[strPropName]    = propVal;
}

void VistaPropertyList::SetPropertyListValue(
    const std::string& sPropName, const VistaPropertyList& refList) {
  VistaProperty prop(sPropName);
  (*this)[sPropName]     = prop;
  VistaProperty& refProp = (*this)[sPropName];
  refProp.SetPropertyListValue(refList);
  refProp.SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
}

bool VistaPropertyList::HasProperty(const std::string& sPropName) const {
  return find(sPropName) != end();
}
bool VistaPropertyList::HasSubList(const std::string& sPropName) const {
  const_iterator itEntry = find(sPropName);
  if (itEntry == end())
    return false;
  return ((*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST);
}

bool VistaPropertyList::RemoveProperty(const std::string& sPropName) {
  VistaPropertyList::iterator it = find(sPropName);
  if (it != end()) {
    erase(it);
    return true;
  }
  return false;
}

void VistaPropertyList::Print(std::ostream& oStream, int iDepth) const {
  std::string strPrefix;
  for (int i = 0; i < iDepth; ++i)
    strPrefix += "  ";

  for (VistaPropertyList::const_iterator cit = begin(); cit != end(); ++cit) {
    oStream << strPrefix << "[" << (*cit).second.GetNameForNameable() << "]";

    if ((*cit).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
      oStream << " : TYPE [" << VistaProperty::GetPropTypeName((*cit).second.GetPropertyType())
              << "]\n";
      oStream << strPrefix << "### BEGIN [" << (*cit).second.GetNameForNameable() << "] ###\n";
      const VistaPropertyList& refProps = (*cit).second.GetPropertyListConstRef();
      refProps.Print(oStream, iDepth + 1);
      oStream << strPrefix << "### end [" << (*cit).second.GetNameForNameable() << "] ###"
              << std::endl;
    } else {
      oStream << " = [" << (*cit).second.GetValue() << "] : TYPE [";
      oStream << VistaProperty::GetPropTypeName((*cit).second.GetPropertyType()) << ", "
              << VistaProperty::GetPropTypeName((*cit).second.GetPropertyListSubType()) << "]"
              << std::endl;
    }
  }
}

VistaPropertyList VistaPropertyList::MergePropertyLists(
    const VistaPropertyList& oMaster, const VistaPropertyList& oMergeIn) {
  VistaPropertyList oRet = oMaster;
  oRet.MergeWith(oMergeIn);
  return oRet;
}

bool VistaPropertyList::MergeWith(const VistaPropertyList& oMergeIn) {
  for (VistaPropertyList::const_iterator itProp = oMergeIn.begin(); itProp != oMergeIn.end();
       ++itProp) {
    if ((*itProp).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
      VistaProperty& oProp = (*this)[(*itProp).first]; // gets existing one or creates a new one
      if (oProp.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
        oProp.GetPropertyListRef().MergeWith((*itProp).second.GetPropertyListConstRef());
      } else {
        SetPropertyListValue((*itProp).first, (*itProp).second.GetPropertyListConstRef());
      }
    } else {
      // overwrite value from oMergeIn in oMaster
      (*this)[(*itProp).first] = (*itProp).second;
    }
  }

  return true;
}

VistaProperty VistaPropertyList::operator()(const std::string& sName) const {
  VistaPropertyList::const_iterator itEntry = find(sName);
  if (itEntry == end())
    return VistaProperty();
  else
    return (*itEntry).second;
}

VistaProperty::ePropType VistaPropertyList::GetPropertyType(const std::string& sPropName) const {
  VistaPropertyList::const_iterator itEntry = find(sPropName);
  if (itEntry == end())
    return VistaProperty::PROPT_NIL;
  else
    return (*itEntry).second.GetPropertyType();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
