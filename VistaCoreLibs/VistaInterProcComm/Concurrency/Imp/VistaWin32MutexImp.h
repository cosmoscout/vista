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

#ifndef _VISTAWIN32MUTEXIMP_H
#define _VISTAWIN32MUTEXIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/Concurrency/imp/VistaMutexImp.h>

#if !defined(_WINDOWS_)
#define _WIN32_WINNT 0x0500 // we need this for TryLock!!
#include <Windows.h>
#endif

#include <string>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTAINTERPROCCOMMAPI VistaWin32MutexImp : public IVistaMutexImp
{
public:
	VistaWin32MutexImp  (const std::string &sName, IVistaMutexImp::eScope nScope);
	virtual ~VistaWin32MutexImp ();


	virtual void Lock ();

	virtual bool TryLock();

	virtual void Unlock();

	HANDLE GetMutex() { return m_Mutex; };

private:
	CRITICAL_SECTION     win32CriticalSection;
	HANDLE m_Mutex;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //_VISTASYSTEM_H

#endif // VISTA_THREADING_WIN32


