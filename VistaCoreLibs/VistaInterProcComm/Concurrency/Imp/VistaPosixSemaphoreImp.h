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

#ifndef _VISTAPOSIXSEMAPHOREIMP_H
#define _VISTAPOSIXSEMAPHOREIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaSemaphoreImp.h"

#include <semaphore.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaPosixSemaphoreImp : public IVistaSemaphoreImp
{
public:
	VistaPosixSemaphoreImp(int nCount);
	virtual ~VistaPosixSemaphoreImp();	

	/**
	 * if semaphore value is > 0 then decrement it and carry on. If it's
	 * already 0 then block.
	 */
	virtual void Wait    ();

	/**
	 * if semaphore value is > 0 then decrement it and return true.
	 * If it's already 0 then return false.
	 */
	virtual bool TryWait ();

	/**
	 * if any threads are blocked in wait(), wake one of them up. Otherwise
	 * increment the value of the semaphore.
	 */
	virtual void Post    ();

private:
	sem_t *m_pSemaphore;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

#endif // #if defined(VISTA_THREADING_POSIX)

