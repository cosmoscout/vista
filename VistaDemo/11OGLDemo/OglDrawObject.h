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


#ifndef _OGLDRAWOBJECT_H
#define _OGLDRAWOBJECT_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * In this class you can put your OpenGL rendering lines which will be called
 * in each frame.
 * 
 */    
class OglDrawObject : public IVistaOpenGLDraw
{
public:

    OglDrawObject ();
    virtual ~OglDrawObject ();

	// ---------------------------------------
    // INTERFACE IMPLEMENTATION OFIVistaOpenGLDraw
	// ---------------------------------------

    virtual bool Init ();

	/**
	 * The method Do() gets the callback from scene graph during the rendering
	 * process.
	 */
	virtual bool Do();

	/**
	 * This method should return the bounding box of the openGL object you draw
	 * in the method Do().
	 * 
	 */
	virtual bool GetBoundingBox(VistaBoundingBox &bb);

protected:
};

#endif // _OGLDRAWOBJECT_H
