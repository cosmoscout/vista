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


#ifndef _VISTAVECTOR3D_H
#define _VISTAVECTOR3D_H

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

class VISTABASEAPI VistaVector3D
{	
public:
	VistaVector3D();
	VistaVector3D( float fX, float fY, float fZ )	;
	VistaVector3D( float fX, float fY, float fZ, float fW );
	explicit VistaVector3D( const float a3fValues[3] );
	explicit VistaVector3D( const double a3dValues[3] );

	void SetValues( const float a3fValues[3] );
	void SetValues( const double a3dValues[3] );
	void SetValues( float fX, float fY, float fZ, float fW = 1.0f );
	void SetValues( double fX, double fY, double fZ, double fW = 1.0 );
	void GetValues( float a3fValues[3] ) const;
	void GetValues( double a3dValues[3] ) const;
	void GetValues( float& fX, float& fY, float& fZ ) const;
	void GetValues( double& fX, double& fY, double& fZ ) const;
	void GetValues( float& fX, float& fY, float& fZ, float& fW ) const;
	void GetValues( double& fX, double& fY, double& fZ, double& fW ) const;
	VistaVector3D GetNormalized() const;
	VistaVector3D GetHomogenized() const;
	VistaVector3D GetAbsolute() const;
	float GetLength() const;
	float GetLengthSquared() const;
	VistaVector3D Interpolate( const VistaVector3D& v3End, float fFraction ) const;

	void Normalize();
	void Homogenize();
	void SetToZeroVector();
	bool GetIsZeroVector() const;
	float Dot( const VistaVector3D& v3Other ) const;
	VistaVector3D Cross( const VistaVector3D& v3Other ) const;
	VistaVector3D ComponentWiseProduct( const VistaVector3D& v3Other ) const;

	void ComponentWiseClamp(const VistaVector3D& v3Min, const VistaVector3D& v3Max);
	VistaVector3D GetComponentWiseClamp(const VistaVector3D& v3Min, const VistaVector3D& v3Max) const;


	/**
	 * Checks whether the Vector is valid, i.e. whether or not it contains
	 * NaNs or INFs
	 */
	bool CheckForValidity() const;

	VistaVector3D& operator=( const VistaVector3D& v3Other );
	VistaVector3D& operator+=( const VistaVector3D& v3Other );
	VistaVector3D& operator-=( const VistaVector3D& v3Other );
	VistaVector3D& operator*=( const float fScale );
	VistaVector3D& operator/=( const float fScale );

	const float& operator[] ( const int iAxis ) const;
	float& operator[] ( const int iAxis );

private:
	float		m_a4fValues[4];
};

const VistaVector3D operator-( const VistaVector3D& v3Vector );
const VistaVector3D operator+( const VistaVector3D& v3Left, const VistaVector3D& v3Right );
const VistaVector3D operator-( const VistaVector3D& v3Left, const VistaVector3D& v3Right );
const VistaVector3D operator*( const float fScale, const VistaVector3D& v3Vec );
const VistaVector3D operator*( const VistaVector3D& v3Vec, const float fScale );
const VistaVector3D operator/( const VistaVector3D& v3Vec, const float fScale );	
float operator* ( const VistaVector3D& v3Left, const VistaVector3D& v3Right );
bool operator== ( const VistaVector3D& v3Left, const VistaVector3D& v3Right );
bool operator!= ( const VistaVector3D& v3Left, const VistaVector3D& v3Right );

std::ostream& operator<<( std::ostream& oStream, 
										const VistaVector3D& v3Vector );

namespace Vista
{
	const VistaVector3D ViewVector = VistaVector3D( 0, 0, -1, 0 );
	const VistaVector3D UpVector = VistaVector3D( 0, 1, 0, 0 );
	const VistaVector3D ZeroVector = VistaVector3D( 0, 0, 0, 0 );
	const VistaVector3D XAxis = VistaVector3D( 1, 0, 0, 0 );
	const VistaVector3D YAxis = VistaVector3D( 0, 1, 0, 0 );
	const VistaVector3D ZAxis = VistaVector3D( 0, 0, 1, 0 );	
}

/*============================================================================*/
/* INLINE IMPLEMENTATIONS                                                     */
/*============================================================================*/

