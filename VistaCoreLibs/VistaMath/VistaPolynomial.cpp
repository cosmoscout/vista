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


#include "VistaPolynomial.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   cube_root                                                   */
/*                                                                            */
/*============================================================================*/
double cube_root ( double x )
{
  if (x > 0.0)
	return (pow (x, 1.0/3.0));
  else if (x < 0.0)
	return (-pow (-x, 1.0/3.0));
  else
	return 0.0;
}

/*============================================================================*/
/*============================================================================*/
int solve_cubic ( double a, double b, double c, double d,
			  double* x1, double* x2, double* x3 )
{
  /* 
   *         3     2
   * Solve ax  + bx  + cx + d = 0
   *
   * Return values: 0 - no solution 
   *               -1 - infinite number of solutions
   *                1 - only one distinct root, real, possibly of multiplicity 2 or 3.
   *                    returned in *x1
   *		    2 - only two distinct roots, both real, one possibly of multiplicity 2,
   *                    returned in *x1 and *x2
   *		   -2 - only two distinct roots (complex conjugates),
   *		        real part returned in *x1,
   *			imaginary part returned in *x2
   *                3 - three distinct real roots of multiplicity 1
   *                    returned in *x1, *x2, and *x3.
   *               -3 - one real root and one complex conjugate pair.
   *                    real root in *x1, real part of complex conjugate root
   *                    in *x2, imaginary part in *x3.
   *
   *                XXX - this whole scheme is wrong.  
   *                      Each root should be returned along with its multiplicity.
   */

	*x1 = *x2 = *x3 = 0.0;

	// it it merely a quadratic equation
	if (a == 0.0)
		return (solve_quadratic (b, c, d, x1, x2));

	{	
		// calculate normal form of cubic equation by dividing through a
		// ax^3 + bx^2 + cx + d = 0   ==>   y^3 + py^2 + qy + r = 0
		double p, q, r;
		p = b/a;			
		q = c/a;
		r = d/a;

		// calculate the reduced equation (y = z - p/3)
		// z^3 + a2*z + b2 = 0
		//    a2 = (3q - p^2)/3
		//    b2 = 2p^3/27 - pq/3 + r
		double z1, z2, z3;
		double a2, b2, des;
		a2 = (3.0*q - p*p)/3.0;
		Zero_test2 (a2, 3.0*q, p*p);

		b2 = (2.0*p*p*p - 9.0*p*q + 27.0*r)/27.0;
		Zero_test3 (b2, 2.0*p*p*p, 9.0*p*q, 27.0*r);

		// the descriminant: des = (a2/3)^3 + (b2/2)^2
		des = (b2*b2/4.0 + a2*a2*a2/27.0);
		Zero_test2 (des, b2*b2/4.0, a2*a2*a2/27.0);

		// solving a cubic reduced equation the cardano's formula can be used
		// a3 = root[3](-b2/2 + sqrt(des))
		// b3 = root[3](-b2/2 - sqrt(des))
		// z1 = a3 + b3
		// z2 = - (a3 + b3)/2 + sqrt(3)*i((a3 - b3)/2)
		// z3 = - (a3 + b3)/2 - sqrt(3)*i((a3 - b3)/2)
		/* three real roots, at least two equal */ 
		if (des == 0.0)
		{
			// a3 == b3
			double a3 = cube_root (-b2/2.0);
			// one distinct real root of multiplicity three
			if (a3 == 0.0)
			{
				z1 = 0.0;
				// now, convert back to the normal form
				*x1 = z1 - p/3.0;
				Zero_test2 (*x1, z1, p/3);
				// reset the rest
				*x2 = 0.0;
				*x3 = 0.0;
				return (1);
			}
			// one real root of multiplicity one, another of multiplicity two
			else
			{
				z1 = 2*a3;
				// z2 == z3
				z2 = -a3;
				// now, convert back to the normal form
				*x1 = z1 - p/3.0;
				Zero_test2 (*x1, z1, p/3.0);
				*x2 = z2 - p/3.0;
				Zero_test2 (*x2, z2, p/3.0);
				// reset the rest
				*x3 = 0.0;
				return (2);
			}
		}
		/* one real root, one complex conjugate pair */
		else if (des > 0.0)
		{
			double d2 = sqrt(des);
			double t1 = -b2/2.0 + d2;
			double t2 = -b2/2.0 - d2;
			Zero_test2 (t1, b2/2, d2);
			Zero_test2 (t2, b2/2, d2);
			double a3;
			double b3;
			a3 = cube_root (t1);
			b3 = cube_root (t2);
			z1 = a3 + b3;
			Zero_test2 (z1, a3, b3);
			// z2 = - (a3 + b3)/2 + sqrt(3)*i((a3 - b3)/2)
			// z3 = - (a3 + b3)/2 - sqrt(3)*i((a3 - b3)/2)
			// z2 hold the real part only, for both values
			z2 = - z1/2.0;
			t1 = a3-b3;
			Zero_test2 (t1, a3, b3);
			// z3 is the complex value only, and it is used twice:
			// conjugated for the second complex value
			z3 = sqrt(3.0) * t1/2.0;
			// now, convert back to the normal form
			*x1 = z1 - p/3.0;
			Zero_test2 (*x1, z1, p/3.0);
			*x2 = z2 - p/3;
			Zero_test2 (*x2, z2, p/3.0);
			*x3 = z3;
			return (-3);
		}
		/* three unequal real roots */
		else if (des < 0.0)
		{
			double temp_r, theta, cos_term, sin_term, t1;

			// this seems to be similar to the trigo solution for des>0, a2<0
			// ==> z1 = - 2 r cosh(theta/3)
			//     z2 =     r cosh(theta/3) + i sqrt(3) r sinh (theta/3)
			//     z3 =     r cosh(theta/3) - i sqrt(3) r sinh (theta/3)
			//        r = sgn(b2) sqrt(abs(a2)/3)
			//        cosh(theta) = b2 / (2 r^3)
			// BUT DESCRIMINANT < 0 !!! => ??????
			t1 = b2*b2/4.0 - des;
			Zero_test2 (t1, b2*b2/4.0, des);
			temp_r = cube_root (sqrt (b2*b2/4.0 - des)); // should it not be t1, here???
			// theta = atan (sqrt(-des)/(-b2/2))
			// atan2(y,x) is well defined for every point other than the origin, even if x equals 0 
			theta = atan2 (sqrt(-des), (-b2/2.0));
			cos_term = temp_r * cos (theta/3.0);
			sin_term = temp_r * sin (theta/3.0) * sqrt(3.0);
			z1 = 2.0 * cos_term;
			z2 = - cos_term - sin_term;
			Zero_test2 (z2, cos_term, sin_term);
			z3 = - cos_term + sin_term;
			Zero_test2 (z3, cos_term, sin_term);

			// now, convert back to the normal form
			// z1,z2,z3 contain the real parts, only
			*x1 = z1 - p/3.0;
			Zero_test2 (*x1, z1, p/3.0);
			*x2 = z2 - p/3;
			Zero_test2 (*x2, z2, p/3.0);
			*x3 = z3 - p/3;
			Zero_test2 (*x3, z3, p/3.0);
			return (3);
		}
		else
		/* cannot happen */
		{
			//fprintf (stderr, "impossible descriminant in solve_cubic\n");
			return (0);
		}
	}
}

/*============================================================================*/
/*============================================================================*/
int solve_quadratic ( double a, double b, double c, double* x1, double* x2 )
{
  /* 
   *         2
   * Solve ax  + bx + c = 0
   *
   * Return values: 0 - no solution 
   *               -1 - infinite number of solutions
   *                1 - one real root, possibly of multiplicity 2.
   *                    returned in *x1
   *		    2 - two distinct real roots of mutiplicity 1
   *                    returned in *x1 and *x2;
   *		   -2 - two complex roots (conjugates),
   *		        real part returned in *x1,
   *			imaginary part returned in *x2
   */

  double d;

  if (a == 0)
	return(solve_linear (b, c, x1));

  d = b*b - 4.0*a*c;
  Zero_test2 (d, b*b, 4.0*a*c);

  if (d == 0) 
	{
	  *x1 = *x2 = -b/(2.0*a);
	  return (1);
	}

  if (d > 0)
	{
	  d = sqrt(d);
	  *x1 = (-b + d)/(2.0*a);
	  Zero_test2 (*x1, -b/2.0*a, d/2.0*a);
	  *x2 = (-b - d)/(2.0*a);
	  Zero_test2 (*x2, -b/2.0*a, -d/2.0*a);
	  return (2);
	}

  if (d < 0)
	{
	  d = sqrt(-d);
	  *x1 = -b/(2.0*a);
	  *x2 =  d/(2.0*a);
	  return (-2);
	}
  return 0;
}

/*============================================================================*/
/*============================================================================*/
int solve_linear ( double a, double b, double* x )
{
  /* 
   * Solve ax + b = 0
   *
   * Return values: 0 - no solution 
   *                1 - one real solution, returned in *x
   *               -1 - infinite number of solutions
   */

  if (a == 0) 
	{
	  if (b == 0)
	{
	  *x = 0;
	  return (-1);
	}
	  else
	return (0);
	}
  else
	{
	  *x = -b/a;
	  return (1);
	}
}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/


