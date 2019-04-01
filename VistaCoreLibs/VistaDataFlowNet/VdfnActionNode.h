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


#ifndef _VDFNACTIONNODE_H
#define _VDFNACTIONNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnShallowNode.h"
#include "VdfnPortFactory.h"
#include "VdfnPort.h"
#include "VdfnActionObject.h"
#include "VdfnNodeFactory.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VdfnObjectRegistry;
class IVdfnActionObject;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the action node is used to attach it to IVdfnActionObjects. This allows to
 * use all getters of an IVdfnActionObject to use as outports and all the setters
 * of an IVdfnActionObject as inports automatically. There is a precondition on the
 * types that are used at the interface of an IVdfnActionObject.
 * -# the get/set method can only have <b>1</b> argument
 * -# the type of the argument is in principle user definable. however,
      new types have to be registered with the VdfnPortFactory.
 * -# an IVdfnActionObject::IActionSet has to be defined and registered with
      the VdfnPortFactory for SET functionality
 * -# an IVdfnActionObject::IActionGet has to be defined and registered with
      the VdfnPortFactory for GET functionality
 * -# a VdfnPortFactory::CFunctorAccess has to be defined for SET and GET
      access
 * -# the IVdfnActionObject has to be registered with a unique name as an
      IVistaNameable in the VdfnObjectRegistry at the time of the creation
      of this node.
 *
 * The mapping from name to object is determined once during the construction of
 * the node, no way to change it later. The node will register itself as an
 * observer to the IVdfnActionObject, so deleting the instance is safe in the
 * sense that the node processing will not crash the application, but then, there
 * is no way to re-activate the node (other than plugging it by hand and replace
 * it).
 * More things to look out for.
 * -# the ports are created during the VdfnGraph::PreparePorts() run, which is
      a special update before the run to IVdfnNode::PrepareEvaluationRun() call.
      Only <b>after</b> that call, any inports or outports exist.
 * -# port names co-respond to the property names of the IVdfnActionObject property
      names.
 * -# evaluation will <i>first</i> inspect new values with the setters and <i>then</i>
      propagate new values to its outports. Seems natural to you, but can lead to
      non-intuitive situations when you have two VdfnActionNode instances talking about
      the same IVdfnActionObject in the same VdfnGraph object, as there is a natural
      but non-obvious dependency between them.
 *
 * This module defines the node creator for an VdfnActionNode. It takes a single argument
 * - <b>object</b> which is the name of the action object to search for in the registry
     (case sensitive, of course).
 *
 * @ingroup VdfnNodes
 */
class VISTADFNAPI VdfnActionNode : public VdfnShallowNode
{
public:
	/**
	 * this constructor can be used when the IVdfnActionObject is not yet available.
	 * Getting a reference to the IVdfnActionObject is deferred to the PreparePorts() call,
	 * grabbing it directly from the VdfnObjectRegistry using the strKey arguments
	 * as a IVistaNameable instance.
	 * @param pReg the object registry to search for an IVdfnActionObject during PreparePorts().
	               may not be NULL
	 * @param strKey the plain name to look for (case-sensitive) for an IVistaNameable,
	               should not be empty (unless you have registered an object without a name)
     * @param pActionObject (optional) instane of the action object to use initially
	 */
	VdfnActionNode( VdfnObjectRegistry *pReg,
		             const std::string &strKey,
					 IVdfnActionObject *pActionObject = NULL );

	///**
	// * if you have a grip at the action object already, use this constructor, during PreparePorts()
	// * only the input and output ports are constructed according the the property mapping
	// * and the availability of correct creation and access instances in the VdfnPortFactory.
	// */
	//VdfnActionNode( IVdfnActionObject *pActionObject );

	/**
	 * detaches the internal observer from the action object (if it was attached to one).
	 */
	virtual ~VdfnActionNode() ;

	/**
	 * the node is valid as soon as the IVdfnActionObject is set. A node that is not
	 * attached to an IVdfnActionObject is not valid.
	 */
	bool GetIsValid() const;

