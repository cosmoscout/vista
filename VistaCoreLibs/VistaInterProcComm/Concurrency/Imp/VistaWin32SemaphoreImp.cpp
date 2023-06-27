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
#define _WIN32_WINNT 0x0500 // we need this for the try call

#include "VistaWin32SemaphoreImp.h"

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaWin32SemaphoreImp::VistaWin32SemaphoreImp(int iCnt, IVistaSemaphoreImp::eSemType eType)
    : IVistaSemaphoreImp()
    , m_eType(eType)
    , m_hs(NULL)
    , m_bCs(false) {

  if ((iCnt == 1) && (m_eType == IVistaSemaphoreImp::SEM_TYPE_FASTEST)) {
    InitializeCriticalSection(&m_cs);
    m_bCs = true;
  } else {
    m_hs = CreateSemaphore(NULL, iCnt, iCnt, NULL);
  }
}

VistaWin32SemaphoreImp::~VistaWin32SemaphoreImp() {
  if (m_bCs)
    DeleteCriticalSection(&m_cs);
  else {
    CloseHandle(m_hs);
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaWin32SemaphoreImp::Wait() {
  if (m_bCs) {
    EnterCriticalSection(&m_cs);
  } else {
    DWORD hr = WaitForSingleObject(m_hs, INFINITE);
    if (hr == WAIT_ABANDONED) {
      vstr::errp() << "[VSemWin32]: Semaphore @ " << this << ", wait was abandoned" << std::endl;
    }
  }
}

bool VistaWin32SemaphoreImp::TryWait() {
  if (m_bCs) {
    return (TryEnterCriticalSection(&m_cs) != 0);
  } else {
    DWORD hr = WaitForSingleObject(m_hs, 0);
    if (hr == WAIT_ABANDONED) {
      vstr::errp() << "[VSemWin32]: Semaphore @ " << this << ", trywait was abandoned" << std::endl;
      return false;
    }

    return (hr == WAIT_OBJECT_0);
  }
}

void VistaWin32SemaphoreImp::Post() {
  if (m_bCs) {
    LeaveCriticalSection(&m_cs);
  } else {
    ReleaseSemaphore(m_hs, 1, NULL);
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // VISTA_THREADING_WIN32
