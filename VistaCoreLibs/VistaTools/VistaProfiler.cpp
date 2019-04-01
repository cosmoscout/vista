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


#include "VistaToolsStd.h"
#include "VistaProfiler.h"
#include "VistaFileSystemDirectory.h"
#include "VistaFileSystemFile.h"

#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>

#ifdef WIN32
	#pragma warning(disable: 4996)
	#ifdef DEBUG
		#include <direct.h> // getcwd()
	#endif
#endif

#include <stdlib.h>

#ifndef WIN32
	#include <strings.h>
#endif

#include <algorithm>
#include <vector>
#include <set>

#include <cstring>
#include <iomanip>

// always put this line below your constant definitions
// to avoid problems with HP's compiler

using namespace std;

// enable/disable logging (works in debug mode only)
volatile bool VistaProfiler::S_bDoLogging = false;
string VistaProfiler::S_strLogFileName = string("vistaProfiler.log");
bool VistaProfiler::GetIsLoggingEnabled()
{
	return VistaProfiler::S_bDoLogging;
}

void VistaProfiler::SetIsLoggingEnabled(bool state, const char *fileName)
{
	if(fileName)
	{
		S_strLogFileName = string(fileName);
	}
	VistaProfiler::S_bDoLogging = state;
}


VistaProfiler::VistaProfiler()
: m_bEnvironmentVariablesEnabled ( false )
{
//	m_liSearchPath.push_back(""); // always search "."
#ifdef DEBUG
	vstr::erri()
		<< "WARNING: VistaProfiler has been superseded by VistaIniFileParser." << std::endl
		<< "Please migrate your code to use VistaIniFileParser instead, " << std::endl
		<< "VistaProfiler will be removed in subsequent ViSTA releases!"
		<< std::endl;   
#endif
	
	if(GetIsLoggingEnabled())
	{
		VistaType::microtime nTime = VistaTimeUtils::GetStandardTimer().GetSystemTime();
		// open file
		m_logStream.open(S_strLogFileName.c_str(), ios_base::app);
		m_logStream << "\n[" << setprecision(16) << nTime << " Constructor] called without search string.\n";
	}

};

//VistaProfiler::VistaProfiler(const list<string> &liSearchPath)
//: m_bEnviromentVariablesEnabled ( false )
//{
//	m_liSearchPath = liSearchPath;
//    if(m_liSearchPath.empty())
//	    m_liSearchPath.push_front(""); // always search "." first
//
//	if(GetIsLoggingEnabled())
//	{
//		VistaTimer t;
//		// open file
//		m_logStream.open(S_strLogFileName.c_str(), ios_base::app);
//		// append
//		m_logStream << "\n[" << setprecision(16) << t.GetSystemTime() << " Constructor] called with search string:\n";
//		list<string>::const_iterator it = liSearchPath.begin();
//		while(it != liSearchPath.end())
//		{
//			m_logStream << "\t" << it->c_str() << "\n";
//			++it;
//		}
//	}
//};


VistaProfiler::~VistaProfiler()
{
	if(GetIsLoggingEnabled())
	{
		m_logStream << "[Destructor]" << std::endl;
	}
  if( m_logStream.is_open() )
  {
    m_logStream.close();
  }
};

bool VistaProfiler::GetTheProfileSections ( list<string> &liStrResult, const string &IniFileName)
{
	if(GetIsLoggingEnabled())
	{
		m_logStream << "[GetTheProfileSections] " << IniFileName << std::endl;
	}

	// Sektionsueberschrift generieren
	vector<char> LineBuffer;
	LineBuffer.resize(ProfilerMaxBufferLength);

	vector<char> sSectionName; //[256];
	sSectionName.resize(ProfilerMaxBufferLength);

	char tmp[ProfilerMaxBufferLength];

	set<string> sSectionSet;

	ifstream *pFile = OpenIniFile(IniFileName);

	if( pFile == NULL )
	{
		vstr::warnp() << "[VistaProfiler]: File [" << IniFileName
				<< "] could not be opened for reading" << std::endl;
	}
	else
	{
		// ok, we found a file, lets scan for the sections
		while ( !(*pFile).eof() )
		{
			// Eine Zeile in den LineBuffer lesen
		  (*pFile).getline((char*)&LineBuffer[0],ProfilerMaxBufferLength,'\n');

			// Wenn [ am Anfang der Zeile steht, ist eine
			// Section gefunden.
			if( LineBuffer.front() == '[' )
			{
				sscanf(&LineBuffer[0], "[%[A-Z,a-z,0-9,_]]", tmp);

				// we found a section, we put it in the set
				// duplicates will be eliminated automatically
				sSectionSet.insert(string(tmp));
			}
		}
		CloseIniFile(pFile);
		pFile = NULL;

		// we will continue
	}


	// now we will copy the set to the result list
	for(set<string>::const_iterator cit2 = sSectionSet.begin();
		cit2 != sSectionSet.end(); ++cit2)
	{

		liStrResult.push_back(*cit2);
		//vstr::outf << "adding [" << *cit2 << "]\n";
	}

	// I would rather do it like this...
	//liStrResult.assign(sSectionSet.begin(), sSectionSet.end());
	// but some STLs won't do...

	return !liStrResult.empty();

}


/*============================================================================*/
/*============================================================================*/
bool VistaProfiler::GetTheProfileSection ( const string &IniSectionName,
											const string &IniFileName)
{
	if(GetIsLoggingEnabled())
	{
		m_logStream << "[GetTheProfileSection] " << IniFileName << " -> " << IniSectionName << std::endl;
	}

	//	cout << "[VistaProfiler]: GetTheProfileSection("
//		 << IniSectionName << ", " << IniFileName << ")" << std::endl;
	string	SectionBuffer; //[ProfilerMaxBufferLength];
	bool        bSectionFound = false;


	// Sektionsueberschrift generieren
	SectionBuffer = "[" + IniSectionName + "]";

	ifstream	*pFile = OpenIniFile(IniFileName);
	if( pFile == NULL )
	{
		vstr::warnp() << "[VistaProfiler]: File [" << IniFileName
			<< "] could not be opened for reading" << std::endl;
	}
	else
	{
		//			cout << "[VistaProfiler]: found File, looking for section\n";
		// Sektionsueberschrift generieren
		SectionBuffer = "[" + IniSectionName + "]";
		// we look for [<sectionname>]
		bSectionFound = RetrieveSection(*pFile, SectionBuffer);
		if(!bSectionFound)
		{
			//				cout << "[VistaProfiler]: no section " << SectionBuffer << " found.\n";
			CloseIniFile(pFile);
		}
		else
		{
			//				cout << "[VistaProfiler]: section " << SectionBuffer << " FOUND in file ["
			//					<< sTmpName << "].\n";
			CloseIniFile(pFile);
			return true;
		}
	}

	return bSectionFound;
}

/*============================================================================*/
// load one line from configfile and split it by "," into list
/*============================================================================*/
bool VistaProfiler::GetTheProfileList (const string &iniSectionName,
						const string &entryName,
						list<string>  & destList,
						const string &iniFileName)
{
	/**
	 * @todo log the read value, not the default one.
	 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileList] " << iniFileName << " -> " << iniSectionName << " -> " << entryName << " no default" << std::endl;

	string resultStr; //[ProfilerMaxBufferLength];
	if (GetTheProfileString (iniSectionName, entryName,"", resultStr, iniFileName)
			== true)
	{
		string configString;
		string tmpString;
		typedef string::size_type ST;
		// actual searchposition in string
		ST pos;

		configString = resultStr;
		if(resultStr == "")
			return true;

		pos = configString.find (",");

		// only one value
		if((pos==string::npos) && (resultStr != ""))
		{
			RemoveSpaceAndControlChars(&configString);
			destList.push_back(configString);
			return true;
		}

		// terminate while loop if error... should never be needed...
		// originally for testing...
		int dummy=0;

		// split string and fill list with values
		while((pos=configString.find(","))!=string::npos && dummy<1000)
		{
			tmpString=configString.substr(0,pos);
			RemoveSpaceAndControlChars(&tmpString);
			destList.push_back(tmpString);
			configString.erase(0,pos+1);
			dummy++;
		}
		tmpString=configString.substr(0,pos);
		RemoveSpaceAndControlChars(&tmpString);
		destList.push_back(tmpString);
		return true;
	}
	return false;
}


/*============================================================================*/
/*============================================================================*/

bool VistaProfiler::GetTheProfileList (	      const string &IniSectionName,
						const string &EntryName,
						const string &DefaultString,
						list<string> & destinationList,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileList] " << IniFileName << " -> " << EntryName << " " << DefaultString << std::endl;

	string resultStr; //[ProfilerMaxBufferLength];
	string configString;
	string tmpString;
	typedef string::size_type ST;
	// actual searchposition in string
	ST pos;

	// depending on whether we return the truncated default list
	// or not, we will return true or false
	bool bRet = GetTheProfileString (IniSectionName, EntryName, DefaultString, resultStr, IniFileName);

	configString = resultStr;

	pos=configString.find(",");

	// only one value
	if((pos==string::npos) && (resultStr != ""))
	{
		RemoveSpaceAndControlChars(&configString);
		if( m_bEnvironmentVariablesEnabled )
			configString = UseEnviromentVariables( configString );
		destinationList.push_back(configString);
		return bRet;
	}

	// terminate while loop if error... should never be needed...
	// originally for testing...
	int dummy=0;

	// split string and fill list with values
	while((pos=configString.find(","))!= string::npos && dummy<1000) {
		tmpString=configString.substr(0,pos);
		RemoveSpaceAndControlChars(&tmpString);
//		cout << "par: " << tmpString.c_str() << "\n";
		destinationList.push_back(tmpString);
		configString.erase(0,pos+1);
		++dummy;
	}
	tmpString=configString.substr(0,pos);
	RemoveSpaceAndControlChars(&tmpString);
	if( m_bEnvironmentVariablesEnabled )
		tmpString = UseEnviromentVariables( tmpString );
//	cout << "par: " << tmpString.c_str() << "\n";
	if(!tmpString.empty())
		destinationList.push_back(tmpString);

	return bRet;
}


/*============================================================================*/
/*============================================================================*/

bool VistaProfiler::GetTheProfileList (
						const string &IniSectionName,
						const string &EntryName,
						const string &DefaultString,
						list<float> & destinationList,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileList] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << DefaultString << std::endl;

	string resultStr; //[ProfilerMaxBufferLength];
	string configString;
	string tmpString;
	typedef string::size_type ST;
	// actual searchposition in string
	ST pos;

	// depending on whether we return the truncated default list
	// or not, we will return true or false
	bool bRet = GetTheProfileString (IniSectionName, EntryName, DefaultString, resultStr, IniFileName);

	configString = resultStr;

	pos=configString.find(",");

	// only one value
	if((pos==string::npos) && (resultStr != ""))
	{
		RemoveSpaceAndControlChars(&configString);
		destinationList.push_back((float)atof(configString.c_str()));
		if( m_bEnvironmentVariablesEnabled )
			tmpString = UseEnviromentVariables( tmpString );
		return bRet;
	}

	// terminate while loop if error... should never be needed...
	// originally for testing...
	int dummy=0;

	// split string and fill list with values
	while((pos=configString.find(","))!= string::npos && dummy<1000) {
		tmpString=configString.substr(0,pos);
		RemoveSpaceAndControlChars(&tmpString);
//		cout << "par: " << tmpString.c_str() << "\n";
		destinationList.push_back((float)atof(tmpString.c_str()));
		configString.erase(0,pos+1);
		++dummy;
	}
	tmpString=configString.substr(0,pos);
	RemoveSpaceAndControlChars(&tmpString);
	if( m_bEnvironmentVariablesEnabled )
		tmpString = UseEnviromentVariables( tmpString );
//	cout << "par: " << tmpString.c_str() << "\n";
	if(!tmpString.empty())
		destinationList.push_back((float)atof(tmpString.c_str()));

	return bRet;
}

/*============================================================================*/
/*============================================================================*/
int VistaProfiler::GetTheProfileSectionEntries  ( const string &IniSectionName,
									   list<string> &liStrResult,
									   const string &IniFileName)
{
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileSectionEntries] " << IniFileName << " -> " << IniSectionName << std::endl;

	ifstream	*pFile = NULL;
	char		LineBuffer[ProfilerMaxBufferLength];
	char key[ProfilerMaxBufferLength];
	char value[ProfilerMaxBufferLength];

	set<string> sKeySet;

	// Weitere Fehlerabfragen
	if ( (IniFileName   == "") )
		return ProfilerPointerIsNull;

	//		cout << "[VistaProfiler]: searching for file [" << sTmpName << "]\n";
	pFile = OpenIniFile(IniFileName);
	if( pFile == NULL )
	{
		vstr::warnp() << "[VistaProfiler]: File [" << IniFileName
				<< "] could not be opened for reading" << std::endl;
	}
	else
	{
		string SectionBuffer = "[" + IniSectionName + "]";
		if(!RetrieveSection(*pFile, SectionBuffer))
		{
			// there is no section in this file that we look for
			CloseIniFile(pFile);
			pFile = NULL;
		}
		else
		{
			bool        SectionEnded = false;
			// we have a section, so enumerate over keys
			while ( !SectionEnded && !(*pFile).eof() )
			{
				// Eine Zeile in den Linebuffer lesen
				(*pFile).getline(LineBuffer,ProfilerMaxBufferLength,'\n');

				if (ExtractKeyAndValue (LineBuffer, key, value))
				{
					sKeySet.insert(string(key));
					//liStrResult.push_back(string(key));
				}

				// Wenn die Zeile mit [ beginnt, dann gibt es den Eintrag
				// in der Sektion nicht.
				if ( (strstr(LineBuffer,"[")==LineBuffer) )
					SectionEnded	=	true;
			}
			CloseIniFile(pFile);
			pFile = NULL;
		}
	}


		// now we will copy the set to the result list
	for(set<string>::const_iterator cit2 = sKeySet.begin();
		cit2 != sKeySet.end(); ++cit2)
		liStrResult.push_back(*cit2);

	return ProfilerOK;
}


/*============================================================================*/
/*============================================================================*/
string VistaProfiler::GetTheProfileString(const string &iniSectionName,
	const string &sEntryName,
	const string &sDefaultString,
	const string &sIniFileName)
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileString] " << sIniFileName << " -> " << iniSectionName << " -> " << sEntryName << " " << sDefaultString << std::endl;

	string sTmp;
	GetTheProfileString(iniSectionName, sEntryName, sDefaultString, sTmp, sIniFileName);
	return sTmp;
}

bool VistaProfiler::GetTheProfileString (	      const string &IniSectionName,
						const string &EntryName,
						const string &DefaultString,
						string &DestinationString,
						const string &IniFileName)
{

//	cout << "[VistaProfiler]: GetTheProfileString("
//		 << IniSectionName << ", " << EntryName << ", " << DefaultString
//		 << ", " << IniFileName << ")" << std::endl;

	ifstream	*pFile = NULL;
	bool		SectionFound = false;

	char		LineBuffer[ProfilerMaxBufferLength];
	memset(LineBuffer, 0, ProfilerMaxBufferLength);

	string		SectionBuffer; //[ProfilerMaxBufferLength];

	// Fehlerabfrage

	// Kopieren des Defaults in den Ausgabestring
	//strcpy(DestinationString,DefaultString);
	DestinationString = DefaultString;
	// Weitere Fehlerabfragen
	if ( (IniSectionName=="") ||
		 (EntryName     =="")  ||
		 (IniFileName   =="") )
		return false; //ProfilerPointerIsNull;


	//		cout << "[VistaProfiler]: searching for file [" << sTmpName << "]\n";
	pFile = OpenIniFile(IniFileName);
	if( pFile == NULL )
	{
		vstr::warnp() << "[VistaProfiler]: File [" << IniFileName
				<< "] could not be opened for reading" << std::endl;
	}
	else
	{

		// Sektionsueberschrift generieren
		SectionBuffer = "[" + IniSectionName + "]";
		//			cout << "[VistaProfiler]: found File, looking for section " << SectionBuffer << "\n";
		// we look for [<sectionname>]
		SectionFound = RetrieveSection(*pFile, SectionBuffer);
		if(!SectionFound)
		{
			//				cout << "[VistaProfiler]: no section " << SectionBuffer << " found.\n";
			CloseIniFile(pFile);
		}
		else
		{
			//				cout << "[VistaProfiler]: section found in this file, looking for key [" << EntryName << "]\n";
			if(!RetrieveSectionString(*pFile, EntryName, DestinationString))
			{
				//					cout << "[VistaProfiler]: no key [" << EntryName << "] found here, trying the next file\n";
				CloseIniFile(pFile);
			}
			else
			{
				//					cout << "[VistaProfiler]: found key [" << EntryName << "] in file [" << sTmpName << "] with value ["
				//						<< DestinationString << "]. DONE\n\n\n";
				CloseIniFile(pFile);
				return true;
			}
		}
	}


	DestinationString = DefaultString;
//	cout << "[VistaProfiler]: section [" << IniSectionName << "] with key ["
//		<< EntryName << "] NOT found in search path, returning default value ["
//		 << DestinationString << "]\n\n";

	return false; //ProfilerOK;
}


bool VistaProfiler::RetrieveSection(ifstream &rFile, const string &sSectionName)
{
	bool SectionFound = false;
	char		LineBuffer[ProfilerMaxBufferLength];
	// IniFile nach der Sectionsueberschrift durchsuchen
	while ( !SectionFound && !rFile.eof() )
	{
		// Eine Zeile in den LineBuffer lesen
		rFile.getline(LineBuffer,ProfilerMaxBufferLength,'\n');
		string sTmp = string(LineBuffer).substr(0,sSectionName.size());
		SectionFound = ValueEquals(sTmp, sSectionName, false);
	}
	return SectionFound;
}

bool VistaProfiler::RetrieveSectionString(ifstream &rFile, const string &sKeyName, string &rStorage)
{
	bool EntryFound = false;
	bool SectionEnded = false;

	char key[ProfilerMaxBufferLength];
	char value[ProfilerMaxBufferLength];
	char SingleLineBuffer[ProfilerMaxBufferLength];
	std::string SingleLineStringBuffer;
	std::string LineStringBuffer;

	// Die Sektion nach dem Eintrag durchsuchen
	while ( !EntryFound && !SectionEnded && !rFile.eof() )
	{
		SingleLineStringBuffer = "";
		LineStringBuffer = "";
		
		// Eine Zeile in den Linebuffer lesen
		rFile.getline(SingleLineBuffer, ProfilerMaxBufferLength, '\n');
		SingleLineStringBuffer = SingleLineBuffer;
		while((!SingleLineStringBuffer.empty()) && (*(SingleLineStringBuffer.end()-1) == '\\'))
		{
			SingleLineStringBuffer.erase(SingleLineStringBuffer.end()-1);
			LineStringBuffer.append(SingleLineStringBuffer);

			rFile.getline(SingleLineBuffer, ProfilerMaxBufferLength, '\n');
			SingleLineStringBuffer = SingleLineBuffer;
		}
		LineStringBuffer.append(SingleLineStringBuffer);

		char *LineBuffer;
		LineBuffer = strdup(LineStringBuffer.c_str());
		if (ExtractKeyAndValue (LineBuffer, key, value))
		{
			EntryFound = ValueEquals(sKeyName,key,false);
		}

		// Wenn die Zeile mit [ beginnt, dann gibt es den Eintrag
		// in der Sektion nicht.
		if ( !EntryFound && (strstr(LineBuffer,"[") == LineBuffer) )
			SectionEnded = true;

		free(LineBuffer);
	}

	if(EntryFound)
		rStorage = value;

	if(m_bEnvironmentVariablesEnabled)
		rStorage = UseEnviromentVariables( rStorage );

	return EntryFound;
}


//////////////////////////////////////////////////////////////////////

int	VistaProfiler::GetTheProfileInt      (		      const string &IniSectionName,
						const string &EntryName,
						const int            DefaultInt,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileInt] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << DefaultInt << std::endl;

	string LineBuffer; //[ProfilerMaxBufferLength];
	int	Result;

	Result	=	DefaultInt;

	if ( this->GetTheProfileString (	IniSectionName,
					EntryName,
					"",
					LineBuffer,
					IniFileName)!= ProfilerOK)
		return DefaultInt;

	LineBuffer.resize(ProfilerMaxBufferLength);
	sscanf((char*)LineBuffer.data(),"%i",&Result);
	
	return Result;
}
//////////////////////////////////////////////////////////////////////
bool VistaProfiler::GetTheProfileBool (	      const string &IniSectionName,
						const string &EntryName,
						const bool            DefaultBool,
						const string &IniFileName )

{
	bool retVal = DefaultBool;
	string LineBuffer; //[ProfilerMaxBufferLength];

	if ( this->GetTheProfileString (	IniSectionName,
					EntryName,
					"",
					LineBuffer,
					IniFileName)!= ProfilerOK)
		return retVal;

	if ((LineBuffer == "true") ||
		(LineBuffer == "True") ||
		(LineBuffer == "TRUE") ||
		(LineBuffer == "1")    ||
		(LineBuffer == "ON")   ||
		(LineBuffer == "on"))
	{
		retVal = true;
	}
	else if ((LineBuffer == "false") ||
		(LineBuffer == "False") ||
		(LineBuffer == "FALSE") ||
		(LineBuffer == "OFF") ||
		(LineBuffer == "off") ||
		(LineBuffer == "0"))
	{
		retVal = false;
	}

	if(GetIsLoggingEnabled())
		m_logStream
		<< "[GetTheProfileLong] " << IniFileName << " -> "
		<< IniSectionName << " -> " << EntryName << " "
		<< boolalpha << retVal << std::endl;

	return retVal;
}
//////////////////////////////////////////////////////////////////////

long	VistaProfiler::GetTheProfileLong (	      const string &IniSectionName,
						const string &EntryName,
						const long           DefaultLong,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileLong] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << DefaultLong << std::endl;

	string LineBuffer; //[ProfilerMaxBufferLength];
	long	Result;

	Result	=	DefaultLong;

	if ( this->GetTheProfileString (	IniSectionName,
					EntryName,
					"",
					LineBuffer,
					IniFileName)!= ProfilerOK)
		return DefaultLong;

	LineBuffer.resize(ProfilerMaxBufferLength);
	sscanf((char*)LineBuffer.data(),"%li",&Result);

	return Result;
}

//////////////////////////////////////////////////////////////////////

unsigned int	VistaProfiler::GetTheProfileUInt (   const string &IniSectionName,
						const string &EntryName,
						const unsigned int   DefaultUInt,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileUInt] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << DefaultUInt << std::endl;

	string		LineBuffer; //[ProfilerMaxBufferLength];
	unsigned int	Result;

	Result	=	DefaultUInt;

	if ( this->GetTheProfileString (	IniSectionName,
					EntryName,
					"",
					LineBuffer,
					IniFileName)!= ProfilerOK)
		return DefaultUInt;

	LineBuffer.resize(ProfilerMaxBufferLength);

	// there exists no type conversion specifier for "prefixed" unsigned values,
	// thus check the prefix manually.
	if( !strncmp( (char*)LineBuffer.data(), "0x", 2 ) || !strncmp( (char*)LineBuffer.data(), "0X", 2 ) )
	{
		// string begins with hex prefix
		sscanf((char*)LineBuffer.data(),"%x",&Result);
	}
	else if( !strncmp( (char*)LineBuffer.data(), "0", 1 ) )
	{
		// string begins with octal prefix
		sscanf((char*)LineBuffer.data(),"%o",&Result);
	}
	else
	{
		// no prefix: read as decimal
		sscanf((char*)LineBuffer.data(),"%u",&Result);		
	}

	return Result;
}

