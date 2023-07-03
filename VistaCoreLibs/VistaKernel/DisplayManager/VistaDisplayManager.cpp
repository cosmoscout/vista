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

#include "VistaDisplayManager.h"

#include <VistaKernel/DisplayManager/VistaDisplay.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaDisplayEntity.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaProjection.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaObserver.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <algorithm>
#include <set>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

// forward declaration for helper method
static VistaVirtualPlatform* CreateRefFrameFromSection(
    const std::string& sSectionName, const VistaPropertyList& oConfig);
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaDisplayManager::VistaDisplayManager(IVistaDisplayBridge* pDisplayBridge)
    : VistaEventHandler()
    , m_pBridge(pDisplayBridge) {
}

VistaDisplayManager::~VistaDisplayManager() {
  ShutdownDisplaySystems();

  /** @todo  <ms> - find a better place to do this... */
  delete m_pBridge;

  for (std::vector<VistaReferenceFrame*>::iterator it = m_vecRefFrames.begin();
       it != m_vecRefFrames.end(); ++it) {
    delete *it;
  }
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaDisplayManager::DrawFrame() {
  VistaKernelProfileScope("DRAW");
  return m_pBridge->DrawFrame();
}

bool VistaDisplayManager::DisplayFrame() {
  VistaKernelProfileScope("DISPLAY");
  return m_pBridge->DisplayFrame();
}

bool VistaDisplayManager::SetDisplayUpdateCallback(IVistaExplicitCallbackInterface* pCallback) {
  return m_pBridge->SetDisplayUpdateCallback(pCallback);
}

IVistaDisplayBridge* VistaDisplayManager::GetDisplayBridge() const {
  return m_pBridge;
}
IVistaWindowingToolkit* VistaDisplayManager::GetWindowingToolkit() const {
  return m_pWindowingToolkit;
}

std::map<std::string, VistaDisplaySystem*> VistaDisplayManager::GetDisplaySystems() const {
  return m_mapDisplaySystems;
}

const std::map<std::string, VistaDisplaySystem*>&
VistaDisplayManager::GetDisplaySystemsConstRef() const {
  return m_mapDisplaySystems;
}

std::map<std::string, VistaWindow*> VistaDisplayManager::GetWindows() const {
  return m_mapWindows;
}

const std::map<std::string, VistaWindow*>& VistaDisplayManager::GetWindowsConstRef() const {
  return m_mapWindows;
}

std::map<std::string, VistaViewport*> VistaDisplayManager::GetViewports() const {
  return m_mapViewports;
}

const std::map<std::string, VistaViewport*>& VistaDisplayManager::GetViewportsConstRef() const {
  return m_mapViewports;
}

std::map<std::string, VistaProjection*> VistaDisplayManager::GetProjections() const {
  return m_mapProjections;
}

const std::map<std::string, VistaProjection*>& VistaDisplayManager::GetProjectionsConstRef() const {
  return m_mapProjections;
}

bool VistaDisplayManager::CreateDisplaySystems(
    const std::string& sSectionName, const VistaPropertyList& oConfig) {
  vstr::outi() << "Creating display systems from section [" << sSectionName << "]" << std::endl;
  vstr::IndentObject oIndent;

  // create WindowingToolkit only in case there is none
  if (oConfig.HasSubList(sSectionName) == false) {
    vstr::errp() << "Could not find section [" << sSectionName
                 << "] - DisplaySystem can not be created!" << std::endl;
    return false;
  }
  const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sSectionName);

  vstr::outi() << "Creating WindowingToolkitAbstraction" << std::endl;
  {
    vstr::IndentObject oWtaIndent;
    std::string        sWtaType;
    if (oSection.GetValue("WINDOWINGTOOLKIT", sWtaType) == false) {
      vstr::outi() << "No WindowingToolkitAbstraction specified, defaulting to [GLUT]" << std::endl;
      sWtaType = "SDL2";
    } else {
      vstr::outi() << "WindowingToolkitAbstraction specified as [" << sWtaType << "]" << std::endl;
    }
    m_pWindowingToolkit = m_pBridge->CreateWindowingToolkit(sWtaType);
    if (m_pWindowingToolkit == NULL) {
      vstr::errp() << "Windowing Toolkit could not be created!" << std::endl;
      return false;
    }
  }

  list<string> liDispSysSects;
  oSection.GetValue("DISPLAYSYSTEMS", liDispSysSects);

  if (liDispSysSects.empty()) {
    // utter a sanity warning here
    vstr::warnp() << "No displays have been specified" << std::endl;
    return true;
  }

  list<string>::iterator itDispSysSect;

  for (itDispSysSect = liDispSysSects.begin(); itDispSysSect != liDispSysSects.end();
       ++itDispSysSect) {
    // create the display system
    CreateDisplaySystem((*itDispSysSect), oConfig);
  }

  return true;
}

