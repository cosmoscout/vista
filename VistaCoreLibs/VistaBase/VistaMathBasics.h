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


#ifndef _VISTAMATHBASICS_H
#define _VISTAMATHBASICS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <cassert>
#include <cmath>
#include <limits>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

// windows.h may define min and max macros that break the std::min/max and
// numeric_limints::Min/max, so we undefine them
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace Vista
{
	const float Pi      = 3.14159265358979323846f;
	const float Epsilon = std::numeric_limits<float>::epsilon();

	enum AXIS
	{
		X	= 0,
		Y	= 1,
		Z	= 2,
		W	= 3
	};	

	float DegToRad( const float fDegrees );
	float RadToDeg( const float fRadians );

	// true iff fValue not NaN, not infinity, not degenerated
	template< typename TFloat >
	bool IsValidNumber( const TFloat fValue );

	// clamps val to the range [minVal, maxVal]
	template< typename TFloat > 
	TFloat Clamp( const TFloat val, const TFloat minVal, const TFloat maxVal );

	// mixes valA and valB using a param in [0, 1]
	template< typename TFloat, typename TParam >
	TFloat Mix( const TParam param, const TFloat valA, const TFloat valB );
	
	// true iff val >= minVal && val <= maxVal
	template< typename TFloat >
	bool GetIsInRangeInclusive(const TFloat val, const TFloat minVal, const TFloat maxVal);

	// true iff val > minVal && val < maxVal
	template< typename TFloat >
	bool GetIsInRangeExclusive(const TFloat val, const TFloat minVal, const TFloat maxVal);


	template< typename TFloat >
	TFloat Sign( const TFloat val );
} // namespace Vista

/**
 * Convert angles given in radians to degrees and vice versa.
 */
inline float Vista::DegToRad( const float fDegrees )
{
	return fDegrees / 180.0f * Vista::Pi;
}

inline float Vista::RadToDeg( const float fRadians )
{
	return fRadians * 180.0f / Vista::Pi;
}

template< typename TFloat >
inline bool Vista::IsValidNumber( const TFloat nNumber )
{
	if( nNumber != nNumber )
		return false;
	if( nNumber == 0 )
		return true;
	TFloat nAbs = std::abs( nNumber );
	if( nAbs == std::numeric_limits<TFloat>::infinity() )
		return false;
	if( nAbs < std::numeric_limits<TFloat>::min() )
		return false;
	return true;
}

template< typename TFloat >
TFloat Vista::Clamp( const TFloat val, const TFloat minVal, const TFloat maxVal )
{
	// @todo Interfers with min/max macros on windows.
	//return std::min( std::max( nVal, nMinVal ), nMaxVal );

	//     ------- min check ----|------------- max check --------
	return val < minVal ? minVal : ( val > maxVal ? maxVal : val );
}

template< typename TFloat, typename TParam >
TFloat Vista::Mix( const TParam param, const TFloat valA, const TFloat valB )
{
	assert( param >= TParam( 0.0 ) && param <= TParam( 1.0 ) && "Invalid mix param." );
	return ( TParam( 1.0 ) - param ) * valA + param * valB;
}

template< typename TFloat >
bool Vista::GetIsInRangeInclusive(const TFloat val, const TFloat minVal, const TFloat maxVal)
{
	return ((val >= minVal) && (val <= maxVal));
}

template< typename TFloat >
bool Vista::GetIsInRangeExclusive(const TFloat val, const TFloat minVal, const TFloat maxVal)
{
	return ((val > minVal) && (val < maxVal));
}


template< typename TFloat >
TFloat Vista::Sign( const TFloat val )
{
	const bool bPositive = ( val >= TFloat( 0. ) );
	return ( bPositive ? TFloat( 1. ) : TFloat( -1. ) );
}


#endif //_VISTAMATHBASICS_H