///// CTORS /////
inline VistaVector3D::VistaVector3D()
{
	SetToZeroVector();
}
inline VistaVector3D::VistaVector3D( float fX, float fY, float fZ )	
{
	m_a4fValues[0] = fX;
	m_a4fValues[1] = fY;
	m_a4fValues[2] = fZ;
	m_a4fValues[3] = 1.0f;
}
inline VistaVector3D::VistaVector3D( float fX, float fY, float fZ, float fW )
{
	m_a4fValues[0] = fX;
	m_a4fValues[1] = fY;
	m_a4fValues[2] = fZ;
	m_a4fValues[3] = fW;
}
inline VistaVector3D::VistaVector3D( const float a3fValues[3] )
{
	m_a4fValues[0] = a3fValues[0];
	m_a4fValues[1] = a3fValues[1];
	m_a4fValues[2] = a3fValues[2];
	m_a4fValues[3] = 1.0f;
}
inline VistaVector3D::VistaVector3D( const double a3dValues[3] )
{
	m_a4fValues[0] = (float)a3dValues[0];
	m_a4fValues[1] = (float)a3dValues[1];
	m_a4fValues[2] = (float)a3dValues[2];
	m_a4fValues[3] = 1.0f;
}

// member operators

inline VistaVector3D& VistaVector3D::operator=( const VistaVector3D& v3Other )
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
inline VistaVector3D& VistaVector3D::operator+=( const VistaVector3D& v3Other )
{
	m_a4fValues[0] += v3Other.m_a4fValues[0];
	m_a4fValues[1] += v3Other.m_a4fValues[1];
	m_a4fValues[2] += v3Other.m_a4fValues[2];
	return (*this);
}
inline VistaVector3D& VistaVector3D::operator-=( const VistaVector3D& v3Other )
{
	m_a4fValues[0] -= v3Other.m_a4fValues[0];
	m_a4fValues[1] -= v3Other.m_a4fValues[1];
	m_a4fValues[2] -= v3Other.m_a4fValues[2];
	return (*this);
}
inline VistaVector3D& VistaVector3D::operator*=( const float fScale )
{
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
	return (*this);
}
inline VistaVector3D& VistaVector3D::operator/=( const float fScale )
{
	// interestingly, /= is faster than caching the div results
	// thanks, compiler optimization :D
	//const float fIScale = 1.0f / fScale;
	//m_a4fValues[0] *= fIScale;
	//m_a4fValues[1] *= fIScale;
	//m_a4fValues[2] *= fIScale;
	m_a4fValues[0] /= fScale;
	m_a4fValues[1] /= fScale;
	m_a4fValues[2] /= fScale;
	return (*this);
}

inline const float& VistaVector3D::operator[] ( const int iAxis ) const
{
	return m_a4fValues[iAxis];
}
inline float& VistaVector3D::operator[] ( const int iAxis )
{
	return m_a4fValues[iAxis];
}


///// FUNCTIONS /////
inline bool VistaVector3D::CheckForValidity() const
{
	return ( Vista::IsValidNumber( m_a4fValues[0] )
			&& Vista::IsValidNumber( m_a4fValues[1] )
			&& Vista::IsValidNumber( m_a4fValues[2] )
			&& Vista::IsValidNumber( m_a4fValues[3] ) );
}

inline void VistaVector3D::SetValues( const float a3fValues[3] )
{
	m_a4fValues[0] = a3fValues[0];
	m_a4fValues[1] = a3fValues[1];
	m_a4fValues[2] = a3fValues[2];
}
inline void VistaVector3D::SetValues( const double a3dValues[3] )
{
	m_a4fValues[0] = (float)a3dValues[0];
	m_a4fValues[1] = (float)a3dValues[1];
	m_a4fValues[2] = (float)a3dValues[2];
}

inline void VistaVector3D::SetValues( float fX, float fY, float fZ, float fW )
{
	m_a4fValues[0] = fX;
	m_a4fValues[1] = fY;
	m_a4fValues[2] = fZ;
	m_a4fValues[3] = fW;
}

inline void VistaVector3D::SetValues( double fX, double fY, double fZ, double fW )
{
	m_a4fValues[0] = (float)fX;
	m_a4fValues[1] = (float)fY;
	m_a4fValues[2] = (float)fZ;
	m_a4fValues[3] = (float)fW;
}
inline void VistaVector3D::GetValues( float a3fValues[3] ) const
{
	a3fValues[0] = m_a4fValues[0];
	a3fValues[1] = m_a4fValues[1];
	a3fValues[2] = m_a4fValues[2];
}
inline void VistaVector3D::GetValues( double a3dValues[3] ) const
{
	a3dValues[0] = (double)m_a4fValues[0];
	a3dValues[1] = (double)m_a4fValues[1];
	a3dValues[2] = (double)m_a4fValues[2];
}

inline void VistaVector3D::GetValues( float& fX, float& fY, float& fZ, float& fW ) const
{
	fX = m_a4fValues[0];
	fY = m_a4fValues[1];
	fZ = m_a4fValues[2];
	fW = m_a4fValues[3];
}

