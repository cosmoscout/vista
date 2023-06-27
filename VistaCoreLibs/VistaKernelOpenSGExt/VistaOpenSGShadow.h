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

#ifndef _VISTAOPENSGSHADOW_H
#define _VISTAOPENSGSHADOW_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaKernelOpenSGExtConfig.h"

#include <map>
#include <set>
#include <string>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaWindow;
class VistaOpenSGNodeData;
class VistaLightNode;
class IVistaNode;
class VistaDisplayManager;
class VistaGraphicsManager;
class tShadowViewPortNameMap;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGShadow {
 public:
  /**
   * Constructor/destructor
   *
   * Init the shadow object. This automatically creates a shadow viewport
   * for all existing viewports and adds all lights specified in the vista.ini
   * as light source to the shadow-viewports.
   * Use AddLight to add lights after Init () was called. You can't add Viewports afterwards.
   *
   * Shadows start with the following default values:
   *	- shadows enabled
   *  - perspecitve shadows
   *	- light's shadow intensity 1.0
   *  - global  shadow intensity 0.0 (use per-light settings)
   *
   */
  VistaOpenSGShadow(VistaDisplayManager* pMgr, VistaGraphicsManager* pGrMgr);

  ~VistaOpenSGShadow();

  enum VISTAKERNELOPENSGEXTAPI eShadowMode {
    VOSGSHADOW_ERROR = -1,
    VOSGSHADOW_FIRST = 0, // OSG::ShadowViewport::NO_SHADOW,

    VOSGSHADOW_NO_SHADOW = 0,              // OSG::ShadowViewport::NO_SHADOW,
    VOSGSHADOW_STD_SHADOW_MAP = 1,         // OSG::ShadowViewport::STD_SHADOW_MAP,
    VOSGSHADOW_PERSPECTIVE_SHADOW_MAP = 2, // OSG::ShadowViewport::PERSPECTIVE_SHADOW_MAP,
    VOSGSHADOW_DITHER_SHADOW_MAP = 3,      // OSG::ShadowViewport::DITHER_SHADOW_MAP,
    VOSGSHADOW_PCF_SHADOW_MAP = 4,         // OSG::ShadowViewport::PCF_SHADOW_MAP,
    VOSGSHADOW_PCSS_SHADOW_MAP = 5,        // OSG::ShadowViewport::PCSS_SHADOW_MAP,
    VOSGSHADOW_VARIANCE_SHADOW_MAP = 6,    // OSG::ShadowViewport::VARIANCE_SHADOW_MAP,

    VOSGSHADOW_LAST = 6 // OSG::ShadowViewport::VARIANCE_SHADOW_MAP
  };

  /**
   * Add an additonal light to all shadow viewports.
   */
  bool AddLight(VistaLightNode* pLight);

  /**
   * Remove a light from all shadow viewports.
   */
  bool RemoveLight(VistaLightNode* pLight);

  bool ClearAllLights();

  /**
   * Add a node to the exclude-from-shadowing-functionality list.
   */
  bool AddExcludeNode(IVistaNode* pNode);

  /**
   * Remove a node from the exclude-from-shadowing-functionality list.
   */
  bool RemoveExcludeNode(IVistaNode* pNode);

  /**
   * Clears the exclude-from-shadowing-functionality list.
   */
  bool ClearExcludeNodes();

  /**
   * Shadow parameters
   */
  bool  SetOffBias(float fBias);
  float GetOffBias() const;
  bool  SetOffFactor(float fFactor);
  float GetOffFactor() const;

  bool  SetSmoothness(float fSmooth);
  float GetSmoothness() const;
  bool  SetMapSize(int iMapSize);
  int   GetMapSize() const;

  // set the shadow intensity of a specific light
  bool  SetLightShadowIntensity(VistaLightNode* pLight, float fIntensity);
  float GetLightShadowIntensity(const VistaLightNode* pLight) const;

  // set the shadow intensity of all shadowing lights
  bool SetLightsShadowIntensity(float fIntensity);

  // set the ShadowViewport's global shadow-intensity
  // if set != 0.0, the individual shadow intensities of light nodes is ignored!
  void  SetGlobalShadowIntensity(float fIntensity);
  float GetGlobalShadowIntensity() const;

  /**
   * Enable/Disable shadows.
   * This turns all shadow viewports on/off
   *
   */
  void EnableShadow();
  void DisableShadow();
  bool GetIsShadowEnabled();

  /**
   * Set shadow mode
   */
  void SetShadowMode(const eShadowMode& eShadowMode);

  /**
   * Get shadow mode
   */
  eShadowMode GetShadowMode() const;

 private:
  /**
   * init a specific window
   */

  bool InitWindow(const std::string& strName, VistaWindow* pWindow, VistaGraphicsManager* pGrMgr);

  /**
   * members
   */

  tShadowViewPortNameMap*      m_pShadowVPs;
  std::vector<VistaLightNode*> m_vecLights;

  bool m_bInit;
  bool m_bEnabled;

  VistaDisplayManager*  m_pDispMgr;
  VistaGraphicsManager* m_pGrMgr;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTAOPENSGSHADOW_H
