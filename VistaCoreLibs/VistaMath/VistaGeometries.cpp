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

#include "VistaVector.h"
#include <VistaBase/VistaUtilityMacros.h>
#include <VistaBase/VistaVectorMath.h>

#include "VistaGeometries.h"

bool VistaRay::IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b,
    const VistaVector3D& c, VistaVector3D& contactPoint, const float epsilon) const {
  // The implementation is based on the algorithm from the book
  // Real-Time Rendering, M?ller & Haines, p. 303-305.
  VistaVector3D d = m_Dir;
  d.Normalize();
  const VistaVector3D e2 = c - a;
  const VistaVector3D p  = d.Cross(e2);

  const VistaVector3D e1 = b - a;
  const float         g  = e1 * p;
  if (std::abs(g) < epsilon) {
    return (false);
  }

  const VistaVector3D s = m_Origin - a;
  const float         f = 1.0f / g;
  const float         u = f * (s * p);

  // Check the intersect point lies inside the
  // triangle.
  if ((u < -epsilon) || (u > 1 + epsilon)) {
    return (false);
  }
  const VistaVector3D q = s.Cross(e1);
  const float         v = f * (d * q);
  if ((v < -epsilon) || (u + v > 1 + epsilon)) {
    return (false);
  }

  // Compute the penetration depth.
  const float t = f * (e2 * q);
  if (t < -epsilon) {
    return (false);
  }

  // Passed all tests, line intersects triangle.
  // Update return parameters.
  contactPoint = t * d + m_Origin;

  return true;
}

bool VistaRay::IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b,
    const VistaVector3D& c, VistaVector3D& contactPoint, VistaVector3D& faceNormal,
    float& penetrationDepth, float& barycentric_ab, float& barycentric_ac,
    const float epsilon) const {
  // The implementation is based on the algorithm from the book
  // Real-Time Rendering, M?ller & Haines, p. 303-305.
  VistaVector3D d = m_Dir;
  d.Normalize();
  const VistaVector3D e2 = c - a;
  const VistaVector3D p  = d.Cross(e2);

  const VistaVector3D e1 = b - a;
  const float         g  = e1 * p;
  if (std::abs(g) < epsilon) {
    return (false);
  }

  const VistaVector3D s = m_Origin - a;
  const float         f = 1.0f / g;
  const float         u = f * (s * p);

  // Check the intersect point lies inside the
  // triangle.
  if ((u < -epsilon) || (u > 1.0f + epsilon)) {
    return (false);
  }
  const VistaVector3D q = s.Cross(e1);
  const float         v = f * (d * q);
  if ((v < -epsilon) || (u + v > 1.0f + epsilon)) {
    return (false);
  }

  // Compute the penetration depth.
  const float t = f * (e2 * q);
  if (t < -epsilon) {
    return (false);
  }

  // Passed all tests, line intersects triangle.
  // Update return parameters.
  barycentric_ab   = u;
  barycentric_ac   = v;
  penetrationDepth = t;
  faceNormal       = e1.Cross(e2);
  faceNormal.Normalize();
  contactPoint = t * d + m_Origin;
  return true;
}

/* ########################################################################## */
/* ##  VISTA  LINE SEGMENT  ################################################# */
/* ########################################################################## */

void VistaLineSegment::operator=(const VistaRay& newLn) {
  VistaVector3D orgPnt(newLn.GetOrigin());
  VistaVector3D dirVec(newLn.GetDir());
  SetStartPoint(orgPnt);
  //    SetDir      ( dirVec );
  // now this line segment is defined using the
  // length of the direction vector to get the end point
  m_EndPoint[Vista::X] = orgPnt[Vista::X] + dirVec[Vista::X];
  m_EndPoint[Vista::Y] = orgPnt[Vista::Y] + dirVec[Vista::Y];
  m_EndPoint[Vista::Z] = orgPnt[Vista::Z] + dirVec[Vista::Z];
}

float VistaLineSegment::GetParameterOfNearestPoint(const VistaVector3D& testPnt) const {
  // calc vector to currently given world point
  // which starts from the origin
  VistaVector3D vecToSnapPnt(testPnt[Vista::X], testPnt[Vista::Y], testPnt[Vista::Z]);
  vecToSnapPnt = vecToSnapPnt - GetStartPoint();

  // calculate the vector from origin to end point
  VistaVector3D vecToEndPnt;
  vecToEndPnt = m_EndPoint - GetStartPoint();

  // store length and normalize line segment vec
  float lenLineSeg = vecToEndPnt.GetLength();
  vecToEndPnt.Normalize();

  // use the inner product to calculate the parameter
  float paramT = (vecToEndPnt * vecToSnapPnt) / lenLineSeg;

  return paramT;
}

/*============================================================================*/

bool VistaLineSegment::IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b,
    const VistaVector3D& c, VistaVector3D& contactPoint, const float epsilon) const {
  // The implementation is based on the algorithm from the book
  // Real-Time Rendering, M?ller & Haines, p. 303-305.
  VistaVector3D d = m_EndPoint - m_StartPoint;
  d.Normalize();
  const VistaVector3D e2 = c - a;
  const VistaVector3D p  = d.Cross(e2);

  const VistaVector3D e1 = b - a;
  const float         g  = e1 * p;
  if (std::abs(g) < epsilon) {
    return (false);
  }

  const VistaVector3D s = m_StartPoint - a;
  const float         f = 1.0f / g;
  const float         u = f * (s * p);

  // Check the intersection point lies inside the
  // triangle.
  if ((u < -epsilon) || (u > 1.0f + epsilon)) {
    return (false);
  }
  const VistaVector3D q = s.Cross(e1);
  const float         v = f * (d * q);
  if ((v < -epsilon) || (u + v > 1.0f + epsilon)) {
    return (false);
  }

  // Check penetration depth.
  const float t = f * (e2 * q);
  if ((t < -epsilon) || (t > GetLength() + epsilon)) {
    return (false);
  }

  // Passed all tests, line intersects triangle.
  // Update return parameters.
  contactPoint = t * d + m_StartPoint;

  return true;
}

bool VistaLineSegment::IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b,
    const VistaVector3D& c, VistaVector3D& contactPoint, VistaVector3D& faceNormal,
    float& penetrationDepth, float& barycentric_ab, float& barycentric_ac,
    const float epsilon) const {
  // The implementation is based on the algorithm from the book
  // Real-Time Rendering, M?ller & Haines, p. 303-305.
  VistaVector3D d = m_EndPoint - m_StartPoint;
  d.Normalize();
  const VistaVector3D e2 = c - a;
  const VistaVector3D p  = d.Cross(e2);

  const VistaVector3D e1 = b - a;
  const float         g  = e1 * p;
  if (std::abs(g) < epsilon) {
    return (false);
  }

  const VistaVector3D s = m_StartPoint - a;
  const float         f = 1.0f / g;
  const float         u = f * (s * p);

  // Check the intersection point lies inside the
  // triangle.
  if ((u < -epsilon) || (u > 1.0f + epsilon)) {
    return (false);
  }
  const VistaVector3D q = s.Cross(e1);
  const float         v = f * (d * q);
  if ((v < -epsilon) || (u + v > 1.0f + epsilon)) {
    return (false);
  }

  // Check penetration depth.
  const float t = f * (e2 * q);
  if ((t < -epsilon) || (t > GetLength() + epsilon)) {
    return (false);
  }

  // Passed all tests, line intersects triangle.
  // Update return parameters.
  barycentric_ab   = u;
  barycentric_ac   = v;
  penetrationDepth = t;
  faceNormal       = e1.Cross(e2);
  faceNormal.Normalize();
  contactPoint = t * d + m_StartPoint;
  return true;
}

bool VistaLineSegment::ProximityContains(const VistaLineSegment& segment, const float fProximity,
    float& fBarycentricThisSegment, float& fBarycentricOtherSegment, const float fEpsilon) const {
  // Precompute the threshold as we will use squared distances to save
  // unnecessary sqrt computations by checking actual distances.
  const float fProximityThreshold = (fProximity + fEpsilon) * (fProximity + fEpsilon);

  const float* p1 = &m_StartPoint[0];
  const float* p2 = &m_EndPoint[0];
  const float* p3 = &segment.m_StartPoint[0];
  const float* p4 = &segment.m_EndPoint[0];

  const float p13[3] = {p1[0] - p3[0], p1[1] - p3[1], p1[2] - p3[2]};
  const float p43[3] = {p4[0] - p3[0], p4[1] - p3[1], p4[2] - p3[2]};

  if (std::abs(p43[0]) < fEpsilon && std::abs(p43[1]) < fEpsilon && std::abs(p43[2]) < fEpsilon)
    return false;

  const float p21[3] = {p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]};

  if (std::abs(p21[0]) < fEpsilon && std::abs(p21[1]) < fEpsilon && std::abs(p21[2]) < fEpsilon)
    return false;

  const float d1343 = p13[0] * p43[0] + p13[1] * p43[1] + p13[2] * p43[2];
  const float d4321 = p43[0] * p21[0] + p43[1] * p21[1] + p43[2] * p21[2];
  const float d1321 = p13[0] * p21[0] + p13[1] * p21[1] + p13[2] * p21[2];
  const float d4343 = p43[0] * p43[0] + p43[1] * p43[1] + p43[2] * p43[2];
  const float d2121 = p21[0] * p21[0] + p21[1] * p21[1] + p21[2] * p21[2];

  const float denom = d2121 * d4343 - d4321 * d4321;
  if (std::abs(denom) < fEpsilon)
    return false;

  const float numer = d1343 * d4321 - d1321 * d4343;

  float mua = numer / denom;
  float mub = (d1343 + d4321 * mua) / d4343;

  if (mua > 1.0f)
    mua = 1.0f;
  if (mua < 0.0f)
    mua = 0.0f;
  if (mub > 1.0f)
    mub = 1.0f;
  if (mub < 0.0f)
    mub = 0.0f;

  const float pa[3] = {p1[0] + mua * p21[0], p1[1] + mua * p21[1], p1[2] + mua * p21[2]};
  const float pb[3] = {p3[0] + mub * p43[0], p3[1] + mub * p43[1], p3[2] + mub * p43[2]};

  const float p[3] = {pa[0] - pb[0], pa[1] - pb[1], pa[2] - pb[2]};

  fBarycentricThisSegment  = mua;
  fBarycentricOtherSegment = mub;

  // Finally, check if the estimated clostest points are in the specified proximity to each other.
  return (p[0] * p[0] + p[1] * p[1] + p[2] * p[2]) < fProximityThreshold;
}

/* ########################################################################## */
/* ##  VISTA SPLINE SEGMENT  ################################################ */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTOR
VistaSplineSegment::VistaSplineSegment(VistaVector3D& startPnt, VistaVector3D& endPnt,
    VistaVector3D& startTang, VistaVector3D& endTang, float& startParam, float& endParam,
    int precision)
    : VistaCurveSegment(startPnt, endPnt)
    , m_StartTangent(startTang)
    , m_StartParameter(startParam)
    , m_EndTangent(endTang)
    , m_EndParameter(endParam)
    , m_Precision(precision) {
  /*	int c,i,j,k;
          for(c=0; c<3; c++) // for each coordinate X, Y, Z
          {
                  //Erstellung der Matrix
                  float m[4][4] = {{(float)pow(endParam,3)     , (float)pow(endParam,2)   , endParam
     , 1 },
                                          {(float)3*pow(endParam,2)   , (float)2*endParam        , 1
     , 0},
                                          {(float)3*pow(startParam,2) , (float)2*startParam      , 1
     , 0},
                                          {(float)pow(startParam,3)   , (float)pow(startParam,2) ,
     startParam , 1}};

                  float b[4] = {endPnt[c],endTang[c],startTang[c],startPnt[c]};
                  float val[4];

                  for(j=0; j<3; j++)
                  {
                          if(m[j][j]==0) continue;
                          for(i=j+1; i<4; i++)
                          {
                                  m[i][j] /= m[j][j];
                                  b[i] -=m[i][j]*b[j];
                                  for(k=j+1; k<4; k++) m[i][k]-=m[i][j]*m[j][k];
                          }
                  }

                  val[3] = b[3]/m[3][3] ;
                  val[2] = (b[2]-val[3]*m[2][3])/m[2][2];
                  val[1] = (b[1]-val[3]*m[1][3]-val[2]*m[1][2])/m[1][1] ;
                  val[0] = (b[0]-val[3]*m[0][3]-val[2]*m[0][2]-val[1]*m[0][1])/m[0][0] ;

                  // m_Coefs.SetRow(c,val);
                  for(j=0; j<4; j++)
                          m_Coefs[c][j] = val[j];

          }*/
}
/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
VistaVector3D VistaSplineSegment::GetPointByParameter(const float paramT) const {
  VistaVector3D point;
  // float param = m_StartParameter+(m_EndParameter-m_StartParameter)*paramT;

  // convert parameter to local interval 0,1
  float u = (paramT - m_StartParameter) / (m_EndParameter - m_StartParameter);

  // calculate point's coordinates with hermite base matrix
  float h1 = (2.0f * std::pow(u, 3.0f) - 3.0f * std::pow(u, 2.0f) + 1.0f);
  float h2 = (-2.0f * std::pow(u, 3.0f) + 3.0f * std::pow(u, 2.0f));
  float h3 = (std::pow(u, 3.0f) - 2.0f * std::pow(u, 2.0f) + u);
  float h4 = (std::pow(u, 3.0f) - std::pow(u, 2.0f));

  for (int c = 0; c < 3; c++)
    point[c] =
        h1 * m_StartPoint[c] + h2 * m_EndPoint[c] + h3 * m_StartTangent[c] + h4 * m_EndTangent[c];

  /*	point[Vista::X]=(float)(pow(param,3)*m_Coefs[Vista::X][0] +
     pow(param,2)*m_Coefs[Vista::X][1]+ m_Coefs[Vista::X][2]*param + m_Coefs[Vista::X][3]);
          point[Vista::Y]=(float)(pow(param,3)*m_Coefs[Vista::Y][0] +
     pow(param,2)*m_Coefs[Vista::Y][1]+ m_Coefs[Vista::Y][2]*param + m_Coefs[Vista::Y][3]);
          point[Vista::Z]=(float)(pow(param,3)*m_Coefs[Vista::Z][0] +
     pow(param,2)*m_Coefs[Vista::Z][1]+ m_Coefs[Vista::Z][2]*param + m_Coefs[Vista::Z][3]);*/
  return point;
}

/* ########################################################################## */
/* ##  VISTA  TRIANGLE    ################################################### */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTORS
VistaTriangle::VistaTriangle(const VistaVector3D& a, const VistaVector3D& b, const VistaVector3D& c)
    : m_a(a)
    , m_b(b)
    , m_c(c) {
}

VistaTriangle::~VistaTriangle() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

VistaVector3D VistaTriangle::Normal() const {
  return ((m_b - m_a).Cross(m_c - m_a)).GetNormalized();
}

float VistaTriangle::Area() const {
  return ((m_b - m_a).Cross(m_c - m_a)).GetLength() / 2.0f;
}

bool VistaTriangle::IntersectionPoint(const VistaVector3D& pnt, float& barycentric_ab,
    float& barycentric_ac, const float epsilon) const {
  VistaVector3D x, y, z;
  float         divisor, p, q, r;

  x = m_c - m_a;
  y = m_b - m_a;
  z = pnt - m_a;

  if (x[0] == 0.0f && y[0] == 0.0f) {
    divisor = (x[1] * y[2] - x[2] * y[1]);
    p       = (y[2] * z[1] - y[1] * z[2]) / divisor;
    q       = (x[1] * z[2] - x[2] * z[1]) / divisor;
    r       = z[0];
  } else if (x[1] == 0.0f && y[1] == 0.0f) {
    divisor = (x[2] * y[0] - x[0] * y[2]);
    p       = (y[0] * z[2] - y[2] * z[0]) / divisor;
    q       = (x[2] * z[0] - x[0] * z[2]) / divisor;
    r       = z[1];
  } else {
    divisor = (x[0] * y[1] - x[1] * y[0]);
    p       = (y[1] * z[0] - y[0] * z[1]) / divisor;
    q       = (x[0] * z[1] - x[1] * z[0]) / divisor;
    r       = p * x[2] + q * y[2] - z[2];
  }

  if (-epsilon < r && r < epsilon) // p*x[2]+q*y[2] == z[2])
  {
    if (p >= -epsilon && q >= -epsilon && p + q <= 1 + epsilon) {
      //	closestPoint = (p+q > 0.5) ? ((p > q) ? 2 : 1) : 0;
      barycentric_ab = q;
      barycentric_ac = p;

      return true;
    } // if p,q
  }   // if |r| < epsilon

  barycentric_ab = q;
  barycentric_ac = p;
  return false;
}

bool VistaTriangle::ProximityContains(const VistaVector3D& pnt, const VistaVector3D& triangleNormal,
    const float fProximityDistance, float& ab, float& ac, const float fEpsilon) const {
  // Check if point is close enough (in normal direction) to the triangle
  if (std::abs(triangleNormal * (pnt - m_a)) > fProximityDistance - fEpsilon) {
    return false;
  }
  // Now project the point into the triangle plane.
  const VistaVector3D projectedPoint = pnt - ((pnt - m_a) * triangleNormal) * triangleNormal;
  // Got to check if the projected point lies inside the triangle.
  // Therefor compute the barycentric coordinates respective to the triangle.
  const float m11 = (m_a - m_c) * (m_a - m_c);
  const float m12 = (m_a - m_c) * (m_b - m_c);
  const float m21 = m12;
  const float m22 = (m_b - m_c) * (m_b - m_c);
  const float y1  = (m_a - m_c) * (projectedPoint - m_c);
  const float y2  = (m_b - m_c) * (projectedPoint - m_c);

  // Parametric values of the two clostest points.
  const float b = (m11 * y2 - m21 * y1) / (m11 * m22 - m21 * m12);
  const float a = (y1 - m12 * b) / m11;
  const float c = 1.0f - a - b;
  const float scale =
      ((m_a - m_b).GetLength() + (m_b - m_c).GetLength() + (m_c - m_a).GetLength()) / 3.0f;
  const float delta = fProximityDistance / scale + fEpsilon;
  if ((-delta < a) && (a < 1.0f + delta) && (-delta < b) && (b < 1.0f + delta) && (-delta < c) &&
      (c < 1.0f + delta)) {
    ab = b;
    ac = c;
    return true;
  } else {
    return false;
  }
}

bool VistaTriangle::Intersects(const VistaTriangle& tri) {
  // First, put the values in double vectors. The single
  // precision version of this algorithm produces too many
  // errors.
  VistaVector<double, 3> a1, b1, c1, a2, b2, c2;
  a1[0] = m_a[0];
  a1[1] = m_a[1];
  a1[2] = m_a[2];
  b1[0] = m_b[0];
  b1[1] = m_b[1];
  b1[2] = m_b[2];
  c1[0] = m_c[0];
  c1[1] = m_c[1];
  c1[2] = m_c[2];
  a2[0] = tri.m_a[0];
  a2[1] = tri.m_a[1];
  a2[2] = tri.m_a[2];
  b2[0] = tri.m_b[0];
  b2[1] = tri.m_b[1];
  b2[2] = tri.m_b[2];
  c2[0] = tri.m_c[0];
  c2[1] = tri.m_c[1];
  c2[2] = tri.m_c[2];

  // Compute distance signs  of m_a, q1 and r1 to the plane of
  // triangle(p2,q2,r2)
  const VistaVector<double, 3> a2c2 = a2 - c2;
  const VistaVector<double, 3> b2c2 = b2 - c2;
  VistaVector<double, 3>       N2;
  N2[0]            = a2c2[1] * b2c2[2] - a2c2[2] * b2c2[1];
  N2[1]            = a2c2[2] * b2c2[0] - a2c2[0] * b2c2[2];
  N2[2]            = a2c2[0] * b2c2[1] - a2c2[1] * b2c2[0];
  const double da1 = (a1 - c2) * N2;
  const double db1 = (b1 - c2) * N2;
  const double dc1 = (c1 - c2) * N2;

  // If all vertices have the same sign, the triangle
  // lies completely on one side of the other triangle's
  // plane.
  if (((da1 * db1) > 0.0) && ((da1 * dc1) > 0.0)) {
    return false;
  }

  // Compute distance signs  of p2, q2 and r2 to the plane of
  // triangle(p1,q1,r1)
  const VistaVector<double, 3> b1a1 = b1 - a1;
  const VistaVector<double, 3> c1a1 = c1 - a1;
  VistaVector<double, 3>       N1;
  N1[0]            = b1a1[1] * c1a1[2] - b1a1[2] * c1a1[1];
  N1[1]            = b1a1[2] * c1a1[0] - b1a1[0] * c1a1[2];
  N1[2]            = b1a1[0] * c1a1[1] - b1a1[1] * c1a1[0];
  const double da2 = (a2 - c1) * N1;
  const double db2 = (b2 - c1) * N1;
  const double dc2 = (c2 - c1) * N1;

  // If all vertices have the same sign, the triangle
  // lies completely on one side of the other triangle's
  // plane.
  if (((da2 * db2) > 0.0) && ((da2 * dc2) > 0.0)) {
    return false;
  }

  // If T1 and T2 are not co-planar, there is always one of T1's vertices alone
  // on one side of T2's plane (the other two vertices being on the other side).
  // Permutate such that this vertex is passed as the first one. If the orientation
  // of T1 is switched by this, switch also db2 and dc2.
  if (da1 > 0.0) {
    if (db1 > 0.0) {
      return IntersectionTriTri3D(c1, a1, b1, a2, c2, b2, N1, N2, da2, dc2, db2);
    } else if (dc1 > 0.0) {
      return IntersectionTriTri3D(b1, c1, a1, a2, c2, b2, N1, N2, da2, dc2, db2);
    } else {
      return IntersectionTriTri3D(a1, b1, c1, a2, b2, c2, N1, N2, da2, db2, dc2);
    }
  } else if (da1 < 0.0) {
    if (db1 < 0.0) {
      return IntersectionTriTri3D(c1, a1, b1, a2, b2, c2, N1, N2, da2, db2, dc2);
    } else if (dc1 < 0.0) {
      return IntersectionTriTri3D(b1, c1, a1, a2, b2, c2, N1, N2, da2, db2, dc2);
    } else {
      return IntersectionTriTri3D(a1, b1, c1, a2, c2, b2, N1, N2, da2, dc2, db2);
    }
  } else {
    if (db1 < 0.0) {
      if (dc1 >= 0.0) {
        return IntersectionTriTri3D(b1, c1, a1, a2, c2, b2, N1, N2, da2, dc2, db2);
      } else {
        return IntersectionTriTri3D(a1, b1, c1, a2, b2, c2, N1, N2, da2, db2, dc2);
      }
    } else if (db1 > 0.0) {
      if (dc1 > 0.0) {
        return IntersectionTriTri3D(a1, b1, c1, a2, c2, b2, N1, N2, da2, dc2, db2);
      } else {
        return IntersectionTriTri3D(b1, c1, a1, a2, b2, c2, N1, N2, da2, db2, dc2);
      }
    } else {
      if (dc1 > 0.0) {
        return IntersectionTriTri3D(c1, a1, b1, a2, b2, c2, N1, N2, da2, db2, dc2);
      } else if (dc1 < 0.0) {
        return IntersectionTriTri3D(c1, a1, b1, a2, c2, b2, N1, N2, da2, dc2, db2);
      } else {
        return Coplanar(a1, b1, c1, a2, b2, c2, N1, N2);
      }
    }
  }
}

bool VistaTriangle::CheckMinMax(const VistaVector<double, 3>& p1, const VistaVector<double, 3>& q1,
    const VistaVector<double, 3>& r1, const VistaVector<double, 3>& p2,
    const VistaVector<double, 3>& q2, const VistaVector<double, 3>& r2) {
  const VistaVector<double, 3> p2q1 = p2 - q1;
  const VistaVector<double, 3> p1q1 = p1 - q1;
  VistaVector<double, 3>       N1;
  N1[0] = p2q1[1] * p1q1[2] - p2q1[2] * p1q1[1];
  N1[1] = p2q1[2] * p1q1[0] - p2q1[0] * p1q1[2];
  N1[2] = p2q1[0] * p1q1[1] - p2q1[1] * p1q1[0];
  if ((q2 - q1) * N1 > 0) {
    return false;
  }
  const VistaVector<double, 3> p2p1 = p2 - p1;
  const VistaVector<double, 3> r1p1 = r1 - p1;
  VistaVector<double, 3>       N2;
  N2[0] = p2p1[1] * r1p1[2] - p2p1[2] * r1p1[1];
  N2[1] = p2p1[2] * r1p1[0] - p2p1[0] * r1p1[2];
  N2[2] = p2p1[0] * r1p1[1] - p2p1[1] * r1p1[0];
  if ((r2 - p1) * N2 > 0) {
    return false;
  }
  return true;
}

bool VistaTriangle::IntersectionTriTri3D(const VistaVector<double, 3>& p1,
    const VistaVector<double, 3>& q1, const VistaVector<double, 3>& r1,
    const VistaVector<double, 3>& p2, const VistaVector<double, 3>& q2,
    const VistaVector<double, 3>& r2, const VistaVector<double, 3>& normal1,
    const VistaVector<double, 3>& normal2, const double dp2, const double dq2, const double dr2) {
  // Now orientate T2 vertices the same fashion T1s have been orientated in the
  // Intersects method. The first vertex passed of T2 is the one that is on its
  // own on his side of T1's plane.
  if (dp2 > 0.0) {
    if (dq2 > 0.0) {
      return CheckMinMax(p1, r1, q1, r2, p2, q2);
    } else if (dr2 > 0.0) {
      return CheckMinMax(p1, r1, q1, q2, r2, p2);
    } else {
      return CheckMinMax(p1, q1, r1, p2, q2, r2);
    }
  } else if (dp2 < 0.0) {
    if (dq2 < 0.0) {
      return CheckMinMax(p1, q1, r1, r2, p2, q2);
    } else if (dr2 < 0.0) {
      return CheckMinMax(p1, q1, r1, q2, r2, p2);
    } else {
      return CheckMinMax(p1, r1, q1, p2, q2, r2);
    }
  } else {
    if (dq2 < 0.0) {
      if (dr2 >= 0.0) {
        return CheckMinMax(p1, r1, q1, q2, r2, p2);
      } else {
        return CheckMinMax(p1, q1, r1, p2, q2, r2);
      }
    } else if (dq2 > 0.0) {
      if (dr2 > 0.0) {
        return CheckMinMax(p1, r1, q1, p2, q2, r2);
      } else {
        return CheckMinMax(p1, q1, r1, q2, r2, p2);
      }
    } else {
      if (dr2 > 0.0) {
        return CheckMinMax(p1, q1, r1, r2, p2, q2);
      } else if (dr2 < 0.0) {
        return CheckMinMax(p1, r1, q1, r2, p2, q2);
      } else {
        return Coplanar(p1, q1, r1, p2, q2, r2, normal1, normal2);
      }
    }
  }
}

bool VistaTriangle::Coplanar(const VistaVector<double, 3>& p1, const VistaVector<double, 3>& q1,
    const VistaVector<double, 3>& r1, const VistaVector<double, 3>& p2,
    const VistaVector<double, 3>& q2, const VistaVector<double, 3>& r2,
    const VistaVector<double, 3>& normal1, const VistaVector<double, 3>& normal2) {
  const double n_x = std::abs(normal1[0]);
  const double n_y = std::abs(normal1[1]);
  const double n_z = std::abs(normal1[2]);

  // Projection of the triangles in 3D onto 2D such that the area of
  // the projection is maximized.
  if ((n_x > n_z) && (n_x >= n_y)) {
    // Project onto plane YZ
    VistaVector<double, 2> a1, b1, c1, a2, b2, c2;
    a1[0] = q1[2];
    a1[1] = q1[1];
    b1[0] = p1[2];
    b1[1] = p1[1];
    c1[0] = r1[2];
    c1[1] = r1[1];
    a2[0] = q2[2];
    a2[1] = q2[1];
    b2[0] = p2[2];
    b2[1] = p2[1];
    c2[0] = r2[2];
    c2[1] = r2[1];
    return Intersection2D(a1, b1, c1, a2, b2, c2);
  } else if ((n_y > n_z) && (n_y >= n_x)) {
    // Project onto plane XZ
    VistaVector<double, 2> a1, b1, c1, a2, b2, c2;
    a1[0] = q1[0];
    a1[1] = q1[2];
    b1[0] = p1[0];
    b1[1] = p1[2];
    c1[0] = r1[0];
    c1[1] = r1[2];
    a2[0] = q2[0];
    a2[1] = q2[2];
    b2[0] = p2[0];
    b2[1] = p2[2];
    c2[0] = r2[0];
    c2[1] = r2[2];
    return Intersection2D(a1, b1, c1, a2, b2, c2);
  } else {
    // Project onto plane XY
    VistaVector<double, 2> a1, b1, c1, a2, b2, c2;
    a1[0] = p1[0];
    a1[1] = p1[1];
    b1[0] = q1[0];
    b1[1] = q1[1];
    c1[0] = r1[0];
    c1[1] = r1[1];
    a2[0] = p2[0];
    a2[1] = p2[1];
    b2[0] = q2[0];
    b2[1] = q2[1];
    c2[0] = r2[0];
    c2[1] = r2[1];
    return Intersection2D(a1, b1, c1, a2, b2, c2);
  }
}

bool VistaTriangle::Intersection2D(const VistaVector<double, 2>& p1,
    const VistaVector<double, 2>& q1, const VistaVector<double, 2>& r1,
    const VistaVector<double, 2>& p2, const VistaVector<double, 2>& q2,
    const VistaVector<double, 2>& r2) {
  if (Orientation2D(p1, q1, r1) < 0.0) {
    if (Orientation2D(p2, q2, r2) < 0.0) {
      return Intersection2DCCW(p1, r1, q1, p2, r2, q2);
    } else {
      return Intersection2DCCW(p1, r1, q1, p2, q2, r2);
    }
  } else {
    if (Orientation2D(p2, q2, r2) < 0.0) {
      return Intersection2DCCW(p1, q1, r1, p2, r2, q2);
    } else {
      return Intersection2DCCW(p1, q1, r1, p2, q2, r2);
    }
  }
}

bool VistaTriangle::Intersection2DCCW(const VistaVector<double, 2>& p1,
    const VistaVector<double, 2>& q1, const VistaVector<double, 2>& r1,
    const VistaVector<double, 2>& p2, const VistaVector<double, 2>& q2,
    const VistaVector<double, 2>& r2) {
  if (Orientation2D(p2, q2, p1) >= 0.0) {
    if (Orientation2D(q2, r2, p1) >= 0.0) {
      if (Orientation2D(r2, p2, p1) >= 0.0) {
        return true;
      } else {
        return IntersectionEdge(p1, q1, r1, p2, q2, r2);
      }
    } else {
      if (Orientation2D(r2, p2, p1) >= 0.0) {
        return IntersectionEdge(p1, q1, r1, r2, p2, q2);
      } else {
        return IntersectionVertex(p1, q1, r1, p2, q2, r2);
      }
    }
  } else {
    if (Orientation2D(q2, r2, p1) >= 0.0) {
      if (Orientation2D(r2, p2, p1) >= 0.0) {
        return IntersectionEdge(p1, q1, r1, q2, r2, p2);
      } else {
        return IntersectionVertex(p1, q1, r1, q2, r2, p2);
      }
    } else {
      return IntersectionVertex(p1, q1, r1, r2, p2, q2);
    }
  }
}

double VistaTriangle::Orientation2D(const VistaVector<double, 2>& a,
    const VistaVector<double, 2>& b, const VistaVector<double, 2>& c) {
  return (a[0] - c[0]) * (b[1] - c[1]) - (a[1] - c[1]) * (b[0] - c[0]);
}

bool VistaTriangle::IntersectionVertex(const VistaVector<double, 2>& P1,
    const VistaVector<double, 2>& Q1, const VistaVector<double, 2>& R1,
    const VistaVector<double, 2>& P2, const VistaVector<double, 2>& Q2,
    const VistaVector<double, 2>& R2) {
  if (Orientation2D(R2, P2, Q1) >= 0.0) {
    if (Orientation2D(R2, Q2, Q1) <= 0.0) {
      if (Orientation2D(P1, P2, Q1) > 0.0) {
        return (Orientation2D(P1, Q2, Q1) <= 0.0);
      } else {
        if (Orientation2D(P1, P2, R1) >= 0.0) {
          return (Orientation2D(Q1, R1, P2) >= 0.0);
        } else {
          return false;
        }
      }
    } else {
      if (Orientation2D(P1, Q2, Q1) <= 0.0) {
        if (Orientation2D(R2, Q2, R1) <= 0.0) {
          return (Orientation2D(Q1, R1, Q2) >= 0.0);
        } else {
          return false;
        }
      } else {
        return false;
      }
    }
  } else {
    if (Orientation2D(R2, P2, R1) >= 0.0) {
      if (Orientation2D(Q1, R1, R2) >= 0.0) {
        return (Orientation2D(P1, P2, R1) >= 0.0);
      } else {
        if (Orientation2D(Q1, R1, Q2) >= 0.0) {
          return (Orientation2D(R2, R1, Q2) >= 0.0);
        } else {
          return false;
        }
      }
    } else {
      return false;
    }
  }
}

