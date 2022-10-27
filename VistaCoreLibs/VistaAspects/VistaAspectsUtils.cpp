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


#include "VistaAspectsUtils.h"


#if defined(SUNOS)
#include <strings.h>
#endif

#ifdef WIN32
#pragma warning(disable: 4996)
#endif


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

using std::string;
using std::vector;
using std::list;

#if defined(LINUX)
#include <ctype.h>
#endif // for tolower, toupper

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaAspectsConversionStuff::VistaAspectsConversionStuff()
{
}

VistaAspectsConversionStuff::~VistaAspectsConversionStuff()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

float  VistaAspectsConversionStuff::ConvertToFloat1(const string &sValue )
{
	return VistaAspectsConversionStuff::ConvertToFloat(sValue, true);
}


double VistaAspectsConversionStuff::ConvertToDouble(const string &sValue, bool bCheckPoint)
{
	if(bCheckPoint)
	{
		vector<char> vTmp;
		vTmp.reserve(sValue.length());
		for(string::const_iterator cit = sValue.begin();
			cit != sValue.end(); ++cit)
			{
				if(*cit == ',')
					vTmp.push_back('.');
				else
					vTmp.push_back(*cit);
			}

			string sTmp(vTmp.begin(), vTmp.end());

		return atof(sTmp.c_str());
	}

	return atof(sValue.c_str());
}

double VistaAspectsConversionStuff::ConvertToDouble1(const string &sValue )
{
	return ConvertToDouble(sValue, true);
}

double VistaAspectsConversionStuff::ConvertToDouble2(const string &sValue )
{
	return ConvertToDouble(sValue, false);
}

float VistaAspectsConversionStuff::ConvertToFloat(const string &sValue, bool bCheckPoint)
{
	return float(ConvertToDouble(sValue, bCheckPoint));
}

float VistaAspectsConversionStuff::ConvertToFloat2(const string &sValue )
{
	return float(ConvertToDouble(sValue, false));
}

int    VistaAspectsConversionStuff::ConvertToInt(const string &sValue)
{
	int	nResult;

	nResult = 0;

	sscanf(sValue.c_str(),"%i",&nResult);

	return nResult;
}

unsigned int VistaAspectsConversionStuff::ConvertToUnsignedInt(const string &sValue)
{
	unsigned int	nResult;

	nResult = 0;

	sscanf(sValue.c_str(),"%u",&nResult);

	return nResult;
}

bool   VistaAspectsConversionStuff::ConvertToBool(const string &sValue)
{
	string sTemp = ConvertToUpper(sValue);
	if( (sTemp == "ON") || sTemp == "TRUE" || sTemp == "1")
		return true;
	return false;
}

void *	VistaAspectsConversionStuff::ConvertToId(const string &sValue)
{
#ifdef WIN32
	return (void *) _atoi64(sValue.c_str());
#else
	return (void *) atoll(sValue.c_str());
#endif

}

unsigned int VistaAspectsConversionStuff::ConvertToList(const string &sValue, list<string> &liStrings, char cSep, bool bTrim)
{
	if (sValue.empty())
		return 0;

	liStrings.clear();
	string strTemp;

	// split list according to commas (sp?) and remove spaces
	unsigned int iPos0 = 0;
	unsigned int iPos1 = 0;
	unsigned int iPos2 = 0;
	while (iPos0<sValue.size())
	{
		if (bTrim)
		{
			// find first non-space
			while (iPos0 < sValue.size()
				&& isspace(sValue[iPos0]))
				++iPos0;
		}

		// find the separator
		iPos1 = iPos0;
		while (iPos1 < sValue.size()
			&& sValue[iPos1] != cSep)
			++iPos1;

		iPos2 = iPos1;
		if (bTrim)
		{
			// remove trailing spaces
			while (iPos0 < iPos1
				&& isspace(sValue[iPos1-1]))
				--iPos1;
		}

		liStrings.push_back(sValue.substr(iPos0, iPos1-iPos0));
		iPos0 = iPos2+1;
	}
	// did we end with a separator??
	if (iPos2 < sValue.size() && sValue[iPos2] == cSep)
		liStrings.push_back(string());

	return (unsigned int)liStrings.size();
}

unsigned int VistaAspectsConversionStuff::ConvertToList(const string &sValue, list<double> &liDouble, char cSep)
{
	list<string> liStrings;
	if (ConvertToList(sValue, liStrings, cSep))
	{
		liDouble.clear();
		list<string>::iterator itString;
		for (itString=liStrings.begin(); itString!=liStrings.end(); ++itString)
		{
			liDouble.push_back(ConvertToDouble(*itString));
		}
	}
	return (unsigned int)liDouble.size();
}

unsigned int VistaAspectsConversionStuff::ConvertToList(const string &sValue, list<float> &liFloat, char cSep)
{
	list<string> liStrings;
	if (ConvertToList(sValue, liStrings, cSep))
	{
		liFloat.clear();
		list<string>::iterator itString;
		for (itString=liStrings.begin(); itString!=liStrings.end(); ++itString)
		{
			liFloat.push_back((float)ConvertToDouble(*itString));
		}
	}
	return (unsigned int)liFloat.size();
}

unsigned int VistaAspectsConversionStuff::ConvertToList(const string &sValue, list<bool> &liBool, char cSep)
{
	list<string> liStrings;
	if (ConvertToList(sValue, liStrings, cSep))
	{
		liBool.clear();
		list<string>::iterator itString;
		for (itString=liStrings.begin(); itString!=liStrings.end(); ++itString)
		{
			liBool.push_back(ConvertToBool(*itString));
		}
	}
	return (unsigned int)liBool.size();
}

unsigned int VistaAspectsConversionStuff::ConvertToList(const string &sValue, list<int> &liInt, char cSep)
{
	list<string> liStrings;
	if (ConvertToList(sValue, liStrings, cSep))
	{
		liInt.clear();
		list<string>::iterator itString;
		for (itString=liStrings.begin(); itString!=liStrings.end(); ++itString)
		{
			liInt.push_back(ConvertToInt(*itString));
		}
	}
	return (unsigned int)liInt.size();
}

list<string> VistaAspectsConversionStuff::ConvertToStringList(const string &sValue)
{
	list<string> rList;
		VistaAspectsConversionStuff::ConvertToList(sValue, rList);
		return rList;
}

list<double> VistaAspectsConversionStuff::ConvertToDoubleList(const string &sValue)
{
	list<double> rList;
		VistaAspectsConversionStuff::ConvertToList(sValue, rList);
		return rList;
}

list<float> VistaAspectsConversionStuff::ConvertToFloatList(const string &sValue)
{
	list <float> rList;
	VistaAspectsConversionStuff::ConvertToList(sValue, rList);
	return rList;
}

list<int>    VistaAspectsConversionStuff::ConvertToIntList(const string &sValue)
{
	list<int> rList;
		VistaAspectsConversionStuff::ConvertToList(sValue, rList);
		return rList;
}

list<bool>       VistaAspectsConversionStuff::ConvertToBoolList(const string &sValue)
{
	list<bool> rList;
		VistaAspectsConversionStuff::ConvertToList(sValue, rList);
		return rList;
}

string VistaAspectsConversionStuff::ConvertToString(const string &sString)
{
	return sString;
}

string VistaAspectsConversionStuff::ConvertToString(int iVal)
{
	// kids: never do this at home
	char buffer[4096];
	sprintf(buffer, "%d", iVal);
	return buffer;
}

string VistaAspectsConversionStuff::ConvertToString(unsigned int iVal)
{
	// kids: never do this at home
	char buffer[4096];
	sprintf(buffer, "%u", iVal);
	return buffer;
}

string VistaAspectsConversionStuff::ConvertToString(VistaType::sint64  pVal)
{
	char buffer[4096];
#if defined(WIN32) && defined(_MSC_VER)
	_i64toa_s((_int64) pVal, buffer, 4096, 10);
#else
   	sprintf(buffer, "%lld", (unsigned long long) pVal);
#endif
	return buffer;
}

string VistaAspectsConversionStuff::ConvertToString(double  dVal)
{
	// kids: never do this at home
	char buffer[4096];
	sprintf(buffer, "%.15g", dVal);
	return buffer;
}

string VistaAspectsConversionStuff::ConvertToString(bool bVal)
{
	// kids: never do this at home
	if( bVal )
		return "TRUE";
	return "FALSE";
}

string VistaAspectsConversionStuff::ConvertToString(void *pVal)
{
	char buffer[4096];
#if defined(WIN32) && defined(_MSC_VER)
	_i64toa_s((_int64) pVal, buffer, 4096, 10);
#else
	sprintf(buffer, "%lld", (long long) pVal);
#endif
	return buffer;
}

string VistaAspectsConversionStuff::ConvertToLower(const string &sValue)
{
	string strTemp;
	std::transform( sValue.begin(), sValue.end(), std::back_inserter(strTemp), ::tolower);
	return strTemp;
}

string VistaAspectsConversionStuff::ConvertToUpper(const string &sValue)
{
	string strTemp;
	std::transform(sValue.begin(), sValue.end(), std::back_inserter(strTemp), ::toupper );
	return strTemp;
}

string VistaAspectsConversionStuff::RemoveChar(const string &sValue, char cToRemove)
{
	string strResult;
	string::const_iterator strIt;
	for(strIt = sValue.begin(); strIt != sValue.end(); ++strIt)
	{
		if(*strIt != cToRemove)
			strResult += *strIt;
	}
	return strResult;
}

string VistaAspectsConversionStuff::ConvertToString(const list<string> &liStrings)
{
	string strTemp;
	list<string>::const_iterator itString;
	for (itString=liStrings.begin(); itString!=liStrings.end(); ++itString)
	{
		if (strTemp.size())
			strTemp += ", ";

		strTemp += *itString;
	}

	return strTemp;
}

string VistaAspectsConversionStuff::ConvertToString(const list<string> &liStrings, char cSep)
{
	string strTemp;
	list<string>::const_iterator itString;
	for (itString=liStrings.begin(); itString!=liStrings.end(); ++itString)
	{
		if (strTemp.size())
			strTemp += cSep;

		strTemp += *itString;
	}

	return strTemp;
}

string VistaAspectsConversionStuff::ConvertToString(const list<double> &liDouble)
{
	list<string> liStrings;
	list<double>::const_iterator itDouble;
	for (itDouble=liDouble.begin(); itDouble!=liDouble.end(); ++itDouble)
	{
		liStrings.push_back(ConvertToString(*itDouble));
	}
	return ConvertToString(liStrings);
}

string VistaAspectsConversionStuff::ConvertToString(const list<float> &liFloat)
{
	list<string> liStrings;
	list<float>::const_iterator itFloat;
	for (itFloat=liFloat.begin(); itFloat!=liFloat.end(); ++itFloat)
	{
		liStrings.push_back(ConvertToString(*itFloat));
	}
	return ConvertToString(liStrings);
}

string VistaAspectsConversionStuff::ConvertToString(const list<bool> &liBool)
{
	list<string> liStrings;
	list<bool>::const_iterator itBool;
	for (itBool=liBool.begin(); itBool!=liBool.end(); ++itBool)
	{
		liStrings.push_back(ConvertToString(*itBool));
	}
	return ConvertToString(liStrings);
}

string VistaAspectsConversionStuff::ConvertToString(const list<int> &liInt)
{
	list<string> liStrings;
	list<int>::const_iterator itInt;
	for (itInt=liInt.begin(); itInt!=liInt.end(); ++itInt)
	{
		liStrings.push_back(ConvertToString(*itInt));
	}
	return ConvertToString(liStrings);
}

template <class T, class LCIT>
T *ConvertToArrayT(const list<T> &liInput)
{
	if (liInput.empty())
		return NULL;
	T* pTemp = new T[liInput.size()];
	int iPos=0;
	for (LCIT cit=liInput.begin();
		cit!=liInput.end(); ++cit, ++iPos)
	{
		pTemp[iPos] = (*cit);
	};
	return pTemp;
}

bool *VistaAspectsConversionStuff::ConvertToArray(const list<bool> &liInput)
{
	return ConvertToArrayT<bool, list<bool>::const_iterator>(liInput);
}

char *VistaAspectsConversionStuff::ConvertToArray(const list<char> &liInput)
{
	return ConvertToArrayT<char, list<char>::const_iterator>(liInput);
}

int *VistaAspectsConversionStuff::ConvertToArray(const list<int> &liInput)
{
	return ConvertToArrayT<int, list<int>::const_iterator>(liInput);
}

unsigned int *VistaAspectsConversionStuff::ConvertToArray(const list<unsigned int> &liInput)
{
	return ConvertToArrayT<unsigned int, list<unsigned int>::const_iterator>(liInput);
}

float *VistaAspectsConversionStuff::ConvertToArray(const list<float> &liInput)
{
	return ConvertToArrayT<float, list<float>::const_iterator>(liInput);
}

double *VistaAspectsConversionStuff::ConvertToArray(const list<double> &liInput)
{
	return ConvertToArrayT<double, list<double>::const_iterator>(liInput);
}

void VistaAspectsConversionStuff::DeleteArray(bool *p)
{
	delete [] p;
}

void VistaAspectsConversionStuff::DeleteArray(char *p)
{
	delete [] p;
}

void VistaAspectsConversionStuff::DeleteArray(int *p)
{
	delete [] p;
}

void VistaAspectsConversionStuff::DeleteArray(unsigned int *p)
{
	delete [] p;
}

void VistaAspectsConversionStuff::DeleteArray(float *p)
{
	delete [] p;
}

void VistaAspectsConversionStuff::DeleteArray(double *p)
{
	delete [] p;
}

template<class T>
int ConvertToListT(const T* pArray, int iCount, list<T> &liOutput)
{
	liOutput.clear();
	for (int i=0; i<iCount; ++i)
		liOutput.push_back(pArray[i]);
	return (int)liOutput.size();
}

int VistaAspectsConversionStuff::ConvertToList(const bool *pArray, int iCount, list<bool> &liOutput)
{
	return ConvertToListT(pArray, iCount, liOutput);
}

int VistaAspectsConversionStuff::ConvertToList(const char *pArray, int iCount, list<char> &liOutput)
{
	return ConvertToListT(pArray, iCount, liOutput);
}

int VistaAspectsConversionStuff::ConvertToList(const int *pArray, int iCount, list<int> &liOutput)
{
	return ConvertToListT(pArray, iCount, liOutput);
}

int VistaAspectsConversionStuff::ConvertToList(const unsigned int *pArray, int iCount, list<unsigned int> &liOutput)
{
	return ConvertToListT(pArray, iCount, liOutput);
}

int VistaAspectsConversionStuff::ConvertToList(const float *pArray, int iCount, list<float> &liOutput)
{
	return ConvertToListT(pArray, iCount, liOutput);
}

int VistaAspectsConversionStuff::ConvertToList(const double *pArray, int iCount, list<double> &liOutput)
{
	return ConvertToListT(pArray, iCount, liOutput);
}

bool VistaAspectsConversionStuff::ConvertStringTo2Float(const std::string &sString, float &a, float &b)
{
	return (sscanf(sString.c_str(), "%f, %f", &a, &b) == 2);
}

bool VistaAspectsConversionStuff::ConvertStringTo2Double(const std::string &sString, double &a, double &b)
{
	return (sscanf(sString.c_str(), "%lf, %lf", &a, &b) == 2);
}

bool VistaAspectsConversionStuff::ConvertStringTo3Float(const std::string &sString, float &a, float &b, float &c)
{
	return (sscanf(sString.c_str(), "%f, %f, %f", &a, &b, &c) == 3);
}

bool VistaAspectsConversionStuff::ConvertStringTo4Float(const std::string &sString, float &a, float &b, float &c, float &d)
{
	return (sscanf(sString.c_str(), "%f, %f, %f, %f", &a, &b, &c, &d) == 4);
}

bool VistaAspectsConversionStuff::ConvertStringTo2Int(const std::string &sString, int &a, int &b)
{
	return (sscanf(sString.c_str(), "%d, %d", &a, &b) == 2);
}

bool VistaAspectsConversionStuff::ConvertStringTo3Int(const std::string &sString, int &a, int &b, int &c)
{
	return (sscanf(sString.c_str(), "%d, %d, %d", &a, &b, &c) == 3);
}

bool VistaAspectsConversionStuff::ConvertStringTo4Double(const std::string &sString, double &a, double &b, double &c, double &d)
{
	return (sscanf(sString.c_str(), "%lf, %lf, %lf, %lf", &a, &b, &c, &d) == 4);
}

// ############################################

bool VistaAspectsComparisonStuff::StringEquals(string sFirst, string sSecond, bool bCaseSensitive)
{
	if(bCaseSensitive)
		return sFirst == sSecond;
	else
		return StringCaseInsensitiveEquals( sFirst, sSecond );
}

bool VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( const std::string& sString1,
											const std::string& sString2 )
{
	#ifdef WIN32
		return ( stricmp( sString1.c_str(), sString2.c_str() ) == 0 );
	#elif defined(LINUX) || defined(SUNOS) || defined(IRIX) || defined(HPUX)
		return ( strcasecmp( sString1.c_str(), sString2.c_str() ) == 0 );
	#else
		VISTA_THROW_NOT_IMPLEMENTED;
	#endif
}

bool VistaAspectsComparisonStuff::StringLess( const std::string& sString1,
											const std::string& sString2 )
{
	return ( strcmp( sString1.c_str(), sString2.c_str() ) < 0 );
}

bool VistaAspectsComparisonStuff::StringCaseInsensitiveLess( const std::string& sString1,
											const std::string& sString2 )
{
	#ifdef WIN32
		return ( stricmp( sString1.c_str(), sString2.c_str() ) < 0 );
	#elif defined(LINUX) || defined(SUNOS) || defined(IRIX) || defined(HPUX)
		return ( strcasecmp( sString1.c_str(), sString2.c_str() ) < 0 );
	#else
		VISTA_THROW_NOT_IMPLEMENTED;
	#endif
}

/*============================================================================*/
/* StringCompareObject                                                        */
/*============================================================================*/

VistaAspectsComparisonStuff::StringCompareObject::StringCompareObject( 
											const bool bCaseSensitive )
: m_bCaseSensitive( bCaseSensitive )
{
}

bool VistaAspectsComparisonStuff::StringCompareObject::operator() (
						const std::string& sLeft, const std::string& sRight ) const
{
	if( m_bCaseSensitive )
		return sLeft == sRight;
	else
		return StringCaseInsensitiveEquals( sLeft, sRight );
}

void VistaAspectsComparisonStuff::StringCompareObject::SetIsCaseSensitive( const bool bSet )
{
	m_bCaseSensitive = bSet;
}

bool VistaAspectsComparisonStuff::StringCompareObject::GetIsCaseSensitive() const
{
	return m_bCaseSensitive;
}

/*============================================================================*/
/* StringLessObject                                                           */
/*============================================================================*/

VistaAspectsComparisonStuff::StringLessObject::StringLessObject( 
											const bool bCaseSensitive )
: m_bCaseSensitive( bCaseSensitive )
{
}

bool VistaAspectsComparisonStuff::StringLessObject::operator() (
						const std::string& sLeft, const std::string& sRight ) const
{
	if( m_bCaseSensitive )
		return sLeft < sRight;
	else
		return StringCaseInsensitiveLess( sLeft, sRight );
}

void VistaAspectsComparisonStuff::StringLessObject::SetIsCaseSensitive( const bool bSet )
{
	m_bCaseSensitive = bSet;
}

bool VistaAspectsComparisonStuff::StringLessObject::GetIsCaseSensitive() const
{
	return m_bCaseSensitive;
}



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
