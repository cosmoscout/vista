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

#include "VdfnMatrixComposeNode.h"

#include "VdfnObjectRegistry.h"

//#include <VistaMath/VistaMatrixDeComposer.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

 const std::string VdfnMatrixComposeNode::SOutputMatrixOutPortName("out");

 const std::string VdfnMatrixComposeNode::STranslationInPortName("translation");
 const std::string VdfnMatrixComposeNode::SOrientationInPortName("orientation");
 const std::string VdfnMatrixComposeNode::SScaleInPortName("scale");
 const std::string VdfnMatrixComposeNode::SScaleOrientationInPortName("scale_orientation");

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
/*
 */

VdfnMatrixComposeNode::VdfnMatrixComposeNode()						 
: IVdfnNode(),
  m_pInTranslation(NULL),
  m_pInOrientation(NULL),
  m_pInScale(NULL),
  m_pInScaleOrientation(NULL),
  m_pOutputMatrix( new TVdfnPort<VistaTransformMatrix> )
{
  RegisterInPrototypes();
}

VdfnMatrixComposeNode::~VdfnMatrixComposeNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VdfnMatrixComposeNode::RegisterInPrototypes()
{
  RegisterInPortPrototype( STranslationInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
  RegisterInPortPrototype( SOrientationInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );
  RegisterInPortPrototype( SScaleInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
  RegisterInPortPrototype( SScaleOrientationInPortName, new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );
  
  RegisterOutPort( SOutputMatrixOutPortName, m_pOutputMatrix );
}

bool VdfnMatrixComposeNode::GetIsValid() const
{
  // all inports are optional, if nothing is connected,
  // we default to the VistaTransformMatrix default constructor
  // (identity transformation).
  return true;
}

bool VdfnMatrixComposeNode::PrepareEvaluationRun()
{
  m_pInTranslation	= dynamic_cast<TVdfnPort<VistaVector3D>*>( GetInPort( STranslationInPortName ) );
  m_pInOrientation	= dynamic_cast<TVdfnPort<VistaQuaternion>*>( GetInPort( SOrientationInPortName ) );
  m_pInScale		= dynamic_cast<TVdfnPort<VistaVector3D>*>( GetInPort( SScaleInPortName ) );
  m_pInScaleOrientation	= dynamic_cast<TVdfnPort<VistaQuaternion>*>( GetInPort( SScaleOrientationInPortName ) );
  
  return GetIsValid();
}

bool VdfnMatrixComposeNode::DoEvalNode()
{
  VistaVector3D v3Translation;
  VistaQuaternion qOrientation;
  VistaVector3D v3Scale(1,1,1,0);
  VistaQuaternion qScaleOrientation;

  if( m_pInTranslation )
    v3Translation = m_pInTranslation->GetValueConstRef();
  if( m_pInOrientation )
    qOrientation = m_pInOrientation->GetValueConstRef();
  if( m_pInScale )
    v3Scale = m_pInScale->GetValueConstRef();
  if( m_pInScaleOrientation )
    qScaleOrientation = m_pInScaleOrientation->GetValueConstRef();

  m_pOutputMatrix->GetValueRef().Compose( v3Translation,
											qOrientation,
											v3Scale,
											qScaleOrientation );
  m_pOutputMatrix->IncUpdateCounter();

  //m_pOutputMatrix->SetValue(VistaMatrixDeComposer::Compose(v3Translation,
		//	qOrientation,
		//	v3Scale,
		//	qScaleOrientation),GetUpdateTimeStamp());

  return true;								   
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

