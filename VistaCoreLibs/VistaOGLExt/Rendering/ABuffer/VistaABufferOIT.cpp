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
#include "VistaABufferOIT.h"
#include "VistaABuffer_Array.h"
#include "VistaABuffer_LinkedList.h"
#include "VistaABuffer_Pages.h"

#include <VistaBase/VistaStreamUtils.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/VistaFrameLoop.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaOGLExt/VistaGLSLShader.h>
#include <VistaOGLExt/VistaOGLUtils.h>
#include <VistaOGLExt/VistaShaderRegistry.h>
#include <VistaOGLExt/VistaTexture.h>
/*============================================================================*/
/* GLOBAL VARIABLES                                                           */
/*============================================================================*/
const std::string g_strShader_Default_Vert  = "VistaABufferOIT_Default_vert.glsl";
const std::string g_strShader_Default_Frag  = "VistaABufferOIT_Default_frag.glsl";
const std::string g_strShader_Display_Frag  = "VistaABufferOIT_Display_frag.glsl";
const std::string g_strShader_StoreFrag_Aux = "VistaABufferOIT_StoreFragment_aux.glsl";
const std::string g_strShader_Lighting_Aux  = "Vista_PhongLighting_aux.glsl";

/*============================================================================*/
/* Hidden Class                                                                */
/*============================================================================*/
class VistaABufferOITCallback : public IVistaOpenGLDraw {
 public:
  VistaABufferOITCallback(VistaABufferOIT* pOIT, bool b)
      : m_pOIT(pOIT)
      , m_bPrerender(b) {
  }

  virtual bool Do() {
    if (m_bPrerender) {
      m_pOIT->ClearABuffer();
    } else {
      m_pOIT->DisplayABuffer();
    }

    return true;
  }

  virtual bool GetBoundingBox(VistaBoundingBox& bb) {
    return false;
  }

 private:
  VistaABufferOIT* m_pOIT;
  bool             m_bPrerender;
};

/*============================================================================*/
/* VistaABufferOIT	                                                          */
/*============================================================================*/
/******************************************************************************/
/* CON-/DESTRCUTOR	                                                          */
/******************************************************************************/
VistaABufferOIT::VistaABufferOIT()
    : m_pDisplayShader(NULL)
    , m_pDefaultShader(NULL)
    , m_pNodeClear(NULL)
    , m_pNodeDisplay(NULL)
    , m_pABuffer(NULL)
    , m_pOpaqueDepthTexture(NULL) {
}

VistaABufferOIT::~VistaABufferOIT() {
  delete m_pDefaultShader;
  delete m_pDisplayShader;

  delete m_pNodeClear;
  delete m_pNodeDisplay;

  delete m_pABuffer;

  if (m_pOpaqueDepthTexture)
    delete m_pOpaqueDepthTexture;
}

/******************************************************************************/
/* PUBLIC INTERFACE	                                                          */
/******************************************************************************/
bool VistaABufferOIT::Init(EABufferImplementation eABufferImp, unsigned int uiFragmentesPerPixel,
    unsigned int uiPageSize /*= 4*/) {
  if (m_pABuffer) {
    delete m_pABuffer;
    m_pABuffer = NULL;
  }

  switch (eABufferImp) {
  case ABUFFER_IMPLEMENTATION_ARRAY:
    m_pABuffer = new VistaABufferArray();
    break;
  case ABUFFER_IMPLEMENTATION_LINKEDLIST:
    m_pABuffer = new VistaABufferLinkedList();
    break;
  case ABUFFER_IMPLEMENTATION_PAGES:
    m_pABuffer = new VistaABufferPages(uiPageSize);
    break;
  default:
    vstr::errp() << "[VistaABufferOIT] unknown A-Buffer-Implementation" << std::endl;
    return false;
  }

  // Init A-Buffer
  if (!InitABuffer(uiFragmentesPerPixel, uiPageSize))
    return false;
  // Init Shader
  if (!InitShader())
    return false;
  // Init OpaqueDepthTexture
  if (!InitOpaqueDepthTexture())
    return false;

  return true;
}

bool VistaABufferOIT::AddToSceneGraph(VistaSceneGraph* pSG) {
  if (m_pNodeClear || m_pNodeDisplay)
    return false; // VistaABufferOIT has already been added to a SceneGraph

  m_pNodeClear   = pSG->NewOpenGLNode(NULL, new VistaABufferOITCallback(this, true));
  m_pNodeDisplay = pSG->NewOpenGLNode(NULL, new VistaABufferOITCallback(this, false));

  pSG->GetRealRoot()->InsertChild(m_pNodeClear, 0);
  pSG->GetRealRoot()->AddChild(m_pNodeDisplay);

  return true;
}

VistaOpenGLNode* VistaABufferOIT::GetClearNode() const {
  return m_pNodeClear;
}

VistaOpenGLNode* VistaABufferOIT::GetDisplayNode() const {
  return m_pNodeDisplay;
}

void VistaABufferOIT::SetOpenGLNodes(VistaOpenGLNode* pNodeClear, VistaOpenGLNode* pNodeDisplay) {
  m_pNodeClear   = pNodeClear;
  m_pNodeDisplay = pNodeDisplay;
}

/******************************************************************************/
VistaGLSLShader* VistaABufferOIT::GetDefaultShader() {
  return m_pDefaultShader;
}

VistaGLSLShader* VistaABufferOIT::CreateShaderPrototype() {
  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  std::string strShaderSource = rShaderReg.RetrieveShader(g_strShader_StoreFrag_Aux);

  if (strShaderSource.empty())
    return NULL;

  VistaGLSLShader* pShader = m_pABuffer->CreateShaderPrototype();

  if (pShader != NULL) {
    pShader->InitFragmentShaderFromString(strShaderSource);
  }

  return pShader;
}

bool VistaABufferOIT::RegisterShader(VistaGLSLShader* pShader) {
  if (!pShader || !pShader->IsReadyForUse())
    return false;

  if (!m_pABuffer->RegisterShader(pShader))
    return false;

  m_vecShader.push_back(pShader);
  return true;
}

