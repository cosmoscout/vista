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


#include "VistaSharedMem.h"

// Standard C includes
#include <cstdio>
#include <cerrno>

// Standard C++ includes
#include <string>

#if !defined(VISTA_USE_POSIX)
#   ifndef WIN32
#       if defined(IRIX)
			// we prefer AT&T System-V Rel.4 (SVR4)
#           define VISTA_USE_SYSTEMV
#       else
			// we prefer POSIX
#           define VISTA_USE_POSIX
#           ifdef SUNOS
#               define __EXTENSIONS__
#               define _POSIX_C_SOURCE   199506L
#           endif
#       endif
#   endif
#endif

// system headers

#ifdef WIN32
  #include <Windows.h>
  #include <stdlib.h>
#else
  #include <sys/types.h>       // socket(),gethostbyname(),htons(),connect()
  #if (!defined( LINUX ) && !defined (HPUX))
    #include <sys/conf.h>        // ioctl ( FIONREAD )
  #endif
  #include <sys/socket.h>      // socket(),gethostbyname(),connect()
  #include <netinet/in.h>      // gethostbyname(),htons() libc-Version!
  #include <arpa/inet.h>
  #include <unistd.h>          // ioctl(), FIONREAD
//  #include <stropts.h>         // ioctl(), FIONREAD
  #include <sys/ioctl.h>       // ioctl(), FIONREAD
  #if (!defined( LINUX ) && !defined (HPUX))
    #include <sys/filio.h>
  #endif
  #include <netdb.h>           // getprotoent() libc-Version!,gethostbyname()
  #ifdef IRIX
    #include <sys/ipc.h>
    #include <sys/sem.h>
    #include <sys/prctl.h> // threads
  #endif
  #ifdef VISTA_USE_POSIX
    #include <semaphore.h>
    #include <pthread.h>
  #endif
  #include <sched.h>
  #include <sys/fcntl.h>
  #include <sys/mman.h>
#endif


using namespace std;
/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

VistaSharedMemory::VistaSharedMemory()
{
	m_pSharedMem     = NULL;
	m_nSharedMemSize = 0;
}

VistaSharedMemory::~VistaSharedMemory()
{
	if (m_pSharedMem)
	{
		// try to destroy (last attempt)
		this->DestroySharedMem (m_strMapFileName);
	}
	m_pSharedMem     = NULL;
	m_nSharedMemSize = 0;
}

// ============================================================================
// ============================================================================

bool VistaSharedMemory::CreateSharedMem (string & sharedMem, long size)
{
	void * pSharedMem = NULL;

	if (errno != 0)
	{
		// reset error status
		errno = 0;
	}

	if (size == 0)
	{
		// use already set mem size
		size = m_nSharedMemSize;
	}
	else
	{
		// store given size
		m_nSharedMemSize = size;
	}

#ifdef WIN32
	// handle to file mapping
	HANDLE hMapObj = NULL;
	// create a named file mapping object
	hMapObj = CreateFileMapping (
		(HANDLE) 0xFFFFFFFF,    // use paging file
		NULL,                   // no security attributes
		PAGE_READWRITE,         // read/write acces
		0,                      // size: high 32 bits
		size,                   // size: low 32 bits
		sharedMem.c_str()        // name of the map object
		);
	if (hMapObj != 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// hooops, delete handle again
			CloseHandle (hMapObj);
			return false;
		}

		pSharedMem = GetSharedMemPointer (sharedMem);

		if (pSharedMem == NULL)
		{
			CloseHandle (hMapObj);
			perror ("SHM_CREATE_ERROR (GetSharedMemPointer) ");
			return false;
		}
		// init shared mem
		//memset (pSharedMem, '\0', size);
	}
	else
	{
		//serious error
		perror ("SHM_CREATE_ERROR (ChreateFileMapping) ");
		return false;
	}
