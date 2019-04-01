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


#include <list>
#include "VistaOctree.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
using namespace std;

int VistaOctreeNode::otRootLevel  = 8*sizeof(short)-1;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaOctreeNode::VistaOctreeNode(VistaOctreeNode* pParent, const int i)
: m_pParent(pParent)
{
	if (m_pParent == NULL)	// root node
	{
		m_level = otRootLevel;

		m_xLocCode = m_yLocCode = m_zLocCode = 0;
	}
	else
	{
		m_level = m_pParent->m_level - 1;

		switch(i)
		{
		case ot_BackBottomLeft:
			m_xLocCode = m_pParent->m_xLocCode;
			m_yLocCode = m_pParent->m_yLocCode;
			m_zLocCode = m_pParent->m_zLocCode;
			break;
		case ot_BackBottomRight:
			m_xLocCode = m_pParent->m_xLocCode + (1 << m_level);
			m_yLocCode = m_pParent->m_yLocCode;
			m_zLocCode = m_pParent->m_zLocCode;
			break;
		case ot_BackTopLeft:
			m_xLocCode = m_pParent->m_xLocCode;
			m_yLocCode = m_pParent->m_yLocCode + (1 << m_level);
			m_zLocCode = m_pParent->m_zLocCode;
			break;
		case ot_BackTopRight:
			m_xLocCode = m_pParent->m_xLocCode + (1 << m_level);
			m_yLocCode = m_pParent->m_yLocCode + (1 << m_level);
			m_zLocCode = m_pParent->m_zLocCode;
			break;
		case ot_FrontBottomLeft:
			m_xLocCode = m_pParent->m_xLocCode;
			m_yLocCode = m_pParent->m_yLocCode;
			m_zLocCode = m_pParent->m_zLocCode + (1 << m_level);
			break;
		case ot_FrontBottomRight:
			m_xLocCode = m_pParent->m_xLocCode + (1 << m_level);
			m_yLocCode = m_pParent->m_yLocCode;
			m_zLocCode = m_pParent->m_zLocCode + (1 << m_level);
			break;
		case ot_FrontTopLeft:
			m_xLocCode = m_pParent->m_xLocCode;
			m_yLocCode = m_pParent->m_yLocCode + (1 << m_level);
			m_zLocCode = m_pParent->m_zLocCode + (1 << m_level);
			break;
		case ot_FrontTopRight:
			m_xLocCode = m_pParent->m_xLocCode + (1 << m_level);
			m_yLocCode = m_pParent->m_yLocCode + (1 << m_level);
			m_zLocCode = m_pParent->m_zLocCode + (1 << m_level);
			break;
		}
	}

	m_children.reserve(8);
}

VistaOctreeNode::~VistaOctreeNode()
{
	if (m_children.size() == 8)
	{
		for (int i=0; i<8; i++)
			delete(m_children[i]);
	}
}

VistaOctree::VistaOctree()
{
}

VistaOctree::~VistaOctree()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaOctree::Reduce(VistaOctreeNode& node)
{
	if (node.m_children.size() == 8)
	{
		for (int i=0; i<8; i++)
			delete(node.m_children[i]);

		node.m_children.clear();
	}
	else
		return false;

	return true;
}

bool VistaOctree::Split(VistaOctreeNode& node)
{
	if (node.m_children.size() == 0 && node.GetLevel() > 0)
	{
		for (int i=0; i<8; i++)
			node.m_children.push_back(NewNode(&node, i));
	}
	else
		return false;

	return true;
}

void VistaOctree::GetNodePosition(const VistaOctreeNode& node, float& x, float& y, float& z) const
{
	x = LocCodeToPosition(node.m_xLocCode);
	y = LocCodeToPosition(node.m_yLocCode);
	z = LocCodeToPosition(node.m_zLocCode);
}

void VistaOctree::GetNodeBoundingBox (const VistaOctreeNode& node, float& x, float& y, float& z, float& size) const
{
//	cout << "[VistaOctree::GetNodeBoundingBox] Processing node (" << node.m_xLocCode << ", " << node.m_yLocCode << ", " << node.m_zLocCode << ")" << std::endl;
	x = (float) node.m_xLocCode / (1 << (m_pRoot->m_level));
	y = (float) node.m_yLocCode / (1 << (m_pRoot->m_level));
	z = (float) node.m_zLocCode / (1 << (m_pRoot->m_level));
	size = 1.0f / (float)(1 << (m_pRoot->m_level - node.m_level));
//	cout << "[VistaOctree::GetNodeBoundingBox] BBox of node: [(" << x << ", " << y << ", " << z << ") with size " << size << std::endl;
}

