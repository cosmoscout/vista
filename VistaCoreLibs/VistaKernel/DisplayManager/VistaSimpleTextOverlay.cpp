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

#include "VistaSimpleTextOverlay.h"

#include <VistaKernel/DisplayManager/VistaTextEntity.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaAspects/VistaObserver.h>

// Include Windows header for OpenGL
#if defined(WIN32)
#include <Windows.h>
#endif

#if defined(DARWIN)
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <algorithm>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

bool SortByYPos( const IVistaTextEntity* pText1, const IVistaTextEntity* pText2 )
{
	return pText1->GetYPos() < pText2->GetYPos();
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSimpleTextOverlay::VistaSimpleTextOverlay(
										VistaDisplayManager *pDisplayManager,
										const std::string& sViewportName )
: IVistaSceneOverlay( pDisplayManager, sViewportName )
, m_nWidth( 120 )
, m_nHeight( 120 )
, m_bEnabled( true )
{
	if( GetAttachedViewport() )
	{
		GetAttachedViewport()->GetViewportProperties()->GetSize( m_nWidth, m_nHeight );
		GetAttachedViewport()->GetViewportProperties()->GetPosition( m_nPosX, m_nPosY );
	}
}

VistaSimpleTextOverlay::VistaSimpleTextOverlay( VistaViewport* pViewport )
: IVistaSceneOverlay( pViewport )
, m_nWidth( 120 )
, m_nHeight( 120 )
, m_nPosX( 0 )
, m_nPosY( 0 )
, m_bEnabled( true )
{
	if( GetAttachedViewport() )
	{
		GetAttachedViewport()->GetViewportProperties()->GetSize( m_nWidth, m_nHeight );
		GetAttachedViewport()->GetViewportProperties()->GetPosition( m_nPosX, m_nPosY );
	}
}

VistaSimpleTextOverlay::~VistaSimpleTextOverlay()
{
	for( std::list<IVistaTextEntity*>::iterator itToDelete 
			= m_liMemoryManagedTexts.begin();
			itToDelete != m_liMemoryManagedTexts.end();
			++itToDelete )
	{
		delete (*itToDelete);
	}
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaSimpleTextOverlay::AddText( IVistaTextEntity* pText,
										 bool bManageDeletion )
{
	m_liTexts.push_back( pText );
	m_liTexts.sort( SortByYPos );
	if( bManageDeletion )
		m_liMemoryManagedTexts.push_back( pText );
	return true;
}

bool VistaSimpleTextOverlay::RemText( IVistaTextEntity* pText )
{
	m_liTexts.remove( pText );
	m_liTexts.sort( SortByYPos );
	std::list<IVistaTextEntity*>::iterator itToDelete 
						= std::find( m_liMemoryManagedTexts.begin(),
									m_liMemoryManagedTexts.end(),
									pText );
	if( itToDelete != m_liMemoryManagedTexts.end() )
	{
		m_liMemoryManagedTexts.erase( itToDelete );
		delete pText;
	}
	return true;
}

bool VistaSimpleTextOverlay::GetIsEnabled() const
{
	return m_bEnabled;
}

void VistaSimpleTextOverlay::SetIsEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

void VistaSimpleTextOverlay::UpdateOnViewportChange( int iWidth, int iHeight,
														int iPosX, int iPosY )
{
	m_nWidth = iWidth;
	m_nHeight = iHeight;
	m_nPosX = iPosX;
	m_nPosY = iPosY;
}

bool VistaSimpleTextOverlay::Do()
{
	if( !m_bEnabled || m_liTexts.empty() )
		return true; // everything is all right, we are not visible or have no text

	bool bSortList = false;

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_TEXTURE_2D );

	float fLastYPos = 0.0f;
	float fAccumRasterPos = 0;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Note: y is inverted, so we start at the top left
	//gluOrtho2D( m_nPosX, m_nPosX + m_nWidth, -m_nPosY + m_nHeight, -m_nPosY );	
	gluOrtho2D( 0, m_nWidth, m_nHeight, 0 );	

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	for( std::list<IVistaTextEntity*>::const_iterator cit = m_liTexts.begin();
		cit != m_liTexts.end(); ++cit )
	{
		if( (*cit)->GetEnabled() == false )
			continue;

		if( fLastYPos > (*cit)->GetYPos() )
			bSortList = true;

		int iTextSize =(*cit)->GetFontSize();

		float fLineAdvance = (*cit)->GetYPos() - fLastYPos;
		fAccumRasterPos += (float)iTextSize * fLineAdvance;

		glColor3f( (*cit)->GetColor().GetRed(),
					(*cit)->GetColor().GetGreen(),
					(*cit)->GetColor().GetBlue() );

		 /// @todo Translate OpenGL coords to window coords
		glRasterPos2f( (*cit)->GetXPos() * iTextSize,
						fAccumRasterPos );

		fLastYPos = (*cit)->GetYPos();

		(*cit)->DrawCharacters();			
	}


	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glPopAttrib();

	if( bSortList )
		m_liTexts.sort( SortByYPos );

	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


