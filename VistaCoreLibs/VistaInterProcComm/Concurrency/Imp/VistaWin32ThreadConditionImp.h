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


#if defined(WIN32)

#ifndef _VISTAWIN32THREADCONDITIONIMP_H
#define _VISTAWIN32THREADCONDITIONIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaThreadConditionImp.h"

#if !defined(_WINDOWS_)
#define _WIN32_WINNT 0x0500 // we need this for TryLock!!
#include <Windows.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaWin32MutexImp;
class IVistaMutexImp;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAINTERPROCCOMMAPI VistaWin32ThreadConditionImp : public IVistaThreadConditionImp
{
private:
	int m_nWaitersCount;
	CRITICAL_SECTION m_Waiters_count_lock;
	HANDLE m_WaitQueue;
	HANDLE m_WaitersDone;
	size_t m_wasBroadcast;
protected:
public:
	VistaWin32ThreadConditionImp();
	virtual ~VistaWin32ThreadConditionImp();

	virtual int SignalCondition() ;
	virtual int BroadcastCondition() ;
	virtual int WaitForCondition(IVistaMutexImp *) ;
	virtual int WaitForConditionWithTimeout(IVistaMutexImp*, int iMsecs);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATHREADCONDITIONIMP_H


#endif // WIN32

