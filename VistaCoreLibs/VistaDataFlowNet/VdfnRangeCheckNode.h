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


#ifndef _VDFNRANGECHECKNODE_H
#define _VDFNRANGECHECKNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"


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
 * node that determines if an input value is between a min and a max
 *
 * @ingroup VdfnNodes
 * @inport{ in, type T, mandatory, value to be checked}
 * @inport{ min, type T, optional, minimum of range}
 * @inport{ max, type T, optional, maximum of range}
 * @outport{in_range, bool, bool specifiying whether or not in is in range [min,max]}
 * @outport{fraction, float, value specifying fraction of in rel. to min/max - < 0 if in < min, [0,1] if in range, >1 if in > max}
 */
template<class T>
class TVdfnRangeCheckNode : public IVdfnNode
{
public:
	TVdfnRangeCheckNode( T nMin, T nMax )
	: IVdfnNode()
	, m_pInPort( NULL )
	, m_pMinPort( NULL )
	, m_pMaxPort( NULL )
	, m_pInRangePort( new TVdfnPort<bool> )
	, m_pFractionPort( new TVdfnPort<float> )
	, m_nMin( nMin )
	, m_nMax ( nMax )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterInPortPrototype( "min", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterInPortPrototype( "max", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterOutPort( "in_range", m_pInRangePort );
		RegisterOutPort( "fraction", m_pFractionPort );
	}

	bool PrepareEvaluationRun()
	{
		m_pInPort = dynamic_cast<TVdfnPort<T>*>(GetInPort("in"));
		m_pMinPort = dynamic_cast<TVdfnPort<T>*>(GetInPort("min"));
		m_pMaxPort = dynamic_cast<TVdfnPort<T>*>(GetInPort("max"));
		return GetIsValid();
	}
	virtual bool GetIsValid() const
	{
		if( m_pInPort == NULL )
			return false;
		if( m_pMaxPort || m_pMinPort )
			return true;
		return ( m_nMax >= m_nMin );
	}
protected:
	bool DoEvalNode()
	{
		if( m_pMinPort )
			m_nMin = m_pMinPort->GetValue();
		if( m_pMaxPort )
			m_nMax = m_pMaxPort->GetValue();

		float fFraction = (float)( m_pInPort->GetValue() - m_nMin )	
						/ (float)( m_nMax - m_nMin );
		bool bInRange = ( m_pInPort->GetValue() >= m_nMin && m_pInPort->GetValue() <= m_nMax );
		m_pInRangePort->SetValue( bInRange, GetUpdateTimeStamp() );
		m_pFractionPort->SetValue( fFraction, GetUpdateTimeStamp() );
		return true;
	}
private:
	TVdfnPort<T>* m_pInPort;
	TVdfnPort<T>* m_pMinPort;
	TVdfnPort<T>* m_pMaxPort;
	TVdfnPort<bool>* m_pInRangePort;
	TVdfnPort<float>* m_pFractionPort;
	T m_nMin;
	T m_nMax;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VDFNRANGECHECKNODE_H

