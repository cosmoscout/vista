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

#include "VistaBarrier.h"
#include "VistaMutex.h"
#include "VistaSemaphore.h"
#include "VistaThreadCondition.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaBarrier::VistaBarrier(unsigned int nNumber)
    : m_pQueueFullBlock(new VistaSemaphore(nNumber, VistaSemaphore::SEM_TYPE_COMPATIBLE))
    , m_pWaitCondMutex(new VistaMutex)
    , m_pWaitCondition(new VistaThreadCondition)
    , m_nNumWait(0)
    , m_nMaxWait(nNumber)
    , m_nWaitLeaveState(0) {
}

VistaBarrier::~VistaBarrier() {
  if (GetNumWaiting() > 0) {
    if (m_pWaitCondition)
      (*m_pWaitCondition).BroadcastCondition(); // release all the waiting!
  }

  // this might be a race here, as some threads
  // will still try to access the semaphore
  // but then again... go check your state machine
  delete m_pQueueFullBlock;
  delete m_pWaitCondMutex;
  delete m_pWaitCondition;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaBarrier::Enter(bool bBlockOnQueueFull) {
  if (m_nMaxWait == 0)
    return true;

  if (bBlockOnQueueFull) {
    (*m_pQueueFullBlock).Wait(); // will block once there are nNumber is the queue
  } else {
    if (!(*m_pQueueFullBlock).TryWait() == false)
      return false;
  }

  // once we are here, all seems safe ;)
  (*m_pWaitCondMutex).Lock(); // aquire the lock to increase the counter

  if (++m_nNumWait == m_nMaxWait) {
    m_nNumWait = 0;
    (*m_pWaitCondition).BroadcastCondition();
    (*m_pWaitCondMutex).Unlock();
    (*m_pQueueFullBlock).Post();
    return true;
  } else {
    (*m_pWaitCondition).WaitForCondition(*m_pWaitCondMutex);
    (*m_pQueueFullBlock).Post();
    (*m_pWaitCondMutex).Unlock();
  }

  return (m_nWaitLeaveState == 0 ? true : false);
}

unsigned int VistaBarrier::GetNumWaitFor() const {
  return m_nMaxWait;
}

bool VistaBarrier::SetNumWaiting(unsigned int nMaxWait) {
  if (m_nNumWait > 0) // test if someone is already waiting (safe-read here...)
    return false;

  if (nMaxWait == m_nMaxWait)
    return true;
  {
    VistaMutexLock l(*m_pWaitCondMutex); // prevent anybody from accessing the m_nMaxWait.
    m_nMaxWait = nMaxWait;
    delete m_pQueueFullBlock;
    m_pQueueFullBlock = new VistaSemaphore(nMaxWait, VistaSemaphore::SEM_TYPE_COMPATIBLE);
  }
  return true;
}

unsigned int VistaBarrier::GetNumWaiting() const {
  VistaMutexLock l(*m_pWaitCondMutex);
  return m_nNumWait;
}

void VistaBarrier::CancelWait() {
  ++m_nWaitLeaveState;
  // release all the waiting...
  VistaMutexLock l(*m_pWaitCondMutex);
  (*m_pWaitCondition).BroadcastCondition();
}

void VistaBarrier::ResetCancelFlag() {
  m_nWaitLeaveState = 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