#else
#ifndef LINUX
//#ifndef SUNOS
	// open a mem file
	// #include <sys/mman.h>
	//extern int shm_open(const char *, int, mode_t);
	// #ifdef	__STDC__
	// #if (!defined(_XPG4_2) || (_POSIX_C_SOURCE > 2)) || defined(__EXTENSIONS__)
	//int memFileDesc = shm_open (sharedMem.c_str(), O_RDWR|O_CREAT, 0666);
	int memFileDesc = shm_open ((const char*)sharedMem.c_str(), (int)(O_RDWR|O_CREAT), /*0666*/0777);
	if (memFileDesc <= 0)
	{
		perror ("SHM_CREATE_ERROR (shm_open) ");
		return false;
	}

	/* Set the memory object's size */
	if (ftruncate (memFileDesc, /*sizeof(*addr)*/ m_nSharedMemSize) == -1)
	{
		fprintf (stderr, "ftruncate : %s\n", strerror(errno));
		return false;
	}
	
	int mProt  = PROT_WRITE | PROT_READ;
	int mFlags = MAP_SHARED;
#ifdef IRIX
	// these flags are not defined in POSIX!!
	//mFlags |= (MAP_AUTOGROW | MAP_AUTORESRV);
	mFlags |= MAP_AUTOGROW;
#endif

	// map shared memory to process memory
	// void * mmap (void *addr, size_t len, int prot, int flags, int fildes, off_t off);
	pSharedMem = mmap (NULL, (size_t)size, mProt, mFlags, memFileDesc, (off_t)0);
	//ftruncate (memFileDesc, (off_t)size);
	if (pSharedMem == MAP_FAILED)
	{
		perror ("SHM_CREATE_ERROR (mmap) ");
		return false;
	}
	//mlock (pSharedMem, size);
//#endif
#endif
#endif
	if (errno != 0)
	{
		//perror ("SHM_CREATE_ERROR (?) ");
	}
	// init shared mem
	//memset (pSharedMem, '\0', size);
	// set some member
	m_pSharedMem     = pSharedMem;
	m_strMapFileName = sharedMem;

	return true;
}

// ============================================================================
// ============================================================================
bool VistaSharedMemory::DestroySharedMem (string & sharedMem)
{
	if (errno != 0)
	{
		// reset error status
		errno = 0;
	}

	void * pSharedMem = GetSharedMemPointer (sharedMem);
	if (pSharedMem == NULL)
		return false;
#ifdef WIN32
	//UnmapViewOfFile (pSharedMem);
	HANDLE hMapObj = NULL;
	hMapObj = OpenFileMapping (FILE_MAP_WRITE, FALSE, sharedMem.c_str() );
	if (hMapObj)
		//??!!! or have I use the first handle of creation method
		CloseHandle (hMapObj);
#else
#ifndef LINUX
//#ifndef SUNOS
	//unmap();
	if (shm_unlink (sharedMem.c_str()) == -1)
	{
		perror ("SHM_DESTROY_ERROR ");
	}
//#endif //not SUNOS
#endif //not LINUX
#endif

	// check error state
	if (errno != 0)
	{
		//perror ("SHM_DESTROY_ERROR ");
	}

	// reset member variables
	m_pSharedMem = NULL;
	m_strMapFileName = "";

	return true;
}

