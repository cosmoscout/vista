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

#include "VistaGraphicsManager.h"

#include <VistaKernel/VistaFrameLoop.h>

#include <VistaKernel/GraphicsManager/VistaLightNode.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaVectorMath.h>

#include <map>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaGraphicsManager::VistaGraphicsManager(VistaEventManager*, VistaFrameLoop* pFrameLoop)
    : VistaEventHandler()
    , m_pSceneGraph(NULL)
    , m_pModelRoot(NULL)
    , m_pGraphicsBridge(NULL)
    , m_pNodeBridge(NULL)
    , m_pLoop(pFrameLoop) {
}

VistaGraphicsManager::~VistaGraphicsManager() {
  delete m_pSceneGraph;
  delete m_pGraphicsBridge;
  delete m_pNodeBridge;
}
// ============================================================================

bool VistaGraphicsManager::Init(
    IVistaNodeBridge* pNodeBridge, IVistaGraphicsBridge* pGraphicsBridge) {
  m_pSceneGraph     = new VistaSceneGraph;
  m_pGraphicsBridge = pGraphicsBridge;
  m_pNodeBridge     = pNodeBridge;

  // init sub structures
  if (m_pNodeBridge->Init(m_pSceneGraph) == false)
    return false;

  if (m_pSceneGraph->Init(m_pNodeBridge, pGraphicsBridge) == false)
    return false;

  m_pModelRoot = m_pSceneGraph->GetRoot();

  return true;
}

bool VistaGraphicsManager::SetupScene(
    const std::string& sIniSection, const VistaPropertyList& oConfig) {
  vstr::outi() << "Configuring scene graphics from section [" << sIniSection << "]" << std::endl;
  vstr::IndentObject oIndent;

  if (oConfig.HasSubList(sIniSection) == false) {
    vstr::warnp() << "Section [" << sIniSection << "] does not exist!" << std::endl;
    return false;
  }
  const VistaPropertyList& oProps = oConfig.GetSubListConstRef(sIniSection);

  VistaColor colBackground =
      oProps.GetValueOrDefault<VistaColor>("BACKGROUNDCOLOR", VistaColor::VISTA_BLUE);
  SetBackgroundColor(colBackground);

  // configure default lights
  std::list<std::string> liLights;

  oProps.GetValue("LIGHTS", liLights);
  for (std::list<std::string>::const_iterator itLight = liLights.begin(); itLight != liLights.end();
       itLight++) {
    vstr::outi() << "Creating light [" << (*itLight) << "]" << std::endl;
    if (oConfig.HasSubList((*itLight)) == false) {
      vstr::warnp() << "Requested light [" << (*itLight) << "] but no such sectione extists"
                    << std::endl;
      continue;
    }
    const VistaPropertyList& oLightProps = oConfig.GetSubListConstRef((*itLight));

    m_pSceneGraph->NewLightFromProplist(oLightProps, m_pSceneGraph->GetRoot());
  }
  return true;
}

bool VistaGraphicsManager::RegisterEventTypes(VistaEventManager* pEventMgr) {
  return true;
}

void VistaGraphicsManager::CreateDefaultLights() {
  VistaVector3D v3DTmp;
  /** @todo memory leaks! */

  VistaDirectionalLight* m_pDirectionalLight0 = m_pSceneGraph->NewDirectionalLight(m_pModelRoot);
  m_pDirectionalLight0->SetAmbientColor(0.1f, 0.1f, 0.1f);
  m_pDirectionalLight0->SetDiffuseColor(0.4f, 0.4f, 0.4f);
  m_pDirectionalLight0->SetSpecularColor(0.2f, 0.2f, 0.2f);
  m_pDirectionalLight0->SetName("DefaultDirectionalLight0");
  v3DTmp[0]               = -1.0f;
  v3DTmp[1]               = 1.0f;
  v3DTmp[2]               = 0.0f;
  const VistaVector3D& d0 = v3DTmp;
  m_pDirectionalLight0->SetDirection(d0);

  VistaDirectionalLight* m_pDirectionalLight1 = m_pSceneGraph->NewDirectionalLight(m_pModelRoot);
  m_pDirectionalLight1->SetAmbientColor(0.1f, 0.1f, 0.1f);
  m_pDirectionalLight1->SetDiffuseColor(0.4f, 0.4f, 0.4f);
  m_pDirectionalLight1->SetSpecularColor(0.2f, 0.2f, 0.2f);
  m_pDirectionalLight1->SetName("DefaultDirectionalLight1");
  v3DTmp[0]               = 0.0f;
  v3DTmp[1]               = 0.0f;
  v3DTmp[2]               = 1.0f;
  const VistaVector3D& d1 = v3DTmp;
  m_pDirectionalLight1->SetDirection(d1);

  VistaDirectionalLight* m_pDirectionalLight2 = m_pSceneGraph->NewDirectionalLight(m_pModelRoot);
  m_pDirectionalLight0->SetAmbientColor(0.1f, 0.1f, 0.1f);
  m_pDirectionalLight2->SetDiffuseColor(0.4f, 0.4f, 0.4f);
  m_pDirectionalLight2->SetSpecularColor(0.2f, 0.2f, 0.2f);
  m_pDirectionalLight2->SetName("DefaultDirectionalLight2");
  v3DTmp[0]               = 1.0f;
  v3DTmp[1]               = -1.0f;
  v3DTmp[2]               = -1.0f;
  const VistaVector3D& d2 = v3DTmp;
  m_pDirectionalLight2->SetDirection(d2);
}

// ============================================================================
// ============================================================================
VistaSceneGraph* VistaGraphicsManager::GetSceneGraph() const {
  return m_pSceneGraph;
}

void VistaGraphicsManager::HandleEvent(VistaEvent* pEvent) {
}

float VistaGraphicsManager::GetFrameRate() const {
  return m_pLoop->GetFrameRate();
}

int VistaGraphicsManager::GetFrameCount() const {
  return m_pLoop->GetFrameCount();
}

bool VistaGraphicsManager::GetFrustumCullingEnabled() const {
  return m_pGraphicsBridge->GetFrustumCullingEnabled();
}

void VistaGraphicsManager::SetFrustumCullingEnabled(bool bCullingEnabled) {
  return m_pGraphicsBridge->SetFrustumCullingEnabled(bCullingEnabled);
}

bool VistaGraphicsManager::GetOcclusionCullingEnabled() const {
  return m_pGraphicsBridge->GetOcclusionCullingEnabled();
}

void VistaGraphicsManager::SetOcclusionCullingEnabled(bool bOclCullingEnabled) {
  return m_pGraphicsBridge->SetOcclusionCullingEnabled(bOclCullingEnabled);
}

bool VistaGraphicsManager::GetBBoxDrawingEnabled() const {
  return m_pGraphicsBridge->GetBBoxDrawingEnabled();
}

void VistaGraphicsManager::SetBBoxDrawingEnabled(bool bState) {
  return m_pGraphicsBridge->SetBBoxDrawingEnabled(bState);
}

VistaColor VistaGraphicsManager::GetBackgroundColor() const {
  return m_pGraphicsBridge->GetBackgroundColor();
}

void VistaGraphicsManager::SetBackgroundColor(const VistaColor& color) {
  m_pGraphicsBridge->SetBackgroundColor(color);
}

void VistaGraphicsManager::Debug(std::ostream& oOut) const {
  oOut << vstr::indent << std::endl;
  oOut << vstr::indent << " [ViGraMan]----------------------------------------------------------"
       << std::endl;
  oOut << vstr::indent << " [ViGraMan] INFO SCREEN " << std::endl;
  oOut << vstr::indent << " [ViGraMan] VistaSceneGraph\t\t"
       << (m_pSceneGraph ? "constructed" : "construction FAILED.") << std::endl;
  vstr::IndentObject oIndent;
  if (m_pSceneGraph != NULL)
    m_pSceneGraph->Debug(oOut);
  oOut << vstr::indent << " [ViGraMan]	Application is currently running at " << GetFrameRate()
       << " fps." << std::endl;
  oOut << vstr::indent << " [ViGraMan]----------------------------------------------------------"
       << std::endl;
}

IVistaNodeBridge* VistaGraphicsManager::GetNodeBridge() const {
  return m_pNodeBridge;
}

IVistaGraphicsBridge* VistaGraphicsManager::GetGraphicsBridge() const {
  return m_pGraphicsBridge;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
std::ostream& operator<<(std::ostream& out, const VistaGraphicsManager& device) {
  device.Debug(out);
  return out;
}
