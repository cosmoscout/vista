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

#include <cmath>
#include <iostream>
#include <algorithm>
#include "VistaBoundingBox.h"

#include <VistaBase/VistaVectorMath.h>
#include <VistaBase/VistaStreamUtils.h>

//============================================================================
//  MAKROS AND DEFINES
//============================================================================

#define intersect(a1,a2,b1,b2)	((a1 <= b1) ? (a2>=b1) : (b2>=a1))

bool VistaBoundingBox::ComputeAABB( const VistaVector3D& v3Min,
						const VistaVector3D& v3Max,
						const VistaTransformMatrix& matTransform,
						VistaBoundingBox& oBBOut )
{
	VistaVector3D v[8];
	v[0] = VistaVector3D(v3Min);
	v[1] = VistaVector3D(v3Max[Vista::X], v3Min[Vista::Y], v3Min[Vista::Z]);
	v[2] = VistaVector3D(v3Min[Vista::X], v3Max[Vista::Y], v3Min[Vista::Z]);
	v[3] = VistaVector3D(v3Max[Vista::X], v3Max[Vista::Y], v3Min[Vista::Z]);
	v[4] = VistaVector3D(v3Min[Vista::X], v3Min[Vista::Y], v3Max[Vista::Z]);
	v[5] = VistaVector3D(v3Max[Vista::X], v3Min[Vista::Y], v3Max[Vista::Z]);
	v[6] = VistaVector3D(v3Min[Vista::X], v3Max[Vista::Y], v3Max[Vista::Z]);
	v[7] = VistaVector3D(v3Max);

	v[0] = matTransform * v[0];
	v[1] = matTransform * v[1];
	v[2] = matTransform * v[2];
	v[3] = matTransform * v[3];
	v[4] = matTransform * v[4];
	v[5] = matTransform * v[5];
	v[6] = matTransform * v[6];
	v[7] = matTransform * v[7];

	VistaVector3D v3MinTrans = matTransform * v3Min;
	VistaVector3D v3MaxTrans = matTransform * v3Max;

	for(unsigned i=0; i < 8; ++i)
	{
		if(v[i][Vista::X] < v3MinTrans[Vista::X])
			v3MinTrans[Vista::X] = v[i][Vista::X];
		if(v[i][Vista::X] > v3MaxTrans[Vista::X])
			v3MaxTrans[Vista::X] = v[i][Vista::X];	

		if(v[i][Vista::Y] < v3MinTrans[Vista::Y])
			v3MinTrans[Vista::Y] = v[i][Vista::Y];
		if(v[i][Vista::Y] > v3MaxTrans[Vista::Y])
			v3MaxTrans[Vista::Y] = v[i][Vista::Y];	

		if(v[i][Vista::Z] < v3MinTrans[Vista::Z])
			v3MinTrans[Vista::Z] = v[i][Vista::Z];
		if(v[i][Vista::Z] > v3MaxTrans[Vista::Z])
			v3MaxTrans[Vista::Z] = v[i][Vista::Z];	
	}

	oBBOut = VistaBoundingBox( v3MinTrans, v3MaxTrans );
	return true;
}


//============================================================================
//  CONSTRUCTORS / DESTRUCTOR
//============================================================================

VistaBoundingBox::VistaBoundingBox()	
{
}

VistaBoundingBox::VistaBoundingBox( const float a3fMin[3], const float a3fMax[3] )
: m_v3Min( a3fMin )
, m_v3Max( a3fMax )
{
}
VistaBoundingBox::VistaBoundingBox( const VistaVector3D& v3Min, const VistaVector3D& v3Max )
: m_v3Min( v3Min )
, m_v3Max( v3Max)
{
}


VistaBoundingBox::VistaBoundingBox( const float a3fVertexA[3],
								   const float a3fVertexB[3],
								   const float a3fVertexC[3] )
{
	m_v3Min[0] = std::min( a3fVertexA[0], std::min( a3fVertexB[0], a3fVertexC[0] ) );
	m_v3Min[1] = std::min( a3fVertexA[1], std::min( a3fVertexB[1], a3fVertexC[1] ) );
	m_v3Min[2] = std::min( a3fVertexA[2], std::min( a3fVertexB[2], a3fVertexC[2] ) );

	m_v3Max[0] = std::max( a3fVertexA[0], std::max( a3fVertexB[0], a3fVertexC[0] ) );
	m_v3Max[1] = std::max( a3fVertexA[1], std::max( a3fVertexB[1], a3fVertexC[1] ) );
	m_v3Max[2] = std::max( a3fVertexA[2], std::max( a3fVertexB[2], a3fVertexC[2] ) );
}

VistaBoundingBox::VistaBoundingBox( const VistaVector3D& v3VertexA,
									const VistaVector3D& v3VertexB, 
									const VistaVector3D& v3VertexC )
{
	m_v3Min[0] = std::min( v3VertexA[0], std::min( v3VertexB[0], v3VertexC[0] ) );
	m_v3Min[1] = std::min( v3VertexA[1], std::min( v3VertexB[1], v3VertexC[1] ) );
	m_v3Min[2] = std::min( v3VertexA[2], std::min( v3VertexB[2], v3VertexC[2] ) );

	m_v3Max[0] = std::max( v3VertexA[0], std::max( v3VertexB[0], v3VertexC[0] ) );
	m_v3Max[1] = std::max( v3VertexA[1], std::max( v3VertexB[1], v3VertexC[1] ) );
	m_v3Max[2] = std::max( v3VertexA[2], std::max( v3VertexB[2], v3VertexC[2] ) );
}

VistaBoundingBox::~VistaBoundingBox()
{
}

//============================================================================
//  IMPLEMENTATION
//============================================================================

void VistaBoundingBox::SetBounds(const float a3fMin[3], const float a3fMax[3])
{
	m_v3Min.SetValues( a3fMin );
	m_v3Max.SetValues( a3fMax );
}

void VistaBoundingBox::SetBounds( const VistaVector3D& v3Min, const VistaVector3D& v3Max )
{
	m_v3Min = v3Min;
	m_v3Max = v3Max;
}

void VistaBoundingBox::GetBounds( float a3fMin[3], float a3fMax[3] ) const
{
	m_v3Min.GetValues( a3fMin );
	m_v3Max.GetValues( a3fMax );
}

void VistaBoundingBox::GetBounds( VistaVector3D& v3Min, VistaVector3D& v3Max ) const
{
	v3Min = m_v3Min;
	v3Max = m_v3Max;
}


bool VistaBoundingBox::IsEmpty() const
{
	float width, height, depth;

	GetSize(width, height, depth);

	if(width == 0.0f && height == 0.0f && depth == 0.0f)
		return true;
	else
		return false;
}

void VistaBoundingBox::GetSize( float& nWidth, float& nHeight, float& nDepth ) const
{
	nWidth = m_v3Max[0] - m_v3Min[0];
	nHeight = m_v3Max[1] - m_v3Min[1];
	nDepth = m_v3Max[2] - m_v3Min[2];
}

VistaVector3D VistaBoundingBox::GetSize() const
{
	return ( m_v3Max - m_v3Min );
}

float VistaBoundingBox::GetDiagonalLength() const
{
	return GetSize().GetLength();
}

float VistaBoundingBox::GetVolume() const
{
	float nWidth, nHeight, nDepth;
	
	GetSize( nWidth, nHeight, nDepth );

	return ( nWidth * nHeight * nDepth );
}

void VistaBoundingBox::GetCenter( float a3fCenter[3] ) const
{
	VistaVector3D v3Center = GetCenter();
	v3Center.GetValues( a3fCenter );
}

VistaVector3D VistaBoundingBox::GetCenter() const
{
	return ( 0.5f * ( m_v3Min + m_v3Max ) );
}

VistaBoundingBox VistaBoundingBox::Expand( const float nSize )
{
	m_v3Min[0] -= nSize;
	m_v3Min[1] -= nSize;
	m_v3Min[2] -= nSize;
	m_v3Max[0] += nSize;
	m_v3Max[1] += nSize;
	m_v3Max[2] += nSize;
	return *this;
}

bool VistaBoundingBox::Intersects (const float a3fPoint[3]) const
{ 
	if (m_v3Min[0] <= a3fPoint[0] && a3fPoint[0] <= m_v3Max[0]
	 && m_v3Min[1] <= a3fPoint[1] && a3fPoint[1] <= m_v3Max[1]
	 && m_v3Min[2] <= a3fPoint[2] && a3fPoint[2] <= m_v3Max[2])
		return true;
	else
		return false;
}
bool VistaBoundingBox::Intersects( const VistaVector3D& v3Point ) const
{
	return Intersects( &v3Point[0] );
}


bool VistaBoundingBox::Contains (const VistaBoundingBox& oBBox) const
{
	if (m_v3Min[0] <= oBBox.m_v3Min[0] && oBBox.m_v3Max[0] <= m_v3Max[0]
	 && m_v3Min[1] <= oBBox.m_v3Min[1] && oBBox.m_v3Max[1] <= m_v3Max[1]
	 && m_v3Min[2] <= oBBox.m_v3Min[2] && oBBox.m_v3Max[2] <= m_v3Max[2])
		return true;
	else
		return false;
}

