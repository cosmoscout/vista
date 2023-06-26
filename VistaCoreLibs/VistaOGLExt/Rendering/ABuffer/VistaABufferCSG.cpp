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
#include "VistaABufferCSG.h"
#include "VistaABuffer.h"

#include <VistaBase/VistaStreamUtils.h>

#include <VistaOGLExt/VistaGLSLShader.h>
#include <VistaOGLExt/VistaShaderRegistry.h>
#include <VistaOGLExt/VistaTexture.h>

/*============================================================================*/
/* GLOBAL VARIABLES                                                           */
/*============================================================================*/
const std::string g_strShader_CSG_Display_Frag  = "VistaABufferCSG_Display_frag.glsl";
const std::string g_strShader_CSG_Display_Aux   = "VistaABufferCSG_Display_aux.glsl";
const std::string g_strShader_CSG_Default_Vert  = "VistaABufferOIT_Default_vert.glsl";
const std::string g_strShader_CSG_Default_Frag  = "VistaABufferCSG_Default_frag.glsl";
const std::string g_strShader_CSG_StoreFrag_Aux = "VistaABufferCSG_StoreFragment_aux.glsl";
const std::string g_strShader_CSG_Lighting_Aux  = "Vista_PhongLighting_aux.glsl";

/*============================================================================*/
/* VistaABufferCSG    	                                                      */
/*============================================================================*/
/******************************************************************************/
/* CON-/DESTRCUTOR	                                                          */
/******************************************************************************/
VistaABufferCSG::VistaABufferCSG()
    : VistaABufferOIT()
    , m_strDisplayShaderExt(g_strShader_CSG_Display_Aux) {
}

VistaABufferCSG::~VistaABufferCSG() {
}

/******************************************************************************/
/* PUBLIC INTERFACE	                                                          */
/******************************************************************************/
void VistaABufferCSG::SetDispalyExtensionShaderName(const std::string& strName) {
  m_strDisplayShaderExt = strName;
}

VistaGLSLShader* VistaABufferCSG::CreateShaderPrototype() {
  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  std::string strShaderSource = rShaderReg.RetrieveShader(g_strShader_CSG_StoreFrag_Aux);

  if (strShaderSource.empty())
    return NULL;

  VistaGLSLShader* pShader = m_pABuffer->CreateShaderPrototype();

  if (pShader != NULL) {
    pShader->InitFragmentShaderFromString(strShaderSource);
  }

  return pShader;
}

/******************************************************************************/
/* PROTECTED INTERFACE	                                                      */
/******************************************************************************/
bool VistaABufferCSG::InitABuffer(unsigned int uiFragmentesPerPixel, unsigned int uiPageSize) {
  m_pABuffer->Init();
  m_pABuffer->SetFragmenstPerPixel(uiFragmentesPerPixel);
  m_pABuffer->AddDataField(sizeof(float), "u_pFragDepth");
  m_pABuffer->AddDataField(sizeof(int), "u_pFragColor");
  m_pABuffer->AddDataField(sizeof(int), "u_pObjectID");

  return true;
}

bool VistaABufferCSG::InitShader() {
  return InitDefaultShader() && InitDisplayShader();
}

bool VistaABufferCSG::InitDefaultShader() {
#ifdef _DEBUG
  vstr::debugi() << "[VistaABufferCSG] Initializing default shader" << std::endl;
  vstr::IndentObject oIndent;
#endif // _DEBUG

  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  std::string strShader[4];
  strShader[0] = rShaderReg.RetrieveShader(g_strShader_CSG_Default_Vert);
  strShader[1] = rShaderReg.RetrieveShader(g_strShader_CSG_Default_Frag);
  strShader[2] = rShaderReg.RetrieveShader(g_strShader_CSG_StoreFrag_Aux);
  strShader[3] = rShaderReg.RetrieveShader(g_strShader_CSG_Lighting_Aux);

  if (strShader[0].empty() || strShader[1].empty() || strShader[2].empty() ||
      strShader[3].empty()) {
    vstr::errp() << "[VistaABufferOIT] - required shader not found." << std::endl;
    vstr::IndentObject oIndent;
    if (strShader[0].empty())
      vstr::erri() << "Can't find " << g_strShader_CSG_Default_Vert << std::endl;
    if (strShader[1].empty())
      vstr::erri() << "Can't find " << g_strShader_CSG_Default_Frag << std::endl;
    if (strShader[2].empty())
      vstr::erri() << "Can't find " << g_strShader_CSG_StoreFrag_Aux << std::endl;
    if (strShader[3].empty())
      vstr::erri() << "Can't find " << g_strShader_CSG_Lighting_Aux << std::endl;
    return false;
  }

  VistaGLSLShader* pShader = m_pABuffer->CreateShaderPrototype();

  if (!pShader)
    return false;

  pShader->InitFromStrings(strShader[0], strShader[1]);
  pShader->InitFragmentShaderFromString(strShader[2]);
  pShader->InitFragmentShaderFromString(strShader[3]);

  if (!pShader->Link()) {
    delete pShader;
    vstr::errp() << "[VistaABufferCSG] - Can't link shader" << std::endl;
    return false;
  }

  if (m_pDefaultShader) {
    DeregisterShader(m_pDefaultShader);
    delete m_pDefaultShader;
    m_pDefaultShader = NULL;
  }

  RegisterShader(pShader);
  m_pDefaultShader = pShader;
  return true;
}

bool VistaABufferCSG::InitDisplayShader() {
#ifdef _DEBUG
  vstr::debugi() << "[VistaABufferCSG] Initializing default shader" << std::endl;
  vstr::IndentObject oIndent;
#endif // _DEBUG

  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  std::string strShader[2];
  strShader[0] = rShaderReg.RetrieveShader(g_strShader_CSG_Display_Frag);
  strShader[1] = rShaderReg.RetrieveShader(m_strDisplayShaderExt);

  if (strShader[0].empty() || strShader[1].empty()) {
    vstr::errp() << "[VistaABufferOIT] - required shader not found." << std::endl;
    vstr::IndentObject oIndent;
    if (strShader[0].empty())
      vstr::erri() << "Can't find " << g_strShader_CSG_Display_Frag << std::endl;
    if (strShader[1].empty())
      vstr::erri() << "Can't find " << m_strDisplayShaderExt << std::endl;
    return false;
  }

  VistaGLSLShader* pShader = m_pABuffer->CreateShaderPrototype();

  if (!pShader)
    return false;

  pShader->InitFragmentShaderFromString(strShader[0]);
  pShader->InitFragmentShaderFromString(strShader[1]);

  if (!pShader->Link()) {
    delete pShader;
    vstr::errp() << "[VistaABufferCSG] - Can't link shader" << std::endl;
    return false;
  }

  if (m_pDisplayShader) {
    DeregisterShader(m_pDisplayShader);
    delete m_pDisplayShader;
    m_pDisplayShader = NULL;
  }

  RegisterShader(pShader);
  m_pDisplayShader = pShader;
  return true;
}
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
