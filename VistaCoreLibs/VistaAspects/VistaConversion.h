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


#ifndef _VISTACONVERSIONUTILS_H
#define _VISTACONVERSIONUTILS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaAspectsConfig.h"

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaColor.h>


#include <string>
#include <string.h> // intel icpc needs this for memcpy
#include <algorithm>
#include <sstream>
#include <list>
#include <vector>
#include <deque>
#include <stack>
#include <queue>
#include <typeinfo>

#include <cstdio>
#include <cstdlib>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

static const char S_cDefaultSeparator = ',';

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaConversion provides different template functions and objects to perform
 * type and string conversions:
 *  - GetTypeName returns (cleaned) class name
 *  - ConvertType and ConvertTypeObject converts from one class to another
 * - StringToUpper/Lower
 * - StringConvertObject converts an object to or from string. By default it
 *      can convert basic types, VistaVectorMath types, and stl-containers
 *      vector, list, deque, stack, and queue
 *      extended to work with custom types by either defining custom
 *      std::ostream operator<< and operator >>; or by creating a template
 *      specialization of StringConvertObject
 * - Different
 */
namespace VistaConversion
{
	enum eConversionErrorHandling
	{
		ON_ERROR_DO_NOTHING			= 0x00,
		ON_ERROR_THROW_EXCEPTION	= 0x01,
		ON_ERROR_PRINT_WARNING		= 0x02,
	};

	/**
	 * returns typename of an Object in human-readable format
	 */
	template<typename T>
	std::string GetTypeName();
	template<typename T>
	std::string GetTypeName( T& oObject );
	template<typename T>
	std::string GetTypeName( T* pObject );

	/**
	 * Converts a os-specific typeid name to human-readable format
	 */
	VISTAASPECTSAPI std::string CleanOSTypeName( const std::string& sName );

	/**
	 * Convert between Objects - by default uses Constructor tTo( tFrom ),
	 * ot string conversion for strings
	 */
	template<typename tTo, typename tFrom>
	tTo ConvertType( const tFrom& oFrom );
	/**
	 * Function object that convert between Objects
	 */
	template<typename tTo, typename tFrom>
	struct ConvertTypeObject
	{
		tTo operator()( const tFrom& oFrom );
	};

	/**
	 * Creates an object that is guaranteed to be initialized - either
	 * with 0/false for base types, or with default ctor for classes
	 */
	template<typename T>
	T CreateInitializedDefaultObject();

	/**
	 * Converts String to all-uppercase letters
	 */
	VISTAASPECTSAPI std::string StringToUpper( const std::string& oSource );
	/**
	 * Converts String to all-lowercase letters
	 */
	VISTAASPECTSAPI std::string StringToLower( const std::string& oSource );

	///////////////////////////////////////////////////////////////
	///// Conversions to and from string                      /////
	///// Works for the following types:                      /////
	///// Basic types (int, char, float, ...)                 /////
	///// VistaMathTypes                                      /////
	///// stl vector, list, deque, stack, and queue           /////
	///////////////////////////////////////////////////////////////

	/**
	 * Generic Object to allow string convertions with partial template specialization
	 */
	template<typename T>
	struct StringConvertObject
	{
		static void ToString( const T& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator );
		static bool FromString( const std::string& sSource, T& sTarget,
								char cSeparator = S_cDefaultSeparator );
	};


	template<typename T>
	void ToString( const T& oValue, std::string& sTarget );
	template<typename T> 
	void ToString( const T& oValue, std::string& sTarget, char cSeparator );

	/**
	 * Convert the parameter to a string representation and return it
	 * If conversion fails, an exception is thrown
	 * @param oValue Object to be converted
	 * @return string representation if successful, empty string on failure
	 * @throws VistaExceptionBase when conversion fails
	 */
	template<typename T>
	std::string ToString( const T& oValue );
	template<typename T>
	std::string ToString( const T& oValue, char cSeparator );

		
	/**
	 * Convert the string to an object representation. Works for:
	 * basic types, VistaMathTypes, and lists and vectors
	 * @return true if conversion worked, false if not
	 *              if false is returned, the target will be default-
	 *              initialized (0 for base types, default ctor for others)
	 */
	template<typename T>
	bool FromString( const std::string& sSource, T& oTarget );
	template<typename T>
	bool FromString( const std::string& sSource, T& oTarget, char cSeparator );

	/**
	 * Convert the string to an instance and return if
	 * If conversion fails, an exception is thrown
	 * Note: not modeled with default eConversionErrorHandling to allow function
	 * pointers accepting just one argument. 
	 * @param sSource string to be converted
	 * @return Converted object if successful, Default object if not
	 *              (NOTE: return value is uninitialized for basic types)
	 */
	template<typename T>
	T FromString( const std::string& sSource );
	template<typename T>
	T FromString( const std::string& sSource, char cSeparator );

	/**
	 * Convert the string to an instance and return if
	 * @param sSource string to be converted
	 * @param nOnError Specifies how conversion errors should be handled
	 * @return Converted object if successful, Default object if not
	 *              (NOTE: return value is uninitialized for basic types)
	 */
	template<typename T>
	T FromString( const std::string& sSource, eConversionErrorHandling nOnError );
	template<typename T>
	T FromString( const std::string& sSource, char cSeparator, eConversionErrorHandling nOnError );


	/**
	 * Implementation for fixed-size arrays - make sure the arrays are
	 * big enough (>= N)!
	 * returns true iff ecactly N elements were extracted, fails if
	 * not enough segments are found in the string, or if some data remains
	 * after filling N entires
	 * Size determined via template parameter or argument
	 */
	
	template<int N, typename T> 
	void ArrayToString( const T aSource[], std::string& sTarget );
	template<int N, typename T> 
	void ArrayToString( const T aSource[], std::string& sTarget, char cSeparator );
	template<typename T>
	void ArrayToString( const T aSource[], std::string& sTarget, unsigned int nSize );
	template<typename T>
	void ArrayToString( const T aSource[], std::string& sTarget, unsigned int nSize, char cSeparator );

