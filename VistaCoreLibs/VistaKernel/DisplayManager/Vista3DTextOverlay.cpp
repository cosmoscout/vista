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

#include "Vista3DTextOverlay.h"

#include  <VistaKernel/DisplayManager/VistaTextEntity.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>

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

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
Vista3DTextOverlay::Vista3DTextOverlay( VistaDisplayManager* pDisplayManager,
											const std::string& sViewportName )
: IVistaSceneOverlay( pDisplayManager, sViewportName )
, m_bEnabled(true)
{
}
Vista3DTextOverlay::Vista3DTextOverlay( VistaViewport* pViewport )
: IVistaSceneOverlay( pViewport )
, m_bEnabled(true)
{
}
Vista3DTextOverlay::~Vista3DTextOverlay()
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
bool Vista3DTextOverlay::AddText( IVistaTextEntity* pText,
										bool bManageDeletion )
{
	m_liTexts.push_back( pText );
	m_liTexts.sort();

	if( bManageDeletion )
		m_liMemoryManagedTexts.push_back( pText );
	return true;
}

bool Vista3DTextOverlay::RemText( IVistaTextEntity* pText )
{
	m_liTexts.remove(pText);
	m_liTexts.sort();

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
bool Vista3DTextOverlay::GetIsEnabled() const
{
	return m_bEnabled;
}

void Vista3DTextOverlay::SetIsEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

bool Vista3DTextOverlay::Do()
{
	if(!m_bEnabled)
		return true; // everything is allright, we are not visible

	if(m_liTexts.empty())
		return true; // no text to draw, save some state changes

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable(GL_LIGHTING);


	// as we are an overlay, we may not assume that the
	// current modelview matrix co-responds to the scene camera
	// so we get the matrix from the display system as the
	// inverse
	VistaTransformMatrix m;
	if( GetAttachedViewport() )
		GetAttachedViewport()->GetDisplaySystem()->GetReferenceFrame()->GetMatrixInverse(m);

	// make it floats
	float af4x4[16];

	// transpose for GL
	m.GetTransposedValues(af4x4);

	// we assume the projection matrix to be "correct", though
	/** @todo maybe check the correct projection here? */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf( af4x4 ); // load cam
	glDisable(GL_DEPTH_TEST);

	for(std::list<IVistaTextEntity*>::const_iterator cit = m_liTexts.begin();
		cit != m_liTexts.end(); ++cit)
	{
		if((*cit)->GetEnabled() && !(*cit)->GetText().empty())
		{

			glColor3f((*cit)->GetColor().GetRed(),
						(*cit)->GetColor().GetGreen(),
						(*cit)->GetColor().GetBlue());

			glRasterPos3f((*cit)->GetXPos(),
						(*cit)->GetYPos(),
						(*cit)->GetZPos());

			(*cit)->DrawCharacters();
		}
	}

	glPopMatrix();
	glPopAttrib();

	return true;
}

void Vista3DTextOverlay::UpdateOnViewportChange( int iWidth, int iHeight, int iPosX, int iPosY )
{
	return;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

