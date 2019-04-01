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


#include "VistaHalfedgeAdaptor.h" 


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
//const int VistaHalfedgeAdaptor::BaseHandle::INVALID = -1;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaHalfedgeAdaptor::VistaHalfedgeAdaptor(const unsigned int iNumberOfVertices,
											 const std::vector<int> &vFaces)
: m_vVertex(iNumberOfVertices), m_vFace(vFaces.size()/3), m_vHalfedge(0)
{
	// The halfedge vector cannot be resized as the number of actually
	// required halfedges depends on both the number of faces and the 
	// size of a possibly existing boundary. However, as most used
	// inputs usually represent closed objects, reserve enough space for
	// this case (3 halfedge for every face, which relates to vFaces.size()
	// / 3 * 3).
	m_vHalfedge.reserve(vFaces.size());
	
	// Iterate over all faces in vFaces.
	// For every face, create necessary halfedges is they do
	// not already exist or correctly relink existing ones.
	std::vector<int>::const_iterator f_it, f_it_end;
	unsigned int iCurrentIndex=0;
	for (f_it = vFaces.begin(), f_it_end = vFaces.end();
		 f_it != f_it_end;
		 f_it += 3, ++iCurrentIndex)
	{
		const VertexHandle hVertexA(*f_it), hVertexB(*(f_it+1)), hVertexC(*(f_it+2));
		
		// Check if the halfedge required for this triangle already exits. If
		// this is the case, lookup the handles for them and otherwise create
		// missing halfedges.
		HalfedgeHandle hHalfedgeAB = GetHalfedgeHandle(hVertexA, hVertexB);
		if (hHalfedgeAB.IsValid() == false)
		{
			hHalfedgeAB = HalfedgeHandle((int)m_vHalfedge.size());
			Halfedge cNewHalfedge;
			cNewHalfedge.m_hToVertex = hVertexB;
			m_vHalfedge.push_back(cNewHalfedge);
			cNewHalfedge.m_hToVertex = hVertexA;
			m_vHalfedge.push_back(cNewHalfedge);
		}
		HalfedgeHandle hHalfedgeBC = GetHalfedgeHandle(hVertexB, hVertexC);
		if (hHalfedgeBC.IsValid() == false)
		{
			hHalfedgeBC = HalfedgeHandle((int)m_vHalfedge.size());
			Halfedge cNewHalfedge;
			cNewHalfedge.m_hToVertex = hVertexC;
			m_vHalfedge.push_back(cNewHalfedge);
			cNewHalfedge.m_hToVertex = hVertexB;
			m_vHalfedge.push_back(cNewHalfedge);
		}
		HalfedgeHandle hHalfedgeCA = GetHalfedgeHandle(hVertexC, hVertexA);
		if (hHalfedgeCA.IsValid() == false)
		{
			hHalfedgeCA = HalfedgeHandle((int)m_vHalfedge.size());
			Halfedge cNewHalfedge;
			cNewHalfedge.m_hToVertex = hVertexA;
			m_vHalfedge.push_back(cNewHalfedge);
			cNewHalfedge.m_hToVertex = hVertexC;
			m_vHalfedge.push_back(cNewHalfedge);
		}


		// Check for necessary re-linking.
		// Make sure that the inner-triangle next links are correct if they
		// have previously existed. A repair may be necessary if a triangle
		// has previously been inserted in the wrong gap.
		unsigned int i;
		for (i=0; i<3; ++i)
		{
			HalfedgeHandle hPrev;
			HalfedgeHandle hNext;
			switch (i)
			{
				case 0:
				{
					hPrev = hHalfedgeCA;
					hNext = hHalfedgeAB;
				} break;
				case 1:
				{
					hPrev = hHalfedgeAB;
					hNext = hHalfedgeBC;
				} break;
				case 2:
				{
					hPrev = hHalfedgeBC;
					hNext = hHalfedgeCA;
				} break;
			}
			if ((GetFaceHandle(GetOppositeHalfedgeHandle(hNext)).IsValid() == true)
			 && (GetFaceHandle(GetOppositeHalfedgeHandle(hPrev)).IsValid() == true))
			{
				if (GetNextHalfedgeHandle(hPrev) != hNext)
				{
					// The Patch to be relinked starts at halfedge A->B and ends before the
					// next boundary.
					// Gap I is the gap around vertex A that starts at halfedge C->A (which
					// is a boundary as we are just inserting the face there) and ends at
					// the next halfedge of C->A.
					// Gap II is that gap starting before the patch and ending after the patch.
					const HalfedgeHandle hGapOneEnd = GetNextHalfedgeHandle(hPrev);
					const HalfedgeHandle hGapTwoBegin = GetPrevHalfedgeHandle(hNext);
					HalfedgeHandle hPatchEnd = hNext;
					while (GetFaceHandle(hPatchEnd).IsValid())
					{
						hPatchEnd = GetNextHalfedgeHandle(GetOppositeHalfedgeHandle(hPatchEnd));
					}
					const HalfedgeHandle hGapTwoEnd = GetNextHalfedgeHandle(hPatchEnd);
					m_vHalfedge[hPrev.Index()].m_hNextHalfedge = hNext;
					m_vHalfedge[GetOppositeHalfedgeHandle(hPatchEnd).Index()].m_hNextHalfedge = hGapOneEnd;
					m_vHalfedge[hGapTwoBegin.Index()].m_hNextHalfedge = hGapTwoEnd;
				}
			}
		}

		// Now that the rest of the topology is compatible,
		// update all necessary links.
		// Set outer-triangle next halfedge links.
		typedef std::pair<HalfedgeHandle, HalfedgeHandle>  NextCacheEntry;
		typedef std::vector<NextCacheEntry>                NextCache;
		NextCache    vNextHalfedgeCache(0);
		vNextHalfedgeCache.reserve(6);

		for (i=0; i<3; ++i)
		{
			HalfedgeHandle hCurrent;
			HalfedgeHandle hNext;
			switch (i)
			{
				case 0:
				{
					hCurrent = GetOppositeHalfedgeHandle(hHalfedgeAB);
					hNext = GetOppositeHalfedgeHandle(hHalfedgeCA);
				} break;
				case 1:
				{
					hCurrent = GetOppositeHalfedgeHandle(hHalfedgeBC);
					hNext = GetOppositeHalfedgeHandle(hHalfedgeAB);
				} break;
				case 2:
				{
					hCurrent = GetOppositeHalfedgeHandle(hHalfedgeCA);
					hNext = GetOppositeHalfedgeHandle(hHalfedgeBC);
				} break;
			}
			unsigned int iCaseId = 0;
			if (GetFaceHandle(hCurrent).IsValid() == false)
			{
				iCaseId |= 1;
			}
			if (GetFaceHandle(hNext).IsValid() == false)
			{
				iCaseId |= 2;
			}
			switch(iCaseId)
			{
				case 0: 
				{
					// Both edges already existed, so the outer next link should
					// already be correctly set from previous face insertions.
				} break;
				case 1:
				{
					// The current edge is new but the next edge is not.
					const HalfedgeHandle hBoundaryNext = GetNextHalfedgeHandle(GetOppositeHalfedgeHandle(hNext));
					vNextHalfedgeCache.push_back(NextCacheEntry(hCurrent, hBoundaryNext));
				} break;
				case 2:
				{
					// The current edge already existed, however, the next edge is new.
					const HalfedgeHandle hBoundaryPrev = GetPrevHalfedgeHandle(GetOppositeHalfedgeHandle(hCurrent));
					vNextHalfedgeCache.push_back(NextCacheEntry(hBoundaryPrev, hNext));
				} break;
				case 3:
				{
					// Both edges are new.
					// Check if the current face is the first connection to the vertex
					// between the two halfedges.
					if (GetHalfedgeHandle(GetToVertexHandle(hCurrent)).IsValid() == false)
					{
						// There is no other face connected to the crictical vertex and
						// the next link, just goes from the current to the next edge.
						vNextHalfedgeCache.push_back(NextCacheEntry(hCurrent, hNext));
					}
					else
					{
						// There is at least one face already incident to the critical vertex
						// but still both edges are new. Find a gap to insert this face.
						HalfedgeHandle hGapBegin = GetOppositeHalfedgeHandle(GetHalfedgeHandle(GetToVertexHandle(hCurrent)));
						while (GetFaceHandle(hGapBegin).IsValid() == true)
						{
							hGapBegin = GetOppositeHalfedgeHandle(GetNextHalfedgeHandle(hGapBegin));
						}
						HalfedgeHandle hGapEnd = GetNextHalfedgeHandle(hGapBegin);
						while (GetFaceHandle(hGapEnd).IsValid() == true)
						{
							hGapEnd = GetNextHalfedgeHandle(GetOppositeHalfedgeHandle(hGapEnd));
						}
						// Insert the face in the gap.
						vNextHalfedgeCache.push_back(NextCacheEntry(hCurrent, hGapEnd));
						vNextHalfedgeCache.push_back(NextCacheEntry(hGapBegin, hNext));
					}
				} break;
			}
		}
		// Set all halfedge that have been cached.
		NextCache::const_iterator it, it_end;
		for (it = vNextHalfedgeCache.begin(), it_end = vNextHalfedgeCache.end();
			 it != it_end;
			 ++it)
		{
			m_vHalfedge[(*it).first.Index()].m_hNextHalfedge = (*it).second;
		}

		// Set inner-triangle next halfedge links.
		m_vHalfedge[hHalfedgeAB.Index()].m_hNextHalfedge = hHalfedgeBC;
		m_vHalfedge[hHalfedgeBC.Index()].m_hNextHalfedge = hHalfedgeCA;
		m_vHalfedge[hHalfedgeCA.Index()].m_hNextHalfedge = hHalfedgeAB;
		// Set Face Handles for innter-triangle halfedge.
		const FaceHandle hCurrentFace(iCurrentIndex);
		m_vHalfedge[hHalfedgeAB.Index()].m_hFaceHandle = hCurrentFace;
		m_vHalfedge[hHalfedgeBC.Index()].m_hFaceHandle = hCurrentFace;
		m_vHalfedge[hHalfedgeCA.Index()].m_hFaceHandle = hCurrentFace;
		// Update halfedge pointers for all vertices.
		m_vVertex[hVertexA.Index()] = hHalfedgeAB;
		m_vVertex[hVertexB.Index()] = hHalfedgeBC;
		m_vVertex[hVertexC.Index()] = hHalfedgeCA;
		// Set halfedge pointer of the face.
		m_vFace[hCurrentFace.Index()] = hHalfedgeAB;
	}
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


int
VistaHalfedgeAdaptor::NumberOfVertices() const
{
	return (int)m_vVertex.size();
}

int
VistaHalfedgeAdaptor::NumberOfFaces() const
{
	return (int)m_vFace.size();
}


int VistaHalfedgeAdaptor::NumberOfHalfedges() const
{
	return (int)m_vHalfedge.size();
}

bool
VistaHalfedgeAdaptor::IsBoundary(const VertexHandle hVertex) const
{
	assert(hVertex.Index() < (int)m_vVertex.size());
	// The vertex is part of the boundary if there is at least one
	// outgoing halfedge with an invalid face pointer.
	const HalfedgeHandle hStartHalfedge = m_vVertex[hVertex.Index()];
	if (hStartHalfedge.IsValid() == false)
	{
		return true;
	}
	HalfedgeHandle hSearchHalfedge = hStartHalfedge;
	for (;
		 hSearchHalfedge != hStartHalfedge;
		 hSearchHalfedge = GetNextHalfedgeHandle(GetOppositeHalfedgeHandle(hSearchHalfedge)))
	{
		if (m_vHalfedge[hSearchHalfedge.Index()].m_hFaceHandle.IsValid() == false)
		{
			return true;
		}
	}
	return false;
}


bool
VistaHalfedgeAdaptor::IsBoundary(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());
	return GetFaceHandle(hHalfedge).IsValid() == false;
}

bool
VistaHalfedgeAdaptor::IsBoundary(const EdgeHandle hEdge) const
{
	assert((hEdge.Index() << 1) < (int)m_vHalfedge.size());
	FaceHandle test1 = GetFaceHandle(GetHalfedgeHandle(hEdge));
	FaceHandle test2 = GetFaceHandle(GetOppositeHalfedgeHandle(GetHalfedgeHandle(hEdge)));
	return( test1.IsValid() == false || test2.IsValid() == false );
}


VistaHalfedgeAdaptor::VertexHandle
VistaHalfedgeAdaptor::GetToVertexHandle(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());
	return m_vHalfedge[hHalfedge.Index()].m_hToVertex;
}

VistaHalfedgeAdaptor::VertexHandle
VistaHalfedgeAdaptor::GetFromVertexHandle(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());
	return m_vHalfedge[GetOppositeHalfedgeHandle(hHalfedge).Index()].m_hToVertex;
}

VistaHalfedgeAdaptor::HalfedgeHandle
VistaHalfedgeAdaptor::GetNextHalfedgeHandle(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());
	return m_vHalfedge[hHalfedge.Index()].m_hNextHalfedge;
}

VistaHalfedgeAdaptor::HalfedgeHandle
VistaHalfedgeAdaptor::GetPrevHalfedgeHandle(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());
	HalfedgeHandle hResult = GetOppositeHalfedgeHandle(hHalfedge);
	while (GetNextHalfedgeHandle(hResult) != hHalfedge)
	{
		hResult = GetOppositeHalfedgeHandle(GetNextHalfedgeHandle(hResult));
	}
	return hResult;
}

