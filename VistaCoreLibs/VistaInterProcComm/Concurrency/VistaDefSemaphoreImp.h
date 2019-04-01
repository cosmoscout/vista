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


#ifndef _VISTADEFSEMAPHOREIMP_H
#define _VISTADEFSEMAPHOREIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaIpcThreadModel.h"
#include "VistaMutex.h"
#include "Imp/VistaSemaphoreImp.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSemaphoreImp;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaDefSemaphoreImp : public IVistaSemaphoreImp
{
public:
	VistaDefSemaphoreImp( unsigned int initial = 1 );
	virtual ~VistaDefSemaphoreImp() {};	

	/**
	 * if semaphore value is > 0 then decrement it and carry on. If it's
	 * already 0 then block.
	 */
	void Wait    ();	

	/**
	 * if semaphore value is > 0 then decrement it and return true.
	 * If it's already 0 then return false.
	 */
	bool TryWait ();	

	/**
	 * if any threads are blocked in wait(), wake one of them up. Otherwise
	 * increment the value of the semaphore.
	 */
	void Post    ();

private:
	VistaDefSemaphoreImp ( const VistaDefSemaphoreImp & );
	VistaDefSemaphoreImp & operator=       ( const VistaDefSemaphoreImp & );

	VistaMutex		counterAccess, wait;
	int				counter;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
