
/*============================================================================*/
/*                    ViSTA VR toolkit - OpenAL1.1 driver                  */
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

#include "VistaOpenALRecordDriver.h"
#include "OpenALSoundContext.h"

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
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

REFL_IMPLEMENT_FULL(
    VistaOpenALRecordDriver::Parameters, VistaDriverGenericParameterAspect::IParameterContainer);

VistaOpenALRecordDriverCreationMethod::VistaOpenALRecordDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType("TYPE",
      sizeof(VistaOpenALRecordDriver::_audiotype), // default block size for one slot is 512 bytes
      1, // we do not expect this to be updated frequently for this device
      fac->CreateFactoryForType("TYPE"));
  RegisterSensorType("AUDIO",
      8820, // default block size for one slot is 2205(+1) bytes (assume 16bit mono / 44100 kHz ->
            // 100ms)
      400,  // 128bit/s is probably a very high rate, so exaggerate a bit...
      fac->CreateFactoryForType("AUDIO"));
}

IVistaDeviceDriver* VistaOpenALRecordDriverCreationMethod::CreateDriver() {
  return new VistaOpenALRecordDriver(this);
}

// #############################################################################
// PARAMETER CREATE
// #############################################################################

typedef TParameterCreate<VistaOpenALRecordDriver, VistaOpenALRecordDriver::Parameters> paramCreator;

template <class T>
bool propChange(VistaOpenALRecordDriver* driver, IVistaReflectionable* props, T& value,
    const T& toSet, int msg, bool def = false) {
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
std::string GetFormatStringFor(int format) {
  switch (format) {
  case AL_FORMAT_MONO8:
    return "MONO8";
  case AL_FORMAT_MONO16:
    return "MONO16";
  case AL_FORMAT_STEREO8:
    return "STEREO8";
  case AL_FORMAT_STEREO16:
    return "STEREO16";
  default:
    return "<UNKNOWN-FORMAT>";
  }
}

int GetNumberOfBytesPerSample(int format) {
  switch (format) {
  case AL_FORMAT_MONO8:
    return 1;
  case AL_FORMAT_MONO16:
  case AL_FORMAT_STEREO8:
    return 2;
  case AL_FORMAT_STEREO16:
    return 4;
  default:
    return 0;
  }
}

//	const std::string SsReflectionName("VistaOpenALRecordDriver::Parameters");

IVistaPropertyGetFunctor* SaParameterGetter[] = {
    new TVistaPropertyGet<int, VistaOpenALRecordDriver::Parameters, VistaProperty::PROPT_INT>(
        "MINBUFFERTIME",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenALRecordDriver::Parameters::GetMinimumBufferTime,
        "returns the minimum buffer time allowed for OpenAL capture."),
    new TVistaPropertyGet<std::string, VistaOpenALRecordDriver::Parameters,
        VistaProperty::PROPT_INT>("FORMAT", SsReflectionName,
        &VistaOpenALRecordDriver::Parameters::GetRecordingFormatString,
        "returns the current recording format."),
    new TVistaPropertyGet<int, VistaOpenALRecordDriver::Parameters, VistaProperty::PROPT_INT>(
        "FREQUENCY", SsReflectionName, &VistaOpenALRecordDriver::Parameters::GetRecordingFrequency,
        "returns the current recording frequency."),
    new TVistaPropertyGet<int, VistaOpenALRecordDriver::Parameters, VistaProperty::PROPT_INT>(
        "BUFFERTIME", SsReflectionName, &VistaOpenALRecordDriver::Parameters::GetBufferTime,
        "returns the current length of the capture buffer in ms."),
    new TVistaPropertyGet<int, VistaOpenALRecordDriver::Parameters, VistaProperty::PROPT_INT>(
        "SAMPLES", SsReflectionName, &VistaOpenALRecordDriver::Parameters::GetNumberOfSamples,
        "returns the current number of samples per measure."),
    NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {
    new TVistaPropertySet<int, int, VistaOpenALRecordDriver::Parameters>("MINBUFFERTIME",
        SsReflectionName, &VistaOpenALRecordDriver::Parameters::SetMinimumBufferTime,
        "set the minimum buffer time (>0) that can be used for OpenAL capture."),
    new TVistaPropertySet<int, int, VistaOpenALRecordDriver::Parameters>("FREQUENCY",
        SsReflectionName, &VistaOpenALRecordDriver::Parameters::SetRecordingFrequency,
        "sets the recording frequency, should be one of the usual suspects of what sound cards can "
        "record."),
    new TVistaPropertySet<int, int, VistaOpenALRecordDriver::Parameters>("BUFFERTIME",
        SsReflectionName, &VistaOpenALRecordDriver::Parameters::SetBufferTime,
        "sets time to record for a single measure."),
    new TVistaPropertySet<const std::string&, std::string, VistaOpenALRecordDriver::Parameters>(
        "FORMAT", SsReflectionName, &VistaOpenALRecordDriver::Parameters::SetRecordingFormat,
        "sets the recording format (one of: MONO8, MONO16, STEREO8, STEREO16)."),
    NULL};

class TimeoutBasedOnProps : public VistaDriverThreadAspect::IDriverUpdatePrepare {
 public:
  TimeoutBasedOnProps(VistaOpenALRecordDriver::Parameters* p)
      : IDriverUpdatePrepare()
      , m_params(p) {
  }

  VistaOpenALRecordDriver::Parameters* m_params;

  virtual bool PrePoll() {
    int msWait = m_params->GetBufferTime();
    // sample twice as often as probably needed
    VistaTimeUtils::Sleep(int(float(msWait) * 0.5f));
    return true;
  }

  virtual bool PostPoll() {
    return true;
  }
};

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

VistaOpenALRecordDriver::Parameters::Parameters(VistaOpenALRecordDriver* pDriver)

    : VistaDriverGenericParameterAspect::IParameterContainer()
    , m_parent(pDriver)
    , m_format(AL_FORMAT_MONO16)
    , m_recordingFrequency(44100)
    , m_bufferTime(100)
    , m_minimumBufferTime(100)
    , m_bufferSize(0)
    , m_isDef(false) {
  //	UpdateBufferSize();
}

int VistaOpenALRecordDriver::Parameters::GetNumberOfBytesPerSample() const {
  return ::GetNumberOfBytesPerSample(m_format);
}

bool VistaOpenALRecordDriver::Parameters::SetRecordingFormat(const std::string& format) {
  if (VistaAspectsComparisonStuff::StringEquals(format, "MONO8", false))
    return SetRecordingFormat(AL_FORMAT_MONO8);
  else if (VistaAspectsComparisonStuff::StringEquals(format, "MONO16", false))
    return SetRecordingFormat(AL_FORMAT_MONO16);
  else if (VistaAspectsComparisonStuff::StringEquals(format, "STEREO8", false))
    return SetRecordingFormat(AL_FORMAT_STEREO8);
  else if (VistaAspectsComparisonStuff::StringEquals(format, "STEREO16", false))
    return SetRecordingFormat(AL_FORMAT_STEREO16);
  else
    return false;
}

std::string VistaOpenALRecordDriver::Parameters::GetDeviceName() const {
  return m_strDeviceName;
}

bool VistaOpenALRecordDriver::Parameters::SetDeviceName(const std::string& strName) {
  return propChange<std::string>(
      m_parent, this, m_strDeviceName, strName, MSG_DEVICENAME_CHG, m_isDef);
}

int VistaOpenALRecordDriver::Parameters::GetRecordingFrequency() const {
  return m_recordingFrequency;
}

bool VistaOpenALRecordDriver::Parameters::SetRecordingFrequency(int nFreq) {
  if (propChange<int>(m_parent, this, m_recordingFrequency, nFreq, MSG_RECORDINGFREQ_CHG, m_isDef))
    return UpdateBufferSize();

  return false;
}

int VistaOpenALRecordDriver::Parameters::GetRecordingFormat() const {
  return m_format;
}

std::string VistaOpenALRecordDriver::Parameters::GetRecordingFormatString() const {
  return GetFormatStringFor(m_format);
}

bool VistaOpenALRecordDriver::Parameters::SetRecordingFormat(int nFormat) {
  if (propChange<int>(m_parent, this, m_format, nFormat, MSG_FORMAT_CHG, m_isDef))
    return UpdateBufferSize();
  return false;
}

int VistaOpenALRecordDriver::Parameters::GetBufferTime() const {
  return m_bufferTime;
}

bool VistaOpenALRecordDriver::Parameters::SetBufferTime(int nTime) {
  if (nTime < m_minimumBufferTime) // we do not allow anything lower than this value
    return false;

  if (propChange<int>(m_parent, this, m_bufferTime, nTime, MSG_BUFFERTIME_CHG, m_isDef))
    return UpdateBufferSize();
  return false;
}

int VistaOpenALRecordDriver::Parameters::GetBufferSize() const {
  return m_bufferSize;
}

bool VistaOpenALRecordDriver::Parameters::SetBufferSize(int nSize) {
  return propChange<int>(m_parent, this, m_bufferSize, nSize, MSG_BUFFERSIZE_CHG, m_isDef);
}

int VistaOpenALRecordDriver::Parameters::GetMinimumBufferTime() const {
  return m_minimumBufferTime;
}

bool VistaOpenALRecordDriver::Parameters::SetMinimumBufferTime(int nMin) {
  if (nMin < 0)
    return false;

  return propChange<int>(m_parent, this, m_minimumBufferTime, nMin, MSG_MINBUFFERTIME_CHG, m_isDef);
}

int VistaOpenALRecordDriver::Parameters::GetNumberOfSamples() const {
  return m_bufferSize / ::GetNumberOfBytesPerSample(m_format);
}

bool VistaOpenALRecordDriver::Parameters::UpdateBufferSize() {
  int nBytes = ::GetNumberOfBytesPerSample(m_format);
  // calculate the number of bytes needed for a recording of a buffer length
  // given by the user as m_bufferTime.
  int bufferSize = int((((float(m_recordingFrequency) * float(nBytes))) / 1000.0f) * m_bufferTime);
  // set the buffer (will flush old samples)
  return SetBufferSize(bufferSize);
}

bool VistaOpenALRecordDriver::Parameters::TurnDef(bool def) {
  m_isDef = def;
  return !m_isDef; // return old state
}

bool VistaOpenALRecordDriver::Parameters::Apply() {
  if (m_isDef)
    return false;

  m_parent->SignalPropertyChanged(MSG_PROBABLY_ALL_CHANGED);
  return true;
}
// #############################################################################
// MEASURES
// #############################################################################

typedef TVistaDriverEnableAspect<VistaOpenALRecordDriver> VistaOpenALEnableAspect;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenALRecordDriver::VistaOpenALRecordDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pInfo(new VistaDriverInfoAspect)
    , m_pThread(NULL)
    , m_pContext(NULL)
    , m_pCapture(NULL)
    , nToGo(-1)
    , samples(-1)
    , numBytesPerSample(-1) {
  m_pThread = new VistaDriverThreadAspect(this);

  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  VistaDeviceSensor* pSensor = new VistaDeviceSensor;
  pSensor->SetMeasureTranscode(crm->GetTranscoderFactoryForSensor("AUDIO")->CreateTranscoder());
  pSensor->SetTypeHint("AUDIO");
  AddDeviceSensor(pSensor);

  pSensor = new VistaDeviceSensor;
  pSensor->SetMeasureTranscode(crm->GetTranscoderFactoryForSensor("TYPE")->CreateTranscoder());
  pSensor->SetTypeHint("TYPE");
  AddDeviceSensor(pSensor);

  RegisterAspect(m_pInfo);
  RegisterAspect(m_pThread);

  RegisterAspect(new VistaOpenALEnableAspect(this, &VistaOpenALRecordDriver::PhysicalEnable));

  m_pParams = new VistaDriverGenericParameterAspect(new paramCreator(this));
  RegisterAspect(m_pParams);

  m_pThread->SetDriverUpdatePrepare(
      new TimeoutBasedOnProps(m_pParams->GetParameter<VistaOpenALRecordDriver::Parameters>()));

  // already fill the information that we can get about the environment.
  std::list<std::string> l = OpenALSoundContext::OpenALCaptureContext::getCaptureDeviceNames();
  VistaType::uint32      n = 0;
  VistaPropertyList      devs;
  for (std::list<std::string>::const_iterator cit = l.begin(); cit != l.end(); ++cit, ++n) {
    devs.SetProperty(VistaProperty(VistaConversion::ToString(n), *cit));
  }
  m_pInfo->GetInfoPropsWrite().SetPropertyListValue("CAPTUREDEVICES", devs);
  m_pInfo->GetInfoPropsWrite().SetProperty(VistaProperty(
      "DEFAULTDEVICE", OpenALSoundContext::OpenALCaptureContext::getDefaultCaptureDevice()));
}

VistaOpenALRecordDriver::~VistaOpenALRecordDriver() {
  VistaDeviceSensor* pSensor = GetSensorByIndex(1);
  GetFactory()->GetTranscoderFactoryForSensor("TYPE")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  pSensor = GetSensorByIndex(0);
  GetFactory()->GetTranscoderFactoryForSensor("AUDIO")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  delete m_pThread->GetDriverUpdatePrepare();
  m_pThread->SetDriverUpdatePrepare(NULL);

  UnregisterAspect(m_pThread, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pThread;

  UnregisterAspect(m_pInfo, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pInfo;

  UnregisterAspect(m_pParams, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pParams;

  VistaOpenALEnableAspect* enabler =
      GetAspectAs<VistaOpenALEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;

  delete m_pCapture;
  delete m_pContext;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaOpenALRecordDriver::DoConnect() {
  if (m_pContext)
    return true; // already connected

  m_pContext = new OpenALSoundContext;

  OpenALSoundContext::OpenALSoundEnvironment* pEnv = m_pContext->CreateAndAddSoundEnv("default");
  if (!pEnv) {
    delete m_pContext;
    delete m_pCapture;
    m_pContext = NULL;
    m_pCapture = NULL;
    return false;
  }

  m_pInfo->GetInfoPropsWrite().SetProperty(VistaProperty("VENDOR", m_pContext->GetVendor()));
  m_pInfo->GetInfoPropsWrite().SetProperty(VistaProperty("VERSION", m_pContext->GetVersion()));
  m_pInfo->GetInfoPropsWrite().SetProperty(VistaProperty("RENDERER", m_pContext->GetRenderer()));
  m_pInfo->GetInfoPropsWrite().SetProperty(
      VistaProperty("EXTENSIONS", m_pContext->GetExtensions()));

  return true;
}

bool VistaOpenALRecordDriver::DoDisconnect() {
  delete m_pCapture;
  m_pCapture = NULL;
  delete m_pContext;
  m_pContext = NULL;

  return true;
}

bool VistaOpenALRecordDriver::PhysicalEnable(bool bEnable) {
  if (m_pCapture == NULL)
    return false;

  if (bEnable) {
    if (m_pThread->GetIsProcessing() && m_pCapture->doesCapture())
      return true;

    if (m_pCapture->start())
      return m_pThread->UnpauseProcessing();
    return false;
  } else {
    if (m_pThread->GetIsProcessing()) {
      if (m_pThread->PauseProcessing())
        return m_pCapture->stop();
      return false;
    }
    return true;
  }
}

bool VistaOpenALRecordDriver::DoSensorUpdate(VistaType::microtime nTs) {
  if (!m_pCapture)
    // can not do this during 'connect' as
    // the history is probably not set up and a call to UpdateBufferSize() will crash.
    m_pParams->GetParameter<Parameters>()->UpdateBufferSize();

  if (m_pCapture && !m_pCapture->doesCapture())
    return true; // no capture, no new stuff
  else if (!m_pCapture)
    return false; // no capture, no fun.

  VistaDeviceSensor* pSensor = GetSensorByIndex(0);

  MeasureStart(0, nTs);
  // get the current place for the decoding for sensor 0
  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

  assert(samples != -1 && numBytesPerSample != -1);

  if (nToGo == -1)
    nToGo = samples; // normalize to samples

  nToGo -=
      (int)m_pCapture->captureTo(&pM->m_vecMeasures[(samples - nToGo) * numBytesPerSample], nToGo);

  if (nToGo == 0) {
    nToGo = -1;
    MeasureStop(0); // close this block
  }
  return true;
}

void VistaOpenALRecordDriver::SignalPropertyChanged(int msg) {
  switch (msg) {
  case IVistaObserveable::MSG_PROBABLY_ALL_CHANGED:
  case Parameters::MSG_RECORDINGFREQ_CHG:
  case Parameters::MSG_FORMAT_CHG:
  case Parameters::MSG_BUFFERSIZE_CHG:
  case Parameters::MSG_DEVICENAME_CHG: {
    OnChangeRecordingParameters();
    break;
  }
  default:
    break;
  }
}

void VistaOpenALRecordDriver::OnChangeRecordingParameters() {
  bool bEnable = false;
  if (GetIsEnabled()) {
    SetIsEnabled(false);
    bEnable = true;
  }

  delete m_pCapture;

  VistaDeviceSensor* pSensor = GetSensorByIndex(1);
  Parameters*        p       = m_pParams->GetParameter<Parameters>();

  m_pCapture = new OpenALSoundContext::OpenALCaptureContext(p->GetRecordingFrequency(),
      p->GetRecordingFormat(),
      // add a bit of room for the driver ;)
      // and yes... 5 is totally arbitrary
      p->GetBufferSize() * 5, p->GetDeviceName());

  VistaType::microtime nTs = GetUpdateTimer().GetMicroTime();
  MeasureStart(1, nTs);
  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

  _audiotype* m   = (*pM).getWrite<_audiotype>();
  m->nFormat      = p->GetRecordingFormat();
  m->nFreq        = p->GetRecordingFrequency();
  m->nMeasureSize = p->GetBufferSize();

  samples           = p->GetNumberOfSamples();
  numBytesPerSample = p->GetNumberOfBytesPerSample();

  MeasureStop(1);

  OnUpdateMeasureSize();

  if (bEnable)
    SetIsEnabled(true);
}

void VistaOpenALRecordDriver::OnUpdateMeasureSize() {
  VistaDeviceSensor*         pSensor = GetSensorByIndex(0);
  const VistaMeasureHistory& history = pSensor->GetMeasures();

  // here we assume that client read size and driver write size are already pre-given (on user
  // demand) for a 'good' access of the history (meaning: the temporal aspects, its just the size
  // per measure that changes). This operation will flush all old contents currently in the history.
  m_pHistoryAspect->SetHistorySize(pSensor, history.m_nClientReadSize, history.m_nDriverWriteSize,
      m_pParams->GetParameter<Parameters>()->GetBufferSize());
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/
