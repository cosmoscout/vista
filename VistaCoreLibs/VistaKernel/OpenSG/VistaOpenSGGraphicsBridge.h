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


#ifndef _VISTAOPENSGGRAPHICSBRIDGE_H
#define _VISTAOPENSGGRAPHICSBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsBridge.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/OpenSG/VistaOpenSGSystemClassFactory.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif

#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGImageForeground.h>
#include <OpenSG/OSGPolygonChunk.h>
#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGTextureChunk.h>
#include <OpenSG/OSGBlendChunk.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif
/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class Vista2DText;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaOpenSGGeometryData : public IVistaGeometryData
{
public:
	VistaOpenSGGeometryData();
	virtual ~VistaOpenSGGeometryData();

	osg::GeometryPtr GetGeometry() const;
	bool GetGeometry( osg::GeometryPtr & geom ) const;
	bool SetGeometry( const osg::GeometryPtr & geom );

	osg::PolygonChunkPtr GetAttrChunk() const;
	bool SetAttrChunk(osg::PolygonChunkPtr ptr);

	VistaRenderingAttributes GetRenderingAttributes() const;
	bool GetRenderingAttributes( VistaRenderingAttributes & renderingattributes ) const;
	bool SetRenderingAttributes( const VistaRenderingAttributes & renderingattributes );

	osg::Int32 GetOsgFaceType() const;
	void SetOsgFaceType(osg::Int32 nType);

private:
	osg::GeometryRefPtr          m_ptrGeometry;
	VistaRenderingAttributes m_pRenderingAttributes;
	osg::PolygonChunkRefPtr           m_ptrAttrChunk;
	osg::Int32 m_nCachedFaceType;
};

/*============================================================================*/
class VISTAKERNELAPI VistaOpenSGGraphicsBridge : public IVistaGraphicsBridge
{
public:

	VistaOpenSGGraphicsBridge(osg::RenderAction * pAction,
							   VistaOpenSGSystemClassFactory * pFactory );
	virtual ~VistaOpenSGGraphicsBridge();

	// additional debuging for the OSG scene graph content
	void DebugOSG();	

	// little helper to get the osg::RenderAction from the DisplayBridge
	osg::RenderAction* GetRenderAction() const { return m_pRenderAction; }

	bool GetOSGMaterialByIndex(int iIndex, osg::ChunkMaterialPtr & stm);


	/////////////////////////////////////////////
	// GraphicsBridge Interface Implementation //
	/////////////////////////////////////////////

	/**
	 * Global States
	 */

	virtual VistaColor GetBackgroundColor() const;
	virtual void SetBackgroundColor( const VistaColor&  color );

	virtual bool GetFrustumCullingEnabled() const;
	virtual void SetFrustumCullingEnabled( bool bCullingEnabled );
	
	virtual bool GetOcclusionCullingEnabled() const;
	virtual void SetOcclusionCullingEnabled( bool bOclCullingEnabled );

	virtual bool GetBBoxDrawingEnabled() const;
	virtual void SetBBoxDrawingEnabled( bool bState );


	/**
	 * Geometry Creation 
	 */
	virtual IVistaGeometryData* NewGeometryData() const;
	virtual bool DeleteGeometryData( IVistaGeometryData* pData ) const;

	virtual bool CreateIndexedGeometry( const std::vector<VistaIndexedVertex>& vecVertices,
									const std::vector<VistaVector3D>& vecCoords,
									const std::vector<VistaVector3D>& vecTexextureCoords2D,
									const std::vector<VistaVector3D>& vecNormals,
									const std::vector<VistaColor>& vecColorsRGB,
									const VistaVertexFormat& oFormat,
									const VistaGeometry::FaceType fType,
									IVistaGeometryData* pData );

	virtual bool CreateIndexedGeometry( const std::vector<VistaIndexedVertex>& vecVertices,
									const std::vector<float>& vecCoords,
									const std::vector<float>& vecTextureCoords,
									const std::vector<float>& vecNormals,
									const std::vector<VistaColor>& vecColors,
									const VistaVertexFormat& oFormat,
									const VistaGeometry::FaceType fType,
									IVistaGeometryData* pData );

	virtual bool CalcVertexNormals( IVistaGeometryData* pData,
									const float& fCreaseAngle = 0.524f ); // 30 degrees
	virtual bool CalcFaceNormals( IVistaGeometryData* pData );

	virtual IVistaGeometryData* CloneGeometryData( const IVistaGeometryData* pGeomData ) const;
	

	/**
	 * Geometry Geometric Properties
	 */

	virtual int GetNumberOfVertices( const IVistaGeometryData* pData ) const;
	virtual int GetNumberOfFaces( const IVistaGeometryData* pData ) const;

	virtual int GetNumberOfColors( const IVistaGeometryData* pData ) const;
	virtual int GetNumberOfCoords( const IVistaGeometryData* pData ) const;
	virtual int GetNumberOfNormals( const IVistaGeometryData* pData ) const;
	virtual int GetNumberOfTextureCoords( const IVistaGeometryData* pData ) const;

	virtual VistaVertexFormat GetVertexFormat( const IVistaGeometryData* pData ) const;
	virtual VistaGeometry::FaceType	GetFaceType( const IVistaGeometryData* pData ) const;
	
	// indexed geometry
	virtual bool GetVertices( std::vector<VistaIndexedVertex>& vecVertices,
									const IVistaGeometryData* pData ) const;
	virtual bool GetCoordinates( std::vector<VistaVector3D>& vecCoords,
									const IVistaGeometryData* pData ) const;	
	virtual bool GetCoordinates( std::vector<float>& vecCoords,
									const IVistaGeometryData* pData ) const;
	virtual bool GetTextureCoords2D( std::vector<VistaVector3D>& vecTexCoords,
									const IVistaGeometryData* pData ) const;
	virtual bool GetTextureCoords2D( std::vector<float>& vecTexCoords,
									const IVistaGeometryData* pData ) const;
	virtual bool GetNormals( std::vector<VistaVector3D>& vecNormals,
									 const IVistaGeometryData* pData ) const;
	virtual bool GetNormals( std::vector<float>& vecNormals,
									const IVistaGeometryData* pData ) const;
	virtual bool GetColors( std::vector<VistaColor>& vecColors,
									const IVistaGeometryData* pData ) const;
	virtual bool GetColors( std::vector<float>& vecColors,
									const IVistaGeometryData* pData ) const;

	/**
	 * Gives indices for all triangles in the geometry
	 */
	virtual bool GetTrianglesVertexIndices( std::vector<int>& vecVertexIndices,
									const IVistaGeometryData* pData ) const;
	virtual bool GetTrianglesNormalIndices( std::vector<int>& vecNormalIndices,
									const IVistaGeometryData* pData ) const;
	virtual bool GetTrianglesTextureCoordinateIndices( std::vector<int>& vecTexCoordIndices,
									const IVistaGeometryData* pData ) const;
	virtual bool GetTrianglesVertexAndNormalIndices( std::vector<int>& vecVertexIndices,
									std::vector<int>& vecNormalIndices,
									const IVistaGeometryData* pData ) const;

	virtual int  GetCoordinateIndex( const int nIndex,
									const IVistaGeometryData* pData ) const;
	virtual bool GetCoordinate( const int nIndex, float a3fCoord[3],
									const IVistaGeometryData* pData ) const;
	virtual VistaVector3D GetCoordinate( const int nIndex,
									const IVistaGeometryData* pData ) const;

	virtual int  GetNormalIndex( const int nIndex, const IVistaGeometryData* pData ) const;
	virtual bool GetNormal( const int nIndex, float a3fNormal[3],
									const IVistaGeometryData* pData ) const;
	virtual VistaVector3D GetNormal( const int nIndex, const IVistaGeometryData* pData ) const;

	virtual int  GetColorIndex( const int nIndex, const IVistaGeometryData* pData ) const;
	virtual VistaColor GetColor( const int nIndex, const IVistaGeometryData* pData ) const;

	virtual int  GetTextureCoordIndex( const int nIndex, const IVistaGeometryData* pData ) const;
	virtual bool GetTextureCoord( const int nIndex, float a3fTexCoord[3],
									const IVistaGeometryData* pData ) const;
	virtual VistaVector3D GetTextureCoord( const int nIndex,
									const IVistaGeometryData* pData ) const;

	virtual bool GetFaceCoords( const int nIndex, std::vector<int>& vecCoords, int& nMod,
									const IVistaGeometryData* pData ) const;
	virtual bool GetFaceVertices( const int nIndex, int& nVertexId0,
									int& nVertexId1, int& nVertexId2,
									const IVistaGeometryData* pData ) const;

	virtual bool SetCoordinates( const int nStartIndex,
									const std::vector<VistaVector3D>& vecCoords,
									IVistaGeometryData* pData );
	virtual bool SetCoordinates( const int nStartIndex,
									const std::vector<float>& vecCoords,
									IVistaGeometryData* pData );

	virtual bool SetCoordinate( const int nIndex, const float a3fCoords[3],
									IVistaGeometryData* pData );
	virtual bool SetCoordinate( const int nIndex, const VistaVector3D& v3Coord,
									IVistaGeometryData* pData );
	virtual bool SetCoordIndex( const int nIndex, const int nValue,
									IVistaGeometryData* pData );
	virtual bool SetCoordIndices( const int nStartIndex, const std::vector<int>& indices,
									IVistaGeometryData* pData );

	virtual bool SetTextureCoords2D( const int nStartIndex,
									const std::vector<VistaVector3D>& vecTextureCoords2D,
									IVistaGeometryData* pData );
	virtual bool SetTextureCoords2D( const int nStartIndex,
									const std::vector<float>& vecTextureCoords2D,
									IVistaGeometryData* pData );

	virtual bool SetTextureCoord2D( const int nIndex, const float a3fCoord[3],
									IVistaGeometryData* pData );
	virtual bool SetTextureCoord2D( const int nIndex, const VistaVector3D& v3Coord,
									 IVistaGeometryData* pData );

	virtual bool SetTextureCoordIndex( const int nIndex, const int value,
									IVistaGeometryData* pData );
	virtual bool SetTextureCoordsIndices( const int nStartIndex, const std::vector<int>& vecIndices,
									IVistaGeometryData* pData );

	virtual bool SetNormals( const int nStartIndex, const std::vector<float>& vecNormals,
									IVistaGeometryData* pData );
	virtual bool SetNormals( const int nStartIndex, const std::vector<VistaVector3D>& vecNormals,
									IVistaGeometryData* pData );

	virtual bool SetNormal( const int nIndex, const float a3fNormal[3],
									IVistaGeometryData* pData );
	virtual bool SetNormal( const int nIndex, const VistaVector3D& v3Normal,
									IVistaGeometryData* pData );
	virtual bool SetNormalIndex( const int nIndex, const int value,
									IVistaGeometryData* pData );
	virtual bool SetNormalIndices( const int nStartIndex, const std::vector<int>& vecIndices,
									IVistaGeometryData* pData );

	virtual bool SetColors( const int nStartIndex,
									const std::vector<VistaColor>& vecColors,
									IVistaGeometryData* pData );
	virtual bool SetColors( const int nStartIndex, const int nBufferLength,
									float* afColors,
									IVistaGeometryData* pData );

	virtual bool SetColor( const int nIndex, const VistaColor& oColor,
									IVistaGeometryData* pData );
	virtual bool SetColorIndex( const int nIndex, const int nValue,
									IVistaGeometryData* pData );
	virtual bool SetColorIndices( const int nStartIndex,
									const std::vector<int>& vecIndices,
									IVistaGeometryData* pData );

