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


#include "VistaDriverUtils.h"

#include "VistaDeviceDriver.h"
#include "DriverAspects/VistaDriverMeasureHistoryAspect.h"

#include <VistaBase/VistaStreamUtils.h>

#include <VistaTools/VistaFileSystemFile.h>
#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaEnvironment.h>

#include <cmath>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
namespace VddUtil
{

	bool LoadCreationMethodFromPlugin( const std::string &strPathToPlugin,
		                               VistaDriverPlugin *pStoreTo  )
	{
		if( pStoreTo->m_pTranscoder == NULL )
			return false;

		// make a test for existence?
		pStoreTo->m_Plugin = VistaDLL::Open( strPathToPlugin );
		if(pStoreTo->m_Plugin)
		{
			VistaDLL::DLLSYMBOL name = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "GetDeviceClassName");
			if(name)
			{
				typedef const char *(*GetNameMethod)();
				typedef IVistaDriverCreationMethod *(*CrMethod)(IVistaTranscoderFactoryFactory *);

				GetNameMethod f = (GetNameMethod)name;
				const char *pcName = f(); // claim name
				pStoreTo->m_strDriverClassName = (pcName ? std::string(pcName) : "");

				VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pStoreTo->m_Plugin, "GetCreationMethod");
				if(sym)
				{
					CrMethod m = (CrMethod)sym;
					pStoreTo->m_pMethod = m(pStoreTo->m_pTranscoder);
					if(pStoreTo->m_pMethod)
					{
						// ok, everything is nice so far,
						return true;
					}
					else
					{
						VistaDLL::Close( pStoreTo->m_Plugin );
						pStoreTo->m_Plugin = NULL;
						return false;
					}
				}
				else
				{
					VistaDLL::Close( pStoreTo->m_Plugin );
					pStoreTo->m_Plugin = NULL;
					return false;
				}
			}
			else
			{
				VistaDLL::Close( pStoreTo->m_Plugin );
				pStoreTo->m_Plugin = NULL;
			}
		}
		else
			vstr::errp() << VistaDLL::GetError() << std::endl;
		return false;
	}


	bool LoadTranscoderFromPlugin( const std::string &strPathToPlugin,
				                           VistaDriverPlugin *pStoreTo )
	{
		// make a test for existence?		
		pStoreTo->m_TranscoderDll = VistaDLL::Open( strPathToPlugin );
		if(pStoreTo->m_TranscoderDll)
		{
			typedef IVistaTranscoderFactoryFactory *(*CrMethod)();
			VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pStoreTo->m_TranscoderDll, "CreateTranscoderFactoryFactory");
			if(sym)
			{
				CrMethod m = (CrMethod)sym;
				pStoreTo->m_pTranscoder = m(); // call m
				if(pStoreTo->m_pTranscoder)
					return true;
				else
				{
					VistaDLL::Close( pStoreTo->m_TranscoderDll );
					pStoreTo->m_TranscoderDll = NULL;
					return false;
				}
			}
			else
			{
				VistaDLL::Close( pStoreTo->m_TranscoderDll );
				pStoreTo->m_TranscoderDll = NULL;
				return false;
			}
		}
		else
			vstr::errp() << VistaDLL::GetError() << std::endl;

		return false;
	}

	bool DisposeTranscoderFromPlugin( VistaDriverPlugin *pPlug )
	{
		if( pPlug->m_Plugin || pPlug->m_pMethod )
			return false;

		if(pPlug->m_TranscoderDll)
		{
			if( pPlug->m_pTranscoder )
			{
				VistaDLL::DLLSYMBOL sym = VistaDLL::FindSymbol(pPlug->m_TranscoderDll, "OnUnloadTranscoderFactoryFactory");
				if(sym)
				{
					typedef bool (*UnloadM)(IVistaTranscoderFactoryFactory*);
					UnloadM unload = (UnloadM)sym;
					unload(pPlug->m_pTranscoder);
				}
				else
					IVistaReferenceCountable::refdown(pPlug->m_pTranscoder);
			}
			pPlug->m_pTranscoder = NULL;

			VistaDLL::Close( pPlug->m_TranscoderDll );
			pPlug->m_TranscoderDll = NULL;
		}

		return true;
	}

	bool DisposePlugin( VistaDriverPlugin *pPlug, bool bDeleteCm )
	{
		if( pPlug->m_pMethod )
		{
			VistaDLL::DLLSYMBOL name = VistaDLL::FindSymbol(pPlug->m_Plugin, "UnloadCreationMethod");
			if(name)
			{
				// ok, we have an unload method. lets query and cast it!
				typedef void (*UnloadMt)(IVistaDriverCreationMethod*);
				UnloadMt unloadFunc = (UnloadMt)name;

				unloadFunc(pPlug->m_pMethod); // call

				pPlug->m_pMethod = NULL;
			}
			else
			{
				if(bDeleteCm)
					IVistaReferenceCountable::refdown(pPlug->m_pMethod);

				pPlug->m_pMethod = NULL;
			}
		}

		if(pPlug->m_Plugin)
		{
			VistaDLL::Close( pPlug->m_Plugin ); // hopefully, the OS will take care for
			                                     // not *really* releasing resources
												 // iff we opened the SO more than once
			pPlug->m_Plugin = NULL;
		}

		DisposeTranscoderFromPlugin( pPlug );
		pPlug->m_strDriverClassName = std::string(); // clear name

		return true;
	}

	// ######################################################################################################


	bool InitVdd()
	{
		return true;
	}

	bool ExitVdd()
	{
		return true;
	}

	std::string GetTranscoderLibName( const std::string &infix )
	{
		return GetPlugPrefix() + infix + GetTranscoderLibInfix() + GetPlugPostfix();
	}

	std::string GetTranscoderLibInfix()
	{
		return "Transcoder";
	}

	std::string GetPlugInfix()
	{
		return "Plugin";
	}

	std::string GetDriverInfix()
	{	
		return "Driver";
	}

	std::string GetPluginLibName( const std::string &infix )
	{
		return GetPlugPrefix() + infix + GetPlugInfix() + GetPlugPostfix();
	}

	std::string GetDriverLibName( const std::string &infix )
	{
		return GetPlugPrefix() + infix + GetDriverInfix() + GetPlugPostfix();
	}

	std::string GetPlugPrefix()
	{
#if defined(WIN32)
		return "Vista";
#else
		return "libVista";
#endif
	}

	std::string GetPlugPostfix()
	{
#if defined(DEBUG)
	#if defined(WIN32)
			return "D.dll";
	#else
			return "D.so";
	#endif
#else
	#if defined(WIN32)
			return ".dll";
	#else
			return ".so";
	#endif
#endif
	}

	std::string GetDefaultPluginPath()
	{

		std::string strPlugPath = VistaEnvironment::GetEnv("DRIVERPLUGINDIRS");
		if (strPlugPath.empty())
			strPlugPath = VistaEnvironment::GetEnv("VISTA_ROOT")
				+ VistaFileSystemDirectory::GetOSSpecificSeparator()
				+ std::string("lib")
				+ VistaFileSystemDirectory::GetOSSpecificSeparator()
				+ std::string(VistaEnvironment::GetOSystem());
		return strPlugPath;
	}

}

