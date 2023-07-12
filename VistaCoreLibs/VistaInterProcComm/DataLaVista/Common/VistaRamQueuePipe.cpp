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

#include "VistaRamQueuePipe.h"
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaPacketQueue.h>

#include <cstdio>
#include <deque>
using namespace std;

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaRamQueuePipe::DLVistaRamQueuePipe()
    : m_pquInQueue(new deque<IDLVistaDataPacket*>)
    , m_pquRecycleQueue(new deque<IDLVistaDataPacket*>)
    , m_pMutexIn(new VistaMutex)
    , m_pMutexOut(new VistaMutex)
    , m_pRecycleEvent(new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT))
    , m_pQueueNotEmptyEvent(new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT))
    , m_bCanRecycle(false)
    , m_bBlockedPending(false)
    , m_pOutsideEvent(NULL)
    , m_bDoesBlockOnReturn(false)
    , m_iNumDownstreamPackets(0) {
}

DLVistaRamQueuePipe::~DLVistaRamQueuePipe() {
  (*m_pMutexIn).Lock();  // no new packets
  (*m_pMutexOut).Lock(); // no more recycling now

  m_pOutsideEvent = NULL;

  // flush queues
  deque<IDLVistaDataPacket*>::const_iterator in_q;

  for (in_q = (*m_pquInQueue).begin(); in_q != (*m_pquInQueue).end(); ++in_q) {
    // give back to
    (*m_pquRecycleQueue).push_back(*in_q);
  }

  deque<IDLVistaDataPacket*>::const_iterator out_q;
  for (out_q = (*m_pquRecycleQueue).begin(); out_q != (*m_pquRecycleQueue).end(); ++out_q) {
    // send back to its maker
    this->m_pInput->RecycleDataPacket(*out_q, this);
  }

  delete m_pquInQueue;
  delete m_pquRecycleQueue;

  (*m_pMutexIn).Unlock();
  delete m_pMutexIn;
  (*m_pMutexOut).Unlock();
  delete m_pMutexOut;
  delete m_pRecycleEvent;
  delete m_pQueueNotEmptyEvent;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

IDLVistaDataPacket* DLVistaRamQueuePipe::GivePacket(bool bBlock) {

  IDLVistaDataPacket* pPacket = NULL;
  // if we want to block -> stop if queue is empty
  m_bBlockedPending = bBlock;
  if (bBlock && m_pquInQueue->empty()) {
    m_pQueueNotEmptyEvent->WaitForEvent(true);
    m_pQueueNotEmptyEvent->ResetThisEvent();
  }

  if (!(m_pquInQueue->empty())) {
    if (GrabMutex(m_pMutexIn, bBlock)) {
      // ok, give away front
      pPacket = (*m_pquInQueue).front();
      (*m_pquInQueue).pop_front();
      // printf("DLVistaRamQueuePipe::GivePacket() -- gave away, packets in queue: %d\n",
      // (*m_pquInQueue).size());
      (*m_pMutexIn).Unlock();
    }
  }
  m_bBlockedPending = false;

  return pPacket;
}

IDLVistaDataPacket* DLVistaRamQueuePipe::ReturnPacket() {
  IDLVistaDataPacket* pPacket = NULL;

  if (!m_bCanRecycle && !m_bDoesBlockOnReturn) // shortcut, avoid mutex grab!
  {
    //#ifdef DEBUG
    //		printf("***WARNING *** [DLVistaRamQueuePipe::ReturnPacket] "
    //				"Unable to return packet. Returning NULL!\n");
    //#endif
    return NULL;
  }

  // will there be something?
  if (m_iNumDownstreamPackets <= 0) {
    //#ifdef DEBUG
    //		printf("***WARNING *** [DLVistaRamQueuePipe::ReturnPacket] "
    //				"No downstream packets. Returning NULL!\n");
    //#endif
    return NULL;
  }

  // if we have to wait here -> do so
  if (m_bDoesBlockOnReturn &&     // shall we block?
      m_pquRecycleQueue->empty()) // is there something?
  {
    // blocking wait for packets to be recycled
    m_pRecycleEvent->WaitForEvent(true);
    m_pRecycleEvent->ResetThisEvent();
  } else {
    // block, but only wait for a short burst!
    m_pRecycleEvent->WaitForEvent(5);
    m_pRecycleEvent->ResetThisEvent();
  }
  // get exclusive acces to the recycle queue
  this->GrabMutex(m_pMutexOut, true);
  if (!m_pquRecycleQueue->empty()) {
    pPacket = m_pquRecycleQueue->front();
    m_pquRecycleQueue->pop_front();
    if (m_pquRecycleQueue->empty())
      m_bCanRecycle = false;
    // update the number of packets under way in the downstream pipe
    --m_iNumDownstreamPackets;
  }
  m_pMutexOut->Unlock();

  //#ifdef DEBUG
  //	if(!pPacket)
  //	{
  //		printf("***WARNING *** [DLVistaRamQueuePipe::ReturnPacket] "
  //				"Returning NULL!\n");
  //	}
  //#endif
  return pPacket;
}

////	printf("DLVistaRamQueuePipe::ReturnPacket()\n");
////	printf("DLVistaRamQueuePipe -- GrabMutex()\n");
////	if(GrabMutex(m_pMutexOut, false))
//	{
////		printf("DLVistaRamQueuePipe -- OK\n");
//		if((*m_pquRecycleQueue).empty())
//		{
////			printf("DLVistaRamQueuePipe -- recycle queue empty!\n");
//			// wait for some packets
////			(*m_pMutexOut).Unlock();
//
//
////			printf("DLVistaRamQueuePipe::ReturnPacket(): Recylce-request pending on empty
///queue... thread blocking.\n"); /			if((*m_pRecycleEvent).WaitForEvent(5)==1) //
///block, but only wait for a short burst! /			{ /
///printf("DLVistaRamQueuePipe::ReturnPacket(): Got signal... there must be some packets in the
///recycle-queue\n"); /			} /			else /			{ /
///printf("DLVistaRamQueuePipe::ReturnPacket(): TIMEOUT, continue.\n"); /
///return NULL; /			}
//
//			//blocking wait until there are results to pass back
//			m_pRecycleEvent->WaitForEvent();
//
////			if(GrabMutex(m_pMutexOut, true))
////			{
////				printf("DLVistaRamQueuePipe::ReturnPacket(): Got mutex to
///queue.\n"); /			}
//			// ok, now we are sure that there is at least one packet in this queue!
//			// AND we have exclusive access to recycle-queue
//		}
//		//block to get recycle queue access
//		GrabMutex(m_pMutexOut, true);
//		if(!(*m_pquRecycleQueue).empty())
//		{
////			printf("DLVistaRamQueuePipe -- second branch empty\n");
//			pPacket = (*m_pquRecycleQueue).front();
//			(*m_pquRecycleQueue).pop_front();
////			printf("DLVistaRamQueuePipe::ReturnPacket() -- Returning packet %X to active
///component. -- recycle queue-size = %d\n", pPacket, (*m_pquRecycleQueue).size());
//		}
//		else
//		{
//			printf("DLVistaRamQueuePipe::ReturnPacket() -- RETURN QUEUE EMPTY.\n");
//		}
//
//		// this test is used to toggle emtpy flag
////		printf("DLVistaRamQueuePipe-- checking for empty flag\n");
//		if((*m_pquRecycleQueue).empty())
//		{
//			m_bCanRecycle = false;
//  //         	printf("DLVistaRamQueuePipe setting recycle flag to false\n");
//		}
//	//	printf("DLVistaRamQueuePipe -- unlocking mutex");
//		(*m_pMutexOut).Unlock();
////	}
////	else
////		printf("DLVistaRamQueuePipe::ReturnPacket() -- no mutex, no fun.\n");
//
////	printf("Returning from Recycling queue: %X\n", pPacket);
//
//	return pPacket;
//}

VistaMutex* DLVistaRamQueuePipe::GrabMutex(VistaMutex* pMutex, bool bBlock) {
  if (bBlock) {
    (*pMutex).Lock();
  } else {
    if ((*pMutex).TryLock() == false)
      return NULL;
  }

  return pMutex;
}

bool DLVistaRamQueuePipe::RecycleDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  // printf("DLVistaRamQueuePipe::RecycleDataPacket() -- %x\n", pComp);
  if (GrabMutex(m_pMutexOut, bBlock)) {
    (*m_pquRecycleQueue).push_back(pPacket);
    m_bCanRecycle = true;
    // printf("DLVistaRamQueuePipe::RecycleDataPacket(%X) -- packets in queue: %d\n", pPacket,
    // (*m_pquRecycleQueue).size());
    (*m_pMutexOut).Unlock();
    // printf("DLVistaRamQueuePipe::SignalingEvent\n");
    (*m_pRecycleEvent).SignalEvent(); // signal in case someone is waiting.
    // printf("DLVistaRamQueuePipe::SignalingEvent  -- DONE\n");
    return true;
  } else {

    // printf("DLVistaRamQueuePipe::RecycleDataPacket(%X) -- failed to grab mutex!\n", pPacket);
  }

  return false;
}

