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


#ifndef _VISTATHREADEVENT_H
#define _VISTATHREADEVENT_H

#include "VistaIpcThreadModel.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaUtilityMacros.h>



/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class IVistaThreadEventImp;


class VISTAINTERPROCCOMMAPI VistaThreadEvent
{
public:
	virtual ~VistaThreadEvent();
	
	enum WaitBehavior
	{
		NON_WAITABLE_EVENT=0,
		WAITABLE_EVENT=1
	};
	
	//! @deprecated left in for compatibility
	//! @FIXME remove this c'tor
	VISTA_DEPRECATED VistaThreadEvent( bool waitable_event );

	VistaThreadEvent(WaitBehavior wait_behavior = WAITABLE_EVENT );

	/**
	 * Set this event to the signaled state. This wakes one of the
	 * threads currently blocking on the signal using the WaitForEvent
	 * methods. Iff a waiting thread is released by this, the event
	 * will automatically be reset to the non-signaled state, see
	 * ResetThisEvent().
	 */
	 void SignalEvent();

	/**
	 * Poll/Wait for signaled event. If bBlock is true, do a blocking
	 * wait, otherwise just poll and return immediately.
	 *
	 * @return true if event was signaled, false otherwise.
	 */
	bool WaitForEvent(bool bBlock);

	/**
	 * Block on this event until the timeout specified in milliseconds
	 * has elapsed.
	 *
	 * @return true if signaled, false upon timeout.
	 */
	bool WaitForEvent(int iTimeoutMSecs);


	HANDLE GetEventSignalHandle() const;
	HANDLE GetEventWaitHandle() const;

	enum ResetBehavior
	{
		RESET_ALL_EVENTS=0,
		RESET_JUST_ONE_EVENT
	};
	
	/**
	 * Manually reset the event to the non-signaled state. This is for
	 * the case that the event has been set to signaled, but no thread
	 * was resumed by it, i.e. no one was listening.
	 */

	bool ResetThisEvent( ResetBehavior reset_behavior = RESET_ALL_EVENTS );

private:
	IVistaThreadEventImp *m_pImpl;
};


/*============================================================================*/

#endif // _VISTATHREADEVENT_H
