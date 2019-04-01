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


#include "OglDrawObject.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <VistaMath/VistaBoundingBox.h>

#include <GL/gl.h>


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
OglDrawObject::OglDrawObject()
{
}

OglDrawObject::~OglDrawObject()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
bool OglDrawObject::Init ()
{
    return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Do                                                          */
/*                                                                            */
/*============================================================================*/
/**
 * Note: This is not a good way to interact with your scenegraph cause all changes
 * here are not done in the scenegraph. Especially the GLTranslatf() call should
 * be handled with a TransformNoden in the SG.
 */
bool OglDrawObject::Do()
{
	//save current lighting and meterial state of the OpenGL state machine
	glPushAttrib(GL_LIGHTING | GL_COLOR_MATERIAL);
	//enable lighting
	glEnable(GL_LIGHTING);
	//disable color_material in order to ensure that the given diffuse material value is used
	glDisable(GL_COLOR_MATERIAL);
	//prepare materials for all surfaces of the cube
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0 };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0 };
    GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0  }; 
    GLfloat mat_shininess[] = { 64 }; // 0..128

	//set the materials which are identical for all faces
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess );

    // ==================================
    // draw OpenGL cube
    // ==================================

	//make the model view matrix the current one
	glMatrixMode( GL_MODELVIEW );
    //save current model view matrix
    glPushMatrix();
    //translate our frame to 0.1, 0.1, 0.1
	glTranslatef (0.1f, 0.1f, 0.1f);

	// start drawing  a unit cube in the current (translated) coordinate frame
    // front face (red)
    mat_diffuse[0] = 1.0f;
    mat_diffuse[1] = 0.0f;
    mat_diffuse[2] = 0.0f;
    // set diffuse material value to color
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glBegin(GL_POLYGON);
	    //provide normal data for current polygon (mandatory for lighting!)
        glNormal3f (0.0f, 0.0f, -1.0f);
		//provide the vertices making up the polygon
		glVertex3f (0.0f, 0.0f, 0.0f); 
        glVertex3f (0.0f, 0.3f, 0.0f);
        glVertex3f (0.3f, 0.3f, 0.0f);
        glVertex3f (0.3f, 0.0f, 0.0f);
    glEnd();

    // back face (green)
    mat_diffuse[0] = 0.0f;
    mat_diffuse[1] = 1.0f;
    mat_diffuse[2] = 0.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glBegin(GL_POLYGON);
      glNormal3f (0.0f, 0.0f, 1.0f);
	  glVertex3f (0.0f, 0.0f, 0.3f);
      glVertex3f (0.3f, 0.0f, 0.3f);
      glVertex3f (0.3f, 0.3f, 0.3f);
      glVertex3f (0.0f, 0.3f, 0.3f);
    glEnd();

    // bottom face (blue)
    mat_diffuse[0] = 0.0;
    mat_diffuse[1] = 0.0;
    mat_diffuse[2] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glBegin(GL_POLYGON);
      glNormal3f (0.0f, -1.0f, 0.0f);
	  glVertex3f (0.0f, 0.0f, 0.0f);
      glVertex3f (0.3f, 0.0f, 0.0f);
      glVertex3f (0.3f, 0.0f, 0.3f);
      glVertex3f (0.0f, 0.0f, 0.3f);
    glEnd();

    // right face (violet)
    mat_diffuse[0] = 1.0;
    mat_diffuse[1] = 0.0;
    mat_diffuse[2] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glBegin(GL_POLYGON);
      glNormal3f (1.0f, 0.0f, 0.0f);
	  glVertex3f (0.3f, 0.0f, 0.0f);
      glVertex3f (0.3f, 0.3f, 0.0f);
      glVertex3f (0.3f, 0.3f, 0.3f);
      glVertex3f (0.3f, 0.0f, 0.3f);
    glEnd();

    // top face (yellow)
    mat_diffuse[0] = 1.0;
    mat_diffuse[1] = 1.0;
    mat_diffuse[2] = 0.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glBegin(GL_POLYGON);
      glNormal3f (0.0f, 1.0f, 0.0f);
	  glVertex3f (0.3f, 0.3f, 0.0f);
      glVertex3f (0.0f, 0.3f, 0.0f);
      glVertex3f (0.0f, 0.3f, 0.3f);
      glVertex3f (0.3f, 0.3f, 0.3f);
    glEnd();

    // left face (cyan)
    mat_diffuse[0] = 0.0f;
    mat_diffuse[1] = 1.0f;
    mat_diffuse[2] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glBegin(GL_POLYGON);
      glNormal3f (-1.0f, 0.0f, 0.0f);
      glVertex3f (0.0f, 0.3f, 0.0f);
      glVertex3f (0.0f, 0.0f, 0.0f); 
      glVertex3f (0.0f, 0.0f, 0.3f);
      glVertex3f (0.0f, 0.3f, 0.3f);
    glEnd();

	//finally restore the model view matrix (unwind the translation)
    glPopMatrix();

    // ==================================
    // ==================================
    
    // ==================================
    // draw World Coordinate System Cross
    // ==================================
    // lines are drawn without lighting
    glDisable  ( GL_LIGHTING );
	
	glPushAttrib(GL_LINE_WIDTH);
	// make lines somewhat more dense
    glLineWidth (2.0);

    // X axis (red)
    glColor3f (1.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f (0.0f, 0.0f, 0.0f); 
        glVertex3f (0.5f, 0.0f, 0.0f);
    glEnd();

    // Y axis (green)
    glColor3f (0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f (0.0f, 0.0f, 0.0f); 
        glVertex3f (0.0f, 0.5f, 0.0f);
    glEnd();

    // Z axis (blue)
    glColor3f (0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex3f (0.0f, 0.0f, 0.0f); 
        glVertex3f (0.0f, 0.0f, 0.5f);
    glEnd();

    //restore line width (which has been pushed via the last glPushAttrib call)
	glPopAttrib();
	//restore lighting state (which has been pushed via glPushAttrib)
	glPopAttrib();
    // ==================================
    // ==================================

    return true;
}

bool OglDrawObject::GetBoundingBox(VistaBoundingBox &oBoundingBox)
{

	// Boundingbox is computed by translation an edge points

	float fMin[3] = { 0.1f, 0.1f, 0.1f };
	float fMax[3] = { 0.4f, 0.4f, 0.4f };

	oBoundingBox.SetBounds(fMin, fMax);

	return true;
}


