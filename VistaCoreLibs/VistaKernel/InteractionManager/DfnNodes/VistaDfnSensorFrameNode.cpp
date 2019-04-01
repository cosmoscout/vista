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


#include "VistaDfnSensorFrameNode.h" 

#include <VistaDataFlowNet/VdfnUtil.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnSensorFrameNode::VistaDfnSensorFrameNode()
										  : IVdfnNode(),
											m_pTranslation(NULL),
											m_pPosIn(NULL),
											m_pTransPosOut( new TVdfnPort<VistaVector3D> ),
											m_pScale(NULL),
											m_pOrientation(NULL),
											m_pOriIn(NULL),
											m_pLocalOri(NULL),
											m_pTransOriOut( new TVdfnPort<VistaQuaternion> ),
											m_pUpd(new _sUpdate)
{
	RegisterInPortPrototype( "pos_in", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
	RegisterInPortPrototype( "ori_in", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );

	
	RegisterInPortPrototype( "translation", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
	RegisterInPortPrototype( "rotation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );
	RegisterInPortPrototype( "scale", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "lc_ori", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );
	
	RegisterOutPort( "position", m_pTransPosOut );
	RegisterOutPort( "orientation", m_pTransOriOut );
}


VistaDfnSensorFrameNode::~VistaDfnSensorFrameNode()
{
	delete m_pUpd;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaDfnSensorFrameNode::GetIsValid() const
{ 
	return (m_pPosIn && m_pOriIn); // others are optional
}

bool VistaDfnSensorFrameNode::PrepareEvaluationRun()
{
	m_pPosIn = VdfnUtil::GetInPortTyped<TVdfnPort<VistaVector3D>*>( "pos_in", this );
	m_pOriIn = VdfnUtil::GetInPortTyped<TVdfnPort<VistaQuaternion>*>( "ori_in", this );

	m_pScale = VdfnUtil::GetInPortTyped<TVdfnPort<float>*>( "scale", this );
	m_pLocalOri = VdfnUtil::GetInPortTyped<TVdfnPort<VistaQuaternion>*>( "lc_ori", this );
	m_pOrientation = VdfnUtil::GetInPortTyped<TVdfnPort<VistaQuaternion>*>( "rotation", this );
	m_pTranslation = VdfnUtil::GetInPortTyped<TVdfnPort<VistaVector3D>*>( "translation", this );
	
	return GetIsValid();
}


bool   VistaDfnSensorFrameNode::DoEvalNode()
{
	const VistaVector3D &vPosIn = m_pPosIn->GetValueConstRef();
	const VistaQuaternion &qOriIn = m_pOriIn->GetValueConstRef();

	VistaVector3D &v3Out = m_pTransPosOut->GetValueRef();
	VistaQuaternion &qOut = m_pTransOriOut->GetValueRef();


	VistaQuaternion qRef, qLoc;

	if(m_pOrientation)
		qRef = m_pOrientation->GetValue();

	if(m_pLocalOri)
		qLoc = m_pLocalOri->GetValue();
	
	v3Out = m_pUpd->m_mtPosTrans * vPosIn; // transform in-value by cached matrix
	qOut = qRef * qOriIn * qRef.GetComplexConjugated() * qLoc;

	m_pTransPosOut->IncUpdateCounter();
	m_pTransOriOut->IncUpdateCounter();
	
	return true;
}


unsigned int VistaDfnSensorFrameNode::CalcUpdateNeededScore() const
{
	_sUpdate *pUpd = m_pUpd;

	bool bRebuild = false;
	// check all revisions for all incoming ports
	for(PortMap::const_iterator it = m_mpInPorts.begin();
		it != m_mpInPorts.end(); ++it)
	{
		// lookup last revision read
		REFMAP::iterator rit = pUpd->m_mpRevision.find( (*it).second );
		if(rit != pUpd->m_mpRevision.end())
		{
			// found, check revision
			if( (*rit).second != (*it).second->GetUpdateCounter() )
			{
				// new revision!
				++pUpd->m_nScore;
		pUpd->m_mpRevision[ (*it).second ] = (*it).second->GetUpdateCounter(); // update revision
		bRebuild = true;
		break;
			}
		}
	else
	{
		pUpd->m_mpRevision[ (*it).second ] = (*it).second->GetUpdateCounter(); // store revision only
		bRebuild = true;
		break;
	}
	}

	if(bRebuild)
	{
		float nScale = 1.0f;
	 VistaQuaternion qPosRot;
	 if(m_pOrientation)
		 qPosRot = m_pOrientation->GetValueConstRef();
	 VistaVector3D   v3Trans;
	 if(m_pTranslation)
		 v3Trans = m_pTranslation->GetValueConstRef();
				
		if(m_pScale)
			nScale = m_pScale->GetValue();
		
	VistaTransformMatrix rotMatrix( qPosRot ),
						 scaleMatrix( nScale ),
						  transMatrix( v3Trans ); // Translation

		// first scale to world, then rotate to world ref frame
		// the translate to world origin (in world coordinates)
		pUpd->m_mtPosTrans = transMatrix * rotMatrix * scaleMatrix;
	}
	return pUpd->m_nScore;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


