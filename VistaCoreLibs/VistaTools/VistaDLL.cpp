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


#include "VistaDLL.h" 
#include <string>
#include <cassert>
using std::string;

#if defined(WIN32)
#include <Windows.h>
#elif defined(HPUX)

#else
#include <dlfcn.h>              // POSIX
#endif
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaDLL::DLLHANDLE VistaDLL::Open(const string &strDllName)
{
	if(!strDllName.empty())
	{
#ifndef WIN32
		return dlopen(strDllName.c_str(),RTLD_NOW|RTLD_GLOBAL);
#else
		// WIN32
		return LoadLibraryExA(strDllName.c_str(),NULL,LOAD_WITH_ALTERED_SEARCH_PATH);
#endif
	}
	return NULL;
}

bool VistaDLL::Close(DLLHANDLE hDllHandle)
{
	if(hDllHandle)
	{

#if !defined(WIN32)
		dlclose(hDllHandle);
		return true;
#else                   // WIN32
		FreeLibrary((HMODULE)hDllHandle);
		return true;
#endif
	}
	return false;
}

VistaDLL::DLLSYMBOL VistaDLL::FindSymbol( DLLHANDLE hDll, const string &strSymbol )
{
	if(hDll && !strSymbol.empty())
	{
#ifndef WIN32
		return dlsym(hDll,strSymbol.c_str());
#else                   // WIN32
		return (VistaDLL::DLLSYMBOL)GetProcAddress((HMODULE)hDll,strSymbol.c_str());
#endif
	}
	return NULL;
}

string VistaDLL::GetError()
{
#ifndef WIN32
	const char *err = dlerror();
	return string(err ? err : "no error");
#else                   // WIN32
	DWORD dw=GetLastError();
	char buffer[512];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,dw,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR)buffer,sizeof(buffer),NULL);
	return string(buffer);
#endif
}

#if 0
string VistaDLL::GetFilePath(VistaDLL::DLLHANDLE hDllHandle)
{
#ifndef WIN32
  /**
   *  @todo  implement...if possible.
   * okay... as there is nothing like GetModuleFileName on linux (yet?)
   * ...we can't do much about it... the only "solution" may be to parse
   * some /proc... stuff. But these may be different on different
   * distributions!?
   * hint: there is a trick for delay-loaded libs, to get the module:
   *       get a known symbol of that library with dlsym, then pass
   *       this handle to dladdr which returns the file....but how to do
   *       this for ANY module you don't know any functions from. I think
   *       this ist not possible without getting dependencies to the
   *       linux-kernel-headers. However, there's a library called
   *       "BinReloc2" that shall do the trick.
   * VISTA_THROW("VistaDLL::GetFilePath() not implemented for this platform!", 0x2bad);
   * throw NotImplementedException("VistaDLL::GetFilePath()");
   **/
	
assert(0 && "NOT IMPLEMENTED ON THIS PLATFORM!");
#else
	char buffer[1024];
	DWORD retval = GetModuleFileName((HMODULE)hDllHandle, buffer, 1024);
	if(retval == 0 || retval > 1024)
	{
		DWORD dw=GetLastError();
		char errBuffer[512];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,dw,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR)errBuffer,sizeof(errBuffer),NULL);
		throw string(errBuffer);
	}
	else return string(buffer);

#endif
}

#endif

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

