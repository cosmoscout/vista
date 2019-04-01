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

#include <string>
using namespace std;

#include <VistaInterProcComm/Concurrency/VistaIpcThreadModel.h>


#if defined(VISTA_THREADING_WIN32)
#include <Windows.h>
#include "VistaWin32ProcessImp.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaWin32ProcessImp::VistaWin32ProcessImp()
{
	win32Handle = 0;
}

VistaWin32ProcessImp::~VistaWin32ProcessImp()
{
	//Join ();
}

/// start a process
bool     VistaWin32ProcessImp::Run( const string & inCommand )
{
	if(win32Handle>0)
		return false;

	STARTUPINFO          si;
	PROCESS_INFORMATION  pi;

	// initialize windows memory structures
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	// start the child process
	if ( CreateProcess (NULL,             // NULL: No module name (use command line)
						(char*)inCommand.c_str (), // Command line
						NULL,             // Process handle not inheritable
						NULL,             // Thread handle not inheritable
						FALSE,            // Set handle inheritance to FALSE
						0,                // No creation flags
						NULL,             // Use parent's environment block
						NULL,             // NULL: Use parent's starting directory
						&si,              // Pointer to STARTUPINFO structure
						&pi ) )           // Pointer to PROCESS_INFORMATION structure
	{
		win32Handle = pi.hProcess;
		return true;
	}

	return false;
}


bool     VistaWin32ProcessImp::Suspend()
{
	if (win32Handle <= 0)
		return false;

		SuspendThread(win32Handle);

	return true;
}


bool     VistaWin32ProcessImp::Resume()
{
	if (win32Handle <= 0)
		return false;

		ResumeThread(win32Handle);

	return true;
}


bool     VistaWin32ProcessImp::Join()
{
	if (win32Handle <= 0)
		return false;

	// Wait until child process exits.
	unsigned long retCode = 0;
	retCode = WaitForSingleObject( win32Handle, INFINITE );
	if (retCode != 0xFFFFFFFF)
	{
		unsigned long exitCode  = 0;
		/* pick up an exit code for the process */
		GetExitCodeProcess (win32Handle, &exitCode);
	}
	CleanupProcess();
	return true;
}

bool     VistaWin32ProcessImp::Abort()
{
//    unsigned long exitCode  = 0;
	if (win32Handle <= 0)
		return false;

	if (TerminateProcess(win32Handle,0))
		CleanupProcess();
	else return Join();

	return true;
}

bool     VistaWin32ProcessImp::SetPriority( const VistaPriority & )
{
	if (win32Handle <= 0)
		return false;

	SetThreadPriority(win32Handle,THREAD_PRIORITY_IDLE);
	return true;
}

void VistaWin32ProcessImp::GetPriority( VistaPriority & ) const
{
	int GetPriority;
	GetPriority = GetThreadPriority(win32Handle);

}


void VistaWin32ProcessImp::CleanupProcess()
{
	if(win32Handle<=0)
		return;
	// Close process and thread handles. 
	CloseHandle( win32Handle );
	win32Handle = 0;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif


