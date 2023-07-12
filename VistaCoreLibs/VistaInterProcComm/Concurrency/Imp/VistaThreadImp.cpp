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

#include "VistaThreadImp.h"

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>
#include <VistaInterProcComm/Concurrency/VistaThread.h>

#include <VistaBase/VistaStreamUtils.h>

#if defined(VISTA_THREADING_WIN32)
#include "VistaWin32ThreadImp.h"
#elif defined(VISTA_THREADING_POSIX)
#include "VistaPthreadsThreadImp.h"
#else
#error "PROCESSIMPLEMENTATION UNSUPPORTED FOR THIS PLATFORM!"
#endif

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// initialize with NULL
IVistaThreadImp::IVistaThreadImpFactory* IVistaThreadImp::m_pSImpFactory = NULL;

IVistaThreadImp::IVistaThreadImpFactory::IVistaThreadImpFactory() {
}

IVistaThreadImp::IVistaThreadImpFactory::~IVistaThreadImpFactory() {
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaThreadImp::IVistaThreadImp() {
}

IVistaThreadImp::~IVistaThreadImp() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVistaThreadImp* IVistaThreadImp::CreateThreadImp(const VistaThread& thread) {
  if (m_pSImpFactory) {
    return m_pSImpFactory->CreateThread(thread);
  } else {
// factory method
#if defined(VISTA_THREADING_WIN32)
    return new VistaWin32ThreadImp(thread);
#elif defined(VISTA_THREADING_POSIX)
    return new VistaPthreadThreadImp(thread);
#else
    return NULL;
#endif
  }
}

void IVistaThreadImp::RegisterThreadImpFactory(IVistaThreadImpFactory* pFactory) {
  if (m_pSImpFactory)
    vstr::errp() << "IVistaThreadImp::RegisterThreadImpFactory() -- "
                 << "Factory already registered - overwriting previous one" << std::endl;

  m_pSImpFactory = pFactory;
}

IVistaThreadImp::IVistaThreadImpFactory* IVistaThreadImp::GetThreadImpFactory() {
  return m_pSImpFactory;
}

bool IVistaThreadImp::Equals(const IVistaThreadImp& oImp) const {
  return false; /** @todo define me in subclasses */
}

bool IVistaThreadImp::SetProcessorAffinity(int iProcessorNum) {
  return false; /** @todo define me in subclasses */
}

int IVistaThreadImp::GetCpu() const {
  return -1; // define me in subclass
}

bool IVistaThreadImp::SetThreadName(const std::string& sName) {
  DoSetThreadName(sName);
  return true;
}

std::string IVistaThreadImp::GetThreadName() const {
  return m_sThreadName;
}

void IVistaThreadImp::DoSetThreadName(const std::string& sName) {
  m_sThreadName = sName;
}

long IVistaThreadImp::GetCallingThreadIdentity(bool bBypassFactory) {
  if (m_pSImpFactory && !bBypassFactory) {
    return m_pSImpFactory->GetCallingThreadIdentity();
  } else {
// factory method
#if defined(VISTA_THREADING_WIN32)
    return VistaWin32ThreadImp::GetCallingThreadIdentity();
#elif defined(VISTA_THREADING_POSIX)
    return VistaPthreadThreadImp::GetCallingThreadIdentity();
#else
    VISTA_THROW_NOT_IMPLEMENTED;
#endif
  }
}

void IVistaThreadImp::DeleteThreadImpFactory() {
  delete m_pSImpFactory;
  m_pSImpFactory = NULL;
}

bool IVistaThreadImp::SetCallingThreadPriority(
    const VistaPriority& oPrio, bool bBypassFactory /*= false */) {
  if (m_pSImpFactory && !bBypassFactory) {
    return m_pSImpFactory->SetCallingThreadPriority(oPrio);
  } else {
// factory method
#if defined(VISTA_THREADING_WIN32)
    return VistaWin32ThreadImp::SetCallingThreadPriority(oPrio);
#elif defined(VISTA_THREADING_POSIX)
    return VistaPthreadThreadImp::SetCallingThreadPriority(oPrio);
#else
    VISTA_THROW_NOT_IMPLEMENTED;
#endif
  }
}

bool IVistaThreadImp::GetCallingThreadPriority(
    VistaPriority& oPrio, bool bBypassFactory /*= false */) {
  if (m_pSImpFactory && !bBypassFactory) {
    return m_pSImpFactory->GetCallingThreadPriority(oPrio);
  } else {
// factory method
#if defined(VISTA_THREADING_WIN32)
    return VistaWin32ThreadImp::GetCallingThreadPriority(oPrio);
#elif defined(VISTA_THREADING_POSIX)
    return VistaPthreadThreadImp::GetCallingThreadPriority(oPrio);
#else
    VISTA_THROW_NOT_IMPLEMENTED;
#endif
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
