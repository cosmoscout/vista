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


#ifndef _VISTAMATRIX_H
#define _VISTAMATRIX_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <iostream>
#include <cmath>     // sin, cos, sqrt ...
#include <complex>
#include <algorithm>

//#include "VistaPolynom.h"
#include "VistaMathTools.h"
#include "VistaMathConfig.h"

#include <VistaBase/VistaMathBasics.h>
#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                        */
/*============================================================================*/

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
/* VistaMatrix                                                               */
/*============================================================================*/
/**
 * VistaMatrix
 * Matrix[row][col]
 */
template <class Type>
class VistaMatrix
{
public:
	/// constructs nXm matrix, ordering: row X column, defaults are zero
	VistaMatrix(int rowDim, int colDim);
	VistaMatrix(const VistaMatrix<Type> & orgMat);
	~VistaMatrix();

protected:
	bool AllocCoeff();
	bool ReleaseCoeff();

public:
	bool SetNull();
	inline Type GetVal (int row, int col) const
		{return m_pMat[row][col];}
	inline bool SetVal (int row, int col, const Type & newVal)
		{m_pMat[row][col] = newVal; return true;}
	inline int GetRowDimension () const
		{return m_nRowDimension;}
	inline int GetColDimension () const
		{return m_nColDimension;}
	inline bool IsSquare() const
	   {return (m_nRowDimension == m_nColDimension);}
	inline bool IsNull() const;
	inline bool Transpose();

	// assignment operator
	VistaMatrix<Type>& operator = (const VistaMatrix<Type>& rhs);

	inline bool operator== ( const VistaMatrix<Type> & rhs ) const;
	inline bool operator!= ( const VistaMatrix<Type> & rhs ) const;

	// calculation operators
	VistaMatrix<Type> operator+ (const Type& rhs) const;
	VistaMatrix<Type> operator* (const Type& rhs) const;

	VistaMatrix<Type> operator* (const VistaMatrix<Type>& rhs) const;
	VistaMatrix<Type> operator+ (const VistaMatrix<Type>& rhs) const;
	VistaMatrix<Type> operator- (const VistaMatrix<Type>& rhs) const;

	inline Type* operator[]( const int i );
	inline const Type* operator[]( const int i ) const;

	VistaMatrix<Type> GivensRotation (int valP, int valQ);
	bool GivensElimination (Type * vecX, Type * vecB);

	/**
	 * singular value decomposition (SVD) of a matrix
	 *
	 * the decomposition consists of matrices U, V and the vector W such that
	 * - U * diag(W) * V^T equals to the decomposed matrix
	 * - W = (w1,..,wn) contains the singular values, wi >= 0.0
	 * - V contains orthonormal columns.
	 *     columns corresponding to zero wi's build a base for the nullspace
	 * - U contains orthonormal columns for all wi > 0, which span the range of A
	 *
	 * SVD can be used to determine the rank / range and singularity / nullspace
	 * of a given matrix, and in case of a regular (non-singular) the inverse of
	 * a matrix
	 *
	 * @param pDefensive: true  iff consistency checking shall be done;
	 *                      this can cost a lot of computation time
	 *                false for decomposition only, results can be
	 *                      checked with SingularValueDecompositionTest()
	 * @param the user must define the subsequent matrices with sufficient dimensionality
	 *    matU: m-n-matrix (with the same dimensionality as the member matrix)
	 *    matW: n-1-matrix (n-vector)
	 *    matV: n-n-matrix
	 * @return
	 *    true, iff SVD successfull, false otherwise
	 */
	bool SingularValueDecomposition( VistaMatrix<Type> & matU,
									 VistaMatrix<Type> & matW,
									 VistaMatrix<Type> & matV,
									 bool bDefensive = false );

	/**
	 * test whether the results of a SVD holds for
	 *   1. U * W * V^T == A
	 *   2. V^T * V == E
	 *   3. U^T * U == E for columns corresponding to nonzero singular values
	 *
	 * @param matU, matW, matV from SVD-Method
	 *        epsilon specifies the 0-range (0-epsilon;0+epsilon)
	 * @return
	 *    true, iff SVD matrices pass the tests, false otherwise
	 */
	bool SingularValueDecompositionTest( const VistaMatrix<Type> & matU,
										 const VistaMatrix<Type> & matW,
										 const VistaMatrix<Type> & matV,
										 Type epsilon );

	Type GetAbsMaxComponent();
	bool Debug ();
	void PrintMatrix( std::string strTitle, std::ostream & out );

protected:
	int        m_nRowDimension;
	int        m_nColDimension;
	Type **    m_pMat;
	Type *     m_pr[4];
	Type       m_pc0[4];
	Type       m_pc1[4];
	Type       m_pc2[4];
	Type       m_pc3[4];
};

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
VistaMatrix<Type>::VistaMatrix(int rowDim, int colDim)
:m_nRowDimension(rowDim), m_nColDimension(colDim)
{
	AllocCoeff();
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
VistaMatrix<Type>::~VistaMatrix()
{
	ReleaseCoeff();
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
VistaMatrix<Type>::VistaMatrix(const VistaMatrix<Type> & orgMat)
{
	m_nRowDimension = orgMat.GetRowDimension();
	m_nColDimension = orgMat.GetColDimension();

	AllocCoeff();

	for (int row = 0; row < m_nRowDimension; ++row)
		for (int col = 0; col < m_nColDimension; ++col)
			m_pMat[row][col] = orgMat.GetVal (row,col);
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
bool VistaMatrix<Type>::AllocCoeff()
{
	int row = 0;

	if ( (m_nRowDimension <= 4) && (m_nColDimension <= 4) )
	{
		m_pMat = m_pr;
		m_pMat[0] = m_pc0;
		m_pMat[1] = m_pc1;
		m_pMat[2] = m_pc2;
		m_pMat[3] = m_pc3;
	}
	else
	{
		// create all rows
		m_pMat = new Type* [m_nRowDimension];
		// and all columns
/*        for (row = 0; row < m_nRowDimension; ++row)
		{
			m_pMat[row] = new Type [m_nColDimension];
		}
*/
		// this is condiderably faster:
		Type * array = new Type [m_nColDimension * m_nRowDimension];
		for (row = 0; row < m_nRowDimension; ++row)
		{
			m_pMat[row] = &array[row*m_nColDimension];
		}
	}
	SetNull();
	return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
bool VistaMatrix<Type>::ReleaseCoeff()
{
	if ( (m_nRowDimension <= 4) && (m_nColDimension <= 4) )
	{
	}
	else
	{
//        for (int row=0; row < m_nRowDimension; ++row)
//        {
//            delete [] m_pMat[row];
//        }
		delete [] m_pMat[0];
		delete [] m_pMat;
	}
	return true;
}


/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
bool VistaMatrix<Type>::SetNull()
{
	for (int row = 0; row < m_nRowDimension; ++row)
	{
		for (int col = 0; col < m_nColDimension; ++col)
		{
			m_pMat[row][col] = 0;
		}
	}
	return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
bool VistaMatrix<Type>::Transpose()
{
	VistaMatrix<Type>  tempMatrix(*this);

	// create new mem map
	ReleaseCoeff();
	m_nRowDimension = m_nColDimension;
	m_nColDimension = tempMatrix.GetRowDimension();
	AllocCoeff();

	for (int row = 0; row < m_nRowDimension; ++row)
		for (int col = 0; col < m_nColDimension; ++col)
			m_pMat[row][col] = tempMatrix.GetVal(col, row);

	return true;
}


/*============================================================================*/
/*============================================================================*/
// Determine if this is a null matrix
template <class Type>
inline
bool VistaMatrix<Type>::IsNull () const
{
	for (int row = 0; row < m_nRowDimension; ++row)
		for (int col = 0; col < m_nColDimension; ++col)
			if (m_pMat[row][col] != 0)
				return false;
	return true;
}

/*============================================================================*/
/*============================================================================*/
// equals operator
template <class Type>
inline
bool VistaMatrix<Type>::operator== ( const VistaMatrix<Type> & rhs ) const
{
	if (m_nRowDimension != rhs.GetRowDimension() || m_nColDimension != rhs.GetColDimension())
        return false;

	for (int row = 0; row < m_nRowDimension; ++row)
		for (int col = 0; col < m_nColDimension; ++col)
			if ( std::abs( m_pMat[row][col] - rhs.GetVal(row,col))  > Vista::Epsilon )
                return false;

    return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
bool VistaMatrix<Type>::operator!= ( const VistaMatrix<Type> & rhs ) const
{
	return !((*this)==rhs);
}

/*============================================================================*/
/*============================================================================*/
// assignment operator
template <class Type>
inline
VistaMatrix<Type> &
VistaMatrix<Type>::operator= (const VistaMatrix<Type>& rhs)
{
	if (m_nRowDimension != rhs.GetRowDimension() || m_nColDimension != rhs.GetColDimension())
	{
		// create new mem map
		ReleaseCoeff();
		m_nRowDimension = rhs.GetRowDimension();
		m_nColDimension = rhs.GetColDimension();
		AllocCoeff();
	}

	for (int row = 0; row < m_nRowDimension; ++row)
		for (int col = 0; col < m_nColDimension; ++col)
			m_pMat[row][col] = rhs.GetVal (row,col);

	return *this;
}

/*============================================================================*/
/*============================================================================*/
// binary scalar multiplication operator
template <class Type>
inline
VistaMatrix<Type>
VistaMatrix<Type>::operator* (const Type& rhs) const
{
	VistaMatrix<Type> tempMat(m_nRowDimension,m_nColDimension);

	for (int row = 0; row < m_nRowDimension; ++row)
		for (int col = 0; col < m_nColDimension; ++col)
			tempMat.SetVal(row, col, rhs * m_pMat[row][col]);

	return tempMat;
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
VistaMatrix<Type>
VistaMatrix<Type>::operator+ (const Type& rhs) const
{
	VistaMatrix<Type> tempMat(m_nRowDimension,m_nColDimension);

	for (int row = 0; row < m_nRowDimension; ++row)
		for (int col = 0; col < m_nColDimension; ++col)
			tempMat.SetVal(row, col, rhs + m_pMat[row][col]);

	return tempMat;
}


/*============================================================================*/
/*============================================================================*/
// binary matrix multiplication operator
template <class Type>
inline
VistaMatrix<Type>
VistaMatrix<Type>::operator* (const VistaMatrix<Type>& rhs) const
{
	//VistaMatrix<Type,rowDim,colDim> temp(m1.Row,m2.Col);
	VistaMatrix<Type> tempMat(m_nRowDimension, rhs.GetColDimension());

	if (m_nColDimension != rhs.GetRowDimension())
		// Inconsistent matrix size in multiplication!
		return tempMat;

	for (int row=0; row < m_nRowDimension; ++row)
	{
		for (int col=0; col < tempMat.GetColDimension(); ++col)
		{
			tempMat.SetVal(row,col,0);
			for (int k=0; k < m_nColDimension; ++k)
			{
				tempMat.SetVal(row,col, tempMat.GetVal(row,col) + m_pMat[row][k] * rhs.GetVal(k,col));
			}
		}
	}
	return tempMat;
}


/*============================================================================*/
/*============================================================================*/
// binary matrix addition operator
template <class Type>
inline
VistaMatrix<Type>
VistaMatrix<Type>::operator+ (const VistaMatrix<Type>& rhs) const
{
	if( ( rhs.GetRowDimension() != m_nRowDimension ) ||
		( rhs.GetColDimension() != m_nColDimension ) )
	{
		// inconsistent matrix size
		VistaMatrix<Type> tempMat(rhs);
		return tempMat;
	}

	VistaMatrix<Type> tempMat(m_nRowDimension, m_nColDimension);
	for (int row=0; row < m_nRowDimension; ++row)
	{
		for (int col=0; col < tempMat.GetColDimension(); ++col)
		{
			tempMat.SetVal(row,col, rhs.GetVal(row,col) + m_pMat[row][col] );
		}
	}
	return tempMat;
}


/*============================================================================*/
/*============================================================================*/
// binary matrix substraction operator
template <class Type>
inline
VistaMatrix<Type>
VistaMatrix<Type>::operator- (const VistaMatrix<Type>& rhs) const
{
	if( ( rhs.GetRowDimension() != m_nRowDimension ) ||
		( rhs.GetColDimension() != m_nColDimension ) )
	{
		// inconsistent matrix size
		VistaMatrix<Type> tempMat(0,0);
		return tempMat;
	}

	VistaMatrix<Type> tempMat(m_nRowDimension, m_nColDimension);
	for (int row=0; row < m_nRowDimension; ++row)
	{
		for (int col=0; col < tempMat.GetColDimension(); ++col)
		{
			tempMat.SetVal(row,col, m_pMat[row][col] - rhs.GetVal(row,col) );
		}
	}
	return tempMat;
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
const Type * VistaMatrix<Type>::operator[] ( const int row ) const
{
	return m_pMat[row];
}

/*============================================================================*/
/*============================================================================*/
template <class Type>
inline
Type * VistaMatrix<Type>::operator[] ( const int row )
{
	return m_pMat[row];
}


/*============================================================================*/
/*============================================================================*/
// QR-reduction using Givens elimination (Weller, p.83)
template <class Type>
inline
VistaMatrix<Type>
VistaMatrix<Type>::GivensRotation (int valP, int valQ)
{
	// Givens rotation around (p,q):
	//    newMat[p][j] = Mat[p][j]*cosPhi - Mat[q][j]*sinPhi
	//    newMat[q][j] = Mat[p][j]*sinPhi + Mat[q][j]*cosPhi
	// The goal is to eliminate Mat[p][q] to zero, i.e.:
	//    Mat[p][p]*sinPhi + Mat[q][p]*cosPhi = 0
	//    ==> (e.g.)
	//    sinPhi = - mat[q][p] / w
	//    cosPhi =   mat[p][p] / w
	//    w = sqrt (mat[p][p]^2 + mat[q][p]^2)
	// The similarity transformation matrix i a unity matrix with:
	//    matU[p][p] =  cosPhi
	//    matU[p][q] =  sinPhi
	//    matU[q][p] = -sinPhi
	//    matU[q][q] =  cosPhi
	VistaMatrix<Type> matU (m_nColDimension, m_nColDimension);
	for (int diag = 0; diag < m_nRowDimension; ++diag)
		matU.SetVal(diag,diag,1);

	if (m_pMat[valQ][valP] != 0)
	{
		Type valHelp = 0;
		Type valW = sqrt(m_pMat[valP][valP] * m_pMat[valP][valP] +
						 m_pMat[valQ][valP] * m_pMat[valQ][valP]);
		Type valCos =   m_pMat[valP][valP] / valW;
		Type valSin = - m_pMat[valQ][valP] / valW;
		// the first coefficient
		m_pMat[valP][valP] = m_pMat[valP][valP] * valCos - m_pMat[valQ][valP] * valSin;
		// all other coefficients
		for (int col = valP + 1; col < m_nColDimension; ++col)
		{
			valHelp           = m_pMat[valP][col] * valCos - m_pMat[valQ][col] * valSin;
			m_pMat[valQ][col] = m_pMat[valP][col] * valSin + m_pMat[valQ][col] * valCos;
			m_pMat[valP][col] = valHelp;
		}
		m_pMat[valQ][valP] = 0;
		// create similarity matrix
		matU.SetVal (valP, valP,  valCos);
		matU.SetVal (valP, valQ,  valSin);
		matU.SetVal (valQ, valP, -valSin);
		matU.SetVal (valQ, valQ,  valCos);
	}
	return matU;
}

/*============================================================================*/
/*============================================================================*/
// QR-reduction using Givens elimination (Weller, p.83)
// R = Q^T * A; R: upper right matrix; Q: orthogonal matrix
// A * x = b
template <class Type>
inline
bool VistaMatrix<Type>::GivensElimination (Type * vecX, Type * vecB)
{
	// =============
	// row >= col, rank(Mat)=row, length(vecX) = row, length(vecB) = row
	// =============
	int valP;
	int valQ;
	int col;
	Type valHelp = 0;
	for (valP = 0; valP < m_nRowDimension; ++valP)
	{
		for (valQ = valP + 1; valQ < m_nRowDimension; ++valQ)
		{
			// START OF GIVENS ROTATION ==>>
			// Givens rotation around (p,q):
			//    newMat[p][j] = Mat[p][j]*cosPhi - Mat[q][j]*sinPhi
			//    newMat[q][j] = Mat[p][j]*sinPhi + Mat[q][j]*cosPhi
			// The goal is to eliminate Mat[p][q] to zero, i.e.:
			//    Mat[p][p]*sinPhi + Mat[q][p]*cosPhi = 0
			//    ==> (e.g.)
			//    sinPhi = - mat[q][p] / w
			//    cosPhi =   mat[p][p] / w
			//    w = sqrt (mat[p][p]^2 + mat[q][p]^2)
			if (m_pMat[valQ][valP] != 0)
			{
				Type valW = sqrt(m_pMat[valP][valP] * m_pMat[valP][valP] +
								 m_pMat[valQ][valP] * m_pMat[valQ][valP]);
				Type valCos =   m_pMat[valP][valP] / valW;
				Type valSin = - m_pMat[valQ][valP] / valW;
				// the first coefficient
				m_pMat[valP][valP] = m_pMat[valP][valP] * valCos - m_pMat[valQ][valP] * valSin;
				for (col = valP + 1; col < m_nColDimension; ++col)
				{
					valHelp           = m_pMat[valP][col] * valCos - m_pMat[valQ][col] * valSin;
					m_pMat[valQ][col] = m_pMat[valP][col] * valSin + m_pMat[valQ][col] * valCos;
					m_pMat[valP][col] = valHelp;
				}
				m_pMat[valQ][valP] = 0;
				// <<== end OF GIVENS ROTATION.

				// .. but the rotation concerns the solution: Mat * X = B
				valHelp    = vecB[valP] * valCos - vecB[valQ] * valSin;
				vecB[valQ] = vecB[valP] * valSin + vecB[valQ] * valCos;
				vecB[valP] = valHelp;
			}
		}
		if (m_pMat[valP][valP] == 0)
			return false; // singularity
	}
	// reverse insertion
	for  (valP = m_nRowDimension - 1; valP >= 0; --valP)
	{
		Type valSum = vecB[valP];
		for (valQ = valP + 1; valQ < m_nRowDimension; ++valQ)
		{
			valSum = valSum - m_pMat[valP][valQ] * vecX[valQ];
		}
		vecX[valP] = valSum / m_pMat[valP][valP];
	}
	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetAbsMaxComponent                                          */
/*                                                                            */
/*  This function is also known as "L-Infinity-Norm"                          */
/*                                                                            */
/*============================================================================*/
template <class Type>
inline
Type VistaMatrix<Type>::GetAbsMaxComponent ()
{
	Type infNorm  = 0.0;
	Type curAbs   = 0.0;
	for (int row=0; row < m_nRowDimension; ++row)
	{
		for (int col=0; col < m_nColDimension; ++col)
		{
			curAbs = std::abs(m_pMat[row][col]);
			if (curAbs > infNorm)
				infNorm = curAbs;
		}
	}
	return infNorm;
}

/*============================================================================*/
/*============================================================================*/

template <class Type>
inline
bool VistaMatrix<Type>::Debug ()
{
	for (int row = 0; row < m_nRowDimension; ++row)
	{
		for (int col = 0; col < m_nColDimension; ++col)
		{
			vstr::out()
				// << std::cout.precision(8) << std::cout.width(9)
				// set fix point notation
				//std::cout << std::cout.setf ( ios::fixed | ios::showpos )
					  << m_pMat[row][col] << "  ";
		}
		vstr::out() << std::endl;
	}
	return true;
}

/*============================================================================*/
/*============================================================================*/

template <class Type>
inline
bool VistaMatrix<Type>::SingularValueDecomposition( VistaMatrix<Type> & matU, VistaMatrix<Type> & matW, VistaMatrix<Type> & matV, bool bDefensive )
// diese Methode zerlegt die m-n-Matrix A in die Matrizen U, V, W,
// so dass gilt A = UWV^T, U ist eine m-n-Matrix mit orthogonalen Spalten
// bzw. Zeilen, V ist quadratisch und orthonormal. W ist eine Diagonalmatrix
// und enth�lt die Singulaerwerte.
{

	bool flag;
	int i, its, j, jj, k, l, nm;
	double anorm, c, f, g, h, s, scale, x, y, z;
	double volatile temp; // this must be done in order to prevent some 80-bit optimization!

	int m = m_nRowDimension;
	int n = m_nColDimension;

	if( ( matU.GetRowDimension() != m ) ||
		( matU.GetColDimension() != n ) ||
		( matW.GetRowDimension() != n ) ||
		( matW.GetColDimension() != 1 ) ||
		( matV.GetRowDimension() != n ) ||
		( matV.GetColDimension() != n ) )
	{
		vstr::warnp() << "[VistaMatrix] SingularValueDecomposition: wrong W and V dimensionality" << std::endl;
		return false;
	}

	matU = *this; // copy matrix to U

	VistaMatrix<double> rv1( n, 1 );
	g = 0.0;
	scale = 0.0;
	anorm = 0.0;

	for( i = 0; i < n; i++ )			// householder reduction to bidiagonal form
	{
		l = i + 2;
		rv1[i][0] = scale * g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if( i < m )
		{
			for( k = i; k < m; k++ )
				scale += std::abs( matU.GetVal(k,i) );
			if( scale != 0.0 )
			{
				for( k = i; k < m; k++ )
				{
					matU[k][i] /= scale;
					s += matU[k][i] * matU[k][i];
				}
				f = matU[i][i];
				g = -VistaMathTools<double>::SIGN( sqrt(s), f );
				h = f * g - s;
				matU[i][i] =  f - g;
				for( j = l - 1; j < n; j++ )
				{
					for( s = 0.0, k = i; k < m; k++ )
						s+= matU[k][i] * matU[k][j];
					f = s / h;
					for( k = i; k < m; k++ )
						matU[k][j] += f * matU[k][i];
				}
				for( k = i; k < m; k++ )
					matU[k][i] *= scale;
			}
		}
		matW[i][0] = scale * g;

		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if( ( i+1 <= m ) && ( i+1 != n ) )		// if( ( i + 1 <= m ) && ( i != n )
		{
			for( k = l - 1; k < n; k++ )
				scale += std::abs( matU[i][k] );
			if( scale != 0.0 )
			{
				for( k = l - 1; k < n; k++ )
				{
					matU[i][k] /= scale;
					s += matU[i][k] * matU[i][k];
				}
				f = matU[i][l-1];
				g = - VistaMathTools<double>::SIGN( sqrt(s), f );
				h = f * g - s;
				matU[i][l-1] = f - g;
				for( k = l - 1; k < n; k++ )
					rv1[k][0] = matU[i][k] / h;
				for( j = l - 1; j < m; j++ )
				{
					for( s = 0.0, k = l - 1; k < n; k++ )
						s += matU[j][k] * matU[i][k];
					for( k = l - 1; k < n; k++ )
						matU[j][k] += s * rv1[k][0];
				}
				for( k = l - 1; k < n; k++ )
					matU[i][k] *= scale;
			}
		}
		//anorm = VistaMathTools<double>::MAX( anorm, ( std::abs( matW.GetVal(i,0) )  + std::abs( rv1.GetVal(i,0) ) ) );
		anorm = std::max<double>( anorm, ( std::abs( matW.GetVal(i,0) )  + std::abs( rv1.GetVal(i,0) ) ) );
	}
	for( i = n - 1; i >= 0; i-- )
	{
		// accumulation of right-hand transformations
		if( i < n - 1 )
		{
			if( g != 0.0 )
			{
				for( j = l; j < n; j++ )	// double division to avoid possible underflow
					matV[j][i] = ( matU[i][j] / matU[i][l] ) / g;
				for( j = l; j < n; j++ )
				{
					for( s = 0.0, k = l; k < n; k++ )
						s += matU[i][k] * matV[k][j];
					for( k = l; k < n; k++ )
						matV[k][j] += s * matV[k][i];
				}
			}
			for( j = l; j < n; j++ )
			{
				matV[i][j] = 0.0;
				matV[j][i] = 0.0;
			}
		}
		matV[i][i] = 1.0;
		g = rv1[i][0];
		l = i;
	}
	//for( i = VistaMathTools<double>::MIN( m, n ) - 1; i >= 0; i-- )
	for( i = std::min( m, n ) - 1; i >= 0; i-- )
	{
		// accumulation of left-hand transformations
		l = i + 1;
		g = matW[i][0];
		for( j = l; j < n; j++ )
			matU[i][j] = 0.0;
		if( g != 0.0 )
		{
			g = 1.0 / g;
			for( j = l; j < n; j++ )
			{
				for( s = 0.0, k = l; k < m; k++ )
					s += matU[k][i] * matU[k][j];
				f = (s / matU[i][i] ) * g;
				for( k = i; k < m; k++ )
					matU[k][j] += f * matU.GetVal(k,i);
			}
			for( j = i; j < m; j++ )
				matU[j][i] *= g ;
		} else {
			for( j = i; j < m; j++ )
				matU[j][i] = 0.0;
		}
		matU[i][i] += 1;
	}

	for( k = n - 1; k >= 0; k-- )
	{
		// diagonalization of the bidiagonal form:
		// loop over singular values, and over allowed iterations
		for( its = 0; its < 30; its++ )
		{
			flag = true;
			for( l = k; l >= 0; l-- )
			{
				// test for splitting
				// note that rv1[0] is always zero
				nm = l - 1;

				temp = std::abs( rv1[l][0] ) + anorm;
				if( temp == anorm )
				{
					flag = false;
					break;
				}

				temp = std::abs( matW[nm][0] ) + anorm;
				if( temp == anorm )
					break;
			}
			if( flag )
			{
				c = 0.0;	// cancellation of rv1[l], if l > 0
				s = 1.0;
				for( i = l; i < k + 1; i++ )		// for( i = l - 1; i < k + 1; i++ ) BUGFIX
				{
					f = s * rv1[i][0];
					rv1[i][0] *= c;

					temp = std::abs( f ) + anorm;
					if( temp == anorm )
						break;
					g = matW[i][0];
					h = VistaMathTools<double>::Pythagoras( f, g );
					matW[i][0] = h;
					h = 1.0 / h;
					c = g * h;
					s = -f * h;
					for( j = 0; j < m; j++ )
					{
						y = matU[j][nm];
						z = matU[j][i];
						matU[j][nm] = y * c + z * s;
						matU[j][i] = z * c - y * s;
					}
				}
			}
			z = matW[k][0];
			if( l == k )	// convergence
			{
				if( z < 0.0 )	// singular value is made nonnegative
				{
					matW[k][0] = -z;
					for( j = 0; j < n; j++ )
						matV[j][k] = -matV[j][k];
				}
				break;
			}
			if( its == 29 )
			{
				vstr::warnp() << "[VistaMatrix] SingularValueDecomposition: no convergence in 30 iterations" << std::endl;
				return false;	/** @todo exit here? */
			}
			x = matW[l][0];
			nm = k - 1;
			y = matW[nm][0];
			g = rv1[nm][0];
			h = rv1[k][0];
			f = ( ( y - z ) * ( y + z ) + ( g - h ) * (g + h ) ) / ( 2.0 * h * y );
			g = VistaMathTools<double>::Pythagoras( f, 1.0 );
			f = ( ( x - z ) * ( x + z ) + h * ( ( y / ( f + VistaMathTools<double>::SIGN( g, f ) ) ) - h ) ) / x;
			c = 1.0;
			s = 1.0;
			for( j = l; j <= nm; j++ )	// next QR transformation
			{
				i = j + 1;
				g = rv1[i][0];
				y = matW[i][0];
				h = s * g;
				g = c * g;
				z = VistaMathTools<double>::Pythagoras( f, h );
				rv1[j][0] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for( jj = 0; jj < n; jj++ )
				{
					x = matV[jj][j];
					z = matV[jj][i];
					matV[jj][j] = x * c + z * s;
					matV[jj][i] = z * c - x * s;
				}
				z = VistaMathTools<double>::Pythagoras( f, h );
				matW[j][0] = z;	// rotation can be arvitrary if z=0
				if( z )
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				for( jj = 0; jj < m; jj++ )
				{
					y = matU[jj][j];
					z = matU[jj][i];
					matU[jj][j] = y * c + z * s;
					matU[jj][i] = z * c - y * s;
				}
			}
			rv1[l][0] = 0.0;
			rv1[k][0] = f;
			matW[k][0] = x;
		}
	}

	// now we have computed the decomposition

	// check the results
	if( bDefensive )
	{
		if( !this->SingularValueDecompositionTest(matU,matW,matV,0.000001) )
			return false;
	}

	return true;
}


/*============================================================================*/
/*============================================================================*/

template <class Type>
inline
bool VistaMatrix<Type>::SingularValueDecompositionTest( const VistaMatrix<Type> & matU, const VistaMatrix<Type> & matW, const VistaMatrix<Type> & matV, Type epsilon )
{
	// check the SVD-results with following tests
	// 1. U * W * V^T == A
	// 2. V^T * V == E
	// 2. U^T * U == E' diagonal elements are zero if the coresponding singular values are zero.

	int m = m_nRowDimension;
	int n = m_nColDimension;

	int iRow, iCol;

	bool bTest1 = false;
	bool bTest2 = false;
	bool bTest3 = false;

	if( ( matU.GetRowDimension() != m ) ||
		( matU.GetColDimension() != n ) ||
		( matW.GetRowDimension() != n ) ||
		( matW.GetColDimension() != 1 ) ||
		( matV.GetRowDimension() != n ) ||
		( matV.GetColDimension() != n ) )
	{
		vstr::warnp() << "[VistaMatrix] SingularValueDecompositionTest: mismatch in A, U, W, V dimensionality" << std::endl;
		return false;
	}

	// create Diag(w1,w2,...,wn) with matW = (w1,w2,...,wn)^T
	VistaMatrix<double> matDiagW( n, n );
	for( iRow = 0; iRow < n; ++iRow )
		matDiagW[iRow][iRow] = matW[iRow][0];

	// create transposed of matV and matU
	// we do need the original matrices later on, so create new ones..
	VistaMatrix<double> matTransposedV( matV );
	matTransposedV.Transpose();
	VistaMatrix<double> matTransposedU( matU );
	matTransposedU.Transpose();

	// test for U * W * V^T == A
	VistaMatrix<double> matTest1 = (matU * matDiagW * matTransposedV) - (*this);
	for( iRow = 0; iRow < matTest1.GetRowDimension(); ++iRow )
		for( iCol = 0; iCol < matTest1.GetColDimension(); ++iCol )
			if( std::abs( matTest1[iRow][iCol] ) < epsilon )
				matTest1[iRow][iCol] = 0.0;
	bTest1 = matTest1.IsNull();

	// create identity matrix En = Diag(1,...,1)
	VistaMatrix<double> matEn( n, n );
	for( iRow = 0; iRow < n; ++iRow )
		matEn[iRow][iRow] = 1.0;

	// test for V^T * V == E
	VistaMatrix<double> matTest2 = ( matTransposedV * matV ) - matEn;
	for( iRow = 0; iRow < matTest2.GetRowDimension(); ++iRow )
		for( iCol = 0; iCol < matTest2.GetColDimension(); ++iCol )
			if( std::abs( matTest2[iRow][iCol] ) < epsilon )
				matTest2[iRow][iCol] = 0.0;
	bTest2 = matTest2.IsNull();

	// test for U^T * U = E'
	VistaMatrix<double> matTest3 = matTransposedU * matU - matEn;
	for( iRow = 0; iRow < matTest3.GetRowDimension(); ++iRow )
		if( std::abs( matW[iRow][0] ) < epsilon )
			matTest3[iRow][iRow] = 0.0; // U needs not to be orthonormal for zero singular values

	for( iRow = 0; iRow < matTest3.GetRowDimension(); ++iRow )
		for( iCol = 0; iCol < matTest3.GetColDimension(); ++iCol )
			if( std::abs( matTest3[iRow][iCol] ) < epsilon )
				matTest3[iRow][iCol] = 0.0;
	bTest3 = matTest3.IsNull();

	if( !bTest1 )
	{
		vstr::warnp() << "[VistaMatrix] SingularValueDecompositionTest: test failed: U * W * V^T != A (epsilon="
			<< epsilon << ")" << std::endl;
	}
	if( !bTest2 )
	{
		vstr::warnp() << "[VistaMatrix] SingularValueDecompositionTest: test failed: V^T * V != En (epsilon="
					<< epsilon << ")" << std::endl;
	}
	if( !bTest3 )
	{
		vstr::warnp() << "[VistaMatrix] SingularValueDecompositionTest: test failed: U^T * U != E' (epsilon="
					<< epsilon << ")" << std::endl;
		//(matTransposedU * matU).PrintMatrix( "U^T * U =", std::cout );
		//matTest3.PrintMatrix( "U^T * U - E' =", std::cout );
		//matEn.PrintMatrix( "E'=", std::cout );
		//matDiagW.PrintMatrix( "W = ", std::cout );
	}

	if( bTest1 && bTest2 && bTest3 )
		return true;

	return false;
}

/*============================================================================*/
/*============================================================================*/

template <class Type>
inline
void VistaMatrix<Type>::PrintMatrix( std::string strTitle, std::ostream & out )
{
	if( !strTitle.empty() )
		out << strTitle << "\n";

	int n = m_nColDimension;
	int m = m_nRowDimension;

	for( int i = 0; i < m; ++i )
	{
		out << "[";

		for( int j = 0; j < n; ++j )
		{
			out << " ";
			out.precision( 8 );
			out.width( 15 );
			out << m_pMat[i][j];
		}

		out << " ]\n";
	}
}

/*============================================================================*/
/*============================================================================*/

#endif //_VISTAMATRIX_H


