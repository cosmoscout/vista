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

#include "VistaSystem.h"

#include <VistaKernel/Cluster/VistaClusterMaster.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/Cluster/VistaClusterSlave.h>
#include <VistaKernel/Cluster/VistaClusterStandalone.h>
#include <VistaKernel/Cluster/VistaNewClusterMaster.h>
#include <VistaKernel/Cluster/VistaNewClusterSlave.h>
#include <VistaKernel/Cluster/VistaReplaySlave.h>
#include <VistaKernel/VistaDriverPropertyConfigurator.h>
#include <VistaKernel/VistaFrameLoop.h>
#include <VistaKernel/VistaKernelMsgPort.h>
#include <VistaKernel/VistaSystemCommands.h>
#include <VistaKernel/VistaSystemConfigurators.h>

#include <VistaKernel/EventManager/VistaCommandEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaKernel/InteractionManager/DfnNodes/VistaKernelDfnNodeCreators.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/InteractionManager/VistaUserPlatform.h>
#include <VistaKernel/InteractionManager/VistaVirtualPlatformAdapter.h>

#include <VistaKernel/Stuff/VistaFrameSeriesCapture.h>
#include <VistaKernel/Stuff/VistaFramerateDisplay.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>
#include <VistaKernel/Stuff/VistaRuntimeLimiter.h>
#include <VistaKernel/Stuff/VistaStreamManagerExt.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaBase/VistaDefaultTimerImp.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimerImp.h>
#include <VistaBase/VistaUtilityMacros.h>
#include <VistaBase/VistaVersion.h>

#include <VistaAspects/VistaConversion.h>
#include <VistaTools/VistaEnvironment.h>
#include <VistaTools/VistaStreams.h>
#include <VistaTools/VistaTopologyGraph.h>

#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaFileSystemFile.h>
#include <VistaTools/VistaIniFileParser.h>
#include <VistaTools/VistaRandomNumberGenerator.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/Drivers/VistaShallowDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>

#include <VistaDataFlowNet/VdfnObjectRegistry.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnUtil.h>

#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaConnectionFileTimed.h>
#include <VistaInterProcComm/Connections/VistaConnectionSerial.h>
#include <VistaInterProcComm/IPNet/VistaIPComm.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <list>
#include <set>
#include <string>

#ifndef WIN32
#include <unistd.h>
#endif

/**
 * @todo remove from VistaSystem and put into SystemCommands, add API to the bridges!
 */
#if defined(VISTA_SYS_OPENSG)
#include <VistaKernel/OpenSG/VistaOpenSGSystemClassFactory.h>
#else
#error VistaKernel requires a scenegraph to be defined as compiler flag (current options: VISTA_SYS_OPENSG)
#endif // SystemClassFactory for OpenSG

#if defined(LINUX) || defined(WIN32)
#define DLV_IX86
#include <VistaInterProcComm/DataLaVista/Common/VistaPentiumBasedRTC.h>
#include <VistaTools/VistaCPUInfo.h>
#elif defined(_USE_HRRTC)
#include <VistaInterProcComm/DataLaVista/Common/VistaHRTimerRTC.h>
#else
#include <VistaInterProcComm/DataLaVista/Common/VistaDummyRTC.h>
#endif

#if !defined(WIN32)
#include <signal.h>
#endif

#if !defined(VISTAKERNELSTATIC)
#include <VistaTools/VistaDLL.h>
#endif

#ifdef VISTA_WITH_ZEROMQ
#include <VistaKernel/Cluster/ZeroMQExt/VistaZeroMQCommon.h>
#endif

struct DllHelper {
#if !defined(VISTAKERNELSTATIC)
  std::list<VddUtil::VistaDriverPlugin> m_liDevices;
#endif
};

struct DriverInfo {
  DriverInfo()
      : m_nPrio(-1)
      , m_pDriver(NULL) {
  }

  DriverInfo(IVistaDeviceDriver* pDriver, const std::string& strTypeName,
      const std::string& strName, const std::string& strSection, int nPrio,
      const std::list<std::string>& liDependsOn, int default_history_size = 1)
      : m_strName(strName)
      , m_strSection(strSection)
      , m_strTypeName(strTypeName)
      , m_nPrio(nPrio)
      , m_default_history_size(default_history_size)
      , m_pDriver(pDriver)
      , m_liDependsOn(liDependsOn) {
  }

  std::string            m_strName, m_strSection, m_strTypeName;
  int                    m_nPrio;
  int                    m_default_history_size;
  IVistaDeviceDriver*    m_pDriver;
  std::list<std::string> m_liDependsOn;
};

namespace {
// @todo: move absolute path functions to VistaFileSystemNode
bool isAbsolutePath(const std::string& path) {
  if (path.empty() ||
#ifdef WIN32
      path.size() < 2) // cannot be absolute if that short
#else
      path.size() < 1) // cannot be absolute if that short
#endif
    return false;
#ifdef WIN32
  if (path[1] == ':') // only works with "normal" pathes, no network shares or ntfs mounts!
    return true;
#else
  if (path[0] == '/')
    return true;
#endif
  return false;
}

/// get absolute path, depending on current working directory
std::string getAbsolutePath(const std::string& path) {
  if (isAbsolutePath(path)) {
    return path;
  }
#ifdef WIN32
  // cerr << "[getAbsolutePath] pwd: " << VistaFileSystemDirectory::GetCurrentWorkingDirectory() <<
  // std::endl;
  char  buf[MAX_PATH];
  char* newName = _fullpath(buf, path.c_str(), MAX_PATH);
  if (newName)
    return std::string(newName);
  else {
    assert(false); // _fullpath failed!
    return path;
  }
#else
  char* newName = realpath(path.c_str(), NULL);
  if (newName) {
    std::string ret(newName);
    free(newName);
    return ret;
  } else {
    vstr::errp() << "[ViSystem] \"realpath\" syscall failed for " << path << std::endl;
    vstr::errp() << vstr::singleindent
                 << "temporary workaround: try to build a pseudo-path on our own..." << std::endl;
    char* pwd = getcwd(NULL, 0);
    std::string newPath(pwd);
    free(pwd);
    newPath.append("/");
    newPath.append(path);
    vstr::errp() << vstr::singleindent << "approximated (non-checked) path: " << newPath
                 << std::endl;
    return newPath;
  }
#endif
}

/// get absolute path, depending on the given working directory/file
std::string getAbsolutePathRelativeTo(const std::string& path, std::string absWorkingDirOrFile) {
  if (isAbsolutePath(path) || path.empty())
    return path;

  VistaFileSystemFile f(absWorkingDirOrFile);
  if (f.IsFile()) {
    size_t idx =
        absWorkingDirOrFile.find_last_of(VistaFileSystemDirectory::GetOSSpecificSeparator());
    size_t idx2 = absWorkingDirOrFile.find_last_of('/');
    if (idx2 != std::string::npos) {
      if (idx == std::string::npos || idx2 > idx)
        idx = idx2;
    }
    absWorkingDirOrFile = absWorkingDirOrFile.substr(0, idx);
  }

  // concatenate and try to resolv an absolute path...
  std::string buf = absWorkingDirOrFile;
  buf.append(VistaFileSystemDirectory::GetOSSpecificSeparator());
  buf.append(path);
  std::string str = getAbsolutePath(buf);
  return str;
}

} // namespace

namespace {
#ifdef WIN32
static int  g_nConsoleCloseCount = 0;
BOOL WINAPI ConsoleHandler(DWORD CEvent) {
  switch (CEvent) {
  case CTRL_C_EVENT: {
    vstr::warni() << "VistaSystem Console close event received" << std::endl;
    ::GetVistaSystem()->Quit();

    ++g_nConsoleCloseCount;
    if (g_nConsoleCloseCount == 3) {
      vstr::warni() << "VistaSystem Console close event caught caught 3 times, brute force exit"
                    << std::endl;
      ::GetVistaSystem()->Quit();
      exit(-1);
    }
    break;
  }
  case CTRL_CLOSE_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT: {
    vstr::warni() << "VistaSystem force quit event received - process will be terminated"
                  << std::endl;
    ::GetVistaSystem()->Quit();
    break;
  }
  default:
    return false;
  }
  return true;
}
#else
static int g_nSigIntCount = 0;
void SIGINTHandler(int iMask) {
  vstr::warni() << "VistaSystem::SIGINTHandler(" << iMask << ") -- SIGINT caught" << std::endl;
  ::GetVistaSystem()->Quit();

  ++g_nSigIntCount;

  if (g_nSigIntCount == 3) {
    vstr::warni() << "VistaSystem::SIGINTHandler() -- SIGINT caught 3 times, brute force exit"
                  << std::endl;
    ::GetVistaSystem()->Quit();
    exit(-1);
  }
}

void SIGTERMHandler(int iMask) {
  vstr::warni() << "VistaSystem::SIGTERMHandler(" << iMask << ") -- SIGTERM caught" << std::endl;
  ::GetVistaSystem()->Quit();
  exit(-2);
}

void SIGPIPEHandler(int iMask) {
  vstr::warni() << "VistaSystem::SIGPIPEHandler(" << iMask << ") -- SIGPIPE caught." << std::endl;
  vstr::warni() << "some socket blew it... you better check." << std::endl;
}
#endif
} // namespace

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

VistaSystem* S_pVistaSystem = NULL;
VistaSystem* GetVistaSystem() {
  return S_pVistaSystem;
}
std::string g_SystemSectionName = "SYSTEM";

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaSystem::VistaSystem()
    : m_pDllHlp(new DllHelper)
    , m_pClusterMode(NULL)
    , m_pSystemClassFactory(NULL)
    , m_pDisplayManager(NULL)
    , m_pInteractionManager(NULL)
    , m_pGraphicsManager(NULL)
    , m_pEventManager(new VistaEventManager)
    , m_pKeyboardSystemControl(NULL)
    , m_pRuntimeLimiter(NULL)
    , m_nModelScale(1.0)
    , m_bInitialized(false)
    , m_pFrameLoop(new VistaFrameLoop)
    , m_nClusterNodeType(VistaClusterMode::NT_STANDALONE)
    , m_sClusterNodeName("VistaSystem")
    , m_bUseNewClusterMaster(false)
    , m_pMessagePort(NULL)
    , m_pDriverMap(new VistaDriverMap)
    , m_pConfigurator(new VistaDriverPropertyConfigurator)
    , m_pDfnObjects(new VdfnObjectRegistry)
    , m_bLockSearchPath(false)
    , m_pExternalMsg(NULL)
    , m_pCommandEvent(NULL)
    , m_iInitProgressIndicator(-1)
    , m_pFramerateDisplay(NULL)
    , m_bAllowStreamColors(true)
    , m_bRecordOnlyMaster(false)
    , m_eFrameCaptureMode(-1)
    , m_nFrameCaptureParameter(-1)
    , m_dStartClock(0) {
  VddUtil::InitVdd();

  SetHandlerToken("SYSTEM");
  S_pVistaSystem = this;

  RegisterSigTermCallbacks();

  /**
   * Set up IPC specific stuff. We may want to use network
   * somewhere; UseIPComm can be called several times, so this will
   * not hurt.
   */
  VistaIPComm::UseIPComm();

#ifdef VISTA_WITH_ZEROMQ
  /**
   * ZeroMQ/OpenPGM initialization seems to cause problems in
   * other sockets (esp. listening ones) that are currently open.
   * (e.g. MsgPort starts accepting, but never answers connection
   * request when zmq_init is called after it's Accept() )
   * @todo: try to find the reason for this interaction
   * @todo: move to dll-load?
   */
  VistaZeroMQCommon::RegisterZeroMQUser();
#endif

  /**
   * We are using DLV for cluster, so setting up the timer
   * RTC will be a good thing.
   */
#if defined(DLV_IX86)
  // ok, we are on a ix86-cpu, we can use the RTC clock then
  DLVistaPentiumBasedRTC* pRTC = new DLVistaPentiumBasedRTC;

  // determine clock frequency
  VistaCPUInfo info;
  pRTC->SetClockFrequency(info.GetSpeed());

#elif defined(_USE_HRRTC)
  DLVistaHRTimerRTC* pRTC = new DLVistaHRTimerRTC;
#else
  // no, no rtc, lets use posix/empty calls instead :(
  DLVistaDummyRTC* pRTC = new DLVistaDummyRTC;
#endif

  // New timer interface: we have to create a TimerImp and set is as timer's singleton
  if (IVistaTimerImp::GetSingleton(false) == NULL)
    IVistaTimerImp::SetSingleton(new VistaDefaultTimerImp);

  // setting the oProfiler singleton
  VistaBasicProfiler::SetSingleton(new VistaBasicProfiler);

  std::string sEnvSearchPath = VistaEnvironment::GetEnv("VISTAINIPATH");
  if (sEnvSearchPath.empty()) {
    std::list<std::string> liSearchPaths;
    liSearchPaths.push_back("configfiles/");
    SetIniSearchPaths(liSearchPaths);
  } else {
    std::list<std::string> liSearchPaths;
    VistaConversion::FromString(sEnvSearchPath, liSearchPaths);
    SetIniSearchPaths(liSearchPaths);
    vstr::outi() << "VISTAINIPATH = " << sEnvSearchPath << std::endl;
  }

  RegisterEventTypes();
}

