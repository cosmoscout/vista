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

#if defined(SUNOS)
#include <sunmath.h>
#endif

#include "VistaSystemConfigurators.h"
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaAspects/VistaPropertyFunctorRegistry.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDeviceIdentificationAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverLoggingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverReferenceFrameAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaInterProcComm/Connections/VistaConnectionFile.h>
#include <VistaInterProcComm/Connections/VistaConnectionFileTimed.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaConnectionSerial.h>

#include "DisplayManager/VistaWindowingToolkit.h"
#include <list>
#include <memory>
#include <set>
#include <string>

using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

void VistaConnectionConfigurator::IConnectionFactoryMethod::FailOnConnection(
    VistaConnection* pCon, const std::string& sReason) const {
  delete pCon;
  vstr::warnp() << " [VistaIM] Failed on connection. Cause given: " << sReason << "\n";
  vstr::warni() << vstr::singleindent << " [VistaIM] Giving up. This connection will NOT work."
                << std::endl;
}

class VistaSerialConnectionFactoryMethod
    : public VistaConnectionConfigurator::IConnectionFactoryMethod {
 public:
  VistaSerialConnectionFactoryMethod()
      : IConnectionFactoryMethod() {
  }

  VistaConnection* CreateConnection(const VistaPropertyList& oProps) {
    VistaConnectionSerial* pConn = NULL;

    // Read information from config file and set connection properties
    std::string sPortName;
    oProps.GetValue("PORTNAME", sPortName);
    int nPort = -1;

    if (sPortName.empty()) {
      if (oProps.GetValue("PORT", nPort)) {
        oProps.GetValue("PORT", nPort);
        pConn = new VistaConnectionSerial;
        pConn->SetPortByIndex(nPort);
      }
    } else {
      pConn = new VistaConnectionSerial;
      pConn->SetPortByName(sPortName);
    }

    if (!pConn)
      return NULL;

    int nSpeed = oProps.GetValueOrDefault<int>("SPEED", 9600);
    pConn->SetSpeed(nSpeed);

    if (oProps.HasProperty("PARITY")) {
      std::string sParity;
      oProps.GetValue("PARITY", sParity);
      if (VistaAspectsComparisonStuff::StringEquals(sParity, "odd", false)) {
        pConn->SetParity(1);
      } else if (VistaAspectsComparisonStuff::StringEquals(sParity, "even", false)) {
        pConn->SetParity(2);
      } else if (VistaAspectsComparisonStuff::StringEquals(sParity, "none", false)) {
        pConn->SetParity(0);
      }
    } else
      pConn->SetParity(0);

    int nBits = oProps.GetValueOrDefault("DATABITS", 8);
    pConn->SetDataBits(nBits);

    if (oProps.HasProperty("STOPBITS")) {
      std::string sStopBits;
      oProps.GetValue("STOPBITS", sStopBits);
      if (sStopBits == "2")
        pConn->SetStopBits(2.0f);
      else if (sStopBits == "1.5" || sStopBits == "1,5")
        pConn->SetStopBits(1.5f);
      else
        pConn->SetStopBits(1);
    } else
      pConn->SetStopBits(1);

    bool bHwFlow = oProps.GetValueOrDefault("HARDWAREFLOW", false);
    pConn->SetHardwareFlow(bHwFlow);

    bool bSwFlow = oProps.GetValueOrDefault("SOFTWAREFLOW", false);
    pConn->SetSoftwareFlow(bSwFlow);

    if (pConn->Open() == false) {
      // will delete pConn for me!
      FailOnConnection(pConn, "COULD NOT OPEN SERIAL PORT");
      return NULL;
    }

    return pConn;
  }
};

