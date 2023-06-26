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

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#if defined(VISTA_THREADING_POSIX)

#include "VistaPthreadThreadEventImp.h"
#include <errno.h>
#include <unistd.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaPthreadThreadEventImp::VistaPthreadThreadEventImp()
    : autoreset(true)
    , state(0) {
  pthread_mutex_init(&mtx, 0);
  pthread_cond_init(&cond, 0);
}

VistaPthreadThreadEventImp::~VistaPthreadThreadEventImp() {
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mtx);
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaPthreadThreadEventImp::SignalEvent() {
  TEMP_FAILURE_RETRY(pthread_mutex_lock(&mtx));

  state = 1;
  if (autoreset)
    pthread_cond_signal(&cond);
  else
    pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mtx);
}

bool VistaPthreadThreadEventImp::WaitForEvent(int iTimeoutMSecs) {
  bool ret = true;

  TEMP_FAILURE_RETRY(pthread_mutex_lock(&mtx));
  if (state == 0) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    // add delta to nanoseconds
    ts.tv_nsec += long(iTimeoutMSecs) * 1000000L;

    // wrap nanoseconds
    int sec_wrap = ts.tv_nsec / 1000000000L;
    ts.tv_sec += sec_wrap;
    ts.tv_nsec = ts.tv_nsec - sec_wrap * 1000000000L;

    if (pthread_cond_timedwait(&cond, &mtx, &ts) == ETIMEDOUT)
      ret = false;
  }
  if (autoreset)
    state = 0;
  pthread_mutex_unlock(&mtx);

  return ret;
}

bool VistaPthreadThreadEventImp::WaitForEvent(bool bBlock) {
  bool ret = true;

  TEMP_FAILURE_RETRY(pthread_mutex_lock(&mtx));
  if (bBlock) {
    if (state == 0)
      TEMP_FAILURE_RETRY(pthread_cond_wait(&cond, &mtx));
  } else if (state == 0)
    ret = false;

  if (autoreset)
    state = 0;
  pthread_mutex_unlock(&mtx);

  return ret;
}

HANDLE VistaPthreadThreadEventImp::GetEventSignalHandle() const {
  // to be honest... we should throw here
  return (HANDLE)0xFFFFFFFF;
}

HANDLE VistaPthreadThreadEventImp::GetEventWaitHandle() const {
  // to be honest... we should throw here
  return (HANDLE)0xFFFFFFFF;
}

bool VistaPthreadThreadEventImp::ResetThisEvent(ResetBehavior /* reset_behavior */) {
  TEMP_FAILURE_RETRY(pthread_mutex_lock(&mtx));
  state = 0;
  pthread_mutex_unlock(&mtx);
  return true;
}

#endif

// ============================================================================
// ============================================================================
