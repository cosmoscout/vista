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


#include "VdfnToggleNode.h"
#include "VdfnPort.h"
#include "VdfnUtil.h"

#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VdfnToggleNode::VdfnToggleNode( ToggleMode eMode,
								const bool bInitialState )
: m_eToggleMode( eMode )
, m_pTogglePort( NULL )
, m_pOutPort( new TVdfnPort<bool> )
, m_bState( bInitialState )
, m_bLastValue( false )
{
	RegisterInPortPrototype( "toggle", new TVdfnPortTypeCompare<TVdfnPort<bool> > );
	RegisterOutPort( "out", m_pOutPort );
	m_pOutPort->SetValue( bInitialState, 0 );
}

VdfnToggleNode::~VdfnToggleNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VdfnToggleNode::PrepareEvaluationRun()
{
	m_pTogglePort = VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>( "toggle", this );
	m_pOutPort->SetValue( m_bState, 0 );

	return ( m_pTogglePort != NULL );
}

bool VdfnToggleNode::DoEvalNode()
{
	switch( m_eToggleMode )
	{
		case TM_ON_ANY_UPDATE:
		{
			Toggle();
			break;
		}
		case TM_ON_UPDATE_IF_TRUE:
		{
			if( m_pTogglePort->GetValue() == true )
				Toggle();
			break;
		}
		case TM_ON_UPDATE_IF_FALSE:
		{
			if( m_pTogglePort->GetValue() == false )
				Toggle();
			break;
		}
		case TM_ON_ANY_CHANGE:
		{
			if( CheckForInportChange() )
				Toggle();
			break;
		}
		case TM_ON_CHANGE_TO_TRUE:
		{
			if( CheckForInportChange() && m_pTogglePort->GetValue() == true )
				Toggle();
			break;
		}
		case TM_ON_CHANGE_TO_FALSE:
		{
			if( CheckForInportChange() && m_pTogglePort->GetValue() == false )
				Toggle();
			break;
		}
		default:
			VISTA_THROW( "unsupported ToggleMode type", -1 );
	};
	return true;
}

bool VdfnToggleNode::CheckForInportChange()
{
	if( m_bLastValue == m_pTogglePort->GetValue() )
		return false;
	m_bLastValue = m_pTogglePort->GetValue();
	return true;
}

void VdfnToggleNode::Toggle()
{
	m_bState = !m_bState;
	m_pOutPort->SetValue( m_bState, GetUpdateTimeStamp() );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

