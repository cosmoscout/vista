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

#ifndef _VISTAPTHREADSTHREADCONDITIONIMP_H
#define _VISTAPTHREADSTHREADCONDITIONIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(VISTA_THREADING_POSIX)

#include "VistaThreadConditionImp.h"
#include <pthread.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaMutexImp;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VistaPthreadsThreadConditionImp : public IVistaThreadConditionImp {
 private:
 protected:
  pthread_cond_t m_Condition;

 public:
  VistaPthreadsThreadConditionImp();
  virtual ~VistaPthreadsThreadConditionImp();

  virtual int SignalCondition();
  virtual int BroadcastCondition();
  virtual int WaitForCondition(IVistaMutexImp*);
  virtual int WaitForConditionWithTimeout(IVistaMutexImp*, int iMsecs);
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATHREADCONDITIONIMP_H

#endif // VISTA_THREADING_POSIX
