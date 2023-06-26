/*============================================================================*/
/*                    ViSTA VR toolkit - OpenCV2 driver                  */
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

#include "VistaOpenCVCaptureDriver.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <cassert>
#include <string.h>

#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv/highgui.h>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

REFL_IMPLEMENT_FULL(
    VistaOpenCVCaptureDriver::Parameters, VistaDriverGenericParameterAspect::IParameterContainer);

VistaOpenCVCaptureDriverCreationMethod::VistaOpenCVCaptureDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType("TYPE",
      sizeof(VistaOpenCVCaptureDriver::_videotype), // default block size for one slot is 512 bytes
      1, // we do not expect this to be updated frequently for this device
      fac->CreateFactoryForType("TYPE"));
  RegisterSensorType("VIDEO",
      921600, // 800x600x3 bytes (assume RGB8 format)
      60,     // make it 60Hz per second
      fac->CreateFactoryForType("VIDEO"));
}

IVistaDeviceDriver* VistaOpenCVCaptureDriverCreationMethod::CreateDriver() {
  return new VistaOpenCVCaptureDriver(this);
}

namespace {
class AllocateBufferPointer : public IVistaDeviceDriver::AllocMemoryFunctor {
 public:
  virtual ~AllocateBufferPointer() {
  }

  void operator()(VistaSensorMeasure& pM) const {
    VistaOpenCVCaptureDriver::FrameType* ft = pM.getWrite<VistaOpenCVCaptureDriver::FrameType>();
    ft->m_Frame                             = new cv::Mat;
  }
};

class DeallocateBufferPointer : public IVistaDeviceDriver::ClaimMemoryFunctor {
 public:
  virtual ~DeallocateBufferPointer() {
  }

  void operator()(VistaSensorMeasure& pM) const {
    VistaOpenCVCaptureDriver::FrameType* ft = pM.getWrite<VistaOpenCVCaptureDriver::FrameType>();
    delete ft->m_Frame;
    ft->m_Frame = NULL;
  }
};
} // namespace

// #############################################################################
// PARAMETER CREATE
// #############################################################################

typedef TParameterCreate<VistaOpenCVCaptureDriver, VistaOpenCVCaptureDriver::Parameters>
    paramCreator;

