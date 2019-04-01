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


#ifndef _VISTATOPOLOGYGRAPH_H
#define _VISTATOPOLOGYGRAPH_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <list>
#include <queue>
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
class VistaTopologyGraph
{
public:
	class _Node
	{
	public:
		friend class VistaTopologyGraph;

		enum eColor
		{
			WHITE = 0,
			GREY,
			BLACK
		};
	public:
		_Node(T oElement)
			: m_oElement(oElement),
			  m_eColor(WHITE),
			  m_nFinishTs(-1),
			  m_nDiscoverTs(-1) {}

		T m_oElement;
	private:

		eColor m_eColor;
		int    m_nFinishTs,
			   m_nDiscoverTs;

		std::list<_Node*> m_liAdj;
	};

	typedef _Node Node;


	~VistaTopologyGraph()
	{
		for(typename std::list<Node*>::iterator it = m_liGraph.begin();
			it != m_liGraph.end(); ++it)
			delete *it;
	}


	Node *AddNode( T value )
	{
		Node *pRet = new Node(value);
		m_liGraph.push_back(pRet);
		return pRet;
	}

	void AddNode( Node *pNode )
	{
		m_liGraph.push_back(pNode);
	}

	bool HasNode( Node *pNode ) const
	{
		return !(std::find( m_liGraph.begin(), m_liGraph.end(), pNode ) == m_liGraph.end() );
	}

	bool HasNodeValue( const T &value ) const
	{
		for(typename std::list<Node*>::const_iterator cit = m_liGraph.begin();
			cit != m_liGraph.end(); ++cit)
		{
			if( (*cit)->m_oElement == value )
				return true;
		}
		return false;
	}

	void AddDependency( Node *pNode, Node *pDependsOnNode)
	{
		pNode->m_liAdj.push_back(pDependsOnNode);
	}

	void AddDependency( T nodeValue, T oDependsOnNodeValue)
	{

		Node *pNode = GetNodeByValue( nodeValue );
		if(!pNode)
			pNode = AddNode( nodeValue );

		Node *pDependsOn = GetNodeByValue( oDependsOnNodeValue );
		if(!pDependsOn)
			pDependsOn = AddNode(oDependsOnNodeValue);

		AddDependency( pNode, pDependsOn );
	}

	Node *GetNodeByValue( const T &value ) const
	{
		for(typename std::list<Node*>::const_iterator cit = m_liGraph.begin();
			cit != m_liGraph.end(); ++cit)
		{
			if( (*cit)->m_oElement == value )
				return (*cit);
		}
		return NULL;
	}

	std::list<Node*> CalcTopology()
	{
		for(typename std::list<Node*>::iterator cit1 = m_liGraph.begin();
			cit1 != m_liGraph.end(); ++cit1)
		{
			(*cit1)->m_eColor = Node::WHITE;
			(*cit1)->m_nFinishTs = -1;
			(*cit1)->m_nDiscoverTs = -1;
		}

		std::list<Node*> liReturn;
		int nTime = 0;
		for(typename std::list<Node*>::iterator cit = m_liGraph.begin();
			cit != m_liGraph.end(); ++cit)
		{
			if( (*cit)->m_eColor == Node::WHITE )
			{
				DFSVisit( *cit, liReturn, nTime );
			}
		}

		return liReturn;
	}

	std::list<Node*> CalcBFSSubgraph( Node* pStartNode )
	{
		for(typename std::list<Node*>::iterator cit1 = m_liGraph.begin();
			cit1 != m_liGraph.end(); ++cit1)
		{
			(*cit1)->m_eColor = Node::WHITE;
			(*cit1)->m_nFinishTs = -1;
			(*cit1)->m_nDiscoverTs = -1;
		}

		std::list<Node*> liReturn;
		std::queue<Node*> queBFSQueue;
		queBFSQueue.push( pStartNode );
		while( queBFSQueue.empty() == false )
		{
			Node* pNode = queBFSQueue.front();
			liReturn.push_back( pNode );
			queBFSQueue.pop();
			if( pNode->m_eColor != Node::WHITE )
				continue;
			pNode->m_eColor = Node::BLACK;
			for( typename std::list<Node*>::iterator itChild = pNode->m_liAdj.begin();
				itChild != pNode->m_liAdj.end(); ++itChild )
			{
				queBFSQueue.push( (*itChild) );
			}
		}

		return liReturn;
	}

private:
	void DFSVisit( Node *pNode,
				   std::list<Node*> &liStore,
				   int &nTime )
	{
		pNode->m_eColor = Node::GREY;
		pNode->m_nDiscoverTs = ++nTime;
		for(typename std::list<Node*>::iterator it = pNode->m_liAdj.begin();
			it != pNode->m_liAdj.end(); ++it)
		{
			if( (*it)->m_eColor == Node::WHITE )
			{
				DFSVisit( *it, liStore, nTime );
			}
		}
		pNode->m_eColor = Node::BLACK;
		pNode->m_nFinishTs = ++nTime;
		liStore.push_front( pNode );
	}

	typename std::list<Node*> m_liGraph;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATOPOLOGYGRAPH_H

