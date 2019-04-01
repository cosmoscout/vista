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


#ifndef _VDFNDIFFERENCENODE_H
#define _VDFNDIFFERENCENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"
#include "VdfnUtil.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * class template a node that calculates the difference between inputs of
 * (numerical) values to the inport between several evaluations.
 * Thus, it is like a combination of a buffer node and a minus node.
 *
 *
 * @ingroup VdfnNodes
 * @inport{in, T, mandatory, input value }
 * @outport{out, T, difference between input value and last input value}
 *
 * Conversion using this node is done by applying a user defined conversion
 * function from tFrom to tTo. In case users want a different conversion, it
 * is suggested to re-instatiate this template using a different conversion
 * function during construction and re-register the node type using a different
 * name. The conversion function can be free floating function and is performed
 * by a copy-in, so it can be expensive.
 */
template<class T>
class TVdfnDifferenceNode : public IVdfnNode
{
public:
	TVdfnDifferenceNode()
	: IVdfnNode()
	, m_pInport(NULL)
	, m_pOutport( new TVdfnPort<T>() )
	, m_nLastValue( 0 )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare< TVdfnPort<T> >);
		RegisterOutPort( "out", m_pOutport );
	}

	~TVdfnDifferenceNode() {}

	bool PrepareEvaluationRun()
	{
		m_pInport = VdfnUtil::GetInPortTyped< TVdfnPort<T>* >( "in", this );
		return GetIsValid();
	}
protected:
	bool DoEvalNode()
	{
		T nNewValue = m_pInport->GetValue();
		m_pOutport->SetValue( nNewValue - m_nLastValue, GetUpdateTimeStamp() );
		m_nLastValue = nNewValue;
		return true;
	}
private:
	TVdfnPort<T>* m_pInport;
	TVdfnPort<T>* m_pOutport;
	T m_nLastValue;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNDIFFERENCENODE_H

