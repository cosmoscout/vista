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
#include <VistaTestingUtils/VistaTestingRandom.h>

#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>

#include <VistaDataFlowNet/VdfnGraph.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>
#include <VistaInterProcComm/Connections/VistaProgressMessage.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaKernel/Cluster/Utils/VistaMasterSlaveUtils.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

// Derived from IVistaSerializable:
//    VdfnGraph
//    VdfnPortSerializeAdapter
//      TVdfnPortSerializerAdapter
//    VistaMsg
//    VistaProgressMessage
//    IDLVDataPacket
//    DLVistaByteBufferPacket
//    VistaMasterSlave::Message
//    VistaClusterMode::ClusterInfo
//    VistaEvent
//      VistaInteractionEvent
//      VistaExternalMsgEvent
//      VistaCommandEvent
//
// Indirectly serializable
// X  VistaPropertyList
//    VdfnPort
//    VdfnHistoryPortData
//    TVdfnPortSerializerAdapter
//      VistaInteractionContext

static const int S_nNumIterations = 200;

namespace SerializableTest {
template <typename T>
T* NewSerializable() {
  return new T;
}

template <typename T>
T* RandomSerializable() {
  T* pT = new T;
  *pT   = VistaTesting::GenerateRandom<T>();
}

template <typename T>
bool CompareSerializables(T* pLeft, T* pRight) {
  return (&pLeft == &pRight);
}
} // namespace SerializableTest

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

TEST(VistaSerializableTest, VistaPropertyList) {
  VistaByteBufferSerializer   oSer;
  VistaByteBufferDeSerializer oDeSer;

  for (int i = 0; i < S_nNumIterations; ++i) {
    oSer.ClearBuffer();

    VistaPropertyList oSource     = VistaTesting::GenerateRandom<VistaPropertyList>();
    std::string       sNameSource = VistaTesting::GenerateRandom<std::string>();
    VistaPropertyList oTarget;
    std::string       sNameTarget;
    VistaPropertyList::SerializePropertyList(oSer, oSource, sNameSource);
    oDeSer.SetBuffer(oSer.GetBuffer(), oSer.GetBufferSize());

    VistaPropertyList::DeSerializePropertyList(oDeSer, oTarget, sNameTarget);

    ASSERT_TRUE(VistaTesting::Compare(sNameSource, sNameTarget));
    ASSERT_TRUE(VistaTesting::Compare(oSource, oTarget));
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  unsigned int nSeed = VistaTesting::InitializeRandomSeed();
  SCOPED_TRACE("RandomSeed: " + VistaConversion::ToString(nSeed));
  return RUN_ALL_TESTS();
}
