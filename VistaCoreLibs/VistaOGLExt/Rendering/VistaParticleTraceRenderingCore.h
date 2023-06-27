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

#ifndef __VISTAPARTICLETRACERENDERINGCORE_H
#define __VISTAPARTICLETRACERENDERINGCORE_H

#include "../VistaOGLExtConfig.h"

#include <VistaAspects/VistaObserver.h>

#include <VistaBase/VistaColor.h>
#include <VistaBase/VistaVector3D.h>

class VistaTexture;
class VistaGLSLShader;
class VflLookupTexture;
class VistaBufferObject;
class VistaFramebufferObj;
class VistaVertexArrayObject;
class VistaParticleRenderingProperties;

class VISTAOGLEXTAPI VistaParticleTraceRenderingCore : public IVistaObserver {
 public:
  VistaParticleTraceRenderingCore();
  virtual ~VistaParticleTraceRenderingCore();

  bool Init();

  void Draw();

  VistaParticleRenderingProperties* GetProperties();

  void  SetLineWidth(float fWidth);
  float GetLineWidth() const;

  void  SetParticleRadius(float fRadius);
  float GetParticleRadius() const;
  /**
   * Sets the particle Color.
   * This color will be used if no LUT is used.
   */
  void              SetParticleColor(const VistaColor& rColor);
  const VistaColor& GetParticleColor() const;

  /**
   * Sets the lookup texture which is used to map scalar values to colors.
   */
  void          SetLookupTexture(VistaTexture* pLUT);
  VistaTexture* GetLookupTexture() const;

  void SetLookupRange(float fMin, float fMax);
  void GetLookupRange(float& fMin, float& fMax) const;

  void                 SetViewerPosition(const VistaVector3D& v3ViewerPos);
  const VistaVector3D& GetViewerPosition() const;

  void                 SetLightDirection(const VistaVector3D& v3LightDir);
  const VistaVector3D& GetLightDirection() const;

  /**
   * Used to reducing the tubelet luminance along the time window.
   * The luminance is computes by:
   * 1.0 - LuminanceReductionFactor*( MaxLuminanceFactor - LuminanceFactor )
   */
  bool  SetMaxLuminanceFactor(float fFactor);
  float GetMaxLuminanceFactor() const;

  bool  SetLuminanceReductionFactor(float fFactor);
  float GetLuminanceReductionFactor() const;

  /**
   * The PrimitiveRestartIndex is used in the index buffer object to
   * signalize the end of a GL_TRIANGLE_STRIP.
   */
  void         SetPrimitiveRestartIndex(unsigned int n);
  unsigned int GetPrimitiveRestartIndex() const;

  /**
   *	Sets the VertexArrayObject with contains the vertices and indices
   *  of the tubelet bodies, which will be renderd as GL_TRIANGLE_STRIPs.
   *
   *	The VAO must be specified as follows:
   *	GL_VERTEX_ATTRIB_ARRAY0 should contain particle positions
   *	GL_VERTEX_ATTRIB_ARRAY1 should contain particle directions
   *	GL_VERTEX_ATTRIB_ARRAY2 should contain scalar value
   *	GL_VERTEX_ATTRIB_ARRAY3 should contain luminance factor
   *	GL_VERTEX_ATTRIB_ARRAY4 should contain vertex offsets
   *
   * There must be two vertices for every particle on a particle track.
   * These two vertices should only differ in the vertex offsets.
   * on should have the offset (0,-1) and the other (0,1).
   * Additionally two more verticals ale necessary for the head/tail of every
   * particle track.
   * These two vertices should have the offsets (-1,-1) and (-1,1).
   */
  void SetBodyVBO(VistaVertexArrayObject* pVAO);
  void SetNumBodyIndices(unsigned int n);

  /**
   *	Sets the VertexArrayObject with contains the vertices of the caps
   *
   *	The VAO must be specified as follows:
   *	GL_VERTEX_ATTRIB_ARRAY0 should contain cap positions
   *	GL_VERTEX_ATTRIB_ARRAY1 should contain the inverted normals of the Caps
   *	GL_VERTEX_ATTRIB_ARRAY2 should contain scalar values
   *	GL_VERTEX_ATTRIB_ARRAY3 should contain sim time (used to reduce luminance over time)
   *	GL_VERTEX_ATTRIB_ARRAY3 should contain vertex offsets
   *
   * There must be fore vertices for every cap.
   * These fore vertices should only differ in the vertex offsets.
   * They should have the offsets (-1,-1), (1,-1), (1,1) and (-1,1);
   */
  void SetCapVBO(VistaVertexArrayObject* pVAO);
  void SetCapCount(unsigned int n);

  // observer api
  virtual bool Observes(IVistaObserveable* pObserveable);
  virtual void Observe(IVistaObserveable* pObserveable, int nTicket);
  virtual bool ObserveableDeleteRequest(IVistaObserveable* pObserveable, int nTicket);
  virtual void ObserveableDelete(IVistaObserveable* pObserveable, int nTicket);
  virtual void ReleaseObserveable(IVistaObserveable* pObserveable, int nTicket);

  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int nMsg, int nTicket);

 protected:
  bool InitShaders();
  bool InitTextures();

  VistaGLSLShader* CreateShader(
      const std::string& strVert, const std::string& strFrag, const std::string& strAux = "");

  void DeleteShaders();

  void UpdateTextures();
  void UpdateUniformVariables(VistaGLSLShader* pShader);

  void RenderLines();
  void RenderBodies(VistaGLSLShader*, VistaTexture*);
  void RenderCaps(VistaGLSLShader*, VistaTexture*);
  void RenderCapsIntoDeapthTexture();
  void RenderTransparentBillboards();
  void RenderHalos();

 protected:
  VistaParticleRenderingProperties* m_pProperties;

  float m_fLineWidth;

  float      m_fParticleRadius;
  VistaColor m_oParticleColor;

  VistaTexture* m_pLUT;
  float         m_aLookupRange[2];

  unsigned int m_nPrimitiveRestartIndex;

  VistaVertexArrayObject* m_pBodyVAO;
  VistaVertexArrayObject* m_pCapVAO;

  unsigned int m_nBodyIndices;
  unsigned int m_nCapCount;

  VistaVector3D m_v3ViewerPos;
  VistaVector3D m_v3LightDir;

  float m_fMaxLuminanceFactor;
  float m_fLuminanceReductionFactor;

  VistaGLSLShader* m_pLineShader;
  VistaGLSLShader* m_pSmokeShader;
  VistaGLSLShader* m_pBillboardShader;
  VistaGLSLShader* m_pBumpedShaders[3];
  VistaGLSLShader* m_pDepthShaders[3];

  VistaGLSLShader* m_pHaloShader;

  VistaTexture* m_pBlendingTexture;
  VistaTexture* m_pIlluminationTexture;
  VistaTexture* m_pTubeNormalsTexture;
  VistaTexture* m_pCapNormalsTexture;

  VistaTexture* m_pWhiteTexture;

  VistaFramebufferObj* m_pFBO;
  VistaTexture*        m_pDepthTexture;
  int                  m_aDepthTextureSize[2];
};

#endif // __VFLGPUPARTICLERENDERER_H

/*============================================================================*/
/*  END OF FILE                                                               */
/*============================================================================*/
