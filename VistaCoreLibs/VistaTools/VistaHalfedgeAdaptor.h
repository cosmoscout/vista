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


#ifndef _VISTAHALFEDGEADAPTORGUARD_H
#define _VISTAHALFEDGEADAPTORGUARD_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

#include <vector>
#include <assert.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * \brief Constructs a halfedge-based mesh representation to facilitate
 *        topological navigation.
 *
 * Halfedge based data structures are excellent tools for fast neighborhood
 * queries and topological navigation. Consider the following example: Given a
 * standard mesh representation using vectors of triple of vertex indices to
 * represent triangle faces. Return all adjacent triangles for an arbitrary
 * triangle in the mesh requires a search through the complete list of triangles
 * and thus has a runtime complexity of O(n). Using a halfedge-based
 * representation, the same query operation can be done in O(1).
 *
 * This implementation of a halfedge-based representation is only a limited
 * adaptor and only contains a fraction of the functionality a full
 * implementation as for example OpenMesh (www.openmesh.org) offers. It is
 * designed to create a structure for a topological constant mesh (it is
 * expected that the indices in vFaces remain the same), thus no split, merge or
 * other topological operations are allowed. Note however that the adaptor has
 * no knowledge of vertex positions and is therefor suited to handle deformable
 * objects.
 *
 * Literature: S. Campagna, L. Kobbelt, H.-P. Seidel,  Directed Edges - A
 *             Scalable Representation For Triangle Meshes , ACM Journal of
 *             Graphics Tools 3 (4), 1998.
 *
 * \note The current implementation only includes a small number of possible
 *       iterators, circulators and operators out of all useful entities
 *       (i.e., an edge interator, vertex face iterator, ...).
 *       Wherever an operator is not included in this implementation, it is
 *       probably due to prior insufficient need for it rather than a
 *       deliberate design decision against it.
 */
class VISTATOOLSAPI VistaHalfedgeAdaptor
{
public:
	/**
	 * \brief Constructor.
	 *
	 * Builds the internal representation of the topology stored in the face list vFaces. It is assumed
	 * that vFaces contains a list of indices where every three successive indices represent a triangle.
	 *
	 * \param iNumberOfVertices The highest used index in vFaces incremented by one.
	 * \param vFaces A list of indices that represent a triangle topological.
	 */
	VistaHalfedgeAdaptor(const unsigned int iNumberOfVertices, const std::vector<int> &vFaces);

	// Handles
	/// Generic base for all handles.
	class BaseHandle
	{
		public:
			enum VISTATOOLSAPI
			{
				INVALID = -1
			};

			/**
			 * \brief Constructor.
			 *
			 * Note that handles are designed such that the entity they link to cannot be changed
			 * after creation except for invalidation.
			 */
			explicit BaseHandle(int iIndex = INVALID) : m_iIndex(iIndex) {}
			/// Get the index to the entity this handle is linked to.
			int Index() const
			{
				return m_iIndex;
			}
			/// Check if this handle is valid.
			bool IsValid() const
			{
				return m_iIndex != INVALID;
			}
			/// Set this handle to invalid.
			void Invalidate()
			{
				m_iIndex = INVALID;
			}
			/// Compare operator.
			bool operator==(const BaseHandle& rhs) const {
				return m_iIndex == rhs.m_iIndex;
			}
			/// Compare operator.
			bool operator!=(const BaseHandle& rhs) const {
				return m_iIndex != rhs.m_iIndex;
			}
			/// Compare operator.
			bool operator<(const BaseHandle& rhs) const {
				return m_iIndex < rhs.m_iIndex;
			}

		protected:
			/// Constant used to indicate that this handle is not linked to a valid entity.
			//static const int INVALID;
			/// The internal index that can be used to retreive the object this handle links to.
			int m_iIndex;
	};

	/// A handle for vertices, see BaseHandle.
	class VertexHandle : public BaseHandle
	{
		public:
			explicit VertexHandle(int iIndex= INVALID) : BaseHandle(iIndex) {}
	};
	/// A handle for halfedges, see BaseHandle.
	class HalfedgeHandle : public BaseHandle
	{
		public:
			explicit HalfedgeHandle( int iIndex= INVALID) : BaseHandle(iIndex) {}
	};
	/// A handle for edges, see BaseHandle.
	class EdgeHandle : public BaseHandle
	{
		public:
			explicit EdgeHandle( int iIndex= INVALID) : BaseHandle(iIndex) {}
	};
	/// A handle for faces, see BaseHandle.
	class FaceHandle : public BaseHandle
	{
		public:
			explicit FaceHandle( int iIndex= INVALID) : BaseHandle(iIndex) {}
	};


	// Iterators
	/**
	 * \brief An iterator to iterate over all faces in the representation.
	 */

	class ConstFaceIter
	{
		public:
			ConstFaceIter() : m_pHalfedgeAdaptor(0), m_iCurrentIndex( 0 )
			{}
			ConstFaceIter(const VistaHalfedgeAdaptor* const cAdaptor, const  int iIndex)
						 : m_pHalfedgeAdaptor(cAdaptor),
						   m_iCurrentIndex(iIndex)
			{
				assert(m_pHalfedgeAdaptor != 0);
				assert(m_iCurrentIndex <= m_pHalfedgeAdaptor->NumberOfFaces());
			}
			/// Copy constructor
			ConstFaceIter(const ConstFaceIter &cFaceIter)
						 : m_pHalfedgeAdaptor(cFaceIter.m_pHalfedgeAdaptor),
						   m_iCurrentIndex(cFaceIter.m_iCurrentIndex)
			{
				assert(m_iCurrentIndex <= m_pHalfedgeAdaptor->NumberOfFaces());
			}
			/// Assignment operator
			ConstFaceIter& operator=(const ConstFaceIter &cFaceIter)
			{
				m_pHalfedgeAdaptor = cFaceIter.m_pHalfedgeAdaptor;
				m_iCurrentIndex = cFaceIter.m_iCurrentIndex;
				return *this;
			}
			/// Standard dereferencing operator
			//const FaceHandle& operator*() const
			//{
			//	return(FaceHandle(i));
			//}
			///// Standard pointer operator.
			//const FaceHandle* operator->() const { return &(mesh_->deref(hnd_)); }
			FaceHandle handle() const
			{
				return FaceHandle(m_iCurrentIndex);
			}
			/// Cast to the current FaceHandle.
			operator FaceHandle() const
			{
				return FaceHandle(m_iCurrentIndex);
			}
			/// Compare operator
			bool operator==(const ConstFaceIter &cFaceIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor == cFaceIter.m_pHalfedgeAdaptor)
						&& (m_iCurrentIndex == cFaceIter.m_iCurrentIndex);
			}
			/// Compare operator
			bool operator!=(const ConstFaceIter &cFaceIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor)
						|| (m_iCurrentIndex != cFaceIter.m_iCurrentIndex);
			}
			/// Standard pre-increment operator
			ConstFaceIter& operator++()
			{
				assert(m_iCurrentIndex <= m_pHalfedgeAdaptor->NumberOfFaces());
				++m_iCurrentIndex;
				return *this;
			}
			/// Standard pre-decrement operator
			ConstFaceIter& operator--()
			{
				assert(m_iCurrentIndex > 0);
				--m_iCurrentIndex;
				return *this;
			}

		protected:
			const VistaHalfedgeAdaptor* m_pHalfedgeAdaptor;
			int m_iCurrentIndex;
	};

	// Circulators
	class ConstVertexFaceIter
	{
		public:
			ConstVertexFaceIter() : m_pHalfedgeAdaptor(0), m_bIterated( false )
			{}
			ConstVertexFaceIter(const VistaHalfedgeAdaptor* cAdaptor, const VertexHandle hVertex)
							   : m_pHalfedgeAdaptor(cAdaptor),
								 m_hStartHalfedge(cAdaptor->GetHalfedgeHandle(hVertex)),
								 m_hCurrentHalfedge(cAdaptor->GetHalfedgeHandle(hVertex)),
								 m_bIterated(false)
			{
				assert(cAdaptor != 0);
				assert(hVertex.Index() < m_pHalfedgeAdaptor->NumberOfVertices());
			}
			/// Copy constructor
			ConstVertexFaceIter(const ConstVertexFaceIter &cVertexFaceIter)
							   : m_pHalfedgeAdaptor(cVertexFaceIter.m_pHalfedgeAdaptor),
								 m_hStartHalfedge(cVertexFaceIter.m_hStartHalfedge),
								 m_hCurrentHalfedge(cVertexFaceIter.m_hCurrentHalfedge),
								 m_bIterated(cVertexFaceIter.m_bIterated)
			{}
			/// Assignment operator
			ConstVertexFaceIter& operator=(const ConstVertexFaceIter &cVertexFaceIter)
			{
				m_pHalfedgeAdaptor = cVertexFaceIter.m_pHalfedgeAdaptor;
				m_hStartHalfedge = cVertexFaceIter.m_hStartHalfedge;
				m_hCurrentHalfedge = cVertexFaceIter.m_hCurrentHalfedge;
				m_bIterated = cVertexFaceIter.m_bIterated;
				return *this;
			}
			/// Standard dereferencing operator
			//const FaceHandle& operator*() const
			//{
			//	return(FaceHandle(i));
			//}
			///// Standard pointer operator.
			//const FaceHandle* operator->() const { return &(mesh_->deref(hnd_)); }
			FaceHandle handle() const
			{
				return m_pHalfedgeAdaptor->GetFaceHandle(m_hCurrentHalfedge);
			}
			/// Cast to the current FaceHandle.
			operator FaceHandle() const
			{
				return m_pHalfedgeAdaptor->GetFaceHandle(m_hCurrentHalfedge);
			}
			/// Compare operator
			bool operator==(const ConstVertexFaceIter &cVertexFaceIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor == cVertexFaceIter.m_pHalfedgeAdaptor)
						&& (m_hStartHalfedge == cVertexFaceIter.m_hStartHalfedge)
						&& (m_hCurrentHalfedge == cVertexFaceIter.m_hCurrentHalfedge);
			}
			/// Compare operator
			bool operator!=(const ConstVertexFaceIter &cVertexFaceIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor != cVertexFaceIter.m_pHalfedgeAdaptor)
						|| (m_hStartHalfedge != cVertexFaceIter.m_hStartHalfedge)
						|| (m_hCurrentHalfedge != cVertexFaceIter.m_hCurrentHalfedge);
			}
			/// Standard pre-increment operator
			ConstVertexFaceIter& operator++()
			{
				m_bIterated = true;
				m_hCurrentHalfedge = m_pHalfedgeAdaptor->GetNextHalfedgeHandle(m_pHalfedgeAdaptor->GetOppositeHalfedgeHandle(m_hCurrentHalfedge));
				return *this;
			}
			operator bool() const
			{
				return (m_bIterated == false) || (m_hCurrentHalfedge != m_hStartHalfedge);
			}


		protected:
			/// Has the ++ operator already been used?
			bool m_bIterated;
			const VistaHalfedgeAdaptor* m_pHalfedgeAdaptor;
			HalfedgeHandle m_hStartHalfedge;
			HalfedgeHandle m_hCurrentHalfedge;
	};


	/**
	 * \brief An operator that iterates all vertices of a face.
	 */
	class ConstFaceVertexIter
	{
		public:
			ConstFaceVertexIter() : m_pHalfedgeAdaptor(0), m_bIterated( false )
			{}
			ConstFaceVertexIter(const VistaHalfedgeAdaptor* cAdaptor, const FaceHandle hFace)
							   : m_pHalfedgeAdaptor(cAdaptor),
								 m_hStartHalfedge(cAdaptor->GetHalfedgeHandle(hFace)),
								 m_hCurrentHalfedge(cAdaptor->GetHalfedgeHandle(hFace)),
								 m_bIterated(false)
			{
				assert(cAdaptor != 0);
				assert(hFace.Index() < m_pHalfedgeAdaptor->NumberOfFaces());
			}
			/// Copy constructor
			ConstFaceVertexIter(const ConstFaceVertexIter &cFaceVertexIter)
							   : m_pHalfedgeAdaptor(cFaceVertexIter.m_pHalfedgeAdaptor),
								 m_hStartHalfedge(cFaceVertexIter.m_hStartHalfedge),
								 m_hCurrentHalfedge(cFaceVertexIter.m_hCurrentHalfedge),
								 m_bIterated(cFaceVertexIter.m_bIterated)
			{}
			/// Assignment operator
			ConstFaceVertexIter& operator=(const ConstFaceVertexIter &cFaceVertexIter)
			{
				m_pHalfedgeAdaptor = cFaceVertexIter.m_pHalfedgeAdaptor;
				m_hStartHalfedge = cFaceVertexIter.m_hStartHalfedge;
				m_hCurrentHalfedge = cFaceVertexIter.m_hCurrentHalfedge;
				m_bIterated = cFaceVertexIter.m_bIterated;
				return *this;
			}
			/// Standard dereferencing operator
			//const FaceHandle& operator*() const
			//{
			//	return(FaceHandle(i));
			//}
			///// Standard pointer operator.
			//const FaceHandle* operator->() const { return &(mesh_->deref(hnd_)); }
			VertexHandle handle() const
			{
				return VertexHandle(m_pHalfedgeAdaptor->GetToVertexHandle(m_hCurrentHalfedge));
			}
			/// Cast to the current FaceHandle.
			operator VertexHandle() const
			{
				return VertexHandle(m_pHalfedgeAdaptor->GetToVertexHandle(m_hCurrentHalfedge));
			}
			/// Compare operator
			bool operator==(const ConstFaceVertexIter &cFaceVertexIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor == cFaceVertexIter.m_pHalfedgeAdaptor)
						&& (m_hStartHalfedge == cFaceVertexIter.m_hStartHalfedge)
						&& (m_hCurrentHalfedge == cFaceVertexIter.m_hCurrentHalfedge);
			}
			/// Compare operator
			bool operator!=(const ConstFaceVertexIter &cFaceVertexIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor != cFaceVertexIter.m_pHalfedgeAdaptor)
						|| (m_hStartHalfedge != cFaceVertexIter.m_hStartHalfedge)
						|| (m_hCurrentHalfedge != cFaceVertexIter.m_hCurrentHalfedge);
			}
			/// Standard pre-increment operator
			ConstFaceVertexIter& operator++()
			{
				m_bIterated = true;
				m_hCurrentHalfedge = m_pHalfedgeAdaptor->GetNextHalfedgeHandle(m_hCurrentHalfedge);
				return *this;
			}
			operator bool() const
			{
				return (m_bIterated == false) || (m_hCurrentHalfedge != m_hStartHalfedge);
			}


		protected:
			/// Has the ++ operator already been used?
			bool m_bIterated;
			const VistaHalfedgeAdaptor* m_pHalfedgeAdaptor;
			HalfedgeHandle m_hStartHalfedge;
			HalfedgeHandle m_hCurrentHalfedge;
	};

	class ConstFaceHalfedgeIter
	{
		public:
			ConstFaceHalfedgeIter() : m_pHalfedgeAdaptor(0), m_bIterated( false )
			{}
			ConstFaceHalfedgeIter(const VistaHalfedgeAdaptor* cAdaptor, const FaceHandle hFace)
							   : m_pHalfedgeAdaptor(cAdaptor),
								 m_hStartHalfedge(cAdaptor->GetHalfedgeHandle(hFace)),
								 m_hCurrentHalfedge(cAdaptor->GetHalfedgeHandle(hFace)),
								 m_bIterated(false)
			{
				assert(cAdaptor != 0);
				assert(hFace.Index() < m_pHalfedgeAdaptor->NumberOfFaces());
			}
			/// Copy constructor
			ConstFaceHalfedgeIter(const ConstFaceHalfedgeIter &cFaceHalfedgeIter)
							   : m_pHalfedgeAdaptor(cFaceHalfedgeIter.m_pHalfedgeAdaptor),
								 m_hStartHalfedge(cFaceHalfedgeIter.m_hStartHalfedge),
								 m_hCurrentHalfedge(cFaceHalfedgeIter.m_hCurrentHalfedge),
								 m_bIterated(cFaceHalfedgeIter.m_bIterated)
			{}
			/// Assignment operator
			ConstFaceHalfedgeIter& operator=(const ConstFaceHalfedgeIter &cFaceHalfedgeIter)
			{
				m_pHalfedgeAdaptor = cFaceHalfedgeIter.m_pHalfedgeAdaptor;
				m_hStartHalfedge = cFaceHalfedgeIter.m_hStartHalfedge;
				m_hCurrentHalfedge = cFaceHalfedgeIter.m_hCurrentHalfedge;
				m_bIterated = cFaceHalfedgeIter.m_bIterated;
				return *this;
			}
			/// Standard dereferencing operator
			//const FaceHandle& operator*() const
			//{
			//	return(FaceHandle(i));
			//}
			///// Standard pointer operator.
			//const FaceHandle* operator->() const { return &(mesh_->deref(hnd_)); }
			HalfedgeHandle handle() const
			{
				return m_hCurrentHalfedge;
			}
			/// Cast to the current FaceHandle.
			operator HalfedgeHandle() const
			{
				return m_hCurrentHalfedge;
			}
			/// Compare operator
			bool operator==(const ConstFaceHalfedgeIter &cFaceHalfedgeIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor == cFaceHalfedgeIter.m_pHalfedgeAdaptor)
						&& (m_hStartHalfedge == cFaceHalfedgeIter.m_hStartHalfedge)
						&& (m_hCurrentHalfedge == cFaceHalfedgeIter.m_hCurrentHalfedge);
			}
			/// Compare operator
			bool operator!=(const ConstFaceHalfedgeIter &cFaceHalfedgeIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor != cFaceHalfedgeIter.m_pHalfedgeAdaptor)
						|| (m_hStartHalfedge != cFaceHalfedgeIter.m_hStartHalfedge)
						|| (m_hCurrentHalfedge != cFaceHalfedgeIter.m_hCurrentHalfedge);
			}
			/// Standard pre-increment operator
			ConstFaceHalfedgeIter& operator++()
			{
				m_bIterated = true;
				m_hCurrentHalfedge = m_pHalfedgeAdaptor->GetNextHalfedgeHandle(m_hCurrentHalfedge);
				return *this;
			}
			operator bool() const
			{
				return (m_bIterated == false) || (m_hCurrentHalfedge != m_hStartHalfedge);
			}


		protected:
			/// Has the ++ operator already been used?
			bool m_bIterated;
			const VistaHalfedgeAdaptor* m_pHalfedgeAdaptor;
			HalfedgeHandle m_hStartHalfedge;
			HalfedgeHandle m_hCurrentHalfedge;
	};

	// ConstFaceEdgeIter
	/**
	 * \brief An operator that iterates all edges in a face.
	 *
	 */
	class ConstFaceEdgeIter
	{
		public:
			ConstFaceEdgeIter() : m_pHalfedgeAdaptor(0), m_bIterated( false )
			{}
			ConstFaceEdgeIter(const VistaHalfedgeAdaptor* const pAdaptor, const FaceHandle hFace)
							   : m_pHalfedgeAdaptor(pAdaptor),
								 m_hStartHalfedge(pAdaptor->GetHalfedgeHandle(hFace)),
								 m_hCurrentHalfedge(pAdaptor->GetHalfedgeHandle(hFace)),
								 m_bIterated(false)
			{
				assert(pAdaptor != 0);
				assert(hFace.Index() < m_pHalfedgeAdaptor->NumberOfFaces());
			}
			/// Copy constructor
			ConstFaceEdgeIter(const ConstFaceEdgeIter &cFaceEdgeIter)
							   : m_pHalfedgeAdaptor(cFaceEdgeIter.m_pHalfedgeAdaptor),
								 m_hStartHalfedge(cFaceEdgeIter.m_hStartHalfedge),
								 m_hCurrentHalfedge(cFaceEdgeIter.m_hCurrentHalfedge),
								 m_bIterated(cFaceEdgeIter.m_bIterated)
			{}
			/// Assignment operator
			ConstFaceEdgeIter& operator=(const ConstFaceEdgeIter &cFaceEdgeIter)
			{
				m_pHalfedgeAdaptor = cFaceEdgeIter.m_pHalfedgeAdaptor;
				m_hStartHalfedge = cFaceEdgeIter.m_hStartHalfedge;
				m_hCurrentHalfedge = cFaceEdgeIter.m_hCurrentHalfedge;
				m_bIterated = cFaceEdgeIter.m_bIterated;
				return *this;
			}
			/// Standard dereferencing operator
			//const FaceHandle& operator*() const
			//{
			//	return(FaceHandle(i));
			//}
			///// Standard pointer operator.
			//const FaceHandle* operator->() const { return &(mesh_->deref(hnd_)); }
			EdgeHandle handle() const
			{
				return m_pHalfedgeAdaptor->GetEdgeHandle(m_hCurrentHalfedge);
			}
			/// Cast to the current FaceHandle.
			operator EdgeHandle() const
			{
				return m_pHalfedgeAdaptor->GetEdgeHandle(m_hCurrentHalfedge);
			}
			/// Compare operator
			bool operator==(const ConstFaceEdgeIter &cFaceEdgeIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor == cFaceEdgeIter.m_pHalfedgeAdaptor)
						&& (m_hStartHalfedge == cFaceEdgeIter.m_hStartHalfedge)
						&& (m_hCurrentHalfedge == cFaceEdgeIter.m_hCurrentHalfedge);
			}
			/// Compare operator
			bool operator!=(const ConstFaceEdgeIter &cFaceEdgeIter) const
			{
				//assert((m_pHalfedgeAdaptor != cFaceIter.m_pHalfedgeAdaptor));
				return (m_pHalfedgeAdaptor != cFaceEdgeIter.m_pHalfedgeAdaptor)
						|| (m_hStartHalfedge != cFaceEdgeIter.m_hStartHalfedge)
						|| (m_hCurrentHalfedge != cFaceEdgeIter.m_hCurrentHalfedge);
			}
			/// Standard pre-increment operator
			ConstFaceEdgeIter& operator++()
			{
				m_bIterated = true;
				m_hCurrentHalfedge = m_pHalfedgeAdaptor->GetNextHalfedgeHandle(m_hCurrentHalfedge);
				return *this;
			}
			operator bool() const
			{
				return (m_bIterated == false) || (m_hCurrentHalfedge != m_hStartHalfedge);
			}


		protected:
			/// Has the ++ operator already been used?
			bool m_bIterated;
			const VistaHalfedgeAdaptor* m_pHalfedgeAdaptor;
			HalfedgeHandle m_hStartHalfedge;
			HalfedgeHandle m_hCurrentHalfedge;
	};

	/// Get a handle to the vertex the specified halfedge points to.
	VertexHandle	GetToVertexHandle(const HalfedgeHandle hHalfedge) const;
	/// Get a handle to the vertex the specified halfedge originates from.
	VertexHandle	GetFromVertexHandle(const HalfedgeHandle hHalfedge) const;
	/// Get a handle to the next halfedge with respect to the specified one.
	HalfedgeHandle	GetNextHalfedgeHandle(const HalfedgeHandle hHalfedge) const;
	/// Get a handle to the halfedge which next is linked to the specified halfedge.
	HalfedgeHandle	GetPrevHalfedgeHandle(const HalfedgeHandle hHalfedge) const;
	/// Get a handle to the halfedge opposite to the specified one.
	HalfedgeHandle	GetOppositeHalfedgeHandle(const HalfedgeHandle hHalfedge) const;
	/// Get a handle to one of the halfedges that orignates at hVertex.
	HalfedgeHandle	GetHalfedgeHandle(const VertexHandle hVertex) const;
	/// Search for a halfedge that originates at hFromVertex and points to hToVertex.
	HalfedgeHandle	GetHalfedgeHandle(const VertexHandle hFromVertex, const VertexHandle hToVertex) const;
	/// Get a handle to one of the halfedges that belong to this edge.
	HalfedgeHandle	GetHalfedgeHandle(const EdgeHandle hEdge) const;
	/// Get a handle to one of the halfedges that belong to this face.
	HalfedgeHandle	GetHalfedgeHandle(const FaceHandle hFace) const;
	/// Create an edge handle from a halfedge.
	EdgeHandle		GetEdgeHandle(const HalfedgeHandle hHalfedge) const;
	/// Get a handle to the face that this halfedge belongs to, INVALID if halfedge is on a boundary.
	FaceHandle		GetFaceHandle(const HalfedgeHandle hHalfedge) const;

	/// Get the number of vertices in the topology.
	int	NumberOfVertices() const;
	/// Get the number of faces in the topology.
	int	NumberOfFaces() const;
	/// Get the number of halfedges in the topology.
	int	NumberOfHalfedges() const;

	/// Check if the vertex lies on a boundary (has an outgoing halfedge that lies on a boundary).
	bool			IsBoundary(const VertexHandle hVertex) const;
	/// Check if the halfedge lies on a boundary (does not belong to a face).
	bool			IsBoundary(const HalfedgeHandle hHalfedge) const;
	/// Check if the edge lies on a boundary (not between two faces).
	bool			IsBoundary(const EdgeHandle hEdge) const;

	/// Get a const face iterator to the first face.
	ConstFaceIter	FacesBegin() const;
	/// Gat a const face iterator to the space after the last face.
	ConstFaceIter	FacesEnd() const;

protected:
	/// Internal structure to handle all links of a halfedge.
	struct Halfedge
	{
		/// The vertex the halfedge points to.
		VertexHandle m_hToVertex;
		/// The link to the next halfedge.
		HalfedgeHandle m_hNextHalfedge;
		/// The face the halfedge belongs to, INVALID if boundary halfedge.
		FaceHandle m_hFaceHandle;
	};

	/// List of Vertices, each vertex storing an outgoing halfedge.
	std::vector<HalfedgeHandle> m_vVertex;
	/// List of Halfedges, each halfedge storing a to vertex, next halfedge and face.
	std::vector<Halfedge> m_vHalfedge;
	/// List of Faces, each face storing one member halfedge.
	std::vector<HalfedgeHandle> m_vFace;
private:

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAHALFEDGEADAPTORGUARD_H
