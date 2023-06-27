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

#ifndef __VISTAPARTICLERENDERINGPROPERTIES_H
#define __VISTAPARTICLERENDERINGPROPERTIES_H

#include "../VistaOGLExtConfig.h"

#include <VistaAspects/VistaObserveable.h>
#include <VistaBase/VistaColor.h>

class VISTAOGLEXTAPI VistaParticleRenderingProperties : public IVistaObserveable {
 public:
  VistaParticleRenderingProperties();
  virtual ~VistaParticleRenderingProperties();

  enum {
    MSG_DRAW_MODE_CHANGED,
    MSG_BLEND_MODE_CHANGED,
    MSG_LIGHTING_MODE_CHANGED,
    MSG_RENDER_HALOS_CHANGED,
    MSG_HALO_SIZE_CHANGED,
    MSG_HALO_COLOR_CHANGED,
    MSG_TEXTURE_RESOLUTION_CHANGED,
    MSG_GENERATE_MIPMAPS_CHANGED

  };

  enum DRAW_MODE {
    // draw Particles using OpenGL primitives (GL_LINES, GL_POINS)
    DM_SIMPLE,
    // draw Particles as transparent Billboards using a GaussianBlendingTexture
    DM_SMOKE,
    // draw Particles as Billboards using a SphereIlluminationTexture
    DM_BILLBOARDS,
    // draw Particles as Billboards and compute lighting based on a SphereNormalsTexture
    DM_BUMPED_BILLBOARDS,
    // draw Particles as Billboards and compute lighting and correct depth values based on a
    // SphereNormalsTexture
    DM_BUMPED_BILLBOARDS_DEPTH_REPLACE,

    DM_LAST
  };

  enum BLEND_MODE { ADDITIVE_BLENDING, ALPHA_BLENDING };

  enum LIGHTING_MODE { LIGHTING_DIFFUSE, LIGHTING_PHONG, LIGHTING_GOOCH };

  /**
   * Sets the Draw mode with is used to display the particles.
   */
  bool SetDrawMode(int iMode);
  int  GetDrawMode() const;

  /**
   * Sets the blending mode witch is used to blend transparent Billboards.
   * Can be set to ADDITIVE_BLENDING or ALPHA_BLENDING.
   * The BlendingMode only affects the DM_SMOKE draw mode.
   * If ALPHA_BLENDING is used, the particles should be sorted according
   * to their distance to the viewer.
   */
  bool SetBlendingMode(int iMode);
  int  GetBlendingMode() const;

  /**
   * Sets the lighting mode witch is used for the draw modes
   * DM_BUMPED_BILLBOARDS  and DM_BUMPED_BILLBOARDS_DEPTH_REPLACE
   */
  bool SetLightingMode(int iMode);
  int  GetLightingMode() const;

  /**
   * Sets whether halos should be render or not.
   * If BlendingMode != NO_BLENDING or DrawMode == DM_SIMPLE,
   * no halos will be rendered even if RenderHalos is set to true.
   */
  bool SetRenderHalos(bool bRenderHalos);
  bool GetRenderHalos() const;

  /**
   * Sets the size of of the Halo. This size will be multiplied with the
   * particle radius.
   */
  bool  SetHaloSize(float fSize);
  float GetHaloSize() const;

  bool              SetHaloColor(const VistaColor& rColor);
  const VistaColor& GetHaloColor() const;

  /**
   * Sets the resolution of the textures, witch are used when rendering Billboards
   */
  bool SetTextureResolution(int iResolution);
  int  GetTextureResolution() const;

  /**
   * Sets whether Mipmaps should be used for the textures.
   */
  bool SetGenerateMipmaps(bool bGenMipmaps);
  bool GetGenerateMipmaps() const;

 protected:
  int m_iDrawMode;
  int m_iBlendingMode;
  int m_iLightingMode;

  bool       m_bRenderHalos;
  float      m_fHaloSize;
  VistaColor m_oHaloColor;

  int  m_iResolution;
  bool m_bGenMipmaps;
};

#endif // __VFLGPUPARTICLERENDERER_H

/*============================================================================*/
/*  END OF FILE                                                               */
/*============================================================================*/
