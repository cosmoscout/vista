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

/*============================================================================*/
/* INCLUDES			                                                          */
/*============================================================================*/
#include "VistaTexture.h"
#include "VistaOGLUtils.h"

#include <cassert>

/*============================================================================*/
/* IMPLEMENTATION	                                                          */
/*============================================================================*/
VistaTexture::VistaTexture(GLenum eTarget)
    : m_eTarget(eTarget)
    , m_uiId(0)
    , m_bManaged(true)
    , m_fMaxS(1.0f)
    , m_fMaxT(1.0f)
    , m_fMaxR(1.0f) {
  glGenTextures(1, &m_uiId);
  // First bind also allocates & creates the actual texture object (similar
  // to new in c++).
  glBindTexture(m_eTarget, m_uiId);
}

VistaTexture::VistaTexture(GLenum eTarget, GLuint iId, bool bManaged /* = false */)
    : m_eTarget(eTarget)
    , m_uiId(iId)
    , m_bManaged(bManaged)
    , m_fMaxS(1.0f)
    , m_fMaxT(1.0f)
    , m_fMaxR(1.0f) {
}

VistaTexture::~VistaTexture() {
  if (m_bManaged && m_uiId) {
    glDeleteTextures(1, &m_uiId);
  }
}

void VistaTexture::Bind() {
  glBindTexture(m_eTarget, m_uiId);
}

void VistaTexture::Unbind() {
  glBindTexture(m_eTarget, 0);
}

void VistaTexture::Bind(GLenum eTextureUnit) {
  int nActiveTexUnit = 0;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &nActiveTexUnit);

  glActiveTexture(eTextureUnit);
  Bind();

  glActiveTexture(nActiveTexUnit);
}

void VistaTexture::Unbind(GLenum eTextureUnit) {
  int nActiveTexUnit = 0;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &nActiveTexUnit);

  glActiveTexture(eTextureUnit);
  Unbind();

  glActiveTexture(nActiveTexUnit);
}

void VistaTexture::Enable() {
  glEnable(m_eTarget);
}

void VistaTexture::Enable(GLenum eTextureUnit) {
  // @todo Verify whether supplied enum is valid.
  glActiveTexture(eTextureUnit);
  Enable();
}

void VistaTexture::Disable() {
  glDisable(m_eTarget);
}

void VistaTexture::Disable(GLenum eTextureUnit) {
  glActiveTexture(eTextureUnit);
  Disable();
}

GLenum VistaTexture::GetTarget() const {
  return m_eTarget;
}

GLuint VistaTexture::GetId() const {
  return m_uiId;
}

void VistaTexture::SetMaxS(float fMax) {
  m_fMaxS = fMax;
}

float VistaTexture::GetMaxS() const {
  return m_fMaxS;
}

void VistaTexture::SetMaxT(float fMax) {
  m_fMaxT = fMax;
}

float VistaTexture::GetMaxT() const {
  return m_fMaxT;
}

void VistaTexture::SetMaxR(float fMax) {
  m_fMaxR = fMax;
}

float VistaTexture::GetMaxR() const {
  return m_fMaxR;
}

void VistaTexture::SetMinFilter(GLenum eMode) {
  Bind();
  glTexParameteri(GetTarget(), GL_TEXTURE_MIN_FILTER, eMode);
}

GLenum VistaTexture::GetMinFilter() {
  GLint m;
  Bind();
  glGetTexParameteriv(GetTarget(), GL_TEXTURE_MIN_FILTER, &m);
  return m;
}

void VistaTexture::SetMagFilter(GLenum eMode) {
  Bind();
  glTexParameteri(GetTarget(), GL_TEXTURE_MAG_FILTER, eMode);
}

GLenum VistaTexture::GetMagFilter() {
  GLint m;
  Bind();
  glGetTexParameteriv(GetTarget(), GL_TEXTURE_MAG_FILTER, &m);
  return m;
}

void VistaTexture::SetWrapS(GLenum eMode) {
  Bind();
  glTexParameteri(GetTarget(), GL_TEXTURE_WRAP_S, eMode);
}

GLenum VistaTexture::GetWrapS() {
  GLint m;
  Bind();
  glGetTexParameteriv(GetTarget(), GL_TEXTURE_WRAP_S, &m);
  return m;
}

void VistaTexture::SetWrapT(GLenum eMode) {
  Bind();
  glTexParameteri(GetTarget(), GL_TEXTURE_WRAP_T, eMode);
}

GLenum VistaTexture::GetWrapT() {
  GLint m;
  Bind();
  glGetTexParameteriv(GetTarget(), GL_TEXTURE_WRAP_T, &m);
  return m;
}

void VistaTexture::SetWrapR(GLenum eMode) {
  Bind();
  glTexParameteri(GetTarget(), GL_TEXTURE_WRAP_R, eMode);
}

GLenum VistaTexture::GetWrapR() {
  GLint m;
  Bind();
  glGetTexParameteriv(GetTarget(), GL_TEXTURE_WRAP_R, &m);
  return m;
}

bool VistaTexture::GenerateMipmaps() {
  Bind();

  // Manual mipmap generation is a feature introduced as part of the
  // framebuffer object extension.
  if (GLEW_EXT_framebuffer_object) {
    glGenerateMipmap(GetTarget());
  } else
    return false;

  return VistaOGLUtils::CheckForOGLError(__FILE__, __LINE__);
}

bool VistaTexture::UploadTexture(const int& iWidth, const int& iHeight, void* pData,
    bool bGenMipmaps /* = true */, GLenum eInputPixelFormat /* = GL_RGBA */,
    GLenum eDataFormat /* = GL_UNSIGNED_BYTE */) {
  Bind();

  if (GetTarget() == GL_TEXTURE_2D) {
    if (!bGenMipmaps) {
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, eInputPixelFormat, eDataFormat, pData);
    } else {
      gluBuild2DMipmaps(
          GL_TEXTURE_2D, GL_RGBA8, iWidth, iHeight, eInputPixelFormat, eDataFormat, pData);
    }
  } else if (GetTarget() == GL_TEXTURE_1D) {
    if (!bGenMipmaps) {
      glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, iWidth, 0, eInputPixelFormat, eDataFormat, pData);
    } else {
      gluBuild1DMipmaps(GL_TEXTURE_1D, GL_RGBA8, iWidth, eInputPixelFormat, eDataFormat, pData);
    }
  } else {
    // neither 1D nor 2D -> fail
    return false;
  }

  if (bGenMipmaps) {
    glTexParameteri(GetTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  } else {
    glTexParameteri(GetTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  return !VistaOGLUtils::CheckForOGLError(__FILE__, __LINE__);
}

/*============================================================================*/
/*  END OF FILE					                                              */
/*============================================================================*/
