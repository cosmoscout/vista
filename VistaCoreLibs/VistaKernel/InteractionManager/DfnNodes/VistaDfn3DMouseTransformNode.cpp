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


#include "VistaDfn3DMouseTransformNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaProjection.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaAspects/VistaObserver.h>
#include <VistaAspects/VistaObserveable.h>

#include <VistaDataFlowNet/VdfnUtil.h>

#include <algorithm>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaVdfn3DMouseTransformNode::TransObserver : public IVistaObserver
{
public:
	enum
	{
		ID_PROJECTION=0,
		ID_VIEWPORT,
		ID_DISPSYSTEM,
		ID_PLATFORM
	};

	TransObserver( VistaVdfn3DMouseTransformNode *pParent )
		: m_pParent( pParent )
	{
		Observe(pParent->m_pProjection->GetProjectionProperties(), ID_PROJECTION );
		Observe(pParent->m_pViewport->GetViewportProperties(), ID_VIEWPORT );
		Observe(pParent->m_pSystem->GetDisplaySystemProperties(), ID_DISPSYSTEM );
		Observe(pParent->m_pSystem->GetReferenceFrame(), ID_PLATFORM );
	}

	~TransObserver()
	{
		for(std::vector<IVistaObserveable*>::const_iterator cit = m_vecObjs.begin();
			cit != m_vecObjs.end(); ++cit)
		{
			(*cit)->DetachObserver( this );
		}
	}


	bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, int nTicket)
	{
		return true;
	}

	void ObserveableDelete(IVistaObserveable *pObserveable, int nTicket)
	{
		ReleaseObserveable(pObserveable, nTicket);
	}

	void ReleaseObserveable(IVistaObserveable *pObserveable, int nTicket)
	{
		std::vector<IVistaObserveable*>::iterator it = std::remove( m_vecObjs.begin(), m_vecObjs.end(), pObserveable );
		m_vecObjs.erase(it, m_vecObjs.end());
	}

	void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
	{
		switch( ticket )
		{
		case ID_PROJECTION:
			{
				m_pParent->UpdateOnProjectionChange();
				break;
			}
		case ID_VIEWPORT:
			{
				m_pParent->UpdateOnViewportChange();
				break;
			}
		case ID_DISPSYSTEM:
			{
				m_pParent->UpdateOnDisplaySystemChange();
				break;
			}
		case ID_PLATFORM:
			{
				m_pParent->UpdateOnPlatformChange();
				break;
			}
		default:
			break;
		}
	}

	bool Observes(IVistaObserveable *pObserveable)
	{
		return (std::find( m_vecObjs.begin(), m_vecObjs.end(), pObserveable ) != m_vecObjs.end());
	}

	void Observe(IVistaObserveable *pObservable, int eTicket)
	{
		pObservable->AttachObserver( this, eTicket );
		m_vecObjs.push_back( pObservable );
	}

	VistaVdfn3DMouseTransformNode *m_pParent;

	// access for the parent to be public
	std::vector<IVistaObserveable*> m_vecObjs;

};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaVdfn3DMouseTransformNode::VistaVdfn3DMouseTransformNode(VistaDisplaySystem *pSystem,
											 VistaViewport *pViewport,
											 float fOriginOffsetAlongRay )
											 : IVdfnNode(),
											 m_fOriginOffsetAlongRay( fOriginOffsetAlongRay ),
											 m_pX(NULL),
											 m_pY(NULL),
											 m_pPosition(new TVdfnPort<VistaVector3D>),
											 m_pOrientation(new TVdfnPort<VistaQuaternion>),
											 m_pViewport(pViewport),
											 m_pProjection(pViewport->GetProjection()),
											 m_pSystem(pSystem),
											 m_nLeft(0), m_nRight(0), m_nBottom(0), m_nTop(0),
											 m_nVpW(0),m_nVpH(0), m_bTransFromFrame(false),
											 m_nUpdateCount(0),
											 m_bNeedsUpdate(false)
{
	RegisterInPortPrototype( "x_pos", new TVdfnPortTypeCompare<TVdfnPort<int> >);
	RegisterInPortPrototype( "y_pos", new TVdfnPortTypeCompare<TVdfnPort<int> >);
	RegisterOutPort( "position", m_pPosition );
	RegisterOutPort( "orientation", m_pOrientation );

	UpdateOnViewportChange();
	UpdateOnProjectionChange();
	UpdateOnDisplaySystemChange();

	m_pObserver = new TransObserver(this);
}

VistaVdfn3DMouseTransformNode::~VistaVdfn3DMouseTransformNode()
{
	delete m_pObserver;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaVdfn3DMouseTransformNode::PrepareEvaluationRun()
{
	m_pX = VdfnUtil::GetInPortTyped<TVdfnPort<int>*>( "x_pos", this );
	m_pY = VdfnUtil::GetInPortTyped<TVdfnPort<int>*>( "y_pos", this );
	return GetIsValid();
}

bool VistaVdfn3DMouseTransformNode::DoEvalNode()
{
	// set new position according to current information
	// cast ray from viewposition through pixelposition
	// and convert the direction into a quaternion

	// normalize to viewport coordinates
	float fPixelPosXNormed = ( static_cast<float>( m_pX->GetValue() ) ) / ( static_cast<float>(m_nVpW) );
	float fPixelPosYNormed = ( static_cast<float>( m_pY->GetValue() ) ) / ( static_cast<float>(m_nVpH) );

	// cast ray from mid point to pixel point in normalized vp coords
	VistaVector3D v3PixelPoint =    m_v3Midpoint
		+ ( (float)(m_nRight - m_nLeft) * fPixelPosXNormed + (float)m_nLeft ) * m_v3RightVector
		+ ( (float)(m_nBottom - m_nTop) * fPixelPosYNormed + (float)m_nTop )  * m_v3Up;

	// determine ray from viewer pos to pixel position in vp
	VistaVector3D v3PixelRay = v3PixelPoint - m_v3ViewerPos;
	v3PixelRay.Normalize();

	// determine ray from viewer to mid point of projection
	VistaVector3D v3Viewer2MidpointRay = m_v3Midpoint - m_v3ViewerPos;
	v3Viewer2MidpointRay.Normalize();

	// calculate rotation from one vector to another
	// yoields rotation such that the pointing direction of the mouse is qOut*(0,0,-1)
	// VistaQuaternion qOut = VistaQuaternion(vViewer2MidpointRay,vPixelRay);
	VistaQuaternion qOut = VistaQuaternion( VistaVector3D( 0, 0, -1 ), v3PixelRay );

	// for now, this is the output
	VistaVector3D v3Out = m_v3ViewerPos;
	if(m_bTransFromFrame) // does user want it in world frame?
	{
		m_pSystem->GetReferenceFrame()->TransformFromFrame( v3Out, qOut ); // yes
	}

	//if the position should be offset along the ray, do so
	if( m_fOriginOffsetAlongRay != 0 )
	{
		v3Out = v3Out + m_fOriginOffsetAlongRay * qOut.Rotate( VistaVector3D( 0, 0, -1 ) );
	}

	// set outports approriately
	m_pOrientation->SetValue( qOut, GetUpdateTimeStamp() );
	m_pPosition->SetValue( v3Out, GetUpdateTimeStamp() );

	return true;
}

bool VistaVdfn3DMouseTransformNode::GetTransformPositionFromFrame() const
{
	return m_bTransFromFrame;
}

void VistaVdfn3DMouseTransformNode::SetTransformPositionFromFrame(bool bDo )
{
	m_bTransFromFrame = bDo;
}


unsigned int    VistaVdfn3DMouseTransformNode::CalcUpdateNeededScore() const
{
	if(m_bNeedsUpdate) // use the const-version here, we need the flags later
	{
		m_bNeedsUpdate = false; // one-timer
		return ((++m_nUpdateCount) + IVdfnNode::CalcUpdateNeededScore());
	}
	else
		return (m_nUpdateCount + IVdfnNode::CalcUpdateNeededScore()); // maybe mouse coords have changed?
}

void VistaVdfn3DMouseTransformNode::UpdateOnProjectionChange()
{
	m_bNeedsUpdate = true;
	m_pProjection->GetProjectionProperties()->GetProjectionPlane(m_v3Midpoint, m_v3Normal, m_v3Up);
	m_pProjection->GetProjectionProperties()->GetProjPlaneExtents(m_nLeft, m_nRight, m_nBottom, m_nTop);

	m_v3RightVector = m_v3Up.Cross(m_v3Normal);
	m_v3RightVector.Normalize();

}

void VistaVdfn3DMouseTransformNode::UpdateOnDisplaySystemChange()
{
	m_bNeedsUpdate = true;
	m_pSystem->GetDisplaySystemProperties()->GetViewerPosition(m_v3ViewerPos);
	if(!m_pSystem->GetDisplaySystemProperties()->GetLocalViewer()) // the position was given in "world-coords"
																   // but we want it local to the projection
		m_v3ViewerPos = m_pSystem->GetReferenceFrame()->TransformPositionToFrame(m_v3ViewerPos); // transform back
}

void VistaVdfn3DMouseTransformNode::UpdateOnViewportChange()
{
	m_bNeedsUpdate = true;
	m_pViewport->GetViewportProperties()->GetSize(m_nVpW, m_nVpH);
}

void VistaVdfn3DMouseTransformNode::UpdateOnPlatformChange()
{
	if(m_bTransFromFrame)
		m_bNeedsUpdate = true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


