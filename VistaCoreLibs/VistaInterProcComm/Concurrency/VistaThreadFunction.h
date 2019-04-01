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


#ifndef _VISTATHREADFUNCTION_H
#define _VISTATHREADFUNCTION_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaThread.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 * A thread function objects servers the purpose of a simplified way to run
 * a function concurrently. Remember that a function is not bound to a class.
 * In order to do this, we create a VistaThreadFunction() instance and pass
 * to it the pointer to the function to run. Note that the return type of the
 * function is assumed to be void. If that is not the case, you may implement
 * a void wrapper that deals with return values of the function.<br>
 * Note that arguments are passed as pointer values. Complex structures may
 * point to instances or structures, for base types this may seem awkward, but
 * can still be done ;)<br>
 * Usage example
 * * void MyFunction(int *iArg) { ... }
 * * [...]
 * * int iArg = 42; // <-- must outlive func, OR be deep copied by func
 * * VistaThreadFunction<int> func(MyFunction, &iArg);
 * * func.Run(); // performs MyFunction asynchronously
 * * [...]
 * * func.Join(); // join with MyFunction (make sure, it terminates)
 */
template<class T>
class VistaThreadFunction : public VistaThread
{
public:
	/**
	 * @param pfc pointer of a function to run as a thread
	 * @param data pointer to the data that the function might need
	 */
	VistaThreadFunction<T>( void (*pfc)(T*), T *data, IVistaThreadImp *pImp = NULL )
		: VistaThread(pImp),
		  m_pfc(pfc),
		  m_pFctData(data)
	{
	}

	/**
	 * Note that we do not assume that the thread already stopped. Take
	 * care of that yourself.
	 */
	~VistaThreadFunction()
	{
	}


	/**
	 * Simply calls the function and passes the arguments. When this call returns,
	 * the thread will cease to exists (after returning from ThreadBody())
	 */
	void ThreadBody   ()
	{
		(*m_pfc)(m_pFctData);
	}

	/**
	 * returns the function/local data that was passed to the function.
	 * Can be handy, for example when statistics or return values are
	 * to be retrieved.
	 * @return the function data that was passed upon creation.
	 * @see VistaThreadFunction()
	 */
	T *GetThreadFctData() const
	{
		return m_pFctData;
	}
protected:
private:
	/**
	 * A pointer to the function to execute.
	 */
	void (*m_pfc)(T*);

	/**
	 * Pointer to the data to pass to the function upon execution.
	 */
	T *m_pFctData;
};


/**
 * A simple typedef to run threaded functions as they are often
 * typed by threaded subsystems. (That is, pthreads and win32
 * typically offer to run void (*f)(void*) functions as a thread.
 */
typedef VistaThreadFunction<void> VistaThreadedFunction;

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
