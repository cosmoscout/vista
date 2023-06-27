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

#include "VistaActiveDataProducer.h"
#include <VistaInterProcComm/Concurrency/VistaPriority.h>
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>
#include <cstdio>
#include <list>
using namespace std;

#if defined(VISTA_IPC_USE_EXCEPTIONS)
#include <VistaBase/VistaExceptionBase.h>
#endif

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

class DLVistaProducerLoop : public VistaThreadLoop {
 private:
  DLVistaActiveDataProducer* m_pTarget;
  bool                       m_bWaitForData;
  int                        m_iTimeout;

 protected:
 public:
  DLVistaProducerLoop(DLVistaActiveDataProducer* pProducer);
  virtual ~DLVistaProducerLoop();

  bool GetWaitForData() const;
  void SetWaitForData(bool bWait);
  void SetWaitTimeout(int iTimeout);
  int  GetWaitTimeout() const;
  bool LoopBody();
};

DLVistaProducerLoop::DLVistaProducerLoop(DLVistaActiveDataProducer* pProducer) {
  m_pTarget      = pProducer;
  m_bWaitForData = false;
  m_iTimeout     = 0;
}

DLVistaProducerLoop::~DLVistaProducerLoop() {
}

bool DLVistaProducerLoop::LoopBody() {
  // pull packet
#if defined(VISTA_IPC_USE_EXCEPTIONS)
  try {
#endif
    if (!m_pTarget->PushPacket())
    // oops, this failed... maybe we should wait a bit
    {
      // well, we can at least try to recycle some packets while we are here ;)
      m_pTarget->TryToReclaimPendingPackets();

      if (m_bWaitForData) {
        // printf("DLVistaProducerLoop::LoopBody -> WaitForData\n");
        bool b = m_pTarget->WaitForNextPacket(m_iTimeout);
        // printf("%s", (b ? "INCOMING\n" : "NO PACKET, LOOP\n"));
        return b;
      }

      // VistaTimer::Sleep(5);
      // printf("DLVistaProducerLoop::LoopBody()\n");
    }
#if defined(VISTA_IPC_USE_EXCEPTIONS)
  } catch (VistaExceptionBase& x) {
    x.PrintException();
    IndicateLoopEnd(); // we will end this
  }
#endif

  return false;
}

bool DLVistaProducerLoop::GetWaitForData() const {
  return m_bWaitForData;
}

void DLVistaProducerLoop::SetWaitForData(bool bWait) {
  m_bWaitForData = bWait;
}

void DLVistaProducerLoop::SetWaitTimeout(int iTimeout) {
  m_iTimeout = iTimeout;
}

int DLVistaProducerLoop::GetWaitTimeout() const {
  return m_iTimeout;
}

// ##############################################################################

DLVistaActiveDataProducer::DLVistaActiveDataProducer(IDLVistaDataProducer* pRealProducer) {
  m_pRealProducer = pRealProducer;
  m_pThreadLoop   = new DLVistaProducerLoop(this);
  //#if defined(WIN32)
  //    m_pThreadLoop->SetPriority(THREAD_PRIORITY_BELOW_NORMAL);
  //#endif
  m_bStopOnDestruct = true;
}

DLVistaActiveDataProducer::~DLVistaActiveDataProducer() {
  if (m_bStopOnDestruct)
    m_pThreadLoop->StopGently(true);

  delete m_pThreadLoop;
}

int DLVistaActiveDataProducer::TryToReclaimPendingPackets() {
  return m_pRealProducer->TryToReclaimPendingPackets();
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

IDLVistaDataPacket* DLVistaActiveDataProducer::ProducePacket() {
  return m_pRealProducer->ProducePacket();
}

bool DLVistaActiveDataProducer::PushPacket(IDLVistaDataPacket* pPacket) {
  return m_pRealProducer->PushPacket(pPacket);
}

bool DLVistaActiveDataProducer::HasPacket() const {
  return m_pRealProducer->HasPacket();
}

bool DLVistaActiveDataProducer::AttachInputComponent(IDLVistaPipeComponent* pComp) {
  return m_pRealProducer->AttachInputComponent(pComp);
}

bool DLVistaActiveDataProducer::AttachOutputComponent(IDLVistaPipeComponent* pComp) {
  return m_pRealProducer->AttachOutputComponent(pComp);
}

bool DLVistaActiveDataProducer::AcceptDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  return m_pRealProducer->AcceptDataPacket(pPacket, this, bBlock);
}

bool DLVistaActiveDataProducer::RecycleDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  return m_pRealProducer->RecycleDataPacket(pPacket, this, bBlock);
}

IDLVistaDataPacket* DLVistaActiveDataProducer::GivePacket(bool bBlock) {
  return m_pRealProducer->GivePacket(bBlock);
}

IDLVistaDataPacket* DLVistaActiveDataProducer::ReturnPacket() {
  return m_pRealProducer->ReturnPacket();
}