bool VistaDisplayManager::ShutdownDisplaySystems() {
  vstr::outi() << "[VistaDisplayManager] - shutting down display systems..." << std::endl;

  while (!m_vecDisplaySystems.empty()) {
    VistaDisplaySystem* pDisplaySystem = m_vecDisplaySystems[0];

    while (pDisplaySystem->GetNumberOfViewports()) {
      VistaViewport* pViewport = pDisplaySystem->GetViewport(0);

      VistaProjection* pProjection = pViewport->GetProjection();

      if (!DestroyProjection(pProjection))
        return false;

      if (!DestroyViewport(pViewport))
        return false;
    }

    if (!DestroyDisplaySystem(pDisplaySystem))
      return false;
  }

  while (!m_vecDisplays.empty()) {
    VistaDisplay* pDisplay = m_vecDisplays[0];

    while (pDisplay->GetNumberOfWindows()) {
      VistaWindow* pWindow = pDisplay->GetWindow(0);

      if (!DestroyVistaWindow(pWindow))
        return false;
    }

    if (!DestroyDisplay(pDisplay))
      return false;
  }

  // there might still be some windows left...
  while (!m_mapWindows.empty()) {
    VistaWindow* pWindow = (*(m_mapWindows.begin())).second;

    if (!DestroyVistaWindow(pWindow))
      return false;
  }

  vstr::outi() << "[VistaDisplayManager] - display system shutdown complete..." << std::endl;

  return true;
}

bool VistaDisplayManager::Update() {
  return m_pBridge->UpdateDisplaySystems(this);
}

VistaDisplaySystem* VistaDisplayManager::GetDisplaySystemByName(const std::string& strName) const {
  map<string, VistaDisplaySystem*>::const_iterator it = m_mapDisplaySystems.find(strName);

  if (it != m_mapDisplaySystems.end())
    return (*it).second;

  return NULL;
}

VistaDisplay* VistaDisplayManager::GetDisplayByName(const std::string& strName) const {
  map<string, VistaDisplay*>::const_iterator it = m_mapDisplays.find(strName);

  if (it != m_mapDisplays.end())
    return (*it).second;

  return NULL;
}

VistaWindow* VistaDisplayManager::GetWindowByName(const std::string& strName) const {
  map<string, VistaWindow*>::const_iterator it = m_mapWindows.find(strName);

  if (it != m_mapWindows.end())
    return (*it).second;

  return NULL;
}

VistaViewport* VistaDisplayManager::GetViewportByName(const std::string& strName) const {
  map<string, VistaViewport*>::const_iterator it = m_mapViewports.find(strName);

  if (it != m_mapViewports.end())
    return (*it).second;

  return NULL;
}

VistaProjection* VistaDisplayManager::GetProjectionByName(const std::string& strName) const {
  map<string, VistaProjection*>::const_iterator it = m_mapProjections.find(strName);

  if (it != m_mapProjections.end())
    return (*it).second;

  return NULL;
}

VistaDisplayEntity* VistaDisplayManager::GetDisplayEntityByName(const std::string& strName) const {

  VistaDisplayEntity* pDisEntity = NULL;

  pDisEntity = (VistaDisplayEntity*)GetDisplaySystemByName(strName);
  if (pDisEntity != NULL)
    return pDisEntity;

  pDisEntity = (VistaDisplayEntity*)GetViewportByName(strName);
  if (pDisEntity != NULL)
    return pDisEntity;

  pDisEntity = (VistaDisplayEntity*)GetDisplayByName(strName);
  if (pDisEntity != NULL)
    return pDisEntity;

  pDisEntity = (VistaDisplayEntity*)GetWindowByName(strName);
  if (pDisEntity != NULL)
    return pDisEntity;

  pDisEntity = (VistaDisplayEntity*)GetProjectionByName(strName);
  if (pDisEntity != NULL)
    return pDisEntity;

  return NULL;
}

int VistaDisplayManager::GetNumberOfDisplaySystems() const {
  return (int)m_vecDisplaySystems.size();
}

VistaDisplaySystem* VistaDisplayManager::GetDisplaySystem(int iIndex) const {
  if (0 <= iIndex && iIndex < int(m_vecDisplaySystems.size()))
    return m_vecDisplaySystems[iIndex];
  return NULL;
}

int VistaDisplayManager::GetNumberOfDisplays() const {
  return (int)m_vecDisplays.size();
}

VistaDisplay* VistaDisplayManager::GetDisplay(int iIndex) const {
  if (0 <= iIndex && iIndex < int(m_vecDisplays.size()))
    return m_vecDisplays[iIndex];
  return NULL;
}