	/**
	 * Individual Faces/Coords
	 */
	virtual bool GetFaces( std::vector<int>& vecFaces,
									const IVistaGeometryData* pData ) const;
	virtual bool GetFaceBoundingBox( const int nIndex,
									VistaVector3D& v3Min, VistaVector3D& v3Max,
									const IVistaGeometryData* pData ) const;
	virtual bool GetFaceCoords( const int nIndex, VistaVector3D& v3VertexA,
									VistaVector3D& v3VertexB, VistaVector3D& v3VertexC,
									const IVistaGeometryData* pData ) const;

	
	/** Add new face defined by vertices 0,1,2
	 * @param int faceId : faceId of the new face can be specified.
	 */
	virtual bool AddFace( const int nVertexId0, const int nVertexId1, const int nVertexId2,
						int& nFaceId, IVistaGeometryData* pData );
	/** Delete face faceId. Resulting isolated vertices will be deleted if
	 * deleteVertices is true
	 * @param int faceId : face to be deleted
	 * @param bool deleteVertices : delete isolated vertices
	 * @return bool true/false
	 */
	virtual bool DeleteFace( const int nFaceId, bool bDeleteVertices, IVistaGeometryData* pData );
	/** Create a new vertex at a given position.
	 * @return int new vertexId
	 */
	virtual int AddVertex( const VistaVector3D& v3Pos, IVistaGeometryData* pData );
	/**
	 * Delete vertex vertexId and all incident faces
	 */
	virtual bool DeleteVertex( const int nVertexId, IVistaGeometryData* pData );
	

	/**
	 * Materials
	 */

	virtual	bool CreateMaterialTable();
	virtual int	 GetNumberOfMaterials();

	/**
	 * @todo
	 * this one is really dangerous!!!!
	 * any material used in an opensg geometry will be destroyed with the geometry object,
	 * IS THIS METHOD IN USE SOMEWEHRE!!!??
	 */
	virtual int	 AddMaterial( const VistaMaterial& Material );
	virtual bool SetMaterialIndex( const int& materialIndex,
									IVistaGeometryData* pData );
	virtual bool GetMaterialByIndex( int iIndex, VistaMaterial& oIn ) const;
	virtual bool GetMaterialByName( const std::string& sName, VistaMaterial& oIn ) const;

	virtual bool SetColor( const VistaColor& oColor, IVistaGeometryData* pData );

	virtual bool SetMaterial( const VistaMaterial& oMaterial,
									IVistaGeometryData* pData );
	virtual bool BindToVistaMaterialTable( IVistaGeometryData* pData );

	/**
	 * Textures
	 */

	virtual bool SetTexture( const std::string& , IVistaGeometryData* pData );
	virtual bool SetTexture( const int nId,				/**< unknown parameter ? */
									const int nWidth,			/**< width in pixel */
									const int nHeight,			/**< height in pixel */
									const int nBitsPerPixel,	/**< bits per color channel ( 8,32 ) */
									VistaType::byte* pBuffer,		/**< pointer to pixelbuffer RGB( A ) */
									bool bHasAlpha,				/**< pixel data has alpha or not RGBA iff true, RGB else */
									IVistaGeometryData* pData );
	virtual bool DeleteTexture( IVistaGeometryData* pData );

	/**
	 * various
	 */
	virtual bool ScaleGeometry( const float, IVistaGeometryData* pData );
	virtual bool ScaleGeometry( const float[3], IVistaGeometryData* pData );
	virtual bool ScaleGeometry( const float, const float, const float, IVistaGeometryData* pData );

	virtual bool GetBoundingBox( VistaVector3D& v3Min, VistaVector3D& v3Max, 
									const IVistaGeometryData* pData ) const;	

	virtual bool GetRenderingAttributes( VistaRenderingAttributes& oAttrib,
									const IVistaGeometryData* pData ) const;
	virtual	bool SetRenderingAttributes( const VistaRenderingAttributes& oAttrib,
									IVistaGeometryData* pData );

	virtual float GetTransparency( const IVistaGeometryData* pData ) const;
	virtual bool SetTransparency( float fTransparency, IVistaGeometryData* pData );

	virtual bool GetIsStatic( const IVistaGeometryData* pData );
	virtual void SetIsStatic( bool bIsStatic, IVistaGeometryData* pData );

private:
	bool SetTexture(osg::ImagePtr image, IVistaGeometryData* pData);

	bool m_bShowCursor;

	VistaOpenSGGraphicsBridge();
	VistaOpenSGSystemClassFactory * m_pFactory;
	osg::RenderAction * m_pRenderAction;

	std::vector<VistaMaterial>   m_vMaterialTable;
	std::vector<osg::ChunkMaterialPtr> m_vOSGMatTab;
	std::map<std::string,osg::ImagePtr> m_mTextureTable;

	// internal helper to avoid loading the same image twice from disc
	osg::ImagePtr GetCachedOrLoadImage( const std::string &sFileName );

};


#endif

