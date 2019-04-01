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

#include "VistaFileDataSet.h"

#include <VistaBase/VistaStreamUtils.h>

#include <sstream>
#include <cassert>    // assert()
#include <iostream>
using namespace std;

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

std::ostream& operator<<(std::ostream& out,VistaFileDataSet &data)
{
	assert (data.m_nType != VistaFileDataSet::DS_NOTSET);

	out << data.m_nType << " ";
	switch (data.m_nType)
	{
	case VistaFileDataSet::DS_PATTERN:
		{
				out << data.m_listWildcards.size () << " ";
				std::list<VistaWildcard>::iterator itWildcard = data.m_listWildcards.begin();
				while (itWildcard != data.m_listWildcards.end())
				{
					out << (*itWildcard)<<" ";
					itWildcard++;
				}
				out << data.m_strPattern;
				break;
		}
	case VistaFileDataSet::DS_DIRECTORY:
		{
				out << data.m_pDirectory->GetName ();
				break;
		}
	case VistaFileDataSet::DS_FILELIST:
		{
				out << data.m_listFiles.size()<<" ";
				std::list<VistaFileSystemFile>::iterator itFile = data.m_listFiles.begin();
				while (itFile != data.m_listFiles.end())
				{
					out << (*itFile).GetName()<<" ";
					itFile++;
				}
				break;
		}
	default: break;
	}

	return out;
}

std::istream& operator>>(std::istream& in, VistaFileDataSet &data)
{
	int type;
	in >> type;

	assert ((type==data.m_nType)||(data.m_nType==VistaFileDataSet::DS_NOTSET));
	if (data.m_nType==VistaFileDataSet::DS_NOTSET)
		data.m_nType = (VistaFileDataSet::DATASET_TYPE) type;

	//vstr::outd << "Dataset type " << type << std::endl;

	switch (type)
	{
	case VistaFileDataSet::DS_PATTERN:
		{
				int size = 0;
				in >> size;
				data.m_listWildcards.clear();
				for (int i=0; i < size; ++i)
				{
					VistaWildcard wildcard;
					in >> wildcard;
					data.AddWildcard (wildcard);
				}
				std::string strPattern;
				in >> strPattern;
				data.SetPatternString (strPattern);
				break;
		}
	case VistaFileDataSet::DS_DIRECTORY:
		{
				std::string strPath;
				in >> strPath;
				data.SetDirectory (strPath);
				break;
		}
	case VistaFileDataSet::DS_FILELIST:
		{
				int size = 0;
				in >> size;
				data.m_listFiles.clear();
				for (int i=0; i < size; ++i)
				{
					std::string strFilename ("");
					in >> strFilename;
					data.AddFile (strFilename);
				}

				break;
		}
	}

	return in;

}

std::ostream& operator<<(std::ostream& out,VistaWildcard &wildcard)
{
	out << wildcard.m_cSymbol << " " << wildcard.m_nDigits << " " << wildcard.m_nMinRange << " " << wildcard.m_nMaxRange;
	return out;
}

