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

#include "VistaColorOverlay.h"

#ifdef WIN32
#include "Windows.h"
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


VistaColorOverlay::VistaColorOverlay( VistaViewport* pViewport )
: IVistaSceneOverlay( pViewport )
, m_bEnabled( true )
, m_oColor( VistaColor::WHITE )
{
}

VistaColorOverlay::VistaColorOverlay( VistaDisplayManager* pDisplayManager, 
									 const std::string& sViewportName )
: IVistaSceneOverlay( pDisplayManager, sViewportName )
, m_bEnabled( true )
, m_oColor( VistaColor::WHITE )
{
}

VistaColorOverlay::~VistaColorOverlay()
{

}

float VistaColorOverlay::GetOpacity() const
{
	return m_oColor.GetAlpha();
}

void VistaColorOverlay::SetColor( const VistaColor& oColor )
{
	m_oColor = oColor;
}

VistaColor VistaColorOverlay::GetColor() const
{
	return m_oColor;
}

bool VistaColorOverlay::Do()
{
	if( m_bEnabled == false )
		return false;

	if( m_oColor.GetAlpha() <= 0 )
		return true;

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable( GL_CULL_FACE );

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// Note: y is inverted, so we start with (0,0) at the top left
	gluOrtho2D( 0, 1, 0, 1 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4fv( &m_oColor[0] );
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

void VistaColorOverlay::UpdateOnViewportChange( int iWidth, int iHeight, int iPosX, int iPosY )
{

}

void VistaColorOverlay::SetIsEnabled( bool bEnabled )
{
	m_bEnabled = bEnabled;
}

bool VistaColorOverlay::GetIsEnabled() const
{
	return m_bEnabled;
}

void VistaColorOverlay::SetOpacity( const float nOpacity )
{
	m_oColor.SetAlpha( nOpacity );
}
