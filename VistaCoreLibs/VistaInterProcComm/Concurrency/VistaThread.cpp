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


#include "VistaThread.h"
#include "Imp/VistaThreadImp.h"
#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/

VistaThread::VistaThread (IVistaThreadImp *pImp)
: m_bIsRunning(false), m_bIsFinished(false),
  m_pImp(NULL)
{
  m_pImp = (pImp ? pImp : IVistaThreadImp::CreateThreadImp(*this));
}

/*============================================================================*/

VistaThread::~VistaThread ()
{
	delete m_pImp; /** @todo use factory-method from imp-interface? */
}

/*============================================================================*/

bool VistaThread::IsRunning() const
{
	return m_bIsRunning;
}


bool VistaThread::Run ()
{
	return (m_bIsRunning=m_pImp->Run()) == true;
}

/*============================================================================*/

bool VistaThread::Suspend ()
{
	return m_pImp->Suspend();
}

bool VistaThread::Resume ()
{
	return m_pImp->Resume();
}

bool VistaThread::Join ()
{
	return m_pImp->Join();
}

bool VistaThread::Abort ()
{
#ifdef DEBUG
	vstr::warnp() <<
		"WARNING: You are using VistaThread::Abort(). The concrete behavior " << std::endl <<
		"of this method is not defined, probably differs between systems " << std::endl <<
		"and can lead to undefined behavior of the overall application." << std::endl <<
		"Consider using an own implementation to exit a thread." << std::endl;
#endif
	return (m_bIsRunning = m_pImp->Abort()) == true;
}


/*============================================================================*/

void VistaThread::GetPriority (VistaPriority &prio) const
{
	m_pImp->GetPriority(prio);
}

/*============================================================================*/

bool VistaThread::SetPriority ( const VistaPriority &prio )
{
	return m_pImp->SetPriority(prio);
}

void VistaThread::YieldThread ()
{
	m_pImp->YieldThread();
}

void VistaThread::PreRun()
{
	m_bIsFinished = false;
	m_pImp->PreRun();
}

void VistaThread::PostRun()
{
	m_pImp->PostRun();
	m_bIsRunning = false;
	m_bIsFinished = true;
}

bool VistaThread::Equals(const VistaThread &oOther) const
{
	return m_pImp->Equals(*oOther.m_pImp);
}

bool VistaThread::operator==(const VistaThread &oOther)
{
	return Equals(oOther);
}


bool VistaThread::SetProcessorAffinity(int iProcessorNum)
{
	return m_pImp->SetProcessorAffinity(iProcessorNum);
}

int VistaThread::GetCpu() const
{
	return m_pImp->GetCpu();
}

bool VistaThread::GetIsFinished() const
{
	return m_bIsFinished;
}

bool VistaThread::SetThreadName(const std::string &sName)
{
	return m_pImp->SetThreadName(sName);
}

std::string VistaThread::GetThreadName() const
{
	return m_pImp->GetThreadName();
}

long VistaThread::GetThreadIdentity() const
{
	return m_pImp->GetThreadIdentity();
}

long VistaThread::GetCallingThreadIdentity()
{
	return IVistaThreadImp::GetCallingThreadIdentity();
}


IVistaThreadImp *VistaThread::GetThreadImp() const
{
	return m_pImp;
}

bool VistaThread::SetCallingThreadPriority( const VistaPriority& oPrio )
{
	return IVistaThreadImp::SetCallingThreadPriority( oPrio );
}

bool VistaThread::GetCallingThreadPriority( VistaPriority& oPrio )
{
	return IVistaThreadImp::GetCallingThreadPriority( oPrio );
}

