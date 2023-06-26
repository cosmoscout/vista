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

#include <cerrno>

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>
#if defined(VISTA_THREADING_POSIX)

#include "VistaPthreadsMutexImp.h"
#include "VistaPthreadsThreadConditionImp.h"

#include <unistd.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaPthreadsThreadConditionImp::VistaPthreadsThreadConditionImp() {
  pthread_cond_init(&m_Condition, NULL);
}

VistaPthreadsThreadConditionImp::~VistaPthreadsThreadConditionImp() {
  pthread_cond_destroy(&m_Condition);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaPthreadsThreadConditionImp::SignalCondition() {
  return pthread_cond_signal(&m_Condition);
}

int VistaPthreadsThreadConditionImp::BroadcastCondition() {
  return pthread_cond_broadcast(&m_Condition);
}

int VistaPthreadsThreadConditionImp::WaitForCondition(IVistaMutexImp* pMutexImp) {
  return TEMP_FAILURE_RETRY(pthread_cond_wait(
      &m_Condition, (static_cast<VistaPthreadsMutexImp*>(pMutexImp)->GetPthreadMutex())));
}

int VistaPthreadsThreadConditionImp::WaitForConditionWithTimeout(IVistaMutexImp* pImp, int iMsecs) {
  return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // VISTA_THREADING_POSIX
