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

#include "VistaIniFileParser.h"

#include <VistaBase/VistaStreamUtils.h>

#include <fstream>
#include <iostream>
#include <stack>
#include <sys/stat.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

namespace {
bool CheckFileExists(const std::string& sFilename) {
#ifdef WIN32

  struct _stat attributes;

  if (_stat(sFilename.c_str(), &attributes) != 0)
    return false;

  if (attributes.st_mode & _S_IFREG)
    return true;
  else
    return false;

#else

  struct stat attributes;

  if (stat(sFilename.c_str(), &attributes) != 0)
    return false;

  if (attributes.st_mode & S_IFREG)
    return true;
  else
    return false;

#endif
}

const int S_iBufferSize = 4096;
} // namespace
#define PARSE_WARN                                                                                 \
  vstr::warnp() << "[VistaIniFileParser]: "                                                        \
                << "While parsing file [" << m_sFilename << "], Line [" << m_iCurrentLine << "]\n" \
                << vstr::singleindent

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

class IniFileReader {
 public:
  IniFileReader(char cSectionHeaderStartSymbol, char cSectionHeaderEndSymbol,
      char cKeySeparatorSymbol, char cCommentSymbol)
      : m_cSectionHeaderStartSymbol(cSectionHeaderStartSymbol)
      , m_cSectionHeaderEndSymbol(cSectionHeaderEndSymbol)
      , m_cKeySeparatorSymbol(cKeySeparatorSymbol)
      , m_cCommentSymbol(cCommentSymbol)
      , m_bReplaceEnvironmentVariables(false)
      , m_sFileVariableSectionName("FILE_VARIABLES")
      , m_cCurrentRead(0x0)
      , m_iCurrentLine(0)
      , m_pVariablesProplist(NULL) {
  }

  bool ReadFile(const std::string& sFileName, VistaPropertyList& oTarget,
      const bool bReplaceEnvironmentVariables, const std::string& sFileVariableSectionName) {
    m_iCurrentLine = 0;

    m_sFilename                    = sFileName;
    m_bReplaceEnvironmentVariables = bReplaceEnvironmentVariables;
    m_sFileVariableSectionName     = sFileVariableSectionName;
    m_pVariablesProplist           = NULL;

    m_oFile.open(m_sFilename.c_str());

    if (m_oFile.is_open() == false) {
      vstr::warnp() << "[VistaIniFileParser]: "
                    << "Could not open file [" << m_sFilename << "] for reading" << std::endl;
      return false;
    }
    int bSuccess = FillPropertyList(oTarget, 0);
    m_oFile.close();
    return (bSuccess == 0);
  }

 private:
  int FillPropertyList(VistaPropertyList& oTarget, int iLevel) {
    std::string sKey, sEntry;

    while (m_oFile.good()) {
      ++m_iCurrentLine;
      m_oFile.getline(m_acLineBuffer, S_iBufferSize);

      m_cCurrentRead = m_acLineBuffer;
      RemoveSpaces();

      if (EndOfCurrentLine())
        continue;

      // check if it's a section header
      int iSectionDepth = CheckForSectionHeader(m_sSectionName);
      if (iSectionDepth > iLevel) {
        // we fill subproplists until a higher-level proplist is
        // encountered (1<=ret<ownlvl) or the file ends (0)
        do {
          // It's a sub-proplist of us
          VistaProperty& oProp = oTarget[m_sSectionName];
          if (oProp.GetIsNilProperty() == false) {
            PARSE_WARN << "Duplicate key [" << m_sSectionName
                       << "] was encountered as section - overwriting old entry" << std::endl;
            oProp = VistaProperty();
          }
          oProp.SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
          if (iLevel == 0 && m_sSectionName == m_sFileVariableSectionName) {
            m_pVariablesProplist = &oProp.GetPropertyListRef();
          }
          oProp.SetNameForNameable(m_sSectionName);
          oProp.GetPropertyListRef().SetIsCaseSensitive(oTarget.GetIsCaseSensitive());
          iSectionDepth = FillPropertyList(oProp.GetPropertyListRef(), iSectionDepth);
        } while (iSectionDepth > iLevel);
        return iSectionDepth;
      } else if (iSectionDepth > 0) {
        // it's a proplist, but not out child -> we pass it on to our caller
        return iSectionDepth;
      }

      // read the key
      if (ReadKey(sKey) == false)
        continue;

      RemoveSpaces();

      sEntry.clear();
      if (ReadEntry(sEntry) == false)
        continue;

      CheckAndReplaceVariables(sEntry);

      VistaProperty& oProp = oTarget[sKey];
      if (oProp.GetIsNilProperty() == false) {
        PARSE_WARN << "Duplicate entry for key [" << sKey << "] was encountered" << std::endl;
        continue;
      }
      oProp.SetNameForNameable(sKey);
      oProp.SetValue(sEntry);
    }
    // file ended, return 0;
    return 0;
  }

