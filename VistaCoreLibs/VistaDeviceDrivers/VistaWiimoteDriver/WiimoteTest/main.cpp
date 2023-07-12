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

#include <VistaDeviceDrivers/WiimoteDriver/VistaWiimoteDriver.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaPentiumBasedRTC.h>

#include <VistaDataFlowNet/VdfnGraph.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnObjectRegistry.h>
#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnUtil.h>

#include <VistaBase/VistaTimer.h>

#include <VistaBase/VistaDefaultTimerImp.h>
#include <signal.h>

extern "C" IVistaDriverCreationMethod*     GetCreationMethod(IVistaTranscoderFactoryFactory* fac);
extern "C" IVistaTranscoderFactoryFactory* CreateTranscoderFactoryFactory();

class CPrintNode : public IVdfnNode {
 public:
  CPrintNode()
      : m_pIn(NULL) {
    RegisterInPortPrototype("in", new TVdfnPortTypeCompare<TVdfnPort<std::string>>);
  }

  bool PrepareEvaluationRun() {
    m_pIn = dynamic_cast<TVdfnPort<std::string>*>(GetInPort("in"));
    return GetIsValid();
  }

 protected:
  bool DoEvalNode() {
    std::cout << m_pIn->GetValue() << std::endl;
    std::cout.flush();

    return true;
  }

  TVdfnPort<std::string>* m_pIn;
};

class CDetermineQuit : public IVdfnNode {
 public:
  CDetermineQuit()
      : IVdfnNode()
      , m_pQuitter(NULL)
      , nTimes(0) {
    RegisterInPortPrototype("in", new TVdfnPortTypeCompare<TVdfnPort<double>>);
  }

  bool PrepareEvaluationRun() {
    m_pQuitter = dynamic_cast<TVdfnPort<double>*>(GetInPort("in"));
    return GetIsValid();
  }

 protected:
  bool DoEvalNode() {
    if (nTimes++ > 2) {
      std::cout << "CDetermineQuit -- quit" << std::endl;
      return false; // will cause the graph to return false and
                    // abort the evaluation
    } else {
      std::cout << "nTimes = " << nTimes << std::endl;
    }
    return true;
  }

 private:
  TVdfnPort<double>* m_pQuitter;
  unsigned int       nTimes;
};

bool bDone = false;

void sigc_handler(int mask) {
  bDone = true;
}

int main(int argc, char** argv) {

  signal(SIGINT, sigc_handler);

  IVistaTimerImp::SetSingleton(new VistaDefaultTimerImp);

  VistaDriverMap     oMap;
  VdfnObjectRegistry oReg;

  VdfnUtil::InitVdfn(&oMap, &oReg);

  VdfnNodeFactory::GetSingleton()->SetNodeCreator("print", new TVdfnDefaultNodeCreate<CPrintNode>);
  VdfnNodeFactory::GetSingleton()->SetNodeCreator(
      "quitter", new TVdfnDefaultNodeCreate<CDetermineQuit>);

  IVistaTranscoderFactoryFactory* pfac     = CreateTranscoderFactoryFactory();
  IVistaDriverCreationMethod*     crm      = GetCreationMethod(pfac);
  VistaWiimoteDriver*             pWiiMote = new VistaWiimoteDriver(crm);

  if (pWiiMote->Connect()) {
    oMap.AddDeviceDriver("WIIMOTE", pWiiMote);
    // ok create and map sensors
    pWiiMote->EnableWiiMoteSensor(1, 10.0f);
    pWiiMote->EnableStateSensor(1, 10.0f);
    pWiiMote->EnableNunchukSensor(1, 10.0f);

    VistaWiimoteDriver::CWiiMoteParameters* pParams = pWiiMote->GetParameters();
    pParams->SetAccelerationThreshold(0);
    pParams->SetDoesAccelerationReports(false);
    pParams->SetDoesIRReports(false);

    // enable
    pWiiMote->SetIsEnabled(true);

    // setup dfn
    VdfnGraph* pGraph = VdfnPersistence::LoadGraph("wiimote.xml", "wiimote", true, true);
    if (pGraph) {
      VdfnPersistence::SaveAsDot(pGraph, "wiimote.dot", "wiimote", true, false);
      do {
        for (VistaDriverMap::iterator it = oMap.begin(); it != oMap.end(); ++it) {
          if ((*it).second->GetUpdateType() == IVistaDeviceDriver::UPDATE_EXPLICIT_POLL)
            (*it).second->Update();
          (*it).second->SwapSensorMeasures();
        }
      } while (!bDone and pGraph->EvaluateGraph(IVistaTimerImp::GetSingleton()->GetMicroTime()));
    } else {
      std::cerr << "could not load graph..." << std::endl;
    }

    pWiiMote->SetIsEnabled(false);

    // remove from map
    oMap.RemDeviceDriver("WIIMOTE ");
  } else {
    std::cerr << "wiimote did not connect properly.\n";
  }

  // gently delete
  delete pWiiMote;
  delete crm;
  delete pfac;

  std::cout << "bye..." << std::endl;
  return 0;
}
