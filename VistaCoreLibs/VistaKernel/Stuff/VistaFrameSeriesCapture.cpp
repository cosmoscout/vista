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

#include "VistaFrameSeriesCapture.h"

#include <VistaBase/VistaTimeUtils.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/VistaFrameLoop.h>
#include <VistaKernel/VistaSystem.h>
#include <VistaTools/VistaFileSystemDirectory.h>

#if defined(WIN32)
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaFrameSeriesCapture::VistaFrameSeriesCapture(
    VistaSystem* pSystem, VistaWindow* pWindow, const bool bAutoRegisterUnregisterAsEventHandler)
    : m_pSystem(pSystem)
    , m_pWindow(pWindow)
    , m_bAutoRegisterUnregisterAsEventHandler(bAutoRegisterUnregisterAsEventHandler)
    , m_bIsRegistered(false)
    , m_nScreenshotCount(0)
    , m_nPeriod(0)
    , m_nFrameCylce(0)
    , m_eCaptureMode(CM_INVALID)
    , m_nLastCaptureFrameIndex(0)
    , m_nLastCaptureTime(0) {
  if (m_pWindow == NULL) {
    const std::map<std::string, VistaWindow*>& mapWindows =
        m_pSystem->GetDisplayManager()->GetWindowsConstRef();
    if (mapWindows.size() == 1) {
      m_pWindow = (*mapWindows.begin()).second;
    } else {
      vstr::warnp() << "[VistaFrameSeriesCapture]: no window specified, and system does not have "
                       "exactly one window"
                    << std::endl;
    }
  }

  ResetReplaceFalgs();
}

VistaFrameSeriesCapture::~VistaFrameSeriesCapture() {
  if (m_bIsRegistered)
    m_pSystem->GetEventManager()->RemEventHandler(
        this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);
}

