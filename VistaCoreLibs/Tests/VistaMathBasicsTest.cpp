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

#include <VistaBase/VistaVector3D.h>

#include <VistaTestingUtils/VistaTestingCompare.h>

#include <limits>

template <typename T>
void TestIsValidNumberFloatingPoint() {
  ASSERT_TRUE(Vista::IsValidNumber((T)0));
  ASSERT_TRUE(Vista::IsValidNumber((T)1));
  ASSERT_TRUE(Vista::IsValidNumber((T)-23e-12));
  ASSERT_TRUE(Vista::IsValidNumber((T)-65.0));
  ASSERT_TRUE(Vista::IsValidNumber((T)1.45e-32));
  ASSERT_TRUE(Vista::IsValidNumber((T)6.66e32));

  T nZeroValue = 0;
  ASSERT_FALSE(Vista::IsValidNumber((T)-65.0 / nZeroValue));
  ASSERT_FALSE(Vista::IsValidNumber(std::numeric_limits<T>::infinity()));
  // some implementations flush denormals to zero - we will check that
  if (std::numeric_limits<T>::has_denorm && (T(0.01) * std::numeric_limits<T>::min()) != 0) {
    ASSERT_FALSE(Vista::IsValidNumber(std::numeric_limits<T>::denorm_min()));
    ASSERT_FALSE(Vista::IsValidNumber(std::numeric_limits<T>::min() / (T)1.0001));
    ASSERT_FALSE(Vista::IsValidNumber(-std::numeric_limits<T>::min() / (T)1.0001));
  }
  ASSERT_FALSE(Vista::IsValidNumber(std::numeric_limits<T>::quiet_NaN()));
  ASSERT_FALSE(Vista::IsValidNumber(std::numeric_limits<T>::signaling_NaN()));

  ASSERT_TRUE(Vista::IsValidNumber(std::numeric_limits<T>::min()));
  ASSERT_TRUE(Vista::IsValidNumber(-std::numeric_limits<T>::min()));

  ASSERT_TRUE(Vista::IsValidNumber(std::numeric_limits<T>::max()));
  ASSERT_TRUE(Vista::IsValidNumber(-std::numeric_limits<T>::max()));
  ASSERT_FALSE(Vista::IsValidNumber(std::numeric_limits<T>::max() * (T)1.0001));
  ASSERT_FALSE(Vista::IsValidNumber(-std::numeric_limits<T>::max() * (T)1.0001));
}

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

TEST(VistaMathBasicsTest, IsValidNumber) {
  TestIsValidNumberFloatingPoint<float>();
  TestIsValidNumberFloatingPoint<double>();
  TestIsValidNumberFloatingPoint<long double>();
}

TEST(VistaMathBasicsTest, Clamp) {
  ASSERT_EQ(Vista::Clamp(-1, 0, 1), 0);
  ASSERT_EQ(Vista::Clamp(1.0, 0.0, 1.0), 1.0);
  ASSERT_EQ(Vista::Clamp(1.0f, 0.0f, 2.0f), 1.0f);
  ASSERT_EQ(Vista::Clamp(-1.1, -2.2, 1.0), -1.1);
  ASSERT_EQ(Vista::Clamp(-1.65, 0.01, 1.0), 0.01);
  ASSERT_EQ(Vista::Clamp(6.0f, 0.0f, 3.45f), 3.45f);
}

TEST(VistaMathBasicsTest, DegToRad) {
  ASSERT_EQ(Vista::DegToRad(0.0f), 0.0f);
  ASSERT_EQ(Vista::DegToRad(90.0f), 0.5f * Vista::Pi);
  ASSERT_EQ(Vista::DegToRad(180.0f), Vista::Pi);
  ASSERT_EQ(Vista::DegToRad(360.0f), 2 * Vista::Pi);
  ASSERT_EQ(Vista::DegToRad(-90.0f), -0.5f * Vista::Pi);
  ASSERT_EQ(Vista::DegToRad(45.0f), 0.25f * Vista::Pi);
}

TEST(VistaMathBasicsTest, RadToDef) {
  ASSERT_EQ(Vista::RadToDeg(0.0f), 0.0f);
  ASSERT_EQ(Vista::RadToDeg(0.5f * Vista::Pi), 90.0f);
  ASSERT_EQ(Vista::RadToDeg(Vista::Pi), 180.0f);
  ASSERT_EQ(Vista::RadToDeg(2 * Vista::Pi), 360.0f);
  ASSERT_EQ(Vista::RadToDeg(-0.5f * Vista::Pi), -90.0f);
  ASSERT_EQ(Vista::RadToDeg(0.25f * Vista::Pi), 45.0f);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