	template<int N, typename T> 
	std::string ArrayToString( const T aSource[]  );
	template<int N, typename T> 
	std::string ArrayToString( const T aSource[], char cSeparator );
	template<typename T> 
	std::string ArrayToString( const T aSource[], unsigned int nSize );
	template<typename T> 
	std::string ArrayToString( const T aSource[], unsigned int nSize, char cSeparator  );
	template<int N, typename T>

	bool ArrayFromString( const std::string& sSource, T aTarget[] );
	template<int N, typename T> 
	bool ArrayFromString( const std::string& sSource, T aTarget[],	char cSeparator );
	template<typename T>
	bool ArrayFromString( const std::string& sSource, T aTarget[], unsigned int nSize );
	template<typename T> 
	bool ArrayFromString( const std::string& sSource, T aTarget[], unsigned int nSize,	char cSeparator );

	/**
	 * Helper function that extract separated substrings form a larger string.
	 * It extracts a substring between nStartPos and a linebreak, an occurance
	 * of cSeparator character, or the end of the string, and crops leading
	 * and trailing whitespaces.
	 * If successful, the result is stored in sTarget, and the next char's position
	 * is returned (suitable as nStartPos for the next call). If the extraction fails
	 * because there is no more data in the string, std::string::npos is returned;
	 */
	VISTAASPECTSAPI std::size_t GetSeparatedSubString( std::string& sTarget,
									const std::string& sSource,
									size_t nStartPos,
									char cSeparator );

	/**
	 * Removes all occuring braces () {} [] from the string
	 */
	VISTAASPECTSAPI void RemoveBraces( std::string& sString );
	/**
	 * Checks if all remaining elements (from nFrom on)
	 * are either whitespaces or separators
	 */
	VISTAASPECTSAPI bool CheckRemainingStringEmpty( 
										const std::string& sString,
										std::size_t nFrom,
										char cSeparator );

	/**
	 * returns the Metric Prefix (e.g. kilo, mega, milli, etc.) or its Symbol(e.g. k, M, m, etc.)
	 * for the given exponent of 1000, i.e. 1000^nExponent = 1^(3*nExponentGetNumberOfMetricPrefixes
	 * If the exponent is not in the valid range [1, n] or [-n, -1] where n = GetNumberOfBinaryPrefixes(),
	 * an empty string is returned.
	 */
	VISTAASPECTSAPI std::string GetMetricPrefixForExponent( const int nExponent );
	VISTAASPECTSAPI std::string GetMetricPrefixSymbolForExponent( const int nExponent );
	/**
	 *  returns number of metric prefixes (for either positive or negative ones)
	 */
	VISTAASPECTSAPI int GetNumberOfMetricPrefixes();

	// returns the Binary Prefix (e.g. kibi, mebi, gibi) for the given exponent of 1024,
	// i.e. 1024^nExponent
	VISTAASPECTSAPI std::string GetBinaryPrefixForExponent( const int nExponent );
	/**
	 * returns the Binary Prefix (e.g. kibi, mebi, gibi) or its symbol (e.g. Ki, Mi, Gi, etc.)
	 * or the given exponent of 1024, i.e. 1024^nExponent.
	 * If the exponent is not in the valid range [1, n] where n = GetNumberOfBinaryPrefixes(),
	 * an empty string is returned.
	 */ 
	VISTAASPECTSAPI std::string GetBinaryPrefixSymbolForExponent( const int nExponent );
	/**
	 * returns number of binary prefixes (for either positive or negative ones)
	 */
	VISTAASPECTSAPI int GetNumberOfBinaryPrefixes();

	/**
	 * Returns the prefix or prefix symbol for nNumber, and stores the remaining value
	 * in nPrefixNumber (i.e. 1234.5 becomes 1.2345 and prefix is kilo)
	 */
	template< typename TNumber >
	std::string GetMetricPrefixForNumber( const TNumber nNumber,
											TNumber& nPrefixNumber,
											const bool bUseAbbreviatedSymbole );
	template< typename TNumber >
	std::string GetBinaryPrefixForNumber( const TNumber nNumber,
											TNumber& nPrefixNumber,
											const bool bUseAbbreviatedSymbol );

	/**
	 * Writes the number to a string with the appended metric or binary prefix, and optionally
	 * fixes the number of digits after the period.
	 */
	template< typename TNumber >
	std::string ConvertNumberToStringWithMetricPrefix( const TNumber nNumber,
											const bool bUseAbbreviatedSymbol,
											const bool bAddSpaceBetweenNumberAndPrefix = false,
											const int nNumPostDotDigits = -1 );
	template< typename TNumber >
	std::string ConvertNumberToStringWithBinaryPrefix( const TNumber nNumber,
											const bool bUseAbbreviatedSymbol,
											const bool bAddSpaceBetweenNumberAndPrefix = false,
											const int nNumPostDotDigits = -1 );
}

/////////////////////////////////////////////////////////
////// Inline implementations                      //////
/////////////////////////////////////////////////////////

namespace VistaConversion
{
	// GetSeparatedSubString helper function
	inline std::size_t GetSeparatedSubString( std::string& sTarget,
										const std::string& sSource,
										std::size_t nStartPos,
										char cSeparator )
	{
		if( nStartPos >= sSource.size() )
			return std::string::npos;

		std::size_t nPos = nStartPos;
		std::size_t nSegmentStart = 0;
		std::size_t nSegmentEnd = std::string::npos;
		
		std::string::const_iterator itCurrent = sSource.begin() + nStartPos;

		// eliminate start fillers
		while( (*itCurrent) == ' ' || (*itCurrent) == '\n' 
					|| (*itCurrent) == '\t' || (*itCurrent) == cSeparator )
		{
			if( ++itCurrent == sSource.end() )
				return std::string::npos;
			++nPos;
		}
		nSegmentStart = nPos;

		// parse until end of segment
		while( (*itCurrent) != '\n' && (*itCurrent) != cSeparator )
		{		
			// we store endpos only at end - to discard following whitespaces
			if( (*itCurrent) != ' ' && (*itCurrent) != '\t' )
				nSegmentEnd = nPos;
			if( ++itCurrent == sSource.end() )
				break;
			++nPos;		
		}

		if( nSegmentEnd == std::string::npos )
			return std::string::npos;

		sTarget = sSource.substr( nSegmentStart, nSegmentEnd - nSegmentStart + 1 );
		return nPos + 1;

	}

	///////////////////////////////////
	// PREFIX IMPLEMENTATIONS      ///
	///////////////////////////////////

	// helper function
	template< typename TNumber >
	inline int GetExponentForPrefix( const TNumber nNumber,
								TNumber& nPrefixNumber,
								const TNumber nFactor,
								const int nMaxExponent )
	{
		if( nNumber == 0 )
		{
			nPrefixNumber = nNumber;
			return 0;
		}
		nPrefixNumber = std::abs( nNumber );
		int nExponent = 0;
		if( nPrefixNumber < TNumber( 1 ) )
		{
			while( nPrefixNumber < TNumber( 1 ) && nExponent > -nMaxExponent )
			{
				nPrefixNumber *= nFactor;
				--nExponent;
			}
		}
		else
		{
			while( nPrefixNumber >= nFactor && nExponent < nMaxExponent )
			{
				nPrefixNumber /= nFactor;
				++nExponent;
			}
		}
		if( nNumber < 0 )
		{
			nPrefixNumber = -nPrefixNumber;
		}
		return nExponent;
	}
	
	template< typename TNumber >
	inline std::string GetMetricPrefixForNumber( const TNumber nNumber,
																	TNumber& nPrefixNumber,
																	const bool bUseAbbreviatedSymbol )
	{
		int nExponent = GetExponentForPrefix( nNumber, nPrefixNumber, TNumber( 1000 ), GetNumberOfMetricPrefixes() );
		if( bUseAbbreviatedSymbol )
			return GetMetricPrefixSymbolForExponent( nExponent );
		else
			return GetMetricPrefixForExponent( nExponent );
	}

	template< typename TNumber >
	inline std::string GetBinaryPrefixForNumber( const TNumber nNumber,
																	TNumber& nPrefixNumber,
																	const bool bUseAbbreviatedSymbol )
	{
		int nExponent = GetExponentForPrefix( nNumber, nPrefixNumber, TNumber( 1024 ), GetNumberOfBinaryPrefixes() );
		if( bUseAbbreviatedSymbol )
			return GetBinaryPrefixSymbolForExponent( nExponent );
		else
			return GetBinaryPrefixForExponent( nExponent );
	}

	template< typename TNumber >
	inline std::string ConvertNumberToStringWithMetricPrefix( const TNumber nNumber,
																	const bool bUseAbbreviatedSymbol,
																	const bool bAddSpaceBetweenNumberAndPrefix,
																	const int nNumPostDotDigits )
	{
		TNumber nPrefixNumber = 0;
		int nExponent = GetExponentForPrefix( nNumber, nPrefixNumber, TNumber( 1000 ), GetNumberOfMetricPrefixes() );
		std::stringstream sResult;
		if( nNumPostDotDigits >= 0 )
		{
			sResult << std::setprecision( nNumPostDotDigits ) << std::fixed;
		}
		sResult << nPrefixNumber;
		if( bAddSpaceBetweenNumberAndPrefix )
			sResult << " ";
		if( bUseAbbreviatedSymbol )
			sResult << GetMetricPrefixSymbolForExponent( nExponent );
		else
			sResult << GetMetricPrefixForExponent( nExponent );
		return sResult.str();
	}

	template< typename TNumber >
	inline std::string ConvertNumberToStringWithBinaryPrefix( const TNumber nNumber,
																	const bool bUseAbbreviatedSymbol,
																	const bool bAddSpaceBetweenNumberAndPrefix,
																	const int nNumPostDotDigits )
	{
		TNumber nPrefixNumber = 0;
		int nExponent = GetExponentForPrefix( nNumber, nPrefixNumber, TNumber( 1024 ), GetNumberOfBinaryPrefixes() );
		std::stringstream sResult;
		if( nNumPostDotDigits >= 0 )
		{
			sResult << std::setprecision( nNumPostDotDigits ) << std::fixed;
		}
		sResult << nPrefixNumber;
		if( bAddSpaceBetweenNumberAndPrefix )
			sResult << " ";
		if( bUseAbbreviatedSymbol )
			sResult << GetBinaryPrefixSymbolForExponent( nExponent );
		else
			sResult << GetBinaryPrefixForExponent( nExponent );
		return sResult.str();
	}

	///////////////////////////////////
	// TYPENAME IMPLEMENTATIONS     ///
	///////////////////////////////////

	template<typename T>
	inline std::string GetTypeName()
	{
		return CleanOSTypeName( typeid(T).name() );
	}
	template<typename T>
	inline std::string GetTypeName( T& oObject )
	{
		return CleanOSTypeName( typeid( oObject ).name() );
	}
	template<typename T>
	inline std::string GetTypeName( T* oObject )
	{
		return CleanOSTypeName( typeid( (*oObject) ).name() );
	}




	//////////////////////////////////////
	// DEFAULTINIT IMPLEMENTATIONS     ///
	//////////////////////////////////////

	// CreateInitializedDefaultObject implementation and specialization
	template<typename T>
	inline T CreateInitializedDefaultObject()
	{
		return T();
	}
	
	////////////////////////////////
	// CONVERTTYPE IMPLEMENTATION //
	////////////////////////////////
	
	
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 4800)
#endif
	// Convertion from bool to int triggers a warning. We know of the
	// problem, and disable the waring
	
	template<typename tTo, typename tFrom>
	tTo ConvertTypeObject<tTo, tFrom>::operator()( const tFrom& oFrom )
	{
		return tTo( oFrom );
	}

	// specializations: convert from or to string are 
	template<typename tTo>
	struct ConvertTypeObject<tTo, std::string>
	{
		tTo operator()( const std::string& oFrom )
		{
			return FromString<tTo>( oFrom, ON_ERROR_THROW_EXCEPTION );
		}
	};
	template<typename tFrom>
	struct ConvertTypeObject<std::string, tFrom>
	{
		std::string operator()( const tFrom& oFrom )
		{
			return ToString( oFrom );
		}
	};
#ifdef WIN32
#pragma warning(pop)
#endif

	template<typename tTo, typename tFrom>
	inline tTo ConvertType( const tFrom& oFrom )
	{
		// we use the ConvertTypeObject here, because functions dont allow partial specialization
		// which we need to differentiate for strings
		ConvertTypeObject<tTo, tFrom> oConv;
		return oConv( oFrom );
	}



	/////////////////////////////////////////
	// TOSTRING FUNCTION IMPLEMENTATIONS   //
	/////////////////////////////////////////

	template<typename T>
	inline void ToString( const T& oValue, std::string& sTarget )
	{
		return StringConvertObject<T>::ToString( oValue, sTarget );
	}
	template<typename T> 
	inline void ToString( const T& oValue, std::string& sTarget, char cSeparator )
	{
		return StringConvertObject<T>::ToString( oValue, sTarget, cSeparator );
	}

	template<typename T>
	inline std::string ToString( const T& oValue )
	{
		std::string sReturn;
		StringConvertObject<T>::ToString( oValue, sReturn );
		return sReturn;
	}
	template<typename T>
	inline std::string ToString( const T& oValue, char cSeparator )
	{
		std::string sReturn;
		StringConvertObject<T>::ToString( oValue, sReturn, cSeparator );
		return sReturn;
	}

	/////////////////////////////////////////
	// FROMSTRING FUNCTION IMPLEMENTATIONS //
	/////////////////////////////////////////
	
	template<typename T>
	inline bool FromString( const std::string& sSource, T& oTarget )
	{
		return StringConvertObject<T>::FromString( sSource, oTarget );
	}
	template<typename T>
	inline bool FromString( const std::string& sSource, T& oTarget, char cSeparator )
	{
		return StringConvertObject<T>::FromString( sSource, oTarget, cSeparator );
	}

	template<typename T>
	inline T FromString( const std::string& sSource, char cSeparator,
							eConversionErrorHandling nOnError )
	{
		T oValue = CreateInitializedDefaultObject<T>();
		if( StringConvertObject<T>::FromString( sSource, oValue, cSeparator ) == false )
		{
			if( nOnError & ON_ERROR_PRINT_WARNING )
			{
				vstr::warnp() << "VistaConversion::FromString() from "
					<< GetTypeName<T>() << " failed!" << std::endl;
			}
			if( nOnError & ON_ERROR_THROW_EXCEPTION )
			{
				VISTA_THROW( ("Conversion from std::string [" + sSource + "]to " + GetTypeName<T>() + " failed!").c_str(), -1 );
			}
		}
		return oValue;
	}
	template<typename T>
	inline T FromString( const std::string& sSource,
							eConversionErrorHandling nOnError )
	{
		return FromString<T>( sSource, S_cDefaultSeparator, nOnError );
	}
	template<typename T>
	inline T FromString( const std::string& sSource )
	{
		return FromString<T>( sSource, S_cDefaultSeparator, ON_ERROR_THROW_EXCEPTION );
	}
	template<typename T>
	inline T FromString( const std::string& sSource, char cSeparator )
	{
		return FromString<T>( sSource, cSeparator, ON_ERROR_THROW_EXCEPTION );
	}

	///////////////////////////////////////////////////
	// ARRAYTOSTRING IMPLEMENTATION                  //
	///////////////////////////////////////////////////

	template<typename T> 
	inline void ArrayToString( const T aSource[], std::string& sTarget,
						unsigned int nSize, char cSeparator )
	{
		sTarget.clear();
		if( nSize == 0 )
			return;
		StringConvertObject<T>::ToString( aSource[0], sTarget, cSeparator );
		std::string sTmp;
		for( unsigned int i = 1; i < nSize; ++i )
		{
			StringConvertObject<T>::ToString( aSource[i], sTmp, cSeparator );
			sTarget += std::string( 1, cSeparator ) + " " +  sTmp;
		}
	}
	template<typename T> 
	inline void ArrayToString( const T aSource[], std::string& sTarget,
						unsigned int nSize )
	{
		ArrayToString<T>( aSource, sTarget, nSize, S_cDefaultSeparator );
	}
	template<typename T> 
	inline std::string ArrayToString( const T aSource[], unsigned int nSize, char cSeparator )
	{
		std::string sReturn;
		ArrayToString<T>( aSource, sReturn, nSize, cSeparator );
		return sReturn;
	}
	template<typename T> 
	inline std::string ArrayToString( const T aSource[], unsigned int nSize )
	{
		return ArrayToString<T>( aSource, nSize, S_cDefaultSeparator );
	}

	template<int N, typename T> 
	inline void ArrayToString( const T aSource[], std::string& sTarget, char cSeparator )
	{
		return ArrayToString<T>( aSource, sTarget, N, cSeparator );
	}
	template<int N, typename T> 
	inline void ArrayToString( const T aSource[], std::string& sTarget )
	{
		return ArrayToString<T>( aSource, sTarget, N, S_cDefaultSeparator );
	}
	template<int N, typename T> 
	inline std::string ArrayToString( const T aSource[], char cSeparator )
	{
		return ArrayToString<T>( aSource, N, cSeparator );
	}
	template<int N, typename T> 
	inline std::string ArrayToString( const T aSource[] )
	{
		return ArrayToString<T>( aSource, N, S_cDefaultSeparator );
	}

	template<typename T> 
	inline bool ArrayFromString( const std::string& sSource, T aTarget[],
						unsigned int nSize, char cSeparator )
	{
		std::size_t nStringPos = 0;
		std::string sSegment;

		for( unsigned int i = 0; i < nSize; ++i )
		{
			nStringPos = GetSeparatedSubString( sSegment, sSource,
													nStringPos, cSeparator );
			if( nStringPos == std::string::npos )
				return false; // not enough entries

			if( StringConvertObject<T>::FromString( sSegment, aTarget[i], cSeparator ) == false )
			{
				return false;
			} 
		}
			
		return CheckRemainingStringEmpty( sSource, nStringPos, cSeparator );
	}
	template<typename T> 
	inline bool ArrayFromString( const std::string& sSource, T aTarget[],
						unsigned int nSize )
	{
		return ArrayFromString<T>( sSource, aTarget, nSize, S_cDefaultSeparator );
	}

	template<int N, typename T> 
	inline bool ArrayFromString( const std::string& sSource, T aTarget[],	char cSeparator )
	{
		return ArrayFromString<T>( sSource, aTarget, N, cSeparator );
	}
	template<int N, typename T> 
	inline bool ArrayFromString( const std::string& sSource, T aTarget[] )
	{
		return ArrayFromString<T>( sSource, aTarget, N, S_cDefaultSeparator );
	}


	///////////////////////////////////////////////////
	// GENERIC STRINGCONVERTOBJECT IMPLEMENTATION    //
	///////////////////////////////////////////////////

	template<typename T>
	inline void StringConvertObject<T>::ToString( const T& oValue,
													std::string& sTarget, 
													char cSeparator )
	{
		std::ostringstream oStream;
		oStream << oValue;
		sTarget = oStream.str();
	}

	template<typename T>
	inline bool StringConvertObject<T>::FromString( const std::string& sValue,
													T& oTarget,
													char cSeparator )
	{
		std::istringstream oStream( sValue );
		oStream >> oTarget;
		char c;
		if( oStream.fail() || oStream.bad() || oStream.get( c ) )
			return false;
		return true;
	}

	template<typename T>
	struct StringConvertObject<const T>
	{
		static void ToString( const T& oValue,std::string& sTarget, 
							char cSeparator = S_cDefaultSeparator )
		{
			StringConvertObject<T>::ToString( oValue, sTarget, cSeparator );
		}
		static bool FromString( const std::string& sValue,	T& oTarget,
							char cSeparator = S_cDefaultSeparator )
		{
			VISTA_THROW( "VISTACONVERSION::FROMSTRING CANNOT WORK ON CONST OBJECT", -1 );
		}
	};

}






/////////////////////////////////////////////////////////
// STRINGCONVERTOBJECT SPECIALIZATIONS                 //
/////////////////////////////////////////////////////////


namespace VistaConversion
{
	////////////////////////////////////
	// STL SPECIALIZATIONS            //

	// std::string
	template<>
	struct StringConvertObject<std::string>
	{
		static void ToString( const std::string& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			sTarget = oSource;
		}
		static bool FromString( const std::string& sSource, std::string& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			oTarget = sSource;
			return true;
		}
	};

	// std::vector
	template<typename T>
	struct StringConvertObject<std::vector<T> >
	{
		static void ToString( const std::vector<T>& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			sTarget.clear();
			if( oSource.empty() )
				return;
			std::string sTmp;
			for( typename std::vector<T>::const_iterator itEntry = oSource.begin();; )
			{
				StringConvertObject<T>::ToString( (*itEntry), sTmp, cSeparator );
				sTarget += sTmp;
				if( ++itEntry == oSource.end() )
					break;
				sTarget += std::string( 1, cSeparator ) + " ";
			}
		}
		static bool FromString( const std::string& sSource, std::vector<T>& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			oTarget.clear();

			std::size_t nStringPos = 0;
			std::string sSegment;

			T oConvTarget;
			
			for( ;; )
			{
				nStringPos = GetSeparatedSubString( sSegment, sSource,
														nStringPos, cSeparator );
				if( nStringPos == std::string::npos )
					break;

				if( StringConvertObject<T>::FromString( sSegment, oConvTarget, cSeparator ) == false )
				{
					oTarget.clear();
					return false;
				}
				oTarget.push_back( oConvTarget ); 
			}
			
			return true;
		}
	};

	// std::list
	template<typename T>
	struct StringConvertObject<std::list<T> >
	{
		static void ToString( const std::list<T>& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			sTarget.clear();
			if( oSource.empty() )
				return;
			std::string sTmp;
			for( typename std::list<T>::const_iterator itEntry = oSource.begin();; )
			{
				StringConvertObject<T>::ToString( (*itEntry), sTmp, cSeparator );
				sTarget += sTmp;
				if( ++itEntry == oSource.end() )
					break;
				sTarget += std::string( 1, cSeparator ) + " ";
			}
		}
		static bool FromString( const std::string& sSource, std::list<T>& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			oTarget.clear();

			std::size_t nStringPos = 0;
			std::string sSegment;

			T oConvTarget;
			
			for( ;; )
			{
				nStringPos = GetSeparatedSubString( sSegment, sSource,
														nStringPos, cSeparator );
				if( nStringPos == std::string::npos )
					break;

				if( StringConvertObject<T>::FromString( sSegment, oConvTarget, cSeparator ) == false )
				{
					oTarget.clear();
					return false;
				}
				oTarget.push_back( oConvTarget ); 
			}
			
			return true;
		}
	};

