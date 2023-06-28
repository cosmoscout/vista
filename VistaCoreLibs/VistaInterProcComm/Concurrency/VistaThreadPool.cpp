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

#include <cstdarg>
#include <cstdio>

#include <algorithm>
#include <deque>
using namespace std;

#include "VistaMutex.h"
#include "VistaSemaphore.h"
#include "VistaThread.h"
#include "VistaThreadPool.h"

#include "VistaThreadCondition.h"

#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class VistaThreadPoolThread : public VistaThread {
 public:
  VistaThreadPoolThread(VistaThreadPool* pPool, int iId);
  virtual ~VistaThreadPoolThread();

  void ThreadBody();

  bool GetEnableDebug() const {
    return (m_fProtOut != NULL);
  }

  void SetEnableDebug(const char* pcFileName) {
    if (pcFileName) {
      if (!m_fProtOut) {
        m_fProtOut = fopen(pcFileName, "a+");
        char c[4096];
        sprintf(c, "Enabled log for thread %ld, %d @ %lx\n", this->GetThreadIdentity(), m_id,
            long(this));
        PutMsg(c);
      }
    } else {
      if (m_fProtOut) {
        char c[4096];
        sprintf(c, "Closed log for thread %ld, %d @ %lx\n", this->GetThreadIdentity(), m_id,
            long(this));
        fclose(m_fProtOut);
        m_fProtOut = NULL;
      }
    }
  }

 protected:
  void PutMsg(const char* pcMsg) {
    if (m_fProtOut) {
      fprintf(m_fProtOut, "%s", pcMsg);
    } else {
      printf("%s", pcMsg);
    }
  }

 private:
  VistaThreadPool* m_pThreadPool;
  int              m_id;
  FILE*            m_fProtOut;
};

VistaThreadPoolThread::VistaThreadPoolThread(VistaThreadPool* pPool, int iId)
    : m_pThreadPool(pPool)
    , m_id(iId)
    , m_fProtOut(NULL) {
}

VistaThreadPoolThread::~VistaThreadPoolThread() {
  if (m_fProtOut) {
    fclose(m_fProtOut);
  }
  m_pThreadPool = (VistaThreadPool*)0xDEADBEEF;
}

