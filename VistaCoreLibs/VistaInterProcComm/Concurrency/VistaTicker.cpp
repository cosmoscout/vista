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

//#include "ITAStopwatch.h"

#include "VistaTicker.h"

#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <algorithm>
#include <cassert>

#include <iostream>

// static ITAStopwatch watch;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
VistaTicker::TriggerContext::TriggerContext(int nTimeout, bool bPulsing)
    : m_nTimeout(nTimeout)
    , m_nCurrent(nTimeout)
    , m_bPulsing(bPulsing)
    , m_pTrigger(new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT))
    , m_bActive(true)
    , m_bOwnEvent(true) {
}

VistaTicker::TriggerContext::TriggerContext(int nTimeout, bool bPulsing, VistaThreadEvent* pEvent)
    : m_nTimeout(nTimeout)
    , m_nCurrent(nTimeout)
    , m_bPulsing(bPulsing)
    , m_pTrigger(pEvent)
    , m_bActive(true)
    , m_bOwnEvent(false) {
  assert(pEvent != NULL);
}

VistaTicker::TriggerContext::~TriggerContext() {
  if (m_bOwnEvent)
    delete m_pTrigger;
}

HANDLE VistaTicker::TriggerContext::GetEventWaitHandle() const {
  return m_pTrigger->GetEventWaitHandle();
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

#if defined(WIN32)
VistaSigned32Atomic VistaTicker::m_nNumTimers = 0;

VistaTicker::VistaTicker(unsigned int nGranularity)
    : m_nGranularity(nGranularity)
    , m_Ticker(NULL)
    , m_bRunning(false)
    , m_pAfterPulseFunctor(NULL)
    , m_pFirst(NULL)
    , m_nSizeCache(0) {
  TIMECAPS caps;
  if (timeGetDevCaps(&caps, sizeof(caps)) == TIMERR_NOERROR) {
    if (m_nGranularity < caps.wPeriodMin || m_nGranularity > caps.wPeriodMax)
      VISTA_THROW("timeGetDevCaps error specified illegal value for granularity!\n", 0x00000000);
  }

  if (m_nNumTimers++ > 16)
    VISTA_THROW("VistaTicker::VistaTicker[WIN32]: no more than 16 tickers allowed.\n", 0x00000000);
}

VistaTicker::~VistaTicker() {
  StopTicker();
  m_Ticker = NULL;
  for (TRIGVEC::iterator it = m_vecTriggers.begin(); it != m_vecTriggers.end(); ++it)
    delete *it;

  delete m_pAfterPulseFunctor;

  --m_nNumTimers;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaTicker::StartTicker() {
  m_Ticker = timeSetEvent(m_nGranularity, 5, &TickerCallback, (DWORD_PTR)this,
      TIME_PERIODIC | TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS);
  if (m_Ticker == NULL)
    VISTA_THROW("VistaTicker::StartTicker() -- could not start\n", 0x00000000);

  // watch.start();

  m_bRunning = true;
  return true;
}

bool VistaTicker::StopTicker() {
  if (m_Ticker == NULL)
    return true;

  if (timeKillEvent(m_Ticker) == TIMERR_NOERROR) {
    m_Ticker   = NULL;
    m_bRunning = false;
    return true;
  }
  return false;
}

// static int n=0;
void VistaTicker::TickerCallback(
    UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
  VistaTicker* pT = (VistaTicker*)(dwUser);
  if (pT)
    pT->UpdateTriggerContexts();
}

#else

#include "VistaThreadLoop.h"
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

class VistaTicker::TickerThread : public VistaThreadLoop {
 public:
  TickerThread(VistaTicker* parent, unsigned int ticker_granularity)
      : VistaThreadLoop()
      , m_parent(parent)
      , m_cb_granularity(ticker_granularity)
      , m_average_cb_cost(0)
      , m_average_time() {
  }

 protected:
  bool LoopBody() {

    m_average_time.StartRecording();
    m_parent->UpdateTriggerContexts();
    m_average_time.RecordTime();

    m_average_cb_cost = (unsigned int)(m_average_time.GetAverageTime() * 1000);

    VistaTimeUtils::Sleep(m_cb_granularity /*- m_average_cb_cost*/);

    return false; // no YIELD
  }

 private:
  VistaTicker* m_parent;
  unsigned int m_cb_granularity;
  unsigned int m_average_cb_cost;

  VistaAverageTimer m_average_time;
};

/** @todo implement me for unix builds :) */
VistaTicker::VistaTicker(unsigned int nGranularity)
    : m_nGranularity(nGranularity)
    , m_bRunning(false)
    , m_pAfterPulseFunctor(NULL)
    , m_pFirst(NULL)
    , m_nSizeCache(0)
    , m_ticker_thread(new TickerThread(this, nGranularity)) {
  m_ticker_thread->PauseThread(true);
  m_ticker_thread->Run();
}

VistaTicker::~VistaTicker() {
  m_ticker_thread->StopGently(true);

  for (TRIGVEC::iterator it = m_vecTriggers.begin(); it != m_vecTriggers.end(); ++it)
    delete *it;

  delete m_pAfterPulseFunctor;

  delete m_ticker_thread;
}

bool VistaTicker::StartTicker() {
  return m_ticker_thread->UnpauseThread();
}

bool VistaTicker::StopTicker() {
  return m_ticker_thread->PauseThread(true);
}

#endif

void VistaTicker::UpdateTriggerContexts() {
  if (!m_pFirst)
    return;

  TriggerContext** p = m_pFirst;
  for (TRIGVEC::size_type n = 0; n < m_nSizeCache; ++n, ++p) {
    // p = m_vecTriggers[n];
    if (!(*p)->m_bActive)
      continue;

    (*p)->m_nCurrent -= m_nGranularity;
    if ((*p)->m_nCurrent <= 0) {
      (*p)->m_pTrigger->SignalEvent();
      if ((*p)->m_bPulsing)
        (*p)->m_nCurrent = (*p)->m_nTimeout;
      else
        (*p)->m_bActive = false;
    }
  }

  if (m_pAfterPulseFunctor)
    (*m_pAfterPulseFunctor)();
}

bool VistaTicker::AddTrigger(TriggerContext* pCtx) {
  bool bStart = false;
  if (m_bRunning) {
    StopTicker();
    bStart = true;
  }

  m_vecTriggers.push_back(pCtx);
  ++m_nSizeCache;
  m_pFirst = &m_vecTriggers[0];

  if (bStart)
    StartTicker();
  return true;
}

bool VistaTicker::RemTrigger(TriggerContext* pCtx, bool bDelete) {
  bool bStart = false;
  if (m_bRunning) {
    StopTicker();
    bStart = true;
  }

  TRIGVEC::iterator it = std::find(m_vecTriggers.begin(), m_vecTriggers.end(), pCtx);
  if (it == m_vecTriggers.end()) {
    StartTicker();
    return false;
  }

  m_vecTriggers.erase(it);
  --m_nSizeCache;

  if (!m_vecTriggers.empty())
    m_pFirst = &m_vecTriggers[0];
  else
    m_pFirst = NULL;

  if (bStart)
    StartTicker();

  if (bDelete)
    delete pCtx;

  return true;
}

VistaTicker::AfterPulseFunctor* VistaTicker::GetAfterPulseFunctor() const {
  return m_pAfterPulseFunctor;
}

void VistaTicker::SetAfterPulseFunctor(AfterPulseFunctor* pFnct) {
  m_pAfterPulseFunctor = pFnct;
}

unsigned int VistaTicker::GetGranularity() const {
  return m_nGranularity;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

void VistaTicker::TriggerContext::Reset() {
  m_pTrigger->ResetThisEvent(VistaThreadEvent::RESET_JUST_ONE_EVENT);
}
