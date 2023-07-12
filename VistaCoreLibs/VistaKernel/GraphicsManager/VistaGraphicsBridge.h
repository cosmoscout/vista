/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright ( c ) 1997-2011 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  ( at your option ) any later version.                                       */
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
// $nId: VistaGraphicsBridge.h 22163 2011-07-03 14:11:15Z dr165799 $

#ifndef _VISTAGRAPHICSBRIDGE_H
#define _VISTAGRAPHICSBRIDGE_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/GraphicsManager/VistaGeometry.h>

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
// class VistaVertex;
class VistaVertexFormat;
class VistaFace;
class VistaVector3D;
class VistaPlane;
class VistaRay;
class VistaColor;
class VistaMaterial;

class VistaRenderingAttributes;

class Vista2DDrawingObject;
class Vista2DText;
class Vista2DBitmap;
class Vista2DLine;
class Vista2DRectangle;
class VistaGeometry;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI IVistaGeometryData {
  friend class VistaGeometry;

 public:
  virtual ~IVistaGeometryData(){};

 protected:
};

class VISTAKERNELAPI IVistaGraphicsBridge {
 public:
  virtual ~IVistaGraphicsBridge();

  /*******************************************/
  /* Global States                           */
  /*******************************************/

  virtual VistaColor GetBackgroundColor() const                  = 0;
  virtual void       SetBackgroundColor(const VistaColor& color) = 0;

  virtual bool GetFrustumCullingEnabled() const               = 0;
  virtual void SetFrustumCullingEnabled(bool bCullingEnabled) = 0;

  virtual bool GetOcclusionCullingEnabled() const                  = 0;
  virtual void SetOcclusionCullingEnabled(bool bOclCullingEnabled) = 0;

  virtual bool GetBBoxDrawingEnabled() const      = 0;
  virtual void SetBBoxDrawingEnabled(bool bState) = 0;

  /*******************************************/
  /* Geometry Creation                       */
  /*******************************************/

  virtual IVistaGeometryData* NewGeometryData() const                             = 0;
  virtual bool                DeleteGeometryData(IVistaGeometryData* pData) const = 0;

  virtual bool CreateIndexedGeometry(const std::vector<VistaIndexedVertex>& vecVertices,
      const std::vector<VistaVector3D>&                                     vecCoords,
      const std::vector<VistaVector3D>&                                     vecTexextureCoords2D,
      const std::vector<VistaVector3D>& vecNormals, const std::vector<VistaColor>& vecColors,
      const VistaVertexFormat& oFormat, const VistaGeometry::FaceType fType,
      IVistaGeometryData* pData) = 0;

  virtual bool CreateIndexedGeometry(const std::vector<VistaIndexedVertex>& vecVertices,
      const std::vector<float>& vecCoords, const std::vector<float>& vecTextureCoords,
      const std::vector<float>& vecNormals, const std::vector<VistaColor>& vecColors,
      const VistaVertexFormat& oFormat, const VistaGeometry::FaceType fType,
      IVistaGeometryData* pData) = 0;

  virtual bool CalcVertexNormals(IVistaGeometryData* pData,
      const float&                                   fCreaseAngle = 0.524f)                 = 0; // 30 degrees
  virtual bool CalcFaceNormals(IVistaGeometryData* pData) = 0;

  /*******************************************/
  /* Geometry Geometric Properties           */
  /*******************************************/

  virtual IVistaGeometryData* CloneGeometryData(const IVistaGeometryData* pGeomData) const = 0;

  virtual int GetNumberOfVertices(const IVistaGeometryData* pData) const = 0;
  virtual int GetNumberOfFaces(const IVistaGeometryData* pData) const    = 0;

  virtual int GetNumberOfColors(const IVistaGeometryData* pData) const        = 0;
  virtual int GetNumberOfCoords(const IVistaGeometryData* pData) const        = 0;
  virtual int GetNumberOfNormals(const IVistaGeometryData* pData) const       = 0;
  virtual int GetNumberOfTextureCoords(const IVistaGeometryData* pData) const = 0;

  virtual VistaVertexFormat       GetVertexFormat(const IVistaGeometryData* pData) const = 0;
  virtual VistaGeometry::FaceType GetFaceType(const IVistaGeometryData* pData) const     = 0;

  // indexed geometry
  virtual bool GetVertices(
      std::vector<VistaIndexedVertex>& vecVertices, const IVistaGeometryData* pData) const = 0;
  virtual bool GetCoordinates(
      std::vector<VistaVector3D>& vecCoords, const IVistaGeometryData* pData) const = 0;
  virtual bool GetCoordinates(
      std::vector<float>& vecCoords, const IVistaGeometryData* pData) const = 0;
  virtual bool GetTextureCoords2D(
      std::vector<VistaVector3D>& vecTexCoords, const IVistaGeometryData* pData) const = 0;
  virtual bool GetTextureCoords2D(
      std::vector<float>& vecTexCoords, const IVistaGeometryData* pData) const = 0;
  virtual bool GetNormals(
      std::vector<VistaVector3D>& vecNormals, const IVistaGeometryData* pData) const = 0;
  virtual bool GetNormals(
      std::vector<float>& vecNormals, const IVistaGeometryData* pData) const = 0;
  virtual bool GetColors(
      std::vector<VistaColor>& vecColors, const IVistaGeometryData* pData) const               = 0;
  virtual bool GetColors(std::vector<float>& vecColors, const IVistaGeometryData* pData) const = 0;
  /**
   * Gives indices for all triangles in the geometry
   */
  virtual bool GetTrianglesVertexIndices(
      std::vector<int>& vecVertexIndices, const IVistaGeometryData* pData) const = 0;
  virtual bool GetTrianglesNormalIndices(
      std::vector<int>& vecNormalIndices, const IVistaGeometryData* pData) const = 0;
  virtual bool GetTrianglesTextureCoordinateIndices(
      std::vector<int>& vecTexCoordIndices, const IVistaGeometryData* pData) const = 0;
  virtual bool GetTrianglesVertexAndNormalIndices(std::vector<int>& vecVertexIndices,
      std::vector<int>& vecNormalIndices, const IVistaGeometryData* pData) const   = 0;

  virtual int  GetCoordinateIndex(const int nIndex, const IVistaGeometryData* pData) const = 0;
  virtual bool GetCoordinate(
      const int nIndex, float a3fCoord[3], const IVistaGeometryData* pData) const              = 0;
  virtual VistaVector3D GetCoordinate(const int nIndex, const IVistaGeometryData* pData) const = 0;

  virtual int  GetNormalIndex(const int nIndex, const IVistaGeometryData* pData) const = 0;
  virtual bool GetNormal(
      const int nIndex, float a3fNormal[3], const IVistaGeometryData* pData) const         = 0;
  virtual VistaVector3D GetNormal(const int nIndex, const IVistaGeometryData* pData) const = 0;

  virtual int        GetColorIndex(const int nIndex, const IVistaGeometryData* pData) const = 0;
  virtual VistaColor GetColor(const int nIndex, const IVistaGeometryData* pData) const      = 0;

  virtual int  GetTextureCoordIndex(const int nIndex, const IVistaGeometryData* pData) const = 0;
  virtual bool GetTextureCoord(
      const int nIndex, float a3fTexCoord[3], const IVistaGeometryData* pData) const = 0;
  virtual VistaVector3D GetTextureCoord(
      const int nIndex, const IVistaGeometryData* pData) const = 0;

  virtual bool GetFaceCoords(const int nIndex, std::vector<int>& vecCoords, int& nMod,
      const IVistaGeometryData* pData) const = 0;
  virtual bool GetFaceVertices(const int nIndex, int& nVertexId0, int& nVertexId1, int& nVertexId2,
      const IVistaGeometryData* pData) const = 0;

  virtual bool SetCoordinates(const int nStartIndex, const std::vector<VistaVector3D>& vecCoords,
      IVistaGeometryData* pData) = 0;
  virtual bool SetCoordinates(
      const int nStartIndex, const std::vector<float>& vecCoords, IVistaGeometryData* pData) = 0;

  virtual bool SetCoordinate(
      const int nIndex, const float a3fCoords[3], IVistaGeometryData* pData) = 0;
  virtual bool SetCoordinate(
      const int nIndex, const VistaVector3D& v3Coord, IVistaGeometryData* pData)            = 0;
  virtual bool SetCoordIndex(const int nIndex, const int nValue, IVistaGeometryData* pData) = 0;
  virtual bool SetCoordIndices(
      const int nStartIndex, const std::vector<int>& indices, IVistaGeometryData* pData) = 0;

  virtual bool SetTextureCoords2D(const int nStartIndex,
      const std::vector<VistaVector3D>& vecTextureCoords2D, IVistaGeometryData* pData) = 0;
  virtual bool SetTextureCoords2D(const int nStartIndex,
      const std::vector<float>& vecTextureCoords2D, IVistaGeometryData* pData)         = 0;

  virtual bool SetTextureCoord2D(
      const int nIndex, const float a3fCoord[3], IVistaGeometryData* pData) = 0;
  virtual bool SetTextureCoord2D(
      const int nIndex, const VistaVector3D& v3Coord, IVistaGeometryData* pData) = 0;

  virtual bool SetTextureCoordIndex(
      const int nIndex, const int value, IVistaGeometryData* pData) = 0;
  virtual bool SetTextureCoordsIndices(
      const int nStartIndex, const std::vector<int>& vecIndices, IVistaGeometryData* pData) = 0;

  virtual bool SetNormals(
      const int nStartIndex, const std::vector<float>& vecNormals, IVistaGeometryData* pData) = 0;
  virtual bool SetNormals(const int nStartIndex, const std::vector<VistaVector3D>& vecNormals,
      IVistaGeometryData* pData)                                                              = 0;

  virtual bool SetNormal(const int nIndex, const float a3fNormal[3], IVistaGeometryData* pData) = 0;
  virtual bool SetNormal(
      const int nIndex, const VistaVector3D& v3Normal, IVistaGeometryData* pData)           = 0;
  virtual bool SetNormalIndex(const int nIndex, const int value, IVistaGeometryData* pData) = 0;
  virtual bool SetNormalIndices(
      const int nStartIndex, const std::vector<int>& vecIndices, IVistaGeometryData* pData) = 0;

  virtual bool SetColors(const int nStartIndex, const std::vector<VistaColor>& vecColors,
      IVistaGeometryData* pData) = 0;
  virtual bool SetColors(const int nStartIndex, const int nBufferLength, float* afColors,
      IVistaGeometryData* pData) = 0;

  virtual bool SetColor(const int nIndex, const VistaColor& oColor, IVistaGeometryData* pData) = 0;
  virtual bool SetColorIndex(const int nIndex, const int nValue, IVistaGeometryData* pData)    = 0;
  virtual bool SetColorIndices(
      const int nStartIndex, const std::vector<int>& vecIndices, IVistaGeometryData* pData) = 0;

  /*******************************************/
  /* Individual Face/Coord methods           */
  /*******************************************/
  virtual bool GetFaces(std::vector<int>& vecFaces, const IVistaGeometryData* pData) const = 0;
  virtual bool GetFaceBoundingBox(const int nIndex, VistaVector3D& v3Min, VistaVector3D& v3Max,
      const IVistaGeometryData* pData) const                                               = 0;
  virtual bool GetFaceCoords(const int nIndex, VistaVector3D& v3VertexA, VistaVector3D& v3VertexB,
      VistaVector3D& v3VertexC, const IVistaGeometryData* pData) const                     = 0;

  /** Add new face defined by vertices 0,1,2
   * @param int faceId : faceId of the new face can be specified.
   */
  virtual bool AddFace(const int nVertexId0, const int nVertexId1, const int nVertexId2,
      int& nFaceId, IVistaGeometryData* pData) = 0;
  /** Delete face faceId. Resulting isolated vertices will be deleted if
   * deleteVertices is true
   * @param int faceId : face to be deleted
   * @param bool deleteVertices : delete isolated vertices
   * @return bool true/false
   */
  virtual bool DeleteFace(const int nFaceId, bool bDeleteVertices, IVistaGeometryData* pData) = 0;
  /** Create a new vertex at a given position.
   * @return int new vertexId
   */
  virtual int AddVertex(const VistaVector3D& v3Pos, IVistaGeometryData* pData) = 0;
  /**
   * Delete vertex vertexId and all incident faces
   */
  virtual bool DeleteVertex(const int nVertexId, IVistaGeometryData* pData) = 0;

  /*******************************************/
  /* Materials                               */
  /*******************************************/

  virtual bool CreateMaterialTable() = 0;

  virtual int  GetNumberOfMaterials()                                                = 0;
  virtual int  AddMaterial(const VistaMaterial& Material)                            = 0;
  virtual bool SetMaterialIndex(const int& materialIndex, IVistaGeometryData* pData) = 0;
  virtual bool GetMaterialByIndex(int iIndex, VistaMaterial& oIn) const              = 0;
  virtual bool GetMaterialByName(const std::string& sName, VistaMaterial& oIn) const = 0;

  virtual bool SetColor(const VistaColor& oColor, IVistaGeometryData* pData) = 0;

  virtual bool SetMaterial(const VistaMaterial& oMaterial, IVistaGeometryData* pData) = 0;
  virtual bool BindToVistaMaterialTable(IVistaGeometryData* pData)                    = 0;

  /*******************************************/
  /* Textures                                */
  /*******************************************/

  virtual bool SetTexture(const std::string&, IVistaGeometryData* pData) = 0;
  virtual bool SetTexture(const int nId,           /**< unknown parameter ? */
      const int                     nWidth,        /**< width in pixel */
      const int                     nHeight,       /**< height in pixel */
      const int                     nBitsPerPixel, /**< bits per color channel ( 8,32 ) */
      VistaType::byte*              pBuffer,       /**< pointer to pixelbuffer RGB( A ) */
      bool                bHasAlpha, /**< pixel data has alpha or not RGBA iff true, RGB else */
      IVistaGeometryData* pData)                                         = 0;
  virtual bool DeleteTexture(IVistaGeometryData* pData)                  = 0;

  /*******************************************/
  /* Various methods                         */
  /*******************************************/
  virtual bool ScaleGeometry(const float, IVistaGeometryData* pData)                           = 0;
  virtual bool ScaleGeometry(const float[3], IVistaGeometryData* pData)                        = 0;
  virtual bool ScaleGeometry(const float, const float, const float, IVistaGeometryData* pData) = 0;

  virtual bool GetBoundingBox(
      VistaVector3D& v3Min, VistaVector3D& v3Max, const IVistaGeometryData* pData) const = 0;

  virtual bool GetRenderingAttributes(
      VistaRenderingAttributes& oAttrib, const IVistaGeometryData* pData) const = 0;
  virtual bool SetRenderingAttributes(
      const VistaRenderingAttributes& oAttrib, IVistaGeometryData* pData) = 0;

  virtual float GetTransparency(const IVistaGeometryData* pData) const          = 0;
  virtual bool  SetTransparency(float fTransparency, IVistaGeometryData* pData) = 0;

  virtual bool GetIsStatic(const IVistaGeometryData* pData)           = 0;
  virtual void SetIsStatic(bool bIsStatic, IVistaGeometryData* pData) = 0;

  /*******************************************/
  /* NON-VIRTUAL FUNCTIONS                   */
  /*******************************************/
  VistaGeometry* NewGeometry(IVistaGeometryData* pData);

 protected:
  IVistaGraphicsBridge();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
