/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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


#include "VistaShaderRegistry.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaTools/VistaEnvironment.h>
#include <VistaAspects/VistaAspectsUtils.h>

#include <cstdio>
#include <algorithm>

using namespace std;

/*============================================================================*/
/* STATIC VARIABLES                                                           */
/*============================================================================*/
VistaShaderRegistry VistaShaderRegistry::m_instance;
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
/******************************************************************************/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/******************************************************************************/
VistaShaderRegistry::VistaShaderRegistry()
{
	string strEnv = VistaEnvironment::GetEnv("VISTA_SHADER_DIRS");

#ifdef WIN32
	VistaAspectsConversionStuff::ConvertToList( 
			strEnv, m_liSearchDirectories, ';' );
#else
	VistaAspectsConversionStuff::ConvertToList( 
			strEnv, m_liSearchDirectories, ':' );
#endif // WIN32

	// Add trailing '/' if necessary
	list<string>::iterator it = m_liSearchDirectories.begin();
	for (; it != m_liSearchDirectories.end(); ++it)
	{
		if( (*it)[(*it).length()-1] != '/' )
		{
			(*it).push_back( '/' );
		}
	}
}
/******************************************************************************/
/* GETTER                                                                     */
/******************************************************************************/
VistaShaderRegistry& VistaShaderRegistry::GetInstance()
{
	return m_instance;
}

std::list<std::string> VistaShaderRegistry::GetSearchDirectories() const
{
	return m_liSearchDirectories;
}
/******************************************************************************/
/* ADD/REMOVE SEARCH DIRECTORY                                                */ 
/******************************************************************************/
void VistaShaderRegistry::AddSearchDirectory( const string& strSearchDir )
{
	// Add trailing '/' if necessary 
	string strSearchDirectory = strSearchDir;
	if( strSearchDirectory[strSearchDirectory.length()-1] != '/' )
		strSearchDirectory.push_back( '/' );

	list<string>::iterator it = find(	m_liSearchDirectories.begin(),
										m_liSearchDirectories.end(),
										strSearchDirectory );

	if(it == m_liSearchDirectories.end())
	{
		m_liSearchDirectories.push_front(strSearchDirectory);
	}
#	ifndef DISABLE_SHADER_REGISTRY_WARNINGS
	else
	{
		vstr::warnp() << "[VistaShaderRegistry]::AddSearchDirectory"<< endl;
		vstr::IndentObject oIndent;
		vstr::warni() << "\"" << strSearchDirectory 
			<< "\" is already in the list of search directories."  << endl;
	}
#	endif

}

void VistaShaderRegistry::RemoveSearchDirectory( const string& strSearchDir )
{
	// Add trailing '/' if necessary
	string strSearchDirectory = strSearchDir;
	if( strSearchDirectory[strSearchDirectory.length()-1] != '/' )
		strSearchDirectory.push_back( '/' );

	list<string>::iterator it = find(	m_liSearchDirectories.begin(),
										m_liSearchDirectories.end(),
										strSearchDirectory );

	if(it != m_liSearchDirectories.end())
	{
		m_liSearchDirectories.erase( it );
	}
#	ifndef DISABLE_SHADER_REGISTRY_WARNINGS
	else
	{
		vstr::warnp() << "[VistaShaderRegistry]::RemoveSearchDirectory"<< endl;
		vstr::IndentObject oIndent;
		vstr::warni() << "Can't find \"" << strSearchDirectory 
			<< "\" in the list of search directories."  << endl;
	}
#	endif
}

/******************************************************************************/
/* RegisterShader                                                             */
/******************************************************************************/
bool VistaShaderRegistry::RegisterShader(	const string& strShaderPath,
                                         	bool bAllowOverwrite /* = false */ )
{
	const size_t nLasSlashPos = strShaderPath.find_last_of( '/' );
	const string strShaderFilename = strShaderPath.substr( nLasSlashPos + 1 );

	return RegisterShader( strShaderPath, strShaderFilename, bAllowOverwrite );
}

/******************************************************************************/
bool VistaShaderRegistry::RegisterShader(	const string& strShaderPath,
                                         	const string& strRegisterAlias, 
                                         	bool bAllowOverwrite /* = false */ )
{
	// Nested if for lazily checking if cached or not.
	if( !bAllowOverwrite )
	{
		map<string, string>::const_iterator citSource =
			m_mapShadersFnToSource.find( strRegisterAlias );
		if( m_mapShadersFnToSource.end() != citSource )
			return false;
	}

	const string strSource = LoadSourceFromFile( strShaderPath );

	if( !strSource.empty() )
	{
		m_mapShadersFnToSource[strRegisterAlias] = strSource;
		return true;
	}
#	ifndef DISABLE_SHADER_REGISTRY_WARNINGS
	else
	{
		vstr::warnp() << "[VistaShaderRegistry]::RegisterShader()"<< endl;
		vstr::IndentObject oIndent;
		vstr::warni()	<< "Shader source could not be found, "
						<< "or source file is empty." << endl;
	}
#	endif

	return false;
}