VistaDisplaySystem* VistaDisplayManager::CreateDisplaySystem(
    const std::string& sSectionName, const VistaPropertyList& oConfig) {
  if (!m_pBridge)
    return NULL;

  vstr::outi() << "Creating display system from section [" << sSectionName << "]" << std::endl;
  vstr::IndentObject oDisplaySystemIndent;

  if (oConfig.HasSubList(sSectionName) == false) {
    vstr::errp() << "Unable to create display system  [" << sSectionName
                 << "] - no such section found" << std::endl;
    return NULL;
  }
  VistaPropertyList oDisplaySection = oConfig.GetSubListConstRef(sSectionName);

  VistaVirtualPlatform* pReferenceFrame = NULL;

  string sReferenceFrame;
  if (oDisplaySection.GetValue("REFERENCE_FRAME", sReferenceFrame)) {
    pReferenceFrame = CreateRefFrameFromSection(sReferenceFrame, oConfig);
    // remove to avoid warnign by SetPorpertiesByList
    oDisplaySection.RemoveProperty("REFERENCE_FRAME");
  }
  if (pReferenceFrame == NULL)
    pReferenceFrame = new VistaVirtualPlatform;

  list<string> liStrings;
  if (oDisplaySection.GetValue("VIEWPORTS", liStrings) == false) {
    vstr::warnp() << "No VIEWPORTS specified!" << std::endl;
  }
  // remove to avoid warnign by SetPorpertiesByList
  oDisplaySection.RemoveProperty("VIEWPORTS");

  VistaDisplaySystem* pDisplaySystem =
      m_pBridge->CreateDisplaySystem(pReferenceFrame, this, oDisplaySection);

  if (pDisplaySystem == NULL) {
    delete pReferenceFrame;
    return NULL;
  }

  // check, whether the display system name is unique...
  string strDisplaySystemName = pDisplaySystem->GetNameForNameable();
  if (strDisplaySystemName.empty()) {
    strDisplaySystemName = sSectionName;
    pDisplaySystem->SetNameForNameable(sSectionName);
  }

  if (m_mapDisplaySystems.find(strDisplaySystemName) != m_mapDisplaySystems.end()) {
    vstr::errp() << "Unable to create display system [" << strDisplaySystemName
                 << "] - name not unique" << std::endl;

    m_pBridge->DestroyDisplaySystem(pDisplaySystem);
    delete pReferenceFrame;
    return NULL;
  }
  m_mapDisplaySystems[strDisplaySystemName] = pDisplaySystem;

  // memorize display system
  m_vecDisplaySystems.push_back(pDisplaySystem);
  m_vecRefFrames.push_back(pReferenceFrame);

  // create its viewports
  for (list<string>::iterator itString = liStrings.begin(); itString != liStrings.end();
       ++itString) {
    CreateViewport((*itString), oConfig, pDisplaySystem);
  }

  return pDisplaySystem;
}

bool VistaDisplayManager::DestroyDisplaySystem(VistaDisplaySystem* pDisplaySystem) {
  if (!m_pBridge)
    return false;

  if (!pDisplaySystem)
    return false;

  // find display system in display system vector
  std::vector<VistaDisplaySystem*>::iterator itDS;
  for (itDS = m_vecDisplaySystems.begin(); itDS != m_vecDisplaySystems.end(); ++itDS) {
    if (pDisplaySystem == *itDS) {
      m_vecDisplaySystems.erase(itDS);
      break;
    }
  }

  map<string, VistaDisplaySystem*>::iterator itMap =
      m_mapDisplaySystems.find(pDisplaySystem->GetNameForNameable());
  if (itMap != m_mapDisplaySystems.end())
    m_mapDisplaySystems.erase(itMap);

  // and have it destroyed
  return m_pBridge->DestroyDisplaySystem(pDisplaySystem);
}

VistaDisplay* VistaDisplayManager::CreateDisplay(const VistaPropertyList& refProps) {
  if (!m_pBridge)
    return NULL;

  VistaDisplay* pDisplay = m_pBridge->CreateDisplay(this, refProps);

  if (pDisplay) {
    // check, whether the display name is unique...
    string strDisplayName = pDisplay->GetNameForNameable();

    if (strDisplayName.empty()) {
      strDisplayName = string("DISPLAY_") + pDisplay->GetNameableIdAsString();
      pDisplay->SetNameForNameable(strDisplayName);

      vstr::warnp() << "[VistaDisplayManager] - trying to create unnamed display - dafaulting to "
                    << strDisplayName << std::endl;
    }

    if (m_mapDisplays.find(strDisplayName) != m_mapDisplays.end()) {
      vstr::warnp() << "[VistaDisplayManager] - Unable to create display [" << strDisplayName
                    << "] - name is not unique!" << std::endl;

      m_pBridge->DestroyDisplay(pDisplay);
      return NULL;
    }
    m_mapDisplays[strDisplayName] = pDisplay;

    // memorize display
    m_vecDisplays.push_back(pDisplay);
  }

  return pDisplay;
}

bool VistaDisplayManager::DestroyDisplay(VistaDisplay* pDisplay) {
  if (!m_pBridge)
    return false;

  if (!pDisplay)
    return true;

  // check, whether we don't have any dependent objects left...
  if (pDisplay->GetNumberOfWindows() > 0) {
    vstr::warnp() << "[VistaDisplayManager] - unable to destroy display ["
                  << pDisplay->GetNameForNameable() << "] - still has windows!" << std::endl;
    return false;
  }

  // find display in display vector
  std::vector<VistaDisplay*>::iterator itD;
  for (itD = m_vecDisplays.begin(); itD != m_vecDisplays.end(); ++itD) {
    if (pDisplay == *itD) {
      m_vecDisplays.erase(itD);
      break;
    }
  }

  map<string, VistaDisplay*>::iterator itMap = m_mapDisplays.find(pDisplay->GetNameForNameable());
  if (itMap != m_mapDisplays.end())
    m_mapDisplays.erase(itMap);

  // and have it destroyed
  return m_pBridge->DestroyDisplay(pDisplay);
}

VistaWindow* VistaDisplayManager::CreateVistaWindow(
    const std::string& sSectionName, const VistaPropertyList& oConfig) {
  if (!m_pBridge)
    return NULL;

  if (oConfig.HasSubList(sSectionName) == false) {
    vstr::errp() << "Cannot create window - section [" << sSectionName << "] does not exist"
                 << std::endl;
    return NULL;
  }
  const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sSectionName);

  string sWindowName;
  if (oSection.GetValue("NAME", sWindowName) == false)
    sWindowName = sSectionName;

  VistaWindow* pWindow = GetWindowByName(sWindowName);
  if (pWindow != NULL) {
    vstr::outi() << "Using existing window [" << sWindowName << "]" << std::endl;
    return pWindow;
  }

  vstr::outi() << "Creating new window from section [" << sWindowName << "]" << std::endl;
  vstr::IndentObject oWindowIndent;

  // create display or use existing one
  VistaDisplay* pDisplay = NULL;
  string        sDisplaySection;
  if (oSection.GetValue("DISPLAY", sDisplaySection)) {
    vstr::outi() << "Using display from section [" << sDisplaySection << "]" << std::endl;
    vstr::IndentObject oDisplayIndent;

    if (oConfig.HasSubList(sDisplaySection) == false) {
      vstr::errp() << "Cannot create display - section [" << sDisplaySection << "] does not exist"
                   << std::endl;
    } else {
      const VistaPropertyList& oDisplaySection = oConfig.GetSubListConstRef(sDisplaySection);

      string strDisplayName;
      oDisplaySection.GetValue("NAME", strDisplayName);
      pDisplay = GetDisplayByName(strDisplayName);
      if (!pDisplay) {
        vstr::outi() << "Creating display [" << strDisplayName << "]" << std::endl;
        pDisplay = CreateDisplay(oDisplaySection);
      } else {
        vstr::outi() << "Using existing display [" << strDisplayName << "]" << std::endl;
      }
    }
  }

  pWindow = m_pBridge->CreateVistaWindow(pDisplay, oSection);

  if (pWindow == NULL) {
    vstr::errp() << "[VistaDisplayManager]: Could not create Window" << std::endl;
    return NULL;
  }

  // check, whether the window name is unique...
  string sCurrentWindowName = pWindow->GetNameForNameable();
  if (sCurrentWindowName.empty()) {
    pWindow->SetNameForNameable(sWindowName);
    sCurrentWindowName = sWindowName;
  }

  if (m_mapWindows.find(sCurrentWindowName) != m_mapWindows.end()) {
    vstr::warnp() << "[VistaDisplayManager]: unable to create window - "
                  << "Name [" << sCurrentWindowName << "] not unique" << std::endl;

    m_pBridge->DestroyVistaWindow(pWindow);
    return NULL;
  }
  m_mapWindows[sCurrentWindowName] = pWindow;

  if (pDisplay) {
    pDisplay->GetWindows().push_back(pWindow);
  }

  return pWindow;
}

bool VistaDisplayManager::DestroyVistaWindow(VistaWindow* pWindow) {
  if (!m_pBridge)
    return false;

  if (!pWindow)
    return true; // well, there's no window there, right?!

  if (pWindow->GetNumberOfViewports() > 0) {
    vstr::warnp() << "[VistaDisplayManager] - Unable to destroy window ["
                  << pWindow->GetNameForNameable() << "] - it still has viewports" << std::endl;
    return false;
  }

  // remove references to window
  VistaDisplay* pParent = pWindow->GetDisplay();
  if (pParent) {
    vector<VistaWindow*>&          vecWindows = pParent->GetWindows();
    vector<VistaWindow*>::iterator itW;
    for (itW = vecWindows.begin(); itW != vecWindows.end(); ++itW) {
      if (pWindow == *itW) {
        vecWindows.erase(itW);
        break;
      }
    }
  }

  map<string, VistaWindow*>::iterator itMap = m_mapWindows.find(pWindow->GetNameForNameable());
  if (itMap != m_mapWindows.end())
    m_mapWindows.erase(itMap);

  vstr::outi() << "Destroying window [" << pWindow->GetNameForNameable() << "]" << std::endl;
  return m_pBridge->DestroyVistaWindow(pWindow);
}

VistaViewport* VistaDisplayManager::CreateViewport(const std::string& sSectionName,
    const VistaPropertyList& oConfig, VistaDisplaySystem* pDisplaySystem) {
  if (m_pBridge == NULL)
    return NULL;

  vstr::outi() << "Creating viewport from section [" << sSectionName << "]" << std::endl;
  vstr::IndentObject oViewportIndent;

  if (oConfig.HasSubList(sSectionName) == false) {
    vstr::errp() << "Unable to find viewport section - skipping viewport" << std::endl;
    return NULL;
  }
  const VistaPropertyList& oViewportProps = oConfig.GetSubListConstRef(sSectionName);

  // now, find out, whether we already know about the window...
  string sWindowSection;
  if (oViewportProps.GetValue("WINDOW", sWindowSection) == false) {
    vstr::errp() << "No WINDOW specified" << std::endl;
    return NULL;
  }
  VistaWindow* pWindow = CreateVistaWindow(sWindowSection, oConfig);
  if (pWindow == NULL) {
    vstr::errp() << "Could not create Window - skipping Display" << std::endl;
    return NULL;
  }

  VistaViewport* pViewport = m_pBridge->CreateViewport(pDisplaySystem, pWindow, oViewportProps);
  if (pViewport == NULL) {
    vstr::errp() << "Viewport Creating failed!";
    delete pWindow;
    return NULL;
  }

  // check, whether the viewport name is unique...
  string sViewportName = pViewport->GetNameForNameable();
  if (sViewportName.empty()) {
    sViewportName = sSectionName;
    pViewport->SetNameForNameable(sViewportName);
  }
  if (m_mapViewports.find(sViewportName) != m_mapViewports.end()) {
    vstr::errp() << "Unable to create viewport [" << sViewportName << "] - Name not unique"
                 << std::endl;

    m_pBridge->DestroyViewport(pViewport);
    delete pWindow;
    return NULL;
  }

  // finally, create the corresponding projection...
  // ... but only if non exists yet (Oculus viewports create their own projection for now @IMGTODO)
  if (pViewport->m_pProjection == NULL) {
    string sProjSection;
    if (oViewportProps.GetValue("PROJECTION", sProjSection) == false) {
      vstr::errp() << "No PROJECTION specified" << std::endl;
      return NULL;
    }
    VistaProjection* pProjection = CreateProjection(pViewport, sProjSection, oConfig);
    if (!pProjection) {
      vstr::errp() << "Unable to create projection" << std::endl;
    }
  } else if (oViewportProps.HasProperty("PROJECTION")) {
    string sProjSection = oViewportProps.GetValue<std::string>("PROJECTION");
    if (oConfig.HasSubList(sProjSection) == false) {
      vstr::warnp() << "Projection cannot be created - section [" << sProjSection
                    << "] does not exist" << std::endl;
      return NULL;
    }
    const VistaPropertyList& oProjSection = oConfig.GetSubListConstRef(sProjSection);

    VistaProjection* pProjection = pViewport->GetProjection();
    pProjection->GetProjectionProperties()->SetPropertiesByList(oProjSection);
  }

  m_mapViewports[sViewportName] = pViewport;

  if (pDisplaySystem) {
    vector<VistaViewport*>& refViewports = pDisplaySystem->GetViewports();
    refViewports.push_back(pViewport);
  }

  if (pWindow) {
    vector<VistaViewport*>& refViewports = pWindow->GetViewports();
    refViewports.push_back(pViewport);
  }

  return pViewport;
}

