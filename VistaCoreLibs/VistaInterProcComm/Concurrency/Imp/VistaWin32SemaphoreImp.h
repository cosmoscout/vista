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


#if defined(VISTA_THREADING_WIN32)

#ifndef _VISTAWIN32SEMAPHOREIMP_H
#define _VISTAWIN32SEMAPHOREIMP_H

#if defined(WIN32)
#include <Windows.h>
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaSemaphoreImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaWin32SemaphoreImp : public IVistaSemaphoreImp
{
public:
	VistaWin32SemaphoreImp(int iCnt, IVistaSemaphoreImp::eSemType eType);
	virtual ~VistaWin32SemaphoreImp();


	virtual void Wait    ();
	virtual bool TryWait ();
	virtual void Post    ();
protected:
private:
	IVistaSemaphoreImp::eSemType m_eType;
	CRITICAL_SECTION             m_cs;
	bool                         m_bCs;
	HANDLE                       m_hs;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif

#endif // VISTA_THREADING_WIN32


