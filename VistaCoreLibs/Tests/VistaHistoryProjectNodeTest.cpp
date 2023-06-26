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

#include <memory>

#include "VistaTestDriver.h"

#include "VistaBase/VistaTimeUtils.h"
#include "VistaBase/VistaTimer.h"
#include "VistaDataFlowNet/VdfnActionObject.h"
#include "VistaDataFlowNet/VdfnDriverSensorNode.h"
#include <VistaDataFlowNet/VdfnHistoryPort.h>
#include <VistaDataFlowNet/VdfnHistoryProjectNode.h>
#include <VistaDeviceDriversBase/VistaSensorReadState.h>

class HistoryProjectNodeWrapper {
 public:
  HistoryProjectNodeWrapper(
      VdfnHistoryProjectNode::eMode eUpdateMode = VdfnHistoryProjectNode::MD_LAZY,
      const std::list<std::string>& liMappings  = {"VALUE"})
      : m_nLastPortUpdateMeasureCount(0)
      , m_nLastEvaluationCount(0) {
    // configure port access
    VdfnPortFactory* pFac = VdfnPortFactory::GetSingleton();
    pFac->AddPortAccess(typeid(VdfnHistoryPortData*).name(),
        new VdfnPortFactory::CPortAccess(new VdfnTypedPortCreate<VdfnHistoryPortData*>,
            new TVdfnTranscodePortSet<VdfnHistoryPortData*>,
            new VdfnTypedPortStringGet<VdfnHistoryPortData*>));
    pFac->AddFunctorAccess(typeid(VdfnHistoryPortData*).name(),
        new VdfnPortFactory::CFunctorAccess(
            new VdfnTypedPortTypeCompareCreate<VdfnHistoryPortData*>,
            new TActionSet<VdfnHistoryPortData*>, new VdfnTypedPortCreate<VdfnHistoryPortData*>,
            NULL));
    pFac->AddPortAccess(
        typeid(int).name(), new VdfnPortFactory::CPortAccess(new VdfnTypedPortCreate<int>,
                                new TVdfnTranscodePortSet<int>, new VdfnTypedPortStringGet<int>));
    pFac->AddFunctorAccess(typeid(int).name(),
        new VdfnPortFactory::CFunctorAccess(new VdfnTypedPortTypeCompareCreate<int>,
            new TActionSet<int>, new VdfnTypedPortCreate<int>, NULL));

    m_pReadState =
        std::unique_ptr<VistaSensorReadState>(new VistaSensorReadState(GetSensor(), false));

    m_pSensorNode = std::unique_ptr<VdfnDriverSensorNode>(
        new VdfnDriverSensorNode(m_oDevice.GetSensor(), m_pReadState.get()));
    m_pProjectNode = std::unique_ptr<VdfnHistoryProjectNode>(
        new VdfnHistoryProjectNode(liMappings, eUpdateMode));
    m_pBackwardIndexPort = std::unique_ptr<TVdfnPort<unsigned int>>(new TVdfnPort<unsigned int>());

    m_pSensorNode->PreparePorts();
    m_pProjectNode->PreparePorts();

    m_pHistoryPort = dynamic_cast<HistoryPort*>(m_pSensorNode->GetOutPort("history"));
    m_pProjectNode->SetInPort("history", m_pHistoryPort);
    m_pProjectNode->SetInPort("backward_index", m_pBackwardIndexPort.get());

    m_pSensorNode->PrepareEvaluationRun();
    m_pProjectNode->PrepareEvaluationRun();

    m_pValuePort = dynamic_cast<TVdfnPort<int>*>(m_pProjectNode->GetOutPort("VALUE"));
  }

  void UpdateReadState() {
    m_pReadState->SetToLatestSample();
  }
  void PushValueToDriver(const int nValue) {
    m_oDevice.GetDriver()->PushValue(nValue);
  }

  void EvaluateNodes() {
    VistaType::systemtime nUpdateTimestamp = VistaTimeUtils::GetStandardTimer().GetSystemTime();

    if (m_pSensorNode->NeedsEval()) {
      m_pSensorNode->EvalNode(nUpdateTimestamp);
    }

    m_nLastEvaluationCount = 0;
    m_vecLastEvaluationValues.clear();
    unsigned int nValuePortUpdateCount = (m_pValuePort ? m_pValuePort->GetUpdateCounter() : 0);
    while (m_pProjectNode->NeedsEval()) {
      m_pProjectNode->EvalNode(nUpdateTimestamp);
      ++m_nLastEvaluationCount;
      if (m_pValuePort && m_pValuePort->GetUpdateCounter() > nValuePortUpdateCount) {
        nValuePortUpdateCount = m_pValuePort->GetUpdateCounter();
        m_vecLastEvaluationValues.push_back(m_pValuePort->GetValue());
      }
    }
  }

  void SetProjectNodeBackwardIndex(const unsigned int nIndex) {
    m_pBackwardIndexPort->SetValue(nIndex, VistaTimeUtils::GetStandardTimer().GetSystemTime());
  }

  VdfnDriverSensorNode* GetSensorNode() {
    return m_pSensorNode.get();
  }
  VdfnHistoryProjectNode* GetProjectNode() {
    return m_pProjectNode.get();
  }
  HistoryPort* GetHistoryPort() {
    return m_pHistoryPort;
  }
  TVdfnPort<int>* GetValuePort() {
    return m_pValuePort;
  }

  VistaDeviceSensor* GetSensor() {
    return m_oDevice.GetSensor();
  }
  const VistaMeasureHistory& GetHistory() {
    return m_oDevice.GetHistory();
  }
  VistaSensorReadState* GetReadState() {
    return m_pReadState.get();
  }

  int GetLastEvaluationCount() {
    return m_nLastEvaluationCount;
  }
  bool GetLastEvaluationResultEqualts(const std::vector<int>& vecValues) {
    if (vecValues.size() != m_vecLastEvaluationValues.size())
      return false;
    return std::equal(vecValues.begin(), vecValues.end(), m_vecLastEvaluationValues.begin());
  }
  bool GetLastEvaluationResultEqualts(const int nValue) {
    if (m_vecLastEvaluationValues.size() != 1)
      return false;
    return (m_vecLastEvaluationValues.front() == nValue);
  }

 private:
  VistaTestDriverWrapper                   m_oDevice;
  std::unique_ptr<VdfnDriverSensorNode>    m_pSensorNode;
  std::unique_ptr<VdfnHistoryProjectNode>  m_pProjectNode;
  std::unique_ptr<VistaSensorReadState>    m_pReadState;
  HistoryPort*                             m_pHistoryPort;
  std::unique_ptr<TVdfnPort<unsigned int>> m_pBackwardIndexPort;
  TVdfnPort<int>*                          m_pValuePort;
  VistaType::uint32                        m_nLastPortUpdateMeasureCount;
  int                                      m_nLastEvaluationCount;
  std::vector<int>                         m_vecLastEvaluationValues;
};

/////////////////////////////////////
///////// ACTUAL TESTS //////////////
/////////////////////////////////////

TEST(VistaHistoryProjectNode, NoSensorNodeUpdateWithoutDriverData) {
  HistoryProjectNodeWrapper oData;
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetHistoryPort()->GetUpdateCounter(), 0);
}
TEST(VistaHistoryProjectNode, NoSensorNodeUpdateWithoutReadstateUpdate) {
  HistoryProjectNodeWrapper oData;
  oData.PushValueToDriver(42);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetHistoryPort()->GetUpdateCounter(), 0);
}
TEST(VistaHistoryProjectNode, SensorNodeUpdateAfterReadstateUpdate) {
  HistoryProjectNodeWrapper oData;
  oData.PushValueToDriver(42);
  oData.UpdateReadState();
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetHistoryPort()->GetUpdateCounter(), 1);
  oData.PushValueToDriver(12345);
  oData.PushValueToDriver(666);
  oData.UpdateReadState();
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetHistoryPort()->GetUpdateCounter(), 2);
}

TEST(VistaHistoryProjectNode, NoProjectNodeEvalsWithoutSensorMesures) {
  HistoryProjectNodeWrapper oData;
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 0);
}
TEST(VistaHistoryProjectNode, NoProjectNodeEvalsWithoutReadstateUpdate) {
  HistoryProjectNodeWrapper oData;
  oData.PushValueToDriver(42);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 0);
}
TEST(VistaHistoryProjectNode, ProjectNodeEvalAfterReadstateUpdate) {
  HistoryProjectNodeWrapper oData;
  oData.PushValueToDriver(42);
  oData.UpdateReadState();
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(42));
}

TEST(VistaHistoryProjectNode, ProjectNodeLazyYieldsLatestReadstateValue) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_LAZY);
  oData.PushValueToDriver(42);
  oData.PushValueToDriver(12345);
  oData.UpdateReadState();
  oData.PushValueToDriver(666);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(12345));
}

TEST(VistaHistoryProjectNode, ProjectNodeIndexedYieldsRelativeToLatestReadstateValue) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_INDEXED);
  oData.SetProjectNodeBackwardIndex(1);
  oData.PushValueToDriver(42);
  oData.PushValueToDriver(12345);
  oData.UpdateReadState();
  oData.PushValueToDriver(666);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(42));
}

TEST(VistaHistoryProjectNode, ProjectNodeIndexedYieldsNewResultAfterIndexChange) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_INDEXED);
  oData.SetProjectNodeBackwardIndex(0);
  oData.PushValueToDriver(42);
  oData.PushValueToDriver(12345);
  oData.PushValueToDriver(666);
  oData.UpdateReadState();
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(666));

  oData.SetProjectNodeBackwardIndex(1);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(12345));

  oData.SetProjectNodeBackwardIndex(2);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(42));
}

TEST(VistaHistoryProjectNode, ProjectNodeIndexedYieldsUpdateButNoResultWhenIndexTooLarge) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_INDEXED);
  oData.SetProjectNodeBackwardIndex(3);
  oData.PushValueToDriver(42);
  oData.PushValueToDriver(12345);
  oData.PushValueToDriver(666);
  oData.UpdateReadState();
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(std::vector<int>()));
}

TEST(VistaHistoryProjectNode, ProjectNodeHotYieldsLatestDriverValue) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_HOT);
  oData.PushValueToDriver(42);
  oData.PushValueToDriver(12345);
  oData.UpdateReadState();
  oData.PushValueToDriver(666);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 1);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(666));
}

TEST(VistaHistoryProjectNode, ProjectNodeIterateYieldsNothingWithoutDriverUpdate) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_ITERATE);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 0);
}

TEST(VistaHistoryProjectNode, ProjectNodeIterateYieldsNothingWithoutReadstateUpdate) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_ITERATE);
  oData.PushValueToDriver(42);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 0);
}

TEST(VistaHistoryProjectNode, ProjectNodeIterateYieldsAllReadstateValues) {
  HistoryProjectNodeWrapper oData(VdfnHistoryProjectNode::MD_ITERATE);
  oData.PushValueToDriver(42);
  oData.PushValueToDriver(12345);
  oData.UpdateReadState();
  oData.PushValueToDriver(666);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 2);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(std::vector<int>{42, 12345}));

  oData.PushValueToDriver(31415);
  oData.PushValueToDriver(299792458);
  oData.PushValueToDriver(-1);
  oData.UpdateReadState();
  oData.PushValueToDriver(13);
  oData.EvaluateNodes();
  ASSERT_EQ(oData.GetLastEvaluationCount(), 4);
  ASSERT_TRUE(oData.GetLastEvaluationResultEqualts(std::vector<int>{666, 31415, 299792458, -1}));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