bool VistaBoundingBox::Contains( const VistaVector3D& v3Point ) const
{
	return (   m_v3Min[0] <= v3Point[0] && v3Point[0] <= m_v3Max[0]
			&& m_v3Min[1] <= v3Point[1] && v3Point[1] <= m_v3Max[1]
			&& m_v3Min[2] <= v3Point[2] && v3Point[2] <= m_v3Max[2] );
}

bool VistaBoundingBox::Intersects (const VistaBoundingBox& oBBox) const
{
	if(intersect(m_v3Min[0], m_v3Max[0], oBBox.m_v3Min[0], oBBox.m_v3Max[0])
	&& intersect(m_v3Min[1], m_v3Max[1], oBBox.m_v3Min[1], oBBox.m_v3Max[1])
	&& intersect(m_v3Min[2], m_v3Max[2], oBBox.m_v3Min[2], oBBox.m_v3Max[2]))
		return true;
	else
		return false;
}

VistaBoundingBox VistaBoundingBox::Include(const VistaBoundingBox& BBox)
{
	if (BBox.m_v3Min[0] < m_v3Min[0])
	{
		m_v3Min[0] = BBox.m_v3Min[0];
	}
	if (BBox.m_v3Max[0] > m_v3Max[0])
	{
		m_v3Max[0] = BBox.m_v3Max[0];
	}
	if (BBox.m_v3Min[1] < m_v3Min[1])
	{
		m_v3Min[1] = BBox.m_v3Min[1];
	}
	if (BBox.m_v3Max[1] > m_v3Max[1])
	{
		m_v3Max[1] = BBox.m_v3Max[1];
	}
	if (BBox.m_v3Min[2] < m_v3Min[2])
	{
		m_v3Min[2] = BBox.m_v3Min[2];
	}
	if (BBox.m_v3Max[2] > m_v3Max[2])
	{
		m_v3Max[2] = BBox.m_v3Max[2];
	}
	return(*this);
}

VistaBoundingBox VistaBoundingBox::Include( const float a3fPoint[3] )
{
	if (a3fPoint[0] < m_v3Min[0])
	{
		m_v3Min[0] = a3fPoint[0];
	}
	if (a3fPoint[0] > m_v3Max[0])
	{
		m_v3Max[0] = a3fPoint[0];
	}
	if (a3fPoint[1] < m_v3Min[1])
	{
		m_v3Min[1] = a3fPoint[1];
	}
	if (a3fPoint[1] > m_v3Max[1])
	{
		m_v3Max[1] = a3fPoint[1];
	}
	if (a3fPoint[2] < m_v3Min[2])
	{
		m_v3Min[2] = a3fPoint[2];
	}
	if (a3fPoint[2] > m_v3Max[2])
	{
		m_v3Max[2] = a3fPoint[2];
	}
	return(*this);
}

VistaBoundingBox VistaBoundingBox::Include( const VistaVector3D& v3Point )
{
	return Include( &v3Point[0] );
}

bool VistaBoundingBox::Intersects(const float origin[3], 
									const float direction[3], 
									const bool isRay,
									const float epsilon) const
{
	float directionNorm[3];
	directionNorm[0] = direction[0];
	directionNorm[1] = direction[1];
	directionNorm[2] = direction[2];

	// If the computation is done for a ray, normalize the
	// direction vector first.
	if (isRay == true)
	{
		const float sqrlen = (direction[0]*direction[0]
							+ direction[1]*direction[1]
							+ direction[2]*direction[2]);
		if (sqrlen>0)
		{
			const float inverse_length = 1.0f/(float)sqrt(sqrlen);
			directionNorm[0] *= inverse_length;
			directionNorm[1] *= inverse_length;
			directionNorm[2] *= inverse_length;
		}
	}

	float tmin = 0;
	float tmax = 0;
	bool init = true;
	// Check all three axis one by one.
	for(int i=0; i<3; ++i)
	{
		if(std::abs(directionNorm[i]) > epsilon)
		{
			// Compute the parametric values for the intersection
			// points of the line and the bounding box according
			// to the current axis only.
			float tmpmin = (m_v3Min[i] - origin[i]) / directionNorm[i];
			float tmpmax = (m_v3Max[i] - origin[i]) / directionNorm[i];

			if (tmpmin > tmpmax)
			{
				// Switch tmpmin and tmpmax.
				const float tmp = tmpmin;
				tmpmin = tmpmax;
				tmpmax = tmp;
			}
			if (init)
			{
				tmin = tmpmin;
				tmax = tmpmax;

				if (tmax < -epsilon)
					return 0;
				if (tmin < 0)
					tmin = 0;

				if(!isRay) // is a line segment
				{
					// First intersection is outside the scope of
					// the line segment.
					if(tmin > 1 + epsilon)
						return 0;
					if(tmax > 1)
						tmax = 1;
				}

				init = false;
			}
			else
			{
				// This is the regular check if the direction
				// vector is non-zero along the current axis.
				if(tmpmin > tmax + epsilon)
					return 0;
				if(tmpmax < tmin - epsilon)
					return 0;
				if(tmpmin > tmin)
					tmin = tmpmin;
				if(tmpmax < tmax)
					tmax = tmpmax;
			}
		}
		else // line parallel to box
		{
			// If the ray or line segment is parallel to an axis
			// and has its origin outside the box's std::min and std::max
			// coordinate for that axis, the ray/line cannot hit
			// the box.
			if ((origin[i] < m_v3Min[i] - epsilon)
				||	(origin[i] > m_v3Max[i] + epsilon))
			{
				return 0;
			}
		}
	}
	return(true);
}

bool VistaBoundingBox::Intersects( const VistaVector3D& v3Origin,
								  const VistaVector3D& v3Direction,
								  const bool bIsRay, const float nEpsilon ) const
{
	return Intersects( &v3Origin[0], &v3Direction[0], bIsRay, nEpsilon );
}

bool VistaBoundingBox::GetIntersectionDistance(const float origin[3], 
									const float direction[3], 
									const bool isRay,
									float& fMinIntersection,
									float& fMaxIntersection,
									const float epsilon) const
{
	float directionNorm[3];
	directionNorm[0] = direction[0];
	directionNorm[1] = direction[1];
	directionNorm[2] = direction[2];

	// If the computation is done for a ray, normalize the
	// direction vector first.
	if (isRay == true)
	{
		const float sqrlen = (direction[0]*direction[0]
							+ direction[1]*direction[1]
							+ direction[2]*direction[2]);
		if (sqrlen>0)
		{
			const float inverse_length = 1.0f/(float)sqrt(sqrlen);
			directionNorm[0] *= inverse_length;
			directionNorm[1] *= inverse_length;
			directionNorm[2] *= inverse_length;
		}
	}

	float tmin = 0;
	float tmax = 0;
	bool init = true;
	// Check all three axis one by one.
	for(int i=0; i<3; ++i)
	{
		if(std::abs(directionNorm[i]) > epsilon)
		{
			// Compute the parametric values for the intersection
			// points of the line and the bounding box according
			// to the current axis only.
			float tmpmin = (m_v3Min[i] - origin[i]) / directionNorm[i];
			float tmpmax = (m_v3Max[i] - origin[i]) / directionNorm[i];

			if (tmpmin > tmpmax)
			{
				// Switch tmpmin and tmpmax.
				const float tmp = tmpmin;
				tmpmin = tmpmax;
				tmpmax = tmp;
			}
			if (init)
			{
				tmin = tmpmin;
				tmax = tmpmax;

				if (tmax < -epsilon)
					return false;
				if (tmin < 0)
					tmin = 0;

				if(!isRay) // is a line segment
				{
					// First intersection is outside the scope of
					// the line segment.
					if(tmin > 1 + epsilon)
						return false;
					if(tmax > 1)
						tmax = 1;
				}

				init = false;
			}
			else
			{
				// This is the regular check if the direction
				// vector is non-zero along the current axis.
				if(tmpmin > tmax + epsilon)
					return false;
				if(tmpmax < tmin - epsilon)
					return false;
				if(tmpmin > tmin)
					tmin = tmpmin;
				if(tmpmax < tmax)
					tmax = tmpmax;
			}
		}
		else // line parallel to box
		{
			// If the ray or line segment is parallel to an axis
			// and has its origin outside the box's std::min and std::max
			// coordinate for that axis, the ray/line cannot hit
			// the box.
			if ((origin[i] < m_v3Min[i] - epsilon)
				||	(origin[i] > m_v3Max[i] + epsilon))
			{
				return false;
			}
		}
	}
	fMinIntersection = tmin;
	fMaxIntersection = tmax;
	return(true);
}

bool VistaBoundingBox::GetIntersectionDistance( const VistaVector3D& v3Origin,
								const VistaVector3D& v3Direction,
								const bool bIsRay, float& fMinIntersection,
								float& fMaxIntersection, const float nEpsilon ) const
{
	return GetIntersectionDistance( &v3Origin[0], &v3Direction[0], bIsRay, 
								fMinIntersection, fMaxIntersection, nEpsilon );
}

/**
 * Intersection of the bounding-box with the sphere given by position and radius.
 * \param fPosition The position of the center of the sphere
 * \param fRadius The radius of the sphere
 * \return bool Do the bounding box and the sphere intersect?
 * */
bool VistaBoundingBox::IntersectsSphere
		(const float fPosition[3], const float fRadius) const
{
	int iTypeOfIntersection[3] = { 0, 0, 0 };
	// detection by exclusion
	for(int iCtr = 0; iCtr < 3; ++iCtr)
	{
		if(fPosition[iCtr] < m_v3Min[iCtr] - fRadius || m_v3Max[iCtr] + fRadius < fPosition[iCtr])
			return false;
		else
			if(m_v3Min[iCtr] <= fPosition[iCtr] && fPosition[iCtr] <= m_v3Max[iCtr])
				iTypeOfIntersection[iCtr] = 1;
	}
	// okay, if we reached this point, we have determined that the center of
	// the sphere is not outside the cube given by the boundingbox extended by
	// the radius of the sphere. There's no trouble if the center of the sphere
	// is inside the boundingbox (case 3 of the following switch statement),
	// but if not one has to take into account a sphere is tested against the
	// box and at the edges of the boundingbox there are special cases.

	switch(iTypeOfIntersection[0] + iTypeOfIntersection[1] + iTypeOfIntersection[2])
	{
		case 3: // center of sphere inside the bounding box
		case 2: // center of sphere inside the extensions of the side of the bounding box
			return true;
			break;
		case 1: // center of sphere inside the extension along the edge of the bounding box
			// determine the edge which is a candidate for intersection
		{
			float fPoint[3];
			float fDirection[3] = { 0.0f, 0.0f, 0.0f };
			int iEdge[3];
			iEdge[0] = (iTypeOfIntersection[0] == 1) ? 0 : ((iTypeOfIntersection[1] == 1) ? 1 : 2);
			iEdge[1] = (iEdge[0] + 1)%3;
			iEdge[2] = (iEdge[0] + 2)%3;
			fDirection[iEdge[0]] = 1.0f;
			fPoint[iEdge[0]] = m_v3Min[iEdge[0]];
			if(fPosition[iEdge[1]] < m_v3Min[iEdge[1]])	fPoint[iEdge[1]] = m_v3Min[iEdge[1]];
			else										fPoint[iEdge[1]] = m_v3Max[iEdge[1]];
			if(fPosition[iEdge[2]] < m_v3Min[iEdge[2]])	fPoint[iEdge[2]] = m_v3Min[iEdge[2]];
			else										fPoint[iEdge[2]] = m_v3Max[iEdge[2]];
			// a sphere is given by (p-c)(p-c)-r^2 = 0 and a ray by (o+td) = p
			// => (o+td-c)(o+td-c) - r^2 = 0
			// => At^2+Bt+C = 0 with A = dd, B = 2(o-c)d, C = (o-c)(o-c)-r^2
			// => if B^2 - 4AC >= 0 then there's an intersection
			// idea from http://www.geometrictools.com
			float fDiff[3] = { fPoint[0]-fPosition[0], fPoint[1]-fPosition[1], fPoint[2]-fPosition[2] };
			float fA0 = fDiff[0]*fDiff[0] + fDiff[1]*fDiff[1] + fDiff[2]*fDiff[2] - fRadius*fRadius;
			float fA1 = fDirection[0]*fDiff[0] + fDirection[1]*fDiff[1] + fDirection[2]*fDiff[2];
			if(fA1*fA1 - fA0 >= 0.0f)
				return true;
			break;
		}
		case 0: // center of sphere inside the extension of a corner of the bounding box
			// determine corner next to the position of the sphere
		{
			float fCorner[3] = { m_v3Min[0], m_v3Min[1], m_v3Min[2] };
			for(int iCtr = 0; iCtr < 3; ++iCtr)
				if(fPosition[iCtr] > m_v3Min[iCtr]) fCorner[iCtr] = m_v3Max[iCtr];
			// compute the distance from the corner to the center of the sphere
			float fSqrDistance =
					(fCorner[0] - fPosition[0]) * (fCorner[0] - fPosition[0]) +
					(fCorner[1] - fPosition[1]) * (fCorner[1] - fPosition[1]) +
					(fCorner[2] - fPosition[2]) * (fCorner[2] - fPosition[2]);
			// if the distance (attention there's no square root therefore
			// we've to check against the squared radius) is smaller than the
			// squared radius there is an intersection
			if(fSqrDistance <= fRadius * fRadius)
				return true;
			break;
		}
	}
	return false;
}

bool VistaBoundingBox::IntersectsSphere( const VistaVector3D& v3Center, const float nRadius ) const
{
	return IntersectsSphere( &v3Center[0], nRadius );
}

bool VistaBoundingBox::IntersectsBox
	(const float fTransformation[16],
	 const float fWidth, const float fHeight, const float fDepth) const
{
	// some variables needed later on
	const float fMyExtend[3] = {	(m_v3Max[0] - m_v3Min[0])/2.0f,
									(m_v3Max[1] - m_v3Min[1])/2.0f,
									(m_v3Max[2] - m_v3Min[2])/2.0f };
	const  float fHisExtend[3] = {	fWidth /2.0f,
									fHeight/2.0f,
									fDepth /2.0f };
	const float fMyCenter[3]={	m_v3Min[0] + fMyExtend[0],
								m_v3Min[1] + fMyExtend[1],
								m_v3Min[2] + fMyExtend[2] };
	const float fHisCenter[3]={	fTransformation[3],
								fTransformation[7],
								fTransformation[11] };

	// compute the axis of the rotated box
	float fHisAxis[3][3] = {{ fTransformation[ 0], fTransformation[ 4], fTransformation[ 8] },
							{ fTransformation[ 1], fTransformation[ 5], fTransformation[ 9] },
							{ fTransformation[ 2], fTransformation[ 6], fTransformation[10] } };
	float fMyAxis[3][3] = {	{ 1.0f, 0.0f, 0.0f },
							{ 0.0f, 1.0f, 0.0f },
							{ 0.0f, 0.0f, 1.0f } };
	// determine the bounding box of the rotated box
	float fMin[3] = { fHisCenter[0], fHisCenter[1], fHisCenter[2] },
		  fMax[3] = { fHisCenter[0], fHisCenter[1], fHisCenter[2] };

	int iCtrAxis = 0;
	for(iCtrAxis = 0; iCtrAxis < 3; ++iCtrAxis)
		for(int iCtr = 0; iCtr < 3; ++iCtr)
		{
			if(fHisAxis[iCtrAxis][iCtr] > 0.0f)
			{
				fMin[iCtr] -= fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
				fMax[iCtr] += fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
			}
			else
			{
				fMin[iCtr] += fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
				fMax[iCtr] -= fHisExtend[iCtr]*fHisAxis[iCtrAxis][iCtr];
			}
		}
	if(!(m_v3Min[0] <= fMax[0] && fMin[0] <= m_v3Max[0])) return false;
	if(!(m_v3Min[1] <= fMax[1] && fMin[1] <= m_v3Max[1])) return false;
	if(!(m_v3Min[2] <= fMax[2] && fMin[2] <= m_v3Max[2])) return false;

	const float fMyPoints[8][3] = { { m_v3Min[0], m_v3Min[1], m_v3Min[2] },
									{ m_v3Min[0], m_v3Min[1], m_v3Max[2] },
									{ m_v3Min[0], m_v3Max[1], m_v3Min[2] },
									{ m_v3Min[0], m_v3Max[1], m_v3Max[2] },
									{ m_v3Max[0], m_v3Min[1], m_v3Min[2] },
									{ m_v3Max[0], m_v3Min[1], m_v3Max[2] },
									{ m_v3Max[0], m_v3Max[1], m_v3Min[2] },
									{ m_v3Max[0], m_v3Max[1], m_v3Max[2] } };

	for(iCtrAxis = 0; iCtrAxis < 3; ++iCtrAxis)
	{
		// compute the scalar product of all relevant points
		ProjectBoxToAxis(fHisCenter, fHisAxis[iCtrAxis], fMyPoints, fMin[iCtrAxis], fMax[iCtrAxis]);
		if(!(fMin[iCtrAxis] <= fHisExtend[iCtrAxis] && -fHisExtend[iCtrAxis] <= fMax[iCtrAxis]))
			return false;
	}

	float fExtendedAxis[3][3] = { {	fTransformation[ 0]*fHisExtend[0],
									fTransformation[ 4]*fHisExtend[0],
									fTransformation[ 8]*fHisExtend[0] },
								  { fTransformation[ 1]*fHisExtend[1],
									fTransformation[ 5]*fHisExtend[1],
									fTransformation[ 9]*fHisExtend[1] },
								  { fTransformation[ 2]*fHisExtend[2],
									fTransformation[ 6]*fHisExtend[2],
									fTransformation[10]*fHisExtend[2] } };

	const float fHisPoints[8][3] = {
		{
			fHisCenter[0]+fExtendedAxis[0][0]+fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]+fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]+fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]+fExtendedAxis[0][0]+fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]+fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]+fExtendedAxis[1][2]-fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]+fExtendedAxis[0][0]-fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]-fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]-fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]+fExtendedAxis[0][0]-fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]+fExtendedAxis[0][1]-fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]+fExtendedAxis[0][2]-fExtendedAxis[1][2]-fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]+fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]+fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]+fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]+fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]+fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]+fExtendedAxis[1][2]-fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]-fExtendedAxis[1][0]+fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]-fExtendedAxis[1][1]+fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]-fExtendedAxis[1][2]+fExtendedAxis[2][2],
		},
		{
			fHisCenter[0]-fExtendedAxis[0][0]-fExtendedAxis[1][0]-fExtendedAxis[2][0],
			fHisCenter[1]-fExtendedAxis[0][1]-fExtendedAxis[1][1]-fExtendedAxis[2][1],
			fHisCenter[2]-fExtendedAxis[0][2]-fExtendedAxis[1][2]-fExtendedAxis[2][2] 
		} };

	for(int iCtrAxisA = 0; iCtrAxisA < 3; ++iCtrAxisA)
		for(int iCtrAxisB = 0; iCtrAxisB < 3; ++iCtrAxisB)
		{
			const float fAxis[3] = {	fMyAxis[iCtrAxisA][1]*fHisAxis[iCtrAxisB][2] +
										fMyAxis[iCtrAxisA][2]*fHisAxis[iCtrAxisB][1],
										fMyAxis[iCtrAxisA][0]*fHisAxis[iCtrAxisB][2] +
										fMyAxis[iCtrAxisA][2]*fHisAxis[iCtrAxisB][0],
										fMyAxis[iCtrAxisA][0]*fHisAxis[iCtrAxisB][1] +
										fMyAxis[iCtrAxisA][1]*fHisAxis[iCtrAxisB][0] };
			// cross product equals zero-vector?? If true the axis are parallel ...
			bool bAxisParallel = false;
			float epsilon = 1e-5f;
			if(-epsilon < fAxis[0] && fAxis[0] < epsilon)
				if(-epsilon < fAxis[0] && fAxis[0] < epsilon)
					if(-epsilon < fAxis[0] && fAxis[0] < epsilon)
						bAxisParallel = true;

			if(! bAxisParallel)
			{
				ProjectBoxToAxis(fMyCenter, fAxis, fMyPoints,  fMin[0], fMax[0]);
				ProjectBoxToAxis(fMyCenter, fAxis, fHisPoints, fMin[1], fMax[1]);
				if(!(fMin[0] <= fMax[1] && fMin[1] <= fMax[0]))
				{
					vstr::outi() << fMin[0] << " " << fMax[0] << "|" << fMin[1] << " " << fMax[1] << std::endl;
					return false;
				}
			}
		}
	return true;
}

bool VistaBoundingBox::IntersectsBox( const VistaTransformMatrix& matTransform, 
									 const float nWidth, const float nHeight, const float nDepth ) const
{
	float a16fTransform[16]; 
	matTransform.GetValues( a16fTransform );
	return IntersectsBox( a16fTransform, nWidth, nHeight, nDepth );
}

bool VistaBoundingBox::IntersectsBox( const VistaTransformMatrix& matTransform,
									 const VistaVector3D& v3Extents ) const
{
	float a16fTransform[16]; 
	matTransform.GetValues( a16fTransform );
	return IntersectsBox( a16fTransform, v3Extents[0], v3Extents[1], v3Extents[2] );
}

/** \brief A helper function to project a box to an axis
 * The box is project to the given axis and the minimum and maximum values are returned.
 * \param float[3] The origin of the axis
 * \param float[3] The direction of the axis
 * \param float[8][3] The points of the box
 * \param float The minimum (call by reference!)
 * \param float The maximum (call by reference!)
 **/
void VistaBoundingBox::ProjectBoxToAxis
	(const float fOrigin[3], const float fAxis[3],
	 const float fBoxPoints[8][3],
	 float& fMin, float& fMax) const
{
	float fScalar[8];
	float fTBoxPoints[8][3];

	int iCtr = 0;
	for(iCtr = 0; iCtr < 8; ++iCtr)
	{
		fTBoxPoints[iCtr][0] = fBoxPoints[iCtr][0] - fOrigin[0];
		fTBoxPoints[iCtr][1] = fBoxPoints[iCtr][1] - fOrigin[1];
		fTBoxPoints[iCtr][2] = fBoxPoints[iCtr][2] - fOrigin[2];
	}

	float fNAxis[3]; // the normalized Axis
	float fLength = fAxis[0]*fAxis[0] + fAxis[1]*fAxis[1] + fAxis[2]*fAxis[2];
	fLength = sqrt(fLength);
	fNAxis[0] = fAxis[0] / fLength;
	fNAxis[1] = fAxis[1] / fLength;
	fNAxis[2] = fAxis[2] / fLength;

	for(iCtr = 0; iCtr < 8; ++iCtr)
		fScalar[iCtr] =
			fTBoxPoints[iCtr][0]*fNAxis[0] +
			fTBoxPoints[iCtr][1]*fNAxis[1] +
			fTBoxPoints[iCtr][2]*fNAxis[2];
	fMin = fScalar[0];
	fMax = fScalar[0];
	for(iCtr = 1; iCtr < 8; ++iCtr)
	{
		if(fScalar[iCtr] < fMin)
			fMin = fScalar[iCtr];
		else if(fMax < fScalar[iCtr])
			fMax = fScalar[iCtr];
	}
}

