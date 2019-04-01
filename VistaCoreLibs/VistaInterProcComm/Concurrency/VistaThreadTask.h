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


#ifndef _VISTATHREADTASK_H
#define _VISTATHREADTASK_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include "VistaThread.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaThreadedTask;
class VistaThreadTask;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaThreadTask : public VistaThread
{
public:
	VistaThreadTask();
	virtual ~VistaThreadTask();
	virtual void ThreadBody();

	bool SetThreadedTask(IVistaThreadedTask *);
	IVistaThreadedTask *GetThreadedTask() const;

	virtual void PostRun();
protected:
private:

	IVistaThreadedTask *m_pTask;
};

/**
 * @ingroup VistaInterProcComm
 * 
 * @brief Base class for all kinds of threaded tasks in ViSTA.
 *
 * The interface defines the three methods PreWork(),
 * DefinedThreadWork() and PostWork(), which are executed in that
 * order for the task to be executed. PreWork() is provided for
 * preparations of the actual task, like allocating resources and
 * preparing the job's environment. DefinedThreadWork() is the actual
 * task to be executed, and PostWork() is provided for any kind of
 * cleanup after the task finished, like deallocating resources which
 * were allocated beforehand.
 */
class VISTAINTERPROCCOMMAPI IVistaThreadedTask
{
	friend class VistaThreadTask;
public:
	virtual ~IVistaThreadedTask();

	
	bool GetIsDone() const;

	/**
	 * Indicates whether this work instance is CURRENTLY processed. A
	 * DONE task will return false here!
	 */
	bool GetIsProcessed() const;

	/**
	 * Call this routine to execute the whole threaded task. This
	 * comprises executing PreWork(), DefinedThreadWork() and
	 * PostWork(). Do not override this method in subclasses, but
	 * instead overwrite those three designated methods, respectively.
	 */
	void ThreadWork();

protected:
	IVistaThreadedTask();

    /**
	 * Overwrite this method to prepare your working task. Resource
	 * allocation and other preparation of the actual job execution
	 * should happen here.
	 */
	virtual void PreWork();

	/**
	 * Overwrite this method for any cleanup measures after the task
	 * has finished, like freeing up any allocated resources.
	 */
	virtual void PostWork();

	/**
	 * This method has to be overwritten in subclasses and defines the
	 * actual work this threaded task will perform.
	 */
	virtual void DefinedThreadWork() = 0;

private:
	void StartWork();
	void StopWork();

	bool m_bIsProcessed;
	bool m_bIsDone;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATHREADTASK_H
