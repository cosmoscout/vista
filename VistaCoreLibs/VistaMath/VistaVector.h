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


#ifndef _VISTAVECTOR_H
#define _VISTAVECTOR_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaMathConfig.h"

#include <VistaBase/VistaMathBasics.h>
#include <VistaBase/VistaStreamUtils.h>

//#include <iostream>
//#include <cmath>     // sin, cos, sqrt ...
#if !defined(WIN32)
#include <complex>
#endif

//#include <cstdarg>   // va_list, ...

#include <iostream>
#include <limits>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//#define Vista::Epsilon std::numeric_limits<float>::epsilon()
//#define Vista::Epsilon std::numeric_limits<Type>::epsilon()

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CONSTANTS                                                                  */
/*============================================================================*/

/*============================================================================*/
/* ENUMERATIONS                                                               */
/*============================================================================*/


/*============================================================================*/
/* VistaVector                                                            */
/*============================================================================*/
/**
 * VistaVector
 * Vector[col]
 */
template <class Type, int dim>
class VistaVector //: public VistaMatrix<Type>
{
public:
	/// constructs nDim vector, defaults are zero
	inline VistaVector (bool bSetNull = true);
	inline VistaVector (const VistaVector<Type,dim> & orgVec);
	//explicit inline VistaVector (const Type ...);
	explicit inline VistaVector (const Type *pInitVals);
	inline ~VistaVector();

protected:
//    inline void AllocCoeff();
//    inline void ReleaseCoeff();


public:
	inline bool SetNull();
	inline Type GetVal (int idx) const
		{return m_pVec[idx];}
	inline bool SetVal (int idx, const Type & newVal);
	//    {m_pMat[idx] = newVal; return true;}
	inline int GetDimension () const
		{return m_nDimension;}
	inline bool IsNull() const;
	inline bool Transpose();
	inline bool IsTransposed() const
	   {return m_bTransposed;}

	inline bool HasNAN() const
	{
		for( int n=0; n < GetDimension(); ++n )
			if( m_pVec[n] != m_pVec[n] )
				return true;
		return false;
	}


public:
	/*========================================================================*/
	/* OPERATORS                                                              */
	/*========================================================================*/
	// assignment operator
	inline VistaVector<Type,dim>& operator= (const VistaVector<Type,dim>& vec2);

	// calculation operators
	inline       VistaVector<Type,dim>    operator+    ( const VistaVector<Type,dim> & other ) const;
	inline       VistaVector<Type,dim>    operator-    ( const VistaVector<Type,dim> & other ) const;
	inline       VistaVector<Type,dim>    operator+    ( const Type                      & scale ) const;
	inline       VistaVector<Type,dim>    operator*    ( const Type                        scale ) const;
	inline       VistaVector<Type,dim>    operator/    ( const Type                        scale ) const;

	inline		 VistaVector<Type,dim>    operator-    ( ) const;

	inline const VistaVector<Type,dim> &  operator+=   ( const VistaVector<Type,dim> & other );
	inline const VistaVector<Type,dim> &  operator-=   ( const VistaVector<Type,dim> & other );
	inline const VistaVector<Type,dim> &  operator*=   ( const Type                        scale );
	inline const VistaVector<Type,dim> &  operator/=   ( const Type                        scale );

	inline       bool             operator==   ( const VistaVector<Type,dim> & other ) const;
	inline       bool             operator!=   ( const VistaVector<Type,dim> & other ) const;

	// dot product
	inline       Type             operator*    ( const VistaVector<Type,dim> & other ) const;
	inline       void             operator=    ( const Type * );


	/*========================================================================*/
	/* DATA ACCESS                                                            */
	/*========================================================================*/

public:

	inline const Type &  operator[]  ( const int axis ) const;
	inline       Type &  operator[]  ( const int axis );

	inline void  GetValues  (       Type out[dim] ) const;
	inline void  SetValues  ( const Type in [dim] );

	inline	VistaVector<Type,dim>   Absolute ( ) const;
	inline	VistaVector<Type,dim>   Interpolate (const VistaVector<Type,dim>& in, const Type& t ) const;

	inline       VistaVector<Type,dim>    GetNormalized ()                        const;
	inline       Type             GetLength     ()                                const;
	inline       Type             GetLengthSquared     ()                         const;

	/// normalize this vector
	inline       void              Normalize     ();

	/// dot product or scalar product
	inline       Type             Dot           ( const VistaVector<Type,dim> & other  )  const;
/*
	/// cross product or vector product
	inline       VistaVector<Type,dim>    Cross         ( const VistaVector<Type,dim> & other  )  const;
*/

	inline bool Debug();

protected:
	bool        m_bTransposed;
	int         m_nDimension;
	//Type *      m_pVec;
	Type        m_pVec[dim];
};

template <class Type, int dim>
std::ostream& operator<< ( std::ostream& oStream, VistaVector<Type, dim> oVector );

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim>::VistaVector(bool bSetNull)
:m_bTransposed(false),m_nDimension(dim)
{
	if(bSetNull)
		SetNull();
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim>::VistaVector(const Type * pVals)
  :m_bTransposed(false),m_nDimension(dim)
{
	//SetNull(); is overwriten form next line //av006ss
	for (int idx=0; idx < dim; ++idx)
		m_pVec[idx] = pVals[idx];
}

/*============================================================================*/
/*============================================================================*/
#if 0
template <class Type, int dim>
inline
VistaVector<Type,dim>::VistaVector (const Type first ...)
{
	//SetNull();  // see above ; gets overwritten
	va_list ap;
	va_start (ap, first);
	m_pVec[0] = first;
	for (int idx=1; idx < dim; ++idx)
	{
		m_pVec[idx] = va_arg (ap, Type);
	}
	va_end(ap);
}
#endif
/*============================================================================*/
/*============================================================================*/
#if 1
template <class Type, int dim>
inline
VistaVector<Type,dim>::VistaVector(const VistaVector<Type,dim> & orgVec)
:m_nDimension(dim)
{
	//SetNull(); // gets overwriten => obsolete?
	m_bTransposed = orgVec.IsTransposed();

	for (int idx = 0; idx < dim; ++idx)
		m_pVec[idx] = orgVec.GetVal (idx);
}
#endif

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim>::~VistaVector()
{
	//ReleaseCoeff();
}

/*============================================================================*/
/*============================================================================*/
//template <class Type, int dim>
//inline
//void VistaVector<Type,dim>::AllocCoeff()
//{
//    m_pVec = new Type[dim];
//    SetNull();
//}

/*============================================================================*/
/*============================================================================*/
//template <class Type, int dim>
//inline
//void VistaVector<Type,dim>::ReleaseCoeff()
//{
//    delete [] m_pVec;
//}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaVector<Type,dim>::SetNull()
{
	for (int idx = 0; idx < dim; ++idx)
	{
		m_pVec[idx] = 0;
	}
	return true;
}

/*============================================================================*/
/*============================================================================*/
// Determine if this is a null vector
template <class Type, int dim>
inline
bool VistaVector<Type,dim>::IsNull () const
{
	for (int idx = 0; idx < dim; ++idx)
		if (m_pVec[idx] != 0)
				return false;
	return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaVector<Type,dim>::SetVal (int idx, const Type & newVal)
{
	m_pVec[idx] = newVal;
	return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaVector<Type,dim>::Transpose()
{
	m_bTransposed = !m_bTransposed;
	return true;
}

/*============================================================================*/
/*============================================================================*/
// assignment operator
#if 1
template <class Type, int dim>
inline
VistaVector<Type,dim> &
VistaVector<Type,dim>::operator= (const VistaVector<Type,dim>& vec2)
{
	for (int idx = 0; idx < dim; ++idx)
		m_pVec[idx] = vec2.GetVal (idx);
	m_bTransposed = vec2.IsTransposed();
	return *this;
}
#endif

/*============================================================================*/
/*============================================================================*/
// binary scalar multiplication operator
template <class Type, int dim>
inline
VistaVector<Type,dim>
VistaVector<Type,dim>::operator+ (const Type& num) const
{
	VistaVector<Type,dim> tempVec;

	for (int idx = 0; idx < dim; ++idx)
		tempVec[idx] = num + m_pVec[idx];

	return tempVec;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim> VistaVector<Type,dim>::operator- ( ) const
{
	VistaVector<Type,dim> res;
	for (int idx = 0; idx < dim; ++idx)
		res[idx] = -m_pVec[idx];
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim> VistaVector<Type,dim>::operator*  ( const Type scaleVal ) const
{
	VistaVector<Type,dim> res;
	for (int idx = 0; idx < dim; ++idx)
		res[idx] = m_pVec[idx] * scaleVal;
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim> VistaVector<Type,dim>::operator/  ( const Type scaleVal ) const
{
	const Type scaleValInv = 1.0f/scaleVal;
	VistaVector<Type,dim> res;
	for (int idx = 0; idx < dim; ++idx)
		res[idx] = m_pVec[idx] * scaleValInv;
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim> VistaVector<Type,dim>::operator+ ( const VistaVector<Type,dim> & other ) const
{
	VistaVector<Type,dim> res;
	for (int idx = 0; idx < dim; ++idx)
		res[idx] = m_pVec[idx] + other[idx];
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim> VistaVector<Type,dim>::operator- ( const VistaVector<Type,dim> & other ) const
{
	VistaVector<Type,dim> res;
	for (int idx = 0; idx < dim; ++idx)
		res[idx] = m_pVec[idx] - other[idx];
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
Type VistaVector<Type,dim>::operator* ( const VistaVector<Type,dim> & other ) const
{
	Type res = 0;
	for (int idx = 0; idx < dim; ++idx)
		res += m_pVec[idx]*other[idx];
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
const VistaVector<Type,dim> & VistaVector<Type,dim>::operator*= ( const Type scaleVal )
{
	for (int idx = 0; idx < dim; ++idx)
		m_pVec[idx] *= scaleVal;
	return *this;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
const VistaVector<Type,dim> & VistaVector<Type,dim>::operator/= ( const Type scaleVal )
{
	const Type scaleValInv = 1.0f/scaleVal;
	for (int idx = 0; idx < dim; ++idx)
		m_pVec[idx] *= scaleValInv;
	return *this;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
const VistaVector<Type,dim> & VistaVector<Type,dim>::operator+= ( const VistaVector<Type,dim> & other )
{
	for (int idx = 0; idx < dim; ++idx)
		m_pVec[idx] += other[idx];
	return *this;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
const VistaVector<Type,dim> & VistaVector<Type,dim>::operator-= ( const VistaVector<Type,dim> & other )
{
	for (int idx = 0; idx < dim; ++idx)
		m_pVec[idx] -= other[idx];

	return *this;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaVector<Type,dim>::operator== ( const VistaVector<Type,dim> & other ) const
{
	for (int idx = 0; idx < dim; ++idx)
		if (std::abs( m_pVec[idx] - other[idx] ) > Vista::Epsilon)
			return false;
	return true;
}

template <class Type, int dim>
inline
bool VistaVector<Type,dim>::operator!= ( const VistaVector<Type,dim> & other ) const
{
	return !((*this)==other);
}

template <class Type, int dim>
inline
void VistaVector<Type,dim>::operator=    ( const Type *pOther )
{
	for(int n=0; n < dim; ++n)
		m_pVec[n] = pOther[n];
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim>   VistaVector<Type,dim>::Absolute ( ) const
{
	VistaVector<Type,dim> res;
	for (int idx = 0; idx < dim; ++idx)
		res[idx] = (m_pVec[idx]>=0) ? m_pVec[idx] : -m_pVec[idx];
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim>    VistaVector<Type,dim>::GetNormalized () const
{
	return *this / GetLength ();
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
Type  VistaVector<Type,dim>::GetLength () const
{
	return static_cast<Type> ( sqrt ( GetLengthSquared()) );
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
Type  VistaVector<Type,dim>::GetLengthSquared () const
{
	Type res = 0;
	for (int idx = 0; idx < dim; ++idx)
		res += m_pVec[idx] * m_pVec[idx];

	return res;
}


/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
/// normalize this vector
inline
void VistaVector<Type,dim>::Normalize ()
{
	(*this) /= this->GetLength();
}


/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
/// dot product or scalar product
inline
Type VistaVector<Type,dim>::Dot ( const VistaVector<Type,dim> & other  )  const
{
	// this is just a convenient form of "operator* "
	return (*this) * other;
}


/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaVector<Type,dim>   VistaVector<Type,dim>::Interpolate (
					const VistaVector<Type,dim>& in, const Type& t ) const
{
	if(t<=0.0)
		return *this;

	if(t>=1.0)
		return in;

	VistaVector<Type,dim> res;
	for (int idx = 0; idx < dim; ++idx)
		res[idx] = (1.0-t) * m_pVec[idx] + t * in[idx];
	return res;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
void VistaVector<Type,dim>::GetValues ( Type out[dim] ) const
{
	for (int idx = 0; idx < dim; ++idx)
		out[idx] = m_pVec[idx];
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
void VistaVector<Type,dim>::SetValues ( const Type in[dim] )
{
	for (int idx = 0; idx < dim; ++idx)
		m_pVec[idx] = in[idx];
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
const Type & VistaVector<Type,dim>::operator[] ( const int axis ) const
{
	return m_pVec[axis];
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
Type & VistaVector<Type,dim>::operator[] ( const int axis )
{
	return m_pVec[axis];
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaVector<Type,dim>::Debug ()
{
	for (int idx = 0; idx < dim; ++idx)
	{
		vstr::out() << m_pVec[idx];
		if (m_bTransposed)
			vstr::out() << "  ";
		else
			vstr::out() << std::endl;
	}
	if (m_bTransposed)
		vstr::out() << std::endl;

	return true;
}


/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
#if 0
bool VecExample ()
{
	double init[3] = {1.0,2.0,3.0};
	// NO range check
	VistaVector<double,3>   myVec1 (init);
	VistaVector<double,3>   myVec2 (myVec1);
	VistaVector<double,3>   myVec3 = myVec2;
	//NO type convertion!!! NO range check!!!
	VistaVector<double,3>   myVec4 (0.0,1.0,5.0);
	myVec2.Transpose();
	myVec2[1] = -2;
	myVec4 = myVec1 * ((myVec2 * myVec1)/3.5);
	myVec3 -= myVec4;
	myVec1.Debug();
	myVec2.Debug();
	myVec3.Debug();
	myVec4.Debug();
	return true;
}
#endif


template <class Type, int dim>
inline std::ostream& operator<< ( std::ostream& oStream, VistaVector<Type, dim> oVector )
{
	const std::streamsize iOldPrecision( oStream.precision( 3 ) );
	const std::ios::fmtflags iOldflags( oStream.flags() );

	// set fix point notation
	oStream.setf( std::ios::fixed | std::ios::showpos );

	// write to the stream
	oStream << "( " << oVector[0];
	for( int i = 1; i < dim; ++i )
		oStream << ", " << oVector[i];
	oStream << " )";
	oStream.unsetf( std::ios::fixed | std::ios::showpos );	

	// restore old stream settings
	oStream.precision( iOldPrecision );
	oStream.flags( iOldflags );

	return oStream;
}


#endif //_VISTAVECTOR_H
