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


#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h> 

#if defined(VISTA_THREADING_POSIX)
#include "VistaPosixSemaphoreImp.h"
#include <VistaBase/VistaExceptionBase.h>

#include <errno.h>
#include <unistd.h>

#if defined(DARWIN)
#include <sys/semaphore.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPosixSemaphoreImp::VistaPosixSemaphoreImp(int nCount)
{
#if defined(DARWIN)
	// since mac os x only supports pthreads named semaphores, we have to use
	// those. since we don't want to synthesize unique names for each semaphore
	// after the creation we call sem_unlink immediately.
	// @todo: check for thread safety! (another thread might call sem_open before
	// sem_unlink was called, thus the call to sem_open will fail...
	if((m_pSemaphore = sem_open("/vista_fake_semaphore", O_CREAT, 0, nCount)) == SEM_FAILED)
    {
		perror("error: ");
		VISTA_THROW("[VistaPosixSemaphore]: sem_open() failed", 0L);
    }
	else
    {
		sem_unlink("/vista_fake_semaphore");
    }
#else
	m_pSemaphore = new sem_t;
	if(sem_init(m_pSemaphore, 0, nCount) == -1)
		VISTA_THROW("[VistaPosixSemaphore]: sem_init() failed", 0L);
#endif
}

VistaPosixSemaphoreImp::~VistaPosixSemaphoreImp()
{
#if defined(DARWIN)
	sem_close(m_pSemaphore);
#else
	sem_destroy(m_pSemaphore);
	delete m_pSemaphore;
#endif
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


void VistaPosixSemaphoreImp::Wait    ()
{
	int nRet = TEMP_FAILURE_RETRY( sem_wait(m_pSemaphore) );
	if( nRet != 0 )
	{
		switch(nRet)
		{
			case EINTR:
				VISTA_THROW("[VistaPosixSemaphore]: Wait() canceled by signal", 0L);
			case EINVAL:
				VISTA_THROW("[VistaPosixSemaphore]: NOT A VALID SEMAPHORE", 0L);
			default:
				break;
		}
	}
}

bool VistaPosixSemaphoreImp::TryWait ()
{
	int nRet = 0;
	if( ( nRet = sem_trywait( m_pSemaphore) ) )
	{
		switch( nRet )
		{
		case EINTR:
			VISTA_THROW("[VistaPosixSemaphore]: Wait() canceled by signal", 0L);
		case EINVAL:
			VISTA_THROW("[VistaPosixSemaphore]: NOT A VALID SEMAPHORE", 0L);
		case EDEADLK:
			VISTA_THROW("[VistaPosixSemaphore]: Deadlock detected.", 0L);
		case EAGAIN:
			VISTA_THROW("[VistaPosixSemaphore]: semaphore already locked.", 0L);
		default:
			break;
		}
	}

	return ( nRet == 0 );
}


void VistaPosixSemaphoreImp::Post    ()
{
	sem_post(m_pSemaphore);
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // #if defined(VISTA_THREADING_POSIX)
