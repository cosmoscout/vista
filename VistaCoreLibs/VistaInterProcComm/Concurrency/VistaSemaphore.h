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


#ifndef _VISTASEMAPHORE_H
#define _VISTASEMAPHORE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaIpcThreadModel.h"
#include "VistaMutex.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSemaphoreImp;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaSemaphore
{
public:

	enum eSemType
	{
		SEM_TYPE_FASTEST=0,
		SEM_TYPE_COMPATIBLE
	};

	VistaSemaphore( unsigned int initial = 1 , eSemType eType = SEM_TYPE_FASTEST );
	virtual ~VistaSemaphore();

	/**
	 * if semaphore value is > 0 then decrement it and carry on. If it's
	 * already 0 then block.
	 */
	void Wait();


	/**
	 * if semaphore value is > 0 then decrement it and return true.
	 * If it's already 0 then return false.
	 */
	bool TryWait();


	/**
	 * if any threads are blocked in wait(), wake one of them up. Otherwise
	 * increment the value of the semaphore.
	 */
	void Post();

private:
	IVistaSemaphoreImp *m_pImp;
	VistaSemaphore ( const VistaSemaphore & );

	VistaSemaphore & operator=       ( const VistaSemaphore & );
};

class VISTAINTERPROCCOMMAPI VistaSemaphoreLock
{
public:

	inline VistaSemaphoreLock  ( VistaSemaphore & inSemaphore )
	: semaphore( inSemaphore )
	{ this->semaphore.Wait (); }


	inline ~VistaSemaphoreLock ()
	{ this->semaphore.Post (); }

private:
	VistaSemaphoreLock ( const VistaSemaphoreLock & );
	VistaSemaphoreLock & operator=           ( const VistaSemaphoreLock & );
	VistaSemaphore & semaphore;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
