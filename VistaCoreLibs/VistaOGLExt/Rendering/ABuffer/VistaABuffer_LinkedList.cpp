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
#include "VistaABuffer_LinkedList.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaOGLExt/VistaGLSLShader.h>
#include <VistaOGLExt/VistaOGLUtils.h>
#include <VistaOGLExt/VistaShaderRegistry.h>

/*============================================================================*/
/* DEFINES			                                                          */
/*============================================================================*/
using namespace std;
/*============================================================================*/
/* GLOBAL VARIABLES                                                           */
/*============================================================================*/
const std::string g_strShader_List_Frag = "VistaABuffer_LinktList_frag.glsl";
const std::string g_strShader_List_Aux  = "VistaABuffer_LinktList_aux.glsl";

/*============================================================================*/
/* VistaABufferLinkedList    	                                              */
/*============================================================================*/
/******************************************************************************/
/* CON-/DESTRCUTOR	                                                          */
/******************************************************************************/
VistaABufferLinkedList::VistaABufferLinkedList()
    : IVistaABuffer()
    , m_uiHeadPointerBuffer(0)
    , m_addrHeadPointerBuffer(0)
    , m_uiAtomicFragCounter(0) {
}

VistaABufferLinkedList::~VistaABufferLinkedList() {
  if (m_uiHeadPointerBuffer)
    glDeleteBuffers(1, &m_uiHeadPointerBuffer);
  if (m_uiAtomicFragCounter)
    glDeleteBuffers(1, &m_uiAtomicFragCounter);
}

/******************************************************************************/
/* PUBLIC INTERFACE	                                                          */
/******************************************************************************/
bool VistaABufferLinkedList::ClearABuffer() {
  ResetAtomicCounter();
  return IVistaABuffer::ClearABuffer();
}

VistaGLSLShader* VistaABufferLinkedList::CreateShaderPrototype() const {
  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();
  std::string          strShader  = rShaderReg.RetrieveShader(g_strShader_List_Aux);

  if (strShader.empty())
    return NULL;

  VistaGLSLShader* pShader = new VistaGLSLShader();

  pShader->InitFragmentShaderFromString(strShader);

  return pShader;
}
/******************************************************************************/
/* PROTECTED INTERFACE	                                                      */
/******************************************************************************/
bool VistaABufferLinkedList::InitShader() {
#ifdef _DEBUG
  vstr::debugi() << "[VistaABufferLinkedList] INIT SHADER" << endl;
  vstr::IndentObject oIndent;
#endif // _DEBUG

  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  std::string strShader;

  strShader = rShaderReg.RetrieveShader(g_strShader_List_Frag);

  if (strShader.empty()) {
    vstr::errp() << "[VistaABufferLinkedList] - required shader not found." << endl;
    vstr::IndentObject oIndent;
    vstr::erri() << "Can't find " << g_strShader_List_Frag << endl;
    return false;
  }

  m_pClearShader = new VistaGLSLShader();

  m_pClearShader->InitFragmentShaderFromString(strShader);

  return m_pClearShader->Link();
}

bool VistaABufferLinkedList::InitBuffer() {
  CreateAtomicCounter();

  if (!m_uiHeadPointerBuffer)
    glGenBuffers(1, &m_uiHeadPointerBuffer);

  IVistaABuffer::ResizeBuffer(m_uiHeadPointerBuffer, m_addrHeadPointerBuffer,
      sizeof(unsigned int) * m_uiWidth * m_uiHeight);

  DataField oNextPointerBuffer;
  oNextPointerBuffer.m_nSizeInByte = sizeof(unsigned int);
  oNextPointerBuffer.m_strName     = "g_pNextPointerBuffer";

  glGenBuffers(1, &oNextPointerBuffer.m_uiBuffer);

  IVistaABuffer::ResizeBuffer(oNextPointerBuffer.m_uiBuffer, oNextPointerBuffer.m_addrBuffer,
      oNextPointerBuffer.m_nSizeInByte * m_uiBufferSize);

  m_vecDataFileds.push_back(oNextPointerBuffer);

  UpdateUniforms();

  return !VistaOGLUtils::CheckForOGLError(__FILE__, __LINE__);
}

void VistaABufferLinkedList::ResizePerPixelBuffer() {
  IVistaABuffer::ResizeBuffer(m_uiHeadPointerBuffer, m_addrHeadPointerBuffer,
      sizeof(unsigned int) * m_uiWidth * m_uiHeight);
}
/******************************************************************************/
void VistaABufferLinkedList::AssignUniforms(VistaGLSLShader* pShader) {
  if (pShader == NULL)
    return;

  unsigned int uiProgram = pShader->GetProgram();

  int aiULoc[4];

  aiULoc[0] = pShader->GetUniformLocation("g_nWidth");
  aiULoc[1] = pShader->GetUniformLocation("g_nHeight");
  aiULoc[2] = pShader->GetUniformLocation("g_nMaxNumFragments");
  aiULoc[3] = pShader->GetUniformLocation("g_pHeadPointerBuffer");

  if (aiULoc[0] != -1)
    glProgramUniform1uiEXT(uiProgram, aiULoc[0], m_uiWidth);
  if (aiULoc[1] != -1)
    glProgramUniform1uiEXT(uiProgram, aiULoc[1], m_uiHeight);
  if (aiULoc[2] != -1)
    glProgramUniform1uiEXT(uiProgram, aiULoc[2], m_uiBufferSize);

  if (aiULoc[3] != -1)
    glProgramUniformui64NV(uiProgram, aiULoc[3], m_addrHeadPointerBuffer);

  for (size_t n = 0; n < m_vecDataFileds.size(); ++n) {
    int iULoc = pShader->GetUniformLocation(m_vecDataFileds[n].m_strName);
    if (iULoc != -1)
      glProgramUniformui64NV(uiProgram, iULoc, m_vecDataFileds[n].m_addrBuffer);
  }
}

#define GL_ATOMIC_COUNTER_BUFFER 0x92C0

void VistaABufferLinkedList::CreateAtomicCounter() {
  if (!m_uiAtomicFragCounter)
    glGenBuffers(1, &m_uiAtomicFragCounter);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_uiAtomicFragCounter);
  glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_uiAtomicFragCounter);
}

void VistaABufferLinkedList::ResetAtomicCounter() {
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_uiAtomicFragCounter);
  GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
  ptr[0]      = 0;
  glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
