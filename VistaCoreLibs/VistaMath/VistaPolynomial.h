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


/* dont forget that these deal with floats! */

#ifndef _VISTAPOLYNOMIAL_H
#define _VISTAPOLYNOMIAL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaMathConfig.h"
#include "VistaMathTools.h"

#include <VistaBase/VistaStreamUtils.h>

#include <limits>
#include <iostream>
#include <cmath>     // sin, cos, sqrt ...
#include <complex>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                        */
/*============================================================================*/
//########################
//#define fuzz_factor 1.0e-7
//#define fuzz_factor std::numeric_limits<float>::epsilon()
#define fuzz_factor std::numeric_limits<double>::epsilon()
//########################

#define Zero_test1(result,term1) \
  if (std::abs(result) <= fuzz_factor * std::abs(term1)) \
	result = 0.0

#define Zero_test2(result,term1,term2) \
  if (std::abs(result) <= fuzz_factor * (std::abs(term1) + std::abs(term2))) \
	result = 0.0

#define Zero_test3(result,term1,term2,term3) \
  if (std::abs(result) <= fuzz_factor * (std::abs(term1) + std::abs(term2) + std::abs(term3))) \
	result = 0.0

#define Zero_test4(result,term1,term2,term3, term4) \
  if (std::abs(result) <= fuzz_factor * (std::abs(term1) + std::abs(term2) + std::abs(term3) + std::abs(term4))) \
	result = 0.0

#define Zero_test6(result,term1,term2,term3, term4, term5, term6) \
  if (std::abs(result) <= fuzz_factor * (std::abs(term1) + std::abs(term2) + std::abs(term3) + std::abs(term4) + std::abs(term5) + std::abs(term6))) \
	result = 0.0

double VISTAMATHAPI cube_root ( double x );
int VISTAMATHAPI solve_cubic ( double a, double b, double c, double d,
			  double* x1, double* x2, double* x3 );
int VISTAMATHAPI solve_quadratic ( double a, double b, double c, double* x1, double* x2 );
int VISTAMATHAPI solve_linear ( double a, double b, double* x );


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
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* VistaPolynomial                                                           */
/*============================================================================*/
/**
 * VistaPolynomial
 * Pn(x)=Sum(k=0,n)(ak*x^k)=an*x^n+a(n-1)*x^(n-1)+...+a2*x^2+a1*x+a0
 */
template <class Type, int dim>
class VistaPolynomial
{
public:
	VistaPolynomial ();
	~VistaPolynomial (){};

	inline Type GetVal (int coeffNum)
		{return m_pCoeff[coeffNum];};
	inline bool SetVal (int coeffNum, const Type & newVal)
		{m_pCoeff[coeffNum] = newVal; return true;};
	inline int GetDegree ();

	// (Nullstellen, Wurzeln), returns number of found roots
	inline int FindRoots (std::vector<std::complex<double>*> & vecRoots);

	inline bool Debug();

protected:
	inline bool FindLinearRoot  (std::complex<double> & theRoot);
	inline bool FindSquareRoots (std::complex<double> & firstRoot,
						  std::complex<double> & secRoot);
	inline bool FindCubicRoots  (std::complex<double> & firstRoot,
						  std::complex<double> & secRoot,
						  std::complex<double> & thirdRoot);
	//bool FindNumericRoots (std::complex<double> & secRoot)
	inline bool FindNumericRoots (void* pRoot)
		{return 0;}

protected:
	int    m_nLength;
	Type   m_pCoeff[dim];
};

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
VistaPolynomial<Type,dim>::VistaPolynomial ()
:m_nLength(dim)
{
	m_pCoeff[dim-1]=1;
	for (int coeff = m_nLength -2; coeff >= 0; --coeff)
		m_pCoeff[coeff]=0;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
int VistaPolynomial<Type,dim>::GetDegree ()
{
	if (m_nLength < 0)
		return -1;

	for (int grad = m_nLength; grad > 0; --grad)
	{
		if (m_pCoeff[grad-1] != 0)
			return grad;
	}
	return 0;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
int VistaPolynomial<Type,dim>::FindRoots (std::vector<std::complex<double>*> & vecRoots)
{
	int grad = GetDegree();
	switch (grad)
	{
		case 0: // 0=0
			return 0;
			break;
		case 1: // a[0]=0, a[0]!=0
			return -1;
			break;
		case 2: // a[1]x+a[0]=0
			if (!FindLinearRoot(*vecRoots[0]))
				return -1;
			return 1;
			break;
		case 3: // a[2]x^2+a[1]x+a[0]=0
			if (!FindSquareRoots(*vecRoots[0],*vecRoots[1]))
				return -1;
			return 2;
			break;
		case 4: // a[3]x^3+a[2]x^2+a[1]x+a[0]=0
			if (!FindCubicRoots(*vecRoots[0],*vecRoots[1],*vecRoots[2]))
				return -1;
			return 3;
			break;
		default:
			/** @todo !!implement a general numeric algorithm!! */
			break;
	}
	return -1;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   FindLinearRoot                                              */
/*                                                                            */
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaPolynomial<Type,dim>::FindLinearRoot (std::complex<double> & linRoot)
{
	// a[1]x + a[0] = 0  ==>  x = -a[0]/a[1]
	if (m_pCoeff[1] == 0)
		return false;

	linRoot = std::complex<double>(-m_pCoeff[0]/m_pCoeff[1],0);
	VistaMathTools< std::complex<double> >::IsZeroSetZero (linRoot);
	return true;
}


/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaPolynomial<Type,dim>::FindSquareRoots (
	std::complex<double> & firstRoot, std::complex<double> & secRoot)
{
	// a[2]x^2  + a[1]x + a[0] = 0  ==>
	// x1,2 = -p/2 +- sqrt(p^2/4 - q),  p=a[1]/a[2], q=a[0]/a[2]
	//      = -a[1]/(2a[2]) +- sqrt(a[1]^2/(4a[2]^2) - 4a[0]a[2]/(4a[2]^2))
	//      = (-a[1] +- sqrt(a[1]^2 - 4a[0]a[2]) / 2a[2]
	if (m_pCoeff[2] == 0)
		return false;

	// compute discriminant: a[1]*a[1]-4*a[0]*a[2]
	double dis = m_pCoeff[1]*m_pCoeff[1] - 4.0*m_pCoeff[2]*m_pCoeff[0];
	Zero_test2 (dis, m_pCoeff[1]*m_pCoeff[1], 4.0*m_pCoeff[2]*m_pCoeff[0]);

	// only one root (multiplicity=2)
	if (dis == 0)
	{
		firstRoot = std::complex<double> (-m_pCoeff[1]/(2.0*m_pCoeff[2]),0);
		secRoot = std::complex<double>(firstRoot.real(),0);
	}

	// two real roots
	if (dis > 0)
	{
		dis = sqrt(dis);
		double sol = (-m_pCoeff[1] + dis) / (2.0 * m_pCoeff[2]);
		Zero_test2 (sol, -m_pCoeff[1]*m_pCoeff[2]/2.0, dis*m_pCoeff[2]/2.0);
		firstRoot = std::complex<double>(sol,0);
		sol = (-m_pCoeff[1] - dis) / (2 * m_pCoeff[2]);
		Zero_test2 (sol, -m_pCoeff[1]*m_pCoeff[2]/2.0, dis*m_pCoeff[2]/2.0);
		secRoot = std::complex<double>(sol,0);
	}

	// two conjugate complex roots
	if (dis < 0)
	{
		dis = sqrt(-dis);
		firstRoot = std::complex<double>(-m_pCoeff[1] / (2.0 * m_pCoeff[2]),
										 dis / (2.0 * m_pCoeff[2]) );
		secRoot = std::complex<double>(firstRoot.real(), -firstRoot.imag());
	}

	VistaMathTools< std::complex<double> >::IsZeroSetZero (firstRoot);
	VistaMathTools< std::complex<double> >::IsZeroSetZero (secRoot);

	return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaPolynomial<Type,dim>::FindCubicRoots (std::complex<double> & firstRoot,
			  std::complex<double> & secRoot, std::complex<double> & thirdRoot)
{
	// a[3]x^3 + a[2]x^2 + a[1]x + a[0] = 0
	if (m_pCoeff[3] == 0)
		return false;

	double x1[3], x2[3], x3[3];
	// search roots
	int resType = solve_cubic (m_pCoeff[3], m_pCoeff[2],
		m_pCoeff[1], m_pCoeff[0], x1, x2, x3);

	// cubic types: -3, 1, 2, 3
	switch (resType)
	{
	case -3: // one real root and one complex conjugate pair.
		// real root in *x1, real part of complex conjugate root in *x2
		// imaginary part in *x3.
		firstRoot = std::complex<double>(*x1,  0);
		secRoot   = std::complex<double>(*x2, *x3);
		thirdRoot = std::complex<double>(*x2,-*x3);
		break;
	case -2: // only two distinct roots (complex conjugates)
		// real part returned in *x1, imaginary part returned in *x2
		return false;
		break;
	case -1: // infinite number of solutions
		return false;
		break;
	case 0: // no solution
		return false;
		break;
	case 1: // only one distinct root, real, possibly of multiplicity 2 or 3
		// returned in *x1
		firstRoot = std::complex<double>(*x1,0);
		secRoot   = std::complex<double>(*x1,0);
		thirdRoot = std::complex<double>(*x1,0);
		break;
	case 2: // only two distinct roots, both real, one possibly of multiplicity 2,
		// returned in *x1 and *x2
		firstRoot = std::complex<double>(*x1,0);
		secRoot   = std::complex<double>(*x2,0);
		thirdRoot = std::complex<double>(*x2,0);
		break;
	case 3: // three distinct real roots of multiplicity 1
		// returned in *x1, *x2, and *x3
		firstRoot = std::complex<double>(*x1,0);
		secRoot   = std::complex<double>(*x2,0);
		thirdRoot = std::complex<double>(*x3,0);
		break;
	}

	VistaMathTools< std::complex<double> >::IsZeroSetZero (firstRoot);
	VistaMathTools< std::complex<double> >::IsZeroSetZero (secRoot);
	VistaMathTools< std::complex<double> >::IsZeroSetZero (thirdRoot);

	return true;
}

/*============================================================================*/
/*============================================================================*/
template <class Type, int dim>
inline
bool VistaPolynomial<Type,dim>::Debug ()
{
	int grad = GetDegree();
	vstr::out() << " P(x) = ";
	for (int pos = grad -1; pos >= 0; --pos)
	{
		vstr::out() << " + " << m_pCoeff[pos] << " x^" << pos;
	}
	vstr::out() << std::endl;
	return true;
}


/*============================================================================*/
/*============================================================================*/


#endif // _VISTAPOLYNOMIAL_H
