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


#ifndef _VISTADRIVERUTILS_H
#define _VISTADRIVERUTILS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"
#include <string>
#include <VistaTools/VistaDLL.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaDeviceDriver;
class IVistaDriverCreationMethod;
class IVistaTranscoderFactoryFactory;
class VistaDLL;
class VistaDeviceSensor;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a collection of tools, mostly for driver plugins
 */
namespace VddUtil
{
	/**
	 * a helper class to manage driver plugins.
	 */
	class VISTADEVICEDRIVERSAPI VistaDriverPlugin
	{
	public:
		VistaDriverPlugin()
			: m_pMethod(NULL),
			  m_Plugin(NULL),
			  m_TranscoderDll(NULL),
			  m_pTranscoder(NULL)
		{
		}
		IVistaDriverCreationMethod     *m_pMethod; /**< a pointer to the creation methods for drivers of the type */
		IVistaTranscoderFactoryFactory *m_pTranscoder;
		VistaDLL::DLLHANDLE             m_Plugin;  /**< a handle of the DLL that is managing the driver */
		VistaDLL::DLLHANDLE             m_TranscoderDll;
		std::string                     m_strDriverClassName; /**< symbolic name of the type of devices that can be created */
	};

	/**
	 * opens a dll or so and claims a creation method from the shared resource.
	 * this can be used to create a number of devices of the same type.
	 * @param strPathToPlugin the //absolute// path of the dll or so to open
	 * @param pStoreTo a non-NULL pointer to a storage for the DLL info
	 * @return false if
	           - the dll could not be loaded (path?)
	           - the dll does not contain a C symbol "GetDeviceClassName"
	           - the dll does not contain a C symbol "GetCreationMethod"
	           - the dll failed to create a creation method
	   in all these cases, the dll is closed and the value of the members of pStoreTo are undefined.
	 */
	bool VISTADEVICEDRIVERSAPI LoadCreationMethodFromPlugin( const std::string &strPathToPlugin,
		                               VistaDriverPlugin *pStoreTo );

	bool VISTADEVICEDRIVERSAPI LoadTranscoderFromPlugin( const std::string &strPathToPlugin,
			                           VistaDriverPlugin *pStoreTo );

	bool VISTADEVICEDRIVERSAPI DisposeTranscoderFromPlugin( VistaDriverPlugin *plug );

	/**
	 * releases memory from a DLL and associated resources. call this method, when cleaning up,
	 * or when absolutely sure that //NO// resource allocated by the dll or so is still in use.
	 * this can sometimes be non-trivial! The method will call the OnUnload() method of the
	 * creation method, which might unregister some global stuff and lead to crashes.
	 * In that case: report that! and fix it!
	 * @param a pointer to a driver plugin (non-NULL)
	 * @param bDeleteCm indicates whether the creation method is to be deleted
	          note that the plugin will be unloaded, any subsequent call to
	          destroy the creation method will fail, as the memory is no
	          longer part of the application.
	 * @return true
	 */
	bool VISTADEVICEDRIVERSAPI DisposePlugin( VistaDriverPlugin *, bool bDeleteCm = false );


	// #########################################################################################
	// MISC UTIL
	// #########################################################################################

	std::string VISTADEVICEDRIVERSAPI GetTranscoderLibName( const std::string &infix );
	std::string VISTADEVICEDRIVERSAPI GetTranscoderLibInfix();
	std::string VISTADEVICEDRIVERSAPI GetPluginLibName( const std::string &infix );
	std::string VISTADEVICEDRIVERSAPI GetDriverLibName( const std::string &infix );
	std::string VISTADEVICEDRIVERSAPI GetPlugPrefix();
	std::string VISTADEVICEDRIVERSAPI GetPlugInfix();
	std::string VISTADEVICEDRIVERSAPI GetDriverInfix();
	std::string VISTADEVICEDRIVERSAPI GetPlugPostfix();
	
	std::string VISTADEVICEDRIVERSAPI GetDefaultPluginPath();


	bool VISTADEVICEDRIVERSAPI InitVdd();
	bool VISTADEVICEDRIVERSAPI ExitVdd();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERUTILS_H

