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


#ifndef _VISTAPROFILER_H
#define _VISTAPROFILER_H

/*============================================================================*/
/* MACROS AND DEFINES (part one)                                              */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"
#include "VistaToolsStd.h"

#include <VistaBase/VistaUtilityMacros.h>

#include <list>
#include <string>
#include <iostream>
#include <fstream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Read config values from ini files
 * @deprecated Use VistaIniFileParser instead!
 */
class VISTATOOLSAPI VISTA_DEPRECATED VistaProfiler
{
public:
	VistaProfiler();
	/// @todo does this constructor make sense? the profiler gets a file name, 
	/// why should it try to search for "something" on it's own? This doesn't
	/// belong here! First find your file, then give it to the profiler.
	//VistaProfiler(const std::list<std::string> &liSearchPath);

	virtual ~VistaProfiler();
	bool GetTheProfileSections(std::list<std::string> &liStrResult,
							   const std::string &IniFileName);

	int	GetTheProfileSectionEntries(const std::string  &iniSectionName,
									std::list<std::string> &liStrResult,
									const std::string &IniFileName);

	bool GetTheProfileSection(const std::string &IniSectionName,
							  const std::string &IniFileName);

	bool GetTheProfileList(const std::string &IniSectionName,
						   const std::string &EntryName,
						   const std::string &DefaultString,
						   std::list<std::string> & destinationList,
						   const std::string &IniFileName );

	bool GetTheProfileList(const std::string &iniSectionName,
						   const std::string &entryName,
						   std::list<std::string>  & destList,
						   const std::string &iniFileName);

	bool GetTheProfileList(const std::string &iniSectionName,
						   const std::string &entryName,
						   const std::string &DefaultString,
						   std::list<float>  &destinationList,
						   const std::string &iniFileName);

	bool GetTheProfileString(const std::string  &iniSectionName,
							 const std::string  &entryName,
							 const std::string  &defaultString,
								   std::string  &destinationString,
							 const std::string  &iniFileName);

	std::string GetTheProfileString(const std::string &iniSectionName,
									const std::string &sEntryName,
									const std::string &sDefaultString,
									const std::string &sIniFileName);

/**
 * @todo does it make sense to distinguish int/uint/long etc. for an ini file parser?
 */
	int GetTheProfileInt(const std::string &IniSectionName,
						 const std::string &EntryName,
						 const int          DefaultInt,
						 const std::string &IniFileName );

	bool GetTheProfileBool(const std::string &IniSectionName,
						   const std::string &EntryName,
						   const bool         DefaultBool,
						   const std::string &IniFileName );

	long GetTheProfileLong(const std::string &IniSectionName,
						   const std::string &EntryName,
						   const long         DefaultLong,
						   const std::string &IniFileName );

	unsigned int GetTheProfileUInt(const std::string &IniSectionName,
								   const std::string &EntryName,
								   const unsigned int   DefaultUInt,
								   const std::string &IniFileName);

	unsigned long GetTheProfileULong(const std::string &IniSectionName,
									 const std::string &EntryName,
									 const unsigned long  DefaultULong,
									 const std::string &IniFileName);

	float GetTheProfileFloat(const std::string &IniSectionName,
							 const std::string &EntryName,
							 const float          DefaultFloat,
							 const std::string &IniFileName);

	int SetTheProfileString(const std::string &IniSectionName,
							const std::string &EntryName,
							const std::string &ValueString,
							const std::string &IniFileName);

	int SetTheProfileInt(const std::string &IniSectionName,
						 const std::string &EntryName,
						 const int ValueInt,
						 const std::string &IniFileName);

	int SetTheProfileLong(const std::string &IniSectionName,
						  const std::string &EntryName,
						  const long ValueLong,
						  const std::string &IniFileName);

	int SetTheProfileUInt(const std::string &IniSectionName,
						  const std::string &EntryName,
						  const unsigned int ValueUInt,
						  const std::string &IniFileName);

	int SetTheProfileULong(const std::string &IniSectionName,
						   const std::string &EntryName,
						   const unsigned long ValueULong,
						   const std::string &IniFileName);

	int SetTheProfileFloat(const std::string &IniSectionName,
						   const std::string &EntryName,
						   const float ValueFloat,
						   const std::string &IniFileName);

	static std::string ToUpper(const std::string &strInput);
	static std::string ToLower(const std::string &strInput);

	static bool ValueEquals(const std::string &value,
							const std::string &expected,
							bool bCaseSensitive=false);
	/**
	* @param sIniName the postfix(file-name) to search for in ini-path
	* @return the number of hits in ini-search-path
	*/
	/// @todo move to vistasystem
	//int PrintIniInSearchPath(const std::string &sIniName) const;

	bool RemoveEntry(const std::string &strSection,
					 const std::string &strEntry,
					 const std::string &strIniFile);

	void SetEnvironmentVariablesEnabled( bool bState );
	bool GetEnvironmentVariablesEnabled();

	/**
	 * hint: Logging is only for analysis and debugging, to help decide
	 * whether another mechanism should be chosen and to reveal hidden options.
	 * Thus it is (and should not) not settable from the ini file for now.
	 * Works only in DEBUG mode.
	 */
	static void SetIsLoggingEnabled(bool state, const char *fileName = NULL);
	static bool GetIsLoggingEnabled();

private:
	/// @todo move to vistasystem
	//std::ifstream _SearchIniFile(const std::string &sFileName) const;
	std::ifstream   *OpenIniFile(const std::string &sFileName) const;

	/**
	 * @return true stream is positioned right in front of the first line of the section
	 */
	bool RetrieveSection(std::ifstream &rFile, const std::string &sSectionName);
	/**
	 * returns false when string is no
	 */
	bool RetrieveSectionString(std::ifstream &rFile, const std::string &sKeyName, std::string &rStorage);

	void CloseIniFile(std::ifstream *pFile) const;

	// The key and the value are separated by a '='
	// The key is not case sensitive but the value!
	// Leading or following spaces and chars < ascii(32) do not belong
	// to the keys or values!
	bool  ExtractKeyAndValue (char * totalLine, char * chKey, char * chValue);
	bool  RemoveSpaceAndControlChars (char * chString);
	bool  RemoveSpaceAndControlChars (std::string * stlString);

	static bool  TestForIniFileReadAccess(const std::string &sFileName);
	static bool  TestForIniFileWriteAccess(const std::string &sFileName);
	static bool  TestForIniFileReadWriteAccess(const std::string &sFileName);

	//std::list<std::string> m_liSearchPath;

	std::string UseEnviromentVariables( std::string sInput );

	bool m_bEnvironmentVariablesEnabled;

	// logging
	static volatile bool S_bDoLogging;
	static std::string S_strLogFileName;
	std::ofstream	m_logStream;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROFILER_H
