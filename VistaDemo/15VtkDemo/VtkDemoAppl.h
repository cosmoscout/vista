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


#ifndef _VTKDEMOAPPL_H
#define _VTKDEMOAPPL_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaSystem.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class OldOpenGLNode;
class VistaOpenGLNode;
class VtkDrawObject;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VtkDemoAppl
{
public:
    // CONSTRUCTORS / DESTRUCTOR

    VtkDemoAppl(int argc = 0, char  *argv[] = NULL);
    virtual ~VtkDemoAppl();

    // IMPLEMENTATION
    bool   Run  ();

    // ANOTHER
    bool   CreateScene ();


// MEMBERS
private:
    VistaSystem		mVistaSystem;
	VistaOpenGLNode *m_pVtkRoot;
    VtkDrawObject   *m_pVtkDrawObj;
};


#endif // _VTKDEMOAPPL_H
