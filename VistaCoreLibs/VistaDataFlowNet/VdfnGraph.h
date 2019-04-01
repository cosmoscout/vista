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


#ifndef _VDFNGRAPH_H
#define _VDFNGRAPH_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnConfig.h"
#include <VistaAspects/VistaSerializable.h>


#include <map>
#include <list>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVdfnNode;
class IVdfnReEvalNode;
class IVdfnPort;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * @brief Graph container and traversal interface.
 *
 * The VdfnGraph implements the following responsibilities.
 * - management of nodes and detection of edges
 * - definition of a traversal structure and evaluation algorithm
 * - access structures for composite nodes (using Exports)
 * - retrieval of "input nodes", i.e., nodes with 0 inports
 *
 * A graph can be active, i.e., the node traversal actually evaluates node
 * values, or inactive, i.e., traversal does not change node values.
 *
 * @note It is really not meant to traverse this graph's structure by hand
 *       as a programmer. Meaning: this is not a general purpose graph data
 *       structure.
 */
class VISTADFNAPI VdfnGraph : public IVistaSerializable
{
public:
	/**
	 * @brief helper structure to access graph 'exports'.
	 * Usually required in conjunction with Composite Nodes, i.e., nodes
	 * that contain graphs themselves.
	 * Exports define virtual in- and output ports of a graph that are
	 * reflected on a Composite Node.
	 *
	 * @see SetExports()
	 * @see GetExports()
	 */
	class ExportData
	{
	public:

		enum eDir
		{
			UNDEFINED=-1, /**< error tag, initial value */
			INPORT=0,     /**< export direction marks an inport  */
			OUTPORT       /**< export direction marks an outport */
		};

		/**
		 * @brief default c'tor.
		 * Initializes export direction to UNDEFINED and target node as NULL
		 */
		ExportData()
		: m_nDirection(UNDEFINED),
		  m_pTargetNode(NULL)
		{

		}


		/**
		 * @brief connection defining c'tor.
		 * @param strNodeName export defining node name
		 * @param strPortName port name from node <strNodeName> to export
		 * @param strMapName  name to use when exporting the port
		 * @param direction   choice between inport or outport. using
		 *                    undefined is, well, undefined.
		 */
		ExportData( const std::string &strNodeName,
				    const std::string &strPortName,
				    const std::string &strMapName,
				    eDir direction)
		: m_strNodeName(strNodeName),
		  m_strPortName(strPortName),
		  m_strMapName(strMapName),
		  m_nDirection(direction),
		  m_pTargetNode(NULL)
		{

		}

		std::string m_strNodeName; /**< name of the node to export */
		std::string m_strPortName; /**< name of the port to export */
		std::string m_strMapName;  /**< export name, can be used to rename ports */

		eDir        m_nDirection;  /**< port direction, INPORT or OUTPORT */
		IVdfnNode  *m_pTargetNode; /**< cache pointer in case of INPORT port */
	};

	/**
	 * typdef a list of nodes
	 */
	typedef std::list<IVdfnNode*>             Nodes;

	/**
	 * used to note to which port the connection was done and under
	 * what name.
	 */
	typedef std::pair<std::string, IVdfnPort*> ConPortInfo;

	/**
	 * is meant to reflect a target node and a corresponding port
	 * where the edge in coming in.
	 */
	typedef std::list< std::pair<IVdfnNode*, ConPortInfo> > ConInfo;

	/**
	 * maps all the nodes that are connected by Connect.first to
	 * the list of Connect.second.
	 */
	typedef std::pair<IVdfnPort*, ConInfo> Connect;

	/**
	 * Edges define a mapping from a node which is connected over
	 * a number of connects, which in term map from a port to all
	 * the nodes which are reached by that port. The ConInfo
	 * provides a link between the target node of the Connect.Port
	 * and the TargetNode.Port
	 */
	typedef std::map < IVdfnNode*, std::list<Connect> >      Edges;

	/**
	 * A list of exported ports.
	 */
	typedef std::list<ExportData>               ExportList;

