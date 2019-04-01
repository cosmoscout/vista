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

#include <VistaBase/VistaStreamUtils.h>

#if defined(VISTA_THREADING_POSIX) || defined(VISTA_THREADING_SPROC)

#include "VistaPosixProcessImp.h"
#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <vector>
#include <cstring>
#include <cstdio>

using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPosixProcessImp::VistaPosixProcessImp()
{
	unixPID =0;
}

VistaPosixProcessImp::~VistaPosixProcessImp()
{
	//Join ();
}

/// start a process

bool     VistaPosixProcessImp::Run(const string &inCommand)
{
	if(unixPID>0)
		return false;

	// since unix doesn't provide means to spawn a parallel process,
	// we have to fork first and then replace the new fork with the command
	unixPID = fork ();
	// let the callee return
	if ( unixPID != 0 )
		   return ( unixPID != -1 );

	unixPID = getpid();

	//vstr::outi() << "getpid process ID : " << unixPID << std::endl;

	vector<char *> argv;
	char *pch;
	char *tempStr;
	tempStr = new char[inCommand.length()+1];
	if(tempStr == NULL)
	{
		vstr::errp() << "Memoryfail..." << std::endl;
	   return false;
	}
	strcpy(tempStr, inCommand.c_str());
	pch = strtok ( tempStr ," ");
	while (pch != NULL)
	{
		argv.push_back(pch);
		vstr::outi() << "found argument [" << pch << "]" << std::endl;
		pch = strtok (NULL, " ");
	}
		argv.push_back((char *)0);

		vstr::outi() << "calling [" << argv[0] << "] with "
					<< argv.size() << " arguments" << std::endl;
		  
	if(execvp (argv[0], &argv[0]) == -1) //execlp (inCommand.c_str(),  inCommand.c_str(), "-xa", (char *)0)
	{
		int ierr = errno;
		vstr::errp() << "error = " << ierr << std::endl;
		switch(ierr)
		{
		  case EACCES:
		  {
			  vstr::errp() << "EACCES" << std::endl;
			break;
		  }
		  case ENOEXEC:
		  {
			vstr::errp() << "ENOEXEC" << std::endl;
			break;
		  }
		  case ENOENT:
		  {
			vstr::errp() << "No such file or directory" << std::endl;
			break;
		  }
		}
		vstr::errp() << "execl failed to execute... " << std::endl;
	}

  delete [] tempStr;
  return true;
}


bool     VistaPosixProcessImp::Suspend()
{
	return ( kill ( unixPID, SIGSTOP ) == 0 );
}


bool     VistaPosixProcessImp::Resume()
{
	return ( kill ( unixPID, SIGCONT ) == 0 );
}


bool     VistaPosixProcessImp::Join()
{
	// do we have to wait?
	if ( unixPID == 0 )
		return false;

	// wait for process to be finished, killed, etc.
	bool yeah = TEMP_FAILURE_RETRY( waitpid ( unixPID, NULL, 0 ) == unixPID );

	// clear pid
	unixPID = 0;
	return yeah;
}

bool     VistaPosixProcessImp::Abort()
{
	if(unixPID == 0)
		return true;

	bool yeah = ( kill ( unixPID , SIGKILL ) == 0 );
	//cout << "Kill process ID : " << unixPID << std::endl;
	if(yeah)
		CleanupProcess();
	return yeah;
}

bool     VistaPosixProcessImp::SetPriority( const VistaPriority & )
{
	return false;
}

void VistaPosixProcessImp::GetPriority( VistaPriority & ) const
{
	return;
}


void VistaPosixProcessImp::CleanupProcess()
{
	unixPID = 0;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif


