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

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>
#if defined(VISTA_THREADING_POSIX) && defined(_USE_PTHREAD_RWLOCK)
#include "VistaPthreadReaderWriterLockImp.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPthreadReaderWriterLockImp::VistaPthreadReaderWriterLockImp() {
  pthread_rwlock_init(&m_rwLock, NULL);
}

VistaPthreadReaderWriterLockImp::~VistaPthreadReaderWriterLockImp() {
  pthread_rwlock_destroy(&m_rwLock);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaPthreadReaderWriterLockImp::ReaderLock() {
  return (pthread_rwlock_rdlock(&m_rwLock) == 0);
}

bool VistaPthreadReaderWriterLockImp::WriterLock() {
  return (pthread_rwlock_wrlock(&m_rwLock) == 0);
}

bool VistaPthreadReaderWriterLockImp::ReaderUnlock() {
  return (pthread_rwlock_unlock(&m_rwLock) == 0);
}

bool VistaPthreadReaderWriterLockImp::WriterUnlock() {
  return (pthread_rwlock_unlock(&m_rwLock) == 0);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // VISTA_THREADING_POSIX && _USE_PTHREAD_RWLOCK
