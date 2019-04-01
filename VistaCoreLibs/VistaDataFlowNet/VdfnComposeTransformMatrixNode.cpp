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


#include "VdfnComposeTransformMatrixNode.h" 
#include "VistaAspects/VistaConversion.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnComposeTransformMatrixNode::VdfnComposeTransformMatrixNode()
: m_pOut( new TVdfnPort< VistaTransformMatrix > )
{
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			m_a4x4pInPorts[i][j] = NULL;
			RegisterInPortPrototype( VistaConversion::ToString( i ) + "_" + VistaConversion::ToString( j ),
										new TVdfnPortTypeCompare< TVdfnPort< float > >() );
		}
	}

	RegisterOutPort( "out", m_pOut );
}
VdfnComposeTransformMatrixNode::~VdfnComposeTransformMatrixNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnComposeTransformMatrixNode::GetIsValid() const
{
	for( int i = 0; i < 3; ++i ) // last row is optional
	{
		for( int j = 0; j < 4; ++j )
		{
			if( m_a4x4pInPorts[i][j] == NULL )
				return false;
		}
	}
	return true;
}

bool VdfnComposeTransformMatrixNode::PrepareEvaluationRun()
{
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )

		{
			m_a4x4pInPorts[i][j] = dynamic_cast< TVdfnPort< float >* >( 
					GetInPort( VistaConversion::ToString( i ) + "_" + VistaConversion::ToString( j ) ) );
		}
	}
	return GetIsValid();
}

bool VdfnComposeTransformMatrixNode::DoEvalNode()
{
	VistaTransformMatrix& matValues = m_pOut->GetValueRef();
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			if( m_a4x4pInPorts[i][j] != NULL )
			{
				matValues[i][j] = m_a4x4pInPorts[i][j]->GetValue();
			}
		}
	}
	m_pOut->GetUpdateTimeStamp();
	m_pOut->IncUpdateCounter();
	return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

