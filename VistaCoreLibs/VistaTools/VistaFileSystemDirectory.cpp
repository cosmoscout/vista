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



/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaFileSystemDirectory.h"
#include "VistaFileSystemFile.h"

#include <sys/stat.h>
#include <sys/types.h>

#ifdef WIN32
#include <Windows.h>      // file access in windows
#include <direct.h>
#else
#include <dirent.h>       // file access in unix
#include <unistd.h>
#include <sys/types.h>
#include <fnmatch.h>


static char *scPattern = 0;
static int wcFilter(const struct dirent *dir_entry)
{
	if(scPattern)
		return fnmatch(scPattern, dir_entry->d_name, 0) ? 0 : 1;
	else
		return 1; // match all
}

#endif

#include <cstdlib>

#define MAXPATHLEN 4096

using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaFileSystemDirectory::VistaFileSystemDirectory(const string &dir_name) :
VistaFileSystemNode(dir_name),
m_bIsParsed(false),
m_sPattern("*")
{
}

VistaFileSystemDirectory::~VistaFileSystemDirectory()
{
	ClearEntries(); // get rid of stored pointers
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/


int VistaFileSystemDirectory::NumberOfEntries()
{
	ReadEntries();
	return (int)m_Entries.size();
}


int VistaFileSystemDirectory::NumberOfEntries(int ft)
{
	ReadEntries();
	int i=0;

	vector< Pair >::iterator it = m_Entries.begin();
	for(; it!=m_Entries.end(); ++it)
		if (it->second & ft)
			++i;

	return i;
}


bool VistaFileSystemDirectory::Create()
{
	if (!Exists())
#ifdef WIN32  // ##### WINDOWS VERSION ########################################
		return _mkdir(GetName().c_str()) == 0 ? true : false;
#else        // ##### UNIX VERSION ############################################
		return mkdir(GetName().c_str(), S_IRWXU|S_IRGRP|S_IXGRP) == 0 ? true : false;
#endif

	return false;
}


bool VistaFileSystemDirectory::Delete()
{
	if (Exists())
#ifdef WIN32  // ##### WINDOWS VERSION ########################################
		return _rmdir(GetName().c_str()) == 0 ? true : false;
#else        // ##### UNIX VERSION ############################################
		return rmdir(GetName().c_str()) == 0 ? true : false;
#endif

	/** @todo  : delete directory recursively */

	return false;
}

string VistaFileSystemDirectory::GetOSSpecificSeparator()
{
#if defined(WIN32)
	return std::string("\\");
#else
	return std::string("/");
#endif
}

bool VistaFileSystemDirectory::Exists() const
{
#ifdef WIN32  // ##### WINDOWS VERSION ########################################

	struct _stat attributes;

	if ( _stat(GetName().c_str(), &attributes) != 0 )
		return false;

	if ( attributes.st_mode & _S_IFDIR )
		return true;
	else
		return false;

#else        // ##### UNIX VERSION ############################################

	struct stat attributes;

	if (  stat(GetName().c_str(), &attributes) != 0 )
		return false;

	if ( attributes.st_mode & S_IFDIR )
		return true;
	else
		return false;

#endif
}

bool VistaFileSystemDirectory::SetCurrentWorkingDirectory(const string &sDirName)
{
#ifdef WIN32
	return !sDirName.empty() && SetCurrentDirectory(sDirName.c_str());
#else
	return !sDirName.empty() && chdir(sDirName.c_str())==0;
#endif
}

string VistaFileSystemDirectory::GetCurrentWorkingDirectory()
{
	char buffer[MAXPATHLEN];
#ifndef WIN32
	if(getcwd(buffer,MAXPATHLEN)) return string(buffer);
#else
	if(GetCurrentDirectory(MAXPATHLEN,buffer)) return string(buffer);
#endif
	return "";
}


string VistaFileSystemDirectory::GetEntries(int ft)
{
	ReadEntries();

	string s;
	vector< Pair >::iterator it = m_Entries.begin();
	for(; it!=m_Entries.end(); ++it) {
		if (it->second & ft)
			s = s + it->first + '\n';
	}

	return s;
}


vector<string> VistaFileSystemDirectory::GetEntriesVector(int ft)
{
	ReadEntries();

	vector<string> v;
	vector< Pair >::iterator it = m_Entries.begin();
	while(it != m_Entries.end()) {
		v.push_back(it->first);
		++it;
	}

	return v;
}

void VistaFileSystemDirectory::SetPattern(const std::string &sPattern)
{
	m_sPattern = sPattern;
}

std::string VistaFileSystemDirectory::GetPattern() const
{
	return m_sPattern;
}

std::string VistaFileSystemDirectory::CreateStorageName(const std::string &sFileName) const
{
	return GetName() + GetOSSpecificSeparator() + sFileName;
}

bool VistaFileSystemDirectory::AddFileName(const std::string &sFileName, int iType)
{
	m_Entries.push_back( Pair(sFileName, iType) );
	if(iType == FT_FILE)
		m_EntryList.push_back( new VistaFileSystemFile(sFileName) );
	else if(iType == FT_DIRECTORY)
		m_EntryList.push_back( new VistaFileSystemDirectory(sFileName) );

	return true;
}

int VistaFileSystemDirectory::ReadEntries()
{
	if (!m_bIsParsed) {

		ClearEntries();
#ifdef WIN32  // ##### WINDOWS VERSION ########################################

		/** @todo change \\ to / !!! */
		struct _stat attributes;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFile;
		hFile = FindFirstFile( CreateStorageName(m_sPattern).c_str(), &FindFileData );

		if(hFile != INVALID_HANDLE_VALUE)
		{
			// ok, we found a first file

			do
			{
				if ( FindFileData.cFileName != string(".") &&
					FindFileData.cFileName != string("..") )
				{
					std::string sFName = CreateStorageName(FindFileData.cFileName);
					_stat( sFName.c_str() , &attributes );
					if (attributes.st_mode & _S_IFREG)
					{
						AddFileName(sFName, FT_FILE);
					}
					else if (attributes.st_mode & _S_IFDIR)
					{
						AddFileName(sFName, FT_DIRECTORY);
					}
					else
						AddFileName(sFName, FT_UNKNOWN);
				}
			}
			while( FindNextFile(hFile, &FindFileData) );
			FindClose(hFile);
		};
#else        // ##### UNIX VERSION ############################################

		//    DIR *dir;
		struct dirent **namelist;
		int n = 0;

		struct stat   attributes;

		//    if ( ( dir = opendir( GetName().c_str() ) ) == NULL )
		//      return 0;

		// @TODO: make this thread-safe!
		// we should not do this in multiple threads, though!
		scPattern = &m_sPattern[0];

		n = scandir(GetName().c_str(), &namelist, wcFilter, alphasort);
		if(n<0)
			return 0;
		else
		{
			while(n--)
			{
				struct dirent *dir_entry = namelist[n];
				if ( dir_entry->d_name != string(".") &&
					dir_entry->d_name != string("..") )
				{
					stat( (GetName() + "/" + string(dir_entry->d_name)).c_str() , &attributes );
					if ( ( attributes.st_mode & S_IFREG ) || ( attributes.st_mode & S_IFCHR ) )
					{
						m_Entries.push_back( Pair((GetName() + "/" + string(dir_entry->d_name)), FT_FILE) );
						m_EntryList.push_back( new VistaFileSystemFile((GetName() + "/" + string(dir_entry->d_name))) );
					}
					else if (attributes.st_mode & S_IFDIR)
					{
						m_Entries.push_back( Pair((GetName() + "/" + string(dir_entry->d_name)), FT_DIRECTORY) );
						m_EntryList.push_back( new VistaFileSystemDirectory((GetName() + "/" + string(dir_entry->d_name))) );
					}
					else
						m_Entries.push_back( Pair((GetName() + "/" + string(dir_entry->d_name)), FT_UNKNOWN) );
				}


				free(dir_entry); // give back resources
			}
			free(namelist); // give back resources
		};


		/*    while( (dir_entry = readdir(dir)) != NULL ) {
		if ( dir_entry->d_name != string(".") &&
		dir_entry->d_name != string("..") )
		{

		stat( (GetName() + "/" + string(dir_entry->d_name)).c_str() , &attributes );
		if (attributes.st_mode & S_IFREG) {
		m_Entries.push_back( Pair((GetName() + "/" + string(dir_entry->d_name)), FT_FILE) );
		m_EntryList.push_back( new VistaFileSystemFile((GetName() + "/" + string(dir_entry->d_name))) );
		} else if (attributes.st_mode & S_IFDIR) {
		m_Entries.push_back( Pair((GetName() + "/" + string(dir_entry->d_name)), FT_DIRECTORY) );
		m_EntryList.push_back( new VistaFileSystemDirectory((GetName() + "/" + string(dir_entry->d_name))) );
		} else
		m_Entries.push_back( Pair((GetName() + "/" + string(dir_entry->d_name)), FT_UNKNOWN) );
		}
		}

		closedir(dir);
		*/
#endif

		m_bIsParsed = true;
	}

	return (int)m_Entries.size();
}

long VistaFileSystemDirectory::GetSize()
{
	m_bIsParsed = false;
	ReadEntries();

	long nAccumulatedSize = 0;

	iterator itFile = m_EntryList.begin ();
	while (itFile != m_EntryList.end())
	{
		nAccumulatedSize += (*itFile)->GetSize();
		++itFile;
	}


	return nAccumulatedSize;
}

void VistaFileSystemDirectory::ClearEntries()
{
	for(list< VistaFileSystemNode* >::iterator it = m_EntryList.begin();
		it != m_EntryList.end(); ++it)
	{
		delete *it;
	}

	m_EntryList.clear();
	m_Entries.clear();
}
