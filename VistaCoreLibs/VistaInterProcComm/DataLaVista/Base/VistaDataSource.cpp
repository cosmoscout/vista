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

#include "VistaDataSource.h"
#include "VistaDataPacket.h"
#include "VistaPacketQueue.h"
#include "VistaRTC.h"

#include <cstdio>

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaDataSource::DLVistaDataSource() {
  m_iQueueSize = m_iDropCount = m_iPackageCount = 0;
  m_pPacketQueue                                = new DLVistaPacketQueue(this, 0);
  m_pTimer                                      = IDLVistaRTC::GetRTCSingleton();
}

DLVistaDataSource::~DLVistaDataSource() {
  printf("DLVistaDataSource::~DLVistaDataSource() -- %d drop for %d produced packets (%d packets "
         "in queue: ratio [%f]).\n",
      m_iDropCount, m_iPackageCount, (*m_pPacketQueue).GetPacketCount(),
      m_iPackageCount ? ((double)m_iDropCount) / ((double)m_iPackageCount) : 0.0);
  delete m_pPacketQueue;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void DLVistaDataSource::SetQueueSize(int iSize) {
  m_iQueueSize = (*m_pPacketQueue).Resize(iSize);
}

IDLVistaDataPacket* DLVistaDataSource::FillUp() {
  IDLVistaDataPacket* p = NULL;
  if (TryToReclaimPendingPackets()) {
    // printf("DLVistaDataSource::FillUp() -- retrieved %d packets.\n", i);
    p = GetEmptyPacket();
  };
  return p; // see if we got one ;)
}

IDLVistaDataPacket* DLVistaDataSource::ProducePacket() {
  if (HasPacket()) {
    // ok, ready to rumble
    // get an empty packet first
    IDLVistaDataPacket* pPacket = GetEmptyPacket();
    // printf("DLVistaInputDeviceDataProducer::ProducePacket() -- first try got packet @%X\n",
    // pPacket);
    if (!pPacket)
      pPacket = FillUp();
    // this could be null!!
    if (pPacket) {
      // time-stamp it
      pPacket->Stamp(m_pTimer->GetTickCount(), m_pTimer->GetTimeStamp());
      // fill it
      FillPacket(pPacket);
      ++m_iPackageCount;
      // printf("DLVistaInputDeviceDataProducer::ProducePacket() -- Producing Packet %X\n",
      // pPacket);
      return pPacket;
    } else {
      ++m_iDropCount;
#ifdef DEBUG
      printf(
          "DLVistaDataSource::ProducePacket() -- Data-Drop! Data-ready, but no packet to send!\n");
#endif
    };
  } else {
    // printf("DLVistaDataSource::ProducePacket() -- No packet ready, return NULL\n");
  };

  return NULL;
}

bool DLVistaDataSource::PushPacket(IDLVistaDataPacket* pPacket) {
  if (!pPacket)
    pPacket = this->ProducePacket();

  if (!pPacket)
    return false;
  if (!this->m_pDatipcoutput->AcceptDataPacket(pPacket, this)) {
    // we failed to deliver this baby
    RecycleDataPacket(pPacket, this, true);
    return false;
  };

  return true;
}

bool DLVistaDataSource::AcceptDataPacket(
    IDLVistaDataPacket*, IDLVistaPipeComponent* pComp, bool bBlock) {
  return false;
}

bool DLVistaDataSource::ConsumePacket(IDLVistaDataPacket*) {
  return false;
}

bool DLVistaDataSource::InitPacketMgmt() {
  if ((*m_pPacketQueue).Resize(m_iQueueSize) == m_iQueueSize)
    return true;

  return false;
}

IDLVistaDataPacket* DLVistaDataSource::GetEmptyPacket(bool bBlock) {
  IDLVistaDataPacket* pPacket = NULL;

  pPacket = this->m_pDatipcoutput->ReturnPacket(); // recycle FIRST!

  if (!pPacket)
    pPacket = (*m_pPacketQueue).GiveFreshPacket(); // nothing to recycle, get new one
  else {
    // ok, got one, we have to "real-recycle" this first
    this->RecycleDataPacket(pPacket, this);
    pPacket = (*m_pPacketQueue).GiveFreshPacket(); // should be valid
  };

  return pPacket; // this is either a valid packet or NULL (indicating failure)
}

bool DLVistaDataSource::RecycleDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  // printf("DLVistaDataSource[%X]::RecycleDataPacket(%X).\n", this, pPacket);
  (*m_pPacketQueue).RecyclePacket(pPacket);
  return true;
}

IDLVistaDataPacket* DLVistaDataSource::GivePacket(bool bBlock) {
  return this->ProducePacket();
}

int DLVistaDataSource::GetOutputPacketType() const {
  return (*m_pPacketQueue).GetPacketType();
}

int DLVistaDataSource::GetPacketCount() const {
  return m_iPackageCount;
}
