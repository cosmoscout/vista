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


#ifndef _VDFNTHRESHOLDNODE
#define _VDFNTHRESHOLDNODE

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnPort.h"

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
 * VdfnThresholdNode
 * Tests if the value on the in-port is above the threshold (which is either set
 * as parameter or over the optional threshold-inport).
 * Depending on the setting, a node can let values pass if only if they are
 * above the threshold, or also if its absolute value is above it.
 * If the value falls below the threshold, the output is handled in accordance to
 * the mode setting:
 * BLOCK - the outport is not changed (no new value available)
 * OUTPUT_ZERO - the zero value is written to the outport
 * OUTPUT_THRESHOLD - the outport is set to the threshold value
 * OUTPUT_LAST_VALID_VALUE - the last occurred value that was above the threshold
 *                           is written to the port again
 *
 * @ingroup VdfnNodes
 * @inport{in,T,mandatory, value to be compared }
 * @inport{orientation,VistaQuaternion,optional, threshold value }
 *
 * @outport{out,T, value of the in-port if it is above the threshold}
 */
template<class T>
class VISTADFNAPI VdfnThresholdNode : public IVdfnNode
{
public:
	enum
	{
		BTHM_BLOCK,
		BTHM_OUTPUT_ZERO,
		BTHM_OUTPUT_THRESHOLD,
		BTHM_OUTPUT_LAST_VALID_VALUE,
	};
public:
	VdfnThresholdNode( const T& oThresholdValue,
						const bool bTestAbsoluteValue,
						const bool bSubtractThreshold,
						const int iMode )
	: m_oThresholdValue( oThresholdValue )
	, m_pIn(NULL)
	, m_pThreshold(NULL)
	, m_pOut( new TVdfnPort<T> )
	, m_iMode( iMode )
	, m_bTestAbsoluteValue( bTestAbsoluteValue )
	, m_bSubtractThreshold( bSubtractThreshold )
	{
		RegisterInPortPrototype( "in", 
			new TVdfnPortTypeCompare<TVdfnPort<T> > );
		RegisterInPortPrototype( "threshold", 
			new TVdfnPortTypeCompare<TVdfnPort<T> > );

		RegisterOutPort( "out", m_pOut );
	}	
	virtual ~VdfnThresholdNode()
	{
	}

	bool GetIsValid() const
	{
		return ( m_pIn != NULL );
	}

	bool PrepareEvaluationRun()
	{
		m_pIn = dynamic_cast<TVdfnPort<T>*>( GetInPort( "in" ) );
		m_pThreshold = dynamic_cast<TVdfnPort<T>*>( GetInPort( "threshold" ) );
		return GetIsValid();
	}

protected:
	virtual bool DoEvalNode()
	{
		if( m_pThreshold != NULL )
			m_oThresholdValue = m_pThreshold->GetValueConstRef();

		const T& oValue = m_pIn->GetValueConstRef();
		
		if( oValue >= m_oThresholdValue )		
		{
			if( m_bSubtractThreshold )
				m_pOut->SetValue( oValue, GetUpdateTimeStamp() - m_oThresholdValue );
			else
				m_pOut->SetValue( oValue, GetUpdateTimeStamp() );
		}
		// this second line may look a little strange, but is necessary for unsigned
		// types: for them, y=-x is greater than zero, which may lead to wrong results.
		// Thus, we verify that the initial value is actually negative
		// Since negating an unsigned type triggers a warning, we disable it
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable: 4146 )
#endif
		else if( m_bTestAbsoluteValue && -oValue >= m_oThresholdValue && oValue < T(0) )
		{
			if( m_bSubtractThreshold )
				m_pOut->SetValue( oValue, GetUpdateTimeStamp() + m_oThresholdValue );
			else
				m_pOut->SetValue( oValue, GetUpdateTimeStamp() );	
		}
#ifdef WIN32
#pragma warning( push )
#endif
		else
		{
			switch( m_iMode )
			{
			case BTHM_BLOCK:
				break;
			case BTHM_OUTPUT_LAST_VALID_VALUE:
				m_pOut->IncUpdateCounter();
				break;
			case BTHM_OUTPUT_THRESHOLD:
				m_pOut->SetValue( m_oThresholdValue, GetUpdateTimeStamp() );
				break;
			case BTHM_OUTPUT_ZERO:
				m_pOut->SetValue( T(0), GetUpdateTimeStamp() );
				break;
			default: 
				return false;				
			}
		}

		return true;
	}

private:
	T					m_oThresholdValue;
	T					m_bSubtractThreshold;
	bool				m_bTestAbsoluteValue;
	int					m_iMode;

	TVdfnPort<T>*		m_pIn;
	TVdfnPort<T>*		m_pThreshold;
	TVdfnPort<T>*		m_pOut;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNTHRESHOLDNODE
