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


#ifndef IDLVISTAACTIVECOMPONENT_H
#define IDLVISTAACTIVECOMPONENT_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaPriority;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is the pure interface for active components and does nothing more than
 * defining the interface for active pipe-components. It does not contain
 * data members, only pure virtual methods.
 * The idea is to put up a single thread for each component.
 */
class VISTAINTERPROCCOMMAPI IDLVistaActiveComponent
{
private:
protected:
	IDLVistaActiveComponent();
public:
	/**
	 * Pure virtual destructor. Does nothing.
	 */
	virtual ~IDLVistaActiveComponent();

	/**
	 * Any active component should start its task after the call to this
	 * method.
	 * @return true iff the component was started, false if the component was not started and something failed.
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool StartComponent() = 0;

	/**
	 * Components do usually iterate over a single task. A call to this method will
	 * gently ask to stop its work. After the next iteration the component will
	 * cease to work. You can specify to return after the component has surely ceased.
	 * @param bJoin if set to true, this call will return as soon as the component has stopped
	 * @return true iff the stop message could be given to the component.
	 */
	virtual bool StopComponentGently(bool bJoin) = 0;

	/**
	 * Use this method to pause the work of this component. A pause-message will be
	 * send to the running thread.
	 * @param bJoin set to true in order to return after the running thread has paused
	 * @return true iff the message could be sent to the running thread
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool PauseComponent(bool bJoin) = 0;

	/**
	 * Use this method to unpause a paused thread. Note that a call to this method
	 * for an unpaused thread may or may not reveal nondeterministic behaviour.
	 * @param bJoin indicates whether the calling thread shall be blocked until the active thread has resumed to function
	 * @return true iff the UnPause-message could be sent to the paused thread.
	 */
	virtual bool UnPauseComponent(bool bJoin) = 0;


	/**
	 * Use this method to stop the running component. Stopping means to halt it immediately.
	 * A stop message will be sent to the running thread. Note that a thread may catch the stop
	 * signal and exit cleanly without doing another iteration.
	 * @param bJoin set to true in order to return after the thread has stopped
	 * @return true iff the message could be sent to the running thread
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool StopComponent(bool bJoin) = 0;

	/**
	 * Use this message in case of emergency. A halted component will exit without the chance
	 * to cleanup.
	 * @return true iff the message could be sent to the running thread
	 * @todo we should define a way to propagate error-states and messages from component to this interface
	 */
	virtual bool HaltComponent() = 0;

	/**
	 * Use this method in order to find out whether this component is running or not.
	 * @todo there is no way to find out whether a thread is running or not in VistaThread
	 * @return true iff the component is running
	 */
	virtual bool IsComponentRunning() const = 0;


	virtual int  SetComponentPriority(const VistaPriority &pPrio) = 0;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTAACTIVECOMPONENT_H


