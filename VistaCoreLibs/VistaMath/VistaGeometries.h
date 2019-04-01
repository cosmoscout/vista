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


#ifndef _VISTAGEOMETRIES_H
#define _VISTAGEOMETRIES_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <vector>
#include <VistaBase/VistaVectorMath.h>
#include "VistaMathConfig.h"
#include "VistaVector.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                        */
/*============================================================================*/

//#define Vista::Epsilon std::numeric_limits<float>::epsilon()

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaRay;
class VistaCurveSegment;
class VistaLineSegment;
class VistaNURBSCurveSegment;
class VistaSplineSegment;
class VistaCurve;
class VistaSplineCurve;
class VistaSurface;
class VistaTriangle;
class VistaPlane;
class VistaPolygon;
class VistaPolygonWithHoles;
class VistaSphericalSurface;
class VistaCylindricalSurface;
class VistaNURBSSurface;
class VistaExtrusion;
class VistaLinearExtrusion;
class VistaCurveExtrusion;
class VistaRotationalExtrusion;
class VistaSolid;
class VistaTetrahedron;

/*============================================================================*/
// VISTA RAY
/*============================================================================*/

/**
 * \brief Vector repesentation of a ray.
 */
class VISTAMATHAPI VistaRay
{
public:

	// CONSTRUCTOR
	VistaRay() :
	  m_Dir(VistaVector3D(0.0f,0.0f,1.0f))
	{}
	VistaRay ( const VistaVector3D & origin, const VistaVector3D & dir ) : m_Origin ( origin ), m_Dir ( dir ) {};

	// IMPLEMENTION
	void            SetOrigin ( const VistaVector3D & org )   {m_Origin = org;}
	VistaVector3D   GetOrigin () const                        {return m_Origin;}
	void            SetDir    ( const VistaVector3D & dir )  {m_Dir = dir;}
	VistaVector3D  GetDir    () const                        {return m_Dir;}

	VistaVector3D   GetPointByParameter        ( const float          paramT ) const;

	/**
	 * \brief Intersection test between the ray and a triangle.
	 *
	 * Tests if the ray and the triangle (specified by the coordinates
	 * a,b,c) intersect. If they do, the method returns true and the reference
	 * parameters are filled with the computed values for barycentric coordinates,
	 * face normal, penetration depth and contact point.
	 *
	 * The implementation is based on the algorithm from the book
	 * Real-Time Rendering, M�ller & Haines, p. 303-305.
	 *
	 * \note Since floating precision is used for the calcuation, an epsilon
	 * of about 10e-5 should be used to attribute for floating precision
	 * errors (i.e. receiving -2.1e-8 instead of 0.0).
	 */
	bool IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b, const VistaVector3D& c,
							 VistaVector3D& contactPoint,
							 const float epsilon=0.00001f) const;

	bool IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b, const VistaVector3D& c,
							 VistaVector3D& contactPoint, VistaVector3D& faceNormal,
							 float& penetrationDepth, float& barycentric_ab, float& barycentric_ac,
							 const float epsilon=0.00001f) const;

	// MEMBERS

private:
	/// Origin of the ray.
	VistaVector3D  m_Origin;
	/// Direction vector of the ray (doesn't have to be normalized).
	VistaVector3D  m_Dir;
};


/*============================================================================*/
// VISTA CURVE SEGMENT
/*============================================================================*/

/**
 * A curve segment has a start and an end point.
 */
class VISTAMATHAPI VistaCurveSegment
{
public:
	// CONSTRUCTORS
	VistaCurveSegment () {};
	VistaCurveSegment (const VistaVector3D & startPnt, const VistaVector3D & endPnt )
	: m_StartPoint( startPnt ), m_EndPoint ( endPnt ) {}

	// DESTRUCTORS
	virtual ~VistaCurveSegment() {}

	// IMPLEMENTATION
	void           SetStartPoint ( const VistaVector3D & startPnt ) { m_StartPoint = startPnt; }
	VistaVector3D  GetStartPoint () const                           { return m_StartPoint;     }
	void           SetEndPoint ( const VistaVector3D & endPnt )  { m_EndPoint = endPnt; }
	VistaVector3D  GetEndPoint () const                          { return m_EndPoint;   }