void VistaThreadPoolThread::ThreadBody() {
  for (;;) {
    //  printf("%d: ... Waiting for QueueLock\n", m_id);
    m_pThreadPool->m_pQueueLock->Lock();
    //  printf("%d: ... got queuelock\n", m_id);
    while (m_pThreadPool->m_vWorkPool.empty() && !m_pThreadPool->IsShutdownFlagSet()) {
      //      printf("%d: ... waiting for queue not empty condition\n", m_id);
      m_pThreadPool->m_pQueueNotEmpty->WaitForCondition(*m_pThreadPool->m_pQueueLock);
      // printf("%d: ... QUEUE NOT EMPTY (%d)\n", m_id, m_pThreadPool->m_vWorkPool.size());
    }

    if (m_pThreadPool->IsShutdownFlagSet()) {
      m_pThreadPool->m_pQueueLock->Unlock();
      //      printf("%d: ... thread exit\n", m_id);
      return; // leave thread-body!!
    }

#if 0
		char buffer[4096];
		sprintf(buffer, "thread=%ld -- TP @%x has size = %d\n",
			   GetThreadIdentity(), m_pThreadPool, m_pThreadPool->m_vWorkPool.size());
		PutMsg(buffer);
#endif
    // PUTMSG("thread=%s %ld -- TP @%x has size = %d\n", (pTest, GetThreadIdentity(), m_pThreadPool,
    // m_pThreadPool->m_vWorkPool.size()));
    IVistaThreadPoolWorkInstance* pWork = m_pThreadPool->m_vWorkPool.front();

#if 0
		sprintf(buffer, "TP @%x has size = %d\ndequed front @ %x",
			m_pThreadPool, m_pThreadPool->m_vWorkPool.size(),
			pWork);
		PutMsg(buffer);
#endif

    // PUTMSG("TP @%x has size = %d\ndequed front @ %x",
    //	     (m_pThreadPool, m_pThreadPool->m_vWorkPool.size(),
    //		 pWork));

    // set up condition and mutex
    pWork->m_pFinishMutex     = m_pThreadPool->m_vFinishConditions[m_id].first;
    pWork->m_pFinishCondition = m_pThreadPool->m_vFinishConditions[m_id].second;

    m_pThreadPool->m_vWorkPool.pop_front();

    ++m_pThreadPool->m_nNumberOfCurrentlyProcessedJobs;

    if (m_pThreadPool->m_vWorkPool.empty())
      m_pThreadPool->m_pQueueEmpty->SignalCondition();

    m_pThreadPool->m_pQueueLock->Unlock();

    VistaPriority prio;
    pWork->GetJobPriority(prio);
    this->SetPriority(prio);

    if (pWork->GetCpuAffinity() > 0)
      SetProcessorAffinity(pWork->GetCpuAffinity());

    pWork->m_pProcessingThread = this;
#if 0
		sprintf(buffer, "%d: ... unlocked queue, starting work with job-id [%d].\n",
						m_id, pWork->GetJobId());
		PutMsg(buffer);
#endif
    // printf("%d: ... unlocked queue, starting work with job-id [%d].\n", m_id, pWork->GetJobId());
    pWork->ThreadWork(); // process work

    m_pThreadPool->m_pDoneLock->Lock();
    pWork->m_pProcessingThread = NULL;

    bool enqueue_as_done_job = !pWork->GetAutoRemoveDoneJob();

    if (enqueue_as_done_job)
      m_pThreadPool->m_mpDonePool[pWork->GetJobId()] = pWork;

    /** @todo check this for deadlock capabilities! (should be ok) */
    // m_pThreadPool->m_pQueueLock->Lock();
    ++m_pThreadPool->m_nTotalNumberOfProcessedJobs;
    --m_pThreadPool->m_nNumberOfCurrentlyProcessedJobs;
    /** @todo check this for deadlock capabilities! (should be ok) */
    // m_pThreadPool->m_pQueueLock->Unlock();

    // now tell everybody waiting that things are finished

    if (pWork->m_pFinishCondition)
      pWork->m_pFinishCondition->BroadcastCondition();
    else {
      vstr::warnp() << "[VistaThreadPoolThread::ThreadBody]: No Finishing Condition!" << std::endl;
    }

    pWork->m_pFinishCondition = NULL;
    pWork->m_pFinishMutex     = NULL;

    if (m_pThreadPool->AreThereStillJobsProcessed() == false) {
      m_pThreadPool->m_pAllJobsDone->SignalCondition();
    }
    m_pThreadPool->m_pDoneLock->Unlock();

    if (pWork->GetReenqueueTask()) {
      if (enqueue_as_done_job && m_pThreadPool->RemoveDoneJob(pWork->GetJobId()) != pWork)
        PutMsg("Error... remove job did not return pWork\n");
      else
        m_pThreadPool->AddWork(pWork);
    } else {
      pWork->DoneHandshake();
    }
  }
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaThreadPoolWorkInstance::IVistaThreadPoolWorkInstance()
    : IVistaThreadedTask()
    , m_nJobId(-1)
    , // none
    m_cpu_affinity(0)
    , // none
    m_oPrio(VistaPriority::VISTA_MID_PRIORITY)
    , m_pProcessingThread(NULL)
    , m_pAccess(new VistaSemaphore(1, VistaSemaphore::SEM_TYPE_FASTEST))
    , m_pFinishMutex(NULL)
    , m_pFinishCondition(NULL)
    , m_bReenqueueTask(false)
    , m_bRemoveDoneJob(false)
    , m_pReenqueueMutex(new VistaMutex) {
}

IVistaThreadPoolWorkInstance::~IVistaThreadPoolWorkInstance() {
  delete m_pReenqueueMutex;
  delete m_pAccess;
}

void IVistaThreadPoolWorkInstance::SetJobPriority(const VistaPriority& oPrio) {
  m_oPrio = oPrio;
  if (m_pProcessingThread)
    m_pProcessingThread->SetPriority(m_oPrio); // mirror to thread
}

int IVistaThreadPoolWorkInstance::GetCpuAffinity() const {
  return m_cpu_affinity;
}

void IVistaThreadPoolWorkInstance::SetCpuAffinity(int cpu_index) {
  m_cpu_affinity = cpu_index;
}

void IVistaThreadPoolWorkInstance::GetJobPriority(VistaPriority& oPrio) const {
  oPrio = m_oPrio;
}

bool IVistaThreadPoolWorkInstance::GetReenqueueTask() const {
  VistaMutexLock l(*m_pReenqueueMutex);
  return m_bReenqueueTask;
}

void IVistaThreadPoolWorkInstance::SetReenqueueTask(bool reenqueue) {
  VistaMutexLock l(*m_pReenqueueMutex);
  m_bReenqueueTask = reenqueue;
}

// bool IVistaThreadPoolWorkInstance::WaitForJobFinish()
//{
//	//VistaSemaphoreLock l(*m_pAccess);
//    if(m_pFinishMutex && m_pFinishCondition)
//    {
//        VistaMutexLock l(*m_pFinishMutex);
//        return ((*m_pFinishCondition).WaitForCondition(*m_pFinishMutex) == 0);
//    }
//    return true; // we assume job to be finished
//}

void IVistaThreadPoolWorkInstance::SetAutoRemoveDoneJob(bool bDoAutoremove) {
  this->m_bRemoveDoneJob = bDoAutoremove;
}

bool IVistaThreadPoolWorkInstance::GetAutoRemoveDoneJob() const {
  return this->m_bRemoveDoneJob;
}

bool IVistaThreadPoolWorkInstance::DoneHandshake() {
  return true;
}

// #########################################################################

VistaThreadPool::VistaThreadPool(
    int iNumMaxThreads, int iNumMaxTasks, const string& sThreadNamePrefix)
    : m_bWorkStarted(false)
    , m_bQueueClosed(false)
    , m_bShutdown(false)
    , m_nMaxThreads(iNumMaxThreads)
    , m_nTotalNumberOfJobs(0)
    , m_nTotalNumberOfProcessedJobs(0)
    , m_nNumberOfCurrentlyProcessedJobs(0)
    , m_sThreadNamePrefix(sThreadNamePrefix) {
  m_pQueueLock     = new VistaMutex;
  m_pDoneLock      = new VistaMutex;
  m_pQueueEmpty    = new VistaThreadCondition;
  m_pQueueNotEmpty = new VistaThreadCondition;
  m_pAllJobsDone   = new VistaThreadCondition;
}

VistaThreadPool::~VistaThreadPool() {
  StopPoolWork();

  delete m_pQueueLock;
  delete m_pDoneLock;
  delete m_pQueueEmpty;
  delete m_pQueueNotEmpty;
  delete m_pAllJobsDone;
}

bool VistaThreadPool::IsPoolClosed() const {
  return m_bQueueClosed;
}

bool VistaThreadPool::IsShutdownFlagSet() const {
  return m_bShutdown;
}

IVistaThreadPoolWorkInstance* VistaThreadPool::GetWorkByIndex(int iIndex) const {
  VistaMutexLock l(*m_pQueueLock);
  return m_vWorkPool[iIndex];
}

int VistaThreadPool::GetNumberOfJobs() const {
  VistaMutexLock l(*m_pQueueLock);
  return (int)m_vWorkPool.size();
}

// bool VistaThreadPool::WaitForJob(int iIndex) const
//{
//    IVistaThreadPoolWorkInstance *pJob = GetJobById(iIndex);
//    if(pJob->GetIsDone())
//        return true;
//
//    // join on thread
//    return pJob->WaitForJobFinish();
//}
//
//
// bool VistaThreadPool::WaitForJob(IVistaThreadPoolWorkInstance *pJob) const
//{
//    if(pJob==NULL)
//        return true; // we can always stop "nothing"
//    return pJob->WaitForJobFinish();
//}

IVistaThreadPoolWorkInstance* VistaThreadPool::GetJobById(int iJobId) const {
  // check for done job
  VistaMutexLock          l(*m_pDoneLock);
  DONEMAP::const_iterator cit = m_mpDonePool.find(iJobId);
  if (cit != m_mpDonePool.end()) {
    // ok, was done
    return (*cit).second;
  }

  l.Unlock();

  VistaMutexLock l1(*m_pQueueLock);

  for (deque<IVistaThreadPoolWorkInstance*>::const_iterator cit1 = m_vWorkPool.begin();
       cit1 != m_vWorkPool.end(); ++cit1) {
    if ((*cit1)->GetJobId() == iJobId)
      return *cit1;
  }

  return NULL; // not found!
}

bool VistaThreadPool::WaitForWorkQueueEmpty() const {
  VistaMutexLock l(*m_pQueueLock);
  return (m_pQueueEmpty->WaitForCondition(*m_pQueueLock) != 0);
}

bool VistaThreadPool::WaitForAllJobsDone() const {
  VistaMutexLock l(*m_pDoneLock);
  if (!AreThereStillJobsProcessed())
    return true;
  return (m_pAllJobsDone->WaitForCondition(*m_pDoneLock) == 0);
}

bool VistaThreadPool::StartPoolWork() {
  if (m_bWorkStarted)
    return false; // we do this only ONCE!

  char buffer[256];

  for (int i = 0; i < m_nMaxThreads; ++i) {
    VistaThreadPoolThread* pThread = new VistaThreadPoolThread(this, i);
    sprintf(buffer, "%s[%d]", m_sThreadNamePrefix.c_str(), i);
    pThread->SetThreadName(buffer);

    m_vThreadPool.push_back(pThread);
    m_vFinishConditions.push_back(MTPAIR(new VistaMutex, new VistaThreadCondition));
    //		(m_vThreadPool[i])->SetEnableDebug(buffer);
    (m_vThreadPool[i])->Run();
  }

  m_bWorkStarted = true;
  return true;
}

bool VistaThreadPool::GetIsPoolRunning() const {
  return m_bWorkStarted;
}

bool VistaThreadPool::StopPoolWork() {
  if (!m_bWorkStarted)
    return true; // we do not care for work that did not begin, yet

  m_pQueueLock->Lock();
  if (IsPoolClosed() || IsShutdownFlagSet()) {
    m_pQueueLock->Unlock();
    return false;
  }

  m_bQueueClosed = true;
  while (!m_vWorkPool.empty())
    m_pQueueEmpty->WaitForCondition(*m_pQueueLock);
  m_bShutdown = true;
  m_pQueueLock->Unlock();
  m_pQueueNotEmpty->BroadcastCondition();

  for (unsigned int i = 0; i < m_vThreadPool.size(); ++i) {
    m_vThreadPool[i]->Join();
    delete m_vThreadPool[i];
  }
  for (vector<MTPAIR>::const_iterator cit = m_vFinishConditions.begin();
       cit != m_vFinishConditions.end(); ++cit) {
    delete (*cit).first;
    delete (*cit).second;
  }

  for (unsigned int j = 0; j < m_vWorkPool.size(); ++j)
    delete m_vWorkPool[j];

  m_vThreadPool.clear();
  m_vWorkPool.clear();

  m_bWorkStarted = false;

  return true;
}

int VistaThreadPool::GetNumberOfTotalWork() const {
  return m_nTotalNumberOfJobs;
}

int VistaThreadPool::GetNumberOfTotalDoneWork() const {
  return m_nTotalNumberOfProcessedJobs;
}

int VistaThreadPool::GetNumberOfCurrentlyProcessedJobs() const {
  return m_nNumberOfCurrentlyProcessedJobs;
}

int VistaThreadPool::ResizeMaxThreads(int iNumMaxThreads) {
  int iNewMaxThreads = m_nMaxThreads;

  m_pQueueLock->Lock();
  if (IsPoolClosed()) {
    m_pQueueLock->Unlock();
    return iNewMaxThreads;
  }

  // pool work started already and we try to make the pool bigger !
  if (GetIsPoolRunning() && (iNumMaxThreads > m_nMaxThreads)) {
    char buffer[265];
    for (int i = m_nMaxThreads; i < iNumMaxThreads; ++i) {
      VistaThreadPoolThread* pThread = new VistaThreadPoolThread(this, i);
      sprintf(buffer, "%s[%d]", m_sThreadNamePrefix.c_str(), i);
      pThread->SetThreadName(buffer);
      m_vThreadPool.push_back(pThread);
      m_vFinishConditions.push_back(MTPAIR(new VistaMutex, new VistaThreadCondition));
      (m_vThreadPool[i])->Run();
    }
    m_nMaxThreads  = iNumMaxThreads;
    iNewMaxThreads = m_nMaxThreads;
  }

  m_pQueueLock->Unlock();

  return iNewMaxThreads;
}

int VistaThreadPool::AddWork(IVistaThreadPoolWorkInstance* pWork) {
  m_pQueueLock->Lock();
  if (IsPoolClosed()) {
    m_pQueueLock->Unlock();
    return false;
  }

  deque<IVistaThreadPoolWorkInstance*>::const_iterator cit =
      std::find(m_vWorkPool.begin(), m_vWorkPool.end(), pWork);
  if (cit != m_vWorkPool.end()) {
    vstr::warnp() << "[VistaThreadPool::AddWork]: Work Instance already registered" << std::endl;
  }

  m_vWorkPool.push_back(pWork);
  pWork->m_nJobId = ++m_nTotalNumberOfJobs;
  if (m_vWorkPool.size() == 1)
    m_pQueueNotEmpty->BroadcastCondition();

  m_pQueueLock->Unlock();
  return true;
}

bool VistaThreadPool::RemoveWork(IVistaThreadPoolWorkInstance* pWork) {
  m_pQueueLock->Lock();
  deque<IVistaThreadPoolWorkInstance*>::iterator it =
      find(m_vWorkPool.begin(), m_vWorkPool.end(), pWork);

  if (it != m_vWorkPool.end()) {
    m_vWorkPool.erase(it);
    // consider removed jobs as done
    ++m_nTotalNumberOfProcessedJobs;
    m_pQueueLock->Unlock();
    return true;
  } else {
    m_pQueueLock->Unlock();
  }

  return false;
}

bool VistaThreadPool::AreThereStillJobsProcessed() const {
  /** @todo check this for deadlock capabilities! (quite unsure here!!) */
  // VistaMutexLock l(*m_pQueueLock);
  return (m_nTotalNumberOfJobs != m_nTotalNumberOfProcessedJobs);
}

int VistaThreadPool::GetNumberOfPendingDoneJobs() const {
  VistaMutexLock l(*m_pDoneLock);
  int            iRet = (int)m_mpDonePool.size();
  return iRet;
}

IVistaThreadPoolWorkInstance* VistaThreadPool::RemoveDoneJob(int iJobId) {
  VistaMutexLock    l(*m_pDoneLock);
  DONEMAP::iterator it = m_mpDonePool.find(iJobId);
  if (it != m_mpDonePool.end()) {
    // found it.
    IVistaThreadPoolWorkInstance* ins = (*it).second;
    m_mpDonePool.erase(it);
    return ins;
  }
  return NULL;
}

IVistaThreadPoolWorkInstance* VistaThreadPool::ReadDoneJob(int iJobId) {
  VistaMutexLock          l(*m_pDoneLock);
  DONEMAP::const_iterator it = m_mpDonePool.find(iJobId);
  if (!(it == m_mpDonePool.end())) {
    // found it.
    return (*it).second;
  }
  return NULL;
}

IVistaThreadPoolWorkInstance* VistaThreadPool::RemoveNextDoneJob() {
  VistaMutexLock l(*m_pDoneLock);
  if (!m_mpDonePool.empty()) {
    IVistaThreadPoolWorkInstance* ins = (*m_mpDonePool.begin()).second;
    m_mpDonePool.erase(m_mpDonePool.begin());
    return ins;
  } else
    return NULL;
}

IVistaThreadPoolWorkInstance* VistaThreadPool::ReadNextDoneJob() {
  VistaMutexLock l(*m_pDoneLock);
  if (!m_mpDonePool.empty())
    return (*m_mpDonePool.begin()).second;
  else
    return NULL;
}

int VistaThreadPool::GetNumberOfThreads() const {
  return m_nMaxThreads;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