VistaSystem::~VistaSystem() {
  vstr::debugi() << " [ViSys] >> DESTRUCTOR <<" << std::endl;

  // remove user platforms
  for (std::map<VistaDisplaySystem*, VistaUserPlatform*>::iterator pit = m_mapUserPlatforms.begin();
       pit != m_mapUserPlatforms.end(); ++pit) {
    delete (*pit).second;
  }

  // reverse destruction order

  if (m_pSystemClassFactory) {
    std::vector<IVistaSystemClassFactory::Manager> vOrder = m_pSystemClassFactory->GetInitOrder();
    for (std::vector<IVistaSystemClassFactory::Manager>::reverse_iterator rit = vOrder.rbegin();
         rit != vOrder.rend(); ++rit) {
      switch (*rit) {
      case IVistaSystemClassFactory::DISPLAY: {
        delete m_pDisplayManager;
        m_pDisplayManager = nullptr;
        break;
      }
      case IVistaSystemClassFactory::GRAPHICS: {
        delete m_pGraphicsManager;
        m_pGraphicsManager = nullptr;
        break;
      }
      case IVistaSystemClassFactory::INTERACTION: {
        delete m_pInteractionManager;
        m_pInteractionManager = nullptr;
        break;
      }
      default:
        break;
      }
    }
  }

  delete m_pKeyboardSystemControl;
  m_pKeyboardSystemControl = nullptr;

  delete m_pFramerateDisplay;
  m_pFramerateDisplay = nullptr;

  delete m_pFrameLoop;
  m_pFrameLoop = nullptr;

  delete m_pClusterMode;
  m_pClusterMode = nullptr;

  delete m_pRuntimeLimiter;
  m_pRuntimeLimiter = nullptr;

  for (std::vector<VistaFrameSeriesCapture*>::iterator itCapture = m_vecFrameCaptures.begin();
       itCapture != m_vecFrameCaptures.end(); ++itCapture) {
    delete (*itCapture);
  }

  delete m_pEventManager;
  m_pEventManager = nullptr;

  delete m_pCommandEvent;
  m_pCommandEvent = nullptr;

  delete m_pMessagePort;
  m_pMessagePort = nullptr;

  delete m_pExternalMsg;
  m_pExternalMsg = nullptr;

  for (VistaDriverMap::iterator it = m_pDriverMap->begin(); it != m_pDriverMap->end(); ++it) {
    (*it).second->SetIsEnabled(false);
    (*it).second->Disconnect();
    delete (*it).second;
  }

  delete m_pConfigurator;
  m_pConfigurator= nullptr;

#ifdef VISTA_WITH_ZEROMQ
  /**
   * ZeroMQ/OpenPGM initialization seems to cause problems in
   * other sockets (esp. listening ones) that are currently open.
   * (e.g. MsgPort starts accepting, but never answers connection
   * request when zmq_init is called after it's Accept() )
   * @todo: try to find the reason for this interaction
   * @todo: move to dll-load?
   */
  VistaZeroMQCommon::UnregisterZeroMQUser();
#endif

#if !defined(VISTAKERNELSTATIC)
  // removed device drivers from dll loads
  for (std::list<VddUtil::VistaDriverPlugin>::iterator dllIt = m_pDllHlp->m_liDevices.begin();
       dllIt != m_pDllHlp->m_liDevices.end(); ++dllIt) {
    // we should unregister the stuff from the interaction manager first...
    GetDriverMap()->UnregisterDriverCreationMethod(
        (*dllIt).m_strDriverClassName, false); // do not kill the memory on it
    // will be disposed here!
    VddUtil::DisposePlugin(&(*dllIt));
  }
#endif

  delete m_pDllHlp;
  m_pDllHlp = nullptr;

  delete m_pDriverMap;
  m_pDriverMap = nullptr;

  delete m_pDfnObjects;
  m_pDfnObjects = nullptr;

  // delete RTC
  delete IDLVistaRTC::GetRTCSingleton();
  IVistaTimerImp::SetSingleton(nullptr);

  VistaIPComm::CloseIPComm();

  delete m_pSystemClassFactory;
  m_pSystemClassFactory = nullptr;

  S_pVistaSystem = nullptr;

  m_liDriverPluginPathes.push_back(".");

  UnregisterSigTermHandlers();
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaSystem::SetIniFile(const std::string& strNewIni) {
  // std::string absolutePath = FindFileInIniSearchPath(strNewIni);
  // if(absolutePath.empty())
  //	return false;
  // m_sVistaConfigFile = absolutePath;
  m_sVistaConfigFile = strNewIni;
  return true;
}

bool VistaSystem::SetDisplayIniFile(const std::string& strNewIni) {
  // std::string absolutePath = FindFileInIniSearchPath(strNewIni);
  // if(absolutePath.empty())
  //	return false;
  // m_sDisplayConfigFile = absolutePath;
  m_sDisplayConfigFile = strNewIni;
  return true;
}

bool VistaSystem::SetGraphicsIniFile(const std::string& strNewIni) {
  // std::string absolutePath = FindFileInIniSearchPath(strNewIni);
  // if(absolutePath.empty())
  //	return false;
  // m_sGraphicsConfigFile = absolutePath;
  m_sGraphicsConfigFile = strNewIni;
  return true;
}

bool VistaSystem::SetInteractionIniFile(const std::string& strNewIni) {
  // std::string absolutePath = FindFileInIniSearchPath(strNewIni);
  // if(absolutePath.empty())
  //	return false;
  // m_sInteractionConfigFile = absolutePath;
  m_sInteractionConfigFile = strNewIni;
  return true;
}

bool VistaSystem::SetClusterIniFile(const std::string& strNewIni) {
  // std::string absolutePath = FindFileInIniSearchPath(strNewIni);
  // if(absolutePath.empty())
  //	return false;
  // m_sClusterConfigFile = absolutePath;
  m_sClusterConfigFile = strNewIni;
  return true;
}

std::string VistaSystem::GetIniFile() const {
  return m_sVistaConfigFile;
}

std::string VistaSystem::GetDisplayIniFile() const {
  return m_sDisplayConfigFile;
}

std::string VistaSystem::GetGraphicsIniFile() const {
  return m_sGraphicsConfigFile;
}

std::string VistaSystem::GetInteractionIniFile() const {
  return m_sInteractionConfigFile;
}

std::string VistaSystem::GetClusterIniFile() const {
  return m_sClusterConfigFile;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::Init(int argc, char* argv[]) {
  vstr::outi() << "\n#############################################\n";
  vstr::outi() << "######## ViSTA SYSTEM INITIALIZATION ########\n";
  vstr::outi() << "#############################################" << std::endl;

  // we have to have at LEAST the application name
  // (argc==1) and a valid entry in argv[0]
  if (argc >= 1 && argv[0]) {
    m_sApplicationName = argv[0];
    if (!ArgParser(argc, argv))
      return false;
  }

  if (!this->DoInit(argc, argv))
    return false;

  vstr::outi() << "#############################################\n";
  vstr::outi() << "######## FINISHED VISTA INITIALIZATION ######\n";
  vstr::outi() << "#############################################\n" << std::endl;

  return true;
}

bool VistaSystem::DoInit(int argc, char** argv) {
  m_iInitProgressIndicator = 0;

  if (LoadIniFiles() == false) {
    vstr::errp() << "VistaSystem::Init() - "
                 << "loading Ini filess failed - aborting!" << std::endl;
    return false;
  }
  if (m_oVistaConfig.HasProperty(GetSystemSectionName()) == false) {
    vstr::errp() << "VistaSystem::Init() - "
                 << "Config file [" << m_sVistaConfigFile << "] does not contain system section ["
                 << GetSystemSectionName() << "] - aborting" << std::endl;
    return false;
  }

  SetupOutputStreams();

  // Create SystemClassFactory
#if defined(VISTA_SYS_OPENSG)
  bool bUseOpenSGThreads = m_oVistaConfig.GetValueInSubListOrDefault<bool>(
      "USE_OPENSG_THREADS", GetSystemSectionName(), true);
  m_pSystemClassFactory = new VistaOpenSGSystemClassFactory(this, bUseOpenSGThreads);
#else
  VISTA_THROW("NO SYSTEM TAG DURING COMPILE TIME GIVEN?", 0x0000000);
#endif

  if (!m_pSystemClassFactory) {
    vstr::errp() << "Unable to initialize the system - "
                    "no SystemClassFactory available!"
                 << std::endl;
    m_bInitialized = false;
    return false;
  }

  m_pInteractionManager = m_pSystemClassFactory->CreateInteractionManager();

  CreateClusterMode();

  SetupMessagePort();

  if (SetupEventManager() == false)
    return false;

  if (SetupCluster() == false)
    return false;

  std::vector<IVistaSystemClassFactory::Manager> vOrder = m_pSystemClassFactory->GetInitOrder();
  for (unsigned int i = 0; i < vOrder.size(); ++i) {
    switch (vOrder[i]) {
    case IVistaSystemClassFactory::DISPLAY: {
      if (SetupDisplayManager() == false)
        return false;
      break;
    }
    case IVistaSystemClassFactory::GRAPHICS: {
      if (SetupGraphicsManager() == false)
        return false;
      break;
    }
    case IVistaSystemClassFactory::INTERACTION: {
      if (SetupInteractionManager() == false)
        return false;
      break;
    }
    default: {
      vstr::errp() << "VistaSystem::DoInit() -> unknown init routine " << std::endl;
      return false;
    }
    };
  }

  // initialize our main loop
  m_pFrameLoop->Init(this);

  SetRandomSeed((int)m_pClusterMode->GetFrameClock());

  {
    vstr::outi() << "\n-------------------------------------------------------------";
    vstr::outi() << "\n----- [VistaSystem]: Setting up Scene                   -----" << std::endl;
    vstr::IndentObject oIndent;

    std::string sGraphicsSectionName;
    m_oGraphicsConfig.GetValueInSubList(
        "GRAPHICSSECTION", GetSystemSectionName(), sGraphicsSectionName);

    m_pGraphicsManager->SetupScene(sGraphicsSectionName, m_oGraphicsConfig);

    bool bLoadModel = m_oGraphicsConfig.GetValueInSubListOrDefault<bool>(
        "displayactive", GetSystemSectionName(), true);
    if (bLoadModel && m_sModelFile.empty() == false) {
      VistaSceneGraph* pSG     = m_pGraphicsManager->GetSceneGraph();
      VistaGroupNode*  pSGRoot = pSG->GetRoot();
      pSGRoot->AddChild(pSG->LoadNode(m_sModelFile, VistaSceneGraph::OPT_NONE, m_nModelScale));
    }
  }

  m_pClusterMode->PostInit();

  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: Application Init                   -----" << std::endl;

  // call event to signal the initialization process
  vstr::outi() << "\nEmitting Application INIT event" << std::endl;
  vstr::IndentObject oAppInitIndent;

  IndicateSystemProgress("Application Init-phase", false);
  // in either case we will run through the ini loop

  VistaSystemEvent oEvent(VistaSystemEvent::VSE_INIT);
  m_pEventManager->ProcessEvent(&oEvent);

  m_bInitialized = true;

  IndicateSystemProgress("INIT DONE", true);
  return true;
}

void VistaSystem::RegisterSigTermCallbacks() {
#ifdef WIN32
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, true);
#else
  struct sigaction sa;

  sigemptyset(&sa.sa_mask);
  // RESETHAND and NODEFER what the old signal handler did when registering
  // with deprecated signal(). we add SA_RESTART to have all system calls
  // supporting it restart automatically instead of failing with EINTR.
  sa.sa_flags = SA_RESTART | SA_RESETHAND | SA_NODEFER;

  sa.sa_handler = SIGINTHandler;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    // @todo error handling
  }
  sa.sa_handler = SIGTERMHandler;
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    // @todo error handling
  }
  sa.sa_handler = SIGPIPEHandler;
  if (sigaction(SIGPIPE, &sa, NULL) == -1) {
    // @todo error handling
  }
#endif
}

void VistaSystem::UnregisterSigTermHandlers() {
  // unregister handlers that were registered in constructor
#ifdef WIN32
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)NULL, true);
#else
  ::signal(SIGINT, NULL);
  ::signal(SIGTERM, NULL);
  ::signal(SIGPIPE, NULL);
#endif
}

void VistaSystem::RegisterEventTypes() {
  // setup event manager, we really need this...
  m_pEventManager->Init();

  // register new event type
  VistaEventManager::EVENTTYPE eEventType = m_pEventManager->RegisterEventType("VET_SYSTEM");
  VistaSystemEvent::SetTypeId(eEventType);

  // we should register all ids now
  for (int n = VistaSystemEvent::VSE_INIT; n < VistaSystemEvent::VSE_UPPER_BOUND; ++n) {
    m_pEventManager->RegisterEventId(eEventType, VistaSystemEvent::GetIdString(n));
  }

  VistaInteractionManager::RegisterEventTypes(m_pEventManager);

  // register and map command events
  eEventType = m_pEventManager->RegisterEventType("VET_COMMAND");
  VistaCommandEvent::SetTypeId(eEventType);
  for (int n = VistaCommandEvent::VEIDC_CMD; n < VistaCommandEvent::VEIDC_LAST; ++n) {
    m_pEventManager->RegisterEventId(eEventType, VistaCommandEvent::GetIdString(n));
  }

  eEventType = m_pEventManager->RegisterEventType("VET_EXTERNAL_MESSAGE");
  VistaExternalMsgEvent::SetTypeId(eEventType);
  for (int n = VistaExternalMsgEvent::VEID_INCOMING_MSG; n < VistaExternalMsgEvent::VEID_LAST;
       ++n) {
    m_pEventManager->RegisterEventId(eEventType, VistaExternalMsgEvent::GetIdString(n));
  }

  m_pCommandEvent = new VistaCommandEvent;
  m_pExternalMsg  = new VistaExternalMsgEvent;
}

bool VistaSystem::SetupEventManager() {
  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: EventManager Setup                 -----" << std::endl;
  IndicateSystemProgress("Setup EventManager", false);
  vstr::IndentObject oIndent;

  if (m_pEventManager == NULL) {
    vstr::errp() << "No EventManager generated - program exit!" << std::endl;
    m_bInitialized = false;
    return false;
  }

  // register self (i.e. VistaSystem) as system event handler
  m_pEventManager->AddEventHandler(this, VistaSystemEvent::GetTypeId(), VistaEventManager::NVET_ALL,
      VistaEventManager::PRIO_FALLBACK);

  // we want to catch interaction events in order to keep clean the msg port
  // communication!
  m_pEventManager->SetPriority(this, VistaSystemEvent::GetTypeId(),
      VistaSystemEvent::VSE_UPDATE_INTERACTION, 2 * VistaEventManager::PRIO_HIGH);

  m_pEventManager->AddEventHandler(this, VistaExternalMsgEvent::GetTypeId(),
      VistaEventManager::NVET_ALL, VistaEventManager::PRIO_FALLBACK);

  return true;
}

void VistaSystem::CreateClusterMode() {
  if (m_pClusterMode != NULL) {
    vstr::outi() << "[CreateClusterMode]: Encountered user-provided ClusterMode" << std::endl;
    return;
  }

  if (m_sRecordFile.empty() == false) {
    if (m_nClusterNodeType == VistaClusterMode::NT_STANDALONE) {
      vstr::outi() << "[CreateClusterMode]: explicit replay file was set, changing from Standalone"
                      " to NewClusterMaster"
                   << std::endl;
      m_nClusterNodeType     = VistaClusterMaster::NT_MASTER;
      m_bUseNewClusterMaster = true;
      m_bRecordOnlyMaster    = true;
      m_sClusterNodeName     = "SYSTEM";
    } else if (m_nClusterNodeType != VistaClusterMaster::NT_MASTER ||
               m_bUseNewClusterMaster == false) {
      vstr::warnp() << "[CreateClusterMode]: explicit replay file was set, but config is neither"
                       "  Standalone nor NewClusterMaster - recording ignored"
                    << std::endl;
    }
  }

  switch (m_nClusterNodeType) {
  case VistaClusterMode::NT_STANDALONE: {
    m_pClusterMode = new VistaClusterStandalone;
    vstr::outi() << "Creating ClusterMode as [STANDALONE]" << std::endl;
    break;
  }
  case VistaClusterMode::NT_MASTER: {
    if (m_bUseNewClusterMaster) {
      m_pClusterMode = new VistaNewClusterMaster(this, m_sClusterNodeName);
      if (m_sRecordFile.empty() == false)
        static_cast<VistaNewClusterMaster*>(m_pClusterMode)->SetRecordDataFolder(m_sRecordFile);
      vstr::outi() << "Creating ClusterMode as [NEWMASTER]" << std::endl;
    } else {
      m_pClusterMode = new VistaClusterMaster(m_pEventManager, m_sClusterNodeName);
      vstr::outi() << "Creating ClusterMode as [MASTER]" << std::endl;
    }

    break;
  }
  case VistaClusterMode::NT_SLAVE: {
    if (m_bUseNewClusterMaster) {
      m_pClusterMode = new VistaNewClusterSlave(this, m_sClusterNodeName);
      vstr::outi() << "Creating ClusterMode as [NEWSLAVE]" << std::endl;
    } else {
      m_pClusterMode =
          new VistaClusterSlave(m_pEventManager, m_sClusterNodeName, m_pInteractionManager);
      vstr::outi() << "Creating ClusterMode as [SLAVE]" << std::endl;
    }
    break;
  }
  case VistaClusterMode::NT_REPLAY_SLAVE: {
    {
      m_pClusterMode = new VistaReplaySlave(this, m_sClusterNodeName);
      vstr::outi() << "Creating ClusterMode as [REPLAY_SLAVE]" << std::endl;
    }
    break;
  }
  default: {
    vstr::errp() << "Unknown cluster Mode [" << m_nClusterNodeType << "]" << std::endl;
    VISTA_THROW("UNKNOWN_CLUSTER_MODE", 0x00000066);
  }
  }
}

