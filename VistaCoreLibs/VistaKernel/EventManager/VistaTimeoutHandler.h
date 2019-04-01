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


#ifndef _VISTATIMEOUTHANDLER_H
#define _VISTATIMEOUTHANDLER_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>

#include <vector>
#include <list>

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEventManager;
class VistaTickTimer;
class VistaClusterMode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Usage: subclass, overload VistaTimeoutHandler::HandleTimeout(), call
 * VistaTimeoutHandler::AddTimeout() as much as you like and remember what
 * return value of AddTimeout() leads to which path of execution in HandleTimeout.
 * As simple as that.
 * @see VistaTickTimer()
 */
class VISTAKERNELAPI VistaTimeoutHandler : public VistaEventHandler
{
public:
	/**
	 * HD_TIMERs are simply a pointer to a tick timer, but no one
	 * should rely on this (could change to an index in an array
	 * of tick-timers). So consider this a handle only.
	 */
	typedef VistaTickTimer* HD_TIMER;

public:
	VistaTimeoutHandler(VistaEventManager *pEvMgr,
						 VistaClusterMode *pClusterMode);
	virtual ~VistaTimeoutHandler();

	// ########################################################################
	// TIMER HANDLE ROUTINES
	// ########################################################################
	/**
	 * Resets a timer for a given handle. Running timers will keep
	 * running, but be reset to their starting point.
	 * @param tim a valid handle to a timer object
	 * @precondition IsValidHandle(tim) == true
	 */
	void ResetTimeout(HD_TIMER tim);

	/**
	 * Change (set) the timeout value for an already existing timer
	 * handle.
	 * @param tim a valid handle to a timer object
	 * @precondition IsValidHandle(tim) == true
	 * @postcondition tim has new timeout-value
	 */
	void SetTimeout(HD_TIMER tim, double dNewTimeout);

	/**
	 * Create and add a tick timer, set its timeout value to dTimeout,
	 * make it pulsing, create a handle and return this to the user
	 * for future identification.
	 * @return a handle for a timeout that will 'ping' after dTimeout
	 * @param dTimeout the timeout value (defaults to msecs, change this with the returned handle
	 */
	virtual HD_TIMER AddTimeout(double dTimeout);

	/**
	 * Remove means: do not use tim after this call, it might(!) be pointing
	 * to a deleted object.
	 * @return true iff the handle was registered and was removed from the active list
	 */
	virtual bool RemoveTimeout(HD_TIMER tim);


	// ########################################################################
	// MAINTENANCE ROUTINES
	// ########################################################################

	/**
	 * Conversion method: switch from handle to VistaTickTimer object<br>
	 * Note: this is a simple cast right now, but might change in the future,
	 * so you better do not rely on HD_TIMER being a pointer.
	 * @return a pointer to a VistaTickTimer (null for invalid handles)
	 * @param tim a valid handle to a timeout
	 */
	VistaTickTimer *GetTimerForHandle(HD_TIMER tim) const;

	/**
	 * Check, whether a handle is registered with this handler
	 * @param tim a valid timout handle
	 * @return true iff tim is a registered timout for this handler
	 */
	bool IsValidHandle(HD_TIMER tim) const;

	/**
	 * Finds an *arbitrary* timer that suits a given timeout value and returns
	 * a handle to it. Check its validity with IsValidHandle()
	 * @return a handle to a timer for this handler
	 * @param dTimeout a timeout value to be matched
	 * @see IsValidHandle()
	 */
	HD_TIMER FindTimer(double dTimeout) const;

	// ########################################################################
	// PROCESSING ROUTINES
	// ########################################################################
	/**
	 * This method is registered for the handling of tick-timer events and will be
	 * called by the VistaEventManager. Non-pulsing timers will not be reenqueed
	 * to be processed
	 * @param pEvent a pointer to the event to be processed
	 * @see VistaEventManager()
	 * @see VistaEventHandler()
	 */
	virtual void HandleEvent(VistaEvent *pEvent);


	/**
	 * SUBCLASSES MUST OVERLOAD THIS. You can chose your path of execution by
	 * checking the handle that is passed to this method (tim).
	 * @param tim the timeout to be handled right now
	 */
	virtual void HandleTimeout(HD_TIMER tim) = 0;


private:
	VistaTickTimer *ConvertFromHandle(HD_TIMER tim) const;

protected:
	VistaEventManager *m_pEventManager; /**< handy pointer to the EvMgr */
	VistaClusterMode *m_pClusterAux;
	std::vector<VistaTickTimer *> m_veWatches; /** the current active handle vector */

	/**
	  * we do reuse old handles as creation might take some time, and hey...
	  * it's time, we are talking about, right?
	  */
	std::list<VistaTickTimer*> m_liGarbage;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif // _VISTATIMEOUTHANDLER_H
