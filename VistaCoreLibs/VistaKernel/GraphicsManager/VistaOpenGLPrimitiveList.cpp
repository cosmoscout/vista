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

#include "VistaOpenGLPrimitiveList.h"

#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>

#include <VistaBase/VistaStreamUtils.h>

#include <algorithm>
#include <cstring>
#include <iostream>

#if defined(WIN32)
// include _before_ gl.h
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif
#include <cmath>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaOpenGLPrimitiveList::VistaOpenGLPrimitiveList(
    VistaSceneGraph* pVistaSceneGraph, VistaGroupNode* pParent)
    : m_pDrawInterface(NULL)
    , m_pOglNode(NULL)
    , m_bRemoveFromSGOnDelete(true)
    , m_ePrimitiveType(LINES) {
  m_pDrawInterface = new COpenGLPrimitiveList;
  m_pOglNode       = pVistaSceneGraph->NewOpenGLNode(pParent, m_pDrawInterface);
  m_pOglNode->SetName("VistaOpenGLPolyLine");
  SetPrimitiveType(LINES);
}

VistaOpenGLPrimitiveList::~VistaOpenGLPrimitiveList() {
  // ogl node was deleted by VistaPrimitive, so
  // discard the draw interface
  if (GetRemoveFromSGOnDelete())
    delete m_pDrawInterface;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

IVistaNode* VistaOpenGLPrimitiveList::GetVistaNode() const {
  return m_pOglNode;
}

void VistaOpenGLPrimitiveList::SetVertices(const std::vector<float>& vecPoints) {
  m_pDrawInterface->SetVertices(vecPoints);
}

bool VistaOpenGLPrimitiveList::GetVertices(std::vector<float>& vecPoints) const {
  return m_pDrawInterface->GetVertices(vecPoints);
}

bool VistaOpenGLPrimitiveList::GetRemoveFromSGOnDelete() const {
  return m_bRemoveFromSGOnDelete;
}

void VistaOpenGLPrimitiveList::SetRemoveFromSGOnDelete(bool bRemove) {
  m_bRemoveFromSGOnDelete = bRemove;
}

bool VistaOpenGLPrimitiveList::SetPrimitiveType(const ePrimitiveType& primitiveType) {
  switch (primitiveType) {
  case POINTS:
    m_pDrawInterface->SetPrimitiveType(GL_POINTS);
    break;
  case LINES:
    m_pDrawInterface->SetPrimitiveType(GL_LINES);
    break;
  case LINE_LOOP:
    m_pDrawInterface->SetPrimitiveType(GL_LINE_LOOP);
    break;
  case LINE_STRIP:
    m_pDrawInterface->SetPrimitiveType(GL_LINE_STRIP);
    break;
  case TRIANGLES:
    m_pDrawInterface->SetPrimitiveType(GL_TRIANGLES);
    break;
  case TRIANGLE_STRIP:
    m_pDrawInterface->SetPrimitiveType(GL_TRIANGLE_STRIP);
    break;
  case TRIANGLE_FAN:
    m_pDrawInterface->SetPrimitiveType(GL_TRIANGLE_FAN);
    break;
  case QUADS:
    m_pDrawInterface->SetPrimitiveType(GL_QUADS);
    break;
  case QUAD_STRIP:
    m_pDrawInterface->SetPrimitiveType(GL_QUAD_STRIP);
    break;
  case POLYGON:
    m_pDrawInterface->SetPrimitiveType(GL_POLYGON);
    break;
  default:
    return false;
  }
  m_ePrimitiveType = primitiveType;
  return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaOpenGLPrimitiveList::COpenGLPrimitiveList::COpenGLPrimitiveList()
    : m_iDispId(-1)
    , m_bDlistDirty(false)
    , m_eGLPrimitiveType(GL_LINES)
    , m_bUseLighting(false)
    , m_fGlPrimitiveSize(1.5f) {
}

VistaOpenGLPrimitiveList::COpenGLPrimitiveList::~COpenGLPrimitiveList() {
  if (m_iDispId == -1)
    glDeleteLists(m_iDispId, 1); // remove old list
}

// bool VistaOpenGLPolyLine::COpenGLPolyLineDraw::DrawDisplayObjects()
bool VistaOpenGLPrimitiveList::COpenGLPrimitiveList::Do() {
  if (m_bDlistDirty)
    if (!UpdateDisplayList())
      return false;

  glMatrixMode(GL_MODELVIEW);

  // draw the
  glPushMatrix();
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  if (m_bUseLighting) {
    glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
  }
  glCallList(m_iDispId);
  glPopAttrib();
  glPopMatrix();

  return true;
}

bool VistaOpenGLPrimitiveList::COpenGLPrimitiveList::UpdateDisplayList() {
  if (m_iDispId != -1) {
    // delete old display list
    glDeleteLists(m_iDispId, 1);
    m_iDispId = -1; // reset list id
  }

  if (m_iDispId == -1) {
    // ok, no disp created
    if ((m_iDispId = glGenLists(1)) == -1) // create _one_ display list
      return false;                        // or bail out
  }

  // create a new list @ index 0
  // and compile it
  glNewList(m_iDispId, GL_COMPILE);
  glPushAttrib(GL_ENABLE_BIT);

  float fMin[3], fMax[3];
  fMin[0] = fMin[1] = fMin[2] = 0.0f;
  fMax[0] = fMax[1] = fMax[2] = 0.0f;

  bool bUseColorVector = m_bIsUsingColorVector && (m_vecPoints.size() <= m_vecColors.size() * 3);
  glEnable(GL_POINT_SMOOTH);
  glPointSize(m_fGlPrimitiveSize);
  glLineWidth(m_fGlPrimitiveSize);

  glBegin(m_eGLPrimitiveType);

  float fDiff[4];
  m_oColor.GetValues(fDiff, VistaColor::RGBA);
  glColor4fv(fDiff);

  float normal[3];
  for (unsigned int i = 0; i < m_vecPoints.size(); i = i + 3) {
    // normal calculation.
    if (m_bUseLighting && m_eGLPrimitiveType == GL_TRIANGLE_STRIP && m_vecPoints.size() > 12 &&
        !(i & 1)) {
      if (i < 6) {
        CalculateNormal(normal, i, i + 3, i + 6);
      } else {
        CalculateNormal(normal, i, i - 3, i - 6);
      }
      glNormal3fv(normal);
    }
    if (bUseColorVector) {
      m_vecColors[i / 3].GetValues(fDiff, VistaColor::RGBA);
      glColor4fv(fDiff);
    }
    glVertex3f(m_vecPoints[i], m_vecPoints[i + 1], m_vecPoints[i + 2]);

#if defined(WIN32) && defined(_MSC_VER) && (_MSC_VER < 1300)
#if !defined(MIN)
#define MIN(A, B) A > B ? B : A
#endif
#if !defined(MAX)
#define MAX(A, B) A > B ? A : B
#endif

    fMin[0] = MIN(m_vecPoints[i], fMin[0]);
    fMin[1] = MIN(m_vecPoints[i + 1], fMin[1]);
    fMin[2] = MIN(m_vecPoints[i + 2], fMin[2]);
    fMax[0] = MAX(m_vecPoints[i], fMax[0]);
    fMax[1] = MAX(m_vecPoints[i + 1], fMax[1]);
    fMax[2] = MAX(m_vecPoints[i + 2], fMax[2]);

#else
    fMin[0] = std::min<float>(m_vecPoints[i], fMin[0]);
    fMin[1] = std::min<float>(m_vecPoints[i + 1], fMin[1]);
    fMin[2] = std::min<float>(m_vecPoints[i + 2], fMin[2]);
    fMax[0] = std::max<float>(m_vecPoints[i], fMax[0]);
    fMax[1] = std::max<float>(m_vecPoints[i + 1], fMax[1]);
    fMax[2] = std::max<float>(m_vecPoints[i + 2], fMax[2]);
#endif
  }
  glEnd();
  glPopAttrib();
  glEndList();

  m_oBBox.SetBounds(fMin, fMax);

  // tell the SG to fetch a new BBox

  m_bDlistDirty = false;
  return true;
}

bool VistaOpenGLPrimitiveList::COpenGLPrimitiveList::GetBoundingBox(VistaBoundingBox& bb) {
  bb = m_oBBox;
  return true;
}

void VistaOpenGLPrimitiveList::COpenGLPrimitiveList::SetVertices(
    const std::vector<float>& vecPoints) {
  if (vecPoints.size() % 3 != 0)
    vstr::outi() << "COpenGLPolyLineDraw::SetLinePoints() -- vecPoints.size() %3 != 0" << std::endl;
  m_vecPoints   = vecPoints;
  m_bDlistDirty = true;
}

bool VistaOpenGLPrimitiveList::COpenGLPrimitiveList::GetVertices(
    std::vector<float>& vecPoints) const {
  vecPoints = m_vecPoints;
  return true;
}

GLenum VistaOpenGLPrimitiveList::COpenGLPrimitiveList::GetPrimitiveType() const {
  return m_eGLPrimitiveType;
}

void VistaOpenGLPrimitiveList::COpenGLPrimitiveList::SetPrimitiveType(GLenum eMd) {
  if (eMd != m_eGLPrimitiveType) {
    m_eGLPrimitiveType = eMd;
    m_bDlistDirty      = true;
  }
}

VistaColor VistaOpenGLPrimitiveList::COpenGLPrimitiveList::GetColor() const {
  return m_oColor;
}

void VistaOpenGLPrimitiveList::COpenGLPrimitiveList::SetColor(const VistaColor& Color) {
  m_oColor      = Color;
  m_bDlistDirty = true;
}

bool VistaOpenGLPrimitiveList::COpenGLPrimitiveList::GetUseLighting() const {
  return m_bUseLighting;
}

void VistaOpenGLPrimitiveList::COpenGLPrimitiveList::SetUseLighting(bool val) {
  m_bDlistDirty  = true;
  m_bUseLighting = val;
}

inline void VistaOpenGLPrimitiveList::COpenGLPrimitiveList::CalculateNormal(
    float normal[3], unsigned int i, int param2, int param3) {
  float a[3] = {m_vecPoints[i] - m_vecPoints[param2], m_vecPoints[i + 1] - m_vecPoints[param2 + 1],
      m_vecPoints[i + 2] - m_vecPoints[param2 + 2]};
  float b[3] = {m_vecPoints[i] - m_vecPoints[param3], m_vecPoints[i + 1] - m_vecPoints[param3 + 1],
      m_vecPoints[i + 2] - m_vecPoints[param3 + 2]};

  normal[0] = a[1] * b[2] - b[1] * a[2];
  normal[1] = a[2] * b[0] - b[2] * a[0];
  normal[2] = a[0] * b[1] - b[0] * a[1];
}

bool VistaOpenGLPrimitiveList::SetColor(const VistaColor& oColor) const {
  m_pDrawInterface->SetColor(oColor);
  return true;
}

bool VistaOpenGLPrimitiveList::SetColor(const VistaColor& color) {
  m_pDrawInterface->SetColor(color);
  return true;
}

bool VistaOpenGLPrimitiveList::SetUseLighting(bool bUseLighting) {
  m_pDrawInterface->SetUseLighting(bUseLighting);
  return true;
}