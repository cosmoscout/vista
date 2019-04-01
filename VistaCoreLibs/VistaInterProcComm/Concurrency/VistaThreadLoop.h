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


#ifndef _VISTATHREADLOOP_H
#define _VISTATHREADLOOP_H

#include "VistaThread.h"


class VistaMutex;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 * This class provides a looping-idiom for VistaThread.
 * In case a task has to be done over and over again, and has to be stopped
 * from the outside, this class is useful.
 * Overload the VistaIterationThread::LoopBody() method to peform the work
 * for a single work-through.
 * The iteration-loop is braced by a call to PreIterationLoop() and PostIterationLoo(),
 * and thus a typical usage of an instance would be:<br>
 * PreIterationLoop()<br>
 * [Iteration]+<br>
 * PostIterationLoop()<br>
 * -> Thread exits cleanly.<br>
 * The iteration can be paused from an outside thread using PauseThread(). Use UnpauseThread()
 * To wake the iteration again.
 */

class VISTAINTERPROCCOMMAPI VistaThreadLoop : public VistaThread
{
private:
	/**
	 * Internal flags for the sake of speed ;)
	 * In case this flag is true, there will be no more iteration.
	 * This flag will be checked AFTER each successful iteration.
	 */
	bool m_bStopGently;

	/**
	 * indicates whether this thread is active in the sense that it is
	 * Paused from the outside.
	 */
	bool m_bIsPaused;

protected:
	/**
	* These are used for the synchronisation to the pause of the iteration.
	*/

	/**
     * is hold by this thread if it is running, note that the thread is "running",
	 * even if it is "pausing" ;), meaning: running will be release after ThreadBody() was left
	 */
	VistaMutex *m_pRunning;

	/**
	 * the paused thread tries to get access to this mutex, indicating that he can continue
	 */
	VistaMutex *m_pPausing;

	/**
	 * an outside thread will give a signal to the running thread, that it does want to pause it
	 * and waits for the m_pPauseGain-mutex to get a hold on
	 */
	VistaMutex *m_pPauseRequest;

	/**
	 * the thread releases this as an outside thread indicated that it wants to this thread to pause
	 */
	VistaMutex *m_pPauseGain;

	/**
	 * Creates the resources for the mutexes.
	 */
	VistaThreadLoop( IVistaThreadImp *pImp = NULL );

	/**
	 * This is a function to be overloaded by a specific thread.
	 * It will be called ONCE before the iteration loop is entered. A thread can use this method
	 * to setup some variabled before actually entering the loop, as this is not left until explicitly
	 * asked to do so.
	 */
	virtual void PreLoop();

	/**
	 * This is a function to be overloaded by a specific thread.
	 * It will be called ONCE after the iteration loop is exited. A thread can use this in order to
	 * cleanup of revert certain setups done before.
	 */
	virtual void PostLoop();


	/**
	 * Abstract loop-body, specifiy a single walk through the algorithm you will loop over.
	 * The return value indicates to ThreadBody() whether this thread should yield or not.<br>
	 * ###############################################
	 * WARNINGS
	 * <ul>
	 *  <li>yielding may have performance impact on different OSs.</li>
	 * </ul>
	 * ################################################<br>
	 * @return true iff ThreadBody() shall call YieldThread() before the next loop, false else
	 */
	virtual bool LoopBody() = 0;
 public:
	 virtual ~VistaThreadLoop();

	/**
	 * A call to this method will bold the iteration AFTER a call to LoopBody().
	 * This thread will be put into a state where it will wait for a mutex to be
	 * free again and will not be resumed until a call to UnpausThread() will occur.
	 * A possibility to completely stop a thread is to call Stop(), but this will
	 * simply send a signal to the system to kill the thread completely. Note that
	 * pausing the thread uses mutexes, to avoid platform-specifig signal-handling
	 * or object-waiting constructions. If you specify a value of false to this method,
	 * it will return immediately after a VistaMutex::TryLock() call. It can really
	 * take some time to stop a thread then ;)<br>
	 * ###############################################
	 * WARNINGS
	 * <ul>
	 *  <li>a threadloop that is not running CAN NOT BE PAUSED. A call to Pause thread will deadlock.</li>
	 *  <li>this method MUST ONLY be called FROM THE OUTSIDE OF LOOPBODY/THREADBODY. Any call inside those methods will cause a deadlock</li>
	 *  <li>giving bBlock=false will cause the caller to return IMMEDIATELY, this does not mean, that the thread has actually paused</li>
	 * </ul>
	 * ################################################<br>
	 * @see Iteration()
	 * @param bBlock true if the external thread will wait for this thread to pause (after iteration)
	 * @todo see if we can remove the three mutexes and make this simpler
	 */
	virtual bool PauseThread(bool bBlock=true);


	/**
	 * A paused thread will be unpaused by this method.
	 * Note that a recursice (nested) call might not be supported on your specific plattform,
	 * so it is better to check that this does not get called twice and only from the
	 * thread that paused this one.<br>
	 * ###############################################
	 * WARNINGS
	 * <ul>
	 *  <li>a thread loop that is NOT RUNNING (i.e. not startet/paused) can NOT be unpaused.</li>
	 * </ul>
	 * ################################################<br>
	 */
	virtual bool UnpauseThread();


	/**
	 * This call can be used to Stop this thread from extern without interrupting its
	 * work. Please note: Stopping a paused thread does not make  much sense and will FAIL.
	 * @param bJoin iff set to true, the external thread will only return after this thread finished (e.g. left ThreadBody())
	 * @return always true (this might change)
	 */
	virtual bool StopGently(bool bBlockTheOutsideCallerUntilTheThreadIsReallyFinished);


	void IndicateLoopEnd();

	/**
	 * Overloaded from superclass.
	 * You should not overload it again, as it does constitute this class's state-machine.
	 */
	virtual void ThreadBody();


	/**
	 * returns whether this thread is pausing.
	 * Note that an unstarted thread IS NOT PAUSING.
	 * @return true iff (Thread not started || Thread paused), false else
	 */
	virtual bool IsPausing() const { return m_bIsPaused; };

private:


};


/*============================================================================*/

#endif // _VISTAITERATIONTHREAD_H


