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

#include "VistaThreadTask.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaThreadTask::VistaThreadTask() {
  m_pTask = 0;
}

VistaThreadTask::~VistaThreadTask() {
  m_pTask = (IVistaThreadedTask*)0xDEADBEEF;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaThreadTask::ThreadBody() {
  if (m_pTask) {
    m_pTask->ThreadWork();
  }
}

bool VistaThreadTask::SetThreadedTask(IVistaThreadedTask* pTask) {
  if (m_pTask && !m_pTask->GetIsDone())
    return false;

  if (!m_pTask) {
    m_pTask = pTask;
    return true;
  }

  return false; // task not done, yet
}

IVistaThreadedTask* VistaThreadTask::GetThreadedTask() const {
  return m_pTask;
}

void VistaThreadTask::PostRun() {
  VistaThread::PostRun();
  if (m_pTask && m_pTask->GetIsProcessed()) {
    m_pTask->PostWork();
    m_pTask->StopWork(); // declare finished ;)
  }
}

// #############################################################################

IVistaThreadedTask::IVistaThreadedTask() {
  m_bIsDone      = false;
  m_bIsProcessed = false;
}

IVistaThreadedTask::~IVistaThreadedTask() {
}

bool IVistaThreadedTask::GetIsDone() const {
  return m_bIsDone;
}

bool IVistaThreadedTask::GetIsProcessed() const {
  return m_bIsProcessed;
}

void IVistaThreadedTask::PreWork() {
}

void IVistaThreadedTask::PostWork() {
}

void IVistaThreadedTask::StartWork() {
  m_bIsDone      = false;
  m_bIsProcessed = true;
}
void IVistaThreadedTask::StopWork() {
  m_bIsDone      = true;
  m_bIsProcessed = false;
}

void IVistaThreadedTask::ThreadWork() {
  StartWork();
  PreWork();
  DefinedThreadWork();
  PostWork();
  StopWork();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