/******************************************************************************/
/* RetrieveShader                                                             */
/******************************************************************************/
string VistaShaderRegistry::RetrieveShader( const string& strShaderName )
{
	map<string, string>::const_iterator citSource =
			m_mapShadersFnToSource.find( strShaderName );

	if( m_mapShadersFnToSource.end() != citSource )
		return (*citSource).second;

	const string strSource = LoadSourceFromDirs( strShaderName );

	if( !strSource.empty() )
	{
		m_mapShadersFnToSource[strShaderName] = strSource;
	}

	return strSource;
}

/******************************************************************************/
bool VistaShaderRegistry::RegisterShaderAlias(	const std::string& strShaderName,
                                              	const std::string& strAliasName,
                                             	bool bAllowOverwrite /*= false*/ )
{
	// Nested if for lazily checking if cached or not.
	if( !bAllowOverwrite )
	{
		map<string, string>::const_iterator citSource =
			m_mapShadersFnToSource.find( strAliasName );
		if( m_mapShadersFnToSource.end() != citSource )
			return false;
	}
	const string strSource = RetrieveShader( strShaderName );

	if( !strSource.empty() )
	{
		m_mapShadersFnToSource[strAliasName] = strSource;
		return true;
	}
#	ifndef DISABLE_SHADER_REGISTRY_WARNINGS
	else
	{
		vstr::warnp() << "[VistaShaderRegistry]::RegisterShader()"<< endl;
		vstr::IndentObject oIndent;
		vstr::warni()	<< "Shader source could not be found, "
			<< "or source file is empty." << endl;
	}
#	endif

	return false;
}


/******************************************************************************/
/* Reset/DropFromCache                                                        */
/******************************************************************************/
void VistaShaderRegistry::Reset()
{
	m_mapShadersFnToSource.clear();
}

void VistaShaderRegistry::DropFromCache( const std::string& strShaderFilename )
{
	m_mapShadersFnToSource.erase( strShaderFilename );
}

/******************************************************************************/
/* PROTECTED METHODS                                                          */
/******************************************************************************/
string VistaShaderRegistry::LoadSourceFromFile( const string& strFilePath )
{
	FILE *pShaderFile = fopen(strFilePath.c_str(), "rb");
	if(pShaderFile)
	{
		char *pShaderString = NULL;
		int iShaderLength=-1;

		// determine file size
		fseek(pShaderFile , 0, SEEK_END);
		iShaderLength = ftell(pShaderFile);
		fseek(pShaderFile , 0, SEEK_SET);

		// read the program from file
		pShaderString = new char[iShaderLength+1];
		fread(pShaderString, iShaderLength, 1, pShaderFile);
		fclose(pShaderFile);
		pShaderString[iShaderLength] = '\0';

		string strShaderString(pShaderString);

#		ifndef DISABLE_SHADER_REGISTRY_WARNINGS
		if(strShaderString.empty())
		{
			vstr::warnp() << "[VistaShaderRegistry] Shader File\"" 
				<< strFilePath << "\" is empty!" << endl;
		}
#		endif

		delete[] pShaderString;

		return strShaderString;
	}

	return string("");
}

/******************************************************************************/
string VistaShaderRegistry::LoadSourceFromDirs( const string& strShaderName )
{
	string strShaderSource = "";
	list<string>::const_iterator it = m_liSearchDirectories.begin();
	for( ; it != m_liSearchDirectories.end(); ++it )
	{
		const string strFilePath = (*it) + strShaderName;
		const string strSource = LoadSourceFromFile( strFilePath );

		if( !strSource.empty() )
		{
			if(strShaderSource.empty())
			{
				strShaderSource = strSource;

#				ifdef _DEBUG
				vstr::debugi() << "[VistaShaderRegistry] found Shader \""
					<< strShaderName << "\" in \"" << (*it) << "\"." << endl;
#				endif
			}
#			ifndef DISABLE_SHADER_REGISTRY_WARNINGS
			else
			{
				vstr::warnp() << "[VistaShaderRegistry] Found Shader \""
					<< strShaderName << "\" in \"" << (*it) << "\", but Shader"
					<< " has already been found in a different directory!" 
					<< endl;
			}
#			endif
		}
	} // Loop search dirs.

#	ifndef DISABLE_SHADER_REGISTRY_WARNINGS
	if (strShaderSource.empty())
	{
		vstr::warnp() << "[VistaShaderRegistry] - can't find \""<< strShaderName << "\"  in any of the  following directories:" << endl;
		vstr::IndentObject oIndent;
		for( it = m_liSearchDirectories.begin(); it != m_liSearchDirectories.end(); ++it )
		{
			vstr::warni() << *it << endl;
		} // Loop search dirs.
	}
#	endif

	return strShaderSource;
}
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
