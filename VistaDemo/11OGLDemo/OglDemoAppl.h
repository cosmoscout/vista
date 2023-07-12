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

#ifndef _OGLDEMOAPPL_H
#define _OGLDEMOAPPL_H

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
class VistaOpenGLNode;
class OglDrawObject;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This application is a simple example to show you how to generate a node
 * in ViSTA scene graph where you can call the well-known openGL lines.
 */
class OglDemoAppl {
 public:
  // CONSTRUCTORS / DESTRUCTOR
  OglDemoAppl(int argc = 0, char* argv[] = NULL);
  virtual ~OglDemoAppl();

  void Run();

  void CreateScene();

 private:
  VistaSystem m_vistaSystem;

  OglDrawObject*   m_pOglDrawObj;
  VistaOpenGLNode* m_pOglRootNode;
};

#endif // _OGLDEMOAPPL_H
