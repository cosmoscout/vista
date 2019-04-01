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

#include <string>

#include "VistaWin32ThreadImp.h"
#include <VistaInterProcComm/Concurrency/VistaThread.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>
#include <VistaBase/VistaStreamUtils.h>

#include <exception>

//
// Usage: SetThreadName (-1, "MainThread");
//
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // must be 0x1000
   LPCSTR szName; // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

static void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = szThreadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
#ifdef WIN32
	#if defined (_MSC_VER)
		#if ( _MSC_VER >= 1400 )
	  RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info );
		#else
	  RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
		#endif
	#endif
#endif
   }
   __except(EXCEPTION_CONTINUE_EXECUTION)
   {
   }
}

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static DWORD WINAPI Win32EntryPoint ( LPVOID that )
{
	VistaThread *pThread = reinterpret_cast<VistaThread*>(that);
	
	// set name
	std::string sName = pThread->GetThreadName();

	try
	{
		if(!sName.empty())
		{
			DWORD threadId = GetCurrentThreadId();
			::SetThreadName(threadId, sName.c_str());
		}

		pThread->PreRun();
		pThread->ThreadBody ();
		pThread->PostRun();
		return 0;
	}
	catch( std::exception &x ) 
	{
		vstr::err() << "Win32-VistaThreadImp -- [" << sName << "] -- execution terminated, given exception: " << x.what() << std::endl;
	}
	catch( ... )
	{
		vstr::err() << "Win32-VistaThreadImp -- [" << sName << "] -- execution terminated due to unkown exception (ellipse catch).";
	}

	return -1;
	
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaWin32ThreadImp::VistaWin32ThreadImp( const VistaThread& oThread )
: m_rThread( oThread )
, m_oWin32Handle( 0 )
, m_nThreadId( 0 )
, m_bIsRunning( false )
, m_bCanBeCancelled( false )
, m_dwAffinityMask( ~0 )
{
	m_oWin32Handle = CreateThread ( 0, 0, Win32EntryPoint, (void*)&m_rThread, CREATE_SUSPENDED, &m_nThreadId );
}
 
VistaWin32ThreadImp::~VistaWin32ThreadImp()
{
}

/// start a process
bool     VistaWin32ThreadImp::Run( )
{
	// just to make sure
	if(m_oWin32Handle == 0)
		return false;

	if( m_bIsRunning )
		return false;

	if( ::ResumeThread  ( m_oWin32Handle ) == -1 )
		return false;

	//// create a thread without special security settings
	//// m_oWin32Handle = CreateThread ( 0, 0, Win32EntryPoint, (void*)&m_rThread, 0, & m_nThreadId );
	//m_bIsRunning = (m_oWin32Handle!=0);
	//if( m_bIsRunning == false )
	//	return false;
	//SetThreadPriority( m_oWin32Handle, m_oPriority.GetSystemPriority() );
	//// if somebody has set an affinity mask != the init value ==> set it now!
	//if( m_dwAffinityMask != ~0)
	//	SetThreadAffinityMask(m_oWin32Handle, m_dwAffinityMask);

	return true;
}


bool     VistaWin32ThreadImp::Suspend()
{
	if(::SuspendThread ( m_oWin32Handle ) != -1)
		m_bIsRunning = false;

	return ( !m_bIsRunning );
}


bool     VistaWin32ThreadImp::Resume()
{
	if(::ResumeThread  ( m_oWin32Handle ) != -1)
		m_bIsRunning = true;
	return ( m_bIsRunning );
}


bool     VistaWin32ThreadImp::Join()
{
	if(WaitForSingleObject(m_oWin32Handle, INFINITE) == WAIT_OBJECT_0)
	{
		m_bIsRunning = false;
		CloseHandle(m_oWin32Handle);
		m_oWin32Handle  = 0;
		m_nThreadId = 0;
	}


	if(m_oWin32Handle)
		return false;

	return true;
}

bool     VistaWin32ThreadImp::Abort()
{
	DWORD wCode=0;
	bool bRet = (TerminateThread(m_oWin32Handle,wCode) ? true : false);
	if(bRet)
	{
		CloseHandle(m_oWin32Handle);
		m_oWin32Handle = 0;
		m_nThreadId = 0;
	}
	return bRet;
}

bool     VistaWin32ThreadImp::SetPriority( const VistaPriority & inPrio)
{
	return (SetThreadPriority(m_oWin32Handle, inPrio.GetSystemPriority()) == TRUE);
}

void VistaWin32ThreadImp::GetPriority( VistaPriority &outPrio ) const
{
	int prio = GetThreadPriority(m_oWin32Handle);
	outPrio.SetVistaPriority(outPrio.GetVistaPriorityForSystemPriority(prio));
}

/**
 * give the processor away temporarily
 */
void VistaWin32ThreadImp::YieldThread   ()
{
	Sleep(0);
}

void VistaWin32ThreadImp::SetCancelAbility(const bool bOkToCancel)
{
}

bool VistaWin32ThreadImp::CanBeCancelled() const
{
	return false;
}

void VistaWin32ThreadImp::PreRun()
{
}

void VistaWin32ThreadImp::PostRun()
{
	if(m_oWin32Handle)
		CloseHandle(m_oWin32Handle);
	m_oWin32Handle = 0;
	m_nThreadId = 0;
}

bool VistaWin32ThreadImp::Equals(const IVistaThreadImp &oImp) const
{
	if(!m_oWin32Handle)
		return false;

	return (m_oWin32Handle == static_cast<const VistaWin32ThreadImp &>(oImp).m_oWin32Handle);
}

bool VistaWin32ThreadImp::SetProcessorAffinity(int iProcessorNum)
{
	m_dwAffinityMask = (1 << iProcessorNum);
	return( SetThreadAffinityMask(m_oWin32Handle, m_dwAffinityMask) != 0 );
}

bool VistaWin32ThreadImp::SetThreadName(const std::string &sName)
{
	// we can not change the name when the tread is running
	if(m_bIsRunning)
		return false;
	return IVistaThreadImp::SetThreadName(sName);
}

long VistaWin32ThreadImp::GetThreadIdentity() const
{
	return (long)m_nThreadId;
}

long VistaWin32ThreadImp::GetCallingThreadIdentity()
{
	return (long)GetCurrentThreadId();
}

bool VistaWin32ThreadImp::SetCallingThreadPriority( const VistaPriority& oPrio )
{
	int nSysPrio = oPrio.GetSystemPriority();
	return( SetThreadPriority( GetCurrentThread(), nSysPrio ) != 0 );
}

bool VistaWin32ThreadImp::GetCallingThreadPriority( VistaPriority& oPrio )
{
	int nSysPrio = GetThreadPriority( GetCurrentThread() );
	if( nSysPrio == THREAD_PRIORITY_ERROR_RETURN )
		return false;
	oPrio.SetVistaPriority( oPrio.GetVistaPriorityForSystemPriority( nSysPrio ) );
	return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif


