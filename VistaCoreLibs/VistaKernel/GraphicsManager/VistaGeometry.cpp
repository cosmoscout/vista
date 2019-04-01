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


#include "VistaGeometry.h"

#include <VistaKernel/GraphicsManager/VistaGraphicsBridge.h>

#include <VistaMath/VistaGeometries.h>
#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

using namespace std;

static const float EPSILON = 0.00001f;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaGeometry::VistaGeometry()
: m_pBridge( NULL )
, m_pData( NULL )
, m_iMaterialIndex( -1 )
{
}

VistaGeometry::VistaGeometry(IVistaGraphicsBridge* pBridge, IVistaGeometryData* pData)
: m_pBridge( pBridge )
, m_pData( pData )
, m_iMaterialIndex( -1 )
{
}

VistaGeometry::~VistaGeometry()
{
	delete m_pData;
}

VistaIndexedVertex::VistaIndexedVertex()
{
	m_nCoordinatesIndex = -1;
	m_nNormalIndex = -1;
	m_nColorIndex = -1;
	m_nTextureCoordIndex = -1;
}

VistaIndexedVertex::VistaIndexedVertex(const int coordIndex)
{
	m_nCoordinatesIndex = coordIndex;
	m_nNormalIndex = -1;
	m_nColorIndex = -1;
	m_nTextureCoordIndex = -1;
}

VistaIndexedVertex::~VistaIndexedVertex()
{
}

void VistaIndexedVertex::SetCoordinateIndex( const int nCoordinatesIndex )
{
	m_nCoordinatesIndex = nCoordinatesIndex;
}

int VistaIndexedVertex::GetCoordinateIndex() const
{
	return m_nCoordinatesIndex;
}

void VistaIndexedVertex::SetNormalIndex( const int nNormalIndex )
{
	m_nNormalIndex = nNormalIndex;
}

int VistaIndexedVertex::GetNormalIndex() const
{
	return m_nNormalIndex;
}

void VistaIndexedVertex::SetColorIndex( const int nColorIndex )
{
	m_nColorIndex = nColorIndex;
}

int VistaIndexedVertex::GetColorIndex() const
{
	return m_nColorIndex;
}

void VistaIndexedVertex::SetTextureCoordinateIndex( const int nTextureCoordIndex )
{
	m_nTextureCoordIndex = nTextureCoordIndex;
}

int VistaIndexedVertex::GetTextureCoordinateIndex() const
{
	return m_nTextureCoordIndex;
}

// ============================================================================
// ============================================================================
bool VistaGeometry::CalcVertexNormals(const float fCreaseAngle)
{
	return m_pBridge->CalcVertexNormals( m_pData, fCreaseAngle );
}
bool VistaGeometry::CalcFaceNormals()
{
	return m_pBridge->CalcFaceNormals( m_pData );
}

int  VistaGeometry::GetCoordinateIndex(const int idx) const
{
	return m_pBridge->GetCoordinateIndex(idx, m_pData);
}

bool VistaGeometry::GetCoordinate(const int idx, float fCoord[3]) const
{
	return m_pBridge->GetCoordinate(idx, fCoord, m_pData);
}

VistaVector3D VistaGeometry::GetCoordinate(const int idx) const
{
	return m_pBridge->GetCoordinate(idx, m_pData);
}