bool DLVistaActiveDataProducer::InitPacketMgmt() {
  return m_pRealProducer->InitPacketMgmt();
}

IDLVistaDataPacket* DLVistaActiveDataProducer::CreatePacket() {
  return m_pRealProducer->CreatePacket();
}

void DLVistaActiveDataProducer::DeletePacket(IDLVistaDataPacket*) {
}

bool DLVistaActiveDataProducer::StartComponent() {
  printf("DLVistaActiveDataProducer::StartComponent()\n");
  return m_pThreadLoop->Run();
}

bool DLVistaActiveDataProducer::PauseComponent(bool bJoin) {
  printf("DLVistaActiveDataProducer::PauseComponent(%d)\n", bJoin);
  return m_pThreadLoop->PauseThread(bJoin);
}

bool DLVistaActiveDataProducer::UnPauseComponent(bool bJoin) {
  m_pThreadLoop->UnpauseThread();
  return true;
}

bool DLVistaActiveDataProducer::StopComponent(bool bJoin) {
  // printf("DLVistaActiveDataProducer::StopComponent(%d)\n", bJoin);
  m_pThreadLoop->Suspend();
  if (bJoin)
    m_pThreadLoop->Join(); // this could be a deadlock!
  return true;
}

bool DLVistaActiveDataProducer::HaltComponent() {
  // printf("DLVistaActiveDataProducer::HaltComponent()\n");
  return m_pThreadLoop->Abort();
}

bool DLVistaActiveDataProducer::StopComponentGently(bool bJoin) {
  return m_pThreadLoop->StopGently(bJoin);
}

void DLVistaActiveDataProducer::IndicateProductionEnd() {
  m_pThreadLoop->IndicateLoopEnd();
}

bool DLVistaActiveDataProducer::IsComponentRunning() const {
  // printf("DLVistaActiveDataProducer::IsComponentRunning()\n");
  return m_pThreadLoop->IsRunning();
}

void DLVistaActiveDataProducer::FillPacket(IDLVistaDataPacket* pPacket) {
  // printf("DLVistaActiveDataProducer::FillPacket(%X) -- why this??\n", pPacket);
  //	m_pRealProducer->FillPacket(pPacket);
}

list<IDLVistaPipeComponent*> DLVistaActiveDataProducer::GetOutputComponents() const {
  return m_pRealProducer->GetOutputComponents();
}

int DLVistaActiveDataProducer::GetInputPacketType() const {
  return m_pRealProducer->GetInputPacketType();
}

int DLVistaActiveDataProducer::GetOutputPacketType() const {
  return m_pRealProducer->GetOutputPacketType();
}

list<IDLVistaPipeComponent*> DLVistaActiveDataProducer::GetInputComponents() const {
  return m_pRealProducer->GetInputComponents();
}

IDLVistaPipeComponent* DLVistaActiveDataProducer::GetOutboundByIndex(int iIndex) const {
  return m_pRealProducer->GetOutboundByIndex(iIndex);
}

IDLVistaPipeComponent* DLVistaActiveDataProducer::GetInboundByIndex(int iIndex) const {
  return m_pRealProducer->GetInboundByIndex(iIndex);
}

int DLVistaActiveDataProducer::GetNumberOfOutbounds() const {
  return m_pRealProducer->GetNumberOfOutbounds();
}

int DLVistaActiveDataProducer::GetNumberOfInbounds() const {
  return m_pRealProducer->GetNumberOfInbounds();
}

int DLVistaActiveDataProducer::SetComponentPriority(const VistaPriority& pPrio) {
  m_pThreadLoop->SetPriority(pPrio);
  return pPrio.GetSystemPriority();
}

bool DLVistaActiveDataProducer::GetWaitForData() const {
  return m_pThreadLoop->GetWaitForData();
}

void DLVistaActiveDataProducer::SetWaitForData(bool bWait) {
  m_pThreadLoop->SetWaitForData(bWait);
}

void DLVistaActiveDataProducer::SetWaitTimeout(int iTimeout) {
  m_pThreadLoop->SetWaitTimeout(iTimeout);
}

int DLVistaActiveDataProducer::GetWaitTimeout() const {
  return m_pThreadLoop->GetWaitTimeout();
}

bool DLVistaActiveDataProducer::WaitForNextPacket(int iTimeout) const {
  return m_pRealProducer->WaitForNextPacket(iTimeout);
}

bool DLVistaActiveDataProducer::GetStopOnDestruct() const {
  return m_bStopOnDestruct;
}

void DLVistaActiveDataProducer::SetStopOnDestruct(bool bStop) {
  m_bStopOnDestruct = bStop;
}

void DLVistaActiveDataProducer::SetThreadName(const std::string& strName) {
  m_pThreadLoop->SetThreadName(strName);
}

std::string DLVistaActiveDataProducer::GetThreadName() const {
  return m_pThreadLoop->GetThreadName();
}
