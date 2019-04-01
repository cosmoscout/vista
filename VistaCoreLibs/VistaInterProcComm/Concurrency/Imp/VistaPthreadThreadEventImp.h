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


#if defined(VISTA_THREADING_POSIX)

#ifndef _VISTAPTHREADTHREADEVENTIMPL_H
#define _VISTAPTHREADTHREADEVENTIMPL_H


#include "VistaThreadEventImp.h"

#include <pthread.h>


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaPthreadThreadEventImp : public IVistaThreadEventImp
{
public:
	VistaPthreadThreadEventImp( );
	virtual ~VistaPthreadThreadEventImp();

	void SignalEvent();

	bool WaitForEvent(bool bBlock);
	bool WaitForEvent(int iTimeoutMSecs);

	virtual HANDLE GetEventSignalHandle() const;
	virtual HANDLE GetEventWaitHandle() const;

	bool ResetThisEvent( ResetBehavior reset_behavior );

private:
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	int state;
	bool autoreset;
};


/*============================================================================*/

#endif // _VISTAPTHREADTHREADEVENTIMPL_H

#endif // VISTA_THREADING_POSIX