bool VistaTriangle::IntersectionEdge(const VistaVector<double, 2>& P1,
    const VistaVector<double, 2>& Q1, const VistaVector<double, 2>& R1,
    const VistaVector<double, 2>& P2, const VistaVector<double, 2>& Q2,
    const VistaVector<double, 2>& R2) {
  if (Orientation2D(R2, P2, Q1) >= 0.0) {
    if (Orientation2D(P1, P2, Q1) >= 0.0) {
      return (Orientation2D(P1, Q1, R2) >= 0.0);
    } else {
      if (Orientation2D(Q1, R1, P2) >= 0.0) {
        return (Orientation2D(R1, P1, P2) >= 0.0);
      } else {
        return false;
      }
    }
  } else {
    if (Orientation2D(R2, P2, R1) >= 0.0) {
      if (Orientation2D(P1, P2, R1) >= 0.0) {
        if (Orientation2D(P1, R1, R2) >= 0.0) {
          return true;
        } else {
          return (Orientation2D(Q1, R1, R2) >= 0.0);
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
}

VistaVector3D VistaTriangle::GetA() const {
  return m_a;
}

void VistaTriangle::SetA(const VistaVector3D& val) {
  m_a = val;
}

VistaVector3D VistaTriangle::GetB() const {
  return m_b;
}

void VistaTriangle::SetB(const VistaVector3D& val) {
  m_b = val;
}

void VistaTriangle::SetC(const VistaVector3D& val) {
  m_c = val;
}

VistaVector3D VistaTriangle::GetC() const {
  return m_c;
}

/* ########################################################################## */
/* ##  VISTA  PLANE ######################################################### */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTOR
VistaPlane::VistaPlane(const VistaVector3D& origin, const VistaVector3D& xDir,
    const VistaVector3D& yDir, const VistaVector3D& vNormVector) {
  m_Origin      = origin;
  m_XDir        = xDir;
  m_YDir        = yDir;
  m_vNormVector = vNormVector;
  GetEquationRep(m_parA, m_parB, m_parC, m_parD);
}

// DESTRUCTOR
VistaPlane::~VistaPlane() {
}

/*============================================================================*/
/*  OPERATIONS                                                                */
/*============================================================================*/
void VistaPlane::operator=(const VistaPlane& oOther) {
  m_Origin = oOther.GetOrigin();
  m_XDir   = oOther.GetXDir();
  m_YDir   = oOther.GetYDir();

  m_vNormVector = m_XDir.Cross(m_YDir);
  GetEquationRep(m_parA, m_parB, m_parC, m_parD);
}

void VistaPlane::SetOrigin(const VistaVector3D& org) {
  m_Origin = org;
}

void VistaPlane::SetXDir(const VistaVector3D& xDir) {
  m_XDir = xDir;
  GetEquationRep(m_parA, m_parB, m_parC, m_parD);
}

void VistaPlane::SetYDir(const VistaVector3D& yDir) {
  m_YDir = yDir;
  GetEquationRep(m_parA, m_parB, m_parC, m_parD);
}

void VistaPlane::CalcPlane(const VistaVector3D& upVector, const VistaVector3D& orgPoint) {
  // I choose to vecs which are not colinear but perpendicular and normalized
  VistaVector3D testVec1(1.0, 0.0, 0.0);
  VistaVector3D testVec2(0.0, 1.0, 0.0);
  VistaVector3D baseVec1;
  VistaVector3D baseVec2;
  // I search a vec which is perpenticular to the given up vector
  // if two vec are near colinear than use the other
  // cos(v1v2)=v1*v2/(length(v1)*length(v2))
  if ((upVector * testVec1) >= 0.5f) {
    // use the first
    baseVec1 = upVector.Cross(testVec1);
  } else {
    // use the other
    baseVec1 = upVector.Cross(testVec2);
  }

  // the second base vector is perpendicular to up and the first base
  baseVec2 = upVector.Cross(baseVec1);

  // set plane origin and axes
  m_Origin = orgPoint;
  m_XDir   = baseVec1;
  m_YDir   = baseVec2;

  m_vNormVector = m_XDir.Cross(m_YDir);
  GetEquationRep(m_parA, m_parB, m_parC, m_parD);
}

/*============================================================================*/
void VistaPlane::GetEquationRep(float& paramA, float& paramB, float& paramC, float& paramD) const {
  paramA = m_vNormVector[Vista::X];
  paramB = m_vNormVector[Vista::Y];
  paramC = m_vNormVector[Vista::Z];
  paramD = -(m_vNormVector[Vista::X] * m_Origin[Vista::X] +
             m_vNormVector[Vista::Y] * m_Origin[Vista::Y] +
             m_vNormVector[Vista::Z] * m_Origin[Vista::Z]);
}

/*============================================================================*/
float VistaPlane::CalcDistance(const VistaVector3D& testPnt) const {
  float nDist =
      m_parA * testPnt[Vista::X] + m_parB * testPnt[Vista::Y] + m_parC * testPnt[Vista::Z] + m_parD;
  return nDist;
}

/*============================================================================*/
bool VistaPlane::CalcIntersection(const VistaRay& testLine, VistaVector3D& resPnt) const {
  float lambda;
  if (CalcIntersection(testLine, lambda)) {
    resPnt = testLine.GetPointByParameter(lambda);
    return true;
  }
  return false;
}

bool VistaPlane::CalcIntersection(const VistaLineSegment& lineSeg, VistaVector3D& resPnt) const {
  //   float dist1 = CalcDistance (lineSeg.GetStartPoint());
  //   float dist2 = CalcDistance (lineSeg.GetEndPoint());
  //   float distDelta = dist1 - dist2;
  //   float distTotal = std::abs(dist1) + std::abs(dist2);

  //   // is the test line coincide with the plane
  //   if (distTotal < Vista::Epsilon)
  //       return false;

  //   if ( std::abs(distDelta / distTotal) < Vista::Epsilon)
  //       return false;

  //   resPnt = lineSeg.GetPointByParameter ( dist1 / distDelta );
  //   return true;

  float lambda;
  if (CalcIntersection(lineSeg, lambda)) {
    resPnt = lineSeg.GetPointByParameter(lambda);
    return true;
  }
  return false;
}

bool VistaPlane::CalcIntersection(const VistaRay& ray, float& lambda) const {
  float divisor = m_vNormVector * ray.GetDir();
  if (std::abs(divisor) < Vista::Epsilon) // the ray and the line are parallel
  {
    // if(std::abs(m_parD + m_vNormVector * ray.GetOrigin()) < Vista::Epsilon) // the ray lies in
    // the plane
    //{
    //	lambda = 0.0f;
    //	return true;
    //}
    // else
    return false;
  }

  lambda = (-m_parD - m_vNormVector * ray.GetOrigin()) / divisor;
  return true;
}

bool VistaPlane::CalcIntersection(const VistaLineSegment& line, float& lambda) const {
  if (CalcIntersection(
          VistaRay(line.GetStartPoint(), line.GetEndPoint() - line.GetStartPoint()), lambda)) {
    if (-lambda < Vista::Epsilon && lambda - 1.0f < Vista::Epsilon)
      return true;
  }
  return false;
}

/*============================================================================*/
VistaVector3D VistaPlane::CalcNearestPointOnPlane(const VistaVector3D& worldPnt) const {
  VistaVector3D resPnt;

  // calculate the intersection point of created line and this plane
  CalcIntersection(VistaRay(worldPnt, m_vNormVector), resPnt);

  // return the provided intersection point
  return resPnt;
}

/*============================================================================*/
VistaVector3D VistaPlane::TransformPointOnPlane(
    const VistaVector3D& givenPnt, float rot, float scale, float transX, float transY) const {
  VistaVector3D resPnt, lgivenPnt;

  // Normally we assume that the provided point lies directly on the plane
  // but it is better to snap the given point to the plane, first.
  // After this it will lie on the plane! But this can cause mistakes,
  // because this is a geometrical snap and not an optical snap!!
  lgivenPnt = CalcNearestPointOnPlane(givenPnt);

  // consider as a local plane point
  resPnt = ConvertWorldToPlane(lgivenPnt);

  // ==== R O T A T I O N =================
  // rotate point:   [  cos  -sin   0 ]   [Vista::X]   [resX]
  //                 [  sin   cos   0 ] * [Vista::Y] = [resY]
  //                 [   0     0    1 ]   [Vista::Z]   [  Z ]
  float nSinRot    = std::sin(rot * Vista::Pi / 180.0f);
  float nCosRot    = std::cos(rot * Vista::Pi / 180.0f);
  float tempX      = resPnt[Vista::X];
  float tempY      = resPnt[Vista::Y];
  resPnt[Vista::X] = tempX * nCosRot - tempY * nSinRot;
  resPnt[Vista::Y] = tempX * nSinRot + tempY * nCosRot;
  // reconvert to world
  resPnt = ConvertPlaneToWorld(resPnt);

  // ==== S C A L I N G =================
  // now scale the rotated point refered to the origin (0,0,0)
  VistaVector3D resVec = resPnt;
  // resVec = resPnt;
  float scaleFac = resVec.GetLength();
  scaleFac       = scaleFac * scale / scaleFac;
  resVec         = resVec * scaleFac;
  // copy back into a point structur
  resPnt[Vista::X] = (resVec[Vista::X]);
  resPnt[Vista::Y] = (resVec[Vista::Y]);
  resPnt[Vista::Z] = (resVec[Vista::Z]);

  // ==== T R A N S L A T I O N =================
  resPnt[Vista::X] = resPnt[Vista::X] + transX;
  resPnt[Vista::Y] = resPnt[Vista::Y] + transY;

  return resPnt;
}

/*============================================================================*/

VistaVector3D VistaPlane::ConvertWorldToPlane(const VistaVector3D& worldPnt) const {
  // prepare transformation matrix
  VistaTransformMatrix transMat;
  // calculate up vector (z direction)
  VistaVector3D upVector = m_XDir.Cross(m_YDir);

  // set matrix with new coor sys
  transMat.SetRow(0, this->m_XDir);
  transMat.SetRow(1, this->m_YDir);
  transMat.SetRow(2, upVector);

  // consider the translation of the origin
  transMat.SetValue(0, 3,
      m_Origin[Vista::X] * (0 - transMat.GetValue(0, 0)) +
          m_Origin[Vista::Y] * (0 - transMat.GetValue(0, 1)) +
          m_Origin[Vista::Z] * (0 - transMat.GetValue(0, 2)));
  transMat.SetValue(1, 3,
      m_Origin[Vista::X] * (0 - transMat.GetValue(1, 0)) +
          m_Origin[Vista::Y] * (0 - transMat.GetValue(1, 1)) +
          m_Origin[Vista::Z] * (0 - transMat.GetValue(1, 2)));
  transMat.SetValue(2, 3,
      m_Origin[Vista::X] * (0 - transMat.GetValue(2, 0)) +
          m_Origin[Vista::Y] * (0 - transMat.GetValue(2, 1)) +
          m_Origin[Vista::Z] * (0 - transMat.GetValue(2, 2)));

  // calculate transformed position
  return transMat.Transform(worldPnt);
}

/*============================================================================*/

VistaVector3D VistaPlane::ConvertPlaneToWorld(const VistaVector3D& planePnt) const {
  // prepare transformation matrix
  VistaTransformMatrix transMat;

  // calculate up vector (z direction)
  VistaVector3D upVector = m_XDir.Cross(m_YDir);

  // set matrix with new coor sys
  transMat.SetRow(0, this->m_XDir);
  transMat.SetRow(1, this->m_YDir);
  transMat.SetRow(2, upVector);

  // consider the translation of the origin
  // SetValue (row, col, val);
  transMat.SetValue(0, 3,
      m_Origin[Vista::X] * (0 - transMat.GetValue(0, 0)) +
          m_Origin[Vista::Y] * (0 - transMat.GetValue(0, 1)) +
          m_Origin[Vista::Z] * (0 - transMat.GetValue(0, 2)));
  transMat.SetValue(1, 3,
      m_Origin[Vista::X] * (0 - transMat.GetValue(1, 0)) +
          m_Origin[Vista::Y] * (0 - transMat.GetValue(1, 1)) +
          m_Origin[Vista::Z] * (0 - transMat.GetValue(1, 2)));
  transMat.SetValue(2, 3,
      m_Origin[Vista::X] * (0 - transMat.GetValue(2, 0)) +
          m_Origin[Vista::Y] * (0 - transMat.GetValue(2, 1)) +
          m_Origin[Vista::Z] * (0 - transMat.GetValue(2, 2)));

  // inverse given matrix
  VistaTransformMatrix invMat = transMat.GetInverted();

  // transform
  return invMat.Transform(planePnt);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   SetNormVector                                                 */
/*                                                                            */
/*============================================================================*/
bool VistaPlane::SetNormVector(const VistaVector3D& vNormVector) {
  // vNormVector.Normalize();
  m_vNormVector = vNormVector;

  //@GF: GetEquationRep aufrufen.
  GetEquationRep(m_parA, m_parB, m_parC, m_parD);

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   CalcDistanceToZero                                            */
/*                                                                            */
/*============================================================================*/
float VistaPlane::CalcDistanceToZero() const {
  return (m_vNormVector * m_Origin);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   CalcIntersectionPoint                                         */
/*                                                                            */
/*============================================================================*/
bool VistaPlane::CalcIntersectionPoint(
    const VistaRay& testLine, VistaVector3D& resolutionPoint) const {
  // calc the LineFactor for the IntersectionPoint
  float nLineFactor = 0.0;

  if ((m_vNormVector * testLine.GetDir()) == 0)
    return false;
  else {
    nLineFactor = ((CalcDistanceToZero() - (m_vNormVector * testLine.GetOrigin())) /
                   (m_vNormVector * testLine.GetDir()));
  }

  // calc the IntersectionPoint and return it
  resolutionPoint = testLine.GetDir() * nLineFactor + testLine.GetOrigin();

  return true;
}

int VistaPlane::CalcOrientation(const float px, const float py, const float pz) const {
  const float TOLERANCE = 1.e-6f;
  // line equation in 2-D and that of the plane in 3-D
  float planeEq = m_vNormVector[0] * (px - m_Origin[0]) + m_vNormVector[1] * (py - m_Origin[1]) +
                  m_vNormVector[2] * (pz - m_Origin[2]);

  if (std::abs(planeEq) <= TOLERANCE)
    return 0;
  else if (planeEq > TOLERANCE)
    return 1;
  else
    return -1;
}

int VistaPlane::CalcOrientation(const float p[3]) const {
  return CalcOrientation(p[0], p[1], p[2]);
}

int VistaPlane::CalcOrientation(const VistaVector3D& pt) const {
  return CalcOrientation(pt[0], pt[1], pt[2]);
}

/* ########################################################################## */
/* ##  VISTA  POLYGON ######################################################### */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTOR
VistaPolygon::VistaPolygon() {
}

// COPY CONSTRUCTOR
VistaPolygon::VistaPolygon(std::vector<VistaVector3D*>& polyVtx) {
  SetPolyPoints(polyVtx);
}

// DESTRUCTOR
VistaPolygon::~VistaPolygon() {
  m_vecPolyPnts.clear();
}

/*============================================================================*/
/*  OPERATIONS                                                                */
/*============================================================================*/
void VistaPolygon::operator=(VistaPolygon NewPoly) {
  //    m_Origin = NewPoly.GetOrigin();
  //    m_XDir   = NewPoly.GetXDir();
  //    m_YDir   = NewPoly.GetYDir();
  m_vecPolyPnts.clear();
  m_vecPolyPnts = NewPoly.m_vecPolyPnts;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
bool VistaPolygon::SetPolyPoints(std::vector<VistaVector3D*>& polyVtx) {
  m_vecPolyPnts.clear();
  for (unsigned int nCount = 0; nCount < polyVtx.size(); ++nCount) {
    m_vecPolyPnts.push_back(*(polyVtx[nCount]));
  }

  if (m_vecPolyPnts.size() < 2)
    return false;
  /*
          VistaVector3D  upVec = CalcUpVector();
          VistaVector3D  xDir = m_vecPolyPnts[1] - m_vecPolyPnts[0];
          VistaVector3D  yDir = upVec.Cross (xDir);

          SetOrigin (m_vecPolyPnts[0]);
          SetXDir   (xDir);
          SetYDir   (yDir);
  */
  return true;
}

/*============================================================================*/
void VistaPolygon::GetPolyPoints(std::vector<VistaVector3D*>& polyVtx) {
  for (unsigned int nCount = 0; nCount < m_vecPolyPnts.size(); ++nCount) {
    VistaVector3D* pNewPnt = new VistaVector3D;
    *pNewPnt               = m_vecPolyPnts[nCount];
    polyVtx.push_back(pNewPnt);
  }
}

VistaVector3D VistaPolygon::CalcUpVector() {
  //  calculates the normal vector of a polygon by summing
  //  the cross products of any neighbored pair of poly points
  //  and normalizing the result
  VistaVector3D normVec(0, 0, 0);
  long          numPnts = (long)m_vecPolyPnts.size();
  for (int vtxNum = 0; vtxNum < numPnts; vtxNum++) {
    VistaVector3D vec1(0, 0, 0);
    if (vtxNum == 0) {
      vec1[Vista::X] = (m_vecPolyPnts[numPnts - 1][Vista::X]);
      vec1[Vista::Y] = (m_vecPolyPnts[numPnts - 1][Vista::Y]);
      vec1[Vista::Z] = (m_vecPolyPnts[numPnts - 1][Vista::Z]);
    } else {
      vec1[Vista::X] = (m_vecPolyPnts[vtxNum - 1][Vista::X]);
      vec1[Vista::Y] = (m_vecPolyPnts[vtxNum - 1][Vista::Y]);
      vec1[Vista::Z] = (m_vecPolyPnts[vtxNum - 1][Vista::Z]);
    }
    VistaVector3D vec2(m_vecPolyPnts[vtxNum][Vista::X], m_vecPolyPnts[vtxNum][Vista::Y],
        m_vecPolyPnts[vtxNum][Vista::Z]);

    // add cross product to already added cross products
    VistaVector3D newCross(0, 0, 0);
    newCross = vec1.Cross(vec2);
    normVec  = normVec + newCross;
  }

  // now we are ready and must normalize the result
  normVec.Normalize();
  return normVec;
}

/*============================================================================*/
VistaVector3D VistaPolygon::GetUpVector() {
  // return ( GetXDir().Cross (GetYDir()) );
  return CalcUpVector();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DivideIntoMonotonePolygons                                  */
/*                                                                            */
/*  AUTHOR    :   Andreas Gerndt                                              */
/*  DATE      :   13.07.1999                                                  */
/*                                                                            */
/*  PURPOSE   :   It is easy to triangulate a convex polygon. But an          */
/*                arbitrary concave polygon must be divided into convex       */
/*                polygons, first. The first step to get convex polygons      */
/*                could be divide the original polygon into y-monotone        */
/*                polygons. If a polygon is y-monotone, you can walk from     */
/*                the topmost to bottommost vertex along one boundary chain   */
/*                without to change the downwards or horizontal direction.    */
/*                You will never go upwards!                                  */
// ALGORITHM:
// 1.) Search topmost vertex and the bottommost vertex
// 2.) Go to the bottommost vertex on the left or right boundary chain
// 3.) Search TURN VERTEX (direction turns up)
// 4.) Search vertex wich lies below of the TURN VERTEX
// 5.) Divide Polygon into two polygons using found vertex and TURN VERTEX
// 6.) At the bottommost vertex go along the other boundary chain upwards
// 7.) Search TURN VERTEX again and insert diagonal to the next upper vertex
// 8.) Divide the seperated polygons, too.
// LITERATUR: Computational Geometry; M. de Berg et.al., Springer 1997
/*                                                                            */
/*  INPUT     :   --                                                          */
/*  OUTPUT    :   --                                                          */
/*  GLOBALS   :   --                                                          */
/*  RETURN    :   TRUE  = success                                             */
/*                FALSE = failure                                             */
/*                                                                            */
/*============================================================================*/
/*
std::vector<void*> VistaPolygon::DivideIntoMonotonePolygons
        (std::vector<VistaVector3D> & orgPoly)
{
        std::vector<void*>   monoPolyList;
        return monoPolyList;
}
*/

/* ########################################################################## */
/* ##  VISTA  POLYGONWITHHOLES ############################################## */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTOR
VistaPolygonWithHoles::VistaPolygonWithHoles(
    std::vector<VistaVector3D*>& polyVtx, std::vector<VistaPolygon>& holes) {
  SetPolyPoints(polyVtx);
  SetHoles(holes);
}

// DESTRUCTOR
VistaPolygonWithHoles::~VistaPolygonWithHoles() {
  m_vecPolyPnts.clear();
  m_vecHoles.clear();
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
bool VistaPolygonWithHoles::SetHoles(std::vector<VistaPolygon>& holes) {
  m_vecHoles.clear();
  for (unsigned int nCount = 0; nCount < holes.size(); ++nCount) {
    m_vecHoles.push_back(holes[nCount]);
  }

  return true;
}

/*============================================================================*/
void VistaPolygonWithHoles::GetHoles(std::vector<VistaPolygon>& holes) {
  holes.clear();
  for (unsigned int nCount = 0; nCount < m_vecHoles.size(); ++nCount) {
    VistaPolygon NewPoly = m_vecHoles[nCount];
    holes.push_back(NewPoly);
  }
}

/* ########################################################################## */
/* ##  VISTA  CURVE   ####################################################### */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTOR

VistaCurve::VistaCurve(std::vector<VistaCurveSegment*> curves) {
  unsigned int       i;
  VistaCurveSegment *buf = NULL, *last = NULL;

  m_vecCurve = curves;

  // sort algorithm
  for (last = m_vecCurve[0], i = 1; i < m_vecCurve.size(); ++i) {
    if (last->GetEndPoint() == m_vecCurve[i]->GetStartPoint()) {
      last = m_vecCurve[i];
    } else {
      unsigned int j;
      buf = m_vecCurve[i];
      if (buf->GetEndPoint() == m_vecCurve[0]->GetStartPoint()) {
        for (j = i; j > 0; --j) {
          m_vecCurve[j] = m_vecCurve[j - 1];
        }
        last = m_vecCurve[i];
      } else {
        for (j = i; j < m_vecCurve.size() - 1; ++j) {
          m_vecCurve[j] = m_vecCurve[j + 1];
        }
        i--;
      }
      m_vecCurve[j] = buf;
    }
  }
}

// DESTRUCTOR

VistaCurve::~VistaCurve() {
  m_vecCurve.clear();
}

/*============================================================================*/
/*  OPERATIONS                                                                */
/*============================================================================*/
void VistaCurve::operator=(VistaCurve NewCv) {
  m_vecCurve.clear();
  m_vecCurve = NewCv.m_vecCurve;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
void VistaCurve::GetSegments(std::vector<VistaCurveSegment*>& segments) const {
  segments = m_vecCurve;
}

bool VistaCurve::IsLoop() const {
  return m_vecCurve[0]->GetStartPoint() == m_vecCurve[m_vecCurve.size() - 1]->GetEndPoint();
}

/* ########################################################################## */
/* ##  VISTA  SPLINE CURVE   ################################################ */
/* ########################################################################## */

VistaSplineCurve::VistaSplineCurve(std::vector<VistaVector3D>& points,
    std::vector<VistaVector3D>& tangents, std::vector<float>& parameters)
    : m_vecPoints(points)
    , m_vecTangents(tangents)
    , m_vecParameters(parameters) {
  unsigned int       i;
  VistaCurveSegment* pSegment;

  for (i = 0; i < m_vecParameters.size() - 1; i++) {
    pSegment = new VistaSplineSegment(
        points[i], points[i + 1], tangents[i], tangents[i + 1], parameters[i], parameters[i + 1]);
    m_vecCurve.push_back(pSegment);
  }
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
void VistaSplineCurve::InterpolatePoints(std::vector<VistaVector3D>& points, int pointsPerSegment) {
  int           i, j;
  float         paramT, step; //, test;
  VistaVector3D pPoint, buf;

  points.clear();

  for (i = 0; i < NofSegments(); i++) {
    step = (m_vecParameters[i + 1] - m_vecParameters[i]) / pointsPerSegment;
    // test = m_vecParameters[i+1];
    // test = m_vecParameters[i];
    for (j = 0; j < pointsPerSegment; j++) {
      paramT = m_vecParameters[i] + (j * step);
      pPoint = m_vecCurve[i]->GetPointByParameter(paramT);
      // pPoint = VistaVector3D(buf);
      points.push_back(pPoint);
    }
  }
  pPoint = m_vecCurve[i - 1]->GetEndPoint();
  points.push_back(pPoint);
}

/*============================================================================*/
void VistaSplineCurve::GetSegments(std::vector<VistaCurveSegment*>& segments) const {
  int                i, j, precision;
  float              paramT, step;
  VistaVector3D      start, end;
  VistaCurveSegment* pLine;

  segments.clear();

  for (i = 0; i < NofSegments(); i++) {
    precision = Vista::assert_cast<VistaSplineSegment*>(m_vecCurve[i])->GetPrecision();
    step      = 1.0f / precision;
    start     = m_vecCurve[i]->GetStartPoint();

    for (j = 1; j < precision; j++) {
      paramT = (j * step);
      end    = m_vecCurve[i]->GetPointByParameter(paramT);
      pLine  = new VistaLineSegment(start, end);
      segments.push_back(pLine);
      start = end;
    }
    end   = m_vecCurve[i]->GetEndPoint();
    pLine = new VistaLineSegment(start, end);
    segments.push_back(pLine);
  }
}

/*============================================================================*/
void VistaSplineCurve::SetPrecision(const int& precision) {
  int i;

  for (i = 0; i < NofSegments(); i++) {
    ((VistaSplineSegment*)m_vecCurve[i])->SetPrecision(precision);
  }
}

/* ########################################################################## */
/* ##  VISTA  EXTRUSION   ################################################### */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTORS
VistaExtrusion::VistaExtrusion(const VistaCurve& curve) {
  std::vector<VistaCurveSegment*> segments;
  curve.GetSegments(segments);
  VistaCurve newCurve(segments);
  m_Curve = newCurve;
}

/* ########################################################################## */
/* ##  VISTA  TETRAHEDRON  ################################################## */
/* ########################################################################## */
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTORS
VistaTetrahedron::VistaTetrahedron() {
}

VistaTetrahedron::VistaTetrahedron(
    const float p1[3], const float p2[3], const float p3[3], const float p4[3])
    : m_p1(p1)
    , m_p2(p2)
    , m_p3(p3)
    , m_p4(p4) {
  m_volume = ComputeVolume(p1, p2, p3, p4);
}

VistaTetrahedron::VistaTetrahedron(const VistaVector3D& p1, const VistaVector3D& p2,
    const VistaVector3D& p3, const VistaVector3D& p4)
    : m_p1(p1)
    , m_p2(p2)
    , m_p3(p3)
    , m_p4(p4) {
  m_volume = ComputeVolume(p1, p2, p3, p4);
}

VistaTetrahedron::~VistaTetrahedron() {
}

// IMPLEMENTATION
void VistaTetrahedron::SetCoordinates(const VistaVector3D& p1, const VistaVector3D& p2,
    const VistaVector3D& p3, const VistaVector3D& p4) {
  m_p1     = p1;
  m_p2     = p2;
  m_p3     = p3;
  m_p4     = p4;
  m_volume = ComputeVolume(p1, p2, p3, p4);
}

void VistaTetrahedron::SetCoordinates(
    const float p1[3], const float p2[3], const float p3[3], const float p4[3]) {
  m_p1     = VistaVector3D(p1);
  m_p2     = VistaVector3D(p2);
  m_p3     = VistaVector3D(p3);
  m_p4     = VistaVector3D(p4);
  m_volume = ComputeVolume(p1, p2, p3, p4);
}

void VistaTetrahedron::GetBarycentricCoordinate(const float p[3], float bc[3]) const {
  const float D1 = ComputeVolume(p, &m_p2[0], &m_p3[0], &m_p4[0]);
  const float D2 = ComputeVolume(&m_p1[0], p, &m_p3[0], &m_p4[0]);
  const float D3 = ComputeVolume(&m_p1[0], &m_p2[0], p, &m_p4[0]);

  bc[0] = D1 / m_volume;
  bc[1] = D2 / m_volume;
  bc[2] = D3 / m_volume;
}

void VistaTetrahedron::GetBarycentricCoordinate(const VistaVector3D& p, float bc[3]) const {
  GetBarycentricCoordinate(&p[0], bc);
}

bool VistaTetrahedron::PtInsideTetrahedron(const float pt[3], float coords[4]) const {
  GetBarycentricCoordinate(pt, coords);
  coords[3] = 1.0f - coords[0] - coords[1] - coords[2];

  const float epsilon = -0.00001f;
  // the point pt lies inside a tetrahedron
  if (coords[0] >= epsilon && coords[1] >= epsilon && coords[2] >= epsilon && coords[3] >= epsilon)
    return true;
  else
    return false;
}

float VistaTetrahedron::ComputeVolume(
    const float p1[3], const float p2[3], const float p3[3], const float p4[3]) const {
  VistaVector3D r, a(p2), b(p3), c(p4);

  a -= VistaVector3D(p1);
  b -= VistaVector3D(p2);
  c -= VistaVector3D(p1);

  // r = a x b
  r = a.Cross(b);

  return -(r * c) / 6.0f;
}

float VistaTetrahedron::ComputeVolume(const VistaVector3D& p1, const VistaVector3D& p2,
    const VistaVector3D& p3, const VistaVector3D& p4) const {
  VistaVector3D r, a(p2), b(p3), c(p4);

  a -= p1;
  b -= p2;
  c -= p1;

  // r = a x b
  r = a.Cross(b);

  return -(r * c) / 6.0f;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaEvenCone::VistaEvenCone()
    : m_nHeight(1.0f)
    , m_nRadius(1.0f) {
}

VistaEvenCone::VistaEvenCone(float nHeight, float nRadius)
    : m_nHeight(nHeight)
    , m_nRadius(nRadius) {
}

VistaEvenCone::~VistaEvenCone() {
}

bool VistaEvenCone::Intersects(
    const float pnt[3], float& dPerp, float& dProj, float& dAngle) const {
  if (pnt[Vista::Z] <= 0 || (pnt[Vista::Z] > m_nHeight))
    return false;

  dPerp = ::sqrtf(pnt[Vista::X] * pnt[Vista::X] + pnt[Vista::Y] * pnt[Vista::Y]);
  dProj = pnt[Vista::Z];

  dAngle = ::atanf(dPerp / dProj);

  if (std::abs(dAngle) <= GetOpeningAngle())
    return true;

  return false;
}

float VistaEvenCone::GetHeight() const {
  return m_nHeight;
}

float VistaEvenCone::GetRadius() const {
  return m_nRadius;
}

void VistaEvenCone::SetHeight(float nHeight) {
  m_nHeight = nHeight;
}

void VistaEvenCone::SetRadius(float nRadius) {
  m_nRadius = nRadius;
}

float VistaEvenCone::GetOpeningAngle() const {
  return ::atanf(m_nRadius / m_nHeight);
}
