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


#include "VistaSemaphore.h"
#include "VistaDefSemaphoreImp.h"
#include "Imp/VistaSemaphoreImp.h"

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

VistaSemaphore::VistaSemaphore ( unsigned int inStart, eSemType eType )
{
	IVistaSemaphoreImp::eSemType eTp = IVistaSemaphoreImp::SEM_TYPE_FASTEST;
	switch(eType)
	{
	case VistaSemaphore::SEM_TYPE_COMPATIBLE:
		eTp = IVistaSemaphoreImp::SEM_TYPE_COMPATIBLE;
		break;
	case VistaSemaphore::SEM_TYPE_FASTEST:
		eTp = IVistaSemaphoreImp::SEM_TYPE_FASTEST;
		break;
	default:
		break;
	}

	m_pImp = IVistaSemaphoreImp::CreateSemaphoreImp( inStart, eTp );
	if( m_pImp == NULL )
	{
		// this failed, create default
		m_pImp = new VistaDefSemaphoreImp(inStart);
	}
}


VistaSemaphore::~VistaSemaphore()
{
	delete m_pImp;
}

void VistaSemaphore::Wait()
{
	m_pImp->Wait();
}

bool VistaSemaphore::TryWait ()
{
	return m_pImp->TryWait();
}

void VistaSemaphore::Post ()
{
	m_pImp->Post();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

