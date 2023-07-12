/*======	======================================================================*/
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

#include "VistaGLLine.h"

#include "VistaGLSLShader.h"
#include "VistaShaderRegistry.h"
#include "VistaTexture.h"

#include <VistaBase/VistaStreamUtils.h>

#include <vector>

using namespace std;
/*============================================================================*/
/*  STATIC Variables                                                          */
/*============================================================================*/
namespace VistaGLLine {
const string g_strDefault_VS       = "VistaGLLine_Default_vert.glsl";
const string g_strDefault_GS       = "VistaGLLine_Default_geom.glsl";
const string g_strAdjacency_GS     = "VistaGLLine_Adjacency_geom.glsl";
const string g_strHaloLine_FS      = "VistaGLLine_HaloLine_frag.glsl";
const string g_strTubelets_FS      = "VistaGLLine_Tubelets_frag.glsl";
const string g_strCylinder_GS      = "VistaGLLine_Cylinder_geom.glsl";
const string g_strCylinder_FS      = "VistaGLLine_Cylinder_frag.glsl";
const string g_strArrow_GS         = "VistaGLLine_Arrow_geom.glsl";
const string g_strArrow_FS         = "VistaGLLine_Arrow_frag.glsl";
const string g_strTruncatedCone_VS = "VistaGLLine_TruncatedCone_vert.glsl";
const string g_strTruncatedCone_GS = "VistaGLLine_TruncatedCone_geom.glsl";
const string g_strTruncatedCone_FS = "VistaGLLine_TruncatedCone_frag.glsl";
const string g_strLighting_Ext     = "Vista_PhongLighting_aux.glsl";

vector<pair<VistaGLSLShader*, bool>> g_vecShader;
int                                  g_iCourentBoundShader = -1;

float g_fLineWith = 0.01f;

float g_fHaloSize   = 0.50f;
float g_fConeRadius = 0.02f;
float g_fConeHeight = 0.04f;

bool g_bComputeLighting = true;

VistaTexture* g_pTexture = NULL;
}; // namespace VistaGLLine

