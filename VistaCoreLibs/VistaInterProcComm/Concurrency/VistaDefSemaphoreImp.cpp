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


#include "VistaDefSemaphoreImp.h"

#include <limits.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/

VistaDefSemaphoreImp::VistaDefSemaphoreImp ( unsigned int inStart )
: IVistaSemaphoreImp()
{
	if ( inStart > INT_MAX )
		counter = INT_MAX;
	else
		counter = static_cast<int> ( inStart );

	wait.Lock ();
}

void VistaDefSemaphoreImp::Wait()
{
	counterAccess.Lock ();

	if ( counter -- <= 0 )
	{
		counterAccess.Unlock ();
		wait.Lock   ();
	}
	else
	{
		counterAccess.Unlock ();
	}
}

bool VistaDefSemaphoreImp::TryWait ()
{
	counterAccess.Lock ();

	if ( counter <= 1  )
	{
		counterAccess.Unlock ();
		return false;
	}
	else
	{
		counter --;
		counterAccess.Unlock ();
		return true;
	}
}

void VistaDefSemaphoreImp::Post ()
{
	counterAccess.Lock ();

	if ( counter < 0 )
	{
		wait.Unlock();
	}

	counter ++;

	counterAccess.Unlock ();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

