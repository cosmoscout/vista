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

#include "VistaDriverManager.h"
#include "VistaConnectionUpdater.h"
#include "VistaDeviceDriver.h"
#include <VistaTools/VistaEnvironment.h>
#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaFileSystemFile.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>

#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
/**
 * @brief the update callback that is called when a connection signals new data
 */
class DriverUpdateCallback : public IVistaExplicitCallbackInterface {
 public:
  DriverUpdateCallback(IVistaDeviceDriver* pDriver)
      : m_pDriver(pDriver)
      , m_nMsecsSleep(0) {
    IVistaDriverCreationMethod* mt    = pDriver->GetFactory();
    unsigned int                nMax  = mt->GetUpdateEstimatorFor(0);
    double                      dRate = (1.0 / (2.0 * double(nMax))) *
                   1000.0; // create a loop-time to match a half of the minimum latency
                           // (which is derived here from the maximum update rate)
                           // the wait time calculated here is in ms
    if (dRate >= 1.0)      // more than 1msec?
      m_nMsecsSleep = (unsigned int)std::ceil(dRate); // create upper bound wait time as a guess
  }

  virtual ~DriverUpdateCallback() {
  }

  virtual bool PrepareCallback() {
    if (m_nMsecsSleep)
      VistaTimeUtils::Sleep(m_nMsecsSleep); // yes! waste some time here...
    return m_pDriver->PreUpdate();
  }

  virtual bool Do() {
    return m_pDriver->Update();
  }

  virtual bool PostCallback() {
    return m_pDriver->PostUpdate();
  }

  IVistaDeviceDriver* m_pDriver;
  int                 m_nMsecsSleep;
};

class _connectDriver
    : public std::unary_function<std::pair<std::string, IVistaDeviceDriver*>, void> {
 public:
  void operator()(std::pair<std::string, IVistaDeviceDriver*> driver) const {
    if (driver.second->Connect() == false)
      vstr::errp() << "Could not connect driver [" << driver.first << "]"
                   << "-- this device will not work." << std::endl;
  }
};

class _disconnectDriver
    : public std::unary_function<std::pair<std::string, IVistaDeviceDriver*>, void> {
 public:
  void operator()(std::pair<std::string, IVistaDeviceDriver*> driver) const {
    if (driver.second->Disconnect() == false) {
      vstr::errp() << "Could not disconnect driver [" << driver.first << "] -- "
                   << "this device will not work." << std::endl;
    }
  }
};
} // namespace

VistaDriverManager::VistaDriverManager(VistaDriverMap& dmap)
    : m_pConnUpdater(new VistaConnectionUpdater)
    , m_Drivers(dmap) {
  VddUtil::InitVdd();
}

VistaDriverManager::~VistaDriverManager() {
  if (m_pConnUpdater && m_pConnUpdater->GetIsDispatching())
    m_pConnUpdater->ShutdownUpdaterLoop(true);

  DisconnectAllDrivers();

  // this will delete all the *drivers* in the driver map
  // (the driver map will only delete all the creation methods for us)
  for (VistaDriverMap::iterator it = m_Drivers.begin(); it != m_Drivers.end(); ++it) {
    (*it).second->SetIsEnabled(false);

    VistaDriverConnectionAspect* pConAsp = dynamic_cast<VistaDriverConnectionAspect*>(
        (*it).second->GetAspectById(VistaDriverConnectionAspect::GetAspectId()));
    if (pConAsp) {
      IVistaExplicitCallbackInterface* pI = m_pConnUpdater->RemConnectionUpdate(
          pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex()));
      delete pI;
    }
  }

  // dispose all the plugins now
  DisposePlugins();

  // m_Drivers.Purge(true);

  delete m_pConnUpdater;

  VddUtil::ExitVdd();
}

IVistaDeviceDriver* VistaDriverManager::CreateAndRegisterDriver(
    const std::string& strDriverName, const std::string& strDriverClassName) {
  IVistaDriverCreationMethod* cr = m_Drivers.GetDriverCreationMethod(strDriverClassName);
  if (cr) {
    IVistaDeviceDriver* drv = (*cr).CreateDriver(); // creator function
    if (drv) {
      m_Drivers.AddDeviceDriver(strDriverName, drv);
      return drv;
    } else {
      vstr::errp() << "VistaDriverManager::CreateAndRegisterDriver() -- "
                   << "could not load driver [" << strDriverName << "] using factory ["
                   << strDriverClassName << "]" << std::endl;
    }
  }
  return NULL;
}

void VistaDriverManager::UnregisterDriver(IVistaDeviceDriver* pDr) {
  m_Drivers.RemDeviceDriver(pDr);
}

void VistaDriverManager::RegisterDriver(const std::string& strDriverName, IVistaDeviceDriver* drv) {
  m_Drivers.AddDeviceDriver(strDriverName, drv);
}

bool VistaDriverManager::InitPlugin(
    const std::string& driver_name, const std::string& search_dir_root) {
  std::string              strPlugPath = search_dir_root;
  VistaFileSystemDirectory dir(search_dir_root);
  if (strPlugPath.empty() || dir.Exists() == false) {
    strPlugPath = VddUtil::GetDefaultPluginPath();
  }

  vstr::outi() << "VistaDriverManager::InitPlugin( " << driver_name << ") -- "
               << "plug path resolved to: " << strPlugPath << std::endl;

  PlugDesc dsc;

  // find default transcoder for this time...

  std::string plugin =
      VddUtil::GetPlugPrefix() + driver_name + VddUtil::GetPlugInfix() + VddUtil::GetPlugPostfix();

  // try to load transcoder for this device
  dsc.m_TranscoderSourceName = strPlugPath + VistaFileSystemDirectory::GetOSSpecificSeparator() +
                               VddUtil::GetTranscoderLibName(driver_name);

  dsc.m_DriverCreationSourceName =
      strPlugPath + VistaFileSystemDirectory::GetOSSpecificSeparator() + plugin;

  if (LoadPlugin(dsc)) {
    m_vecDriverPlugins.push_back(dsc.m_plg);
    m_Drivers.RegisterDriverCreationMethod(dsc.m_plg.m_strDriverClassName, dsc.m_plg.m_pMethod);
    return true;
  }

  return false;
}

int VistaDriverManager::InitAllAvailablePlugins(const std::string& search_dir_root) {
  std::string strPlugPath = search_dir_root;
  if (strPlugPath.empty()) {
    strPlugPath = VddUtil::GetDefaultPluginPath();
  }

  vstr::outi() << "VistaDriverManager::InitPlugins() -- "
               << "plug path resolved to: " << strPlugPath << std::endl;

  VistaFileSystemDirectory plugs(strPlugPath);
  plugs.SetPattern("*" + VddUtil::GetPlugInfix() + VddUtil::GetPlugPostfix());

  vstr::outi() << "found [" << plugs.GetNumberOfEntries() << "] plugins." << std::endl;
  for (VistaFileSystemDirectory::iterator it = plugs.begin(); it != plugs.end(); ++it) {
    vstr::outi() << "[" << (*it)->GetName() << "]" << std::endl;

    PlugDesc dsc;

    // find default transcoder for this time...
    std::string::size_type slash =
        (*it)->GetName().find_last_of(VistaFileSystemDirectory::GetOSSpecificSeparator());
    if (slash == std::string::npos)
      slash = 0;

    std::string::size_type nameP = slash + VddUtil::GetPlugPrefix().size() + 1;

    std::string::size_type plugin     = (*it)->GetName().rfind(VddUtil::GetPlugInfix());
    std::string            driverName = (*it)->GetName().substr(nameP, plugin - nameP);

    // try to load transcoder for this device
    dsc.m_TranscoderSourceName = strPlugPath + VistaFileSystemDirectory::GetOSSpecificSeparator() +
                                 VddUtil::GetTranscoderLibName(driverName);

    dsc.m_DriverCreationSourceName = (*it)->GetName();

    if (LoadPlugin(dsc)) {
      m_vecDriverPlugins.push_back(dsc.m_plg);
      m_Drivers.RegisterDriverCreationMethod(dsc.m_plg.m_strDriverClassName, dsc.m_plg.m_pMethod);
    }
  }
  return (int)m_vecDriverPlugins.size();
}

bool VistaDriverManager::LoadPlugin(PlugDesc& dsc) const {
  if (!VddUtil::LoadTranscoderFromPlugin(dsc.m_TranscoderSourceName, &dsc.m_plg)) {
    vstr::errp() << "Could not load transcoder by name: [" << dsc.m_TranscoderSourceName << "]"
                 << std::endl;
    return false;
  }

  if (VddUtil::LoadCreationMethodFromPlugin(dsc.m_DriverCreationSourceName, &dsc.m_plg)) {
#if defined(DEBUG)
    vstr::outi() << "Loaded factory from [" << dsc.m_DriverCreationSourceName << "] @ "
                 << dsc.m_plg.m_pMethod << " with name [" << dsc.m_plg.m_strDriverClassName << "]"
                 << std::endl;
#endif
    return true;
  } else {
    vstr::errp() << "Failed to load factory from [" << dsc.m_DriverCreationSourceName << "]"
                 << std::endl;
    VddUtil::DisposeTranscoderFromPlugin(&dsc.m_plg);
    return false;
  }
}

bool VistaDriverManager::UnloadPlugin(PlugDesc& dsc) const {
  // FIXME
  return false;
}

void VistaDriverManager::RegisterDriverPlugin(const VddUtil::VistaDriverPlugin& plug) {
  m_vecDriverPlugins.push_back(plug);
}