bool VistaDisplayManager::DestroyViewport(VistaViewport* pViewport) {
  if (!m_pBridge)
    return false;

  if (!pViewport)
    return true;

  if (pViewport->GetProjection()) {
    vstr::warnp() << "[VistaDisplayManager] - unable to destroy viewport ["
                  << pViewport->GetNameForNameable() << "] - it still has a projection"
                  << std::endl;
    return false;
  }

  VistaDisplaySystem* pDS = pViewport->GetDisplaySystem();
  if (pDS) {
    vector<VistaViewport*>&          refViewports = pDS->GetViewports();
    vector<VistaViewport*>::iterator itVp;
    for (itVp = refViewports.begin(); itVp != refViewports.end(); ++itVp) {
      if (pViewport == *itVp) {
        refViewports.erase(itVp);
        break;
      }
    }
  }

  VistaWindow* pW = pViewport->GetWindow();
  if (pW) {
    vector<VistaViewport*>&          refViewports = pW->GetViewports();
    vector<VistaViewport*>::iterator itVp;
    for (itVp = refViewports.begin(); itVp != refViewports.end(); ++itVp) {
      if (pViewport == *itVp) {
        refViewports.erase(itVp);
        break;
      }
    }
  }

  map<string, VistaViewport*>::iterator itMap =
      m_mapViewports.find(pViewport->GetNameForNameable());
  if (itMap != m_mapViewports.end())
    m_mapViewports.erase(itMap);

  return m_pBridge->DestroyViewport(pViewport);
}

VistaProjection* VistaDisplayManager::CreateProjection(
    VistaViewport* pViewport, const std::string& sSectionName, const VistaPropertyList& oConfig) {
  if (!m_pBridge)
    return NULL;

  vstr::outi() << "Creating Projection from section [" << sSectionName << "]" << std::endl;

  if (oConfig.HasSubList(sSectionName) == false) {
    vstr::warnp() << "Projection cannot be created - section [" << sSectionName
                  << "] does not exist" << std::endl;
    return NULL;
  }
  const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sSectionName);

  VistaProjection* pProjection = m_pBridge->CreateProjection(pViewport, oSection);
  if (pProjection == NULL)
    return NULL;

  // check, whether the projection name is unique...
  string sProjectionName = pProjection->GetNameForNameable();
  if (sProjectionName.empty()) {
    sProjectionName = sSectionName;
    pProjection->SetNameForNameable(sProjectionName);
  }

  if (m_mapProjections.find(sProjectionName) != m_mapProjections.end()) {
    vstr::warnp() << "[VistaDisplayManager] - Unable to create projection [" << sProjectionName
                  << "] - name is not unique" << std::endl;

    m_pBridge->DestroyProjection(pProjection);
    return NULL;
  }
  m_mapProjections[sProjectionName] = pProjection;

  if (pViewport) {
    pViewport->SetProjection(pProjection);
  }

  return pProjection;
}

bool VistaDisplayManager::DestroyProjection(VistaProjection* pProjection) {
  if (!m_pBridge)
    return false;

  if (!pProjection)
    return true;

  VistaViewport* pVp = pProjection->GetViewport();
  if (pVp) {
    pVp->SetProjection(NULL);
  }

  map<string, VistaProjection*>::iterator itMap =
      m_mapProjections.find(pProjection->GetNameForNameable());
  if (itMap != m_mapProjections.end())
    m_mapProjections.erase(itMap);

  return m_pBridge->DestroyProjection(pProjection);
}

bool VistaDisplayManager::RenameDisplaySystem(
    const std::string& strOldName, const std::string& strNewName) {
  if (m_mapDisplaySystems.find(strNewName) != m_mapDisplaySystems.end())
    return false;

  map<string, VistaDisplaySystem*>::iterator it = m_mapDisplaySystems.find(strOldName);

  if (it != m_mapDisplaySystems.end()) {
    VistaDisplaySystem* pTemp = (*it).second;
    m_mapDisplaySystems.erase(it);

    m_mapDisplaySystems[strNewName] = pTemp;
  }

  return true;
}

bool VistaDisplayManager::RenameViewport(
    const std::string& strOldName, const std::string& strNewName) {
  if (m_mapViewports.find(strNewName) != m_mapViewports.end())
    return false;

  map<string, VistaViewport*>::iterator it = m_mapViewports.find(strOldName);

  if (it != m_mapViewports.end()) {
    VistaViewport* pTemp = (*it).second;
    m_mapViewports.erase(it);

    m_mapViewports[strNewName] = pTemp;
  }

  return true;
}

bool VistaDisplayManager::RenameProjection(
    const std::string& strOldName, const std::string& strNewName) {
  if (m_mapProjections.find(strNewName) != m_mapProjections.end())
    return false;

  map<string, VistaProjection*>::iterator it = m_mapProjections.find(strOldName);

  if (it != m_mapProjections.end()) {
    VistaProjection* pTemp = (*it).second;
    m_mapProjections.erase(it);

    m_mapProjections[strNewName] = pTemp;
  }

  return true;
}

bool VistaDisplayManager::RenameWindow(
    const std::string& strOldName, const std::string& strNewName) {
  if (m_mapWindows.find(strNewName) != m_mapWindows.end())
    return false;

  map<string, VistaWindow*>::iterator it = m_mapWindows.find(strOldName);

  if (it != m_mapWindows.end()) {
    VistaWindow* pTemp = (*it).second;
    m_mapWindows.erase(it);

    m_mapWindows[strNewName] = pTemp;
  }

  return true;
}

bool VistaDisplayManager::RenameDisplay(
    const std::string& strOldName, const std::string& strNewName) {
  if (m_mapDisplays.find(strNewName) != m_mapDisplays.end())
    return false;

  map<string, VistaDisplay*>::iterator it = m_mapDisplays.find(strOldName);

  if (it != m_mapDisplays.end()) {
    VistaDisplay* pTemp = (*it).second;
    m_mapDisplays.erase(it);

    m_mapDisplays[strNewName] = pTemp;
  }

  return true;
}

bool VistaDisplayManager::MakeScreenshot(VistaWindow* pWindow, const std::string& sFilename) const {
  return m_pBridge->MakeScreenshot(*pWindow, sFilename);
}

bool VistaDisplayManager::MakeScreenshot(
    const std::string& sWindowName, const std::string& sFilename) const {
  VistaWindow* pWin = GetWindowByName(sWindowName);
  if (pWin)
    return MakeScreenshot(pWin, sFilename);
  else
    return false;
}

bool VistaDisplayManager::AddSceneOverlay(
    IVistaSceneOverlay* pDraw, const std::string& sViewportName) {
  if (sViewportName.empty()) {
    if (m_mapViewports.size() == 1) {
      return m_pBridge->AddSceneOverlay(pDraw, (*m_mapViewports.begin()).second);
    } else {
      vstr::warnp() << "VistaDisplayManager::AddSceneOverlay() -- "
                    << "Trying to add Overlay to default viewport, but there is not "
                    << "exactly one viewport, but " << m_mapViewports.size() << std::endl;
    }
    return false;
  } else
    return m_pBridge->AddSceneOverlay(pDraw, GetViewportByName(sViewportName));
}

bool VistaDisplayManager::AddSceneOverlay(IVistaSceneOverlay* pDraw, VistaViewport* pViewport) {
  if (pViewport == NULL)
    return false;
  return m_pBridge->AddSceneOverlay(pDraw, pViewport);
}

bool VistaDisplayManager::RemSceneOverlay(
    IVistaSceneOverlay* pDraw, const std::string& sViewportName) {
  if (sViewportName.empty()) {
    if (m_mapViewports.size() == 1) {
      return m_pBridge->RemSceneOverlay(pDraw, (*m_mapViewports.begin()).second);
    } else {
      vstr::warnp() << "VistaDisplayManager::RemSceneOverlay() -- "
                    << "Trying to remove Overlay from default viewport, but there is not "
                    << "exactly one viewport, but " << m_mapViewports.size() << std::endl;
    }
    return false;
  } else
    return m_pBridge->RemSceneOverlay(pDraw, GetViewportByName(sViewportName));
}

bool VistaDisplayManager::RemSceneOverlay(IVistaSceneOverlay* pDraw, VistaViewport* pViewport) {
  if (pViewport == NULL)
    return false;
  return m_pBridge->RemSceneOverlay(pDraw, pViewport);
}

Vista2DText* VistaDisplayManager::New2DText(const std::string& sViewportName) {
  if (sViewportName.empty()) {
    const std::map<std::string, VistaViewport*>& mp = GetViewportsConstRef();
    if (mp.size() == 1) {
      return m_pBridge->New2DText((*mp.begin()).first);
    }
    vstr::warnp() << "[ViDiMa]: Warning: Cannot add 2D Text without given viewport name,"
                  << "when more than one viewport is registered." << std::endl;
    return NULL;
  } else
    return m_pBridge->New2DText(sViewportName);
}

Vista2DBitmap* VistaDisplayManager::New2DBitmap(const std::string& sViewportName) {
  if (sViewportName.empty()) {
    const std::map<std::string, VistaViewport*>& mp = GetViewportsConstRef();
    if (mp.size() == 1) {
      return m_pBridge->New2DBitmap((*mp.begin()).first);
    }
    return NULL;
  } else
    return m_pBridge->New2DBitmap(sViewportName);
}

Vista2DLine* VistaDisplayManager::New2DLine(const std::string& sViewportName) {
  if (sViewportName.empty()) {
    const std::map<std::string, VistaViewport*>& mp = GetViewportsConstRef();
    if (mp.size() == 1) {
      return m_pBridge->New2DLine((*mp.begin()).first);
    }
    return NULL;
  } else
    return m_pBridge->New2DLine(sViewportName);
}

Vista2DRectangle* VistaDisplayManager::New2DRectangle(const std::string& sViewportName) {
  if (sViewportName.empty()) {
    const std::map<std::string, VistaViewport*>& mp = GetViewportsConstRef();
    if (mp.size() == 1) {
      return m_pBridge->New2DRectangle((*mp.begin()).first);
    }
    return NULL;
  } else
    return m_pBridge->New2DRectangle(sViewportName);
}