unsigned long	VistaProfiler::GetTheProfileULong (  const string &IniSectionName,
						const string &EntryName,
						const unsigned long  DefaultULong,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileULong] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << DefaultULong << std::endl;

	string		LineBuffer;//[ProfilerMaxBufferLength];
	unsigned long	Result;

	Result	=	DefaultULong;

	if ( this->GetTheProfileString (	IniSectionName,
					EntryName,
					"",
					LineBuffer,
					IniFileName)!= ProfilerOK)
		return DefaultULong;

	LineBuffer.resize(ProfilerMaxBufferLength);

	// there exists no type conversion specifier for "prefixed" unsigned values,
	// thus check the prefix manually.
	if( !strncmp( (char*)LineBuffer.data(), "0x", 2 ) || !strncmp( (char*)LineBuffer.data(), "0X", 2 ) )
	{
		// string begins with hex prefix
		sscanf((char*)LineBuffer.data(),"%lx",&Result);
	}
	else if( !strncmp( (char*)LineBuffer.data(), "0", 1 ) )
	{
		// string begins with octal prefix
		sscanf((char*)LineBuffer.data(),"%lo",&Result);
	}
	else
	{
		// no prefix: read as decimal
		sscanf((char*)LineBuffer.data(),"%lu",&Result);		
	}

	return Result;
}

//////////////////////////////////////////////////////////////////////

float	VistaProfiler::GetTheProfileFloat (	      const string &IniSectionName,
						const string &EntryName,
						const float          DefaultFloat,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[GetTheProfileFloat] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << DefaultFloat << std::endl;

	string	LineBuffer; //[ProfilerMaxBufferLength];
	float	Result;

	Result	=	DefaultFloat;

	if ( this->GetTheProfileString (	IniSectionName,
					EntryName,
					"",
					LineBuffer,
					IniFileName)!= ProfilerOK)
		return DefaultFloat;


	LineBuffer.resize(ProfilerMaxBufferLength);
	sscanf((char*)LineBuffer.data(),"%g",&Result);

	return Result;
}

//////////////////////////////////////////////////////////////////////

int	VistaProfiler::SetTheProfileString (	      const string &IniSectionName,
						const string &EntryName,
						const string &ValueString,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[SetTheProfileString] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << ValueString << std::endl;

#ifdef WIN32
	char *TmpFileName;
#else
	char 		TmpFileName[] = "_TEMP_FILE_XXXXXX";
#endif
	ofstream	fout1;
	ofstream	fout2;
	ifstream	fin1;
	ifstream	fin2;
	string		LineBuffer; //[ProfilerMaxBufferLength];
	string		Entry0; //[ProfilerMaxBufferLength];
	string		Entry1; //[ProfilerMaxBufferLength];
	string     Entry2; //[ProfilerMaxBufferLength];

	string		SectionBuffer; //[ProfilerMaxBufferLength];
	int		SectionFound = 0;

	// Fehlerpruefung
	if ( (IniSectionName=="") ||
		 (EntryName     =="") ||
		 (ValueString   =="") ||
		 (IniFileName   =="") )
		return ProfilerPointerIsNull;

	// Existiert das IniFile ueberhaupt und kann es gelesen und beschrieben
	// werden. Falls nicht, dann muss das File einfach erzeugt werden.
	if(!TestForIniFileReadWriteAccess(IniFileName))
	{
		fout1.open(IniFileName.c_str());

		// Sektionsnamen schreiben
		fout1 << "[" << IniSectionName.c_str() << "]\n";
		// Eintrag schreiben
		fout1 << EntryName.c_str() << " = " << ValueString.c_str() << "\n\n";

		fout1.close();

		return ProfilerOK;
	}

	// Status pruefen
	bool bSectionFound =	this->GetTheProfileSection(IniSectionName,IniFileName);

	// Wenn die Sektion nicht existiert, dann einfach eine Sektion mit
	// dem angegebenen Eintrag anhaengen
	if ( !bSectionFound )
	{
		fout1.open(IniFileName.c_str(),ios::app);
		fout1 << "\n";
		fout1 << "[" << IniSectionName.c_str() << "]\n";
		fout1 << EntryName.c_str() << " = " << ValueString.c_str() << "\n";
		fout1.close();

		return ProfilerOK;
	}

	// Ansonsten wird's komplizierter:

	// Namen einer temporaeren Datei generieren
#ifdef WIN32
	char TMPSTR[L_tmpnam];
	tmpnam(TMPSTR);
	/* NOTE: a leading backslash in the tmp file name indicates that the tmp fname 
	is valid in the current working directry ==> remove this backslash in order to
	prevent writing to FS-root (e.g. c:\) which might be a problem with no admin rights */
	TmpFileName = TMPSTR;
	if(TmpFileName[0] == '\\')
		++TmpFileName;
#else
	mkstemp(TmpFileName);
#endif
	// Falls der Eintrag bereits existierte, die drei moeglichen
	// Zeilenanfaenge zusammenbasteln
	Entry0 = EntryName + "=";
	Entry1 = EntryName + " ";
	Entry2 = EntryName + "\t";

	/*strcpy(Entry0,EntryName);
	strcpy(Entry1,EntryName);
	strcpy(Entry2,EntryName);
	strcat(Entry0,"=");
	strcat(Entry1," ");
	strcat(Entry2,"\t");
*/
	// Dateien oeffnen
	fin1.open(IniFileName.c_str());
	fout1.open(TmpFileName);

	// Sectionsueberschrift zusammenstellen
	SectionBuffer = "[" + IniSectionName + "]";

	//strcpy(SectionBuffer,"[");
	//strcat(SectionBuffer,IniSectionName);
	//strcat(SectionBuffer,"]");

	// Zunaechst einmal das IniFile kopieren, falls der Eintrag existierte
	// dann erst einmal einfach weglassen
	LineBuffer.resize(ProfilerMaxBufferLength);

	while ( !fin1.eof() )
	{
		fin1.getline((char*)LineBuffer.data(),ProfilerMaxBufferLength,'\n');

		// Ist der Beginn der Sektion gefunden
		if ( LineBuffer[0]=='[' )
		{
			if ( strstr(LineBuffer.c_str(),SectionBuffer.c_str())==LineBuffer.c_str() )
				SectionFound	=	1;
			else
				SectionFound	=	0;
		}
 
		// Wenn Eintrag nicht existierte, dann einfach kopieren,
		// ansonsten gegebenenfalls weglassen
		if ( !bSectionFound || !SectionFound )
		{
			if ( fin1.eof() )
				fout1 << LineBuffer.c_str();
			else
				fout1 << LineBuffer.c_str() << "\n";
		}
		else
		{
			if ( SectionFound && !( (strstr(LineBuffer.c_str(),Entry0.c_str())==LineBuffer.c_str()) ||
						(strstr(LineBuffer.c_str(),Entry1.c_str())==LineBuffer.c_str()) ||
						(strstr(LineBuffer.c_str(),Entry2.c_str())==LineBuffer.c_str()) ) )
			{
				if ( fin1.eof() )
					fout1 << LineBuffer.c_str();
				else
					fout1 << LineBuffer.c_str() << "\n";
			}
		}
	}

	// Dateien schliessen und in umgekehrter Form wieder oeffnen
	fin1.close();
	fout1.close();
	

	fin2.open(TmpFileName);
	fout2.open(IniFileName.c_str());
	
	// Datei wieder zurueckkopieren und dabei den neuen Eintrag nach der
	// Sektionsueberschrift einfuegen.
	while ( !fin2.eof() )
	{
		fin2.getline((char*)LineBuffer.data(),ProfilerMaxBufferLength,'\n');
		
		// Zeile kopieren
		if ( fin2.eof() )
			fout2 << LineBuffer.c_str();
		else
			fout2 << LineBuffer.c_str() << "\n";

		// Wenn Sektionsanfang gefunden, dann den neuen Eintrag 
		// anhaengen.
		if ( strstr(LineBuffer.c_str(),SectionBuffer.c_str())==LineBuffer.c_str() )
		{
			if ( fin2.eof() )
				fout2 << EntryName.c_str() << " = " << ValueString.c_str();
			else
				fout2 << EntryName.c_str() << " = " << ValueString.c_str() << "\n";
		}
	}

	// Dateien wieder schliessen und temporaere Datei loeschen.
	fin2.close();
	fout2.close();

	remove(TmpFileName);

	return ProfilerOK;
}

