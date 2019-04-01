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


#ifndef _VDFNMULTIPLEXNODE_H__
#define _VDFNMULTIPLEXNODE_H__


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDataFlowNet/VdfnConfig.h>

#include "VdfnSerializer.h"
#include "VdfnNode.h"
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
 * MultiplexNode takes a "select" inport and arbitrarily many inports named
 * as a number ("0", "1", ...), and forwards the input node corresponding
 * to the value of "select" to the outport.
 * @ingroup VdfnNodes
 * @inport{select, int, mandatory, input channel to select}
 * @inport{0;1;2;..., T, arbitrarily many, input channels}
 * @outport{out, T , selected channel's value}
 */
template<typename T>
class TVdfnMultiplexNode : public IVdfnNode
{
public:
	TVdfnMultiplexNode()
	: IVdfnNode()
	, m_pOutPort( new TVdfnPort<T> )
	, m_pSelectPort( NULL )
	{
		RegisterOutPort( "out", m_pOutPort );
		RegisterInPortPrototype( "select", new TVdfnPortTypeCompare<TVdfnPort<int> > );
	}
	virtual ~TVdfnMultiplexNode()
	{
	}

	virtual bool SetInPort( const std::string &sName,
						   IVdfnPort *pPort )
	{
		if( sName == "select" )
		{
			m_pSelectPort = dynamic_cast<TVdfnPort<int>*>( pPort );
			if( m_pSelectPort == NULL )
				return false;
		}
		else
		{
			int nPortIndex;
			TVdfnPort<T>* pTypedPort = dynamic_cast<TVdfnPort<T>*>( pPort );
			if( pTypedPort == NULL )
				return false;
			if( VistaConversion::FromString( sName, nPortIndex ) == false )
				return false;
			if( m_mapInPorts.find( nPortIndex ) != m_mapInPorts.end() )
				return false;
			m_mapInPorts[nPortIndex] = pTypedPort;
			RegisterInPortPrototype( sName, pPort->GetPortTypeCompare() );
		}

		DoSetInPort( sName, pPort );
		return true;
	}

	bool GetIsValid() const
	{
		return ( m_pSelectPort != NULL && m_mapInPorts.empty() == false );
	}

	bool PrepareEvaluationRun()
	{
		return GetIsValid();
	}

protected:
	bool DoEvalNode()
	{
		typename std::map<int, TVdfnPort<T>*>::iterator itPort = m_mapInPorts.find( m_pSelectPort->GetValue() );
		if( itPort == m_mapInPorts.end() )
			return false;
		m_pOutPort->SetValue( (*itPort).second->GetValue(), GetUpdateTimeStamp() );
		return true;
	}

private:
	std::map<int, TVdfnPort<T>*> m_mapInPorts;
	TVdfnPort<int>* m_pSelectPort;
	TVdfnPort<T>* m_pOutPort;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VDFNMULTIPLEXNODE_H__

