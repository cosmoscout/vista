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


#ifndef _VDFNDEMULTIPLEXNODE_H__
#define _VDFNDEMULTIPLEXNODE_H__


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
 * DemultiplexNode takes a "select" and a "value" inport and arbitrarily many
 * outports named as a number ("0", "1", ...), and forwards the input to the
 * corresponding outport
 *
 * @ingroup VdfnNodes
 * @inport{select, int, mandatory, input channel to select}
 * @inport{value, T, mandatory, input value}
 * @outport{0;1;..., T , output channels}
 */
template<typename T>
class TVdfnDemultiplexNode : public IVdfnNode
{
public:
	TVdfnDemultiplexNode( const int nNumOutports )
	: IVdfnNode()
	, m_pValuePort( NULL )
	, m_pSelectPort( NULL )
	{
		RegisterInPortPrototype( "value", new TVdfnPortTypeCompare<TVdfnPort<T> > );
		RegisterInPortPrototype( "select", new TVdfnPortTypeCompare<TVdfnPort<int> > );
		m_vecOutPorts.resize( nNumOutports );
		for( int i = 0; i < nNumOutports; ++i )
		{
			TVdfnPort<T>* pPort = new TVdfnPort<T>;
			m_vecOutPorts[i] = pPort;
			RegisterOutPort( VistaConversion::ToString( i ), pPort );
		}
	}
	virtual ~TVdfnDemultiplexNode()
	{
	}

	bool GetIsValid() const
	{
		return ( m_pSelectPort != NULL && m_pValuePort != NULL );
	}

	bool PrepareEvaluationRun()
	{
		m_pSelectPort = dynamic_cast<TVdfnPort<int>*>( GetInPort( "select" ) );
		m_pValuePort = dynamic_cast<TVdfnPort<T>*>( GetInPort( "value" ) );
		return GetIsValid();
	}

protected:
	bool DoEvalNode()
	{
		int nValue = m_pSelectPort->GetValue();
		if( nValue < 0 || nValue >= (int)m_vecOutPorts.size() )
			return false;
		m_vecOutPorts[nValue]->SetValue( m_pValuePort->GetValue(), GetUpdateTimeStamp() );
		return true;
	}

private:
	std::vector<TVdfnPort<T>*> m_vecOutPorts;
	TVdfnPort<int>* m_pSelectPort;
	TVdfnPort<T>* m_pValuePort;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VDFNDEMULTIPLEXNODE_H__

