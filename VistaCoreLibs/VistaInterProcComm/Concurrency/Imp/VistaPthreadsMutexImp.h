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

#ifndef _VISTAPTHREADSMUTEXIMP_H
#define _VISTAPTHREADSMUTEXIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaMutexImp.h"


#include <pthread.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VistaPthreadsMutexImp : public IVistaMutexImp
{
public:
	VistaPthreadsMutexImp(const std::string &sName, const IVistaMutexImp::eScope nScope);
	virtual ~VistaPthreadsMutexImp ();

	virtual void Lock    ();
	virtual bool TryLock ();
	virtual void Unlock  ();

	pthread_mutex_t *GetPthreadMutex() { return pPosixMutex; };

private:
	pthread_mutex_t*     pPosixMutex;
	//pthread_mutexattr_t  posixMutexAttr;
	int m_sharedMem_fd;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTASYSTEM_H

#endif // VISTA_THREADING_PTHREAD