  void CheckAndReplaceVariables(std::string& sString) {
    if (m_pVariablesProplist == NULL && m_bReplaceEnvironmentVariables == false)
      return;
    std::size_t nSearchStart = 0;

    while (nSearchStart != std::string::npos) {
      std::size_t nStart = sString.find("${", nSearchStart);
      if (nStart == std::string::npos)
        return;
      std::size_t nEnd = sString.find("}", nStart);
      if (nEnd == std::string::npos)
        return;

      std::string nVariable = sString.substr(nStart + 2, nEnd - nStart - 2);
      nSearchStart          = nEnd;

      if (m_pVariablesProplist != NULL) {
        std::string vVarValue;
        if (m_pVariablesProplist->GetValue(nVariable, vVarValue)) {
          sString      = sString.replace(nStart, nEnd - nStart + 1, vVarValue);
          nSearchStart = nStart + vVarValue.size();
          continue;
        }
      }
      if (m_bReplaceEnvironmentVariables) {
        char* sVarValue = getenv(nVariable.c_str());
        if (sVarValue != NULL) {
          sString      = sString.replace(nStart, nEnd - nStart + 1, sVarValue);
          nSearchStart = nStart + strlen(sVarValue);
          continue;
        }
      }

      PARSE_WARN << "Could not replace variable [" << nVariable << "]" << std::endl;
    }
  }

  bool EndOfCurrentLine() {
    return (*m_cCurrentRead) == '\n' || (*m_cCurrentRead) == '\r' || (*m_cCurrentRead) == 0 ||
           (*m_cCurrentRead) == m_cCommentSymbol;
  }

  void RemoveSpaces() {
    while ((*m_cCurrentRead) == ' ' || (*m_cCurrentRead) == '\t')
      ++m_cCurrentRead;
  }

  int CheckForSectionHeader(std::string& sDestination) {
    // check that the first symbol is the section start symbol
    int iDepth = 0;
    while ((*m_cCurrentRead) == m_cSectionHeaderStartSymbol) {
      ++m_cCurrentRead;
      ++iDepth;
    }

    if (iDepth == 0)
      return -1;

    // we allow spaces and tabs between the
    RemoveSpaces();

    char*       cStartBuffer = m_cCurrentRead;
    std::size_t iCount       = 0;
    while ((*m_cCurrentRead) != m_cSectionHeaderEndSymbol) {
      // we ignore for now how many closing brackets there are
      if (EndOfCurrentLine()) {
        sDestination = std::string(cStartBuffer, iCount);
        RemoveSpacesAtEnd(sDestination);
        PARSE_WARN << "An unterminated section header [" << sDestination << "] was encountered"
                   << std::endl;
      }
      ++iCount;
      ++m_cCurrentRead;
    }

    sDestination = std::string(cStartBuffer, iCount);
    RemoveSpacesAtEnd(sDestination);

    if (sDestination.empty())
      PARSE_WARN << "An empty section name was encountered" << std::endl;

    return iDepth;
  }