int VistaDriverManager::DisposePlugins() {
  int n = 0;
  // clean up
  for (std::vector<VddUtil::VistaDriverPlugin>::iterator vit = m_vecDriverPlugins.begin();
       vit != m_vecDriverPlugins.end(); ++vit) {
    vstr::outi() << "close plugin for [" << (*vit).m_strDriverClassName << "]: ";

    // do not kill the memory on it, as this will be done by
    // the call to DisposePlugin below (on some systems or setups,
    // the call to delete might need to call a specific delete
    // operator. So this is more safe to do.
    m_Drivers.UnregisterDriverCreationMethod((*vit).m_strDriverClassName, false);

    if (VddUtil::DisposePlugin(&(*vit), true)) {
      vstr::out() << " -- SUCCESS" << std::endl;
      ++n;
    } else {
      vstr::out().flush();
      vstr::err() << " -- ERROR" << std::endl;
    }

    VddUtil::DisposeTranscoderFromPlugin(&(*vit));
  }

  m_vecDriverPlugins.clear();

  return n;
}

IVistaDriverCreationMethod* VistaDriverManager::GetCreationMethodForClass(
    const std::string& strDriverClassName) const {
  for (std::vector<VddUtil::VistaDriverPlugin>::const_iterator it = m_vecDriverPlugins.begin();
       it != m_vecDriverPlugins.end(); ++it) {
    if ((*it).m_strDriverClassName == strDriverClassName)
      return (*it).m_pMethod;
  }

  return NULL;
}

VistaDriverMap& VistaDriverManager::GetDriverMap() {
  return m_Drivers;
}

const VistaDriverMap& VistaDriverManager::GetDriverMap() const {
  return m_Drivers;
}

void VistaDriverManager::SetEnableStateOnAllDrivers(bool bState) {
  for (VistaDriverMap::iterator it = m_Drivers.begin(); it != m_Drivers.end(); ++it)
    (*it).second->SetIsEnabled(bState);
}

bool VistaDriverManager::StartAsyncDriverDispatch(eLauchOption launch_option) {
  for (VistaDriverMap::iterator it = m_Drivers.begin(); it != m_Drivers.end(); ++it) {
    IVistaDeviceDriver* pDriver = (*it).second;
    if (!pDriver->GetIsConnected())
      continue;

    VistaDriverConnectionAspect* pConAsp = dynamic_cast<VistaDriverConnectionAspect*>(
        pDriver->GetAspectById(VistaDriverConnectionAspect::GetAspectId()));
    if (pConAsp) {
      VistaConnection* pCon = pConAsp->GetConnection(pConAsp->GetUpdateConnectionIndex());
      if (pCon) {
        DriverUpdateCallback* cb = new DriverUpdateCallback(pDriver);
        if (m_pConnUpdater->AddConnectionUpdate(pCon, cb) == false)
          delete cb;
      } else {
        vstr::errp() << "No connection given for connection based aspect driver [" << (*it).first
                     << "]" << std::endl;
      }
    } else {
      VistaDriverThreadAspect* pThread = dynamic_cast<VistaDriverThreadAspect*>(
          pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
      if (pThread)
        pThread->StartProcessing();
      else {
        if (launch_option == E_LAUNCH_ALL_AS_THREAD) {
          pThread = new VistaDriverThreadAspect((*it).second);
          (*it).second->RegisterAspect(pThread);
          pThread->StartProcessing();
        } else {
          vstr::erri() << "Driver [" << (*it).first
                       << "] does not offer a thread aspect and user asked not to launch one."
                       << std::endl;
        }
      }
    }
  }

  return m_pConnUpdater->StartUpdaterLoop();
}

bool VistaDriverManager::StopAsyncDriverDispatch() {
  StopThreadedDevices();
  m_pConnUpdater->ShutdownUpdaterLoop(true);
  return true;
}

void VistaDriverManager::StopThreadedDevices() {
  for (VistaDriverMap::iterator it = m_Drivers.begin(); it != m_Drivers.end(); ++it) {
    IVistaDeviceDriver* pDriver = (*it).second;
    if (pDriver->GetIsConnected()) {
      VistaDriverThreadAspect* pThread = dynamic_cast<VistaDriverThreadAspect*>(
          pDriver->GetAspectById(VistaDriverThreadAspect::GetAspectId()));
      if (pThread)
        pThread->StopProcessing();
    }
  }
}

bool VistaDriverManager::ConnectAllDrivers() {
  std::for_each(m_Drivers.begin(), m_Drivers.end(), _connectDriver());
  return true;
}

bool VistaDriverManager::DisconnectAllDrivers() {
  std::for_each(m_Drivers.begin(), m_Drivers.end(), _disconnectDriver());
  return true;
}
