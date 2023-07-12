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

#include "VistaIOMultiplexer.h"

#include <VistaInterProcComm/Concurrency/VistaThreadCondition.h>
#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>
#include <VistaInterProcComm/Concurrency/VistaThreadTask.h>

#include <VistaInterProcComm/Concurrency/VistaMutex.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaIOMultiplexer::VistaIOMultiplexer() {
  m_pStateMutex = new VistaMutex;

  m_pShutdownMutex = new VistaMutex;
  m_pTicketMutex   = new VistaMutex;

  m_pShutdown = new VistaThreadCondition;
  m_eState    = MP_NONE;
  m_eCom      = MPC_NONE;

  m_iTicket = 1;
}

VistaIOMultiplexer::~VistaIOMultiplexer() {

  delete m_pTicketMutex;

  delete m_pStateMutex;
  delete m_pShutdownMutex;
  delete m_pShutdown;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaIOMultiplexer::GetNextTicket() {
  VistaMutexLock l(*m_pTicketMutex);
  return ++m_iTicket;
}

void VistaIOMultiplexer::Shutdown() {
  SetCommand(MPC_SHUTDOWN);
}

VistaIOMultiplexer::eIOState VistaIOMultiplexer::GetState() const {
  VistaMutexLock l(*m_pStateMutex);
  return m_eState;
}

bool VistaIOMultiplexer::SetCommand(eIOCom cmd) {
  m_eCom = cmd;
  return true;
}

bool VistaIOMultiplexer::SetState(eIOState eState) {
  VistaMutexLock l(*m_pStateMutex);
  m_eState = eState;

  return true;
}

void VistaIOMultiplexer::WaitForShutdownComplete() {
  VistaMutexLock l(*m_pShutdownMutex);
  (*m_pShutdown).WaitForCondition(*m_pShutdownMutex);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
