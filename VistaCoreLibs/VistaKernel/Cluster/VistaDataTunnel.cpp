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

#include "VistaDataTunnel.h"
#include <VistaInterProcComm/DataLaVista/Base/VistaDataConsumer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataPacket.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDataProducer.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>
#include <cassert>

using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaDataTunnel::IVistaDataTunnel()
    : m_pUpstreamInput(0)
    , m_pUpstreamOutput(0)
    , m_pDownstreamInput(0)
    , m_pDownstreamOutput(0) {
}

IVistaDataTunnel::IVistaDataTunnel(const IVistaDataTunnel& obj)
    : m_pUpstreamInput(obj.m_pUpstreamInput)
    , m_pUpstreamOutput(obj.m_pUpstreamOutput)
    , m_pDownstreamInput(obj.m_pDownstreamInput)
    , m_pDownstreamOutput(obj.m_pDownstreamOutput) {
}

IVistaDataTunnel::~IVistaDataTunnel() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool IVistaDataTunnel::IsComplete() const {
  return (m_pUpstreamInput && m_pDownstreamInput && m_pUpstreamOutput && m_pDownstreamOutput);
}

IDLVistaPipeComponent* IVistaDataTunnel::GetUpstreamInput() const {
  return m_pUpstreamInput;
}
IDLVistaPipeComponent* IVistaDataTunnel::GetUpstreamOutput() const {
  return m_pUpstreamOutput;
}
IDLVistaPipeComponent* IVistaDataTunnel::GetDownstreamInput() const {
  return m_pDownstreamInput;
}
IDLVistaPipeComponent* IVistaDataTunnel::GetDownstreamOutput() const {
  return m_pDownstreamOutput;
}

void IVistaDataTunnel::SetUpstreamInput(IDLVistaPipeComponent* pNewProd) {
  // assert(pNewProd && "[IVistaDataTunnel::SetUpstreamInput] ERROR: Upstream producer == NULL");
  m_pUpstreamInput = pNewProd;
}

void IVistaDataTunnel::SetUpstreamOutput(IDLVistaPipeComponent* pNewCon) {
  // assert(pNewCon && "[IVistaDataTunnel::SetUpstreamOutput] ERROR: Upstream consumer == NULL");
  m_pUpstreamOutput = pNewCon;
}

void IVistaDataTunnel::SetDownstreamInput(IDLVistaPipeComponent* pNewProd) {
  // assert(pNewProd && "[IVistaDataTunnel::SetUpstreamInput] ERROR: Producer == NULL");
  m_pDownstreamInput = pNewProd;
}

void IVistaDataTunnel::SetDownstreamOutput(IDLVistaPipeComponent* pNewCon) {
  // assert(pNewCon && "[IVistaDataTunnel::SetUpstreamInput] ERROR: Producer == NULL");
  m_pDownstreamOutput = pNewCon;
}

void IVistaDataTunnel::StartTunneling() {
  return;
}

void IVistaDataTunnel::StopTunnelingGently(bool bJoin) {
  return;
}

void IVistaDataTunnel::HaltTunneling() {
  return;
}

void IVistaDataTunnel::IndicateTunnelingEnd() {
  return;
}

void IVistaDataTunnel::SignalPendingRequest() {
  return;
}
