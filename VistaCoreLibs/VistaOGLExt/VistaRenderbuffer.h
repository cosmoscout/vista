/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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

#ifndef _VISTARENDERBUFFER_H
#define _VISTARENDERBUFFER_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include "VistaOGLExtConfig.h"
#include <GL/glew.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* PROTOTYPES                                                                 */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaRenderbuffer provides a basic abstraction for render-to-texture
 * functionality based on the OpenGL extension GL_EXT_framebuffer_object.
 */
class VISTAOGLEXTAPI VistaRenderbuffer {
 public:
  VistaRenderbuffer();
  VistaRenderbuffer(GLenum eFormat, int iWidth, int iHeight);
  virtual ~VistaRenderbuffer();

  /**
   * Initialize this renderbuffer according to the given values.
   */
  virtual void Init(GLenum eFormat, int iWidth, int iHeight);

  /**
   * Bind this renderbuffer.
   */
  virtual void Bind();

  /**
   * Release any renderbuffer.
   */
  virtual void Release();

  /**
   * Retrieve this renderbuffer's id.
   */
  GLuint GetId() const;

  /**
   * Check for framebuffer object support.
   */
  bool IsSupported() const;

 protected:
  /**
   * Check, whether this renderbuffer is the currently bound one. If not,
   * bind our renderbuffer, but don't reset the static class variable.
   */
  void FastBind();

  /**
   * Check, whether our renderbuffer is supposed to remain bound, i.e. if
   * the static class variable points towards our renderbuffer. If not,
   * re-bind the one from the static variable.
   */
  void FastRelease();

  GLuint        m_iId;
  static GLuint s_iCurrentlyBoundRenderbuffer;
};

/*============================================================================*/
/* INLINED METHODS                                                            */
/*============================================================================*/

#endif // _VISTARENDERBUFFER_H

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