VistaHalfedgeAdaptor::HalfedgeHandle
VistaHalfedgeAdaptor::GetOppositeHalfedgeHandle(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());
	if ((hHalfedge.Index() & 1) == 1)
	{
		return HalfedgeHandle(hHalfedge.Index()-1);
	}
	else
	{
		return HalfedgeHandle(hHalfedge.Index()+1);
	}
}

VistaHalfedgeAdaptor::HalfedgeHandle
VistaHalfedgeAdaptor::GetHalfedgeHandle(const VertexHandle hVertex) const
{
	assert(hVertex.Index() < (int)m_vVertex.size());
	return m_vVertex[hVertex.Index()];
}
	

VistaHalfedgeAdaptor::HalfedgeHandle
VistaHalfedgeAdaptor::GetHalfedgeHandle(const VertexHandle hFromVertex, const VertexHandle hToVertex) const
{
	assert(hFromVertex.Index() <(int) m_vVertex.size());
	assert(hToVertex.Index() < (int)m_vVertex.size());

	const HalfedgeHandle hStartHalfedge = m_vVertex[hFromVertex.Index()];
	if (hStartHalfedge.IsValid() == false)
	{
		return HalfedgeHandle();
	}
	HalfedgeHandle hSearchHalfedge = hStartHalfedge;
	do
	{
		if (m_vHalfedge[hSearchHalfedge.Index()].m_hToVertex == hToVertex)
		{
			return hSearchHalfedge;
		}
		hSearchHalfedge = GetNextHalfedgeHandle(GetOppositeHalfedgeHandle(hSearchHalfedge));
	} while (hSearchHalfedge != hStartHalfedge);
	return HalfedgeHandle();
}

VistaHalfedgeAdaptor::HalfedgeHandle
VistaHalfedgeAdaptor::GetHalfedgeHandle(const EdgeHandle hEdge) const
{
	assert((hEdge.Index() << 1) < (int)m_vHalfedge.size());

	return HalfedgeHandle(hEdge.Index() << 1);
}

VistaHalfedgeAdaptor::HalfedgeHandle
VistaHalfedgeAdaptor::GetHalfedgeHandle(const FaceHandle hFace) const
{
	assert(hFace.Index() < (int)m_vFace.size());

	return m_vFace[hFace.Index()];
}


VistaHalfedgeAdaptor::EdgeHandle
VistaHalfedgeAdaptor::GetEdgeHandle(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());

	return EdgeHandle(hHalfedge.Index() >> 1);
}

VistaHalfedgeAdaptor::FaceHandle
VistaHalfedgeAdaptor::GetFaceHandle(const HalfedgeHandle hHalfedge) const
{
	assert(hHalfedge.Index() < (int)m_vHalfedge.size());

	return m_vHalfedge[hHalfedge.Index()].m_hFaceHandle;
}

VistaHalfedgeAdaptor::ConstFaceIter
VistaHalfedgeAdaptor::FacesBegin() const
{
	return ConstFaceIter(this, 0);
}

VistaHalfedgeAdaptor::ConstFaceIter
VistaHalfedgeAdaptor::FacesEnd() const
{
	return ConstFaceIter(this, (unsigned int)m_vFace.size());
}