	/**
	 * this method tries to do a deferred lookup of the IVdfnActionObject in the
	 * VdfnObjectRegistry in the case it was not attached, yet. This method is mainly
	 * useful for the VdfnGraph initializing the graph.
	 * @return false when no action object was attached at the time of call AND
	           no nameable that was down-castable to an IVdfnActionObject interface
	           was found in the registry; true else
	 */
	bool PreparePorts();

	/**
	 * the method tries to minimize the number of ports in fact attached to <i>only</i>
	 * the subset that was actually connected to the node during construction phase.
	 * For example, if an IVdfnActionObject has two SET properties <b>A</b> and <b>B</b> and
	 * the graph only connects <b>A</b>, then <i>only<i> one setter inport will be created,
	 * named 'A'.
	 */
	bool PrepareEvaluationRun();

	/**
	 * Reloads the action object with the original name, in case it has changed
	 */
	bool ReloadActionObject();

protected:
	/**
	 * the method tries to determine the need for an update of the node based on
	 * -# the usual update scoring from the superclass
	 * -# it increases score in case the IVdfnActionObject::Pull() method returns true
	      (in that case an update is triggered). This can be used to reflect a state
	      change that was not reflected by a 'set' of a client, but is determined by the
	      IVdfnActionObject itself.
	 * -# in case the IVdfnActionObject notified the node as part of a SET, the update
	      score is increased.
	 */
	virtual unsigned int    CalcUpdateNeededScore() const;

	/**
	 * evaluates setter (inports) first, then getter (outports). It will only forward
	 * changes based on the revision flag, so if the IVdfnActionObject decides to push
	 * the same value with a new revision, an update is triggered.
	 * @return true
	 */
	bool DoEvalNode();

private:
	/**
	 * private utility method for port mapping and construction. It uses the VdfnPortFactory
	 * singleton.
	 * @return true
	 */
	bool UpdatePorts();

	struct _sHlp
	{
	public:
		_sHlp()
			: m_pPort(NULL),
			  m_pSet(NULL),
			  m_pFunctor(NULL),
			  m_nRevision(0)
		{
		}

		_sHlp( IVdfnPort *pPort,
			   VdfnPortFactory::CPortGetFunctor *pSet,
			   IVdfnActionObject::IActionSet *pFunc)
			   : m_pPort(pPort), m_pSet(pSet),
			   m_pFunctor(pFunc),
			   m_nRevision(0)
		{}

		IVdfnPort                         *m_pPort;
		VdfnPortFactory::CPortGetFunctor *m_pSet;
		IVdfnActionObject::IActionSet *m_pFunctor;
		unsigned int m_nRevision;
	};


	std::map<std::string, _sHlp> m_mpBuildMap;
	std::list<_sHlp> m_liPortSets;
	IVdfnActionObject *m_pActionObject;
	VdfnObjectRegistry *m_pReg;
	std::string m_strKey;

	struct _sOutHlp
	{
	public:
		_sOutHlp() {}
		_sOutHlp( const std::string &strPortName,
			VdfnPortFactory::CPortGetFunctor *pF,
			IVdfnActionObject::IActionGet *pGet )
			: m_pSet(pF),
			  m_pGet(pGet),
			  m_strPortName(strPortName)
		{
		}


		VdfnPortFactory::CPortGetFunctor *m_pSet;
		IVdfnActionObject::IActionGet     *m_pGet;
		std::string m_strPortName;
	};


	mutable unsigned int m_nCount;
	mutable unsigned int m_nInCnt;
	std::list<_sOutHlp> m_mpOutMap;

	std::string m_sReflectionableType;

	class _cActObserver;
	_cActObserver *m_pObs;
};


/**
 * creates a VdfnActionNode
 */
class VISTADFNAPI VdfnActionNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	/**
	 * the registry must outlive the node creator, as well as all the nodes
	 * that were created and not destroyed. In short: it should live throughout
	 * the application run :)
	 * @param pReg a pointer to the object registry to search action objects in
	 */
	VdfnActionNodeCreate( VdfnObjectRegistry *pReg );

	/**
	 * creates a VdfnActionNode, accepts
	 * - object: a case-sensitive name for a nameable to search in the registry
	 * @return something not null, action nodes that can not be bound to an
	           action object immediately search for it later.
	 */
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
public:
	VdfnObjectRegistry *m_pReg;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

