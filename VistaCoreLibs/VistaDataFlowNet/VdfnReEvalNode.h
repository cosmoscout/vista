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


#ifndef _VDFNREEVALNODE_H
#define _VDFNREEVALNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnConfig.h"
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
 * ReEvalNode offers an interface for nodes that can be evaluated more than
 * once per Graph traversal. The Node can signal that he needs another
 * evaluation run by returning bool from the GetNeedsReEvaluation function.
 * During a reevaluation, only the subtree of the reevaluation node is traversed,
 * other nodes stay passive.
 * To create a new instance, the necessary functions to implement are
 * DoEvalNode(): Defines the action of the node (see IVdfnNode)
 * GetNeedsReEvaluation(): To determine if another evaluation run is needed this frame
 * Other functions of interest:
 * PrepareEvaluationRun() : (see IVdfnNode)
 * GetIsValid() : (see IVdfnNode)
 * @ingroup VdfnNodes
 */
class VISTADFNAPI IVdfnReEvalNode : public IVdfnNode
{
public:
	IVdfnReEvalNode();
	~IVdfnReEvalNode();

	/**
	 * Extents IVdfnNode::NeedsEval to special check whether or not a
	 * ReEvaluation run is required( which is determined by the
	 * GetNeedsReEvaluation() function.
	 * @see GetNeedsReEvaluation()
	 */
	virtual bool NeedsEval() const;


	/**
	 * EvalNode Re-implementation, which allows to handle specifics
	 * of the reevaluation call. Should not be implemented by derived classes.
	 * Overloading this API is on expert level, so normal users should not touch
	 * it, but overload DoEvalNode()
	 * @see DoEvalNode()
	 */
	virtual bool EvalNode( double nTimeStamp );

protected:
	/**
	 * Implement as for a normal IVdfnNode, but keep in mind that it is
	 * called once during normal graph traversal, and then repeatedly until
	 * GetNeedsReEvaluation returns false. During reevaluation, the input
	 * ports don't change
	 */
	virtual bool DoEvalNode() = 0;

	/**
	 * Implement this function to tell whether or not another evaluation run is needed.
	 * @see NeedsEval()
	 */
	virtual bool GetNeedsReEvaluation() const = 0;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNREEVALNODE_H

