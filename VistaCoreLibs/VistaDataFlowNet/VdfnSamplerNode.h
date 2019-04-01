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


#ifndef _VDFNSAMPLERNODE_H
#define _VDFNSAMPLERNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnSerializer.h"
#include "VdfnShallowNode.h"
#include "VdfnNodeFactory.h"
#include "VdfnPort.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#include <map>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the sampler node forwards <b>all</b> inports as outports with the same name
 * and type. It can be configured to forward under two conditions
 * - MD_TRIGGER_OR: forward on change of <i>any</i> inport (default)
 * - MD_TRIGGER_AND: forward on change of <i>all</i> inports
 *
 * The node can be configured to react on a subset of inports for activating
 * (or firing) the sampling process, given a <i>triggerlist</i> during
 * construction. If this list is empty, all inports will be inspected.
 * The AND mode will inspect the revisions of the inports and fire as soon
 * as all revisions have changed. This means that more than one evaluation can
 * or will be needed to accumulate to an outport change. In case of a fire on
 * the outports, there may be old values.
 * All values are <i>copied</i> from inport to outport.
 *
 * @ingroup VdfnNodes
 * @inport{many,any,accepts all port types and names}
 * @outport{many,any,forwards all assigned inports in type and name}
 */
class VISTADFNAPI VdfnSamplerNode : public VdfnShallowNode
{
public:
	/**
	 * @param strTriggerList the names of the ports (case-sensitive)
	           that will be inspected for firing, depending on the
	           mode flag.
	 * @see GetTriggerMode()
	 * @see SetTriggerMode()
	 */
	VdfnSamplerNode(const std::list<std::string> &strTriggerList);
	virtual ~VdfnSamplerNode();

	/**
	 * intercepts a set-port to clone it and assigne as an outport called
	 * sName.
	 * @param sName the inport name to set (will result in an outport set
	          with the same name)
	 * @param pPort the port to set
	 * @return the value of VdfnShallowNode::SetInPort(), can be false
	           when a call to IVdfnPort::Clone() fails for pPort.
	 */
	virtual bool SetInPort(const std::string &sName,
						   IVdfnPort *pPort );

	/**
	 * creates the revision maps for sampling updates. In case the triggerlist
	 * given during construction was empty, <b>all</b> set inports will be used
	 * as triggerlist.
	 * @return VdfnShallowNode::PrepareEvaluationRun()
	 */
	bool PrepareEvaluationRun();

	enum eMd
	{
		MD_NONE=-1, /**< state undefined, not good... */
		MD_TRIGGER_OR = 0, /**< (default) one port change causes sampling at the outports */
		MD_TRIGGER_AND  /**< sampling is delayed until <b>all</b> inports have changed revision */
	};

	/**
	 * @return the current trigger mode
	 */
	eMd GetTriggerMode() const;

	/**
	 * @param md set the current trigger mode
	 */
	void SetTriggerMode( eMd md );

protected:
	bool DoEvalNode();
	virtual unsigned int    CalcUpdateNeededScore() const;
private:
	typedef std::map<IVdfnPort*, unsigned int> REVMAP;

	mutable REVMAP m_mpRevisions;
	mutable unsigned int m_nUpdateScore;

	std::list<std::string> m_liTriggerList;
	eMd m_eMd;
};


/**
 * creates a VdfnSamplerNode
 */
class VISTADFNAPI VdfnSamplerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	/**
	 * accepts
	 * - triggerlist: a list of strings, port names to assign as triggers.
	                  can be empty, defaults to <i>all</i>
	 * - more: anything else than 'and' (case-sensitive) will result in 'or'
	 */
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNSAMPLERNODE_H

