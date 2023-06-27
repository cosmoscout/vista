/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2014 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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
// $Id: VistaVolumeRaycasterCore.h 41859 2014-05-08 15:07:56Z sp841227 $
//
#ifndef __VistaVolumeRaycasterCore_h
#define __VistaVolumeRaycasterCore_h

#include "VistaVolumeRenderingCore.h"

class VistaBufferObject;
class VistaVertexArrayObject;
class VistaGLSLShader;
class VistaTexture;
class VistaFramebufferObj;

#include <string>
#include <vector>

/**
 * If you use this renderer and especially plan to specify your own sampling
 * shader via SetSamplingShaderFilename, note the following use of textue units:
 *   0  --> ray entry points (fixed, do not change)
 *   1  --> ray exit points (fixed, do not change)
 *   2  --> depth texture (fixed, do not change)
 *	 3  --> jitter texture (fixed, do not change)
 *	 4+ --> free to be used by user, use Declare*Texture() functions
 * Also note, that texture unit 4 is also assumed to have the volume data bound
 * that is used during lighting calculations to determine the normal gradient.
 * Change the value of the uniform variable to that texture unit that should be
 * used for these calculations (this only has to be done if you use lighting).
 * In short, do not use texture units 0 to 2 for your own shader in any case.
 * All other texture units should be used depending on the sample shader.
 *
 * @TODO The following is still missing:
 *		1. Pre-integration to improve sampling quality
 */
class VISTAOGLEXTAPI VistaVolumeRaycasterCore : public IVistaVolumeRenderingCore {
 public:
  VistaVolumeRaycasterCore();
  virtual ~VistaVolumeRaycasterCore();

  //! Sets the raycast step size used to traverse the volume data.
  /**
   *  @param fSize Step size that is used to traverse the volume. Given in
   *		   size relative to the (volume) extents. Smaller values usually
   *		   yield a better quality at the cost of speed. Values must be
   *		   greater than 0.
   */
  bool  SetStepSize(const float fSize);
  float GetStepSize() const;

  //! Sets the alpha saturation limit which is used for early ray termination.
  /**
   * @param fLimit The saturation level that is used for early ray termination.
   *		  If a pixel's alpha value has reached the limit ray traversal is
   *		  terminated. Values must be in the range [0, 1].
   */
  bool  SetAlphaSaturationLimit(const float fLimit);
  float GetAlphaSaturationLimit() const;

  //! Sets the alpha compensation factor used to modulate sample alpha values.
  /**
   * @param fFactor The factor by which to compensate the alpha value of every
   *		  sample that is taken from the volume data during ray traversal.
   *		  Values must be greather than 0.
   */
  bool  SetAlphaCompensationFactor(const float fFactor);
  float GetAlphaCompensationFactor() const;

  //! Switches on/off gradient-based lighting
  void SetIsLightingActive(const bool bIsActive);
  bool GetIsLightingActive() const;

  void SetPerformDepthTest(const bool bIsActive);
  bool GetPerformDepthTest() const;

  void SetPerformRayJittering(const bool bIsActive);
  bool GetPerformRayJittering() const;

  // Only provide the full filename but without the path.
  bool        SetSamplerShaderFilename(const std::string& strFilename);
  std::string GetSamplerShaderFilename() const;

  // *** IVistaVolumeRenderingCore support interface ***
  /**
   * Declare a certain sampler to represent a volume or lookup texture. Other
   * uniform variables can be semantically declared to be lookup range
   * variables. Note, that all resources to be used need to be declared
   * first. All of the below functions return a value >= 0 in case of success.
   * Use that value as id with the appropriate setters & getters. A return
   * value of -1 indicates an error. Re-declarations and invalid sampler names
   * are also errors. Note, that all declarations become void on a successful
   * call to SetSamplerShaderFilename().
   */
  int DeclareVolumeTexture(const std::string& strSamplerName);
  int DeclareLookupTexture(const std::string& strSamplerName);
  int DeclareLookupRange(const std::string& strUniformName);

  /**
   * Indicates whether to base the calculation of the gradient needed for
   * proper lighting on the volume density (false, default) or on the alpha
   * value of the classified density value, i.e. the color returned from the
   * lookup table (true).
   * A common use case for the latter is the application of windowed lookup
   * table to cut-out certain parts of a volume by means of alpha modulation.
   */
  void SetUseAlphaClassifiedGradient(const bool bUse);
  bool GetUseAlphaClassifiedGradient() const;

