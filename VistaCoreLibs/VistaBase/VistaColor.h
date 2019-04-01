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


#ifndef _VISTACOLOR_H
#define _VISTACOLOR_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaBaseConfig.h"

#include "VistaMathBasics.h"

#include <cmath>
#include <iostream>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Class representing a color in RGBA format. It can also be used as RGB only by
 * just ignoring the alpha channel. It provides conversion functions from and to
 * other color models (only HSL and HSV at the moment, but perhaps somebody will
 * add more color models in the future ^^).
 */
class VISTABASEAPI VistaColor
{	
public:

	enum
	{
		AQUAMARINE              = 0x7FFFD4,
		BLACK                   = 0x000000,
		BLUE                    = 0x0000FF,
		BRICK_RED               = 0xB22222,
		BROWN                   = 0xA52A2A,
		CORAL                   = 0xFF7F50,
		CYAN                    = 0x00FFFF,
		FOREST_GREEN    		= 0x228B22,
		GOLD                    = 0xFFD700,
		GRAY                    = 0x808080,
		GREEN                   = 0x00FF00,
		GREEN_YELLOW    		= 0xADFF2F,
		INDIGO                  = 0x4B0082,
		KHAKI                   = 0xBDB76B,
		LAWN_GREEN              = 0x7CFC00,
		LIME                    = 0x32CD32,
		MAGENTA                 = 0xFF00FF,
		MAROON                  = 0x800000,
		NAVY                    = 0x000080,
		OLIVE                   = 0x808000,
		ORANGE                  = 0xFFA500,
		ORANGE_RED              = 0xFF4500,
		PINK                    = 0xFF1493,
		PURPLE                  = 0x800080,
		SKY_BLUE                = 0x00BFFF,
		RED                     = 0xFF0000,
		ROYAL_BLUE              = 0x4169E1,
		SALMON                  = 0xFA8072,
		VIOLET                  = 0x8A2BE2,
		WHITE                   = 0xFFFFFF,
		YELLOW                  = 0xFFFF00,
		LIGHT_BLUE              = 0xADD8E6,
		LIGHT_GRAY              = 0xC0C0C0,
		LIGHT_GREEN             = 0x90EE90,
		LIGHT_ORANGE    		= 0xFFA07A,
		LIGHT_PINK              = 0xFFB6C1,
		LIGHT_SKY_BLUE  		= 0x87CEFA,
		LIGHT_STEEL_BLUE		= 0xB0C4DE,
		DARK_BLUE               = 0x00008B,
		DARK_CYAN               = 0x008B8B,
		DARK_GREEN              = 0x006400,
		DARK_MAGENTA   			= 0x8B008B,
		DARK_RED                = 0x8B0000,
		DARK_OLIVE              = 0x556B2F,
		DARK_ORANGE             = 0xFF8C00,
		DARK_ORCHID             = 0x9932CC,
		DARK_SALMON             = 0xE9967A,
		DARK_TURQUOISE  		= 0x00CED1,
		VISTA_BLUE		  		= 0x0066CC,
		RWTH_BLUE				= 0x00549F,
		RWTH_BLACK				= 0x000000,
		RWTH_MAGENTA			= 0xE30066,
		RWTH_YELLOW				= 0xFFED00,
		RWTH_PETROL				= 0x006165,
		RWTH_TURQUOISE			= 0x0098A1,
		RWTH_GREEN				= 0x57AB27,
		RWTH_MAY_GREEN			= 0xBDCD00,
		RWTH_ORANGE				= 0xF6A800,
		RWTH_RED				= 0xCC071E,
		RWTH_BORDEAUX			= 0xA11035,
		RWTH_VIOLET				= 0x612158,
		RWTH_PURPLE				= 0x7A6FAC
	};

	/**
	 * Interpretes a string as a color. The string has to contain a color name as it
	 * is listed in the above enum. Names are case-insensitive and may use spaces and
	 * underscores interchangeably
	 */
	static bool GetColorFromName( const std::string& sColorName, VistaColor& oResult );

	enum EFormat
	{
		RGB,
		RGBA,
		HSL,
		HSLA,
		HSV,
		HSVA,
		LAB,
		LABA
	};

	/**
	 * ctors. Construct a color from various inputs. When using RGB/HSV/HSL as
	 * format, only the first three entries of arrays will be used and alpha
	 * will be set to 1. When using RGBA/HSVA/HSLA the first four entries will
	 * be used.
	 */
	VistaColor( int iColor = WHITE, EFormat eFormat = RGB );
	VistaColor( int, int, int, EFormat eFormat = RGB);
	VistaColor( float, float, float, EFormat eFormat = RGB);
	VistaColor( double, double, double, EFormat eFormat = RGB);
	VistaColor( int, int, int, int, EFormat eFormat = RGBA );
	VistaColor( float, float, float, float, EFormat eFormat = RGBA );
	VistaColor( double, double, double, double, EFormat eFormat = RGBA );
	VistaColor( const int aiValues[], EFormat eFormat = RGB );
	VistaColor( const float afValues[], EFormat eFormat = RGB );
	VistaColor( const double adValues[], EFormat eFormat = RGB );

	/**
	 * @see ctors
	 */
	void SetValues( int iColor, EFormat eFormat = RGB );
	void SetValues( int, int, int, EFormat eFormat = RGB );
	void SetValues( float, float, float, EFormat eFormat = RGB );
	void SetValues( double, double, double, EFormat eFormat = RGB );
	void SetValues( int, int, int, int, EFormat eFormat = RGBA);
	void SetValues( float, float, float, float, EFormat eFormat = RGBA );
	void SetValues( double, double, double, double, EFormat eFormat = RGBA );
	void SetValues( const int aiValues[], EFormat eFormat = RGB );
	void SetValues( const float afValues[], EFormat eFormat = RGB );
	void SetValues( const double adValues[], EFormat eFormat = RGB );

	/**
	 * Setters for the RGBA values
	 */
	void SetRed( float fR );
	void SetGreen( float fG );
	void SetBlue( float fB );
	void SetAlpha( float fA );

	/**
	 * Getters for HSV/HSL values. These are time consuming because a conversion
	 * from RGB and back is performed upon each call!
	 */
	void SetHue( float fHue );
	void SetHSLSaturation( float fSaturation );
	void SetLightness( float fLightness );
	void SetHSVSaturation( float fSaturation );
	void SetValue( float fValue );

	/**
	 * @see ctors
	 */
	int GetValues( EFormat eFormat = RGB ) const;
	void GetValues( int aiValues[], EFormat eFormat = RGB) const;
	void GetValues( float afValues[], EFormat eFormat = RGB ) const;
	void GetValues( double adValues[], EFormat eFormat = RGB ) const;

	/**
	 * Getters for the RGBA values
	 */
	float GetRed() const;
	float GetGreen() const;
	float GetBlue() const;
	float GetAlpha() const;

	/**
	 * Getters for HSV/HSL values. These are time consuming because a conversion
	 * from RGB is performed upon each call!
	 */
	float GetHue() const;
	float GetHSLSaturation() const;
	float GetLightness() const;
	float GetHSVSaturation() const;
	float GetValue() const;

	/**
	 * Mixes to colors by its RGBA values. Fraction can be used to create an
	 * imbalanced mixing or for interpolations
	 */
	VistaColor Mix(const VistaColor& colEnd, float fFraction = .5f,
		EFormat eFormat = VistaColor::RGBA ) const;

	/**
	 * Clamps all RGBA values to [0.f;1.f]
	 */
	void ClampValues();

	/**
	 * These methods work the same way as the operators, but only operate on
	 * RGB values. The alpha channel remains untouched.
	 */
	VistaColor& AddRGB(const VistaColor &oOther);
	VistaColor& SubtractRGB(const VistaColor &oOther);
	VistaColor& MultiplyRGB(const float fScale);
	VistaColor& DivideRGB(const float fScale);

	/**
	 * All operators work on the full RGBA representation, so alpha values are
	 * added/subtracted/multiplied or divided too.
	 */
	VistaColor& operator=( const VistaColor& oOther );
	VistaColor& operator+=( const VistaColor& oOther );
	VistaColor& operator-=( const VistaColor& oOther );
	VistaColor& operator*=( const float fScale );
	VistaColor& operator/=( const float fScale );

	/**
	 * Access RGBA channels directly.
	 */
	const float& operator[] ( const int iChannel ) const;
	float& operator[] ( const int iChannel );

	/**
	 * Conversion methods for color model conversion.
	 */
	static void HSLtoRGB(const float a3fHSL[3], float a3fRGB[3]);
	static void RGBtoHSL(const float a3fRGB[3], float a3fHSL[3]);
	
	static void HSLtoHSV(const float a3fHSL[3], float a3fHSV[3]);
	static void HSVtoHSL(const float a3fHSV[3], float a3fHSL[3]);

	static void HSVtoRGB(const float a3fHSV[3], float a3fRGB[3]);
	static void RGBtoHSV(const float a3fRGB[3], float a3fHSV[3]);


	static void LabtoRGB(const float a3fLAB[3], float a3fRGB[3]);
	// note: the extreme values for L,a,b coming out of this conversion
	// are approximately: 
	// L: [0, 100]
	// a: [-87, 99]
	// b: [-108, 95]
	// (though not all of these combinations correspond to
	// actual RGB colors, these are just 1-dim minima/maxima)
	static void RGBtoLab(const float a3fRGB[3], float a3fLAB[3]);

private:

	/**
	 * Helper method for HSL conversion.
	 */
	static float HueToRGB(float f1, float f2, float fHue);

	float		m_a4fValues[4];

};

const VistaColor operator+ ( 
	const VistaColor& oLeft, const VistaColor& oRight );
const VistaColor operator- ( 
	const VistaColor& oLeft, const VistaColor& oRight );
const VistaColor operator*( const VistaColor& oColor, const float fScale );
const VistaColor operator*( const float fScale, const VistaColor& oColor );
const VistaColor operator/( const VistaColor& oColor, const float fScale );
bool operator== ( const VistaColor& oLeft, const VistaColor& oRight );
bool operator!= ( const VistaColor& oLeft, const VistaColor& oRight );
std::ostream& operator<<( std::ostream& oStream, const VistaColor& oColor );

/*============================================================================*/
/* INLINE IMPLEMENTATIONS                                                     */
/*============================================================================*/

inline VistaColor& VistaColor::operator=( const VistaColor& v3Other )
{
	if( this != &v3Other )
	{
		m_a4fValues[0] = v3Other.m_a4fValues[0];
		m_a4fValues[1] = v3Other.m_a4fValues[1];
		m_a4fValues[2] = v3Other.m_a4fValues[2];
		m_a4fValues[3] = v3Other.m_a4fValues[3];
	}
	return (*this);
}

inline VistaColor& VistaColor::operator+=( const VistaColor& v3Other )
{
	m_a4fValues[0] += v3Other.m_a4fValues[0];
	m_a4fValues[1] += v3Other.m_a4fValues[1];
	m_a4fValues[2] += v3Other.m_a4fValues[2];
	m_a4fValues[3] += v3Other.m_a4fValues[3];
	return (*this);
}

inline VistaColor& VistaColor::operator-=( const VistaColor& v3Other )
{
	m_a4fValues[0] -= v3Other.m_a4fValues[0];
	m_a4fValues[1] -= v3Other.m_a4fValues[1];
	m_a4fValues[2] -= v3Other.m_a4fValues[2];
	m_a4fValues[3] -= v3Other.m_a4fValues[3];
	return (*this);
}

inline VistaColor& VistaColor::operator*=( const float fScale )
{
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
	m_a4fValues[3] *= fScale;
	return (*this);
}

inline VistaColor& VistaColor::operator/=( const float fScale )
{
	m_a4fValues[0] /= fScale;
	m_a4fValues[1] /= fScale;
	m_a4fValues[2] /= fScale;
	m_a4fValues[3] /= fScale;
	return (*this);
}

inline const float& VistaColor::operator[] ( const int iChannel ) const
{
	return m_a4fValues[iChannel];
}

inline float& VistaColor::operator[] ( const int iChannel )
{
	return m_a4fValues[iChannel];
}

inline const VistaColor operator+ ( 
	const VistaColor& oLeft, const VistaColor& oRight )
{	
	VistaColor oRes( oLeft );
	oRes += oRight;
	return oRes;
}

inline const VistaColor operator- ( 
	const VistaColor& oLeft, const VistaColor& oRight )
{
	VistaColor oRes( oLeft );
	oRes -= oRight;
	return oRes;
}

inline const VistaColor operator*( 
	const VistaColor& oColor, const float fScale )
{
	VistaColor oRes( oColor );
	oRes *= fScale;
	return oRes;
}

inline const VistaColor operator*( 
	const float fScale, const VistaColor& oColor )
{
	return oColor * fScale;
}

inline const VistaColor operator/( 
	const VistaColor& oColor, const float fScale )
{
	VistaColor oRes( oColor );
	oRes /= fScale;
	return oRes;
}

inline bool operator== ( const VistaColor& oLeft, const VistaColor& oRight )
{
	return( oLeft[0] == oRight[0]
	&& oLeft[1] == oRight[1]
	&& oLeft[2] == oRight[2]
	&& oLeft[3] == oRight[3] );
}

inline bool operator!= ( const VistaColor& oLeft, const VistaColor& oRight )
{
	return !operator==( oLeft, oRight );
}

inline std::ostream& operator<<( 
	std::ostream& oStream, const VistaColor& oColor )
{
	const std::streamsize iOldPrecision( oStream.precision( 3 ) );
	const std::ios::fmtflags iOldflags( oStream.flags() );

	// set fix point notation
	oStream.setf( std::ios::fixed | std::ios::showpos );

	// write to the stream
	oStream 
		<< "(r:" << oColor[0] 
	<< ", g:" << oColor[1]
	<< ", b:" << oColor[2]
	<< ", a:" << oColor[3]
	<< ")";

	oStream.unsetf( std::ios::fixed | std::ios::showpos );

	// restore old stream settings
	oStream.precision( iOldPrecision );
	oStream.flags( iOldflags );

	return oStream;
}

#endif // include guard
