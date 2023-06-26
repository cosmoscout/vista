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

#include "VistaOpenSGThreadImp.h"
#include <VistaAspects/VistaConversion.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGChangeList.h>
#include <OpenSG/OSGThread.h>
#include <OpenSG/OSGThreadManager.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#if defined(WIN32)
#include <Windows.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
#ifdef WIN32
typedef struct tagTHREADNAME_INFO {
  DWORD  dwType;     // must be 0x1000
  LPCSTR szName;     // pointer to name (in user addr space)
  DWORD  dwThreadID; // thread ID (-1=caller thread)
  DWORD  dwFlags;    // reserved for future use, must be zero
} THREADNAME_INFO;
#endif

#ifdef WIN32
static void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName) {
  THREADNAME_INFO info;
  info.dwType     = 0x1000;
  info.szName     = szThreadName;
  info.dwThreadID = dwThreadID;
  info.dwFlags    = 0;

  __try {
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400)
    RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
#else
    RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (DWORD*)&info);
#endif
#endif
  } __except (EXCEPTION_CONTINUE_EXECUTION) {}
}
#endif

void VistaOSGThreadImp::threadFct(void* vp) {
  VistaOSGThreadImp::_hlp* pHlp = reinterpret_cast<VistaOSGThreadImp::_hlp*>(vp);

  std::string sName = pHlp->m_pVistaThread->GetThreadName();
#ifdef WIN32
  // set name

  if (!sName.empty()) {
    DWORD threadId = GetCurrentThreadId();
    ::SetThreadName(threadId, sName.c_str());
  }
#endif

  try {

    pHlp->m_pVistaThread->PreRun();
    pHlp->m_pVistaThread->ThreadBody();
    pHlp->m_pVistaThread->PostRun();

    pHlp->m_pOsgThread->PostRun();
  } catch (std::exception& x) {
    vstr::err() << "OSG-VistaThreadImp -- [" << sName
                << "] -- execution terminated, given exception: " << x.what() << std::endl;
  } catch (...) {
    vstr::err() << "OSG-VistaThreadImp -- [" << sName
                << "] -- execution terminated due to unknown exception (ellipse catch).";
  }

#if defined(WIN32)
  ExitThread(0);
#endif
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaOSGThreadImp::VistaOSGThreadImp(const std::string& sName, VistaThread* pThread, int nAspect) {
  DoSetThreadName(sName);
  m_pOSGThread         = NULL;
  m_bRunning           = false;
  m_hlp.m_pVistaThread = pThread;
  m_nAspect            = nAspect;
}

VistaOSGThreadImp::~VistaOSGThreadImp() {
  if (m_pOSGThread != NULL)
    subRefP(m_pOSGThread);
}

bool VistaOSGThreadImp::Run() {
  if (m_pOSGThread == NULL) {
    if (!GetThreadName().empty()) {
      m_pOSGThread =
          dynamic_cast<OSG::Thread*>(OSG::ThreadManager::the()->getThread(GetThreadName().c_str()));
    } else
      m_pOSGThread = dynamic_cast<OSG::Thread*>(OSG::ThreadManager::the()->getThread(NULL));

    addRefP(m_pOSGThread);
    m_hlp.m_pOsgThread = this;
    // m_hlp.m_pVista<Thread = m_pVistaThread;

    m_pOSGThread->runFunction(threadFct, m_nAspect, &m_hlp);
    return true;
  }

  else if (m_bRunning) {
    return false;
  } else {
    // thread created and not running: run again
    m_pOSGThread->runFunction(threadFct, m_nAspect, &m_hlp);
    return true;
  }
}

bool VistaOSGThreadImp::Suspend() {
  return false;
}

bool VistaOSGThreadImp::Resume() {
  return false;
}

bool VistaOSGThreadImp::Join() {
  if (m_pOSGThread == NULL)
    return true;

  if (m_pOSGThread->exists()) {
    OSG::BaseThread::getCurrent()->join(m_pOSGThread);
    subRefP(m_pOSGThread);
    m_pOSGThread = 0;
    return true;
  } else
    return false;
}

bool VistaOSGThreadImp::Abort() {
  if (m_pOSGThread == NULL)
    return true; // non-running thread is aborted ;)

  m_pOSGThread->kill();

  return true; // we did our best.
}

bool VistaOSGThreadImp::SetPriority(const VistaPriority&) {
  return false;
}

void VistaOSGThreadImp::GetPriority(VistaPriority&) const {
}

void VistaOSGThreadImp::YieldThread() {
  if (m_pOSGThread == NULL)
    return;

  // sorry, there seems to be no API for this in OpenSG
}

void VistaOSGThreadImp::SetCancelAbility(const bool bOkToCancel) {
}

bool VistaOSGThreadImp::CanBeCancelled() const {
  return false;
}

void VistaOSGThreadImp::PreRun() {
  m_bRunning = true;
}

void VistaOSGThreadImp::PostRun() {
  m_bRunning = false;
}

long VistaOSGThreadImp::GetThreadIdentity() const {
  if (m_pOSGThread == NULL)
    return -1;

  return long(m_pOSGThread);
  // return 0;
}

VistaThread* VistaOSGThreadImp::GetVistaThread() const {
  return m_hlp.m_pVistaThread;
}

void VistaOSGThreadImp::SetVistaThread(VistaThread* pThread) {
  m_hlp.m_pVistaThread = pThread;
}

OSG::Thread* VistaOSGThreadImp::GetOSGThread() const {
  return m_pOSGThread;
}

bool VistaOSGThreadImp::ApplyAndClearChangeList() {
  m_pOSGThread->getChangeList()->applyAndClear();
  return true;
}

// ############################################################################
VistaOSGThreadImp::VistaOSGThreadImpFactory::VistaOSGThreadImpFactory() {
  m_nCount = 0;
}

VistaOSGThreadImp::VistaOSGThreadImpFactory::~VistaOSGThreadImpFactory() {
}

IVistaThreadImp* VistaOSGThreadImp::VistaOSGThreadImpFactory::CreateThread(
    const VistaThread& rThread) {
  std::string sName = VistaConversion::ToString(++m_nCount);
  return new VistaOSGThreadImp(sName, const_cast<VistaThread*>(&rThread), 0);
}

long VistaOSGThreadImp::VistaOSGThreadImpFactory::GetCallingThreadIdentity() const {
  // OpenSG doesn't provide thread Id's so we have to revert to our
  // own implementations
  return IVistaThreadImp::GetCallingThreadIdentity(true);
}

bool VistaOSGThreadImp::VistaOSGThreadImpFactory::SetCallingThreadPriority(
    const VistaPriority& oPrio) const {
  return IVistaThreadImp::SetCallingThreadPriority(oPrio, true);
}

bool VistaOSGThreadImp::VistaOSGThreadImpFactory::GetCallingThreadPriority(
    VistaPriority& oPrio) const {
  return IVistaThreadImp::GetCallingThreadPriority(oPrio, true);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