	// std::deque
	template<typename T>
	struct StringConvertObject<std::deque<T> >
	{
		static void ToString( const std::deque<T>& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			sTarget.clear();
			if( oSource.empty() )
				return;
			std::string sTmp;
			for( typename std::deque<T>::const_iterator itEntry = oSource.begin();; )
			{
				StringConvertObject<T>::ToString( (*itEntry), sTmp, cSeparator );
				sTarget += sTmp;
				if( ++itEntry == oSource.end() )
					break;
				sTarget += std::string( 1, cSeparator ) + " ";
			}
		}
		static bool FromString( const std::string& sSource, std::deque<T>& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			oTarget.clear();

			std::size_t nStringPos = 0;
			std::string sSegment;

			T oConvTarget;
			
			for( ;; )
			{
				nStringPos = GetSeparatedSubString( sSegment, sSource,
														nStringPos, cSeparator );
				if( nStringPos == std::string::npos )
					break;

				if( StringConvertObject<T>::FromString( sSegment, oConvTarget, cSeparator ) == false )
				{
					oTarget.clear();
					return false;
				}
				oTarget.push_back( oConvTarget ); 
			}
			
			return true;
		}
	};

	// std::stack
	template<typename T>
	struct StringConvertObject<std::stack<T> >
	{
		static void ToString( const std::stack<T>& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{	
			sTarget.clear();
			if( oSource.empty() )
				return;
			std::stack<T> oCopy( oSource ); // we have to copy, since we may not pop
			std::string sTmp;
			for( ;; )
			{
				StringConvertObject<T>::ToString( oCopy.top(), sTmp, cSeparator );
				sTarget = sTmp + sTarget;
				oCopy.pop();
				if( oCopy.empty() )
					break;
				sTarget = std::string( 1, cSeparator ) + " " + sTarget;
			}
		}
		static bool FromString( const std::string& sSource, std::stack<T>& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			while( oTarget.empty() == false )
				oTarget.pop();

			std::size_t nStringPos = 0;
			std::string sSegment;

			T oConvTarget;
			
			for( ;; )
			{
				nStringPos = GetSeparatedSubString( sSegment, sSource,
														nStringPos, cSeparator );
				if( nStringPos == std::string::npos )
					break;

				if( StringConvertObject<T>::FromString( sSegment, oConvTarget, cSeparator ) == false )
				{
					while( oTarget.empty() == false )
						oTarget.pop();
					return false;
				}
				oTarget.push( oConvTarget ); 
			}
			
			return true;
		}
	};

	// std::queue
	template<typename T>
	struct StringConvertObject<std::queue<T> >
	{
		static void ToString( const std::queue<T>& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			sTarget.clear();
			if( oSource.empty() )
				return;
			std::string sTmp;
			std::queue<T> oCopy( oSource ); // we have to copy, since we may not pop
			for( ;; )
			{
				StringConvertObject<T>::ToString( oCopy.front(), sTmp, cSeparator );
				sTarget += sTmp;
				oCopy.pop();
				if( oCopy.empty() )
					break;
				sTarget += std::string( 1, cSeparator ) + " ";
			}
		}
		static bool FromString( const std::string& sSource, std::queue<T>& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			while( oTarget.empty() == false )
				oTarget.pop();

			std::size_t nStringPos = 0;
			std::string sSegment;

			T oConvTarget;
			
			for( ;; )
			{
				nStringPos = GetSeparatedSubString( sSegment, sSource,
														nStringPos, cSeparator );
				if( nStringPos == std::string::npos )
					break;

				if( StringConvertObject<T>::FromString( sSegment, oConvTarget, cSeparator ) == false )
				{
					while( oTarget.empty() == false )
						oTarget.pop();
					return false;
				}
				oTarget.push( oConvTarget ); 
			}
			
			return true;
		}
	};


	////////////////////////////////////
	// BASETYPES SPECIALIZATION       //

	template<>
	struct StringConvertObject<bool>
	{
		static void ToString( const bool& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			if( oSource )
				sTarget = "true";
			else
				sTarget = "false";
		}
		static bool FromString( const std::string& sSource, bool& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			std::string sTemp = StringToUpper( sSource );
			if( sTemp == "TRUE" || sTemp == "ON" || sTemp == "1" )
			{
				oTarget = true;
				return true;
			}
			if( sTemp == "FALSE" || sTemp == "OFF" || sTemp == "0" )
			{
				oTarget = false;
				return true;
			}
			oTarget = false;
			return false;
		}
	};

	template<>
	struct StringConvertObject<char>
	{
		static void ToString( const char& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			sTarget = std::string( 1, oSource );
		}
		static bool FromString( const std::string& sSource, char& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			if( sSource.size() != 1 )
				return false;
			oTarget = sSource[0];
			return true;
		}
	};

	template<>
	struct StringConvertObject<signed char>
	{
		static void ToString( const signed char& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			sTarget = std::string( 1, (char)oSource );
		}
		static bool FromString( const std::string& sSource, signed char& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			if( sSource.size() != 1 )
				return false;
			oTarget = sSource[0];
			return true;
		}
	};

	template<>
	struct StringConvertObject<unsigned char>
	{
		static void ToString( const unsigned char& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			sTarget = std::string( 1, (char)oSource );
		}
		static bool FromString( const std::string& sSource, unsigned char& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			if( sSource.size() != 1 )
				return false;
			oTarget = sSource[0];
			return true;
		}
	};

	template<>
	struct StringConvertObject<wchar_t>
	{
		static void ToString( const wchar_t& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			sTarget = std::string( 1, (char)oSource );
		}
		static bool FromString( const std::string& sSource, wchar_t& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			if( sSource.size() != 1 )
				return false;
			oTarget = sSource[0];
			return true;
		}
	};

