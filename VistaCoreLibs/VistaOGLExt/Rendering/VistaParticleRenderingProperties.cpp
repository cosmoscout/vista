/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                             .                              */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      FileName :  VflGpuParticleRenderer.cpp   RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :  VistaFlowLib                 RR  R    WW  WW  TT  HH  HH  */
/*                                                                            */
/*      Project  :  ViSTA                          Rheinisch-Westfaelische    */
/*                                               Technische Hochschule Aachen */
/*      Purpose  :  ...                                                       */
/*                                                                            */
/*                                                 Copyright (c)  1998-2016   */
/*                                                 by  RWTH-Aachen, Germany   */
/*                                                 All rights reserved.       */
/*                                             .                              */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES			                                                          */
/*============================================================================*/
#include "VistaParticleRenderingProperties.h"

using namespace std;
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaParticleRenderingProperties::VistaParticleRenderingProperties()
    : m_iDrawMode(DM_BILLBOARDS)
    , m_iBlendingMode(ADDITIVE_BLENDING)
    , m_iLightingMode(LIGHTING_PHONG)
    , m_bRenderHalos(false)
    , m_fHaloSize(1.25f)
    , m_oHaloColor(VistaColor::BLACK)
    , m_iResolution(256)
    , m_bGenMipmaps(true) {
}

VistaParticleRenderingProperties::~VistaParticleRenderingProperties() {
}

/******************************************************************************/

int VistaParticleRenderingProperties::GetDrawMode() const {
  return m_iDrawMode;
}
int VistaParticleRenderingProperties::GetBlendingMode() const {
  return m_iBlendingMode;
}
int VistaParticleRenderingProperties::GetLightingMode() const {
  return m_iLightingMode;
}

bool VistaParticleRenderingProperties::GetRenderHalos() const {
  return m_bRenderHalos;
}
float VistaParticleRenderingProperties::GetHaloSize() const {
  return m_fHaloSize;
}
const VistaColor& VistaParticleRenderingProperties::GetHaloColor() const {
  return m_oHaloColor;
}

int VistaParticleRenderingProperties::GetTextureResolution() const {
  return m_iResolution;
}
bool VistaParticleRenderingProperties::GetGenerateMipmaps() const {
  return m_bGenMipmaps;
}

/******************************************************************************/

bool VistaParticleRenderingProperties::SetDrawMode(int iMode) {
  if (iMode < DM_SIMPLE || iMode >= DM_LAST)
    return false; // invalid draw mode
  if (m_iDrawMode == iMode)
    return false; // noting changed

  m_iDrawMode = iMode;
  Notify(MSG_DRAW_MODE_CHANGED);
  return true;
}

bool VistaParticleRenderingProperties::SetBlendingMode(int iMode) {
  if (iMode != ADDITIVE_BLENDING && iMode != ALPHA_BLENDING)
    return false; // invalid blending mode
  if (m_iBlendingMode == iMode)
    return false; // noting changed

  m_iBlendingMode = iMode;
  Notify(MSG_BLEND_MODE_CHANGED);
  return true;
}
bool VistaParticleRenderingProperties::SetLightingMode(int iMode) {
  if (iMode != LIGHTING_DIFFUSE && iMode != LIGHTING_PHONG && iMode != LIGHTING_GOOCH)
    return false; // invalid lighting mode
  if (m_iLightingMode == iMode)
    return false; // noting changed

  m_iLightingMode = iMode;
  Notify(MSG_LIGHTING_MODE_CHANGED);
  return true;
}

bool VistaParticleRenderingProperties::SetRenderHalos(bool b) {
  if (m_bRenderHalos == b)
    return false; // noting changed

  m_bRenderHalos = b;
  Notify(MSG_RENDER_HALOS_CHANGED);
  return true;
}
bool VistaParticleRenderingProperties::SetHaloSize(float fSize) {
  if (m_fHaloSize == fSize)
    return false; // noting changed

  m_fHaloSize = fSize;
  Notify(MSG_HALO_SIZE_CHANGED);
  return true;
}
bool VistaParticleRenderingProperties::SetHaloColor(const VistaColor& rColor) {
  if (m_oHaloColor == rColor)
    return false; // noting changed

  m_oHaloColor = rColor;
  Notify(MSG_HALO_COLOR_CHANGED);
  return true;
}

bool VistaParticleRenderingProperties::SetTextureResolution(int iResolution) {
  if (m_iResolution == iResolution)
    return false; // noting changed

  m_iResolution = iResolution;
  Notify(MSG_TEXTURE_RESOLUTION_CHANGED);
  return true;
}
bool VistaParticleRenderingProperties::SetGenerateMipmaps(bool bGenMipmaps) {
  if (m_bGenMipmaps == bGenMipmaps)
    return false; // noting changed

  m_bGenMipmaps = bGenMipmaps;
  Notify(MSG_GENERATE_MIPMAPS_CHANGED);
  return true;
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
