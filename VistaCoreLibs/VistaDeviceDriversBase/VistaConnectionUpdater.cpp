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

#include "VistaConnectionUpdater.h"

#include <VistaInterProcComm/AsyncIO/VistaIOHandleBasedMultiplexer.h>
#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>
#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>
#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <cassert>
#include <iostream>
#include <vector>

#if defined(WIN32)
#include <Windows.h>
#else
#include <sys/select.h>
#endif

#include <cstdio>

class VistaUpdateThreadWork : public IVistaThreadPoolWorkInstance {
 public:
  VistaUpdateThreadWork(IVistaExplicitCallbackInterface* pCb)
      : IVistaThreadPoolWorkInstance()
      , m_pJobDone(new VistaThreadEvent(VistaThreadEvent::WAITABLE_EVENT))
      , m_pCallback(pCb)
      , m_nMode(0) {
    // SetAutoRemoveDoneJob(true);
  }

  virtual ~VistaUpdateThreadWork() {
    delete m_pJobDone;
  }

  VistaThreadEvent*                m_pJobDone;
  IVistaExplicitCallbackInterface* m_pCallback;
  int                              m_nMode;

  virtual bool DoneHandshake() {
    m_pJobDone->SignalEvent();
    return true;
  }

 protected:
  virtual void PreWork() {
    m_pCallback->PrepareCallback();
  }

  virtual void DefinedThreadWork() {
    m_pCallback->Do();
  }

  virtual void PostWork() {
    m_pCallback->PostCallback();
  }

 private:
};

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaConnectionUpdater::UpdateThread : public VistaThreadLoop {
 public:
  UpdateThread(int nNumJobs = 3)
      : m_pThreadPool(new VistaThreadPool(nNumJobs, 0, "ConnUpdt")) {
    SetThreadName("Connection Update");
  }

  virtual ~UpdateThread() {
    // clean all threads still in mp
    // should panic when trying to do this with a running connection
    // updater
    for (std::vector<UPDIT>::iterator it = m_vecUpdated.begin(); it != m_vecUpdated.end();
         ++it) // while iterating, increase ticket...
    {
      delete (*it).second;
    }
    delete m_pThreadPool;
  }

  VistaIOHandleBasedIOMultiplexer m_mp;

  bool AddConnectionUpdate(VistaConnection* pCon, IVistaExplicitCallbackInterface* pInt) {
    if (pCon->GetConnectionWaitForDescriptor() == HANDLE(~0)) {
      vstr::errp() << "[ConUpd]: trying to add a connection with an invalid wait-for-descriptor.\n"
                   << vstr::indent
                   << "[ConUpd]: maybe a connection failed to open and you did not check?"
                   << std::endl;
      return false;
    }

    // devices may be demanding in terms of computational time, this is especially
    // true for "slow and small buffered" devices that hang on a serial port.
    // So we tweak the priority a bit, not going to max, as this might slow down the rendering
    // on a single core machine.
    VistaUpdateThreadWork* pUpdateWork = new VistaUpdateThreadWork(pInt);
    VistaPriority prio((VistaPriority::VISTA_MAX_PRIORITY - VistaPriority::VISTA_MID_PRIORITY) / 2);
    pUpdateWork->SetJobPriority(prio);

    m_vecUpdated.push_back(UPDIT(pCon, pUpdateWork));
    if (m_mp.AddMultiplexPoint(pCon->GetConnectionWaitForDescriptor(),
            ((int)m_vecUpdated.size() - 1) + 2, // 1 is reserved in mp
            VistaIOMultiplexer::eIODir(
                VistaIOMultiplexer::MP_IOIN | VistaIOMultiplexer::MP_IOERR))) // add for all access
    {
      return true;
    } else {
      std::vector<UPDIT>::iterator it = m_vecUpdated.end() - 1;
      delete (*it).second;
      m_vecUpdated.erase(it);
      return false;
    }
  }

  IVistaExplicitCallbackInterface* RemConnectionUpdate(VistaConnection* pCon) {
    unsigned int i = 0; // ticket count
    for (std::vector<UPDIT>::iterator it = m_vecUpdated.begin(); it != m_vecUpdated.end();
         ++it, ++i) // while iterating, increase ticket...
    {
      if ((*it).first == pCon) {
        IVistaExplicitCallbackInterface* pI = (*it).second->m_pCallback;
        if ((*it).second->GetIsProcessed()) {
          // panic would be a solution
          //(*it).second->WaitForJobFinish();
          vstr::errp() << "IVistaExplicitCallbackInterface::RemConnectionUpdate() -- "
                       << " Trying to delete a callback that is still processed" << std::endl;
          return NULL;
        }

        if (m_mp.RemMultiplexPointByTicket(i + 2)) // remove from mp
                                                   // remember that tickets are shifted by 2
        {
          // delete carrier (internal helper / the above created update work)
          delete (*it).second;

          // remove from vector
          m_vecUpdated.erase(it);
          return pI; // remove the callback as it does not belong to us
        }
      }
    }
    return NULL; // not found
  }

  IVistaExplicitCallbackInterface* GetConnectionUpdate(VistaConnection* pCon) const {
    for (std::vector<UPDIT>::const_iterator it = m_vecUpdated.begin(); it != m_vecUpdated.end();
         ++it) {
      if ((*it).first == pCon) {
        return (*it).second->m_pCallback;
      }
    }
    return NULL;
  }

 protected:
  void PreLoop() {
    m_pThreadPool->StartPoolWork();
  }

  void PostLoop() {
    m_pThreadPool->WaitForAllJobsDone();
    m_pThreadPool->StopPoolWork();
  }

  bool LoopBody() {
    int nIdx = m_mp.Demultiplex();
    if (nIdx == 0) {
      // timeout
    } else if (nIdx >= 2) {
      // handle action
      const UPDIT& up = m_vecUpdated[nIdx - 2];
      // remove connection handle from active mp mapping
      if (!up.first->GetIsFine()) {
        VISTA_VERIFY(m_mp.RemMultiplexPointByTicket(nIdx), true);
      }
      if (up.second->m_nMode == 0) {
        up.second->m_nMode = 1;
        VISTA_VERIFY(m_mp.RemMultiplexPointByTicket(nIdx), true);

        up.second->m_pJobDone->ResetThisEvent();
        //  				printf("adding event  job wait\n");

        // insert handle for callback-thread work instead
        m_mp.AddMultiplexPoint(up.second->m_pJobDone->GetEventWaitHandle(), nIdx,
            VistaIOMultiplexer::eIODir(VistaIOMultiplexer::MP_IOIN | VistaIOMultiplexer::MP_IOERR));

        // add update job to thread pool
        m_pThreadPool->AddWork(up.second);
        //  				printf("DONE\n") ;
      } else {
        up.second->m_nMode = 0;
        // remove event handle from active mp mapping
        bool b = m_mp.RemMultiplexPointByTicket(nIdx);
        assert(b == true);

        // this is a polling approach, as we need to wait for
        // the job to be in the done queue of the thread pool
        // in order to re-use it later on again.
        // however, when we get the signal that the job is done,
        // we get this by the thread that is through its
        // workload routine, but is still not 'really' done,
        // especially, it is not in the done queue
        while (m_pThreadPool->RemoveDoneJob(up.second->GetJobId()) == NULL) {
          VistaTimeUtils::Sleep(10); // we should not get here in the ideal case
                                     // but we give a timeout of 10ms in order to
                                     // relax the locking of the done-queue-lock
                                     // in the thread pool
        }

        // add connection handle instead
        b = m_mp.AddMultiplexPoint(HANDLE(up.first->GetConnectionWaitForDescriptor()), nIdx,
            VistaIOMultiplexer::eIODir(VistaIOMultiplexer::MP_IOIN | VistaIOMultiplexer::MP_IOERR));

        assert(b == true);
      }
    } else {
      // error!
      IndicateLoopEnd();
    }
    return false;
  }

 private:
  typedef std::pair<VistaConnection*, VistaUpdateThreadWork*> UPDIT;

  std::vector<UPDIT> m_vecUpdated;
  VistaThreadPool*   m_pThreadPool;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaConnectionUpdater::VistaConnectionUpdater(int nNumJobs)
    : m_pUpdateThread(new UpdateThread(nNumJobs)) {
}

VistaConnectionUpdater::~VistaConnectionUpdater() {
  ShutdownUpdaterLoop();
  delete m_pUpdateThread;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaConnectionUpdater::AddConnectionUpdate(
    VistaConnection* pCon, IVistaExplicitCallbackInterface* pInt) {
  return (*m_pUpdateThread).AddConnectionUpdate(pCon, pInt);
}

IVistaExplicitCallbackInterface* VistaConnectionUpdater::RemConnectionUpdate(
    VistaConnection* pCon) {
  return (*m_pUpdateThread).RemConnectionUpdate(pCon);
}

IVistaExplicitCallbackInterface* VistaConnectionUpdater::GetConnectionUpdate(
    VistaConnection* pCon) const {
  return (*m_pUpdateThread).GetConnectionUpdate(pCon);
}

bool VistaConnectionUpdater::StartUpdaterLoop() {
  if (!m_pUpdateThread->IsRunning())
    return m_pUpdateThread->Run();

  return true;
}

bool VistaConnectionUpdater::ShutdownUpdaterLoop(bool bWaitFinished) {
  if (m_pUpdateThread->IsRunning()) {
    m_pUpdateThread->m_mp.Shutdown();
    return m_pUpdateThread->Join();
  }
  return false;
}

bool VistaConnectionUpdater::GetIsDispatching() const {
  return m_pUpdateThread->IsRunning();
}
