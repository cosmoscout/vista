/*============================================================================*/
/*               ViSTA VR toolkit - DepthSense 3D Camera Driver               */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/

#include <cassert>
#include <cstring>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <VistaAspects/VistaAspectsUtils.h>

#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <DepthSense.hxx>

#include "VistaDepthSenseDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
REFL_IMPLEMENT_FULL(VistaDepthSenseDriver::DepthSenseParameters,
    VistaDriverGenericParameterAspect::IParameterContainer);

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {
using DepthSense::ColorNode;
using DepthSense::Context;
using DepthSense::DepthNode;
using DepthSense::Device;
using DepthSense::Node;

const char* g_sLogPrefix = "[DepthSenseDriver] ";

std::string FrameFormatToString(DepthSense::FrameFormat format) {
  std::string sFrameFormat;

  switch (format) {
  case DepthSense::FRAME_FORMAT_UNKNOWN:
    sFrameFormat = "unknown";
    break;
  case DepthSense::FRAME_FORMAT_QQVGA:
    sFrameFormat = "QQVGA (160x120)";
    break;
  case DepthSense::FRAME_FORMAT_QCIF:
    sFrameFormat = "QCIF (176x144)";
    break;
  case DepthSense::FRAME_FORMAT_HQVGA:
    sFrameFormat = "HQVGA (240x160)";
    break;
  case DepthSense::FRAME_FORMAT_QVGA:
    sFrameFormat = "QVGA (320x240)";
    break;
  case DepthSense::FRAME_FORMAT_CIF:
    sFrameFormat = "CIF (352x288)";
    break;
  case DepthSense::FRAME_FORMAT_HVGA:
    sFrameFormat = "HVGA (480x320)";
    break;
  case DepthSense::FRAME_FORMAT_VGA:
    sFrameFormat = "VGA (640x480)";
    break;
  case DepthSense::FRAME_FORMAT_WXGA_H:
    sFrameFormat = "WXGA_H (1280x720)";
    break;
  case DepthSense::FRAME_FORMAT_DS311:
    sFrameFormat = "DS311 (320x120)";
    break;
  case DepthSense::FRAME_FORMAT_XGA:
    sFrameFormat = "XGA (1024x768)";
    break;
  case DepthSense::FRAME_FORMAT_SVGA:
    sFrameFormat = "SVGA (800x600)";
    break;
  case DepthSense::FRAME_FORMAT_OVVGA:
    sFrameFormat = "OVVGA (636x480)";
    break;
  case DepthSense::FRAME_FORMAT_WHVGA:
    sFrameFormat = "WHVGA (640x240)";
    break;
  case DepthSense::FRAME_FORMAT_NHD:
    sFrameFormat = "nHD (640x360)";
    break;
  }

  return sFrameFormat;
}

void PrintColorNodeConfig(ColorNode::Configuration conf) {
  std::string sCompression;
  std::string sPowerLineFreq;

  switch (conf.compression) {
  case DepthSense::COMPRESSION_TYPE_YUY2:
    sCompression = "Y'UV422";
    break;
  case DepthSense::COMPRESSION_TYPE_MJPEG:
    sCompression = "MJPEG";
    break;
  }

  switch (conf.powerLineFrequency) {
  case DepthSense::POWER_LINE_FREQUENCY_DISABLED:
    sPowerLineFreq = "disabled";
    break;

  case DepthSense::POWER_LINE_FREQUENCY_50HZ:
    sPowerLineFreq = "50 Hz";
    break;

  case DepthSense::POWER_LINE_FREQUENCY_60HZ:
    sPowerLineFreq = "60 Hz";
    break;
  }

  vstr::debugi() << "Compression:\t\t" << sCompression << std::endl;
  vstr::debugi() << "Frame Format:\t\t" << FrameFormatToString(conf.frameFormat) << std::endl;
  vstr::debugi() << "Frame Rate:\t\t" << conf.framerate << std::endl;
  vstr::debugi() << "Power Line Freq:\t" << sPowerLineFreq << std::endl;
}

void PrintColorNodeParams(ColorNode node) {
  vstr::debugi() << std::boolalpha << "WhiteBalanceAuto: " << node.getWhiteBalanceAuto()
                 << std::endl;
}

void PrintDepthNodeConfig(DepthNode::Configuration conf) {
  std::string sMode;

  switch (conf.mode) {
  case DepthSense::DepthNode::CAMERA_MODE_UNKNOWN:
    sMode = "Unknown";
    break;

  case DepthSense::DepthNode::CAMERA_MODE_CLOSE_MODE:
    sMode = "Close";
    break;

  case DepthSense::DepthNode::CAMERA_MODE_LONG_RANGE:
    sMode = "Long range";
    break;
  }

  vstr::debugi() << "Mode:\t\t" << sMode << std::endl;
  vstr::debugi() << "Frame Format:\t" << FrameFormatToString(conf.frameFormat) << std::endl;
  vstr::debugi() << "Frame Rate:\t" << conf.framerate << std::endl;
  vstr::debugi() << "Saturation:\t" << conf.saturation << std::endl;
}

void PrintDepthNodeParams(DepthNode node) {
  vstr::debugi() << std::boolalpha << "DepthDenoising: " << node.getEnableDenoising() << std::endl;
}

inline int clamp(int value) {
  value = value > 255 ? 255 : value;
  value = value < 0 ? 0 : value;
  return value;
}

inline void YUV444ToRGB888(const unsigned char* in, unsigned char* out) {
  int c = in[0] - 16;
  int d = in[1] - 128;
  int e = in[2] - 128;

  out[0] = clamp((298 * c + 409 * e + 128) >> 8);
  out[1] = clamp((298 * c - 100 * d - 208 * e + 128) >> 8);
  out[2] = clamp((298 * c + 516 * d + 128) >> 8);
}
} // namespace

struct DepthSensePrivate {
  DepthSense::Context   oContext;
  DepthSense::ColorNode oColorNode;
  DepthSense::DepthNode oDepthNode;
};

// #############################################################################
// DRIVER CREATE
// #############################################################################
VistaDepthSenseDriverCreationMethod::VistaDepthSenseDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType("COLOR",
      230400, // 320x240x3 bytes (RGB888 color format)
      30,     // frame rate
      fac->CreateFactoryForType("COLOR"));
  RegisterSensorType("DEPTH",
      153600, // 320x240x2 bytes (16 bit depth map)
      30,     // frame rate
      fac->CreateFactoryForType("DEPTH"));
  RegisterSensorType("UVMAP",
      614400, // 320x240x4 bytes (2 floats per depth pixel)
      30,     // frame rate
      fac->CreateFactoryForType("UVMAP"));
}

IVistaDeviceDriver* VistaDepthSenseDriverCreationMethod::CreateDriver() {
  return new VistaDepthSenseDriver(this);
}

// #############################################################################
// PARAMETER CREATE
// #############################################################################
typedef TParameterCreate<VistaDepthSenseDriver, VistaDepthSenseDriver::DepthSenseParameters>
    paramCreator;

template <class T>
bool propChange(VistaDepthSenseDriver* driver, IVistaReflectionable* props, T& value,
    const T& toSet, int msg, bool def) {
  if (value != toSet) {
    value = toSet;
    if (!def)
      driver->SignalPropertyChanged(msg);

    props->Notify(msg);
    return true;
  }

  return false;
}

namespace {
IVistaPropertyGetFunctor* SaParameterGetter[] = {
    new TVistaPropertyGet<bool, VistaDepthSenseDriver::DepthSenseParameters,
        VistaProperty::PROPT_BOOL>("WHITEBALANCE_AUTO", SsReflectionName,
        &VistaDepthSenseDriver::DepthSenseParameters::GetWhiteBalanceAuto,
        "Automatic white balance for color frames"),
    new TVistaPropertyGet<bool, VistaDepthSenseDriver::DepthSenseParameters,
        VistaProperty::PROPT_BOOL>("DUMP_AVAILABLE_CONFIGS", SsReflectionName,
        &VistaDepthSenseDriver::DepthSenseParameters::GetDumpAvailableConfigs,
        "Dump available camera configs to console upon Connect"),
    new TVistaPropertyGet<bool, VistaDepthSenseDriver::DepthSenseParameters,
        VistaProperty::PROPT_BOOL>("DEPTH_DENOISING", SsReflectionName,
        &VistaDepthSenseDriver::DepthSenseParameters::GetDepthDenoising, "Depth frame denoising"),
    NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {
    new TVistaPropertySet<bool, bool, VistaDepthSenseDriver::DepthSenseParameters>(
        "WHITEBALANCE_AUTO", SsReflectionName,
        &VistaDepthSenseDriver::DepthSenseParameters::SetWhiteBalanceAuto,
        "Automatic white balance for color frames"),
    new TVistaPropertySet<bool, bool, VistaDepthSenseDriver::DepthSenseParameters>(
        "DUMP_AVAILABLE_CONFIGS", SsReflectionName,
        &VistaDepthSenseDriver::DepthSenseParameters::SetDumpAvailableConfigs,
        "Dump available camera configs to console upon Connect"),
    new TVistaPropertySet<bool, bool, VistaDepthSenseDriver::DepthSenseParameters>(
        "DEPTH_DENOISING", SsReflectionName,
        &VistaDepthSenseDriver::DepthSenseParameters::SetDepthDenoising, "Depth frame denoising"),
    NULL};
} // namespace

#if !defined(WIN32)
static void releaseParameterProps() __attribute__((destructor));
#else
static void releaseParameterProps();
#endif

#if defined(WIN32)
#include <windows.h>
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    if (lpReserved == 0)
      releaseParameterProps();
    break;
  }
  return TRUE;
}
#endif

