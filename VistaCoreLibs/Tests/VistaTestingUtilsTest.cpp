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

#include <gtest/gtest.h>

#include <VistaTestingUtils/VistaTestingCompare.h>
#include <VistaTestingUtils/VistaTestingDummyStruct.h>

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

TEST(VistaTestingCompare, bool) {
  ASSERT_TRUE(VistaTesting::Compare(true, true));
  ASSERT_FALSE(VistaTesting::Compare(true, false));
  ASSERT_TRUE(VistaTesting::Compare(false, false, 0.1, 0.1));
}

TEST(VistaTestingCompare, int) {
  ASSERT_TRUE(VistaTesting::Compare(1, 1));
  ASSERT_FALSE(VistaTesting::Compare(1, -1));
  ASSERT_TRUE(VistaTesting::Compare(0, 0, 3.1, -0.1));
}

TEST(VistaTestingCompare, float_double) {
  ASSERT_TRUE(VistaTesting::Compare(1.0f, 1.0f));
  ASSERT_TRUE(VistaTesting::Compare(-0.0, +0.0));
  ASSERT_FALSE(VistaTesting::Compare(1.0f, 1.00001f));
  ASSERT_TRUE(VistaTesting::Compare(1.0, 1.00001, 1e-4));
  ASSERT_FALSE(VistaTesting::Compare(1.0f, 1.00001f, 1e-7));
  ASSERT_FALSE(VistaTesting::Compare(1e-20, 1.1e-20, 1e-4));
  ASSERT_FALSE(VistaTesting::Compare(1.2e-20f, 1.1e-20f, 1e-4, 1e-20));
  ASSERT_TRUE(VistaTesting::Compare(1.3e-20, 1.1e-20, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(1.3e-20f, 1.1e-20f, 1e-4, 1.2e-20));
}

TEST(VistaTestingCompare, VistaColor) {
  ASSERT_TRUE(VistaTesting::Compare(VistaColor(0, 1, 0), VistaColor(0, 1, 0)));
  ASSERT_TRUE(VistaTesting::Compare(
      VistaColor(0.1f, 0.2f, 0.3f, 0.4f), VistaColor(0.1f, 0.2f, 0.3f, 0.4f)));
  ASSERT_FALSE(VistaTesting::Compare(VistaColor(-1, 1, -1), VistaColor(-1, 1, 1)));
  ASSERT_FALSE(
      VistaTesting::Compare(VistaColor(0.0f, 1.0f, 0.0f), VistaColor(0.0f, 1.00001f, 0.0f)));
  ASSERT_TRUE(
      VistaTesting::Compare(VistaColor(0.0f, 1.0f, 0.0f), VistaColor(0.0f, 1.00001f, 0.0f), 1e-4));
}

TEST(VistaTestingCompare, VistaVector3D) {
  ASSERT_TRUE(VistaTesting::Compare(VistaVector3D(0, 1, 0), VistaVector3D(0, 1, 0)));
  ASSERT_TRUE(VistaTesting::Compare(
      VistaVector3D(0.1f, 0.2f, 0.3f, 0.4f), VistaVector3D(0.1f, 0.2f, 0.3f, 0.4f)));
  ASSERT_FALSE(VistaTesting::Compare(VistaVector3D(-1, 1, -1), VistaVector3D(-1, 1, 1)));
  ASSERT_FALSE(VistaTesting::Compare(VistaVector3D(0, 1, 0), VistaVector3D(0, 1.00001f, 0)));
  ASSERT_TRUE(VistaTesting::Compare(VistaVector3D(0, 1, 0), VistaVector3D(0, 1.00001f, 0), 1e-4));
}

TEST(VistaTestingCompare, VistaQuaternion) {
  ASSERT_TRUE(VistaTesting::Compare(VistaQuaternion(0, 1, 0, 0), VistaQuaternion(0, 1, 0, 0)));
  ASSERT_TRUE(VistaTesting::Compare(
      VistaQuaternion(0.1f, 0.2f, 0.3f, 0.4f), VistaQuaternion(0.1f, 0.2f, 0.3f, 0.4f)));
  ASSERT_FALSE(
      VistaTesting::Compare(VistaQuaternion(0, 1, 0, 0), VistaQuaternion(0, 1.00001f, 0, 0)));
  ASSERT_TRUE(
      VistaTesting::Compare(VistaQuaternion(0, 1, 0, 0), VistaQuaternion(0, 1.00001f, 0, 0), 1e-4));
  ASSERT_TRUE(VistaTesting::Compare(VistaQuaternion(0, 0, 0, 1), VistaQuaternion(0, 0, 0, -1)));
  ASSERT_FALSE(
      VistaTesting::Compare(VistaQuaternion(0, 0.1f, 0, 1), VistaQuaternion(0, 0.1f, 0, -1)));
  ASSERT_FALSE(VistaTesting::Compare(
      VistaQuaternion(0, 0.707f, 0, 0.707f), VistaQuaternion(0, 0.707f, 0, -0.707f)));
}

TEST(VistaTestingCompare, pair) {
  ASSERT_TRUE(VistaTesting::Compare(std::make_pair(1.0f, "test"), std::make_pair(1.0f, "test")));
  ASSERT_FALSE(VistaTesting::Compare(std::make_pair(1.0f, "tast"), std::make_pair(1.0f, "test")));
  ASSERT_FALSE(
      VistaTesting::Compare(std::make_pair(1.00001f, "test"), std::make_pair(1.0f, "test")));
  std::vector<long double> vecLeft, vecRight;
  vecLeft.push_back(10000.1);
  vecRight.push_back(10000.0);
  ASSERT_TRUE(VistaTesting::Compare(
      std::make_pair(1.00001f, vecLeft), std::make_pair(1.0f, vecRight), 1e-4));
}

TEST(VistaTestingCompare, vector_int) {
  std::vector<int> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.push_back(1);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.push_back(1);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 3.1, -0.1));
}