inline void VistaVector3D::GetValues( double& fX, double& fY, double& fZ, double& fW ) const
{
	fX = (double)m_a4fValues[0];
	fY = (double)m_a4fValues[1];
	fZ = (double)m_a4fValues[2];
	fW = (double)m_a4fValues[3];
}

inline void VistaVector3D::GetValues( float& fX, float& fY, float& fZ ) const
{
	fX = m_a4fValues[0];
	fY = m_a4fValues[1];
	fZ = m_a4fValues[2];
}

inline void VistaVector3D::GetValues( double& fX, double& fY, double& fZ ) const
{
	fX = (double)m_a4fValues[0];
	fY = (double)m_a4fValues[1];
	fZ = (double)m_a4fValues[2];
}

inline VistaVector3D VistaVector3D::GetNormalized() const
{
	const float fScale = 1.0f / GetLength();
	if( Vista::IsValidNumber( fScale ) == false )
		return VistaVector3D();
	return VistaVector3D( fScale * m_a4fValues[0],
						fScale * m_a4fValues[1], 
						fScale * m_a4fValues[2],
						m_a4fValues[3] );
}
inline VistaVector3D VistaVector3D::GetHomogenized() const
{
	if( m_a4fValues[3] < Vista::Epsilon )
		return (*this);
	const float fScale = 1.0f / m_a4fValues[3];
	return VistaVector3D( fScale * m_a4fValues[0],
							fScale * m_a4fValues[1],
							fScale * m_a4fValues[2],
							1.0f );
}
inline VistaVector3D VistaVector3D::GetAbsolute() const
{	
	return VistaVector3D( std::abs( m_a4fValues[0] ),
							std::abs( m_a4fValues[1] ),
							std::abs( m_a4fValues[2] ),
							std::abs( m_a4fValues[3] ) );
}
inline float VistaVector3D::GetLength() const
{
	return std::sqrt( m_a4fValues[0]*m_a4fValues[0] 
			+ m_a4fValues[1]*m_a4fValues[1]
			+ m_a4fValues[2]*m_a4fValues[2] );
}
inline float VistaVector3D::GetLengthSquared() const
{
	return ( m_a4fValues[0]*m_a4fValues[0] 
			+ m_a4fValues[1]*m_a4fValues[1]
			+ m_a4fValues[2]*m_a4fValues[2] );
}
inline VistaVector3D VistaVector3D::Interpolate( const VistaVector3D& v3End, float fFraction ) const
{
	float fOwn = 1.0f - fFraction;
	return VistaVector3D( fOwn * m_a4fValues[0] + fFraction * v3End.m_a4fValues[0],
						fOwn * m_a4fValues[1] + fFraction * v3End.m_a4fValues[1],
						fOwn * m_a4fValues[2] + fFraction * v3End.m_a4fValues[2] );
}
inline void VistaVector3D::Normalize()
{
	float fScale = 1.0f / GetLength();
	if( Vista::IsValidNumber( fScale ) )
	{
		m_a4fValues[0] *= fScale;
		m_a4fValues[1] *= fScale;
		m_a4fValues[2] *= fScale;
	}
}
inline void VistaVector3D::Homogenize()
{
	if( m_a4fValues[3] < Vista::Epsilon )
	{
		m_a4fValues[3] = 0.0f;
		return;
	}
	float fScale = 1.0f / m_a4fValues[3];
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
	m_a4fValues[3] = 1.0f;
}
inline void VistaVector3D::SetToZeroVector()
{
	m_a4fValues[0] = 0.0f;
	m_a4fValues[1] = 0.0f;
	m_a4fValues[2] = 0.0f;
	m_a4fValues[3] = 1.0f;
}
inline bool VistaVector3D::GetIsZeroVector() const
{
	/** @todo maybe check using epsilon-compare */
	return( m_a4fValues[0] == 0.0f
			&& m_a4fValues[1] == 0.0f
			&& m_a4fValues[2] == 0.0f );
}
inline float VistaVector3D::Dot( const VistaVector3D& v3Other ) const
{
	return ( (*this) * v3Other );
	//return float( m_a4fValues[0] * v3Other[0] 
	//			+ m_a4fValues[1] * v3Other[1]
	//			+ m_a4fValues[2] * v3Other[2] );
}
inline VistaVector3D VistaVector3D::Cross( const VistaVector3D& v3Other ) const
{
	VistaVector3D v3Ret;
	v3Ret[0] = m_a4fValues[1] * v3Other[2]
				- m_a4fValues[2] * v3Other[1];
	v3Ret[1] = m_a4fValues[2] * v3Other[0]
				- m_a4fValues[0] * v3Other[2];
	v3Ret[2] = m_a4fValues[0] * v3Other.m_a4fValues[1]
				- m_a4fValues[1] * v3Other[0];
	return v3Ret;
	//return  VistaVector3D( m_a4fValues[1] * v3Other.m_a4fValues[2]
	//						- m_a4fValues[2] * v3Other.m_a4fValues[1],
	//					m_a4fValues[2] * v3Other.m_a4fValues[0]
	//						- m_a4fValues[0] * v3Other.m_a4fValues[2],
	//					m_a4fValues[0] * v3Other.m_a4fValues[1] 
	//						- m_a4fValues[1] * v3Other.m_a4fValues[0] );
}

