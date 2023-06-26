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

#ifndef _VTKDRAWOBJECT_H
#define _VTKDRAWOBJECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
// =======================================================
// ATTENTION: Include VTK headers before including WTK !!!
// =======================================================
// INCREASE THE HEAP: /Zm1000

//#include <vtk.h>
// USE: "vtk3.2/include/vtkConfigure.cxx"
//#define VTK_USE_ANSI_STDLIB
//#define NOMINMAX
// USE: "vtk3.2/include/vtkToolkits.cxx"
//#define VTK_USE_PATENTED 1
//#define VTK_USE_CONTRIB 1
//#define VTK_USE_IMAGING 1
//#define VTK_USE_GRAPHICS 1

// include OpenGL and VTK libraries
#ifdef _WIN32
#include <vtkWin32OpenGLRenderWindow.h>
#else
#include <vtkXOpenGLRenderWindow.h>
#endif

//#include <VistaKernel/VistaOldGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

// include inherited fromIVistaOpenGLDraw
#include <VistaAspects/VistaExplicitCallbackInterface.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#ifdef _WIN32
#define VTKOGLRENWIN vtkWin32OpenGLRenderWindow
#else
#define VTKOGLRENWIN vtkXOpenGLRenderWindow
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class vtkActorCollection;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VtkDrawObject : public VTKOGLRENWIN, public IVistaOpenGLDraw {
 public:
  // CONSTRUCTORS / DESTRUCTOR
  VtkDrawObject();
  virtual ~VtkDrawObject();

  // INTERFACE REALIZATION OFIVistaOpenGLDraw
  virtual bool GetBoundingBox(VistaBoundingBox&);
  virtual bool Do();

 private:
  // OVERRIDE METHODS OF vtk?OpenGLRenderWindow
  void MakeCurrent() {
    return;
  }

 protected:
  vtkRenderer*        m_pVtkRenderer;
  vtkActorCollection* m_pActorColl;
};

#endif // _VTKDRAWOBJECT_H
