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


#ifndef _VISTAGEOMFACTORY_H
#define _VISTAGEOMFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <vector>

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSceneGraph;
class VistaColor;

using std::vector;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This factory needs to work properly an instance of VistaSceneGraph.
 * All methods can be called without any parameters.
 */
class VISTAKERNELAPI VistaGeometryFactory
{
public:

	VistaGeometryFactory(VistaSceneGraph *pSG);
	virtual ~VistaGeometryFactory();

	enum FaceType
	{
		FT_TRIANGLE,
		FT_QUAD,
		FT_QUAD_SPLIT_SYMMETRICALLY, // splits a quad into four
									 // triangles by adding a new
									 // vertex in the center
	};

	/**
	 * Create a plane as VistaGeometry.
	 *
	 * @param nSizeX Plane extent in X direction.
	 * @param nSizeZ Plane extent in Z direction.
	 * @param nResolutionX Resolution in X direction.
	 * @param nResolutionZ Resolution in Z direction.
	 * @param eNormalDirection Plane normal axis.
	 * @param oColor Plane color.
	 * @param eFaceType Face type. @see FaceType.
	 */
	VistaGeometry* CreatePlane(
		float nSizeX = 1, float nSizeZ = 1,
		int nResolutionX = 1, int nResolutionZ = 1,
		Vista::AXIS eNormalDirection = Vista::Y,
		VistaColor oColor = VistaColor::WHITE,
		FaceType eFaceType = FT_TRIANGLE,
		float nMinTextureCoordX = 0.0f, float nMaxTextureCoordX = 1.0f,
		float nMinTextureCoordZ = 0.0f, float nMaxTextureCoordZ = 1.0f );

	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of a plane geometry.
	 *
	 * For description of the geometric parameters, @see CreatePlane()
	 * documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 * @param pFaceType  Pointer to VistaGeometry face type.
	 */
	static bool CreatePlaneData(
		vector<VistaIndexedVertex> *pIndex,
		vector<float> *pCoords,
		vector<float> *pTexCoords,
		vector<float> *pNormals,
		vector<VistaColor> *pColors,
		VistaGeometry::FaceType *pFaceType,
		float nSizeX = 1, float nSizeZ = 1,
		int nResolutionX = 1.0f, int nResolutionZ = 1.0f,
		Vista::AXIS eNormalDirection = Vista::Y,
		VistaColor oColor = VistaColor::WHITE,
		FaceType eFaceType = FT_TRIANGLE,
		float nMinTextureCoordX = 0.0f, float nMaxTextureCoordX = 1.0f,
		float nMinTextureCoordZ = 0.0f, float nMaxTextureCoordZ = 1.0f);

	/**
	 * Create a box as VistaGeometry.
	 *
	 * @param nSizeX Box extent in X direction.
	 * @param nSizeY Box extent in Y direction.
	 * @param nSizeZ Box extent in Z direction.
	 * @param nResolutionX Resolution in X direction.
	 * @param nResolutionY Resolution in Y direction.
	 * @param nResolutionZ Resolution in Z direction.
	 * @param oColor Plane color.
	 * @param eFaceType Face type. @see FaceType.
	 */
	VistaGeometry* CreateBox(
		float nSizeX = 1.0f, float nSizeY = 1.0f, float nSizeZ = 1.0f,
		int nResolutionX = 1, int nResolutionY = 1, int nResolutionZ = 1,
		VistaColor oColor = VistaColor::WHITE,
		FaceType eFaceType = FT_TRIANGLE,
		float nMinTextureCoordX = 0.0f, float nMaxTextureCoordX = 1.0f,
		float nMinTextureCoordY = 0.0f, float nMaxTextureCoordY = 1.0f,
		float nMinTextureCoordZ = 0.0f, float nMaxTextureCoordZ = 1.0f );

	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of a box geometry.
	 *
	 * For description of the geometric parameters, @see CreateBox()
	 * documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 * @param pFaceType  Pointer to VistaGeometry face type.
	 */
	static bool CreateBoxData(
		vector<VistaIndexedVertex> *pIndex,
		vector<float> *pCoords,
		vector<float> *pTexCoords,
		vector<float> *pNormals,
		vector<VistaColor> *pColors,
		VistaGeometry::FaceType *pFaceType,
		float nSizeX = 1.0f, float nSizeY = 1.0f, float nSizeZ = 1.0f,
		int nResolutionX = 1, int nResolutionY = 1, int nResolutionZ = 1,
		VistaColor oColor = VistaColor::WHITE,
		FaceType eFaceType = FT_TRIANGLE,
		float nMinTextureCoordX = 0.0f, float nMaxTextureCoordX = 1.0f,
		float nMinTextureCoordY = 0.0f, float nMaxTextureCoordY = 1.0f,
		float nMinTextureCoordZ = 0.0f, float nMaxTextureCoordZ = 1.0f);

	
	/**
	 * Create a disk as VistaGeometry.
	 *
	 * @param radius Disk radius.
	 * @param resolutionC Circular resolution.
	 * @param resolutionD Diameter resolution.
	 * @param normal Normal axis.
	 */
	VistaGeometry* CreateDisk(
		float radius = 0.5f,
		int resC = 16, int resD = 1,
		Vista::AXIS normal = Vista::Y,
		VistaColor color = VistaColor::WHITE);

	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of a disk geometry.
	 *
	 * For description of the geometric parameters, @see CreateBox()
	 * documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 */
	static bool CreateDiskData(
		vector<VistaIndexedVertex> *pIndex,
		vector<float> *pCoords,
		vector<float> *pTexCoords,
		vector<float> *pNormals,
		vector<VistaColor> *pColors,
		float radius = 0.5f,
		int resC = 16, int resD = 1,
		Vista::AXIS normal = Vista::Y,
		VistaColor color = VistaColor::WHITE);

	/**
	 * Create a cone as VistaGeometry.
	 *
	 * @param botRad Radius of the bottom cap.
	 * @param topRad Radius of the top cap.
	 * @param height Cone height.
	 * @param resD Diameter resolution.
	 * @param resC Circumference resolution.
	 * @param resY Resolution on the Y axis.
	 * @param color Cone color.
	 * @param bBottom Generate bottom cap geometry.
	 * @param bTop Generate Top cap geometry.
	 * @param bSides Generate side geometry.
	 */
	VistaGeometry* CreateCone(
		float radiusBottom = 0.5f, float radiusTop = 0.5f, float height = 1.0f,
		int resolutionC = 16, int resolutionD = 1, int resolutionY = 1,
		VistaColor color = VistaColor::WHITE,
		bool bottom = true, bool top = true, bool sides = true
		);

	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of a cone geometry.
	 *
	 * For description of the geometric parameters, @see CreateCone()
	 * documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 */
	static bool CreateConeData(
		vector<VistaIndexedVertex> *pIndex,
		vector<float> *pCoords,
		vector<float> *pTexCoords,
		vector<float> *pNormals,
		vector<VistaColor> *pColors,
		float botRad = 0.5f, float topRad = 0.5f, float height = 1.0f,
		int resC = 16, int resD = 1, int resY = 1,
		VistaColor color = VistaColor::WHITE,
		bool bBottom = true, bool bTop = true, bool bSides = true);

	/**
	 * Create a torus as VistaGeometry.
	 *
	 * @param ringRadius Radius of the center torus ring.
	 * @param outerRadius Outer radius of the torus.
	 * @param resolutionSides Resolution of the sides orthogonal to the torus ring.
	 * @param resolutionRing Resolution of the torus ring.
	 * @param color Torus color.
	 */
	VistaGeometry* CreateTorus(
		float ringRadius = 0.1f, float outerRadius = 0.4f,
		int resolutionSides = 30, int resolutionRing = 30,
		VistaColor color = VistaColor::WHITE
		);

	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of a torus geometry.
	 *
	 * For description of the geometric parameters, @see CreateTorus()
	 * documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 */
	static bool CreateTorusData(
		float innerRad, float outerRad,
		int resSides, int resRings,
		VistaColor color,
		vector<VistaIndexedVertex> *pIndex,
		vector<float> *pCoords,
		vector<float> *pTexCoords,
		vector<float> *pNormals,
		vector<VistaColor> *pColors);

