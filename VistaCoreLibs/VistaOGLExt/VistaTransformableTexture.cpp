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

#include <GL/glew.h>

#include "VistaOGLUtils.h"
#include "VistaTransformableTexture.h"

#ifdef WIN32
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <cassert>
#include <cmath>
#include <cstring>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaTransformableTexture::VistaTransformableTexture(GLenum eTarget)
    : VistaTexture(eTarget) {
  SetToIdentitiy();
}

VistaTransformableTexture::VistaTransformableTexture(GLenum eTarget, GLuint iId, bool bManaged)
    : VistaTexture(eTarget, iId, bManaged) {
  SetToIdentitiy();
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
void VistaTransformableTexture::Bind() {
  VistaTexture::Bind();
  GLint matrixmode;
  glGetIntegerv(GL_MATRIX_MODE, &matrixmode);
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glMultMatrixf(m_oglMatrix);
  glMatrixMode(matrixmode);
}

void VistaTransformableTexture::Unbind() {
  GLint matrixmode;
  glGetIntegerv(GL_MATRIX_MODE, &matrixmode);
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(matrixmode);
  VistaTexture::Unbind();
}

void VistaTransformableTexture::SetToRotZ(const float& degrees) {
  const float d2r = 0.0174532925199433f;
  // transform texture coordinates CCW -> results in CW rotated image.
  SetToIdentitiy();
  m_oglMatrix[0] = cos(degrees * d2r);
  m_oglMatrix[4] = -sin(degrees * d2r);
  m_oglMatrix[1] = sin(degrees * d2r);
  m_oglMatrix[5] = cos(degrees * d2r);
}

void VistaTransformableTexture::SetMatrix(const float m_gl[16]) {
  memcpy(m_oglMatrix, m_gl, sizeof(float) * 16);
}

void VistaTransformableTexture::SetToIdentitiy() {
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      m_oglMatrix[i + j * 4] = (i == j) ? 1.0f : 0.0f;
}