/*============================================================================*/
/*  STATIC FUNKTIONS                                                          */
/*============================================================================*/
namespace VistaGLLine {
void UpdateTexture() {
  const int     iWidth = 64;
  unsigned char aData[iWidth];

  for (int i = 0; i < iWidth; ++i) {
    const float f = static_cast<float>(i) / static_cast<float>(iWidth - 1);
    aData[i]      = (f < g_fHaloSize) ? 0 : 255;
  }

  // exploiting that the texture size cannot change (const int above).
  // thus, a full init is only performed the first time the texture is
  // generated.
  if (!g_pTexture) {
    g_pTexture = new VistaTexture(GL_TEXTURE_1D);
    g_pTexture->UploadTexture(iWidth, 0, &aData, true, GL_ALPHA, GL_UNSIGNED_BYTE);
    g_pTexture->SetWrapS(GL_CLAMP_TO_EDGE);
  } else {
    g_pTexture->Bind();
    glTexSubImage1D(g_pTexture->GetTarget(), 0, 0, iWidth, GL_ALPHA, GL_UNSIGNED_BYTE, &aData);
    glGenerateMipmap(g_pTexture->GetTarget());
    g_pTexture->Unbind();
  }
}

VistaGLSLShader* CreateShader(
    const string& strVS_Name, const string& strGS_Name, const string& strFS_Name) {
  VistaShaderRegistry* pShaderReg = &VistaShaderRegistry::GetInstance();

  string strVS = pShaderReg->RetrieveShader(strVS_Name);
  string strGS = pShaderReg->RetrieveShader(strGS_Name);
  string strFS = pShaderReg->RetrieveShader(strFS_Name);

  if (strVS.empty() || strGS.empty() || strFS.empty()) {
    vstr::errp() << "[VistaGLLine] - required shader not found." << endl;
    vstr::IndentObject oIndent;
    if (strVS.empty())
      vstr::erri() << "Can't find " << strVS_Name << endl;
    if (strGS.empty())
      vstr::erri() << "Can't find " << strGS_Name << endl;
    if (strFS.empty())
      vstr::erri() << "Can't find " << strFS_Name << endl;
    return NULL;
  }

  VistaGLSLShader* pShader = new VistaGLSLShader();

  pShader->InitVertexShaderFromString(strVS);
  pShader->InitGeometryShaderFromString(strGS);
  pShader->InitFragmentShaderFromString(strFS);

  if (!pShader->Link()) {
    delete pShader;
    return NULL;
  }

  return pShader;
}
VistaGLSLShader* CreateShader(const string& strVS_Name, const string& strGS_Name,
    const string& strFS_Name, const string& strEXT_Name) {
  VistaShaderRegistry* pShaderReg = &VistaShaderRegistry::GetInstance();

  string strVS  = pShaderReg->RetrieveShader(strVS_Name);
  string strGS  = pShaderReg->RetrieveShader(strGS_Name);
  string strFS  = pShaderReg->RetrieveShader(strFS_Name);
  string strEXT = pShaderReg->RetrieveShader(strEXT_Name);

  if (strVS.empty() || strGS.empty() || strFS.empty() || strEXT.empty()) {
    vstr::errp() << "[VistaGLLine] - required shader not found." << endl;
    vstr::IndentObject oIndent;
    if (strVS.empty())
      vstr::erri() << "Can't find " << strVS_Name << endl;
    if (strGS.empty())
      vstr::erri() << "Can't find " << strGS_Name << endl;
    if (strFS.empty())
      vstr::erri() << "Can't find " << strFS_Name << endl;
    if (strEXT.empty())
      vstr::erri() << "Can't find " << strEXT_Name << endl;
    return NULL;
  }

  VistaGLSLShader* pShader = new VistaGLSLShader();

  pShader->InitVertexShaderFromString(strVS);
  pShader->InitGeometryShaderFromString(strGS);
  pShader->InitFragmentShaderFromString(strFS);
  pShader->InitFragmentShaderFromString(strEXT);

  if (!pShader->Link()) {
    delete pShader;
    return NULL;
  }

  return pShader;
}

void InitShader(int iShader) {
  VistaGLSLShader* pShader = NULL;
  switch (iShader) {
  case SHADER_HALO_LINES:
    pShader = CreateShader(g_strDefault_VS, g_strDefault_GS, g_strHaloLine_FS);
    break;
  case SHADER_TUBELETS:
    pShader = CreateShader(g_strDefault_VS, g_strDefault_GS, g_strTubelets_FS, g_strLighting_Ext);
    break;
  case SHADER_ADJACENCY_HALO_LINES:
    pShader = CreateShader(g_strDefault_VS, g_strAdjacency_GS, g_strHaloLine_FS);
    break;
  case SHADER_ADJACENCY_TUBELETS:
    pShader = CreateShader(g_strDefault_VS, g_strAdjacency_GS, g_strTubelets_FS, g_strLighting_Ext);
    break;
  case SHADER_CYLINDER:
    pShader = CreateShader(g_strDefault_VS, g_strCylinder_GS, g_strCylinder_FS, g_strLighting_Ext);
    break;
  case SHADER_ARROWS:
    pShader = CreateShader(g_strDefault_VS, g_strArrow_GS, g_strArrow_FS, g_strLighting_Ext);
    break;
  case SHADER_TRUNCATED_CONES:
    pShader = CreateShader(
        g_strTruncatedCone_VS, g_strTruncatedCone_GS, g_strTruncatedCone_FS, g_strLighting_Ext);
    break;
  }
  if (!pShader)
    return;

  g_vecShader[iShader].first  = pShader;
  g_vecShader[iShader].second = false;
  if (iShader == SHADER_ADJACENCY_HALO_LINES || iShader == SHADER_ADJACENCY_TUBELETS)
    g_vecShader[iShader].second = true;
}
}; // namespace VistaGLLine

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaGLLine::SetLineWidth(float fLineWith) {
  if (g_fLineWith == fLineWith)
    return;

  g_fLineWith = fLineWith;

  if (g_iCourentBoundShader >= 0) {
    // a shader is currently bound so we have to update the uniform variable.
    VistaGLSLShader* pShader = g_vecShader[g_iCourentBoundShader].first;
    int              iUniLoc = pShader->GetUniformLocation("u_fLineRadius");
    if (-1 != iUniLoc)
      glUniform1f(iUniLoc, g_fLineWith);
  }
}

void VistaGLLine::SetEnableLighting(bool b) {
  if (g_bComputeLighting == b)
    return;

  g_bComputeLighting = b;

  if (g_iCourentBoundShader >= 0) {
    // a shader is currently bound so we have to update the uniform variable.
    VistaGLSLShader* pShader = g_vecShader[g_iCourentBoundShader].first;
    int              iUniLoc = pShader->GetUniformLocation("u_bComputeLighting");
    if (-1 != iUniLoc)
      glUniform1i(iUniLoc, g_bComputeLighting);
  }
}

void VistaGLLine::SetHaloSize(float fHaloSize) {
  if (g_fHaloSize != fHaloSize) {
    g_fHaloSize = fHaloSize;
    UpdateTexture();
  }
}

void VistaGLLine::SetArrowheadRadius(float fConeRadius) {
  if (g_fConeRadius == fConeRadius)
    return;

  g_fConeRadius = fConeRadius;

  if (g_iCourentBoundShader >= 0) {
    // a shader is currently bound so we have to update the uniform variable.
    VistaGLSLShader* pShader = g_vecShader[g_iCourentBoundShader].first;
    int              iUniLoc = pShader->GetUniformLocation("u_fConeRadius");
    if (-1 != iUniLoc)
      glUniform1f(iUniLoc, g_fConeRadius);
  }
}

void VistaGLLine::SetArrowheadLength(float fConeHeight) {
  if (g_fConeHeight == fConeHeight)
    return;

  g_fConeHeight = fConeHeight;

  if (g_iCourentBoundShader >= 0) {
    // a shader is currently bound so we have to update the uniform variable.
    VistaGLSLShader* pShader = g_vecShader[g_iCourentBoundShader].first;
    int              iUniLoc = pShader->GetUniformLocation("u_fConeHeight");
    if (-1 != iUniLoc)
      glUniform1f(iUniLoc, g_fConeHeight);
  }
}
bool VistaGLLine::Begin(int iLineTyp /*= VISTA_GL_LINES*/) {
  if (g_iCourentBoundShader == -1)
    return false;

  if (g_vecShader[g_iCourentBoundShader].second) {
    if (iLineTyp == VISTA_GL_LINES) {
      glBegin(GL_LINES_ADJACENCY);
      return true;
    }
    if (iLineTyp == VISTA_GL_LINE_STRIP) {
      glBegin(GL_LINE_STRIP_ADJACENCY);
      return true;
    }
  } else {
    if (iLineTyp == VISTA_GL_LINES) {
      glBegin(GL_LINES);
      return true;
    }
    if (iLineTyp == VISTA_GL_LINE_STRIP) {
      glBegin(GL_LINE_STRIP);
      return true;
    }
  }
  return false;
}

bool VistaGLLine::End() {
  if (g_iCourentBoundShader < 0)
    return false;

  glEnd();

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME: Enable                                                              */
/*                                                                            */
/*============================================================================*/

bool VistaGLLine::Enable(int iShader /*= VISTA_GL_HALO_LINE_SHADER*/) {
  if (g_vecShader.size() < NUM_SHADES)
    g_vecShader.resize(NUM_SHADES);

  if (g_iCourentBoundShader >= 0 || iShader < 0 ||
      static_cast<size_t>(iShader) >= g_vecShader.size())
    return false;

  if (!g_vecShader[iShader].first)
    InitShader(iShader);

  VistaGLSLShader* pShader = g_vecShader[iShader].first;

  if (!pShader)
    return false;
  if (!g_pTexture)
    UpdateTexture();

  glPushAttrib(GL_TEXTURE_BIT);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  g_pTexture->Bind();

  pShader->Bind();
  g_iCourentBoundShader = iShader;

  int iUniLoc = pShader->GetUniformLocation("u_fLineRadius");
  if (-1 != iUniLoc)
    glUniform1f(iUniLoc, g_fLineWith);

  iUniLoc = pShader->GetUniformLocation("u_bComputeLighting");
  if (-1 != iUniLoc)
    glUniform1i(iUniLoc, g_bComputeLighting);

  iUniLoc = pShader->GetUniformLocation("u_fConeRadius");
  if (-1 != iUniLoc)
    glUniform1f(iUniLoc, g_fConeRadius);
  iUniLoc = pShader->GetUniformLocation("u_fConeHeight");
  if (-1 != iUniLoc)
    glUniform1f(iUniLoc, g_fConeHeight);

  return true;
}
/*============================================================================*/
/*                                                                            */
/*  NAME: Disable                                                             */
/*                                                                            */
/*============================================================================*/

bool VistaGLLine::Disable() {
  if (g_iCourentBoundShader < 0)
    return false;

  g_vecShader[g_iCourentBoundShader].first->Release();
  g_iCourentBoundShader = -1;

  g_pTexture->Disable();
  glPopAttrib();

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME: AddLineShader                                                       */
/*                                                                            */
/*============================================================================*/
int VistaGLLine::AddLineShader(VistaGLSLShader* pShader, bool bNeedAdjacencyInformations) {
  if (g_vecShader.size() < 6)
    g_vecShader.resize(6);

  if (!pShader)
    return -1;

  g_vecShader.push_back(pair<VistaGLSLShader*, bool>(pShader, bNeedAdjacencyInformations));

  return static_cast<int>(g_vecShader.size()) - 1;
}

/*============================================================================*/
/*                                                                            */
/*  NAME: RemoveLineShader                                                    */
/*                                                                            */
/*============================================================================*/
bool VistaGLLine::RemoveLineShader(VistaGLSLShader* pShader) {
  for (size_t n = 0; n < g_vecShader.size(); ++n) {
    if (g_vecShader[n].first == pShader) {
      g_vecShader[n].first = NULL;
      return true;
    }
  }
  return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME: GetDoesShaderNeedAdjacencyInformations                              */
/*                                                                            */
/*============================================================================*/
bool VistaGLLine::GetDoesShaderNeedAdjacencyInformations(int iShader) {
  if (iShader < 0 || static_cast<size_t>(iShader) >= g_vecShader.size())
    return false;

  return g_vecShader[iShader].second;
}