TEST(VistaTestingCompare, list_double) {
  std::list<double> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.push_back(1.0);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.push_back(1.00001);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecLeft.push_back(1e-11);
  vecRight.clear();
  vecRight.push_back(1.0);
  vecRight.push_back(1e-13);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-14));
}
TEST(VistaTestingCompare, deque_double) {
  std::deque<double> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.push_back(1.0);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.push_back(1.00001);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecLeft.push_back(1e-11);
  vecRight.clear();
  vecRight.push_back(1.0);
  vecRight.push_back(1e-13);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-14));
}
TEST(VistaTestingCompare, queue_double) {
  std::queue<double> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.push(1.0);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.push(1.00001);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecLeft.push(1e-11);
  vecRight.pop();
  vecRight.push(1.0);
  vecRight.push(1e-13);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-14));
}
TEST(VistaTestingCompare, priorityqueue_double) {
  std::priority_queue<double> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.push(1.0);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.push(1.00001);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecLeft.push(1e-11);
  vecRight.pop();
  vecRight.push(1.0);
  vecRight.push(1e-13);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-14));
}
TEST(VistaTestingCompare, stack_double) {
  std::stack<double> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.push(1.0);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.push(1.00001);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecLeft.push(1e-11);
  vecRight.pop();
  vecRight.push(1.0);
  vecRight.push(1e-13);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-14));
}

TEST(VistaTestingCompare, set_double) {
  std::set<double> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.insert(1.0);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.insert(1.00001);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecLeft.insert(1e-11);
  vecRight.clear();
  vecRight.insert(1.0);
  vecRight.insert(1e-13);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-14));
}
TEST(VistaTestingCompare, multiset_double) {
  std::set<double> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.insert(1.0);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.insert(1.00001);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecLeft.insert(1e-11);
  vecRight.clear();
  vecRight.insert(1.0);
  vecRight.insert(1e-13);
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-10));
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight, 1e-4, 1e-14));
}

TEST(VistaTestingCompare, map_string_Vector3D) {
  std::map<std::string, VistaVector3D> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft["test"] = VistaVector3D(1, 2, 3);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight["test"] = VistaVector3D(1, 2, 3.00001f);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecRight.clear();
  vecRight["tast"] = VistaVector3D(1, 2, 3);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.clear();
  vecRight.clear();
  vecLeft["X"]     = Vista::XAxis;
  vecRight["X"]    = Vista::XAxis;
  vecLeft["Zero"]  = Vista::ZeroVector;
  vecRight["Zero"] = Vista::ZeroVector;
  vecLeft["Y"]     = Vista::YAxis;
  vecRight["Y"]    = Vista::YAxis;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
}
TEST(VistaTestingCompare, multimap_string_Vector3D) {
  std::map<std::string, VistaVector3D> vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft["test"] = VistaVector3D(1, 2, 3);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight["test"] = VistaVector3D(1, 2, 3.00001f);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight, 1e-4));
  vecRight.clear();
  vecRight["tast"] = VistaVector3D(1, 2, 3);
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.clear();
  vecRight.clear();
  vecLeft["X"]     = Vista::XAxis;
  vecRight["X"]    = Vista::XAxis;
  vecLeft["Zero"]  = Vista::ZeroVector;
  vecRight["Zero"] = Vista::ZeroVector;
  vecLeft["Y"]     = Vista::YAxis;
  vecRight["Y"]    = Vista::YAxis;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
}

