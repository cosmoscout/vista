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

#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32MutexImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaWin32MutexImp::VistaWin32MutexImp(const std::string& sName, IVistaMutexImp::eScope nScope) {
#if defined(_USE_WIN32_MUTEX)
  switch (nScope) {
  case IVistaMutexImp::eInterProcess: {
    if (sName.empty())
      m_Mutex = NULL;
    else {
      // try open handle
      m_Mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, sName.c_str());
      if (!m_Mutex) {
        // ok, we can not open the mutex, so we try to create it
        m_Mutex = CreateMutex(NULL, false, (sName.c_str()));
      }
    }
    break;
  }
  case IVistaMutexImp::eIntraProcess: {
    m_Mutex = CreateMutex(NULL, false, (sName == "" ? NULL : sName.c_str()));
    break;
  }
  default:
    m_Mutex = NULL;
    break;
  }
#else
  InitializeCriticalSection(&win32CriticalSection);
#endif
}

VistaWin32MutexImp::~VistaWin32MutexImp() {
#if defined(_USE_WIN32_MUTEX)
  if (m_Mutex)
    CloseHandle(m_Mutex);
#else
  DeleteCriticalSection(&win32CriticalSection);
#endif
}

void VistaWin32MutexImp::Lock() {
#if defined(_USE_WIN32_MUTEX)
  WaitForSingleObject(m_Mutex, INFINITE);
#else
  EnterCriticalSection(&win32CriticalSection);
#endif
}

bool VistaWin32MutexImp::TryLock() {
#if defined(_USE_WIN32_MUTEX)
  DWORD result = WaitForSingleObject(m_Mutex, 1L);
  if (result == WAIT_OBJECT_0)
    return true;

  return false;
#else
  return (TryEnterCriticalSection(&win32CriticalSection) != 0);
#endif
}

void VistaWin32MutexImp::Unlock() {
#if defined(_USE_WIN32_MUTEX)
  ReleaseMutex(m_Mutex);
#else
  LeaveCriticalSection(&win32CriticalSection);
#endif
}

#endif // VISTA_THREADING_WIN32
