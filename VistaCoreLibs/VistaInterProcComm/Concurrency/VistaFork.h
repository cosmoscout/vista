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


#ifndef _VISTAFORK_H
#define _VISTAFORK_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include "VistaIpcThreadModel.h"


/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/

class VistaPriority;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * @ingroup VistaInterProcComm
 *
 * This is the base class for all concurrency issues. Forking is a way
 * to split the path of execution into more than just one track. This
 * is the basis for threads as well as processes.  @todo remove or
 * rename this..."fork" is misleading
 */
class VISTAINTERPROCCOMMAPI VistaFork
{

public:
	/**
	 * Empty constructor.
	 */
	VistaFork() {};


	/**
	 * Empty destructor.
	 */
	virtual ~VistaFork() {};

	/**
	 * A suspended thread can be resumed. Consider this to be an
	 * atomic "pause". Note that the pause state can be ANY state in
	 * the execution path of any process or thread. You should usually
	 * only suspend threads that have no timing issues or the like. BE
	 * SURE TO KNOW WHAT YOU ARE DOING WHEN CALLING SUSPEND.  
	 *
	 * @return true iff the thread is suspended and waiting for resume.
	 */
	virtual bool Suspend() = 0;


	/**
	 * A thread that is suspended can be resumed. Note that it does
	 * not make any sense to call Resume on a running thread.  
	 *
	 * @return true iff the thread/process could be resumed (which
	 * implies that the thread was suspended beforehand)
	 */
	virtual bool Resume() = 0;

	/**
	 * Join is called from OUTSIDE the scope of this fork-instance. A
	 * call to join will ONLY return when the forked process is
	 * finished. You can use this to suspend the caller of join until
	 * the forked work is REALLY done. Note that joining on a forked
	 * process that will never quit will block the caller of join
	 * forever and ever.  
	 * 
	 * @return true always, as otherwise the caller gets blocked.
	 */
	virtual bool Join() = 0;

	/**
	 * Abort "CANCELS" a forked process, see this as a kind of
	 * "TERMINATE"-command from the outside of VistaFork. A forked
	 * instance that is running is aborted right aways without any
	 * chance to synchronize or to give back resources. BE SURE THAT
	 * YOU KNOW WHAT YOU ARE DOING WHEN CALLING Abort!
	 *
	 * @return true iff the forked process could be terminated, false
	 * else (the forked thing is still running)
	 */
	virtual bool Abort() = 0;

	/**
	 * Allows modification of this forked thing's priority. See the
	 * definition of VistaPriority for more explanation.
	 *
	 * @param prio the priority for the forked thing to get
	 * @return true iff the priority that was given could be set for
	 * this forked thing
	 */
	virtual bool SetPriority(const VistaPriority &prio) = 0;


	/**
	 * Retrieves the priority of a thread to the caller.
	 * @see VistaPriority()
	 *
	 * @param the out parameter to hold the priority value in
	 */
	virtual void GetPriority( VistaPriority & ) const = 0;

	/**
	 * Method that is to be performed BEFORE departed fork starts
	 * execution.
	 */
	virtual void PreRun() = 0;

	/**
	 * Method that is to be performed AFTER forked work is done.
	 */
	virtual void PostRun() = 0;

	virtual bool GetIsFinished() const = 0;

private:
	/**
	 * Copy-constructor, we deny copying.
	 */
	VistaFork( const VistaFork & );

	/**
	 * Assigment is denied.
	 */
	VistaFork & operator= ( const VistaFork & );
};

#endif // _VISTAFORK_H
