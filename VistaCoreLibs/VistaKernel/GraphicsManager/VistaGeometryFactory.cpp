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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaGeometryFactory.h"

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cassert>
#include <vector>
using namespace std;

/*============================================================================*/
/*  HELPERS                                                                   */
/*============================================================================*/

namespace {
bool ReadColorFromProplist(
    const VistaPropertyList& oPropList, const std::string& sPropName, VistaColor& oColorTarget) {
  return oPropList.GetValue<VistaColor>(sPropName, oColorTarget);
}

void PushQuad(std::vector<VistaIndexedVertex>::iterator& itFaceList, int nVertex0, int nVertex1,
    int nVertex2, int nVertex3, int nTexCoord0, int nTexCoord1, int nTexCoord2, int nTexCoord3,
    int nNormalIndex, bool bUseQuads) {
  if (bUseQuads) {
    (*itFaceList).SetCoordinateIndex(nVertex0);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord0);
    ++itFaceList;
    (*itFaceList).SetCoordinateIndex(nVertex1);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord1);
    ++itFaceList;
    (*itFaceList).SetCoordinateIndex(nVertex2);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord2);
    ++itFaceList;
    (*itFaceList).SetCoordinateIndex(nVertex3);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord3);
    ++itFaceList;
  } else {
    (*itFaceList).SetCoordinateIndex(nVertex0);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord0);
    ++itFaceList;
    (*itFaceList).SetCoordinateIndex(nVertex1);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord1);
    ++itFaceList;
    (*itFaceList).SetCoordinateIndex(nVertex2);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord2);
    ++itFaceList;

    (*itFaceList).SetCoordinateIndex(nVertex0);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord0);
    ++itFaceList;
    (*itFaceList).SetCoordinateIndex(nVertex2);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord2);
    ++itFaceList;
    (*itFaceList).SetCoordinateIndex(nVertex3);
    (*itFaceList).SetNormalIndex(nNormalIndex);
    (*itFaceList).SetTextureCoordinateIndex(nTexCoord3);
    ++itFaceList;
  }
}

void PushSplitQuad(std::vector<VistaIndexedVertex>::iterator& itFaceList, int nVertex0,
    int nVertex1, int nVertex2, int nVertex3, int nVertexCenter, int nTexCoord0, int nTexCoord1,
    int nTexCoord2, int nTexCoord3, int nTexCoordCenter, int nNormalIndex) {
  (*itFaceList).SetCoordinateIndex(nVertex0);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord0);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertex1);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord1);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertexCenter);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoordCenter);
  ++itFaceList;

  (*itFaceList).SetCoordinateIndex(nVertex1);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord1);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertex2);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord2);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertexCenter);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoordCenter);
  ++itFaceList;

  (*itFaceList).SetCoordinateIndex(nVertex2);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord2);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertex3);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord3);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertexCenter);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoordCenter);
  ++itFaceList;

  (*itFaceList).SetCoordinateIndex(nVertex3);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord3);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertex0);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoord0);
  ++itFaceList;
  (*itFaceList).SetCoordinateIndex(nVertexCenter);
  (*itFaceList).SetNormalIndex(nNormalIndex);
  (*itFaceList).SetTextureCoordinateIndex(nTexCoordCenter);
  ++itFaceList;
}

void PushQuadWithEqualTexCoords(std::vector<VistaIndexedVertex>::iterator& itFaceList, int nVertex0,
    int nVertex1, int nVertex2, int nVertex3, int nNormalIndex, bool bUseQuads) {
  PushQuad(itFaceList, nVertex0, nVertex1, nVertex2, nVertex3, nVertex0, nVertex1, nVertex2,
      nVertex3, nNormalIndex, bUseQuads);
}
void PushSplitQuadWithEqualTexCoords(std::vector<VistaIndexedVertex>::iterator& itFaceList,
    int nVertex0, int nVertex1, int nVertex2, int nVertex3, int nVertexCenter, int nNormalIndex) {
  PushSplitQuad(itFaceList, nVertex0, nVertex1, nVertex2, nVertex3, nVertexCenter, nVertex0,
      nVertex1, nVertex2, nVertex3, nVertexCenter, nNormalIndex);
}

void CreateAverageVertex(int nVertex0, int nVertex1, int nVertex2, int nVertex3, int nVertexCenter,
    int nTexCoord0, int nTexCoord1, int nTexCoord2, int nTexCoord3, int nTexCoordCenter,
    std::vector<float>& vecCoords, std::vector<float>& vecTexCoords) {
  for (int i = 0; i < 3; ++i) {
    vecCoords[3 * nVertexCenter + i] =
        (vecCoords[3 * nVertex0 + i] + vecCoords[3 * nVertex1 + i] + vecCoords[3 * nVertex2 + i] +
            vecCoords[3 * nVertex3 + i]) /
        4;
  }
  for (int i = 0; i < 2; ++i) {
    vecTexCoords[2 * nTexCoordCenter + i] =
        (vecTexCoords[2 * nTexCoord0 + i] + vecTexCoords[2 * nTexCoord1 + i] +
            vecTexCoords[2 * nTexCoord2 + i] + vecTexCoords[2 * nTexCoord3 + i]) /
        4;
  }
}
void TypedQuadCreate(std::vector<VistaIndexedVertex>::iterator& itFaceList, int nVertex0,
    int nVertex1, int nVertex2, int nVertex3, int nTexCoord0, int nTexCoord1, int nTexCoord2,
    int nTexCoord3, int nNormalIndex, VistaGeometryFactory::FaceType eType,
    std::vector<float>& vecCoords, std::vector<float>& vecTexCoords, int& nNextCoordId,
    int& nNextTexCoordId) {
  switch (eType) {
  case VistaGeometryFactory::FT_TRIANGLE: {
    PushQuad(itFaceList, nVertex0, nVertex1, nVertex2, nVertex3, nTexCoord0, nTexCoord1, nTexCoord2,
        nTexCoord3, nNormalIndex, false);
    break;
  }
  case VistaGeometryFactory::FT_QUAD: {
    PushQuad(itFaceList, nVertex0, nVertex1, nVertex2, nVertex3, nTexCoord0, nTexCoord1, nTexCoord2,
        nTexCoord3, nNormalIndex, true);
    break;
  }
  case VistaGeometryFactory::FT_QUAD_SPLIT_SYMMETRICALLY: {
    CreateAverageVertex(nVertex0, nVertex1, nVertex2, nVertex3, nNextCoordId, nTexCoord0,
        nTexCoord1, nTexCoord2, nTexCoord3, nNextTexCoordId, vecCoords, vecTexCoords);
    PushSplitQuad(itFaceList, nVertex0, nVertex1, nVertex2, nVertex3, nNextCoordId, nTexCoord0,
        nTexCoord1, nTexCoord2, nTexCoord3, nNextTexCoordId, nNormalIndex);
    ++nNextCoordId;
    ++nNextTexCoordId;
  }
  }
}

// helper function for box geometry creation
void AddVertexLine(const int nCount, VistaVector3D& v3Position, float nDelta, Vista::AXIS nAxis,
    std::vector<float>::iterator& itCoords) {
  for (int i = 0; i < nCount; ++i) {
    (*itCoords)   = v3Position[Vista::X];
    (*++itCoords) = v3Position[Vista::Y];
    (*++itCoords) = v3Position[Vista::Z];
    ++itCoords;
    v3Position[nAxis] += nDelta;
  }
}
int GetCapBorderIndex(const int nIndex, const int nNumYRings, const int nNumZRings) {
  int nNumYSegments = nNumYRings - 1;
  int nNumZSegments = nNumZRings - 1;
  if (nIndex < nNumZSegments)
    return nIndex;
  else if (nIndex < nNumZSegments + nNumYSegments) {
    int nRelIndex = nIndex - nNumZSegments + 1;
    return (nRelIndex * nNumZRings) - 1;
  } else if (nIndex < 2 * nNumZSegments + nNumYSegments) {
    int nRelIndex = nIndex - (nNumZSegments + nNumYSegments);
    int nMaxIndex = nNumZRings * nNumYRings - 1;
    return (nMaxIndex - nRelIndex);
  } else {
    int nRelIndex = nIndex - (2 * nNumZSegments + nNumYSegments);
    int nTopIndex = (nNumYRings - 1) * nNumZRings;
    return (nTopIndex - nRelIndex * nNumZRings);
  }
}
} // namespace

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaGeometryFactory::VistaGeometryFactory(VistaSceneGraph* pSG)
    : m_pSG(pSG) {
}

VistaGeometryFactory::~VistaGeometryFactory() {
}
/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

VistaSceneGraph* VistaGeometryFactory::GetSG() const {
  return m_pSG;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreatePlane                                                 */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreatePlane(float nSizeX, float nSizeZ, int nResolutionX,
    int nResolutionZ, Vista::AXIS eNormalDirection, VistaColor oColor, FaceType eFaceType,
    float nMinTextureCoordX, float nMaxTextureCoordX, float nMinTextureCoordZ,
    float nMaxTextureCoordZ) {
  VistaVertexFormat oVertexFormat;
  oVertexFormat.coordinate   = VistaVertexFormat::COORDINATE;
  oVertexFormat.color        = VistaVertexFormat::COLOR_NONE;
  oVertexFormat.normal       = VistaVertexFormat::NORMAL;
  oVertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

  std::vector<VistaIndexedVertex> index;
  std::vector<float>              coords;
  std::vector<float>              textureCoords;
  std::vector<float>              normals;
  std::vector<VistaColor>         colors;
  VistaGeometry::FaceType         nFaceType;

  CreatePlaneData(&index, &coords, &textureCoords, &normals, &colors, &nFaceType, nSizeX, nSizeZ,
      nResolutionX, nResolutionZ, eNormalDirection, oColor, eFaceType, nMinTextureCoordX,
      nMaxTextureCoordX, nMinTextureCoordZ, nMaxTextureCoordZ);

  VistaGeometry* pGeom = GetSG()->NewIndexedGeometry(
      index, coords, textureCoords, normals, colors, oVertexFormat, nFaceType);
  pGeom->SetColor(oColor);

  return pGeom;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreatePlaneData                                             */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreatePlaneData(vector<VistaIndexedVertex>* pIndex,
    vector<float>* pCoords, vector<float>* pTexCoords, vector<float>* pNormals,
    vector<VistaColor>* pColors, VistaGeometry::FaceType* pFaceType, float nSizeX, float nSizeZ,
    int nResX, int nResZ, Vista::AXIS eNormalDirection, VistaColor oColor, FaceType eFaceType,
    float nMinTextureCoordX, float nMaxTextureCoordX, float nMinTextureCoordZ,
    float nMaxTextureCoordZ) {
  assert(nSizeX > 0 && nSizeZ > 0);
  assert(nResX > 0 && nResZ > 0);

  // we just need one normal
  pNormals->resize(3, 0.0f);
  (*pNormals)[eNormalDirection] = 1.0f;

  Vista::AXIS eIndexX        = Vista::X;
  Vista::AXIS eIndexZ        = Vista::Z;
  float       nTexCoordXFlip = 1.0f;
  float       nTexCoordZFlip = 1.0f;
  switch (eNormalDirection) {
  case Vista::X:
    eIndexX        = Vista::Z;
    eIndexZ        = Vista::Y;
    nTexCoordXFlip = -1.0f;
    nTexCoordZFlip = 1.0f;
    break;
  case Vista::Z:
    eIndexX        = Vista::X;
    eIndexZ        = Vista::Y;
    nTexCoordXFlip = 1.0f;
    nTexCoordZFlip = 1.0f;
    break;
  case Vista::Y:
    nTexCoordXFlip = 1.0f;
    nTexCoordZFlip = -1.0f;
  default:
    break;
  };

  int nNumVertices = (nResX + 1) * (nResZ + 1);
  int nNumFaces    = nResX * nResZ; // faces - seen as quads
  if (eFaceType == FT_QUAD_SPLIT_SYMMETRICALLY)
    nNumVertices += nNumFaces;
  pCoords->resize(3 * nNumVertices);
  std::vector<float>::iterator itCoordData = pCoords->begin();

  pTexCoords->resize(2 * nNumVertices);
  std::vector<float>::iterator itTexData = pTexCoords->begin();

  VistaVector3D v3CoordPosition;
  v3CoordPosition[eIndexX] = -0.5f * nSizeX;
  v3CoordPosition[eIndexZ] = -0.5f * nSizeZ;
  float nDeltaXPos         = nSizeX / (float)nResX;
  float nDeltaZPos         = nSizeZ / (float)nResZ;

  float a2fTecCoord[2] = {nTexCoordXFlip * nMinTextureCoordX, nTexCoordZFlip * nMinTextureCoordZ};
  float nDeltaXTex     = nTexCoordXFlip * (nMaxTextureCoordX - nMinTextureCoordX) / (float)nResX;
  float nDeltaZTex     = nTexCoordZFlip * (nMaxTextureCoordZ - nMinTextureCoordZ) / (float)nResZ;

  for (int nX = 0; nX < nResX + 1; ++nX) {
    v3CoordPosition[eIndexZ] = -0.5f * nSizeZ;
    a2fTecCoord[1]           = nTexCoordZFlip * nMinTextureCoordZ;
    for (int nZ = 0; nZ < nResZ + 1; ++nZ) {
      v3CoordPosition.GetValues(&(*itCoordData));
      v3CoordPosition[eIndexZ] += nDeltaZPos;
      itCoordData += 3;

      (*itTexData) = a2fTecCoord[0];
      ++itTexData;
      (*itTexData) = a2fTecCoord[1];
      ++itTexData;
      a2fTecCoord[1] += nDeltaZTex;
    }
    v3CoordPosition[eIndexX] += nDeltaXPos;
    a2fTecCoord[0] += nDeltaXTex;
  }
  // if we split, we also have to add vertices for all intermediate points (added at the end )
  if (eFaceType == FT_QUAD_SPLIT_SYMMETRICALLY) {
    v3CoordPosition[eIndexX] = -0.5f * nSizeX + 0.5f * nDeltaXPos;
    a2fTecCoord[0]           = nTexCoordXFlip * nMinTextureCoordX + 0.5f * nDeltaXTex;
    for (int nX = 0; nX < nResX; ++nX) {
      v3CoordPosition[eIndexZ] = -0.5f * nSizeZ + 0.5f * nDeltaZPos;
      a2fTecCoord[1]           = nTexCoordZFlip * nMinTextureCoordZ + 0.5f * nDeltaZTex;
      for (int nZ = 0; nZ < nResZ; ++nZ) {
        v3CoordPosition.GetValues(&(*itCoordData));
        v3CoordPosition[eIndexZ] += nDeltaZPos;
        itCoordData += 3;

        (*itTexData) = a2fTecCoord[0];
        ++itTexData;
        (*itTexData) = a2fTecCoord[1];
        ++itTexData;
        a2fTecCoord[1] += nDeltaZTex;
      }
      v3CoordPosition[eIndexX] += nDeltaXPos;
      a2fTecCoord[0] += nDeltaXTex;
    }
  }

  switch (eFaceType) {
  case FT_TRIANGLE: {
    (*pFaceType) = VistaGeometry::VISTA_FACE_TYPE_TRIANGLES;
    pIndex->resize(2 * 3 * nNumFaces);
    int                                       nFirstColStart = 0;
    int                                       nNextColStart  = nResZ + 1;
    std::vector<VistaIndexedVertex>::iterator itVertexData   = pIndex->begin();
    for (int nX = 0; nX < nResX; ++nX) {
      for (int nZ = 0; nZ < nResZ; ++nZ) {
        PushQuadWithEqualTexCoords(itVertexData, nFirstColStart + nZ + 0, nNextColStart + nZ + 0,
            nNextColStart + nZ + 1, nFirstColStart + nZ + 1, 0, false);
      }
      nFirstColStart += nResZ + 1;
      nNextColStart += nResZ + 1;
    }
    break;
  }
  case FT_QUAD: {
    *pFaceType = VistaGeometry::VISTA_FACE_TYPE_QUADS;
    pIndex->resize(4 * nNumFaces);
    int                                       nFirstColStart = 0;
    int                                       nNextColStart  = nResZ + 1;
    std::vector<VistaIndexedVertex>::iterator itVertexData   = pIndex->begin();
    for (int nX = 0; nX < nResX; ++nX) {
      for (int nZ = 0; nZ < nResZ; ++nZ) {
        PushQuadWithEqualTexCoords(itVertexData, nFirstColStart + nZ + 0, nNextColStart + nZ + 0,
            nNextColStart + nZ + 1, nFirstColStart + nZ + 1, 0, true);
      }
      nFirstColStart += nResZ + 1;
      nNextColStart += nResZ + 1;
    }
    break;
  }
  case FT_QUAD_SPLIT_SYMMETRICALLY: {
    *pFaceType = VistaGeometry::VISTA_FACE_TYPE_TRIANGLES;
    pIndex->resize(4 * 3 * nNumFaces);
    int                                       nFirstColStart = 0;
    int                                       nNextColStart  = nResZ + 1;
    int                                       nCenterVertex  = nNumVertices - nNumFaces;
    std::vector<VistaIndexedVertex>::iterator itVertexData   = pIndex->begin();
    for (int nX = 0; nX < nResX; ++nX) {
      for (int nZ = 0; nZ < nResZ; ++nZ) {
        PushSplitQuadWithEqualTexCoords(itVertexData, nFirstColStart + nZ + 0,
            nNextColStart + nZ + 0, nNextColStart + nZ + 1, nFirstColStart + nZ + 1, nCenterVertex,
            0);
        ++nCenterVertex;
      }
      nFirstColStart += nResZ + 1;
      nNextColStart += nResZ + 1;
    }
    break;
  }
  default:
    VISTA_THROW("VistaGeometryFactory encountered invalid FaceType enum value", -1);
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateBox                                                   */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateBox(float nSizeX, float nSizeY, float nSizeZ,
    int nResolutionX, int nResolutionY, int nResolutionZ, VistaColor oColor, FaceType eFaceType,
    float nMinTextureCoordX, float nMaxTextureCoordX, float nMinTextureCoordY,
    float nMaxTextureCoordY, float nMinTextureCoordZ, float nMaxTextureCoordZ) {
  VistaVertexFormat oFormat;
  oFormat.color        = VistaVertexFormat::COLOR_NONE;
  oFormat.normal       = VistaVertexFormat::NORMAL;
  oFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

  std::vector<VistaIndexedVertex> index;
  std::vector<float>              coords;
  std::vector<float>              textureCoords;
  std::vector<float>              normals;
  std::vector<VistaColor>         colors;
  VistaGeometry::FaceType         nFaceType;

  CreateBoxData(&index, &coords, &textureCoords, &normals, &colors, &nFaceType, nSizeX, nSizeY,
      nSizeZ, nResolutionX, nResolutionY, nResolutionZ, oColor, eFaceType, nMinTextureCoordX,
      nMaxTextureCoordX, nMinTextureCoordY, nMaxTextureCoordY, nMinTextureCoordZ,
      nMaxTextureCoordZ);

  VistaGeometry* pGeo =
      m_pSG->NewIndexedGeometry(index, coords, textureCoords, normals, colors, oFormat, nFaceType);
  pGeo->SetColor(oColor);

  return pGeo;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateBoxData                                               */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreateBoxData(vector<VistaIndexedVertex>* pIndex, vector<float>* pCoords,
    vector<float>* pTexCoords, vector<float>* pNormals, vector<VistaColor>* pColors,
    VistaGeometry::FaceType* pFaceType, float nSizeX, float nSizeY, float nSizeZ, int nResolutionX,
    int nResolutionY, int nResolutionZ, VistaColor oColor, FaceType eFaceType,
    float nMinTextureCoordX, float nMaxTextureCoordX, float nMinTextureCoordY,
    float nMaxTextureCoordY, float nMinTextureCoordZ, float nMaxTextureCoordZ) {
  int nNumXRings = nResolutionX + 1;
  int nNumYRings = nResolutionY + 1;
  int nNumZRings = nResolutionZ + 1;

  pNormals->push_back(0);
  pNormals->push_back(-1);
  pNormals->push_back(0);

  pNormals->push_back(0);
  pNormals->push_back(0);
  pNormals->push_back(1);

  pNormals->push_back(0);
  pNormals->push_back(1);
  pNormals->push_back(0);

  pNormals->push_back(0);
  pNormals->push_back(0);
  pNormals->push_back(-1);

  pNormals->push_back(-1);
  pNormals->push_back(0);
  pNormals->push_back(0);

  pNormals->push_back(1);
  pNormals->push_back(0);
  pNormals->push_back(0);

  VistaVector3D v3Extents(nSizeX, nSizeY, nSizeZ);

  int nNumVerticesPerRing = 4 + 2 * (nNumYRings - 2) + 2 * (nNumZRings - 2);

  // create vertex coordinates
  int nNumVertices = 2 * (nNumYRings * nNumZRings)             // two caps
                     + (nNumXRings - 2) * nNumVerticesPerRing; // intermediate rings
  // create faces
  int nNumFaces = 2 * nResolutionY * nResolutionZ    // caps
                  + 2 * nResolutionX * nResolutionY  // front/back
                  + 2 * nResolutionX * nResolutionZ; // top/bottom;

  if (eFaceType == FT_QUAD_SPLIT_SYMMETRICALLY)
    nNumVertices += nNumFaces; // center vertices

  pCoords->resize(3 * nNumVertices);

  std::vector<float>::iterator itCoords = pCoords->begin();

  VistaVector3D v3HalfExtents = 0.5f * v3Extents;

  // create cap positions
  VistaVector3D v3Position;
  v3Position[Vista::X] = -v3HalfExtents[Vista::X];
  for (int nY = 0; nY <= nResolutionY; ++nY) {
    v3Position[Vista::Y] = -v3HalfExtents[Vista::Y] + (float)nY / (float)(nResolutionY)*nSizeY;
    for (int nZ = 0; nZ <= nResolutionZ; ++nZ) {
      v3Position[Vista::Z] = -v3HalfExtents[Vista::Z] + (float)nZ / (float)(nResolutionZ)*nSizeZ;
      for (int i = Vista::X; i <= Vista::Z; ++i) {
        (*itCoords) = v3Position[i];
        ++itCoords;
      }
    }
  }
  // create cap position
  v3Position[Vista::X] = v3HalfExtents[Vista::X];
  for (int nY = 0; nY <= nResolutionY; ++nY) {
    v3Position[Vista::Y] = -v3HalfExtents[Vista::Y] + (float)nY / (float)(nResolutionY)*nSizeY;
    for (int nZ = 0; nZ <= nResolutionZ; ++nZ) {
      v3Position[Vista::Z] = -v3HalfExtents[Vista::Z] + (float)nZ / (float)(nResolutionZ)*nSizeZ;
      for (int i = Vista::X; i <= Vista::Z; ++i) {
        (*itCoords) = v3Position[i];
        ++itCoords;
      }
    }
  }
  // create intermediate rings
  v3Position[Vista::X] = -v3HalfExtents[Vista::X];
  float nDeltaX        = v3Extents[Vista::X] / nResolutionX;
  float nDeltaY        = v3Extents[Vista::Y] / (float)nResolutionY;
  float nDeltaZ        = v3Extents[Vista::Z] / (float)nResolutionZ;
  for (int nXPos = 1; nXPos < nNumXRings - 1; ++nXPos) {
    v3Position[Vista::X] += nDeltaX;
    // add vertices ccw -> four line segments
    v3Position[Vista::Y] = -v3HalfExtents[Vista::Y];
    v3Position[Vista::Z] = -v3HalfExtents[Vista::Z];
    AddVertexLine(nResolutionZ, v3Position, nDeltaZ, Vista::Z, itCoords);
    AddVertexLine(nResolutionY, v3Position, nDeltaY, Vista::Y, itCoords);
    AddVertexLine(nResolutionZ, v3Position, -nDeltaZ, Vista::Z, itCoords);
    AddVertexLine(nResolutionY, v3Position, -nDeltaY, Vista::Y, itCoords);
  }

  // create texture coordinates
  int nNumTexCoords = nNumXRings * nNumYRings + nNumYRings * nNumZRings + nNumZRings * nNumXRings;
  if (eFaceType == FT_QUAD_SPLIT_SYMMETRICALLY)
    nNumTexCoords += nNumFaces; // center vertices

  pTexCoords->resize(2 * nNumTexCoords);
  std::vector<float>::iterator itTexCoords = pTexCoords->begin();
  float                        nTexDeltaX  = (nMaxTextureCoordX - nMinTextureCoordX) / nResolutionX;
  float                        nTexDeltaY  = (nMaxTextureCoordY - nMinTextureCoordY) / nResolutionY;
  float                        nTexDeltaZ  = (nMaxTextureCoordZ - nMinTextureCoordZ) / nResolutionZ;
  float                        nTexCoordX, nTexCoordY, nTexCoordZ;
  // x-y-plane
  nTexCoordX = nMinTextureCoordX;
  for (int nX = 0; nX < nNumXRings; ++nX) {
    nTexCoordY = nMinTextureCoordY;
    for (int nY = 0; nY < nNumYRings; ++nY) {
      (*itTexCoords) = nTexCoordX;
      ++itTexCoords;
      (*itTexCoords) = nTexCoordY;
      ++itTexCoords;
      nTexCoordY += nTexDeltaY;
    }
    nTexCoordX += nTexDeltaX;
  }
  // y-z-plane
  nTexCoordY = nMinTextureCoordY;
  for (int nY = 0; nY < nNumYRings; ++nY) {
    nTexCoordZ = nMinTextureCoordZ;
    for (int nZ = 0; nZ < nNumZRings; ++nZ) {
      (*itTexCoords) = nTexCoordZ;
      ++itTexCoords;
      (*itTexCoords) = nTexCoordY;
      ++itTexCoords;
      nTexCoordZ += nTexDeltaZ;
    }
    nTexCoordY += nTexDeltaY;
  }
  // z-x-plane
  nTexCoordX = nMinTextureCoordX;
  for (int nX = 0; nX < nNumXRings; ++nX) {
    nTexCoordZ = nMinTextureCoordZ;
    for (int nZ = 0; nZ < nNumZRings; ++nZ) {
      (*itTexCoords) = nTexCoordX;
      ++itTexCoords;
      (*itTexCoords) = nTexCoordZ;
      ++itTexCoords;
      nTexCoordZ += nTexDeltaZ;
    }
    nTexCoordX += nTexDeltaX;
  }

  // create faces
  *pFaceType = VistaGeometry::VISTA_FACE_TYPE_QUADS;
  switch (eFaceType) {
  case FT_TRIANGLE: {
    pIndex->resize(2 * 3 * nNumFaces);
    *pFaceType = VistaGeometry::VISTA_FACE_TYPE_TRIANGLES;
    break;
  }
  case FT_QUAD: {
    pIndex->resize(4 * nNumFaces);
    *pFaceType = VistaGeometry::VISTA_FACE_TYPE_QUADS;
    break;
  }
  case FT_QUAD_SPLIT_SYMMETRICALLY: {
    pIndex->resize(4 * 3 * nNumFaces);
    *pFaceType = VistaGeometry::VISTA_FACE_TYPE_TRIANGLES;
    break;
  }
  }

  std::vector<VistaIndexedVertex>::iterator itFaces = pIndex->begin();

  const int nCapOneStart   = 0;
  const int nCapTwoStart   = nNumYRings * nNumZRings;
  const int nRingsStart    = 2 * (nNumYRings * nNumZRings);
  int       nCenterCoordId = nNumVertices - nNumFaces;
  int       nCenterTexId   = nNumTexCoords - nNumFaces;

  // add caps - y-z-planes
  const int nXYTexCoordOffset = 0;
  const int nYZTexCoordOffset = nNumXRings * nNumYRings;
  const int nXZTexCoordOffset = nNumXRings * nNumYRings + nNumYRings * nNumZRings;
  for (int nY = 0; nY < nResolutionY; ++nY) {
    const int nLower = nY * nNumZRings;
    const int nUpper = nY * nNumZRings + nNumZRings;
    for (int nZ = 0; nZ < nResolutionZ; ++nZ) {
      const int nLeft  = nZ;
      const int nRight = nZ + 1;
      TypedQuadCreate(itFaces, nCapOneStart + nLower + nLeft, nCapOneStart + nLower + nRight,
          nCapOneStart + nUpper + nRight, nCapOneStart + nUpper + nLeft,
          nYZTexCoordOffset + nLower + nLeft, nYZTexCoordOffset + nLower + nRight,
          nYZTexCoordOffset + nUpper + nRight, nYZTexCoordOffset + nUpper + nLeft, 4, eFaceType,
          *pCoords, *pTexCoords, nCenterCoordId, nCenterTexId);
    }
  }
  for (int nY = 0; nY < nResolutionY; ++nY) {
    const int nLower = nY * nNumZRings;
    const int nUpper = nY * nNumZRings + nNumZRings;
    for (int nZ = 0; nZ < nResolutionZ; ++nZ) {
      const int nLeft  = nZ;
      const int nRight = nZ + 1;
      TypedQuadCreate(itFaces, nCapTwoStart + nLower + nRight, nCapTwoStart + nLower + nLeft,
          nCapTwoStart + nUpper + nLeft, nCapTwoStart + nUpper + nRight,
          nYZTexCoordOffset + nLower + nResolutionZ - nRight,
          nYZTexCoordOffset + nLower + nResolutionZ - nLeft,
          nYZTexCoordOffset + nUpper + nResolutionZ - nLeft,
          nYZTexCoordOffset + nUpper + nResolutionZ - nRight, 5, eFaceType, *pCoords, *pTexCoords,
          nCenterCoordId, nCenterTexId);
    }
  }

  // create four sides ( -y, +z, +y, -z )
  for (int nSideIndex = 0; nSideIndex < 4; ++nSideIndex) {
    int  nHeightResolution = 0;
    int  nXStart           = 0;
    int  nXEnd             = nResolutionX;
    int  nXDelta           = 1;
    int  nNormalIndex      = nSideIndex;
    int  nSideOffset       = 0;
    bool bFlipSide         = false;
    int  nTexCoordOffset   = 0;
    switch (nSideIndex) {
    case 0: // -y
      bFlipSide         = true;
      nHeightResolution = nResolutionZ;
      nSideOffset       = 0;
      nTexCoordOffset   = nXZTexCoordOffset;
      break;
    case 1: // +z
      bFlipSide         = false;
      nHeightResolution = nResolutionY;
      nSideOffset       = nResolutionZ;
      nTexCoordOffset   = nXYTexCoordOffset;
      break;
    case 2: // +y
      bFlipSide         = false;
      nHeightResolution = nResolutionZ;
      nSideOffset       = nResolutionZ + nResolutionY;
      nTexCoordOffset   = nXZTexCoordOffset;
      break;
    case 3: // -z
      bFlipSide         = true;
      nHeightResolution = nResolutionY;
      nSideOffset       = 2 * nResolutionZ + nResolutionY;
      nTexCoordOffset   = nXYTexCoordOffset;
      break;
    }
    if (bFlipSide) {
      std::swap(nXStart, nXEnd);
      nXDelta = -1;
    }
    for (int nX = nXStart; nX != nXEnd; nX += nXDelta) {
      int nNextX          = nX + nXDelta;
      int nCurrentXOffset = nRingsStart + (nX - 1) * nNumVerticesPerRing;
      int nNextXOffset    = nRingsStart + (nNextX - 1) * nNumVerticesPerRing;
      for (int nHeight = 0; nHeight < nHeightResolution; ++nHeight) {
        int nRingCurrent = nSideOffset + nHeight;
        int nRingNext    = nSideOffset + nHeight + 1;
        if (nRingNext == -1)
          nRingNext = nNumVerticesPerRing - 1;
        else if (nRingNext == nNumVerticesPerRing)
          nRingNext = 0;

        int nNumHeightVertices = nHeightResolution + 1;

        int nVertexBL = nCurrentXOffset + nRingCurrent;
        int nVertexBR = nNextXOffset + nRingCurrent;
        int nVertexTR = nNextXOffset + nRingNext;
        int nVertexTL = nCurrentXOffset + nRingNext;

        if (nX == 0) {
          nVertexBL = nCapOneStart + GetCapBorderIndex(nRingCurrent, nNumYRings, nNumZRings);
          nVertexTL = nCapOneStart + GetCapBorderIndex(nRingNext, nNumYRings, nNumZRings);
        } else if (nX == nResolutionX) {
          nVertexBL = nCapTwoStart + GetCapBorderIndex(nRingCurrent, nNumYRings, nNumZRings);
          nVertexTL = nCapTwoStart + GetCapBorderIndex(nRingNext, nNumYRings, nNumZRings);
        }
        if (nNextX == 0) {
          nVertexBR = nCapOneStart + GetCapBorderIndex(nRingCurrent, nNumYRings, nNumZRings);
          nVertexTR = nCapOneStart + GetCapBorderIndex(nRingNext, nNumYRings, nNumZRings);
        } else if (nNextX == nResolutionX) {
          nVertexBR = nCapTwoStart + GetCapBorderIndex(nRingCurrent, nNumYRings, nNumZRings);
          nVertexTR = nCapTwoStart + GetCapBorderIndex(nRingNext, nNumYRings, nNumZRings);
        }

        int nTexCoordBL, nTexCoordBR, nTexCoordTR, nTexCoordTL;
        if (nSideIndex != 3) {
          nTexCoordBL = nTexCoordOffset + nHeight + nNumHeightVertices * nX;
          nTexCoordBR = nTexCoordOffset + nHeight + nNumHeightVertices * (nX + nXDelta);
          nTexCoordTR = nTexCoordOffset + (nHeight + 1) + nNumHeightVertices * (nX + nXDelta);
          nTexCoordTL = nTexCoordOffset + (nHeight + 1) + nNumHeightVertices * nX;
        } else {
          // swap along s and z
          nTexCoordBL = nTexCoordOffset + (nNumHeightVertices - nHeight) +
                        nNumHeightVertices * (nResolutionX - nX);
          nTexCoordBR = nTexCoordOffset + (nNumHeightVertices - nHeight) +
                        nNumHeightVertices * (nResolutionX - nX - nXDelta);
          nTexCoordTR = nTexCoordOffset + (nNumHeightVertices - nHeight - 1) +
                        nNumHeightVertices * (nResolutionX - nX - nXDelta);
          nTexCoordTL = nTexCoordOffset + (nNumHeightVertices - nHeight - 1) +
                        nNumHeightVertices * (nResolutionX - nX);
        }

        if (bFlipSide) {
          TypedQuadCreate(itFaces, nVertexBL, nVertexTL, nVertexTR, nVertexBR, nTexCoordBL,
              nTexCoordTL, nTexCoordTR, nTexCoordBR, nNormalIndex, eFaceType, *pCoords, *pTexCoords,
              nCenterCoordId, nCenterTexId);
        } else {
          TypedQuadCreate(itFaces, nVertexBL, nVertexBR, nVertexTR, nVertexTL, nTexCoordBL,
              nTexCoordBR, nTexCoordTR, nTexCoordTL, nNormalIndex, eFaceType, *pCoords, *pTexCoords,
              nCenterCoordId, nCenterTexId);
        }
      }
    }
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateDisk                                                  */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateDisk(
    float radius, int resC, int resD, Vista::AXIS normal, VistaColor color) {
  VistaVertexFormat vertexFormat;
  vertexFormat.coordinate   = VistaVertexFormat::COORDINATE;
  vertexFormat.color        = VistaVertexFormat::COLOR_NONE;
  vertexFormat.normal       = VistaVertexFormat::NORMAL;
  vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

  vector<VistaIndexedVertex> index;
  vector<float>              coords;
  vector<float>              textureCoords;
  vector<float>              normals;
  vector<VistaColor>         colors;

  CreateDiskData(
      &index, &coords, &textureCoords, &normals, &colors, radius, resC, resD, normal, color);

  VistaGeometry* ret = GetSG()->NewIndexedGeometry(index, coords, textureCoords, normals, colors,
      vertexFormat, VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
  ret->SetColor(color);

  return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateDiskData                                              */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreateDiskData(vector<VistaIndexedVertex>* pIndex,
    vector<float>* pCoords, vector<float>* pTexCoords, vector<float>* pNormals,
    vector<VistaColor>* pColors, float radius, int resC, int resD, Vista::AXIS normal,
    VistaColor color) {
  VistaIndexedVertex ci;
  ci.SetColorIndex(0);
  ci.SetNormalIndex(0);

  float normalVec[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

  pNormals->push_back(normalVec[normal][0]);
  pNormals->push_back(normalVec[normal][1]);
  pNormals->push_back(normalVec[normal][2]);

  pCoords->push_back(0.0);
  pCoords->push_back(0.0);
  pCoords->push_back(0.0); // Middle

  pTexCoords->push_back(0.5f);
  pTexCoords->push_back(0.5f);

  float val[3];
  val[(normal + 0) % 3] = 0.0f;

  float step  = 2.0f / resC;
  float stepR = radius / resD;

  if (step == 0.0f)
    return false;

  for (int j = 1; j <= resD; j++) {
    for (int i = 0; i < resC; i++) {
      val[(normal + 2) % 3] = (float)(j * stepR * std::cos(i * 2 * Vista::Pi / resC));
      val[(normal + 1) % 3] = (float)(-j * stepR * std::sin(i * 2 * Vista::Pi / resC));

      pCoords->push_back(val[0]);
      pCoords->push_back(val[1]);
      pCoords->push_back(val[2]);
      switch (normal) {
      case Vista::X:
        pTexCoords->push_back(
            (float)((-j * stepR * std::cos(i * 2 * Vista::Pi / resC)) / radius + 1) / 2);
        pTexCoords->push_back(
            (float)((-j * stepR * std::sin(i * 2 * Vista::Pi / resC)) / radius + 1) / 2);
        break;
      default:
      case Vista::Y:
        pTexCoords->push_back(
            (float)((j * stepR * std::cos(i * 2 * Vista::Pi / resC)) / radius + 1) / 2);
        pTexCoords->push_back(
            (float)((j * stepR * std::sin(i * 2 * Vista::Pi / resC)) / radius + 1) / 2);
        break;
      case Vista::Z:
        pTexCoords->push_back(
            (float)((-j * stepR * std::sin(i * 2 * Vista::Pi / resC)) / radius + 1) / 2);
        pTexCoords->push_back(
            (float)((j * stepR * std::cos(i * 2 * Vista::Pi / resC)) / radius + 1) / 2);
        break;
      }
    }
  }

  int i = 1;
  for (i = 1; i < resC; i++) {
    ci.SetCoordinateIndex(0);
    ci.SetTextureCoordinateIndex(0);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(i);
    ci.SetTextureCoordinateIndex(i);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(i + 1);
    ci.SetTextureCoordinateIndex(i + 1);
    pIndex->push_back(ci);
  }
  ci.SetCoordinateIndex(0);
  ci.SetTextureCoordinateIndex(0);
  pIndex->push_back(ci);
  ci.SetCoordinateIndex(i);
  ci.SetTextureCoordinateIndex(i);
  pIndex->push_back(ci);
  ci.SetCoordinateIndex(1);
  ci.SetTextureCoordinateIndex(1);
  pIndex->push_back(ci);

  for (int j = 0; j <= resD - 2; j++) {
    for (i = 1 + j * resC; i < resC + j * resC; i++) {
      ci.SetCoordinateIndex(i);
      ci.SetTextureCoordinateIndex(i);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(i + resC);
      ci.SetTextureCoordinateIndex(i + resC);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(i + resC + 1);
      ci.SetTextureCoordinateIndex(i + resC + 1);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(i);
      ci.SetTextureCoordinateIndex(i);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(i + 1);
      ci.SetTextureCoordinateIndex(i + 1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(i + resC + 1);
      ci.SetTextureCoordinateIndex(i + resC + 1);
      pIndex->push_back(ci);
    }
    ci.SetCoordinateIndex(i);
    ci.SetTextureCoordinateIndex(i);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(i + resC);
    ci.SetTextureCoordinateIndex(i + resC);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(1 + j * resC + resC);
    ci.SetTextureCoordinateIndex(1 + j * resC + resC);
    pIndex->push_back(ci);

    ci.SetCoordinateIndex(i);
    ci.SetTextureCoordinateIndex(i);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(1 + j * resC);
    ci.SetTextureCoordinateIndex(1 + j * resC);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(1 + j * resC + resC);
    ci.SetTextureCoordinateIndex(1 + j * resC + resC);
    pIndex->push_back(ci);
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateCone                                                  */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateCone(float botRad, float topRad, float height, int resC,
    int resD, int resY, VistaColor color, bool bBottom, bool bTop, bool bSides) {
  vector<VistaIndexedVertex> index;
  vector<float>              coords;
  vector<float>              textureCoords;
  vector<float>              normals;
  vector<VistaColor>         colors;

  VistaVertexFormat vertexFormat;
  vertexFormat.coordinate   = VistaVertexFormat::COORDINATE;
  vertexFormat.color        = VistaVertexFormat::COLOR_NONE;
  vertexFormat.normal       = VistaVertexFormat::NORMAL;
  vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

  CreateConeData(&index, &coords, &textureCoords, &normals, &colors, botRad, topRad, height, resC,
      resD, resY, color, bBottom, bTop, bSides);

  VistaGeometry* ret = GetSG()->NewIndexedGeometry(index, coords, textureCoords, normals, colors,
      vertexFormat, VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
  ret->SetColor(color);

  return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateConeData                                              */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreateConeData(vector<VistaIndexedVertex>* pIndex,
    vector<float>* pCoords, vector<float>* pTexCoords, vector<float>* pNormals,
    vector<VistaColor>* pColors, float botRad, float topRad, float height, int resC, int resD,
    int resY, VistaColor color, bool bBottom, bool bTop, bool bSides) {
  VistaIndexedVertex ci;
  ci.SetColorIndex(0);
  ci.SetNormalIndex(0);

  pCoords->push_back(0.0);
  pCoords->push_back(-height / 2.0f);
  pCoords->push_back(0.0); // Middle of the bottom

  pTexCoords->push_back(0.5);
  pTexCoords->push_back(0.5); // Middle of the bottom

  float step   = 2.0f / resC;
  float stepR  = botRad / resD;
  float stepYR = (botRad - topRad) / resY;
  float stepY  = height / resY;
  float y      = -height / 2.0f;
  float r      = botRad;

  if (step == 0.0f)
    return false;

  // Coords Bottom circle
  if (bBottom) {
    for (int j = 1; j <= resD; j++) {
      for (int i = 0; i < resC; i++) {
        pCoords->push_back((float)(j * stepR * std::cos(i * 2 * Vista::Pi / resC)));
        pCoords->push_back(-height / 2.0f);
        pCoords->push_back((float)(-j * stepR * std::sin(i * 2 * Vista::Pi / resC)));

        pTexCoords->push_back(
            (float)((-j * stepR * std::cos(i * 2 * Vista::Pi / resC)) / botRad + 1) / 2);
        pTexCoords->push_back(
            (float)((j * stepR * std::sin(i * 2 * Vista::Pi / resC)) / botRad + 1) / 2);
      }
    }
  }

  // Coords Ring
  if (bSides) {
    for (int j = 0; j < resY + 1; j++) {
      for (int i = 0; i < resC; i++) {
        pCoords->push_back((float)(r * std::cos(i * 2 * Vista::Pi / resC)));
        pCoords->push_back(y);
        pCoords->push_back((float)(-r * std::sin(i * 2 * Vista::Pi / resC)));

        pTexCoords->push_back((float)i / resC);
        pTexCoords->push_back((float)j / (resY /*+2*/));
      }
      y += stepY;
      r = r - stepYR;
    }
  }

  // Coords Top circle
  if (bTop) {
    float stepR1 = topRad / resD;
    for (int j = 1; j <= resD; j++) {
      for (int i = 0; i < resC; i++) {
        pCoords->push_back((float)(j * stepR1 * std::cos(i * 2 * Vista::Pi / resC)));
        pCoords->push_back(height / 2.0f);
        pCoords->push_back((float)(-j * stepR1 * std::sin(i * 2 * Vista::Pi / resC)));

        pTexCoords->push_back(
            (float)((j * stepR1 * std::cos(i * 2 * Vista::Pi / resC)) / topRad + 1) / 2);
        pTexCoords->push_back(
            (float)((j * stepR1 * std::sin(i * 2 * Vista::Pi / resC)) / topRad + 1) / 2);
      }
    }
    pCoords->push_back(0);
    pCoords->push_back(height / 2.0f);
    pCoords->push_back(0);

    pTexCoords->push_back(0.5);
    pTexCoords->push_back(0.5);
  }

  // Adding the last texture coords
  // cause of double using the geocoords but single use of texcoords
  if (bSides) {
    for (int i = 0; i < resY + 2; i++) {
      pTexCoords->push_back(1);
      pTexCoords->push_back((float)i / (resY /*+2*/));
    }
  }

  // Creating normals
  pNormals->resize(3 * resC + 6);
  std::vector<float>::iterator itNormalEntry = pNormals->begin();
  float                        fYComponent;
  float                        fFactor = atan((botRad - topRad) / height);
  fYComponent                          = sin(fFactor);
  fFactor                              = cos(fFactor);
  (*itNormalEntry)                     = 0.0f;
  (*++itNormalEntry)                   = -1.0f;
  (*++itNormalEntry)                   = 0.0f;
  (*++itNormalEntry)                   = 0.0f;
  (*++itNormalEntry)                   = 1.0f;
  (*++itNormalEntry)                   = 0.0f;
  for (int i = 0; i < resC; ++i) {
    float fAngle       = i * 2 * Vista::Pi / resC;
    (*++itNormalEntry) = fFactor * cos(fAngle);
    (*++itNormalEntry) = fYComponent;
    (*++itNormalEntry) = -fFactor * sin(fAngle);
  }

  // Building the geometry
  int texSize = (int)pTexCoords->size() / 2;
  int ind     = 1;
  if (bBottom) {
    ci.SetNormalIndex(1);
    for (ind = 1; ind < resC; ind++) {
      ci.SetCoordinateIndex(ind + 1);
      ci.SetTextureCoordinateIndex(ind + 1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(0);
      ci.SetTextureCoordinateIndex(0);
      pIndex->push_back(ci);
    }
    ci.SetCoordinateIndex(1);
    ci.SetTextureCoordinateIndex(1);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(ind);
    ci.SetTextureCoordinateIndex(ind);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(0);
    ci.SetTextureCoordinateIndex(0);
    pIndex->push_back(ci);

    for (int j = 0; j <= resD - 2; j++) {
      for (ind = 1 + j * resC; ind < resC + j * resC; ind++) {
        ci.SetCoordinateIndex(ind + resC + 1);
        ci.SetTextureCoordinateIndex(ind + resC + 1);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + resC);
        ci.SetTextureCoordinateIndex(ind + resC);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind);
        ci.SetTextureCoordinateIndex(ind);
        pIndex->push_back(ci);

        ci.SetCoordinateIndex(ind);
        ci.SetTextureCoordinateIndex(ind);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + 1);
        ci.SetTextureCoordinateIndex(ind + 1);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + resC + 1);
        ci.SetTextureCoordinateIndex(ind + resC + 1);
        pIndex->push_back(ci);
      }
      ci.SetCoordinateIndex(1 + j * resC + resC);
      ci.SetTextureCoordinateIndex(1 + j * resC + resC);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + resC);
      ci.SetTextureCoordinateIndex(ind + resC);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(1 + j * resC);
      ci.SetTextureCoordinateIndex(1 + j * resC);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(1 + j * resC + resC);
      ci.SetTextureCoordinateIndex(1 + j * resC + resC);
      pIndex->push_back(ci);
    }
  }

  // sides
  if (bSides) {

    if (bBottom) {
      ind = resD * resC + 1;
    } else {
      ind = 1;
    }
    for (int j = 0; j < resY - 1; j++) {
      for (int n = 0; n < resC - 1; n++) {
        ci.SetCoordinateIndex(ind);
        ci.SetTextureCoordinateIndex(ind);
        ci.SetNormalIndex(2 + n);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + resC + 1);
        ci.SetTextureCoordinateIndex(ind + resC + 1);
        ci.SetNormalIndex(3 + n);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + resC);
        ci.SetTextureCoordinateIndex(ind + resC);
        ci.SetNormalIndex(2 + n);
        pIndex->push_back(ci);

        ci.SetCoordinateIndex(ind + 1);
        ci.SetTextureCoordinateIndex(ind + 1);
        ci.SetNormalIndex(3 + n);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + resC + 1);
        ci.SetTextureCoordinateIndex(ind + resC + 1);
        ci.SetNormalIndex(3 + n);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind);
        ci.SetTextureCoordinateIndex(ind);
        ci.SetNormalIndex(2 + n);
        pIndex->push_back(ci);
        ind++;
      }

      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      ci.SetNormalIndex(1 + resC);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + 1);
      // ci.SetTextureCoordinateIndex(ind+1);
      ci.SetTextureCoordinateIndex(texSize - resY + j - 1);
      ci.SetNormalIndex(2);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + resC);
      ci.SetTextureCoordinateIndex(ind + resC);
      ci.SetNormalIndex(1 + resC);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      ci.SetNormalIndex(1 + resC);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind - resC + 1);
      ci.SetTextureCoordinateIndex(texSize - resY + j - 2);
      ci.SetNormalIndex(2);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + 1);
      ci.SetTextureCoordinateIndex(texSize - resY + j - 1);
      ci.SetNormalIndex(2);
      pIndex->push_back(ci);
      ind++;
    }

    // last ring
    for (int n = 0; n < resC - 1; n++) {
      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      ci.SetNormalIndex(2 + n);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + resC + 1);
      ci.SetTextureCoordinateIndex(ind + resC + 1);
      ci.SetNormalIndex(3 + n);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + resC);
      ci.SetTextureCoordinateIndex(ind + resC);
      ci.SetNormalIndex(2 + n);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(ind + 1);
      ci.SetTextureCoordinateIndex(ind + 1);
      ci.SetNormalIndex(3 + n);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + resC + 1);
      ci.SetTextureCoordinateIndex(ind + resC + 1);
      ci.SetNormalIndex(3 + n);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      ci.SetNormalIndex(2 + n);
      pIndex->push_back(ci);
      ind++;
    }

    ci.SetCoordinateIndex(ind + resC);
    ci.SetTextureCoordinateIndex(ind + resC);
    ci.SetNormalIndex(1 + resC);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(ind);
    ci.SetTextureCoordinateIndex(ind);
    ci.SetNormalIndex(1 + resC);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(ind + 1);
    ci.SetTextureCoordinateIndex(texSize - 2);
    ci.SetNormalIndex(2);
    pIndex->push_back(ci);

    ci.SetCoordinateIndex(ind);
    ci.SetTextureCoordinateIndex(ind);
    ci.SetNormalIndex(1 + resC);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(ind - resC + 1);
    ci.SetTextureCoordinateIndex(texSize - 3);
    ci.SetNormalIndex(2);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(ind + 1);
    ci.SetTextureCoordinateIndex(texSize - 2);
    ci.SetNormalIndex(2);
    pIndex->push_back(ci);
    ind++;
  }

  // top circle
  if (bTop) {
    ind = (int)pCoords->size() / 3 - 1 - resC * resD;
    ci.SetNormalIndex(2);
    for (int i = 1; i < resC; i++) {
      ci.SetCoordinateIndex((int)pCoords->size() / 3 - 1);
      ci.SetTextureCoordinateIndex((int)pCoords->size() / 3 - 1);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind + 1);
      ci.SetTextureCoordinateIndex(ind + 1);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
      ind++;
    }

    ci.SetCoordinateIndex((int)pCoords->size() / 3 - 1);
    ci.SetTextureCoordinateIndex((int)pCoords->size() / 3 - 1);
    ci.SetNormalIndex(1);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(ind);
    ci.SetTextureCoordinateIndex(ind);
    ci.SetNormalIndex(1);
    pIndex->push_back(ci);
    ci.SetCoordinateIndex(ind - resC + 1);
    ci.SetTextureCoordinateIndex(ind - resC + 1);
    ci.SetNormalIndex(1);
    pIndex->push_back(ci);

    for (int j = 1; j < resD; j++) {
      int stop = ind + resC;
      for (ind = ind + 1; ind < stop; ind++) {

        ci.SetCoordinateIndex(ind);
        ci.SetTextureCoordinateIndex(ind);
        ci.SetNormalIndex(1);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + 1);
        ci.SetTextureCoordinateIndex(ind + 1);
        ci.SetNormalIndex(1);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind - resC);
        ci.SetTextureCoordinateIndex(ind - resC);
        ci.SetNormalIndex(1);
        pIndex->push_back(ci);

        ci.SetCoordinateIndex(ind - resC);
        ci.SetTextureCoordinateIndex(ind - resC);
        ci.SetNormalIndex(1);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind + 1);
        ci.SetTextureCoordinateIndex(ind + 1);
        ci.SetNormalIndex(1);
        pIndex->push_back(ci);
        ci.SetCoordinateIndex(ind - resC + 1);
        ci.SetTextureCoordinateIndex(ind - resC + 1);
        ci.SetNormalIndex(1);
        pIndex->push_back(ci);
      }
      ci.SetCoordinateIndex(ind - resC);
      ci.SetTextureCoordinateIndex(ind - resC);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind);
      ci.SetTextureCoordinateIndex(ind);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind - resC + 1);
      ci.SetTextureCoordinateIndex(ind - resC + 1);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(ind - resC + 1);
      ci.SetTextureCoordinateIndex(ind - resC + 1);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind - 2 * resC + 1);
      ci.SetTextureCoordinateIndex(ind - 2 * resC + 1);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
      ci.SetCoordinateIndex(ind - resC);
      ci.SetTextureCoordinateIndex(ind - resC);
      ci.SetNormalIndex(1);
      pIndex->push_back(ci);
    }
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateTorus                                                 */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateTorus(
    float innerRad, float outerRad, int resSides, int resRings, VistaColor color) {
  VistaVertexFormat vertexFormat;
  vertexFormat.coordinate   = VistaVertexFormat::COORDINATE;
  vertexFormat.color        = VistaVertexFormat::COLOR_NONE;
  vertexFormat.normal       = VistaVertexFormat::NORMAL;
  vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

  vector<VistaIndexedVertex> index;
  vector<float>              coords;
  vector<float>              textureCoords;
  vector<float>              normals;
  vector<VistaColor>         colors;

  CreateTorusData(innerRad, outerRad, resSides, resRings, color, &index, &coords, &textureCoords,
      &normals, &colors);

  VistaGeometry* ret = GetSG()->NewIndexedGeometry(index, coords, textureCoords, normals, colors,
      vertexFormat, VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
  ret->SetColor(color);

  return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateTorusData                                             */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreateTorusData(float innerRad, float outerRad, int resSides,
    int resRings, VistaColor color, vector<VistaIndexedVertex>* pIndex, vector<float>* pCoords,
    vector<float>* pTexCoords, vector<float>* pNormals, vector<VistaColor>* pColors) {
  VistaIndexedVertex ci;
  ci.SetColorIndex(0);

  int   a, b;
  float theta, phi;
  float cosTheta, sinTheta;
  float ringDelta, sideDelta;
  ringDelta = 2.0f * Vista::Pi / resRings;
  sideDelta = 2.0f * Vista::Pi / resSides;

  for (a = 0, theta = 0.0; a <= resRings; a++, theta += ringDelta) {
    cosTheta = std::cos(theta);
    sinTheta = std::sin(theta);

    for (b = 0, phi = 0.0; b <= resSides; b++, phi += sideDelta) {
      float cosPhi, sinPhi, dist;

      cosPhi = std::cos(phi);
      sinPhi = std::sin(phi);
      dist   = outerRad + innerRad * cosPhi;

      pNormals->push_back(cosTheta * cosPhi);
      pNormals->push_back(-sinTheta * cosPhi);
      pNormals->push_back(sinPhi);

      pCoords->push_back(cosTheta * dist);
      pCoords->push_back(-sinTheta * dist);
      pCoords->push_back(innerRad * sinPhi);

      pTexCoords->push_back(1 - (float)a / resRings);
      pTexCoords->push_back((float)b / resSides);
    }
  }

  for (a = 0; a < resSides; a++) {
    for (b = 0; b < resRings; b++) {
      ci.SetCoordinateIndex(b * (resSides + 1) + a);
      ci.SetTextureCoordinateIndex(b * (resSides + 1) + a);
      ci.SetNormalIndex(b * (resSides + 1) + a);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(b * (resSides + 1) + a + 1);
      ci.SetTextureCoordinateIndex(b * (resSides + 1) + a + 1);
      ci.SetNormalIndex(b * (resSides + 1) + a + 1);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(b * (resSides + 1) + a + resSides + 1);
      ci.SetTextureCoordinateIndex(b * (resSides + 1) + a + resSides + 1);
      ci.SetNormalIndex(b * (resSides + 1) + a + resSides + 1);
      pIndex->push_back(ci);

      // NEXT TRIANGLE

      ci.SetCoordinateIndex(b * (resSides + 1) + a + resSides + 1);
      ci.SetTextureCoordinateIndex(b * (resSides + 1) + a + resSides + 1);
      ci.SetNormalIndex(b * (resSides + 1) + a + resSides + 1);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(b * (resSides + 1) + a + 1);
      ci.SetTextureCoordinateIndex(b * (resSides + 1) + a + 1);
      ci.SetNormalIndex(b * (resSides + 1) + a + 1);
      pIndex->push_back(ci);

      ci.SetCoordinateIndex(b * (resSides + 1) + a + resSides + 2);
      ci.SetTextureCoordinateIndex(b * (resSides + 1) + a + resSides + 2);
      ci.SetNormalIndex(b * (resSides + 1) + a + resSides + 2);
      pIndex->push_back(ci);
    }
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateSphere                                                */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateSphere(float radius, int resolution, VistaColor color) {
  vector<VistaIndexedVertex> index;
  vector<VistaVector3D>      coords;
  vector<VistaVector3D>      textureCoords;
  vector<VistaVector3D>      normals;
  vector<VistaColor>         colors;

  VistaVertexFormat vFormat;

  vFormat.coordinate   = VistaVertexFormat::COORDINATE;
  vFormat.normal       = VistaVertexFormat::NORMAL;
  vFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;
  vFormat.color        = VistaVertexFormat::COLOR_NONE;

  CreateSphereData(&index, &coords, &textureCoords, &normals, &colors, radius, resolution, color);

  VistaGeometry* ret = GetSG()->NewIndexedGeometry(index, coords, textureCoords, normals, colors,
      vFormat, VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
  ret->SetColor(color);

  return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateSphereData                                            */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreateSphereData(vector<VistaIndexedVertex>* pIndex,
    vector<VistaVector3D>* pCoords, vector<VistaVector3D>* pTexCoords,
    vector<VistaVector3D>* pNormals, vector<VistaColor>* pColors, float radius, int resolution,
    const VistaColor& color) {
  return CreateEllipsoidData(pIndex, pCoords, pTexCoords, pNormals, pColors, radius, radius, radius,
      resolution, resolution, color);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateEllipsoid                                             */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateEllipsoid(float radius_a, float radius_b, float radius_c,
    int thetaPrecision, int phiPrecision, const VistaColor& color) {
  vector<VistaIndexedVertex> index;
  vector<VistaVector3D>      coords;
  vector<VistaVector3D>      textureCoords;
  vector<VistaVector3D>      normals;
  vector<VistaColor>         colors;

  VistaVertexFormat vFormat;

  vFormat.coordinate   = VistaVertexFormat::COORDINATE;
  vFormat.normal       = VistaVertexFormat::NORMAL;
  vFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;
  vFormat.color        = VistaVertexFormat::COLOR_NONE;

  CreateEllipsoidData(&index, &coords, &textureCoords, &normals, &colors, radius_a, radius_b,
      radius_c, thetaPrecision, phiPrecision, color);

  VistaGeometry* ret = GetSG()->NewIndexedGeometry(index, coords, textureCoords, normals, colors,
      vFormat, VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);
  ret->SetColor(color);

  return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateEllipsoidData                                         */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreateEllipsoidData(vector<VistaIndexedVertex>* pIndex,
    vector<VistaVector3D>* pCoords, vector<VistaVector3D>* pTexCoords,
    vector<VistaVector3D>* pNormals, vector<VistaColor>* pColors, float radius_a, float radius_b,
    float radius_c, int thetaPrecision, int phiPrecision, const VistaColor& color) {
  int   i, j, idx, tidx;
  float phi, deltaPhi;
  float theta, deltaTheta;
  int   phiResolution;

  VistaIndexedVertex vertex;

  // determine increments
  deltaPhi      = Vista::Pi / (phiPrecision - 1);
  deltaTheta    = 2.0f * Vista::Pi / thetaPrecision;
  phiResolution = phiPrecision - 2;

  pIndex->resize(phiResolution * 6 * thetaPrecision);

  pCoords->resize(thetaPrecision * (phiPrecision - 2) + 2);
  pNormals->resize(thetaPrecision * (phiPrecision - 2) + 2);
  pTexCoords->resize((thetaPrecision + 1) * (phiPrecision - 2) + 2 * (thetaPrecision));

  // Point and normal creation
  // intermediate points
  idx = 0;
  for (i = 0; i < thetaPrecision; i++) {
    theta = i * deltaTheta;

    for (j = 1; j < phiPrecision - 1; j++) {
      phi = j * deltaPhi;

      (*pCoords)[idx] = VistaVector3D(-radius_a * sinf(phi) * cosf(theta), -radius_b * cosf(phi),
          -radius_c * sinf(phi) * sinf(theta));

      (*pNormals)[idx] = VistaVector3D(-sinf(phi) * cosf(theta) / radius_a, -cosf(phi) / radius_b,
          -sinf(phi) * sinf(theta) / radius_c);
      (*pNormals)[idx].Normalize();
      idx++;
    }
  }

  // north and south poles
  (*pCoords)[idx]     = VistaVector3D(0.0, -radius_b, 0.0);
  (*pCoords)[idx + 1] = VistaVector3D(0.0, radius_b, 0.0);

  (*pNormals)[idx]     = VistaVector3D(0.0, -1.0, 0.0);
  (*pNormals)[idx + 1] = VistaVector3D(0.0, 1.0, 0.0);

  // Texture coordinates creation
  tidx = 0;
  for (i = 0; i <= thetaPrecision; i++)
    for (j = 1; j < phiPrecision - 1; j++)
      (*pTexCoords)[tidx++] = VistaVector3D(
          float(i) / float(thetaPrecision), float(j) / (float(phiPrecision) - 1.0f), 0.0);

  for (j = 0; j < 2; j++)
    for (i = 0; i < thetaPrecision; i++)
      (*pTexCoords)[tidx++] = VistaVector3D(
          (2.0f * float(i) + 1.0f) / (2.0f * float(thetaPrecision)), j ? 1.0f : 0.0f, 0.0f);

  // Generate vertices and faces
  idx = 0;
  for (i = 0; i < thetaPrecision; i++) {
    // around north pole
    vertex = VistaIndexedVertex(phiResolution * thetaPrecision);
    vertex.SetNormalIndex(phiResolution * thetaPrecision);
    vertex.SetTextureCoordinateIndex(phiResolution * (thetaPrecision + 1) + i);
    (*pIndex)[idx++] = vertex;

    vertex = VistaIndexedVertex(phiResolution * i);
    vertex.SetNormalIndex(phiResolution * i);
    vertex.SetTextureCoordinateIndex(phiResolution * i);
    (*pIndex)[idx++] = vertex;

    vertex = VistaIndexedVertex(phiResolution * ((i + 1) % thetaPrecision));
    vertex.SetNormalIndex(phiResolution * ((i + 1) % thetaPrecision));
    vertex.SetTextureCoordinateIndex(phiResolution * (i + 1));
    (*pIndex)[idx++] = vertex;

    // around south pole
    vertex = VistaIndexedVertex(phiResolution * thetaPrecision + 1);
    vertex.SetNormalIndex(phiResolution * thetaPrecision + 1);
    vertex.SetTextureCoordinateIndex((phiResolution + 1) * (thetaPrecision + 1) - 1 + i);
    (*pIndex)[idx++] = vertex;

    vertex = VistaIndexedVertex(phiResolution * (((i + 1) % thetaPrecision) + 1) - 1);
    vertex.SetNormalIndex(phiResolution * (((i + 1) % thetaPrecision) + 1) - 1);
    vertex.SetTextureCoordinateIndex(phiResolution * (i + 2) - 1);
    (*pIndex)[idx++] = vertex;

    vertex = VistaIndexedVertex(phiResolution * (i + 1) - 1);
    vertex.SetNormalIndex(phiResolution * (i + 1) - 1);
    vertex.SetTextureCoordinateIndex(phiResolution * (i + 1) - 1);
    (*pIndex)[idx++] = vertex;
  }

  // bands in-between poles
  for (i = 0; i < phiResolution - 1; i++) {
    for (j = 0; j < thetaPrecision; j++) {
      vertex = VistaIndexedVertex(i + phiResolution * j);
      vertex.SetNormalIndex(i + phiResolution * j);
      vertex.SetTextureCoordinateIndex(i + phiResolution * j);
      (*pIndex)[idx++] = vertex;

      vertex = VistaIndexedVertex(i + phiResolution * j + 1);
      vertex.SetNormalIndex(i + phiResolution * j + 1);
      vertex.SetTextureCoordinateIndex(i + phiResolution * j + 1);
      (*pIndex)[idx++] = vertex;

      vertex = VistaIndexedVertex(i + (phiResolution * ((j + 1) % thetaPrecision)) + 1);
      vertex.SetNormalIndex(i + (phiResolution * ((j + 1) % thetaPrecision)) + 1);
      vertex.SetTextureCoordinateIndex(i + (phiResolution * (j + 1)) + 1);
      (*pIndex)[idx++] = vertex;

      vertex = VistaIndexedVertex(i + phiResolution * j);
      vertex.SetNormalIndex(i + phiResolution * j);
      vertex.SetTextureCoordinateIndex(i + phiResolution * j);
      (*pIndex)[idx++] = vertex;

      vertex = VistaIndexedVertex(i + (phiResolution * ((j + 1) % thetaPrecision)) + 1);
      vertex.SetNormalIndex(i + (phiResolution * ((j + 1) % thetaPrecision)) + 1);
      vertex.SetTextureCoordinateIndex(i + (phiResolution * (j + 1)) + 1);
      (*pIndex)[idx++] = vertex;

      vertex = VistaIndexedVertex(i + (phiResolution * ((j + 1) % thetaPrecision)));
      vertex.SetNormalIndex(i + (phiResolution * ((j + 1) % thetaPrecision)));
      vertex.SetTextureCoordinateIndex(i + (phiResolution * (j + 1)));
      (*pIndex)[idx++] = vertex;
    }
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateTriangle                                              */
/*                                                                            */
/*============================================================================*/
VistaGeometry* VistaGeometryFactory::CreateTriangle(const VistaVector3D& a, const VistaVector3D& b,
    const VistaVector3D& c, int resolution, const VistaColor& color) {
  // set up vector for indexed geometries
  vector<VistaIndexedVertex> index;
  vector<VistaVector3D>      coords;
  vector<VistaVector3D>      textureCoords;
  vector<VistaVector3D>      normals;
  vector<VistaColor>         colors;

  // set up vertex format
  VistaVertexFormat vertexFormat;
  vertexFormat.coordinate   = VistaVertexFormat::COORDINATE;
  vertexFormat.color        = VistaVertexFormat::COLOR_RGB;
  vertexFormat.normal       = VistaVertexFormat::NORMAL;
  vertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_2D;

  CreateTriangleData(
      a, b, c, resolution, color, &index, &coords, &textureCoords, &normals, &colors);

  VistaGeometry* ret = GetSG()->NewIndexedGeometry(index, coords, textureCoords, normals, colors,
      vertexFormat, VistaGeometry::VISTA_FACE_TYPE_TRIANGLES);

  return ret;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateTriangleData                                          */
/*                                                                            */
/*============================================================================*/
bool VistaGeometryFactory::CreateTriangleData(const VistaVector3D& a, const VistaVector3D& b,
    const VistaVector3D& c, int resolution, const VistaColor& color,
    std::vector<VistaIndexedVertex>* pIndex, std::vector<VistaVector3D>* pCoords,
    std::vector<VistaVector3D>* pTexCoords, std::vector<VistaVector3D>* pNormals,
    std::vector<VistaColor>* pColors) {
  // calculate vectors relative to a
  VistaVector3D v1 = (c - a) / (float)resolution;
  VistaVector3D v2 = (b - a) / (float)resolution;

  // create vertices
  for (int i = 0; i < resolution + 1; ++i) {
    VistaVector3D offset = a + (float)i * v1;
    for (int j = 0; j < resolution + 1 - i; ++j) {
      pCoords->push_back(offset + v2 * (float)j);
    }
  }

  // create normals
  pNormals->push_back(v1.Cross(v2));

  // create colors
  pColors->push_back(color);

  // create texture coordinates
  // ToDo: calculate texture-coordinates for triangle
  pTexCoords->push_back(VistaVector3D(0, 0, 0));

  // create indices
  int offset = 0;
  int step   = resolution;
  for (int i = 0; i < resolution; ++i) {
    for (int j = 0; j < resolution - i; ++j) {

      VistaIndexedVertex vertex(offset + j);
      vertex.SetColorIndex(0);
      vertex.SetNormalIndex(0);
      vertex.SetTextureCoordinateIndex(0);
      pIndex->push_back(vertex);

      vertex = VistaIndexedVertex(offset + j + 1);
      vertex.SetColorIndex(0);
      vertex.SetNormalIndex(0);
      vertex.SetTextureCoordinateIndex(0);
      pIndex->push_back(vertex);

      vertex = VistaIndexedVertex(offset + j + 1 + step);
      vertex.SetColorIndex(0);
      vertex.SetNormalIndex(0);
      vertex.SetTextureCoordinateIndex(0);
      pIndex->push_back(vertex);

      if (i > 0) {
        VistaIndexedVertex vertex(offset + j);
        vertex.SetColorIndex(0);
        vertex.SetNormalIndex(0);
        vertex.SetTextureCoordinateIndex(0);
        pIndex->push_back(vertex);

        vertex = VistaIndexedVertex(offset + j - (1 + step));
        vertex.SetColorIndex(0);
        vertex.SetNormalIndex(0);
        vertex.SetTextureCoordinateIndex(0);
        pIndex->push_back(vertex);

        vertex = VistaIndexedVertex(offset + j + 1);
        vertex.SetColorIndex(0);
        vertex.SetNormalIndex(0);
        vertex.SetTextureCoordinateIndex(0);
        pIndex->push_back(vertex);
      }
    }
    offset += resolution - i + 1;
    --step;
  }

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateGeoFromPropertyList                                   */
/*                                                                            */
/*============================================================================*/

VistaGeometry* VistaGeometryFactory::CreateFromPropertyList(const VistaPropertyList& oPropList) {
  std::string sType;
  if (oPropList.GetValue("TYPE", sType) == false) {
    vstr::warnp() << "[VistaGeometryFactory]: No type specified!" << std::endl;
    return NULL;
  }

  VistaAspectsComparisonStuff::StringCompareObject oCompare(false);

  if (oCompare(sType, "PLANE")) {
    float       fSizeX        = oPropList.GetValueOrDefault<float>("SIZEX", 1.0f);
    float       fSizeY        = oPropList.GetValueOrDefault<float>("SIZEZ", 1.0f);
    int         iResolutionX  = oPropList.GetValueOrDefault<int>("RESOLUTIONX", 1);
    int         iResolutionZ  = oPropList.GetValueOrDefault<int>("RESOLUTIONZ", 1);
    bool        bUseQuads     = oPropList.GetValueOrDefault<bool>("USE_QUADS", false);
    float       nTexCoordMinX = oPropList.GetValueOrDefault<float>("TEXCOORD_MIN_X", 0.0f);
    float       nTexCoordMaxX = oPropList.GetValueOrDefault<float>("TEXCOORD_MAX_X", 1.0f);
    float       nTexCoordMinZ = oPropList.GetValueOrDefault<float>("TEXCOORD_MIN_Z", 0.0f);
    float       nTexCoordMaxZ = oPropList.GetValueOrDefault<float>("TEXCOORD_MAX_Z", 1.0f);
    Vista::AXIS eFacing       = Vista::Y;
    std::string sFacingEntry;
    if (oPropList.GetValue<std::string>("FACING", sFacingEntry)) {
      if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sFacingEntry, "X"))
        eFacing = Vista::X;
      else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sFacingEntry, "Y"))
        eFacing = Vista::Y;
      else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sFacingEntry, "Z"))
        eFacing = Vista::Z;
      else {
        int iIndex = -1;
        if (VistaConversion::FromString(sFacingEntry, iIndex) && iIndex >= 0 && iIndex < 3) {
          eFacing = (Vista::AXIS)iIndex;
        } else {
          vstr::warnp() << "{VistaGeometryFactory::CreateFromPropertyList] -- Invealid Entry \""
                        << sFacingEntry << "\" for FACING component of plane" << std::endl;
        }
      }
    }

    VistaGeometryFactory::FaceType eFaceType =
        (bUseQuads ? VistaGeometryFactory::FT_QUAD
                   : VistaGeometryFactory::FT_QUAD_SPLIT_SYMMETRICALLY);

    VistaColor oColor = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", oColor);
    return CreatePlane(fSizeX, fSizeY, iResolutionX, iResolutionZ, eFacing, oColor, eFaceType,
        nTexCoordMinX, nTexCoordMaxX, nTexCoordMinZ, nTexCoordMaxZ);
  } else if (oCompare(sType, "BOX")) {
    float fSizeX        = oPropList.GetValueOrDefault<float>("SIZEX", 1.0f);
    float fSizeY        = oPropList.GetValueOrDefault<float>("SIZEY", 1.0f);
    float fSizeZ        = oPropList.GetValueOrDefault<float>("SIZEZ", 1.0f);
    int   iResolutionX  = oPropList.GetValueOrDefault<int>("RESOLUTIONX", 1);
    int   iResolutionY  = oPropList.GetValueOrDefault<int>("RESOLUTIONY", 1);
    int   iResolutionZ  = oPropList.GetValueOrDefault<int>("RESOLUTIONZ", 1);
    bool  bUseQuads     = oPropList.GetValueOrDefault<bool>("USE_QUADS", false);
    float nTexCoordMinX = oPropList.GetValueOrDefault<float>("TEXCOORD_MIN_X", 0.0f);
    float nTexCoordMaxX = oPropList.GetValueOrDefault<float>("TEXCOORD_MAX_X", 1.0f);
    float nTexCoordMinY = oPropList.GetValueOrDefault<float>("TEXCOORD_MIN_Y", 0.0f);
    float nTexCoordMaxY = oPropList.GetValueOrDefault<float>("TEXCOORD_MAX_Y", 1.0f);
    float nTexCoordMinZ = oPropList.GetValueOrDefault<float>("TEXCOORD_MIN_Z", 0.0f);
    float nTexCoordMaxZ = oPropList.GetValueOrDefault<float>("TEXCOORD_MAX_Z", 1.0f);

    VistaColor oColor = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", oColor);
    return CreateBox(fSizeX, fSizeY, fSizeZ, iResolutionX, iResolutionY, iResolutionZ, oColor,
        (bUseQuads ? FT_QUAD : FT_TRIANGLE), nTexCoordMinX, nTexCoordMaxX, nTexCoordMinY,
        nTexCoordMaxY, nTexCoordMinZ, nTexCoordMaxZ);
  } else if (oCompare(sType, "DISK")) {
    float       fRadius      = oPropList.GetValueOrDefault<float>("RADIUS", 0.5f);
    int         iResolutionC = oPropList.GetValueOrDefault<int>("RESOLUTIONC", 16);
    int         iResolutionD = oPropList.GetValueOrDefault<int>("RESOLUTIOND", 16);
    Vista::AXIS nNormal      = Vista::AXIS(oPropList.GetValueOrDefault<int>("NORMAL", Vista::Y));

    VistaColor color = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", color);

    return CreateDisk(fRadius, iResolutionC, iResolutionD, nNormal, color);
  } else if (oCompare(sType, "CONE")) {
    float fRadiusBottom = oPropList.GetValueOrDefault<float>("RADIUSBOTTOM", 0.5f);
    float fRadiusTop    = oPropList.GetValueOrDefault<float>("RADIUSTOP", 0.5f);
    float fHeight       = oPropList.GetValueOrDefault<float>("HEIGHT", 1.0f);
    int   fResolutionC  = oPropList.GetValueOrDefault<int>("RESOLUTIONC", 16);
    int   fResolutionD  = oPropList.GetValueOrDefault<int>("RESOLUTIOND", 1);
    int   fResolutionY  = oPropList.GetValueOrDefault<int>("RESOLUTIONY", 1);
    bool  fBottom       = oPropList.GetValueOrDefault<bool>("BOTTOM", true);
    bool  fTop          = oPropList.GetValueOrDefault<bool>("TOP", true);
    bool  fSsides       = oPropList.GetValueOrDefault<bool>("SIDES", true);

    VistaColor color = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", color);

    return CreateCone(fRadiusBottom, fRadiusTop, fHeight, fResolutionC, fResolutionD, fResolutionY,
        color, fBottom, fTop, fSsides);
  } else if (oCompare(sType, "CYLINDER")) {
    float fRadius      = oPropList.GetValueOrDefault<float>("RADIUS", 0.5f);
    float fHeight      = oPropList.GetValueOrDefault<float>("HEIGHT", 1.0f);
    int   fResolutionC = oPropList.GetValueOrDefault<int>("RESOLUTIONC", 16);
    int   fResolutionD = oPropList.GetValueOrDefault<int>("RESOLUTIOND", 1);
    int   fResolutionY = oPropList.GetValueOrDefault<int>("RESOLUTIONY", 1);
    bool  fBottom      = oPropList.GetValueOrDefault<bool>("BOTTOM", true);
    bool  fTop         = oPropList.GetValueOrDefault<bool>("TOP", true);
    bool  fSsides      = oPropList.GetValueOrDefault<bool>("SIDES", true);

    VistaColor color = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", color);

    return CreateCone(fRadius, fRadius, fHeight, fResolutionC, fResolutionD, fResolutionY, color,
        fBottom, fTop, fSsides);
  } else if (oCompare(sType, "TORUS")) {

    float ringRadius      = oPropList.GetValueOrDefault<float>("RINGRADIUS", 0.1f);
    float outerRadius     = oPropList.GetValueOrDefault<float>("OUTERRADIUS", 0.4f);
    int   resolutionSides = oPropList.GetValueOrDefault<int>("RESOLUTIONSIDES", 30);
    int   resolutionRing  = oPropList.GetValueOrDefault<int>("RESOLUTIONRING", 30);

    VistaColor color = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", color);

    return CreateTorus(ringRadius, outerRadius, resolutionSides, resolutionRing, color);

  } else if (oCompare(sType, "SPHERE")) {

    float      radius     = oPropList.GetValueOrDefault<float>("RADIUS", 0.5f);
    int        resolution = oPropList.GetValueOrDefault<int>("RESOLUTION", 32);
    VistaColor color      = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", color);

    return CreateSphere(radius, resolution, color);
  } else if (oCompare(sType, "ELLIPSOID")) {
    float radius_a       = oPropList.GetValueOrDefault<float>("RADIUS_A", 0.5f);
    float radius_b       = oPropList.GetValueOrDefault<float>("RADIUS_B", 0.5f);
    float radius_c       = oPropList.GetValueOrDefault<float>("RADIUS_C", 0.5f);
    int   thetaPrecision = oPropList.GetValueOrDefault<int>("THETARESOLUTION", 32);
    int   phiPrecision   = oPropList.GetValueOrDefault<int>("PHIRESOLUTION", 32);

    VistaColor color = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", color);

    return CreateEllipsoid(radius_a, radius_b, radius_c, thetaPrecision, phiPrecision, color);
  } else if (oCompare(sType, "TRIANGLE")) {
    VistaVector3D v3PointA =
        oPropList.GetValueOrDefault<VistaVector3D>("POINT_A", VistaVector3D(0.5f, 0.5f, 0));
    VistaVector3D v3PointB =
        oPropList.GetValueOrDefault<VistaVector3D>("POINT_B", VistaVector3D(0.5f, -0.5f, 0));
    VistaVector3D v3PointC =
        oPropList.GetValueOrDefault<VistaVector3D>("POINT_C", VistaVector3D(0, 0.5f, 0));
    int        iResolution = oPropList.GetValueOrDefault<int>("RESOLUTION", 3);
    VistaColor color       = VistaColor::WHITE;
    ReadColorFromProplist(oPropList, "COLOR", color);

    return CreateTriangle(v3PointA, v3PointB, v3PointC, iResolution, color);
  } else {
    vstr::warnp() << "[VistaGeometryFactory]: Given type [" << sType << "] not valid!" << std::endl;
    return NULL;
  }
}