class VistaIPConnectionFactoryMethod
    : public VistaConnectionConfigurator::IConnectionFactoryMethod {
 public:
  VistaConnection* CreateConnection(const VistaPropertyList& oProps) {
    std::string strHost;
    int         nPort = -1;
    if (oProps.GetValue("ADDRESS", strHost) == false || oProps.GetValue("PORT", nPort) == false ||
        nPort <= 0) {
      vstr::warnp() << "VistaIPConnectionFactoryMethod::CreateConnection() -- "
                    << "Missing or invalid ADDRESS and/or PORT values" << std::endl;
      return NULL;
    }

    std::string sType;
    if (oProps.GetValue("TYPE", sType) == false) {
      vstr::warnp() << "VistaIPConnectionFactoryMethod::CreateConnection() -- "
                    << "No type has been specified" << std::endl;
      return NULL;
    }

    VistaConnectionIP* pCon = NULL;
    if (sType == "TCP")
      pCon = CreateTCPConnection(strHost, nPort, oProps);
    else if (sType == "UDP")
      pCon = CreateUDPConnection(strHost, nPort, oProps);
    else {
      vstr::warnp() << "VistaIPConnectionFactoryMethod::CreateConnection() -- "
                    << "Given type [" << sType << "] unknown - use TCP or UDP" << std::endl;
      return NULL;
    }

    if (!pCon)
      return NULL;

    bool bBlocking   = oProps.GetValueOrDefault<bool>("BLOCKING", true);
    bool bBuffering  = oProps.GetValueOrDefault<bool>("BUFFERING", false);
    bool bBufferSwap = oProps.GetValueOrDefault<bool>("BYTESWAP", true);
    bool bOpen       = oProps.GetValueOrDefault<bool>("OPEN", true);

    pCon->SetIsBlocking(bBlocking);
    pCon->SetIsBuffering(bBuffering);
    pCon->SetByteorderSwapFlag(bBufferSwap
                                   ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                                   : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
    pCon->SetLingerTime(oProps.GetValueOrDefault<int>("LINGERTIME", 0));
    if (bOpen) {
      if (pCon->GetIsOpen() == false)
        pCon->Open();
    } else {
      if (pCon->GetIsOpen())
        pCon->Close();
    }

    return pCon;
  }

  VistaConnectionIP* CreateUDPConnection(
      const std::string& strHost, int nPort, const VistaPropertyList& oProps) {
    std::string        sDirection = oProps.GetValue<std::string>("DIRECTION");
    VistaConnectionIP* pConn      = NULL;
    if (VistaAspectsComparisonStuff::StringEquals(sDirection, "INCOMING", false)) {
      pConn = new VistaConnectionIP(strHost, nPort);
    } else if (VistaAspectsComparisonStuff::StringEquals(sDirection, "OUTGOING", false)) {
      pConn = new VistaConnectionIP(VistaConnectionIP::CT_UDP, strHost, nPort);
    }

    return pConn;
  }

  VistaConnectionIP* CreateTCPConnection(
      const std::string& strHost, int nPort, const VistaPropertyList& oProps) {
    VistaConnectionIP* pCon = new VistaConnectionIP(VistaConnectionIP::CT_TCP, strHost, nPort);
    return pCon;
  }
};

class VistaTimedFileConnectionFactoryMethod
    : public VistaConnectionConfigurator::IConnectionFactoryMethod {
 public:
  VistaConnection* CreateConnection(const VistaPropertyList& oProps) {
    std::string sFileName  = oProps.GetValue<std::string>("FILENAME");
    float       fFrequency = oProps.GetValueOrDefault<float>("FREQUENCY", 1.0f);
    bool        bPulse     = oProps.GetValueOrDefault<bool>("PULSE", false);
    bool        bByteSwap  = oProps.GetValueOrDefault<bool>("BYTESWAP", false);

    std::string strDirection;
    if (oProps.GetValue("DIRECTION", strDirection) == false)
      return NULL;

    bool bIncoming = false;
    if (VistaAspectsComparisonStuff::StringEquals(strDirection, "INCOMING", false))
      bIncoming = true;

    VistaConnectionFileTimed* pConn =
        new VistaConnectionFileTimed(sFileName, bPulse, fFrequency, bIncoming);

    pConn->SetByteorderSwapFlag(bByteSwap
                                    ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                                    : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);

    return pConn;
  }
};

class VistaFileConnectionFactoryMethod
    : public VistaConnectionConfigurator::IConnectionFactoryMethod {
 public:
  VistaConnection* CreateConnection(const VistaPropertyList& oProps) {
    std::string sFileName = oProps.GetValue<std::string>("FILENAME");
    bool        bByteSwap = oProps.GetValueOrDefault("BYTESWAP", false);

    std::string strDirection;
    if (oProps.GetValue("DIRECTION", strDirection) == false)
      return NULL;

    int mode = VistaConnectionFile::READ;
    // if(VistaAspectsComparisonStuff::StringEquals(strDirection, "INCOMING", false))
    //  mode = VistaConnectionFile::READ;
    if (VistaAspectsComparisonStuff::StringEquals(strDirection, "OUTGOING", false))
      mode = VistaConnectionFile::WRITE;
    else if (VistaAspectsComparisonStuff::StringEquals(strDirection, "DUPLEX", false))
      mode = VistaConnectionFile::READWRITE;

    VistaConnectionFile* pConn = new VistaConnectionFile(sFileName, mode);
    pConn->SetByteorderSwapFlag(bByteSwap
                                    ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                                    : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);

    return pConn;
  }
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaWindowConfigurator::VistaWindowConfigurator(VistaDisplayManager* pDispSys)
    : m_pDispMgr(pDispSys) {
}
VistaWindowConfigurator::~VistaWindowConfigurator() {
  for (std::pair<VistaWindow*, VistaDriverAbstractWindowAspect::IWindowHandle*> oHandle :
      m_mapHandles) {
    delete oHandle.second;
  }
}

bool VistaWindowConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  std::list<std::string>  liWindowNames;
  std::list<VistaWindow*> liWindows;
  if (oDriverSection.GetValue("WINDOWS", liWindowNames)) {
    for (std::list<std::string>::const_iterator itWindow = liWindowNames.begin();
         itWindow != liWindowNames.end(); ++itWindow) {
      VistaWindow* pWindow = m_pDispMgr->GetWindowByName((*itWindow));
      if (pWindow == NULL) {
        vstr::warnp() << "[VistaWindowConfigurator]: Driver requests Window [" << (*itWindow)
                      << "] which does not exist" << std::endl;
        continue;
      }
      liWindows.push_back(pWindow);
    }
  } else {
    // no windows explicitely provided, there may be a request to use defaultwindow
    bool bDefaultWindow = oDriverSection.GetValueOrDefault<bool>("DEFAULTWINDOW", true);
    if (bDefaultWindow) {
      const std::map<std::string, VistaWindow*>& mapWindows = m_pDispMgr->GetWindowsConstRef();
      for (std::map<std::string, VistaWindow*>::const_iterator cit = mapWindows.begin();
           cit != mapWindows.end(); ++cit) {
        liWindows.push_back((*cit).second);
      }
    }
  }

  VistaDriverAbstractWindowAspect* pAsp = dynamic_cast<VistaDriverAbstractWindowAspect*>(
      pDriver->GetAspectById(VistaDriverAbstractWindowAspect::GetAspectId()));
  if (pAsp == NULL)
    return false;

  std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> liCurrentWindows =
      pAsp->GetWindowList();
  for (std::list<VistaDriverAbstractWindowAspect::IWindowHandle*>::const_iterator cit =
           liCurrentWindows.begin();
       cit != liCurrentWindows.end(); ++cit) {
    pAsp->DetachFromWindow(*cit);
  }

  for (std::list<VistaWindow*>::const_iterator itWin = liWindows.begin(); itWin != liWindows.end();
       ++itWin) {
    VistaDriverAbstractWindowAspect::IWindowHandle* pHandle = GetHandleForWindow((*itWin));
    pAsp->AttachToWindow(pHandle);
  }

  return true;
}

VistaDriverAbstractWindowAspect::IWindowHandle* VistaWindowConfigurator::GetHandleForWindow(
    VistaWindow* pWindow) {
  VistaDriverAbstractWindowAspect::IWindowHandle* pHandle = m_mapHandles[pWindow];
  if (pHandle == NULL) {
    pHandle               = new IVistaWindowingToolkit::VistaKernelWindowHandle(pWindow);
    m_mapHandles[pWindow] = pHandle;
  }
  return pHandle;
}

// ############################################################################

VistaSensorMappingConfigurator::VistaSensorMappingConfigurator()
    : VistaDriverPropertyConfigurator::IConfigurator() {
}

VistaSensorMappingConfigurator::~VistaSensorMappingConfigurator() {
}

bool VistaSensorMappingConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  VistaDriverSensorMappingAspect* pSensorAsp = dynamic_cast<VistaDriverSensorMappingAspect*>(
      pDriver->GetAspectById(VistaDriverSensorMappingAspect::GetAspectId()));

  std::list<std::string> liSensors;
  oDriverSection.GetValue("SENSORS", liSensors);

  for (std::list<std::string>::const_iterator itSensor = liSensors.begin();
       itSensor != liSensors.end(); ++itSensor) {
    // get sensor PropertyList
    if (oConfig.HasSubList(*itSensor) == false) {
      vstr::warnp() << "[SensorMappingConfiguator]: Driver requests sensor [" << (*itSensor)
                    << "], but no such entry exists" << std::endl;
      continue;
    }

    const VistaPropertyList& oSensor = oConfig.GetSubListConstRef(*itSensor);
    std::string              sType   = oSensor.GetValueOrDefault<std::string>("TYPE", "");
    std::string sSensorName          = oSensor.GetValueOrDefault<std::string>("NAME", (*itSensor));
    int         nHistorySize = std::max<int>(2, oSensor.GetValueOrDefault<int>("HISTORY", 5));

    int nRawId;
    if (oSensor.GetValue<int>("RAWID", nRawId) == false) {
      vstr::warnp() << "[SensorMappingConfiguator]: Driver requests sensor [" << (*itSensor)
                    << "], which has no RAWID entry" << std::endl;
      continue;
    }

    if (pSensorAsp) // driver has sensor mapping, so use this to set up the sensors...
    {
      VistaDeviceSensor* pSensor = new VistaDeviceSensor;
      pSensor->SetTypeHint(sType);
      pSensor->SetSensorName(sSensorName);

      unsigned int nDriverSensorId = pDriver->AddDeviceSensor(pSensor);
      if (nDriverSensorId != IVistaDriverCreationMethod::INVALID_TYPE) {
        unsigned int nMappingType = pSensorAsp->GetTypeId(sType);

        pSensorAsp->SetSensorId(nMappingType, nRawId, nDriverSensorId);

        pDriver->SetupSensorHistory(pSensor, nHistorySize, 66.6);

        // create transcoder for sensor
        IVistaMeasureTranscoderFactory* pFac =
            pDriver->GetFactory()->GetTranscoderFactoryForSensor(sType);
        if (pFac)
          pSensor->SetMeasureTranscode((*pFac).CreateTranscoder());
      }
    } else // no sensor mapping...
    {
      VistaDeviceSensor* pSensor = pDriver->GetSensorByIndex(nRawId);
      if (pSensor) {
        // ok, this, at least worked..
        if (pSensor->GetSensorName().empty())
          pSensor->SetSensorName(sSensorName);
        // while here... we setup the history properly
        pDriver->SetupSensorHistory(pSensor, nHistorySize, 66.6);
      }
    }
  }

  return true;
}

// #############################################################################

bool VistaDriverProtocolConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  IVistaDriverProtocolAspect* pProt = dynamic_cast<IVistaDriverProtocolAspect*>(
      pDriver->GetAspectById(IVistaDriverProtocolAspect::GetAspectId()));
  if (pProt == NULL)
    return true; // optional aspect

  std::string sProtocolSection;
  if (oDriverSection.GetValue("PROTOCOL", sProtocolSection) == false)
    return false;
  if (oConfig.HasSubList(sProtocolSection) == false)
    return false;
  const VistaPropertyList& oProtocolConfig = oConfig.GetSubListConstRef(sProtocolSection);

  std::string sProtocolName;
  if (oProtocolConfig.GetValue("NAME", sProtocolName) == false)
    return false;
  std::string sRevision = oProtocolConfig.GetValueOrDefault<std::string>("REVISION", "");

  IVistaDriverProtocolAspect::_cVersionTag cTag(sProtocolName, sRevision);
  if (pProt->SetProtocol(cTag) == false)
    return false; // protocol given, but could not be set... this is an error!
  return true;    // ok
}

// #############################################################################

VistaGenericHistoryConfigurator::VistaGenericHistoryConfigurator() {
}

VistaGenericHistoryConfigurator::~VistaGenericHistoryConfigurator() {
}