bool DLVistaRamQueuePipe::AcceptDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  bool bRet = false;
  if (GrabMutex(m_pMutexIn, bBlock)) {
    (*m_pquInQueue).push_back(pPacket);
    // printf("DLVistaRamQueuePipe::AcceptDataPacket(%x) -- accepted, packets in queue: %d\n",
    // pPacket, (*m_pquInQueue).size());
    ++m_iNumDownstreamPackets;
    (*m_pMutexIn).Unlock();
    bRet = true;
    // if queue has been empty before => signal that there is one element now
    if (m_pquInQueue->size() == 1) {
      m_pQueueNotEmptyEvent->SignalEvent();
      if (m_pOutsideEvent)
        m_pOutsideEvent->SignalEvent();
    }
  } else {
    // printf("DLVistaRamQueuePipe::AcceptDataPacket(%X) -- could not put packet to queue!!\n");
  }

  return bRet;
}

bool DLVistaRamQueuePipe::InitPacketMgmt() {
  return false;
}

bool DLVistaRamQueuePipe::IsFull() const {
  return !(*m_pquRecycleQueue).empty();
}

bool DLVistaRamQueuePipe::IsEmpty() const {
  return ((*m_pquInQueue).size() == 0);
  // return (*m_pquInQueue).GetPacketCount();
}

bool DLVistaRamQueuePipe::GetIsBlockedPending() const {
  return m_bBlockedPending;
}

bool DLVistaRamQueuePipe::SignalPendingRequest() {
  if (m_bBlockedPending) {
    m_pQueueNotEmptyEvent->SignalEvent();
    if (m_pOutsideEvent)
      m_pOutsideEvent->SignalEvent();
  }
  return m_bBlockedPending;
}

void DLVistaRamQueuePipe::SetOutsideThreadEvent(VistaThreadEvent* pEvent) {
  m_pOutsideEvent = pEvent;
}

void DLVistaRamQueuePipe::SetDoesBlockOnReturnPacket(bool b) {
  m_bDoesBlockOnReturn = b;
}

bool DLVistaRamQueuePipe::GetDoesBlockOnReturnPacket() const {
  return m_bDoesBlockOnReturn;
}
