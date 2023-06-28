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

#include "VistaOpenCVFaceTrackDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaAspects/VistaAspectsUtils.h>

#include <VistaTools/VistaFileSystemFile.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <cassert>
#include <string.h>

#include <cv.h>
#include <cv.hpp>
#include <highgui.h>

#ifdef OPENCVFACETRACK_WITH_CLEYE
#include <CLEyeMulticam.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

static const std::string S_sDebugWindowName      = "OpenCVFaceTrack Debug Output";
static const float       S_fDefaultSizeTolerance = 1.11f;
static const float       S_fToleranceChange      = 1.11f;
static const float       S_fMaxTolerance         = 0.0f;

REFL_IMPLEMENT_FULL(VistaOpenCVFaceTrackDriver::FaceTrackParameters,
    VistaDriverGenericParameterAspect::IParameterContainer);

VistaOpenCVFaceTrackDriverCreationMethod::VistaOpenCVFaceTrackDriverCreationMethod(
    IVistaTranscoderFactoryFactory* pFac)
    : IVistaDriverCreationMethod(pFac) {
  RegisterSensorType("FACE", sizeof(VistaOpenCVFaceTrackDriver::FacePoseMeasure),
      60, // should be at most 60 fps for a camera - usually more about 25-30
      pFac->CreateFactoryForType("FACE"));
}

IVistaDeviceDriver* VistaOpenCVFaceTrackDriverCreationMethod::CreateDriver() {
  return new VistaOpenCVFaceTrackDriver(this);
}

// #############################################################################
// PARAMETER CREATE
// #############################################################################

typedef TParameterCreate<VistaOpenCVFaceTrackDriver,
    VistaOpenCVFaceTrackDriver::FaceTrackParameters>
    paramCreator;

namespace {
//	const std::string SsReflectionName("VistaOpenCVFaceTrackDriver::FaceTrackParameters");

IVistaPropertyGetFunctor* SaParameterGetter[] = {
    new TVistaPropertyGet<int, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_INT>("CAPTUREDEVICE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetCaptureDevice,
        "returns the current OpenCV capture device index"),
    new TVistaPropertyGet<int, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_INT>("WIDTH",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetWidth,
        "returns the delivered width for capture."),
    new TVistaPropertyGet<int, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_INT>("HEIGHT",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetHeight,
        "returns the delivered height for capture"),
    new TVistaPropertyGet<float, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_DOUBLE>("FRAMERATE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetFrameRate,
        "returns the frame-rate for the capture"),
    new TVistaPropertyGet<float, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_DOUBLE>("EYEDISTANCE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetEyeDistance,
        "returns eye distance of user (in m)"),
    new TVistaPropertyGet<float, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_DOUBLE>("NORMALIZEDEYEDISTANCE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetNormalizedEyeDistance,
        "returns normalized eye distance (at 1 m distance)"),
    new TVistaPropertyGet<bool, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_BOOL>("SHOWDEBUGWINDOW",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetShowDebugWindow,
        "returns if a debug window is displayed"),
    new TVistaPropertyGet<std::string, VistaOpenCVFaceTrackDriver::FaceTrackParameters,
        VistaProperty::PROPT_STRING>("CLASSIFIERFILE",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetClassifierFile,
        "returns filename of the classifier file"),
    NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {
    new TVistaPropertySet<int, int, VistaOpenCVFaceTrackDriver::FaceTrackParameters>(
        "CAPTUREDEVICE", SsReflectionName,
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetCaptureDevice,
        "sets the current OpenCV capture device index"),
    new TVistaPropertySet<int, int, VistaOpenCVFaceTrackDriver::FaceTrackParameters>("WIDTH",
        SsReflectionName, &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetWidth,
        "sets the desired horizontal resolution"),
    new TVistaPropertySet<int, int, VistaOpenCVFaceTrackDriver::FaceTrackParameters>("HEIGHT",
        SsReflectionName, &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetHeight,
        "sets the desired vertical resolution"),
    new TVistaPropertySet<float, float, VistaOpenCVFaceTrackDriver::FaceTrackParameters>(
        "FRAMERATE", SsReflectionName,
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetFrameRate,
        "sets the desired framerate"),
    new TVistaPropertySet<float, float, VistaOpenCVFaceTrackDriver::FaceTrackParameters>(
        "EYEDISTANCE", SsReflectionName,
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetEyeDistance,
        "sets eye distance of user (in m)"),
    new TVistaPropertySet<float, float, VistaOpenCVFaceTrackDriver::FaceTrackParameters>(
        "NORMALIZEDEYEDISTANCE", SsReflectionName,
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetNormalizedEyeDistance,
        "sets normalized eye distance (at 1 m distance)"),
    new TVistaPropertySet<bool, bool, VistaOpenCVFaceTrackDriver::FaceTrackParameters>(
        "SHOWDEBUGWINDOW", SsReflectionName,
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetShowDebugWindow,
        "sets if a debug window is displayed"),
    new TVistaPropertySet<const std::string&, std::string,
        VistaOpenCVFaceTrackDriver::FaceTrackParameters>("CLASSIFIERFILE", SsReflectionName,
        &VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetClassifierFile,
        "sets filename of the classifier file"),
    NULL};
} // namespace

#if !defined(WIN32)
static void ReleaseParameterProps() __attribute__((destructor));
#else
static void ReleaseParameterProps();
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
      ReleaseParameterProps();
    break;
  }
  return TRUE;
}

