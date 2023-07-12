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
/* INCLUDES
 */
/*============================================================================*/
#include "VistaABuffer.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaOGLExt/VistaGLSLShader.h>
#include <VistaOGLExt/VistaOGLUtils.h>

/*============================================================================*/
/* DEFINES			                                                          */
/*============================================================================*/
#ifndef GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV
#define GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV 0x00000010
#endif

using namespace std;

/*============================================================================*/
/* VistaABufferLinkedList    	                                              */
/*============================================================================*/
/******************************************************************************/
/* CON-/DESTRCUTOR	                                                          */
/******************************************************************************/
IVistaABuffer::IVistaABuffer()
    : m_pClearShader(NULL)
    , m_uiWidth(800)
    , m_uiHeight(600)
    , m_uiFragmenstPerPixel(8)
    , m_uiBufferSize(m_uiWidth * m_uiHeight * m_uiFragmenstPerPixel)
    , m_bIsInitialized(false) {
}

IVistaABuffer::~IVistaABuffer() {
  delete m_pClearShader;
}

/******************************************************************************/
/* PUBLIC INTERFACE	                                                          */
/******************************************************************************/
bool IVistaABuffer::Init() {
  if (m_bIsInitialized) {
    vstr::warnp() << "[IVistaABuffer] - A-Buffer is already initialized" << endl;
    return false;
  }

  // Init Shader
  if (!InitShader())
    return false;
  // Init Buffer
  if (!InitBuffer())
    return false;

  m_bIsInitialized = true;
  return true;
}

/******************************************************************************/
bool IVistaABuffer::ClearABuffer() {
  if (m_pClearShader == NULL)
    return false;

  glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glViewport(0, 0, m_uiWidth, m_uiHeight);
  VistaOGLUtils::BeginOrtho2D(0, 1, 0, 1);

  m_pClearShader->Bind();
  glBegin(GL_QUADS);
  glVertex2f(0.0, 0.0);
  glVertex2f(1.0, 0.0);
  glVertex2f(1.0, 1.0);
  glVertex2f(0.0, 1.0);
  glEnd();
  m_pClearShader->Release();

  VistaOGLUtils::EndOrtho();
  glPopAttrib();

  glMemoryBarrierEXT(GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV);

  return true;
}

void IVistaABuffer::Barrier() {
  glMemoryBarrierEXT(GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV);
}
/******************************************************************************/

bool IVistaABuffer::SetABufferSize(unsigned int uiWidth, unsigned int uiHeight) {
  if (m_uiWidth == uiWidth && m_uiHeight == uiHeight)
    return false;

  m_uiWidth  = uiWidth;
  m_uiHeight = uiHeight;

  m_uiBufferSize = m_uiFragmenstPerPixel * m_uiWidth * m_uiHeight;

  ResizePerPixelBuffer();
  ResizeDataFileds();
  UpdateUniforms();
  return true;
}

bool IVistaABuffer::SetFragmenstPerPixel(unsigned int uiMaxNumFrag) {
  if (m_uiFragmenstPerPixel == uiMaxNumFrag)
    return false;

  m_uiFragmenstPerPixel = uiMaxNumFrag;
  m_uiBufferSize        = m_uiFragmenstPerPixel * m_uiWidth * m_uiHeight;

  ResizeDataFileds();
  UpdateUniforms();
  return true;
}

/******************************************************************************/
bool IVistaABuffer::AddDataField(unsigned int nSizeInByte, const std::string& strName) {
  if (!m_bIsInitialized) {
    vstr::errp() << "[IVistaABuffer] - ABuffer must be initialized before DataFields can be added."
                 << endl;
    return false;
  }

  DataField oDataField;
  oDataField.m_nSizeInByte = nSizeInByte;
  oDataField.m_strName     = strName;

  glGenBuffers(1, &oDataField.m_uiBuffer);

  ResizeBuffer(
      oDataField.m_uiBuffer, oDataField.m_addrBuffer, oDataField.m_nSizeInByte * m_uiBufferSize);

  m_vecDataFileds.push_back(oDataField);

  return true;
}

bool IVistaABuffer::RegisterShader(VistaGLSLShader* pShader) {
  for (size_t n = 0; n < m_vecShader.size(); ++n) {
    if (m_vecShader[n] == pShader)
      return false; // Shader has already been registered
  }

  AssignUniforms(pShader);
  m_vecShader.push_back(pShader);

  return true;
}
bool IVistaABuffer::DeregisterShader(VistaGLSLShader* pShader) {
  for (size_t n = 0; n < m_vecShader.size(); ++n) {
    if (m_vecShader[n] == pShader) {
      m_vecShader.erase(m_vecShader.begin() + n);
      return true;
    }
  }
  return false;
}
/******************************************************************************/
/* PROTECTED INTERFACE	                                                      */
/******************************************************************************/
void IVistaABuffer::ResizeDataFileds() {
  for (size_t n = 0; n < m_vecDataFileds.size(); ++n) {
    ResizeBuffer(m_vecDataFileds[n].m_uiBuffer, m_vecDataFileds[n].m_addrBuffer,
        m_vecDataFileds[n].m_nSizeInByte * m_uiBufferSize);
  }
}

void IVistaABuffer::ResizeBuffer(
    GLuint uiBufferID, GLuint64EXT& uiBufferAdress, unsigned int iSize) {
  glBindBuffer(GL_TEXTURE_BUFFER, uiBufferID);

  glBufferData(GL_TEXTURE_BUFFER, iSize, NULL, GL_STATIC_DRAW);

  glMakeBufferResidentNV(GL_TEXTURE_BUFFER, GL_READ_WRITE);

  glGetBufferParameterui64vNV(GL_TEXTURE_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &uiBufferAdress);

  glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

/******************************************************************************/
void IVistaABuffer::UpdateUniforms() {
  AssignUniforms(m_pClearShader);

  for (size_t n = 0; n < m_vecShader.size(); ++n) {
    AssignUniforms(m_vecShader[n]);
  }
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