bool VistaSystem::SetupCluster() {
  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: Cluster Setup                      -----" << std::endl;
  IndicateSystemProgress("Setup Cluster", false);
  vstr::IndentObject oIndent;

  if (m_bRecordOnlyMaster) {
    bool bResult  = Vista::assert_cast<VistaNewClusterMaster*>(m_pClusterMode)->Init(m_sRecordFile);
    m_dStartClock = GetFrameClock();
    return bResult;
  } else {
    bool bResult  = m_pClusterMode->Init(m_sClusterNodeName, m_oClusterConfig);
    m_dStartClock = GetFrameClock();
    return bResult;
  }
}

bool VistaSystem::LoadIniFiles() {
  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: Loading INI files                  -----" << std::endl;

  VistaIniFileParser oParser(true);

  if (m_sVistaConfigFile.empty())
    m_sVistaConfigFile = "vista.ini";
  if (oParser.ReadFile(m_sVistaConfigFile, m_liSearchPathes) == false) {
    vstr::errp() << "Vista-ini [" << m_sVistaConfigFile << "] not found!" << std::endl;
    return false;
  }
  m_oVistaConfig     = oParser.GetPropertyList();
  m_sVistaConfigFile = oParser.GetFilename();

  if (m_oVistaConfig.HasSubList(GetSystemSectionName()) == false) {
    vstr::errp() << "Vista-ini [" << m_sVistaConfigFile << "] invalid - no system section ["
                 << GetSystemSectionName() << "]!" << std::endl;
    return false;
  }

  const VistaPropertyList& oSystemSection =
      m_oVistaConfig.GetSubListConstRef(GetSystemSectionName());

  // If not yet specified - check if specific files are given in cista.ini
  if (m_sInteractionConfigFile.empty()) {
    oSystemSection.GetValue("INTERACTIONINI", m_sInteractionConfigFile);
  }
  if (m_sDisplayConfigFile.empty()) {
    oSystemSection.GetValue("DISPLAYINI", m_sDisplayConfigFile);
  }
  if (m_sGraphicsConfigFile.empty()) {
    oSystemSection.GetValue("GRAPHICSINI", m_sGraphicsConfigFile);
  }
  if (m_sClusterConfigFile.empty()) {
    oSystemSection.GetValue("CLUSTERINI", m_sClusterConfigFile);
  }

  if (m_sDisplayConfigFile.empty()) {
    m_sDisplayConfigFile = m_sVistaConfigFile;
    m_oDisplayConfig     = m_oVistaConfig;
  } else {
    if (oParser.ReadFile(m_sDisplayConfigFile, m_liSearchPathes) == false) {
      vstr::errp() << "Could not find Display Config File [" << m_sDisplayConfigFile
                   << "] not found!" << std::endl;
      return false;
    }
    m_oDisplayConfig     = oParser.GetPropertyList();
    m_sDisplayConfigFile = oParser.GetFilename();
  }

  if (m_sGraphicsConfigFile.empty()) {
    m_sGraphicsConfigFile = m_sVistaConfigFile;
    m_oGraphicsConfig     = m_oVistaConfig;
  } else {
    if (oParser.ReadFile(m_sGraphicsConfigFile, m_liSearchPathes) == false) {
      vstr::errp() << "Could not find Graphics Config File [" << m_sGraphicsConfigFile
                   << "] not found!" << std::endl;
      return false;
    }
    m_oGraphicsConfig     = oParser.GetPropertyList();
    m_sGraphicsConfigFile = oParser.GetFilename();
  }

  if (m_sInteractionConfigFile.empty()) {
    m_sInteractionConfigFile = m_sVistaConfigFile;
    m_oInteractionConfig     = m_oVistaConfig;
  } else {
    if (oParser.ReadFile(m_sInteractionConfigFile, m_liSearchPathes) == false) {
      vstr::errp() << "Could not find Interaction Config File [" << m_sInteractionConfigFile
                   << "] not found!" << std::endl;
      return false;
    }
    m_oInteractionConfig     = oParser.GetPropertyList();
    m_sInteractionConfigFile = oParser.GetFilename();
  }

  if (m_sClusterConfigFile.empty()) {
    m_sClusterConfigFile = m_sVistaConfigFile;
    m_oClusterConfig     = m_oVistaConfig;
  } else {
    if (oParser.ReadFile(m_sClusterConfigFile, m_liSearchPathes) == false) {
      vstr::errp() << "Could not find Cluster Config File [" << m_sClusterConfigFile
                   << "] not found!" << std::endl;
      return false;
    }
    m_oClusterConfig     = oParser.GetPropertyList();
    m_sClusterConfigFile = oParser.GetFilename();
  }

  vstr::outi() << "Loaded INI files:\n";
  vstr::IndentObject oIndent;
  vstr::outi() << "System-ini     : " + m_sVistaConfigFile + "\n";
  vstr::outi() << "Display-ini    : " + m_sDisplayConfigFile + "\n";
  vstr::outi() << "Graphics-ini   : " + m_sGraphicsConfigFile + "\n";
  vstr::outi() << "Interaction-ini: " + m_sInteractionConfigFile + "\n";
  vstr::outi() << "Cluster-ini    : " + m_sClusterConfigFile << std::endl;

  return true;
}

const std::list<std::string>& VistaSystem::GetIniSearchPaths() const {
  return m_liSearchPathes;
}

bool VistaSystem::SetIniSearchPaths(const std::list<std::string>& liSearchPaths) {
  if (m_bLockSearchPath) // user gave -inisearchpaths option
    return false;

  m_liSearchPathes.clear();
  for (std::list<std::string>::const_iterator it = liSearchPaths.begin(); it != liSearchPaths.end();
       ++it) {
    VistaFileSystemDirectory oDir(*it);
    if (oDir.Exists()) {
      if (isAbsolutePath(*it)) {
        m_liSearchPathes.push_back(*it);
      } else {
        std::string absPath = getAbsolutePath(*it);
        // assure that paths all do end with the separator to ease searching for files later
        if (absPath[absPath.size() - 1] !=
            VistaFileSystemDirectory::GetOSSpecificSeparator().c_str()[0]) {
          absPath.append(VistaFileSystemDirectory::GetOSSpecificSeparator());
        }
        m_liSearchPathes.push_back(absPath);
      }
    }
  }
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetupDisplayManager                                         */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetupDisplayManager() {
  // create a new display manager object
  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: DisplayManager Setup               -----" << std::endl;
  IndicateSystemProgress("Setup DisplayManager", false);
  vstr::IndentObject oIndent;

  m_pDisplayManager = m_pSystemClassFactory->CreateDisplayManager();

  if (m_pDisplayManager == NULL)
    return false;

  std::string sDisplaySystemSectionName = m_pClusterMode->GetConfigSectionName();

  // setup display systems
  if (m_pDisplayManager->CreateDisplaySystems(sDisplaySystemSectionName, m_oDisplayConfig) ==
      false) {
    vstr::errp() << "Unable to create display systems!" << std::endl;
    return false;
  }

  std::string sGraphicsSectionName;
  m_oGraphicsConfig.GetValueInSubList(
      "GRAPHICSSECTION", GetSystemSectionName(), sGraphicsSectionName);

  bool bShowCursor = m_oGraphicsConfig.GetValueInSubListOrDefault<bool>(
      "ShowCursor", sGraphicsSectionName, (m_pClusterMode->GetIsLeader()) ? (true) : (false));

  m_pDisplayManager->GetDisplayBridge()->SetShowCursor(bShowCursor);

  if (m_eFrameCaptureMode != -1) {
    vstr::outi() << "[VistaSystem]: Setting up FrameSeriesCapture for all windows" << std::endl;
    const std::map<std::string, VistaWindow*>& mapWindows =
        GetDisplayManager()->GetWindowsConstRef();
    for (std::map<std::string, VistaWindow*>::const_iterator itWin = mapWindows.begin();
         itWin != mapWindows.end(); ++itWin) {
      std::string sFolder = "screenshots/series_%D%_%T%_" + (*itWin).first;
      if (GetClusterMode()->GetNumberOfNodes() > 1)
        sFolder += "_" + GetClusterMode()->GetNodeName();
      std::string sFile = "screenshot_%S%_%T%.%M%.png";
      if (m_sCaptureFramesFilePattern.empty() == false) {
#ifdef WIN32
        std::replace(
            m_sCaptureFramesFilePattern.begin(), m_sCaptureFramesFilePattern.end(), '\\', '/');
#endif
        std::size_t nSlashPos = m_sCaptureFramesFilePattern.rfind('/');
        if (nSlashPos != std::string::npos) {
          sFolder = m_sCaptureFramesFilePattern.substr(0, nSlashPos);
          sFile   = m_sCaptureFramesFilePattern.substr(nSlashPos);
        } else {
          sFile = m_sCaptureFramesFilePattern;
        }
      }
      VistaFrameSeriesCapture* pCapture = new VistaFrameSeriesCapture(this, (*itWin).second, true);
      switch (m_eFrameCaptureMode) {
      case VistaFrameSeriesCapture::CM_EVERY_FRAME: {
        pCapture->InitCaptureEveryFrame(sFolder, sFile);
        break;
      }
      case VistaFrameSeriesCapture::CM_EVERY_NTH_FRAME: {
        pCapture->InitCaptureEveryNthFrame(sFolder, sFile, (int)m_nFrameCaptureParameter);
        break;
      }
      case VistaFrameSeriesCapture::CM_PERIODICALLY: {
        pCapture->InitCapturePeriodically(sFolder, sFile, m_nFrameCaptureParameter);
        break;
      }
      case VistaFrameSeriesCapture::CM_FIXED_FRAMERATE: {
        pCapture->InitCaptureWithFramerate(sFolder, sFile, m_nFrameCaptureParameter);
        break;
      }
      default:
        delete pCapture;
        break;
      }
      m_vecFrameCaptures.push_back(pCapture);
    }
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetupGraphicsManager                                        */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::SetupGraphicsManager() {
  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: GraphicsManager Setup              -----" << std::endl;
  IndicateSystemProgress("Setup GraphicsManager", false);
  vstr::IndentObject oIndent;

  m_pGraphicsManager = m_pSystemClassFactory->CreateGraphicsManager();
  if (m_pGraphicsManager == NULL) {
    vstr::errp() << "[VistaSystem] Could not create GraphicsManager!" << std::endl;
    m_bInitialized = false;
    return false;
  }

  IVistaGraphicsBridge* pGraphicsBridge = m_pSystemClassFactory->CreateGraphicsBridge();
  IVistaNodeBridge*     pNodeBridge     = m_pSystemClassFactory->CreateNodeBridge();
  if (pGraphicsBridge && pNodeBridge) {
    m_pGraphicsManager->Init(pNodeBridge, pGraphicsBridge);
  } else {
    delete pGraphicsBridge;
    delete pNodeBridge;
    m_bInitialized = false;
    return false;
  }

  return true;
}

bool VistaSystem::SetupBasicInteraction() {
  CreateFramerateDisplay();

  BindKeyboardActions();

  RegisterConfigurators();

  {
    vstr::outi() << "Registering DataFlowNet Ports and Nodes" << std::endl;
    vstr::IndentObject oDfnIndent;

    // SETUP DFN
    // THIS IS UTTERLY IMPORTANT TO CALL! InitVdfn().
    VdfnUtil::InitVdfn(GetDriverMap(), GetDfnObjectRegistry());

    // register kernel-specific nodes
    VistaKernelDfnNodeCreators::RegisterNodeCreates(this);
    // END SETUP DFN
  }

  CreateDeviceDrivers();

  // SETUP INTERACTIONCONTEXTS
  {
    vstr::outi() << "Loading InteractionContexts" << std::endl;
    vstr::IndentObject     oContextsIndent;
    std::list<std::string> liInteractionContexts;
    m_oInteractionConfig.GetValueInSubList(
        "INTERACTIONCONTEXTS", GetSystemSectionName(), liInteractionContexts);
    vstr::outi() << "Found " << liInteractionContexts.size() << " entries in INTERACTIONCONTEXTS"
                 << std::endl;

    for (std::list<std::string>::const_iterator itContext = liInteractionContexts.begin();
         itContext != liInteractionContexts.end(); ++itContext) {
      VistaInteractionContext* pCtx = SetupInteractionContext((*itContext));
      if (pCtx) {
        m_liCreateCtxs.push_back(pCtx);
      }
    }
  }

  // setup projection updates
  std::string sDisplaySystemSectionName = m_pClusterMode->GetConfigSectionName();

  const std::map<std::string, VistaDisplaySystem*> m_mapDisplays =
      m_pDisplayManager->GetDisplaySystemsConstRef();

  for (std::map<std::string, VistaDisplaySystem*>::const_iterator itDisplay = m_mapDisplays.begin();
       itDisplay != m_mapDisplays.end(); ++itDisplay) {
    VistaDisplaySystem* pDisplaySystem = (*itDisplay).second;
    VistaUserPlatform*  pPlatform =
        new VistaUserPlatform(GetGraphicsManager()->GetSceneGraph(), pDisplaySystem);
    m_mapUserPlatforms[pDisplaySystem] =
        pPlatform; // add to memory (for later disposal during deconstruction)

    // register this with the object registry
    GetDfnObjectRegistry()->SetObject(std::string("CAM:") + pDisplaySystem->GetNameForNameable(),
        pDisplaySystem->GetProperties(), pPlatform->GetPlatformTrans());
  }

  return true;
}

void VistaSystem::BindKeyboardActions() {
  // setup basic key-strok support.

  m_pKeyboardSystemControl->BindAction(
      'q', new VistaQuitCommand(this), "Quits the ViSTA updateloop and exits the application");
  m_pKeyboardSystemControl->BindAction(
      '?', new VistaShowAvailableCommands(m_pKeyboardSystemControl, this), "Show avaliable keys");
  m_pKeyboardSystemControl->BindAction(
      'F', new VistaToggleFramerateCommand(m_pFramerateDisplay), "Toggle framerate info");
  m_pKeyboardSystemControl->BindAction('E',
      new TVistaDebugToConsoleCommand<VistaEventManager>(GetEventManager()),
      "Debug printout for EventManager");
  m_pKeyboardSystemControl->BindAction('G',
      new TVistaDebugToConsoleCommand<VistaGraphicsManager>(GetGraphicsManager()),
      "Debug printout for GraphicsManager");
  m_pKeyboardSystemControl->BindAction('D',
      new TVistaDebugToConsoleCommand<VistaDisplayManager>(GetDisplayManager()),
      "Debug printout for DisplayManager");
  m_pKeyboardSystemControl->BindAction(
      'C', new VistaToggleCursorCommand(GetDisplayManager()), "Enable / Disable Cursor");
  m_pKeyboardSystemControl->BindAction(
      'V', new VistaToggleVSyncCommand(GetDisplayManager()), "Enable / Disable VSync");
  m_pKeyboardSystemControl->BindAction(
      'I', new VistaPrintProfilerOutputCommand, "Print Profiling Information");
  m_pKeyboardSystemControl->BindAction('o', VISTA_KEYMOD_ALT,
      new VistaPrintProfilerOutputForAllNodesCommand(GetClusterMode()),
      "Print Profiling Info for all cluster nodes");
  m_pKeyboardSystemControl->BindAction(
      'i', VISTA_KEYMOD_ALT, new VistaResetProfilerCommand, "Reset Profiling Information");
  m_pKeyboardSystemControl->BindAction(VISTA_KEY_DOWNARROW, VISTA_KEYMOD_CTRL,
      new VistaChangeEyeDistanceCommand(-0.001f, GetDisplayManager()),
      "Decrease x eye offset by 0.001");
  m_pKeyboardSystemControl->BindAction(VISTA_KEY_UPARROW, VISTA_KEYMOD_CTRL,
      new VistaChangeEyeDistanceCommand(0.001f, GetDisplayManager()),
      "Increase x eye offset by 0.001");
  m_pKeyboardSystemControl->BindAction(VISTA_KEY_F5, VISTA_KEYMOD_CTRL,
      new VistaToggleEyeTesterCommand(this), "Enable / Disable EyeTester");
  m_pKeyboardSystemControl->BindAction(VISTA_KEY_ENTER, VISTA_KEYMOD_ALT,
      new VistaToggleFullscreenCommand(GetDisplayManager()), "Toggle Fullscreen Windows");
  m_pKeyboardSystemControl->BindAction('*',
      new TVistaDebugToConsoleCommand<VistaClusterMode>(m_pClusterMode),
      "Show ClusterMode debug output");
  m_pKeyboardSystemControl->BindAction('$',
      new VistaToggleFrustumCullingCommand(GetGraphicsManager()),
      "Enable / Disable frustum culling (OpenSG specific)");
  m_pKeyboardSystemControl->BindAction('%',
      new VistaToggleOcclusionCullingCommand(GetGraphicsManager()),
      "Enable / Disable occlusion culling (OpenSG specific)");
  m_pKeyboardSystemControl->BindAction('&', new VistaToggleBBoxDrawingCommand(GetGraphicsManager()),
      "Enable / Disable BBox drawing (OpenSG specific)");
  m_pKeyboardSystemControl->BindAction('S',
      new VistaMakeScreenshotCommand(m_pDisplayManager, m_pClusterMode),
      "Make a screenshot (stored in screenshots subdir)");
}

void VistaSystem::RegisterConfigurators() {
  // the following registers the mapping from a trigger (key) in the ini
  // to some code that takes care for a proper parsing and setup for what it does
  // mean

  // as an example: when the key "DRIVERLOGGING" is encountered in the ini file
  // the VistaDriverLoggingConfigurator is used for setup. It is passed the section
  // where the DRIVERLOGGING key was found as a start and can decide on deeper
  // ini parsing and so on.
  m_pConfigurator->RegisterConfigurator("DRIVERLOGGING", new VistaDriverLoggingConfigurator);

  // this goes for HISTORY keys.
  m_pConfigurator->RegisterConfigurator("HISTORY", new VistaGenericHistoryConfigurator);

  // this goes for generic parameters.
  m_pConfigurator->RegisterConfigurator("PARAMETERS", new VistaDriverParameterConfigurator);

  // REGISTER CONFIGURATORS AS SUPPORTED BY THE SYSTEM
  // window stuff is kernel specific, so it is passed the display manager. Otherwise
  // is can not work.
  VistaWindowConfigurator* pWindowConfigurator = new VistaWindowConfigurator(GetDisplayManager());

  // now... that is tricky: some keys depend on the setup of other keys BEFOREHAND.
  // for example, the PROTOCOL key needs an ATTACHONLY to be setup BEFOREHAND.
  // knowing these dependencies is not easy and has to be deduced from the code.
  // cycles are going to make a bad day here. Dependencies can be modelled as a std::list of strings
  // that contain all keys that have to be setup BEFORE the key that is dependent on them (simple,
  // not?)

  std::list<std::string> liDepends;

  // add attach-only
  liDepends.push_back("ATTACHONLY");

  // liDepends is given as 3rd argument. So PROTOCOL depends on ATTACHONLY
  m_pConfigurator->RegisterConfigurator("PROTOCOL", new VistaDriverProtocolConfigurator, liDepends);

  // this is just for re-use of the container: clear dependency
  liDepends.clear();

  liDepends.push_back("DEFAULTWINDOW");
  // same as above: WINDOWS depends on DEFAULTWINDOW
  m_pConfigurator->RegisterConfigurator("WINDOWS", pWindowConfigurator, liDepends);

  // same as above: re-use container
  liDepends.clear();

  m_pConfigurator->RegisterConfigurator("DEFAULTWINDOW", pWindowConfigurator);
  liDepends.push_back("HISTORY");
  liDepends.push_back(
      "WINDOWS"); // some sensors might only be present on the connection to a window

  // SENSORS -> { WINDOWS, HISTORY }
  m_pConfigurator->RegisterConfigurator("SENSORS", new VistaSensorMappingConfigurator, liDepends);
  liDepends.clear();

  liDepends.push_back("PROTOCOL");
  m_pConfigurator->RegisterConfigurator(
      "REFERENCE_FRAME", new VistaDriverReferenceFrameConfigurator, liDepends);
  liDepends.clear();

  liDepends.push_back("PROTOCOL");
  liDepends.push_back("ATTACHONLY");
  liDepends.push_back("REFERENCE_FRAME");
  // CONNECTIONS -> { ATTACKONLY, PROTOCOL }
  // and yes... this is transitive when seen in relation to PROTOCOL
  m_pConfigurator->RegisterConfigurator("CONNECTIONS", new VistaConnectionConfigurator, liDepends);

  liDepends.clear();

  m_pConfigurator->RegisterConfigurator("ATTACHONLY", new VistaDriverAttachOnlyConfigurator);
  m_pConfigurator->RegisterConfigurator(
      "IDENTIFICATION", new VistaDeviceIdentificationConfigurator);
}

VistaInteractionContext* VistaSystem::SetupInteractionContext(const std::string& sContextName) {
  VistaInteractionContext* pContext = NULL;
  vstr::outi() << "[VistaSystem::SetupContext()]: Creating interaction context [" << sContextName
               << "]" << std::endl;
  vstr::IndentObject oIndent;

  if (m_oInteractionConfig.HasSubList(sContextName) == false) {
    vstr::warnp() << "Section for interaction Context [" << sContextName << "] does not exist"
                  << std::endl;
    return NULL;
  }

  const VistaPropertyList& oSection = m_oInteractionConfig.GetSubListConstRef(sContextName);

  std::string sRole;
  if (oSection.GetValue("ROLE", sRole) == false) {
    vstr::warnp() << "No role given for context [" << sContextName << "]!" << std::endl;
  }

  int nPrio = oSection.GetValueOrDefault<int>("PRIORITY", VistaInteractionManager::PRIO_DONTCARE);

  bool        bDelayedUpdate = oSection.GetValueOrDefault<bool>("DELAYED_UPDATE", false);
  std::string sGraphFile;
  oSection.GetValue("GRAPH", sGraphFile);
  sGraphFile = getAbsolutePathRelativeTo(sGraphFile, m_sInteractionConfigFile);
  if (sGraphFile.empty()) {
    vstr::warnp() << "No graph file defined for context [" << sRole << "]" << std::endl;
    return NULL;
  }

  pContext = new VistaInteractionContext(m_pInteractionManager, m_pEventManager);
  pContext->SetRoleId(m_pInteractionManager->RegisterRole(sRole));

  // positive on the role
  std::string sReloadKey;
  if (oSection.GetValue("RELOADTRIGGER", sReloadKey)) {
    int nKey, nMod;
    if (VistaKeyboardSystemControl::GetKeyAndModifiersValueFromString(sReloadKey, nKey, nMod) ==
        false) {
      vstr::warnp() << "RelaodTrigger [" << sReloadKey << "] for context [" << sRole
                    << "] could not be interpreted as key" << std::endl;
    } else if (m_pKeyboardSystemControl->GetActionForToken(nKey, nMod)) {
      vstr::warnp() << "RelaodTrigger [" << sReloadKey << "] for context [" << sRole
                    << "] already occupied\n";
      vstr::warni() << vstr::singleindent << "Current function: "
                    << m_pKeyboardSystemControl->GetHelpTextForToken(nKey, nMod) << std::endl;
    } else {
      bool bDumpGraphs =
          m_oInteractionConfig.GetValueInSubListOrDefault("DUMPDFNGRAPHS", "SYSTEM", false);
      bool bWritePorts =
          m_oInteractionConfig.GetValueInSubListOrDefault("WRITEDFNPORTS", "SYSTEM", false);

      m_pKeyboardSystemControl->BindAction(nKey, nMod,
          new VistaReloadContextGraphCommand(this, pContext, sRole, bDumpGraphs, bWritePorts),
          "reload graph for [" + sRole + "]");
      vstr::outi() << " [VistaSystem]: ReloadTrigger [" << sReloadKey << "] for context [" << sRole
                   << "] applied" << std::endl;
    }
  }

  // configure graph debugging
  std::string   sDebugStream;
  std::ostream* pStream       = &vstr::out();
  bool          bManageStream = false;
  bool          bDumpAsDot    = false;
  if (oSection.GetValue("DEBUGSTREAM", sDebugStream)) {
    if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sDebugStream, "dot")) {
      pStream    = NULL;
      bDumpAsDot = true;
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
                   sDebugStream, "VSTR::OUT")) {
      pStream = &vstr::out();
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
                   sDebugStream, "VSTR::warn")) {
      pStream = &vstr::warn();
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
                   sDebugStream, "VSTR::ERR")) {
      pStream = &vstr::err();
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sDebugStream, "cout")) {
      pStream = &std::cout;
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sDebugStream, "cerr")) {
      pStream = &std::cerr;
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sDebugStream, "clog")) {
      pStream = &std::clog;
    } else {
      pStream = new std::ofstream(sDebugStream.c_str());
      if (pStream->good()) {
        bManageStream = true;
      } else {
        delete pStream;
        pStream = NULL;
        vstr::warnp() << "Cannot create filestream [" << sDebugStream << "} for InterActionContext "
                      << sContextName << "]" << std::endl;
      }
    }
  }
  pContext->SetDumpAsDot(bDumpAsDot);
  pContext->SetDebuggingStream(pStream, bManageStream);

  std::string sDebugKey;
  if (oSection.GetValue("DEBUGTRIGGER", sDebugKey)) {
    // reload trigger was given by user:
    int nKey, nMod;
    if (VistaKeyboardSystemControl::GetKeyAndModifiersValueFromString(sDebugKey, nKey, nMod) ==
        false) {
      vstr::warnp() << "DebugTrigger [" << sDebugKey << "] for context [" << sRole
                    << "] could not be interpreted as key" << std::endl;
    } else if (m_pKeyboardSystemControl->GetActionForToken(nKey, nMod)) {
      vstr::warnp() << "DebugTrigger [" << sDebugKey << "] for context [" << sRole
                    << "] already occupied\n";
      vstr::warni() << vstr::singleindent << "Current function: "
                    << m_pKeyboardSystemControl->GetHelpTextForToken(nKey, nMod) << std::endl;
    } else {
      m_pKeyboardSystemControl->BindAction(nKey, nMod, new VistaDebugContextGraphCommand(pContext),
          "debug graph for [" + sRole + "]");
      vstr::outi() << " [VistaSystem]: DebugTrigger [" << sDebugKey << "] for context [" << sRole
                   << "] applied" << std::endl;
    }
  }

  pContext->SetAutoPrintDebugInfo(oSection.GetValueOrDefault<bool>("AUTODEBUG", false));

  bool bEnable = oSection.GetValueOrDefault<bool>("ENABLED", true);
  pContext->SetIsEnabled(bEnable);

  m_pInteractionManager->AddInteractionContext(pContext, nPrio, bDelayedUpdate);
  pContext->SetGraphSource(sGraphFile);

  return pContext;
}

bool VistaSystem::SetupMessagePort() {
  if (m_oVistaConfig.HasSubList(GetSystemSectionName()) == false)
    return false;
  const VistaPropertyList& oSystemSection =
      m_oVistaConfig.GetSubListConstRef(GetSystemSectionName());

  bool bCreateMsgPort = oSystemSection.GetValueOrDefault("MSGPORT", false);
  if (bCreateMsgPort == false)
    return true;

  if (m_pClusterMode->GetIsFollower()) {
    vstr::outi() << "[SetupMessagePort]: Creation skipped on Cluster-Follower-Node - "
                 << "will only be created on Leader-Nodes" << std::endl;
    return true;
  }

  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: Creating External TCP/IP Msg Port  -----" << std::endl;
  vstr::IndentObject oIndent;

  std::string sHost;
  oSystemSection.GetValueOrDefault<std::string>("MSGPORTIP", sHost, "localhost");
  int iPort = oSystemSection.GetValueOrDefault<int>("MSGPORTPORT", 6666);

  bool        bCreateIndicator = oSystemSection.GetValueOrDefault<bool>("PROGRESSINDICATOR", false);
  std::string sProgressHost;
  oSystemSection.GetValueOrDefault<std::string>("PROGRESSHOST", sProgressHost, "localhost");
  int iProgressPort = oSystemSection.GetValueOrDefault<int>("PROGRESSPORT", 6667);

  m_pMessagePort = new VistaKernelMsgPort(this, sHost, iPort, GetApplicationName(),
      bCreateIndicator, sProgressHost, iProgressPort, NULL);

  vstr::outi() << "MessagePort created at IP [" << sHost << "] - Port [" << iPort << "]"
               << std::endl;
  if (bCreateIndicator) {
    vstr::outi() << "MsgPort ProgressIndicator created at IP [" << sProgressHost << "] - Port ["
                 << iProgressPort << "]" << std::endl;
  }
  return true;
}

VistaUserPlatform* VistaSystem::GetPlatformFor(VistaDisplaySystem* pDSys) const {

  std::map<VistaDisplaySystem*, VistaUserPlatform*>::const_iterator cit =
      m_mapUserPlatforms.find(pDSys);
  if (cit == m_mapUserPlatforms.end())
    return NULL;
  return (*cit).second;
}

bool VistaSystem::SetupInteractionManager() {
  vstr::outi() << "\n-------------------------------------------------------------";
  vstr::outi() << "\n----- [VistaSystem]: InteractionManager Setup           -----" << std::endl;
  IndicateSystemProgress("Setup InteractionManager", false);
  vstr::IndentObject oIndent;

  m_pInteractionManager->Init(m_pDriverMap, m_pClusterMode->GetIsLeader());
  m_pKeyboardSystemControl = new VistaKeyboardSystemControl;
  m_pKeyboardSystemControl->SetProcessKeyCallbacksImmediately(false);
  GetEventManager()->AddEventHandler(m_pKeyboardSystemControl, VistaSystemEvent::GetTypeId(),
      VistaSystemEvent::VSE_POSTAPPLICATIONLOOP, VistaEventManager::PRIO_HIGH + 5);

  m_pInteractionManager->SetDriverMap(m_pDriverMap);

  if (SetupBasicInteraction() == false)
    return false;

  return true;
}

VistaInteractionManager* VistaSystem::GetInteractionManager() const {
  return this->m_pInteractionManager;
}

bool VistaSystem::Run() {
  if (m_bInitialized == false) {
    vstr::errp() << "[VistaSystem]: Trying to Run non-initialized VistaSystem!" << std::endl;
    return false;
  }

  if (GetInteractionManager()) {
    // load and initialize all graph structures
    GetInteractionManager()->InitializeGraphContexts(m_pClusterMode->GetNodeName());

    // for servers / master nodes, we will offer a dump on the graphs as a feature
    bool bDumpGraphs =
        m_oInteractionConfig.GetValueInSubListOrDefault("DUMPDFNGRAPHS", "SYSTEM", false);
    bool bWritePorts =
        m_oInteractionConfig.GetValueInSubListOrDefault("WRITEDFNPORTS", "SYSTEM", false);
    if (m_pClusterMode->GetIsLeader() && bDumpGraphs) {
      GetInteractionManager()->DumpGraphsToDot(bWritePorts);
    }

    vstr::outi() << "[VistaSystem]: Starting driver threads" << std::endl;
    GetInteractionManager()->StartDriverThread();
  }

  vstr::outi() << "##### Starting ViSTA Main Loop #####" << std::endl;
  m_pFrameLoop->Run();
  vstr::outi() << "##### Leaving ViSTA Main Loop #####" << std::endl;

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Quit                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::Quit() {
  m_pFrameLoop->Quit();
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ProcessEvent                                                */
/*                                                                            */
/*============================================================================*/
void VistaSystem::HandleEvent(VistaEvent* pEvent) {
  if (pEvent->GetType() == VistaSystemEvent::GetTypeId()) {
    switch (((VistaSystemEvent*)pEvent)->GetId()) {
    case VistaSystemEvent::VSE_UPDATE_INTERACTION: {
      if (m_pMessagePort && m_pMessagePort->GetRescheduleFlag()) {
        // we should clean-up first
        m_pMessagePort->Disconnect();
      } else if (m_pMessagePort && m_pMessagePort->HasMessage()) {
        VistaMsg* p = m_pMessagePort->GetNextMsg();
        m_pExternalMsg->SetHandled(false); // reset state
        m_pExternalMsg->SetThisMsg(p);
        m_pExternalMsg->SetId(VistaExternalMsgEvent::VEID_INCOMING_MSG);
        GetEventManager()->ProcessEvent(m_pExternalMsg);
#if defined(DEBUG)
        if (!p->GetMsgSuccess())
          PrintMsg("[VistaSystem]: KernelMsg marked as non-success.\n");
#endif
        m_pMessagePort->AnswerQuery(p);
        pEvent->SetHandled(true);
      }

      // check app msg ports
      if (!m_liAppMsgPorts.empty()) {
        for (std::list<VistaKernelMsgPort*>::iterator it = m_liAppMsgPorts.begin();
             it != m_liAppMsgPorts.end(); ++it) {
          if ((*it)->HasMessage()) {
            VistaMsg* p = (*it)->GetNextMsg();
            m_pExternalMsg->SetHandled(false); // reset state
            m_pExternalMsg->SetThisMsg(p);
            m_pExternalMsg->SetId(VistaExternalMsgEvent::VEID_INCOMING_MSG);
            GetEventManager()->ProcessEvent(m_pExternalMsg);
#if defined(DEBUG)
            if (!p->GetMsgSuccess())
              vstr::warnp() << "[VistaSystem]: KernelMsg marked as non-success" << std::endl;
#endif
            (*it)->AnswerQuery(p);
            pEvent->SetHandled(true);
          }
        }
      }
      break;
    }
    case VistaSystemEvent::VSE_QUIT: {
      this->Quit();
      pEvent->SetHandled(true);
      break;
    }

    case VistaSystemEvent::VSE_EXIT: {
      if (GetInteractionManager()) {
        GetInteractionManager()->StopDriverThread();
      }
      pEvent->SetHandled(true);
      break;
    }
    }
  } else if (pEvent->GetType() == VistaExternalMsgEvent::GetTypeId()) {
    VistaMsg* pMsg = Vista::assert_cast<VistaExternalMsgEvent*>(pEvent)->GetThisMsg();

    switch (pMsg->GetMsgType()) {
    case VistaKernelMsgType::VKM_KERNEL_MSG: {
      pEvent->SetHandled(true);
      try {
        if (m_pMessagePort)
          m_pMessagePort->DispatchKernelMsg(*pMsg);
      } catch (...) { PrintMsg("[VisSys]: ##-- Exception during HandleKernelMsg()! --##\n"); }
      break;
    }
    case VistaKernelMsgType::VKM_PROPERTYLIST_MSG: {
      // PrintMsg("[VistaSystem]: ## -- VKM_APPLICATION_MSG -- ##\n");

      VistaByteBufferDeSerializer dsSer;
      VistaMsg::MSG               vecMsg = pMsg->GetThisMsg();
      dsSer.SetBuffer(&vecMsg[0], (int)vecMsg.size());
      // get method
      int iMethod = 0;
      int iTicket = 0;

      dsSer.ReadInt32(iMethod);
      dsSer.ReadInt32(iTicket);
      (*m_pCommandEvent).SetMethodToken(iMethod);
      (*m_pCommandEvent).SetMessageTicket(iTicket);

      VistaPropertyList oPropList;
      std::string       sName;
      VistaPropertyList::DeSerializePropertyList(dsSer, oPropList, sName);
      (*m_pCommandEvent).SetPropertiesByList(oPropList);

      //(*m_pCommandEvent).DeSerialize(dsSer);
      (*m_pCommandEvent).SetCommandMsg(pMsg); // for answers
      GetEventManager()->ProcessEvent(m_pCommandEvent);
      m_pCommandEvent->GetCommandMsg()->SetMsgSuccess(m_pCommandEvent->IsHandled());

      if (m_pCommandEvent->UsesPropertyListAnswer()) { // set answer parameters
        VistaByteBufferSerializer ser;
        oPropList.clear();
        (*m_pCommandEvent).GetPropertiesByList(oPropList);

        VistaPropertyList::SerializePropertyList(ser, oPropList, "answer");

        //(*m_pCommandEvent).Serialize(ser);
        VistaMsg::MSG veAnswer;
        ser.GetBuffer(veAnswer);
        VistaMsg* pAnswerMsg = m_pCommandEvent->GetCommandMsg();
        if (pAnswerMsg)
          pAnswerMsg->SetMsgAnswer(veAnswer);
        else {
          // bogus.
        }
      }
      pEvent->SetHandled(true);
      break;
    }
    /** @todo remove me */
    case VistaKernelMsgType::VKM_SCRIPTED_MSG:
    default:
      break;
    }
  }
}

void VistaSystem::CreateDeviceDrivers() {
  vstr::outi() << "Creating Device Drivers" << std::endl;
  vstr::IndentObject oIndent;

  // first, we extract the list of pathes to search for driver plugins
  std::string sDriverPlugins;
  m_oInteractionConfig.GetValueInSubList(
      "DRIVERPLUGINDIRS", GetSystemSectionName(), sDriverPlugins);
  if (sDriverPlugins.empty()) {
    m_liDriverPluginPathes.push_back(".");
  } else {
    std::string sTmp = VistaEnvironment::ReplaceOSEnvironemntPathSeparators(sDriverPlugins, ',');
    VistaConversion::FromString(sTmp, m_liDriverPluginPathes);
  }

  if (m_liDriverPluginPathes.size() == 1) {
    vstr::outi() << "Driver Plugin Dir: " << m_liDriverPluginPathes.front() << std::endl;
  } else {
    vstr::outi() << "Driver Plugin Dirs:" << std::endl;
    vstr::IndentObject oIndent2;
    for (std::list<std::string>::const_iterator itDir = m_liDriverPluginPathes.begin();
         itDir != m_liDriverPluginPathes.end(); ++itDir) {
      vstr::outi() << (*itDir) << std::endl;
    }
  }

  std::list<std::string>            liDriverList;
  std::map<std::string, DriverInfo> mapCreatedDrivers;
  // get user defined devices to create, this looks up the SECTION to parse
  m_oInteractionConfig.GetValueInSubList("DEVICEDRIVERS", GetSystemSectionName(), liDriverList);

  // Load Plugins for all Drivers
  for (std::list<std::string>::const_iterator itDriver = liDriverList.begin();
       itDriver != liDriverList.end(); ++itDriver) {

    std::string sType;
    if (m_oInteractionConfig.GetValueInSubList("TYPE", (*itDriver),
            sType) == false) // but empty type is a no-go
    {
      vstr::errp() << "No type name given for the device driver [" << (*itDriver) << "]"
                   << std::endl;
      continue;
    }

    std::string sDriverPlugin;
    m_oInteractionConfig.GetValueInSubList("DRIVERPLUGIN", (*itDriver), sDriverPlugin);
    std::string sTranscoderPlugin;
    m_oInteractionConfig.GetValueInSubList("TRANSCODERPLUGIN", (*itDriver), sTranscoderPlugin);

    IVistaDriverCreationMethod* pDriverCRM = m_pInteractionManager->GetDriverCreationMethod(sType);

    if (!pDriverCRM) {
      if (LoadDriverPlugin(sType, sDriverPlugin, sTranscoderPlugin) == false) {
        vstr::warnp() << "Could not load Plugin for Driver of Type [" << sType << "]" << std::endl;
        continue;
      }
    }
  }

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // tricky stuff: setting up the devices for the standalone / master mode
  // and (more below) for the slave mode
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  if (m_pClusterMode->GetIsLeader()) {
    // create drivers as specified in the ini
    for (std::list<std::string>::const_iterator itDriver = liDriverList.begin();
         itDriver != liDriverList.end(); ++itDriver) {
      if (m_oInteractionConfig.HasSubList((*itDriver)) == false) {
        vstr::errp() << "Section for device driver [" << *itDriver << "] does not exist"
                     << std::endl;
        continue;
      }
      const VistaPropertyList& oDriverSection =
          m_oInteractionConfig.GetSubListConstRef((*itDriver));

      std::string sName = oDriverSection.GetValueOrDefault("NAME", (*itDriver));
      std::string sType;
      if (oDriverSection.GetValue("TYPE", sType) == false) // but empty type is a no-go
      {
        vstr::errp() << "No type name given for the device driver [" << *itDriver << "]"
                     << std::endl;
        continue;
      }

      // try to see whether we have a name clash
      if (m_pDriverMap->GetIsDeviceDriver(sName)) {
        // yes. no-go!
        vstr::warnp() << "Device Driver river of type [" + *itDriver + "] with name [" + sName +
                             "] already registered -- SKIPPING!"
                      << std::endl;
        continue;
      }

      // create driver of this type and name and
      // register with the driver map
      IVistaDriverCreationMethod* pDriverCRM =
          m_pInteractionManager->GetDriverCreationMethod(sType);
      if (pDriverCRM == NULL) {
        vstr::warnp() << "No driver of type [" << sType
                      << "] registered, check if the DRIVERPLUGINDIRS directory is set correctly "
                         "for your platform!"
                      << std::endl;
        continue;
      }

      // a bit stoopid to read: this creates the driver (operator call)
      IVistaDeviceDriver* pDriver = pDriverCRM->CreateDriver();
      if (pDriver == NULL) {
        vstr::warnp() << "Could not create driver [" << *itDriver << "] from factory method!"
                      << std::endl;
        continue;
      }

      vstr::outi() << "Adding driver of type [" + sType + "] with name [" + sName + "]"
                   << std::endl;

      // drivers can be sorted by priority for later use
      unsigned int nPrio = oDriverSection.GetValueOrDefault<unsigned int>("PRIORITY", 0);

      std::list<std::string> liDepends;
      // compound drivers can depend on other drivers to exist and setup beforehand.
      // this is modelled by an explicit dependency. for example:
      // a compound driver collects input from MOUSE and KEYBOARD, virtually
      // resulting in a 2DOF device with a number of buttons
      // COMPOUND -> { MOUSE, KEYBOARD }. Otherwise, the setup of the COMPOUND
      // may fail, just because the user swapped the entries in the DEVICEDRIVERS
      // section of the interaction ini file. The std::list can be empty, so in that case,
      // the creation and setup order is arbitrary.
      oDriverSection.GetValue("DEPENDSON", liDepends);
      mapCreatedDrivers[sName] = DriverInfo(pDriver, sType, sName, *itDriver, nPrio, liDepends);
    }

    // make a topological sort of the driver, respecting dependencies on other drivers
    VistaTopologyGraph<DriverInfo*> oDependencyGraph;
    for (std::map<std::string, DriverInfo>::iterator itDriver = mapCreatedDrivers.begin();
         itDriver != mapCreatedDrivers.end(); ++itDriver) {
      oDependencyGraph.AddNode(&(*itDriver).second);
      std::list<std::string>& liDependencies = (*itDriver).second.m_liDependsOn;
      for (std::list<std::string>::iterator itDep = liDependencies.begin();
           itDep != liDependencies.end(); ++itDep) {
        std::map<std::string, DriverInfo>::iterator itDepDriver = mapCreatedDrivers.find(*itDep);
        if (itDepDriver == mapCreatedDrivers.end())
          continue; // skip
        oDependencyGraph.AddDependency(&(*itDepDriver).second, &(*itDriver).second);
      }
    }

    // sort
    std::list<VistaTopologyGraph<DriverInfo*>::Node*> liConfigOrder =
        oDependencyGraph.CalcTopology();

    // iterate over this order.
    for (std::list<VistaTopologyGraph<DriverInfo*>::Node*>::const_iterator itDriverNode =
             liConfigOrder.begin();
         itDriverNode != liConfigOrder.end(); ++itDriverNode) {
      if (m_oInteractionConfig.HasSubList((*itDriverNode)->m_oElement->m_strSection) == false) {
        vstr::warnp() << "Could not initialize driver [" << (*itDriverNode)->m_oElement->m_strName
                      << "] - section [" << (*itDriverNode)->m_oElement->m_strSection
                      << "] does not exist" << std::endl;
        continue;
      }
      vstr::outi() << "Creating and initializing driver [" << (*itDriverNode)->m_oElement->m_strName
                   << "] from section [" << (*itDriverNode)->m_oElement->m_strSection << "]"
                   << std::endl;

      const VistaPropertyList& oDriverSection =
          m_oInteractionConfig.GetSubListConstRef((*itDriverNode)->m_oElement->m_strSection);

      // get a sorted order for the handling of the keys.
      // we want to evaluate the triggers with the above given dependencies,
      // but not all drivers have the same set of configurations,
      // so first, we figure out: which keys are used and which add an
      // dependency to other keys

      // in a nutshell: the keys might have a dependency, for example we should not
      // configure the protocol, when the connection is not yet setup and running.
      // and we only want to configure those keys that are really given in the ini
      // for example a non-given ATTACHONLY key should not be configured (as it
      // might be evaluated to true or false, ruining the default setup of the
      // driver)
      VistaTopologyGraph<std::string> liConfigDeps;
      for (VistaPropertyList::const_iterator itKey = oDriverSection.begin();
           itKey != oDriverSection.end(); ++itKey) {
        // for all keys, we look up the std::list of dependencies which was given
        // as a construction argument to the configurator
        std::list<std::string> liDependsOn = m_pConfigurator->GetDependsForTrigger((*itKey).first);

        // for any key in this dependency list, we check whether it really is a
        // dependency, or not
        if (liDependsOn.empty()) {
          // this trigger is used, but has no dependency to any other key
          // we add it to the dependency graph as a single node
          if (!liConfigDeps.HasNodeValue(
                  (*itKey).first))                // this checks the case when a user has given
          {                                       // a key twice in the ini as a mistake
            liConfigDeps.AddNode((*itKey).first); // add it as lonely node in the graph
          }
        } else {
          // this trigger has a dependency, but the other triggers must not be
          // part of this configuration, so we check
          for (std::list<std::string>::const_iterator cit = liDependsOn.begin();
               cit != liDependsOn.end(); ++cit) {
            // is the dependency part of this configuration?(e.g., is it given in the ini?)
            if (oDriverSection.find((*cit)) != oDriverSection.end())
              liConfigDeps.AddDependency(
                  (*cit), (*itKey).first); // yes, add only iff part of the configuration
            else {
              if (!liConfigDeps.HasNodeValue((*itKey).first)) // not a node, yet...
                liConfigDeps.AddNode(
                    (*itKey).first); // add the node without dependency, as a floating node
            }
          }
        }
      }

      // calculate a topological sort for the keys
      std::list<VistaTopologyGraph<std::string>::Node*> liConfig = liConfigDeps.CalcTopology();

      // iterate over the sorted std::list of triggers for the configuration
      for (std::list<VistaTopologyGraph<std::string>::Node*>::const_iterator keyit =
               liConfig.begin();
           keyit != liConfig.end(); ++keyit) {
        VistaDriverPropertyConfigurator::IConfigurator* pConf =
            m_pConfigurator->RetrieveConfigurator((*keyit)->m_oElement);
        if (pConf == NULL) {
          // except for TYPE and NAME, all entries should have a meaning
          // if not, we utter a warning
          if ((*keyit)->m_oElement != "TYPE" && (*keyit)->m_oElement != "NAME" &&
              (*keyit)->m_oElement != "DRIVERPLUGIN" &&
              (*keyit)->m_oElement != "TRANSCODERPLUGIN") {
            vstr::warnp() << "No driver configurator for key [" << (*keyit)->m_oElement
                          << "] in section of driver [" << (*itDriverNode)->m_oElement->m_strName
                          << "]" << std::endl;
          }
          continue;
        }
        // we have a configurator for this trigger
        VistaPropertyList oProps;

        if (pConf->Configure((*itDriverNode)->m_oElement->m_pDriver, oDriverSection,
                m_oInteractionConfig) == false) {
          vstr::errp() << "Unable to configure element type [" << (*keyit)->m_oElement << "]"
                       << std::endl;
        }
      }

      // we finally activate the drivers
      if ((*itDriverNode)->m_oElement->m_pDriver->Connect() == false) {
        vstr::errp() << "Could not successfully connect the driver,"
                     << " so it will not be enabled / registered." << std::endl;
        delete (*itDriverNode)->m_oElement->m_pDriver;
        continue;
      }

      // ok, worked, add to interaction manager for update
      m_pInteractionManager->AddDeviceDriver((*itDriverNode)->m_oElement->m_strName,
          (*itDriverNode)->m_oElement->m_pDriver, (*itDriverNode)->m_oElement->m_nPrio);

      // ok, in case the driver had a sensor mapping aspect, the
      // sensor mapping configurator has taken care of setting up
      // the sensor names. In case this driver adds sensors dynamically
      // after a connect, this is not working well, so we create the proper
      // names AFTER a successful connect

      VistaDriverSensorMappingAspect* pSensorAsp = dynamic_cast<VistaDriverSensorMappingAspect*>(
          (*itDriverNode)
              ->m_oElement->m_pDriver->GetAspectById(
                  VistaDriverSensorMappingAspect::GetAspectId()));
      if (pSensorAsp == NULL) {
        std::list<std::string> liSensors;
        oDriverSection.GetValue("SENSORS", liSensors);

        int nDefaultHistorySize = oDriverSection.GetValueOrDefault("HISTORY", 5);

        for (std::list<std::string>::const_iterator itSensor = liSensors.begin();
             itSensor != liSensors.end(); ++itSensor) {
          // @todo: code duplicate with sensormappingconfigrator and also historyconfigurator
          // - think about this - how should we set it up properly?

          // get sensor PropertyList
          if (m_oInteractionConfig.HasSubList(*itSensor) == false) {
            vstr::warnp() << "[SensorMappingConfiguator]: Driver requests sensor [" << (*itSensor)
                          << "], but no such entry exists" << std::endl;
            continue;
          }

          const VistaPropertyList& oSensor = m_oInteractionConfig.GetSubListConstRef(*itSensor);
          std::string              sType   = oSensor.GetValueOrDefault<std::string>("TYPE", "");
          std::string sSensorName = oSensor.GetValueOrDefault<std::string>("NAME", (*itSensor));
          int         nHistorySize =
              std::max<int>(2, oSensor.GetValueOrDefault<int>("HISTORY", nDefaultHistorySize));

          int nRawId;
          if (oSensor.GetValue<int>("RAWID", nRawId) == false) {
            vstr::warnp() << "[SensorMappingConfiguator]: Driver requests sensor [" << (*itSensor)
                          << "] which has no RAWID entry" << std::endl;
            continue;
          }

          VistaDeviceSensor* pSensor =
              (*itDriverNode)->m_oElement->m_pDriver->GetSensorByIndex(nRawId);
          if (pSensor) {
            pSensor->SetSensorName(sSensorName);
            (*itDriverNode)->m_oElement->m_pDriver->SetupSensorHistory(pSensor, nHistorySize, 66.6);
          }
        }
      }

      /**
      std::list<std::string> liSensorNames;
      oDriverSection.GetValue( "SENSORS", liSensorNames );
      for( std::list<std::string>::const_iterator itSensor = liSensorNames.begin();
              itSensor != liSensorNames.end(); ++itSensor )
      {
              if( m_oInteractionConfig.HasSubList( (*itSensor) ) == false )
              {
                      vstr::warnp() << "Driver ["
                                      << (*itDriverNode)->m_oElement->m_strName
                                      << "] requests sensor [" << (*itSensor)
                                      << "] which is not defined!" << std::endl;
                      continue;
              }
              const VistaPropertyList& oSensorSection = m_oInteractionConfig.GetSubListConstRef(
      (*itSensor) );
              // see whether the user specified the proper index
              unsigned int nIdx;
              if( oSensorSection.GetValue( "RAWID", nIdx ) == false )
              {
                      vstr::warnp() << "Driver ["
                                      << (*itDriverNode)->m_oElement->m_strName
                                      << "] requests sensor [" << (*itSensor)
                                      << "] which is missing the RAWID entry!" << std::endl;
                      continue;
              }

              // see whether the user *wants* to set a name
              std::string strName;
              if( oSensorSection.GetValue( "NAME", strName ) )
                      continue; // no need to set name

              std::string sSensorType;
              oSensorSection.GetValue("TYPE", sSensorType);

              // get sensor by index
              IVistaDeviceDriver *pDriver = (*itDriverNode)->m_oElement->m_pDriver;
              VistaDeviceSensor *pSensor = pDriver->GetSensorByIndex(nIdx);
              if(pSensor)
              {
                      int nHistory = oSensorSection.GetValueOrDefault<int>( "HISTORY", 5 );
                      VistaDriverMeasureHistoryAspect *pHist
                              = dynamic_cast<VistaDriverMeasureHistoryAspect*>(
                                              pDriver->GetAspectById(
      VistaDriverMeasureHistoryAspect::GetAspectId() ) ); if(pHist) // should never be NULL
                      {
                              unsigned int nUpdateRateOfSensorInHz =
      pDriver->GetFactory()->GetUpdateEstimatorFor( pSensor->GetTypeHint() ); if(
      nUpdateRateOfSensorInHz == IVistaDriverCreationMethod::INVALID_TYPE )
                              {
                                      vstr::warnp() << "DriverCreationMethod of driver ["
                                                      << (*itDriverNode)->m_oElement->m_strName <<
      "] does not report an Update Rate for Sensor Type ["
                                                      << sSensorType << "]" << std::endl;
                                      continue;
                              }

                              int nNum = int( ceilf( 66.6f / ( 1000.0f /
      float(nUpdateRateOfSensorInHz) ) ) ); unsigned int nMeasureSize =
      pDriver->GetFactory()->GetMeasureSizeFor( pSensor->GetTypeHint() ); if( nMeasureSize == ~0 )
                                      nMeasureSize = 0;

                              pHist->SetHistorySize( pSensor, nHistory, 2*nNum, nMeasureSize );
                      }
                      pSensor->SetSensorName( strName ); // set name
              }
      }
      */
    }
  } else // --> NOT m_pClusterMode->GetIsLeader
  {
    // in follower mode, we need a driver that is capable of receiving the incoming sensor data
    // during event dispatching. However, we do not want to create the physical driver, as this
    // device might not even be connected to the host machine. So we create a dummy / shallow driver
    // for every driver request and set it up according to the specification.

    for (std::list<std::string>::const_iterator itDriver = liDriverList.begin();
         itDriver != liDriverList.end(); ++itDriver) {
      if (m_oInteractionConfig.HasSubList((*itDriver)) == false) {
        vstr::errp() << "Section for device driver [" << *itDriver << "] does not exist"
                     << std::endl;
        continue;
      }
      const VistaPropertyList& oDriverSection =
          m_oInteractionConfig.GetSubListConstRef((*itDriver));

      // we need a name and a type. without... there is not much sense in
      // continuing... well, for the name, we default to the section name.
      std::string sName = oDriverSection.GetValueOrDefault("NAME", (*itDriver));
      std::string sType;
      if (oDriverSection.GetValue("TYPE", sType) == false) // but empty type is a no-go
      {
        vstr::errp() << "No type name given for the device driver [" << *itDriver << "]"
                     << std::endl;
        continue;
      }

      // try to see whether we have a name clash
      if (m_pDriverMap->GetIsDeviceDriver(sName)) {
        // yes. no-go!
        vstr::warnp() << "Device Driver river of type [" + *itDriver + "] with name [" + sName +
                             "] already registered -- SKIPPING!"
                      << std::endl;
        continue;
      }

      // check if driver creation method is available
      IVistaDriverCreationMethod* pCreationMethod =
          m_pInteractionManager->GetDriverCreationMethod(sType);
      if (pCreationMethod == NULL) {
        vstr::warnp() << "No driver of type [" << sType
                      << "] registered, check if the DRIVERPLUGINDIRS directory is set correctly "
                         "for your platform!"
                      << std::endl;
        continue;
      }

      // create a shallow driver for this type
      VistaShallowDriver* pDriver = dynamic_cast<VistaShallowDriver*>(
          (*VistaShallowDriver::GetDriverFactoryMethod()).CreateDriver());

      vstr::outi() << "Adding driver of type [" + sType + "] with name [" + sName + "]"
                   << std::endl;

      int def_history_size;
      oDriverSection.GetValueOrDefault<int>("HISTORY", def_history_size, 1);

      unsigned int nPrio       = oDriverSection.GetValueOrDefault<unsigned int>("PRIORITY", 0);
      mapCreatedDrivers[sName] = DriverInfo(
          pDriver, sType, sName, (*itDriver), nPrio, std::list<std::string>(), def_history_size);
    }

    for (std::map<std::string, DriverInfo>::const_iterator itDriver = mapCreatedDrivers.begin();
         itDriver != mapCreatedDrivers.end(); ++itDriver) {
      if (m_oInteractionConfig.HasSubList(itDriver->second.m_strSection) == false)
        continue; // should not happen, but better check than cry
      const VistaPropertyList& oDriverSection =
          m_oInteractionConfig.GetSubListConstRef(itDriver->second.m_strSection);

      VistaDriverSensorMappingAspect* pSensorMapping = NULL;
      // for this shallow driver, rebuild the settings in the driver creation method
      // 1. claim original method
      IVistaDriverCreationMethod* pOrigin =
          m_pDriverMap->GetDriverCreationMethod((*itDriver).second.m_strTypeName);
      if (pOrigin == NULL) {
        vstr::warnp() << "Could not retrieve DriverCreationMethod for driver type["
                      << (*itDriver).second.m_strTypeName << "]" << std::endl;
      } else {
        std::list<unsigned int> liTypes = pOrigin->GetTypes();
        if (!liTypes.empty()) {
          // this driver has a sensor mapping!
          pSensorMapping = new VistaDriverSensorMappingAspect(pOrigin);
          (*itDriver).second.m_pDriver->RegisterAspect(pSensorMapping);
        }

        // rebuild sensors using the original sensor attributes
        std::list<std::string> liSensorNames;
        oDriverSection.GetValue("SENSORS", liSensorNames);
        for (std::list<std::string>::const_iterator itSensor = liSensorNames.begin();
             itSensor != liSensorNames.end(); ++itSensor) {
          if (m_oInteractionConfig.HasSubList((*itSensor)) == false) {
            vstr::warnp() << "Driver [" << (*itDriver).second.m_strName << "] requests sensor ["
                          << (*itSensor) << "] which is not defined!" << std::endl;
            continue;
          }
          const VistaPropertyList& oSensorSection =
              m_oInteractionConfig.GetSubListConstRef((*itSensor));
          // see whether the user specified the proper index
          unsigned int nSensorId;
          if (oSensorSection.GetValue("RAWID", nSensorId) == false) {
            vstr::warnp() << "Driver [" << (*itDriver).second.m_strName << "] requests sensor ["
                          << (*itSensor) << "] which is missing the RAWID entry!" << std::endl;
            continue;
          }

          // int nHistory = oSensorSection.GetValueOrDefault<int>( "HISTORY", 5 );

          // see whether the user *wants* to set a name
          std::string sSensorName = (*itSensor);
          oSensorSection.GetValue("NAME", sSensorName);

          std::string sSensorType;
          oSensorSection.GetValue("TYPE", sSensorType);

          unsigned int nType = pOrigin->GetTypeFor(sSensorType);
          if (nType == IVistaDriverCreationMethod::INVALID_TYPE) {
            vstr::warnp() << "Requested unknown sensor type [" << sSensorType << "] for driver ["
                          << (*itDriver).first << "]" << std::endl;
            continue;
          }
          IVistaMeasureTranscoderFactory* pTranscoderFac =
              pOrigin->GetTranscoderFactoryForSensor(nType);
          if (pTranscoderFac == NULL) {
            vstr::warnp() << "Could not retrieve transcoder factory for sensor type ["
                          << sSensorType << "] for driver [" << (*itDriver).first << "]"
                          << std::endl;
            continue;
          } else {
            VistaDeviceSensor* pSensor = new VistaDeviceSensor;
            pSensor->SetTypeHint(sSensorType);
            pSensor->SetSensorName(sSensorName);

            pSensor->SetMeasureTranscode(pTranscoderFac->CreateTranscoder());

            unsigned int nDriverSensorId = (*itDriver).second.m_pDriver->AddDeviceSensor(pSensor);

            if (pSensorMapping) {
              // create a proper mapping
              pSensorMapping->SetSensorId(nType, nSensorId, nDriverSensorId);
            }
          }
        }
      } // if pOrigin
      // add driver to the driver map with the given name
      if ((*itDriver).second.m_pDriver->Connect()) {
        m_pInteractionManager->AddDeviceDriver(
            (*itDriver).second.m_strName, (*itDriver).second.m_pDriver, (*itDriver).second.m_nPrio);

        // note that for the clustered case, we create shallow drivers
        // with a proper sensor mapping aspect, so the sensor naming is
        // supposed to be allright at this point and there is no need
        // to setup the names (was already done!)
      } else {
        vstr::errp() << "Could not sucessfully connect the driver,"
                     << " so it will not be enabled / registered." << std::endl;
        delete (*itDriver).second.m_pDriver;
      }
    } // for all drivers
  }
}

VistaKernelMsgPort* VistaSystem::SetExternalMsgPort(VistaKernelMsgPort* pPort) {
  VistaKernelMsgPort* pOldPort = m_pMessagePort;
  m_pMessagePort               = pPort;
  return pOldPort;
}

void VistaSystem::DisconnectExternalMsgPort() {
  if (m_pMessagePort)
    m_pMessagePort->Disconnect();
}

bool VistaSystem::HasExternalMsgPort() const {
  return (m_pMessagePort != NULL);
}

bool VistaSystem::HasExternalConnection() const {
  return m_pMessagePort && m_pMessagePort->GetIsConnected();
}

VistaGraphicsManager* VistaSystem::GetGraphicsManager() const {
  return m_pGraphicsManager;
}

VistaDisplayManager* VistaSystem::GetDisplayManager() const {
  return m_pDisplayManager;
}

VistaEventManager* VistaSystem::GetEventManager() const {
  return m_pEventManager;
}

VistaDriverMap* VistaSystem::GetDriverMap() const {
  return m_pDriverMap;
}

VistaDriverPropertyConfigurator* VistaSystem::GetDriverConfigurator() const {
  return m_pConfigurator;
}

VistaClusterMode* VistaSystem::GetClusterMode() const {
  return m_pClusterMode;
}

bool VistaSystem::GetIsClusterLeader() const {
  return m_pClusterMode->GetIsLeader();
}

bool VistaSystem::GetIsClusterFollower() const {
  return m_pClusterMode->GetIsFollower();
}

bool VistaSystem::IndicateApplicationProgress(
    int iAppProgress, const std::string& sProgressMessage) {
  if (HasExternalMsgPort()) {
    return m_pMessagePort->WriteProgress(
        VistaKernelMsgPort::PRG_APP, iAppProgress, sProgressMessage);
  }
  return false;
}

bool VistaSystem::IndicateSystemProgress(const std::string& sProgressMessageText, bool bDone) {
  if (HasExternalMsgPort()) {
    return m_pMessagePort->WriteProgress(
        (bDone ? VistaKernelMsgPort::PRG_DONE : VistaKernelMsgPort::PRG_SYSTEM),
        ++m_iInitProgressIndicator, sProgressMessageText);
  }
  return false;
}

int VistaSystem::GenerateRandomNumber(int iLowerBound, int iMaxBound) const {
  // return m_pClusterMode->GenerateRandomNumber(iLowerBound, iMaxBound);
  return VistaRandomNumberGenerator::GetStandardRNG()->GenerateInt32(iLowerBound, iMaxBound);
}

void VistaSystem::SetRandomSeed(int iSeed) {
  // m_pClusterMode->SetRandomSeed(iSeed);
  VistaRandomNumberGenerator::GetStandardRNG()->SetSeed(iSeed);
}

bool VistaSystem::PrintMsg(const char* pMsg, std::ostream* pStream) {
  if (pMsg == NULL)
    pMsg = ""; // lets be tolerant :)

  return PrintMsg(std::string(pMsg), pStream);
}

bool VistaSystem::PrintMsg(const std::string& strMsg, std::ostream* pStream) {
  if (pStream == NULL) {
    pStream = &vstr::out();
  }
  (*pStream) << strMsg;
  (*pStream).flush();
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IntroMsg                                                    */
/*                                                                            */
/*============================================================================*/

bool VistaSystem::IntroMsg(std::ostream* pStream) {
  PrintMsg("\n******************************************************************************\n",
      pStream);
  PrintMsg(
      "*                                                                            *\n", pStream);
  PrintMsg(
      "* ViSTA - Virtual Reality for Scientific and Technical Applications          *\n", pStream);
  PrintMsg(
      "* =================================================================          *\n", pStream);
  PrintMsg(
      "*                                                                            *\n", pStream);
  PrintMsg(
      "* Developed at:                                                              *\n", pStream);
  PrintMsg(
      "* Virtual Reality Group, RWTH Aachen University                              *\n", pStream);
  PrintMsg(
      "*                                                                            *\n", pStream);
  PrintMsg(
      "* Torsten W. Kuhlen et al.,           1998-2014                              *\n", pStream);
  PrintMsg(
      "*                                                                            *\n", pStream);
  PrintMsg(
      "* use option -h for help                                                     *\n", pStream);
  PrintMsg(
      "*                                                                            *\n", pStream);
  PrintMsg(
      "******************************************************************************\n", pStream);
  PrintMsg("ViSTA-Version: " + std::string(VistaVersion::GetVersion()) +
               ", Release: " + VistaVersion::GetReleaseName() +
               "\nbuild date: " + VistaVersion::GetBuildDateString() +
               ", time: " + VistaVersion::GetBuildTimeString() + "\n\n",
      pStream);
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ArgHelpMsg                                                  */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::ArgHelpMsg(const std::string& sAppName, std::ostream* pStream) {
  /**
   * @todo add registry for command-line options? (boost::program_options or alike?)
   */
  PrintMsg("\n", pStream);
  PrintMsg(sAppName + "\n[-vistaini <IniFile>]\n", pStream);
  PrintMsg("      [-loadmodel <ModelFile> [-scalemodel <scale>]]\n", pStream);
  PrintMsg("      [-h|help]\n", pStream);
  PrintMsg("      \n", pStream);
  PrintMsg("Possible parameters:\n", pStream);
  PrintMsg("--------------------\n");
  PrintMsg("-loadmodel <ModelFile>              : load the specified model\n", pStream);
  PrintMsg("-scalemodel <scale>                 : scale the specified model\n", pStream);
  PrintMsg("-vistaini <IniFileName>             : use the specified initialization file for "
           "SYSTEMWIDE (or all) settings\n",
      pStream);
  PrintMsg("-displayini <IniFileName>           : use the specified initialization file for "
           "DISPLAY settings\n",
      pStream);
  PrintMsg("-graphicsini <IniFileName>          : use the specified initialization file for "
           "GRAPHICS settings\n",
      pStream);
  PrintMsg("-interactionini <IniFileName>       : use the specified initialization file for "
           "INTERACTION settings\n",
      pStream);
  PrintMsg("-clusterini <IniFileName>           : use the specified initialization file for "
           "CLUSTER settings\n",
      pStream);
  PrintMsg("-inisearchpath {(<path>{,})*}<path> : use the specified, comma-separated search paths "
           "for ini files\n",
      pStream);
  PrintMsg("-record <path>                      : record the session and write it to the specified "
           "folder\n",
      pStream);
  PrintMsg("-replay <path>                      : replay a recorded session written to the "
           "specified folder\n",
      pStream);
  PrintMsg("-kill_after_frame <frames>          : ends application after the specified amount of "
           "frames\n",
      pStream);
  PrintMsg(
      "-kill_after_time <seconds>          : ends application after the specified time\n", pStream);
  PrintMsg("-capture_frames <n>                 : captures every n'th frame\n", pStream);
  PrintMsg("-capture_frames_periodically <t>    : captures frame every t seconds\n", pStream);
  PrintMsg("-capture_frames_with_framerate <f>  : captures frame at f Hz frequency framerate\n",
      pStream);
  PrintMsg("-capture_frames_filename <pattern>  : captured files are written with the according "
           "filename pattern\n",
      pStream);
  PrintMsg("-disable_stream_colors              : disables the use of stream colors when parsing "
           "stream configs\n",
      pStream);

  PrintMsg("-h|help                 : this message\n", pStream);
  PrintMsg("--------------------\n", pStream);
  PrintMsg("\n\n\n", pStream);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ArgParser                                                   */
/*                                                                            */
/*============================================================================*/
bool VistaSystem::ArgParser(int argc, char* argv[]) {
  // if( argc == 1 )
  //{
  //	ArgHelpMsg(argv[0]);
  //	return true;
  //}

  VistaAspectsComparisonStuff::StringCompareObject oStringCompare(true);

  // 'parse' command-line arguments
  for (int arg = 1; arg < argc; ++arg) {
    // get argument key
    std::string strArg = argv[arg];

    // check if user wants help
    if ((strArg.compare("-h") == 0) || (strArg.compare("-help") == 0)) {
      ArgHelpMsg(argv[0]);
      return false;
    }
    // select ViSTA's initialization file
    else if (oStringCompare(strArg, "-vistaini")) {
      ++arg;
      if (arg < argc)
        SetIniFile(argv[arg]);
    } else if (oStringCompare(strArg, "-displayini")) {
      ++arg;
      if (arg < argc) {
        SetDisplayIniFile(argv[arg]);
      }
    } else if (oStringCompare(strArg, "-graphicsini")) {
      ++arg;
      if (arg < argc) {
        SetGraphicsIniFile(argv[arg]);
      }
    } else if (oStringCompare(strArg, "-interactionini")) {
      ++arg;
      if (arg < argc) {
        SetInteractionIniFile(argv[arg]);
      }
    } else if (oStringCompare(strArg, "-clusterini")) {
      ++arg;
      if (arg < argc)
        SetClusterIniFile(argv[arg]);
    }
    // check if we should load an initial file
    else if (oStringCompare(strArg, "-loadmodel")) {
      ++arg;
      if (arg < argc) {
        m_sModelFile = argv[arg];
      }
    }
    // check if we should load an initial file
    else if (oStringCompare(strArg, "-scalemodel")) {
      ++arg;
      if (arg < argc) {
        std::string strScale = argv[arg];
        m_nModelScale        = (float)atof(strScale.c_str());
      }
    }
    // set Vistas ClusterMode
    else if (oStringCompare(strArg, "-standalone")) {
      ++arg;
      if (arg < argc) {
        m_sClusterNodeName = argv[arg];
        m_nClusterNodeType = VistaClusterMode::NT_STANDALONE;
      }
    } else if (oStringCompare(strArg, "-clustermaster")) {
      ++arg;
      if (arg < argc) {
        m_sClusterNodeName = argv[arg];
        m_nClusterNodeType = VistaClusterMode::NT_MASTER;
      }
    } else if (oStringCompare(strArg, "-clusterslave")) {
      ++arg;
      if (arg < argc) {
        m_sClusterNodeName = argv[arg];
        m_nClusterNodeType = VistaClusterMode::NT_SLAVE;
      }
    } else if (oStringCompare(strArg, "-newclustermaster")) {
      ++arg;
      if (arg < argc) {
        m_sClusterNodeName     = argv[arg];
        m_nClusterNodeType     = VistaClusterMode::NT_MASTER;
        m_bUseNewClusterMaster = true;
      }
    } else if (oStringCompare(strArg, "-newclusterslave")) {
      ++arg;
      if (arg < argc) {
        m_sClusterNodeName     = argv[arg];
        m_nClusterNodeType     = VistaClusterMode::NT_SLAVE;
        m_bUseNewClusterMaster = true;
      }
    } else if (oStringCompare(strArg, "-record")) {
      ++arg;
      if (arg < argc) {
        m_sRecordFile = argv[arg];
      }
    } else if (oStringCompare(strArg, "-replay")) {
      ++arg;
      if (arg < argc) {
        m_sClusterNodeName = argv[arg];
        m_nClusterNodeType = VistaClusterMode::NT_REPLAY_SLAVE;
      }
    } else if (oStringCompare(strArg, "-inisearchpath")) {
      vstr::outi() << "[ViSy]: encountered argument -inisearchpath" << std::endl;
      ++arg;
      if (arg < argc) {
        vstr::outi() << "[ViSy]: using explicit inisearchpath [" << argv[arg] << "]" << std::endl;
        m_liSearchPathes.clear();
        std::string sArg(argv[arg]);
        if (sArg.compare("-none-") != 0) {
          VistaConversion::FromString(sArg, m_liSearchPathes);
        } else {
          vstr::outi() << "[VistaSystem]: Using -none-, only local path (.) is used (default)."
                       << std::endl;
        }
        m_bLockSearchPath = true;
      }
    } else if (oStringCompare(strArg, "-kill_after_frame")) {
      ++arg;
      if (arg < argc) {
        unsigned int nFrameCount = 0;
        if (VistaConversion::FromString<unsigned int>(argv[arg], nFrameCount) == false) {
          vstr::warnp() << "[ViSys]: param \"-kill_after_frame\" specifies \"" << argv[arg]
                        << "which cannot be parsed as uint" << std::endl;
        } else {
          if (m_pRuntimeLimiter == NULL)
            m_pRuntimeLimiter = new VistaRuntimeLimiter(this);
          m_pRuntimeLimiter->SetFrameLimit(nFrameCount);
        }
      }
    } else if (oStringCompare(strArg, "-kill_after_time")) {
      ++arg;
      if (arg < argc) {
        VistaType::microtime nTimeLimit = 0;
        if (VistaConversion::FromString<VistaType::microtime>(argv[arg], nTimeLimit) == false) {
          vstr::warnp() << "[ViSys]: param \"-kill_after_time\" specifies \"" << argv[arg]
                        << "which cannot be parsed as double" << std::endl;
        } else {
          if (m_pRuntimeLimiter == NULL)
            m_pRuntimeLimiter = new VistaRuntimeLimiter(this);
          m_pRuntimeLimiter->SetTimeLimit(nTimeLimit);
        }
      }
    } else if (oStringCompare(strArg, "-capture_frames")) {
      ++arg;
      if (arg < argc) {
        int nTime = 1;
        if (VistaConversion::FromString<int>(argv[arg], nTime) == false) {
          vstr::warnp() << "[ViSys]: param \"-capture_frames\" specifies \"" << argv[arg]
                        << "which cannot be parsed as int" << std::endl;
        } else {
          if (nTime == 1) {
            m_eFrameCaptureMode = VistaFrameSeriesCapture::CM_EVERY_FRAME;
          } else {
            m_eFrameCaptureMode      = VistaFrameSeriesCapture::CM_EVERY_NTH_FRAME;
            m_nFrameCaptureParameter = (double)nTime;
          }
        }
      }
    } else if (oStringCompare(strArg, "-capture_frames_periodically")) {
      ++arg;
      if (arg < argc) {
        if (VistaConversion::FromString<double>(argv[arg], m_nFrameCaptureParameter) == false) {
          vstr::warnp() << "[ViSys]: param \"-capture_frames_periodically\" specifies \""
                        << argv[arg] << "which cannot be parsed as double" << std::endl;
        } else {
          m_eFrameCaptureMode = VistaFrameSeriesCapture::CM_PERIODICALLY;
        }
      }
    } else if (oStringCompare(strArg, "-capture_frames_with_framerate")) {
      ++arg;
      if (arg < argc) {
        if (VistaConversion::FromString<double>(argv[arg], m_nFrameCaptureParameter) == false) {
          vstr::warnp() << "[ViSys]: param \"-capture_frames_with_framerate\" specifies \""
                        << argv[arg] << "which cannot be parsed as double" << std::endl;
        } else {
          m_eFrameCaptureMode = VistaFrameSeriesCapture::CM_FIXED_FRAMERATE;
        }
      }
    } else if (oStringCompare(strArg, "-capture_frames_filename")) {
      ++arg;
      if (arg < argc) {
        m_sCaptureFramesFilePattern = argv[arg];
      }
    } else if (oStringCompare(strArg, "-disable_stream_colors")) {
      m_bAllowStreamColors = false;
    }
  }
  return true;
}

double VistaSystem::GetFrameClock() const {
  return m_pClusterMode->GetFrameClock();
}

const std::string& VistaSystem::GetApplicationName() const {
  return m_sApplicationName;
}

bool VistaSystem::RegisterMsgPort(VistaKernelMsgPort* pPort) {
  m_liAppMsgPorts.push_back(pPort);
  return true;
}

VistaKernelMsgPort* VistaSystem::UnregisterMsgPort(VistaKernelMsgPort* pPort) {
  std::list<VistaKernelMsgPort*>::iterator it =
      std::find(m_liAppMsgPorts.begin(), m_liAppMsgPorts.end(), pPort);
  if (it != m_liAppMsgPorts.end()) {
    // found
    m_liAppMsgPorts.erase(it); // remove iterator
    return pPort;
  }
  return NULL;
}

IVistaSystemClassFactory* VistaSystem::GetSystemClassFactory() const {
  return m_pSystemClassFactory;
}

VistaFrameLoop* VistaSystem::GetFrameLoop() const {
  return m_pFrameLoop;
}

void VistaSystem::SetFrameLoop(VistaFrameLoop* pLoop, bool bDeleteExisting) {
  if (bDeleteExisting && m_pFrameLoop)
    delete m_pFrameLoop;

  m_pFrameLoop = pLoop;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaSystem::Debug(std::ostream& oOut) const {
  oOut << " [ViSystem] ViSTA Version : " << VistaVersion::GetVersion() << "("
       << VistaVersion::GetReleaseName() << ")";
  oOut << " (Major = " << VistaVersion::GetMajor();
  oOut << " , Minor = " << VistaVersion::GetMinor();
  oOut << " , Revision = " << VistaVersion::GetRevision() << ")\n";
  oOut << " [ViSystem] ViSTA       INI: " << GetIniFile() << "\n";
  oOut << " [ViSystem] Display     INI: " << GetDisplayIniFile() << "\n";
  oOut << " [ViSystem] Graphics    INI: " << GetGraphicsIniFile() << "\n";
  oOut << " [ViSystem] Interaction INI: " << GetInteractionIniFile() << "\n";
  oOut << " [ViSystem] Cluster     INI: " << GetClusterIniFile() << "\n";
  oOut << " [ViSystem] Loaded Model   : " << (m_sModelFile.empty() ? "<none>" : m_sModelFile)
       << "\n";
  oOut << " [ViSystem] initialized    : " << (m_bInitialized ? "true" : "false") << "\n";
  oOut << " [ViSystem] ClusterMode    : " << m_pClusterMode->GetClusterModeName() << std::endl;
}

std::string VistaSystem::FindFileInIniSearchPath(
    const std::string& filename, std::ostream* logger) const {
#ifdef DEBUG
  if (!logger)
    logger = &vstr::out();
#endif

  // treat absolute pathes
  if (isAbsolutePath(filename)) {
    VistaFileSystemFile f(filename);
    if (f.Exists() && !f.IsDirectory()) {
      if (logger)
        *logger << "'" << filename << "' found." << std::endl;
      return filename;
    } else
      return std::string("");
  }

  // treat relative pathes. search in all search pathes.
  if (logger) {
    *logger << "Searching for : '" << filename << "' in the following places:\n";
  }

  for (std::list<std::string>::const_iterator it = GetIniSearchPaths().begin();
       it != GetIniSearchPaths().end(); ++it) {
    std::string fname = *it + filename;
    fname             = getAbsolutePath(fname);
    VistaFileSystemFile iniFile(fname);
    if (iniFile.Exists() && !iniFile.IsDirectory()) {
      if (logger)
        *logger << "\t[0] '" << fname << "' MATCH" << std::endl;
      return fname;
    } else {
      if (logger)
        *logger << "\t[ ] '" << fname << "'" << std::endl;
    }
  }

  if (logger)
    *logger << "\t    *file not found*" << std::endl;

  return std::string("");
}

// ############################################################
// STATIC METHODS
// ############################################################

std::string VistaSystem::GetSystemSectionName() {
  return g_SystemSectionName;
}

VistaKeyboardSystemControl* VistaSystem::GetKeyboardSystemControl() const {
  return m_pKeyboardSystemControl;
}

VdfnObjectRegistry* VistaSystem::GetDfnObjectRegistry() const {
  return m_pDfnObjects;
}

bool VistaSystem::LoadDriverPlugin(const std::string& sDriverType, const std::string& sPluginName,
    const std::string& sTranscoderName) {
#if defined(VISTAKERNELSTATIC)
  return false;
#else
  vstr::outi() << "Loading plugins for driver type [" << sDriverType << "]" << std::endl;
  vstr::IndentObject oIndent;

  // we will later adjust the library path, so we save it so that it can be restored in the end
  std::string sCurrentPath = VistaEnvironment::GetLibraryPathEnv();

  std::string sPluginFilename, sTranscoderFilename;
  std::string sPluginPattern = "Vista*Plugin";
  std::string sTranscoderPattern = "Vista*Transcoder";

  if (sPluginName.empty()) {
    // derive from driver type
    sPluginFilename = "Vista" + sDriverType + "Plugin";
  } else {
    if (sPluginName.substr(0, 5) != "Vista")
      sPluginFilename = "Vista" + sPluginName;
    else
      sPluginFilename = sPluginName;
  }

  if (sTranscoderName.empty()) {
    // derive from driver type
    sTranscoderFilename = "Vista" + sDriverType + "Transcoder";
  } else {
    if (sTranscoderName.substr(0, 5) != "Vista")
      sTranscoderFilename = "Vista" + sTranscoderName;
    else
      sTranscoderFilename = sTranscoderName;
  }

#ifdef DEBUG
  sPluginFilename += "D";
  sTranscoderFilename += "D";
  sPluginPattern += "D";
  sTranscoderPattern += "D";
#endif
#ifdef WIN32
  sPluginFilename += ".dll";
  sTranscoderFilename += ".dll";
  sPluginPattern += ".dll";
  sTranscoderPattern += ".dll";
#else
  sPluginFilename     = "lib" + sPluginFilename + ".so";
  sTranscoderFilename = "lib" + sTranscoderFilename + ".so";
  sPluginPattern      = "lib" + sPluginPattern + ".so";
  sTranscoderPattern  = "lib" + sTranscoderPattern + ".so";
#endif

  std::string sActualPluginFile, sActualTranscoderFile;

  // we support multiple plugin directories
  for (std::list<std::string>::const_iterator itDirectory = m_liDriverPluginPathes.begin();
       itDirectory != m_liDriverPluginPathes.end(); ++itDirectory) {
    VistaEnvironment::AddPathToLibraryPathEnv((*itDirectory));
    VistaFileSystemDirectory oDirectory((*itDirectory));
    oDirectory.SetPattern(sPluginPattern);

    // we cant just check for the existance of the file, since the filename
    // may have different casing - we have to check all Vista'.dll/.so files
    // manually and do a case-insensitive comparison
    for (VistaFileSystemDirectory::const_iterator itFile = oDirectory.begin();
         itFile != oDirectory.end(); ++itFile) {
      if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
              (*itFile)->GetLocalName(), sPluginFilename)) {
        sActualPluginFile = (*itFile)->GetName();
        break;
      }
    } // for .dll file in plugin folder

    VistaFileSystemDirectory oTransDirectory((*itDirectory));
    oTransDirectory.SetPattern(sTranscoderPattern);

    // we can't just check for the existence of the file, since the filename
    // may have different casing - we have to check all Vista'.dll/.so files
    // manually and do a case-insensitive comparison
    for (VistaFileSystemDirectory::const_iterator itFile = oTransDirectory.begin();
         itFile != oTransDirectory.end(); ++itFile) {
      if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
              (*itFile)->GetLocalName(), sTranscoderFilename)) {
        sActualTranscoderFile = (*itFile)->GetName();
        break;
      }
    } // for .dll file in plugin folder

  } // for plugin dir

  if (sActualPluginFile.empty()) {
    vstr::warnp() << "Cannot find Driver Plugin file [" << sPluginFilename << "]" << std::endl;
    return false;
  }
  if (sActualTranscoderFile.empty()) {
    vstr::warnp() << "Cannot find Driver Plugin file [" << sTranscoderFilename << "]" << std::endl;
    return false;
  }

  VddUtil::VistaDriverPlugin oPlugin;

  if (VddUtil::LoadTranscoderFromPlugin(sActualTranscoderFile, &oPlugin) == false) {

    vstr::warnp() << "[VistaSystem]: Loading of driver plugin [" << sActualTranscoderFile
                  << "] failed." << std::endl;
    VddUtil::DisposePlugin(&oPlugin);
    return false;
  }

  if (VddUtil::LoadCreationMethodFromPlugin(sActualPluginFile, &oPlugin) == false) {
    vstr::warnp() << "[VistaSystem]: Loading of plugin [" << sActualPluginFile << "] failed."
                  << std::endl;
    VddUtil::DisposePlugin(&oPlugin);
    return false;
  }

  VistaDriverMap* pDriverMap = GetDriverMap();
  if (oPlugin.m_strDriverClassName.empty()) {
    vstr::warnp() << "[VistaSystem]: Loading of plugin for driver [" << sDriverType
                  << "] failed - plugin loaded, but has no name" << std::endl;
    VddUtil::DisposePlugin(&oPlugin);
    return false;
  }

  if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
          oPlugin.m_strDriverClassName, sDriverType) == false) {
    vstr::warnp() << "[VistaSystem]: Loaded plugin for driver [" << sDriverType
                  << "], but plugin reported driver name [" << oPlugin.m_strDriverClassName << "]"
                  << std::endl;
  }

  if (pDriverMap->GetDriverCreationMethod(oPlugin.m_strDriverClassName)) {
    vstr::warnp() << "[VistaSystem]: Loading of plugin for driver [" << sDriverType
                  << "] failed - plugin loaded, has no driver creation method" << std::endl;
    VddUtil::DisposePlugin(&oPlugin);
    return false;
  }

  vstr::outi() << "Registering method for devices of class [" << sDriverType << "]" << std::endl;

  m_pDllHlp->m_liDevices.push_back(oPlugin);
  pDriverMap->RegisterDriverCreationMethod(oPlugin.m_strDriverClassName, oPlugin.m_pMethod);

  // restore library path
  VistaEnvironment::SetLibraryPathEnv(sCurrentPath);

  return true;
#endif // no static build
}

