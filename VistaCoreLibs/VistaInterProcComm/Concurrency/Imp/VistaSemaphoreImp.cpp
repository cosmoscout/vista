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
#include "VistaSemaphoreImp.h"

#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32SemaphoreImp.h"
#elif defined(VISTA_THREADING_POSIX)
#include "VistaPosixSemaphoreImp.h"
#else
#pragma warning "SEMAPHOREIMPLEMENTATION UNSUPPORTED FOR THIS PLATFORM (USING DEFAULT)!"
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaSemaphoreImp::IVistaSemaphoreImp( ) 
{
}

IVistaSemaphoreImp::~IVistaSemaphoreImp()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/

IVistaSemaphoreImp *IVistaSemaphoreImp ::CreateSemaphoreImp(int iCnt, eSemType eType )
{
#if defined(VISTA_THREADING_WIN32)
	return new VistaWin32SemaphoreImp(iCnt, eType);
#elif defined(VISTA_THREADING_POSIX)
	return new VistaPosixSemaphoreImp(iCnt);
#else
	return 0; /** @todo */
#endif
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

