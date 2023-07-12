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

#include "VistaMasterDataTunnel.h"

#include <VistaInterProcComm/DataLaVista/Base/VistaActiveFilter.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataConsumer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataProducer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>

#include <VistaInterProcComm/DataLaVista/Common/VistaNetDataDispatcher.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaProfilingFilter.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaRamQueuePipe.h>
#include <VistaInterProcComm/DataLaVista/Common/VistaSerialTeeFilter.h>

#include <VistaInterProcComm/DataLaVista/Construct/VistaCheckError.h>
#include <VistaInterProcComm/DataLaVista/Construct/VistaPipeConstructionKit.h>

using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMasterDataTunnel::VistaMasterDataTunnel(
    const std::vector<std::string>& vecHostNames, const std::vector<int>& vecClientPorts)
    : m_pTeeFilter(new DLVistaSerialTeeFilter)
    , m_pProfiler(new DLVistaProfilingFilter())
    , m_pDispatcher(new DLVistaNetDataDispatcher)
    , m_pActivator(new DLVistaActiveFilter(m_pProfiler))
    , m_pQueue(new DLVistaRamQueuePipe()) {
  // NOTE: Sanity checks have been performed by VistaDataTunnelFactory beforehand...
  for (unsigned int i = 0; i < vecHostNames.size(); ++i) {
    m_pDispatcher->AddClient(vecHostNames[i], vecClientPorts[i]);
  }
  // be defensive about dropped packets
  m_pQueue->SetDoesBlockOnReturnPacket(true);

  // join the internal pipe -> all downstream packets will be mirrored to all connected clients
  DLVistaPipeConstructionKit::Join(m_pProfiler, m_pTeeFilter);
  DLVistaPipeConstructionKit::Join(m_pTeeFilter, m_pDispatcher);
  DLVistaPipeConstructionKit::Join(m_pTeeFilter, m_pQueue);

  m_pActivator->SetThreadName("MasterDataTunnelDSActivator");
  // m_pActivator->StartComponent();
}

VistaMasterDataTunnel::VistaMasterDataTunnel(const std::vector<VistaConnectionIP*>& vecConnections)
    : m_pTeeFilter(new DLVistaSerialTeeFilter)
    , m_pProfiler(new DLVistaProfilingFilter())
    , m_pDispatcher(new DLVistaNetDataDispatcher)
    , m_pActivator(new DLVistaActiveFilter(m_pProfiler))
    , m_pQueue(new DLVistaRamQueuePipe()) {
  for (std::vector<VistaConnectionIP*>::const_iterator itCon = vecConnections.begin();
       itCon != vecConnections.end(); ++itCon) {
    m_pDispatcher->AddClient((*itCon));
  }
  // be defensive about dropped packets
  m_pQueue->SetDoesBlockOnReturnPacket(true);

  // join the internal pipe -> all downstream packets will be mirrored to all connected clients
  DLVistaPipeConstructionKit::Join(m_pProfiler, m_pTeeFilter);
  DLVistaPipeConstructionKit::Join(m_pTeeFilter, m_pDispatcher);
  DLVistaPipeConstructionKit::Join(m_pTeeFilter, m_pQueue);

  m_pActivator->SetThreadName("MasterDataTunnelDSActivator");
  // m_pActivator->StartComponent();
}

VistaMasterDataTunnel::~VistaMasterDataTunnel() {
  m_pActivator->SetStopOnDestruct(false);
  m_pActivator->StopComponent(true);

  this->DisconnectDownstreamInput();
  this->DisconnectDownstreamOutput();
  this->DisconnectUpstreamInput();
  this->DisconnectDownstreamOutput();

  delete m_pActivator;
  delete m_pTeeFilter;
  delete m_pDispatcher;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaMasterDataTunnel::ConnectUpstreamInput(IDLVistaPipeComponent* pInput) {
  if (this->GetUpstreamInput())
    this->DisconnectUpstreamInput();

  // remember the pipe component
  this->SetUpstreamInput(pInput);
  // check if there is already a consumer
  if (this->GetUpstreamOutput()) {
    // connect to consumer
    DLVistaPipeConstructionKit::Join(pInput, this->GetUpstreamOutput());
  }
  return true;
}

bool VistaMasterDataTunnel::ConnectDownstreamInput(IDLVistaPipeComponent* pInput) {
  if (this->GetDownstreamInput())
    this->DisconnectDownstreamInput();

  // remember the pipe component
  this->SetDownstreamInput(pInput);
  // get packets to the tee filter
  // DLVistaPipeConstructionKit::Join(pInput, m_pTeeFilter);
  DLVistaPipeConstructionKit::Join(pInput, m_pProfiler);
  return true;
}

bool VistaMasterDataTunnel::ConnectUpstreamOutput(IDLVistaPipeComponent* pOutput) {
  if (this->GetUpstreamOutput())
    this->DisconnectUpstreamOutput();

  this->SetUpstreamOutput(pOutput);
  if (this->GetUpstreamInput()) {
    DLVistaPipeConstructionKit::Join(this->GetUpstreamInput(), pOutput);
  }
  return true;
}

bool VistaMasterDataTunnel::ConnectDownstreamOutput(IDLVistaPipeComponent* pOutput) {
  if (this->GetDownstreamOutput())
    this->DisconnectDownstreamOutput();

  this->SetDownstreamOutput(pOutput);
  DLVistaPipeConstructionKit::Join(m_pQueue, pOutput);
  return true;
}

bool VistaMasterDataTunnel::DisconnectUpstreamInput() {
  /*
  Since the upstream consumer is directly connected to the producing pipe,
  this means disconnect the incoming from our upstream consumer
  --- I know, this looks kinda stupid, but we need something like this
          to hide the internal data tunnel pipe in the cluster case...
  */
  IDLVistaPipeComponent* pOutput = this->GetUpstreamOutput();
  if (pOutput) {
    IDLVistaPipeComponent* pIncoming = pOutput->GetInboundByIndex(0);
    if (pIncoming) {
      pOutput->DetachInputComponent(pIncoming);
      pIncoming->DetachOutputComponent(pOutput);
      this->SetUpstreamInput(NULL);
      return true;
    }
  }
  return false;
}
bool VistaMasterDataTunnel::DisconnectUpstreamOutput() {
  /*
  Since the upstream producer is directly connected to the consuming pipe,
  this means disconnect the outgoing from our upstream producer
  */
  IDLVistaPipeComponent* pInput = this->GetUpstreamInput();
  if (pInput) {
    IDLVistaPipeComponent* pOutgoing = pInput->GetOutboundByIndex(0);
    if (pOutgoing) {
      pInput->DetachInputComponent(pOutgoing);
      pOutgoing->DetachOutputComponent(pInput);
      this->SetUpstreamOutput(NULL);
      return true;
    }
  }
  return false;
}
bool VistaMasterDataTunnel::DisconnectDownstreamInput() {
  /*
  This means disconnecting the input from out TEE-Filter
  */
  IDLVistaPipeComponent* pIncoming = m_pTeeFilter->GetInboundByIndex(0);
  if (pIncoming && pIncoming == this->GetDownstreamInput()) {
    m_pTeeFilter->DetachInputComponent(pIncoming);
    pIncoming->DetachOutputComponent(m_pTeeFilter);
    this->SetDownstreamInput(NULL);
    return true;
  }
  return false;
}
bool VistaMasterDataTunnel::DisconnectDownstreamOutput() {
  /*
   * this means disconnecting the the outbound component form the TEE
   * FILTER
   */
  IDLVistaPipeComponent* pOutgoing = m_pQueue->GetOutboundByIndex(0);
  if (pOutgoing && pOutgoing == this->GetDownstreamOutput()) {
    pOutgoing->DetachInputComponent(m_pQueue);
    m_pTeeFilter->DetachOutputComponent(pOutgoing);
    this->SetDownstreamOutput(NULL);
    return true;
  }
  return false;
}

void VistaMasterDataTunnel::StartTunneling() {
  m_pActivator->StartComponent();
}

void VistaMasterDataTunnel::StopTunnelingGently(bool bJoin) {
  m_pActivator->StopComponentGently(bJoin);
}

void VistaMasterDataTunnel::HaltTunneling() {
  m_pActivator->HaltComponent();
}

void VistaMasterDataTunnel::IndicateTunnelingEnd() {
  m_pActivator->IndicateFilteringEnd();
}

void VistaMasterDataTunnel::SignalPendingRequest() {
  m_pQueue->SignalPendingRequest();
}
