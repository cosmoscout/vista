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

#include "VistaKernel/GraphicsManager/VistaLightNode.h"
#include "VistaKernel/GraphicsManager/VistaNodeBridge.h"
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaLightNode::VistaLightNode()
    : m_nLightType(VISTA_LIGHTTYPE_NONE) {
  m_nType = VISTA_LIGHTNODE;
}

VistaLightNode::VistaLightNode(VistaGroupNode* pParent, IVistaNodeBridge* pBridge,
    IVistaNodeData* pData, const std::string& strName)
    : VistaLeafNode(pParent, pBridge, pData, strName)
    , m_nLightType(VISTA_LIGHTTYPE_NONE) {
  m_nType = VISTA_LIGHTNODE;
}

VistaLightNode::~VistaLightNode() {
  m_pBridge->UnregisterLightNode(this);
}

// ============================================================================

bool VistaLightNode::SetColor(float r, float g, float b) {
  return m_pBridge->SetLightColor(m_nLightType, r, g, b, m_pData);
}

bool VistaLightNode::SetColor(const VistaColor& oColor) {
  return m_pBridge->SetLightColor(m_nLightType, oColor[0], oColor[1], oColor[2], m_pData);
}

float VistaLightNode::GetIntensity() const {
  return m_pBridge->GetLightIntensity(m_pData);
}
// ============================================================================
// ============================================================================
bool VistaLightNode::SetIntensity(float i) {
  return m_pBridge->SetLightIntensity(i, m_pData);
}

// ============================================================================
// ============================================================================
bool VistaLightNode::GetAmbientColor(float& r, float& g, float& b) const {
  return m_pBridge->GetLightAmbientColor(m_nLightType, r, g, b, m_pData);
}

VistaColor VistaLightNode::GetAmbientColor() const {
  VistaColor oColor;
  GetAmbientColor(oColor[0], oColor[1], oColor[2]);
  return oColor;
}

// ============================================================================
// ============================================================================
bool VistaLightNode::SetAmbientColor(float r, float g, float b) {
  return m_pBridge->SetLightAmbientColor(m_nLightType, r, g, b, m_pData);
}

bool VistaLightNode::SetAmbientColor(const VistaColor& oColor) {
  return SetAmbientColor(oColor[0], oColor[1], oColor[2]);
}

// ============================================================================
// ============================================================================
bool VistaLightNode::GetDiffuseColor(float& r, float& g, float& b) const {
  return m_pBridge->GetLightDiffuseColor(m_nLightType, r, g, b, m_pData);
}

VistaColor VistaLightNode::GetDiffuseColor() const {
  VistaColor oColor;
  GetDiffuseColor(oColor[0], oColor[1], oColor[2]);
  return oColor;
}

// ============================================================================
// ============================================================================
bool VistaLightNode::SetDiffuseColor(float r, float g, float b) {
  return m_pBridge->SetLightDiffuseColor(m_nLightType, r, g, b, m_pData);
}

bool VistaLightNode::SetDiffuseColor(const VistaColor& oColor) {
  return SetDiffuseColor(oColor[0], oColor[1], oColor[2]);
}

// ============================================================================
// ============================================================================
bool VistaLightNode::GetSpecularColor(float& r, float& g, float& b) const {
  return m_pBridge->GetLightSpecularColor(m_nLightType, r, g, b, m_pData);
}

VistaColor VistaLightNode::GetSpecularColor() const {
  VistaColor oColor;
  GetSpecularColor(oColor[0], oColor[1], oColor[2]);
  return oColor;
}

// ============================================================================
// ============================================================================
bool VistaLightNode::SetSpecularColor(float r, float g, float b) {
  return m_pBridge->SetLightSpecularColor(m_nLightType, r, g, b, m_pData);
}

bool VistaLightNode::SetSpecularColor(const VistaColor& oColor) {
  return SetSpecularColor(oColor[0], oColor[1], oColor[2]);
}

VistaVector3D VistaLightNode::GetAttenuation() const {
  VistaVector3D pAtt;
  m_pBridge->GetLightAttenuation(m_nLightType, pAtt, m_pData);
  return pAtt;
}
// ============================================================================
// ============================================================================
bool VistaLightNode::SetAttenuation(const VistaVector3D& pAtt) {
  return m_pBridge->SetLightAttenuation(m_nLightType, pAtt, m_pData);
}

// ============================================================================
// ============================================================================
bool VistaLightNode::GetIsEnabled() const {
  return m_pBridge->GetLightIsEnabled(m_pData);
}

// ============================================================================
// ============================================================================
void VistaLightNode::SetIsEnabled(bool bEnabled) {
  m_pBridge->SetLightIsEnabled(bEnabled, m_pData);
}

// ============================================================================
// DEPRECATED!
// ============================================================================
bool VistaLightNode::Enable() {
  SetIsEnabled(true);
  return true;
}

// ============================================================================
// DEPRECATED!
// ============================================================================
bool VistaLightNode::Disable() {
  SetIsEnabled(false);
  return true;
}

// ============================================================================
// DEPRECATED!
// ============================================================================
bool VistaLightNode::IsEnabled() const {
  return GetIsEnabled();
}
// ============================================================================
// ============================================================================
bool VistaLightNode::CanHaveChildren() const {
  return false;
}

// ============================================================================
// ============================================================================
VISTA_LIGHTTYPE VistaLightNode::GetLightType() const {
  return m_nLightType;
}

// ============================================================================
// ============================================================================

VistaAmbientLight::VistaAmbientLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge,
    IVistaNodeData* pData, const std::string& strName)
    : VistaLightNode(pParent, pBridge, pData, strName) {
  // set default light color
  m_nType      = VISTA_AMBIENTLIGHTNODE;
  m_nLightType = VISTA_AMBIENT_LIGHT;
}

VistaAmbientLight::~VistaAmbientLight() {
}

// ============================================================================
// ============================================================================

VistaDirectionalLight::VistaDirectionalLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge,
    IVistaNodeData* pData, const std::string& strName)
    : VistaLightNode(pParent, pBridge, pData, strName) {
  m_nType      = VISTA_DIRECTIONALLIGHTNODE;
  m_nLightType = VISTA_DIRECTIONAL_LIGHT;
}

VistaDirectionalLight::~VistaDirectionalLight() {
}

// ============================================================================
// ============================================================================
VistaVector3D VistaDirectionalLight::GetDirection() const {
  VistaVector3D v3Dir;
  m_pBridge->GetLightDirection(m_nLightType, v3Dir, m_pData);
  return v3Dir;
}
// ============================================================================
// ============================================================================
bool VistaDirectionalLight::SetDirection(const VistaVector3D& pDir) {
  return m_pBridge->SetLightDirection(m_nLightType, pDir, m_pData);
}
// ============================================================================
// ============================================================================
// ============================================================================

VistaPointLight::VistaPointLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge,
    IVistaNodeData* pData, const std::string& strName)
    : VistaLightNode(pParent, pBridge, pData, strName) {
  m_nType      = VISTA_POINTLIGHTNODE;
  m_nLightType = VISTA_POINT_LIGHT;
}

VistaPointLight::~VistaPointLight() {
}
// ============================================================================
// ============================================================================
VistaVector3D VistaPointLight::GetPosition() const {
  VistaVector3D pPos;
  m_pBridge->GetLightPosition(m_nLightType, pPos, m_pData);
  return pPos;
}
// ============================================================================
// ============================================================================
bool VistaPointLight::SetPosition(const VistaVector3D& pPos) {
  return m_pBridge->SetLightPosition(m_nLightType, pPos, m_pData);
}
// ============================================================================
// ============================================================================

// ============================================================================
// ============================================================================
const float VistaSpotLight::SPOT_TORCH       = 22.5f;
const float VistaSpotLight::SPOT_DESKLAMP    = 90.0f;
const float VistaSpotLight::SPOT_SHADED_BULB = 180.0f;

VistaSpotLight::VistaSpotLight(VistaGroupNode* pParent, IVistaNodeBridge* pBridge,
    IVistaNodeData* pData, const std::string& strName)
    : VistaLightNode(pParent, pBridge, pData, strName) {
  m_nType      = VISTA_SPOTLIGHTNODE;
  m_nLightType = VISTA_SPOT_LIGHT;
}

VistaSpotLight::~VistaSpotLight() {
}
// ============================================================================
// ============================================================================
VistaVector3D VistaSpotLight::GetPosition() const {
  VistaVector3D pPos;
  m_pBridge->GetLightPosition(m_nLightType, pPos, m_pData);
  return pPos;
}
// ============================================================================
// ============================================================================
bool VistaSpotLight::SetPosition(const VistaVector3D& pPos) {
  return m_pBridge->SetLightPosition(m_nLightType, pPos, m_pData);
}
// ============================================================================
// ============================================================================
// ============================================================================
VistaVector3D VistaSpotLight::GetDirection() const {
  VistaVector3D pDir;
  m_pBridge->GetLightDirection(m_nLightType, pDir, m_pData);
  return pDir;
}
// ============================================================================
// ============================================================================
bool VistaSpotLight::SetDirection(const VistaVector3D& pDir) {
  return m_pBridge->SetLightDirection(m_nLightType, pDir, m_pData);
}

bool VistaSpotLight::SetSpotDistribution(eLightDistribution eLd) {
  return m_pBridge->SetSpotDistribution(int(eLd), m_pData);
}

bool VistaSpotLight::SetSpotDistribution(int iDist) {
  return m_pBridge->SetSpotDistribution(iDist, m_pData);
}

int VistaSpotLight::GetSpotDistribution() const {
  return m_pBridge->GetSpotDistribution(m_pData);
}

bool VistaSpotLight::SetSpotCharacter(float eC) {
  return m_pBridge->SetSpotCharacter(eC, m_pData);
}

float VistaSpotLight::GetSpotCharacter() const {
  return m_pBridge->GetSpotCharacter(m_pData);
}

// ============================================================================
// ============================================================================
