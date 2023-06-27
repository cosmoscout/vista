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

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
#include "VistaRoutingPipe.h"
#include "VistaPipeComponent.h"
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <limits>
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
IDLVistaRoutingPipe::IDLVistaRoutingPipe()
    : m_pOutputLock(new VistaMutex) {
}
IDLVistaRoutingPipe::~IDLVistaRoutingPipe() {
  delete m_pOutputLock;
}
/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
int IDLVistaRoutingPipe::Capacity() const {
  return numeric_limits<int>::max();
}
bool IDLVistaRoutingPipe::IsOutputComponent(IDLVistaPipeComponent* pComp) const {
  VistaMutexLock l(*m_pOutputLock);
  for (unsigned int i = 0; i < m_vecOutputs.size(); ++i)
    if (m_vecOutputs[i] == pComp)
      return true;
  return false;
}
IDLVistaPipeComponent* IDLVistaRoutingPipe::GetOutboundByIndex(int iIndex) const {
  VistaMutexLock l(*m_pOutputLock);
  return m_vecOutputs.at(iIndex);
}
int IDLVistaRoutingPipe::GetNumberOfOutbounds() const {
  VistaMutexLock l(*m_pOutputLock);
  return (int)m_vecOutputs.size();
}
bool IDLVistaRoutingPipe::AttachOutputComponent(IDLVistaPipeComponent* pComp) {
  VistaMutexLock l(*m_pOutputLock);
  m_vecOutputs.push_back(pComp);
  m_pOutput = m_vecOutputs[0];
  return true;
}
bool IDLVistaRoutingPipe::DetachAllOuputs() {
  VistaMutexLock l(*m_pOutputLock);
  for (unsigned int i = 0; i < m_vecOutputs.size(); ++i)
    m_vecOutputs[i] = NULL;
  m_pOutput = NULL;
  return true;
}
bool IDLVistaRoutingPipe::AcceptDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pSender, bool bBlock /*=false*/) {
  m_pOutputLock->Lock();
  int iOutPort = this->RoutePacket(pPacket);
  if (iOutPort >= 0) {
    // routing worked fine -> forward data to the right output
    (m_vecOutputs[iOutPort])->AcceptDataPacket(pPacket, this, bBlock);
    m_pOutputLock->Unlock();
    return true;
  } else {
    m_pOutputLock->Unlock();
    // routing did not work -> recycle the packet directly...
    this->RecycleDataPacket(pPacket, this, bBlock);
    return false;
  }
}
bool IDLVistaRoutingPipe::RecycleDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pSender, bool bBlock /*=false*/) {
  return m_pInput->RecycleDataPacket(pPacket, this, bBlock);
}
IDLVistaDataPacket* IDLVistaRoutingPipe::GivePacket(bool bBlock) {
  /*
   * this is a somewhat strange implementation:
   * GivePacket tries to get a packet from the input. We then <accept> the
   * packet in order to route it. In effect the "right" outbound will
   * get a packet (which is the ultimate goal of <give packet>).
   * We then return NULL to prevent doubly processing the same packet.
   */
  IDLVistaDataPacket* pPacket = m_pInput->GivePacket(bBlock);
  this->AcceptDataPacket(pPacket, this, bBlock);
  return NULL;
}
IDLVistaDataPacket* IDLVistaRoutingPipe::ReturnPacket() {
  IDLVistaDataPacket* pPacket = NULL;
  unsigned int        i       = 0;
  // try to get a packet from any of the inputs
  while (i < m_vecOutputs.size() && pPacket == NULL) {
    if (m_vecOutputs[i])
      pPacket = (m_vecOutputs[i])->ReturnPacket();
    ++i;
  }
  return pPacket;
}
bool IDLVistaRoutingPipe::InitPacketMgmt() {
  return true;
}
list<IDLVistaPipeComponent*> IDLVistaRoutingPipe::GetOutputComponents() const {
  VistaMutexLock               l(*m_pOutputLock);
  list<IDLVistaPipeComponent*> liOuts;
  for (unsigned int i = 0; i < m_vecOutputs.size(); ++i)
    liOuts.push_back(m_vecOutputs[i]);
  return liOuts;
}