template <class T>
bool propChange(VistaOpenCVCaptureDriver* driver, IVistaReflectionable* props, T& value,
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
    new TVistaPropertyGet<int, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_INT>(
        "CAPTUREDEVICE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetCaptureDevice,
        "returns the current OpenCV capture device index."),
    new TVistaPropertyGet<int, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_INT>(
        "REQUESTEDWIDTH",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetWidthRequested,
        "returns the requested width for capture."),
    new TVistaPropertyGet<int, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_INT>(
        "REQUESTEDHEIGHT",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetHeightRequested,
        "returns the requested height for capture."),
    new TVistaPropertyGet<int, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_INT>(
        "WIDTH",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetWidth,
        "returns the delivered width for capture."),
    new TVistaPropertyGet<int, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_INT>(
        "HEIGHT",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetHeight,
        "returns the delivered height for capture."),
    new TVistaPropertyGet<int, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_INT>(
        "FORMAT",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetFormat,
        "returns the format mask for the capture."),
    new TVistaPropertyGet<int, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_INT>(
        "FRAMERATE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetFrameRate,
        "returns the frame-rate for the capture."),
    new TVistaPropertyGet<std::string, VistaOpenCVCaptureDriver::Parameters,
        VistaProperty::PROPT_STRING>("FROMFILE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetFromFile,
        "returns the set from-file to capture from (empty string means: no file set)"),
    new TVistaPropertyGet<bool, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_BOOL>(
        "DOESHWRGB",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetDoesHW2RGB,
        "returns whether the device delivers RGB directly from the hardware."),
    new TVistaPropertyGet<bool, VistaOpenCVCaptureDriver::Parameters, VistaProperty::PROPT_BOOL>(
        "WANTRGB",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVCaptureDriver::Parameters::GetWantRGB,
        "returns whether the user wants RGB values as output (may decrease performance)."),

    NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {
    new TVistaPropertySet<int, int, VistaOpenCVCaptureDriver::Parameters>("CAPTUREDEVICE",
        SsReflectionName, &VistaOpenCVCaptureDriver::Parameters::SetCaptureDevice,
        "sets the current OpenCV capture device index (causes reset and loss of data)"),
    new TVistaPropertySet<int, int, VistaOpenCVCaptureDriver::Parameters>("REQUESTEDWIDTH",
        SsReflectionName, &VistaOpenCVCaptureDriver::Parameters::SetWidthRequested,
        "sets the width (requested) for OpenCV capture (can cause reset and loss of data)"),
    new TVistaPropertySet<int, int, VistaOpenCVCaptureDriver::Parameters>("REQUESTEDHEIGHT",
        SsReflectionName, &VistaOpenCVCaptureDriver::Parameters::SetHeightRequested,
        "sets the height (requested) for OpenCV capture (can cause reset and loss of data)"),
    new TVistaPropertySet<const std::string&, std::string, VistaOpenCVCaptureDriver::Parameters>(
        "FROMFILE", SsReflectionName, &VistaOpenCVCaptureDriver::Parameters::SetFromFile,
        "sets the current from file to capture from."),
    new TVistaPropertySet<bool, bool, VistaOpenCVCaptureDriver::Parameters>("WANTRGB",
        SsReflectionName, &VistaOpenCVCaptureDriver::Parameters::SetWantRGB,
        "sets whether the output data is forced to be RGB."),
    new TVistaPropertySet<int, int, VistaOpenCVCaptureDriver::Parameters>("FORMAT",
        SsReflectionName, &VistaOpenCVCaptureDriver::Parameters::SetFormat,
        "sets the format for the capture device. See opencv docs on CV_CAP_PROP_FORMAT."),

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

VistaOpenCVCaptureDriver::Parameters::Parameters(VistaOpenCVCaptureDriver* pDriver)

    : VistaDriverGenericParameterAspect::IParameterContainer()
    , m_parent(pDriver)
    , m_device(0)
    , m_widthRQ(640)
    , m_heightRQ(480)
    , m_width(640)
    , m_height(480)
    , m_bDef(false)
    , m_format(0)
    , m_framerate(0)
    , m_doesHw2RGB(false)
    , m_bwantRGB(false) {
}

int VistaOpenCVCaptureDriver::Parameters::GetCaptureDevice() const {
  return m_device;
}

bool VistaOpenCVCaptureDriver::Parameters::SetCaptureDevice(int device) {
  return propChange<int>(m_parent, this, m_device, device, MSG_CAPTUREDEVICE_CHG, m_bDef);
}

int VistaOpenCVCaptureDriver::Parameters::GetWidthRequested() const {
  return m_widthRQ;
}

bool VistaOpenCVCaptureDriver::Parameters::SetWidthRequested(int width) {
  return propChange<int>(m_parent, this, m_widthRQ, width, MSG_REQUESTEDWIDTH_CHG, m_bDef);
}

int VistaOpenCVCaptureDriver::Parameters::GetHeightRequested() const {
  return m_heightRQ;
}

bool VistaOpenCVCaptureDriver::Parameters::SetHeightRequested(int height) {
  return propChange<int>(m_parent, this, m_heightRQ, height, MSG_REQUESTEDHEIGHT_CHG, m_bDef);
}

int VistaOpenCVCaptureDriver::Parameters::GetWidth() const {
  return m_width;
}

bool VistaOpenCVCaptureDriver::Parameters::SetWidth(int width) {
  return propChange<int>(m_parent, this, m_width, width, MSG_WIDTH_CHG, m_bDef);
}

int VistaOpenCVCaptureDriver::Parameters::GetHeight() const {
  return m_height;
}

bool VistaOpenCVCaptureDriver::Parameters::SetHeight(int height) {
  return propChange<int>(m_parent, this, m_height, height, MSG_HEIGHT_CHG, m_bDef);
}

size_t VistaOpenCVCaptureDriver::Parameters::GetMemorySize() const {
  return GetWidth() * GetHeight() * 3; // for now: assume either BRG / RGB
}

bool VistaOpenCVCaptureDriver::Parameters::TurnDef(bool def) {
  m_bDef = def;
  return !m_bDef;
}

bool VistaOpenCVCaptureDriver::Parameters::GetIsDef() const {
  return m_bDef;
}

bool VistaOpenCVCaptureDriver::Parameters::Apply() {
  if (m_bDef)
    return false;
  m_parent->SignalPropertyChanged(IVistaObserveable::MSG_PROBABLY_ALL_CHANGED);
  return true;
}

int VistaOpenCVCaptureDriver::Parameters::GetFormat() const {
  return m_format;
}

bool VistaOpenCVCaptureDriver::Parameters::SetFormat(int format) {
  return propChange<int>(m_parent, this, m_format, format, MSG_FORMAT_CHG, m_bDef);
}

int VistaOpenCVCaptureDriver::Parameters::GetFrameRate() const {
  return m_framerate;
}

bool VistaOpenCVCaptureDriver::Parameters::SetFrameRate(int framerate) {
  return propChange<int>(m_parent, this, m_framerate, framerate, MSG_FRAMERATE_CHG, m_bDef);
}

bool VistaOpenCVCaptureDriver::Parameters::GetDoesHW2RGB() const {
  return m_doesHw2RGB;
}

bool VistaOpenCVCaptureDriver::Parameters::SetDoesHW2RGB(bool does) {
  return propChange<bool>(m_parent, this, m_doesHw2RGB, does, MSG_DOESHW2RGB_CHG, m_bDef);
}

std::string VistaOpenCVCaptureDriver::Parameters::GetFromFile() const {
  return m_fromFile;
}

bool VistaOpenCVCaptureDriver::Parameters::SetFromFile(const std::string& fromfile) {
  if (propChange<std::string>(m_parent, this, m_fromFile, fromfile, MSG_FROMFILE_CHG, m_bDef)) {
    if (m_fromFile.empty())
      SetCaptureDevice(0);
    else
      SetCaptureDevice(-1);
    return true;
  }
  return false;
}

bool VistaOpenCVCaptureDriver::Parameters::GetWantRGB() const {
  return m_bwantRGB;
}

bool VistaOpenCVCaptureDriver::Parameters::SetWantRGB(bool bwantrgb) {
  return propChange<bool>(m_parent, this, m_bwantRGB, bwantrgb, MSG_WANTRGB_CHG, m_bDef);
}

// #############################################################################
// MEASURES
// #############################################################################

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

typedef TVistaDriverEnableAspect<VistaOpenCVCaptureDriver> VistaOpenCVCaptureEnableAspect;

VistaOpenCVCaptureDriver::VistaOpenCVCaptureDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pInfo(new VistaDriverInfoAspect)
    , m_pThread(NULL)
    , m_pCapture(NULL)
    , m_alloc(new AllocateBufferPointer)
    , m_dealloc(new DeallocateBufferPointer) {
  m_pThread = new VistaDriverThreadAspect(this);

  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  RegisterAspect(m_pInfo);
  RegisterAspect(m_pThread);

  m_pParams = new VistaDriverGenericParameterAspect(new paramCreator(this));
  RegisterAspect(m_pParams);

  RegisterAspect(
      new VistaOpenCVCaptureEnableAspect(this, &VistaOpenCVCaptureDriver::PhysicalEnable));
  Parameters* p = m_pParams->GetParameter<Parameters>();
  p->TurnDef(true);
}

VistaOpenCVCaptureDriver::~VistaOpenCVCaptureDriver() {
  UnregisterAspect(m_pThread, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pThread;

  UnregisterAspect(m_pInfo, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pInfo;

  UnregisterAspect(m_pParams, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pParams;

  delete m_alloc;
  delete m_dealloc;

  VistaOpenCVCaptureEnableAspect* enabler =
      GetAspectAs<VistaOpenCVCaptureEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaOpenCVCaptureDriver::DoConnect() {
  if (m_pCapture)
    return true;

  OnReconnectCamera(true);

  if (!m_pCapture) {
    delete m_pCapture;
    m_pCapture = NULL;
    m_pInfo->GetInfoPropsWrite().SetValue("STATE", "ERROR");
    return false;
  } else
    m_pInfo->GetInfoPropsWrite().SetValue("STATE", "OK");

  // sensor 0
  VistaDeviceSensor* pSensor = GetSensorByIndex(0);
  if (!pSensor) {
    pSensor = new VistaDeviceSensor;
    pSensor->SetMeasureTranscode(
        GetFactory()->GetTranscoderFactoryForSensor("VIDEO")->CreateTranscoder());
    pSensor->SetTypeHint("VIDEO");

    AddDeviceSensor(pSensor, 0, m_alloc, m_dealloc);
  } else // sensor given from the outside?
  {
    VistaDriverMeasureHistoryAspect* history = GetAspect<VistaDriverMeasureHistoryAspect>();
    history->SetAllocAndClaimFunctor(pSensor, m_alloc, m_dealloc);
  }

  IVistaDeviceDriver::SensorInfo info = GetSensorInfo(0);
  if (info.IsValid()) {
    SetupSensorHistory(pSensor, (unsigned int)info.m_sensorHistorySize, info.m_MaxAccessTimeInMs);
    pSensor->SetSensorName(info.m_sensorName);
  }

  // sensor 1
  pSensor = GetSensorByIndex(1);
  if (!pSensor) {
    pSensor = new VistaDeviceSensor;
    pSensor->SetMeasureTranscode(
        GetFactory()->GetTranscoderFactoryForSensor("TYPE")->CreateTranscoder());
    pSensor->SetTypeHint("TYPE");
    AddDeviceSensor(pSensor, 1);
  }

  info = GetSensorInfo(1);
  if (info.IsValid()) {
    SetupSensorHistory(pSensor, (unsigned int)info.m_sensorHistorySize, info.m_MaxAccessTimeInMs);
    pSensor->SetSensorName(info.m_sensorName);
  }

  OnUpdateMeasureSize();

  VistaSensorMeasure* pM =
      MeasureStart(1, GetUpdateTimer().GetMicroTime(), IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (pM) {
    Parameters* p      = m_pParams->GetParameter<Parameters>();
    VideoType*  videoT = pM->getWrite<VideoType>();
    videoT->m_nFormat  = p->GetFormat();
    videoT->m_nWidth   = p->GetWidth();
    videoT->m_nHeight  = p->GetHeight();
    MeasureStop(1); // close this block
  }

  Parameters* p = m_pParams->GetParameter<Parameters>();
  p->TurnDef(false);
  return true;
}

bool VistaOpenCVCaptureDriver::DoDisconnect() {
  if (!m_pCapture)
    return true;

  VistaDeviceSensor* pSensor = GetSensorByIndex(1);
  GetFactory()->GetTranscoderFactoryForSensor("TYPE")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  pSensor = GetSensorByIndex(0);
  GetFactory()->GetTranscoderFactoryForSensor("VIDEO")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  delete m_pCapture;
  m_pCapture = NULL;

  m_pInfo->GetInfoPropsWrite().clear();
  m_pInfo->GetInfoPropsWrite().SetValue<std::string>("STATE", "DISCONNECTED");

  Parameters* p = m_pParams->GetParameter<Parameters>();
  p->TurnDef(true);

  return true;
}

bool VistaOpenCVCaptureDriver::PhysicalEnable(bool bEnable) {
  if (!m_pCapture)
    return true;

  if (bEnable) {
    if (m_pThread->GetIsProcessing())
      return true;

    Parameters* p = m_pParams->GetParameter<Parameters>();

    if (m_pCapture && !m_pCapture->isOpened())
      m_pCapture->open(p->GetCaptureDevice());

    if (GetIsConnected())
      m_pThread->UnpauseProcessing();
  } else {
    m_pThread->PauseProcessing();
    if (m_pCapture->isOpened())
      m_pCapture->release();
  }
  return bEnable; // we can not dis-/enable it
}

bool VistaOpenCVCaptureDriver::DoSensorUpdate(VistaType::microtime nTs) {
  if (!m_pCapture)
    return false;

  VistaSensorMeasure* pM = MeasureStart(0, nTs, IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (pM) {
    // get the current place for the decoding for sensor 0
    FrameType* frameT = pM->getWrite<FrameType>();

    // capture frame to temporary matrix (will reference the video frame data
    // from the driver, so this is a non-costly operation)
    cv::Mat t;
    (*m_pCapture) >> t;

    // now make a full deep copy, as otherwise, all measures will point
    // to the same video data that is constantly updated
    *frameT->m_Frame = t.clone();

    // check for end-of-stream
    if ((*frameT).m_Frame->data != NULL) {
      // can be null in case of file-stream
      MeasureStop(0); // close this block
    }
  }
  return true;
}

void VistaOpenCVCaptureDriver::SignalPropertyChanged(int msg) {
  switch (msg) {
  case Parameters::MSG_CAPTUREDEVICE_CHG:
  case Parameters::MSG_REQUESTEDWIDTH_CHG:
  case Parameters::MSG_REQUESTEDHEIGHT_CHG:
  case Parameters::MSG_FROMFILE_CHG:
  case IVistaObserveable::MSG_PROBABLY_ALL_CHANGED: {
    bool bEnabled = GetIsEnabled();
    if (bEnabled)
      SetIsEnabled(false);
    OnReconnectCamera();
    OnUpdateMeasureSize();
    if (bEnabled)
      SetIsEnabled(true);
    break;
  }
  case Parameters::MSG_WIDTH_CHG:
  case Parameters::MSG_HEIGHT_CHG:
  case Parameters::MSG_FORMAT_CHG:
  case Parameters::MSG_FRAMERATE_CHG: {
    bool bEnabled = GetIsEnabled();
    if (bEnabled)
      SetIsEnabled(false);
    OnUpdateMeasureSize();
    if (bEnabled)
      SetIsEnabled(true);
    break;
  }
  }
}

void VistaOpenCVCaptureDriver::OnReconnectCamera(bool bForce) {
  if (!bForce && !GetIsConnected())
    return;

  delete m_pCapture;
  m_pCapture = NULL;

  Parameters* p = m_pParams->GetParameter<Parameters>();

  bool bOldState = !p->TurnDef(true);

  std::string fromfile = p->GetFromFile();

  if (fromfile.empty())
    m_pCapture = new cv::VideoCapture(p->GetCaptureDevice());
  else
    m_pCapture = new cv::VideoCapture(fromfile);

  if (!m_pCapture->isOpened()) {
    delete m_pCapture;
    m_pCapture = NULL;
    return;
  }

  if (m_pCapture->set(CV_CAP_PROP_CONVERT_RGB, 1) == false)
    p->SetDoesHW2RGB(false);
  else
    p->SetDoesHW2RGB(true);

  if (p->GetFormat() != 0) {
    if ((*m_pCapture).set(CV_CAP_PROP_FORMAT, p->GetFormat()) == false)
      p->SetFormat((*m_pCapture).get(CV_CAP_PROP_FORMAT));
  } else
    p->SetFormat((*m_pCapture).get(CV_CAP_PROP_FORMAT));

  if (p->GetFrameRate() != 0) {
    if ((*m_pCapture).set(CV_CAP_PROP_FPS, p->GetFrameRate()) == false)
      p->SetFrameRate((int)(*m_pCapture).get(CV_CAP_PROP_FPS));
  } else
    p->SetFrameRate((int)(*m_pCapture).get(CV_CAP_PROP_FPS));

  (*m_pCapture).set(CV_CAP_PROP_FRAME_WIDTH, p->GetWidthRequested());
  (*m_pCapture).set(CV_CAP_PROP_FRAME_HEIGHT, p->GetHeightRequested());

  p->SetWidth((int)(*m_pCapture).get(CV_CAP_PROP_FRAME_WIDTH));
  p->SetHeight((int)(*m_pCapture).get(CV_CAP_PROP_FRAME_HEIGHT));

  if (p->GetWidth() == 0 || p->GetHeight() == 0) {
    vstr::err() << "Could not set frame dimensions. OpenCV reported (" << p->GetWidth() << ", "
                << p->GetHeight() << ") as dimensions";
    delete m_pCapture;
    m_pCapture = 0;
    return;
  }
  m_pInfo->GetInfoPropsWrite().SetValue("REALWIDTH", p->GetWidth());
  m_pInfo->GetInfoPropsWrite().SetValue("REALHEIGHT", p->GetHeight());
  m_pInfo->GetInfoPropsWrite().SetValue("FPS", p->GetFrameRate());
  m_pInfo->GetInfoPropsWrite().SetValue("FORMAT", p->GetFormat());

  VistaSensorMeasure* pM =
      MeasureStart(1, GetUpdateTimer().GetMicroTime(), IVistaDeviceDriver::RETURN_CURRENT_SLOT);
  if (pM) {
    VideoType* videoT = pM->getWrite<VideoType>();
    videoT->m_nFormat = p->GetFormat();
    videoT->m_nWidth  = p->GetWidth();
    videoT->m_nHeight = p->GetHeight();
    MeasureStop(1); // close this block
  }

  p->TurnDef(bOldState);
}

void VistaOpenCVCaptureDriver::OnUpdateMeasureSize() {
  VistaDeviceSensor* pSensor = GetSensorByIndex(0);
  if (!pSensor) // could be called during connect, i.e. before any sensor is registered
    return;

  const VistaMeasureHistory& history = pSensor->GetMeasures();
  assert(m_pParams->GetParameter<Parameters>()->GetMemorySize() > 0);

  // here we assume that client read size and driver write size are already pre-given (on user
  // demand) for a 'good' access of the history (meaning: the temporal aspects are OK, its just the
  // size per measure that changes). This operation will flush all old contents currently in the
  // history.
  m_pHistoryAspect->SetHistorySize(pSensor, history.m_nClientReadSize, history.m_nDriverWriteSize,
      // this is the line that will change the memory
      (unsigned int)m_pParams->GetParameter<Parameters>()->GetMemorySize());
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/
