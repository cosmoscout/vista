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

#ifndef _VISTAPTHREADTHREADIMP_H
#define _VISTAPTHREADTHREADIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaThreadImp.h"


#include <pthread.h>

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

class VistaPthreadThreadImp : public IVistaThreadImp
{
public:
	VistaPthreadThreadImp(const VistaThread &);

	virtual ~VistaPthreadThreadImp();
	virtual bool Run( ) ;
	virtual bool Suspend() ;
	virtual bool Resume() ;
	virtual bool Join() ;
	virtual bool Abort() ;

	virtual bool SetPriority( const VistaPriority & ) ;
	virtual void GetPriority( VistaPriority & ) const ;

	static IVistaThreadImp *CreateThreadImp( const VistaThread & );

	void YieldThread();


	void SetCancelAbility( const bool bOkToCancel );
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

	virtual bool Equals(const IVistaThreadImp &) const;

	virtual long GetThreadIdentity() const;
	static long GetCallingThreadIdentity();

	static bool SetCallingThreadPriority( const VistaPriority& oPrio );
	static bool GetCallingThreadPriority( VistaPriority& oPrio );

	virtual bool SetProcessorAffinity(int iProcessorNum);
	virtual int  GetCpu() const;


protected:
	const VistaThread  &m_rThread; /**< @todo think about this */
	bool     m_bCanBeCancelled;
	pthread_t   posixThreadID;
	pthread_attr_t m_ptAttr;
	int m_nPriority;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAWIN32THREADMP_H


#endif // VISTA_THREADING_POSIX