bool VistaFrameSeriesCapture::InitCaptureEveryFrame(
    const std::string& sLocation, const std::string& sFilenamePattern) {
  if (m_pWindow == NULL)
    return false;

  VistaType::microtime nTime = m_pSystem->GetFrameClock();
  m_sFileLocation            = sLocation;
  ResetReplaceFalgs();
  ReplaceDate(m_sFileLocation, nTime);
  ReplaceTime(m_sFileLocation, nTime);

  VistaFileSystemDirectory oFileLocation(m_sFileLocation);
  if (oFileLocation.Exists() == false && oFileLocation.CreateWithParentDirectories() == false) {
    vstr::warnp() << "[VistaFrameSeriesCapture]: cannot create screenshot target directory ["
                  << m_sFileLocation << "]" << std::endl;
    return false;
  }

  m_sFilePattern = sFilenamePattern;

  if (m_bAutoRegisterUnregisterAsEventHandler && m_bIsRegistered == false)
    m_pSystem->GetEventManager()->AddEventHandler(
        this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

  m_nScreenshotCount = 0;
  m_eCaptureMode     = CM_EVERY_FRAME;

  m_nLastCaptureFrameIndex = 0;
  m_nLastCaptureTime       = 0;

  ResetReplaceFalgs();

  return true;
}

bool VistaFrameSeriesCapture::InitCaptureEveryNthFrame(
    const std::string& sLocation, const std::string& sFilenamePattern, const int nFrame) {
  if (m_pWindow == NULL)
    return false;

  VistaType::microtime nTime = m_pSystem->GetFrameClock();
  m_sFileLocation            = sLocation;
  ResetReplaceFalgs();
  ReplaceDate(m_sFileLocation, nTime);
  ReplaceTime(m_sFileLocation, nTime);

  VistaFileSystemDirectory oFileLocation(m_sFileLocation);
  if (oFileLocation.Exists() == false && oFileLocation.CreateWithParentDirectories() == false) {
    vstr::warnp() << "[VistaFrameSeriesCapture]: cannot create screenshot target directory ["
                  << m_sFileLocation << "]" << std::endl;
    return false;
  }

  m_sFilePattern = sFilenamePattern;

  if (m_bAutoRegisterUnregisterAsEventHandler && m_bIsRegistered == false)
    m_pSystem->GetEventManager()->AddEventHandler(
        this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

  m_nScreenshotCount = 0;
  m_eCaptureMode     = CM_EVERY_NTH_FRAME;
  m_nFrameCylce      = nFrame;

  m_nLastCaptureFrameIndex = 0;
  m_nLastCaptureTime       = 0;

  ResetReplaceFalgs();

  return true;
}

bool VistaFrameSeriesCapture::InitCapturePeriodically(const std::string& sLocation,
    const std::string& sFilenamePattern, const VistaType::microtime nPeriod) {
  if (m_pWindow == NULL)
    return false;

  VistaType::microtime nTime = m_pSystem->GetFrameClock();
  m_sFileLocation            = sLocation;
  ResetReplaceFalgs();
  ReplaceDate(m_sFileLocation, nTime);
  ReplaceTime(m_sFileLocation, nTime);

  VistaFileSystemDirectory oFileLocation(m_sFileLocation);
  if (oFileLocation.Exists() == false && oFileLocation.CreateWithParentDirectories() == false) {
    vstr::warnp() << "[VistaFrameSeriesCapture]: cannot create screenshot target directory ["
                  << m_sFileLocation << "]" << std::endl;
    return false;
  }

  m_sFilePattern = sFilenamePattern;

  if (m_bAutoRegisterUnregisterAsEventHandler && m_bIsRegistered == false)
    m_pSystem->GetEventManager()->AddEventHandler(
        this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

  m_nScreenshotCount = 0;
  m_eCaptureMode     = CM_PERIODICALLY;
  m_nPeriod          = (float)nPeriod;

  m_nLastCaptureFrameIndex = 0;
  m_nLastCaptureTime       = 0;

  ResetReplaceFalgs();

  return true;
}

bool VistaFrameSeriesCapture::InitCaptureWithFramerate(
    const std::string& sLocation, const std::string& sFilenamePattern, const float nFramerate) {
  if (m_pWindow == NULL)
    return false;

  VistaType::microtime nTime = m_pSystem->GetFrameClock();
  m_sFileLocation            = sLocation;
  ResetReplaceFalgs();
  ReplaceDate(m_sFileLocation, nTime);
  ReplaceTime(m_sFileLocation, nTime);

  VistaFileSystemDirectory oFileLocation(m_sFileLocation);
  if (oFileLocation.Exists() == false && oFileLocation.CreateWithParentDirectories() == false) {
    vstr::warnp() << "[VistaFrameSeriesCapture]: cannot create screenshot target directory ["
                  << m_sFileLocation << "]" << std::endl;
    return false;
  }

  m_sFilePattern = sFilenamePattern;

  if (m_bAutoRegisterUnregisterAsEventHandler && m_bIsRegistered == false)
    m_pSystem->GetEventManager()->AddEventHandler(
        this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

  m_nScreenshotCount = 0;
  m_eCaptureMode     = CM_FIXED_FRAMERATE;
  m_nPeriod          = 1.0f / nFramerate;

  m_nLastCaptureFrameIndex = -1;
  m_nLastCaptureTime       = -1;

  ResetReplaceFalgs();

  return true;
}

VistaFrameSeriesCapture::CaptureMode VistaFrameSeriesCapture::GetCaptureMode() {
  return m_eCaptureMode;
}

std::string VistaFrameSeriesCapture::GetFileLocation() {
  return m_sFileLocation;
}

std::string VistaFrameSeriesCapture::GetFilePattern() {
  return m_sFilePattern;
}

void VistaFrameSeriesCapture::HandleEvent(VistaEvent* pEvent) {
  if (m_nLastCaptureTime <= 0) {
    // first frame - initialize reference time
    m_nLastCaptureTime = pEvent->GetTime();
    return;
  }

  switch (m_eCaptureMode) {
  case CM_EVERY_FRAME: {
    MakeScreenshot(pEvent->GetTime());
    break;
  }
  case CM_EVERY_NTH_FRAME: {
    int nDelta = m_pSystem->GetFrameLoop()->GetFrameCount() - m_nLastCaptureFrameIndex;
    if (nDelta >= m_nFrameCylce)
      MakeScreenshot(pEvent->GetTime());
    break;
  }
  case CM_PERIODICALLY: {
    VistaType::microtime nDelta = pEvent->GetTime() - m_nLastCaptureTime;
    if (nDelta > m_nPeriod)
      MakeScreenshot(pEvent->GetTime());
    break;
  }
  case CM_FIXED_FRAMERATE: {
    VistaType::microtime nDelta = pEvent->GetTime() - m_nLastCaptureTime;
    while (nDelta > m_nPeriod) {
      MakeScreenshot(m_nLastCaptureTime + m_nPeriod);
      nDelta = pEvent->GetTime() - m_nLastCaptureTime;
    }
    break;
  }
  default:
    break;
  }
}

bool VistaFrameSeriesCapture::StringReplace(
    std::string& sString, const std::string& sFind, const std::string& sReplace) {
  std::size_t nPos = sString.find(sFind);
  if (nPos == std::string::npos)
    return false;
  do {
    sString.replace(nPos, sFind.length(), sReplace);
    nPos += sReplace.length();
    nPos = sString.find(sFind, nPos);
  } while (nPos != std::string::npos);
  return true;
}

void VistaFrameSeriesCapture::ReplaceDate(std::string& sName, const VistaType::microtime nTime) {
  if (m_bNeedsDateReplace == false)
    return;

  std::string sDateString = VistaTimeUtils::ConvertToFormattedTimeString(nTime, "%y%m%d");
  m_bNeedsDateReplace =
      StringReplace(sName, "%D%", sDateString) || StringReplace(sName, "$D$", sDateString);
}

void VistaFrameSeriesCapture::ReplaceTime(std::string& sName, const VistaType::microtime nTime) {
  if (m_bNeedsTimeReplace == false)
    return;

  std::string sTimeString = VistaTimeUtils::ConvertToFormattedTimeString(nTime, "%H%M%S");
  m_bNeedsTimeReplace =
      (StringReplace(sName, "%T%", sTimeString) || StringReplace(sName, "$T$", sTimeString));
}

void VistaFrameSeriesCapture::ReplaceMilliSeconds(
    std::string& sName, const VistaType::microtime nTime) {
  if (m_bNeedsMilliReplace == false)
    return;
  int         nMillis = (int)(1000.0 * (nTime - (double)(int)nTime));
  std::string sMilliString;
  int         nCheck = 100;
  while (nMillis < nCheck && nCheck > 1) {
    sMilliString += "0";
    nCheck /= 10;
  }
  sMilliString += VistaConversion::ToString(nMillis);

  m_bNeedsMilliReplace =
      (StringReplace(sName, "%M%", sMilliString) || StringReplace(sName, "$M$", sMilliString));
}

void VistaFrameSeriesCapture::ReplaceFrameCount(std::string& sName) {
  if (m_bNeedsFrameReplace == false)
    return;

  int         nCount = m_pSystem->GetFrameLoop()->GetFrameCount();
  std::string sCountString;
  int         nCheck = 100000;
  while (nCount < nCheck && nCheck > 1) {
    sCountString += "0";
    nCheck /= 10;
  }
  sCountString += VistaConversion::ToString(nCount);

  m_bNeedsFrameReplace =
      (StringReplace(sName, "%F%", sCountString) || StringReplace(sName, "$F$", sCountString));
}

void VistaFrameSeriesCapture::ReplaceScreenshotCount(std::string& sName) {
  if (m_bNeedsScreenshotReplace == false)
    return;

  int         nCount = m_nScreenshotCount;
  std::string sCountString;
  int         nCheck = 100000;
  while (nCount < nCheck && nCheck > 1) {
    sCountString += "0";
    nCheck /= 10;
  }
  sCountString += VistaConversion::ToString(nCount);

  m_bNeedsScreenshotReplace =
      (StringReplace(sName, "%S%", sCountString) || StringReplace(sName, "$S$", sCountString));
}

void VistaFrameSeriesCapture::ReplaceNodeName(std::string& sName) {
  if (m_bNeedsNodeNameReplace == false)
    return;

  m_bNeedsNodeNameReplace =
      StringReplace(sName, "%N%", m_pSystem->GetClusterMode()->GetNodeName()) ||
      StringReplace(sName, "$N$", m_pSystem->GetClusterMode()->GetNodeName());
}

void VistaFrameSeriesCapture::ResetReplaceFalgs() {
  m_bNeedsDateReplace       = true;
  m_bNeedsTimeReplace       = true;
  m_bNeedsMilliReplace      = true;
  m_bNeedsScreenshotReplace = true;
  m_bNeedsFrameReplace      = true;
  m_bNeedsNodeNameReplace   = true;
}

void VistaFrameSeriesCapture::MakeScreenshot(const VistaType::microtime nTime) {
  std::string sFilename = m_sFilePattern;
  ReplaceDate(sFilename, nTime);
  ReplaceTime(sFilename, nTime);
  ReplaceMilliSeconds(sFilename, nTime);
  ReplaceFrameCount(sFilename);
  ReplaceScreenshotCount(sFilename);
  ReplaceNodeName(sFilename);

  sFilename = m_sFileLocation + "/" + sFilename;

  m_pSystem->GetDisplayManager()->MakeScreenshot(m_pWindow, sFilename);

  m_nLastCaptureFrameIndex = m_pSystem->GetFrameLoop()->GetFrameCount();
  m_nLastCaptureTime       = nTime;
  ++m_nScreenshotCount;
}
