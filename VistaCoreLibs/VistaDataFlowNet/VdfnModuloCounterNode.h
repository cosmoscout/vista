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


#ifndef _VDFNMODULOCOUNTERNODE_H
#define _VDFNMODULOCOUNTERNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnCounterNode.h"
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
 * @inport{modulo,T,optional,modulo value of the function}
 * @outport{out,T,current value of the counter}
 *
 * The ModuloCounterNode extents the functionality of the CounterNode by applying
 * a modulo-m operation, i.e. clamping the accumulated value to the range [0,m].
 * The modulo value can either be passed as ctor parameter, or adjusted dynamically
 * by the port "modulo".
 * @see TVdfnCounterNode
 */
template<class T>
class TVdfnModuloCounterNode : public TVdfnCounterNode<T>
{
public:
	TVdfnModuloCounterNode( T oInitialValue,
							T oModulo )
		: TVdfnCounterNode<T>( oInitialValue ),
		  m_pModulo( NULL ),
		  m_oModuloValue( oModulo )
	{
		this->RegisterInPortPrototype( "modulo", new TVdfnPortTypeCompare<TVdfnPort<T> > );
	}

	bool PrepareEvaluationRun()
	{
		// call super-class port-connect
		TVdfnCounterNode<T>::PrepareEvaluationRun();
		m_pModulo = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>( "modulo", this );
		return TVdfnCounterNode<T>::GetIsValid();
	}


protected:
	bool DoEvalNode()
	{
		// Check if modulo is set, and if so, use the inport value
		if( m_pModulo )
		{
			m_oModuloValue = m_pModulo->GetValue();
			if( m_oModuloValue == 0 )
				vstr::warnp() << "[VdfnModuloCounterNode] Encountered Modulo of 0!" << std::endl;
				return false;
		}

		//Check if a reset signal has arrived
		if( this->m_pReset && this->m_pReset->GetUpdateCounter() > this->m_iResetLastChange )
		{
			this->m_iResetLastChange = this->m_pReset->GetUpdateCounter();
			this->m_oValue = this->m_pReset->GetValue();
		}
		else
			this->m_oValue += this->m_pDelta->GetValue();

		//m_oValue = m_oValue - T( int( m_oValue / m_oModulo ) ) * m_oModulo;
		while( this->m_oValue >= m_oModuloValue )
			this->m_oValue -= m_oModuloValue;
		while( this->m_oValue < 0 )
			this->m_oValue += m_oModuloValue;
		this->m_pOut->SetValue( this->m_oValue, TVdfnModuloCounterNode<T>::GetUpdateTimeStamp() );
		return true;
	}

	/**
	 * To prevent that a change of the modulo inport triggers an evaluation
	 * and thus adds the current value of the Delta-Port more than once,
	 * we override CalcUpdateNeededScore to only regard the Delta-Port
	 */
	unsigned int CalcUpdateNeededScore() const
	{
		return this->m_pDelta->GetUpdateCounter();
	}

private:
	TVdfnPort<T>	*m_pModulo;
	T				m_oModuloValue;
};


/**
 * class template for a modulo node
 *
 * @inport{value,T,mandatory,inport for the value to set}
 * @inport{modulo,T,optional,modulo value of the function}
 * @outport{out,T,current modulo}
 *
 * The ModuloCounterNode extents the functionality of the CounterNode by applying
 * a modulo-m operation, i.e. clamping the accumulated value to the range [0,m].
 * The modulo value can either be passed as ctor parameter, or adjusted dynamically
 * by the port "modulo".
 * @see TVdfnCounterNode
 */
template<class T>
class TVdfnModuloNode : public IVdfnNode
{
public:
	TVdfnModuloNode( T oModulo )
		: IVdfnNode(),
		  m_pModuloPort( NULL ),
		  m_pInPort( NULL ),
		  m_pOutPort( new TVdfnPort<T> ),
		  m_nModuloValue( oModulo )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<T> > );
		RegisterInPortPrototype( "modulo", new TVdfnPortTypeCompare<TVdfnPort<T> > );
		RegisterOutPort( "out", m_pOutPort );
	}

	bool PrepareEvaluationRun()
	{
		m_pInPort = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>( "in", this );
		m_pModuloPort = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>( "modulo", this );
		return GetIsValid();
	}

	bool GetIsValid() const
	{
		return ( m_pInPort != NULL );
	}


protected:
	bool DoEvalNode()
	{
		// Check if modulo is set, and if so, use the inport value
		if( m_pModuloPort )
		{
			m_nModuloValue = m_pModuloPort->GetValue();
			if( m_nModuloValue <= 0 )
				vstr::warnp() << "[VdfnModuloCounterNode] Encountered Modulo <= 0!" << std::endl;
				return false;
		}

		T nValue = m_pInPort->GetValue();
		//m_oValue = m_oValue - T( int( m_oValue / m_oModulo ) ) * m_oModulo;
		while( nValue >= m_nModuloValue )
			nValue -= m_nModuloValue;
		while( nValue < 0 )
			nValue += m_nModuloValue;
		m_pOutPort->SetValue( nValue, GetUpdateTimeStamp() );
		return true;
	}

private:
	TVdfnPort<T>*	m_pInPort;
	TVdfnPort<T>*	m_pOutPort;
	TVdfnPort<T>*	m_pModuloPort;
	T				m_nModuloValue;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNMODULOCOUNTERNODE_H

