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

#include "VistaProcess.h"
#include "Imp/VistaProcessImp.h"

// c/c++ language includes
#include <cstdlib>
#include <string>
using namespace std;

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/

VistaProcess::VistaProcess() {
  m_pProcessImp = IVistaProcessImp::CreateProcessImp();
  m_bRunning    = false;
  m_bIsFinished = false;
}

/*============================================================================*/

VistaProcess::~VistaProcess() {
  delete m_pProcessImp;
}

/*============================================================================*/

bool VistaProcess::Run(const string& inCommand) {
  m_bRunning = m_pProcessImp->Run(inCommand);
  if (m_bRunning)
    m_sProcessCommand = inCommand;

  return m_bRunning;
}

/// stop (== halt) a process
bool VistaProcess::Suspend() {
  return m_pProcessImp->Suspend();
}

/// let a stopped process continue
bool VistaProcess::Resume() {
  return m_pProcessImp->Resume();
}

/// wait for a process to be finished
bool VistaProcess::Join() {
  return m_pProcessImp->Join();
}

/// kill a process
bool VistaProcess::Abort() {
  return (m_bRunning = m_pProcessImp->Abort());
}

/// set the priority of a process
bool VistaProcess::SetPriority(const VistaPriority& prio) {
  return m_pProcessImp->SetPriority(prio);
}

/// get the priority of a process
void VistaProcess::GetPriority(VistaPriority& prio) const {
  m_pProcessImp->GetPriority(prio);
}

void VistaProcess::PreRun() {
  m_bIsFinished = false;
  m_pProcessImp->PreRun();
}

void VistaProcess::PostRun() {
  m_pProcessImp->PostRun();
  m_sProcessCommand = "";
  m_bIsFinished     = true;
  m_bRunning        = false;
}

string VistaProcess::GetProcessCommand() const {
  return m_sProcessCommand;
}

int VistaProcess::GetIsRunning() const {
  return m_bRunning;
}

bool VistaProcess::GetIsFinished() const {
  return m_bIsFinished;
}
