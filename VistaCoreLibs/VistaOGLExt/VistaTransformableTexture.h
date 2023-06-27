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

#ifndef _VISTATRANSFORMABLETEXTURE_H_
#define _VISTATRANSFORMABLETEXTURE_H_

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include "VistaOGLExtConfig.h"
#include "VistaTexture.h"
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
 * VistaTransformableTexture provides a basic abstraction for transformed OpenGL textures.
 */
class VISTAOGLEXTAPI VistaTransformableTexture : public VistaTexture {
 public:
  //! forwards to the VistaTexture constructor
  VistaTransformableTexture(GLenum eTarget);
  //! forwards to the VistaTexture constructor
  VistaTransformableTexture(GLenum eTarget, GLuint iId, bool bManaged);

  /**
   * Bind the texture and multiply texture transform matrix.
   * Be sure to call Unbind to not influence subsequent texturing!!!
   */
  virtual void Bind();

  /**
   * Unbinds texture by binding default target. Pop texture matrix stack.
   */
  virtual void Unbind();

  // set texture transform to rotate the texture clockwise -> transforms texture coordinates CCW!
  void SetToRotZ(const float& degrees);

  // set the texture transform matrix to be multiplied onto the current texture transform stack.
  // reminder: this transforms the texture coordinates, not the texture itself!!!
  void SetMatrix(const float m_gl[16]);
  void SetToIdentitiy();

 private:
  float m_oglMatrix[16];
};

#endif // _VISTATRANSFORMABLETEXTURE _H_
