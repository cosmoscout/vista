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


#ifndef _VISTAMATHTOOLS_H__
#define _VISTAMATHTOOLS_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <limits>
//#include <cmath>     // sin, cos, sqrt ...
#include <complex>
//#include <algorithm>

/*============================================================================*/
/* MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CONSTANTS                                                                  */
/*============================================================================*/
//#define VISTA_MATH_TOOLS_EPSILON std::numeric_limits<float>::epsilon()

/*============================================================================*/
/* ENUMERATIONS                                                               */
/*============================================================================*/


template <class Type>
class VistaMathTools
{
public:

	static inline Type SQR( const Type &a )
		{ return a * a; };
	// USE: <algorithm>::min/max
	//static inline const Type MIN( const Type a, const Type b ) { return b < a ? (b) : (a); };
	//static inline const Type MAX( const Type a, const Type b ) { return b > a ? (b) : (a); };
	static inline Type SIGN( const Type &a, const Type &b )
		{ return b >= 0 ? ( a >= 0 ? a : -a ) : ( a >= 0 ? -a : a ); };

	// computes (a^2 + b^2)^(1/2) without destructive underflow or overflow
	static inline Type Pythagoras( const Type &a, const Type &b );

	static inline bool IsZero (const Type &val);
	static inline bool IsZeroSetZero (Type & val);
};

/*============================================================================*/
/*============================================================================*/

template <class Type>
inline
Type VistaMathTools<Type>::Pythagoras( const Type &a, const Type &b )
{
	double absa, absb;

	absa = std::abs( a );
	absb = std::abs( b );

	if (absa > absb)
		return absa*sqrt(1.0+SQR(absb/absa));
	else
		return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsZero                                                      */
/*                                                                            */
/*============================================================================*/
template <class Type>
inline
bool VistaMathTools<Type>::IsZero (const Type &val)
{
	if (std::abs(val) <= std::numeric_limits<Type>::epsilon())
		return true;
	return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsZero                                                      */
/*                                                                            */
/*============================================================================*/
template <>
inline
bool VistaMathTools< std::complex<double> >::IsZero (const std::complex<double> &val)
{
	int compCount = 0;
#if defined(_MSC_VER) && (_MSC_VER < 1400)
	if( std::abs(val.real()) <= std::numeric_limits<double>::epsilon() )
#else
	if (std::abs(val.real()) < (std::numeric_limits<double>::min)() /
								std::numeric_limits<double>::epsilon())
#endif
	{
		compCount++;
	}

#if defined(_MSC_VER) && (_MSC_VER < 1400)
	if( std::abs(val.real()) <= std::numeric_limits<double>::epsilon() )
#else
	if (std::abs(val.imag()) < (std::numeric_limits<double>::min)() /
								std::numeric_limits<double>::epsilon())
#endif
	{
		compCount++;
	}
	if (compCount == 2)
		return true;
	return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsZeroSetZero                                               */
/*                                                                            */
/*============================================================================*/
template <class Type>
inline
bool VistaMathTools<Type>::IsZeroSetZero (Type & val)
{
#if defined(_MSC_VER) && (_MSC_VER < 1400)
	if (std::abs(val) <= std::numeric_limits<Type>::epsilon())
#else
	if (std::abs(val) < (std::numeric_limits<Type>::min)() /
					std::numeric_limits<Type>::epsilon())
#endif
	{
		val = 0;
		return true;
	}
	return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsZeroSetZero                                               */
/*                                                                            */
/*============================================================================*/
template <>
inline
bool VistaMathTools< std::complex<double> >::IsZeroSetZero (std::complex<double> & val)
{
	int compCount = 0;

#if defined(_MSC_VER) && (_MSC_VER < 1400)
	if (std::abs(val.real()) <= std::numeric_limits<double>::epsilon())
#else
	if (std::abs(val.real()) < (std::numeric_limits<double>::min)() /
						   std::numeric_limits<double>::epsilon())
#endif
	{
		val = std::complex<double>(0,val.imag());
		compCount++;
	}

#if defined(_MSC_VER) && (_MSC_VER < 1400)
	if (std::abs(val.imag()) <= std::numeric_limits<double>::epsilon())
#else
	if (std::abs(val.imag()) < (std::numeric_limits<double>::min)() /
								std::numeric_limits<double>::epsilon())
#endif
	{
		val = std::complex<double>(val.real(),0);
		compCount++;
	}
	if (compCount == 2)
		return true;
	return false;
}

/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*============================================================================*/

#endif // ifndef _VISTAMATHTOOLS_H__