inline VistaVector3D VistaVector3D::ComponentWiseProduct( const VistaVector3D& v3Other ) const
{
	return  VistaVector3D( m_a4fValues[0] * v3Other[0],
						   m_a4fValues[1] * v3Other[1],
						   m_a4fValues[2] * v3Other[2] );
}

inline void VistaVector3D::ComponentWiseClamp(const VistaVector3D& v3Min, const VistaVector3D& v3Max)
{
	m_a4fValues[0] = Vista::Clamp(m_a4fValues[0], v3Min[0], v3Max[0]);
	m_a4fValues[1] = Vista::Clamp(m_a4fValues[1], v3Min[1], v3Max[1]);
	m_a4fValues[2] = Vista::Clamp(m_a4fValues[2], v3Min[2], v3Max[2]);
}

inline VistaVector3D VistaVector3D::GetComponentWiseClamp(const VistaVector3D& v3Min, const VistaVector3D& v3Max) const
{
	return VistaVector3D(
		Vista::Clamp(m_a4fValues[0], v3Min[0], v3Max[0]),
		Vista::Clamp(m_a4fValues[1], v3Min[1], v3Max[1]),
		Vista::Clamp(m_a4fValues[2], v3Min[2], v3Max[2]),
		m_a4fValues[3]);
}

///// global OPERATORS /////
inline const VistaVector3D operator+ ( const VistaVector3D& v3Left, const VistaVector3D& v3Right )
{	
	VistaVector3D v3Res( v3Left );
	v3Res += v3Right;
	return v3Res;
}
inline const VistaVector3D operator- ( const VistaVector3D& v3Left, const VistaVector3D& v3Right )
{
	VistaVector3D v3Res( v3Left );
	v3Res -= v3Right;
	return v3Res;
}
inline const VistaVector3D operator*( const VistaVector3D& v3Vec, const float fScale )
{
	VistaVector3D v3Res( v3Vec );
	v3Res *= fScale;
	return v3Res;
}
inline const VistaVector3D operator*( const float fScale, const VistaVector3D& v3Vec )
{
	return v3Vec * fScale;
}

inline const VistaVector3D operator/( const VistaVector3D& v3Vec, const float fScale )
{
	VistaVector3D v3Res( v3Vec );
	v3Res *= 1.0f / fScale;
	return v3Res;
}

inline const VistaVector3D operator-( const VistaVector3D& v3Vec )
{
	return VistaVector3D( -v3Vec[0], -v3Vec[1], -v3Vec[2], v3Vec[3] );
}

inline float operator* ( const VistaVector3D& v3Left, const VistaVector3D& v3Right )
{
	return ( v3Left[0] * v3Right[0]	+ v3Left[1] * v3Right[1] + v3Left[2] * v3Right[2] );
}

inline bool operator== ( const VistaVector3D& v3Left, const VistaVector3D& v3Right )
{
	return( v3Left[0] == v3Right[0]
			&& v3Left[1] == v3Right[1]
			&& v3Left[2] == v3Right[2]
			&& v3Left[3] == v3Right[3] );
}
inline bool operator!= ( const VistaVector3D& v3Left, const VistaVector3D& v3Right )
{
	return !operator==( v3Left, v3Right );
}


inline std::ostream& operator<<( std::ostream& oStream, const VistaVector3D& v3Vector )
{
	const std::streamsize iOldPrecision( oStream.precision( 3 ) );
	const std::ios::fmtflags iOldflags( oStream.flags() );

	// set fix point notation
	oStream.setf( std::ios::fixed | std::ios::showpos );

	// write to the stream
	oStream << "( " << v3Vector[0] 
			<< ", " << v3Vector[1]
			<< ", " << v3Vector[2];
	oStream.unsetf( std::ios::fixed | std::ios::showpos );
	oStream << " | " << v3Vector[3] << " )";

	// restore old stream settings
	oStream.precision( iOldPrecision );
	oStream.flags( iOldflags );

	return oStream;
}


#endif //_VISTAVECTOR3D_H
