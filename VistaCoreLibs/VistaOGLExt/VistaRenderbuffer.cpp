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

#include <VistaBase/VistaStreamUtils.h>

#include "VistaRenderbuffer.h"
#include <cassert>
#include <iostream>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

GLuint VistaRenderbuffer::s_iCurrentlyBoundRenderbuffer = 0;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaRenderbuffer::VistaRenderbuffer()
    : m_iId(0) {
  if (!GLEW_EXT_framebuffer_object) {
    vstr::errp() << " [VistaRenderbuffer] missing EXT_framebuffer_object extension..." << endl;
    return;
  }

  // generate buffer id and create buffer
  glGenRenderbuffersEXT(1, &m_iId);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_iId);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, s_iCurrentlyBoundRenderbuffer);
}

VistaRenderbuffer::VistaRenderbuffer(GLenum eFormat, int iWidth, int iHeight)
    : m_iId(0) {
  if (!GLEW_EXT_framebuffer_object) {
    vstr::errp() << " [VistaRenderbuffer] missing EXT_framebuffer_object extension..." << endl;
    return;
  }

  glGenRenderbuffersEXT(1, &m_iId);

  Init(eFormat, iWidth, iHeight);
}

VistaRenderbuffer::~VistaRenderbuffer() {
  if (GLEW_EXT_framebuffer_object && m_iId)
    glDeleteRenderbuffersEXT(1, &m_iId);
  m_iId = 0;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
void VistaRenderbuffer::Init(GLenum eFormat, int iWidth, int iHeight) {
  FastBind();

  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, eFormat, iWidth, iHeight);

  FastRelease();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Bind                                                        */
/*                                                                            */
/*============================================================================*/
void VistaRenderbuffer::Bind() {
  if (s_iCurrentlyBoundRenderbuffer == m_iId)
    return;

  if (s_iCurrentlyBoundRenderbuffer != 0) {
    vstr::warnp() << "[VistaRenderbuffer] An other Render-Buffer is currently bound!" << endl;
    vstr::warni() << "Old bind state will be overwritten!" << endl;
  }

  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_iId);
  s_iCurrentlyBoundRenderbuffer = m_iId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Disable                                                     */
/*                                                                            */
/*============================================================================*/
void VistaRenderbuffer::Release() {
  if (s_iCurrentlyBoundRenderbuffer != m_iId) {
    vstr::warnp() << "[VistaRenderbuffer] The Render-Buffer, that should be released,"
                  << " is not the current bound Render-Buffer!" << endl;
  }

  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
  s_iCurrentlyBoundRenderbuffer = 0;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetId                                                       */
/*                                                                            */
/*============================================================================*/
GLuint VistaRenderbuffer::GetId() const {
  return m_iId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsSupported                                                 */
/*                                                                            */
/*============================================================================*/
bool VistaRenderbuffer::IsSupported() const {
  return GLEW_EXT_framebuffer_object ? true : false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SafeBind                                                    */
/*                                                                            */
/*============================================================================*/
void VistaRenderbuffer::FastBind() {
  if (s_iCurrentlyBoundRenderbuffer != m_iId)
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_iId);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SafeRelease                                                 */
/*                                                                            */
/*============================================================================*/
void VistaRenderbuffer::FastRelease() {
  if (s_iCurrentlyBoundRenderbuffer != m_iId)
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, s_iCurrentlyBoundRenderbuffer);
}

/*============================================================================*/
/*  LOKAL VARS / FUNCTIONS                                                    */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "VistaRenderbuffer.cpp"                                       */
/*============================================================================*/
