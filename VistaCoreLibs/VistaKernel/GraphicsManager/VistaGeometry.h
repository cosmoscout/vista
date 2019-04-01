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


#ifndef _VISTAGEOMETRY_H
#define _VISTAGEOMETRY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <vector>
#include <set>

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaColor.h>
#include <VistaAspects/VistaObserveable.h>
#include <VistaMath/VistaBoundingBox.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGeomNode;
class VistaVector3D;
class VistaPlane;
class VistaRay;
class IVistaGeometryData;
class IVistaGraphicsBridge;
class VistaColor;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaVertexFormat
{
public:
	enum coordinateFormat { COORDINATE };
	enum colorFormat { COLOR_NONE, COLOR_RGB };
	enum normalFormat { NORMAL_NONE, NORMAL, NORMAL_AUTO };
	enum textureCoordFormat { TEXTURE_COORD_NONE, TEXTURE_COORD_2D };

	coordinateFormat coordinate;
	colorFormat color;
	normalFormat normal;
	textureCoordFormat textureCoord;

	VistaVertexFormat()
		: coordinate(VistaVertexFormat::COORDINATE),
		color(VistaVertexFormat::COLOR_NONE),
		normal(VistaVertexFormat::NORMAL_AUTO),
		textureCoord(VistaVertexFormat::TEXTURE_COORD_NONE)
		{}

		~VistaVertexFormat() {};
};

class VistaRenderingAttributes
{
public:
	enum raShading { SHADING_DEFAULT, SHADING_FLAT, SHADING_GOURAUD, SHADING_NICEST};
	enum raCulling { CULL_DEFAULT, CULL_BACK, CULL_FRONT, CULL_NONE};
	enum raStyle { STYLE_DEFAULT, STYLE_FILLED, STYLE_WIREFRAME, STYLE_POINT};
	enum raTextureFilter { TEXTURE_FILTER_DEFAULT, TEXTURE_FILTER_NEAREST, TEXTURE_FILTER_LINEAR, TEXTURE_FILTER_MIPMAP, TEXTURE_FILTER_NICEST};

	raShading       shading;
	raCulling       culling;
	raStyle         style;
	raTextureFilter texFilter;

	VistaRenderingAttributes(raShading sh = SHADING_DEFAULT, raCulling c = CULL_DEFAULT, raStyle st = STYLE_DEFAULT, raTextureFilter t = TEXTURE_FILTER_DEFAULT)
		: shading(sh),
		  culling(c),
		  style(st),
		  texFilter(t)
		  {}

	~VistaRenderingAttributes() {};
};

class VISTAKERNELAPI VistaMaterial
{
public:
	VistaMaterial();
	VistaMaterial(const VistaColor &ambient, const VistaColor &diffuse,
				   const VistaColor &specular, const VistaColor &emission,
				   const float shininess, const float opacity, const std::string &name);
	VistaMaterial(const float ambient[3], const float diffuse[3], const float specular[3],
				   const float emission[3],
				   const float shininess, const float opacity, const std::string &name);

	~VistaMaterial(){};

	
	VistaColor GetAmbientColor() const;
	void GetAmbientColor( float a3fColor[3] ) const;
	VistaColor GetDiffuseColor() const;
	void GetDiffuseColor( float a3fColor[3] ) const;
	VistaColor GetSpecularColor() const;
	void GetSpecularColor( float a3fColor[3] ) const;
	VistaColor GetEmissionColor() const;
	void GetEmissionColor( float a3fColor[3] ) const;
	float GetShininess() const;
	float GetOpacity() const;

	void SetAmbientColor( const float a3fColor[3] );
	void SetAmbientColor( const VistaColor& oColor ) ;
	void SetDiffuseColor( const float a3fColor[3] );
	void SetDiffuseColor( const VistaColor& oColor ) ;
	void SetSpecularColor( const float a3fColor[3] );
	void SetSpecularColor( const VistaColor& oColor ) ;
	void SetEmissionColor( const float a3fColor[3] );
	void SetEmissionColor( const VistaColor& oColor ) ;
	void SetShininess( const float nShininess ) ;
	void SetOpacity( const float nOpacity  ) ;

	std::string GetName() const;

	int  GetMaterialIndex() const;
	bool SetMaterialIndex(int iIndex);

	static void Emerald(VistaMaterial&);
	static void Jade(VistaMaterial&);
	static void Obsidian(VistaMaterial&);
	static void Pearl(VistaMaterial&);
	static void Ruby(VistaMaterial&);
	static void Turquoise(VistaMaterial&);
	static void Brass(VistaMaterial&);
	static void Bronze(VistaMaterial&);
	static void Chrome(VistaMaterial&);
	static void Copper(VistaMaterial&);
	static void Gold(VistaMaterial&);
	static void Silver(VistaMaterial&);
	static void BlackPlastic(VistaMaterial&);
	static void CyanPlastic(VistaMaterial&);
	static void GreenPlastic(VistaMaterial&);
	static void RedPlastic(VistaMaterial&);
	static void WhitePlastic(VistaMaterial&);
	static void YellowPlastic(VistaMaterial&);
	static void BlackRubber(VistaMaterial&);
	static void CyanRubber(VistaMaterial&);
	static void GreenRubber(VistaMaterial&);
	static void RedRubber(VistaMaterial&);
	static void WhiteRubber(VistaMaterial&);
	static void YellowRubber(VistaMaterial&);

private:
	VistaColor          m_oAmbient;
	VistaColor          m_oDiffuse;
	VistaColor          m_oSpecular;
	VistaColor          m_oEmission;
	float               m_nShininess;
	float               m_nOpacity;
	std::string         m_strName;
	int                 m_iIndex;
};

class VISTAKERNELAPI VistaIndexedVertex
{
public:
	VistaIndexedVertex ();
	VistaIndexedVertex (const int );

	~VistaIndexedVertex();

	void SetCoordinateIndex( const int nCoordinatesIndex );
	int GetCoordinateIndex() const;

	void SetNormalIndex( const int nNormalIndex);
	int GetNormalIndex() const;

	void SetColorIndex( const int nColorIndex );
	int GetColorIndex() const;

	void SetTextureCoordinateIndex( const int nTextureCoordIndex );
	int GetTextureCoordinateIndex() const;

private:
	int             m_nCoordinatesIndex;
	int             m_nNormalIndex;
	int             m_nColorIndex;
	int             m_nTextureCoordIndex;
};

class VISTAKERNELAPI VistaGeometryObservable : public IVistaObserveable
{
public:
	enum
	{
		MSG_SET_COORD = MSG_LAST+1,
		MSG_SET_NORMAL,
		MSG_SET_COLOR,
		MSG_SET_TEXTURECOORD,
		MSG_SET_TEXTURE,
		MSG_ADD_VERTEX,
		MSG_DELETE_VERTEX,
		MSG_ADD_FACE,
		MSG_DELETE_FACE
	};

public:
	int				m_nLastVertexChanged;
	int				m_nLastFaceChanged;
	VistaVector3D	m_nLastPositionChanged;

};

class VISTAKERNELAPI VistaGeometry : public VistaGeometryObservable
{
	friend class VistaGeomNode;
	friend class VistaSceneGraph;
	friend class IVistaGraphicsBridge;
public:
	enum FaceType{
		VISTA_FACE_TYPE_UNDETERMINED=-1, //<** user code should not use that flag */
		VISTA_FACE_TYPE_POINTS = 0,
		VISTA_FACE_TYPE_LINES,
		VISTA_FACE_TYPE_POLYLINE,
		VISTA_FACE_TYPE_POLYLINE_LOOP,
		VISTA_FACE_TYPE_TRIANGLES,
		VISTA_FACE_TYPE_TRIANGLE_STRIP,
		VISTA_FACE_TYPE_QUADS,
		VISTA_FACE_TYPE_UNKNOWN, //<** we tried, but we can not say, what it is */
	};

protected:

	//! Deprecated version for creating an indexed geometry, as vectors of VistaVector3Ds are used.
	bool CreateIndexedGeometry(	const std::vector<VistaIndexedVertex>& vecVertices,
							const std::vector<VistaVector3D>& vecCoords,
							const std::vector<VistaVector3D>& vecTextureCoords,
							const std::vector<VistaVector3D>& vecNormals,
							const std::vector<VistaColor>& vecColors,
							const VistaVertexFormat& vFormat,
							const VistaGeometry::FaceType fType );

	//! Create an indexed geometry.
	/**
	 *
	 * @param coords - vector with x,y,z value for the textures length%3 should be 0
	 * @param textureCoords - vector with u,v values for the textures length%2 should be 0
	 * @param normals - vector with x,y,z value for the textures length%3 should be 0
	 *
	*/
	bool CreateIndexedGeometry( const std::vector<VistaIndexedVertex>& vecVertices,
							const std::vector<float>& vecCoords,
							const std::vector<float>& vecTextureCoords,
							const std::vector<float>& vecNormals,
							const std::vector<VistaColor>& vecColors,
							const VistaVertexFormat& vFormat,
							const VistaGeometry::FaceType fType);

public:
	virtual ~VistaGeometry();

	/**
	 * Clones the geometry, i.e. creates a copy with the same material, faces, vertices etc.
	 * but can thereforth be modified without changing the original - e.g. to allow different
	 * materials etc.
	 */
	VistaGeometry* Clone() const;

	/**
	 * retrieves whether this geometry is considered to be changed
	 * in time. This is a user defined setting; the toolkit can use
	 * this flag to decide whether to precalculate or optimize
	 * the geometry, but it might as well ignore it. Consider
	 * this to be a hint. Default is true, all geometry is considered
	 * to be static.
	 * @see SetIsStatic()
	 * @return user defined flag as set by SetIsStatic() or the default: true
	 */
	bool GetIsStatic() const;

	/**
	 *  calculates the Vertex normals in depending on the given angle
	 *  Expensive!
	 */
	bool CalcVertexNormals( const float fCreaseAngle = 0.524f ); // 30 degrees
	/**
	 * calculating the Face normals.
	 * Expensive!
	 */
	bool CalcFaceNormals();

	/**
	 * Consider this to be a hint to the toolkit implementation as to
	 * try and optimize, dlist calculate or whatever.
	 * @param bStatic true if you do not or seldomly change this geometry
	 */
	void SetIsStatic(bool bStatic);

	//! Return the total number of vertices in the geometry.
	unsigned int  GetNumberOfVertices() const;
	//! Get the total number of faces in the geometry.
	unsigned int  GetNumberOfFaces() const;
	//! Get the number of different colors in the geometry.
	unsigned int  GetNumberOfColors() const;
	//! Get the number of coordinates in the geometry.
	unsigned int  GetNumberOfCoords() const;
	//! Get the number of normals in the geometry.
	unsigned int  GetNumberOfNormals() const;
	//! Returns the number of texture coordinates in the geometry.
	unsigned int  GetNumberOfTextureCoords() const;

//      bool IsIndexed() const;
	//! Which format do the vertices of the geometry have?
	VistaVertexFormat      GetVertexFormat() const;
	//! Returns the type of the polygons in the geometry (either triangles or quads).
	VistaGeometry::FaceType        GetFaceType() const;

	//! Computes the two points defining the geometries bounding box.
	/** \param pMin The first vertex of the bounding box.
	 *  \param pMax The second vertex of the bounding box.
	 *  \return bool Always true.
	 *  */
	bool GetBoundingBox( VistaVector3D& pMin, VistaVector3D& pMax ) const;
	VistaBoundingBox GetBoundingBox() const;

	//! Scale geometry in all directions by the same given factor
	/** \param scale The floating point value to scale the geometry with.
	 *  \return bool Returns true if scaling succeeded.
	 *  */
	bool ScaleGeometry(const float scale);
	//! Scale geometry by factors given for the different axis.
	/** \param scale The array of values to use for scaling in the different directions.
	 *  \return bool Returns true if scaling succeeded.
	 *  */
	bool ScaleGeometry(const float scale[3]);
	//! Scale geometry by factors given for the three axis x, y and z.
	/** \param sx Factor to scale in x direction.
	 *  \param sy Factor to scale in y direction.
	 *  \param sz Factor to scale in z direction.
	 *  \return bool Returns true if scaling succeeded.
	 *  */
	bool ScaleGeometry(const float, const float, const float);

	//! Set color of all vertices to the given value.
	/** \param color The color to use for all vertices of the geometry.
	 *  \return bool Returns true if operation succeeded.
	 *  */
	bool SetColor(const VistaColor  & color);

	/** @todo */
	bool SetMaterialIndex(const int& materialIndex);
	bool BindToVistaMaterialTable(void);
	int  GetMaterialIndex() const;
	bool SetMaterial(const VistaMaterial & material);

	bool SetTexture(const std::string& textureFileName);

	/**
	 * Set a raw texture for this geometry. Only one texture can be set
	 * at the same time. Make no assumption about the memory, the underlying
	 * toolkit may or may not copy the memory. A godd strategy is to keep the
	 * pointer to the memory alive as long as the geometry lives.
	 *
	 *  @param width the width in pixels
	 *  @param height the height in pixels
	 *  @param bpp bits per color channel (8/32)
	 *  @param bHasAlpha RGBA assumed if set to true, RGB else
	 *  @param data the pointer to the pixel buffer to assign
	 *  @return false if the texture could not be set.
	 */
	bool SetTexture(const int width,
			        const int height,
			        const int bpp,
			        bool  bHasAlpha,
			        VistaType::byte* pData);

	bool DeleteTexture();
	/// @todo what shall this do?
	void GetTexture( std::string & strFileName ) const;

	//! Return the index of the coordinate of the vertex with given index.
	/** For every vertex references to values like position, color, normal, etc. are
	 *  stored. This function returns the index to the coordinate of a specific
	 *  vertex.
	 *  \param idx The vertex in question.
	 *  \return The index of the coordinate of the vertex.
	 *  */
	int  GetCoordinateIndex(const int idx) const;

	//! Return the coordinate with given index.
	/** All different coordinates are stored in a list (they need not but should be
	 *  different). To get a specific coordinate use this function.
	 *  \param idx The index in terms of the list of coordinates.
	 *  \param fCoord An array taking the coordinate.
	 *  \return Did method finish successfully?
	 *  */
	bool GetCoordinate(const int idx, float fCoord[3]) const;

	//! This function returns a coordinate
	/** All different coordinates are stored in a list (they need not but should be
	 *  different). To get a specific coordinate use this function.This function is
	 *  deprecated as object creation isn't that efficient as one would like it to
	 *  be.
	 *  \param idx The index in terms of the list of coordinates.
	 *  \return The coordinate of the vertex as a VistaVector3D.
	 *  */
	VistaVector3D GetCoordinate(const int idx) const;

	//! Overwrite a set of coordinates of the geometry.
	/** This function can be used to alter the values of a set of consecutive
	 *  coordinates.
	 *  \param startIdx The index of the coordinate to start with.
	 *  \param coords The vector of new coordinates.
	 *  \return bool Success?
	 *  */
	bool SetCoordinates(const int startIdx, const std::vector<float>& coords);


	//g has not been reviewed yet.@todo
	bool SetCoordinates(const int startIndex, const std::vector<VistaVector3D>& coords);

	//! Alter the value of a coordinate.
	/** This function alters a single coordinate.
	 *  \param idx The coordinate in question.
	 *  \param coord The new values.
	 *  \return Success?
	 *  */
	bool SetCoordinate(const int idx, const float coord[3]);

	//! Alter the value of a coordinate
	/** This function alters a single coordinate.
	 *  \param idx The coordinate in question.
	 *  \param coord The new values.
	 *  \return Success?
	 *  */
	bool SetCoordinate(const int idx, const VistaVector3D& coord);

	bool SetCoordIndex(const int idx, const int value);
	bool SetCoordIndices(const int startIdx, const std::vector<int>& indices);


	/**
	 * Gives indices for all triangles in the geometry
	 * ! Returns the indices of Vertices, Normals, or Both
	 */
	bool GetTrianglesVertexIndices( std::vector<int>& vecVertexIndices ) const;
	bool GetTrianglesNormalIndices( std::vector<int>& vecNormalIndices ) const;
	bool GetTrianglesTextureCoordinateIndices( std::vector<int>& vecTexCoordIndices ) const;
	bool GetTrianglesVertexAndNormalIndices( std::vector<int>& vecVertexIndices,
									std::vector<int>& vecNormalIndices ) const;

	int  GetTextureCoordIndex(const int idx) const;
	bool GetTextureCoord(const int idx, float fTexCoord[3]) const;
	VistaVector3D GetTextureCoord(const int idx) const;

	bool SetTextureCoords2D(const int startIndex, const std::vector<VistaVector3D>& textureCoords2D);
	bool SetTextureCoords2D(const int startIndex, const std::vector<float>& textureCoords2D);
	bool GetTextureCoords2D(std::vector< VistaVector3D >& vecCoordinates ) const;
	bool GetTextureCoords2D(std::vector< float >& vecCoordinates ) const;

	//! Change the texture coordinate with index idx.
	bool SetTextureCoord2D(const int idx, const float coord[3]);
	//! Change the texsture coordinate with index idx 
	bool SetTextureCoord2D(const int idx, const VistaVector3D& coord);

	bool SetTextureCoordIndex(const int idx, const int value);
	bool SetTextureCoordsIndices(const int startIdx, const std::vector<int>& indices);

	//! Returns the index of the normal of the vertex with given index.
	int  GetNormalIndex(const int idx) const;

	//! Return the normal with given index.
	bool GetNormal(const int idx, float fNormal[3]) const;

	//! Return the normal with given index
	VistaVector3D GetNormal(const int idx) const;

	//! Set the given values to the normals at the given positions.
	bool SetNormals(const int startIdx, const std::vector<float>& normals);
	bool SetNormals(const int startIndex, const std::vector<VistaVector3D>& normals);
    bool GetNormals(std::vector<VistaVector3D> &normals) const;
	bool GetNormals(std::vector<float> &normals) const;

	//! Change a normal to the given values.
	bool SetNormal(const int idx, const float normal[3]);

	//! Change a normal to the given values.
	bool SetNormal(const int idx, const VistaVector3D& normal);
	bool SetNormalIndex(const int idx, const int value);
	bool SetNormalIndices(const int startIdx, const std::vector<int>& indices);

	int  GetColorIndex(const int idx) const;
	VistaColor GetColor(const int idx) const;

	bool SetColors(const int startIndex, const std::vector<VistaColor>& colors);
	bool SetColors(const int startIndex, const int bufferLength, float* colors);
	bool GetColors( std::vector< VistaColor >& vecColors ) const;
	bool GetColors( std::vector< float >& vecColors ) const;

	bool SetColor(const int idx, const VistaColor& color);
	bool SetColorIndex(const int idx, const int value);
	bool SetColorIndices(const int startIdx, const std::vector<int>& indices);

	bool GetFaces(std::vector<int>& faces) const;
	bool GetCoordinates(std::vector<float>& coords) const;
	bool GetCoordinates(std::vector<VistaVector3D>& coords) const;

	// Rendering attributes
	bool GetRenderingAttributes(VistaRenderingAttributes& attr) const;
	bool SetRenderingAttributes(const VistaRenderingAttributes& attr);

	float GetTransparency() const;

	bool SetTransparency(const float& transparency);

	/** Given a face index and a point this functions returns the barycentric coordinates
	 *  of the point in relation to the face. Let be v0, v1 and v2 the vertices in the
	 *  order given back by WTK. And let x=v1-v0 and y=v2-v0, then v0+a*x+b*y=point. Computing
	 *  the third barycentric coordinate is up to the user (simply compute c = 1 - (a+b));
	 *  @param const int face : id of the face
	 *  @param const VistaVector3D& point : point for which the barycentric coords are computed
	 *  @param float& a : first barycentric coordinate
	 *  @param float& b : second barycentric coordinate
	 *  @return bool : false if point outside of face or there is an error in computation, true in all other cases
	 */
	bool ComputeBarycentricCoords(const int, const VistaVector3D&, float&, float&) const;
	/** Given the identifier of a face GetFaceCoords returns the coordinates, i.e. the locations of
	 *  the vertices of the face
	 *  @params const int idx : identifier of the face
	 *  @params VistaVector3D& a : first vertex of the face
	 *  @params VistaVector3D& b : second vertex of the face
	 *  @params VistaVector3D& c : third vertex of the face
	 */
	bool GetFaceCoords(const int idx, VistaVector3D& a, VistaVector3D& b, VistaVector3D& c) const;
	bool GetFaceVertices(const int idx, int& vertexId0, int& vertexId1, int& vertexId2 ) const;
	bool GetFaceCoords(const int idx, std::vector<int> &coords, int &nMod) const;
	/** Given the identifier of a face GetFaceBBox returns the corners of the bounding box of
	 *  the face.
	 *  @params const int idx : identifier of the face
	 *  @params VistaVector3D& min : first corner of the bounding box
	 *  @params VistaVector3D& max : second corner of the bounding box
	 */
	bool GetFaceBBox(const int idx, VistaVector3D& min, VistaVector3D& max) const;

	IVistaGeometryData* GetData() { return m_pData; }
	const IVistaGeometryData* GetData() const { return m_pData; }

	/** Initializes the halfedge datastructure, which is needed for topological
	 * queries and changes.
	 */
	void InitTopologyData();

	/** If the halfedge datastructure is initialized topological
	 * information about the geometry is available. As a consequence it's possible
	 * to receive the neighbors of the face in one single step. Given a face and
	 * a vertex of this face (via it's identifier) this function returns the face,
	 * that shares the edge from the given vertex to the next one (as usual
	 * counterclockwise) with the given face.
	 * @param int faceId : a face whose neihgbor has to be found
	 * @param int edgeId : which neighbor has to be found, valid values: 0,1,2
	 * @return bool true/false
	 */
	int GetNeighbor (const int faceId, const int edgeId) const;
	/** If the halfedge datastructure is initialized topological
	 * information about the geometry is available. As a consequence it's possible
	 * to receive the neighbors of the face in one single step. Given a face and
	 * a vertex of this face (via it's identifier) this function returns the face,
	 * that shares the edge from the given vertex to the next one (as usual
	 * counterclockwise) with the given face. After calling this function you'll
	 * have the correct values in retFaceID and retVertexID.
	 * @param int faceId : a face whose neihgbor has to be found
	 * @param int edgeId : which neighbor has to be found, valid values: 0,1,2
	 * @param int& retFaceId : neighbor face Id
	 * @param int& retEdgeId : which neighbor of the retFace is the current face
	 * @return bool true/false
	 */
	bool GetNeighbor(const int faceId, const int edgeId, int& retFaceId, int& retEdgeId) const;
	/** Returns the neighbors neighbor1, neighbor2 and neighbor3 of the face "face". This
	 * is of course only possible if topological information about the mesh is available.
	 * @param int faceId : a face whose neihgbors have to be found
	 * @param int neighbors[3] : the found neighbors
	 * @return bool true/false
	 */
	bool GetNeighbors(const int faceId, int neighbors[3]) const;

	/** Add new face defined by vertices 0,1,2
	 * @param int faceId : faceId of the new face can be specified.
	 */
	bool AddFace(const int vertexId0, const int vertexId1, const int vertexId2);
	bool AddFace(const int vertexId0, const int vertexId1, const int vertexId2, int& faceId);
	/** Delete face faceId. Resulting isolated vertices will be deleted if
	 * deleteVertices is true
	 * @param int faceId : face to be deleted
	 * @param bool deleteVertices : delete isolated vertices
	 * @return bool true/false
	 */
	bool DeleteFace(const int faceId, bool deleteVertices = true);
	/** Create a new vertex at a given position.
	 * @return int new vertexId
	 */
	int AddVertex(const VistaVector3D& pos = VistaVector3D(0,0,0));
	/** Create a new vertex at the position defined by the barycentric
	 *  coordinates a,b in relation to face
	 *  @param const int face : identifier of the face
	 *  @param const float a : first barycentric coordinate
	 *  @param const float b : second barycentric coordinate
	 *  @return int identifier of the newly created vertex
	 */
	int AddVertex(const int face, const float a, const float b);
	/** Delete vertex vertexId and all incident faces
	 */
	bool DeleteVertex(const int vertexId);
	/** Is face faceId a boundary, i.e. is one of its edges a boundary edge
	 */
	bool IsBoundary(const int faceId);
	/** Check if the geometry is valid.
	 */
	bool IsConsistent() const;

	bool GetEdge(const int vertexId0, const int vertexId1, int &face, int &edgeID) const;

	/** Return a list of adjacent faces. This will fail on weird geometries (nodes connecting
	 *  two geometries.
	 *  @param const int vertexID : ID of the node
	 *  @param list<int>& listFaces : the list of faces adjacent to node nodeID of face face.
	 *  @return bool
	 */
	bool GetAdjacentFaces(const int vertexID, std::list<int>& listFaces);


protected:
	VistaGeometry(IVistaGraphicsBridge*, IVistaGeometryData*);
	VistaGeometry();

private:
	int GetEdge(const int vertexId0, const int vertexId1) const;

private:
	IVistaGraphicsBridge*   m_pBridge;
	IVistaGeometryData*             m_pData;

	std::string m_strTextureFileName;
	int m_iMaterialIndex;

	// topological information
	std::vector<int>                     m_halfedges;
	typedef std::set<int>        SETINT;
	std::vector<SETINT>          m_adjacentFaces;

private:
	//Counts the geometry-nodes currently adressing a specific geometry object
//      int m_nNodeCounter;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAGEOMETRY_H
