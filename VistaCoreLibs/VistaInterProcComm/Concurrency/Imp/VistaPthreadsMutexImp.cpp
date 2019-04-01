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


#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 199506L
#endif

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>

#include <VistaBase/VistaStreamUtils.h>


#if defined(VISTA_THREADING_POSIX)
#include "VistaPthreadsMutexImp.h"


#include <iostream>
using namespace std;


#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <fcntl.h>      /* O_flags */
#include <sys/mman.h>   /* shared memory and mmap() */
#include <unistd.h>		/* ftruncate */

#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaPthreadsMutexImp::VistaPthreadsMutexImp (const string &sName, const IVistaMutexImp::eScope nScope)
{
	pPosixMutex = NULL;
	m_sharedMem_fd = -1;
	switch(nScope)
	{
	case IVistaMutexImp::eIntraProcess:
		{
			pPosixMutex = new pthread_mutex_t;
			#if (defined PthreadDraftVersion && PthreadDraftVersion == 4)
				pthread_mutex_init ( pPosixMutex, PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP );
			#else
				pthread_mutexattr_t oMutexAttribute;
				pthread_mutexattr_init(&oMutexAttribute);
				pthread_mutexattr_settype(&oMutexAttribute, PTHREAD_MUTEX_RECURSIVE);
				pthread_mutex_init(pPosixMutex, &oMutexAttribute);
			#endif
				break;
		}
	case IVistaMutexImp::eInterProcess:
		{
			vstr::errp() << "INTERPROCESS-MUTEX CURRENTLY UNSUPPORTED FOR THIS PLATFORM" << std::endl;
			pPosixMutex = new pthread_mutex_t;
			#if (defined PthreadDraftVersion && PthreadDraftVersion == 4)
				pthread_mutex_init ( pPosixMutex, PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP );
			#else
				pthread_mutexattr_t oMutexAttribute;
				pthread_mutexattr_init(&oMutexAttribute);
				pthread_mutexattr_settype(&oMutexAttribute, PTHREAD_MUTEX_RECURSIVE);
				pthread_mutex_init(pPosixMutex, &oMutexAttribute);
			#endif
		

			////to have an interprocess mutex, we have to create one in memory shared by the participating processes
			//string sFileName = "/";
			//sFileName.append(sName);
			//m_sharedMem_fd = shm_open(sFileName.c_str(), (O_RDWR), (mode_t)0600);
			//if(m_sharedMem_fd != -1)
			//{
			//	VISTA_THROW("COULD NOT SHM_OPEN INTERPROCESS MUTEX["+sName+"]", 0x0000001);
			//}

			//	//so the shared memory object doesn't exist yet.
			//	//this means that we create it and initialize the mutex
			//	m_sharedMem_fd = shm_open(sFileName.c_str(), (O_CREAT | O_RDWR), (mode_t)0600);
			//	assert(m_sharedMem_fd != -1);

			//	//we set the size of our new shm object to the mutexe's size
			//	ftruncate(m_sharedMem_fd, sizeof(pthread_mutex_t));

			//	//we map the new shared memory object into the address space of our process
			//	pPosixMutex 
			//		= (pthread_mutex_t*)mmap(NULL, sizeof(pthread_mutex_t), (PROT_READ | PROT_WRITE), MAP_SHARED, m_sharedMem_fd, 0);
			//	assert(pPosixMutex != (void *)-1);

			//	//finally we initialize our mutex
			//	pthread_mutexattr_setpshared( & posixMutexAttr, PTHREAD_PROCESS_SHARED);
			//	pthread_mutex_init ( pPosixMutex, &posixMutexAttr);
			//}
			//else
			//{
			//	//the shared memory object already existed, so another process has already created the mutex
			//	//we just map the existing shared memory object into address space of our process
			//	pPosixMutex 
			//		= (pthread_mutex_t*)mmap(NULL, sizeof(pthread_mutex_t), (PROT_READ | PROT_WRITE), MAP_SHARED, m_sharedMem_fd, 0);
			//}
			break;
		}
	}
}

VistaPthreadsMutexImp::~VistaPthreadsMutexImp ()
{
	pthread_mutex_destroy ( pPosixMutex );
	if(m_sharedMem_fd != -1)
	{
		munmap((char *)pPosixMutex, sizeof(pthread_mutex_t));
		close(m_sharedMem_fd);
	}
	else
	{
		delete pPosixMutex;
	}
}


void VistaPthreadsMutexImp::Lock    ()
{
	TEMP_FAILURE_RETRY( pthread_mutex_lock( pPosixMutex ) );
}

bool VistaPthreadsMutexImp::TryLock ()
{
	int iRet = pthread_mutex_trylock ( pPosixMutex );
	return (  iRet  == 0 );
}

void VistaPthreadsMutexImp::Unlock  ()
{
	pthread_mutex_unlock ( pPosixMutex );
}

#endif // VISTA_THREADING_SPROC