	// SOME SPECIAL METHODS
	virtual VistaVector3D   GetPointByParameter        ( const float            paramT ) const =0;
	virtual float          GetParameterOfNearestPoint ( const VistaVector3D &   testPnt) const =0;
	virtual float           GetLength                  () const                                 =0;

	// MEMBERS
public:
	// a curve segment has a start and an end point
	VistaVector3D     m_StartPoint;
	VistaVector3D     m_EndPoint;
};


/*============================================================================*/
// VISTA LINE SEGMENT
/*============================================================================*/

/**
 * \brief Representation of a line segment.
 *
 */
class VISTAMATHAPI VistaLineSegment : public VistaCurveSegment
{
public:
	// CONSTRUCTORS
	/// Default constructor.
	VistaLineSegment () {};
	/// Constructor with initialization of members.
	VistaLineSegment ( const VistaVector3D & startPnt, const VistaVector3D & endPnt )
	: VistaCurveSegment ( startPnt, endPnt ) {}

	// OPERATIONS
	/// Copy constructor.
	void operator= (const VistaRay &NewLn);

	// IMPLEMENTATION

	// SOME SPECIAL METHODS
	VistaVector3D   GetPointByParameter        ( const float          paramT ) const;
	float           GetParameterOfNearestPoint ( const VistaVector3D & testPnt) const;
	float           GetLength                  () const;
	/**
	 * \brief Checks if the specified segment lies in a proximity of fProximity of this segment.
	 *
	 * Implementation based on the paper "Robust Treatment of Collisions, Contact and
	 * Friction for Cloth Animation" by Bridson, Fedkiw and Anderson, page 4. First
	 * checks if the two lines are parallel or one of the lines has a close to zero
	 * length. If the situation is not one of those degenerate cases. A 2x2 equation
	 * system is solved to compute the parametric values of the two closest points
	 * lying on infinite lines associated to the two segments. The points are clipped
	 * onto the range of the line segments and a finally tested if their distance is
	 * less than the specified proximity (fEpsilon is used to count boundary cases as
	 * rather inside the proximity opposed to outside of it and to handle floating
	 * precision errors in general).
	 *
	 * If the answer is true, fBarycentricThisSegment and fBarycentricOtherSegment
	 * contain the barycentric/parametric coordinates of the two clostest points. If
	 * the answer is false, the value of those parameters if undefined.
	 */
	bool			ProximityContains(const VistaLineSegment &segment,
									  const float fProximity,
									  float& fBarycentricThisSegment,
									  float& fBarycentricOtherSegment,
									  const float fEpsilon = 1.0e-5f) const;

	/**
	 * \brief Intersection test between the line segment and a triangle.
	 *
	 * Tests if the line segment and the triangle specified by the coordinates
	 * a,b,c intersect. If they do, the method returns true and the reference
	 * parameters are filled with the computed values for barycentric coordinates,
	 * face normal and contact point.
	 *
	 * The implementation is based on the algorithm from the book
	 * Real-Time Rendering, M�ller & Haines, p. 303-305.
	 *
	 * \note Since floating precision is used for the calcuation, an epsilon
	 * of about 10e-5 should be used to attribute for floating precision
	 * errors (i.e. receiving 2.1e-8 instead of 0.0).
	 */
	bool IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b, const VistaVector3D& c,
							 VistaVector3D& contactPoint,
							 const float epsilon=0.00001f) const;

	bool IntersectionTriangle(const VistaVector3D& a, const VistaVector3D& b, const VistaVector3D& c,
							 VistaVector3D& contactPoint, VistaVector3D& faceNormal,
							 float& penetrationDepth, float& barycentric_ab, float& barycentric_ac,
							 const float epsilon=0.00001f) const;
	// MEMBERS

};


/*============================================================================*/
// VISTA NURBSCurve SEGMENT
/*============================================================================*/

class VISTAMATHAPI VistaNURBSCurveSegment : public VistaCurveSegment
{
public:
	// CONSTRUCTORS
	VistaNURBSCurveSegment () {};
	VistaNURBSCurveSegment ( VistaVector3D & startPnt, VistaVector3D & endPnt )
	: VistaCurveSegment ( startPnt, endPnt ) {}

	// OPERATIONS

	// IMPLEMENTATION

