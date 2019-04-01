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


#ifndef _VDFNCOUNTERNODE_H
#define _VDFNCOUNTERNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <iostream>

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
 * class template for a counter node
 *
 * @ingroup VdfnNodes
 * @inport{delta,T,mandatory,inport for the value to be added to the current count}
 * @inport{reset_to_value,T,optional,resets the counter to the value at the inport}
 * @outport{out,T,current value of the counter}
 *
 * creates a node that starts with an initial value (which is a parameter of the
 * ctor) and then successively adds the value from the port "delta" whenever it
 * changes. The accumulated value is passed on to the port "out". *
 */
template<class T>
class TVdfnCounterNode : public IVdfnNode
{
public:
	TVdfnCounterNode( T oInitialValue )
		: IVdfnNode(),
		  m_pDelta(NULL),
		  m_pReset(NULL),
		  m_pOut( new TVdfnPort<T> ),
		  m_oValue( oInitialValue ),
		  m_iResetLastChange( false )
	{
		RegisterInPortPrototype( "delta", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterInPortPrototype( "reset_to_value", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterOutPort( "out", m_pOut );
	}


	bool PrepareEvaluationRun()
	{
		m_pDelta = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>( "delta", this );
		m_pReset = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>( "reset_to_value", this );
		if( m_pReset )
			m_iResetLastChange = m_pReset->GetUpdateCounter();
		return GetIsValid();
	}

	bool GetIsValid() const
	{
		return ( m_pDelta != NULL );
	}

protected:
	bool DoEvalNode()
	{
		//Check if a reset signal has arrived
		if( m_pReset && m_pReset->GetUpdateCounter() > m_iResetLastChange )
		{
			m_iResetLastChange = m_pReset->GetUpdateCounter();
			m_oValue = m_pReset->GetValue();
		}
		else
			m_oValue += m_pDelta->GetValue();
		m_pOut->SetValue( m_oValue, GetUpdateTimeStamp() );
		return true;
	}
protected:
	TVdfnPort<T>	*m_pDelta;
	TVdfnPort<T>	*m_pReset;
	TVdfnPort<T>	*m_pOut;
	unsigned int	m_iResetLastChange;

	T				m_oValue;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNCOUNTERNODE_H

