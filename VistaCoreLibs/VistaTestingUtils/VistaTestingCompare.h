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

#ifndef _VISTATESTINGCOMPARE_H
#define _VISTATESTINGCOMPARE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaConversion.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaColor.h>
#include <VistaBase/VistaMathBasics.h>

#include <gtest/gtest.h>

#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

/*============================================================================*/
/* HELPERS                                                                    */
/*============================================================================*/

namespace VistaTesting {
template <typename T>
struct CompareObject {
  ::testing::AssertionResult operator()(const T& oLeft, const T& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    if (oLeft == oRight)
      return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
    else
      return ::testing::AssertionFailure() << oLeft << " != " << oRight;
  }
};
template <typename T>
inline ::testing::AssertionResult Compare(const T& oLeft, const T& oRight,
    const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) {
  CompareObject<T> oCompare;
  return oCompare(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
}

template <typename T>
inline ::testing::AssertionResult CompareArray(const T* oLeft, const T* oRight,
    const int nArraySize, const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) {
  int nEntryId = 0;
  for (; nEntryId != nArraySize; ++nEntryId) {
    ::testing::AssertionResult oResult =
        Compare(oLeft[nEntryId], oRight[nEntryId], nMaxRelDeviation, nMinRelevantSize);
    if (oResult == false) {
      return oResult << " ( entry " << nEntryId << " )"
                     << "[ " << VistaConversion::GetTypeName<T>() << "-array ]";
    }
  }
  return ::testing::AssertionSuccess();
}

template <typename TFloat>
::testing::AssertionResult CompareFloatingPointNumber(const TFloat& oLeft, const TFloat& oRight,
    const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) {
  if (oLeft == oRight)
    return ::testing::AssertionSuccess() << oLeft << " == " << oRight;
  if (std::abs(oLeft) < (TFloat)nMinRelevantSize && std::abs(oLeft) < (TFloat)nMinRelevantSize) {
    return ::testing::AssertionSuccess()
           << oLeft << " && " << oRight << " below min of " << (TFloat)nMinRelevantSize;
  }
  if (nMaxRelDeviation > 0) {
    TFloat nMaxAllowedDeviation = (TFloat)std::max(std::abs(oLeft), std::abs(oRight));
    nMaxAllowedDeviation *= (TFloat)nMaxRelDeviation;
    TFloat nDelta = (TFloat)std::abs(oLeft - oRight);
    if (nDelta < nMaxAllowedDeviation)
      return ::testing::AssertionSuccess() << oLeft << " ~= " << oRight;
  }
  return ::testing::AssertionFailure() << oLeft << " != " << oRight;
}

template <>
struct CompareObject<float> {
  ::testing::AssertionResult operator()(const float& oLeft, const float& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareFloatingPointNumber(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <>
struct CompareObject<double> {
  ::testing::AssertionResult operator()(const double& oLeft, const double& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareFloatingPointNumber(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <>
struct CompareObject<long double> {
  ::testing::AssertionResult operator()(const long double& oLeft, const long double& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareFloatingPointNumber(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};

template <>
struct CompareObject<VistaColor> {
  ::testing::AssertionResult operator()(const VistaColor& oLeft, const VistaColor& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    for (int nAxis = Vista::X; nAxis <= Vista::W; ++nAxis) {
      ::testing::AssertionResult oRes =
          Compare(oLeft[nAxis], oRight[nAxis], nMaxRelDeviation, nMinRelevantSize);
      if (oRes == false)
        return oRes << " ( VistaColor component " << nAxis << " )";
    }
    return ::testing::AssertionSuccess();
  }
};
template <>
struct CompareObject<VistaVector3D> {
  ::testing::AssertionResult operator()(const VistaVector3D& oLeft, const VistaVector3D& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    for (int nAxis = Vista::X; nAxis <= Vista::W; ++nAxis) {
      ::testing::AssertionResult oRes =
          Compare(oLeft[nAxis], oRight[nAxis], nMaxRelDeviation, nMinRelevantSize);
      if (oRes == false)
        return oRes << " ( VistaVector3D component " << nAxis << " )";
    }
    return ::testing::AssertionSuccess();
  }
};
template <>
struct CompareObject<VistaQuaternion> {
  ::testing::AssertionResult operator()(const VistaQuaternion& oLeft, const VistaQuaternion& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    for (int nAxis = Vista::X; nAxis <= Vista::W; ++nAxis) {
      ::testing::AssertionResult oRes =
          Compare(oLeft[nAxis], oRight[nAxis], nMaxRelDeviation, nMinRelevantSize);
      if (oRes == false) {
        // we also ensure that, if we are a unit quaternion, that -1 == 1 for one value,
        // and zero for all others
        ::testing::AssertionResult oRes1 =
            Compare(std::abs(oLeft[nAxis]), 1.0f, nMaxRelDeviation, nMinRelevantSize);
        ::testing::AssertionResult oRes2 =
            Compare(std::abs(oRight[nAxis]), 1.0f, nMaxRelDeviation, nMinRelevantSize);
        if (oRes1 && oRes2) {
          bool bRestIsZero = true;
          for (int nAxis2 = Vista::X; nAxis2 <= Vista::W; ++nAxis2) {
            if (nAxis == nAxis2)
              continue;
            if (Compare(oLeft[nAxis2], 0.0f, nMaxRelDeviation, nMinRelevantSize) == false ||
                Compare(oRight[nAxis2], 0.0f, nMaxRelDeviation, nMinRelevantSize) == false) {
              bRestIsZero = false;
              break;
            }
          }
          if (bRestIsZero)
            return ::testing::AssertionSuccess();
        }
        return oRes << " ( VistaQuaternion component " << nAxis << " )";
      }
    }
    return ::testing::AssertionSuccess();
  }
};
template <>
struct CompareObject<VistaReferenceFrame> {
  ::testing::AssertionResult operator()(const VistaReferenceFrame& oLeft,
      const VistaReferenceFrame& oRight, const double nMaxRelDeviation = 0,
      const double nMinRelevantSize = 0) const {
    ::testing::AssertionResult oRes = Compare(oLeft.GetTranslationConstRef(),
        oRight.GetTranslationConstRef(), nMaxRelDeviation, nMinRelevantSize);
    if (oRes == false)
      return oRes << " of VistarefenreceFrame";
    ::testing::AssertionResult oRes2 = Compare(oLeft.GetRotationConstRef(),
        oRight.GetRotationConstRef(), nMaxRelDeviation, nMinRelevantSize);
    if (oRes2 == false)
      return oRes2 << " of VistarefenreceFrame";
    return ::testing::AssertionSuccess();
  }
};
template <>
struct CompareObject<VistaTransformMatrix> {
  ::testing::AssertionResult operator()(const VistaTransformMatrix& oLeft,
      const VistaTransformMatrix& oRight, const double nMaxRelDeviation = 0,
      const double nMinRelevantSize = 0) const {
    for (int nRow = Vista::X; nRow <= Vista::W; ++nRow) {
      for (int nCol = Vista::X; nCol <= Vista::W; ++nCol) {
        ::testing::AssertionResult oRes =
            Compare(oLeft(nRow, nCol), oRight(nRow, nCol), nMaxRelDeviation, nMinRelevantSize);
        if (oRes == false)
          return oRes << " ( VistaTransformMatrix entry ( " << nRow << ", " << nCol << " ) )";
      }
    }
    return ::testing::AssertionSuccess();
  }
};

template <typename T1, typename T2>
struct CompareObject<std::pair<T1, T2>> {
  ::testing::AssertionResult operator()(const std::pair<T1, T2>& oLeft,
      const std::pair<T1, T2>& oRight, const double nMaxRelDeviation = 0,
      const double nMinRelevantSize = 0) const {
    ::testing::AssertionResult oResFirst =
        Compare(oLeft.first, oRight.first, nMaxRelDeviation, nMinRelevantSize);
    if (oResFirst == false) {
      return oResFirst << " " << VistaConversion::GetTypeName<std::pair<T1, T2>>() << ".first";
    }
    ::testing::AssertionResult oResSecond =
        Compare(oLeft.second, oRight.second, nMaxRelDeviation, nMinRelevantSize);
    if (oResSecond == false) {
      return oResSecond << " " << VistaConversion::GetTypeName<std::pair<T1, T2>>() << ".second";
    }
    return ::testing::AssertionSuccess();
  }
};

template <typename TContainer>
::testing::AssertionResult CompareIteratorContainer(const TContainer& oLeft,
    const TContainer& oRight, const double nMaxRelDeviation = 0,
    const double nMinRelevantSize = 0) {
  if (oLeft.size() != oRight.size()) {
    return ::testing::AssertionFailure()
           << " sizes differ: " << oLeft.size() << " != " << oRight.size() << "[ "
           << VistaConversion::GetTypeName<TContainer>() << " ]";
  }
  typename TContainer::const_iterator itLeft   = oLeft.begin();
  typename TContainer::const_iterator itRight  = oRight.begin();
  typename TContainer::const_iterator itEnd    = oLeft.end();
  int                                 nEntryId = 0;
  for (; itLeft != itEnd; ++itLeft, ++itRight, ++nEntryId) {
    ::testing::AssertionResult oResult =
        Compare((*itLeft), (*itRight), nMaxRelDeviation, nMinRelevantSize);
    if (oResult == false) {
      return oResult << " ( entry " << nEntryId << " )"
                     << "[ " << VistaConversion::GetTypeName<TContainer>() << " ]";
    }
  }
  return ::testing::AssertionSuccess();
}
template <typename TContainer>
::testing::AssertionResult CompareTopPopContainer(const TContainer& oLeft, const TContainer& oRight,
    const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) {
  if (oLeft.size() != oRight.size()) {
    return ::testing::AssertionFailure()
           << " sizes differ: " << oLeft.size() << " != " << oRight.size() << "[ "
           << VistaConversion::GetTypeName<TContainer>() << " ]";
  }
  TContainer oLeftCopy  = oLeft;
  TContainer oRightCopy = oRight;
  int        nEntryId   = 0;
  while (oLeftCopy.empty() == false) {
    ::testing::AssertionResult oResult =
        Compare(oLeftCopy.top(), oRightCopy.top(), nMaxRelDeviation, nMinRelevantSize);
    if (oResult == false) {
      return oResult << " ( entry " << nEntryId << " )"
                     << "[ " << VistaConversion::GetTypeName<TContainer>() << " ]";
    }
    oLeftCopy.pop();
    oRightCopy.pop();
  }
  return ::testing::AssertionSuccess();
}
template <typename TContainer>
::testing::AssertionResult CompareFrontPopContainer(const TContainer& oLeft,
    const TContainer& oRight, const double nMaxRelDeviation = 0,
    const double nMinRelevantSize = 0) {
  if (oLeft.size() != oRight.size()) {
    return ::testing::AssertionFailure()
           << " sizes differ: " << oLeft.size() << " != " << oRight.size() << "[ "
           << VistaConversion::GetTypeName<TContainer>() << " ]";
  }
  TContainer oLeftCopy  = oLeft;
  TContainer oRightCopy = oRight;
  int        nEntryId   = 0;
  while (oLeftCopy.empty() == false) {
    ::testing::AssertionResult oResult =
        Compare(oLeftCopy.front(), oRightCopy.front(), nMaxRelDeviation, nMinRelevantSize);
    if (oResult == false) {
      return oResult << " ( entry " << nEntryId << " )"
                     << "[ " << VistaConversion::GetTypeName<TContainer>() << " ]";
    }
    oLeftCopy.pop();
    oRightCopy.pop();
  }
  return ::testing::AssertionSuccess();
}

template <typename T>
struct CompareObject<std::vector<T>> {
  typedef std::vector<T>     Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T>
struct CompareObject<std::stack<T>> {
  typedef std::stack<T>      Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareTopPopContainer<Container>(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T>
struct CompareObject<std::deque<T>> {
  typedef std::deque<T>      Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T>
struct CompareObject<std::list<T>> {
  typedef std::list<T>       Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T>
struct CompareObject<std::queue<T>> {
  typedef std::queue<T>      Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareFrontPopContainer<Container>(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T>
struct CompareObject<std::priority_queue<T>> {
  typedef std::priority_queue<T> Container;
  ::testing::AssertionResult     operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareTopPopContainer<Container>(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T>
struct CompareObject<std::set<T>> {
  typedef std::set<T>        Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T>
struct CompareObject<std::multiset<T>> {
  typedef std::multiset<T>   Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T1, typename T2>
struct CompareObject<std::map<T1, T2>> {
  typedef std::map<T1, T2>   Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <typename T1, typename T2>
struct CompareObject<std::multimap<T1, T2>> {
  typedef std::multimap<T1, T2> Container;
  ::testing::AssertionResult    operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
template <>
struct CompareObject<VistaProperty> {
  ::testing::AssertionResult operator()(const VistaProperty& oLeft, const VistaProperty& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    ::testing::AssertionResult oResName = Compare(oLeft.GetNameForNameable(),
        oRight.GetNameForNameable(), nMaxRelDeviation, nMinRelevantSize);
    if (oResName == false)
      return oResName << " Property name";
    ::testing::AssertionResult oResType = Compare((int)oLeft.GetPropertyType(),
        (int)oRight.GetPropertyType(), nMaxRelDeviation, nMinRelevantSize);
    if (oResType == false)
      return oResType << " Property type";

    switch (oLeft.GetPropertyType()) {
    case VistaProperty::PROPT_NIL:
    case VistaProperty::PROPT_STRING:
    case VistaProperty::PROPT_ID: {
      ::testing::AssertionResult oResValue = Compare(
          oLeft.GetValueConstRef(), oRight.GetValueConstRef(), nMaxRelDeviation, nMinRelevantSize);
      if (oResValue == false)
        return oResValue << " Property value";
      break;
    }
    case VistaProperty::PROPT_INT: {
      ::testing::AssertionResult oResValue =
          Compare(VistaConversion::FromString<int>(oLeft.GetValueConstRef()),
              VistaConversion::FromString<int>(oRight.GetValueConstRef()), nMaxRelDeviation,
              nMinRelevantSize);
      if (oResValue == false)
        return oResValue << " Property value";
      break;
    }
    case VistaProperty::PROPT_BOOL: {
      ::testing::AssertionResult oResValue =
          Compare(VistaConversion::FromString<bool>(oLeft.GetValueConstRef()),
              VistaConversion::FromString<bool>(oRight.GetValueConstRef()), nMaxRelDeviation,
              nMinRelevantSize);
      if (oResValue == false)
        return oResValue << " Property value";
      break;
    }
    case VistaProperty::PROPT_DOUBLE: {
      ::testing::AssertionResult oResValue =
          Compare(VistaConversion::FromString<double>(oLeft.GetValueConstRef()),
              VistaConversion::FromString<double>(oRight.GetValueConstRef()), nMaxRelDeviation,
              nMinRelevantSize);
      if (oResValue == false)
        return oResValue << " Property value";
      break;
    }
    case VistaProperty::PROPT_LIST: {
      ::testing::AssertionResult oResSublistType = Compare(oLeft.GetPropertyListSubType(),
          oRight.GetPropertyListSubType(), nMaxRelDeviation, nMinRelevantSize);
      if (oResSublistType == false)
        return oResSublistType << " Property as list subtype";

      switch (oLeft.GetPropertyListSubType()) {
      case VistaProperty::PROPT_NIL:
      case VistaProperty::PROPT_STRING:
      case VistaProperty::PROPT_INT: {
        std::vector<std::string> vecLeft, vecRight;
        VistaConversion::FromString(oLeft.GetValueConstRef(), vecLeft);
        VistaConversion::FromString(oRight.GetValueConstRef(), vecRight);
        ::testing::AssertionResult oResList =
            Compare(vecLeft, vecRight, nMaxRelDeviation, nMinRelevantSize);
        if (oResList == false)
          return oResList << " Property as list";
        break;
      }
      case VistaProperty::PROPT_BOOL: {
        std::vector<bool> vecLeft, vecRight;
        VistaConversion::FromString(oLeft.GetValueConstRef(), vecLeft);
        VistaConversion::FromString(oRight.GetValueConstRef(), vecRight);
        ::testing::AssertionResult oResList =
            Compare(vecLeft, vecRight, nMaxRelDeviation, nMinRelevantSize);
        if (oResList == false)
          return oResList << " Property as list";
        break;
      }
      case VistaProperty::PROPT_DOUBLE: {
        std::vector<double> vecLeft, vecRight;
        VistaConversion::FromString(oLeft.GetValueConstRef(), vecLeft);
        VistaConversion::FromString(oRight.GetValueConstRef(), vecRight);
        ::testing::AssertionResult oResList =
            Compare(vecLeft, vecRight, nMaxRelDeviation, nMinRelevantSize);
        if (oResList == false)
          return oResList << " Property as list";
        break;
      }
      case VistaProperty::PROPT_PROPERTYLIST:
      case VistaProperty::PROPT_LIST:
        VISTA_THROW("Property with list of lists is invalid", -1);
      case VistaProperty::PROPT_ID:
      default:
        VISTA_THROW("Invalid Property list type", -1);
      }

      break;
    }
    case VistaProperty::PROPT_PROPERTYLIST: {
      ::testing::AssertionResult oResList = Compare(oLeft.GetPropertyListConstRef(),
          oRight.GetPropertyListConstRef(), nMaxRelDeviation, nMinRelevantSize);
      if (oResList == false)
        return oResList << " Property value sub proplist";
      break;
    }
    default:
      VISTA_THROW("Invalid Property list type", -1);
    }

    return ::testing::AssertionSuccess();
  }
};
template <>
struct CompareObject<VistaPropertyList> {
  typedef VistaPropertyList  Container;
  ::testing::AssertionResult operator()(const Container& oLeft, const Container& oRight,
      const double nMaxRelDeviation = 0, const double nMinRelevantSize = 0) const {
    return CompareIteratorContainer(oLeft, oRight, nMaxRelDeviation, nMinRelevantSize);
  }
};
} // namespace VistaTesting

#endif //_VISTATESTINGCOMPARE_H
