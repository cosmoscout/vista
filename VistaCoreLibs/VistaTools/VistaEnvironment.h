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


#if !defined(_VISTAENVIRONMENT_H)
#define _VISTAENVIRONMENT_H



/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

#include <string>
#include <list>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTATOOLSAPI VistaEnvironment
{
	private:
	/**
	 * This class is not meant to be instantiated. Use the static
	 * methods instead.
	 */
	VistaEnvironment();

	/**
	 * This class is not meant to be instantiated, nor destructed.
	 */
	~VistaEnvironment();
		//IMPLEMENTATION
	public:

		/**
		 * Returns the current machine's hostname, as stored in
		 * the environment 'Computername' or 'HOSTNAME'
		 * @return the name of the current machine
		 */
		static std::string GetHostname();

		/**
		 * Returns a string that describes the current OS. This is a
		 * compile-time static text, WIN32, LINUX, HPUX, IRIX are
		 * valid returns here.
		 * @return the name of the OS
		 */
		static std::string GetOSystem();

		/**
		 * Returns a string that indicates the CPU architecture (single or
		 * SMP variants).
		 */
		static std::string GetCPUType();

		/**
		 * Returns a string that defines the number of Processors available
		 * on this machine.
		 * @return a human readable string that gives the number of processors on this machine
		 */
		static std::string GetNumberOfProcessors();


		static std::string GetMemory();

		static std::string GetCompileFlagSYSTEM();

		static std::string GetCompileTime();

		/**
		 * Returns the compile-date for THIS file only!
		 * (VistaEnvironment.cpp)
		 * @return a human readable date string that describes the compile time of this file
		 */
		static std::string GetCompileDate();


		static std::string GetEnv(const std::string &);
		static void SetEnv(const std::string &sKey, const std::string &sValue);

		static std::string GetLibraryPathEnv();
		static void SetLibraryPathEnv( const std::string &sValue );
		static void AddPathToLibraryPathEnv( const std::string &sValue, bool bAddAtBack = true );

		/**
		 * returns the OS-specific separator that separates several directories in an env var
		 */
		static char GetOSEnvironmentPathSeparator();
		/**
		 * Replaces any occurences of the OS-specific path separator in the string
		 */
		static std::string ReplaceOSEnvironemntPathSeparators( const std::string& sPathListEntry,
														const char cReplaceWith );
};

#endif //_VISTAENVIRONMENT_H
