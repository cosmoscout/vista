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


#ifndef _RANDOMNUMBERDFNNODE_H
#define _RANDOMNUMBERDFNNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaRandomNumberGenerator;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * At each evaluation run of the interaction context, the node creates a new
 * Random Number and outputs it.
 * This node is a MasterSim node, meaning that in cluster mode, it is only
 * evaluated once on the master and is then transmitted to the clients,
 * ensuring that all nodes on all clients output the same number
 * (Note: With the used RandomNumberGenerator, this is not strictly necessary,
 * since it is already cluster-able, but we still use MasterSim as a Demo here :) )
 *
 * @outport{out,float}
 */
class RandomNumberDfnNode : public IVdfnNode
{
public:
	RandomNumberDfnNode( float fMin, float fMax );
	~RandomNumberDfnNode();

protected:
	virtual bool DoEvalNode();
	virtual bool GetIsMasterSim() const;

protected:
	float							m_fMin;
	float							m_fRange;

	TVdfnPort<float>*				m_pOut;
	VistaRandomNumberGenerator*		m_pRand;
};

/**
 * For each node, we want to know the min and max values between which the
 * random numbers lie. Therefore, the user can specify additional params
 * called 'min' and 'max' in the DFN graph file when creating the node.
 * We now have to tell how this data is created and how the actual node is
 * created, since we do not have a default ctor, but one with parameters.
 * For this, we create our own NodeCreator, which defines how to extract the
 * parameters and create the node
 */
class RandomNumberDfnNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	RandomNumberDfnNodeCreate();
	virtual ~RandomNumberDfnNodeCreate();

	virtual IVdfnNode* CreateNode( const VistaPropertyList& oPropertyList ) const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_RANDOMNUMBERDFNNODE_H

