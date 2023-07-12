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

//============================================================================
//  INCLUDES
//============================================================================

#include "VistaBoundingSphere.h"
#include "VistaBase/VistaMathBasics.h"
#include <cmath>

//============================================================================
//  MAKROS AND DEFINES
//============================================================================

//============================================================================
//  CONSTRUCTORS / DESTRUCTOR
//============================================================================

/**
 * Default constructor, creates an empty bounding sphere centered at the origin.
 */
VistaBoundingSphere::VistaBoundingSphere() {
  m_radius    = 0.0f;
  m_center[0] = m_center[1] = m_center[2] = 0.0f;
}

/**
 * Computes the circumcenter and radius of a bounding circle for the triangle
 * specified by the coordinates of its vertices a,b,c. The circumcenter
 * is computed after "Eric W. Weisstein, "Circumcenter", from Mathworld --
 * A Wolfram Web Resource." http://mathworld.wolfram.com/Circumcenter.html . The
 * circumradius is than computed as the distance from the circumcenter towards
 * one corner of the triangle.
 */
VistaBoundingSphere::VistaBoundingSphere(const float a[3], const float b[3], const float c[3]) {
  // Compute the circumcenter.
  const float fEdgeAB[3] = {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
  const float fEdgeBC[3] = {b[0] - c[0], b[1] - c[1], b[2] - c[2]};
  const float fEdgeCA[3] = {c[0] - a[0], c[1] - a[1], c[2] - a[2]};
  const float fLengthAB =
      sqrt(fEdgeAB[0] * fEdgeAB[0] + fEdgeAB[1] * fEdgeAB[1] + fEdgeAB[2] * fEdgeAB[2]);
  const float fLengthBC =
      sqrt(fEdgeBC[0] * fEdgeBC[0] + fEdgeBC[1] * fEdgeBC[1] + fEdgeBC[2] * fEdgeBC[2]);
  const float fLengthCA =
      sqrt(fEdgeCA[0] * fEdgeCA[0] + fEdgeCA[1] * fEdgeCA[1] + fEdgeCA[2] * fEdgeCA[2]);

  // Compute the circumcenter.
  const float fCosinusA =
      (fEdgeAB[0] * fEdgeCA[0] + fEdgeAB[1] * fEdgeCA[1] + fEdgeAB[2] * fEdgeCA[2]) /
      (-1.0f * fLengthAB * fLengthCA);
  const float fCosinusB =
      (fEdgeAB[0] * fEdgeBC[0] + fEdgeAB[1] * fEdgeBC[1] + fEdgeAB[2] * fEdgeBC[2]) /
      (-1.0f * fLengthAB * fLengthBC);
  const float fCosinusC =
      (fEdgeBC[0] * fEdgeCA[0] + fEdgeBC[1] * fEdgeCA[1] + fEdgeBC[2] * fEdgeCA[2]) /
      (-1.0f * fLengthBC * fLengthCA);

  const float factor = fCosinusA + fCosinusB + fCosinusC;
  m_center[0]        = (a[0] * fCosinusA + b[0] * fCosinusB + c[0] * fCosinusC) / factor;
  m_center[1]        = (a[1] * fCosinusA + b[1] * fCosinusB + c[1] * fCosinusC) / factor;
  m_center[2]        = (a[2] * fCosinusA + b[2] * fCosinusB + c[2] * fCosinusC) / factor;

  // Compute the circumradius.
  const float fRadiusVector[3] = {a[0] - m_center[0], a[1] - m_center[1], a[2] - m_center[2]};
  m_radius = (float)sqrt(fRadiusVector[0] * fRadiusVector[0] + fRadiusVector[1] * fRadiusVector[1] +
                         fRadiusVector[2] * fRadiusVector[2]);
}

VistaBoundingSphere::~VistaBoundingSphere() {
}

//============================================================================
//  IMPLEMENTATION
//============================================================================

bool VistaBoundingSphere::IsEmpty() const {
  return (m_radius == 0.0f);
}

float VistaBoundingSphere::GetRadius() const {
  return m_radius;
}

float VistaBoundingSphere::GetVolume() const {
  return Vista::Pi * 4.0f / 3.0f * m_radius * m_radius * m_radius;
}

void VistaBoundingSphere::GetCenter(float center[3]) const {
  center[0] = m_center[0];
  center[1] = m_center[1];
  center[2] = m_center[2];
}

VistaBoundingSphere VistaBoundingSphere::Expand(const float size) {
  m_radius += size;
  return *this;
}

/**
 * Checks if the distance of the sphere's center to the specified point is
 * shorter than the radius of the bounding sphere.
 */
bool VistaBoundingSphere::Intersects(const float point[3]) const {
  const float fDistanceVector[3] = {
      point[0] - m_center[0], point[1] - m_center[1], point[2] - m_center[2]};
  return (fDistanceVector[0] * fDistanceVector[0] + fDistanceVector[1] * fDistanceVector[1] +
             fDistanceVector[2] * fDistanceVector[2]) < m_radius * m_radius;
}

/**
 * Checks if the distance between both sphere's center is smaller than the
 * sum of both radii.
 */
bool VistaBoundingSphere::Intersects(const VistaBoundingSphere& BSphere) const {
  const float fDistanceVector[3] = {m_center[0] - BSphere.m_center[0],
      m_center[1] - BSphere.m_center[1], m_center[2] - BSphere.m_center[2]};
  return (fDistanceVector[0] * fDistanceVector[0] + fDistanceVector[1] * fDistanceVector[1] +
             fDistanceVector[2] * fDistanceVector[2]) <
         (m_radius + BSphere.m_radius) * (m_radius + BSphere.m_radius);
}

/**
 * Merging of both spheres to a new bounding sphere.
 */
VistaBoundingSphere VistaBoundingSphere::Include(const VistaBoundingSphere& BSphere) {
  const float fDistanceVector[3] = {m_center[0] - BSphere.m_center[0],
      m_center[1] - BSphere.m_center[1], m_center[2] - BSphere.m_center[2]};

  const float fDistance2 = fDistanceVector[0] * fDistanceVector[0] +
                           fDistanceVector[1] * fDistanceVector[1] +
                           fDistanceVector[2] * fDistanceVector[2];

  const float R = m_radius + BSphere.m_radius;

  if (fDistance2 > R * R) {
    const float fDistance = sqrt(fDistance2);

    m_center[0] = (m_center[0] + BSphere.m_center[0] -
                      (m_radius - BSphere.m_radius) * fDistanceVector[0] / fDistance) *
                  0.5f;
    m_center[1] = (m_center[1] + BSphere.m_center[1] -
                      (m_radius - BSphere.m_radius) * fDistanceVector[1] / fDistance) *
                  0.5f;
    m_center[2] = (m_center[2] + BSphere.m_center[2] -
                      (m_radius - BSphere.m_radius) * fDistanceVector[2] / fDistance) *
                  0.5f;
    m_radius = (R + fDistance) * 0.5f;
  }

  return (*this);
}

VistaBoundingSphere VistaBoundingSphere::Include(const float Pnt[3]) {
  const float fDistanceVector[3] = {
      m_center[0] - Pnt[0], m_center[1] - Pnt[1], m_center[2] - Pnt[2]};

  const float fDistance2 = fDistanceVector[0] * fDistanceVector[0] +
                           fDistanceVector[1] * fDistanceVector[1] +
                           fDistanceVector[2] * fDistanceVector[2];

  if (fDistance2 > m_radius * m_radius) {
    const float fDistance = sqrt(fDistance2);

    m_center[0] = (m_center[0] + Pnt[0] - m_radius * fDistanceVector[0] / fDistance) * 0.5f;
    m_center[1] = (m_center[1] + Pnt[1] - m_radius * fDistanceVector[1] / fDistance) * 0.5f;
    m_center[2] = (m_center[2] + Pnt[2] - m_radius * fDistanceVector[2] / fDistance) * 0.5f;
    m_radius    = (m_radius + fDistance) * 0.5f;
  }

  return (*this);
}

/**
 * The closest point lying on the ray/line segment towards the bounding
 * sphere's center is estimated. If the distance between this closest point
 * and the sphere's center is smaller than the sphere's radius, an intersection
 * exists.
 */
bool VistaBoundingSphere::Intersects(
    const float origin[3], const float direction[3], const bool isRay, const float epsilon) const {
  float directionNorm[3];
  directionNorm[0] = direction[0];
  directionNorm[1] = direction[1];
  directionNorm[2] = direction[2];

  const float sqrlen =
      (direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
  if (sqrlen < epsilon) {
    return Intersects(origin);
  }

  const float length         = (float)sqrt(sqrlen);
  const float inverse_length = 1.0f / length;
  directionNorm[0] *= inverse_length;
  directionNorm[1] *= inverse_length;
  directionNorm[2] *= inverse_length;

  // Estimate the closest point on the line to the center.
  const float fVectorOriginToCenter[3] = {
      m_center[0] - origin[0], m_center[1] - origin[1], m_center[2] - origin[2]};

  float fParametricValue = fVectorOriginToCenter[0] * directionNorm[0] +
                           fVectorOriginToCenter[1] * directionNorm[1] +
                           fVectorOriginToCenter[2] * directionNorm[2];
  if (isRay == false) {
    // Clip the point back to the line segment.
    if (fParametricValue < 0.0f) {
      fParametricValue = 0.0f;
    } else if (fParametricValue > length) {
      fParametricValue = length;
    }
  }

  const float fClosestPoint[3]   = {origin[0] + fParametricValue * directionNorm[0],
      origin[1] + fParametricValue * directionNorm[1],
      origin[2] + fParametricValue * directionNorm[2]};
  const float fDistanceVector[3] = {m_center[0] - fClosestPoint[0], m_center[1] - fClosestPoint[1],
      m_center[2] - fClosestPoint[2]};
  return (fDistanceVector[0] * fDistanceVector[0] + fDistanceVector[1] * fDistanceVector[1] +
             fDistanceVector[2] * fDistanceVector[2]) < (m_radius * m_radius);
}

//============================================================================
//  END OF FILE VistaBoundingSphere.cpp
//============================================================================