// ============================================================================
// ============================================================================
void *  VistaSharedMemory::GetSharedMemPointer (string & sharedMem)
{
	if (errno != 0)
	{
		// reset error status
		errno = 0;
	}

	void *  pSharedMem = NULL;

	// is there already a stored mem pointer
	if (m_pSharedMem)
	{
		// if the string identifies the same shared mem
		// than return aleady stored pointer
		if (sharedMem.compare(m_strMapFileName) == 0)
		{
			return m_pSharedMem;
		}
		// reset member
		m_pSharedMem = NULL;
	}

#ifdef WIN32
	// handle to file mapping
	HANDLE hMapObj = NULL;
	// create a named file mapping object
	//hMapObj = CreateFileMapping (
	//    (HANDLE) 0xFFFFFFFF,    // use paging file
	//    NULL,                   // no security attributes
	//    PAGE_READWRITE,         // read/write acces
	//    0,                      // size: high 32 bits
	//    8, //SHMEMSIZE,              // size: low 32 bits
	//    sharedMem.data()        // name of the map object
	//    );
	hMapObj = OpenFileMapping (FILE_MAP_WRITE, FALSE, sharedMem.c_str() );

	if (hMapObj)
	{
		//unsigned long errCode = GetLastError();
		GetLastError();
		//if (errCode == ERROR_ALREADY_EXISTS)
		{
			// get a pointer to the file mapped shared memory
			pSharedMem = MapViewOfFile (
				hMapObj,            // object to map view of
				FILE_MAP_WRITE,     // read/write access
				0,                  // high offset: map from beginning
				0,                  // low offset
				0                   // default: map entire file
				);
		}
		// I don't neeed this handle anymore
		//CloseHandle (hMapObj);
	}
#else
#ifndef LINUX
//#ifndef SUNOS
	// reopen a mem file
	int memFileDesc = shm_open (sharedMem.c_str(), O_RDWR, 0666);
	if (memFileDesc <= 0)
	{
		perror ("SHM_REOPEN_ERROR (shm_open) ");
		return NULL;
	}
	int mProt  = PROT_WRITE | PROT_READ;
	int mFlags = MAP_SHARED;

#ifdef IRIX
	// these flags are not defined in POSIX!!
	//mFlags |= (MAP_AUTOGROW | MAP_AUTORESRV);
	mFlags |= MAP_AUTOGROW;
#endif

	int size = m_nSharedMemSize;
	// map shared memory to process memory
	pSharedMem = mmap (NULL, (size_t)size, mProt, mFlags, memFileDesc, (off_t)0);
	if (pSharedMem == MAP_FAILED)
	{
		perror ("SHM_REOPEN_ERROR (mmap) ");
		return NULL;
	}
//#endif
#endif
#endif
	if (errno != 0)
	{
		//perror ("SHM_REOPEN_ERROR ");
	}
	// set retrieved mem pointer to local member
	m_pSharedMem     = pSharedMem;
	m_strMapFileName = sharedMem;

	return pSharedMem;
}

// ============================================================================
// ============================================================================
bool VistaSharedMemory::SetSharedMem (string & sharedMem, void *pBuffer,
									  long size, long offset)
{
	// get address of shared mem
	void * pSharedMem = this->GetSharedMemPointer (sharedMem);
	if (pSharedMem != NULL)
	{
		for (int i = 0; i < size; i++)
		{
			((char*)pSharedMem)[offset + i] = ((char*)pBuffer)[i];
		}
#ifdef WIN32
		//UnmapViewOfFile (pSharedMem);
#else
		//munmap (pSharedMem, 0);
#endif
	}
	else
	{
		perror ("SHM_SET_ERROR (GetSharedMemPointer) ");
		return false;
	}
	return true;
}

// ============================================================================
// ============================================================================
bool VistaSharedMemory::GetSharedMem (string & sharedMem, void *pBuffer,
									   long size, long offset)
{
	// get address of shared mem
	void * pSharedMem = this->GetSharedMemPointer (sharedMem);
	if (pSharedMem != NULL)
	{
		for (int i = 0; i < size; i++)
		{
			((char*)pBuffer)[i] = ((char*)pSharedMem)[offset + i];
		}
#ifdef WIN32
		//UnmapViewOfFile (pSharedMem);
#else
		//munmap (pSharedMem, 0);
#endif
	}
	else
	{
		perror ("SHM_GET_ERROR (GetSharedMemPointer) ");
		return false;
	}
	return true;
}

// ============================================================================
// ============================================================================
bool VistaSharedMemory::SetSharedMemSize (long size)
{
	if (size < 0)
		return false;
	m_nSharedMemSize = size;
	return true;
}

// ============================================================================
// ============================================================================
long VistaSharedMemory::GetSharedMemSize ()
{
	return m_nSharedMemSize;
}

