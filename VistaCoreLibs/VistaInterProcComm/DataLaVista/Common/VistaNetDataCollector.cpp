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

#include "VistaNetDataCollector.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>

#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>

#include <VistaInterProcComm/IPNet/VistaSocket.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>

#include <VistaAspects/VistaSerializable.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cassert>
#include <cstdio>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
DLVistaNetDataCollector::DLVistaNetDataCollector(
    const string& sHostName, int iPort, IDLVistaDataPacket* pPacketProtoype) {
  // make a copy to ensure proper deletion
  m_pPacketPrototype = pPacketProtoype->CreateInstance(this);

  VistaTCPServer server(sHostName, iPort);
  if (server.GetIsValid()) {
    vstr::outi() << "[DLVistaNetDataCollector] Connecting to master...";
    // this call will block
    VistaTCPSocket* serverPlug = server.GetNextClient();
    m_pMasterConnection        = new VistaConnectionIP(serverPlug);
    m_pMasterConnection->SetIsBlocking(true);
    vstr::out() << "DONE!" << std::endl;
  } else {
    vstr::errp() << "[DLVistaNetDataCollector] ERROR: Unable to open server socket for incoming "
                    "connection from MASTER..."
                 << std::endl;
    VISTA_THROW("[DLVistaNetDataCollector] ERROR: Unable to open server socket for incoming "
                "connection from MASTER...",
        0x00000001);
  }
}

DLVistaNetDataCollector::DLVistaNetDataCollector(
    VistaConnectionIP* pConnection, IDLVistaDataPacket* pPacketProtoype) {
  // make a copy to ensure proper deletion
  m_pPacketPrototype = pPacketProtoype->CreateInstance(this);

  m_pMasterConnection = pConnection;
  m_pMasterConnection->SetIsBlocking(true);
}

DLVistaNetDataCollector::~DLVistaNetDataCollector() {
  m_pMasterConnection->Close();
  delete m_pMasterConnection;
  delete m_pPacketPrototype;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IDLVistaDataPacket* DLVistaNetDataCollector::CreatePacket() {
  return m_pPacketPrototype->CreateInstance(this);
}

void DLVistaNetDataCollector::FillPacket(IDLVistaDataPacket* p) {
  p->SetIsValid(false);
  if (m_pMasterConnection->GetIsOpen() == false) {
#ifdef DEBUG
    printf("[DLVistaNetDataCollector::FillPacket] -- master socket is closed \n");
#endif
    return;
  }
  /*
  if we are in blocking mode -> issue blocking read call anyway
  if we are in non-blocking mode -> issue call only if there is pending data
  */
  bool bReadOK =
      (m_pMasterConnection->GetIsBlocking() ? true : m_pMasterConnection->HasPendingData());
#if defined(VISTA_IPC_USE_EXCEPTIONS)
  // there could be an exception during the read operation
  try {
#endif
    // we have to read "something" (>0 bytes) to get a valid packet
    if (bReadOK && m_pMasterConnection->ReadSerializable(*p) > 0) {
      p->SetIsValid(true);
    } else {
#ifdef DEBUG
      printf("[DLVistaNetDataCollector::FillPacket] -- failed to read packet \n");
#endif
    }
#if defined(VISTA_IPC_USE_EXCEPTIONS)
  } catch (VistaExceptionBase& oE) {
    oE.PrintException();
    printf("\n[DLVistaNetDataCollector::FillPacket] -- exception during reception \
			   \n\tclosing socket to master!\n");
    m_pMasterConnection->Close();
  }
#endif
}

bool DLVistaNetDataCollector::HasPacket() const {
  return true; // or !m_pPacketQueue->empty()???
}

void DLVistaNetDataCollector::DeletePacket(IDLVistaDataPacket* p) {
  delete p;
}

void DLVistaNetDataCollector::SetIsBlocking(bool b) {
  m_pMasterConnection->SetIsBlocking(b);
}

bool DLVistaNetDataCollector::GetIsBlocking() const {
  return m_pMasterConnection->GetIsBlocking();
}