  bool ReadKey(std::string& sDestination) {
    if (EndOfCurrentLine())
      return false;

    char*       cStartBuffer = m_cCurrentRead;
    std::size_t iCount       = 0;
    while ((*m_cCurrentRead) != m_cKeySeparatorSymbol) {
      if (EndOfCurrentLine()) {
        sDestination = std::string(cStartBuffer, iCount);
        RemoveSpacesAtEnd(sDestination);
        PARSE_WARN << "Line without key-value-separator encountered!" << std::endl;
        return false;
      }
      ++iCount;
      ++m_cCurrentRead;
    }

    ++m_cCurrentRead;
    sDestination = std::string(cStartBuffer, iCount);
    RemoveSpacesAtEnd(sDestination);

    if (sDestination.empty()) {
      PARSE_WARN << "An empty key was encountered" << std::endl;
      return false;
    }

    return true;
  }
  bool ReadEntry(std::string& sDestination) {
    char*       cStartBuffer = m_cCurrentRead;
    std::size_t iCount       = 0;

    while (EndOfCurrentLine() == false) {
      ++iCount;
      ++m_cCurrentRead;
    }

    sDestination = std::string(cStartBuffer, iCount);
    RemoveSpacesAtEnd(sDestination);

    // empty entries are okay...
    // if( sDestination.empty() )
    //{
    //	PARSE_WARN << "An empty entry was encountered" << std::endl;
    //	return false;
    //}

    return true;
  }
  void RemoveSpacesAtEnd(std::string& sString) {
    std::string::reverse_iterator itPos = sString.rbegin();
    while (itPos != sString.rend() && ((*itPos) == ' ' || (*itPos) == '\t'))
      ++itPos;
    sString.erase(itPos.base(), sString.end());
  }

 private:
  std::string   m_sFilename;
  std::ifstream m_oFile;

  char m_cSectionHeaderStartSymbol;
  char m_cSectionHeaderEndSymbol;
  char m_cKeySeparatorSymbol;
  char m_cCommentSymbol;

  std::string m_sBuffer;
  std::string m_sSectionName;
  int         m_iCurrentLine;
  char        m_acLineBuffer[S_iBufferSize];
  char*       m_cCurrentRead;

  bool               m_bReplaceEnvironmentVariables;
  std::string        m_sFileVariableSectionName;
  VistaPropertyList* m_pVariablesProplist;
};

class IniFileWriter {
 public:
  IniFileWriter(int iMaxKeyIndent, char cSectionHeaderStartSymbol, char cSectionHeaderEndSymbol,
      char cKeySeparatorSymbol)
      : m_cSectionHeaderStartSymbol(cSectionHeaderStartSymbol)
      , m_cSectionHeaderEndSymbol(cSectionHeaderEndSymbol)
      , m_cKeySeparatorSymbol(cKeySeparatorSymbol)
      , m_iMaxKeyIndent(iMaxKeyIndent)
      , m_iSectionDepth(0) {
  }

  bool WriteFile(const std::string& sFileName, const VistaPropertyList& oTarget) {
    m_sFilename     = sFileName;
    m_iSectionDepth = 0;

    int iMaxKeyLength = FindMaxKeyLength(oTarget) + 1;
    if (iMaxKeyLength > m_iMaxKeyIndent)
      iMaxKeyLength = m_iMaxKeyIndent;

    std::ofstream oFile(m_sFilename.c_str());

    for (VistaPropertyList::const_iterator itEntry = oTarget.begin(); itEntry != oTarget.end();
         ++itEntry) {
      WriteEntry((*itEntry).second, iMaxKeyLength, oFile);
    }
    return true;
  }

  int FindMaxKeyLength(const VistaPropertyList& oList) {
    int iMax = 0;
    for (VistaPropertyList::const_iterator itEntry = oList.begin(); itEntry != oList.end();
         ++itEntry) {
      iMax = std::max<int>(iMax, (int)(*itEntry).first.size());
      if ((*itEntry).second.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
        int iSubMax = FindMaxKeyLength((*itEntry).second.GetPropertyListConstRef());
        if (iSubMax > iMax)
          iMax = iSubMax;
      }
    }
    return iMax;
  }

  bool WriteEntry(const VistaProperty& oProp, int iMaxIndent, std::ofstream& oFile) {
    if (oProp.GetPropertyType() == VistaProperty::PROPT_PROPERTYLIST) {
      ++m_iSectionDepth;
      std::string sLine = "\n";
      for (int i = 0; i < m_iSectionDepth; ++i)
        sLine += m_cSectionHeaderStartSymbol;
      sLine += oProp.GetNameForNameable();
      for (int i = 0; i < m_iSectionDepth; ++i)
        sLine += m_cSectionHeaderEndSymbol;
      sLine += '\n';
      oFile.write(sLine.c_str(), sLine.size());

      for (VistaPropertyList::const_iterator itEntry = oProp.GetPropertyListConstRef().begin();
           itEntry != oProp.GetPropertyListConstRef().end(); ++itEntry) {
        WriteEntry((*itEntry).second, iMaxIndent, oFile);
      }

      --m_iSectionDepth;
    } else {
      std::string sKey = oProp.GetNameForNameable();
      if ((int)sKey.size() < iMaxIndent)
        sKey.resize(iMaxIndent, ' ');
      else
        sKey.push_back(' ');
      std::string sLine = sKey + ' ' + m_cKeySeparatorSymbol + ' ' + oProp.GetValue() + '\n';
      oFile.write(sLine.c_str(), sLine.size());
    }
    return true;
  }

 private:
  std::string m_sFilename;

  char m_cSectionHeaderStartSymbol;
  char m_cSectionHeaderEndSymbol;
  char m_cKeySeparatorSymbol;

  int m_iMaxKeyIndent;
  int m_iSectionDepth;
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaIniFileParser::VistaIniFileParser(const bool bReplaceEnvironmentVariables,
    const std::string& sFileVariableSectionName, const bool bCaseSensitiveKeys)
    : m_oFilePropertyList(bCaseSensitiveKeys)
    , m_bReplaceEnvironmentVariables(bReplaceEnvironmentVariables)
    , m_sFileVariableSectionName(sFileVariableSectionName)
    , m_cCommentSymbol('#')
    , m_cKeySeparatorSymbol('=')
    , m_cSectionHeaderEndSymbol(']')
    , m_cSectionHeaderStartSymbol('[')
    , m_iMaxKeyIndent(20)
    , m_bFileIsValid(false) {
}

VistaIniFileParser::VistaIniFileParser(const std::string& sFilename,
    const bool bReplaceEnvironmentVariables, const std::string& sFileVariableSectionName,
    const bool bCaseSensitiveKeys)
    : m_oFilePropertyList(bCaseSensitiveKeys)
    , m_bReplaceEnvironmentVariables(bReplaceEnvironmentVariables)
    , m_sFileVariableSectionName(sFileVariableSectionName)
    , m_cCommentSymbol('#')
    , m_cKeySeparatorSymbol('=')
    , m_cSectionHeaderEndSymbol(']')
    , m_cSectionHeaderStartSymbol('[')
    , m_iMaxKeyIndent(20)
    , m_bFileIsValid(false) {
  this->ReadFile(sFilename);
}

VistaIniFileParser::VistaIniFileParser(const std::string& sFilename,
    std::list<std::string>& liFileSearchPathes, const bool bReplaceEnvironmentVariables,
    const std::string& sFileVariableSectionName, const bool bCaseSensitiveKeys)
    : m_oFilePropertyList(bCaseSensitiveKeys)
    , m_bReplaceEnvironmentVariables(bReplaceEnvironmentVariables)
    , m_sFileVariableSectionName(sFileVariableSectionName)
    , m_cCommentSymbol('#')
    , m_cKeySeparatorSymbol('=')
    , m_cSectionHeaderEndSymbol(']')
    , m_cSectionHeaderStartSymbol('[')
    , m_iMaxKeyIndent(20)
    , m_bFileIsValid(false) {
  this->ReadFile(sFilename, liFileSearchPathes);
}

VistaIniFileParser::~VistaIniFileParser() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaIniFileParser::SetSpecialCharacters(const char cSectionHeaderStart,
    const char cSectionHeaderEnd, const char cKeyAssignment, const char cComment) {
  m_cSectionHeaderStartSymbol = cSectionHeaderStart;
  m_cSectionHeaderEndSymbol   = cSectionHeaderEnd;
  m_cKeySeparatorSymbol       = cKeyAssignment;
  m_cCommentSymbol            = cComment;
}

void VistaIniFileParser::GetSpecialCharacters(
    char& cSectionHeaderStart, char& cSectionHeaderEnd, char& cKeyAssignment, char& cComment) {
  cSectionHeaderStart = m_cSectionHeaderStartSymbol;
  cSectionHeaderEnd   = m_cSectionHeaderEndSymbol;
  cKeyAssignment      = m_cKeySeparatorSymbol;
  cComment            = m_cCommentSymbol;
}

void VistaIniFileParser::SetReplaceEnvironmentVariables(const bool bSet) {
  m_bReplaceEnvironmentVariables = bSet;
}
bool VistaIniFileParser::GetReplaceEnvironmentVariables() const {
  return m_bReplaceEnvironmentVariables;
}

void VistaIniFileParser::SetUseCaseSensitiveKeys(const bool bSet) {
  m_oFilePropertyList.SetIsCaseSensitive(bSet);
}
bool VistaIniFileParser::GetUseCaseSensitiveKeys() const {
  return m_oFilePropertyList.GetIsCaseSensitive();
}

bool VistaIniFileParser::ReadFile(const std::string& sFilename) {
  if (ReadProplistFromFile(sFilename, m_oFilePropertyList, m_bReplaceEnvironmentVariables,
          m_sFileVariableSectionName, GetUseCaseSensitiveKeys())) {
    m_sFilename    = sFilename;
    m_bFileIsValid = true;
    return true;
  }
  m_bFileIsValid = false;
  return false;
}

bool VistaIniFileParser::ReadFile(
    const std::string& sFilename, std::list<std::string>& liFileSearchPathes) {
  m_bFileIsValid = ReadProplistFromFile(sFilename, liFileSearchPathes, m_oFilePropertyList,
      m_sFilename, m_bReplaceEnvironmentVariables, m_sFileVariableSectionName,
      GetUseCaseSensitiveKeys(), m_cSectionHeaderStartSymbol, m_cSectionHeaderEndSymbol,
      m_cKeySeparatorSymbol, m_cCommentSymbol);
  return m_bFileIsValid;
}

bool VistaIniFileParser::WriteFile() {
  if (m_bFileIsValid == false)
    return false;
  if (WriteProplistToFile(m_sFilename, m_oFilePropertyList, true, m_cSectionHeaderStartSymbol,
          m_cSectionHeaderEndSymbol, m_cKeySeparatorSymbol)) {
    m_bFileIsValid = true;
    return true;
  }
  m_bFileIsValid = false;
  return false;
}
bool VistaIniFileParser::WriteFile(const std::string& sFilename, const bool bOverwrite) {
  if (WriteProplistToFile(sFilename, m_oFilePropertyList, bOverwrite, m_cSectionHeaderStartSymbol,
          m_cSectionHeaderEndSymbol, m_cKeySeparatorSymbol)) {
    m_sFilename    = sFilename;
    m_bFileIsValid = true;
    return true;
  }
  m_bFileIsValid = false;
  return false;
}

std::string VistaIniFileParser::GetFilename() const {
  return m_sFilename;
}

bool VistaIniFileParser::GetIsValidFile() const {
  return m_bFileIsValid;
}

VistaPropertyList& VistaIniFileParser::GetPropertyList() {
  return m_oFilePropertyList;
}
const VistaPropertyList& VistaIniFileParser::GetPropertyList() const {
  return m_oFilePropertyList;
}

void VistaIniFileParser::SetPropertyList(const VistaPropertyList& oList) {
  m_oFilePropertyList = oList;
}

bool VistaIniFileParser::ReadProplistFromFile(const std::string& sFilename,
    VistaPropertyList& oTarget, const bool bReplaceEnvironmentVariables,
    const std::string& sFileVariableSectionName, const bool bCaseSensitiveKeys,
    const char cSectionHeaderStartSymbol, const char cSectionHeaderEndSymbol,
    const char cKeyAssignmentSymbol, const char cCommentSymbol) {
  if (CheckFileExists(sFilename) == false) {
    vstr::warnp() << "[VistaIniFileParser]: Could not find file [" << sFilename << "]" << std::endl;
    return false;
  }

  oTarget.clear();
  oTarget.SetIsCaseSensitive(bCaseSensitiveKeys);

  IniFileReader oReader(
      cSectionHeaderStartSymbol, cSectionHeaderEndSymbol, cKeyAssignmentSymbol, cCommentSymbol);
  return oReader.ReadFile(
      sFilename, oTarget, bReplaceEnvironmentVariables, sFileVariableSectionName);
}
VistaPropertyList VistaIniFileParser::ReadProplistFromFile(const std::string& sFilename,
    const bool bReplaceEnvironmentVariables, const std::string& sFileVariableSectionName,
    const bool bCaseSensitiveKeys, const char cSectionHeaderStartSymbol,
    const char cSectionHeaderEndSymbol, const char cKeyAssignmentSymbol,
    const char cCommentSymbol) {
  VistaPropertyList oList;
  ReadProplistFromFile(sFilename, oList, bReplaceEnvironmentVariables, sFileVariableSectionName,
      bCaseSensitiveKeys, cSectionHeaderStartSymbol, cSectionHeaderEndSymbol, cKeyAssignmentSymbol,
      cCommentSymbol);
  return oList;
}
bool VistaIniFileParser::ReadProplistFromFile(const std::string& sFilename,
    const std::list<std::string>& liFileSearchPathes, VistaPropertyList& oTarget,
    std::string& sFullLoadedFile, const bool bReplaceEnvironmentVariables,
    const std::string& sFileVariableSectionName, const bool bCaseSensitiveKeys,
    const char cSectionHeaderStartSymbol, const char cSectionHeaderEndSymbol,
    const char cKeyAssignmentSymbol, const char cCommentSymbol) {
  oTarget.clear();
  oTarget.SetIsCaseSensitive(bCaseSensitiveKeys);

  for (std::list<std::string>::const_iterator itPath = liFileSearchPathes.begin();
       itPath != liFileSearchPathes.end(); ++itPath) {
    std::string sExtFilename = (*itPath) + "/" + sFilename;
    if (CheckFileExists(sExtFilename)) {
      IniFileReader oReader(
          cSectionHeaderStartSymbol, cSectionHeaderEndSymbol, cKeyAssignmentSymbol, cCommentSymbol);
      if (oReader.ReadFile(
              sExtFilename, oTarget, bReplaceEnvironmentVariables, sFileVariableSectionName)) {
        sFullLoadedFile = sExtFilename;
        return true;
      }
      return false;
    }
  }
  vstr::warnp() << "[VistaIniFileParser]: Could not find file [" << sFilename
                << "] in any of the directories:";
  vstr::IndentObject oIndent;
  for (std::list<std::string>::const_iterator itPath = liFileSearchPathes.begin();
       itPath != liFileSearchPathes.end(); ++itPath) {
    vstr::warni() << (*itPath) << "\n";
  }
  vstr::warn().flush();
  return false;
}
VistaPropertyList VistaIniFileParser::ReadProplistFromFile(const std::string& sFilename,
    const std::list<std::string>& liFileSearchPathes, std::string& sFullLoadedFile,
    const bool bReplaceEnvironmentVariables, const std::string& sFileVariableSectionName,
    const bool bCaseSensitiveKeys, const char cSectionHeaderStartSymbol,
    const char cSectionHeaderEndSymbol, const char cKeyAssignmentSymbol,
    const char cCommentSymbol) {
  VistaPropertyList oList;
  ReadProplistFromFile(sFilename, liFileSearchPathes, oList, sFullLoadedFile,
      bReplaceEnvironmentVariables, sFileVariableSectionName, bCaseSensitiveKeys,
      cSectionHeaderStartSymbol, cSectionHeaderEndSymbol, cKeyAssignmentSymbol, cCommentSymbol);
  return oList;
}

bool VistaIniFileParser::WriteProplistToFile(const std::string& sFilename,
    const VistaPropertyList& oSource, const bool bOverwriteExistingFile,
    const char cSectionHeaderStartSymbol, const char cSectionHeaderEndSymbol,
    const char cKeyAssignmentSymbol) {
  if (CheckFileExists(sFilename) && bOverwriteExistingFile == false) {
    vstr::warnp() << " VistaIniFileParser::WriteFile() -- "
                  << "File [" << sFilename << "] already exists, write cancelled" << std::endl;
    return false;
  }

  IniFileWriter oWriter(
      20, cSectionHeaderStartSymbol, cSectionHeaderEndSymbol, cKeyAssignmentSymbol);
  return oWriter.WriteFile(sFilename, oSource);
}

std::string VistaIniFileParser::GetFileVariableSectionName() const {
  return m_sFileVariableSectionName;
}

void VistaIniFileParser::SetFileVariableSectionName(const std::string& oValue) {
  m_sFileVariableSectionName = oValue;
}
