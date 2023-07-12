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

#ifndef _VISTAOPENSGPARTICLES_H__
#define _VISTAOPENSGPARTICLES_H__

#include <GL/glew.h>

#include "VistaKernelOpenSGExtConfig.h"

#include <VistaBase/VistaVersion.h>

#ifdef WIN32
#include <windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cstring>
#include <string>

class VistaGroupNode;
class VistaExtensionNode;
class VistaOpenGLNode;
class VistaSceneGraph;
struct VistaOpenSGParticlesData;

class VISTAKERNELOPENSGEXTAPI VistaOpenSGParticles {
 public:
  enum PARTICLESMODES { PM_VIEWERQUADS = 3, PM_DIRECTEDQUADS = 7 };
  VistaOpenSGParticles(
      VistaSceneGraph* pSG, VistaGroupNode* pParent = NULL, int eParticleMode = PM_VIEWERQUADS);
  virtual ~VistaOpenSGParticles();

  void SetColorsWithAlpha(const bool bWithAlpha = true) const;
  void SetNumParticles(const int iNum, const bool bUseSeparateColors = false,
      const bool bUseSeparateSizes = false) const;

  int  GetNumParticles() const;
  int  GetNumColors() const;
  bool GetColorsWithAlpha() const;
  int  GetNumSizes() const;

  // particle systems have to be fast, thus we provide a fast but
  // unsafe interface

  // call these before/after editing the fields!!
  void BeginEdit() const;
  void EndEdit() const;

  // returns a pointer to the 3d positions field.
  float* GetParticlePositions3fField(const int& idx = 0) const;

  // returns a pointer to the 3f color field. maybe NULL if you specified a 4f field
  float* GetParticleColors3fField(const int& idx = 0) const;

  // returns a pointer to the 4f color field. maybe NULL if you specified a 3f field
  float* GetParticleColors4fField(const int& idx = 0) const;
  float* GetParticleColorsXfField(const int& idx = 0) const;

  // returns a pointer to the 3f sizes field.
  float* GetParticleSizes3fField(const int& idx = 0) const;

  /** @todo  : single/multi values */
  // float* GetParticleNormals3fField() const;
  /** @todo : check for 64 bit issue!? */
  // int*   GetParticleIndices1iField() const;

  void SetUseGaussBlobTexture(
      const int& iTexSize = 64, const float& sigma = 8.0f, const float& m = 50.0f) const;
  void SetUseUnscaledGaussBlobTexture(
      const int iTexSize, const float fSigma, const float fCenterHeight) const;
  bool SetUseSpriteImage(const std::string& strFileName) const;

  // set rendering options. default values show the defaults used.
  void SetMaterialTransparency(const float& a = 0.0f) const;
  void SetGLTexEnvMode(const GLenum& eMode = GL_REPLACE) const;
  void SetGLBlendSrcFactor(const GLenum& eMode = GL_SRC_ALPHA) const;
  void SetGLBlendDestFactor(const GLenum& eMode = GL_ONE_MINUS_SRC_ALPHA) const;

  bool GetParticlesAreLit() const;
  void SetParticlesAreLit(bool bSet);

  enum eDrawOrder { Any = 0, BackToFront, FrontToBack };
  void SetDrawOrder(const eDrawOrder& mode = Any) const;

  /**
   * @BRIEF Returns the beginning address of Secondary Particle Position field -
   * [Particle_Number][3]
   */
  float* GetSecParticlePositions3fField(const int& idx = 0) const;

  /**
   * @BRIEF Allows changing the Mode of the Particles!
   */
  void SetMode(const int& mode = 0);
  int  GetMode() const;
  /**
   *@BRIEF Copies the Pos field into the SecPos-Field
   */
  void CopyPosToSecpos() const;
  /**
   * @BRIEF Creating mSHLChunk. Needs the Vertex and Shader Program! Set the Material FIRST!
   */
  void SetShaderPrograms(
      const std::string& VertexShaderProgram, const std::string& FragmentShaderProgram);
  void RemoveShader();

  VistaExtensionNode* GetParticleNode() const;

 private:
  /**
   *@BRIEF Sets the Shader for Particles
   */
  void SetParticleShader();
  /**
   * @BRIEF Sets the Normal for our Particle System (0,1,0)
   */
  void SetNormal() const;
  /**
   * @BRIEF To give UniformParameters to the Shader Programs!
   */
  void SetUniformParameter(const std::string& UniformName, const int& NumValue) const;

  /**
   * @BRIEF Only use this call except you want to go into detail! Creating ShaderQuads
   * (semi-billboarding) with direction (SECOND POSITION ARRAY!!!)
   */
  void                DetachAndDelete();
  VistaExtensionNode* m_pParticleNode;

  // to keep opensg out of the interface we store stuff
  // using a non-exported class
  VistaOpenSGParticlesData* m_pData;

  int m_eParticleMode;
};

#endif
