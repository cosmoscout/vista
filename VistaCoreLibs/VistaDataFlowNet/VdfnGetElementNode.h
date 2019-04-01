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


#ifndef _VDFNGETELEMENTNODE_H
#define _VDFNGETELEMENTNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
#include "VdfnUtil.h"

#include <VistaBase/VistaStreamUtils.h>

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
 * The GetElementNode returns the element from a Container object (e.g. vector)
 * or an array-based object (like VistaVector3D), and returns 
 *
 * @ingroup VdfnNodes
 * @inport{in,type container<T>,mandatory, container from which to retrieve the element}
 * @inport{index,type T,optional, index to be retrieved}
 * @outport{out,type T, element with index}
 */

// for containers
template<class Container, class Type>
class TVdfnGetElementNode : public IVdfnNode
{
	public:
	TVdfnGetElementNode( const int iIndex )
	: IVdfnNode()
	, m_pOutPort( new TVdfnPort<Type> )
	, m_pInPort( NULL )
	, m_pIndexPort( NULL )
	, m_nDefaultIndex( iIndex )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<Container> > );
		RegisterInPortPrototype( "index", new TVdfnPortTypeCompare<TVdfnPort<int> > );
		RegisterOutPort( "out", m_pOutPort );
	}

	~TVdfnGetElementNode()
	{
	}

	bool PrepareEvaluationRun()
	{
		m_pInPort = VdfnUtil::GetInPortTyped<TVdfnPort<Container>*>( "in", this );
		m_pIndexPort = VdfnUtil::GetInPortTyped<TVdfnPort<int>*>( "index", this );
		return GetIsValid();
	}

	bool GetIsValid() const
	{
		if( m_pInPort == NULL )
			return false;
		if( m_nDefaultIndex < 0 && m_pIndexPort == NULL )
			return false;
		return true;
	}
protected:
	bool DoEvalNode()
	{
		int sIndex;
		if( m_pIndexPort == NULL )
			sIndex = m_nDefaultIndex;
		else
			sIndex = m_pIndexPort->GetValue();

		const Container& oContainer = m_pInPort->GetValueConstRef();

		if( sIndex < 0 || sIndex >= (int)oContainer.size() )
		{
			vstr::errp() << "[TVdfnGetElementNode]: Index [" << sIndex << "] out of bounds"
				<< std::endl;
			return true;
		}

		m_pOutPort->SetValue( Type( oContainer[sIndex] ), GetUpdateTimeStamp() );
		return true;
	}
private:

	int				m_nDefaultIndex;
	TVdfnPort<Type>*	m_pOutPort;
	TVdfnPort<Container>*	m_pInPort;
	TVdfnPort<int>*	m_pIndexPort;
};

template<class Container, class Type, int Size>
class TVdfnGetElementFromArrayNode : public IVdfnNode
{
	public:
	TVdfnGetElementFromArrayNode( const int iIndex )
	: IVdfnNode()
	, m_pOutPort( new TVdfnPort<Type> )
	, m_pInPort( NULL )
	, m_pIndexPort( NULL)
	, m_nDefaultIndex( iIndex )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<Container> > );
		RegisterInPortPrototype( "index", new TVdfnPortTypeCompare<TVdfnPort<int> > );
		RegisterOutPort( "out", m_pOutPort );
	}

	~TVdfnGetElementFromArrayNode()
	{
	}

	bool PrepareEvaluationRun()
	{
		m_pInPort = VdfnUtil::GetInPortTyped<TVdfnPort<Container>*>( "in", this );
		m_pIndexPort = VdfnUtil::GetInPortTyped<TVdfnPort<int>*>( "index", this );
		return GetIsValid();
	}

	bool GetIsValid() const
	{
		if( m_pInPort == NULL )
			return false;
		if( m_nDefaultIndex < 0 && m_pIndexPort == NULL )
			return false;
		return true;
	}
protected:
	bool DoEvalNode()
	{
		int sIndex;
		if( m_pIndexPort == NULL )
			sIndex = m_nDefaultIndex;
		else
			sIndex = m_pIndexPort->GetValue();

		const Container& oContainer = m_pInPort->GetValueConstRef();

		if( sIndex < 0 || sIndex >= Size )
		{
			vstr::errp() << "[TVdfnGetElementNode]: Index [" << sIndex << "] out of bounds"
				<< std::endl;
			return true;
		}

		m_pOutPort->SetValue( Type( oContainer[sIndex] ), GetUpdateTimeStamp() );
		return true;
	}
private:

	int				m_nDefaultIndex;
	TVdfnPort<Type>*	m_pOutPort;
	TVdfnPort<Container>*	m_pInPort;
	TVdfnPort<int>*	m_pIndexPort;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNGETELEMENTNODE_H