static void releaseParameterProps() {
  IVistaPropertyGetFunctor** git = SaParameterGetter;
  IVistaPropertySetFunctor** sit = SaParameterSetter;

  while (*git)
    delete *git++;

  while (*sit)
    delete *sit++;
}

VistaDepthSenseDriver::DepthSenseParameters::DepthSenseParameters(VistaDepthSenseDriver* pDriver)
    : VistaDriverGenericParameterAspect::IParameterContainer()
    , m_parent(pDriver)
    , m_bWhiteBalanceAuto(false)
    , m_bDumpAvailableConfigs(false)
    , m_bDepthDenoising(false) {
}

bool VistaDepthSenseDriver::DepthSenseParameters::GetWhiteBalanceAuto() const {
  return m_bWhiteBalanceAuto;
}
bool VistaDepthSenseDriver::DepthSenseParameters::SetWhiteBalanceAuto(bool bWhiteBalanceAuto) {
  if (m_bWhiteBalanceAuto == bWhiteBalanceAuto)
    return true;
  m_bWhiteBalanceAuto = bWhiteBalanceAuto;
  Notify(MSG_WHITEBALANCE_AUTO_CHG);
  return true;
}

bool VistaDepthSenseDriver::DepthSenseParameters::GetDumpAvailableConfigs() const {
  return m_bDumpAvailableConfigs;
}
bool VistaDepthSenseDriver::DepthSenseParameters::SetDumpAvailableConfigs(
    bool bDumpAvailableConfigs) {
  if (m_bDumpAvailableConfigs == bDumpAvailableConfigs)
    return true;
  m_bDumpAvailableConfigs = bDumpAvailableConfigs;
  Notify(MSG_DUMP_AVAILABLE_CONFIGS_CHG);
  return true;
}

bool VistaDepthSenseDriver::DepthSenseParameters::GetDepthDenoising() const {
  return m_bDepthDenoising;
}
bool VistaDepthSenseDriver::DepthSenseParameters::SetDepthDenoising(bool bDepthDenoising) {
  if (m_bDepthDenoising == bDepthDenoising)
    return true;
  m_bDepthDenoising = bDepthDenoising;
  Notify(MSG_DEPTH_DENOISING_CHG);
  return true;
}

// #############################################################################
// UPDATE THREAD
// #############################################################################
class VistaDepthSenseMeasurementThread : public VistaThread {
 public:
  VistaDepthSenseMeasurementThread(VistaDepthSenseDriver* pDriver)
      : VistaThread()
      , m_pDriver(pDriver)
      , m_pColorSensor(NULL)
      , m_pDepthSensor(NULL)
      , m_pUVMapSensor(NULL)
      , m_lasttime(0) {
  }

  virtual ~VistaDepthSenseMeasurementThread() {
  }

  void RegisterColorNode(ColorNode node) {
    vstr::debugi() << g_sLogPrefix << "Registering with color node" << std::endl;

    VistaDriverGenericParameterAspect* pParamAspect =
        m_pDriver->GetAspect<VistaDriverGenericParameterAspect>();

    VistaDepthSenseDriver::DepthSenseParameters* pParams =
        pParamAspect->GetParameter<VistaDepthSenseDriver::DepthSenseParameters>();

    m_pColorSensor = m_pDriver->GetSensorByName("COLOR");

    typedef std::vector<ColorNode::Configuration> VecConf;
    VecConf                                       vConfigs = node.getConfigurations();
    ColorNode::Configuration                      config   = node.getConfiguration();

    if (pParams->GetDumpAvailableConfigs())
      vstr::debugi() << "Supported color node configurations:" << std::endl;

    for (VecConf::const_iterator it = vConfigs.begin(); it != vConfigs.end(); ++it) {
      // @todo read values from driver config
      if (it->compression == DepthSense::COMPRESSION_TYPE_YUY2 &&
          it->frameFormat == DepthSense::FRAME_FORMAT_QVGA && it->framerate == 30 &&
          it->powerLineFrequency == DepthSense::POWER_LINE_FREQUENCY_DISABLED)
        config = *it;

      if (pParams->GetDumpAvailableConfigs())
        PrintColorNodeConfig(*it);
    }

    node.setConfiguration(config);
    node.setWhiteBalanceAuto(pParams->GetWhiteBalanceAuto());

    vstr::debugi() << "Active color node configuration:" << std::endl;
    PrintColorNodeConfig(config);
    PrintColorNodeParams(node);
    vstr::debugi() << std::endl;

    node.setEnableColorMap(true);
    node.newSampleReceivedEvent().connect(this, &VistaDepthSenseMeasurementThread::NewColorSample);
    m_pDriver->m_pPrivate->oContext.registerNode(node);
  }

  void UnRegisterColorNode(ColorNode node) {
    node.setEnableColorMap(false);
    node.newSampleReceivedEvent().disconnect(
        this, &VistaDepthSenseMeasurementThread::NewColorSample);
    m_pDriver->m_pPrivate->oContext.unregisterNode(node);
  }

  void RegisterDepthNode(DepthNode node) {
    vstr::debugi() << g_sLogPrefix << "Registering with depth node" << std::endl;

    VistaDriverGenericParameterAspect* pParamAspect =
        m_pDriver->GetAspect<VistaDriverGenericParameterAspect>();

    VistaDepthSenseDriver::DepthSenseParameters* pParams =
        pParamAspect->GetParameter<VistaDepthSenseDriver::DepthSenseParameters>();

    m_pDepthSensor = m_pDriver->GetSensorByName("DEPTH");
    m_pUVMapSensor = m_pDriver->GetSensorByName("UVMAP");

    typedef std::vector<DepthNode::Configuration> VecConf;
    VecConf                                       vConfigs = node.getConfigurations();
    DepthNode::Configuration                      config   = node.getConfiguration();

    if (pParams->GetDumpAvailableConfigs())
      vstr::debug() << "Supported depth node configurations:" << std::endl;
    for (VecConf::const_iterator it = vConfigs.begin(); it != vConfigs.end(); ++it) {
      if (it->mode == DepthSense::DepthNode::CAMERA_MODE_CLOSE_MODE &&
          it->frameFormat == DepthSense::FRAME_FORMAT_QVGA && it->framerate == 30 &&
          it->saturation == true)
        config = *it;

      if (pParams->GetDumpAvailableConfigs())
        PrintDepthNodeConfig(*it);
    }

    node.setConfiguration(config);

    node.setEnableDepthMap(true);
    node.setEnableUvMap(true);
    node.setEnableDenoising(pParams->GetDepthDenoising());

    vstr::debugi() << "Active depth node configuration:" << std::endl;
    PrintDepthNodeConfig(config);
    PrintDepthNodeParams(node);
    vstr::debugi() << std::endl;

    node.newSampleReceivedEvent().connect(this, &VistaDepthSenseMeasurementThread::NewDepthSample);
    m_pDriver->m_pPrivate->oContext.registerNode(node);
  }

  void UnRegisterDepthNode(DepthNode node) {
    node.setEnableDepthMap(false);
    node.setEnableUvMap(false);

    node.newSampleReceivedEvent().disconnect(
        this, &VistaDepthSenseMeasurementThread::NewDepthSample);
    m_pDriver->m_pPrivate->oContext.unregisterNode(node);
  }

  void NewColorSample(ColorNode obj, ColorNode::NewSampleReceivedData data) {
    VistaType::microtime mtStamp = VistaTimer::GetStandardTimer().GetMicroTime();

    // vstr::debug()
    // 	<< mtStamp - m_lasttime
    // 	<< std::endl;
    // m_lasttime = mtStamp;

    // we transcode here in the driver already, since mapping yuv
    // to rgb is a costly operation and makes no sense to perform
    // each time the data is accessed.
    // @todo: think about concept, check if transcodes do caching
    VistaSensorMeasure* pM =
        m_pDriver->MeasureStart(0, mtStamp, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
    VistaDepthSenseDriver::ColorMeasure* pMeasure =
        pM->getWrite<VistaDepthSenseDriver::ColorMeasure>();

    // we write directly to the measure memory on transcoding to
    // avoid another copy
    for (int block = 0; block < data.colorMap.size() / 4; block++) {
      unsigned char y0 = data.colorMap[4 * block + 0];
      unsigned char u  = data.colorMap[4 * block + 1];
      unsigned char y1 = data.colorMap[4 * block + 2];
      unsigned char v  = data.colorMap[4 * block + 3];

      unsigned char in[3];
      in[0] = y0;
      in[1] = u;
      in[2] = v;
      YUV444ToRGB888(in, &pMeasure->frame[6 * block]);

      in[0] = y1;
      YUV444ToRGB888(in, &pMeasure->frame[6 * block + 3]);
    }

    m_pDriver->MeasureStop(0);
    m_pColorSensor->SetUpdateTimeStamp(mtStamp);
  }

  void NewDepthSample(DepthNode obj, DepthNode::NewSampleReceivedData data) {
    VistaType::microtime mtStamp = VistaTimer::GetStandardTimer().GetMicroTime();

    VistaSensorMeasure* pMDepth =
        m_pDriver->MeasureStart(1, mtStamp, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
    VistaSensorMeasure* pMUVMap =
        m_pDriver->MeasureStart(2, mtStamp, IVistaDeviceDriver::RETURN_CURRENT_SLOT);

    VistaDepthSenseDriver::DepthMeasure* pDepthMeasure =
        pMDepth->getWrite<VistaDepthSenseDriver::DepthMeasure>();
    VistaDepthSenseDriver::UVMapMeasure* pUVMapMeasure =
        pMUVMap->getWrite<VistaDepthSenseDriver::UVMapMeasure>();

    // we write directly to the measure memory on transcoding to
    // avoid another copy
    // @todo make uv map measurement configurable
    for (int i = 0; i < data.depthMap.size(); i++) {
      pDepthMeasure->frame[i]         = data.depthMap[i];
      pUVMapMeasure->frame[2 * i]     = data.uvMap[i].u;
      pUVMapMeasure->frame[2 * i + 1] = data.uvMap[i].v;
    }

    m_pDriver->MeasureStop(1);
    m_pDriver->MeasureStop(2);
    m_pDepthSensor->SetUpdateTimeStamp(mtStamp);
    m_pUVMapSensor->SetUpdateTimeStamp(mtStamp);
  }

 protected:
  void ThreadBody() {
    vstr::debugi() << g_sLogPrefix << "Starting DepthSense event loop..." << std::endl;
    m_pDriver->m_pPrivate->oContext.run();
    vstr::debugi() << g_sLogPrefix << "DepthSense event loop finished" << std::endl;
  }

 private:
  VistaDepthSenseDriver* m_pDriver;
  VistaDeviceSensor*     m_pColorSensor;
  VistaDeviceSensor*     m_pDepthSensor;
  VistaDeviceSensor*     m_pUVMapSensor;

  VistaType::microtime m_lasttime;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

typedef TVistaDriverEnableAspect<VistaDepthSenseDriver> VistaDepthSenseEnableAspect;

VistaDepthSenseDriver::VistaDepthSenseDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pInfo(NULL)
    , m_pMeasureThread(0)
    , m_pPrivate(new DepthSensePrivate) {
  m_pMeasureThread = new VistaDepthSenseMeasurementThread(this);
  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  VistaDeviceSensor* pSensor = new VistaDeviceSensor;
  pSensor->SetMeasureTranscode(crm->GetTranscoderFactoryForSensor("COLOR")->CreateTranscoder());
  pSensor->SetTypeHint("COLOR");
  pSensor->SetSensorName("COLOR");
  AddDeviceSensor(pSensor);

  pSensor = new VistaDeviceSensor;
  pSensor->SetMeasureTranscode(crm->GetTranscoderFactoryForSensor("DEPTH")->CreateTranscoder());
  pSensor->SetTypeHint("DEPTH");
  pSensor->SetSensorName("DEPTH");
  AddDeviceSensor(pSensor);

  pSensor = new VistaDeviceSensor;
  pSensor->SetMeasureTranscode(crm->GetTranscoderFactoryForSensor("UVMAP")->CreateTranscoder());
  pSensor->SetTypeHint("UVMAP");
  pSensor->SetSensorName("UVMAP");
  AddDeviceSensor(pSensor);

  m_pInfo = new VistaDriverInfoAspect();
  RegisterAspect(m_pInfo);

  m_pParamAspect = new VistaDriverGenericParameterAspect(new paramCreator(this));
  RegisterAspect(m_pParamAspect);

  RegisterAspect(new VistaDepthSenseEnableAspect(this, &VistaDepthSenseDriver::PhysicalEnable));
}

VistaDepthSenseDriver::~VistaDepthSenseDriver() {
  VistaDeviceSensor* pSensor = GetSensorByName("COLOR");
  GetFactory()->GetTranscoderFactoryForSensor("COLOR")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  pSensor = GetSensorByName("DEPTH");
  GetFactory()->GetTranscoderFactoryForSensor("DEPTH")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  pSensor = GetSensorByName("UVMAP");
  GetFactory()->GetTranscoderFactoryForSensor("UVMAP")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  if (m_pMeasureThread)
    m_pMeasureThread->Join();

  delete m_pMeasureThread;

  UnregisterAspect(m_pInfo, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pInfo;

  UnregisterAspect(m_pParamAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pParamAspect;

  VistaDepthSenseEnableAspect* enabler = GetAspect<VistaDepthSenseEnableAspect>();
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaDepthSenseDriver::DoConnect() {
  vstr::debugi() << g_sLogPrefix << "CONNECT" << std::endl;

  m_pPrivate->oContext = Context::create("localhost");

  // @todo: dynamic device connections
  // g_context.deviceAddedEvent().connect(&onDeviceConnected);
  // g_context.deviceRemovedEvent().connect(&onDeviceDisconnected);

  // Get the list of currently connected devices
  std::vector<Device> vecDevices = m_pPrivate->oContext.getDevices();
  std::vector<Node>   vecNodes;
  Device              oDevice;

  // for now we are only interested in the first device
  if (vecDevices.size() >= 1) {
    oDevice  = vecDevices[0];
    vecNodes = oDevice.getNodes();

    vstr::outi() << g_sLogPrefix << "Found " << vecNodes.size() << " nodes on first device"
                 << std::endl;
  } else {
    vstr::erri() << g_sLogPrefix << "No DepthSense devices found. Connect failed!" << std::endl;
    return false;
  }

  for (std::vector<Node>::const_iterator nodeIter = vecNodes.begin(); nodeIter != vecNodes.end();
       nodeIter++) {
    Node node = *nodeIter;

    Node testNode;
    testNode = node.as<ColorNode>();
    if (testNode.isSet())
      m_pPrivate->oColorNode = testNode.as<ColorNode>();
    testNode = node.as<DepthNode>();
    if (testNode.isSet())
      m_pPrivate->oDepthNode = testNode.as<DepthNode>();
  }

  if (!m_pPrivate->oColorNode.isSet() || !m_pPrivate->oDepthNode.isSet()) {
    vstr::erri() << g_sLogPrefix << "Could not find Color and Depth nodes. Connect failed!"
                 << std::endl;
    return false;
  }

  try {
    m_pPrivate->oContext.requestControl(oDevice, 100);
  } catch (DepthSense::TimeoutException) {
    vstr::erri() << g_sLogPrefix << "Timeout requesting camera control. "
                 << "Connect failed!" << std::endl;
    return false;
  }
  m_pMeasureThread->RegisterDepthNode(m_pPrivate->oDepthNode);
  m_pMeasureThread->RegisterColorNode(m_pPrivate->oColorNode);
  m_pPrivate->oContext.releaseControl(oDevice);

  return true;
}

bool VistaDepthSenseDriver::DoDisconnect() {
  vstr::debugi() << g_sLogPrefix << "DISCONNECT" << std::endl;

  m_pMeasureThread->UnRegisterColorNode(m_pPrivate->oColorNode);
  m_pMeasureThread->UnRegisterDepthNode(m_pPrivate->oDepthNode);

  m_pPrivate->oColorNode.unset();
  m_pPrivate->oDepthNode.unset();
  m_pPrivate->oContext.unset();

  return true;
}

bool VistaDepthSenseDriver::PhysicalEnable(bool bEnable) {
  if (bEnable) {
    if (!GetIsEnabled()) {
      m_pPrivate->oContext.startNodes();
      m_pMeasureThread->Run();
    }
  } else {
    if (GetIsEnabled()) {
      m_pPrivate->oContext.quit();
      m_pMeasureThread->Join();
      m_pPrivate->oContext.stopNodes();
    }
  }
  return true;
}

bool VistaDepthSenseDriver::DoSensorUpdate(VistaType::microtime nTs) {
  return true;
}

void VistaDepthSenseDriver::SignalPropertyChanged(int msg) {
}
