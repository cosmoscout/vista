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


/**
 *      Purpose  :  Access to "FileDataSets". A dataset is an
 *                  organized group of related files.
 *
 *      CDataSet manages a filelist, which can be generated depending on the
 *      type of dataset you use. This filelist can be accessed with an
 *      CDataSet::iterator.
 *
 *      A Dataset may have one of four types:
 *        DS_NOTSET     unspecified type, only used during construction
 *                      on use of first type-specific method, the type
 *                      will be set (see documentation of methods which
 *                      of them will set a DS_NOTSET to their type
 *
 *        DS_FILELIST   use AddFile to add several files to the dataset-
 *                      filelist. These files may have nothing in common.
 *                      Example: AddFile ("C:\work\engine_20_01.vtk")
 *                               AddFile ("O:\work\turbine_100.vtk")
 *                               AddFile ("X:\vr\bluntfin.q")
 *
 *        DS_DIRECTORY  this dataset is specified by a directory containing
 *                      all files of the dataset. All files in the directory
 *                      will be in the dataset, too !
 *                      Use SetDirectory to set the directory path,
 *                      to update the filelist fom the directory call
 *                      UpdateDirectory.
 *                      Example:   SetDirectory ("C:\work\engine")
 *
 *
 *        DS_PATTERN    this type is the most complicated, but useful.
 *                      First, you have to define several CWildcards, which
 *                      will be replaced by values in the resulting filenames.
 *                      Second, you define a patternstring as basepattern,
 *                      containing the symbols from the previous defined
 *                      CWildcards.
 *                      Valid filenames are filenames which match the pattern
 *                      and the filelist contains all valid filenames using
 *                      values from [pattern.MinRange,pattern.MaxRange].
 *
 *        Some methods are only useful with one type of datasets. If you
 *        create a DS_DIRECTORY and try to extract a pattern, you will get
 *        a failed assertion. So, be careful with using datasets, be always
 *        aware of their type.
 */

#ifndef VISTAFILEDATASET_H
#define VISTAFILEDATASET_H

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

#include "VistaFileSystemFile.h"
#include "VistaFileSystemDirectory.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cassert>

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
#define OWN_ITERATOR
/*============================================================================*/
/* CWildcard		                                                          */
/*																			  */
/*																			  */
/*																			  */
/*============================================================================*/

class VISTATOOLSAPI VistaWildcard
{
  /**
   *  A CWildcard consists of
   *	 1. symbol [char]		this symbol will be replaced by a value
   *	 2. nDigits				minimal number of digits a value has
   *	 3. MinRange			minimal value
   *	 4. MaxRange			maximal value
   *
   */

public:
	VistaWildcard () {};
	VistaWildcard (char cSymbol, int nDigits, int nMin, int nMax)
		: m_cSymbol(cSymbol),m_nDigits (nDigits), m_nMinRange (nMin), m_nMaxRange (nMax)
	{};

	// no set/get methods, direct access

	char	m_cSymbol;
	int		m_nDigits;
	int		m_nMinRange;
	int		m_nMaxRange;

	friend std::ostream& operator<<(std::ostream& out,VistaWildcard &pattern);
	friend std::istream& operator>>(std::istream& in, VistaWildcard &pattern);

};

