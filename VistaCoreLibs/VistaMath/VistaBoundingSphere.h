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


#ifndef VISTABOUNDINGSPHERE_H
#define VISTABOUNDINGSPHERE_H

//============================================================================
//  INCLUDES
//============================================================================
#include "VistaMathConfig.h"

//============================================================================
//  FORWARD DECLARATIONS
//============================================================================

//============================================================================
//  MAKROS AND DEFINES
//============================================================================


//============================================================================
//  CLASS DEFINITIONS
//============================================================================

/// Representation of a bounding sphere.
class VISTAMATHAPI VistaBoundingSphere
{
public:
	/// Default constructor.
	VistaBoundingSphere();
	/// Constructor for a triangle.
	VistaBoundingSphere(const float a[3], const float b[3], const float c[3]);
	/// Destructor.
	~VistaBoundingSphere();

	/// Test if the bounding sphere is of zero volume.
	bool IsEmpty() const;
	/// Increase the radius of the bounding sphere.
	VistaBoundingSphere Expand(const float size);

	/// Get the radius of the sphere.
	float GetRadius() const;
	/// Get the volume of the bounding sphere.
	float GetVolume() const;
	/// Get the center of the bounding sphere.
	void GetCenter(float center[3]) const;

	/// Test if the specified point lies inside the bounding sphere.
	bool Intersects (const float point[3]) const;
	/// Test intersection with a second sphere.
	bool Intersects (const VistaBoundingSphere& BSphere) const;
	/// Test intersection with a ray or line segment.
	bool Intersects(const float origin[3],
					 const float direction[3],
					 const bool isRay,
					 const float epsilon=0.00001f) const;

	/// Resize the bounding sphere such that the specified sphere is included.
	VistaBoundingSphere Include(const VistaBoundingSphere& BBox);
	VistaBoundingSphere Include(const float Pnt[3]);

public:
	/// Center of the sphere.
	float	m_center[3];
	/// Radius of the sphere
	float	m_radius;
};

//============================================================================
//  LOCAL VARS AND FUNCS
//============================================================================

//============================================================================
//  END OF FILE VistaCollisionOctree.h
//============================================================================

#endif // VISTABOUNDINGSPHERE_H
