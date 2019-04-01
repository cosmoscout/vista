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


#ifndef _VISTATHREADPOOL_H
#define _VISTATHREADPOOL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaPriority.h"

#include <vector>
#include <deque>
#include <map>

#include "VistaThreadTask.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaThread;
class VistaMutex;
class VistaThreadCondition;
class VistaThreadPoolThread;
class VistaSemaphore;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IVistaThreadPoolWorkInstance : public IVistaThreadedTask
{
	friend class VistaThreadPoolThread;
	friend class VistaThreadPool;
public:
	virtual ~IVistaThreadPoolWorkInstance();

	/**
	 * @return -1 for not scheduled, yet, >= 0 else
	 */
	int GetJobId() const { return m_nJobId; };

	void SetJobPriority(const VistaPriority &);
	void GetJobPriority(VistaPriority &) const;

	//bool WaitForJobFinish();

	//! @return 0 for NONE, else the cpu of desire to run on
	int GetCpuAffinity() const;

	//! @brief sets the desired cpu to run on
	//! This call does not check for validity of the cpu (existence),
	//! also it is assumed that the set call happened <b>before</b> the task is executed, not while it is executed.
	void SetCpuAffinity( int cpu_index );


	bool GetReenqueueTask () const;
	void SetReenqueueTask (bool reenqueue);

	void SetAutoRemoveDoneJob(bool bDoAutoremove);
	bool GetAutoRemoveDoneJob() const;

	virtual bool DoneHandshake();

protected:
	IVistaThreadPoolWorkInstance();
private:
	int  m_nJobId;
	int  m_cpu_affinity;
	VistaPriority m_oPrio;
	VistaThreadPoolThread *m_pProcessingThread;

	VistaSemaphore *m_pAccess;
	VistaMutex *m_pFinishMutex;

	VistaThreadCondition *m_pFinishCondition;

	bool                    m_bReenqueueTask,
							m_bRemoveDoneJob;
	VistaMutex             *m_pReenqueueMutex;
};

class VISTAINTERPROCCOMMAPI VistaThreadPool
{
	friend class VistaThreadPoolThread;
public:
	/**
	 * @param iNumMaxTasks 0 means "no limit"
	 */
	VistaThreadPool(int iNumMaxThreads, int iNumMaxTasks = 0,
					 const std::string &sThreadNamePrefix = "tp_");
	virtual ~VistaThreadPool();

	int AddWork(IVistaThreadPoolWorkInstance *);
	bool RemoveWork(IVistaThreadPoolWorkInstance *);
	IVistaThreadPoolWorkInstance *GetWorkByIndex(int) const;
	int GetNumberOfJobs() const;

	int GetNumberOfThreads() const;

	// resize the maximal number of threads
	// you can only make the pool bigger, never smaller
	// returns the new maximal number of threads (equals old, if not successful)
	int ResizeMaxThreads (int iNumMaxThreads);

	/**
	 * currently not working!!
	 */
	//bool WaitForJob(int iIndex) const;
	//bool WaitForJob(IVistaThreadPoolWorkInstance *) const;


	bool StartPoolWork();
	bool StopPoolWork();

	bool GetIsPoolRunning() const;

	bool IsShutdownFlagSet() const;
	bool IsPoolClosed() const;
	bool AreThereStillJobsProcessed() const;
	bool WaitForWorkQueueEmpty() const;

	//! @returns true in case all jobs are done, no error, false else
	bool WaitForAllJobsDone() const;

	int GetNumberOfPendingDoneJobs() const;
	int GetNumberOfTotalWork() const;
	int GetNumberOfTotalDoneWork() const;
	int GetNumberOfCurrentlyProcessedJobs() const;

	IVistaThreadPoolWorkInstance *RemoveDoneJob(int iJobId);
	IVistaThreadPoolWorkInstance *ReadDoneJob(int iJobId);

	IVistaThreadPoolWorkInstance *RemoveNextDoneJob();
	IVistaThreadPoolWorkInstance *ReadNextDoneJob();

	IVistaThreadPoolWorkInstance *GetJobById(int iJobId) const;

protected:
private:
	/// @todo give typedefs nonuppercase names to ease evolution to classes 
	/// (refer to the PropertyList "problem" we are facing currently...)
	typedef std::map<int, IVistaThreadPoolWorkInstance*> DONEMAP;

	typedef std::pair<VistaMutex*, VistaThreadCondition*> MTPAIR;

	std::vector<VistaThreadPoolThread *> m_vThreadPool;
	std::vector<MTPAIR> m_vFinishConditions;
	std::deque<IVistaThreadPoolWorkInstance *> m_vWorkPool;

	std::string m_sThreadNamePrefix;

	DONEMAP m_mpDonePool;

	VistaMutex *m_pQueueLock;
	VistaMutex *m_pDoneLock;

	VistaThreadCondition *m_pQueueNotEmpty;
	VistaThreadCondition *m_pQueueEmpty;
	VistaThreadCondition *m_pAllJobsDone;

	volatile bool m_bQueueClosed;
	volatile bool m_bShutdown;
	volatile bool m_bWorkStarted;
	volatile int  m_nMaxThreads;
	volatile int  m_nTotalNumberOfJobs;
	volatile int  m_nTotalNumberOfProcessedJobs;
	volatile int  m_nNumberOfCurrentlyProcessedJobs;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

