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

#include "VistaProtocol.h"

#include "VistaEnvironment.h"
#include <VistaBase/VistaTimer.h>

#include <VistaBase/VistaStreamUtils.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaProtocol::VistaProtocol() {
  m_Timer          = new VistaTimer;
  m_sDateOfMeasure = VistaTimeUtils::ConvertToLexicographicDateString(m_Timer->GetSystemTime());
  m_bOverwrite     = true;
  m_bWriteHeader   = true;
  m_bWriteKey      = true;
  m_bWriteColumnHeaders = true;

  m_cSeparator = '\t';
}

VistaProtocol::~VistaProtocol() {
  delete m_Timer;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*NAME :  StartMeasureByName */
// creates new time measurement in measuring list and start a measuring by its name

void VistaProtocol::StartMeasureByName(const std::string& sName) {
  double a = m_Timer->GetSystemTime();
  if (m_mpMeasureMap.find(sName) == m_mpMeasureMap.end()) {
    m_mpMeasureMap[sName] = TimeInterval(a);
  } else {
    vstr::warnp() << "\nVistaProtocol[<" << m_sFileName
                  << ">]::StartMeasureByName() :" << sName.c_str() << " is already started."
                  << std::endl;
  }
}

void VistaProtocol::StartMeasureByKey(int iKey) {
  double a = m_Timer->GetSystemTime();
  if (m_mpMsrMap.find(iKey) == m_mpMsrMap.end()) {
    m_mpMsrMap[iKey] = TimeInterval(a);
  } else {
    vstr::warnp() << "\nVistaProtocol[<" << m_sFileName << ">]::StartMeasureByKey() :[" << iKey
                  << "] is already started." << std::endl;
  }
}

void VistaProtocol::StopMeasureByKey(int iKey) {
  double                                a = m_Timer->GetSystemTime();
  std::map<int, TimeInterval>::iterator i = m_mpMsrMap.find(iKey);
  if (i == m_mpMsrMap.end()) {
    vstr::warnp() << "\nVistaProtocol[<" << m_sFileName << ">]::StopMeasureByKey(): [" << iKey
                  << "] : No stop without start." << std::endl;
  } else {
    if (i->second.dEnd == 0) {
      i->second.dEnd = a;
    } else {
      vstr::warnp() << "\nVistaProtocol[<" << m_sFileName << ">]::StopMeasureByKey(): [" << iKey
                    << "] is already stopped." << std::endl;
    }
  }
}

/*NAME :  StopMeasureByName */
// stop a measuring by its name

void VistaProtocol::StopMeasureByName(const std::string& sName) {
  double                                        a = m_Timer->GetSystemTime();
  std::map<std::string, TimeInterval>::iterator i = m_mpMeasureMap.find(sName);
  if (i == m_mpMeasureMap.end()) {
    vstr::warnp() << "\nVistaProtocol[<" << m_sFileName
                  << ">]::StopMeasureByName(): " << sName.c_str() << " : No stop without start."
                  << std::endl;
  } else {
    if (i->second.dEnd == 0) {
      i->second.dEnd = a;
    } else {
      vstr::warnp() << "\nVistaProtocol[<" << m_sFileName
                    << ">]::StopMeasureByName(): " << sName.c_str() << " is already stopped."
                    << std::endl;
    }
  }
}

/*NAME :  GetTimeByName */
// get the actual runtime of measuring by its name

double VistaProtocol::GetIntervalByName(const std::string& sName) {
  double                                              a = m_Timer->GetSystemTime();
  std::map<std::string, TimeInterval>::const_iterator i = m_mpMeasureMap.find(sName);
  if (i == m_mpMeasureMap.end()) {
    vstr::warnp() << "\nVistaProtocol[<" << m_sFileName
                  << ">]::GetIntervalByName(): " << sName.c_str() << " : does not exist."
                  << std::endl;
    return -1;
  } else {
    return ((*i).second.interval(a));
  }
}

/*NAME :  SetFileName */
// set output filename

void VistaProtocol::SetFileName(const std::string& sFileName) {
  m_sFileName = sFileName;
}

/*NAME :  SetOutputInfo */
// set string-info for output of measurings from measuring list

void VistaProtocol::SetUserText(const std::string& sText) {
  m_sUserText = sText;
}

/*NAME : SetOverwrite */
// set file openmode : true - for overwrite, false - for append

void VistaProtocol::SetOverwrite(bool bOverwrite) {
  m_bOverwrite = bOverwrite;
}

/*NAME : GetOverwrite */

bool VistaProtocol::GetOverwrite() const {
  return m_bOverwrite;
}

void VistaProtocol::SetPrintHeader(bool bPrintHeader) {
  m_bWriteHeader = bPrintHeader;
}

bool VistaProtocol::GetPrintHeader() const {
  return m_bWriteHeader;
}

/*NAME :  WriteProtocol */
// writes output file

bool VistaProtocol::WriteProtocol() {

  if (m_sFileName != "") {
    std::ofstream Protocol;
    Protocol.open(m_sFileName.c_str(), (GetOverwrite() ? std::ios::out : std::ios::app));
    if (!Protocol.good()) // sanity check
    {
      vstr::warnp() << "VistaProtocol::WriteProtocol() -- could not open protocol file ["
                    << m_sFileName.c_str() << "]" << std::endl;
      return false;
    } else {

      if (GetPrintHeader()) {
        /*Output of header for protocol*/
        Protocol << "========================================================\n";
        Protocol << "Hostname              : " << VistaEnvironment::GetHostname() << std::endl;
        Protocol << "Operating System      : " << VistaEnvironment::GetOSystem() << std::endl;
        Protocol << "CPU-Type              : " << VistaEnvironment::GetCPUType() << std::endl;
        Protocol << "Number of Processors  : " << VistaEnvironment::GetNumberOfProcessors()
                 << std::endl;
        Protocol << "Memory                : " << VistaEnvironment::GetMemory() << std::endl;
        Protocol << "Date of measure       : " << m_sDateOfMeasure << std::endl;
        if (m_sUserText != "")
          Protocol << "User-Text             : " << m_sUserText << std::endl;

        Protocol << "Compile date (TOOLS): " << VistaEnvironment::GetCompileDate() << std::endl;
        Protocol << "Compile time (TOOLS): " << VistaEnvironment::GetCompileTime() << std::endl;
        Protocol << "Compile flag SYSTEM : " << VistaEnvironment::GetCompileFlagSYSTEM()
                 << std::endl
                 << std::endl;
        Protocol << "========================================================\n";
      };

      /* Write measurements in protocol*/
      std::map<std::string, TimeInterval>::const_iterator i;
      if (m_bWriteColumnHeaders) {
        Protocol << "[Name]/[Key]" << m_cSeparator << "[Begintime (s)]" << m_cSeparator
                 << "[Endtime (s)]" << m_cSeparator << "[Interval-length (s)]\n";
      }

      for (i = m_mpMeasureMap.begin(); i != m_mpMeasureMap.end(); ++i) {
        if (i->second.dBegin != 0 && i->second.dEnd != 0) {
          Protocol << "\n"
                   << (*i).first.c_str() << m_cSeparator << (*i).second.dBegin << m_cSeparator
                   << (*i).second.dEnd << m_cSeparator
                   << (*i).second.interval(0.0) // end-time is present!
                   << std::endl;
        }
      }

      if (!m_mpMeasureMap.empty()) // mark line
        Protocol << std::endl;

      std::map<int, TimeInterval>::const_iterator it;
      for (it = m_mpMsrMap.begin(); it != m_mpMsrMap.end(); ++it) {
        if (it->second.dBegin != 0 && it->second.dEnd != 0) {

          if (m_bWriteKey) {
            Protocol << (*it).first << m_cSeparator;
          }
          Protocol << it->second.dBegin << m_cSeparator << it->second.dEnd << m_cSeparator
                   << it->second.interval(0.0) // end-time is present!
                   << std::endl;
        }
      }

      Protocol.close();
      return true;
    }
  } else {
    vstr::warnp() << "\nVistaProtocol::WriteProtocol(): No filename given for protocol-file!"
                  << std::endl;
    return false;
  }
}

void VistaProtocol::SetSeparator(char cSep) {
  m_cSeparator = cSep;
}

char VistaProtocol::GetSeparator() const {
  return m_cSeparator;
}

void VistaProtocol::SetWriteKey(bool bWriteKey) {
  m_bWriteKey = bWriteKey;
}

bool VistaProtocol::GetWriteKey() const {
  return m_bWriteKey;
}

void VistaProtocol::SetWriteColumnHeaders(bool bWriteColHeads) {
  m_bWriteColumnHeaders = bWriteColHeads;
}

bool VistaProtocol::GetWriteColumnHeaders() const {
  return m_bWriteColumnHeaders;
}