//////////////////////////////////////////////////////////////////////

int	VistaProfiler::SetTheProfileInt (	      const string &IniSectionName,
						const string &EntryName,
						const int            ValueInt,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[SetTheProfileInt] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << ValueInt << std::endl;

	char	LineBuffer[ProfilerMaxBufferLength];

	if (sprintf(LineBuffer,"%d",ValueInt)==0)
		return ProfilerCommonError;

	return this->SetTheProfileString(IniSectionName,EntryName,LineBuffer,IniFileName);
}

//////////////////////////////////////////////////////////////////////

int	VistaProfiler::SetTheProfileLong (	      const string &IniSectionName,
						const string &EntryName,
						const long           ValueLong,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[SetTheProfileLong] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << ValueLong << std::endl;

	char	LineBuffer[ProfilerMaxBufferLength];

	if (sprintf(LineBuffer,"%ld",ValueLong)==0)
		return ProfilerCommonError;

	return this->SetTheProfileString(IniSectionName,EntryName,LineBuffer,IniFileName);
}

//////////////////////////////////////////////////////////////////////

int	VistaProfiler::SetTheProfileUInt (	      const string &IniSectionName,
						const string &EntryName,
						const unsigned int   ValueUInt,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[SetTheProfileUInt] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << ValueUInt << std::endl;

	char	LineBuffer[ProfilerMaxBufferLength];

	if (sprintf(LineBuffer,"%u",ValueUInt)==0)
		return ProfilerCommonError;

	return this->SetTheProfileString(IniSectionName,EntryName,LineBuffer,IniFileName);
}

//////////////////////////////////////////////////////////////////////

int	VistaProfiler::SetTheProfileULong (	      const string &IniSectionName,
						const string &EntryName,
						const unsigned long  ValueULong,
						const string &IniFileName )
{
/**
 * @todo log the read value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[SetTheProfileULong] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << " " << ValueULong << std::endl;

	char	LineBuffer[ProfilerMaxBufferLength];

	if (sprintf(LineBuffer,"%ld",ValueULong)==0)
		return ProfilerCommonError;

	return this->SetTheProfileString(IniSectionName,EntryName,LineBuffer,IniFileName);
}

//////////////////////////////////////////////////////////////////////

int	VistaProfiler::SetTheProfileFloat (	      const string &IniSectionName,
						const string &EntryName,
						const float          ValueFloat,
						const string &IniFileName )
{
/**
 * @todo log the real value, not the default one.
 */
	if(GetIsLoggingEnabled())
		m_logStream << "[SetTheProfileFloat] " << IniFileName << " -> " << IniSectionName << " -> " << EntryName << ValueFloat << std::endl;

	char	LineBuffer[ProfilerMaxBufferLength];

	if (sprintf(LineBuffer,"%f",ValueFloat)==0)
		return ProfilerCommonError;

	return this->SetTheProfileString(IniSectionName,EntryName,LineBuffer,IniFileName);
}

//////////////////////////////////////////////////////////////////////


/*============================================================================*/
/*============================================================================*/
bool  VistaProfiler::ExtractKeyAndValue (char * totalLine, char * chKey, char * chValue)
{
 
	if ( (totalLine[0] == '#') || (totalLine[0] == '['))
		return false;

	if ( strstr( totalLine, "=" ) != NULL)
	{
		int posEqual = (int)(strstr (totalLine, "=") - totalLine + 1);
		int totLen   = (int)strlen (totalLine);
		strncpy ( chKey, totalLine, posEqual - 1 );
		chKey [posEqual - 1] = '\0';
		RemoveSpaceAndControlChars (chKey);
		strncpy ( chValue, totalLine + posEqual, totLen - posEqual );
		chValue [totLen - posEqual] = '\0';
		RemoveSpaceAndControlChars (chValue);
	}
	else
	{
		return false;
	}

	return true;
}


/*============================================================================*/
/*============================================================================*/
bool  VistaProfiler::RemoveSpaceAndControlChars (char * chString)
{
	int strLen = 0;

	// remove special characters from the end of the string
	// that should also handle the problem of DOS - UNIX conversion
	bool checkStringReady = false;
	do
	{
		strLen = (int)strlen (chString);
		if (strLen > 0)
		{
			// get last char as ascii code
			int curChar = chString[strLen-1];
			if (curChar <= 32)
			{
				// cut last char
				chString[strLen-1] = '\0';
			}
			else
			{
				checkStringReady = true;
			}
		}
		else
		{
			// last char checked or empty string
			checkStringReady = true;
		}
	} while (checkStringReady == false);

	// remove special characters from the beginning of the string
	strLen = (int)strlen (chString);
	for (int curPos = 0; curPos < strLen; curPos++)
	{
		int curChar = chString[curPos];
		if (curChar > 32)
		{
			// cut last char
			char tempString[ProfilerMaxBufferLength];
			strncpy ( tempString, chString + curPos, strLen - curPos );
			strncpy ( chString, tempString, strLen - curPos );
			chString[strLen-curPos] = '\0';
			return true;
		}
		else
		{
			// the last char is also a special char
			if (strLen == (curPos + 1))
			{
				chString[0] = '\0';
				return true;
			}
		}
	}

	return true;
}


/*============================================================================*/
/*============================================================================*/

bool  VistaProfiler::RemoveSpaceAndControlChars (string * stlString)
{
	char * tmpString;
	
	tmpString=new char[stlString->length()+1];
	stlString->copy(tmpString,string::npos);
	tmpString[stlString->length()]=0;

	RemoveSpaceAndControlChars(tmpString);

	stlString->assign(tmpString);

	delete [] tmpString;

	return true;
}

/*============================================================================*/
/*============================================================================*/

bool VistaProfiler::ValueEquals(const string &value, const string &expected, bool bCaseSensitive)
{
	if(bCaseSensitive)
		return value == expected;
	else
	{
#if defined(WIN32)
		return (stricmp(value.c_str(), expected.c_str())==0);
#elif defined(LINUX) || defined(SUNOS) || defined(IRIX) || defined(HPUX) || defined(DARWIN)
		return (strcasecmp(value.c_str(), expected.c_str())==0);
#else
		VISTA_THROW_NOT_IMPLEMENTED;
#endif
	}
}

/*============================================================================*/
/*============================================================================*/
string VistaProfiler::ToUpper(const string &strInput) 
{
	string strTemp = strInput;
	for (unsigned int i=0; i<strTemp.size(); ++i)
	{
		strTemp[i] = toupper(strTemp[i]);
	}

	return strTemp;
}

string VistaProfiler::ToLower(const string &strInput) 
{
	string strTemp = strInput;
	for (unsigned int i=0; i<strTemp.size(); ++i)
	{
		strTemp[i] = tolower(strTemp[i]);
	}

	return strTemp;
}

bool  VistaProfiler::TestForIniFileReadAccess(const string &sFileName)
{
	VistaFileSystemFile file(sFileName);
	return file.Exists() && (file.GetType() == VistaFileSystemNode::FT_FILE);
/*
#ifdef WIN32
	// EXISTENCE ONLY = 00, READ PERMISSION = 04, READ-WRITE PERMISSION = 06
	if ( (_access(sFileName.c_str(),00) == 0) )
	{
		// file exists, check for read permission
		if( (_access(sFileName.c_str(), 04 ) == 0 ) )
		{
			// file is readable, everything is ok
			return true;
		}
		else
		{
			// file exists, but no read access, we utter a warning
			vstr::warnp() << "[VistaProfiler] File [" << sFileName << "] exists, "
					  << "but no read access.\n";
			return false;
		}
	}

	return false;
#else
	if ( (access(sFileName.c_str(),R_OK) ==0) )
	{
		// file exists and has read permissions,
		// so everything is fine
		return true;
	};
	return false;
#endif
	*/
}

bool  VistaProfiler::TestForIniFileWriteAccess(const string &sFileName)
{
	VistaFileSystemFile file(sFileName);
	return file.Exists() && (file.GetType() == VistaFileSystemNode::FT_FILE) && !file.IsReadOnly();
/*
#ifdef WIN32
	if ( (_access(sFileName.c_str(),00) == 0) )
	{
		// file exists, check for read permission
		if( (_access(sFileName.c_str(), 02 ) == 0 ) )
		{
			// file is writeable, everything is ok
			return true;
		}
		else
		{
			// file exists, but no read access, we utter a warning
			vstr::warnp() << "[VistaProfiler] File [" << sFileName << "] exists, "
					  << "but no write access.\n";
			return false;
		}
	}
	return false; // file does not exist
#else
	if ((access(sFileName.c_str(),W_OK)==0) )
	{
		// file exists and has write permissions
		return true;
	};

	return false; // does not exist or we do not have the right permissions
#endif
	*/
}