bool VistaGeometry::SetCoordinates(const int startIndex, const vector<float>& coords)
{
	if( m_pBridge->SetCoordinates(startIndex, coords, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetCoordinates(const int startIndex, const vector<VistaVector3D>& coords)
{
	if( m_pBridge->SetCoordinates(startIndex, coords, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetCoordinate(const int idx, const float coord[3])
{
	if( m_pBridge->SetCoordinate(idx, coord, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetCoordinate(const int idx, const VistaVector3D &coord)
{
	if( m_pBridge->SetCoordinate(idx, coord, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetCoordIndex(const int idx, const int value)
{
	if( m_pBridge->SetCoordIndex(idx, value, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetCoordIndices(const int startIdx, const vector<int>& indices)
{
	if( m_pBridge->SetCoordIndices(startIdx, indices, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

// ============================================================================
int  VistaGeometry::GetTextureCoordIndex(const int idx) const
{
	return m_pBridge->GetTextureCoordIndex(idx, m_pData);
}

VistaVector3D VistaGeometry::GetTextureCoord(const int idx) const
{
	return m_pBridge->GetTextureCoord(idx, m_pData);
}

bool VistaGeometry::SetTextureCoords2D(const int startIndex, const vector<VistaVector3D>& textureCoords2D)
{
	if( m_pBridge->SetTextureCoords2D(startIndex, textureCoords2D, m_pData) )
	{
		Notify( MSG_SET_TEXTURECOORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetTextureCoords2D(const int startIndex, const vector<float>& textureCoords2D)
{
	if( m_pBridge->SetTextureCoords2D(startIndex, textureCoords2D, m_pData) )
	{
		Notify( MSG_SET_TEXTURECOORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetTextureCoord2D(const int idx, const VistaVector3D& coord)
{
	if( m_pBridge->SetTextureCoord2D(idx, coord, m_pData) )
	{
		Notify( MSG_SET_TEXTURECOORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetTextureCoordIndex(const int idx, const int value)
{
	if( m_pBridge->SetTextureCoordIndex(idx, value, m_pData) )
	{
		Notify( MSG_SET_TEXTURECOORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::SetTextureCoordsIndices(const int startIdx, const vector<int>& indices)
{
	if( m_pBridge->SetTextureCoordsIndices(startIdx, indices, m_pData) )
	{
		Notify( MSG_SET_TEXTURECOORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::GetTrianglesVertexIndices( vector<int>& vecVertexIndices) const
{
	return m_pBridge->GetTrianglesVertexIndices( vecVertexIndices, m_pData );
}

bool VistaGeometry::GetTrianglesNormalIndices( vector<int>& vecNormalIndices) const
{
	return m_pBridge->GetTrianglesNormalIndices( vecNormalIndices, m_pData);
}

bool VistaGeometry::GetTrianglesTextureCoordinateIndices( std::vector<int>& vecTexCoordIndices ) const
{
	return m_pBridge->GetTrianglesTextureCoordinateIndices( vecTexCoordIndices, m_pData );
}


bool VistaGeometry::GetTrianglesVertexAndNormalIndices( vector<int>& vecVertexIndices,
								vector<int>& vecNormalIndices) const
{
	return m_pBridge->GetTrianglesVertexAndNormalIndices( vecVertexIndices, vecNormalIndices, m_pData );
}



// ============================================================================

int  VistaGeometry::GetNormalIndex(const int idx) const
{
	return m_pBridge->GetNormalIndex(idx, m_pData);
}

VistaVector3D VistaGeometry::GetNormal(const int idx) const
{
	return m_pBridge->GetNormal(idx, m_pData);
}

bool VistaGeometry::GetNormal(const int idx, float fNormal[]) const
{
	return m_pBridge->GetNormal(idx, fNormal, m_pData);
}

bool VistaGeometry::SetNormals(const int startIndex, const vector<float>& normals)
{
	bool bRet = m_pBridge->SetNormals(startIndex, normals, m_pData);
	Notify( MSG_SET_NORMAL );
	return bRet;
}

bool VistaGeometry::SetNormals(const int startIndex, const vector<VistaVector3D>& normals)
{
	bool bRet = m_pBridge->SetNormals(startIndex, normals, m_pData);
	Notify( MSG_SET_NORMAL );
	return bRet;
}

bool VistaGeometry::GetNormals(std::vector<VistaVector3D> &normals) const
{
	return m_pBridge->GetNormals(normals, m_pData);
}

bool VistaGeometry::GetNormals(std::vector<float> &normals) const
{
	return m_pBridge->GetNormals(normals, m_pData);
}

bool VistaGeometry::SetNormal(const int idx, const float normal[3])
{
	bool bRet = m_pBridge->SetNormal(idx, normal, m_pData);
	Notify( MSG_SET_NORMAL );
	return bRet;
}

bool VistaGeometry::SetNormal(const int idx, const VistaVector3D& normal)
{
	bool bRet = m_pBridge->SetNormal(idx, normal, m_pData);
	Notify( MSG_SET_NORMAL );
	return bRet;
}

bool VistaGeometry::SetNormalIndex(const int idx, const int value)
{
	bool bRet =  m_pBridge->SetNormalIndex(idx, value, m_pData);
	Notify( MSG_SET_NORMAL );
	return bRet;
}

bool VistaGeometry::SetNormalIndices(const int startIdx, const vector<int>& indices)
{
	bool bRet =  m_pBridge->SetNormalIndices(startIdx, indices, m_pData);
	Notify( MSG_SET_NORMAL );
	return bRet;
}

// ============================================================================

int  VistaGeometry::GetColorIndex(const int idx) const
{
	return m_pBridge->GetColorIndex(idx, m_pData);
}

VistaColor VistaGeometry::GetColor(const int idx) const
{
	return m_pBridge->GetColor(idx, m_pData);
}

bool VistaGeometry::SetColors(const int startIndex, const vector<VistaColor>& colors)
{
	bool ret = m_pBridge->SetColors(startIndex, colors, m_pData);
	Notify( MSG_SET_COLOR );
	return ret;
}

bool VistaGeometry::SetColors(const int startIndex, const int bufferLength, float* colors)
{
	bool bRet = m_pBridge->SetColors(startIndex, bufferLength, colors, m_pData);
	Notify( MSG_SET_COLOR );
	return bRet;
}

bool VistaGeometry::SetColor(const VistaColor & color)
{
	bool bRet = m_pBridge->SetColor(color, m_pData);
	Notify( MSG_SET_COLOR );
	return bRet;
}

bool VistaGeometry::SetColor(const int idx, const VistaColor& color)
{
	bool bRet = m_pBridge->SetColor(idx, color, m_pData);
	Notify( MSG_SET_COLOR );
	return bRet;
}

bool VistaGeometry::SetColorIndex(const int idx, const int value)
{
	bool bRet = m_pBridge->SetColorIndex(idx, value, m_pData);
	Notify( MSG_SET_COLOR );
	return bRet;
}

bool VistaGeometry::SetColorIndices(const int startIdx, const vector<int>& indices)
{
	bool bRet = m_pBridge->SetColorIndices(startIdx, indices, m_pData);
	Notify( MSG_SET_COLOR );
	return bRet;
}

bool VistaGeometry::SetMaterial(const VistaMaterial & material)
{
	bool bRet = m_pBridge->SetMaterial(material, m_pData);
	Notify( MSG_SET_TEXTURE );
	return bRet;
}

bool VistaGeometry::BindToVistaMaterialTable(void)
{
	bool bRet = m_pBridge->BindToVistaMaterialTable(m_pData);
	Notify( MSG_SET_TEXTURE );
	return bRet;
}

bool VistaGeometry::SetMaterialIndex(const int& materialIndex)
{
	if( m_pBridge->SetMaterialIndex(materialIndex, m_pData) )
	{
		m_iMaterialIndex = materialIndex;
		Notify( MSG_SET_TEXTURE );
		return true;
	}
	return false;
}

int VistaGeometry::GetMaterialIndex() const
{
	return this->m_iMaterialIndex;
}

bool VistaGeometry::SetTexture(const string & textureFileName)
{
	if( m_pBridge->SetTexture(textureFileName, m_pData) )
	{
		m_strTextureFileName = textureFileName;
		Notify( MSG_SET_TEXTURE );
		return true;
	}
	return false;
}

bool VistaGeometry::SetTexture(const int width,
								const int height,
								const int bpp,
								bool bHasAlpha,
								VistaType::byte* pData)
{
	if(m_pBridge->SetTexture(int(long(pData)), width, height, bpp, pData, bHasAlpha, m_pData))
	{
		Notify( MSG_SET_TEXTURE );
		return true;
	}
	return false;
}

bool VistaGeometry::DeleteTexture()
{
	return m_pBridge->DeleteTexture(m_pData);
}

void VistaGeometry::GetTexture( string & strFileName ) const
{
	strFileName = m_strTextureFileName;
}

bool VistaGeometry::GetIsStatic() const
{
	return m_pBridge->GetIsStatic(m_pData);
}

void VistaGeometry::SetIsStatic(bool bStatic)
{
	m_pBridge->SetIsStatic(bStatic, m_pData);
}

bool VistaGeometry::CreateIndexedGeometry(
		const vector<VistaIndexedVertex>& vertices,
		const vector<VistaVector3D>& coords,
		const vector<VistaVector3D>& textureCoords,
		const vector<VistaVector3D>& normals,
		const vector<VistaColor>& colors,
		const VistaVertexFormat& vFormat,
		const VistaGeometry::FaceType fType)
{
	if( m_pBridge->CreateIndexedGeometry(vertices, coords, textureCoords, normals, colors, vFormat, fType, m_pData) )
	{
		Notify( MSG_PROBABLY_ALL_CHANGED );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::CreateIndexedGeometry
		(const vector<VistaIndexedVertex>& vertices,
		 const vector<float>& coords,
		 const vector<float>& textureCoords,
		 const vector<float>& normals,
		 const vector<VistaColor>& colors,
		 const VistaVertexFormat& vFormat,
		 const VistaGeometry::FaceType fType)
{
	if( m_pBridge->CreateIndexedGeometry(vertices, coords, textureCoords, normals, colors, vFormat, fType, m_pData) )
	{
		Notify( MSG_PROBABLY_ALL_CHANGED );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::ScaleGeometry(const float scale)
{
	if( m_pBridge->ScaleGeometry(scale, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::ScaleGeometry(const float scale[3])
{
	return m_pBridge->ScaleGeometry(scale, m_pData);
}

bool VistaGeometry::ScaleGeometry(const float sx, const float sy, const float sz)
{
	if( m_pBridge->ScaleGeometry(sx, sy, sz, m_pData) )
	{
		Notify( MSG_SET_COORD );
		return true;
	}
	else
		return false;
}

bool VistaGeometry::GetRenderingAttributes(VistaRenderingAttributes& attr) const
{
	return m_pBridge->GetRenderingAttributes(attr, m_pData);
}

bool VistaGeometry::SetRenderingAttributes(const VistaRenderingAttributes& attr)
{
	return m_pBridge->SetRenderingAttributes(attr, m_pData);
}

float VistaGeometry::GetTransparency() const
{
	return m_pBridge->GetTransparency(m_pData);
}

bool VistaGeometry::SetTransparency(const float& transparency)
{
	return m_pBridge->SetTransparency(transparency, m_pData);
}

bool VistaGeometry::GetFaces(vector<int>& faces) const
{
	return m_pBridge->GetFaces(faces, m_pData);
}

bool VistaGeometry::GetCoordinates(vector<float>& coords) const
{
	return m_pBridge->GetCoordinates(coords, m_pData);
}

bool VistaGeometry::GetCoordinates( std::vector<VistaVector3D>& coords ) const
{
	return m_pBridge->GetCoordinates(coords, m_pData);
}

// ============================================================================
// ============================================================================

bool VistaGeometry::GetBoundingBox(VistaVector3D& pMin, VistaVector3D& pMax) const
{
	return m_pBridge->GetBoundingBox(pMin, pMax, m_pData);
}

VistaBoundingBox VistaGeometry::GetBoundingBox() const
{
	VistaVector3D v3Min, v3Max;
	GetBoundingBox( v3Min, v3Max );
	return VistaBoundingBox( v3Min, v3Max );
}

unsigned int VistaGeometry::GetNumberOfVertices() const
{
	return m_pBridge->GetNumberOfVertices(m_pData);
}

unsigned int  VistaGeometry::GetNumberOfFaces() const
{
	return m_pBridge->GetNumberOfFaces(m_pData);
}

unsigned int  VistaGeometry::GetNumberOfColors() const
{
	return m_pBridge->GetNumberOfColors(m_pData);
}

unsigned int  VistaGeometry::GetNumberOfCoords() const
{
	return m_pBridge->GetNumberOfCoords(m_pData);
}

unsigned int  VistaGeometry::GetNumberOfNormals() const
{
	return m_pBridge->GetNumberOfNormals(m_pData);
}

unsigned int  VistaGeometry::GetNumberOfTextureCoords() const
{
	return m_pBridge->GetNumberOfTextureCoords(m_pData);
}

/*
bool VistaGeometry::IsIndexed() const
{
	return m_pBridge->IsIndexed(m_pData);
}
*/

VistaVertexFormat	VistaGeometry::GetVertexFormat() const
{
	return m_pBridge->GetVertexFormat(m_pData);
}

VistaGeometry::FaceType	VistaGeometry::GetFaceType() const
{
	return m_pBridge->GetFaceType(m_pData);
}
/*
bool VistaGeometry::GetVertices(vector<VistaVertex>& vertices) const
{
	return m_pBridge->GetVertices(vertices, m_pData);
}

bool VistaGeometry::GetVertices(vector<VistaIndexedVertex>& vertices) const
{
	return m_pBridge->GetVertices(vertices, m_pData);
}

bool VistaGeometry::GetCoordinates(vector<VistaVector3D>& coords) const
{
	return m_pBridge->GetCoordinates(coords, m_pData);
}

bool VistaGeometry::GetTextureCoords2D(vector<VistaVector3D>& textureCoords2D) const
{
	return m_pBridge->GetTextureCoords2D(textureCoords2D, m_pData);
}

bool VistaGeometry::GetNormals(vector<VistaVector3D>& normals) const
{
	return m_pBridge->GetNormals(normals, m_pData);
}

bool VistaGeometry::GetColors(vector<VistaColor>& colorsRGB) const
{
	return m_pBridge->GetColors(colorsRGB, m_pData);
}
*/

bool VistaGeometry::ComputeBarycentricCoords(const int face, const VistaVector3D& point, float& a, float& b) const
{
	VistaVector3D nodes[3];
	GetFaceCoords(face, nodes[0], nodes[1], nodes[2]);

	VistaVector3D x = nodes[1] - nodes[0];
	VistaVector3D y = nodes[2] - nodes[0];
	VistaVector3D z = point - nodes[0];

	float divisor, error;

	if(x[0] == 0.0f && y[0] == 0.0f)
	{
		divisor = (x[1]*y[2]-x[2]*y[1]);
		a = (y[2]*z[1]-y[1]*z[2])/divisor;
		b = (x[1]*z[2]-x[2]*z[1])/divisor;
		error = z[0];
	}
	else
		if (x[1] == 0.0f && y[1] == 0.0f)
		{
			divisor = (x[2]*y[0]-x[0]*y[2]);
			a = (y[0]*z[2]-y[2]*z[0])/divisor;
			b = (x[2]*z[0]-x[0]*z[2])/divisor;
			error = z[1];
		}
		else
		{
			divisor = (x[0]*y[1]-x[1]*y[0]);
			a = (y[1]*z[0]-y[0]*z[1])/divisor;
			b = (x[0]*z[1]-x[1]*z[0])/divisor;
			error = a*x[2]+b*y[2] - z[2];
		}

	if(std::abs(error) > EPSILON)
		return false;

	if(-EPSILON < a && a < 1.0f+EPSILON && -EPSILON < b && b < 1.0f+EPSILON && (a+b < 1.0f+EPSILON))
		return true;

	return false;
}

bool VistaGeometry::GetFaceCoords(const int idx, VistaVector3D& a,VistaVector3D& b,VistaVector3D& c) const
{
	return m_pBridge->GetFaceCoords(idx, a, b, c, m_pData);
}

bool VistaGeometry::GetFaceVertices(const int idx, int& vertexId0, int& vertexId1, int& vertexId2 ) const
{
	return m_pBridge->GetFaceVertices(idx, vertexId0, vertexId1, vertexId2, m_pData);
}

bool VistaGeometry::GetFaceCoords(const int idx, std::vector<int> &coords, int &nMod) const
{
	return m_pBridge->GetFaceCoords(idx, coords, nMod, m_pData);
}

bool VistaGeometry::GetFaceBBox(const int idx, VistaVector3D& min, VistaVector3D& max) const
{
	return m_pBridge->GetFaceBoundingBox(idx, min, max, m_pData);
}

//VistaWTKGraphicsBridge::InitHalfedges===========================================================
//-------------------------------------------------------------------------------------------------
// Date:   14/03/2005
// Description: Creates a list of face identifiers that are references that way, that every
//              vertex points to the same vertex of the face, that shares the edge from this vertex
//              to the next. Initialization of this is very expensive and this data structure has
//              to be updated if the mesh changes.
//=================================================================================================
void VistaGeometry::InitTopologyData()
{
	/** @todo */
	unsigned int i;
	vector< list<int> >     listVertices; // in what faces is this vertex
	vector< list<int> >		listSuccVertices; // successor of vertex
	vector< list<int> >		listPrecVertices; // predecessor of vertex

	unsigned int numVertices = GetNumberOfVertices();

	listVertices.resize(numVertices);
	listSuccVertices.resize(numVertices);
	listPrecVertices.resize(numVertices);

	m_adjacentFaces.reserve(2*numVertices);
	m_adjacentFaces.resize(numVertices);

	for(i=0; i<GetNumberOfFaces(); i++)
	{
		int p0, p1, p2;
		m_pBridge->GetFaceVertices(i, p0, p1, p2, m_pData);

		listVertices[p0].push_back(3 * i + 0);
		listPrecVertices[p0].push_back(p2);
		listSuccVertices[p0].push_back(p1);

		listVertices[p1].push_back(3 * i + 1);
		listPrecVertices[p1].push_back(p0);
		listSuccVertices[p1].push_back(p2);

		listVertices[p2].push_back(3 * i + 2);
		listPrecVertices[p2].push_back(p1);
		listSuccVertices[p2].push_back(p0);

		m_adjacentFaces[p0].insert(i);
		m_adjacentFaces[p1].insert(i);
		m_adjacentFaces[p2].insert(i);
	}

	m_halfedges.reserve(6*GetNumberOfFaces());
	m_halfedges.resize(3*GetNumberOfFaces(), -1);

	for(i = 0; i < numVertices; ++i)
	{
		list<int>::iterator pIter, sIter;
		list<int>::iterator iter1 = listVertices[i].begin();

		for(pIter = listPrecVertices[i].begin(); pIter != listPrecVertices[i].end(); ++pIter, ++iter1)
		{
			list<int>::iterator iter2 = listVertices[i].begin();

			for(sIter = listSuccVertices[i].begin(); sIter != listSuccVertices[i].end(); ++sIter, ++iter2)
			{
				if(*pIter == *sIter)
				{
					// the successor of vertex iter1 points to the predeccessor of the vertex iter2
					// of the neighbored face.
					m_halfedges[((*iter1)%3 > 0) ? ((*iter1)-1) : ((*iter1)+2)] =
																					  ((*iter2)%3 < 2) ? ((*iter2)+1) : ((*iter2)-2);

					// the vertex iter2 points to corresponding vertex of the neighbored face iter1.
					m_halfedges[*iter2] = *iter1;
				}
			}
		}
	}
}

//VistaWTKGraphicsBridge::GetNeighbor=============================================================
//-------------------------------------------------------------------------------------------------
// Date:   14/03/2005
// Description: If the halfedge datastructure is initialized topological information about the
//              geometry is available. As a consequence it's possible to receive the neighbors of
//              the face in one single step. Given a face and a vertex of this face (via it's
//              identifier) this function returns the face, that shares the edge from the given
//              vertex to the next one (as usual counterclockwise) with the given face.
//=================================================================================================
int VistaGeometry::GetNeighbor (const int faceId, const int edgeId) const
{
	if(m_halfedges.empty())
	{
		vstr::warnp() << "[VistaGeometry::GetNeighbor] Halfedge data structure not initialized." << std::endl;
		return -1;
	}

	std::vector<int>::size_type idx = 3*faceId + edgeId;

	if(edgeId>2 || idx>=m_halfedges.size())
	{
		vstr::warnp() << "[VistaGeometry::GetNeighbor]: Index out of range" << std::endl;
		return -1;
	}

	int ret = m_halfedges[idx];

	return  (ret == -1) ? ret : ret/3;
}

//VistaWTKGraphicsBridge::GetNeighbor=============================================================
//-------------------------------------------------------------------------------------------------
// Date:   15/03/2005
// Description: If the halfedge datastructure is initialized topological information about the
//              geometry is available. As a consequence it's possible to receive the neighbors of
//              the face in one single step. Given a face and a vertex of this face (via it's
//              identifier) this function returns the face, that shares the edge from the given
//              vertex to the next one (as usual counterclockwise) with the given face. After
//              calling this function you'll have the correct values in faceID and vertexID.
//=================================================================================================
bool VistaGeometry::GetNeighbor(const int faceId, const int edgeId,
										 int& retFaceId, int& retEdgeId) const
{
	if(m_halfedges.empty())
	{
		vstr::warnp() << "[VistaGeometry::GetNeighbor] Halfedge data structure not initialized." << std::endl;
		return false;
	}

	std::vector<int>::size_type idx = 3*faceId + edgeId;

	if(edgeId>2 || idx>=m_halfedges.size())
	{
		vstr::warnp() << "[VistaGeometry::GetNeighbor]: index out of range" << std::endl;
		return false;
	}

	int ret = m_halfedges[idx];

	if(ret == -1)
	{
		retFaceId = -1;
		retEdgeId = -1;
		return false;
	}

	retFaceId   = ret/3;
	retEdgeId = ret%3;

	return true;
}

//VistaWTKGraphicsBridge::GetNeighbors============================================================
//-------------------------------------------------------------------------------------------------

// Date:   14/03/2005
// Description: Returns the neighbors neighbor1, neighbor2 and neighbor3 of the face "face". This
//              is of course only possible if topological information about the mesh is available.
//              In other words you'll have to initialize the halfedge datastructure first.
//=================================================================================================
bool VistaGeometry::GetNeighbors(const int faceId, int neighbors[3]) const
{
	if (m_halfedges.empty())
	{
		vstr::warnp() << "[VistaGeometry::GetNeighbor()] Halfedge data structure not initialized." << std::endl;
		return false;
	}

	std::vector<int>::size_type idx = 3*faceId;

	if(idx+2>=m_halfedges.size())
	{
		vstr::warnp() << "[VistaGeometry::GetNeighbor]: index out of range" << std::endl;
		return false;
	}

	int ret = m_halfedges[idx++];
	neighbors[0] = (ret == -1) ? ret : ret/3;

	ret = m_halfedges[idx++];
	neighbors[1] = (ret == -1) ? ret : ret/3;

	ret = m_halfedges[idx++];
	neighbors[2] = (ret == -1) ? ret : ret/3;

	return true;
}

bool VistaGeometry::GetEdge(const int vertexId0, const int vertexId1, int &face, int &edgeID) const
{
	int edge = GetEdge(vertexId0, vertexId1);

	if(edge == -1)
		return false;

	face = (int) edge/3;
	edgeID = edge%3;

	return true;
}

int VistaGeometry::GetEdge(const int vertexId0, const int vertexId1) const
{
	// search all adjacent faces of v0
	set<int>::const_iterator	iter0 = m_adjacentFaces[vertexId0].begin();
	set<int>::const_iterator	iter1;

	for(; iter0 != m_adjacentFaces[vertexId0].end(); ++iter0 )
	{
		iter1 = m_adjacentFaces[vertexId1].find(*iter0);
		if(iter1 != m_adjacentFaces[vertexId1].end())
		{
			// a common face of v0 and v1 has been found
			int	p[3];
			m_pBridge->GetFaceVertices(*iter0, p[0], p[1], p[2], m_pData);

			for(int i=0; i<3; i++)
			{
				if(p[i] == vertexId0)
				{
					if(p[(i+1)%3] == vertexId1)
						return 3*(*iter0)+i;
					//	else
					//return m_halfedges[3*(*iter0)+(i-1)%3];
				}
			}
		}
	}

	return -1;
}

bool VistaGeometry::GetAdjacentFaces(const int vertexID, list<int>& listFaces)
{
	for(set<int>::const_iterator iter = m_adjacentFaces[vertexID].begin();
		iter != m_adjacentFaces[vertexID].end(); ++iter )
	{
		listFaces.push_back(*iter);
	}
	return true;
}

bool VistaGeometry::IsBoundary(const int faceId)
{
	int neighbors[3];
	GetNeighbors(faceId, neighbors);

	if(neighbors[0]==-1 || neighbors[1]==-1 || neighbors[2]==-1)
		return true;
	else
		return false;
}

bool VistaGeometry::AddFace(const int vertexId0, const int vertexId1, const int vertexId2)
{
	int tmp = -1;
	return AddFace(vertexId0, vertexId1, vertexId2, tmp);
}

bool VistaGeometry::AddFace(const int vertexId0, const int vertexId1, const int vertexId2, int& faceId)
{
	if(faceId <0 || faceId >= (int)GetNumberOfFaces())
	{
		faceId = GetNumberOfFaces();

		m_halfedges.push_back(-1);
		m_halfedges.push_back(-1);
		m_halfedges.push_back(-1);
	}
	else
	{
		int vertexId0, vertexId1, vertexId2;
		m_pBridge->GetFaceVertices(faceId, vertexId0, vertexId1, vertexId2, m_pData);

		int edge;

		edge = GetEdge(vertexId1, vertexId0); // 0->1
		if(edge != -1)
		{
			m_halfedges[edge] = -1;
			m_halfedges[3*faceId+1] = -1;
		}

		edge = GetEdge(vertexId2, vertexId1); // 1->2
		if(edge != -1)
		{
			m_halfedges[edge] = -1;
			m_halfedges[3*faceId+2] = -1;
		}

		edge = GetEdge(vertexId0, vertexId2); // 2->0
		if(edge != -1)
		{
			m_halfedges[edge] = -1;
			m_halfedges[3*faceId+0] = -1;
		}

		m_adjacentFaces[vertexId0].erase(faceId);
		m_adjacentFaces[vertexId1].erase(faceId);
		m_adjacentFaces[vertexId2].erase(faceId);
	}

	int ret = m_pBridge->AddFace(vertexId0, vertexId1, vertexId2, faceId, m_pData);

	int edge;
	int fId, vId;

	// update topology information
	// create new halfedges
	edge = GetEdge(vertexId1, vertexId0); // 0->1
	if(edge != -1)
	{
		m_halfedges[edge] = 3*faceId+1;
		fId = edge/3;
		vId = (edge+1)%3;
		m_halfedges[3*faceId+0] = 3*fId+vId;
	}

	edge = GetEdge(vertexId2, vertexId1); // 1->2
	if(edge != -1)
	{
		m_halfedges[edge] = 3*faceId+2;
		fId = edge/3;
		vId = (edge+1)%3;
		m_halfedges[3*faceId+1] = 3*fId+vId;
	}

	edge = GetEdge(vertexId0, vertexId2); // 2->0
	if(edge != -1)
	{
		m_halfedges[edge] = 3*faceId+0;
		fId = edge/3;
		vId = (edge+1)%3;
		m_halfedges[3*faceId+2] = 3*fId+vId;
	}

	m_adjacentFaces[vertexId0].insert(faceId);
	m_adjacentFaces[vertexId1].insert(faceId);
	m_adjacentFaces[vertexId2].insert(faceId);

	m_nLastFaceChanged = faceId;
	Notify(VistaGeometry::MSG_ADD_FACE);
	return ret ? true:false;
}

bool VistaGeometry::DeleteFace(const int faceId, bool deleteVertices)
{
	// update topology information
	int vertexId0, vertexId1, vertexId2;
	m_pBridge->GetFaceVertices(faceId, vertexId0, vertexId1, vertexId2, m_pData);

	int edge;

	edge = GetEdge(vertexId1, vertexId0); // 0->1
	if(edge != -1)
	{
		m_halfedges[edge] = -1;
		m_halfedges[3*faceId+1] = -1;
	}

	edge = GetEdge(vertexId2, vertexId1); // 1->2
	if(edge != -1)
	{
		m_halfedges[edge] = -1;
		m_halfedges[3*faceId+2] = -1;
	}

	edge = GetEdge(vertexId0, vertexId2); // 2->0
	if(edge != -1)
	{
		m_halfedges[edge] = -1;
		m_halfedges[3*faceId+0] = -1;
	}

	m_adjacentFaces[vertexId0].erase(faceId);
	m_adjacentFaces[vertexId1].erase(faceId);
	m_adjacentFaces[vertexId2].erase(faceId);

	int ret = m_pBridge->DeleteFace(faceId, deleteVertices, m_pData);

	m_nLastFaceChanged = faceId;
	Notify(VistaGeometry::MSG_DELETE_FACE);
	return ret?true:false;
}

int VistaGeometry::AddVertex(const VistaVector3D& pos)
{
	int ret = m_pBridge->AddVertex(pos, m_pData);

	m_adjacentFaces.resize(m_adjacentFaces.size()+1);

	m_nLastVertexChanged = ret;
	m_nLastFaceChanged = -1;
	m_nLastPositionChanged = pos;
	Notify(VistaGeometry::MSG_ADD_VERTEX);
	return ret;
}

int VistaGeometry::AddVertex(const int face, const float a, const float b)
{
	VistaVector3D nodes[3];
	GetFaceCoords(face, nodes[0], nodes[1], nodes[2]);
	VistaVector3D pos = nodes[0] + a * (nodes[1]-nodes[0]) + b*(nodes[2]-nodes[0]);

	VistaVector3D n1, n2, n3;
	n1 = GetNormal(GetNormalIndex(3*face));
	n2 = GetNormal(GetNormalIndex(3*face+1));
	n3 = GetNormal(GetNormalIndex(3*face+2));
	VistaVector3D normal = n1 + a * (n2-n1) + b*(n3-n1);
	normal.Normalize();

	int ret = AddVertex(pos);
	SetNormal(ret, normal);
	m_nLastVertexChanged = ret;
	m_nLastFaceChanged = face;
	m_nLastPositionChanged = VistaVector3D(a, b, 0.0f);
	Notify(VistaGeometry::MSG_ADD_VERTEX);
	return ret;
}

bool VistaGeometry::DeleteVertex(const int vertexId)
{
	// delete all adjacent faces
	int ret = true;

	set<int>::iterator	iter = m_adjacentFaces[vertexId].begin();

	while(iter != m_adjacentFaces[vertexId].end())
	{
		ret = ret && m_pBridge->DeleteFace(*iter, true, m_pData);
		iter = m_adjacentFaces[vertexId].begin();
	}

	ret = ret && m_pBridge->DeleteVertex(vertexId, m_pData);

	m_nLastVertexChanged = vertexId;
	Notify(VistaGeometry::MSG_DELETE_VERTEX);
	return ret?true:false;
}

bool VistaGeometry::IsConsistent() const
{
	return true;
}

bool VistaGeometry::GetTextureCoords2D( std::vector< VistaVector3D >& vecCoordinates ) const
{
	return m_pBridge->GetTextureCoords2D( vecCoordinates, m_pData ); 
}

bool VistaGeometry::GetTextureCoords2D( std::vector< float >& vecCoordinates ) const
{
	return m_pBridge->GetTextureCoords2D( vecCoordinates, m_pData ); 
}

bool VistaGeometry::GetColors( std::vector< VistaColor >& vecColors ) const
{
	return m_pBridge->GetColors( vecColors, m_pData ); 
}

bool VistaGeometry::GetColors( std::vector< float >& vecColors ) const
{
	return m_pBridge->GetColors( vecColors, m_pData ); 
}

VistaGeometry* VistaGeometry::Clone() const
{
	VistaGeometry* pNewGeometry = new VistaGeometry( m_pBridge, m_pBridge->CloneGeometryData( GetData() ) );
	pNewGeometry->m_strTextureFileName = m_strTextureFileName;
	pNewGeometry->m_iMaterialIndex = m_iMaterialIndex;
	pNewGeometry->m_adjacentFaces = m_adjacentFaces;
	pNewGeometry->m_adjacentFaces = m_adjacentFaces;
	return pNewGeometry;
}

// ============================================================================
// ============================================================================

VistaMaterial::VistaMaterial()
	// set to OpenGL default values
	: m_oAmbient  (0.2f, 0.2f, 0.2f)
	, m_oDiffuse  (0.8f, 0.8f, 0.8f)
	, m_oSpecular (0.0f, 0.0f, 0.0f)
	, m_oEmission (0.0f, 0.0f, 0.0f)
	, m_nOpacity  (1.0f)
	, m_nShininess(0.0f)
	, m_strName  ("OpenGL defaults")
	, m_iIndex   (-1)
{
}

VistaMaterial::VistaMaterial(const VistaColor& ambient, const VistaColor& diffuse,
							   const VistaColor& specular, const VistaColor& emission,
							   const float shininess, const float opacity, const std::string & name)
 : m_oAmbient( ambient )
, m_oDiffuse( diffuse )
, m_oSpecular( specular )
, m_oEmission( emission )
, m_nShininess( shininess )
, m_nOpacity( opacity )
, m_strName( name )
, m_iIndex( -1 )
{
}

VistaMaterial::VistaMaterial(const float ambient[3], const float diffuse[3],
							   const float specular[3], const float emission[3],
							   const float  shininess, const float opacity, const std::string & name)
: m_oAmbient( ambient )
, m_oDiffuse( diffuse )
, m_oSpecular( specular )
, m_oEmission( emission )
, m_nShininess( shininess )
, m_nOpacity( opacity )
, m_strName( name )
, m_iIndex( -1 )
{
}

void VistaMaterial::GetAmbientColor(float a3fColor[3]) const
{
	m_oAmbient.GetValues(a3fColor);
}

VistaColor VistaMaterial::GetAmbientColor() const
{
	return m_oAmbient;
}

void VistaMaterial::GetEmissionColor(float a3fColor[3]) const
{
	m_oEmission.GetValues(a3fColor);
}

VistaColor VistaMaterial::GetEmissionColor() const
{
	return m_oEmission;
}

void VistaMaterial::GetDiffuseColor(float a3fColor[3]) const
{
	m_oDiffuse.GetValues(a3fColor);
}

VistaColor VistaMaterial::GetDiffuseColor() const
{
	return m_oDiffuse;
}

void VistaMaterial::GetSpecularColor(float a3fColor[3]) const
{
	m_oSpecular.GetValues(a3fColor);
}

VistaColor VistaMaterial::GetSpecularColor() const
{
	return m_oSpecular;
}

float VistaMaterial::GetShininess() const
{
	return m_nShininess;
}

float VistaMaterial::GetOpacity() const
{
	return m_nOpacity;
}

void VistaMaterial::SetAmbientColor( const float amb[3])
{
	m_oAmbient = VistaColor(amb);
}

void VistaMaterial::SetAmbientColor( const VistaColor& oColor )
{
	m_oAmbient = oColor;
}

void VistaMaterial::SetDiffuseColor(const float dif[3])
{
	m_oDiffuse = VistaColor(dif);
}

void VistaMaterial::SetDiffuseColor( const VistaColor& oColor )
{
	m_oDiffuse = oColor;
}

void VistaMaterial::SetSpecularColor(const float spec[3])
{
	m_oSpecular = VistaColor(spec);
}

void VistaMaterial::SetSpecularColor( const VistaColor& oColor )
{
	m_oSpecular = oColor;
}

void VistaMaterial::SetEmissionColor(const float emm[3])
{
	m_oEmission = VistaColor(emm);
}

void VistaMaterial::SetEmissionColor( const VistaColor& oColor )
{
	m_oEmission = oColor;
}

void VistaMaterial::SetShininess(float shiny)
{
	m_nShininess = shiny;
}

void VistaMaterial::SetOpacity(float opacity)
{
	m_nOpacity = opacity;
}

std::string VistaMaterial::GetName() const
{
	return m_strName;
}

int  VistaMaterial::GetMaterialIndex() const
{
	return m_iIndex;
}

bool VistaMaterial::SetMaterialIndex(int iIndex)
{
	m_iIndex = iIndex;
	return true;
}

void VistaMaterial::Emerald(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0215f, 0.1745f, 0.0215f);
	mat.m_oDiffuse = VistaColor(0.07568f, 0.61424f, 0.07568f);
	mat.m_oSpecular = VistaColor(0.633f, 0.727811f, 0.633f);
	mat.m_nShininess = 60.0f;
}

void VistaMaterial::Jade(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.135f, 0.2225f, 0.1575f);
	mat.m_oDiffuse = VistaColor(0.54f, 0.89f, 0.63f);
	mat.m_oSpecular = VistaColor(0.316228f, 0.316228f, 0.316228f);
	mat.m_nShininess = 10.0f;
}

void VistaMaterial::Obsidian(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.05375f, 0.05f, 0.06625f);
	mat.m_oDiffuse = VistaColor(0.18275f, 0.17f, 0.22525f);
	mat.m_oSpecular = VistaColor(0.332741f, 0.328634f, 0.346435f);
	mat.m_nShininess = 30.0f;
}

void VistaMaterial::Pearl(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.25f, 0.20725f, 0.20725f);
	mat.m_oDiffuse = VistaColor(1.0f, 0.829f, 0.829f);
	mat.m_oSpecular = VistaColor(0.296648f, 0.296648f, 0.296648f);
	mat.m_nShininess = 8.8f;
}

void VistaMaterial::Ruby(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.1745f, 0.01175f, 0.01175f);
	mat.m_oDiffuse = VistaColor(0.61424f, 0.04136f, 0.04136f);
	mat.m_oSpecular = VistaColor(0.727811f, 0.626959f, 0.626959f);
	mat.m_nShininess = 60.0f;
}

void VistaMaterial::Turquoise(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.1f, 0.18725f, 0.1745f);
	mat.m_oDiffuse = VistaColor(0.396f, 0.74151f, 0.69102f);
	mat.m_oSpecular = VistaColor(0.297254f, 0.30829f, 0.306678f);
	mat.m_nShininess = 10.0f;
}

void VistaMaterial::Brass(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.329412f, 0.223529f, 0.027451f);
	mat.m_oDiffuse = VistaColor(0.780392f, 0.568627f, 0.113725f);
	mat.m_oSpecular = VistaColor(0.992157f, 0.941176f, 0.807843f);
	mat.m_nShininess = 21.794872f;
}

void VistaMaterial::Bronze(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.2125f, 0.1275f, 0.054f);
	mat.m_oDiffuse = VistaColor(0.714f, 0.4284f, 0.18144f);
	mat.m_oSpecular = VistaColor(0.393548f, 0.271906f, 0.166721f);
	mat.m_nShininess = 20.0f;
}

void VistaMaterial::Chrome(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.25f, 0.25f, 0.25f);
	mat.m_oDiffuse = VistaColor(0.4f, 0.4f, 0.4f);
	mat.m_oSpecular = VistaColor(0.774597f, 0.774597f, 0.774597f);
	mat.m_nShininess = 60.0f;
}

void VistaMaterial::Copper(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.19125f, 0.0735f, 0.0225f);
	mat.m_oDiffuse = VistaColor(0.7038f, 0.27048f, 0.0828f);
	mat.m_oSpecular = VistaColor(0.256777f, 0.137622f, 0.086014f);
	mat.m_nShininess = 10.0f;
}

void VistaMaterial::Gold(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.24725f, 0.1995f, 0.0745f);
	mat.m_oDiffuse = VistaColor(0.75164f, 0.60648f, 0.22648f);
	mat.m_oSpecular = VistaColor(0.628281f, 0.555802f, 0.366065f);
	mat.m_nShininess = 40.0f;
}

void VistaMaterial::Silver(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.19225f, 0.19225f, 0.19225f);
	mat.m_oDiffuse = VistaColor(0.50754f, 0.50754f, 0.50754f);
	mat.m_oSpecular = VistaColor(0.508273f, 0.508273f, 0.508273f);
	mat.m_nShininess = 40.0f;
}

void VistaMaterial::BlackPlastic(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.0f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.01f, 0.01f, 0.01f);
	mat.m_oSpecular = VistaColor(0.50f, 0.50f, 0.50f);
	mat.m_nShininess = 25.0f;
}

void VistaMaterial::CyanPlastic(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.1f, 0.06f);
	mat.m_oDiffuse = VistaColor(0.0f, 0.50980392f, 0.50980392f);
	mat.m_oSpecular = VistaColor(0.50196078f, 0.50196078f, 0.50196078f);
	mat.m_nShininess = 25.0f;
}

void VistaMaterial::GreenPlastic(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.0f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.1f, 0.35f, 0.1f);
	mat.m_oSpecular = VistaColor(0.45f, 0.55f, 0.45f);
	mat.m_nShininess = 25.0f;
}

void VistaMaterial::RedPlastic(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.0f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.5f, 0.0f, 0.0f);
	mat.m_oSpecular = VistaColor(0.7f, 0.6f, 0.6f);
	mat.m_nShininess = 25.0f;
}

void VistaMaterial::WhitePlastic(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.0f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.55f, 0.55f, 0.55f);
	mat.m_oSpecular = VistaColor(0.70f, 0.70f, 0.70f);
	mat.m_nShininess = 25.0f;
}

void VistaMaterial::YellowPlastic(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.0f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.5f, 0.5f, 0.0f);
	mat.m_oSpecular = VistaColor(0.60f, 0.60f, 0.50f);
	mat.m_nShininess = 25.0f;
}

void VistaMaterial::BlackRubber(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.02f, 0.02f, 0.02f);
	mat.m_oDiffuse = VistaColor(0.01f, 0.01f, 0.01f);
	mat.m_oSpecular = VistaColor(0.4f, 0.4f, 0.4f);
	mat.m_nShininess = 25.0f;
}

void VistaMaterial::CyanRubber(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.05f, 0.05f);
	mat.m_oDiffuse = VistaColor(0.4f, 0.5f, 0.5f);
	mat.m_oSpecular = VistaColor(0.04f, 0.7f, 0.7f);
	mat.m_nShininess = 7.8125f;
}

void VistaMaterial::GreenRubber(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.0f, 0.05f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.4f, 0.5f, 0.4f);
	mat.m_oSpecular = VistaColor(0.04f, 0.7f, 0.04f);
	mat.m_nShininess = 7.8125f;
}

void VistaMaterial::RedRubber(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.05f, 0.0f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.5f, 0.4f, 0.4f);
	mat.m_oSpecular = VistaColor(0.7f, 0.04f, 0.04f);
	mat.m_nShininess = 7.8125f;
}

void VistaMaterial::WhiteRubber(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.05f, 0.05f, 0.05f);
	mat.m_oDiffuse = VistaColor(0.5f, 0.5f, 0.5f);
	mat.m_oSpecular = VistaColor(0.7f, 0.7f, 0.7f);
	mat.m_nShininess = 7.8125f;
}

void VistaMaterial::YellowRubber(VistaMaterial& mat)
{
	mat.m_oAmbient = VistaColor(0.05f, 0.05f, 0.0f);
	mat.m_oDiffuse = VistaColor(0.5f, 0.5f, 0.4f);
	mat.m_oSpecular = VistaColor(0.7f, 0.7f, 0.04f);
	mat.m_nShininess = 7.8125f;
}