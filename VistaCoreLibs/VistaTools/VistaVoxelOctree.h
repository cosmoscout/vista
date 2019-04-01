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


#ifndef	CVISTAVOXELOCTREE_H
#define CVISTAVOXELOCTREE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

#include "VistaOctree.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

template <class T>
class VistaVoxelOctreeNode : public VistaOctreeNode
{
public:
	VistaVoxelOctreeNode(VistaOctreeNode* pParent, const int i);
	virtual ~VistaVoxelOctreeNode();

	virtual bool SetValue(const T& value);
	virtual T GetValue() const;

	bool AllChildrenEqual() const;

protected:
	virtual void Serialize(ostream&) const;
	virtual void DeSerialize(istream&);

private:

private:
	T	m_value;
};

template <class T>
class VistaVoxelOctree : public VistaOctree
{
public:
	// the public interface is the same as for a 3D array
	VistaVoxelOctree(const int width, const int height, const int depth);
	virtual ~VistaVoxelOctree();

	virtual bool SetValue(const int i, const int j, const int k, const T& value);
	virtual T GetValue(const int i, const int j, const int k) const;

protected:
	// overwritten VistaOctree methods
	virtual VistaOctreeNode* NewNode (VistaOctreeNode* pParent, const int i);

	virtual bool Split (VistaOctreeNode&);
	virtual bool Reduce (VistaOctreeNode&);

	bool SetValue(const int i, const int j, const int k, const T& value, const int level);

protected:
	const int m_iwidth;
	const int m_iheight;
	const int m_idepth;
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

template <class T>
VistaVoxelOctreeNode<T>::VistaVoxelOctreeNode(VistaOctreeNode* pParent, const int i)
: VistaOctreeNode(pParent, i)
{
	m_value = T();
}

template <class T>
VistaVoxelOctreeNode<T>::~VistaVoxelOctreeNode()
{
}

template <class T>
VistaVoxelOctree<T>::VistaVoxelOctree(const int width, const int height, const int depth)
: m_iwidth(width), m_iheight(height), m_idepth(depth)
{
	m_pRoot = NewNode(NULL, 0);

	int level = (width > height) ? width : height;
	level = (level > depth) ? level : depth;
	level -= 1;

	// im level ist jetzt die maximale darstellbare position
	// wenn also width=height=6 und depth=4 ist, ist im level jetzt eine 5

	// jetzt muss man rauskriegen, wieviel bits man braucht um diese position darzustellen
	// dafuer zaehlt man bits bis zur hoechsten 1
	// das entspricht einem right shift bis level=0 ist
	// fuer level=5 muesste das ergebnis 3 bits sein

	int i, maxi = GetMaxLevel();

	for(i=0; i<maxi && level; i++)
		level >>= 1;

	SetRootLevel(i);
}

template <class T>
VistaVoxelOctree<T>::~VistaVoxelOctree()
{
	delete m_pRoot;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

template <class T>
bool VistaVoxelOctreeNode<T>::AllChildrenEqual() const
{
	if(IsLeaf())
		return false;

	T	value = ((VistaVoxelOctreeNode<T> *) GetChild(0))->GetValue();

	if(! GetChild(0)->IsLeaf())
		return false;

	for(int i=1; i<8; i++)
		if(! GetChild(i)->IsLeaf())
			return false;
		else
			if(value != ((VistaVoxelOctreeNode<T> *) GetChild(i))->GetValue())
				return false;

	return true;
}

template <class T>
void VistaVoxelOctreeNode<T>::Serialize(ostream& outFile) const
{
	outFile << GetValue() << ' ';
}

template <class T>
void VistaVoxelOctreeNode<T>::DeSerialize(istream& inFile)
{
	T value;
	inFile >> value;
	SetValue(value);
}

template <class T>
bool VistaVoxelOctreeNode<T>::SetValue(const T& value)
{
	m_value = value;
	return true;
}

template <class T>
T VistaVoxelOctreeNode<T>::GetValue() const
{
	return m_value;
}

template <class T>
VistaOctreeNode* VistaVoxelOctree<T>::NewNode (VistaOctreeNode* pParent, const int i)
{
	return new VistaVoxelOctreeNode<T> (pParent, i);
}

template <class T>
bool VistaVoxelOctree<T>::Split (VistaOctreeNode& node)
{
	// create children
	VistaOctree::Split(node);

	// copy data from node to the new children
	// each child gets the same data
	VistaVoxelOctreeNode<T> *voxelNode = (VistaVoxelOctreeNode<T> *) &node;

	T	value = voxelNode->GetValue();

	VistaVoxelOctreeNode<T> *child;

	for (int i=0; i<8; i++)
	{
		child = (VistaVoxelOctreeNode<T> *) node.GetChild(i);
		child->SetValue(value);
	}

	return true;
}

template <class T>
bool VistaVoxelOctree<T>::Reduce (VistaOctreeNode& node)
{
	// move data from the children to node
	// i assume that all children have the same data value
	// otherwise some kind of average would have to be built
	// which is not doable for all data types (template)

	VistaVoxelOctreeNode<T> *child = (VistaVoxelOctreeNode<T> *) node.GetChild(0);
	((VistaVoxelOctreeNode<T> *) &node)->SetValue(child->GetValue());

	// delete children
	VistaOctree::Reduce(node);

	return true;
}

template <class T>
bool VistaVoxelOctree<T>::SetValue(const int i, const int j, const int k, const T& value)
{
	// get the deepest existing node in the according to i,j,k
	VistaVoxelOctreeNode<T>* node = (VistaVoxelOctreeNode<T>*) Traverse(i,j,k);

	if(node->GetValue() == value)
		return true;	// the node already had this value
	else
	{
		if(node->GetLevel() == 0) // the level 0 node exists already
		{
			node->SetValue(value);

			// check if all brothers have the same value (possible reduce)
			VistaVoxelOctreeNode<T>* parent = (VistaVoxelOctreeNode<T>*) node->GetParent();

			if(parent)
				if(parent->AllChildrenEqual())
					Reduce(*parent);
		}
		else
		{
			while(node->GetLevel() > 0) // split node down to level 0
			{
				Split(*node);
				// get the right child
				node = (VistaVoxelOctreeNode<T>*) Traverse(i,j,k, node);
			}

			node->SetValue(value);
		}

		return true;
	}
}

template <class T>
bool VistaVoxelOctree<T>::SetValue(const int i, const int j, const int k, const T& value, const int level)
{
	// get the deepest existing node in the according to i,j,k
	VistaVoxelOctreeNode<T>* node = (VistaVoxelOctreeNode<T>*) Traverse(i,j,k);

	while(node->GetLevel() < level)
	{
		node = (VistaVoxelOctreeNode<T>*) node->GetParent();
		Reduce(*node);
	}

	while(node->GetLevel() > level)
	{
		Split(*node);
		// get the right child
		node = (VistaVoxelOctreeNode<T>*) Traverse(i,j,k, node);
	}

	if(node->GetLevel() == level)
	{
		node->SetValue(value);

		// check if all brothers have the same value (possible reduce)
		VistaVoxelOctreeNode<T>* parent = (VistaVoxelOctreeNode<T>*) node->GetParent();

		while(parent)
		{
			if(parent->AllChildrenEqual())
			{
				Reduce(*parent);
				parent = (VistaVoxelOctreeNode<T>*) parent->GetParent();
			}
			else
				parent = NULL;
		}
	}

	return true;
}

template <class T>
T VistaVoxelOctree<T>::GetValue(const int i, const int j, const int k) const
{
	VistaVoxelOctreeNode<T>* node = (VistaVoxelOctreeNode<T>*) Traverse(i,j,k);

	return node->GetValue();
}

#endif // CVISTAVOXELOCTREE_H