	// SOME SPECIAL METHODS
	VistaVector3D  GetPointByParameter        ( const float          paramT ) const {return m_StartPoint;}
	float         GetParameterOfNearestPoint ( const VistaVector3D & testPnt) const {return 0.0;}
	float          GetLength                  () const {return 0.0;}

	// MEMBERS

};

/*============================================================================*/
// VISTA Spline SEGMENT
/*============================================================================*/

class VISTAMATHAPI VistaSplineSegment : public VistaCurveSegment
{
public:
	// CONSTRUCTORS
	VistaSplineSegment () {};
	VistaSplineSegment ( VistaVector3D & startPnt, VistaVector3D & endPnt,
						  VistaVector3D & startTang, VistaVector3D & endTang,
						  float & startParam, float & endParam, int precision = 10);

	// OPERATIONS

	// IMPLEMENTATION
	void  SetPrecision ( const int & precision ) { m_Precision = precision; }
	int   GetPrecision () const                  { return m_Precision;      }

	// SOME SPECIAL METHODS
	VistaVector3D  GetPointByParameter        ( const float          paramT ) const;
	float         GetParameterOfNearestPoint ( const VistaVector3D & testPnt) const {return 0.0;}
	float          GetLength                  () const {return 0.0;}

	// MEMBERS
	VistaVector3D   m_StartTangent;
	VistaVector3D   m_EndTangent;
	//float			m_Coefs[4][4];
	int             m_Precision;
	float          m_StartParameter;
	float          m_EndParameter;
};

/*============================================================================*/
// VISTA CURVE
/*============================================================================*/

class VISTAMATHAPI VistaCurve
{
public:
	// CONSTRUCTORS
	VistaCurve () {} ;
	VistaCurve ( std::vector<VistaCurveSegment*> curves );

	// DESTRUCTORS
	virtual ~VistaCurve();

	// OPERATIONS
	void operator= (VistaCurve NewCv);

	// IMPLEMENTATION
	virtual void GetSegments (std::vector<VistaCurveSegment*> & segments) const;

	// SOME SPECIAL METHODS
	bool   IsLoop      () const;
	int    NofSegments () const { return (int) m_vecCurve.size(); };

	// MEMBERS

protected:
	std::vector<VistaCurveSegment*> m_vecCurve;
};


/*============================================================================*/
// VISTA SPLINECURVE
/*============================================================================*/

class VISTAMATHAPI VistaSplineCurve : public VistaCurve
{
public:

	VistaSplineCurve(std::vector<VistaSplineSegment*> &, std::vector<float> &);
	VistaSplineCurve(std::vector<VistaVector3D> &, std::vector<VistaVector3D> &, std::vector<float> &);
	virtual ~VistaSplineCurve(){};

// IMPLEMENTATION
	void InterpolatePoints(std::vector<VistaVector3D> & points, int pointsPerSegment);
	virtual void GetSegments (std::vector<VistaCurveSegment*> & segments) const;
	void SetPrecision ( const int & precision ) ;

// MEMBERS
protected:
	std::vector<VistaVector3D>   m_vecPoints;
	std::vector<VistaVector3D>   m_vecTangents;
	std::vector<float>           m_vecParameters;
//	std::vector<VistaMatrix4x4*> m_vecCoefs;

};

/*============================================================================*/
// VISTA SURFACE
/*============================================================================*/

class VISTAMATHAPI VistaSurface
{
public:
	// CONSTRUCTORS
	VistaSurface () {} ;

	// DESTRUCTORS
	~VistaSurface() {};

	// OPERATIONS
//    void operator= (VistaSurface NewSfc);

	// IMPLEMENTATION

	// SOME SPECIAL METHODS

	// MEMBERS

};


/*============================================================================*/
// VISTA TRIANGLE
/*============================================================================*/

/**
 * \brief A data structure for a single triangle.
 */
class VISTAMATHAPI VistaTriangle
{
public:
	VistaTriangle(const VistaVector3D&, const VistaVector3D&, const VistaVector3D&);
	~VistaTriangle();

	VistaVector3D Normal() const;
	/**
	 * Compute the area of triangle
	 **/
	float Area() const;
	/**
	 * \brief Checks if the specified point lies inside this triangle.
	 *
	 * Barycentric_ab and barycentric_ac are the barycentric coordinates of the specified
	 * point with respect to a, the vector ab and the vector ac:
	 * \f$a+barycentric_ab*(b-a)+barycentric_ac*(c-a)=(1-barycentric_ab-barycentric_ac)*a+barycentrc_ab*b+barycentric_ac*c=p\f$
	 */
	bool IntersectionPoint(const VistaVector3D& pnt,
						   float& barycentric_ab, float& barycentric_ac,
						   const float epsilon=0.0f) const;

	/**
	 * \brief Checks if the specified point lies in the proximity of the triangle.
	 *
	 * \note The test is only done with a prism spanned in normal direction. If a point
	 * is close to an edge but its projected point does not lie inside the triangle, it
	 * is not said to be in proximity.
	 * \param triangleNormal Normalized triangle face normal.
	 */
	bool ProximityContains(const VistaVector3D& pnt, const VistaVector3D& triangleNormal,
						   const float fProximityDistance, float& ab, float& ac, const float fEpsilon = 1.0e-5f) const;
	/**
	 * \brief Test if the specified triangle and this triangle intersect.
	 *
	 * Implementation based on "Fast and Robust Triangle-Triangle Overlap Test using Orientation
	 * Predicates - Philippe Guigue, Olivier Devillers".
	 * \note After previous test, the method has been implemented using double precision. The
	 * single precision version caused a large number of obviously false intersections and
	 * compensation with epsilon checks did not lead to satisfying results.
	 */
	bool Intersects(const VistaTriangle& tri);

	VistaVector3D GetA() const;
	void SetA(const VistaVector3D& val);
	VistaVector3D GetB() const;
	void SetB(const VistaVector3D& val);
	VistaVector3D GetC() const;
	void SetC(const VistaVector3D& val);
private:
	bool CheckMinMax(const VistaVector<double,3>& p1, const VistaVector<double,3>& q1, const VistaVector<double,3>& r1,
						const VistaVector<double,3>& p2, const VistaVector<double,3>& q2, const VistaVector<double,3>& r2);
	bool IntersectionTriTri3D(const VistaVector<double,3>& p1, const VistaVector<double,3>& q1, const VistaVector<double,3>& r1,
								const VistaVector<double,3>& p2, const VistaVector<double,3>& q2, const VistaVector<double,3>& r2,
								const VistaVector<double,3>& normal1, const VistaVector<double,3>& normal2,
								const double dp2, const double dq2, const double dr2);
	bool Coplanar(const VistaVector<double,3>& p1, const VistaVector<double,3>& q1, const VistaVector<double,3>& r1,
				  const VistaVector<double,3>& p2, const VistaVector<double,3>& q2, const VistaVector<double,3>& r2,
				  const VistaVector<double,3>& normal1, const VistaVector<double,3>& normal2);
	bool Intersection2D(const VistaVector<double,2>& p1, const VistaVector<double,2>& q1, const VistaVector<double,2>& r1,
					  const VistaVector<double,2>& p2, const VistaVector<double,2>& q2, const VistaVector<double,2>& r2);
	bool Intersection2DCCW(const VistaVector<double,2>& p1, const VistaVector<double,2>& q1, const VistaVector<double,2>& r1,
					  const VistaVector<double,2>& p2, const VistaVector<double,2>& q2, const VistaVector<double,2>& r2);
	double Orientation2D(const VistaVector<double,2>& a, const VistaVector<double,2>& b, const VistaVector<double,2>& c);
	bool IntersectionVertex(const VistaVector<double,2>& P1, const VistaVector<double,2>& Q1, const VistaVector<double,2>& R1,
								  const VistaVector<double,2>& P2, const VistaVector<double,2>& Q2, const VistaVector<double,2>& R2);
	bool IntersectionEdge(const VistaVector<double,2>& P1, const VistaVector<double,2>& Q1, const VistaVector<double,2>& R1,
								  const VistaVector<double,2>& P2, const VistaVector<double,2>& Q2, const VistaVector<double,2>& R2);


	VistaVector3D m_a, m_b, m_c;
};

/*============================================================================*/
// VISTA PLANE
/*============================================================================*/

class VISTAMATHAPI VistaPlane
{
public:
	// CONSTRUCTOR
	VistaPlane (const VistaVector3D& origin = VistaVector3D(0.0,0.0,0.0),
				 const VistaVector3D& xDir = VistaVector3D(1.0,0.0,0.0),
				 const VistaVector3D& yDir = VistaVector3D(0.0,1.0,0.0),
				 const VistaVector3D& vNormVector = VistaVector3D(0.0,0.0,1.0));
	// DESTRUCTOR
	~VistaPlane();

	// OPERATIONS
	void operator= (const VistaPlane &other);

	// IMPLEMENTATION
	void SetOrigin(const VistaVector3D& org);
	void SetXDir(const VistaVector3D& xDir);
	void SetYDir(const VistaVector3D& yDir);
	VistaVector3D GetOrigin() const { return m_Origin; }
	VistaVector3D GetXDir() const { return m_XDir; }
	VistaVector3D GetYDir() const { return m_YDir; }

public:
	// SOME SPECIAL METHODS
	VistaVector3D  TransformPointOnPlane(const VistaVector3D  &givenPnt,
									float rot = 0.0f, float scale = 1.0f,
									float transX = 0.0f, float transY = 0.0f) const;

	void            CalcPlane(const VistaVector3D &upVector,
							  const VistaVector3D &orgPoint = VistaVector3D (0,0,0) );
	void            GetEquationRep(float& paramA, float& paramB,
								   float& paramC, float& paramD) const;
	float           CalcDistance(const VistaVector3D& testPnt) const;
	bool            CalcIntersection(const VistaRay& testLine, VistaVector3D& resPnt) const;
	bool            CalcIntersection(const VistaLineSegment& testLine, VistaVector3D& resPnt) const;
	/** Find point of intersection of ray with plane.
	 *  ray.origin + lambda * ray.direction = point of intersection
	 *  @params const VistaRay& ray : the ray to be tested
	 *  @params float& lambda : the point of intersection in relation to the ray
	 *  @return bool : any intersection at all??
	 */
	bool            CalcIntersection(const VistaRay& ray, float& lambda) const;
	bool            CalcIntersection(const VistaLineSegment& line, float& lambda) const;
	VistaVector3D  CalcNearestPointOnPlane(const VistaVector3D& worldPnt) const;

	// if you consider this plane as an coord system than you can
	// use these two methods to convert from one to the other coord sys
	VistaVector3D  ConvertWorldToPlane(const VistaVector3D& worldPnt) const;
	VistaVector3D  ConvertPlaneToWorld(const VistaVector3D& planePnt) const;

	VistaVector3D  GetNormVector() const { return m_vNormVector; };
	bool            SetNormVector(const VistaVector3D&);

	float           CalcDistanceToZero() const;
	bool            CalcIntersectionPoint(const VistaRay & testLine,
										  VistaVector3D & resolutionPoint) const;

	//! tests if a point is ON, ABOVE, or BELOW the given (crack) plane
	int				CalcOrientation(const VistaVector3D& point) const;
	int				CalcOrientation(const float pnt[3]) const;
	int				CalcOrientation(const float px, const float py, const float pz) const;

// MEMBERS
public:
protected:
	// this is the vector representation of a plane
	// (parameterized or explicit description)
	// ===========================
	//   P = O + u*X + v*Y
	// ===========================
	// this definition can be used as a new coordinate system definition
	// refered to the global world coord system
	VistaVector3D    m_Origin;
	VistaVector3D    m_XDir;
	VistaVector3D    m_YDir;
	// a second representation of a plane is the scalar one
	// (coordinate or implicit descr.)
	// ===========================
	//   A*x + B*y + C*z + D = 0
	// ===========================
	float             m_parA, m_parB, m_parC, m_parD;
	VistaVector3D    m_vNormVector;
};

/*============================================================================*/
// VISTA POLYGON
/*============================================================================*/

class VISTAMATHAPI VistaPolygon : public VistaSurface
{
public:
	// CONSTRUCTORS
	VistaPolygon();
	VistaPolygon(std::vector<VistaVector3D*> & polyVtx);
	// DESTRUCTORS
	~VistaPolygon();

	// OPERATIONS
	void operator= (VistaPolygon NewPoly);

	// IMPLEMENTATION
	bool SetPolyPoints (std::vector<VistaVector3D*> & polyVtx);
	void GetPolyPoints (std::vector<VistaVector3D*> & polyVtx);

	VistaVector3D GetUpVector ();
//    std::vector <void*>
//        DivideIntoMonotonePolygons (std::vector<VistaVector3D> & orgPoly);

protected:
	// SOME SPECIAL METHODS
	VistaVector3D CalcUpVector ();

	// MEMBERS
protected:
	std::vector <VistaVector3D>       m_vecPolyPnts;
};

/*============================================================================*/
// VISTA POLYGONWITHHOLES
/*============================================================================*/

class VISTAMATHAPI VistaPolygonWithHoles : public VistaPolygon
{
public:
	// CONSTRUCTORS
	VistaPolygonWithHoles() {};
	VistaPolygonWithHoles(std::vector<VistaVector3D*> & polyVtx,
						   std::vector<VistaPolygon>  & holes);

	// DESTRUCTORS
	~VistaPolygonWithHoles();

	// IMPLEMENTATION
	bool SetHoles (std::vector<VistaPolygon> & holes);
	void GetHoles (std::vector<VistaPolygon> & holes);

	// MEMBERS
protected:
	std::vector <VistaPolygon>       m_vecHoles;
};

/*============================================================================*/
// VISTA SPHERICAL SURFACE
/*============================================================================*/

class VISTAMATHAPI VistaSphericalSurface : public VistaSurface
{
public:
	// CONSTRUCTORS
	VistaSphericalSurface () {};
	// DESTRUCTORS
	~VistaSphericalSurface() {};

	// OPERATIONS

	// IMPLEMENTATION

protected:
	// MEMBERS
};

/*============================================================================*/
// VISTA CYLINDRICAL SURFACE
/*============================================================================*/

class VISTAMATHAPI VistaCylindricalSurface : public VistaSurface
{
public:
	// CONSTRUCTORS
	VistaCylindricalSurface () {};
	// DESTRUCTORS
	~VistaCylindricalSurface() {};

	// OPERATIONS

	// IMPLEMENTATION

protected:
	// MEMBERS
};

/*============================================================================*/
// VISTA NURBS SURFACE
/*============================================================================*/

class VISTAMATHAPI VistaNURBSSurface : public VistaSurface
{
public:
	// CONSTRUCTORS
	VistaNURBSSurface () {};
	// DESTRUCTORS
	~VistaNURBSSurface() {};

	// OPERATIONS
	VistaCurve GetBoundary()
	{
		VistaCurve boundary;
		return boundary;
	};

	// IMPLEMENTATION

protected:
	// MEMBERS
};


/*============================================================================*/
// VISTA EXTRUSION
/*============================================================================*/

class VISTAMATHAPI VistaExtrusion : public VistaSurface
{
public:
	// CONSTRUCTORS
//    VistaExtrusion (const VistaCurve & curve) : m_Curve( curve ) {};
	VistaExtrusion (const VistaCurve & curve);
	// DESTRUCTORS
	~VistaExtrusion() {};

	// OPERATIONS

	// IMPLEMENTATION
	VistaCurve GetCurve () { return m_Curve;}

protected:
	// MEMBERS
	VistaCurve     m_Curve;
};

/*============================================================================*/
// VISTA LINEAREXTRUSION
/*============================================================================*/

class VISTAMATHAPI VistaLinearExtrusion : public VistaExtrusion
{
public:
	// CONSTRUCTORS
	VistaLinearExtrusion ( const VistaCurve & curve, const VistaVector3D & direction)
		: VistaExtrusion( curve ), m_Direction ( direction ) {}

	// DESTRUCTORS
	~VistaLinearExtrusion() {};

	// OPERATIONS

	// IMPLEMENTATION
	VistaVector3D GetDirection() {return m_Direction;}

protected:
	// MEMBERS
	VistaVector3D  m_Direction;

};

/*============================================================================*/
// VISTA CURVEEXTRUSION
/*============================================================================*/

class VISTAMATHAPI VistaCurveExtrusion : public VistaExtrusion
{
public:
	// CONSTRUCTORS
	VistaCurveExtrusion ( const VistaCurve & curve, const VistaCurve & direction )
	: VistaExtrusion( curve ), m_DirectionCurve ( direction ) {}

	// DESTRUCTORS
	~VistaCurveExtrusion() {};

	// OPERATIONS

	// IMPLEMENTATION

protected:
	// MEMBERS
	VistaCurve     m_DirectionCurve;

};

/*============================================================================*/
// VISTA ROTATIONALEXTRUSION
/*============================================================================*/

class VISTAMATHAPI VistaRotationalExtrusion : public VistaExtrusion
{
public:
	// CONSTRUCTORS
	VistaRotationalExtrusion ( const VistaCurve & curve, const VistaAxisAndAngle & axis)
	: VistaExtrusion( curve ), m_Axis(axis) { }

	// DESTRUCTORS
	~VistaRotationalExtrusion() {};

	// OPERATIONS

	// IMPLEMENTATION

protected:
	// MEMBERS
	VistaAxisAndAngle  m_Axis;

};

/*============================================================================*/
// VISTA SOLID
/*============================================================================*/

class VISTAMATHAPI VistaSolid
{
public:
	// CONSTRUCTORS
	VistaSolid ( ) { };

	// DESTRUCTORS
	~VistaSolid() {};

	// OPERATIONS

	// IMPLEMENTATION

protected:
	// MEMBERS
	std::vector<VistaSurface*> m_vecSurface;

};

/*============================================================================*/
// VISTA TETRAHEDRON
/*============================================================================*/

/**
 * \brief A data structure for a single tetrahedron.
 */
class VISTAMATHAPI VistaTetrahedron
{
public:
	VistaTetrahedron();
	VistaTetrahedron(const float p1[3], const float p2[3],
					  const float p3[3], const float p4[3]);
	VistaTetrahedron(const VistaVector3D& p1, const VistaVector3D& p2,
					  const VistaVector3D& p3, const VistaVector3D& p4);
	~VistaTetrahedron();
public:
	void SetCoordinates(const VistaVector3D& p1, const VistaVector3D& p2,
						const VistaVector3D& p3, const VistaVector3D& p4);
	void SetCoordinates(const float p1[3], const float p2[3],
						const float p3[3], const float p4[3]);
	void GetBarycentricCoordinate(const float p[3], float bc[3]) const;
	void GetBarycentricCoordinate(const VistaVector3D& p, float bc[3]) const;
	/**
	 * Perform test to see wherether a point pt lies within a tetrahedron.
	 * If yes, output parameter coords will contain barycentric coordinates
	 */
	bool PtInsideTetrahedron(const float pt[3], float coords[4]) const;
private:
	float ComputeVolume(const float p1[3], const float p2[3],
						const float p3[3], const float p4[3]) const;
	float ComputeVolume(const VistaVector3D& p1, const VistaVector3D& p2,
						const VistaVector3D& p3, const VistaVector3D& p4) const;
private:
	float m_volume;
	VistaVector3D m_p1, m_p2, m_p3, m_p4;
};

class VISTAMATHAPI VistaEvenCone
{
public:
	/**
	 * represents a non-askew cone!
	 */
	VistaEvenCone();
	VistaEvenCone(float nHeight, float nRadius);
	~VistaEvenCone();


	/**
	 * @paran pnt the point in the local system of the cone
	 */
	bool Intersects(const float pnt[3],
					float &dPerp,
					float &dProj,
					float &dAngle) const;

	float GetHeight() const;
	float GetRadius() const;
	void SetHeight(float nHeight);
	void SetRadius(float nRadius);

	float GetOpeningAngle() const;

private:
	float m_nHeight,
		  m_nRadius;
};
/*============================================================================*/


/*============================================================================*/
/* INLINED METHODS OF VistaLineSegment                                          */
/*============================================================================*/

inline VistaVector3D  VistaLineSegment::GetPointByParameter  ( const float paramT ) const
{
	return (m_EndPoint * paramT) + (m_StartPoint * (1.0f - paramT));
}

inline float VistaLineSegment::GetLength  () const
{
	return (m_EndPoint - m_StartPoint).GetLength();
}

inline VistaVector3D  VistaRay::GetPointByParameter  ( const float paramT ) const
{
	return m_Origin + m_Dir * paramT;
}


#endif //_VISTAGEO_H
