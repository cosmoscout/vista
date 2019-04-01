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


#ifndef CVISTAOCTREE_H
#define CVISTAOCTREE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

#include <vector>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// this defines are used as child IDs
#define		ot_BackBottomLeft		0
#define		ot_BackBottomRight		1
#define		ot_BackTopLeft			2
#define		ot_BackTopRight			3
#define		ot_FrontBottomLeft		4
#define		ot_FrontBottomRight		5
#define		ot_FrontTopLeft			6
#define		ot_FrontTopRight		7

// this defines are used as directions (e.g. in LocateNeighbor)
#define		ot_dirLeft				1
#define		ot_dirRight				2
#define		ot_dirBottom			4
#define		ot_dirTop				8
#define		ot_dirBack				16
#define		ot_dirFront				32

#define		ot_dirBottomLeft		ot_dirBottom + ot_dirLeft
#define		ot_dirBottomRight		ot_dirBottom + ot_dirRight
#define		ot_dirTopLeft			ot_dirTop + ot_dirLeft
#define		ot_dirTopRight			ot_dirTop + ot_dirRight

#define		ot_dirBackLeft			ot_dirBack + ot_dirLeft
#define		ot_dirBackRight			ot_dirBack + ot_dirRight
#define		ot_dirFrontLeft			ot_dirFront + ot_dirLeft
#define		ot_dirFrontRight		ot_dirFront + ot_dirRight

#define		ot_dirBackBottomLeft	ot_dirBack + ot_dirBottom + ot_dirLeft
#define		ot_dirBackBottomRight	ot_dirBack + ot_dirBottom + ot_dirRight
#define		ot_dirBackTopLeft		ot_dirBack + ot_dirTop + ot_dirLeft
#define		ot_dirBackTopRight		ot_dirBack + ot_dirTop + ot_dirRight

#define		ot_dirFrontBottomLeft	ot_dirFront + ot_dirBottom + ot_dirLeft
#define		ot_dirFrontBottomRight	ot_dirFront + ot_dirBottom + ot_dirRight
#define		ot_dirFrontTopLeft		ot_dirFront + ot_dirTop + ot_dirLeft
#define		ot_dirFrontTopRight		ot_dirFront + ot_dirTop + ot_dirRight

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaOctreeNode;
class VistaOctree;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTATOOLSAPI VistaOctreeNode {

friend class VistaOctree;

public:
	virtual ~VistaOctreeNode();

	inline bool IsLeaf() const
	{ return (m_children.size() == 0);}

	inline int GetLevel() const
	{ return m_level;}

	inline VistaOctreeNode* GetParent() const
	{ return m_pParent;}

	inline VistaOctreeNode* GetChild(const int idx) const
	{ return m_children[idx];}

protected:
	// specify the parent of the new node and which child has
	// to be created (needed for the determination of locCodes)
	VistaOctreeNode(VistaOctreeNode* pParent, const int i);

	virtual void Serialize(std::ostream &) const {};
	virtual void DeSerialize(std::istream &) {};

private:
	static	int	GetMaxLevel() { return otRootLevel;};

protected:
	// each node stores the x,y,z locational codes and level
	// the locational codes define the position of the node
	// the level defines the size of the node
	// root node has the highest level
	unsigned short		m_xLocCode, m_yLocCode, m_zLocCode;
	unsigned char		m_level;

private:
	// maximal thoretical level value = (8*sizeof(short)-1) = usually 15
	// therefore the maximal theoretical octree resoluton is (32768)^3
	static	int		otRootLevel;
	// pointer to a parent node
	VistaOctreeNode				*m_pParent;
	// the children - the number of children is either 0 or 8
	std::vector<VistaOctreeNode*>		m_children;
};

class VISTATOOLSAPI VistaOctree {

public:
	virtual void Serialize(std::ostream &) const;
	virtual void DeSerialize(std::istream &);

protected:
	VistaOctree();
	virtual ~VistaOctree();

	/**********************************************************************************/
	// overwrite these methods
	/**********************************************************************************/

	// subclass must overwrite this method and create the required octree node type
	// this method is called by VistaOctree::Split to create new children
	virtual VistaOctreeNode* NewNode (VistaOctreeNode* pParent, const int i) =0;

	// creates new children, initializes their level and locCodes
	// subclass must initialize the childrens' data after calling VistaOctree::Split
	virtual bool Split (VistaOctreeNode&);

	// deletes children
	// subclass must initialize the parent's data before calling VistaOctree::Reduce
	virtual bool Reduce (VistaOctreeNode&);

	/**********************************************************************************/
	// overwrite the above methods
	/**********************************************************************************/

	// returns the node position (xmin, ymin, zmin) assuming the root size = (1.0)^3
	void GetNodePosition(const VistaOctreeNode&, float& x, float& y, float& z) const;

	// gives back the bounding box of the node "node" via minimum point (x, y, z) and its size
	void GetNodeBoundingBox (const VistaOctreeNode& node, float& x, float& y, float& z, float& size) const;

	// returns the node size, assuming the root size = (1.0)^3
	inline float GetNodeSize(const VistaOctreeNode& node) const
	{ return 1.0f/(float)( 1 << (m_pRoot->m_level - node.m_level));}

	// converts locational code to position
	inline float LocCodeToPosition(int locCode) const
	{ return locCode / (float) (1 << m_pRoot->m_level);}

	// converts position to locational code
	// position is assumed to be within <0.0, 1.0>
	inline int PositionToLocCode(float pos) const
	{ return (int) (pos * (1 << m_pRoot->m_level));}

	// returns the maximal root level possible
	inline int GetMaxLevel() const
	{	return VistaOctreeNode::GetMaxLevel();}

	// returns the current root level
	inline int GetRootLevel() const
	{ return m_pRoot->m_level;}

	// sets the current root level
	// this method has to be called before any children exist
	inline bool SetRootLevel(const int level)
	{ if(level>GetMaxLevel()) return false;
	  else m_pRoot->m_level = level; return true; }

	// returns a leaf node according to the xyzLocCodes
	// if start is NULL, traversal starts at root
	VistaOctreeNode* Traverse (const int xLocCode,
								const int yLocCode,
								const int zLocCode,
								VistaOctreeNode* start = NULL) const;

	VistaOctreeNode *LocateRegion (float const min[3], float const max[3]);

	VistaOctreeNode* TraverseToLevel (	const int xLocCode,
										const int yLocCode,
										const int zLocCode,
										const int level,
										VistaOctreeNode* start = NULL) const;

	VistaOctreeNode* GetCommonAncestor (VistaOctreeNode &node, int &nodeLevel, int diff) const;

	VistaOctreeNode*	LocateLeftNeighbor (VistaOctreeNode&) const;
	VistaOctreeNode*	LocateRightNeighbor (VistaOctreeNode&) const;
	VistaOctreeNode*	LocateBottomNeighbor (VistaOctreeNode&) const;
	VistaOctreeNode*	LocateTopNeighbor (VistaOctreeNode&) const;
	VistaOctreeNode*	LocateFrontNeighbor (VistaOctreeNode&) const;
	VistaOctreeNode*	LocateBackNeighbor (VistaOctreeNode&) const;

	VistaOctreeNode*	LocateNeighbor (VistaOctreeNode&, const int direction) const;

protected:
	VistaOctreeNode	*m_pRoot;

};

#endif // CVISTAOCTREE_H
