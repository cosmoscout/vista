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

//We have different implementations for Windows and Unix. Thus:

#include "VistaStackWalker.h"

#ifdef WIN32
	#include <tchar.h>
	#include <windows.h>
	#include <Tlhelp32.h>
	#include <Psapi.h>
	#include <DbgHelp.h>
	#include <Dbgeng.h>
#else
	#include <signal.h>
	#include <execinfo.h>
	#include <cxxabi.h>
	#include <dlfcn.h>
	#include <stdlib.h>
	#include <string>
	#include <sys/syscall.h>
#endif


#include <sstream>
#include <stdio.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
const static int VISTA_STACKWALK_MAX_NAMELENGTH = 2048;

#ifdef WIN32
// WINDOWS IMPLEMENTATION
namespace
{
	typedef struct internalcsentry
	{
		DWORD64 offset;
		CHAR name[VISTA_STACKWALK_MAX_NAMELENGTH];
		CHAR undName[VISTA_STACKWALK_MAX_NAMELENGTH];
		CHAR undFullName[VISTA_STACKWALK_MAX_NAMELENGTH];
		DWORD64 offsetFromSmybol;
		DWORD offsetFromLine;
		DWORD lineNumber;
		CHAR lineFileName[VISTA_STACKWALK_MAX_NAMELENGTH];
		DWORD symType;
		LPCSTR symTypeString;
		CHAR moduleName[VISTA_STACKWALK_MAX_NAMELENGTH];
		DWORD64 baseOfImage;
		CHAR loadedImageName[VISTA_STACKWALK_MAX_NAMELENGTH];
	} InternalStackEntry;

	bool WINAPI GetCallStackEntry( DWORD64 nAddrPCOffset, PSYMBOL_INFO pSym,
									VistaStackWalker::CallStackEntry& oEntry )
	{
		InternalStackEntry oCallStackEntry;

		IMAGEHLP_LINE64 f_64Line;
		IMAGEHLP_MODULE64 f_64Module;

		memset(&f_64Line, 0, sizeof(f_64Line));
		f_64Line.SizeOfStruct = sizeof(f_64Line);
		memset(&f_64Module, 0, sizeof(f_64Module));
		f_64Module.SizeOfStruct = sizeof(f_64Module);

		//Getting CallstackEntry
		oCallStackEntry.offset = nAddrPCOffset;
		oCallStackEntry.name[0] = 0;
		oCallStackEntry.undName[0] = 0;
		oCallStackEntry.undFullName[0] = 0;
		oCallStackEntry.offsetFromSmybol = 0;
		oCallStackEntry.offsetFromLine = 0;
		oCallStackEntry.lineFileName[0] = 0;
		oCallStackEntry.lineNumber = 0;
		oCallStackEntry.loadedImageName[0] = 0;
		oCallStackEntry.moduleName[0] = 0;

		if (SymFromAddr(GetCurrentProcess(), nAddrPCOffset, &(oCallStackEntry.offsetFromSmybol), pSym) != false)
		{
			strcpy_s(oCallStackEntry.name, pSym->Name);
			UnDecorateSymbolName( pSym->Name, oCallStackEntry.undName, VISTA_STACKWALK_MAX_NAMELENGTH, UNDNAME_NAME_ONLY );
			UnDecorateSymbolName( pSym->Name, oCallStackEntry.undFullName, VISTA_STACKWALK_MAX_NAMELENGTH, UNDNAME_COMPLETE );
		}

		if (SymGetLineFromAddr64 != NULL )
		{
			if (SymGetLineFromAddr64(GetCurrentProcess(), nAddrPCOffset, &(oCallStackEntry.offsetFromLine), &f_64Line) != false)
			{
				oCallStackEntry.lineNumber = f_64Line.LineNumber;
				strcpy(oCallStackEntry.lineFileName, f_64Line.FileName);
			}
		}
						
		
		if ((bool)(SymGetModuleInfo64(GetCurrentProcess(),  nAddrPCOffset, &f_64Module) != false))
		{
		strcpy(oCallStackEntry.moduleName, f_64Module.ModuleName);
		oCallStackEntry.baseOfImage = f_64Module.BaseOfImage;
		strcpy(oCallStackEntry.loadedImageName, f_64Module.LoadedImageName);
		}

		//Actually writes information to output!
		if (oCallStackEntry.offset != 0)
		{
			if (oCallStackEntry.name[0] == 0)
				strcpy(oCallStackEntry.name, "(function-name not available)");

			if (oCallStackEntry.lineFileName[0] == 0)
			{
				strcpy(oCallStackEntry.lineFileName, "(filename not available)");
				oCallStackEntry.lineNumber=0;

				if (oCallStackEntry.moduleName[0] == 0)
					strcpy(oCallStackEntry.moduleName, "(module-name not available)");
			}

			oEntry.m_sFilename.assign(oCallStackEntry.lineFileName);
			oEntry.m_sFunctionName.assign(oCallStackEntry.name);
			oEntry.m_iLineNumber = (int) oCallStackEntry.lineNumber;
		}
		else
		{
			oEntry.m_sFilename = "<unknown>";
			oEntry.m_sFunctionName = "<unknown>";
			oEntry.m_iLineNumber = ~0;
		}

		//Reached last StackEntry
		if ( nAddrPCOffset == 0 )
		{
			SetLastError(ERROR_SUCCESS);
			return false;
		}

		return true;
	}

	bool WINAPI Initialize( const SIZE_T TTBUFLEN = 8096 )
	{
		SymCleanup(0);
		HMODULE f_hDbgHelp=0;

		//Load dbgHelp
		if (f_hDbgHelp == 0)
		{
			f_hDbgHelp = LoadLibrary( _T("dbghelp.dll") );
		}
		
		if (f_hDbgHelp == 0)
		{
			return false;
		}

		SymInitialize(GetCurrentProcess(), NULL, false);
		SymSetOptions(0);


		//dbghelp.dll init successfull :) - proceed with LoadModules!

		HINSTANCE f_hPSAPDll;

		//Needed for finding Modules
		DWORD f_ModuleHandlesNeeded;
		MODULEINFO f_miModuleInfo;
		HMODULE *f_phModuleHandles = 0;
		char *f_pModuleFileName = 0;
		char *f_pModuleName = 0;

		//Check if Library can be found! (_T for unicode/ansi conversion
		f_hPSAPDll = LoadLibrary( _T("Psapi.dll") );
		if (f_hPSAPDll == 0)
			return false;

		//Enforce a maximum length. This can be changed by user!
		f_phModuleHandles = (HMODULE*) malloc(sizeof(HMODULE) * (TTBUFLEN / sizeof HMODULE));
		f_pModuleFileName = (char*) malloc(sizeof(char) * TTBUFLEN);
		f_pModuleName = (char*) malloc(sizeof(char) * TTBUFLEN);

		//Either we have a problem with the PSAPIdll or we need more Modules than space is there. Anyway: --> EXIT
		if ((!EnumProcessModules( GetCurrentProcess(), f_phModuleHandles, (DWORD)TTBUFLEN, &f_ModuleHandlesNeeded ) ) | (f_ModuleHandlesNeeded > TTBUFLEN))
		{
			if (f_hPSAPDll)
				FreeLibrary(f_hPSAPDll);
			free(f_pModuleName);
			free(f_pModuleFileName);
			free(f_phModuleHandles);

			return false;
		}

		//Fetch ModuleInfos for each needed Module!
		unsigned long i;
		for ( i = 0; i < f_ModuleHandlesNeeded / sizeof f_phModuleHandles[0]; i++ )
		{
			// base address, size
			GetModuleInformation(GetCurrentProcess(), f_phModuleHandles[i], &f_miModuleInfo, sizeof f_miModuleInfo );
			// image file name
			f_pModuleFileName[0] = 0;
			GetModuleFileNameExA(GetCurrentProcess(), f_phModuleHandles[i], f_pModuleFileName, (DWORD)TTBUFLEN );
			// module name
			f_pModuleName[0] = 0;
			GetModuleBaseNameA(GetCurrentProcess(), f_phModuleHandles[i], f_pModuleName, (DWORD)TTBUFLEN );

			char *j = _strdup(f_pModuleFileName);
			char *m = _strdup(f_pModuleName);
			DWORD result = ERROR_SUCCESS;
			if (SymLoadModule64(GetCurrentProcess(), 0, j, m, (DWORD64) f_miModuleInfo.lpBaseOfDll,  f_miModuleInfo.SizeOfImage) == 0)
			{
				result = GetLastError();
			}

			delete(j);
			delete(m);
		}

		if (f_hPSAPDll)
			FreeLibrary(f_hPSAPDll);
		free(f_pModuleName);
		free(f_pModuleFileName);
		free(f_phModuleHandles);

		if (i != 0)
		{
			return true;
		}

		return false;
	}	


}; // anonymous namespace

bool VistaStackWalker::GetCallstack( std::vector<VistaStackWalker::CallStackEntry>& vecCallstack )
{
	if( !Initialize() )
	{
		SetLastError(ERROR_DLL_INIT_FAILED);
		return false;
	}

	//Create the CONTEXT for the StackWalk
	CONTEXT f_Context;
	memset(&f_Context, 0, sizeof(CONTEXT));
	f_Context.ContextFlags = CONTEXT_FULL;
	//ATTENTION: NOT AVAILABLE ON OLD OS!
	RtlCaptureContext(&f_Context);

	//Creating the StackFrame64 for the data
	STACKFRAME64 f_StackFrame;
	memset(&f_StackFrame, 0, sizeof(f_StackFrame));
	DWORD imageType;

	#ifdef _M_IX86
		imageType = IMAGE_FILE_MACHINE_I386;
		f_StackFrame.AddrPC.Offset = f_Context.Eip;
		f_StackFrame.AddrPC.Mode = AddrModeFlat;
		f_StackFrame.AddrFrame.Offset = f_Context.Ebp;
		f_StackFrame.AddrFrame.Mode = AddrModeFlat;
		f_StackFrame.AddrStack.Offset = f_Context.Esp;
		f_StackFrame.AddrStack.Mode = AddrModeFlat;
	#elif _M_X64
		imageType = IMAGE_FILE_MACHINE_AMD64;
		f_StackFrame.AddrPC.Offset = f_Context.Rip;
		f_StackFrame.AddrPC.Mode = AddrModeFlat;
		f_StackFrame.AddrFrame.Offset = f_Context.Rsp;
		f_StackFrame.AddrFrame.Mode = AddrModeFlat;
		f_StackFrame.AddrStack.Offset = f_Context.Rsp;
		f_StackFrame.AddrStack.Mode = AddrModeFlat;
	#elif _M_IA64
		imageType = IMAGE_FILE_MACHINE_IA64;
		f_StackFrame.AddrPC.Offset = f_Context.StIIP;
		f_StackFrame.AddrPC.Mode = AddrModeFlat;
		f_StackFrame.AddrFrame.Offset = f_Context.IntSp;
		f_StackFrame.AddrFrame.Mode = AddrModeFlat;
		f_StackFrame.AddrBStore.Offset = f_Context.RsBSP;
		f_StackFrame.AddrBStore.Mode = AddrModeFlat;
		f_StackFrame.AddrStack.Offset = f_Context.IntSp;
		f_StackFrame.AddrStack.Mode = AddrModeFlat;
	#else
		#error "Platform not supported!"
	#endif



	//Creating SymbolInfo
	PSYMBOL_INFO pSym = (SYMBOL_INFO *) malloc(sizeof(SYMBOL_INFO) + VISTA_STACKWALK_MAX_NAMELENGTH);
	memset(pSym, 0, sizeof(SYMBOL_INFO) + VISTA_STACKWALK_MAX_NAMELENGTH);
	pSym->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSym->MaxNameLen = VISTA_STACKWALK_MAX_NAMELENGTH;


	//Start the StackWalk
	VistaStackWalker::CallStackEntry oEntry;

	while( StackWalk64( imageType, GetCurrentProcess(), GetCurrentThread(),
						&f_StackFrame, &f_Context, NULL , 
						SymFunctionTableAccess64, SymGetModuleBase64, NULL ) )
	{
		//Print the Current Stack Entry in the Stream
		if ( !GetCallStackEntry( f_StackFrame.AddrPC.Offset, pSym, oEntry ) )
		{
			break;
		}
		vecCallstack.push_back( oEntry );
	}

	if(pSym)
	{
		free( pSym );
	}

	return true;
}
#else 
// Unix implementation

//	Attention: needs to be compiled with flags -ldl -g -rdynamic!

