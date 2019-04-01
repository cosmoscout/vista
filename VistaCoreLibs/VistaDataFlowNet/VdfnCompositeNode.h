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


#ifndef _VDFNCOMPOSITENODE_H__
#define _VDFNCOMPOSITENODE_H__


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnNodeFactory.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VdfnGraph;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the composite node can be used to calculate complete graphs, looking as a
 * single node to the outside user. It will take a list of exports from a graph
 * to operate correctly. In-Exports will be used as inports, while Out-Exports
 * will be used as outports of this node. The ports will be duplicated from
 * nodes in the graph, so outside nodes will write directly to the nodes in the
 * composite's graph. This is clean, and fast as it avoids copies to be passed
 * along the boundary.
 * - activation/deactivation will be forwarded
 * - PrepareEvaluationRun() will call evaluate(0) on the graph, thus
     initializing it for the first time.
 * - nodes and ports will be assigned directly in the constructor,
     so more advanced node magic might fail to work
 * - GetIsValid() will return whether a graph is set or not
 * - DoEvalNode() will call Evaluate() on the graph, should write all results
     to the assigned outports.
 *
 * @ingroup VdfnNodes
 */
class VISTADFNAPI VdfnCompositeNode : public IVdfnNode
{
public:
	/**
	 * provide a VdfnGraph with proper exports.
	 * - direction in exports will get inports
	 * - direction out exports will get outports
	 * @param pGraph a non NULL pointer to a graph to use for evaluation
	 */
	VdfnCompositeNode( VdfnGraph *pGraph );

	~VdfnCompositeNode();


	/**
	 * forwards event to graph
	 * @param dTs the current timestamp
	 */
	virtual void OnActivation( double dTs );

	/**
	 * forwards event to graph
	 * @param dTs the current timestamp
	 */
	virtual void OnDeactivation( double dTs );

	/**
	 * overrides the default implementation, checks whether it can find
	 * sName as an inport (is determined by the direction-in ports of the
	 * graph's export declaration and the there given mapping), if one is found,
	 * it will forward the port to the co-responding graph node and set the
	 * port directly with that node.
	 * @param sName the name of the port to set
	 * @param pPort a pointer to the port to set (not-NULL)
	 */
	virtual bool SetInPort(const std::string &sName, IVdfnPort *pPort);

	/**
	 * @return this node is valid when the graph is set during construction
	 */
	bool GetIsValid() const;

	/**
	 * calls Evaluate(0) on the graph
	 * @return GetIsValid()
	 */
	bool PrepareEvaluationRun();


	/**
	 * the value of this property is evaluated upon a call to CreatePorts()
	 * which in turn is called upon creation.
	 * @return true when any of the OUTPORTS is connected to a master-sim node
	 */
	virtual bool GetIsMasterSim() const;

	const VdfnGraph *GetGraph() const;

protected:
	/**
	 * forwards to VdfnGraph::EvaluateGraph()
	 * @return the return value VdfnGraph::EvaluateGraph() for the composite's graph
	 */
	bool DoEvalNode();

private:
	virtual bool CreatePorts();
	VdfnGraph *m_pGraph;

	std::list<VdfnGraph::ExportData> m_liExports;
	typedef std::map<std::string,VdfnGraph::ExportData> NAMEMAP;
	NAMEMAP m_InportMap;
	bool m_bContainsMasterSim;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif
