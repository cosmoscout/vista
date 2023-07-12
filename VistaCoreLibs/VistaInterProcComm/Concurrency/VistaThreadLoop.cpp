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

#include "VistaThreadLoop.h"
#include "VistaMutex.h"

#include <VistaBase/VistaExceptionBase.h>

VistaThreadLoop::VistaThreadLoop(IVistaThreadImp* pImp)
    : VistaThread(pImp)
    , m_bStopGently(false)
    , m_bIsPaused(false) {
  this->m_pRunning      = new VistaMutex;
  this->m_pPausing      = new VistaMutex;
  this->m_pPauseRequest = new VistaMutex;
  this->m_pPauseGain    = new VistaMutex;
}

VistaThreadLoop::~VistaThreadLoop() {
  delete m_pRunning;
  delete m_pPausing;
  delete m_pPauseRequest;
  delete m_pPauseGain;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaThreadLoop::PauseThread(bool bBlock) {
  if (m_bIsPaused) {
    // sanity check.
    return false;
  }

  if (bBlock == true) {
    // we will try to get it and until then (thread is iterating)
    // we will wait
    (*m_pPauseGain).Lock(); // this will hold thread, after it got the sign that we are passing in
    (*m_pPauseRequest).Lock();   // signal thread to hold
    (*m_pPausing).Lock();        // wait for thread pausing
    (*m_pPauseRequest).Unlock(); // give way, iff thread wants to continue
    (*m_pPauseGain).Unlock();    // give way, thread will start again
    m_bIsPaused = true;
    return true;
  } else {
    return (*m_pPausing).TryLock();
  }
}

bool VistaThreadLoop::UnpauseThread() {
  if (m_bIsPaused) {
    (*m_pPausing).Unlock();
    m_bIsPaused = false;
    return true;
  }

  return false;
}

void VistaThreadLoop::ThreadBody() {
  VistaMutexLock l(*this->m_pRunning); // hold this while in ThreadBody

  bool bYield = false;
  PreLoop(); // callback to sub-class definable extension
  while (!m_bStopGently) {
    (*m_pPausing).Lock(); // you can not stop me while iterating

    // if someone stopped a paused thread, he/she assumably
    // doesn't want it to iterate a last time
    if (m_bStopGently) {
      (*m_pPausing).Unlock();
      break; // leave loop
    }

    bYield = LoopBody(); // perform loop-function

    if ((*m_pPauseRequest).TryLock() == false) {
      (*m_pPausing).Unlock();   // other thread can pause us now
      (*m_pPauseGain).Lock();   // block, as other thread has this mutex now
      (*m_pPauseGain).Unlock(); // give way for next request
    } else {
      (*m_pPauseRequest).Unlock(); // we got the lock, give it away before we continue
      (*m_pPausing).Unlock();
    }

    // give away the rest of the time-slice, WARNING: this could be costly on your platform
    if (bYield)
      YieldThread();
  }

  m_bStopGently = false; // we will reset this, in case we will start over again

  PostLoop(); // callback to sub-class definable extension
}

void VistaThreadLoop::PreLoop() {
}

void VistaThreadLoop::PostLoop() {
}

bool VistaThreadLoop::StopGently(bool bBlockTheOutsideCallerUntilTheThreadIsReallyFinished) {
  // make sure that we wait until the loop really stopped
  this->PauseThread(true);
  // now we can tell it to exit the loop
  IndicateLoopEnd();
  this->UnpauseThread();

  if (bBlockTheOutsideCallerUntilTheThreadIsReallyFinished) {
    if (Join()) {
      m_bStopGently = false; // reset flag here... loop thread is not running.
                             // in case we want to re-start the loop
    }
  }

  // ok, this should do
  return true;
}

void VistaThreadLoop::IndicateLoopEnd() {
  m_bStopGently = true;
}

// ============================================================================
// ============================================================================
