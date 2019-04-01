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

#include "VistaProximityFadeout.h"

#include <VistaMath/VistaBoundingBox.h>
#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <map>

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
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class VistaProximityFadeout::FadeoutOverlay : public IVistaSceneOverlay
{
public:
	FadeoutOverlay( VistaViewport* pViewport, const VistaColor& oColor )
	: IVistaSceneOverlay( pViewport )
	, m_bEnabled( true )
	, m_oColor( oColor )
	, m_nWarningLevel( 0 )
	, m_nOpacityFactor( 1.0f )
	, m_bFlashState( false )
	{
	}

	virtual bool GetIsEnabled() const
	{
		return m_bEnabled;
	}

	virtual void SetIsEnabled( bool bEnabled ) 
	{
		m_bEnabled = bEnabled;
	}

	virtual void UpdateOnViewportChange( int iWidth, int iHeight, int iPosX, int iPosY ) 
	{		
	}

	virtual bool Do()
	{
		if( m_bEnabled == false )
			return false;
		if( m_nWarningLevel <= 0 )
			return false;

		glPushAttrib( GL_ALL_ATTRIB_BITS );
		glDisable( GL_LIGHTING );
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glDisable( GL_CULL_FACE );

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		// Note: y is inverted, so we start with (0,0) at the top left
		gluOrtho2D( 0, 1, 0, 1 );

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();

		float nAlpha = m_nWarningLevel * m_oColor[3] * m_nOpacityFactor;
		if( m_bFlashState )
			glColor4f( 1.0f - m_oColor[0], 1.0f - m_oColor[1], 1.0f - m_oColor[2], nAlpha );
		else
			glColor4f( m_oColor[0], m_oColor[1], m_oColor[2], nAlpha );
		glBegin( GL_QUADS );
			glVertex3f( 0, 0, 0 );
			glVertex3f( 1, 0, 0 );
			glVertex3f( 1, 1, 0 );
			glVertex3f( 0, 1, 0 );
		glEnd();

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();

		return true;
	}

	VistaColor GetColor() const { return m_oColor; }
	void SetColor( const VistaColor& oValue ) { m_oColor = oValue; }

	float GetWarningLevel() const { return m_nWarningLevel; }
	void SetWarningLevel( const float oValue ) { m_nWarningLevel = oValue; }
	
	float GetOpacityFactor() const { return m_nOpacityFactor; }
	void SetOpacityFactor( const float& oValue ) { m_nOpacityFactor = oValue; }

	bool GetFlashState() const { return m_bFlashState; }
	void SetFlashState( const bool& oValue ) { m_bFlashState = oValue; }
private:
	VistaColor m_oColor;
	bool m_bEnabled;
	float m_nWarningLevel;
	float m_nOpacityFactor;
	bool m_bFlashState;
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


VistaProximityFadeout::VistaProximityFadeout( VistaEventManager* pManager,
											 const float nBeginWarningDistance,
											 const float nMaxWarningDistance )
: IVistaProximityWarningBase( pManager, nBeginWarningDistance, nMaxWarningDistance )
, m_oFadeoutColor( VistaColor::BLACK )
{
}

VistaProximityFadeout::~VistaProximityFadeout()
{

}

VistaColor VistaProximityFadeout::GetFadeoutColor() const
{
	return m_oFadeoutColor;
}

void VistaProximityFadeout::SetFadeoutColor( const VistaColor& oColor )
{
	m_oFadeoutColor = oColor;
	for( std::vector<FadeoutOverlay*>::iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		(*itOverlay)->SetColor( m_oFadeoutColor );
	}
}

void VistaProximityFadeout::AddViewport( VistaViewport* pViewport )
{
	m_vecOverlays.push_back( new FadeoutOverlay( pViewport, m_oFadeoutColor ) );
}

void VistaProximityFadeout::AddAllViewports( VistaDisplayManager* pDisplayManager )
{
	for( std::map<std::string, VistaViewport*>::const_iterator itViewport = pDisplayManager->GetViewportsConstRef().begin();
			itViewport != pDisplayManager->GetViewportsConstRef().end(); ++itViewport )
	{
		m_vecOverlays.push_back( new FadeoutOverlay( (*itViewport).second, m_oFadeoutColor ) );
	}
}

bool VistaProximityFadeout::DoUpdate( const float nMinDistance, const float nWarningLevel, 
									 const VistaVector3D& v3PointOnBounds, const VistaVector3D& v3UserPosition,
									 const VistaQuaternion& qUserOrientation )
{
	for( std::vector<FadeoutOverlay*>::iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		(*itOverlay)->SetWarningLevel( nWarningLevel );
		(*itOverlay)->SetOpacityFactor( 1.0f );
	}
	return true;
}

bool VistaProximityFadeout::GetIsEnabled() const
{
	if( m_vecOverlays.empty() )
		return false;
	return m_vecOverlays[0]->GetIsEnabled();
}

bool VistaProximityFadeout::SetIsEnabled( const bool bSet )
{
	for( std::vector<FadeoutOverlay*>::iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		(*itOverlay)->SetIsEnabled( bSet );
	}
	return true;
}

bool VistaProximityFadeout::DoTimeUpdate( VistaType::systemtime nTime, const float nOpacityScale, const bool bFlashState )
{
	for( std::vector<FadeoutOverlay*>::iterator itOverlay = m_vecOverlays.begin();
			itOverlay != m_vecOverlays.end(); ++itOverlay )
	{
		(*itOverlay)->SetOpacityFactor( nOpacityScale );
		(*itOverlay)->SetFlashState( bFlashState );
	}
	return true;
}

