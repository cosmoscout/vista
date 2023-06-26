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

#include "VdfnDriverSensorNode.h"

#include <VistaBase/VistaTimer.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaSensorReadState.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaSensorReader.h>

#include <VistaDataFlowNet/VdfnObjectRegistry.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnDriverSensorNode::VdfnDriverSensorNode(
    VistaDeviceSensor* pSensor, VistaSensorReadState* readstate)
    : IVdfnNode()
    , m_pSensor(pSensor)
    , m_pHistoryPort(new HistoryPort)
    , m_nOldMeasureCount(0)
    , m_nLastUpdateIdx(0)
    , m_nLastUpdateTsFromReadState(0) {
  RegisterOutPort("history", m_pHistoryPort);
  // set history, but not using SetValue to prevent incrementing port's update count
  m_pHistoryPort->GetValueRef() = new VdfnHistoryPortData(
      m_pSensor->GetMeasures(), m_pSensor->GetMeasureTranscode(), m_pSensor, readstate);
}

VdfnDriverSensorNode::~VdfnDriverSensorNode() {
  delete m_pHistoryPort->GetValue();
}

bool VdfnDriverSensorNode::GetIsValid() const {
  return (m_pSensor != NULL);
}

bool VdfnDriverSensorNode::DoEvalNode() {
  // cache some values
  VdfnHistoryPortData* pHistoryData = m_pHistoryPort->GetValue();
  VistaDeviceSensor*   pSensor      = m_pHistoryPort->GetValue()->m_pSensor;
  IVistaDeviceDriver*  pDriver      = pSensor->GetParent();

  // get last update index from read state
  unsigned int nMeasureCount = pHistoryData->m_pReadState->GetMeasureCount();
  // experimental feature
  m_pHistoryPort->GetValue()->m_nRealNewMeasures = nMeasureCount - m_nOldMeasureCount;

  // determine difference to last update
  // |now - last| but no more than our desired read size (skimming, i.e. only what we are allowed to
  // read as client) note that m_nOldUpdateIndex is changed as a sideffect here to be of use in the
  // next update
  m_pHistoryPort->GetValue()->m_nNewMeasures =
      pHistoryData->m_pReadState->GetNewAvailableMeasureCount(m_nOldMeasureCount);
  m_pHistoryPort->GetValue()->m_nMeasureCount =
      nMeasureCount; // yes, we alias the readstate's field here (simplification during
                     // serialisation).

  // this is a race: but the value is for statistics and highly fluctuating
  m_pHistoryPort->GetValue()->m_nAvgDriverUpdTime = pDriver->GetAverageUpdateTime();
  m_pHistoryPort->GetValue()->m_nAvgUpdFreq       = pDriver->GetAverageUpdateFrequency();

  // indicate change on the port by timestamp
  m_pHistoryPort->SetUpdateTimeStamp(GetUpdateTimeStamp());

  // and revision
  m_pHistoryPort->IncUpdateCounter();

  // note the current index
  m_nOldMeasureCount = nMeasureCount;
  return true;
}

unsigned int VdfnDriverSensorNode::CalcUpdateNeededScore() const {
  // indicate change for this sensor by change of timestamp for
  // our observed readstate (polling)
  if (m_pHistoryPort->GetValue()->m_pReadState->GetUpdateTimeStamp() !=
      m_nLastUpdateTsFromReadState) {
    // mark last update ts
    m_nLastUpdateTsFromReadState = m_pHistoryPort->GetValue()->m_pReadState->GetUpdateTimeStamp();
    return ++m_nLastUpdateIdx; // new state -> update
  } else
    return m_nLastUpdateIdx;
}

// ############################################################################

VdfnDriverSensorNodeCreate::VdfnDriverSensorNodeCreate(
    VistaDriverMap* pMap, VdfnObjectRegistry* pReg)
    : m_pMap(pMap)
    , m_pReg(pReg) {
}

// #############################################################################

IVdfnNode* VdfnDriverSensorNodeCreate::CreateNode(const VistaPropertyList& oParams) const {
  try {
    const VistaPropertyList& subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

    std::string sDriver;
    if (subs.GetValue("driver", sDriver) == false) {
      vstr::warnp() << "[SensorNodeCreate]: - "
                    << "No parameter [driver] has been specified" << std::endl;
      return NULL;
    }
    int         nSensorID = -1;
    std::string sSensorName;

    const bool bHasSensorIndex = subs.GetValue("sensor_index", nSensorID);
    const bool bHasSensorName  = subs.GetValue("sensor_name", sSensorName);

    IVistaDeviceDriver* pDriver = m_pMap->GetDeviceDriver(sDriver);
    if (pDriver == NULL) {
      vstr::warnp() << "[DriverSensorNodeCreate]: Driver [" << sDriver << "] not found in map\n"
                    << vstr::indent << " Available Drivers:" << std::endl;
      if (std::distance(m_pMap->begin(), m_pMap->end()) == 0) {
        vstr::warni() << std::endl << "\tNO DRIVERS REGISTERED AT ALL." << std::endl << std::endl;
      } else {
        vstr::IndentObject oIndent;
        for (VistaDriverMap::const_iterator cit = m_pMap->begin(); cit != m_pMap->end(); ++cit) {
          vstr::warni() << (*cit).first << std::endl;
        }
      }
      return NULL;
    }

    VistaDeviceSensor* pSensor = NULL;

    // check for a sensor mapping aspect
    VistaDriverSensorMappingAspect* pMappingAspect = dynamic_cast<VistaDriverSensorMappingAspect*>(
        pDriver->GetAspectById(VistaDriverSensorMappingAspect::GetAspectId()));

    if (pMappingAspect) {
      std::string  sSensorType = subs.GetValueOrDefault<std::string>("type", "");
      unsigned int nType       = pMappingAspect->GetTypeId(sSensorType);
      if (nType == static_cast<unsigned int>(~0)) {
        vstr::warnp() << "[SensorNodeCreate]: - "
                      << "Driver [" << sDriver << "] has a sensor mapping aspect, "
                      << "but no sensor type with name [" << sSensorType << "].\n"
                      << vstr::indent << "Type names are: " << std::endl;
        std::list<std::string> liTypes = pMappingAspect->GetTypeNames();
        vstr::IndentObject     oIndent;
        for (std::list<std::string>::const_iterator it = liTypes.begin(); it != liTypes.end();
             ++it) {
          vstr::warni() << "[" << *it << "]" << std::endl;
        }
        return NULL;
      }

      // drivers with sensor mapping always need specific sensor targets
      // Exception: There is exactly one type and one sensor
      if (!bHasSensorIndex && !bHasSensorName) {
        if (pMappingAspect->GetNumberOfRegisteredTypes() != 1 &&
            pMappingAspect->GetNumRegisteredSensorsForType(nType) > 1) {
          vstr::warnp() << "[SensorNodeCreate]: - Driver [" << sDriver
                        << "] requires a sensor to be specified!"
                        << " Use \"sensor_name\" or \"sensor_index\"" << std::endl;
          return NULL;
        }
        unsigned int nId = pMappingAspect->GetSensorId(nType, 0);
        pSensor          = pDriver->GetSensorByIndex(nId);
      } else if (bHasSensorIndex) {
        unsigned int nId = pMappingAspect->GetSensorId(nType, nSensorID);
        pSensor          = pDriver->GetSensorByIndex(nId);
      } else if (bHasSensorName) {
        pSensor = pDriver->GetSensorByName(sSensorName);
      }

      if (pSensor == NULL) {
        if (bHasSensorName) {
          vstr::warnp() << "[SensorNodeCreate]: - "
                        << "Driver [" << sDriver << "] has no sensor with name [" << sSensorName
                        << "] for type [" << sSensorType << "].\n";
        } else {
          vstr::warnp() << "[SensorNodeCreate]: - "
                        << "Driver [" << sDriver << "] has no sensor with Id [" << nSensorID
                        << "] for type [" << sSensorType << "].\n";
        }
      }
    } else {
      // no SensorMappingAspect
      if (bHasSensorIndex) {
        pSensor = pDriver->GetSensorByIndex(nSensorID);
      } else if (bHasSensorName) {
        pSensor = pDriver->GetSensorByName(sSensorName);
      } else if (pDriver->GetNumberOfSensors() == 1) {
        // we only have one sensors, so we'll take this one
        pSensor = pDriver->GetSensorByIndex(0);
      } else {
        vstr::warnp() << "[SensorNodeCreate]: - Driver [" << sDriver
                      << "] requires a sensor to be specified! "
                      << "Use \"sensor_name\" or \"sensor_index\"" << std::endl;
        return NULL;
      }

      if (pSensor == NULL) {
        if (bHasSensorName) {
          vstr::warnp() << "[SensorNodeCreate]: - "
                        << "Driver [" << sDriver << "] has no sensor with name [" << sSensorName
                        << "].\n";
        } else {
          vstr::warnp() << "[SensorNodeCreate]: - "
                        << "Driver [" << sDriver << "] has no sensor with Id [" << nSensorID
                        << "].\n";
        }
        vstr::warn() << "Available sensors are: \n";
        vstr::IndentObject oIndent;
        if (pDriver->GetNumberOfSensors() == 0) {
          vstr::warn() << "NONE\n";
        } else {
          for (int i = 0; i < (int)pDriver->GetNumberOfSensors(); ++i) {
            vstr::warn() << "Id: " << i
                         << "\tName: " << pDriver->GetSensorByIndex(i)->GetSensorName()
                         << std::endl;
          }
        }
        return NULL;
      }
    }

    if (pSensor) {
      // FIXME: would be better to get this information from coh. group mgr, no?
      std::string sReaderTag = oParams.GetValueOrDefault<std::string>("tag", "");

      IVistaReadStateSource* pReader =
          dynamic_cast<IVistaReadStateSource*>(m_pReg->RetrieveNameable(sReaderTag));

      if (!pReader) {
        if (sReaderTag.empty() == false)
          vstr::out() << "NOTE: Could not retrieve read-state-source for tag [" << sReaderTag
                      << "]. Using default." << std::endl;
        // vstr::warnp() << "Could not retrieve read-state-source for tag [" << sReaderTag << "]" <<
        // std::endl;

        // go with default
        pReader = m_pMap->GetReadstateSource();
      }

      if (!pReader) {
        vstr::warnp() << "[SensorNodeCreate]: - Sensor has no reader. "
                      << "This is mandatory to create a DriverSensorNode()" << std::endl;
      } else {
        VistaSensorReadState* pReadState = pReader->GetReadState(*pSensor);
        if (!pReadState) {
          vstr::warnp() << "[SensorNodeCreate]: - "
                        << "could not request  readstate [" << sReaderTag << "]" << std::endl;
          return NULL;
        } else {
          return new VdfnDriverSensorNode(pSensor, pReadState);
        }
      }
    } else {
      vstr::warnp() << "[DriverSensorNodeCreate]: Could not create SensorNode\n";
      vstr::IndentObject oIndent;
      vstr::warni() << "DRIVER       : [" << sDriver << "]\n";
      vstr::warni() << "SENSOR NAME  : [" << sSensorName << "]\n";
      vstr::warni() << "SENSOR INDEX : [" << nSensorID << "]" << std::endl;
    }

  } catch (VistaExceptionBase& x) { x.PrintException(); }

  return NULL;
}