bool  VistaProfiler::TestForIniFileReadWriteAccess(const string &sFileName)
{
	return (TestForIniFileReadAccess(sFileName) && TestForIniFileWriteAccess(sFileName));
}

#if 0
ifstream *VistaProfiler::_SearchIniFile(const string &sFileName) const
{
	ifstream *pFile = NULL;

	// first, look in '.'
//	if(m_liSearchPath.empty())
//		cout << "[VistaProfiler]: no search path given.\n";
//	else
//		cout << "[VistaProfiler]: using search paths\n";

	for(list<string>::const_iterator cit = m_liSearchPath.begin();
		cit != m_liSearchPath.end(); ++cit)
	{
		//string sTmpName = *cit + FileSystemDirectory::GetOSSpecificSeparator() + sFileName;
		string sTmpName = *cit + sFileName;
//		cout << "[VistaProfiler]: searching for file [" << sTmpName << "]\n";
		if((pFile = OpenIniFile(sTmpName)))
		{
//			cout << "[VistaProfiler]: found it.\n";
			break; // leave loop, we have found what we were looking for
		};
	}
	return pFile;
}

#endif
ifstream *VistaProfiler::OpenIniFile(const string &sFileName) const
{
	ifstream *pFile = NULL;
	if(TestForIniFileReadAccess(sFileName))
	{
		pFile = new ifstream;
		(*pFile).open(sFileName.c_str());
		return pFile;
	}
	return NULL;
}

void VistaProfiler::CloseIniFile(ifstream *pFile) const
{
	(*pFile).close();
	delete pFile;
}


bool VistaProfiler::RemoveEntry(const string &strSection,
								 const string &strEntry,
								 const string &strIniFile)
{
	if (strSection=="" || strEntry=="" || strIniFile=="")
		return false;

	ofstream	os;
	ifstream	is;
	char			cstrLineBuffer[ProfilerMaxBufferLength];
	string		strLineBuffer;
	string		strEntry0;
	string		strEntry1;
	string     strEntry2;

	string		strSectionBuffer;

	if(!TestForIniFileReadWriteAccess(strIniFile))
	{
		// fine, we're done...
		return true;
	}

	bool bSectionFound = GetTheProfileSection(strSection,strIniFile);
	if (!bSectionFound)
	{
		// hm, that was easy, too...
		return true;
	}

	// allright, we'll test for existance of our entry on the fly...

#ifdef WIN32
	char *cstrTmpName;
	char TMPSTR[L_tmpnam];
	tmpnam(TMPSTR);
	/* NOTE: a leading backslash in the tmp file name indicates that the tmp fname 
	is valid in the current working directry ==> remove this backslash in order to
	prevent writing to FS-root (e.g. c:\) which might be a problem with no admin rights */
	cstrTmpName = TMPSTR;
	if(cstrTmpName[0] == '\\')
		++cstrTmpName;
#else
	char cstrTmpName[] = "_TEMP_FILE_XXXXXX";
	mkstemp(cstrTmpName);
#endif
	strEntry0 = strEntry + "=";
	strEntry1 = strEntry + " ";
	strEntry2 = strEntry + "\t";

	is.open(strIniFile.c_str());
	os.open(cstrTmpName);

	strSectionBuffer = "[" + strSection + "]";

	// copy the ini file, but leave out the entry to be removed...
	strLineBuffer.resize(ProfilerMaxBufferLength);

	bSectionFound = false;
	bool bEntryFound = false;

	while (!is.eof())
	{
		is.getline(cstrLineBuffer,ProfilerMaxBufferLength,'\n');
		strLineBuffer = cstrLineBuffer;

		if (strLineBuffer.find(strSectionBuffer) == 0)
		{
			bSectionFound = true;
			os << strLineBuffer << "\n";
			continue;
		}

		if (bSectionFound)
		{
			// are we leaving the right section again??
			if (strLineBuffer[0] == '[')
			{
				if (bEntryFound)
				{
					// ok, we have found our entry, and now, we're just
					// leaving the section, which means, we still have to
					// copy some stuff...
					bSectionFound = false;
				}
				else
				{
					// yep, we are leaving, but we didn't find anything,
					// so let's just go home...
					break;
				}
			}
			else if (strLineBuffer.find(strEntry0) == 0
				|| strLineBuffer.find(strEntry1) == 0
				|| strLineBuffer.find(strEntry2) == 0)
			{
				// whoopee!!
				bEntryFound = true;
//				is.getline(cstrLineBuffer,ProfilerMaxBufferLength,'\n');
				continue;
			}
		}

		// so, we are not in the right section, or we are in the right section,
		// but are not dealing with the desired entry, so we just write out, 
		// whatever we have here...
		os << strLineBuffer;
		if (!is.eof())
			os << "\n";

//		is.getline(cstrLineBuffer,ProfilerMaxBufferLength,'\n');
	}

	is.close();
	os.close();

	// did we or did we not find (and leave out) our entry??
	if (bEntryFound)
	{
		remove(strIniFile.c_str());
		rename(cstrTmpName, strIniFile.c_str());
	}
	else
	{
		// hm, we didn't find the desired entry in the right section, 
		// so we just leave everything as it was...
		remove(cstrTmpName);
	}
	return true;
}

void VistaProfiler::SetEnvironmentVariablesEnabled( bool bState )
{
	m_bEnvironmentVariablesEnabled = bState;
}

bool VistaProfiler::GetEnvironmentVariablesEnabled()
{
	return m_bEnvironmentVariablesEnabled;
}

std::string VistaProfiler::UseEnviromentVariables( std::string sInput )
{
	int start = (int)sInput.find("${");
	while( start > -1 )
	{
		int end = (int)sInput.find("}");
		std::string var = sInput.substr(start+2,end-start-2);
		char* sEnv = getenv(var.c_str());
		if( sEnv )
			sInput = sInput.replace(start,end-start+1,sEnv);
		else
			sInput = sInput.replace(start,end-start+1,"");
		start = (int)sInput.find("${");
	}
	return sInput;
}

