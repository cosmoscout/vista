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


#ifndef _VISTAMUTEX_H
#define _VISTAMUTEX_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaIpcThreadModel.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <string>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaMutexImp;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaMutex
{
public:
	VistaMutex  ();
	VistaMutex (const std::string &sName);

	virtual ~VistaMutex();

	void Lock();

	/**
	 * Tries to access the lock on this mutex, if it is already gained by some other
	 * thread, this routine will return immediately.
	 * Notes: in comparison to Lock() this is a rather costly operation, in addition to
	 * the non defined semantics in VistaInterProcComm to the aspect of multiple locking
	 * in the same thread context.
	 * Note for win32 systems: This method can only be used on systems >= NT4
	 * @return false iff this mutex is locked by another thread, else true
	 * @todo What happens when the same thread locks the same mutex more than one time on different platforms?
	 */
	bool TryLock ();

	void Unlock  ();

	IVistaMutexImp *GetImplementation() const { return m_pImp; };
private:
	VistaMutex ( const VistaMutex & );
	VistaMutex & operator=   ( const VistaMutex & );
	IVistaMutexImp  *m_pImp;
};

class VISTAINTERPROCCOMMAPI VistaMutexLock
{
public:
	/**
	 * Constructor, locks the mutex. Always create mutex-locks
	 * on stack, so they get destructed after the function is left
	 * (even after throwing an exception)
	 */
	inline VistaMutexLock  ( VistaMutex & inMutex )
	: mutex( inMutex ), m_bLocked(false)
	{
		Lock();
	}


	inline ~VistaMutexLock ()
	{
		if(m_bLocked)
			Unlock();
	}

	void Lock()
	{
		this->mutex.Lock();
		m_bLocked = true;
	}

	void Unlock()
	{
		m_bLocked = false;
		this->mutex.Unlock();
	}

private:
	VistaMutexLock( const VistaMutexLock &other );
	VistaMutexLock & operator=       ( const VistaMutexLock & );


	/**
	 * we need to record the mutex for the destructor
	 */
	VistaMutex & mutex;
	bool          m_bLocked;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