VistaOctreeNode* VistaOctree::Traverse (const int xLocCode, 
										  const int yLocCode, 
										  const int zLocCode,
										  VistaOctreeNode* start) const
{
	VistaOctreeNode* node = start;
	unsigned short childIndex;
	unsigned short childBranchBit;

	if(!node)
		node = m_pRoot;

	for (unsigned short nextLevel = node->m_level - 1; 
		!(node->IsLeaf()); 
		nextLevel--) 
	{ 
		childBranchBit = 1 << nextLevel; 
		
		if(nextLevel>1)
		{
			childIndex = ((xLocCode & childBranchBit) >> nextLevel)
					   + ((yLocCode & childBranchBit) >> (nextLevel-1))
					   + ((zLocCode & childBranchBit) >> (nextLevel-2)); 
		}
		else if(nextLevel==1)
		{
			childIndex = ((xLocCode & childBranchBit) >> 1)
					   +  (yLocCode & childBranchBit)
					   + ((zLocCode & childBranchBit) << 1); 
		}
		else //if(nextLevel==0)
		{
			childIndex =  (xLocCode & childBranchBit)
					   + ((yLocCode & childBranchBit) << 1)
					   + ((zLocCode & childBranchBit) << 2); 
		}

		node = node->m_children[childIndex]; 
	} 

	return node;
}

// LocateRegion returns the minimal node of the octree, fully containing the given region
VistaOctreeNode *VistaOctree::LocateRegion (const float min[3], const float max[3])
{
	int minLocCode[3], maxLocCode[3];
	int diff[3];
	for (int i = 0; i < 3; ++i)
	{
		minLocCode[i] = PositionToLocCode (min[i]);
		maxLocCode[i] = PositionToLocCode (max[i]);
		diff[i] = minLocCode[i] ^ maxLocCode[i];
	}
	int level[3];
	level[0] = level[1] = level[2] = m_pRoot->m_level - 1;
	while ((! (diff[0] >> level[0])) && level[0] > 0) level[0]--;
	while ((! (diff[1] >> level[1])) && level[1] > level[0]) level[1]--;
	while ((! (diff[2] >> level[2])) && level[2] > level[1]) level[2]--;
	level[2]++;
	return (TraverseToLevel (minLocCode[0], minLocCode[1], minLocCode[2], level[2], NULL));
}

// TraverseToLevel does the same thing as Traverse but returns the node at the given level
VistaOctreeNode* VistaOctree::TraverseToLevel (
			const int xLocCode, const int yLocCode, const int zLocCode,
			const int level, VistaOctreeNode* start) const
{
	VistaOctreeNode *node = start;
	if (! start) node = m_pRoot;
	
	unsigned short nextLevel = node->m_level - 1;
	unsigned short childIndex, childBranchBit;
	
	while (nextLevel >= level && (! node->IsLeaf ())) 
	{
		childBranchBit = 1 << nextLevel; 
		
		if(nextLevel > 1)
		{
			childIndex = ((xLocCode & childBranchBit) >> nextLevel)
					   + ((yLocCode & childBranchBit) >> (nextLevel-1))
					   + ((zLocCode & childBranchBit) >> (nextLevel-2)); 
		}
		else if(nextLevel==1)
		{
			childIndex = ((xLocCode & childBranchBit) >> 1)
					   +  (yLocCode & childBranchBit)
					   + ((zLocCode & childBranchBit) << 1); 
		}
		else //if(nextLevel==0)
		{
			childIndex =  (xLocCode & childBranchBit)
					   + ((yLocCode & childBranchBit) << 1)
					   + ((zLocCode & childBranchBit) << 2); 
		}
		node = node->m_children[childIndex]; 

		nextLevel--;
	} 
	return (node);
}

VistaOctreeNode* VistaOctree::GetCommonAncestor (VistaOctreeNode &node, int &nodeLevel, int diff) const
{
	VistaOctreeNode *tmpNode = &node;
	while ((diff) & (1 << (nodeLevel)))
	{
		tmpNode = tmpNode->m_pParent;
		nodeLevel++;
	}
	return (tmpNode);
}

VistaOctreeNode* VistaOctree::LocateNeighbor (VistaOctreeNode &node, const int direction) const
{
	// all valid directions are in the range 0-2^6 
	// but not all numbers in this range are valid directions
	if(direction <= 0 || direction >= 1<<6)
		return NULL;

	// get the node's locCodes
	int xLocCode = node.m_xLocCode;
	int yLocCode = node.m_yLocCode;
	int zLocCode = node.m_zLocCode;
	int nodeLevel = node.m_level;
	int binaryNodeSize = 1 << (node.m_level);

	int diff = 0;

	if(direction & ot_dirLeft)
	{
		if (node.m_xLocCode == 0) 
			return (NULL);
	
		diff = xLocCode ^ (xLocCode-1);	// bitwise XOR
		xLocCode--;
	}
	else if(direction & ot_dirRight)
	{
		if ((node.m_xLocCode + binaryNodeSize) >= (1 << m_pRoot->m_level)) 
			return (NULL);

		diff =  xLocCode ^ (xLocCode + binaryNodeSize);
		xLocCode += binaryNodeSize;
	}

	if(direction & ot_dirBottom)
	{
		if (node.m_yLocCode == 0) 
			return (NULL);
	
		diff = diff | (yLocCode ^ (yLocCode-1));
		yLocCode--;
	}
	else if(direction & ot_dirTop)
	{
		if ((node.m_yLocCode + binaryNodeSize) >= (1 << m_pRoot->m_level)) 
			return (NULL);

		diff = diff | (yLocCode ^ (yLocCode + binaryNodeSize));
		yLocCode += binaryNodeSize;
	}

	if(direction & ot_dirBottom)
	{
		if (node.m_yLocCode == 0) 
			return (NULL);
	
		diff = diff | (yLocCode ^ (yLocCode-1));
		yLocCode--;
	}
	else if(direction & ot_dirTop)
	{
		if ((node.m_yLocCode + binaryNodeSize) >= (1 << m_pRoot->m_level)) 
			return (NULL);

		diff = diff | (yLocCode ^ (yLocCode + binaryNodeSize));
		yLocCode += binaryNodeSize;
	}

	if(direction & ot_dirBack)
	{
		if (node.m_zLocCode == 0) 
			return (NULL);
	
		diff = diff | (zLocCode ^ (zLocCode-1));
		zLocCode--;
	}
	else if(direction & ot_dirFront)
	{
		if ((node.m_zLocCode + binaryNodeSize) >= (1 << m_pRoot->m_level)) 
			return (NULL);

		diff = diff | (zLocCode ^ (zLocCode + binaryNodeSize));
		zLocCode += binaryNodeSize;
	}

	VistaOctreeNode *pNode = GetCommonAncestor (node, nodeLevel, diff);
	
	return (TraverseToLevel (xLocCode, yLocCode, zLocCode, nodeLevel-1, pNode));
}
/*
VistaOctreeNode* VistaOctree::LocateLeftNeighbor (VistaOctreeNode &node) const
{
	if (node.m_xLocCode == 0) return (NULL);
	int xLocCode = node.m_xLocCode;
	int yLocCode = node.m_yLocCode;
	int zLocCode = node.m_zLocCode;
	
	int xLeftLocCode = xLocCode - 1;
	
	int nextLevel = node.m_level;
	int diff = xLocCode ^ xLeftLocCode;
	
	VistaOctreeNode *pNode = GetCommonAncestor (node, nextLevel, diff);
	nextLevel--;
	
	return (TraverseToLevel (xLeftLocCode, yLocCode, zLocCode, nextLevel, pNode));
}

VistaOctreeNode* VistaOctree::LocateRightNeighbor (VistaOctreeNode &node) const
{
	int binaryNodeSize = 1 << (node.m_level);
	
	if ((node.m_xLocCode + binaryNodeSize) >= (1 << m_pRoot->m_level)) return (NULL);
	
	int xLocCode = node.m_xLocCode;
	int yLocCode = node.m_yLocCode;
	int zLocCode = node.m_zLocCode;
	
	int xRightLocCode =  xLocCode + binaryNodeSize;
	
	int diff = xLocCode ^ xRightLocCode;
	
	int nextLevel = node.m_level;
	VistaOctreeNode *pNode = GetCommonAncestor (node, nextLevel, diff);
	
	nextLevel--;
	
	return (TraverseToLevel (xRightLocCode, yLocCode, zLocCode, nextLevel, pNode));
}

VistaOctreeNode* VistaOctree::LocateBottomNeighbor (VistaOctreeNode &node) const
{
	if (node.m_yLocCode == 0) return (NULL);
	int xLocCode = node.m_xLocCode;
	int yLocCode = node.m_yLocCode;
	int zLocCode = node.m_zLocCode;
	
	int yBottomLocCode = yLocCode - 1;
	
	int nextLevel = node.m_level ;
	int diff = yLocCode ^ yBottomLocCode;

	VistaOctreeNode *pNode = GetCommonAncestor (node, nextLevel, diff);

	nextLevel--;
	
	return (TraverseToLevel (xLocCode, yBottomLocCode, zLocCode, nextLevel, pNode));
}

VistaOctreeNode* VistaOctree::LocateTopNeighbor (VistaOctreeNode &node) const
{
	int binaryNodeSize = 1 << node.m_level;
	
	if ((node.m_yLocCode + binaryNodeSize) >= (1 << m_pRoot->m_level)) return (NULL);
	int xLocCode = node.m_xLocCode;
	int yLocCode = node.m_yLocCode;
	int zLocCode = node.m_zLocCode;
	
	int yTopLocCode =  yLocCode + binaryNodeSize;
	
	int diff = yLocCode ^ yTopLocCode;
	int nextLevel = node.m_level;
	
	VistaOctreeNode *pNode = GetCommonAncestor (node, nextLevel, diff);
	
	nextLevel--;
	
	return (TraverseToLevel (xLocCode, yTopLocCode, zLocCode, nextLevel, pNode));
}

VistaOctreeNode* VistaOctree::LocateBackNeighbor (VistaOctreeNode &node) const
{
	if (node.m_zLocCode == 0) return (NULL);
	int xLocCode = node.m_xLocCode;
	int yLocCode = node.m_yLocCode;
	int zLocCode = node.m_zLocCode;
	
	int zBackLocCode = zLocCode - 1;
	
	int nextLevel = node.m_level;
	int diff = zLocCode ^ zBackLocCode;
	VistaOctreeNode *pNode = GetCommonAncestor (node, nextLevel, diff);
	nextLevel--;
	
	return (TraverseToLevel (xLocCode, yLocCode, zBackLocCode, nextLevel, pNode));
}

VistaOctreeNode* VistaOctree::LocateFrontNeighbor (VistaOctreeNode &node) const
{
	int binaryNodeSize = 1 << node.m_level;
	
	if ((node.m_zLocCode + binaryNodeSize) >= (1 << m_pRoot->m_level)) return (NULL);
	int xLocCode = node.m_xLocCode;
	int yLocCode = node.m_yLocCode;
	int zLocCode = node.m_zLocCode;
	
	int zFrontLocCode =  zLocCode + binaryNodeSize;
	
	int diff = zLocCode ^ zFrontLocCode;
	
	int nextLevel = node.m_level;
	VistaOctreeNode *pNode = GetCommonAncestor (node, nextLevel, diff);
	
	nextLevel--;
	
	return (TraverseToLevel (xLocCode, yLocCode, zFrontLocCode, nextLevel, pNode));
}

*/
VistaOctreeNode* VistaOctree::LocateLeftNeighbor (VistaOctreeNode &node) const
{
	return LocateNeighbor(node, ot_dirLeft);
}

VistaOctreeNode* VistaOctree::LocateBottomNeighbor (VistaOctreeNode &node) const
{
	return LocateNeighbor(node, ot_dirBottom);
}

VistaOctreeNode* VistaOctree::LocateBackNeighbor (VistaOctreeNode &node) const
{
	return LocateNeighbor(node, ot_dirBack);
}

VistaOctreeNode* VistaOctree::LocateRightNeighbor (VistaOctreeNode &node) const
{
	return LocateNeighbor(node, ot_dirRight);
}

VistaOctreeNode* VistaOctree::LocateTopNeighbor (VistaOctreeNode &node) const
{
	return LocateNeighbor(node, ot_dirTop);
}

VistaOctreeNode* VistaOctree::LocateFrontNeighbor (VistaOctreeNode &node) const
{
	return LocateNeighbor(node, ot_dirFront);
}

void VistaOctree::Serialize(ostream & outFile) const
{
	list<VistaOctreeNode *> nodesFIFO; 
	VistaOctreeNode* node;

	int currLevel = GetRootLevel();

	outFile << currLevel << std::endl;

	nodesFIFO.push_back(m_pRoot);

	while (!nodesFIFO.empty())
	{
		node = nodesFIFO.front();

		if(node->GetLevel()<currLevel)
		{
			currLevel--;
			outFile << std::endl;
		}

		outFile << (node->IsLeaf() ? 'x' : 'o');
		node->Serialize(outFile);

		if(!nodesFIFO.front()->IsLeaf())
			for (int i = 0; i < 8; i++)
				nodesFIFO.push_back(node->GetChild(i));

		nodesFIFO.pop_front();
	}	
}

void VistaOctree::DeSerialize(istream &inFile)
{
	int i;

	// delete the current tree
	if(!m_pRoot->IsLeaf())
	{
		Reduce(*m_pRoot);
	}

	// This method does the complementary process of the Save method.
	int rootLevel;
	inFile >> rootLevel;
	SetRootLevel(rootLevel);

	list<VistaOctreeNode *> nodesFIFO;
	VistaOctreeNode * node;

	char leaf;

	nodesFIFO.push_back(m_pRoot); 	

	while (!nodesFIFO.empty())
	{
		node = nodesFIFO.front();

		inFile >> leaf;
		
		node->DeSerialize(inFile);

		if(leaf == 'o')
		{
			Split(*node);	// create child nodes for every parent node.
			for (i=0; i<8; i++)
			{
				nodesFIFO.push_back(node->GetChild(i));
			}
		}
		nodesFIFO.pop_front();		
	}
}

