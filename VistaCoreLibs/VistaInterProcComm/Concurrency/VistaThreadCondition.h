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


#ifndef _VISTATHREADCONDITION_H
#define _VISTATHREADCONDITION_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaThreadConditionImp;
class VistaMutex;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A ThreadCondition marks a specific state your thread is in. You want to use
 * it in a situation where you would normally poll for the state of thread-specific
 * flags (e.g.: bool HasThreadReachedStateX() const { return m_bStateXReached; }).
 * It will only work properly, in conjunction with a mutex that will guarantee that
 * the WaitForCondition is called while no other thread is trying to signal.
 * The normal usage of a condition would thus be:<br>
 * VistaMutex *pMutex = new VistaMutex;<br>
 * VistaThreadCondition *pCond = new VistaThreadCondition;<br>
 * <table>
 * <tr><td>{thread 1}       </td><td>{thread 2}</td></tr>
 * <tr><td>(*pMutex).Lock();</td><td></td></tr>
 * <tr><td>[...]            </td><td></td></tr>
 * <tr><td>(*pCond).WaitForCondition(*pMutex);<br>{thread 1 will block}</td><td></td></tr>
 * <tr><td></td><td>[...]</td></tr>
 * <tr><td></td><td>(*pCond).SignalCondition()</td></tr>
 * <tr><td>{thread 1 will continue now}</td><td></td><tr>
 * </table>
 */
class VISTAINTERPROCCOMMAPI VistaThreadCondition
{
public:
	VistaThreadCondition();
	virtual ~VistaThreadCondition();

	/**
	 * The caller will indicate that a specific condition (e.g. state)
	 * is met. A waiting thread for this condition will be woken up and
	 * continue their job. Note that the order (meaning: which thread
	 * is going to continue) is not guaranteed! Do NOT assume a kind of
	 * FIFO strategy here.
	 * @return 0 iff no error, a system specific error code else
	 */
	int SignalCondition();

	/**
	 * The caller will signal to ALL waiting threads that a specific
	 * condition is met. All waiting threads will be allowed to continue
	 * their work.
	 * @return 0 iff no error happened, else a system specific error code
	 */
	int BroadcastCondition();

	/**
	 * The caller will be blocked until some other thread calls
	 * Signal- or BroadcastCondition. Note: the mutex passed to
	 * this method MUST BE LOCKED <b>before</b> calling this
	 * method. It will be released while waiting for the signal,
	 * but it will be locked again before returning from this call!!
	 * @param rMutex the mutex to release while waiting for this condition.
	 * @return 0 iff the wait succeeded, else it will return a system-specific error code != 0
	 * @see SignalCondition()
	 * @see BroadcastCondition()
	 */
	int WaitForCondition(VistaMutex &rMutex);

	/**
	 * AWARE: currently NOT implemented. DO NOT USE.
	 */
	int WaitForConditionWithTimeout(VistaMutex &, int iMsecs);
protected:
private:
	IVistaThreadConditionImp *m_pConditionImp;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATHREADCONDITION_H