	/**
	 * Create an ellipsoid as VistaGeometry.
	 *
	 * @param radius_a Radius in first direction.
	 * @param radius_b Radius in second direction.
	 * @param radius_c Radius in third direction.
	 * @param thetaPrecision Angular resolution in first direction.
	 * @param phiPrecision Angular resolution in second direction.
	 * @parma color Ellipsoid color.
	 */
	VistaGeometry* CreateEllipsoid(
		float radius_a = 0.5f,
		float radius_b = 0.5f,
		float radius_c = 0.5f,
		int thetaPrecision = 32,
		int phiPrecision = 32,
		const VistaColor & color = VistaColor::WHITE
		);

	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of an ellipsoid geometry.
	 *
	 * For description of the geometric parameters, @see
	 * CreateEllipsoid() documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 */
	static bool CreateEllipsoidData(
		vector<VistaIndexedVertex> *pIndex,
		vector<VistaVector3D> *pCoords,
		vector<VistaVector3D> *pTexCoords,
		vector<VistaVector3D> *pNormals,
		vector<VistaColor> *pColors,
		float radius_a = 0.5f,
		float radius_b = 0.5f,
		float radius_c = 0.5f,
		int thetaPrecision = 32,
		int phiPrecision = 32,
		const VistaColor & color = VistaColor::WHITE);

	/**
	 * Create a sphere as VistaGeometry. 
	 *
	 * @param radius Sphere radius.
	 * @param resolution Sphere resolution.
	 * @param color Sphere color.
	 */
	VistaGeometry* CreateSphere(
		float radius = 0.5f,
		int resolution = 32,
		VistaColor color = VistaColor::WHITE
		);
	
	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of a sphere geometry.
	 *
	 * For description of the geometric parameters, @see CreateSphere()
	 * documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 */
	static bool CreateSphereData(
		std::vector<VistaIndexedVertex> *pIndex,
		std::vector<VistaVector3D> *pCoords,
		std::vector<VistaVector3D> *pTexCoords,
		std::vector<VistaVector3D> *pNormals,
		std::vector<VistaColor> *pColors,
		float radius,
		int resolution,
		const VistaColor &color);
	
	/**
	 * Create a triangle as VistaGeometry. The amount of inner
	 * triangles is resolution^2.
	 *
	 * @param a First triangle vertex.
	 * @param b Second triangle vertex.
	 * @param c Thirs triangle vertex.
	 * @param resolution The amount of triangle rows.
	 * @parma color Ellipsoid color.
	 */
	VistaGeometry* CreateTriangle(
		const VistaVector3D & a = VistaVector3D(-1.0f/2,-1.0f/2,0),
		const VistaVector3D & b = VistaVector3D( 1.0f/2,-1.0f/2,0),
		const VistaVector3D & c = VistaVector3D(      0, 1.0f/2,0),
		int resolution = 3,
		const VistaColor & color = VistaColor::WHITE
		);

	/**
	 * Create index, coordinate, tex-coordinate, normal and color vectors
	 * of a triangle geometry.
	 *
	 * For description of the geometric parameters, @see CreateTriangle()
	 * documentation.
	 *
	 * @param pIndex     Pointer to indexed vertex vector.
	 * @param pCoords    Pointer to vertex positions vector.
	 * @param pTexCoords Pointer to texture coordinates vector.
	 * @param pNoramals  Pointer to vertex normals vector.
	 * @param pColors    Pointer to vertex colors vector.
	 */
	static bool CreateTriangleData(
		const VistaVector3D & a,
		const VistaVector3D & b,
		const VistaVector3D & c,
		int resolution,
		const VistaColor & color,
		std::vector<VistaIndexedVertex> *pIndex,
		std::vector<VistaVector3D> *pCoords,
		std::vector<VistaVector3D> *pTexCoords,
		std::vector<VistaVector3D> *pNormals,
		std::vector<VistaColor> *pColors);
	
	/**
	 * Call this method to create geometries from a PropertyList.
	 * Please have a look at the code to get the supported attributes.
	 */
	VistaGeometry* CreateFromPropertyList( const VistaPropertyList &props );

protected:
	VistaSceneGraph* GetSG() const;

private:
	VistaSceneGraph *m_pSG;
};

#endif