	template<>
	struct StringConvertObject<int>
	{
		static void ToString( const int& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			char aBuffer[256];
			sprintf( aBuffer, "%d", oSource );
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, int& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			int iReturn = sscanf( sSource.c_str(), "%d", &oTarget );
			if( iReturn != 1 )
			{
				oTarget = 0;
				return false;
			}
			return true;	
		}
	};
	template<>
	struct StringConvertObject<unsigned int>
	{
		static void ToString( const unsigned int& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			char aBuffer[256];
			sprintf( aBuffer, "%u", oSource );
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, unsigned int& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			int iReturn = sscanf( sSource.c_str(), "%u", &oTarget );
			if( iReturn != 1 )
			{
				oTarget = 0;
				return false;
			}
			return true;
		}
	};

	template<>
	struct StringConvertObject<short>
	{
		static void ToString( const short& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{	
			StringConvertObject<int>::ToString( (int)oSource, sTarget );
		}
		static bool FromString( const std::string& sSource, short& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			int iValue;
			bool bReturn = StringConvertObject<int>::FromString ( sSource, iValue );
			oTarget = (short)iValue;
			return bReturn;
		}
	};
	template<>
	struct StringConvertObject<unsigned short>
	{
		static void ToString( const unsigned short& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			StringConvertObject<unsigned int>::ToString( (unsigned int)oSource, sTarget );
		}
		static bool FromString( const std::string& sSource, unsigned short& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			unsigned int iValue;
			bool bReturn = StringConvertObject<unsigned int>::FromString ( sSource, iValue );
			oTarget = (unsigned short)iValue;
			return bReturn;
		}
	};

	template<>
	struct StringConvertObject<long int>
	{
		static void ToString( const long int& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			StringConvertObject<int>::ToString( (int)oSource, sTarget );
		}
		static bool FromString( const std::string& sSource, long int& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			int iValue;
			bool bReturn = StringConvertObject<int>::FromString ( sSource, iValue );
			oTarget = (long int)iValue;
			return bReturn;
		}
	};

	//template<>
	//struct StringConvertObject<unsigned long int>
	//{
	//	static void ToString( const unsigned long int& oSource, std::string& sTarget,
	//							char cSeparator = S_cDefaultSeparator )
	//	{
	//		StringConvertObject<unsigned int>::ToString( (unsigned int)oSource, sTarget );
	//	}
	//	static bool FromString( const std::string& sSource, unsigned long int& oTarget,
	//							char cSeparator = S_cDefaultSeparator )
	//	{
	//		unsigned int iValue;
	//		bool bReturn = StringConvertObject<unsigned int>::FromString ( sSource, iValue );
	//		oTarget = (unsigned long int)iValue;
	//		return bReturn;
	//	}
	//};

	//template<>
	//struct StringConvertObject<long long int>
	//{
	//	static void ToString( const long long int& oSource, std::string& sTarget,
	//							char cSeparator = S_cDefaultSeparator )
	//	{		
	//		std::stringstream s;
	//		s << oSource;
	//		sTarget = s.str();
	//	}
	//	static bool FromString( const std::string& sSource, long long int& oTarget,
	//							char cSeparator = S_cDefaultSeparator )
	//	{		
	//		std::istringstream is(sSource);
	//		is >> oTarget;
	//		return is.fail();

	//		//int iValue;
	//		//bool bReturn = StringConvertObject<int>::FromString ( sSource, iValue );
	//		//oTarget = (long long int)iValue;
	//		//return bReturn;
	//	}
	//};

	//template<>
	//struct StringConvertObject<unsigned long long int>
	//{
	//	static void ToString( const unsigned long long int& oSource, std::string& sTarget,
	//							char cSeparator = S_cDefaultSeparator )
	//	{
	//		StringConvertObject<unsigned int>::ToString( (unsigned int)oSource, sTarget );
	//	}
	//	static bool FromString( const std::string& sSource, unsigned long long int& oTarget,
	//							char cSeparator = S_cDefaultSeparator )
	//	{
	//		unsigned int iValue;
	//		bool bReturn = StringConvertObject<unsigned int>::FromString ( sSource, iValue );
	//		oTarget = (unsigned long long int)iValue;
	//		return bReturn;
	//	}
	//};

