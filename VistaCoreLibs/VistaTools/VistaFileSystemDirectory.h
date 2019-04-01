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


#ifndef VISTAFILESYSTEMDIRECTORY_H
#define VISTAFILESYSTEMDIRECTORY_H

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaToolsConfig.h"

#include "VistaFileSystemNode.h"

#include <iostream>
#include <string>
#include <vector>
#include <list>

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Encapsulates simple access to a directory on the filesystem on various
 * operating systems.
 *
 * Especially useful for directory listings.
 *
 * A list of all files and directories can be obtained by using the iterators
 * provided by this class. (This class is simply a wrapper around the
 * iterators found in the standard template library list class.)
 *
 */


class VISTATOOLSAPI VistaFileSystemDirectory : public VistaFileSystemNode {

  /**
   * This class implements an iterator which points to
   * 'VistaFileSystemNode*' and delivers a FileSystemFile or
   * a VistaFileSystemDirectory.
   *
   * Access the methods of those classes by using
   * '(*iterator_variable)->MethodName()'
   *
   */
	// iterator specific
public:
	typedef std::list<VistaFileSystemNode*>::iterator iterator;
	typedef std::list<VistaFileSystemNode*>::const_iterator const_iterator;

	inline iterator begin() { ReadEntries(); return m_EntryList.begin(); }
	inline iterator end()   { ReadEntries(); return m_EntryList.end();   }
	inline size_t   size()  { ReadEntries(); return m_EntryList.size();  }


	// class methods
public:
	VistaFileSystemDirectory(const std::string &dir_name);
	virtual ~VistaFileSystemDirectory();


	/* extended functionality */
	/**
	 * Creates a directory on disk with the following
	 * access mode 'drwxr-x---'
	 *
	 */
	virtual bool Create();
	virtual bool Delete();  /** @todo  : delete directories recursively */
	virtual bool Exists() const;

	/* Gets size of file
	*
	*/
   virtual long GetSize();

   int GetNumberOfEntries() { return NumberOfEntries(); }

   static bool SetCurrentWorkingDirectory(const std::string &sDirName);
   static std::string GetCurrentWorkingDirectory();

   static std::string GetOSSpecificSeparator();

   void SetPattern(const std::string &sPattern);
   std::string GetPattern() const;

protected:
	/* new functionality :
	   DELETE THOSE FUNCTIONS AND ONLY PROVIDE ITERATORS !!! */
	std::string GetEntries(int ft = FT_FILE | FT_DIRECTORY );
	std::vector<std::string> GetEntriesVector(int ft = FT_FILE | FT_DIRECTORY );
	int NumberOfEntries();
	int NumberOfEntries(int ft);

	void ClearEntries();

	std::string CreateStorageName(const std::string &sFileName) const;
	bool AddFileName(const std::string &sFileName, int iType);

private:
	bool m_bIsParsed;
	std::list< VistaFileSystemNode* > m_EntryList;

	int ReadEntries();
	typedef std::pair<std::string, int>  Pair;
	std::vector< Pair > m_Entries;    // list of directory entries and file types

	std::string m_sPattern; /**< defaults to '*' */
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //FILESYSTEMDIRECTORY_H

