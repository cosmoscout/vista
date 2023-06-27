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

#include <GL/glew.h>

#include "VistaEyeTester.h"

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#ifdef WIN32
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif

namespace {
const unsigned char S_acRightTexture[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 0, 0, 0, 255, 255,
    255, 0, 255, 255, 255, 0, 255, 0, 0, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255,
    0, 255, 255, 0, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255,
    255, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255,
    0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255,
    0, 255, 255, 0, 255, 255, 0, 0, 255, 255, 0, 0, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 255,
    255, 255, 0, 255, 255, 0, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 0, 255, 255, 0, 255,
    255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 0,
    255, 255, 0, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255,
    255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 0, 255, 255, 255, 0, 0, 0, 255, 255, 255, 0, 255,
    255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

const unsigned char S_acLeftTexture[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 255, 0, 0, 0, 0,
    0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 0, 255, 255,
    255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255,
    255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 255, 255,
    255, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 0, 255, 255, 255,
    255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 255,
    255, 255, 0, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 255, 0, 255, 255,
    255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255};

const float S_nSlabSize     = 0.25f;
const float S_nSlabDistance = 0.5f;
const float S_nSlabHeight   = 0.125f;
} // anonymous namespace

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class VistaEyeTester::EyeTestDraw : public IVistaOpenGLDraw {
 public:
  EyeTestDraw() {
    glGenTextures(1, &m_nLeftTextureId);
    glGenTextures(1, &m_nRightTextureId);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_nLeftTextureId);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_LUMINANCE, 32, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, S_acLeftTexture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, m_nRightTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 32, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
        S_acRightTexture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  ~EyeTestDraw() {
    glDeleteTextures(1, &m_nLeftTextureId);
    glDeleteTextures(1, &m_nRightTextureId);
  }

  virtual bool Do() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glActiveTexture(GL_TEXTURE0);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glNormal3f(1, 0, 0);
    glVertex3f(0, -S_nSlabHeight, 0);
    glVertex3f(0, -S_nSlabHeight, -S_nSlabDistance);
    glVertex3f(0, +S_nSlabHeight, -S_nSlabDistance);
    glVertex3f(0, +S_nSlabHeight, 0);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);
    glNormal3f(-1, 0, 0);
    glVertex3f(0, -S_nSlabHeight, -S_nSlabDistance);
    glVertex3f(0, -S_nSlabHeight, 0);
    glVertex3f(0, +S_nSlabHeight, 0);
    glVertex3f(0, +S_nSlabHeight, -S_nSlabDistance);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_nRightTextureId);
    glBegin(GL_QUADS);
    // the y-coords are inverted, since our data buffers start top-left, but gl expects top-left
    // so we flip vertically
    glColor3f(1.0f, 0.0f, 0.0f);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(0, -S_nSlabHeight, -S_nSlabDistance);
    glTexCoord2f(1, 0);
    glVertex3f(S_nSlabSize, -S_nSlabHeight, -S_nSlabDistance);
    glTexCoord2f(1, -1);
    glVertex3f(S_nSlabSize, +S_nSlabHeight, -S_nSlabDistance);
    glTexCoord2f(0, -1);
    glVertex3f(0, +S_nSlabHeight, -S_nSlabDistance);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_nLeftTextureId);
    glBegin(GL_QUADS);
    // the y-coords are inverted, since our data buffers start top-left, but gl expects top-left
    // so we flip vertically
    glColor3f(0.0f, 1.0f, 0.0f);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(-S_nSlabSize, -S_nSlabHeight, -S_nSlabDistance);
    glTexCoord2f(1, 0);
    glVertex3f(0, -S_nSlabHeight, -S_nSlabDistance);
    glTexCoord2f(1, -1);
    glVertex3f(0, +S_nSlabHeight, -S_nSlabDistance);
    glTexCoord2f(0, -1);
    glVertex3f(-S_nSlabSize, +S_nSlabHeight, -S_nSlabDistance);
    glEnd();

    glPopAttrib();

    return true;
  }

  virtual bool GetBoundingBox(VistaBoundingBox& bb) {
    bb.Expand(10.0f);
    return true;
  }

 private:
  GLuint m_nLeftTextureId;
  GLuint m_nRightTextureId;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaEyeTester::VistaEyeTester(VistaGroupNode* pUserTransNode, VistaSceneGraph* pSceneGraph)
    : m_pDraw(new EyeTestDraw()) {
  m_pDrawNode = pSceneGraph->NewOpenGLNode(pUserTransNode, m_pDraw);
}

VistaEyeTester::~VistaEyeTester() {
  delete m_pDrawNode;
  delete m_pDraw;
}

bool VistaEyeTester::GetIsEnabled() const {
  return m_pDrawNode->GetIsEnabled();
}

void VistaEyeTester::SetIsEnabled(const bool bSet) {
  m_pDrawNode->SetIsEnabled(bSet);
}