	VdfnGraph();
	virtual ~VdfnGraph();

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CALCULATION
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * @param nTimeStamp provide a time stamp that is passed as update time
	                     stamp to all nodes that are evaluated.
						 Pass 0 here for the initial build which will build
						 up the edges and will call the PrepareEvaluationRun()
						 method on all nodes which are valid.
	 * @return true off no error was encountered during setup
	 */
	bool EvaluateGraph(double nTimeStamp);

	/**
	 * activates this graph. if the dTimeStamp is 0, the activation
	 * routine on all nodes will be run, even if the current state of
	 * this graph is active. If the dTimeStamp is not 0, and an already
	 * active graph is to be activated, nothing will happen.
	 * If a graph is activated, it runs over <b>all</b> nodes in the graph
	 * and calls IVdfnNode::OnActivate() on them, passing dTimeStamp.
	 * If a graph is deactivated, it runs over <b>all</b> nodes in the graph
	 * and calls IVdfnNode::OnDeactivate() on them, passing dTimeStamp.
	 * @param bIsActive true if this graph is marked active, false else
	 * @param dTimeStamp a time stamp, 0 for "force-state-propagation-to-nodes"
	 */
	void SetIsActive( bool bIsActive, double dTimeStamp );

	/**
	 * Returns whether this graph is active or nor. By default, a graph is
	 * active.
	 * @see SetIsActive()
	 * @return true if this graph is active, false else.
	 */
	bool GetIsActive() const;

	/**
	 * The graph will iterate over all nodes and check whether they are
	 * dirty or not. For that, the UnconditionalEvaluation() flag will be
	 * @return true when this graph is dirty and needs to be evaluated.
	 */
	bool NeedsEvaluation() const;

	/**
	 * Reinitializes the Graph, thereby fetching the action objects again,
	 * and checking for proper edge/port setup
	 */
	bool ReloadActionObjects();


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// NODE MANAGEMENT
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 *  Adds a node to this graph. This has no direct consequence.
	 *  It is not checked, whether the node is already part of this graph.
	 *  The graph is marked dirty when a node is added. On the next request
	 *  to evaluate the graph, the internal traversal structures will be rebuild.
	 *  @return always true
	 *  @param pNode the node to be added for evaluation
	 */
	bool AddNode( IVdfnNode *pNode );

	/**
	 * removes a node (does not claim the memory for it) and mark this graph dirty
	 */
	bool RemNode( IVdfnNode * );

	/**
	 * @return true when the node given is part of this graph
	 */
	bool GetIsNode( IVdfnNode *) const;

	/**
	 * Searches a node in this graph by name. Note that is is assumes that any
	 * node in this graph has a unique name.
	 * The node is searched by a case-sensitive comparison to the node name.
	 * @return NULL iff a node with name strName was not found in this graph.
	 * @param strName the case sensitive name to be searched for.
	 */
	IVdfnNode *GetNodeByName(const std::string &strName) const;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// EDGE MANAGEMENT
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * @return true if there is a direct connection from n1 to n2, false else
	 * @param n1 non NULL pointer to the first (parent) node
	 * @param n2 non NULL pointer to the second (child) node
	 */
	bool GetIsConnectedFromTo( IVdfnNode *n1, IVdfnNode *n2) const;

	/**
	 * Query the node that has pPort as its OUT port.
	 * @param pPort the port that is part of the return node as an out port
	 * @return the node that owns / contains the outport pPort or NULL
	 */
	IVdfnNode *GetNodeForPort( IVdfnPort *pPort ) const;


	/**
	 * @return a const reference to the nodes stored in the graph
	 */
	const Nodes &GetNodes() const;

	/**
	 * @return all nodes that have a non-empty tag field
	 */
	Nodes GetNodesWithTag() const;

	/**
	 * @return all nodes that have the tag strGroupTag set as their tag
	 */
	Nodes GetNodesByGroupTag( const std::string &strGroupTag ) const;

	/**
	 * @return a const reference on all edges of this graph
	 */
	const Edges &GetEdges() const;

	/**
	 * @return get all nodes that have 0 inports, thus can not depend on any other
	           nodes in this graph (and serve as 'sources')
	 */
	Nodes GetSourceNodes() const;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// COMPOSITE/EXPORTS MANAGEMENT
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * Graphs can contain mappings for nodes and ports that can be used
	 * for exporting nodes. While this is not a true feature of a "graph",
	 * it is embodied here as API for storage. Maybe some day, the information
	 * can be used for dependency checking between graphs. Inports may not be
	 * set twice, while outports, as usual, can be used more than one time
	 * @param liExports container for the list of exports defined for this graph
	 */
	void GetExports( ExportList &liExports ) const;

	/**
	 * will only allow to set exports for existing nodes, checking of port availability
	 * will result in a warning only (as ports may be dynamically created after a call
	 * to SetExports().
	 * @todo implement consistency checking
	 * @param liExports the list of ports which are to be exported to the outside, for
	          inports serve as input to this graph
	 */
	void SetExports( const ExportList &liExports );

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ViSTA CLUSTER API
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 *  returns the number of nodes that are / have to be evaluated
	 *  on the "master-simulation-node" in a cluster environment.
	 *  0 if there are only nodes that can compute locally.
	 */
	unsigned int GetMasterSimCount() const;

	/**
	 * control API, it tells the serializer of this graph to respect the
	 * IVdfnNode::GetIsMasterSim() flag or not. If this method returns
	 * false, the master sim flag can be ignored during serialization, which
	 * will then dump <b>all</b> nodes of this graph,
	 * not only the ones sitting on the master.
	 * @see SetCheckMasterSim()
	 */
	bool GetCheckMasterSim() const;

	/**
	 * determines whether this graph will only serialize those nodes that
	 * are tagged as being simulated / evaluated only on the master machine.
	 */
	void SetCheckMasterSim( bool bCheck );

	/**
	 * Print debug info about the node
	 */
	void PrintInfo( std::ostream& oStream ) const;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// SERIALIZABLE
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual int    Serialize(IVistaSerializer &) const;
	virtual int    DeSerialize(IVistaDeSerializer &);
	virtual std::string GetSignature() const;

private:
	typedef std::map <IVdfnPort*, IVdfnNode*> PortLookup;
	typedef std::vector<IVdfnNode*>           NodeVec;

protected:
	/**
	 * @brief evaluates nodes in the given vector, passing nTimeStamp as argument.
	 */
	bool EvaluateSubGraph( const NodeVec& vecSubGraph, const double nTimeStamp );

	/**
	 * @brief helper method to build the graph traversal vector out of the
	 *        set of given nodes and their connection based on port links.
	 */
	bool UpdateTraversalVector();

	/**
	 * @brief helper method to build an outport-cache when building the traversal
	 *        structure. This map allows to lookup outports based on pointers
	 *        quicker than a node/port lookup. This is needed for establishing
	 *        a backlink structure for edge creation.
	 */
	bool UpdateOutportLookupMap();

	/**
	 * @brief create connect information based on port links, i.e., edges between
	 *        ports.
	 */
	bool UpdateEdgeMap();

	/**
	 * @brief calls PreparePorts() for all nodes in the traversal map.
	 * If a node reports that it can not be prepared, it is disabled.
	 */
	void PrepareAllPorts();

private:
	/**
	 * @brief retrieves or generates the outward connect information
	 *        for the given node and port.
	 */
	Connect &GetOrCreateAdjacencyListForNode( IVdfnNode *, IVdfnPort * );

	// all nodes contained in this graph
	Nodes      m_liNodes;
	// ordered list of nodes to be traversed
	NodeVec    m_vecTraversal;
	// edge / connection lookup table
	Edges      m_mpEdges;
	// cache to speed up lookup of outports during edge creation
	PortLookup m_mpPortLookup;

	// cache to process re-evaluation nodes as subgraphs
	std::map<IVdfnReEvalNode*, NodeVec> m_mpReEvalSubgraphs;

	// exports defined for this graph
	ExportList m_liExports;

	bool m_bUpToDate,       // flag to show dirty (false) / clean state (true)
		 m_bIsActive,       // global activation flag
	     m_bCheckMasterSim; // toggle to care for master sim evaluation
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNGRAPH_H
