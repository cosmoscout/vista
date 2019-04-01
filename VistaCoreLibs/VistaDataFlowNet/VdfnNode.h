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


#ifndef _VDFNNODE_H
#define _VDFNNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include <VistaAspects/VistaNameable.h>

#include <map>
#include <string>
#include <list>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVdfnPort;
class IVdfnPortTypeCompare;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * @defgroup VdfnNodes VdfnNodes
 * base class for nodes. the user can subclass this to define her own behaviour.
 * However, usually there are few methods of interest:
 *
 * -# PrepareEvaluationRun() is called by the graph <b>before</b> an eval run
      and after changes to the graph were done, so it can be assumed that upon
      a call to this method, nothing is certain ;) and every variable has to
      be re-assigned properly.
 * -# GetIsValid() usually sees whether all inports were connected. Overriding
      this can be useful in case the node offers optional nodes or outer
      constraints have to be obeyed. A node that is not valid is not evaluated,
      so the test should be rather quick, as the graph will poke on this frequently.
 * -# DoEvalNode() the actual computation, defining this is a must for new nodes.
 *
 * Nodes do not have default ports. Deleting a node will delete all resources
 * dealing with ports, including port-type compares.
 * Nodes can be dirty. In order to indicate their evaluation an internal score is
 * calculated by the node and queried by the graph upon traversal,
 * see CalcUpdateNeededScore()
 * @ingroup VdfnNodes
 */
class VISTADFNAPI IVdfnNode : public IVistaNameable
{
public:
	/**
	 * Releases memory of outports and inport-prototypes.
	 */
	virtual ~IVdfnNode();


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// NAMEABLE INTERFACE
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual std::string GetNameForNameable() const;
	virtual void   SetNameForNameable(const std::string &sNewName);

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// NODE EVALUATION STUFF
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * the user API to trigger evaluation. Checks the enabled state of this
	 * node and if this node is disabled it will return immediately.
	 * EXPERT API: although virtual, do not implement this in a derived class
	 * unless you have a specific reason to. To define the function of
	 * the nodes, use the DoEvalNode() function.
	 * @return true in case this node is disabled, and on unchanged input,
	           the value of DoEvalNode() else
	 */
	virtual bool EvalNode( double nTimeStamp );

	/**
	 * @return true iff this node was marked "dirty". If the node is not enabled,
	 * the method returns immediately. Else, it calls CalcUpdateNeededScore() which
	 * can be overwritten by specialized nodes to determine a new score that is
	 * compared to an old score and may trigger an evaluation on this node.
	 * @return true iff this node created an update score higher that its last score
	 * @see CalcUpdateNeededScore()
	 */
	bool NeedsEval() const;

	/**
	 * @return enable state of the graph. disabled nodes are not updated.
	 */
	bool GetIsEnabled() const;

	/**
	 * @param bEnabled the enable state of the node, disabled nodes are not updated
	 */
	void SetIsEnabled(bool bEnabled);

	/**
	 * Is called on graph activation. Can be used, for example to reset timers
	 * and other internal stuff.
	 * @param dTs the time stamp in seconds
	 */
	virtual void OnActivation( double dTs );

	/**
	 * Is called on graph deactivation. Can be used for example to stop threads
	 * and other internal stuff.
	 * @param dTs the time stamp in seconds.
	 */
	virtual void OnDeactivation( double dTs );

	/**
	 * The standard implementation counts whether all prototyped in ports are
	 * connected properly. As a result, only nodes that have all inports connected
	 * will function properly. If you have optional requirements, e.g., optional
	 * ports, overload this method and specify your constraint properly.
	 * @return false if this node's EvalNode() method is not to be called by client code
	           true iff this can be done without harm.
	 *
	 */
	virtual bool GetIsValid() const;

	/**
	 * Is called *before* an evaluation run from the graph. Can be used to cache
	 * inports to members or the like. Note that it is not useful to create
	 * or remove in- or outports during this method, as all connectivity is supposed
	 * to be fixed at that point.
	 * @return true iff anything is allright (usually the same as GetIsValid()), false
	           else.
	 */
	virtual bool PrepareEvaluationRun();

	/**
	 * API for dynamic nodes which may set or unset their in- or outports depending
	 * on external influence. This method is called by the IVdfnGraph before any edges
	 * will be applied and before PrepareEvaluationRun().
	 * This is considered to be an expert API, clients should normally not use this.
	 * The default implementation is empty (e.g. does nothing).
	 * @return true iff anything went allright, false iff this node is to be disabled.
	 */
	virtual bool PreparePorts();

	/**
	 * Reflects whether this node, when it is valid, is evaluated regardless of its update
	 * score state. Subclasses can specify the result by using the protected API
	 * SetEvaluationFlag(), e.g., during construction.
	 * @see SetEvaluationFlag()
	 */
	bool UnconditionalEvaluation() const;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// PORT-STUFF
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * Sets the inport with sName by the value of pPort. If the port with
	 * sName was assigned beforehand, this is silently overwritten.
	 * If sName does exist, its port type comparison operator is compared
	 * to the one of pPort. If this matches, the port is assigned as an inport.
	 * Overload this method if you need to assign special ports, or want to
	 * intercept the setting of a special port for additional manipulation.
	 * @return true iff an inport prototype with sName was registered and pPort
	           matches that type.
	 * @param sName the name of the port
	 * @param pPort a pointer to a port, can be null in order to erase an
	           existing port.
	 */
	virtual bool SetInPort(const std::string &sName, IVdfnPort *pPort);

	/**
	 * Clears an existing mapping. Only erases it, does not delete the port.
	 * When a port is removed, the internal update score calculation is triggered.
	 * @return true currently
	 */
	virtual bool RemInPort(const std::string &sName);

	/**
	 * Retrieve a name for an outport.
	 * @param pPort the port to lookup in the outport map
	 * @param strName the container to store the name to
	 * @return true iff the port exists as outport, false if this pPort could not
	           be retrieved from the set of outports.
	 * @see GetNameForInPort()
	 */
	bool GetNameForOutPort( IVdfnPort *pPort, std::string &strName ) const;

	/**
	 * Retrieves an inport by name.
	 * @return NULL if no inport was connected using the sName
	 */
	IVdfnPort *GetInPort(const std::string &sName) const;

	/**
	 * Gets the names of all inports that possibly <b>can</b> be connected.
	 * This means it traverses over the list of prototypes for the inports.
	 * The list is sorted lexicographically, so this may be an expensive call.
	 * @see GetConnectedInPortNames()
	 */
	std::list<std::string> GetInPortNames() const;

	/**
	 * Gets the names of all inports that <b>are</b> connected.
	 * This means it traverses over the list of inports.
	 * The list is sorted lexicographically, so this may be an expensive call.
	 * @see GetConnectedInPortNames()
	 */
	std::list<std::string> GetConnectedInPortNames() const;

	/**
	 * @return the number of really connected inports
	 * @see GetInPortPrototypeCount()
	 */
	unsigned int GetInPortCount() const;

	/**
	 * @return thr number of inports that <b>can</b> be connected.
	 * @see GetInPortCount()
	 */
	unsigned int GetInPortPrototypeCount() const;

	/**
	 * @param the portname, case-sensitive string
	 * @return an outport named sName or NULL
	 */
	IVdfnPort *GetOutPort(const std::string &sName) const;

	/**
	 * @return get a list of all outport port names
	 */
	std::list<std::string> GetOutPortNames() const;

	/**
	 * @return the number of outports of this node.
	 */
	unsigned int GetOutPortCount() const;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// PORT SUB -- TYPE CHECKING
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * Checks whether an inport with name sPortName exists and
	 * has a comparable type to pPortToCheck. If an inport with sPortName
	 * exists <b>and</b> this type compare operator matches the one of
	 * pPortToCheck, this method returns true.
	 */
	bool GetIsValidInPort( const std::string &sPortName, IVdfnPort *pPortToCheck ) const;

	/**
	 * Can be used to query the porttype-compare operator for a given port name.
	 * If this port name does not exist, a nil-port type compare is returned that will
	 * always fail to compare to any other port type.
	 */
	const IVdfnPortTypeCompare &GetPortTypeCompareFor( const std::string &sPortName ) const;

	/**
	 * checks whether this node has a prototype registered for strInPortName,
	 * it will *not* check whether this port is actually set. Use GetInPort()
	 * for that and test against NULL
	 * @see GetInPort()
	 * @see GetPortTypeCompareFor()
	 * @param strInPortName the name for the inport to check
	 * @return true when a port type compare exists for this node and port strInPortName
	 */
	bool GetHasInPort( const std::string &strInPortName ) const;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CLUSTER STUB
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * Overload this method if you want all the ports of this node to be only
	 * calculated on the master node of a cluster setup. This is quite a small
	 * hack, usually totally uninteresting for most node writers. If this
	 * node returns true here, then in a clustered setup, the node's ports
	 * will be serialized and distributed. If it returns false, this node will
	 * be calculated on the slave nodes, and not on the master. Think of this when
	 * dealing with
	 * - time
	 * - random numbers
	 * - local resources, e.g., local windows, devices and stuff only present on the master node
	 * @return true iff the port values are only to be calculated on the master node
	 */
	virtual bool GetIsMasterSim() const { return false; }

	// TAG API for cluster hooks
	/**
	 * sets up a symbolic tag, which users can use to search for specific nodes in a graph.
	 * This is usually filled by code, such as the persistence layer of the DFN.
	 * The value of the tag is of no meaning for this node, it is only stored.
	 * @param strTag a symbol to be set in order to find a specific node by this tag
	 */
	void SetUserTag( const std::string &strTag );

	/**
	 * returns the user tag for this node.
	 * @param strTag the container string to copy the tag in
	 * @return false iff no tag was set (or empty string)
	 */
	bool GetUserTag( std::string &strTag ) const;

	/**
	 * returns whether the user tag is the empty string.
	 * @return true iff the user tag is the empty string
	 */
	bool GetHasUserTag() const;


	/**
	 * the type tag can be used to 'link' the IVdfnNode-C++ type to the type string
	 * that was used for the node creator of this node. The VdfnPersistence class
	 * writes the type tag upon successful creation of the node.
	 * This can be useful if you want to search a node using the user-type during creation
	 * @param strTypeTag the case sensitive type tag for this node
	 */
	void SetTypeTag( const std::string &strTypeTag );

	/**
	 * returns the type tag that was probably set upon creation, indicating the
	 * type string that was used for the node creator of this node.
	 * the VdfnPersistence class makes use of this during creation.
	 */
	std::string GetTypeTag() const;


	/**
	 * this tag can be used to associated this node with a group of other nodes.
	 * This might be handy in some situations. The tag is only stored by the node,
	 * not evaluated.
	 * @param strGroupTag the case-sensitive group tag for this node
	 */
	void SetGroupTag( const std::string &strGroupTag );

	/**
	 * returns the group tag that was given to the node using the SetGroupTag() API.
	 * @see SetGroupTag()
	 * @return the group tag set or the empty string.
	 */
	std::string GetGroupTag() const;




	/**
	 * Print debug info about the node
	 */
	void PrintInfo( std::ostream& oStream ) const;


	/**
	 * get the update time stamp for this node
	 * @return the last timestamp of a value change
	 */
	double GetUpdateTimeStamp() const;
protected:
	IVdfnNode();

	/**
	 * @return false <b>only</b> if a severe flaw is present and the evaluation of
	           the <i>complete</i> graph is to be stopped at this node! so usually
	           one should return true here.
	 */
	virtual bool   DoEvalNode() = 0;


	/**
	 * inports are 'declared' by a node using a prototype (in the form of a
	 * IVdfnPortTypeCompare) and a name. If a port is set using SetInPort(),
	 * its type compare is evaluated with the one given here for the port
	 * with the name sName. Note that a second assignment overwrites
	 * the old one without freeing memory.
	 * @param sName the port name to declare (case-sensitive)
	 * @param pCp the port type compare to use for the port
	 * @return true
	 */
	bool RegisterInPortPrototype( const std::string &sName, IVdfnPortTypeCompare *pCp );

	/**
	 * this method is used to check whether pNodeToCheck has a matching port-type
	 * compare to be registered under the name sName. The method first checks whether
	 * sName is a valid inport, and then checks the type using the IVdfnPortTypeCompare
	 * for this port and the same for the pNodeToCheck.
	 * @param the port to check for (case-sensitive)
	 * @param pNodeToCheck the node that wants to be assigned as inport on sName
	 * @return true if type matches and sName is a valid port, false else
	 */
	IVdfnPortTypeCompare *CheckInPortRegistration( const std::string &sName, IVdfnPort *pNodeToCheck ) const;

	/**
	 * outports are provided by every node, so allocate some memory for it and
	 * pass it this method to manage the outport. Outports are freed upon destruction
	 * of the node. A second assignment to the same name will overwrite the old one
	 * silently, no memory freed
	 * @param sName the name of the port to register
	 * @param pPort the port to use for this outport
	 * @return true
	 */
	bool RegisterOutPort( const std::string &sName, IVdfnPort *pPort );


	/**
	 * this actually forces to set port pPort under sName, no further check.
	 * use it with care as a subclass (better: do not use it). In debug mode,
	 * this method checks whether a port was already assigned.
	 * @todo check whether it is meaningful to make this API private
	 * @param sName the portname to set pPort to as inport
	 * @param pPort the port to set
	 */
	bool DoSetInPort( const std::string &sName, IVdfnPort *pPort );


	/**
	 * sets the update time stamp. the time stamp is only stored, not
	 * evaluated in any sense.
	 * @see GetUpdateTimeStamp()
	 * @param nTs the timestamp to set
	 */
	void   SetUpdateTimeStamp(double nTs);

	/**
	 * calculates the update score for this node. In case of an update, this
	 * method should return a monotonic growing value, that is: in case there is
	 * not change, two subsequent calls to CalcUpdateNeededScore() result in
	 * the same value. In case there is a change, then score_n > score_{n-1}.
	 * The method is called by NeedsEval(), which in turn is called by the
	 * IVdfnGraph during update traversal.
	 * The default implementation simply sums up the revision stamp of each
	 * inport. When a port has changed a value, the new sum is greater than the
	 * old one.
	 * @return the update score for this node
	 * @todo think about overflow of the return value in a larger context
	 */
	virtual unsigned int    CalcUpdateNeededScore() const;

	/**
	 * returns the current update count, which is incremented upon every call
	 * to EvalNode() that results in a successful DoEvalNode() call.
	 * @return the update count for this node
	 * @todo think about making this public, might be nice for debugging/GUI
	 */
	unsigned int GetUpdateCount() const;

	/**
	 * subclasses can decide to cause an unconditional node update, regardless
	 * of the update score. This will result in a node that has its DoEvalNode()
	 * method called upon every single traversal of the graph.
	 * A timer node, for example, might make use of this.
	 * @param bDoUnconditionalUpdate true if you want to be evaluated every time the
	          graph does an update traversal, false else.
	 */
	void SetEvaluationFlag( bool bDoUnconditionalUpdate );

	typedef std::map<std::string, IVdfnPort*> PortMap;
	typedef std::map<std::string, IVdfnPortTypeCompare*> TypeMap;


	/**
	 * protected inport map, use with care
	 */
	PortMap m_mpInPorts;

	/**
	 * protected outport map, use with care
	 */
	PortMap m_mpOutPorts;

	/**
	 * protected prototype map, use with care
	 */
	TypeMap m_mpInPrototypes;

private:
	double           m_nLastUpdate;
	unsigned int     m_nUpdateCount;
	bool m_bUnconditionalUpdate,
	     m_bEnabled;
	std::string m_strNodeName,
	            m_strUserTag,
	            m_strTypeTag,
	            m_strGroupTag;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNNODE_H

