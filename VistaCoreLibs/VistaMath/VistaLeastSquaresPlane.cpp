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

#include "VistaLeastSquaresPlane.h"

#include "VistaSquareMatrix.h"
#include <VistaBase/VistaVectorMath.h>
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
/*============================================================================*/
/*                                                                            */
/*  NAME      :   ComputeLeastSquaresPlane                                    */
/*                                                                            */
/*============================================================================*/
void VistaLeastSquaresPlane::ComputeLeastSquaresPlane(const vector<VistaVector3D>& vecPoints,
    VistaVector3D& v3PlaneCenter, VistaVector3D& v3PlaneNormal) {
  unsigned int  i = 0, j = 0;
  VistaVector3D v3Offset;
  /*
   * compute median (COG) of all points
   */
  v3PlaneCenter[0] = v3PlaneCenter[1] = v3PlaneCenter[2] = 0;
  for (; i < vecPoints.size(); ++i)
    v3PlaneCenter += vecPoints[i];
  v3PlaneCenter = (1.0f / (float)vecPoints.size()) * v3PlaneCenter;
  /*
   * compute entries for symmetric matrix
   */
  VistaSquareMatrix<double, 3> m3SymMatrix;
  m3SymMatrix.SetNull();
  for (i = 0; i < vecPoints.size(); ++i) {
    // compute offset vector from median
    v3Offset = vecPoints[i] - v3PlaneCenter;
    // compute entries
    m3SymMatrix[0][0] += (double)v3Offset[0] * (double)v3Offset[0];
    m3SymMatrix[0][1] += (double)v3Offset[0] * (double)v3Offset[1];
    m3SymMatrix[0][2] += (double)v3Offset[0] * (double)v3Offset[2];
    m3SymMatrix[1][1] += (double)v3Offset[1] * (double)v3Offset[1];
    m3SymMatrix[1][2] += (double)v3Offset[1] * (double)v3Offset[2];
    m3SymMatrix[2][2] += (double)v3Offset[2] * (double)v3Offset[2];
  }
  // do the "symmetry stuff"
  for (i = 0; i < 3; ++i)
    for (j = i + 1; j < 3; ++j)
      m3SymMatrix[j][i] = m3SymMatrix[i][j];

  ComputePlaneNormal(m3SymMatrix, v3PlaneNormal);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ComputeLeastSquaresPlane                                    */
/*                                                                            */
/*============================================================================*/
void VistaLeastSquaresPlane::ComputeLeastSquaresPlane(
    const std::vector<double*>& vecPoints, double nPlaneCenter[3], double nPlaneNormal[3]) {
  unsigned int i = 0, j = 0;
  /*
   * compute median (COG) of all points
   */
  nPlaneCenter[0] = nPlaneCenter[1] = nPlaneCenter[2] = 0.0;
  for (; i < vecPoints.size(); ++i) {
    for (j = 0; j < 3; ++j) {
      nPlaneCenter[j] += vecPoints[i][j];
    }
  }
  double fOneDiv = 1.0 / (double)vecPoints.size();
  for (i = 0; i < 3; ++i)
    nPlaneCenter[i] *= fOneDiv;
  /*
   * compute entries for symmetric matrix
   */
  double                       fOffset[3];
  VistaSquareMatrix<double, 3> m3SymMatrix;
  m3SymMatrix.SetNull();
  for (i = 0; i < vecPoints.size(); ++i) {
    // compute offset vector from median
    for (j = 0; j < 3; ++j)
      fOffset[j] = vecPoints[i][j] - nPlaneCenter[j];
    // compute entries
    m3SymMatrix[0][0] += fOffset[0] * fOffset[0];
    m3SymMatrix[0][1] += fOffset[0] * fOffset[1];
    m3SymMatrix[0][2] += fOffset[0] * fOffset[2];
    m3SymMatrix[1][1] += fOffset[1] * fOffset[1];
    m3SymMatrix[1][2] += fOffset[1] * fOffset[2];
    m3SymMatrix[2][2] += fOffset[2] * fOffset[2];
  }
  // do the "symmetry stuff"
  for (i = 0; i < 3; ++i) {
    for (j = i + 1; j < 3; ++j) {
      m3SymMatrix[j][i] = m3SymMatrix[i][j];
    }
  }
  /*
   * Compute the plane normal
   */
  VistaVector3D v3PlaneNormal;
  ComputePlaneNormal(m3SymMatrix, v3PlaneNormal);
  float fPlaneNormal[3];
  v3PlaneNormal.GetValues(fPlaneNormal);
  nPlaneNormal[0] = fPlaneNormal[0];
  nPlaneNormal[1] = fPlaneNormal[1];
  nPlaneNormal[2] = fPlaneNormal[2];
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ComputeLeastSquaresPlane                                    */
/*                                                                            */
/*============================================================================*/
void VistaLeastSquaresPlane::ComputeLeastSquaresPlane(
    const std::vector<float*>& vecPoints, float fPlaneCenter[3], float fPlaneNormal[3]) {
  unsigned int i = 0, j = 0;
  /*
   * compute median (COG) of all points
   */
  fPlaneCenter[0] = fPlaneCenter[1] = fPlaneCenter[2] = 0.0f;
  for (; i < vecPoints.size(); ++i) {
    for (j = 0; j < 3; ++j) {
      fPlaneCenter[j] += vecPoints[i][j];
    }
  }
  float fOneDiv = (1.0f / (float)vecPoints.size());
  for (i = 0; i < 3; ++i)
    fPlaneCenter[i] *= fOneDiv;
  /*
   * compute entries for symmetric matrix
   */
  double                       fOffset[3];
  VistaSquareMatrix<double, 3> m3SymMatrix;
  m3SymMatrix.SetNull();
  for (i = 0; i < vecPoints.size(); ++i) {
    // compute offset vector from median
    for (j = 0; j < 3; ++j)
      fOffset[j] = (double)vecPoints[i][j] - (double)fPlaneCenter[j];
    // compute entries
    m3SymMatrix[0][0] += fOffset[0] * fOffset[0];
    m3SymMatrix[0][1] += fOffset[0] * fOffset[1];
    m3SymMatrix[0][2] += fOffset[0] * fOffset[2];
    m3SymMatrix[1][1] += fOffset[1] * fOffset[1];
    m3SymMatrix[1][2] += fOffset[1] * fOffset[2];
    m3SymMatrix[2][2] += fOffset[2] * fOffset[2];
  }
  // do the "symmetry stuff"
  for (i = 0; i < 3; ++i) {
    for (j = i + 1; j < 3; ++j) {
      m3SymMatrix[j][i] = m3SymMatrix[i][j];
    }
  }
  /*
   * Compute the plane normal
   */
  VistaVector3D v3PlaneNormal;
  ComputePlaneNormal(m3SymMatrix, v3PlaneNormal);
  v3PlaneNormal.GetValues(fPlaneNormal);
}
/*============================================================================*/
/*                                                                            */
/*  NAME      :   ComputePlaneNormal                                          */
/*                                                                            */
/*============================================================================*/
void VistaLeastSquaresPlane::ComputePlaneNormal(
    VistaSquareMatrix<double, 3>& rMatrix, VistaVector3D& v3PlaneNormal) {
  /*
   * compute eigensystem of matrix
   */
  vector<complex<double>*> vecEigenVals = vector<complex<double>*>(3);
  vector<complex<double>*> vecEigenVecs = vector<complex<double>*>(3);
  int                      i            = 0;
  for (; i < 3; ++i) {
    vecEigenVals[i] = new complex<double>;
    vecEigenVecs[i] = new complex<double>[3];
  }
  rMatrix.CalcEigenSpace(vecEigenVals, vecEigenVecs);

  /*
   * compute smallest EigenValue
   */
  int iSmallestId = 0;
  if (vecEigenVals[1]->real() < vecEigenVals[iSmallestId]->real())
    iSmallestId = 1;
  if (vecEigenVals[2]->real() < vecEigenVals[iSmallestId]->real())
    iSmallestId = 2;
  /*
   * set normal to corresponding EigenVector
   */
  v3PlaneNormal[0] = (float)vecEigenVecs[iSmallestId][0].real();
  v3PlaneNormal[1] = (float)vecEigenVecs[iSmallestId][1].real();
  v3PlaneNormal[2] = (float)vecEigenVecs[iSmallestId][2].real();
  v3PlaneNormal.Normalize();
  /*
   * cleanup math stuff
   */
  for (i = 0; i < 3; ++i) {
    delete vecEigenVals[i];
    delete[] vecEigenVecs[i];
  }
}