int VistaBoundingBox::Intersection(const float origin[3], 
									const float direction[3], 
									const bool isRay,
									float isect1[3], float& delta1, 
									float isect2[3], float& delta2,
									const float epsilon) const
{
	float directionNorm[3];
	directionNorm[0] = direction[0];
	directionNorm[1] = direction[1];
	directionNorm[2] = direction[2];

	// If the computation is done for a ray, normalize the
	// direction vector first.
	if (isRay == true)
	{
		const float sqrlen = (direction[0]*direction[0]
							+ direction[1]*direction[1]
							+ direction[2]*direction[2]);
		if (sqrlen>0)
		{
			const float inverse_length = 1.0f/(float)sqrt(sqrlen);
			directionNorm[0] *= inverse_length;
			directionNorm[1] *= inverse_length;
			directionNorm[2] *= inverse_length;
		}
	}

	float tmin = 0;
	float tmax = 0;
	bool init = true;
	// Check all three axis one by one.
	for(int i=0; i<3; ++i)
	{
		if(std::abs(directionNorm[i]) > epsilon)
		{
			// Compute the parametric values for the intersection
			// points of the line and the bounding box according
			// to the current axis only.
			float tmpmin = (m_v3Min[i] - origin[i]) / directionNorm[i];
			float tmpmax = (m_v3Max[i] - origin[i]) / directionNorm[i];

			if (tmpmin > tmpmax)
			{
				// Switch tmpmin and tmpmax.
				const float tmp = tmpmin;
				tmpmin = tmpmax;
				tmpmax = tmp;
			}
			if (init)
			{
				tmin = tmpmin;
				tmax = tmpmax;

				if (tmax < -epsilon)
					return 0;
				if (tmin < 0)
					tmin = 0;

				if(!isRay) // is a line segment
				{
					// First intersection is outside the scope of
					// the line segment.
					if(tmin > 1 + epsilon)
						return 0;
					if(tmax > 1)
						tmax = 1;
				}

				init = false;
			}
			else
			{
				// This is the regular check if the direction
				// vector is non-zero along the current axis.
				if(tmpmin > tmax + epsilon)
					return 0;
				if(tmpmax < tmin - epsilon)
					return 0;
				if(tmpmin > tmin)
					tmin = tmpmin;
				if(tmpmax < tmax)
					tmax = tmpmax;
			}
		}
		else // line parallel to box
		{
			// If the ray or line segment is parallel to an axis
			// and has its origin outside the box's std::min and std::max
			// coordinate for that axis, the ray/line cannot hit
			// the box.
			if ((origin[i] < m_v3Min[i] - epsilon)
				||	(origin[i] > m_v3Max[i] + epsilon))
			{
				return 0;
			}
		}
	}

	// If init is still true, the vector must be of zero length.
	if (init == true)
	{
		// If the direction vector is of zero length (the line segment/ray is
		// just a single point) and it lies inside the box, make sure that the
		// method indicates this as one intersection point.
		tmin = tmax = 0.0f;
	}

	// Calculate both intersection points according to the
	// computed parameteric values.
	isect1[0] = origin[0] + tmin*directionNorm[0];
	isect1[1] = origin[1] + tmin*directionNorm[1];
	isect1[2] = origin[2] + tmin*directionNorm[2];

	isect2[0] = origin[0] + tmax*directionNorm[0];
	isect2[1] = origin[1] + tmax*directionNorm[1];
	isect2[2] = origin[2] + tmax*directionNorm[2];

	delta1 = tmin;
	delta2 = tmax;

	if( std::abs(tmin-tmax) < epsilon )
		return 1;
	else
		return 2;
}

//============================================================================
//  end OF FILE VistaBoundingBox.cpp
//============================================================================
