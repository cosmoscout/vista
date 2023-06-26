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

#if defined(WIN32)
#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#include "VistaWin32MutexImp.h"
#include "VistaWin32ThreadConditionImp.h"

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static void PrintError(int iError) {
  switch (iError) {
  case WAIT_ABANDONED: {
    vstr::errp() << "The specified object is a mutex object that was not released\n"
                 << "by the thread that owned the mutex object before the owning\n"
                 << "thread terminated. Ownership of the mutex object is granted\n"
                 << "to the calling thread, and the mutex is set to nonsignaled" << std::endl;
    break;
  }
  case WAIT_IO_COMPLETION: {
    vstr::errp() << "The wait was ended by one or more user-mode asynchronous\n"
                 << "procedure calls (APC) queued to the thread" << std::endl;
    break;
  }
  case WAIT_TIMEOUT: {
    vstr::errp() << "The time-out interval elapsed, and the object's state is nonsignaled"
                 << std::endl;
    break;
  }
  case ERROR_NOT_OWNER: {
    vstr::errp() << "Attempt to release mutex not owned by caller" << std::endl;
    break;
  }
  default: {
    vstr::errp() << "Ask the MSDN" << std::endl;
    break;
  }
  }
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaWin32ThreadConditionImp::VistaWin32ThreadConditionImp() {
  m_nWaitersCount = 0;
  m_wasBroadcast  = 0;

  m_WaitQueue = CreateSemaphore(NULL, // no security
      0,                              // initially 0
      0x7fffffff,                     // max count
      NULL);                          // unnamed

  InitializeCriticalSection(&m_Waiters_count_lock);

  m_WaitersDone = CreateEvent(NULL, // no security
      FALSE,                        // auto-reset
      FALSE,                        // non-signaled initially
      NULL);                        // unnamed
}

VistaWin32ThreadConditionImp::~VistaWin32ThreadConditionImp() {

  CloseHandle(m_WaitQueue);
  DeleteCriticalSection(&m_Waiters_count_lock);
  CloseHandle(m_WaitersDone);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaWin32ThreadConditionImp::SignalCondition() {
  EnterCriticalSection(&m_Waiters_count_lock);
  bool have_waiters = m_nWaitersCount > 0;
  LeaveCriticalSection(&m_Waiters_count_lock);
  // If there aren't any waiters, then this is a no-op.
  if (have_waiters)
    ReleaseSemaphore(m_WaitQueue, 1, 0);
  return 1;
}

int VistaWin32ThreadConditionImp::BroadcastCondition() {
  // This is needed to ensure that <waiters_count_> and <was_broadcast_> are 12.10.2003 http://
  // www.cs.wustl.edu/~schmidt/win32-cv-1.html #12 consistent relative to each other.
  EnterCriticalSection(&m_Waiters_count_lock);
  bool have_waiters = 0;
  if (m_nWaitersCount > 0) {
    // We are broadcasting, even if there is just one waiter...
    // Record that we are broadcasting, which helps optimize
    // <pthread_cond_wait> for the non-broadcast case.
    m_wasBroadcast = 1;
    have_waiters   = true;
  }

  if (have_waiters) {
    // Wake up all the waiters atomically.
    ReleaseSemaphore(m_WaitQueue, m_nWaitersCount, 0);
    LeaveCriticalSection(&m_Waiters_count_lock);
    // Wait for all the awakened threads to acquire the counting
    // semaphore.
    WaitForSingleObject(m_WaitersDone, INFINITE);
    // This assignment is okay, even without the <waiters_count_lock_> held
    // because no other waiter threads can wake up to access it.
    m_wasBroadcast = 0;
  } else
    LeaveCriticalSection(&m_Waiters_count_lock);

  return 1;
}

int VistaWin32ThreadConditionImp::WaitForCondition(IVistaMutexImp* pMutexImp) {
  VistaWin32MutexImp* p = static_cast<VistaWin32MutexImp*>(pMutexImp);
  // Avoid race conditions.
  EnterCriticalSection(&m_Waiters_count_lock);
  ++m_nWaitersCount;
  LeaveCriticalSection(&m_Waiters_count_lock);
  // This call atomically releases the mutex and waits on the
  // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
  // are called by another thread.
  if (SignalObjectAndWait(p->GetMutex(), m_WaitQueue, INFINITE, FALSE) == WAIT_FAILED) {
    printf("SignalObjectAndWait FAILED. LastError: %d\n", GetLastError());
    PrintError(GetLastError());
    return GetLastError();
  }
  // Reacquire lock to avoid race conditions.
  EnterCriticalSection(&m_Waiters_count_lock);
  // We're no longer waiting...
  --m_nWaitersCount;
  // Check to see if we're the last waiter after <pthread_cond_broadcast>.
  bool last_waiter = (m_wasBroadcast && (m_nWaitersCount == 0));
  LeaveCriticalSection(&m_Waiters_count_lock);
  // If we're the last waiter thread during this particular broadcast
  // then let all the other threads proceed.
  if (last_waiter)
    // This call atomically signals the <waiters_done_> event and waits until
    // it can acquire the <external_mutex>. This is required to ensure fairness.
    SignalObjectAndWait(m_WaitersDone, p->GetMutex(), INFINITE, FALSE);
  else
    // Always regain the external mutex since that's the guarantee we
    // give to our callers.
    WaitForSingleObject(p->GetMutex(), INFINITE);

  return 0;
}

int VistaWin32ThreadConditionImp::WaitForConditionWithTimeout(IVistaMutexImp* pImp, int iMsecs) {
  return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // WIN32
