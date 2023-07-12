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

#include <VistaBase/VistaStreamUtils.h>

#include "VistaPthreadsThreadImp.h"
#include <VistaInterProcComm/Concurrency/VistaPriority.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>

#include <exception>

#include <signal.h>

using namespace std;

#include <cerrno>
#include <cstdlib>
#include <cstring>

/** @todo check this on other platforms than unix */
// this seems to be glibc (GNU) specific and available only in >= glibc-2.6
#include <utmpx.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static void* PosixEntryPoint(void* that) {
  VistaThread* pThread = reinterpret_cast<VistaThread*>(that);
  try {
    pThread->PreRun();
    pThread->ThreadBody();
    pThread->PostRun();
    return 0;
  } catch (std::exception& x) {
    vstr::err() << "PThreads-VistaThreadImp -- ["
                << (pThread ? pThread->GetThreadName() : "invalid-thread-handle")
                << "] -- execution terminated, given exception: " << x.what() << std::endl;
  } catch (...) {
    vstr::err() << "PThreads-VistaThreadImp -- ["
                << (pThread ? pThread->GetThreadName() : "invalid-thread-handle")
                << "] -- execution terminated due to unkown exception (ellipse catch).";
  }
  return 0;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPthreadThreadImp::VistaPthreadThreadImp(const VistaThread& thread)
    : m_rThread(thread)
    , m_nPriority(-1) {
  m_bCanBeCancelled = false;
  posixThreadID     = 0;
  pthread_attr_init(&m_ptAttr); // default value init
}

VistaPthreadThreadImp::~VistaPthreadThreadImp() {
#if defined(DEBUG)
  if (posixThreadID)
    vstr::outi() << "VistaPthreadThreadImp::~VistaPthreadThreadImp() -- deleting non join'ed thread"
                 << std::endl;
#endif
  pthread_attr_destroy(&m_ptAttr);
}

/// start a process
bool VistaPthreadThreadImp::Run() {
  // assure only one running thread per instance
  if (posixThreadID)
    return false;

  int error = pthread_create(&posixThreadID, &m_ptAttr, PosixEntryPoint, (void*)(&m_rThread));
  if (error == 0) {
    if (m_nPriority != -1)
      SetPriority(m_nPriority);
    return true;
  }

  switch (error) {

  case EAGAIN:
    vstr::errp() << "The maximum number of threads has been created [see NOTES, "
                 << "and setrlimit()] or there is insufficient memory to create the thread"
                 << std::endl;
    break;
  case ENOMEM:
    vstr::errp() << "The system lacked the necessary resources to create another thread"
                 << std::endl;
    break;
  case EINVAL:
    vstr::errp() << "The value specified by attr is invalid" << std::endl;
    break;
  case EPERM:
    vstr::errp() << "The caller does not have appropriate permission to set "
                 << "the required scheduling parameters or scheduling policy" << std::endl;
    break;
  case ENOSYS:
    vstr::errp() << "A call to pthread_create() was made from an executable "
                 << "which is not linked against the POSIX threads library. "
                 << "This typically occurs when the argument \"-lpthread\" is "
                 << "accidentally omitted during compilation of the executable" << std::endl;
    break;
  default:
    vstr::errp() << "Unkown Error (" << error << ")" << std::endl;
    break;
  }

  return false;
}

bool VistaPthreadThreadImp::Suspend() {
  if (posixThreadID)
    return (pthread_kill(posixThreadID, SIGSTOP) == 0);
  return true; // we do consider no thread to be suspended
}

bool VistaPthreadThreadImp::Resume() {
  if (posixThreadID)
    return (pthread_kill(posixThreadID, SIGCONT) == 0);
  return false; // no thread can not be sumed
}

bool VistaPthreadThreadImp::Join() {
  if (posixThreadID) {
    int yeah = pthread_join(posixThreadID, NULL);
    if (yeah == 0) {
      posixThreadID = 0; // ok, clear handle, thread is done
    } else {
      vstr::errp() << "Fail on join @ thread [" << m_rThread.GetThreadName() << "]" << std::endl;
      vstr::IndentObject oIndent;

      switch (yeah) {
      case EDEADLK: {
        vstr::err() << "Deadlock detected, maybe join on meself?" << std::endl;
        break;
      }
      case EINVAL: {
        vstr::err() << "This thread is not joinable, did you detach it?" << std::endl;
        break;
      }
      case ESRCH: {
        vstr::err() << "No thread found by that id. Already terminated?" << std::endl;
        posixThreadID = 0;
        return true;
      }
      default:
        break;
      }
    }
    return (yeah == 0);
  }
  return false;
}

bool VistaPthreadThreadImp::Abort() {
  return (pthread_cancel(posixThreadID) == 0);
}

bool VistaPthreadThreadImp::SetPriority(const VistaPriority& inPrio) {
  if (!posixThreadID) {
    m_nPriority = inPrio.GetVistaPriority();
    return true;
  }

  struct sched_param sp;

  memset(&sp, 0, sizeof(struct sched_param));

  sp.sched_priority = inPrio.GetSystemPriority();
  return (pthread_setschedparam(posixThreadID, SCHED_RR, &sp) == 0);
}

void VistaPthreadThreadImp::GetPriority(VistaPriority& outPrio) const {
  if (!posixThreadID) {
    outPrio.SetVistaPriority(m_nPriority);
    return;
  }

  int                policy;
  struct sched_param sp;

  memset(&sp, 0, sizeof(struct sched_param));

  if (pthread_getschedparam(posixThreadID, &policy, &sp) == 0) {
    vstr::err() << "pthread_getschedparam() failed." << std::endl;
  } else {
    outPrio.SetVistaPriority(outPrio.GetVistaPriorityForSystemPriority(sp.sched_priority));
  }
}

/**
 * give the processor away temporarily
 */
void VistaPthreadThreadImp::YieldThread() {
  sched_yield();
}

void VistaPthreadThreadImp::SetCancelAbility(const bool bOkToCancel) {
}

bool VistaPthreadThreadImp::CanBeCancelled() const {
  return m_bCanBeCancelled;
}

void VistaPthreadThreadImp::PreRun() {
}

void VistaPthreadThreadImp::PostRun() {
}

bool VistaPthreadThreadImp::Equals(const IVistaThreadImp& oImp) const {
  return (pthread_equal(
              posixThreadID, static_cast<const VistaPthreadThreadImp&>(oImp).posixThreadID) != 0);
}

long VistaPthreadThreadImp::GetThreadIdentity() const {
  return (long)posixThreadID;
}

long VistaPthreadThreadImp::GetCallingThreadIdentity() {
  return (long)pthread_self();
}

bool VistaPthreadThreadImp::SetProcessorAffinity(int iProcessorNum) {
#if defined(__USE_GNU)
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(iProcessorNum, &mask);

  if (posixThreadID == 0) {
    // thread not created yet, create a thread attr to indicate the
    // cpu-set for this thread
    if (pthread_attr_setaffinity_np(&m_ptAttr, sizeof(cpu_set_t), &mask) != 0) {
      vstr::errp() << "could not set thread affinity on attribute" << std::endl;
      return false;
    } else
      return true;
  } else {
    if (pthread_setaffinity_np(posixThreadID, sizeof(cpu_set_t), &mask) == 0)
      return true;
    else
      return false;
  }
#else
  return false;
#endif
}

int VistaPthreadThreadImp::GetCpu() const {
#if __GLIBC_MINOR__ >= 6
  /** @todo check: is there a pthreads call for this? */
  return sched_getcpu(); // define me in subclass
#else
  return -1;
#endif
}

bool VistaPthreadThreadImp::SetCallingThreadPriority(const VistaPriority& oPrio) {
  struct sched_param sp;

  memset(&sp, 0, sizeof(struct sched_param));
  sp.sched_priority = oPrio.GetSystemPriority();
  return (pthread_setschedparam(pthread_self(), SCHED_RR, &sp) == 0);
}

bool VistaPthreadThreadImp::GetCallingThreadPriority(VistaPriority& oPrio) {
  int                policy;
  struct sched_param sp;

  memset(&sp, 0, sizeof(struct sched_param));

  if (pthread_getschedparam(pthread_self(), &policy, &sp) == 0)
    return false;

  oPrio.SetVistaPriority(oPrio.GetVistaPriorityForSystemPriority(sp.sched_priority));
  return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
