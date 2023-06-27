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

#include "VistaMasterSlaveUtils.h"

#include <VistaKernel/EventManager/VistaEvent.h>

#include <VistaAspects/VistaConversion.h>
#include <VistaAspects/VistaDeSerializer.h>
#include <VistaAspects/VistaSerializer.h>

#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaMasterSlave::Message::Message()
    : m_nType(CMSG_INVALID)
    , m_nFrameCount(-1)
    , m_nClock(0)
    , m_pSendEvent(NULL) {
}

VistaMasterSlave::Message::~Message() {
}

bool VistaMasterSlave::Message::RegisterEventType(VistaEvent* pEvent) {
  if (m_mapRegisteredEvents.find(pEvent->GetType()) != m_mapRegisteredEvents.end())
    return false;
  m_mapRegisteredEvents[pEvent->GetType()] = pEvent;
  return true;
}

int VistaMasterSlave::Message::Serialize(IVistaSerializer& oSer) const {
  int nRet = 0;
  nRet += oSer.WriteInt32(m_nFrameCount);
  nRet += oSer.WriteDouble(m_nClock);
  nRet += oSer.WriteInt32(m_nType);
  switch (m_nType) {
  case CMSG_EVENT: {
    if (m_pSendEvent == NULL)
      VISTA_THROW("VistaMasterSlaveUtils invalid during serialization", -1);
    nRet += oSer.WriteInt32(m_nEventType);
    nRet += oSer.WriteSerializable(*m_pSendEvent);
    break;
  }
  case CMSG_STARTFRAME:
  case CMSG_ENDFRAME: {
    // nothing to do
    break;
  }
  case CMSG_QUIT: {
    // nothing to do
    break;
  }
  case CMSG_INVALID:
  default: {
    VISTA_THROW("VistaMasterSlaveUtils invalid during serialization", -1);
  }
  }
  return nRet;
}

int VistaMasterSlave::Message::DeSerialize(IVistaDeSerializer& oDeSer) {
  int               nRet = 0;
  VistaType::sint32 nDummy;
  nRet += oDeSer.ReadInt32(nDummy);
  m_nFrameCount = nDummy;
  nRet += oDeSer.ReadDouble(m_nClock);
  nRet += oDeSer.ReadInt32(nDummy);
  m_nType = (CMSG_TYPE)nDummy;

  switch (m_nType) {
  case CMSG_EVENT: {
    nRet += oDeSer.ReadInt32(nDummy);
    m_nEventType                                 = nDummy;
    std::map<int, VistaEvent*>::iterator itEvent = m_mapRegisteredEvents.find(m_nEventType);
    if (itEvent == m_mapRegisteredEvents.end()) {
      VISTA_THROW("VistaMasterSlaveUtils: invalid event type during deserialization", -1);
      break;
    }
    nRet += oDeSer.ReadSerializable(*(*itEvent).second);
    break;
  }
  case CMSG_STARTFRAME:
  case CMSG_ENDFRAME:
  case CMSG_QUIT: {
    // nothing to do
    break;
  }
  case CMSG_INVALID:
  default: {
    VISTA_THROW("VistaMasterSlaveUtils invalid during deserialization", -1);
  }
  }
  return nRet;
}

std::string VistaMasterSlave::Message::GetSignature() const {
  return "VistaMasterSlave::Message";
}

VistaMasterSlave::Message::CMSG_TYPE VistaMasterSlave::Message::GetType() const {
  return m_nType;
}

int VistaMasterSlave::Message::GetFrameCount() const {
  return m_nFrameCount;
}

VistaType::systemtime VistaMasterSlave::Message::GetClock() const {
  return m_nClock;
}

VistaEvent* VistaMasterSlave::Message::GetEvent() const {
  std::map<int, VistaEvent*>::const_iterator itEvent = m_mapRegisteredEvents.find(m_nEventType);
  if (itEvent == m_mapRegisteredEvents.end()) {
    VISTA_THROW("VistaMasterSlaveUtils: requested invalid event", -1);
  }
  return (*itEvent).second;
}

void VistaMasterSlave::Message::SetFrameCount(int nCount) {
  m_nFrameCount = nCount;
}

void VistaMasterSlave::Message::SetEventMsg(const VistaEvent* pEvent) {
  m_nType      = CMSG_EVENT;
  m_nEventType = pEvent->GetType();
  m_nClock     = pEvent->GetTime();
  m_pSendEvent = pEvent;
}

void VistaMasterSlave::Message::SetStartFrameMsg(VistaType::systemtime nTime) {
  m_nType  = CMSG_STARTFRAME;
  m_nClock = nTime;
}

void VistaMasterSlave::Message::SetEndFrameMsg(VistaType::systemtime nTime) {
  m_nType  = CMSG_ENDFRAME;
  m_nClock = nTime;
}

void VistaMasterSlave::Message::SetQuitMsg(VistaType::systemtime nTime) {
  m_nType  = CMSG_QUIT;
  m_nClock = nTime;
}

void VistaMasterSlave::FillPortListFromIni(const std::string& sEntry, FreePortList& vecPorts) {
  // read available ports - these can be used for connections between slave and master
  std::vector<std::string> liPortRangeStrings;
  if (VistaConversion::FromString(sEntry, liPortRangeStrings) == false) {
    vstr::warnp() << "Cannot interprete port list string [" << sEntry << "]" << std::endl;
    return;
  }
  for (std::vector<std::string>::const_iterator itRange = liPortRangeStrings.begin();
       itRange != liPortRangeStrings.end(); ++itRange) {
    size_t nDashPos = (*itRange).find('-');
    if (nDashPos == std::string::npos) {
      // no range, just a single port
      int iPort;
      if (VistaConversion::FromString((*itRange), iPort))
        vecPorts.push_back(std::pair<int, int>(iPort, iPort));
    } else {
      std::string sMin = (*itRange).substr(0, nDashPos);
      std::string sMax = (*itRange).substr(nDashPos + 1, std::string::npos);
      int         nMin = 0;
      int         nMax = 0;
      if (VistaConversion::FromString(sMin, nMin) && VistaConversion::FromString(sMax, nMax)) {
        if (nMax < nMin) {
          vstr::warnp() << "Received invalid free port range [" << nMin << "-" << nMax << "]"
                        << std::endl;
          continue;
        }
      } else {
        vstr::warnp() << "Cannot parse port range [" << (*itRange) << "]" << std::endl;
        continue;
      }
      vecPorts.push_back(PortRange(nMin, nMax));
    }
  }
}

int VistaMasterSlave::GetFreePortFromPortList(FreePortList& vecPorts) {
  if (vecPorts.empty())
    return -1;
  int nNewIP = vecPorts.front().first;
  if (nNewIP >= vecPorts.front().second) // check if it's the last free ip in this block
    vecPorts.pop_front();
  else
    ++vecPorts.front().first; // increment starting ip in this block (i.e. claim current first)
  return nNewIP;
}

std::string VistaMasterSlave::GetDataSyncModeName(const int m_nDataSyncMethod) {
  switch (m_nDataSyncMethod) {
  case DATASYNC_TCP:
    return "TCP/IP";
  case DATASYNC_ZEROMQ:
    return "ZeroMQ";
  case DATASYNC_INTERPROC:
    return "InterProc";
  case DATASYNC_DUMMY:
    return "Dummy";
  }
  return "Unknown";
}

std::string VistaMasterSlave::GetSwapSyncModeName(const int m_nSwapSyncMethod) {
  switch (m_nSwapSyncMethod) {
  case SWAPSYNC_TCP:
    return "TCP/IP";
  case SWAPSYNC_BROADCAST:
    return "Broadcast";
  case SWAPSYNC_INTERPROC:
    return "InterProc";
  case SWAPSYNC_GSYNCGROUP:
    return "nVidia GSYNC";
  case SWAPSYNC_DUMMY:
    return "Dummy";
  case SWAPSYNC_NONE:
    return "None";
  case SWAPSYNC_ZEROMQ:
    return "ZeroMQ";
  case SWAPSYNC_DEFAULTBARRIER:
    return "Default Barrier";
  }
  return "Unknown";
}

std::string VistaMasterSlave::GetBarrierModeName(const int m_nBarrierMethod) {
  switch (m_nBarrierMethod) {
  case BARRIER_TCP:
    return "TCP/IP";
  case BARRIER_DUMMY:
    return "Dummy";
  case BARRIER_INTERPROC:
    return "InterProc";
  case BARRIER_BROADCAST:
    return "Broadcast";
  case BARRIER_ZEROMQ:
    return "ZeroMQ";
  }
  return "Unknown";
}
