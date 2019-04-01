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


#include <cerrno>

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#if !defined(VISTA_THREADING_WIN32)

#include "VistaPosixThreadEventImp.h"

#if defined (LINUX) || defined (DARWIN)
	#include <sys/ioctl.h>
	#include <sys/time.h>
#elif defined(SUNOS) || defined(IRIX)
	#include <sys/types.h>
	#include <sys/filio.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <algorithm>

#define PIPE_R 0
#define PIPE_W 1

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaPosixThreadEventImp::VistaPosixThreadEventImp()
{
	int res = pipe(m_fd);
	if(!res)
	{
		// HOUSTON, WE HAVE GOT A PROBLEM
	}
}


VistaPosixThreadEventImp::~VistaPosixThreadEventImp()
{
	close(m_fd[PIPE_R]);
	close(m_fd[PIPE_W]);
}


/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaPosixThreadEventImp::SignalEvent()
{
	VistaPosixThreadEventImp* pThis = this;
   
	int n = 0;
	if(0 <= ioctl(m_fd[PIPE_W],FIONREAD, reinterpret_cast<char*>(&n)) && n==0)
		TEMP_FAILURE_RETRY( write(m_fd[PIPE_W], &pThis, sizeof(this)) );
}

bool VistaPosixThreadEventImp::WaitForEvent(int iTimeoutMSecs)
{
	VistaPosixThreadEventImp *pThis;

	struct timeval tv;
	int secs = iTimeoutMSecs / 1000;
	tv.tv_sec  = secs;
	tv.tv_usec = (iTimeoutMSecs - secs*1000) * 1000;

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(m_fd[PIPE_R], &readfds);

	// @todo add descriptors to exceptfds set and check for errors
	TEMP_FAILURE_RETRY( select(m_fd[PIPE_R]+1, &readfds, NULL, NULL, &tv) );
	
	if(FD_ISSET(m_fd[PIPE_R], &readfds))
	{
		TEMP_FAILURE_RETRY( read(m_fd[PIPE_R], (void*)&pThis, sizeof(pThis)) );
		assert(pThis==this);
		return true;
	}

	return false;
}

bool VistaPosixThreadEventImp::WaitForEvent(bool bBlock)
{
	VistaPosixThreadEventImp *pThis;

	struct timeval tv;
	struct timeval *ptv=NULL;
	if(!bBlock)
	{
		tv.tv_sec  = 0;
		tv.tv_usec = 0;
		ptv = &tv;
	}	

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(m_fd[PIPE_R], &readfds);

	// @todo add descriptors to exceptfds set and check for errors
	TEMP_FAILURE_RETRY( select(m_fd[PIPE_R]+1, &readfds, NULL, NULL, ptv) );
	
	if(FD_ISSET(m_fd[PIPE_R], &readfds))
	{
		TEMP_FAILURE_RETRY( read(m_fd[PIPE_R], (void*)&pThis, sizeof(pThis)) );
		assert(pThis==this);
		return true;
	}

	return false;
}

HANDLE VistaPosixThreadEventImp::GetEventSignalHandle() const
{
	return (HANDLE)m_fd[PIPE_W];
}

HANDLE VistaPosixThreadEventImp::GetEventWaitHandle() const
{
	return (HANDLE)m_fd[PIPE_R];
}

bool VistaPosixThreadEventImp::ResetThisEvent( ResetBehavior reset_behavior )
{
	// determine if there is a signal waiting
	int result = 0;

	// determine a snapshot of the _current_ state
	// even at the risk of loosing a signal
	if ( ! ioctl ( m_fd[PIPE_R], FIONREAD, & result ) )
	{
		// success
		VistaPosixThreadEventImp *pThis;
		
		// loop might be slower, but we avoid dynamic allocation
		// of an array of pointers to read all results at once
		// anyway... we expect to be only a few signals in the queue
		int num_to_reset = reset_behavior == RESET_ALL_EVENTS ? result/(int)sizeof(void*) : 1;
		for( int i=0; i < num_to_reset; ++i)
		{
			TEMP_FAILURE_RETRY( read(m_fd[PIPE_R], (void*)&pThis, sizeof(pThis)) );
		}
		return true;
	}

	return false;
}


#endif

// ============================================================================
