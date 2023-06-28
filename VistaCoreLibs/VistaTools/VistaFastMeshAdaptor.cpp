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

#include "VistaFastMeshAdaptor.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
const unsigned int VistaFastMeshAdaptor::BaseHandle::INVALID = ~0;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaFastMeshAdaptor::VistaFastMeshAdaptor(
    const unsigned int iNumberOfVertices, const std::vector<unsigned int>& vFaces)
    : m_vIncidentFaces(iNumberOfVertices)
    , m_vOppositeHalfedgeLinks(vFaces.size())
    , m_vFaces(vFaces) {
  unsigned int i;
  // First, create the incident faces lists.
  for (i = 0; i < m_vFaces.size(); i += 3) {
    m_vIncidentFaces[m_vFaces[i]].insert(FaceHandle(i / 3));
    m_vIncidentFaces[m_vFaces[i + 1]].insert(FaceHandle(i / 3));
    m_vIncidentFaces[m_vFaces[i + 2]].insert(FaceHandle(i / 3));
  }
  // Not create the opposite halfedge links.
  for (i = 0; i < m_vFaces.size(); i += 3) {
    m_vOppositeHalfedgeLinks[i] =
        GetHalfedgeHandle(VertexHandle(m_vFaces[i + 1]), VertexHandle(m_vFaces[i]));
    m_vOppositeHalfedgeLinks[i + 1] =
        GetHalfedgeHandle(VertexHandle(m_vFaces[i + 2]), VertexHandle(m_vFaces[i + 1]));
    m_vOppositeHalfedgeLinks[i + 2] =
        GetHalfedgeHandle(VertexHandle(m_vFaces[i]), VertexHandle(m_vFaces[i + 2]));
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

unsigned int VistaFastMeshAdaptor::NumberOfVertices() const {
  return (unsigned int)m_vIncidentFaces.size();
}

unsigned int VistaFastMeshAdaptor::NumberOfFaces() const {
  return (unsigned int)m_vFaces.size();
}
//
//
// bool
// VistaFastMeshAdaptor::IsBoundary(const VertexHandle hVertex) const
//{
//	assert(hVertex.Index() < m_vVertex.size());
//	// The vertex is part of the boundary if there is at least one
//	// outgoing halfedge with an invalid face pointer.
//	const HalfedgeHandle hStartHalfedge = m_vVertex[hVertex.Index()];
//	if (hStartHalfedge.IsValid() == false)
//	{
//		return(true);
//	}
//	HalfedgeHandle hSearchHalfedge = hStartHalfedge;
//	for (;
//		 hSearchHalfedge != hStartHalfedge;
//		 hSearchHalfedge =
//GetNextHalfedgeHandle(GetOppositeHalfedgeHandle(hSearchHalfedge)))
//	{
//		if (m_vHalfedge[hSearchHalfedge.Index()].m_hFaceHandle.IsValid() == false)
//		{
//			return(true);
//		}
//	}
//	return(false);
//}

bool VistaFastMeshAdaptor::IsBoundary(const HalfedgeHandle hHalfedge) const {
  assert((hHalfedge.Index() < m_vFaces.size()) || (hHalfedge.IsValid() == false));
  // The Halfedge indicates a boundary if it in fact does not exist.
  // This test is usually used in the context that the opposite
  // halfedge is queried and this new halfedge checked for being
  // part of the boundary.
  return hHalfedge.IsValid() == false;
}

bool VistaFastMeshAdaptor::IsBoundary(const EdgeHandle hEdge) const {
  assert(hEdge.Index() < m_vFaces.size());
  return GetOppositeHalfedgeHandle(GetHalfedgeHandle(hEdge)).IsValid() == false;
}

VistaFastMeshAdaptor::VertexHandle VistaFastMeshAdaptor::GetToVertexHandle(
    const HalfedgeHandle hHalfedge) const {
  assert(hHalfedge.Index() < m_vFaces.size());
  if (hHalfedge.Index() % 3 != 2) {
    return VertexHandle(m_vFaces[hHalfedge.Index() + 1]);
  } else {
    return VertexHandle(m_vFaces[hHalfedge.Index() - 2]);
  }
}

VistaFastMeshAdaptor::VertexHandle VistaFastMeshAdaptor::GetFromVertexHandle(
    const HalfedgeHandle hHalfedge) const {
  assert(hHalfedge.Index() < m_vFaces.size());
  return VertexHandle(m_vFaces[hHalfedge.Index()]);
}

VistaFastMeshAdaptor::HalfedgeHandle VistaFastMeshAdaptor::GetNextHalfedgeHandle(
    const HalfedgeHandle hHalfedge) const {
  assert(hHalfedge.Index() < m_vFaces.size());
  if (hHalfedge.Index() % 3 != 2) {
    return HalfedgeHandle(hHalfedge.Index() + 1);
  } else {
    return HalfedgeHandle(hHalfedge.Index() - 2);
  }
}

VistaFastMeshAdaptor::HalfedgeHandle VistaFastMeshAdaptor::GetPrevHalfedgeHandle(
    const HalfedgeHandle hHalfedge) const {
  assert(hHalfedge.Index() < m_vFaces.size());
  if (hHalfedge.Index() % 3 != 0) {
    return HalfedgeHandle(hHalfedge.Index() - 1);
  } else {
    return HalfedgeHandle(hHalfedge.Index() + 2);
  }
}

VistaFastMeshAdaptor::HalfedgeHandle VistaFastMeshAdaptor::GetOppositeHalfedgeHandle(
    const HalfedgeHandle hHalfedge) const {
  assert(hHalfedge.Index() < m_vFaces.size());
  return m_vOppositeHalfedgeLinks[hHalfedge.Index()];
}

VistaFastMeshAdaptor::HalfedgeHandle VistaFastMeshAdaptor::GetHalfedgeHandle(
    const VertexHandle hVertex) const {
  assert(hVertex.Index() < m_vIncidentFaces.size());
  if (m_vIncidentFaces[hVertex.Index()].empty() == true) {
    return HalfedgeHandle();
  }
  const unsigned int iFaceIndex = m_vIncidentFaces[hVertex.Index()].begin()->Index() * 3;
  if (m_vFaces[iFaceIndex] == hVertex.Index()) {
    return HalfedgeHandle(iFaceIndex);
  } else if (m_vFaces[iFaceIndex + 1] == hVertex.Index()) {
    return HalfedgeHandle(iFaceIndex + 1);
  } else {
    return HalfedgeHandle(iFaceIndex + 2);
  }
}

VistaFastMeshAdaptor::HalfedgeHandle VistaFastMeshAdaptor::GetHalfedgeHandle(
    const VertexHandle hFromVertex, const VertexHandle hToVertex) const {
  assert(hFromVertex.Index() < m_vIncidentFaces.size());
  assert(hToVertex.Index() < m_vIncidentFaces.size());

  std::set<FaceHandle>::const_iterator it, it_end;
  for (it    = m_vIncidentFaces[hFromVertex.Index()].begin(),
      it_end = m_vIncidentFaces[hFromVertex.Index()].end();
       it != it_end; ++it) {
    const unsigned int iFaceIndex = it->Index() * 3;
    if (m_vFaces[iFaceIndex] == hFromVertex.Index()) {
      if (m_vFaces[iFaceIndex + 1] == hToVertex.Index()) {
        return HalfedgeHandle(iFaceIndex);
      }
    } else if (m_vFaces[iFaceIndex + 1] == hFromVertex.Index()) {
      if (m_vFaces[iFaceIndex + 2] == hToVertex.Index()) {
        return HalfedgeHandle(iFaceIndex + 1);
      }
    } else // Here m_vFaces[iFaceIndex+2) should be equal to hFromVertex.Index()
    {
      if (m_vFaces[iFaceIndex] == hToVertex.Index()) {
        return HalfedgeHandle(iFaceIndex + 2);
      }
    }
  }
  return HalfedgeHandle();
}

VistaFastMeshAdaptor::HalfedgeHandle VistaFastMeshAdaptor::GetHalfedgeHandle(
    const EdgeHandle hEdge) const {
  assert(hEdge.Index() < m_vFaces.size());
  return HalfedgeHandle(hEdge.Index());
}

VistaFastMeshAdaptor::HalfedgeHandle VistaFastMeshAdaptor::GetHalfedgeHandle(
    const FaceHandle hFace) const {
  assert(hFace.Index() < m_vFaces.size());
  return HalfedgeHandle(hFace.Index() * 3);
}

VistaFastMeshAdaptor::EdgeHandle VistaFastMeshAdaptor::GetEdgeHandle(
    const HalfedgeHandle hHalfedge) const {
  assert(hHalfedge.Index() < m_vFaces.size());
  const HalfedgeHandle hOppositeHalfedge = m_vOppositeHalfedgeLinks[hHalfedge.Index()];
  if (hOppositeHalfedge.IsValid() == false) {
    return EdgeHandle(hHalfedge.Index());
  }
  if (hHalfedge.Index() < hOppositeHalfedge.Index()) {
    return EdgeHandle(hHalfedge.Index());
  } else {
    return EdgeHandle(hOppositeHalfedge.Index());
  }
}

VistaFastMeshAdaptor::FaceHandle VistaFastMeshAdaptor::GetFaceHandle(
    const HalfedgeHandle hHalfedge) const {
  // For compatibility with the VistaHalfedgeAdaptor, associate
  // an invalid face handle with an invalid halfedge handle.
  if (hHalfedge.IsValid() == false) {
    return FaceHandle();
  }
  assert(hHalfedge.Index() < m_vFaces.size());
  return FaceHandle((hHalfedge.Index() / 3));
}

VistaFastMeshAdaptor::ConstFaceIter VistaFastMeshAdaptor::FacesBegin() const {
  return ConstFaceIter(this, 0);
}

VistaFastMeshAdaptor::ConstFaceIter VistaFastMeshAdaptor::FacesEnd() const {
  return ConstFaceIter(this, (unsigned int)m_vFaces.size());
}
