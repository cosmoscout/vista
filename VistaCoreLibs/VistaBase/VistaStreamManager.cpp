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

#include "VistaStreamManager.h"

#include "VistaStreamUtils.h"
#include "VistaTimeUtils.h"
#include "VistaTimer.h"

#include <algorithm>
#include <fstream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaStreamManager::VistaStreamManager()
    : m_pTimer(new VistaTimer)
    , m_pInfo(NULL)
    , m_iIndentationLevel(0)
    , m_sIndentationPrefixString("")
    , m_sIndentationPostfixString("")
    , m_sIndentationLevelString("    ")
    , m_sErrorPrefix("###ERROR### ")
    , m_sWarningPrefix("**WARNING** ") {
  m_vecInfoLayout.push_back(LE_FRAMECOUNT);
  m_vecInfoLayout.push_back(LE_COLON);
  m_vecInfoLayout.push_back(LE_FRAMECLOCK);
  m_vecInfoLayout.push_back(LE_PIPE);
}

VistaStreamManager::~VistaStreamManager() {
  for (std::vector<std::ostream*>::iterator itStream = m_vecOwnStreams.begin();
       itStream != m_vecOwnStreams.end(); ++itStream) {
    delete (*itStream);
  }
  delete m_pInfo;
  delete m_pTimer;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

bool VistaStreamManager::AddNewLogFileStream(const std::string& sStreamName,
    const std::string& sFileName, const std::string& sFileExtension, const bool bAddNodeName,
    const bool bAddTimeStamp, const bool bAppendToExistingFile) {

  std::map<std::string, std::ostream*>::iterator itStream = m_mapStreams.find(sStreamName);
  if (itStream != m_mapStreams.end())
    return false; // name already exists

  std::string   sActualFilename = (sFileName.empty() ? sStreamName : sFileName);
  std::ostream* pStream         = CreateNewLogFileStream(
      sActualFilename, sFileExtension, true, bAddNodeName, bAddTimeStamp, bAppendToExistingFile);
  if (pStream == NULL)
    return false;
  m_mapStreams[sStreamName] = pStream;
  return true;
}

std::ostream* VistaStreamManager::CreateNewLogFileStream(const std::string& sFileName,
    const std::string& sFileExtension, const bool bManageDeletion, const bool bAddNodeName,
    const bool bAddTimeStamp, const bool bAppendToExistingFile) {
  std::string sFullName = sFileName;

  if (bAddTimeStamp)
    sFullName += "." + GetDateString();

  if (bAddNodeName)
    sFullName += "." + GetNodeName();

  if (sFileExtension.empty() == false)
    sFullName += "." + sFileExtension;

  std::ofstream* oStream;
  if (bAppendToExistingFile)
    oStream = new std::ofstream(sFullName.c_str(), std::ios_base::out | std::ios_base::app);
  else
    oStream = new std::ofstream(sFullName.c_str());

  if (oStream->bad() || oStream->is_open() == false) {
    delete oStream;
    return NULL;
  }

  if (bManageDeletion)
    m_vecOwnStreams.push_back(oStream);

  return oStream;
}

bool VistaStreamManager::AddStream(const std::string& sName, std::ostream& oStream,
    bool bManageDeletion, bool bReplaceExistingStream) {
  std::map<std::string, std::ostream*>::iterator itStream = m_mapStreams.find(sName);
  if (itStream != m_mapStreams.end() && bReplaceExistingStream == false) {
    return false;
  }
  m_mapStreams[sName] = &oStream;
  if (bManageDeletion)
    m_vecOwnStreams.push_back(&oStream);
  return true;
}
bool VistaStreamManager::RemoveStream(const std::string& sName, const bool bAlwaysDelete) {
  std::map<std::string, std::ostream*>::iterator itStream = m_mapStreams.find(sName);
  if (itStream == m_mapStreams.end())
    return false;

  std::vector<std::ostream*>::iterator itToDel =
      std::find(m_vecOwnStreams.begin(), m_vecOwnStreams.end(), (*itStream).second);
  if (itToDel != m_vecOwnStreams.end()) {
    delete (*itStream).second;
    m_vecOwnStreams.erase(itToDel);
  } else if (bAlwaysDelete)
    delete (*itStream).second;
  m_mapStreams.erase(itStream);
  return true;
}
std::ostream& VistaStreamManager::GetStream(const std::string& sName) {
  std::map<std::string, std::ostream*>::iterator itStream = m_mapStreams.find(sName);
  if (itStream != m_mapStreams.end())
    return *((*itStream).second);
  else
    return GetDefaultStream(sName);
}

bool VistaStreamManager::SetInfoLayout(const std::vector<INFO_LAYOUT_ELEMENT>& vecLayout) {
  vstr::GetStreamManager()->m_vecInfoLayout = vecLayout;
  return true;
}
bool VistaStreamManager::GetInfoLayout(std::vector<INFO_LAYOUT_ELEMENT>& vecLayout) {
  vecLayout = vstr::GetStreamManager()->m_vecInfoLayout;
  return true;
}

std::vector<VistaStreamManager::INFO_LAYOUT_ELEMENT>& VistaStreamManager::GetInfoLayoutRef() {
  return m_vecInfoLayout;
}

VistaType::systemtime VistaStreamManager::GetSystemTime() const {
  return m_pTimer->GetSystemTime();
}
VistaType::systemtime VistaStreamManager::GetMicroTime() const {
  return m_pTimer->GetMicroTime();
}
VistaType::microtime VistaStreamManager::GetFrameClock() const {
  if (m_pInfo == NULL)
    return 0;
  return m_pInfo->GetFrameClock();
}
float VistaStreamManager::GetFrameRate() const {
  if (m_pInfo == NULL)
    return 0;
  return m_pInfo->GetFrameRate();
}
unsigned int VistaStreamManager::GetFrameCount() const {
  if (m_pInfo == NULL)
    return 0;
  return m_pInfo->GetFrameCount();
}

std::string VistaStreamManager::GetNodeName() const {
  if (m_pInfo == NULL)
    return "";
  return m_pInfo->GetNodeName();
}

std::string VistaStreamManager::GetDateString() const {
  if (m_pInfo != NULL)
    return VistaTimeUtils::ConvertToLexicographicDateString(m_pInfo->GetFrameClock());
  else
    return VistaTimeUtils::ConvertToLexicographicDateString(m_pTimer->GetSystemTime());
}

void VistaStreamManager::PrintInfo(std::ostream& oStream) const {
  for (std::vector<INFO_LAYOUT_ELEMENT>::const_iterator itElement = m_vecInfoLayout.begin();
       itElement != m_vecInfoLayout.end(); ++itElement) {
    switch ((*itElement)) {
    case LE_FRAMECLOCK: {
      oStream << vstr::frameclock;
      break;
    }
    case LE_SYSTEMTIME: {
      oStream << vstr::systime;
      break;
    }
    case LE_RELATIVETIME: {
      oStream << vstr::relativetime;
      break;
    }
    case LE_DATE: {
      oStream << vstr::date;
      break;
    }
    case LE_NODENAME: {
      oStream << vstr::nodename;
      break;
    }
    case LE_FRAMECOUNT: {
      oStream << vstr::framecount;
      break;
    }
    case LE_FRAMERATE: {
      oStream << vstr::framerate;
      break;
    }
    case LE_COLON: {
      oStream << ": ";
      break;
    }
    case LE_DASH: {
      oStream << " - ";
      break;
    }
    case LE_PIPE: {
      oStream << " | ";
      break;
    }
    case LE_SPACE: {
      oStream << " ";
      break;
    }
    case LE_TAB: {
      oStream << "\t";
      break;
    }
    case LE_LINEBREAK: {
      oStream << "\n";
      break;
    }
    default:
      break;
    }
  }
}

void VistaStreamManager::SetInfoInterface(VistaStreamManager::IInfoInterface* pInfoInterface) {
  m_pInfo = pInfoInterface;
}

VistaStreamManager::IInfoInterface* VistaStreamManager::GetInfoInterface() const {
  return m_pInfo;
}

std::ostream& VistaStreamManager::GetDefaultStream(const std::string& sName) {
  return vstr::GetNullStream();
}

std::string VistaStreamManager::GetErrorPrefix() const {
  return m_sErrorPrefix;
}
void VistaStreamManager::SetErrorPrefix(const std::string& sPrefix) {
  m_sErrorPrefix = sPrefix;
}
std::string VistaStreamManager::GetWarningPrefix() const {
  return m_sWarningPrefix;
}
void VistaStreamManager::SetWarningPrefix(const std::string& sPrefix) {
  m_sWarningPrefix = sPrefix;
}

int VistaStreamManager::GetIndentationLevel() const {
  return m_iIndentationLevel;
}
void VistaStreamManager::SetIndentationLevel(const int iLevel) {
  if (iLevel == m_iIndentationLevel)
    return;
  m_iIndentationLevel = iLevel;
  if (m_iIndentationLevel < 0)
    m_iIndentationLevel = 0;
  RebuildIndentationString();
}
void VistaStreamManager::AddToIndentationLevel(const int iAdd) {
  m_iIndentationLevel += iAdd;
  if (m_iIndentationLevel < 0) {
    m_iIndentationLevel = 0;
  }
  /** @todo: can be made more efficient */
  RebuildIndentationString();
}

const std::string& VistaStreamManager::GetIndentationPrefixString() const {
  return m_sIndentationPrefixString;
}
void VistaStreamManager::SetIndentationPrefixString(const std::string& sString) {
  m_sIndentationPrefixString = sString;
  RebuildIndentationString();
}
const std::string& VistaStreamManager::GetIndentationPostfixString() const {
  return m_sIndentationPostfixString;
}
void VistaStreamManager::SetIndentationPostfixString(const std::string& sString) {
  m_sIndentationPostfixString = sString;
  RebuildIndentationString();
}
const std::string& VistaStreamManager::GetIndentationLevelString() const {
  return m_sIndentationLevelString;
}
void VistaStreamManager::SetIndentationLevelString(const std::string& sString) {
  m_sIndentationLevelString = sString;
  RebuildIndentationString();
}

void VistaStreamManager::RebuildIndentationString() {
  m_sIndentation.clear();
  if (m_iIndentationLevel > 0) {
    m_sIndentation.append(m_sIndentationPrefixString);
    for (int i = 0; i < m_iIndentationLevel; ++i)
      m_sIndentation.append(m_sIndentationLevelString);
    m_sIndentation.append(m_sIndentationPostfixString);
  }
}

const std::string& VistaStreamManager::GetIndentation() const {
  return m_sIndentation;
}

bool VistaStreamManager::GetHasStream(const std::string& sName) {
  return (m_mapStreams.find(sName) != m_mapStreams.end());
}
