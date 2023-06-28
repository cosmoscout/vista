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

#ifndef _VISTATESTINGDUMMYSTRUCT_H
#define _VISTATESTINGDUMMYSTRUCT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaTestingUtils/VistaTestingCompare.h>
#include <VistaTestingUtils/VistaTestingRandom.h>

/*============================================================================*/
/* HELPERS                                                                    */
/*============================================================================*/

struct DummyStruct {
  DummyStruct()
      : m_fData1(42.0f)
      , m_bData2(false){};

  bool operator()(const DummyStruct& oOther) const {
    return (m_fData1 == oOther.m_fData1 && m_bData2 == oOther.m_bData2);
  }

  static std::string ConvertToString(const DummyStruct& oObject) {
    return (VistaConversion::ToString(oObject.m_fData1) + " | " +
            VistaConversion::ToString(oObject.m_bData2));
  }
  static DummyStruct ConvertFromString(const std::string& sString) {
    DummyStruct oNew;
    // can throw exceptions!
    std::vector<std::string> vecEntries;
    if (VistaConversion::FromString(sString, vecEntries, '|') == false || vecEntries.size() != 2) {
      VISTA_THROW("String-To_DummyObject conversion failed", -1);
    }
    oNew.m_fData1 = VistaConversion::FromString<float>(vecEntries[0]);
    oNew.m_bData2 = VistaConversion::FromString<bool>(vecEntries[1]);
    return oNew;
  }

  float m_fData1;
  bool  m_bData2;
};

namespace VistaTesting {
template <>
inline DummyStruct GenerateRandom<DummyStruct>() {
  DummyStruct oDummy;
  oDummy.m_bData2 = GenerateRandom<bool>();
  oDummy.m_fData1 = GenerateRandom<float>();
  return oDummy;
}
} // namespace VistaTesting

namespace VistaTesting {
template <>
inline ::testing::AssertionResult Compare<DummyStruct>(const DummyStruct& oLeft,
    const DummyStruct& oRight, const double nMaxRelDeviation, const double nMinRelevantSize) {
  ::testing::AssertionResult oRes =
      Compare(oLeft.m_fData1, oRight.m_fData1, nMaxRelDeviation, nMinRelevantSize);
  if (oRes == false) {
    return oRes << "(floatdata)";
  }
  ::testing::AssertionResult oRes2 =
      Compare(oLeft.m_bData2, oRight.m_bData2, nMaxRelDeviation, nMinRelevantSize);
  if (oRes2 == false) {
    return oRes2 << "(booldata)";
  }
  return ::testing::AssertionSuccess();
}
} // namespace VistaTesting

namespace VistaConversion {
template <>
struct StringConvertObject<DummyStruct> {
  static void ToString(
      const DummyStruct& oSource, std::string& sTarget, char cSeparator = S_cDefaultSeparator) {
    sTarget = DummyStruct::ConvertToString(oSource);
  }
  static bool FromString(
      const std::string& sSource, DummyStruct& oTarget, char cSeparator = S_cDefaultSeparator) {
    try {
      oTarget = DummyStruct::ConvertFromString(sSource);
    } catch (VistaExceptionBase&) { return false; }
    return true;
  }
};
} // namespace VistaConversion

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

#endif //_VISTATESTINGUTILS_H
