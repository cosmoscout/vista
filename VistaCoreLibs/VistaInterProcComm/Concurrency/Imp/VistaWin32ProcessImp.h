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

#ifndef _VISTAWIN32PROCESSIMP_H
#define _VISTAWIN32PROCESSIMP_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaProcessImp.h"

#if !defined(_WINDOWS_)
#include <Windows.h>
#endif


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPriority;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaWin32ProcessImp : public IVistaProcessImp
{
public:

	VistaWin32ProcessImp();

	virtual ~VistaWin32ProcessImp();

	virtual bool Run( const std::string & inCommand );
	virtual bool Suspend();
	virtual bool Resume();
	virtual bool Join();
	virtual bool Abort();

	virtual bool SetPriority( const VistaPriority & );
	virtual void GetPriority( VistaPriority & ) const;

private:
	void CleanupProcess();
protected:
	HANDLE   win32Handle;
};

#endif // WIN32

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAWIN32PROCESSIMP_H
