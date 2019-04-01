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


#ifndef _VISTAASPECTSUTILS_H
#define _VISTAASPECTSUTILS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaAspectsConfig.h"
#include <VistaBase/VistaBaseTypes.h>


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

class VISTAASPECTSAPI VistaAspectsConversionStuff
{
public:
	static double ConvertToDouble(const std::string &sValue, bool bCheckPoint=true);

	// defaults bCheckPoint to true
	static double ConvertToDouble1(const std::string &sValue );

	// defaults bCheckPoint to false
	static double ConvertToDouble2(const std::string &sValue );

	static float  ConvertToFloat(const std::string &sValue, bool bCheckPoint = true );
	// defaults bCheckPoint to true
	static float ConvertToFloat1(const std::string &sValue );

	// defaults bCheckPoint to true
	static float ConvertToFloat2(const std::string &sValue );

	static int             ConvertToInt(const std::string &sValue);
	static unsigned int    ConvertToUnsignedInt(const std::string &sValue);
	static bool            ConvertToBool(const std::string &sValue);
	static void *          ConvertToId(const std::string &sValue);
	static unsigned int    ConvertToList(const std::string &sValue, std::list<double> &liStrings, char cSep = ',');
	static unsigned int    ConvertToList(const std::string &sValue, std::list<float> &liStrings, char cSep = ',');
	static unsigned int    ConvertToList(const std::string &sValue, std::list<int> &liStrings, char cSep = ',');
	static unsigned int    ConvertToList(const std::string &sValue, std::list<bool> &liStrings, char cSep = ',');
	static unsigned int    ConvertToList(const std::string &sValue, std::list<std::string> &liStrings, char cSep = ',', bool bTrim=true);

	static std::list<std::string> ConvertToStringList(const std::string &sValue);
	static std::list<double> ConvertToDoubleList(const std::string &sValue);
	static std::list<float>	ConvertToFloatList(const std::string &sValue);
	static std::list<int>    ConvertToIntList(const std::string &sValue);
	static std::list<bool>   ConvertToBoolList(const std::string &sValue);

	static std::string ConvertToString(int iVal);
	static std::string ConvertToString(unsigned int iVal);
	static std::string ConvertToString(VistaType::sint64 iVal);
	static std::string ConvertToString(double dVal);
	static std::string ConvertToString(bool bVal);
	static std::string ConvertToString(void *pVal);
	static std::string ConvertToString(const std::string &sString);
	static std::string ConvertToString(const std::list<double> &liStrings);
	static std::string ConvertToString(const std::list<float> &liStrings);
	static std::string ConvertToString(const std::list<int> &liStrings);
	static std::string ConvertToString(const std::list<bool> &liStrings);
	static std::string ConvertToString(const std::list<std::string> &liStrings);
	static std::string ConvertToString(const std::list<std::string> &liStrings, char cSep);

	static std::string ConvertToUpper(const std::string &sValue);
	static std::string ConvertToLower(const std::string &sValue);
	static std::string RemoveChar(const std::string &sValue, char cToRemove);

	/**
	 * Array operations.
	 * NOTE: ConvertToArray() allocates memory on the heap.
	 *       You are responsible for freeing that memory via
	 *       DeleteArray() - DON'T just use delete [] !!!
	 */
	static bool* ConvertToArray(const std::list<bool> &liInput);
	static char* ConvertToArray(const std::list<char> &liInput);
	static int* ConvertToArray(const std::list<int> &liInput);
	static unsigned int* ConvertToArray(const std::list<unsigned int> &liInput);
	static float* ConvertToArray(const std::list<float> &liInput);
	static double* ConvertToArray(const std::list<double> &liInput);

	static void DeleteArray(bool *p);
	static void DeleteArray(char *p);
	static void DeleteArray(int *p);
	static void DeleteArray(unsigned int *p);
	static void DeleteArray(float *p);
	static void DeleteArray(double *p);

	static int ConvertToList(const bool* pArray, int iCount, std::list<bool> &liOutput);
	static int ConvertToList(const char* pArray, int iCount, std::list<char> &liOutput);
	static int ConvertToList(const int* pArray, int iCount, std::list<int> &liOutput);
	static int ConvertToList(const unsigned int* pArray, int iCount, std::list<unsigned int> &liOutput);
	static int ConvertToList(const float* pArray, int iCount, std::list<float> &liOutput);
	static int ConvertToList(const double* pArray, int iCount, std::list<double> &liOutput);


	// specific convsersion methods
	static bool ConvertStringTo2Float(const std::string &sString, float &a, float &b);
	static bool ConvertStringTo3Float(const std::string &sString, float &a, float &b, float &c);
	static bool ConvertStringTo4Float(const std::string &sString, float &a, float &b, float &c, float &d);

	static bool ConvertStringTo2Int(const std::string &sString, int &a, int &b);
	static bool ConvertStringTo3Int(const std::string &sString, int &a, int &b, int &c);

	static bool ConvertStringTo2Double(const std::string &sString, double &a, double &b);
	static bool ConvertStringTo4Double(const std::string &sString, double &a, double &b, double &c, double &d);

protected:
private:
	VistaAspectsConversionStuff();
	~VistaAspectsConversionStuff();
};

class VISTAASPECTSAPI VistaAspectsComparisonStuff
{
public:
	static bool StringEquals (std::string, std::string, bool bCaseSensitive = true );
	static bool StringCaseInsensitiveEquals( const std::string& sString1,
											const std::string& sString2 );
	static bool StringLess( const std::string& sString1, const std::string& sString2 );
	static bool StringCaseInsensitiveLess( const std::string& sString1,
											const std::string& sString2 );

	/**
	 * Utility Functor for case-sensitive or -insensitive string comparison
	 */
	class VISTAASPECTSAPI StringCompareObject
	{
	public:
		explicit StringCompareObject( const bool bCaseSensitive = true );
		bool operator() ( const std::string& sLeft, const std::string& sRight ) const;

		void SetIsCaseSensitive( const bool bSet );
		bool GetIsCaseSensitive() const;
	private:
		bool m_bCaseSensitive;
	};
	class VISTAASPECTSAPI StringLessObject
	{
	public:
		explicit StringLessObject( const bool bCaseSensitive = true );
		bool operator() ( const std::string& sLeft, const std::string& sRight ) const;

		void SetIsCaseSensitive( const bool bSet );
		bool GetIsCaseSensitive() const;
	private:
		bool m_bCaseSensitive;
	};

	template<class T>
        class ObjEqualsCompare
	{
	public:
		ObjEqualsCompare( const T &arg )
			: m_cmpArg(arg) {}

		~ObjEqualsCompare() {}

		bool operator()( const T &o ) const
		{
			return (o == m_cmpArg);
		}

		T m_cmpArg;
	};
};

namespace AspectUtils
{
	template<class T>
	std::string ConvertToString( const T &value )
	{
		return VistaAspectsConversionStuff::ConvertToString(value);
	}
} // namespace AspectUtils

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAASPECTSUTILS_H