	template<>	
	struct StringConvertObject<float>
	{
		static void ToString( const float& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			char aBuffer[128];
			sprintf( aBuffer, "%.8g", (double)oSource );
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, float& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{				
			int iReturn = sscanf( sSource.c_str(), "%g", &oTarget );
			if( iReturn == 1 )
				return true;
			oTarget = 0;
			return false;
		}
	};
	template<>
	struct StringConvertObject<double>
	{
		static void ToString( const double& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			char aBuffer[128];
			sprintf( aBuffer, "%.16g", oSource );
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, double& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{	
			int iReturn = sscanf( sSource.c_str(), "%lg", &oTarget );
			if( iReturn == 1 )
				return true;
			oTarget = 0;
			return false;
		}
	};
	template<>
	struct StringConvertObject<long double>
	{
		static void ToString( const long double& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{		
			char aBuffer[128];
			sprintf( aBuffer, "%Lg", oSource );
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, long double& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{	
			int iReturn = sscanf( sSource.c_str(), "%Lg", &oTarget );
			if( iReturn == 1 )
				return true;
			oTarget = 0;
			return false;
		}
	};


	////////////////////////////////////
	// VISTA MATHTYPES SPECIALIZATION //
	template<>
	struct StringConvertObject<VistaVector3D>
	{
		static void ToString( const VistaVector3D& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			char aBuffer[128];
			if( oSource[3] == 1 )
			{
				sprintf( aBuffer, "%g%c %g%c %g",
						(double)oSource[0], cSeparator,
						(double)oSource[1], cSeparator,
						(double)oSource[2] );
			}
			else
			{
				sprintf( aBuffer, "%g%c %g%c %g%c %g",
						(double)oSource[0], cSeparator,
						(double)oSource[1], cSeparator,
						(double)oSource[2], cSeparator,
						(double)oSource[3] );
			}
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, VistaVector3D& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			std::string sCleanedSource = sSource;
			RemoveBraces( sCleanedSource );

			std::vector<float> vecEntries;

			if( StringConvertObject<std::vector<float> >::FromString(
									sCleanedSource, vecEntries, cSeparator ) == false )
				return false;

			if( vecEntries.size() == 3 )
			{
				oTarget[0] = vecEntries[0];
				oTarget[1] = vecEntries[1];
				oTarget[2] = vecEntries[2];
				oTarget[3] = 1.0f;
			}
			else if( vecEntries.size() == 4 )
			{
				oTarget[0] = vecEntries[0];
				oTarget[1] = vecEntries[1];
				oTarget[2] = vecEntries[2];
				oTarget[3] = vecEntries[3];
			}
			else
				return false;

			return true;
		}
	};

	template<>
	struct StringConvertObject<VistaQuaternion>
	{
		static void ToString( const VistaQuaternion& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			char aBuffer[128];

			sprintf( aBuffer, "%g%c %g%c %g%c %g",
					(double)oSource[0], cSeparator,
					(double)oSource[1], cSeparator,
					(double)oSource[2], cSeparator,
					(double)oSource[3] );

			sTarget = std::string( aBuffer );
		}

		static bool FromString( const std::string& sSource, VistaQuaternion& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			std::string sCleanedSource = sSource;
			RemoveBraces( sCleanedSource );

			float a4fData[4];

			if( ArrayFromString<4, float>( sCleanedSource, &a4fData[0], cSeparator ) == false )
				return false;
	
			oTarget.SetValues( a4fData );
			oTarget.Normalize();

			return true;
		}
	};

	template<>
	struct StringConvertObject<VistaEulerAngles>
	{
		static void ToString( const VistaEulerAngles& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			char aBuffer[128];
			sprintf( aBuffer, "%g%c %g%c %g",
						(double)oSource.a, cSeparator,
						(double)oSource.b, cSeparator,
						(double)oSource.c );	
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, VistaEulerAngles& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			std::string sCleanedSource = sSource;
			RemoveBraces( sCleanedSource );

			float a3fValues[3];
			if( ArrayFromString<3, float>( sCleanedSource, a3fValues ) == false )
				return false;
			oTarget.a = a3fValues[0];
			oTarget.b = a3fValues[1];
			oTarget.c = a3fValues[2];
			return true;
		}
	};

	template<>
	struct StringConvertObject<VistaAxisAndAngle>
	{
		static void ToString( const VistaAxisAndAngle& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			char aBuffer[128];
			sprintf( aBuffer, "%g%c %g%c %g%c %g",
					(double)oSource.m_v3Axis[0], cSeparator,
					(double)oSource.m_v3Axis[1], cSeparator,
					(double)oSource.m_v3Axis[2], cSeparator,
					(double)oSource.m_fAngle );
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, VistaAxisAndAngle& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			std::string sCleanedSource = sSource;
			RemoveBraces( sCleanedSource );

	
			float a3fValues[4];
			if( ArrayFromString<4, float>( sCleanedSource, a3fValues ) == false )
				return false;

			oTarget.m_v3Axis[0] = a3fValues[0];
			oTarget.m_v3Axis[1] = a3fValues[1];
			oTarget.m_v3Axis[2] = a3fValues[2];
			oTarget.m_fAngle = a3fValues[3];

			return true;
		}
	};

	template<>
	struct StringConvertObject<VistaTransformMatrix>
	{
		static void ToString( const VistaTransformMatrix& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			float a16fBuffer[16];
			oSource.GetValues( a16fBuffer );

			ArrayToString<16, float>( a16fBuffer, sTarget, cSeparator );
		}
		static bool FromString( const std::string& sSource, VistaTransformMatrix& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			std::string sCleanedSource = sSource;
			float a16fBuffer[16];
			RemoveBraces( sCleanedSource );
			if( ArrayFromString<16, float>( sCleanedSource, a16fBuffer, cSeparator ) == false )
				return false;

			oTarget.SetValues( a16fBuffer );
			return true;
		}

	};

	template<>
	struct StringConvertObject<VistaColor>
	{
		static void ToString( const VistaColor& oSource, std::string& sTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			char aBuffer[128];
			if( oSource[3] == 1 )
			{
				sprintf( aBuffer, "%g%c %g%c %g",
						(double)oSource[0], cSeparator,
						(double)oSource[1], cSeparator,
						(double)oSource[2] );
			}
			else
			{
				sprintf( aBuffer, "%g%c %g%c %g%c %g",
						(double)oSource[0], cSeparator,
						(double)oSource[1], cSeparator,
						(double)oSource[2], cSeparator,
						(double)oSource[3] );
			}
			sTarget = std::string( aBuffer );
		}
		static bool FromString( const std::string& sSource, VistaColor& oTarget,
								char cSeparator = S_cDefaultSeparator )
		{
			std::string sCleanedSource = sSource;
			RemoveBraces( sCleanedSource );

			std::vector<float> vecEntries;

			if( StringConvertObject<std::vector<float> >::FromString(
									sCleanedSource, vecEntries, cSeparator ) == false )
			{
				// no float list - maybe it's a predefined name
				return VistaColor::GetColorFromName( sSource, oTarget );
			}

			if( vecEntries.size() == 3 )
			{
				oTarget[0] = vecEntries[0];
				oTarget[1] = vecEntries[1];
				oTarget[2] = vecEntries[2];
				oTarget[3] = 1.0f;
			}
			else if( vecEntries.size() == 4 )
			{
				oTarget[0] = vecEntries[0];
				oTarget[1] = vecEntries[1];
				oTarget[2] = vecEntries[2];
				oTarget[3] = vecEntries[3];
			}
			else
				return false;

			return true;
		}
	};

}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACONVERSIONUTILS_H


