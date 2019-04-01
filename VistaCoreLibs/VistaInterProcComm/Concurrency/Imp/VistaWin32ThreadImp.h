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

#ifndef _VISTAWIN32THREADIMP_H
#define _VISTAWIN32THREADIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaThreadImp.h"


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
class VistaThread;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI  VistaWin32ThreadImp : public IVistaThreadImp
{
public:
	VistaWin32ThreadImp( const VistaThread& );
	virtual ~VistaWin32ThreadImp();

	virtual bool Run();
	virtual bool Suspend();
	virtual bool Resume();
	virtual bool Join();
	virtual bool Abort();

	virtual bool SetPriority( const VistaPriority & );
	virtual void GetPriority(VistaPriority &) const;

	static IVistaThreadImp *CreateThreadImp(const VistaThread &);

	/**
	 * give the processor away temporarily
	 */
	void YieldThread();

	void SetCancelAbility(const bool bOkToCancel);
	bool CanBeCancelled() const;

   /**
	 * Method that is to be performed BEFORE departed fork starts execution
	 * DO NOT CALL THIS METHOD AT ANY COST!
	 */
	virtual void PreRun() ;

	/**
	 * Method that is to be performed AFTER forked work is done
	 * DO NOT CALL THIS METHOD AT ANY COST!
	 */
	virtual void PostRun();

	virtual bool Equals(const IVistaThreadImp &oImp) const;

	virtual bool SetProcessorAffinity(int iProcessorNum);

	virtual bool SetThreadName(const std::string &sName);

	virtual long GetThreadIdentity() const;
	static long GetCallingThreadIdentity();

	static bool SetCallingThreadPriority( const VistaPriority& oPrio );
	static bool GetCallingThreadPriority( VistaPriority& oPrio );


protected:
	HANDLE   m_oWin32Handle;
	DWORD    m_nThreadId;
	const VistaThread  &m_rThread; /**< @todo think about this */
	bool     m_bIsRunning;
	bool     m_bCanBeCancelled;
	DWORD    m_dwAffinityMask;
};

#endif // WIN32

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAWIN32THREADMP_H