bool VistaStackWalker::GetCallstack( std::vector<VistaStackWalker::CallStackEntry>& vecCallstack )
{


	//If you don't get the entire callstack shown, increase this value!
	enum
	{
		MAX_DEPTH = 25
	};

	void *f_pTraceDepth[MAX_DEPTH];
	Dl_info f_DlInfo;
	int iDemangleStatus;
	const char *p_cSymbolName;
	char *p_cDemangledName;

	//Does the actual StackWalk and hands the actual size back!
	int iStackDepth = backtrace(f_pTraceDepth, MAX_DEPTH);

	for (int i=1; i<iStackDepth; ++i)
	{
		//Looking for shared object containing the given address. If not found, proceed!
		if(!dladdr(f_pTraceDepth[i], &f_DlInfo))
			continue;

		//Getting SymbolNames
		p_cSymbolName = f_DlInfo.dli_sname;
		p_cDemangledName = abi::__cxa_demangle(p_cSymbolName, NULL, 0, &iDemangleStatus);

		if(iDemangleStatus == 0)// && &p_cDemangledName)
		{
			p_cSymbolName = p_cDemangledName;
		}
		else
		{	
			//Not in use at the moment!
			if (0 && !p_cDemangledName)
			{
				char fna [] = "(function-name not available)";
				p_cSymbolName = &(fna[0]);
			}
		} 

		//There's still a valid address. If 0 is given back - finish!
		if(!f_DlInfo.dli_saddr)
		{
			continue;
		}

		//Getting Filenames
		FILE *F_OutAddr2line;
		char cCommandLine[VISTA_STACKWALK_MAX_NAMELENGTH];
		char cFileNameLine[VISTA_STACKWALK_MAX_NAMELENGTH];
		
#ifdef DARWIN
		//gaddr2line is available in binutils through fink or mac ports. However it's quite unstable and most probably won't work!
		sprintf(cCommandLine,"gaddr2line -C -e %s -f -i %p", f_DlInfo.dli_fname,f_DlInfo.dli_saddr);
#else //LINUX
		sprintf(cCommandLine,"addr2line -C -e %s -f -i %p", f_DlInfo.dli_fname,f_DlInfo.dli_saddr);
#endif

		F_OutAddr2line = popen(cCommandLine,"r");

		while(fgets(cFileNameLine, VISTA_STACKWALK_MAX_NAMELENGTH, F_OutAddr2line) != NULL)
		{}

		pclose(F_OutAddr2line);

		CallStackEntry fCSEntry;
		fCSEntry.m_sFilename = "";
		fCSEntry.m_iLineNumber = 0;
		fCSEntry.m_sFunctionName = "";

		if ((cFileNameLine[0]=='?') && (cFileNameLine[1]=='?'))
		{
			fCSEntry.m_sFilename="(filename not available)";
			fCSEntry.m_iLineNumber=0;
			
		}
		else
		{
			//Extract Filename and LineNqumber from String.
			long int iDelimeter;
			iDelimeter = 0;
			while( ( cFileNameLine[iDelimeter] != ':' ) 
				&& ( iDelimeter < VISTA_STACKWALK_MAX_NAMELENGTH - 1 ) )
			{
				++iDelimeter;
			}
		
			fCSEntry.m_sFilename.assign(cFileNameLine);
			fCSEntry.m_sFilename.assign(cFileNameLine,iDelimeter);
			
			long int iEnd;
			iEnd = iDelimeter+1;
			while((cFileNameLine[iEnd]!='\0') && (iEnd < VISTA_STACKWALK_MAX_NAMELENGTH-1))
			{
				++iEnd;
			}

			std::string sTemp;
			sTemp.assign(cFileNameLine,iDelimeter+1,iEnd-iDelimeter-2);

			fCSEntry.m_iLineNumber = atoi(sTemp.c_str());

		}

		//p_cSymbolName only contains the Function-Name.
		fCSEntry.m_sFunctionName.assign(const_cast<char*>(p_cSymbolName));
		//Save CallStackEntry in vector.
		vecCallstack.push_back(fCSEntry);

		if (p_cDemangledName)
			{
				free(p_cDemangledName);
			}
	}

	return true;
}
#endif


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaStackWalker::PrintCallStackEntry( const CallStackEntry& oEntry, std::ostream& oOut )
{
	oOut << oEntry.m_sFilename << ":" 
			<< oEntry.m_iLineNumber << " : " 
			<< oEntry.m_sFunctionName << std::endl;
	return true;
}

bool VistaStackWalker::PrintCallStack( std::ostream& oOut )
{
	CallStack oCallStack;
	if( GetCallstack( oCallStack ) == false )
		return false;
	return PrintCallStack( oCallStack, oOut );	
}

bool VistaStackWalker::PrintCallStack( const CallStack& vecCallStack, std::ostream& oOut )
{
	//Modify CallStackEntryVecotr so that there are no leading unknown functions!
	//@Einstein: It might be possible to delete any empty entries (also in the stack!) - and tell the user how many entries got deleted!

	oOut << "##### STACK TRACE #####\n";

	CallStackEntry oEmptyEntry;
	oEmptyEntry.m_sFilename = "(filename not available)";
	oEmptyEntry.m_sFunctionName = "(function-name not available)";
	oEmptyEntry.m_iLineNumber = 0;

	// remove empty entries at beginning
	CallStack::const_reverse_iterator itEraseOffset = vecCallStack.rbegin();
	for( ; itEraseOffset != vecCallStack.rend(); ++itEraseOffset )
	{
		if( (*itEraseOffset).m_sFilename != "(filename not available)"
			|| (*itEraseOffset).m_sFunctionName != "(function-name not available)" )
		{
			break;
		}
	}

	CallStack::const_iterator itLastEntry = itEraseOffset.base();


	if( itLastEntry == vecCallStack.begin() )
	{
		oOut << "(No Entries Available!)" << std::endl;
	}
	else
	{
		for( CallStack::const_iterator itEntry = vecCallStack.begin();
				itEntry != itLastEntry; ++itEntry )			
		{
			PrintCallStackEntry( (*itEntry), oOut );
		}
	}

	oOut << "#######################" << std::endl;

	return true;
}
