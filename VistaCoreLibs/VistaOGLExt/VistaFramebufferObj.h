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

#ifndef _VISTAFRAMEBUFFEROBJ_H
#define _VISTAFRAMEBUFFEROBJ_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include "VistaOGLExtConfig.h"
#include <GL/glew.h>
#include <string>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaTexture;
class VistaRenderbuffer;

/*============================================================================*/
/* PROTOTYPES                                                                 */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaFramebufferObj provides a basic abstraction for render-to-texture
 * functionality based on the OpenGL extension GL_EXT_framebuffer_object.
 */
class VISTAOGLEXTAPI VistaFramebufferObj {
 public:
  VistaFramebufferObj();
  virtual ~VistaFramebufferObj();

  /**
   * Bind this framebuffer object for rendering to.
   */
  void Bind();

  /**
   * Disable rendering to any framebuffer object. Similar to the
   * fboClass available from http://www.gpgpu.org, this generally
   * disables render-to-texture functionality without a developer
   * (that is: you) getting caught in unnecessary Unbind()-Bind()
   * iterations.
   */
  void Release();

  /**
   * Attach a texture as a render target. This attachment is quite loose,
   * i.e. if attaching another texture to the same attachment point, all
   * information about the currently bound texture is lost.
   */
  void Attach(VistaTexture* pTexture, GLenum eAttachment, int iMipLevel = 0, int iZSlice = 0);

  /**
   * Attach a renderbuffer to this framebuffer object. Again, this
   * attachment is quite loose, i.e. if attaching another renderbuffer
   * to the same attachment point, no information whatsoever of the
   * currently bound renderbuffer is preserved.
   */
  void Attach(VistaRenderbuffer* pRenderbuffer, GLenum eAttachment);

  /**
   * Detach the currently attached resource from the given attachment point.
   */
  void Detach(GLenum eAttachment);

  /**
   * Retrieve the id of the attached resource.
   */
  GLuint GetAttachedId(GLenum eAttachment);

  /**
   * Retrieve the type of the attached resource.
   */
  GLenum GetAttachedType(GLenum eAttachment);

  /**
   * Retrieve this framebuffer object's id.
   */
  GLuint GetId() const;

  /**
   * Check framebuffer object for validity.
   */
  bool IsValid() const;

  /**
   * Retrieve framebuffer status as string.
   */
  std::string GetStatusAsString() const;

  /**
   * Check for framebuffer object support.
   */
  bool IsSupported() const;

  /**
   * Retrieve the maximum number of color attachments.
   */
  static int GetMaxColorAttachments();

  /**
   * Retrieve the maximum size of a renderbuffer.
   */
  static int GetMaxRenderbufferSize();

 protected:
  /**
   * Check, whether our FBO is the currently bound one. If not,
   * bind our FBO, but don't reset the static class variable.
   * IMPORTANT NOTE: Never let nested SafeBinds occur as they don't leave the
   *				   bound fbo as you expect it.
   * @todo Fix the safe bind problem.
   */
  void FastBind() const;

  /**
   * Check, whether our FBO is supposed to remain bound, i.e. if
   * the static class variable points towards our FBO. If not,
   * re-bind the one from the static variable.
   */
  void FastRelease() const;

  bool          m_bIsBound;
  GLuint        m_iId;
  GLint         m_nActiveFBOOnBind;
  mutable bool  m_bIsBoundOnFastBind;
  mutable GLint m_nFastBindnActiveFBOOnBind;
};

/*============================================================================*/
/* INLINED METHODS                                                            */
/*============================================================================*/

#endif // _VISTAFRAMEBUFFEROBJ_H

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