class VISTATOOLSAPI VistaFileDataSet
 {
  /**
   * Access the methods of those classes by using
   * '(*iterator_variable)->MethodName()'
   *
   * These iterators are only list::iterators from the filelist,
   * its further work to implement an own iterator for CDataSet.
   *
   */
	// iterator specific
public:

#ifdef OWN_ITERATOR

	class iterator;
	friend class iterator;
	class iterator
		: public std::iterator<std::bidirectional_iterator_tag,
							   VistaFileSystemFile,std::ptrdiff_t>
	{
		std::list<VistaFileSystemFile>::iterator it;
		std::list<VistaFileSystemFile>* r;
	public:
		// "typename" necessary to resolve nesting:
		iterator(std::list<VistaFileSystemFile>& lst,
			const std::list<VistaFileSystemFile>::iterator& i)
			: r(&lst), it(i) {}
		bool operator==(const iterator& x) const {
			return it == x.it;
		}
		bool operator!=(const iterator& x) const {
			return !(*this == x);
		}
		std::list<VistaFileSystemFile>::reference operator*() const {
			return *it;
		}
		iterator& operator++() {
			if(it != r->end())
				++it;
			return *this;
		}
		iterator operator++(int) {
			iterator tmp = *this;
			++*this;
			return tmp;
		}
		iterator& operator--() {
			if(it != r->begin())
				--it;
			return *this;
		}
		iterator operator--(int) {
			iterator tmp = *this;
			--*this;
			return tmp;
		}
		iterator insert(const VistaFileSystemFile& x){
			return iterator(*r, r->insert(it, x));
		}
		iterator erase() {
			return iterator(*r, r->erase(it));
		}
	};
	void push_back(const VistaFileSystemFile& x) {
		m_listFiles.push_back(x);
	}
	iterator begin() {
		return iterator(m_listFiles, m_listFiles.begin());
	}
	iterator end() {
		return iterator(m_listFiles, m_listFiles.end());
	}
	int size() { return (int)m_listFiles.size(); }


#else
	typedef std::list<VistaFileSystemFile>::iterator iterator;
	typedef std::list<VistaFileSystemFile>::const_iterator const_iterator;

	inline iterator begin() { return m_listFiles.begin(); }
	inline iterator end() { return m_listFiles.end(); }
	inline size_t   size() { return m_listFiles.size(); }
#endif

	// types
	enum DATASET_TYPE { DS_NOTSET = -1, DS_PATTERN = 0, DS_DIRECTORY, DS_FILELIST};

	// class methods
public:
	/**
	* Constructor/destructor
	*
	*	On construction you can decide which type of dataset will be constructed or
	*	construct a dataset with unspecified type.
	*	A type will be specified with use of the first method.
	*
	* @param	type			type of dataset to be constructed
	*/
	VistaFileDataSet():m_nType (DS_NOTSET)
	{
		m_pDirectory = NULL;
		m_bSet = false;
	};

	VistaFileDataSet(DATASET_TYPE type):m_nType (type)
	{
		m_pDirectory = NULL;
		m_bSet = false;
	};

	// copy constr.
	VistaFileDataSet(VistaFileDataSet &copy);

	virtual ~VistaFileDataSet();

	/** [DS_PATTERN]
	* SetPattern
	*
	*	Sets filename pattern for DS_PATTERN dataset.
	*
	* @param	string			set filename pattern
	*/
	void SetPatternString (std::string strPattern);

	/** [DS_PATTERN]
	* AddWildcard
	*
	*	Adds wildcard to wildcardlist.
	*	This method will set a DS_NOTSET to DS_PATTERN.
	*
	* @param	VistaWildcard			wildcard to use
	*/
	void AddWildcard (VistaWildcard wildcard);

	/** [DS_PATTERN]
	* GetFileNameX
	*
	*	Get VistaFileSystemFile to filename specified by the given patternstring, wildcards
	*	and the parameters ...valueX-1, valueX.
	*
	*	GetFileName1 will replace the first added wildcard by value1 (to get a valid filename, this must be the only wildcard)
	*	....
	*	GetFileNameA will use an array with 'size' values to replace a number of 'size' wildcards
	*
	* @param	int							value to replace wildcard
	* @RETURN	VistaFileSystemFile*				new VistaFileSystemFile pointer with requested filename
	*										if you use GetFileName with incorrect symbolnumber
	*										this filename could be invalid
	*/
	VistaFileSystemFile GetFileName1 (int value1);
	VistaFileSystemFile GetFileName2 (int value1, int value2);
	VistaFileSystemFile GetFileName3 (int value1, int value2, int value3);
	VistaFileSystemFile GetFileNameA (int* value, int size);

	/** [DS_DIRECTORY]
	* SetDirectory
	*
	*	Sets type to DS_DIRECTORY and constructs filelist from given Directory path.
	*	This method will set a DS_NOTSET to DS_DIRECTORY.
	*
	* @param	string				pathname of Directory
	*/
	void SetDirectory (std::string strDirectory);

	/** [DS_DIRECTORY]
	* UpdateDirectory
	*
	*	This method will clear the filelist and read all files new from the given directory
	*
	*/
	void UpdateDirectory ();

	/** [DS_FILELIST]
	* AddFile
	*
	*	Adds a file (by filename or pointer to VistaFileSystemFile) to filelist.
	*	This method will set a DS_NOTSET to DS_FILELIST.
	*
	* @param	string/VistaFileSystemFile*				filename/pointer to existing VistaFileSystemFile
	*/
	void AddFile (std::string strFilename);
	void AddFile (VistaFileSystemFile pFile);


	/**
	* CheckFileName
	*
	*	Checks, if specified filename is in filelist
	*
	* @param	string					filename to check
	* @RETURN	bool					is filename in dataset ?
	*
	*/
	bool CheckFileName (std::string strFilename);

	/**
	* operator==
	*
	*	compares two datasets
	*
	* @param	CDataSet				right hand size of comparison
	* @RETURN	bool					are datasets equal ?
	*/
	bool operator== (const VistaFileDataSet& rhs);

protected:
	/** [DS_PATTERN]
	* CheckPattern
	*
	*	Checks, if all wildcardss in wildcardlist appear in patternstring
	*
	* @RETURN	bool				do all patterns appear in patternstring ?
	*/
	bool CheckPattern ();

	/** [DS_PATTERN]
	* MatchPattern
	*
	*	Replaces an occuring wildcard in patternstring with a symbol(=value)
	*
	* @param	string				patternstring
	* @param	int					value to replace wildcard in patternstring with
	* @param	VistaWildcard			wildcard in patternstring which is replaced
	* @RETURN	string				resulting patternstring
	*/
	std::string MatchPattern (std::string inStrPattern, int symbol, VistaWildcard pattern);

	/** [DS_PATTERN]
	* MatchRecursivePattern
	*
	*	Replaces recursively all wildcards in strFile from wildcard *itPattern on to
	*	last entry of wildcardlist.
	*	For each wildcard all symbols from MinRange to MaxRange are replaced, if all
	*	wildcards are resolved recursively, the resulting filename is pushed back
	*	into the filelist.
	*
	*	So, a call (m_strPattern, first entry of list) will recursively generate all
	*	filenames using the wildcardlist and push them into the filelist.
	*
	* @param	string				patternstring
	* @param	iterator			iterator of current wildcard in patternlist
	* @RETURN	bool				last entry in patternlist not reached ?
	*/
	bool MatchRecursivePattern (std::string strFile, std::list <VistaWildcard>::iterator itPattern);

	/**
	* Copy
	*
	*	Copies one dataset to this object.
	*
	* @param	CVceDataSet			object to copy from
	*
	*/
	void CopyDataSet (VistaFileDataSet& dataset);

private:
	// dataset type
	DATASET_TYPE				m_nType;

	// was Set method (SetPattern/SetDirectory) called ?
	// you can't do this twice !
	bool						m_bSet;

	// type PATTERN
	std::list <VistaWildcard>		m_listWildcards;
	std::string					m_strPattern;

	//type DIRECTORY
	VistaFileSystemDirectory*		m_pDirectory;

	//type FILELIST
	std::list <VistaFileSystemFile>	m_listFiles;

public:
	/** <<, >>
	* Purpose: streaming operator for CDataSet
	* @RETURN:
	*
	*/
	friend std::ostream& operator<<(std::ostream& out,VistaFileDataSet &data);
	friend std::istream& operator>>(std::istream& in, VistaFileDataSet &data);

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //TOOLSFILEDATASET_H


