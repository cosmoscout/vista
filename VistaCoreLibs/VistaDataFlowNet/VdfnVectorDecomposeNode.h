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


#ifndef _VDFNVECTORDECOMPOSENODE_H
#define _VDFNVECTORDECOMPOSENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnUtil.h"
#include "VdfnConfig.h"
#include "VdfnReEvalNode.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"

#include <vector>
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
 * The VectorDecomposeNode takes an std::vector as input, and sequentially
 * forwards each of its element to the outport. For this, it uses the
 * function of the ReEvalNode that allows multiple evaluation runs per
 * frame so that the values of the vector are output one after the other
 * and the evaluation of the subgraph is triggered.
 * The constructor parameter determines the order in which the elements of
 * the vector are forwarded.
 * @ingroup VdfnNodes
 * @inport{in,vector<T>,mandatory,The vector to decompose}
 * @outport(out,T,The individual elements of the vector}
 * @see ReEvalNode
 */
template<class T>
class TVdfnVectorDecomposeNode : public IVdfnReEvalNode
{
public:
	TVdfnVectorDecomposeNode( const bool bInvertOrder )
	:	IVdfnReEvalNode(),
		m_pInPort( NULL ),
		m_pOutPort( new TVdfnPort<T> ),
		m_pIndexPort( new TVdfnPort<unsigned int> ),
		m_bInvertOrder( bInvertOrder ),
		m_nCurrentIndex( 0 )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<std::vector<T> > >);
		RegisterOutPort( "out", m_pOutPort );
		RegisterOutPort( "index", m_pIndexPort );
	}
	~TVdfnVectorDecomposeNode() {}

	bool PrepareEvaluationRun()
	{
		m_pInPort = VdfnUtil::GetInPortTyped<TVdfnPort<std::vector<T> >* >("in", this);
		return GetIsValid();
	}

	// Tells the evaluation of the Graph if another run is required
	virtual bool GetNeedsReEvaluation() const
	{
		// we need more evaluation runs as long as there are samples to process
		return !m_vecBuffer.empty();
	}
protected:
	virtual bool DoEvalNode()
	{
		if( m_bInvertOrder == false )
		{
			// if the buffered object is empty, this call is not a reevaluation
			// run, but a regular one, so we first buffer the vector from the inport
			if( m_vecBuffer.empty() )
			{
				m_vecBuffer = m_pInPort->GetValueConstRef();
				m_nCurrentIndex = 0;
			}

			if( m_nCurrentIndex < m_vecBuffer.size() )
			{
				m_pOutPort->SetValue( m_vecBuffer[m_nCurrentIndex], GetUpdateTimeStamp() );
				m_pIndexPort->SetValue( m_nCurrentIndex, GetUpdateTimeStamp() );
				++m_nCurrentIndex;
			}
			else
			{
				// when we've passed through all elements, we clear the buffer to
				// indicate that we do not need any more evaluation runs
				m_vecBuffer.clear();
			}
		}
		else
		{
			// if the buffered object is empty, this call is not a reevaluation
			// run, but a regular one, so we first buffer the vector from the inport
			if( m_vecBuffer.empty() )
			{
				m_vecBuffer = m_pInPort->GetValueConstRef();
				m_nCurrentIndex = m_vecBuffer.size();
			}

			if( m_nCurrentIndex > 0 )
			{
				--m_nCurrentIndex;
				m_pOutPort->SetValue( m_vecBuffer[m_nCurrentIndex], GetUpdateTimeStamp() );
				m_pIndexPort->SetValue( m_nCurrentIndex, GetUpdateTimeStamp() );
			}
			else
			{
				// when we've passed through all elements, we clear the buffer to
				// indicate that we do not need any more evaluation runs
				m_vecBuffer.clear();
			}
		}

		return true;
	}

private:
	TVdfnPort<std::vector<T> >*		m_pInPort;
	TVdfnPort<T>*					m_pOutPort;
	TVdfnPort<unsigned int>*		m_pIndexPort;
	std::vector<T>				 	m_vecBuffer;
	unsigned int					m_nCurrentIndex;
	bool							m_bInvertOrder;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNSHALLOWNODE_H

