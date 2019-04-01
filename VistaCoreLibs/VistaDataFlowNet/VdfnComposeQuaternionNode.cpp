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


#include "VdfnComposeQuaternionNode.h" 

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnComposeQuaternionNode::VdfnComposeQuaternionNode()
: m_pInX( NULL )
, m_pInY( NULL )
, m_pInZ( NULL )
, m_pInW( NULL )
, m_pOut( new TVdfnPort< VistaQuaternion >() )
{
	RegisterInPortPrototype( "x", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "y", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "z", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "w", new TVdfnPortTypeCompare<TVdfnPort<float> > );

	RegisterOutPort( "out", m_pOut );
}
VdfnComposeQuaternionNode::~VdfnComposeQuaternionNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnComposeQuaternionNode::PrepareEvaluationRun()
{
	m_pInX = dynamic_cast<TVdfnPort<float>*>( GetInPort( "x" ) );
	m_pInY = dynamic_cast<TVdfnPort<float>*>( GetInPort( "y" ) );
	m_pInZ = dynamic_cast<TVdfnPort<float>*>( GetInPort( "z" ) );
	m_pInW = dynamic_cast<TVdfnPort<float>*>( GetInPort( "w" ) );
	return GetIsValid();
}

bool VdfnComposeQuaternionNode::DoEvalNode()
{
	VistaQuaternion& qOut = m_pOut->GetValueRef();
	qOut[0] = m_pInX->GetValueConstRef();
	qOut[1] = m_pInY->GetValueConstRef();
	qOut[2] = m_pInZ->GetValueConstRef();
	qOut[3] =m_pInW->GetValueConstRef();
	qOut.Normalize();

	m_pOut->SetUpdateTimeStamp( GetUpdateTimeStamp() );
	m_pOut->IncUpdateCounter();
	return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

