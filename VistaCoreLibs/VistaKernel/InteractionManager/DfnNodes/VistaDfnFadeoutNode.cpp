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


#include <GL/glew.h>

#include "VistaDfnFadeoutNode.h" 

#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/DisplayManager/VistaColorOverlay.h>

#ifdef WIN32
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

enum
{
	MODE_CONST_OPACITY,
	MODE_OFF,
	MODE_FADEIN,
	MODE_FADEOUT,
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaDfnFadeoutNode::VistaDfnFadeoutNode( VistaClusterMode* pClusterMode )
: IVdfnNode()
, m_oColor( VistaColor::GRAY )
, m_nFadeinTime( 1.0f )
, m_nFadeoutTime( 1.0f )
, m_pStatePort( NULL )
, m_pOpacityPort( NULL )
, m_pClusterMode( pClusterMode )
, m_nOpacity( 0 )
, m_nState( MODE_OFF )
, m_nStartTime( 0 )
{
	RegisterInPortPrototype( "opacity", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "state", new TVdfnPortTypeCompare<TVdfnPort<bool> > );

	// to update the opacity, we are called every frame
	SetEvaluationFlag( true );
}


VistaDfnFadeoutNode::~VistaDfnFadeoutNode()
{
	for( std::vector<VistaColorOverlay*>::iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		delete (*itOverlay);
	}
}

bool VistaDfnFadeoutNode::GetIsValid() const
{
	return ( m_pStatePort || m_pOpacityPort );
}

bool VistaDfnFadeoutNode::PrepareEvaluationRun()
{
	m_pStatePort = VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>( "state", this );
	m_pOpacityPort = VdfnUtil::GetInPortTyped<TVdfnPort<float>*>( "opacity", this );
	return GetIsValid();
}

void VistaDfnFadeoutNode::AddToViewport( VistaViewport* pViewport )
{
	VistaColorOverlay* pOverlay = new VistaColorOverlay( pViewport );
	pOverlay->SetColor( m_oColor );
	pOverlay->SetOpacity( m_oColor.GetAlpha() * m_nOpacity );
	pOverlay->SetIsEnabled( false );
	m_vecOverlays.push_back( pOverlay );
}

void VistaDfnFadeoutNode::AddToAllViewports( VistaDisplayManager* pManager )
{
	for( std::map<std::string, VistaViewport*>::const_iterator itViewport = pManager->GetViewportsConstRef().begin();
			itViewport != pManager->GetViewportsConstRef().end(); ++itViewport )
	{
		AddToViewport( (*itViewport).second );
	}
}

VistaColor VistaDfnFadeoutNode::GetColor() const
{
	return m_oColor;
}

void VistaDfnFadeoutNode::SetColor( const VistaColor& oValue )
{
	m_oColor = oValue;
}

VistaType::microtime VistaDfnFadeoutNode::GetFadeinTime() const
{
	return m_nFadeinTime;
}

void VistaDfnFadeoutNode::SetFadeinTime( const VistaType::microtime oValue )
{
	m_nFadeinTime = oValue;
}

VistaType::microtime VistaDfnFadeoutNode::GetFadeoutTime() const
{
	return m_nFadeoutTime;
}

void VistaDfnFadeoutNode::SetFadeoutTime( const VistaType::microtime oValue )
{
	m_nFadeoutTime = oValue;
}

void VistaDfnFadeoutNode::SetModeOpacity()
{
	float nOpacityFactor = 0;
	switch( m_nState )
	{
		case MODE_CONST_OPACITY:
			nOpacityFactor = m_nOpacity;
			break;
		case MODE_FADEOUT:
		{
			VistaType::microtime nDiff = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nDiff /= m_nFadeoutTime;
			if( nDiff > 1 )
			{
				m_nState = MODE_CONST_OPACITY;
				nOpacityFactor = 1;
			}
			else
				nOpacityFactor = nDiff;
			break;
				
		}
		case MODE_FADEIN:
		{
			VistaType::microtime nDiff = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nDiff /= m_nFadeinTime;
			if( nDiff > 1 )
			{
				m_nState = MODE_OFF;
				nOpacityFactor =  0;
			}
			else
				nOpacityFactor = ( 1 - nDiff );
			break;
		}
		case MODE_OFF:
		default:
			break;
	}
	float nAlpha = m_oColor.GetAlpha() * nOpacityFactor;
	for( std::vector< VistaColorOverlay* >::iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		(*itOverlay)->SetOpacity( nAlpha );
	}
}

bool VistaDfnFadeoutNode::DoEvalNode()
{
	if( m_pStatePort )
	{
		bool bState = m_pStatePort->GetValue();
		if( bState == true && ( m_nState == MODE_FADEIN || m_nState == MODE_CONST_OPACITY ) )
			return true; // already correct state
		if( bState == false && ( m_nState == MODE_FADEOUT || m_nState == MODE_OFF ) )
			return true; // already correct state

		// check if we aort a fade-in-progress
		VistaType::microtime nOffset = 0;
		if( m_nState == MODE_FADEIN )
		{
			VistaType::microtime nDelta = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nOffset = m_nFadeinTime - nDelta;
		}
		else if( m_nState == MODE_FADEOUT )
		{
			VistaType::microtime nDelta = m_pClusterMode->GetFrameClock() - m_nStartTime;
			nOffset = m_nFadeinTime - nDelta;
		}
		m_nStartTime = m_pClusterMode->GetFrameClock() - nOffset;
		if( bState )
			m_nState = MODE_FADEOUT;
		else
			m_nState = MODE_FADEIN;
	}
	else if( m_pOpacityPort )
	{
		m_nOpacity = m_pOpacityPort->GetValue();
		m_nState = MODE_CONST_OPACITY;
	}
	SetModeOpacity();
	return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