bool VistaGenericHistoryConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {

  unsigned int nHistorySize;
  if (oDriverSection.GetValue("HISTORY", nHistorySize) == false) {
    vstr::warnp() << "[VistaGenericHistoryConfigurator]: Could not find entry "
                  << "HISTORY" << std::endl;
    return false;
  }

  VistaDriverMeasureHistoryAspect* pHist = dynamic_cast<VistaDriverMeasureHistoryAspect*>(
      pDriver->GetAspectById(VistaDriverMeasureHistoryAspect::GetAspectId()));
  if (pHist) {
    // if(pDriver->GetNumberOfSensors() == 0)
    //{
    //	pDriver->SetDefaultHistorySize( std::max<unsigned int>( 1, nHistorySize ) );
    //}
    // else
    //{
    for (unsigned int n = 0; n < pDriver->GetNumberOfSensors(); ++n) {
      if (pHist->GetIsRegistered(pDriver->GetSensorByIndex(n))) {
        pDriver->SetupSensorHistory(
            pDriver->GetSensorByIndex(n), std::max<unsigned int>(1, nHistorySize), 66.6);
      }
    }
    //}
  }
  return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaConnectionConfigurator::VistaConnectionConfigurator()
    : VistaDriverPropertyConfigurator::IConfigurator() {
  m_mpFactories["SERIAL"]   = new VistaSerialConnectionFactoryMethod;
  m_mpFactories["UDP"]      = new VistaIPConnectionFactoryMethod;
  m_mpFactories["TCP"]      = new VistaIPConnectionFactoryMethod;
  m_mpFactories["TIMEFILE"] = new VistaTimedFileConnectionFactoryMethod;
  m_mpFactories["FILE"]     = new VistaFileConnectionFactoryMethod;
}

VistaConnectionConfigurator::~VistaConnectionConfigurator() {
  for (std::map<std::string, IConnectionFactoryMethod*>::iterator it = m_mpFactories.begin();
       it != m_mpFactories.end(); ++it)
    delete (*it).second;
}

bool VistaConnectionConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  VistaDriverConnectionAspect* pConAsp = dynamic_cast<VistaDriverConnectionAspect*>(
      pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId()));
  if (pConAsp == NULL)
    return false;

  std::list<std::string> liConnections;
  if (oDriverSection.GetValue("CONNECTIONS", liConnections) == false)
    return false;

  for (std::list<std::string>::const_iterator itConnName = liConnections.begin();
       itConnName != liConnections.end(); ++itConnName) {
    if (oConfig.HasSubList((*itConnName)) == false) {
      vstr::warnp() << "[VistaConnectionConfigurator]: Requested connection [" << (*itConnName)
                    << "] has no matching section" << std::endl;
      continue;
    }
    const VistaPropertyList& oConSection = oConfig.GetSubListConstRef((*itConnName));

    VistaConnection* pCon = NULL;

    // determine con-settings (type)
    std::string sType;
    if (oConSection.GetValue("TYPE", sType) == false) {
      vstr::warnp() << "[VistaConnectionConfigurator]: Requested connection [" << (*itConnName)
                    << "] has no TYPE specified" << std::endl;
      continue;
    }

    std::string sDriverRole;
    if (oConSection.GetValue("DRIVERROLE", sDriverRole) == false) {
      vstr::warnp() << "[VistaConnectionConfigurator]: Requested connection [" << (*itConnName)
                    << "] has no DRIVERROLE specified" << std::endl;
      continue;
    }

    unsigned int nRoleId = pConAsp->GetIndexForRole(sDriverRole);
    if (nRoleId == ~0u) {
      vstr::warnp() << "[VistaConnectionConfigurator]: Requested connection [" << (*itConnName)
                    << "] has unknown DRIVERROLE [" << sDriverRole << "]" << std::endl;
      continue;
    }

    const_iterator fcIt = m_mpFactories.find(sType);
    if (fcIt == m_mpFactories.end()) {
      vstr::warnp() << "[VistaConnectionConfigurator]: Requested connection [" << (*itConnName)
                    << "] has unknown TYPE [" << sType << "]" << std::endl;
      continue;
    }

    pCon = (*fcIt).second->CreateConnection(oConSection);
    if (pCon == NULL) {
      vstr::warnp() << "[VistaConnectionConfigurator]: Requested connection [" << (*itConnName)
                    << "] could not be created" << std::endl;
      return false;
    } else {
      pConAsp->SetConnection(
          nRoleId, pCon, sDriverRole, VistaDriverConnectionAspect::FORBID_CONNECTION_REPLACEMENT);
    }
  }

  return true;
}

VistaConnectionConfigurator::const_iterator VistaConnectionConfigurator::begin() const {
  return m_mpFactories.begin();
}

VistaConnectionConfigurator::iterator VistaConnectionConfigurator::begin() {
  return m_mpFactories.begin();
}

VistaConnectionConfigurator::const_iterator VistaConnectionConfigurator::end() const {
  return m_mpFactories.end();
}

VistaConnectionConfigurator::iterator VistaConnectionConfigurator::end() {
  return m_mpFactories.end();
}

bool VistaConnectionConfigurator::AddConFacMethod(
    const std::string& sKey, IConnectionFactoryMethod* pMt) {
  if (m_mpFactories.find(sKey) != m_mpFactories.end())
    return false;

  m_mpFactories[sKey] = pMt;
  return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDriverLoggingConfigurator::VistaDriverLoggingConfigurator() {
}

VistaDriverLoggingConfigurator::~VistaDriverLoggingConfigurator() {
}

bool VistaDriverLoggingConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  std::string sSectionName;
  if (oDriverSection.GetValue("DRIVERLOGGING", sSectionName) == false)
    return false;
  if (oConfig.HasSubList(sSectionName) == false)
    return false;
  const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sSectionName);

  /** @todo maybe a lookup structure would be better and more dynamic */
  VistaDriverLoggingAspect* pAsp = NULL;
  std::string               sType;
  if (oDriverSection.GetValue("TYPE", sType) == false)
    return false;
  if (sType == "DEFAULT") {
    // check for old logging aspect
    IVistaDeviceDriver::IVistaDeviceDriverAspect* pOld =
        pDriver->GetAspectById(VistaDriverLoggingAspect::GetAspectId());
    if (pOld) {
      pDriver->UnregisterAspect(pOld,
          IVistaDeviceDriver::DO_NOT_DELETE_ASPECT); // remove, but do not delete, maybe a leak?
    }

    pDriver->RegisterAspect((pAsp = new VistaDriverLoggingAspect));
  }

  if (pAsp) {
    bool        bEnabled  = oSection.GetValueOrDefault<bool>("ENABLED", true);
    int         iLoglevel = oSection.GetValueOrDefault<int>("ENABLED", 0);
    std::string sSymbol;
    oSection.GetValue("ENABLED", sSymbol);

    pAsp->SetEnabled(bEnabled);
    pAsp->SetLogLevel(iLoglevel);
    pAsp->SetMnemonic(sSymbol);
  }
  return (pAsp != NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDriverAttachOnlyConfigurator::VistaDriverAttachOnlyConfigurator() {
}

VistaDriverAttachOnlyConfigurator::~VistaDriverAttachOnlyConfigurator() {
}

bool VistaDriverAttachOnlyConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  bool bDo = oDriverSection.GetValueOrDefault<bool>("ATTACHONLY", false);
  pDriver->SetAttachOnly(bDo);
  return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDeviceIdentificationConfigurator::VistaDeviceIdentificationConfigurator() {
}

VistaDeviceIdentificationConfigurator::~VistaDeviceIdentificationConfigurator() {
}

bool VistaDeviceIdentificationConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  std::string sSectionName;
  if (oDriverSection.GetValue("IDENTIFICATION", sSectionName) == false)
    return false;
  if (oConfig.HasSubList(sSectionName) == false)
    return false;
  const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sSectionName);

  VistaDeviceIdentificationAspect* pAsp = dynamic_cast<VistaDeviceIdentificationAspect*>(
      pDriver->GetAspectById(VistaDeviceIdentificationAspect::GetAspectId()));
  if (!pAsp) {
    vstr::warnp() << "[VistaDeviceIdentificationConfigurator]: "
                  << "IdentificationAspect not found, creating the aspect." << std::endl;
    pAsp = new VistaDeviceIdentificationAspect;
    pDriver->RegisterAspect(pAsp);
  }

  int         iValue;
  std::string sName;

  if (oSection.GetValue("VENDORID", iValue))
    pAsp->SetVendorID(iValue);
  if (oSection.GetValue("PRODUCTID", iValue))
    pAsp->SetProductID(iValue);
  if (oSection.GetValue("SERIALNUMBER", iValue))
    pAsp->SetSerialNumber(iValue);
  if (oSection.GetValue("DEVICENAME", sName))
    pAsp->SetDeviceName(sName);

  return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDriverReferenceFrameConfigurator::VistaDriverReferenceFrameConfigurator() {
}

VistaDriverReferenceFrameConfigurator::~VistaDriverReferenceFrameConfigurator() {
}

bool VistaDriverReferenceFrameConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  std::string sSectionName;
  if (oDriverSection.GetValue("REFERENCE_FRAME", sSectionName) == false)
    return false;
  if (oConfig.HasSubList(sSectionName) == false)
    return false;
  const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sSectionName);

  IVistaDriverReferenceFrameAspect* pAspect = dynamic_cast<IVistaDriverReferenceFrameAspect*>(
      pDriver->GetAspectById(IVistaDriverReferenceFrameAspect::GetAspectId()));
  if (pAspect == NULL)
    return false;

  std::string sHemisphere;
  if (oSection.GetValue("HEMISPHERE", sHemisphere)) {
    IVistaDriverReferenceFrameAspect::eHemisphereCode iHemisphere =
        IVistaDriverReferenceFrameAspect::eHemisphereCode(-1);
    if (sHemisphere == "UP")
      iHemisphere = IVistaDriverReferenceFrameAspect::HS_UP;
    else if (sHemisphere == "DOWN")
      iHemisphere = IVistaDriverReferenceFrameAspect::HS_DOWN;
    else if (sHemisphere == "LEFT")
      iHemisphere = IVistaDriverReferenceFrameAspect::HS_LEFT;
    else if (sHemisphere == "RIGHT")
      iHemisphere = IVistaDriverReferenceFrameAspect::HS_RIGHT;
    else if (sHemisphere == "FRONT")
      iHemisphere = IVistaDriverReferenceFrameAspect::HS_FRONT;
    else if (sHemisphere == "AFT" || sHemisphere == "BACK")
      iHemisphere = IVistaDriverReferenceFrameAspect::HS_AFT;
    if (iHemisphere != IVistaDriverReferenceFrameAspect::eHemisphereCode(-1)) {
      if (pAspect->SetHemisphere(iHemisphere) == false) {
        vstr::warnp() << "[ReferenceFrameConfigurator]: SetHemisphere with parameter ["
                      << sHemisphere << "] failed! Maybe its not supported by the driver"
                      << std::endl;
      }
    } else {
      vstr::warnp() << "[ReferenceFrameConfigurator]: Hemisphere parameter [" << sHemisphere
                    << "] is invalid!" << std::endl;
    }
  }

  std::list<std::string> liStringList;
  VistaEulerAngles       oAngles;
  VistaTransformMatrix   oMatrix;
  if (oSection.HasProperty("EMITTER_ALIGNMENT")) {
    oSection.GetValue("EMITTER_ALIGNMENT", liStringList);
    if (oSection.GetValue("EMITTER_ALIGNMENT", oAngles)) {
      if (pAspect->SetEmitterAlignment(oAngles) == false) {
        vstr::warnp() << "[ReferenceFrameConfigurator]: SetEmitterAlignment( angles )"
                      << " returned false. Maybe it is not supported by the driver?" << std::endl;
      }
    } else if (oSection.GetValue("EMITTER_ALIGNMENT", oMatrix)) {
      if (pAspect->SetEmitterAlignment(oMatrix) == false) {
        vstr::warnp() << "[ReferenceFrameConfigurator]: SetEmitterAlignment( matrix )"
                      << " returned false. Maybe it is not supported by the driver?" << std::endl;
      }
    } else {
      vstr::warnp() << "[ReferenceFrameConfigurator]: Emitter Alignment value has"
                    << " invalid length. Use either angles (3 floats) or matrix (16 floats)."
                    << std::endl;
    }
  }

  std::list<std::string> liSensorSections;
  oSection.GetValue("SENSOR_ALIGNMENT", liSensorSections);
  for (std::list<std::string>::const_iterator cit = liSensorSections.begin();
       cit != liSensorSections.end(); ++cit) {
    if (oConfig.HasSubList((*cit)) == false) {
      vstr::warnp() << "[ReferenceFrameConfigurator]: "
                    << "SENSOR_ALIGNEMT request sensor from non-existing section [" << (*cit)
                    << "] is invalid!" << std::endl;
    }
    VistaPropertyList oSubProps = oConfig.GetSubListConstRef((*cit));

    int                  iSensorID = 0;
    VistaTransformMatrix matTransform;
    if (oSubProps.GetValue("SENSOR_ID", iSensorID) &&
        oSubProps.GetValue("TRANSFORM", matTransform)) {
      if (pAspect->SetSensorAlignment(matTransform, iSensorID) == false) {
        vstr::warnp()
            << "[ReferenceFrameConfigurator]: SetSensorAlignment for SensorID [" << iSensorID
            << "from Section [" << (*cit) << "] returned false!"
            << " Maybe the ID is wrong, or sensor alignment is not supported by the driver."
            << std::endl;
      }

    } else {
      vstr::warnp() << "[ReferenceFrameConfigurator]: Sensor Alignment Section [" << (*cit)
                    << "] must contain valid entries for SENSOR_ID and ALIGNMENT!" << std::endl;
    }
  }

  return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaDriverParameterConfigurator::VistaDriverParameterConfigurator() {
}
VistaDriverParameterConfigurator::~VistaDriverParameterConfigurator() {
}

bool VistaDriverParameterConfigurator::Configure(IVistaDeviceDriver* pDriver,
    const VistaPropertyList& oDriverSection, const VistaPropertyList& oConfig) {
  std::string sSectionName;
  if (oDriverSection.GetValue("PARAMETERS", sSectionName) == false)
    return false;
  if (oConfig.HasSubList(sSectionName) == false)
    return false;
  const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sSectionName);

  VistaDriverGenericParameterAspect* pParamAspect =
      dynamic_cast<VistaDriverGenericParameterAspect*>(
          pDriver->GetAspectById(VistaDriverGenericParameterAspect::GetAspectId()));

  if (pParamAspect == NULL)
    return false;
#ifdef DEBUG
  for (VistaPropertyList::const_iterator itProp = oSection.begin(); itProp != oSection.end();
       ++itProp) {
    if (pParamAspect->GetParameterContainer()->SetProperty((*itProp).second) == false) {
      vstr::warnp() << "[DriverParameterConfigurator]: Could not set parameter [" << (*itProp).first
                    << "] - not known by driver" << std::endl;
    }
  }
#else
  pParamAspect->GetParameterContainer()->SetPropertiesByList(oSection);
#endif

  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