void VistaDisplayManager::Debug(std::ostream& out) const {
  unsigned int       iDispSysIndex;
  set<VistaDisplay*> setDisplays;
  set<VistaWindow*>  setWindows;

  std::cout << "----------------------------------" << std::endl;
  std::cout << "-- [VistaDisplayManager] Status --" << std::endl;
  std::cout << "----------------------------------" << std::endl;
  vstr::IndentObject oIndent;

  for (iDispSysIndex = 0; iDispSysIndex < m_vecDisplaySystems.size(); ++iDispSysIndex) {
    VistaDisplaySystem* pDispSys = m_vecDisplaySystems[iDispSysIndex];
    out << (*pDispSys) << std::endl;

    unsigned int iViewportIndex;
    for (iViewportIndex = 0; iViewportIndex < pDispSys->GetNumberOfViewports(); ++iViewportIndex) {
      VistaViewport* pViewport = pDispSys->GetViewport(iViewportIndex);
      if (pViewport) {
        out << (*pViewport) << std::endl;

        VistaProjection* pProjection = pViewport->GetProjection();
        if (pProjection)
          out << (*pProjection) << std::endl;

        if (pViewport->GetWindow()) {
          setWindows.insert(pViewport->GetWindow());
        }
      }
    }
  }

  set<VistaWindow*>::iterator itWin;
  for (itWin = setWindows.begin(); itWin != setWindows.end(); ++itWin) {
    out << (*(*itWin)) << std::endl;

    if ((*itWin)->GetDisplay()) {
      setDisplays.insert((*itWin)->GetDisplay());
    }
  }

  set<VistaDisplay*>::iterator itDisp;
  for (itDisp = setDisplays.begin(); itDisp != setDisplays.end(); ++itDisp) {
    out << (*(*itDisp)) << std::endl;
  }
}

void VistaDisplayManager::HandleEvent(VistaEvent* pEvent) {
}

ostream& operator<<(std::ostream& out, const VistaDisplayManager& refDisplayManager) {
  refDisplayManager.Debug(out);
  return out;
}

/*============================================================================*/
/*  LOCAL VARS / FUNCTIONS                                                    */
/*============================================================================*/

VistaVirtualPlatform* CreateRefFrameFromSection(
    const std::string& sSectionName, const VistaPropertyList& oConfig) {
  vstr::outi() << "Creating virtual platform from section [" << sSectionName << "]" << std::endl;
  vstr::IndentObject oRefFrameIndent;

  if (oConfig.HasSubList(sSectionName) == false) {
    vstr::warnp() << "Cannot create reference frame - section [" << sSectionName
                  << "] does not exist" << std::endl;
    return NULL;
  }
  const VistaPropertyList& oFrameProps = oConfig.GetSubListConstRef(sSectionName);

  VistaVirtualPlatform* pPlatform = new VistaVirtualPlatform();

  if (oFrameProps.HasProperty("TRANSLATION")) {
    VistaVector3D v3Translation;
    if (oFrameProps.GetValue("TRANSLATION", v3Translation)) {
      pPlatform->SetTranslation(v3Translation);
    } else {
      vstr::warnp() << "Entry for Virtual Platform Translation ["
                    << oFrameProps.GetValue<std::string>("TRANSLATION")
                    << "] does not represent a translation" << std::endl;
    }
  }

  if (oFrameProps.HasProperty("ROTATION")) {
    VistaQuaternion  qQuat;
    VistaEulerAngles oAngles(0, 0, 0);
    if (oFrameProps.GetValue("ROTATION", qQuat)) {
      pPlatform->SetRotation(qQuat);
    } else if (oFrameProps.GetValue("ROTATION", oAngles)) {
      pPlatform->SetRotation(VistaQuaternion(oAngles));
    } else {
      vstr::warnp() << "Entry for Virtual Platform Rotation ["
                    << oFrameProps.GetValue<std::string>("ROTATION")
                    << "] does not represent a rotation" << std::endl;
    }
  }

  float fScale;
  if (oFrameProps.GetValue("SCALE", fScale)) {
    if (fScale > 0) {
      pPlatform->SetScale(fScale);
    } else {
      vstr::warnp() << "Entry for Virtual Platform Scale [" << fScale << " is not positive!"
                    << std::endl;
    }
  }

  std::string sName;
  if (oFrameProps.GetValue("NAME", sName)) {
    pPlatform->SetNameForNameable(sName);
  } else {
    pPlatform->SetNameForNameable(sSectionName);
  }

  return pPlatform;
}

IVistaTextEntity* VistaDisplayManager::CreateTextEntity() {
  return m_pWindowingToolkit->CreateTextEntity();
}

const VistaDisplayManager::RenderInfo* VistaDisplayManager::GetCurrentRenderInfo() const {
  return m_pBridge->GetCurrentRenderInfo();
}
