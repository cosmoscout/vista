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


#ifndef _VISTALEASTSUQARESPLANE_H
#define _VISTALEASTSUQARESPLANE_H
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <vector>
#include "VistaMathConfig.h"

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaVector3D;
template <class Type, int dim> class VistaSquareMatrix;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
* Compute the least squares fitting plane for a given set of points
*/
class VISTAMATHAPI VistaLeastSquaresPlane
{
public:
	/**
	* Compute the least squares fitting plane through the given set of points
	*
	* @param setPoints     set of input points for which to compute the plane
	* @param fPlaneCenter  the point set COG
	* @param fPlaneNormal  the normal of the least squares plane through the point set
	*/
	static void ComputeLeastSquaresPlane(   const std::vector<VistaVector3D>& vecPoints,
											VistaVector3D& v3PlaneCenter,
											VistaVector3D& v3PlaneNormal);
	/**
	* same show as above but with other data types...
	*/
	static void ComputeLeastSquaresPlane(   const std::vector<double*> &vecPoints,
											double nPlaneCenter[3],
											double nPlaneNormal[3]);
	static void ComputeLeastSquaresPlane(   const std::vector<float*> &vecPoints,
											float fPlaneCenter[3],
											float fPlaneNormal[3]);

private:
	/**
	* here the trick is done: Compute the eigensystem of the covariance matrix
	*/
	static void ComputePlaneNormal( VistaSquareMatrix<double,3>& rMatrix,
									VistaVector3D& v3PlaneNormal);

};
#endif /* ifndef _VISTALEASTSUQARESPLANE_H */