  /**
   * Indicates, which volume texture and which channel is to be used for
   * lighting computations. The function only has an effect if lighting is
   * turned on.
   * An error will occur (return value is 'false') if an invalid volume id
   * is specified or the channel index is outside the range [0,3]
   * (corresponding to the r, g, b and a channels).
   * Note, that only 3-dimensional scalar fields should be used for lighting.
   * The initial defaults are volume id 0 and channel index 0.
   *
   * @TODO What if a separate volume dataset should be used only for lighting?
   *		 In that case, no uniform sampler might exist in the user-specified
   *		 sampler shader as the volume will not be used during colorization.
   *		 However, a uniform sampler must be specified when a volume texture
   *		 is registered, making it impossible to register the dedicated
   *		 lighting-related volume dataset.
   */
  bool SetGradientVolume(const int iVolumeId, const int iChannelIdx);
  void GetGradientVolume(int& iVolumeId, int& iChannelIdx) const;

  // *** IVistaVolumeRenderingCore interface ***
  virtual bool          SetVolumeTexture(VistaTexture* pVolumeTexture, int iVolumeId);
  virtual VistaTexture* GetVolumeTexture(int iVolumeId) const;

  virtual bool SetExtents(float aDataExtents[3]);
  virtual void GetExtents(float aDataExtents[3]);

  virtual bool          SetLookupTexture(VistaTexture* pLookupTexture, int iLookupId);
  virtual VistaTexture* GetLookupTexture(int iLookupId) const;

  virtual bool SetLookupRange(
      float* pLookupRange, int iRangeId, unsigned int uiNumComponents, unsigned int uiNumVectors);
  virtual bool GetLookupRange(float* pLookupRange, int iRangeId, unsigned int uiNumComponents,
      unsigned int uiNumVectors) const;

  virtual void Draw();

 protected:
  struct Texture {
    Texture()
        : m_iTextureUnit(-1)
        , m_pTexture(NULL) {
    }

    bool operator==(const std::string& strSamplerName) const {
      return (strSamplerName == m_strSamplerName);
    }

    std::string   m_strSamplerName;
    int           m_iTextureUnit;
    VistaTexture* m_pTexture;
  };

  struct UniformVariable {
    UniformVariable()
        : m_uiNumComponents(0)
        , m_uiNumVectors(0)
        , m_pRangeValues(NULL) {
    }

    bool operator==(const std::string& strUniformName) const {
      return (strUniformName == m_strUniformName);
    }

    std::string  m_strUniformName;
    unsigned int m_uiNumComponents;
    unsigned int m_uiNumVectors;
    float*       m_pRangeValues;
  };

  virtual bool BuildShaders(const bool bPreserveUserUniforms); //< Also for re-building
  void         RefreshUniforms();
  virtual void RefreshUniformsForShader(VistaGLSLShader* pShader);

  void UpdateDepthTexture();

  virtual void GenerateRays();

  virtual void PerformRaycasting();
  virtual void DrawProxyGeometry();

  // Returns 'true' if in volume. In that case aTexCoordsForViewer is valid
  // and holds the texture coordinates for the viewer position within the
  // volume. If 'false' is returned its contents are undefined.
  virtual bool IsInsideVolume(float aTexCoordsForViewer[3]);

  // Returns 'true' if the viewport was resized
  bool CheckForViewportResize();

  virtual void ResizeTextures();

  virtual void GenerateProxyGeometry();

  // Checks if an uniform variable exists and has not been declared, yet
  bool         IsUniformAvailable(const std::string& strUniformName) const;
  bool         IsUniformDeclared(const std::string& strUniformName) const;
  int          DeclareTexture(const std::string& strSamplerName, std::vector<Texture>& vecTarget);
  unsigned int GetNumberOfTextures() const;

  void UpdateJitterTexture();

  VistaBufferObject*      m_pProxyGeometry;
  VistaVertexArrayObject* m_pProxyGeometrySetup;

  VistaGLSLShader*     m_pRayGenerationShader;
  VistaGLSLShader*     m_pRaycastOutsideShader;
  VistaGLSLShader*     m_pRaycastInsideShader;
  VistaFramebufferObj* m_pRayGenerationFBO;

  int m_iUserTextureOffset;

  VistaTexture* m_pEntryPoints;
  VistaTexture* m_pExitPoints;

  VistaTexture* m_pDepthTexture;
  bool          m_bPerformDepthTest;

  bool          m_bPerformRayJittering;
  int           m_iJitterTextureSize;
  VistaTexture* m_pJitterTexture;

  bool  m_bUseAlphaClassifiedGradient;
  int   m_iGradientVolumeId;
  int   m_iGradientChannelIdx;
  float m_fStepSize;
  float m_fAlphaSaturationLimit;
  float m_fAlphaCompensationFactor;
  bool  m_bIsLightingActive;

  // 0 --> width, 1 --> height
  int m_aViewportSize[2];

  // *** IVistaVolumeRenderingCore induced ***
  float m_aDataExtents[3];

  std::vector<Texture>         m_vecVolumeTextures;
  std::vector<Texture>         m_vecLookupTextures;
  std::vector<UniformVariable> m_vecLookupRanges;

  std::string m_strSampleShaderFilename;
};

#endif // Include guard.
