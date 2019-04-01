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


#ifndef _VDFNVARIABLENODE_H
#define _VDFNVARIABLENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"

#include <VistaAspects/VistaConversion.h>


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
 * node that grants read and write access to a named variable that has a con-
 * sistent value across all DFN graphs.
 * @TODO think about a more general approach (Vista-wide, allowing access to 
 *       vars from the application), maybe intergrate in DfnObjectRegistry
 *
 * @ingroup VdfnNodes
 * @inport{value, T, optional, sets the variable's value}
 * @outport{value, T, optional, gets the variabe's value}
 */
template<typename T>
class TVdfnTypedVariableRegistry
{
public:
	class Variable
	{
	public:
		Variable()
		: m_oValue( VistaConversion::CreateInitializedDefaultObject<T>() )
		, m_nUpdateCount( 0 )
		{
		}

		T Get() const
		{
			return m_oValue;
		}
		void Set( const T& oValue )
		{
			m_oValue = oValue;
			++m_nUpdateCount;
		}
		unsigned int GetUpdateCount()
		{
			return m_nUpdateCount;
		}
		std::string GetType() const
		{
			return VistaConversion::GetTypeName<T>();
		}
	private:
		T m_oValue;
		unsigned int m_nUpdateCount;
	};

	Variable* GetVariable( const std::string& sName ) const
	{
		typename VariableMap::const_iterator it = m_mapVariables.find( sName );
		if( it == m_mapVariables.end() )
			return NULL;
		else
			return (*it).second;

	}
	Variable* GetOrCreateVariable( const std::string& sName, const T& nInitialValue )
	{
		typename VariableMap::iterator it = m_mapVariables.find( sName );
		if( it != m_mapVariables.end() )
			return &(*it).second;
		Variable* pVar = &m_mapVariables[sName];
		pVar->Set( nInitialValue );
		return pVar;
	}
	Variable* GetOrCreateVariable( const std::string& sName )
	{
		return GetOrCreateVariable( sName, VistaConversion::CreateInitializedDefaultObject<T>() );
	}
	Variable* SetVariable( const std::string& sName, const T& oValue )
	{
		Variable* pVar = m_mapVariables[sName];
		pVar->Set( oValue );
		return pVar;
	}

private:
	typedef typename std::map<std::string, Variable> VariableMap;
	VariableMap m_mapVariables;
};

template<typename T>
class TVdfnSetVariableNode : public IVdfnNode
{
public:
	typedef typename TVdfnTypedVariableRegistry<T>::Variable Variable;

	TVdfnSetVariableNode( Variable* pVariable )
	: IVdfnNode()
	, m_pVariable( pVariable )
	, m_pSetPort( NULL )
	{
		RegisterInPortPrototype( "value", new TVdfnPortTypeCompare<TVdfnPort<T> > );
	}

	bool PrepareEvaluationRun()
	{
		m_pSetPort = dynamic_cast<TVdfnPort<T>*>( GetInPort( "value" ) );
		return GetIsValid();
	}
protected:
	bool DoEvalNode()
	{
		m_pVariable->Set( m_pSetPort->GetValueConstRef() );
		return true;
	}

	virtual bool GetIsValid() const
	{
		return( m_pVariable != NULL );
	}

private:
	Variable*		m_pVariable;
	TVdfnPort<T>*	m_pSetPort;
};


template<typename T>
class TVdfnGetVariableNode : public IVdfnNode
{
public:
	typedef typename TVdfnTypedVariableRegistry<T>::Variable Variable;

	TVdfnGetVariableNode( Variable* pVariable )
	: IVdfnNode()
	, m_pVariable( pVariable )
	, m_pGetPort( new TVdfnPort<T> )
	{
		RegisterOutPort( "value", m_pGetPort );
	}

protected:
	bool DoEvalNode()
	{
		m_pGetPort->SetValue( m_pVariable->Get(), GetUpdateTimeStamp() );
		return true;
	}

	virtual bool GetIsValid() const
	{
		return( m_pVariable != NULL );
	}

	virtual unsigned int CalcUpdateNeededScore() const
	{
		return m_pVariable->GetUpdateCount();
	}

private:
	Variable*		m_pVariable;
	TVdfnPort<T>*	m_pGetPort;
};



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VDFNNEGATENODE_H

