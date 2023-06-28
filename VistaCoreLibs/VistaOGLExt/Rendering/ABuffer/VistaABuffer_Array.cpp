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
#include "VistaABuffer_Array.h"

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
const std::string g_strShader_Array_Frag = "VistaABuffer_Array_frag.glsl";
const std::string g_strShader_Array_Aux  = "VistaABuffer_Array_aux.glsl";

/*============================================================================*/
/* VistaABufferLinkedList    	                                              */
/*============================================================================*/
/******************************************************************************/
/* CON-/DESTRCUTOR	                                                          */
/******************************************************************************/
VistaABufferArray::VistaABufferArray()
    : IVistaABuffer()
    , m_uiFragCountBuffer(0)
    , m_addrFragCountBuffer(0) {
}

VistaABufferArray::~VistaABufferArray() {
  if (m_uiFragCountBuffer)
    glDeleteBuffers(1, &m_uiFragCountBuffer);
}

/******************************************************************************/
/* PUBLIC INTERFACE	                                                          */
/******************************************************************************/
VistaGLSLShader* VistaABufferArray::CreateShaderPrototype() const {
  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();
  std::string          strShader;

  strShader = rShaderReg.RetrieveShader(g_strShader_Array_Aux);

  if (strShader.empty())
    return NULL;

  VistaGLSLShader* pShader = new VistaGLSLShader();

  pShader->InitFragmentShaderFromString(strShader);

  return pShader;
}
/******************************************************************************/
/* PROTECTED INTERFACE	                                                      */
/******************************************************************************/
bool VistaABufferArray::InitShader() {
#ifdef _DEBUG
  vstr::debugi() << "[VistaABufferArray] INIT SHADER" << endl;
  vstr::IndentObject oIndent;
#endif // _DEBUG

  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  std::string strShader;

  strShader = rShaderReg.RetrieveShader(g_strShader_Array_Frag);

  if (strShader.empty()) {
    vstr::errp() << "[VistaABufferLinkedList] - required shader not found." << endl;
    vstr::IndentObject oIndent;
    vstr::erri() << "Can't find " << g_strShader_Array_Frag << endl;
    return false;
  }

  m_pClearShader = new VistaGLSLShader();

  m_pClearShader->InitFragmentShaderFromString(strShader);

  return m_pClearShader->Link();
}

bool VistaABufferArray::InitBuffer() {
  if (!m_uiFragCountBuffer)
    glGenBuffers(1, &m_uiFragCountBuffer);

  IVistaABuffer::ResizeBuffer(
      m_uiFragCountBuffer, m_addrFragCountBuffer, sizeof(unsigned int) * m_uiWidth * m_uiHeight);

  UpdateUniforms();

  return !VistaOGLUtils::CheckForOGLError(__FILE__, __LINE__);
}

void VistaABufferArray::ResizePerPixelBuffer() {
  IVistaABuffer::ResizeBuffer(
      m_uiFragCountBuffer, m_addrFragCountBuffer, sizeof(unsigned int) * m_uiWidth * m_uiHeight);
}
/******************************************************************************/
void VistaABufferArray::AssignUniforms(VistaGLSLShader* pShader) {
  if (pShader == NULL)
    return;

  unsigned int uiProgram = pShader->GetProgram();

  int aiULoc[4];

  aiULoc[0] = pShader->GetUniformLocation("g_nWidth");
  aiULoc[1] = pShader->GetUniformLocation("g_nHeight");
  aiULoc[2] = pShader->GetUniformLocation("g_nFragmentsPerPixel");
  aiULoc[3] = pShader->GetUniformLocation("g_pFragCountBuffer");

  if (aiULoc[0] != -1)
    glProgramUniform1uiEXT(uiProgram, aiULoc[0], m_uiWidth);
  if (aiULoc[1] != -1)
    glProgramUniform1uiEXT(uiProgram, aiULoc[1], m_uiHeight);
  if (aiULoc[2] != -1)
    glProgramUniform1uiEXT(uiProgram, aiULoc[2], m_uiFragmenstPerPixel);

  if (aiULoc[3] != -1)
    glProgramUniformui64NV(uiProgram, aiULoc[3], m_addrFragCountBuffer);

  for (size_t n = 0; n < m_vecDataFileds.size(); ++n) {
    int iULoc = pShader->GetUniformLocation(m_vecDataFileds[n].m_strName);
    if (iULoc != -1)
      glProgramUniformui64NV(uiProgram, iULoc, m_vecDataFileds[n].m_addrBuffer);
  }
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
