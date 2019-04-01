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


#include "VdfnGetTransformNode.h" 
#include <VistaAspects/VistaTransformable.h>

#include "VdfnObjectRegistry.h"


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnGetTransformNode::VdfnGetTransformNode(VdfnObjectRegistry *pReg,
											 const std::string &strKey)
: IVdfnNode(),
  m_pTransform(NULL),
  m_pOutMat( new TVdfnPort<VistaTransformMatrix> ),
  m_pReg(pReg),
  m_strKey(strKey),
  m_eMode(TRANS_RELATIVE)
{
	SetEvaluationFlag(true); // unconditional update / eval
	RegisterOutPort( "transform", m_pOutMat );
}


VdfnGetTransformNode::VdfnGetTransformNode()
: IVdfnNode(),
  m_pTransform(NULL),
  m_pOutMat( new TVdfnPort<VistaTransformMatrix> ),
  m_pReg(NULL),
  m_eMode(TRANS_RELATIVE)
{
	SetEvaluationFlag(true); // unconditional update / eval
	RegisterOutPort( "transform", m_pOutMat );
}

VdfnGetTransformNode::VdfnGetTransformNode( IVistaTransformable *pTransform )
	: IVdfnNode(),
	  m_pTransform(pTransform),
	  m_pOutMat( new TVdfnPort<VistaTransformMatrix> ),
	  m_pReg(NULL),
      m_eMode(TRANS_RELATIVE)
{
	SetEvaluationFlag(true); // unconditional update / eval
	RegisterOutPort( "transform", m_pOutMat );
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VdfnGetTransformNode::eMode VdfnGetTransformNode::GetTransformGetMode() const
{
    return m_eMode;
}

void VdfnGetTransformNode::SetTransformGetMode( VdfnGetTransformNode::eMode md )
{
    m_eMode = md;
}

bool VdfnGetTransformNode::PrepareEvaluationRun()
{
	if( (m_pTransform == NULL) && m_pReg && !m_strKey.empty() )
	{
		m_pTransform = m_pReg->GetObjectTransform( m_strKey );
	}
	return GetIsValid();
}


bool VdfnGetTransformNode::GetIsValid() const
{
	return (m_pTransform != NULL);
}

bool VdfnGetTransformNode::DoEvalNode()
{
	VistaTransformMatrix& matTransform = m_pOutMat->GetValueRef();
    switch(m_eMode)
    {
    case TRANS_RELATIVE:
        {
			/** @todo think about returning false here!? */
			if(m_pTransform->GetTransform( matTransform )==false)
                return true; // did not work
            break;
        }
    case TRANS_WORLD:
        {
			/** @todo think about returning false here!? */
            if(m_pTransform->GetWorldTransform( matTransform ) == false)
                return true; // did not work...
            break;
        }
    }

    m_pOutMat->SetUpdateTimeStamp(GetUpdateTimeStamp());
    m_pOutMat->IncUpdateCounter();

	return true;
}

IVistaTransformable *VdfnGetTransformNode::GetTransformable() const
{
	return m_pTransform;
}

void VdfnGetTransformNode::SetTransformable( IVistaTransformable *pTrans )
{
	m_pTransform = pTrans;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


