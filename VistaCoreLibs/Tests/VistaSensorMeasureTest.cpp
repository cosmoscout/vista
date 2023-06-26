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

#include "VistaTestDriver.h"

#include "VistaBase/VistaTimeUtils.h"
#include "VistaBase/VistaTimer.h"
#include "VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h"
#include "VistaDeviceDriversBase/VistaDeviceDriver.h"
#include "VistaDeviceDriversBase/VistaSensorReadState.h"

#include <memory>

/////////////////////////////////////
///////// ACTUAL TESTS //////////////
/////////////////////////////////////

TEST(VistaSensorMeatureTest, EmptySensorIsEmpty) {
  VistaTestDriverWrapper oDevice;

  ASSERT_EQ(oDevice.GetSensor()->GetMeasureCount(), 0);
}

TEST(VistaSensorMeatureTest, SensorMeasureCountIsIncreasing) {
  VistaTestDriverWrapper oDevice;

  oDevice.GetDriver()->PushValue(42);
  ASSERT_EQ(oDevice.GetSensor()->GetMeasureCount(), 1);
  oDevice.GetDriver()->PushValue(12345);
  ASSERT_EQ(oDevice.GetSensor()->GetMeasureCount(), 2);
  oDevice.GetDriver()->PushValue(666);
  ASSERT_EQ(oDevice.GetSensor()->GetMeasureCount(), 3);
}

TEST(VistaSensorMeatureTest, SensorValidAccess) {
  VistaTestDriverWrapper oDevice;

  oDevice.GetDriver()->PushValue(42);
  ASSERT_EQ(oDevice.GetSensor()->GetMostCurrentValue<int>("VALUE"), 42);

  oDevice.GetDriver()->PushValue(12345);
  ASSERT_EQ(oDevice.GetSensor()->GetMostCurrentValue<int>("VALUE"), 12345);
}
TEST(VistaSensorMeatureTest, SensorInvalidAccessWrongGetter) {
  VistaTestDriverWrapper oDevice;

  oDevice.GetDriver()->PushValue(42);
  ASSERT_THROW(
      oDevice.GetSensor()->GetMostCurrentValue<int>("NON_EXISTANT_VALUE"), VistaExceptionBase);
}

TEST(VistaSensorMeatureTest, SensorInvalidAccessWrongType) {
  VistaTestDriverWrapper oDevice;

  oDevice.GetDriver()->PushValue(42);
  ASSERT_THROW(
      oDevice.GetSensor()->GetMostCurrentValue<int>("NON_EXISTANT_VALUE"), VistaExceptionBase);
}

TEST(VistaSensorMeatureTest, EmptyHistoryIsEmpty) {
  VistaTestDriverWrapper oDevice;

  ASSERT_EQ(oDevice.GetHistory().GetMeasureCount(), 0);
}
TEST(VistaSensorMeatureTest, HistoryAccessWhenEmptyReturnsNull) {
  VistaTestDriverWrapper oDevice;

  ASSERT_FALSE(oDevice.GetHistory().GetMostCurrent()->GetIsValid());
}
TEST(VistaSensorMeatureTest, HistoryAccessValid) {
  VistaTestDriverWrapper oDevice;

  oDevice.GetDriver()->PushValue(42);
  const VistaSensorMeasure* pMeasure = oDevice.GetHistory().GetMostCurrent();
  ASSERT_TRUE(pMeasure->GetIsValid());
  ASSERT_EQ(pMeasure->getRead<VistaTestDriverMeasure>()->m_nValue, 42);

  oDevice.GetDriver()->PushValue(12345);
  pMeasure = oDevice.GetHistory().GetMostCurrent();
  ASSERT_TRUE(pMeasure->GetIsValid());
  ASSERT_EQ(pMeasure->getRead<VistaTestDriverMeasure>()->m_nValue, 12345);

  oDevice.GetDriver()->PushValue(666);
  pMeasure = oDevice.GetHistory().GetMostCurrent();
  ASSERT_TRUE(pMeasure->GetIsValid());
  ASSERT_EQ(pMeasure->getRead<VistaTestDriverMeasure>()->m_nValue, 666);
}
TEST(VistaSensorMeatureTest, HistoryAccessPastValid) {
  VistaTestDriverWrapper oDevice;

  oDevice.GetDriver()->PushValue(42);
  oDevice.GetDriver()->PushValue(12345);
  oDevice.GetDriver()->PushValue(666);

  const VistaSensorMeasure* pMeasure = oDevice.GetHistory().GetCurrentPast(0);
  ASSERT_TRUE(pMeasure->GetIsValid());
  ASSERT_EQ(pMeasure->getRead<VistaTestDriverMeasure>()->m_nValue, 666);

  pMeasure = oDevice.GetHistory().GetCurrentPast(1);
  ASSERT_TRUE(pMeasure->GetIsValid());
  ASSERT_EQ(pMeasure->getRead<VistaTestDriverMeasure>()->m_nValue, 12345);

  pMeasure = oDevice.GetHistory().GetCurrentPast(2);
  ASSERT_TRUE(pMeasure->GetIsValid());
  ASSERT_EQ(pMeasure->getRead<VistaTestDriverMeasure>()->m_nValue, 42);
}
TEST(VistaSensorMeatureTest, HistoryAccessPastInvalid) {
  VistaTestDriverWrapper oDevice;

  oDevice.GetDriver()->PushValue(42);
  oDevice.GetDriver()->PushValue(12345);
  oDevice.GetDriver()->PushValue(666);

  const VistaSensorMeasure* pMeasure = oDevice.GetHistory().GetCurrentPast(3);
  ASSERT_FALSE(pMeasure->GetIsValid());
}

TEST(VistaSensorMeatureTest, ReadStateValid) {
  VistaTestDriverWrapper oDevice;
  VistaSensorReadState   oReadState(oDevice.GetSensor(), false);

  ASSERT_TRUE(oReadState.GetIsValid());
}
TEST(VistaSensorMeatureTest, ReadStateInvalid) {
  VistaSensorReadState oReadState(NULL, false);

  ASSERT_FALSE(oReadState.GetIsValid());
}
TEST(VistaSensorMeatureTest, ReadStateInitiallyEmpty) {
  VistaTestDriverWrapper oDevice;
  VistaSensorReadState   oReadState(oDevice.GetSensor(), false);

  ASSERT_EQ(oReadState.GetMeasureCount(), 0);
}
TEST(VistaSensorMeatureTest, ReadStateEmptyWithoutSwap) {
  VistaTestDriverWrapper oDevice;
  VistaSensorReadState   oReadState(oDevice.GetSensor(), false);

  oDevice.GetDriver()->PushValue(42);
  ASSERT_EQ(oReadState.GetMeasureCount(), 0);
  oDevice.GetDriver()->PushValue(12345);
  ASSERT_EQ(oReadState.GetMeasureCount(), 0);
}
TEST(VistaSensorMeatureTest, ReadStateEmptyAfterEmptySwap) {
  VistaTestDriverWrapper oDevice;
  VistaSensorReadState   oReadState(oDevice.GetSensor(), false);

  oReadState.SetToLatestSample();
  ASSERT_EQ(oReadState.GetMeasureCount(), 0);
}
TEST(VistaSensorMeatureTest, ReadStateEntriesAfterFillAndSwap) {
  VistaTestDriverWrapper oDevice;
  VistaSensorReadState   oReadState(oDevice.GetSensor(), false);

  oDevice.GetDriver()->PushValue(42);
  oReadState.SetToLatestSample();
  ASSERT_EQ(oReadState.GetMeasureCount(), 1);
  ASSERT_EQ(oReadState.GetCurrentMeasure()->getRead<VistaTestDriverMeasure>()->m_nValue, 42);

  oDevice.GetDriver()->PushValue(12345);
  oReadState.SetToLatestSample();
  ASSERT_EQ(oReadState.GetMeasureCount(), 2);
  ASSERT_EQ(oReadState.GetCurrentMeasure()->getRead<VistaTestDriverMeasure>()->m_nValue, 12345);

  oDevice.GetDriver()->PushValue(666);
  oReadState.SetToLatestSample();
  ASSERT_EQ(oReadState.GetMeasureCount(), 3);
  ASSERT_EQ(oReadState.GetCurrentMeasure()->getRead<VistaTestDriverMeasure>()->m_nValue, 666);
}
TEST(VistaSensorMeatureTest, ReadStateDriverUpdateAndSwap) {
  VistaTestDriverWrapper oDevice;
  VistaSensorReadState   oReadState(oDevice.GetSensor(), false);

  oDevice.GetDriver()->PushValue(42);
  oDevice.GetDriver()->PushValue(12345);
  oDevice.GetDriver()->PushValue(666);
  oReadState.SetToLatestSample();
  oDevice.GetDriver()->PushValue(31415);
  ASSERT_EQ(oReadState.GetMeasureCount(), 3);
  ASSERT_EQ(oReadState.GetCurrentMeasure()->getRead<VistaTestDriverMeasure>()->m_nValue, 666);
}
TEST(VistaSensorMeatureTest, ReadStateHistoryAccess) {
  VistaTestDriverWrapper oDevice;
  VistaSensorReadState   oReadState(oDevice.GetSensor(), false);

  oDevice.GetDriver()->PushValue(42);
  oDevice.GetDriver()->PushValue(12345);
  oDevice.GetDriver()->PushValue(666);
  oReadState.SetToLatestSample();
  oDevice.GetDriver()->PushValue(31415);
  ASSERT_EQ(oReadState.GetPastMeasure(0)->getRead<VistaTestDriverMeasure>()->m_nValue, 666);
  ASSERT_EQ(oReadState.GetPastMeasure(1)->getRead<VistaTestDriverMeasure>()->m_nValue, 12345);
  ASSERT_EQ(oReadState.GetPastMeasure(2)->getRead<VistaTestDriverMeasure>()->m_nValue, 42);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