#endif

static void ReleaseParameterProps() {
  IVistaPropertyGetFunctor** git = SaParameterGetter;
  IVistaPropertySetFunctor** sit = SaParameterSetter;

  while (*git)
    delete *git++;

  while (*sit)
    delete *sit++;
}

VistaOpenCVFaceTrackDriver::FaceTrackParameters::FaceTrackParameters(
    VistaOpenCVFaceTrackDriver* pDriver)

    : VistaDriverGenericParameterAspect::IParameterContainer()
    , m_pDriver(pDriver)
    , m_nDeviceID(0)
    , m_nWidth(-1)
    , m_nHeight(-1)
    , m_nFramerate(0)
    , m_nEyeDistance(0.06f)
    , m_nNormalizedEyeDistance(-1.0f)
    , m_bShowDebugWindow(false) {
}

int VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetCaptureDevice() const {
  return m_nDeviceID;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetCaptureDevice(int nDevice) {
  if (m_nDeviceID == nDevice)
    return true;
  m_nDeviceID = nDevice;
  m_pDriver->ConnectToOpenCVDevice();
  Notify(MSG_CAPTUREDEVICE_CHG);
  return true;
}

int VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetWidth() const {
  return m_nWidth;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetWidth(const int nWidth) {
  if (m_nWidth == nWidth)
    return true;
  m_nWidth = nWidth;
  Notify(MSG_WIDTH_CHG);
  return true;
}

int VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetHeight() const {
  return m_nHeight;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetHeight(int nHeight) {
  if (m_nHeight == nHeight)
    return true;
  m_nHeight = nHeight;
  Notify(MSG_HEIGHT_CHG);
  return true;
}

float VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetFrameRate() const {
  return m_nFramerate;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetFrameRate(float nFramerate) {
  if (m_nFramerate == nFramerate)
    return true;
  m_nFramerate = nFramerate;
  Notify(MSG_FRAMERATE_CHG);
  return true;
}

float VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetEyeDistance() const {
  return m_nEyeDistance;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetEyeDistance(float nDistance) {
  if (m_nEyeDistance == nDistance)
    return true;
  m_nEyeDistance = nDistance;
  m_pDriver->UpdateEyeDistanceFactor();
  Notify(MSG_NORMALIZEDEYEDISTANCE_CHG);
  return true;
}

float VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetNormalizedEyeDistance() const {
  return m_nNormalizedEyeDistance;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetNormalizedEyeDistance(float nDistance) {
  if (m_nNormalizedEyeDistance == nDistance)
    return true;
  m_nNormalizedEyeDistance = nDistance;
  m_pDriver->UpdateEyeDistanceFactor();
  Notify(MSG_NORMALIZEDEYEDISTANCE_CHG);
  return true;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetShowDebugWindow() const {
  return m_bShowDebugWindow;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetShowDebugWindow(bool bShow) {
  if (m_bShowDebugWindow == bShow)
    return true;
  m_bShowDebugWindow = bShow;
  m_pDriver->ChangeShowDebugWindow();
  Notify(MSG_SHOWDEBUGWINDOW_CHG);
  return true;
}

std::string VistaOpenCVFaceTrackDriver::FaceTrackParameters::GetClassifierFile() const {
  return m_sClassifierFile;
}

bool VistaOpenCVFaceTrackDriver::FaceTrackParameters::SetClassifierFile(
    const std::string& sFilename) {
  if (m_sClassifierFile == sFilename)
    return true;
  m_sClassifierFile = sFilename;
  m_pDriver->ReadCascadeClassifier();
  Notify(MSG_CLASSIFIERFILE_CHG);
  return true;
}

typedef TVistaDriverEnableAspect<VistaOpenCVFaceTrackDriver> VistaOpenCVFaceTrackEnableAspect;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenCVFaceTrackDriver::VistaOpenCVFaceTrackDriver(IVistaDriverCreationMethod* pCreationMethod)
    : IVistaDeviceDriver(pCreationMethod)
    , m_pInfo(new VistaDriverInfoAspect)
    , m_pThread(NULL)
    , m_pCapture(NULL)
    , m_pCascade(NULL)
    , m_bDebugWindow(false)
    , m_nEyeDistanceFactor(-1)
    , m_nPosNormalizeFactor(-1)
    , m_bPreConnect(true)
    , m_oTimer(1.0f)
    , m_nLastSizeX(-1)
    , m_nLastSizeY(-1)
    , m_nLastPosX(-1)
    , m_nLastPosY(-1)
    , m_fSizeTolerance(-1)
    , m_pCLEyeCamera(NULL)
    , m_pImage(NULL) {
  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  VistaDeviceSensor* pSensor = new VistaDeviceSensor;
  pSensor->SetMeasureTranscode(
      pCreationMethod->GetTranscoderFactoryForSensor("FACE")->CreateTranscoder());
  pSensor->SetTypeHint("FACE");
  AddDeviceSensor(pSensor);

  m_pParams = new VistaDriverGenericParameterAspect(new paramCreator(this));
  m_pThread = new VistaDriverThreadAspect(this);
  RegisterAspect(m_pParams);
  RegisterAspect(m_pInfo);
  RegisterAspect(m_pThread);

  RegisterAspect(
      new VistaOpenCVFaceTrackEnableAspect(this, &VistaOpenCVFaceTrackDriver::PhysicalEnable));
}

VistaOpenCVFaceTrackDriver::~VistaOpenCVFaceTrackDriver() {
  delete m_pCapture;
  delete m_pCascade;

#ifdef OPENCVFACETRACK_WITH_CLEYE
  if (m_pCLEyeCamera)
    CLEyeDestroyCamera(m_pCLEyeCamera);

  if (m_pImage)
    cvReleaseImage(&m_pImage);
#endif

  VistaDeviceSensor* pSensor = GetSensorByIndex(0);
  GetFactory()->GetTranscoderFactoryForSensor("FACE")->DestroyTranscoder(
      pSensor->GetMeasureTranscode());
  RemDeviceSensor(pSensor);
  delete pSensor;

  cvDestroyWindow(S_sDebugWindowName.c_str());

  UnregisterAspect(m_pThread, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pThread;

  UnregisterAspect(m_pInfo, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pInfo;

  UnregisterAspect(m_pParams, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pParams;

  VistaOpenCVFaceTrackEnableAspect* enabler =
      GetAspectAs<VistaOpenCVFaceTrackEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaOpenCVFaceTrackDriver::DoConnect() {
  if (m_pCapture || m_pCLEyeCamera)
    return true;

  m_bPreConnect = false;

  ConnectToOpenCVDevice();
  ReadCascadeClassifier();
  ChangeShowDebugWindow();

  if ((!m_pCapture && !m_pCLEyeCamera) || !m_pCascade) {
    delete m_pCapture;
    m_pCapture = NULL;
    delete m_pCascade;
    m_pCascade    = NULL;
    m_bPreConnect = true;
    m_pInfo->GetInfoPropsWrite().SetValue("STATE", "ERROR");
    return false;
  } else
    m_pInfo->GetInfoPropsWrite().SetValue("STATE", "OK");

  return true;
}

bool VistaOpenCVFaceTrackDriver::DoDisconnect() {
  delete m_pCapture;
  delete m_pCascade;

  m_pCapture = 0;
  m_pCascade = 0;

  return true;
}

bool VistaOpenCVFaceTrackDriver::PhysicalEnable(bool bEnable) {
  if (bEnable) {
    if (m_pThread->GetIsProcessing())
      return true;

    FaceTrackParameters* p = m_pParams->GetParameter<FaceTrackParameters>();

    if (m_pCapture && !m_pCapture->isOpened())
      m_pCapture->open(p->GetCaptureDevice());
#ifdef OPENCVFACETRACK_WITH_CLEYE
    else if (m_pCLEyeCamera) {
      CLEyeCameraStart(m_pCLEyeCamera);
      CLEyeCameraLED(m_pCLEyeCamera, true);
    }
#endif

    m_pThread->UnpauseProcessing();
  } else {

    m_pThread->PauseProcessing();
    if (m_pCapture && m_pCapture->isOpened())
      m_pCapture->release();
#ifdef OPENCVFACETRACK_WITH_CLEYE
    else if (m_pCLEyeCamera) {
      CLEyeCameraStart(m_pCLEyeCamera);
      CLEyeCameraLED(m_pCLEyeCamera, false);
    }
#endif
  }
  return bEnable;
}

bool VistaOpenCVFaceTrackDriver::DoSensorUpdate(VistaType::microtime nTs) {
  if ((!m_pCapture && !m_pCLEyeCamera) || !m_pCascade)
    return false;

  cv::Mat oFrame;

#ifdef OPENCVFACETRACK_WITH_CLEYE
  if (m_pCLEyeCamera) {
    PBYTE pCapBuffer = NULL;
    cvGetImageRawData(m_pImage, &pCapBuffer);

    CLEyeCameraGetFrame(m_pCLEyeCamera, pCapBuffer);
    oFrame = cv::Mat(m_pImage);
  } else {
    (*m_pCapture) >> oFrame;
  }
#else
  (*m_pCapture) >> oFrame;
#endif

  int nWidth  = oFrame.cols;
  int nHeight = oFrame.rows;

  m_oTimer.RecordTime();

  std::vector<cv::Rect> vecObjects;

  VistaType::microtime nDetectTime = m_oTimer.GetMicroTime();

  if (m_nLastSizeX < 0) {
    // search for any scale
    m_pCascade->detectMultiScale(oFrame, vecObjects, 1.5, 2,
        0 // CV_HAAR_DO_CANNY_PRUNING
        //| CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH
        ,
        cv::Size(nWidth / 10, nHeight / 10));
  } else {
    cv::Size oSizeMin((int)((float)m_nLastSizeX / m_fSizeTolerance),
        (int)((float)m_nLastSizeY / m_fSizeTolerance));
    cv::Size oSizeMax((int)((float)m_nLastSizeX * m_fSizeTolerance),
        (int)((float)m_nLastSizeY * m_fSizeTolerance));
#if (CV_MAJOR_VERSION >= 2) && (CV_MINOR_VERSION >= 2)
    m_pCascade->detectMultiScale(oFrame, vecObjects, 1.1, 3,
        CV_HAAR_SCALE_IMAGE
        // CV_HAAR_SCALE_IMAGE	//| CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH
        ,
        oSizeMin, oSizeMax);
#else
    m_pCascade->detectMultiScale(oFrame, vecObjects, 1.1, 3,
        CV_HAAR_SCALE_IMAGE
        // CV_HAAR_SCALE_IMAGE	//| CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH
        ,
        oSizeMin);
#endif
  }
  nDetectTime = m_oTimer.GetMicroTime() - nDetectTime;

  /** todo: better way to determine best match */
  const cv::Rect* pBestmatch = NULL;
  int             fBestMatch = 0;
  for (std::vector<cv::Rect>::const_iterator itObject = vecObjects.begin();
       itObject != vecObjects.end(); ++itObject) {
    if ((*itObject).width < fBestMatch)
      continue;
    fBestMatch = (*itObject).width;
    pBestmatch = &(*itObject);
  }

  float fImgPosX  = 0.0f;
  float fImgPosY  = 0.0f;
  float fPosX     = 0.0f;
  float fPosY     = 0.0f;
  float fDistance = 0.0f;
  float fEyeWidth = 0.0f;
  if (pBestmatch != NULL) {
    VistaDeviceSensor* pSensor = GetSensorByIndex(0);
    MeasureStart(0, nTs);

    fEyeWidth = (float)pBestmatch->width / (float)nWidth;
    fDistance = m_nEyeDistanceFactor / fEyeWidth;
    fImgPosX  = (float)pBestmatch->x + 0.5f * (float)pBestmatch->width;
    fPosX     = fImgPosX - 0.5f * (float)nWidth;
    fPosX /= (float)nWidth;
    fPosX *= m_nPosNormalizeFactor;
    fImgPosY = (float)pBestmatch->y + 0.5f * (float)pBestmatch->height;
    fPosY = -fImgPosY + 0.5f * (float)nHeight; // negated, since image has origin on the upper left
    fPosY /= (float)nHeight;
    fPosY *= m_nPosNormalizeFactor;

    // get the current ent place for the decoding for sensor 0
    VistaSensorMeasure* pM           = m_pHistoryAspect->GetCurrentSlot(pSensor);
    FacePoseMeasure*    pFaceMeasure = pM->getWrite<FacePoseMeasure>();

    // the image is not flipped before, so now we have to flip x
    pFaceMeasure->m_a3fPosition[0] = -fPosX;
    pFaceMeasure->m_a3fPosition[1] = fPosY;
    pFaceMeasure->m_a3fPosition[2] = fDistance;

    MeasureStop(0); // close this block

    m_nLastSizeX     = pBestmatch->width;
    m_nLastSizeY     = pBestmatch->height;
    m_fSizeTolerance = S_fDefaultSizeTolerance;
  } else if (m_nLastSizeX > 0) {
    m_fSizeTolerance *= S_fToleranceChange;
    if (m_fSizeTolerance > S_fMaxTolerance)
      m_nLastSizeX = -1;
  }

  if (m_bDebugWindow) {
    cv::Scalar oColor     = CV_RGB(255, 0, 0);
    cv::Scalar oTextColor = CV_RGB(255, 255, 0);
    if (pBestmatch) {
      cv::rectangle(oFrame, (*pBestmatch), oColor);
      cv::circle(oFrame, cv::Point((int)fImgPosX, (int)fImgPosY), 2, oColor, 2);
    }
    std::string sPosText = "Pos: ( " + VistaConversion::ToString(fPosX) + ", " +
                           VistaConversion::ToString(fPosY) + ", " +
                           VistaConversion::ToString(fDistance) + ")";
    std::string sEyeText = "RelEyeDist: ( " + VistaConversion::ToString(fEyeWidth) + ")";
    std::string sUpdText =
        "Update Time: " + VistaConversion::ToString(1000 * m_oTimer.GetAverageTime()) + " ms";
    std::string sDetectText =
        "Detect Time: " + VistaConversion::ToString(1000 * nDetectTime) + " ms";
    int   nFont  = cv::FONT_HERSHEY_PLAIN;
    float fScale = 0.8f;
    cv::putText(oFrame, sPosText, cv::Point(4, 20), nFont, fScale, oTextColor);
    cv::putText(oFrame, sEyeText, cv::Point(4, 45), nFont, fScale, oTextColor);
    cv::putText(oFrame, sUpdText, cv::Point(4, 70), nFont, fScale, oTextColor);
    cv::putText(oFrame, sDetectText, cv::Point(4, 95), nFont, fScale, oTextColor);

    cv::imshow(S_sDebugWindowName, oFrame);
  }

  return true;
}

void VistaOpenCVFaceTrackDriver::ConnectToOpenCVDevice() {
  if (m_bPreConnect)
    return;

  delete m_pCapture;
  m_pCapture = NULL;

  FaceTrackParameters* pParams = m_pParams->GetParameter<FaceTrackParameters>();
  int                  iID     = pParams->GetCaptureDevice();

#ifdef OPENCVFACETRACK_WITH_CLEYE
  if (m_pCLEyeCamera) {
    CLEyeDestroyCamera(m_pCLEyeCamera);
    m_pCLEyeCamera = NULL;
  }

  if (iID >= 0 && iID < CLEyeGetCameraCount()) {
    GUID iUUID = CLEyeGetCameraUUID(iID);

    CLEyeCameraResolution nResolution = CLEYE_VGA;
    float                 nReqFPS(pParams->GetFrameRate());
    int                   nWidth  = pParams->GetWidth();
    int                   nHeight = pParams->GetHeight();
    float                 nActualFPS;
    if (nWidth > 0) {
      if (nWidth == 640) {
        nResolution = CLEYE_VGA;
      } else if (nWidth == 320) {
        nResolution = CLEYE_QVGA;
      } else if (nWidth > 480) {
        nResolution = CLEYE_VGA;
        vstr::warnp() << "[OpenCVFaceTrackDriver]: Requested unsupported Resolution [ " << nWidth
                      << "x" << nHeight << "] for SonyEye - using 640x480" << std::endl;
      } else {
        nResolution = CLEYE_QVGA;
        vstr::warnp() << "[OpenCVFaceTrackDriver]: Requested unsupported Resolution [ " << nWidth
                      << "x" << nHeight << "] for SonyEye - using 320x240" << std::endl;
      }
    }

    if (nReqFPS <= 0) {
      if (nResolution == CLEYE_QVGA)
        nActualFPS = 100;
      else
        nActualFPS = 60;
    } else {
      if ((nResolution == CLEYE_QVGA && (nReqFPS == 125 || nReqFPS == 100)) ||
          (nResolution == CLEYE_QVGA && (nReqFPS == 40)) || nReqFPS == 75 || nReqFPS == 60 ||
          nReqFPS == 30 || nReqFPS == 25) {
        nActualFPS = nReqFPS;
      } else {
        // using default
        if (nResolution == CLEYE_QVGA)
          nActualFPS = 100;
        else
          nActualFPS = 60;
        vstr::warnp() << "[OpenCVFaceTrackDriver]: Requested unsupported Framerate [ "
                      << pParams->GetCaptureDevice() << "] for SonyEye - defaulting to ["
                      << nActualFPS << "]" << std::endl;
      }
    }

    m_pCLEyeCamera = CLEyeCreateCamera(iUUID, CLEYE_COLOR_RAW, nResolution, nActualFPS);
    if (m_pCLEyeCamera == NULL) {
      vstr::outi() << "[OpenCVFaceTrackDriver]: Connecting to Sony Eye failed! - trying normal"
                   << " OpenCV cameras" << std::endl;
    } else {
      CLEyeSetCameraParameter(m_pCLEyeCamera, CLEYE_AUTO_GAIN, 1);
      CLEyeSetCameraParameter(m_pCLEyeCamera, CLEYE_AUTO_EXPOSURE, 1);
      CLEyeSetCameraParameter(m_pCLEyeCamera, CLEYE_AUTO_WHITEBALANCE, 1);
      if (m_pImage) {
        cvReleaseImage(&m_pImage);
        m_pImage = NULL;
      }

      if (nResolution == CLEYE_QVGA) {
        m_pImage = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 4);
        m_pInfo->GetInfoPropsWrite().SetValue<int>("WIDTH", 320);
        m_pInfo->GetInfoPropsWrite().SetValue<int>("HEIGHT", 240);
      } else {
        m_pImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 4);
        m_pInfo->GetInfoPropsWrite().SetValue<int>("WIDTH", 640);
        m_pInfo->GetInfoPropsWrite().SetValue<int>("HEIGHT", 480);
      }
      m_pInfo->GetInfoPropsWrite().SetValue<float>("FPS", nActualFPS);
      CLEyeCameraStart(m_pCLEyeCamera);
      CLEyeCameraLED(m_pCLEyeCamera, true);
      return;
    }
  }

#endif

  m_pCapture = new cv::VideoCapture(iID);

  if (!m_pCapture->isOpened()) {
    vstr::warnp() << "[OpenCVFaceTrackDriver]:Connecting to Camera Device [ "
                  << pParams->GetCaptureDevice() << "] failed" << std::endl;
    delete m_pCapture;
    m_pCapture = NULL;
    return;
  }

  float nFPS    = (float)m_pCapture->get(CV_CAP_PROP_FPS);
  int   nWidth  = (int)m_pCapture->get(CV_CAP_PROP_FRAME_WIDTH);
  int   nHeight = (int)m_pCapture->get(CV_CAP_PROP_FRAME_HEIGHT);

  pParams->SetFrameRate(nFPS);
  pParams->SetWidth(nWidth);
  pParams->SetHeight(nHeight);

  m_pInfo->GetInfoPropsWrite().SetValue("WIDTH", nWidth);
  m_pInfo->GetInfoPropsWrite().SetValue("HEIGHT", nHeight);
  m_pInfo->GetInfoPropsWrite().SetValue("FPS", nFPS);

  m_nNormalizeWidth  = nWidth;
  m_nNormalizeHeight = nHeight;
}

void VistaOpenCVFaceTrackDriver::UpdateEyeDistanceFactor() {
  FaceTrackParameters* pParams = m_pParams->GetParameter<FaceTrackParameters>();

  m_nEyeDistanceFactor  = pParams->GetNormalizedEyeDistance() * pParams->GetEyeDistance();
  m_nPosNormalizeFactor = pParams->GetNormalizedEyeDistance();
}

void VistaOpenCVFaceTrackDriver::ChangeShowDebugWindow() {
  if (m_bPreConnect)
    return;

  // @todo namespace-variant? (might be an opencv 2.1 thing...)
  cvDestroyWindow(S_sDebugWindowName.c_str());

  FaceTrackParameters* pParams = m_pParams->GetParameter<FaceTrackParameters>();

  m_bDebugWindow = pParams->GetShowDebugWindow();
  if (m_bDebugWindow) {
    cv::namedWindow(S_sDebugWindowName);
  }
}

void VistaOpenCVFaceTrackDriver::ReadCascadeClassifier() {
  if (m_bPreConnect)
    return;

  delete m_pCascade;
  m_pCascade = NULL;

  FaceTrackParameters* pParams   = m_pParams->GetParameter<FaceTrackParameters>();
  std::string          sFilename = pParams->GetClassifierFile();
  if (sFilename.empty()) {
    vstr::errp() << "[OpenCVFaceTrackDriver]: Loading of classifier file failed - "
                 << "no file specified by CLASSIFIERFILE" << std::endl;
    return;
  }
  VistaFileSystemFile oFile(sFilename);
  if (oFile.Exists() == false) {
    vstr::errp() << "[OpenCVFaceTrackDriver]: Loading of classifier file [" << sFilename
                 << "] failed - file does not exist" << std::endl;
    return;
  }
  m_pCascade = new cv::CascadeClassifier;
  if (m_pCascade->load(sFilename) == false) {
    vstr::errp() << "[OpenCVFaceTrackDriver]: Loading of classifier file [" << sFilename
                 << "] failed - file does not exist" << std::endl;
    delete m_pCascade;
    m_pCascade = NULL;
    return;
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/