bool VistaSystem::SetupOutputStreams() {
  vstr::GetStreamManager()->SetInfoInterface(new VistaKernelStreamInfoInterface(this));

  std::string sOutputConfigSection;
  if (m_oClusterConfig.GetValueInSubList("OUTPUT", m_sClusterNodeName, sOutputConfigSection)) {
    if (m_oClusterConfig.HasSubList(sOutputConfigSection) == false) {
      vstr::warnp() << "Output configuration section [" << sOutputConfigSection
                    << "] requested, but does not exist" << std::endl;
      return false;
    }
    return VistaStreams::CreateStreamsFromProplist(
        m_oClusterConfig.GetSubListConstRef(sOutputConfigSection), m_sClusterNodeName,
        m_bAllowStreamColors);
  } else if (m_oVistaConfig.GetValueInSubList(
                 "OUTPUT", GetSystemSectionName(), sOutputConfigSection)) {
    if (m_oVistaConfig.HasSubList(sOutputConfigSection) == false) {
      vstr::warnp() << "Output configuration section [" << sOutputConfigSection
                    << "] requested, but does not exist" << std::endl;
      return false;
    }
    return VistaStreams::CreateStreamsFromProplist(
        m_oVistaConfig.GetSubListConstRef(sOutputConfigSection), m_sClusterNodeName,
        m_bAllowStreamColors);
  }
  return true;
}

VistaFramerateDisplay* VistaSystem::GetFramerateDisplay() const {
  return m_pFramerateDisplay;
}

void VistaSystem::CreateFramerateDisplay() {
  std::string sFramerateSection;
  if (m_oGraphicsConfig.GetValueInSubList("FRAMERATE_DISPLAY", "GRAPHICS", sFramerateSection)) {
    if (m_oGraphicsConfig.HasSubList(sFramerateSection) == false) {
      vstr::warnp() << "Could not find section [" << sFramerateSection
                    << "] containing the framerate display config" << std::endl;
      m_pFramerateDisplay = new VistaFramerateDisplay(m_pDisplayManager, m_pFrameLoop);
    } else {
      m_pFramerateDisplay = new VistaFramerateDisplay(
          m_pDisplayManager, m_pFrameLoop, m_oGraphicsConfig.GetSubListConstRef(sFramerateSection));
    }
  } else {
    m_pFramerateDisplay = new VistaFramerateDisplay(m_pDisplayManager, m_pFrameLoop);
  }
  m_pEventManager->RegisterObserver(m_pFramerateDisplay, VistaSystemEvent::GetTypeId());
}

double VistaSystem::GetStartUpTime() const {
  return m_dStartClock;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
std::ostream& operator<<(std::ostream& oOut, const VistaSystem& oVistaSystem) {
  oVistaSystem.Debug(oOut);
  return oOut;
}
