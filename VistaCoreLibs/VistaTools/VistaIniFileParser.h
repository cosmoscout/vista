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


#ifndef _VISTAINIFILEPARSER_H
#define _VISTAINIFILEPARSER_H

/*============================================================================*/
/* MACROS AND DEFINES (part one)                                              */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaToolsConfig.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaVectorMath.h>

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
class VISTATOOLSAPI VistaIniFileParser
{
public:
	VistaIniFileParser( const bool bReplaceEnvironmentVariables = false,
						const std::string& sFileVariableSectionName = "FILE_VARIABLES",
						const bool bCaseSensitiveKeys = false );	
	VistaIniFileParser( const std::string& sFilename,
						const bool bReplaceEnvironmentVariables = false,
						const std::string& sFileVariableSectionName = "FILE_VARIABLES",
						const bool bCaseSensitiveKeys = false );
	VistaIniFileParser( const std::string& sFilename,
						std::list<std::string>& liFileSearchPathes,
						const bool bReplaceEnvironmentVariables = false,
						const std::string& sFileVariableSectionName = "FILE_VARIABLES",
						const bool bCaseSensitiveKeys = false );

	virtual ~VistaIniFileParser();

	void SetSpecialCharacters( const char cSectionHeaderStart,
							   const char cSectionHeaderEnd,
							   const char cKeyAssignment,
							   const char cComment );
	void GetSpecialCharacters( char& cSectionHeaderStart,
							   char& cSectionHeaderEnd,
							   char& cKeyAssignment,
							   char& cComment );

	void SetReplaceEnvironmentVariables( const bool bSet );
	bool GetReplaceEnvironmentVariables() const;

	std::string GetFileVariableSectionName() const;
	void SetFileVariableSectionName( const std::string& oValue );

	void SetUseCaseSensitiveKeys( const bool bSet );
	bool GetUseCaseSensitiveKeys() const;

	bool ReadFile( const std::string& sFilename );
	bool ReadFile( const std::string& sFilename,		
				   std::list<std::string>& liFileSearchPathes );
	bool WriteFile();
	bool WriteFile( const std::string& sFilename,
					const bool bOverwriteExistingFile = false );

	std::string GetFilename() const;
	bool GetIsValidFile() const;

	VistaPropertyList& GetPropertyList();
	const VistaPropertyList& GetPropertyList() const;

	void SetPropertyList( const VistaPropertyList& oList );
	
	// static functions to directly 
	static bool ReadProplistFromFile( const std::string& sFilename,
									  VistaPropertyList& oTarget,
									  const bool bReplaceEnvironmentVariables = false,
									  const std::string& sFileVariableSectionName = "FILE_VARIABLES",
									  const bool bCaseSensitiveKeys = false,
									  const char cSectionHeaderStartSymbol = '[',
									  const char cSectionHeaderEndSymbol = ']',
									  const char cKeyAssignmentSymbol = '=',
									  const char cCommentSymbol = '#' );
	static VistaPropertyList ReadProplistFromFile( const std::string& sFilename,
												   const bool bReplaceEnvironmentVariables = false,
												   const std::string& sFileVariableSectionName = "FILE_VARIABLES",
												   const bool bCaseSensitiveKeys = false,
												   const char cSectionHeaderStartSymbol = '[',
												   const char cSectionHeaderEndSymbol = ']',
												   const char cKeyAssignmentSymbol = '=',
												   const char cCommentSymbol = '#' );
	static bool ReadProplistFromFile( const std::string& sFilename,
									  const std::list<std::string>& liFileSearchPathes,
									  VistaPropertyList& oTarget,				
									  std::string& sFullLoadedFile,
									  const bool bReplaceEnvironmentVariables = false,
									  const std::string& sFileVariableSectionName = "FILE_VARIABLES",
									  const bool bCaseSensitiveKeys = false,
									  const char cSectionHeaderStartSymbol = '[',
									  const char cSectionHeaderEndSymbol = ']',
									  const char cKeyAssignmentSymbol = '=',
									  const char cCommentSymbol = '#' );
	static VistaPropertyList ReadProplistFromFile( const std::string& sFilename,		
												   const std::list<std::string>& liFileSearchPathes,
												   std::string& sFullLoadedFile,
												   const bool bReplaceEnvironmentVariables = false,
												   const std::string& sFileVariableSectionName = "FILE_VARIABLES",
												   const bool bCaseSensitiveKeys = false,
												   const char cSectionHeaderStartSymbol = '[',
												   const char cSectionHeaderEndSymbol = ']',
												   const char cKeyAssignmentSymbol = '=',
												   const char cCommentSymbol = '#' );

	static bool WriteProplistToFile( const std::string& sFilename,
									 const VistaPropertyList& oSource,
									 const bool bOverwriteExistingFile = false,
									 const char cSectionHeaderStartSymbol = '[',
									 const char cSectionHeaderEndSymbol = ']',
									 const char cKeyAssignmentSymbol = '=' );

private:
	bool InternalReadFile();
	bool InternalWriteFile();

	void ReadPropertyList( VistaPropertyList& oList, std::ifstream& oFile );

	void SkipSpaces();
	bool EndOfCurrentLine();
	int ReadSection( std::string& sDestination );
	bool ReadKey( char* cBuffer, std::string& sDestination);
	bool ReadEntry( char* cBuffer, std::string& sDestination);
	void RemoveSpacesAtEnd( std::string& sString );
	bool WriteEntry( const VistaProperty& oProp, int iMaxIndent, std::ofstream& oFile );

private:
	bool				m_bReplaceEnvironmentVariables;
	std::string			m_sFileVariableSectionName;
	bool				m_bFileIsValid;
	VistaPropertyList	m_oFilePropertyList;
	std::string			m_sFilename;	

	char				m_cSectionHeaderStartSymbol;
	char				m_cSectionHeaderEndSymbol;
	char				m_cCommentSymbol;
	char				m_cKeySeparatorSymbol;
	
	int					m_iMaxKeyIndent;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINIFILEPARSER_H

