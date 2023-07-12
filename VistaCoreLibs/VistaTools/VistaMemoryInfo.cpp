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

#include "VistaMemoryInfo.h"

#include <VistaBase/VistaExceptionBase.h>

#include <cstdio>
#include <cstdlib>

#if defined(WIN32)
#include <Psapi.h>
#include <Windows.h>
#pragma comment(lib, "Psapi.lib")
#else
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMemoryInfo::VistaMemoryInfo() {
}

VistaMemoryInfo::~VistaMemoryInfo() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
long VistaMemoryInfo::GetWorkingSet() {
#ifdef WIN32
  {
    HANDLE                  hMe = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS oProcData;
    BOOL                    rv = GetProcessMemoryInfo(hMe, &oProcData, sizeof oProcData);
    if (rv == TRUE) {
      return (long)oProcData.WorkingSetSize;
    }
  }
#elif defined(LINUX)
  {
    /**
     *  - read /proc/getpid()/stat
     *  - tokenize the output
     *  - entry 23 (vsize) is what we are looking for here
     *  -> as process names containing numbers, spaces and braces may
     *     result in different numbers of tokens, we count from back...
     */

    long mem = -1;

    // read stat from /proc interface
    char filename[256];
    sprintf(filename, "/proc/%d/stat", getpid());
    FILE* file = fopen(filename, "r");
    char  linebuffer[1024];
    int   nReadBytes = fread(linebuffer, 1, 1024, file);
    fclose(file);

    // tokenize
    std::vector<std::string> tokens(1);
    std::string              cur;
    int                      idx = 0;
    while (idx < nReadBytes) {
      char c = linebuffer[idx++];
      if (isspace(c)) {
        // token finished
        tokens.push_back(cur);
        cur.clear();
        continue;
      }
      cur.push_back(c);
    }

    //		// debug output
    //		for(int i = 0; i < tokens.size(); ++i)
    //		{
    //			std::cout << i << ": " << tokens[i] << std::endl;
    //		}
    //
    //		std::cout << "vsize: " << tokens[tokens.size()-20] << std::endl;

    // get the token containing vsize and make a number out of it
    mem = strtol(tokens[tokens.size() - 20].c_str(), NULL, 10);
    return mem;
  }
#else
  vtoolserr << "[VistaMemoryInfo] GetWorkingSet() not implemented for this platform!" << std::endl;
#endif

  // error
  return -1;
}

long VistaMemoryInfo::GetPeakWorkingSet() {
#ifdef WIN32
  {
    HANDLE                  hMe = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS oProcData;
    BOOL                    rv = GetProcessMemoryInfo(hMe, &oProcData, sizeof oProcData);
    if (rv == TRUE) {
      return (long)oProcData.PeakWorkingSetSize;
    }
  }
#else
  VISTA_THROW_NOT_IMPLEMENTED
#endif

  // error
  return -1;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
