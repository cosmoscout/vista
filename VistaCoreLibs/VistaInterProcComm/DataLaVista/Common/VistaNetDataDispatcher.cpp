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

#include "VistaNetDataDispatcher.h"

#include <VistaAspects/VistaSerializable.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>

#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
DLVistaNetDataDispatcher::DLVistaNetDataDispatcher() {
}

DLVistaNetDataDispatcher::~DLVistaNetDataDispatcher() {
  for (unsigned int i = 0; i < m_vecClients.size(); ++i) {
    m_vecClients[i]->Close();
    delete m_vecClients[i];
  }
  m_vecClients.clear();
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool DLVistaNetDataDispatcher::ConsumePacket(IDLVistaDataPacket* p) {
  bool bSuccess = p->IsValid();
  if (bSuccess) {
    VistaByteBufferSerializer oSer(p->GetDataSize());
    oSer.WriteSerializable(*p);
    const VistaType::byte* pBuffer = oSer.GetBuffer();
    const int              iSize   = oSer.GetBufferSize();
#ifdef DEBUG
    vstr::outi() << "*** Dispatching packet" << std::endl;
#endif
    for (unsigned int i = 0; i < m_vecClients.size(); ++i) {
      bSuccess &= (m_vecClients[i]->WriteRawBuffer(pBuffer, iSize) != -1);
    }
  }
  m_pDataInput->RecycleDataPacket(p, this, true);
  return bSuccess;
}

bool DLVistaNetDataDispatcher::AddClient(const string& sHost, const int iPort) {
  // try to connect to client
  VistaConnectionIP* pNewConn = new VistaConnectionIP(VistaConnectionIP::CT_TCP, sHost, iPort);
  // push connection to the vector
  if (pNewConn->GetIsConnected()) {
    pNewConn->SetIsBlocking(true);
    m_vecClients.push_back(pNewConn);
    return true;
  } else {
    vstr::warnp() << "DLVistaNetDataDispatcher::AddClient() --"
                  << "Failed to establish client connecion to IP [" << sHost << "] - Port ["
                  << iPort << "]" << std::endl;
    return false;
  }
}

bool DLVistaNetDataDispatcher::AddClient(VistaConnectionIP* pConnection) {
  if (pConnection->GetIsConnected()) {
    if (pConnection->GetIsBlocking() == false)
      pConnection->SetIsBlocking(true);
    m_vecClients.push_back(pConnection);
    return true;
  } else {
    vstr::warnp() << "DLVistaNetDataDispatcher::AddClient() --"
                  << "Received client connection is not connected" << std::endl;
    return false;
  }
}