TEST(VistaTestingCompare, Property) {
  VistaProperty oLeft, oRight;
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));

  oLeft = VistaProperty("test", "testest", VistaProperty::PROPT_STRING);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty("test", "testest", VistaProperty::PROPT_STRING);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty("test", "testest1", VistaProperty::PROPT_STRING);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));

  oLeft  = VistaProperty("test", "testest", VistaProperty::PROPT_STRING);
  oRight = VistaProperty("test2", "testest", VistaProperty::PROPT_STRING);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));

  oRight = VistaProperty("test", "true", VistaProperty::PROPT_BOOL);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oLeft = VistaProperty("test", "true", VistaProperty::PROPT_BOOL);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oLeft = VistaProperty("test", "TRUE", VistaProperty::PROPT_BOOL);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oLeft = VistaProperty("test", "false", VistaProperty::PROPT_BOOL);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oLeft = VistaProperty("test", "true", VistaProperty::PROPT_STRING);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));

  oLeft  = VistaProperty("test", "1.00001", VistaProperty::PROPT_DOUBLE);
  oRight = VistaProperty("test", "1.00001", VistaProperty::PROPT_DOUBLE);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty("test", "1.0", VistaProperty::PROPT_DOUBLE);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty("test", "1.0", VistaProperty::PROPT_DOUBLE);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight, 1e-4));
  oLeft  = VistaProperty("test", "1e3", VistaProperty::PROPT_DOUBLE);
  oRight = VistaProperty("test", "1000", VistaProperty::PROPT_DOUBLE);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));

  oLeft  = VistaProperty("test", "1, 2, 3", VistaProperty::PROPT_LIST, VistaProperty::PROPT_STRING);
  oRight = VistaProperty("test", "1, 2, 3", VistaProperty::PROPT_LIST, VistaProperty::PROPT_STRING);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty("test", "1, 2, 3", VistaProperty::PROPT_LIST, VistaProperty::PROPT_DOUBLE);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oLeft = VistaProperty("test", "1, 2, 3", VistaProperty::PROPT_LIST, VistaProperty::PROPT_DOUBLE);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty("test", "1, 2, 4", VistaProperty::PROPT_LIST, VistaProperty::PROPT_DOUBLE);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty("test", "1, 2, 3", VistaProperty::PROPT_LIST, VistaProperty::PROPT_INT);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight = VistaProperty(
      "test", "1, 2, 3.00001", VistaProperty::PROPT_LIST, VistaProperty::PROPT_DOUBLE);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight, 1e-4));

  VistaPropertyList oList1;
  oList1.SetValue("number", 1);
  VistaPropertyList oList2;
  oList1.SetValue("number", 1);
  oList1.SetValue("string", "hello");
  oLeft.SetNameForNameable("plist");
  oLeft.SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
  oRight.SetNameForNameable("plist");
  oRight.SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oLeft.SetPropertyListValue(oList1);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight.SetPropertyListValue(oList1);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oLeft.SetPropertyListValue(oList2);
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight.SetPropertyListValue(oList2);
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
}

TEST(VistaTestingCompare, PropertyList) {
  VistaPropertyList vecLeft, vecRight;
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecLeft.SetValue("test", "testest");
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.SetValue("test", "testest");
  ASSERT_TRUE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.clear();
  vecRight.SetValue("tast", "testest");
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
  vecRight.clear();
  vecRight.SetValue("test", "testest1");
  ASSERT_FALSE(VistaTesting::Compare(vecLeft, vecRight));
}

TEST(VistaTestingCompare, DummyStruct) {
  DummyStruct oLeft, oRight;
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oLeft.m_fData1 = 1;
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight.m_fData1 = 1;
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight));
  oRight.m_fData1 = 1.0000001f;
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight, 1e-4));
  oLeft.m_bData2 = true;
  ASSERT_FALSE(VistaTesting::Compare(oLeft, oRight));
  oRight.m_bData2 = true;
  oLeft.m_fData1  = 0;
  oRight.m_fData1 = 0.0000001f;
  ASSERT_TRUE(VistaTesting::Compare(oLeft, oRight, 0, 1e-6));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