std::istream& operator>>(std::istream& in, VistaWildcard &wildcard)
{
	//out << m_cSymbol << " " << m_nDigits << " " << m_nMinRange << " " << m_nMaxRange;
	in >> wildcard.m_cSymbol;
	in >> wildcard.m_nDigits;
	in >> wildcard.m_nMinRange;
	in >> wildcard.m_nMaxRange;
	return in;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaFileDataSet::~VistaFileDataSet ()
{
	if (m_pDirectory)
		delete m_pDirectory;
	m_pDirectory = NULL;

/*	std::list <VistaFileSystemFile>::iterator itEntry = m_listFiles.begin();
	while (itEntry != m_listFiles.end())
	{
		if ((*itEntry))
			delete (*itEntry);
		(*itEntry) = NULL;
		itEntry = m_listFiles.erase (itEntry);
	}
	*/
}


VistaFileDataSet::VistaFileDataSet (VistaFileDataSet &copy)
{
	this->CopyDataSet (copy);
}

/*============================================================================*/
/*  DS_PATTERN methods                                                        */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :  SetPatternString			                                  */
/*                                                                            */
/*============================================================================*/

void VistaFileDataSet::SetPatternString (std::string strPattern)
{
	assert (m_nType == DS_PATTERN);
	assert (m_bSet == false);

	m_strPattern = strPattern;

	MatchRecursivePattern (strPattern,m_listWildcards.begin());

	m_bSet = true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  AddWildcard				                                  */
/*                                                                            */
/*============================================================================*/
void VistaFileDataSet::AddWildcard (VistaWildcard wildcard)
{
	assert ((m_nType == DS_PATTERN)||(m_nType==DS_NOTSET));
	if (m_nType==DS_NOTSET)
		m_nType = DS_PATTERN;

	// check pattern
	if (wildcard.m_nMinRange <= wildcard.m_nMaxRange)
		m_listWildcards.push_back (wildcard);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  GetFilename				                                  */
/*                                                                            */
/*============================================================================*/

VistaFileSystemFile VistaFileDataSet::GetFileName1 (int value1)
{
	assert (m_nType == DS_PATTERN);
	std::string strFile = m_strPattern;
	std::list<VistaWildcard>::iterator itWildcard = m_listWildcards.begin();
	if (itWildcard != m_listWildcards.end())
		strFile = MatchPattern (strFile,value1,(*itWildcard));
	
	return  VistaFileSystemFile (strFile);
}

VistaFileSystemFile VistaFileDataSet::GetFileName2 (int value1, int value2)
{
	assert (m_nType == DS_PATTERN);
	std::string strFile = m_strPattern;
	std::list<VistaWildcard>::iterator itWildcard = m_listWildcards.begin();
	if (itWildcard != m_listWildcards.end())
		strFile = MatchPattern (strFile,value1,(*itWildcard));
	itWildcard++;
	if (itWildcard != m_listWildcards.end())
		strFile = MatchPattern (strFile,value2,(*itWildcard));
	
	return VistaFileSystemFile (strFile);
}

VistaFileSystemFile VistaFileDataSet::GetFileName3 (int value1, int value2, int value3)
{
	assert (m_nType == DS_PATTERN);
	std::string strFile = m_strPattern;
	std::list<VistaWildcard>::iterator itWildcard = m_listWildcards.begin();
	if (itWildcard != m_listWildcards.end())
		strFile = MatchPattern (strFile,value1,(*itWildcard));
	itWildcard++;
	if (itWildcard != m_listWildcards.end())
		strFile = MatchPattern (strFile,value2,(*itWildcard));
	itWildcard++;
	if (itWildcard != m_listWildcards.end())
		strFile = MatchPattern (strFile,value3,(*itWildcard));
	
	return VistaFileSystemFile (strFile);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  GetFileNameA				                                  */
/*                                                                            */
/*============================================================================*/

VistaFileSystemFile VistaFileDataSet::GetFileNameA (int* value, int size)
{
	assert (m_nType == DS_PATTERN);
	std::string strFile = m_strPattern;
	std::list<VistaWildcard>::iterator itWildcard = m_listWildcards.begin();
	int i = 0;
	while ((itWildcard != m_listWildcards.end())&&(i < size))
	{
		strFile = MatchPattern (strFile,value[i],(*itWildcard));
		itWildcard++;
		i++;
	}

	return  VistaFileSystemFile (strFile);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  CheckPattern				                                  */
/*                                                                            */
/*============================================================================*/

bool VistaFileDataSet::CheckPattern ()
{
	bool ok = true;
	std::list <VistaWildcard>::iterator itWildcard = m_listWildcards.begin ();
	while ((itWildcard != m_listWildcards.end())&&(ok))
	{
		if (m_strPattern.find_first_of((*itWildcard).m_cSymbol) == 0)
			ok = false;
		else
			++itWildcard;
	}
	return ok;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  MatchPattern				                                  */
/*                                                                            */
/*============================================================================*/

std::string VistaFileDataSet::MatchPattern (std::string strPattern, int symbol, VistaWildcard pattern)
{
	int pos = (int)strPattern.find_first_of(pattern.m_cSymbol);

	std::ostringstream strReplace;
	int test = 1;
	for (int i=1; i<pattern.m_nDigits; ++i)
		test *= 10;

	while (symbol < test)
	{
		strReplace << "0";
		test /= 10;

	}
	strReplace << symbol;

	strPattern.replace (pos,1,strReplace.str());
	return strPattern;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  MatchRecursivePattern		                                  */
/*                                                                            */
/*============================================================================*/

bool VistaFileDataSet::MatchRecursivePattern (std::string strFile, std::list <VistaWildcard>::iterator itWildcard)
{
	if (itWildcard == m_listWildcards.end ())
	{
		// if no more pattern to replace, push back file in list
		m_listFiles.push_back (VistaFileSystemFile (strFile));
		return false;
	}
	else
	{
		// get current pattern, inc iterator
		VistaWildcard currentWildcard = (*itWildcard);
		itWildcard++;

		// loop over range of pattern
		for (int i = currentWildcard.m_nMinRange; i <= currentWildcard.m_nMaxRange; i++)
		{
			// insert current symbol in strFile
			std::string tmpFile = MatchPattern (strFile,i,currentWildcard);
			MatchRecursivePattern (tmpFile,itWildcard);
		}
		return true;
	}
}

/*============================================================================*/
/*  DS_DIRECTORY methods                                                      */
/*============================================================================*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :  SetDirectory				                                  */
/*                                                                            */
/*============================================================================*/

void VistaFileDataSet::SetDirectory (std::string strDirectory)
{
	assert ((m_nType == DS_DIRECTORY)||(m_nType==DS_NOTSET));
	if (m_nType==DS_NOTSET)
		m_nType = DS_DIRECTORY;
	assert (m_bSet == false);

	m_pDirectory = new VistaFileSystemDirectory(strDirectory);

	UpdateDirectory ();
	m_bSet = true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  UpdateDirectory			                                  */
/*                                                                            */
/*============================================================================*/

void VistaFileDataSet::UpdateDirectory ()
{
	assert (m_nType == DS_DIRECTORY);

	m_listFiles.clear ();

	// NOTE: work around: getsize will set m_bParse to false and read all files new
	m_pDirectory->GetSize ();

	// copy list from directory
	VistaFileSystemDirectory::iterator itFile = m_pDirectory->begin ();
	while (itFile != m_pDirectory->end ())
	{
		if ((*itFile)->GetType() == VistaFileSystemNode::FT_FILE)
			m_listFiles.push_back (*(static_cast<VistaFileSystemFile*> (*itFile)));
		++itFile;
	}

	// write filelist in m_listFiles
}

/*============================================================================*/
/*  DS_FILELIST methods                                                       */
/*============================================================================*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :  AddFile					                                  */
/*                                                                            */
/*============================================================================*/

void VistaFileDataSet::AddFile (std::string strFilename)
{
	assert ((m_nType == DS_FILELIST)||(m_nType==DS_NOTSET));
	if (m_nType==DS_NOTSET)
		m_nType = DS_FILELIST;

	m_listFiles.push_back ( VistaFileSystemFile (strFilename));
}

void VistaFileDataSet::AddFile (VistaFileSystemFile pFile)
{
	assert ((m_nType == DS_FILELIST)||(m_nType==DS_NOTSET));
	if (m_nType==DS_NOTSET)
		m_nType = DS_FILELIST;

	m_listFiles.push_back (pFile);
}

/*============================================================================*/
/*  methods for all types                                                     */
/*============================================================================*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :  CheckFileName				                                  */
/*                                                                            */
/*============================================================================*/

bool VistaFileDataSet::CheckFileName (std::string strFilename)
{
	
	// check, if strFilename belongs to dataset
	std::list <VistaFileSystemFile>::iterator itFile = m_listFiles.begin ();
	while (itFile != m_listFiles.end())
	{
		if ((*itFile).GetName () == strFilename)
			return true;
		else
			++itFile;
	}

	return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  ==							                                  */
/*                                                                            */
/*============================================================================*/

bool VistaFileDataSet::operator== (const VistaFileDataSet& rhs)
{
	bool bEqual = (m_nType == rhs.m_nType);

	if (bEqual)
	{
		switch (m_nType)
		{
			case DS_PATTERN: 
			{
				// same pattern ?
				if (m_strPattern != rhs.m_strPattern)
					bEqual=false;
				else
				{
					// same size ?
					if (m_listWildcards.size() != rhs.m_listWildcards.size())
						bEqual = false;
					else
					{
						// same content ? (order may not be different)
						std::list<VistaWildcard>::iterator itWildcard = m_listWildcards.begin();
						std::list<VistaWildcard>::const_iterator itWildcard2 = rhs.m_listWildcards.begin();
						while ((itWildcard != m_listWildcards.end())&&(itWildcard2 != rhs.m_listWildcards.end())&&bEqual)
						{
								if (!(((*itWildcard).m_cSymbol==(*itWildcard2).m_cSymbol)
									&&((*itWildcard).m_nDigits==(*itWildcard2).m_nDigits)
									&&((*itWildcard).m_nMinRange==(*itWildcard2).m_nMinRange)
									&&((*itWildcard).m_nMaxRange==(*itWildcard2).m_nMaxRange)
									))
									bEqual = false;
							
							itWildcard++;
							itWildcard2++;
						}
					}
				}


				break;
			}
			case DS_DIRECTORY: 
			{
				// same directory ?
				if (m_pDirectory->GetName() != rhs.m_pDirectory->GetName ())
					bEqual=false;
				break;
			}
			case DS_FILELIST: 
			{
				// same size ?
				if (m_listFiles.size() != rhs.m_listFiles.size())
					bEqual = false;
				else
				{
					// same content ? (order may be different)
					std::list<VistaFileSystemFile>::iterator itFile = m_listFiles.begin();
					while ((itFile != m_listFiles.end())&&bEqual)
					{
						bool bFound = false;
						std::list<VistaFileSystemFile>::const_iterator itFile2 = rhs.m_listFiles.begin();
						while ((itFile2 != rhs.m_listFiles.end())&&(!bFound))
						{
							if ((*itFile).GetName() == (*itFile2).GetName())
								bFound = true;
							itFile2++;
						}
						bEqual = bFound;
						
						itFile++;
					}
					
				}
				break;
			}
			default:
			{
				bEqual = false;
				break;
			}
		}
	}

	return bEqual;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :  CopyDataSet				                                  */
/*                                                                            */
/*============================================================================*/

void VistaFileDataSet::CopyDataSet (VistaFileDataSet& copy)
{
	m_nType = copy.m_nType;
	m_bSet  = false;

	m_strPattern = "";
	m_listWildcards.clear ();
	m_pDirectory = NULL;

	switch (m_nType)
	{
	case DS_NOTSET:
		{
			break;
		}
	case DS_DIRECTORY:
		{
			SetDirectory (copy.m_pDirectory->GetName());

			break;
		}
	case DS_PATTERN:
		{
			m_listWildcards = copy.m_listWildcards;
			SetPatternString (copy.m_strPattern);
			break;
		}
	case DS_FILELIST:
		{
			std::list <VistaFileSystemFile>::iterator itEntry = copy.m_listFiles.begin();
			while (itEntry != copy.m_listFiles.end())
			{
				m_listFiles.push_back (VistaFileSystemFile ((*itEntry).GetName()));
				itEntry++;
			}

			break;
		}

	}
	m_bSet  = copy.m_bSet;

}