bool VistaABufferOIT::DeregisterShader(VistaGLSLShader* pShader) {
  if (!m_pABuffer->DeregisterShader(pShader))
    return false;

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
void VistaABufferOIT::ClearABuffer() {
  // Get ViewPort Size
  glGetIntegerv(GL_VIEWPORT, m_aViewport);

  m_pABuffer->SetABufferSize(m_aViewport[2], m_aViewport[3]);

  for (size_t n = 0; n < m_vecShader.size(); ++n) {
    int iLoc = m_vecShader[n]->GetUniformLocation("u_v4Viewport");
    if (iLoc != -1) {
      glProgramUniform4iv(m_vecShader[n]->GetProgram(), iLoc, 1, m_aViewport);
    }
  }

  m_pABuffer->ClearABuffer();
}

void VistaABufferOIT::DisplayABuffer() {
  if (m_pDisplayShader == NULL)
    return;

  m_pOpaqueDepthTexture->Bind();
  glCopyTexImage2D(
      GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT, 0, 0, m_aViewport[2], m_aViewport[3], 0);

  m_pABuffer->Barrier();

  m_pDisplayShader->Bind();

  glPushAttrib(GL_BLEND);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  VistaOGLUtils::BeginOrtho2D(0, 1, 0, 1);

  glBegin(GL_QUADS);
  glVertex2f(0.0, 0.0);
  glVertex2f(1.0, 0.0);
  glVertex2f(1.0, 1.0);
  glVertex2f(0.0, 1.0);
  glEnd();

  VistaOGLUtils::EndOrtho();

  glPopAttrib();

  m_pDisplayShader->Release();

  m_pOpaqueDepthTexture->Unbind();
}

/******************************************************************************/
/* PRIVATE INTERFACE	                                                      */
/******************************************************************************/
bool VistaABufferOIT::InitABuffer(unsigned int uiFragmentesPerPixel, unsigned int uiPageSize) {
  m_pABuffer->Init();
  m_pABuffer->SetFragmenstPerPixel(uiFragmentesPerPixel);
  m_pABuffer->AddDataField(sizeof(float), "u_pFragDepth");
  m_pABuffer->AddDataField(sizeof(int), "u_pFragColor");

  return true;
}

bool VistaABufferOIT::InitShader() {
#ifdef _DEBUG
  vstr::debugi() << "[VistaABufferOIT] INIT SHADER" << std::endl;
  vstr::IndentObject oIndent;
#endif // _DEBUG

  VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

  std::string strShader[5];
  strShader[0] = rShaderReg.RetrieveShader(g_strShader_Display_Frag);
  strShader[1] = rShaderReg.RetrieveShader(g_strShader_Default_Vert);
  strShader[2] = rShaderReg.RetrieveShader(g_strShader_Default_Frag);
  strShader[3] = rShaderReg.RetrieveShader(g_strShader_StoreFrag_Aux);
  strShader[4] = rShaderReg.RetrieveShader(g_strShader_Lighting_Aux);

  if (strShader[0].empty() || strShader[1].empty() || strShader[2].empty() ||
      strShader[3].empty() || strShader[4].empty()) {
    vstr::errp() << "[VistaABufferOIT] - required shader not found." << std::endl;
    vstr::IndentObject oIndent;
    if (strShader[0].empty())
      vstr::erri() << "Can't find " << g_strShader_Display_Frag << std::endl;
    if (strShader[1].empty())
      vstr::erri() << "Can't find " << g_strShader_Default_Vert << std::endl;
    if (strShader[2].empty())
      vstr::erri() << "Can't find " << g_strShader_Default_Frag << std::endl;
    if (strShader[3].empty())
      vstr::erri() << "Can't find " << g_strShader_StoreFrag_Aux << std::endl;
    if (strShader[4].empty())
      vstr::erri() << "Can't find " << g_strShader_Lighting_Aux << std::endl;
    return false;
  }

  m_pDisplayShader = m_pABuffer->CreateShaderPrototype();
  m_pDefaultShader = m_pABuffer->CreateShaderPrototype();

  if (!m_pDisplayShader || !m_pDefaultShader)
    return false;

  m_pDisplayShader->InitFragmentShaderFromString(strShader[0]);

  m_pDefaultShader->InitFromStrings(strShader[1], strShader[2]);
  m_pDefaultShader->InitFragmentShaderFromString(strShader[3]);
  m_pDefaultShader->InitFragmentShaderFromString(strShader[4]);

  if (!m_pDisplayShader->Link() || !m_pDefaultShader->Link()) {
    delete m_pDisplayShader;
    delete m_pDefaultShader;
    m_pDisplayShader = NULL;
    m_pDefaultShader = NULL;
    vstr::errp() << "[VistaABufferOIT] - Can't link shader" << std::endl;
    return false;
  }

  RegisterShader(m_pDisplayShader);
  RegisterShader(m_pDefaultShader);

  return true;
}

bool VistaABufferOIT::InitOpaqueDepthTexture() {

  m_pOpaqueDepthTexture = new VistaTexture(GL_TEXTURE_RECTANGLE);

  m_pOpaqueDepthTexture->Bind();
  m_pOpaqueDepthTexture->SetWrapS(GL_CLAMP);
  m_pOpaqueDepthTexture->SetWrapT(GL_CLAMP);
  m_pOpaqueDepthTexture->SetMinFilter(GL_NEAREST);
  m_pOpaqueDepthTexture->SetMagFilter(GL_NEAREST);
  m_pOpaqueDepthTexture->Unbind();

  return true;
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
