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

#ifndef _VISTALIGHTNODE_H
#define _VISTALIGHTNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaKernel/GraphicsManager/VistaLeafNode.h"
#include "VistaKernel/GraphicsManager/VistaSceneGraph.h"
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGroupNode;
class IVistaNodeBridge;
class IVistaNodeData;

enum VISTA_LIGHTTYPE {
  VISTA_LIGHTTYPE_NONE = -1,
  VISTA_AMBIENT_LIGHT  = 0,
  VISTA_DIRECTIONAL_LIGHT,
  VISTA_POINT_LIGHT,
  VISTA_SPOT_LIGHT
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaLightNode : public VistaLeafNode {
 public:
  virtual ~VistaLightNode();
  virtual bool CanHaveChildren() const;

  /** Retrieve current ambient light color
   * @param float& r : ambient color's red fraction
   * @param float& g : ambient color's green fraction
   * @param float& b : ambient color's blue fraction
   * @return bool true/false
   */
  bool       GetAmbientColor(float& r, float& g, float& b) const;
  VistaColor GetAmbientColor() const;

  /** Set current ambient light color
   * @param float r : ambient color's red fraction
   * @param float g : ambient color's green fraction
   * @param float b : ambient color's blue fraction
   * @return bool true/false
   */
  bool SetAmbientColor(float r, float g, float b);
  bool SetAmbientColor(const VistaColor& oColor);

  /** Retrieve current diffuse light color
   * @param float& r : diffuse color's red fraction
   * @param float& g : diffuse color's green fraction
   * @param float& b : diffuse color's blue fraction
   * @return bool true/false
   */
  bool       GetDiffuseColor(float& r, float& g, float& b) const;
  VistaColor GetDiffuseColor() const;

  /** Set current diffuse light color
   * @param float r : diffuse color's red fraction
   * @param float g : diffuse color's green fraction
   * @param float b : diffuse color's blue fraction
   * @return bool true/false
   */
  bool SetDiffuseColor(float r, float g, float b);
  bool SetDiffuseColor(const VistaColor& oColor);

  /** Retrieve current specular light color
   * @param float& r : specular color's red fraction
   * @param float& g : specular color's green fraction
   * @param float& b : specular color's blue fraction
   * @return bool true/false
   */
  bool       GetSpecularColor(float& r, float& g, float& b) const;
  VistaColor GetSpecularColor() const;

  /** Set current specular light color
   * @param float r : specular color's red fraction
   * @param float g : specular color's green fraction
   * @param float b : specular color's blue fraction
   * @return bool true/false
   */
  bool SetSpecularColor(float r, float g, float b);
  bool SetSpecularColor(const VistaColor& oColor);

  bool SetColor(float r, float g, float b);
  bool SetColor(const VistaColor& oColor);

  float GetIntensity() const;
  bool  SetIntensity(float i);

  /**
   * Retrieve Attenuation
   * @return VistaVector3D
   */
  VistaVector3D GetAttenuation() const;

  /**
   * Set Attenuation
   * @param VistaVector3D& pAtt new attenuation vector
   * @return bool true/false
   */
  bool SetAttenuation(const VistaVector3D& pAtt);

  /** Turns the light on/off
   */
  void SetIsEnabled(bool bEnabled);
  /** Retrieve current state (on/off)
   * @return bool true/false
   */
  bool GetIsEnabled() const;

  //###################################################
  // deprecated, DONT USE!
  bool Enable();
  bool Disable();
  bool IsEnabled() const;
  //###################################################

  /** Retrieve the light type
   * @return VISTA_LIGHTTYPE
   */
  VISTA_LIGHTTYPE GetLightType() const;

 protected:
  VistaLightNode();
  VistaLightNode(VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData,
      const std::string& strName);
  VISTA_LIGHTTYPE m_nLightType;

 protected:
};

class VISTAKERNELAPI VistaAmbientLight : public VistaLightNode {
  friend class IVistaNodeBridge;
  // friend class IVistaNodeBridge;
 public:
 protected:
  VistaAmbientLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData,
      const std::string& strName);
  virtual ~VistaAmbientLight();
};

class VISTAKERNELAPI VistaDirectionalLight : public VistaLightNode {
  friend class IVistaNodeBridge;

 public:
  /** Retrieve direction
   * @param VistaVector3D* pDir : Pointer to vector that takes the current light direction
   * @return bool true/false
   */
  VistaVector3D GetDirection() const;

  /** Set current light direction
   * @param VistaVector3D* pDir : Pointer to new direction vector
   * @return bool true/false
   */
  bool SetDirection(const VistaVector3D& pDir);

  virtual ~VistaDirectionalLight();

 protected:
  VistaDirectionalLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData,
      const std::string& strName);
};

class VISTAKERNELAPI VistaPointLight : public VistaLightNode {
  // friend class VistaSceneGraph;
  friend class IVistaNodeBridge;

 public:
  /** Retrieve position
   * @param VistaVector3D* pPos : Pointer to vector that takes the current light position
   * @return bool true/false
   */
  VistaVector3D GetPosition() const;
  /** Set current light position
   * @param VistaVector3D* pPos : Pointer to new position
   * @return bool true/false
   */
  bool SetPosition(const VistaVector3D& pPos);
  /** Retrieve current ambient light color
   * @param float& r : ambient color's red fraction
   * @param float& g : ambient color's green fraction
   * @param float& b : ambient color's blue fraction
   * @return bool true/false
   */
  virtual ~VistaPointLight();

 protected:
  VistaPointLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData,
      const std::string& strName);
};

class VISTAKERNELAPI VistaSpotLight : public VistaLightNode {
  friend class IVistaNodeBridge;

 public:
  VistaVector3D GetPosition() const;
  /** Set current light position
   * @param VistaVector3D* pPos : Pointer to new position
   * @return bool true/false
   */
  bool SetPosition(const VistaVector3D& pPos);
  /** Retrieve direction
   * @param VistaVector3D* pDir : Pointer to vector that takes the current light direction
   * @return bool true/false
   */
  VistaVector3D GetDirection() const;
  /** Set current light direction
   * @param VistaVector3D* pDir : Pointer to new direction vector
   * @return bool true/false
   */
  bool SetDirection(const VistaVector3D& pDir);
  /** Retrieve current ambient light color
   * @param float& r : ambient color's red fraction
   * @param float& g : ambient color's green fraction
   * @param float& b : ambient color's blue fraction
   * @return bool true/false
   */

  virtual ~VistaSpotLight();

  enum eLightDistribution { SPOT_DIST_UNIFORM = 0, SPOT_DIST_AVERAGE = 64, SPOT_DIST_DENSE = 128 };

  bool SetSpotDistribution(eLightDistribution eLd);
  bool SetSpotDistribution(int iDist);
  int  GetSpotDistribution() const;

  static const float SPOT_TORCH;
  static const float SPOT_DESKLAMP;
  static const float SPOT_SHADED_BULB;

  bool  SetSpotCharacter(float eC);
  float GetSpotCharacter() const;

 protected:
  VistaSpotLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData,
      const std::string& strName);
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALIGHTNODE_H
