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


#ifndef _VISTASQUAREMATRIX_H
#define _VISTASQUAREMATRIX_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaMatrix.h"
#include "VistaPolynomial.h"
#include "VistaMathConfig.h"

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
#include <cmath>     // sin, cos, sqrt ...
#include <complex>

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
/* VistaSquareMatrix                                                         */
/*============================================================================*/
/**
 * VistaSquareMatrix
 * Matrix[row][col]
 */
template <class Type, int dim>
class VistaSquareMatrix : public VistaMatrix<Type>
{
public:
	/// constructs nXn matrix, ordering: row X column, defaults to identity
	VistaSquareMatrix();
	~VistaSquareMatrix(){};

	VistaSquareMatrix<Type,dim> & operator = (const VistaMatrix<Type>& mat2);

	inline VistaSquareMatrix<Type,dim>   operator+
		( const VistaSquareMatrix<Type,dim> &  other ) const;
	inline VistaSquareMatrix<Type,dim>   operator-
		( const VistaSquareMatrix<Type,dim> &  other ) const;
	//inline VistaSquareMatrix<Type,dim>   operator*
	//    ( const VistaSquareMatrix<Type,dim> &  other ) const;

	//inline Type GetVal (int row, int col)
	//    {return VistaMatrix<Type>::m_pMat[row][col];}
	//inline bool SetVal (int row, int col, const Type & newVal)
	//    {VistaMatrix<Type>::m_pMat[row][col] = newVal; return true;}
	inline int GetDimension () const
		{return VistaMatrix<Type>::m_nRowDimension;}

	inline void SetIdentity();

	/**
	 * CalcInvariants computes the invariants of 2x2 and 3x3 matrices.
	 * The parameter invarR is used only in the case of 3x3 matrices.
	 * @param [invarP] the invarinat P
	 * @param [invarQ] the invarinat Q
	 * @param [invarR] the invarinat R
	 * @return bool true=success/false=failure
	 * @see CalcDiscriminant
	 */
	bool CalcInvariants   (Type & invarP, Type & invarQ, Type & invarR);
	bool CalcDiscriminant (Type & descrim);
	Type CalcDeterminant  ();
	Type CalcAdjunct (int row, int col);
	bool Inverse ();
	Type CalcTrace ();

	/**
	 * Calculation of eivenvalues, eigenvectors, eigenspace
	 * of any square matrices
	 */
	bool CalcEigenValues  (std::vector<std::complex<double>*> & vecEigenVal);
	bool CalcEigenVectors (std::vector<std::complex<double>*> & vecEigenVec);
	bool CalcEigenVector  (const std::complex<double> & vecEigenVal,
						   std::complex<double> * pEigenVec,
						   bool bCalcSecVec = false);
	bool CalcEigenSpace   (std::vector<std::complex<double>*> & vecEigenVal,
						   std::vector<std::complex<double>*> & vecEigenVec);
	bool CalcEigenSpaceNumerically
						  (std::vector<std::complex<double>*> & vecEigenVal,
						   std::vector<std::complex<double>*> & vecEigenVec);

	/**
	 * CalcHessenberg() computes the Hessenberg form H of the existing matrix.
	 * @return VistaSquareMatrix: the similarity matrix Q (A->Q^T*A*Q=H)
	 * @see GivenRotation()
	 */
	VistaSquareMatrix ComputeHessenbergEigensystem ();
	VistaSquareMatrix QRDecomposition ();
	VistaSquareMatrix GetRotationMatrixForHessenberg (int valP, int valQ) const;
	VistaSquareMatrix GetRotationMatrixForHessenberg () const;
	VistaSquareMatrix GivensRotationForHessenberg (int valP, int valQ);
	VistaSquareMatrix CalcUpperHessenberg ();

	inline bool IsSingular ();
	inline bool IsUniModular();
	inline bool IsDiagonal ();
	inline bool IsScalar ();
	inline bool IsUnit ();
	inline bool IsSymmetric ();
	inline bool IsSkewSymmetric ();
	inline bool IsUpperTriangular ();
	inline bool IsLowerTriangular ();

protected:

public:
	bool Debug ();

protected:
};

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>::VistaSquareMatrix()
:VistaMatrix<Type>(dim,dim)
{
	for (int diag = 0; diag < dim; ++diag)
		VistaMatrix<Type>::m_pMat[diag][diag] = 1;
}

/*============================================================================*/
/*============================================================================*/
// assignment operator
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim> &
VistaSquareMatrix<Type,dim>::operator = (const VistaMatrix<Type>& mat2)
{
//    return VistaMatrix<Type>::operator=(mat2);
#if 1
	if (VistaMatrix<Type>::m_nRowDimension != mat2.GetRowDimension() ||
		VistaMatrix<Type>::m_nColDimension != mat2.GetColDimension() )
	{
		// create new mem map
		VistaMatrix<Type>::ReleaseCoeff();
		VistaMatrix<Type>::m_nRowDimension = mat2.GetRowDimension();
		VistaMatrix<Type>::m_nColDimension = mat2.GetColDimension();
		VistaMatrix<Type>::AllocCoeff();
	}

	for (int row = 0; row < VistaMatrix<Type>::m_nRowDimension; ++row)
		for (int col = 0; col < VistaMatrix<Type>::m_nColDimension; ++col)
			VistaMatrix<Type>::m_pMat[row][col] = mat2.GetVal (row,col);

	return *this;
#endif
}

/*============================================================================*/
/*============================================================================*/
// plus operator
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::operator + (const VistaSquareMatrix<Type,dim>& mat2) const
{
	VistaSquareMatrix<Type,dim> tempMat;
	for (int row = 0; row < VistaMatrix<Type>::m_nRowDimension; ++row)
		for (int col = 0; col < VistaMatrix<Type>::m_nColDimension; ++col)
			tempMat.SetVal(row,col, VistaMatrix<Type>::m_pMat[row][col] + mat2.GetVal (row,col));

	return tempMat;
}

/*============================================================================*/
/*============================================================================*/
// minus operator
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::operator - (const VistaSquareMatrix<Type,dim>& mat2) const
{
	VistaSquareMatrix<Type,dim> tempMat;
	for (int row = 0; row < VistaMatrix<Type>::m_nRowDimension; ++row)
		for (int col = 0; col < VistaMatrix<Type>::m_nColDimension; ++col)
			tempMat.SetVal(row,col, VistaMatrix<Type>::m_pMat[row][col] - mat2.GetVal (row,col));

	return tempMat;
}

/*============================================================================*/
/*============================================================================*/
// multiply operator
#if 0
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::operator * (const VistaSquareMatrix<Type,dim>& mat2) const
{
	VistaSquareMatrix<Type,dim> tempMat;
	//(VistaSquareMatrix<Type,dim>) (*this * (VistaMatrix<Type>)mat2);
	return tempMat;
}
#endif

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
void VistaSquareMatrix<Type,dim>::SetIdentity()
{
	for (int row = 0; row < dim; ++row)
		for (int col = 0; col < dim; ++col)
			if(row == col)
				VistaMatrix<Type>::m_pMat[row][col] = Type(1);
			else
				VistaMatrix<Type>::m_pMat[row][col] = Type(0);
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
Type VistaSquareMatrix<Type,dim>::CalcDeterminant ()
{
	Type det = Type(0);
	if (GetDimension() == 2)
	{
		det = (VistaMatrix<Type>::m_pMat[0][0] * VistaMatrix<Type>::m_pMat[1][1] -
			   VistaMatrix<Type>::m_pMat[0][1] * VistaMatrix<Type>::m_pMat[1][0]);
	}

	if (GetDimension() == 3)
	{
		det = ( VistaMatrix<Type>::m_pMat[0][0] * (VistaMatrix<Type>::m_pMat[1][1] * VistaMatrix<Type>::m_pMat[2][2] -
													VistaMatrix<Type>::m_pMat[1][2] * VistaMatrix<Type>::m_pMat[2][1]) -
				VistaMatrix<Type>::m_pMat[0][1] * (VistaMatrix<Type>::m_pMat[1][0] * VistaMatrix<Type>::m_pMat[2][2] -
													VistaMatrix<Type>::m_pMat[1][2] * VistaMatrix<Type>::m_pMat[2][0]) +
				VistaMatrix<Type>::m_pMat[0][2] * (VistaMatrix<Type>::m_pMat[1][0] * VistaMatrix<Type>::m_pMat[2][1] -
													VistaMatrix<Type>::m_pMat[1][1] * VistaMatrix<Type>::m_pMat[2][0]) );
	}

	if (GetDimension() == 4)
	{
		Type result = 0;
		int i, j, k, l, m, n;
		j = 0; // Why not ?
		VistaSquareMatrix<Type,3> SubMat = VistaSquareMatrix<Type,3>();
		for( i = 0; i < GetDimension(); ++i )
		{
			for( k = 0; k < GetDimension(); ++k )
				for( l = 0; l < GetDimension(); ++l )
					if( k != i && l != j )
					{
						m = k < i ? k : k-1;
						n = l < j ? l : l-1;
						SubMat[m][n] = VistaMatrix<Type>::m_pMat[k][l];
					}
			result += pow(-1.0,i+j) * VistaMatrix<Type>::m_pMat[i][j] * SubMat.CalcDeterminant();
		}
		//return result;
		det = result;
	}

	if (GetDimension() == 5)
	{
		Type result = 0;
		int i, j, k, l, m, n;
		j = 0; // Why not ?
		VistaSquareMatrix<Type,4> SubMat = VistaSquareMatrix<Type,4>();
		for( i = 0; i < GetDimension(); ++i )
		{
			for( k = 0; k < GetDimension(); ++k )
				for( l = 0; l < GetDimension(); ++l )
					if( k != i && l != j )
					{
						m = k < i ? k : k-1;
						n = l < j ? l : l-1;
						SubMat[m][n] = VistaMatrix<Type>::m_pMat[k][l];
					}
			result += pow(-1.0,i+j) * VistaMatrix<Type>::m_pMat[i][j] * SubMat.CalcDeterminant();
		}
		//return result;
		det = result;
	}

	if (GetDimension() == 6)
	{
		Type result = 0;
		int i, j, k, l, m, n;
		j = 0; // Why not ?
		VistaSquareMatrix<Type,5> SubMat = VistaSquareMatrix<Type,5>();
		for( i = 0; i < GetDimension(); ++i )
		{
			for( k = 0; k < GetDimension(); ++k )
				for( l = 0; l < GetDimension(); ++l )
					if( k != i && l != j )
					{
						m = k < i ? k : k-1;
						n = l < j ? l : l-1;
						SubMat[m][n] = VistaMatrix<Type>::m_pMat[k][l];
					}
			result += pow(-1.0,i+j) * VistaMatrix<Type>::m_pMat[i][j] * SubMat.CalcDeterminant();
		}
		//return result;
		det = result;
	}

	if (GetDimension() == 7)
	{
		Type result = 0;
		int i, j, k, l, m, n;
		j = 0; // Why not ?
		VistaSquareMatrix<Type,6> SubMat = VistaSquareMatrix<Type,6>();
		for( i = 0; i < GetDimension(); ++i )
		{
			for( k = 0; k < GetDimension(); ++k )
				for( l = 0; l < GetDimension(); ++l )
					if( k != i && l != j )
					{
						m = k < i ? k : k-1;
						n = l < j ? l : l-1;
						SubMat[m][n] = VistaMatrix<Type>::m_pMat[k][l];
					}
			result += pow(-1.0,i+j) * VistaMatrix<Type>::m_pMat[i][j] * SubMat.CalcDeterminant();
		}
		//return result;
		det = result;
	}

	if (GetDimension() == 8)
	{
		Type result = 0;
		int i, j, k, l, m, n;
		j = 0; // Why not ?
		VistaSquareMatrix<Type,7> SubMat = VistaSquareMatrix<Type,7>();
		for( i = 0; i < GetDimension(); ++i )
		{
			for( k = 0; k < GetDimension(); ++k )
				for( l = 0; l < GetDimension(); ++l )
					if( k != i && l != j )
					{
						m = k < i ? k : k-1;
						n = l < j ? l : l-1;
						SubMat[m][n] = VistaMatrix<Type>::m_pMat[k][l];
					}
			result += pow(-1.0,i+j) * VistaMatrix<Type>::m_pMat[i][j] * SubMat.CalcDeterminant();
		}
		//return result;
		det = result;
	}

	if (GetDimension() == 9)
	{
		Type result = 0;
		int i, j, k, l, m, n;
		j = 0; // Why not ?
		VistaSquareMatrix<Type,8> SubMat = VistaSquareMatrix<Type,8>();
		for( i = 0; i < GetDimension(); ++i )
		{
			for( k = 0; k < GetDimension(); ++k )
				for( l = 0; l < GetDimension(); ++l )
					if( k != i && l != j )
					{
						m = k < i ? k : k-1;
						n = l < j ? l : l-1;
						SubMat[m][n] = VistaMatrix<Type>::m_pMat[k][l];
					}
			result += pow(-1.0,i+j) * VistaMatrix<Type>::m_pMat[i][j] * SubMat.CalcDeterminant();
		}
		//return result;
		det = result;
	}

	if (GetDimension() == 10)
	{
		Type result = 0;
		int i, j, k, l, m, n;
		j = 0; // Why not ?
		VistaSquareMatrix<Type,9> SubMat = VistaSquareMatrix<Type,9>();
		for( i = 0; i < GetDimension(); ++i )
		{
			for( k = 0; k < GetDimension(); ++k )
				for( l = 0; l < GetDimension(); ++l )
					if( k != i && l != j )
					{
						m = k < i ? k : k-1;
						n = l < j ? l : l-1;
						SubMat[m][n] = VistaMatrix<Type>::m_pMat[k][l];
					}
			result += pow(-1.0,i+j) * VistaMatrix<Type>::m_pMat[i][j] * SubMat.CalcDeterminant();
		}
		//return result;
		det = result;
	}

	// if determinant close to zero then set it to zero!
	VistaMathTools<Type>::IsZeroSetZero (det);
	return det;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
Type VistaSquareMatrix<Type,dim>::CalcAdjunct (int row, int col)
{
	if (GetDimension() == 2)
	{
		return (VistaMatrix<Type>::m_pMat[(row+1)%2][(col+1)%2]);
	}

	if (GetDimension() == 3)
	{
		return ( VistaMatrix<Type>::m_pMat[(row+1)%3][(col+1)%3] * VistaMatrix<Type>::m_pMat[(row+2)%3][(col+2)%3] -
				 VistaMatrix<Type>::m_pMat[(row+1)%3][(col+2)%3] * VistaMatrix<Type>::m_pMat[(row+2)%3][(col+1)%3] );
	}

	return (Type)0;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::Inverse ()
{
	Type curDet = this->CalcDeterminant();
	if (curDet == 0.0)
		return false;

	VistaSquareMatrix<Type,dim> matInv;

	matInv.SetVal (0,0, (VistaMatrix<Type>::GetVal(1,1) * VistaMatrix<Type>::GetVal(2,2) -
						 VistaMatrix<Type>::GetVal(1,2) * VistaMatrix<Type>::GetVal(2,1)) / curDet );
	matInv.SetVal (0,1, (VistaMatrix<Type>::GetVal(0,2) * VistaMatrix<Type>::GetVal(2,1) -
						 VistaMatrix<Type>::GetVal(0,1) * VistaMatrix<Type>::GetVal(2,2)) / curDet );
	matInv.SetVal (0,2, (VistaMatrix<Type>::GetVal(0,1) * VistaMatrix<Type>::GetVal(1,2) -
						 VistaMatrix<Type>::GetVal(0,2) * VistaMatrix<Type>::GetVal(1,1)) / curDet );

	matInv.SetVal (1,0, (VistaMatrix<Type>::GetVal(1,2) * VistaMatrix<Type>::GetVal(2,0) -
						 VistaMatrix<Type>::GetVal(1,0) * VistaMatrix<Type>::GetVal(2,2)) / curDet );
	matInv.SetVal (1,1, (VistaMatrix<Type>::GetVal(0,0) * VistaMatrix<Type>::GetVal(2,2) -
						 VistaMatrix<Type>::GetVal(0,2) * VistaMatrix<Type>::GetVal(2,0)) / curDet );
	matInv.SetVal (1,2, (VistaMatrix<Type>::GetVal(0,2) * VistaMatrix<Type>::GetVal(1,0) -
						 VistaMatrix<Type>::GetVal(0,0) * VistaMatrix<Type>::GetVal(1,2)) / curDet );

	matInv.SetVal (2,0, (VistaMatrix<Type>::GetVal(1,0) * VistaMatrix<Type>::GetVal(2,1) -
						 VistaMatrix<Type>::GetVal(1,1) * VistaMatrix<Type>::GetVal(2,0)) / curDet );
	matInv.SetVal (2,1, (VistaMatrix<Type>::GetVal(0,1) * VistaMatrix<Type>::GetVal(2,0) -
						 VistaMatrix<Type>::GetVal(0,0) * VistaMatrix<Type>::GetVal(2,1)) / curDet );
	matInv.SetVal (2,2, (VistaMatrix<Type>::GetVal(0,0) * VistaMatrix<Type>::GetVal(1,1) -
						 VistaMatrix<Type>::GetVal(0,1) * VistaMatrix<Type>::GetVal(1,0)) / curDet );

	*this = matInv;

	return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
Type VistaSquareMatrix<Type,dim>::CalcTrace ()
{
	if (GetDimension() == 2)
	{
		return (VistaMatrix<Type>::m_pMat[0][0] + VistaMatrix<Type>::m_pMat[1][1]);
	}

	if (GetDimension() == 3)
	{
		return (VistaMatrix<Type>::m_pMat[0][0] + VistaMatrix<Type>::m_pMat[1][1] + VistaMatrix<Type>::m_pMat[2][2]);
	}

	return (Type)0;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::CalcInvariants (Type & invarP, Type & invarQ, Type & invarR)
{
	if (GetDimension() == 2)
	{
		// P = - trace(matrix); Q = det(matrix); R not defined!
		// calc minus trace
		invarP = (Type)0 - VistaMatrix<Type>::m_pMat[0][0] - VistaMatrix<Type>::m_pMat[1][1];
		// calc determinant
		invarQ = CalcDeterminant();
		invarR = 0;
		return true;
	}

	if (GetDimension() == 3)
	{
		// P = - trace(matrix); Q = Adj00+Adj11+Adj22 ; R = - det(matrix)
		// calc minus trace
		invarP = (Type)0 - VistaMatrix<Type>::m_pMat[0][0] - VistaMatrix<Type>::m_pMat[1][1] - VistaMatrix<Type>::m_pMat[2][2];
		// calc adjuncts
		invarQ  = VistaMatrix<Type>::m_pMat[1][1] * VistaMatrix<Type>::m_pMat[2][2] - VistaMatrix<Type>::m_pMat[1][2] * VistaMatrix<Type>::m_pMat[2][1] +
				  VistaMatrix<Type>::m_pMat[0][0] * VistaMatrix<Type>::m_pMat[2][2] - VistaMatrix<Type>::m_pMat[0][2] * VistaMatrix<Type>::m_pMat[2][0] +
				  VistaMatrix<Type>::m_pMat[0][0] * VistaMatrix<Type>::m_pMat[1][1] - VistaMatrix<Type>::m_pMat[0][1] * VistaMatrix<Type>::m_pMat[1][0];
		// calc negative determinant
		invarR = - (CalcDeterminant());
		return true;
	}

	return false;
}

/*============================================================================*/
// Discriminant, lat. "discriminare": "unterscheiden"
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::CalcDiscriminant (Type & discrim)
{
	// I need the invariants
	Type invarP, invarQ, invarR;
	CalcInvariants (invarP, invarQ, invarR);

	if (GetDimension() == 2)
	{
		discrim = (invarP*invarP - (Type)4*invarQ)/(Type)4.0;
		return true;
	}
	else if (GetDimension() == 3)
	{
#if 0
		// ===== 8 Mult, 4 Div, 3 Sub, 1 Add
		discrim = (invarQ / 3 - invarP * invarP / 9);
		discrim = discrim * discrim * discrim;
		Type secVal1 = (invarQ * invarP - 3 * invarR)/6 - invarP*invarP*invarP / 27;
		discrim +=  secVal1 * secVal1;
		//std::cout << "Discriminant (#1): " << discrim << std::endl;
#else
		// ===== 12 Mult, 1 Div, 3 Sub, 1 Add
		discrim = (Type)3 * invarQ - invarP * invarP;
		discrim = (Type)4 * discrim * discrim * discrim;
		Type secVal2 = (Type)9 * invarQ * invarP - (Type)27 * invarR - (Type)2 * invarP * invarP * invarP;
		discrim = (discrim + secVal2 * secVal2) / (Type)2916;
#ifdef DEBUG
		//std::cout << "Discriminant (2.Vers): " << discrim << std::endl;
#endif
#endif
		return true;
	}

	return false;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::CalcEigenValues (std::vector<std::complex<double>*> & pEV)
{
	//if (this->IsSingular ())
	{
		//cout << "SINGULAR\n";
		/*for (curEVal = 0; curVal < this->GetDimension (); curEVal++)
		{
			pEV[curEVal] = std::complex<double>(0,0);
		}
		return false;*/
	}
	//if (this->IsSymmetric ())
	{
		// if real symmetric matrix A: A = UDU^T (U: orthogonal matrix)
		// D is a diagonal matrix with eigenvalues as diagonal elements
		//cout << "SYMMETRIC\n";
	}
	if (GetDimension() == 2)
	{
		// calc trace/2
		*pEV[0] = std::complex<double>((VistaMatrix<Type>::m_pMat[0][0] + VistaMatrix<Type>::m_pMat[1][1]) / 2.0, 0.0);
		*pEV[1] = std::complex<double>( pEV[0]->real(),0 );
		// calc determinant
		double nDet = CalcDeterminant();
		// calc square imaginary: trace^2/4-det
		double nSIm = (VistaMatrix<Type>::m_pMat[0][0] + VistaMatrix<Type>::m_pMat[1][1]) * (VistaMatrix<Type>::m_pMat[0][0] + VistaMatrix<Type>::m_pMat[1][1]) / 4.0;
		nSIm -= nDet;
		// both eigenvalues are real
		if (nSIm > 0)
		{
			double nSRoot = sqrt(nSIm);
			*pEV[0] = std::complex<double>( pEV[0]->real() + nSRoot, pEV[0]->imag());
			*pEV[1] = std::complex<double>( pEV[1]->real() - nSRoot, pEV[1]->imag());
		}
		// both eigenvalues are conjugate complex
		else
		{
			double nSRoot = sqrt(-nSIm);
			*pEV[0] = std::complex<double>( pEV[0]->real(), pEV[0]->imag() + nSRoot );
			*pEV[1] = std::complex<double>( pEV[1]->real(), pEV[1]->imag() - nSRoot );
		}
		VistaMathTools< std::complex<double> >::IsZeroSetZero (*pEV[0]);
		VistaMathTools< std::complex<double> >::IsZeroSetZero (*pEV[1]);

#ifdef MATHDEBUG
		vstr::out() << "Determinate: " << nDet << std::endl;
		vstr::out() << "Eigenvalue 1: (" << pEV[0]->real() << ", " << pEV[0]->imag() << "i)\n";
		vstr::out() << "Eigenvalue 2: (" << pEV[1]->real() << ", " << pEV[1]->imag() << "i)\n";
		// Vieta: Sum(eVal)=Sum(a[i][i]), Prod(eVal)=det(A)
		std::complex<double> sumEVal  = (*pEV[0]) + (*pEV[1]);
		std::complex<double> prodEVal = (*pEV[0]) * (*pEV[1]);
		vstr::out() << "Vieta EVal-Sum:     (" << sumEVal.real() << ", " << sumEVal.imag() << "i)=trace(A)\n";
		vstr::out() << "Vieta EVal-Product: (" << prodEVal.real() << ", " << prodEVal.imag() << "i)=det(A)\n";
#endif
		return true;
	}

	if (GetDimension() == 3)
	{
		double invarP, invarQ, invarR;
		CalcInvariants (invarP, invarQ, invarR);
		// search roots
		VistaPolynomial<double,4> eValSolver;
		eValSolver.SetVal (3, 1.0);
		eValSolver.SetVal (2, invarP);
		eValSolver.SetVal (1, invarQ);
		eValSolver.SetVal (0, invarR);
		eValSolver.FindRoots (/*pRoots*/pEV);

#ifdef MATHDEBUG
		// calc determinant (Entwicklungssatz)
		double nDet = CalcDeterminant();
		vstr::out() << "Determinant: " << nDet << std::endl;
		// the polynomial
		eValSolver.Debug();
		// the eigenvalues
		vstr::out() << "Eigenvalue 1: (" << pEV[0]->real() << ", " << pEV[0]->imag() << "i)\n";
		vstr::out() << "Eigenvalue 2: (" << pEV[1]->real() << ", " << pEV[1]->imag() << "i)\n";
		vstr::out() << "Eigenvalue 3: (" << pEV[2]->real() << ", " << pEV[2]->imag() << "i)\n";
		// check Vieta
		std::complex<double> sumEVal  = (*pEV[0]) + (*pEV[1]) + (*pEV[2]);
		std::complex<double> prodEVal = (*pEV[0]) * (*pEV[1]) * (*pEV[2]);
		vstr::out() << "Vieta EVal-Sum:     (" << sumEVal.real()  << ", " << sumEVal.imag()  << "i)=trace(A)\n";
		vstr::out() << "Vieta EVal-Product: (" << prodEVal.real() << ", " << prodEVal.imag() << "i)=det(A)\n";
#endif
		return true;
	}

	if (GetDimension() > 3)
	{
		VistaSquareMatrix<Type,dim> matH (*this);
		VistaSquareMatrix<Type,dim> matQH;
		matQH = matH.CalcUpperHessenberg();
		VistaSquareMatrix<Type,dim> matHi;
		VistaSquareMatrix<Type,dim> matRi;
		VistaSquareMatrix<Type,dim> matQi;
		matRi = (*this);
		matQi = matRi.ComputeHessenbergEigensystem ();
		// eigenvalues are stored on diagonal of iterated hessenberg
		// these are identical to those of the input matrix
		matHi = matRi * matQi;
		// if mat[i+1][i] != 0 ==> mat[i][i] to mat[i+1][i+1] depicts
		// a 2x2 sub-matrix which consists of to complex, conjugate
		// eigenvalues
		for (int diag = 0; diag < this->GetDimension (); ++diag)
		{
			if ((diag!=this->GetDimension()-1) &&
				(!VistaMathTools<Type>::IsZero(matHi[diag+1][diag])))
			{
				VistaSquareMatrix<double,2> matSub2;
				matSub2[0][0] = matHi[diag][diag];
				matSub2[0][1] = matHi[diag][diag+1];
				matSub2[1][0] = matHi[diag+1][diag];
				matSub2[1][1] = matHi[diag+1][diag+1];
				std::vector<std::complex<double>*> pSubEVecs = std::vector<std::complex<double>*>(2);
				pSubEVecs[0] = pEV[diag];
				pSubEVecs[1] = pEV[diag+1];
				matSub2.CalcEigenValues(pSubEVecs);
				// jump over next
				diag++;
			}
			else
			{
				(*pEV[diag]) = std::complex<double>(matHi[diag][diag],0);
			}
		}
		return true;
	}

	//std::cout << " [ViSqMx::CalcEigenVal] WARNING: EIGEN VALUES COULD NOT BE DETERMINED!\n";
	return false;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::CalcEigenVectors (std::vector<std::complex<double>*> & pEigenVecs)
{
	if (GetDimension() <= 3)
	{
		std::vector<std::complex<double>*> pEigenValues = std::vector<std::complex<double>*>(3);
		pEigenValues[0] = new std::complex<double>;
		pEigenValues[1] = new std::complex<double>;
		pEigenValues[2] = new std::complex<double>;
		return CalcEigenSpace (pEigenValues, pEigenVecs);
	}
	else
	{
		std::vector<std::complex<double>*> pEigenValues;
		for (int curEVal = 0; curEVal < this->GetDimension(); ++curEVal)
		{
			pEigenValues.push_back(new std::complex<double>);
		}
		return CalcEigenSpace (pEigenValues, pEigenVecs);
	}
	return false;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::CalcEigenVector  (
					 const std::complex<double> & eigenVal,
						   std::complex<double> * pEigenVec,
						   bool bCalcSecVec)
{
	if (GetDimension() == 2)
	{
		// search a solution for following equation:
		// ==================================
		// (j11 - ev) x1 +  j21       x2 = 0
		//  j12       x1 + (j22 - ev) x2 = 0
		// ==================================
#ifdef MATHDEBUG
		VistaSquareMatrix<std::complex<double>,2> eqMat;
		{
			std::complex<double> diag00 = m_pMat[0][0]-eigenVal;
			VistaMathTools< std::complex<double> >::IsZeroSetZero (diag00);
			std::complex<double> diag11 = m_pMat[1][1]-eigenVal;
			VistaMathTools< std::complex<double> >::IsZeroSetZero (diag11);
			eqMat.SetVal(0,0, diag00);
			eqMat.SetVal(0,1, m_pMat[0][1]);
			eqMat.SetVal(1,0, m_pMat[1][0]);
			eqMat.SetVal(1,1, diag11);
		}
		eqMat.Debug();
#endif
		int sIdx = 0;
		int tIdx = 1;
		std::complex<double> sVar (0,0);
		std::complex<double> tVar (1,0);
		if (bCalcSecVec)
		{
			tVar = sVar;
			sVar = std::complex<double>(1,0);
		}
		// THIS SOLUTION IS NOT NUMERICALLY STABLE, YET (I guess)
		//if ((VistaMatrix<Type>::m_pMat[0][0] - eigenVal) != std::complex<double>(0))
		// use fuzzy factor instead!
		if (!VistaMathTools< std::complex<double> >::IsZero (
				VistaMatrix<Type>::m_pMat[0][0] - eigenVal))
		{
			sIdx = 0;
			tIdx = 1;
			pEigenVec[sIdx] = - VistaMatrix<Type>::m_pMat[0][1] /
								(VistaMatrix<Type>::m_pMat[0][0] - eigenVal) * tVar;
			pEigenVec[tIdx] = tVar;
		}
		else if (!VistaMathTools< std::complex<double> >::IsZero (
					VistaMatrix<Type>::m_pMat[1][1] - eigenVal))
		{
			sIdx = 1;
			tIdx = 0;
			pEigenVec[tIdx] = tVar;
			pEigenVec[sIdx] = - VistaMatrix<Type>::m_pMat[1][0] /
								(VistaMatrix<Type>::m_pMat[1][1] - eigenVal) * tVar;
		}
		else
		{
			// select arbitray values
			pEigenVec[sIdx] = sVar;
			pEigenVec[tIdx] = tVar;
			vstr::warnp() << "VistaSquareMatrix::CalcEigenVector() -- Diagonal Coefficients are Zero" << std::endl;
			return false;
		}
		VistaMathTools< std::complex<double> >::IsZeroSetZero (pEigenVec[0]);
		VistaMathTools< std::complex<double> >::IsZeroSetZero (pEigenVec[1]);
		return true;
	}
	if (GetDimension() == 3)
	{
		// search a solution for following equation:
		// =================================================
		// (j11 - ev) x1 +  j12       x2 +  j13       x3 = 0
		//  j21       x1 + (j22 - ev) x2 +  j23       x3 = 0
		//  j31       x1 +  j32       x2 + (j33 - ev) x3 = 0
		// =================================================
		// compute diagonal coefficients
		VistaSquareMatrix<std::complex<double>,3> eqMat;
		{
			std::complex<double> diag00 = VistaMatrix<Type>::m_pMat[0][0]-eigenVal;
			VistaMathTools< std::complex<double> >::IsZeroSetZero (diag00);
			std::complex<double> diag11 = VistaMatrix<Type>::m_pMat[1][1]-eigenVal;
			VistaMathTools< std::complex<double> >::IsZeroSetZero (diag11);
			std::complex<double> diag22 = VistaMatrix<Type>::m_pMat[2][2]-eigenVal;
			VistaMathTools< std::complex<double> >::IsZeroSetZero (diag22);
			eqMat.SetVal(0,0, diag00);
			eqMat.SetVal(0,1, VistaMatrix<Type>::m_pMat[0][1]);
			eqMat.SetVal(0,2, VistaMatrix<Type>::m_pMat[0][2]);
			eqMat.SetVal(1,0, VistaMatrix<Type>::m_pMat[1][0]);
			eqMat.SetVal(1,1, diag11);
			eqMat.SetVal(1,2, VistaMatrix<Type>::m_pMat[1][2]);
			eqMat.SetVal(2,0, VistaMatrix<Type>::m_pMat[2][0]);
			eqMat.SetVal(2,1, VistaMatrix<Type>::m_pMat[2][1]);
			eqMat.SetVal(2,2, diag22);
		}
#ifdef MATHDEBUG
		eqMat.Debug();

		// calc determinant (Entwicklungssatz)
		std::complex<double> nDiscr = 0.0;
		eqMat.CalcDiscriminant(nDiscr);
		if (nDiscr == 0.0)
		{
			// use 1*t and 1*s
		}
		std::complex<double> nDet = 0;
		nDet = eqMat.CalcDeterminant();
#endif

		// search pos != 0 in first row
		int devPnt     = -1;
		int tPnt       = -1;
		int firstPnt   = -1;
		std::complex<double> detU_first(0,0);
		std::complex<double> detU_sec(0,0);
		bool bStartFound = false;

		// ################
		// ## FIRST CASE ##
		// ################
		// all coeffs are zero in first row
		if (VistaMathTools< std::complex<double> >::IsZero(eqMat.GetVal(0,0)) &&
			VistaMathTools< std::complex<double> >::IsZero(eqMat.GetVal(0,1)) &&
			VistaMathTools< std::complex<double> >::IsZero(eqMat.GetVal(0,2)) )
		{
			//std::cout << " [VSM-EVEC] WARNING: ALL COEFFICIENTS IN FIRST ROW ARE ZERO!\n";
			// select tIdx with U1t != zero
			// compute xf for remaining coeff
			// det U00
			std::complex<double> detU00 = eqMat.CalcAdjunct(0,0);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU00);
			// det U01
			std::complex<double> detU01 = eqMat.CalcAdjunct(0,1);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU01);
			// det U02
			std::complex<double> detU02 = eqMat.CalcAdjunct(0,2);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU02);
			if (!VistaMathTools< std::complex<double> >::IsZero(detU00))
			{
				pEigenVec[0] = 1; //t
				pEigenVec[1] = detU01 / detU00;
				pEigenVec[2] = detU02 / detU00;
#ifdef MATHDEBUG
				vstr::outi() << " [VSM-EVEC] EVec(" << eigenVal << ")=["
					<< "t=1," << pEigenVec[1] << "," << pEigenVec[2] << "]\n";
#endif
				return true;
			}
			if (!VistaMathTools< std::complex<double> >::IsZero(detU01))
			{
				pEigenVec[0] = detU00 / detU01;
				pEigenVec[1] = 1; //t
				pEigenVec[2] = detU02 / detU01;
#ifdef MATHDEBUG
				vstr::outi() << " [VSM-EVEC] EVec(" << eigenVal << ")=["
					<< pEigenVec[0] << "," << "t=1," << pEigenVec[2] << "]\n";
#endif
				return true;
			}
			if (!VistaMathTools< std::complex<double> >::IsZero(detU02))
			{
				pEigenVec[0] = detU00 / detU02;
				pEigenVec[1] = detU01 / detU02;
				pEigenVec[2] = 1; //t
#ifdef MATHDEBUG
				vstr::outi() << " [VSM-EVEC] EVec(" << eigenVal << ")=["
					<< pEigenVec[0] << "," << pEigenVec[1] << "t=1," << "]\n";
#endif
				return true;
			}
			vstr::warnp() << "VistaSquareMatrix::CalcEigenVector() -- could not be solved" << std::endl;
			return false;
		}

		// #################
		// ## SECOND CASE ##
		// #################
		// search a start developing point in first row
		if (!VistaMathTools< std::complex<double> >::IsZero(eqMat.GetVal(0,2)) )
		{
			bStartFound = true;
			devPnt   = 2;
			tPnt     = 1;
			firstPnt = 0;
			// det U01
			detU_first = eqMat.CalcAdjunct(0,1);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU_first);
			// det U00
			detU_sec   = eqMat.CalcAdjunct(0,0);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU_sec);
			if (detU_first == std::complex<double>(0,0))
			{
				// switch tPnt
				tPnt     = 0;
				firstPnt = 1;
				detU_first = detU_sec;
				detU_sec   = std::complex<double>(0,0);
				// still zero => try another one
				if (detU_first == std::complex<double>(0,0))
				{
					bStartFound = false;
				}
			}
		}
		if (!bStartFound && !VistaMathTools< std::complex<double> >::IsZero(eqMat.GetVal(0,1)) )
		{
			bStartFound = true;
			devPnt   = 1;
			tPnt     = 0;
			firstPnt = 2;
			// det U00
			detU_first = eqMat.CalcAdjunct(0,0);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU_first);
			// det U02
			detU_sec   = eqMat.CalcAdjunct(0,2);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU_sec);
			if (detU_first == std::complex<double>(0,0) )
			{
				// switch tPnt
				tPnt     = 2;
				firstPnt = 0;
				detU_first = detU_sec;
				detU_sec   = std::complex<double>(0,0);
				if (detU_first == std::complex<double>(0,0))
				{
					bStartFound = false;
				}
			}
		}
		if (!bStartFound && !VistaMathTools< std::complex<double> >::IsZero(eqMat.GetVal(0,0)) )
		{
			bStartFound = true;
			devPnt   = 0;
			tPnt     = 2;
			firstPnt = 1;
			// det U02
			detU_first = eqMat.CalcAdjunct(0,2);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU_first);
			// det U01
			detU_sec   = eqMat.CalcAdjunct(0,1);
			VistaMathTools< std::complex<double> >::IsZeroSetZero (detU_sec);
			if (detU_first == std::complex<double>(0,0) )
			{
				// switch tPnt
				tPnt     = 1;
				firstPnt = 2;
				detU_first = detU_sec;
				detU_sec   = std::complex<double>(0,0);
				if (detU_first == std::complex<double>(0,0))
				{
					bStartFound = false;
				}
			}
		}
#ifdef MATHDEBUG
		vstr::outi() << " [VSM-EVEC] "
				  << "startFound=" << bStartFound
				  << "; devPnt=" << devPnt
				  << "; tPnt=" << tPnt
				  << "; fPnt=" << firstPnt
				  << "; discr="  << nDiscr
				  << "; det="  << nDet
				  << std::endl;
#endif

		// standard computation
		if (bStartFound)
		{
			std::complex<double> resComp1 (0,0);
			resComp1 = detU_sec / detU_first;

			pEigenVec[tPnt] = 1;
			pEigenVec[firstPnt] = resComp1;

			if (devPnt == 0)
			{
				if (tPnt== 1)
				{
					pEigenVec[devPnt] =
						- (VistaMatrix<Type>::m_pMat[0][1] + VistaMatrix<Type>::m_pMat[0][2]*resComp1) / eqMat.GetVal(0,devPnt);
					return true;
				}
				if (tPnt== 2)
				{
					pEigenVec[devPnt] =
						- (VistaMatrix<Type>::m_pMat[0][1]*resComp1 + VistaMatrix<Type>::m_pMat[0][2]) / eqMat.GetVal(0,devPnt);
					return true;
				}
			}
			if (devPnt == 1)
			{
				if (tPnt== 0)
				{
					pEigenVec[devPnt] =
						- (eqMat.GetVal(0,0) + VistaMatrix<Type>::m_pMat[0][2]*resComp1) / VistaMatrix<Type>::m_pMat[0][devPnt];
					return true;
				}
				if (tPnt== 2)
				{
					pEigenVec[devPnt] =
						- (eqMat.GetVal(0,0)*resComp1 + VistaMatrix<Type>::m_pMat[0][2]) / VistaMatrix<Type>::m_pMat[0][devPnt];
					return true;
				}
			}
			if (devPnt == 2)
			{
				if (tPnt== 0)
				{
					pEigenVec[devPnt] =
						- (eqMat.GetVal(0,0) + VistaMatrix<Type>::m_pMat[0][1]*resComp1) / VistaMatrix<Type>::m_pMat[0][devPnt];
					return true;
				}
				if (tPnt== 1)
				{
					pEigenVec[devPnt] =
						- (eqMat.GetVal(0,0)*resComp1 + VistaMatrix<Type>::m_pMat[0][1]) / VistaMatrix<Type>::m_pMat[0][devPnt];
					return true;
				}
			}
		}
		// ################
		// ## THIRD CASE ##
		// ################
		if (!bStartFound)
		{
			// ALL SUB DETERMINATES ARE ZERO!
			// X[f] = (-M[2,s]/M[2,f])s + (-M[2,t]/M[2,f])t
			// X[s]=1, X[t]=0
			// 1. Version: s=(f+1)%2, t=(f+2)%2
			// 2. Version: s=(f+2)%2, t=(f+1)%2
			std::complex<double> sVar (1,0);
			std::complex<double> tVar (0,0);
			if (bCalcSecVec)
			{
				sVar = tVar;
				tVar = std::complex<double>(1,0);
			}
			int fIdx = -1;
			int sIdx = -1;
			int tIdx = -1;

			if (!VistaMathTools< std::complex<double> >::IsZero (eqMat.GetVal(2,0)))
			{
				bStartFound = true;
				fIdx = 0;
				sIdx = 1;
				tIdx = 2;
			}
			if (!bStartFound && !VistaMathTools< std::complex<double> >::IsZero (eqMat.GetVal(2,1)) )
			{
				bStartFound = true;
				fIdx = 1;
				sIdx = 0;
				tIdx = 2;
			}
			if (!bStartFound && !VistaMathTools< std::complex<double> >::IsZero (eqMat.GetVal(2,2)) )
			{
				bStartFound = true;
				fIdx = 2;
				sIdx = 0;
				tIdx = 1;
			}
#ifdef MATHDEBUG
			vstr::outi() << " [VSM-EVEC] "
				  << "startFound=" << bStartFound
				  << "; fIdx=" << fIdx
				  << "; sIdx=" << sIdx
				  << "; tIdx="  << tIdx
				  << std::endl;
#endif
			if (bStartFound)
			{
				pEigenVec[fIdx] =
					(-eqMat.GetVal(2,sIdx))/eqMat.GetVal(2,fIdx) * sVar +
					(-eqMat.GetVal(2,tIdx))/eqMat.GetVal(2,fIdx) * tVar;
				pEigenVec[sIdx] = sVar;
				pEigenVec[tIdx] = tVar;
#ifdef MATHDEBUG
				vstr::outi() << " [VSM-EVEC] EVec(" << eigenVal << ")={"
					<< "x[" << fIdx << "]="
					<< (-eqMat.GetVal(2,sIdx))/eqMat.GetVal(2,fIdx)
					<< "*s+"
					<< (-eqMat.GetVal(2,tIdx))/eqMat.GetVal(2,fIdx)
					<< "*t"
					<< ",x[" << sIdx << "]=" << "s=" << sVar
					<< ",x[" << tIdx << "]=" << "t=" << tVar
					<< "}\n";
#endif
				return true;
			}
			vstr::warnp() << "VistaSquareMatrix::CalcEigenVector() --  t+s in 3rd Row failed" << std::endl;
			return false;
		}


	}

	return false;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::CalcEigenSpace (
							 std::vector<std::complex<double>*> & pEigenVals,
							 std::vector<std::complex<double>*> & pEigenVecs)
{
	if (GetDimension() == 2)
	{
		double invarD = 0;
		CalcDiscriminant (invarD);
		CalcEigenValues (pEigenVals);
		CalcEigenVector (*pEigenVals[0], pEigenVecs[0]);
		if (invarD != 0)
		{
			CalcEigenVector (*pEigenVals[1], pEigenVecs[1]);
		}
		else
		{
			// compute second real variant of same eigenvalue
			CalcEigenVector (*pEigenVals[1], pEigenVecs[1], true);
		}
		return true;
	}

	if (GetDimension() == 3)
	{
		double invarD = 0;
		CalcDiscriminant (invarD);
		CalcEigenValues (pEigenVals);
		//double invarD;
		//CalcDiscriminant (invarD);
		CalcEigenVector (*pEigenVals[0], pEigenVecs[0]);
		CalcEigenVector (*pEigenVals[1], pEigenVecs[1]);
		if (invarD != 0)
		{
			CalcEigenVector (*pEigenVals[2], pEigenVecs[2]);
		}
		else
		{
			CalcEigenVector (*pEigenVals[2], pEigenVecs[2], true);
		}
		return true;
	}
	if (GetDimension() > 3)
	{
		return CalcEigenSpaceNumerically (pEigenVals, pEigenVecs);
	}

	return false;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::CalcEigenSpaceNumerically (
							 std::vector<std::complex<double>*> & pEigenVals,
							 std::vector<std::complex<double>*> & pEigenVecs)
{
	VistaSquareMatrix<Type,dim> matH (*this);
	VistaSquareMatrix<Type,dim> matQH;
	matQH = matH.CalcUpperHessenberg();
	VistaSquareMatrix<Type,dim> matHi;
	VistaSquareMatrix<Type,dim> matRi;
	VistaSquareMatrix<Type,dim> matQi;
	matRi = (*this);
	matQi = matRi.ComputeHessenbergEigensystem ();
	// eigenvalues are stored on diagonal of iterated hessenberg
	// these are identical to those of the input matrix
	matHi = matRi * matQi;
	// if mat[i+1][i] != 0 ==> mat[i][i] to mat[i+1][i+1] depicts
	// a 2x2 sub-matrix which consists of to complex, conjugate
	// eigenvalues
	for (int diag = 0; diag < this->GetDimension (); ++diag)
	{
		if ((diag!=this->GetDimension()-1) &&
			(!VistaMathTools<Type>::IsZero(matHi[diag+1][diag])))
		{
			VistaSquareMatrix<double,2> matSub2;
			matSub2[0][0] = matHi[diag][diag];
			matSub2[0][1] = matHi[diag][diag+1];
			matSub2[1][0] = matHi[diag+1][diag];
			matSub2[1][1] = matHi[diag+1][diag+1];
			std::vector<std::complex<double>*> pSubEVecs = std::vector<std::complex<double>*>(2);
			pSubEVecs[0] = pEigenVals[diag];
			pSubEVecs[1] = pEigenVals[diag+1];
			matSub2.CalcEigenValues(pSubEVecs);
			// jump over next
			diag++;
		}
		else
		{
			(*pEigenVals[diag]) = std::complex<double>(matHi[diag][diag],0);
		}
	}
	// compute eigenvectors EVecHi[i] of Hi: R*EVecHi[i]=Lambda[i]*EVecHi[i]
	// (R - Lambda[i]*I) * EVecHi[i] = 0
	//...
	for (int curEVal = 0; curEVal < this->GetDimension (); ++curEVal)
	{
		pEigenVecs[curEVal];
	}
	VistaSquareMatrix<Type,dim> matQA;
	matQA = matQH * matQi;
	VistaSquareMatrix<std::complex<Type>,dim> matComplex;
	// eigenvectors must be retransformed: EVecA[i] = matQA * EVecHi[i]
	//...
	return true;
}

/*============================================================================*/
/*============================================================================*/
// QR Decomposition is one of the most efficient Approaches at the moment!
// For arbitrary square matrices, we use a Hessenberg transformation and start
// then with iterative QR decomposition. This results in an upper R-matrix
// containing all real and complex eigenvalues, respectively. The composition
// of the rotation matrices yield a matrix containing all eigenvectors.
// (cf. Weller, p.366ff)
/*============================================================================*/
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::ComputeHessenbergEigensystem ()
{
	bool bTerminate = false;
	int curIteration = 0;
	int maxIterations = 1000;

	// transform matrix A to Hessenberg form in order to reduce computation cost
	VistaSquareMatrix<Type,dim> matH (*this);
	VistaSquareMatrix<Type,dim> matQH;
	matQH = matH.CalcUpperHessenberg();

	// now iterate
	VistaSquareMatrix<Type,dim> matHi (matH);
	VistaSquareMatrix<Type,dim> matRi;
	VistaSquareMatrix<Type,dim> matQi;

	while (bTerminate == false)
	{
		curIteration++;
		// use H(i-1) for next iteration
		matRi = matHi;
		matQi = matRi.QRDecomposition ();
		// inverse maultiplication order
		// H(i-1)=QR; H(i)=RQ
		matHi = matRi * matQi;
		// check sub-diagonal elements fo rzero
		int nullSubCount = 0;
		for (int curSub = 0; curSub < matHi.GetDimension() - 1; ++curSub)
		{
			if (std::abs(matHi[curSub+1][curSub]) < std::abs(
					std::numeric_limits<Type>::epsilon() *
						std::max<Type> (matHi[curSub][curSub], matHi[curSub+1][curSub+1])))
			{
				matHi[curSub+1][curSub] = Type(0);
			}
			if (matHi[curSub+1][curSub] == Type(0))
				nullSubCount++;
		}
		if (nullSubCount >= matHi.GetDimension() - 1)
			bTerminate = true;
		if (curIteration == maxIterations)
			bTerminate = true;
	}

	//cout << " Needed iterations: " << curIteration << std::endl;

	(*this) = matRi;
	return matQi;
}

/*============================================================================*/
/*============================================================================*/
// A = Q * R
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::QRDecomposition ()
{
	VistaSquareMatrix<Type,dim> matQT;
	VistaSquareMatrix<Type,dim> matUT;
	int row, col;
	// eliminiate lower triangular matrix by givens rotations
	for (col = 0; col < GetDimension() - 1; ++col)
	{
		for (row = col + 1; row < GetDimension(); ++row)
		{
			// Givens returns U(col,row)
			matUT = this->GivensRotation (col,row);
			matUT.Transpose();
			matQT = matUT * matQT;
		}
	}
	// return Q, not Q transposed
	matQT.Transpose();
	return matQT;
}

/*============================================================================*/
/*============================================================================*/
// QR-reduction with hessenberg using Givens elimination (Weller, p.366ff)
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::GivensRotationForHessenberg (int valP, int valQ)
{
	VistaSquareMatrix<Type,dim> matU;
	for (int diag = 0; diag < this->m_nRowDimension; ++diag)
		matU.SetVal(diag,diag,1);

	if (!VistaMathTools<Type>::IsZero (this->m_pMat[valP][valP-1]))
	{
		Type SignP1P = -1;
		if (this->m_pMat[valP][valP-1] >= Type(0))
			SignP1P = 1;
		Type valW = SignP1P *
					sqrt(this->m_pMat[valP][valP-1] * this->m_pMat[valP][valP-1] +
						 this->m_pMat[valQ][valP-1] * this->m_pMat[valQ][valP-1]);
		Type valCos =   this->m_pMat[valP][valP-1] / valW;
		Type valSin = - this->m_pMat[valQ][valP-1] / valW;
		// the first coefficient
		this->m_pMat[valP][valP-1] = this->m_pMat[valP][valP-1] * valCos - this->m_pMat[valQ][valP-1] * valSin;
		this->m_pMat[valQ][valP-1] = 0;
		// all other coefficients
		Type valHelp = Type(0);
		// THE FOLLOWING LINES ARE SOMEWHAT BUGGY: DONNOT KNOW WHY!
		// Cij: eq. 6.34, p. 359
		for (int col = valP; col < this->m_nColDimension; ++col)
		//for (int col = 0; col < m_nColDimension; ++col)
		{
			valHelp           = this->m_pMat[valP][col] * valCos - this->m_pMat[valQ][col] * valSin;
			this->m_pMat[valQ][col] = this->m_pMat[valP][col] * valSin + this->m_pMat[valQ][col] * valCos;
			this->m_pMat[valP][col] = valHelp;
		}
		// Bij: eq. 6.35, p. 359
		for (int row = 0; row < this->m_nRowDimension; ++row)
		{
		valHelp                 = this->m_pMat[row][valP] * valCos - this->m_pMat[row][valQ] * valSin;
			this->m_pMat[row][valP] = this->m_pMat[row][valP] * valSin + this->m_pMat[row][valQ] * valCos;
			this->m_pMat[row][valQ] = valHelp;
		}
		// END OF BUGGY SECTION
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
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::CalcUpperHessenberg ()
{
	VistaSquareMatrix<Type,dim> matQ;
	//VistaSquareMatrix<Type,dim> matU;
#if 0
	int row, col;
	for (col = 1; col < GetDimension() - 1; ++col)
	{
		for (row = col + 1; row < GetDimension(); ++row)
		{
			matQ = matQ * GivensRotationForHessenberg (col,row);
		}
	}
#else
	matQ = GetRotationMatrixForHessenberg ();
	VistaSquareMatrix<Type,dim> matQT (matQ);
	matQT.Transpose ();
	// H = Q^T * A * Q
	(*this) = matQT * (*this) * matQ;
#endif
	return matQ;
}

/*============================================================================*/
/*============================================================================*/
// QR-reduction with hessenberg using Givens elimination (Weller, p.366ff)
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::GetRotationMatrixForHessenberg () const
{
	int row, col;
	VistaSquareMatrix<Type,dim> matQ;
	for (col = 1; col < GetDimension() - 1; ++col)
	{
		for (row = col + 1; row < GetDimension(); ++row)
		{
			matQ = matQ * GetRotationMatrixForHessenberg (col,row);
		}
	}
	return matQ;
}

/*============================================================================*/
/*============================================================================*/
// QR-reduction with hessenberg using Givens elimination (Weller, p.366ff)
template <class Type, int dim>
inline
VistaSquareMatrix<Type,dim>
VistaSquareMatrix<Type,dim>::GetRotationMatrixForHessenberg (int valP, int valQ) const
{
	VistaSquareMatrix<Type,dim> matU;
	for (int diag = 0; diag < this->m_nRowDimension; ++diag)
		matU.SetVal(diag,diag,1);
	Type valCos =  Type(0);
	Type valSin =  Type(1);
	if (!VistaMathTools<Type>::IsZero (this->m_pMat[valP][valP-1]))
	{
		Type SignP1P = -1;
		if (this->m_pMat[valP][valP-1] >= Type(0))
			SignP1P = 1;
		Type valW = SignP1P *
					sqrt(this->m_pMat[valP][valP-1] * this->m_pMat[valP][valP-1] +
						 this->m_pMat[valQ][valP-1] * this->m_pMat[valQ][valP-1]);
		valCos =   this->m_pMat[valP][valP-1] / valW;
		valSin = - this->m_pMat[valQ][valP-1] / valW;
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
// Determine if the matrix is singular
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsSingular ()
{
	return ( CalcDeterminant() == Type(0) );
}
/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is unimodular
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsUniModular ()
{
	return (VistaMathTools<Type>::IsZero( CalcDeterminant() - Type(1) ) );
}

/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is diagonal
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsDiagonal ()
{
	for (int i=0; i < VistaMatrix<Type>::m_nRowDimension; ++i)
		for (int j=0; j < VistaMatrix<Type>::m_nColDimension; ++j)
			if (i != j &&
				//VistaMatrix<Type>::m_pMat[i][j] != 0)
				!VistaMathTools<Type>::IsZero (VistaMatrix<Type>::m_pMat[i][j]))
				return false;
	return true;
}

/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is scalar
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsScalar ()
{
	if (!IsDiagonal())
		return false;
	Type value = VistaMatrix<Type>::m_pMat[0][0];
	for (int i=1; i < VistaMatrix<Type>::m_nRowDimension; ++i)
	{
		//if (VistaMatrix<Type>::m_pMat[i][i] != value)
		if (!VistaMathTools<Type>::IsZero (VistaMatrix<Type>::m_pMat[i][i] - value))
			return false;
	}
	return true;
}

/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is a unit matrix
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsUnit ()
{
  //    if (IsScalar() &&
		//VistaMatrix<Type>::m_pMat[0][0] == 1)
  //      VistaMathTools<Type>::IsZero (VistaMatrix<Type>::m_pMat[i][i] - 1.0))
  //          return true;
  //  return false;

	if ( !IsScalar() )
		return false ;

		//this->m_pMat[0][0] == 1)
	for( int i = 0 ; i < this->m_nRowDimension ; i++ )
	{
		if( !VistaMathTools<Type>::IsZero (this->m_pMat[i][i] - 1.0))
			return false;
	}

	return true;
}

/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is symmetric
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsSymmetric ()
{
	for (int i=0; i < VistaMatrix<Type>::m_nRowDimension; ++i)
		for (int j=0; j < VistaMatrix<Type>::m_nColDimension; ++j)
			//if (VistaMatrix<Type>::m_pMat[i][j] != VistaMatrix<Type>::m_pMat[j][i])
			if (!VistaMathTools<Type>::IsZero (
				  VistaMatrix<Type>::m_pMat[i][j] - VistaMatrix<Type>::m_pMat[j][i]))
				return false;
   return true;
}

/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is skew-symmetric
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsSkewSymmetric ()
{
	for (int i=0; i < VistaMatrix<Type>::m_nRowDimension; ++i)
		for (int j=0; j < VistaMatrix<Type>::m_nColDimension; ++j)
			//if (VistaMatrix<Type>::m_pMat[i][j] != -VistaMatrix<Type>::m_pMat[j][i])
			if (!VistaMathTools<Type>::IsZero (
				  VistaMatrix<Type>::m_pMat[i][j] + VistaMatrix<Type>::m_pMat[j][i]))
				return false;
	return true;
}

/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is upper triangular
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsUpperTriangular ()
{
	for (int i=1; i < VistaMatrix<Type>::m_nRowDimension; ++i)
		for (int j=0; j < i-1; ++j)
			if (!VistaMathTools<Type>::IsZeroSetZero (VistaMatrix<Type>::m_pMat[i][j]))
				return false;
	return true;
}

/*============================================================================*/
/*============================================================================*/
// Determine if the matrix is lower triangular
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::IsLowerTriangular ()
{
	for (int j=1; j < VistaMatrix<Type>::m_pColDimension; ++j)
		for (int i=0; i < j-1; ++i)
			if (!VistaMathTools<Type>::IsZeroSetZero (VistaMatrix<Type>::m_pMat[i][j]))
				return false;
   return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaSquareMatrix<Type,dim>::Debug ()
{
	return VistaMatrix<Type>::Debug();
}


#endif
