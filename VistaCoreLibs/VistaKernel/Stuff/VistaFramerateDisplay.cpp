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

#include "VistaFramerateDisplay.h"

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/VistaFrameLoop.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaFramerateDisplay::VistaFramerateDisplay(
    VistaDisplayManager* pDisplayManager, VistaFrameLoop* pFrameLoop, const bool bOnAllViewports)
    : m_pFrameLoop(pFrameLoop)
    , m_nSize(20)
    , m_oColor(VistaColor::YELLOW)
    , m_sPrefix("Framerate: ")
    , m_sPostfix("")
    , m_bEnabled(false)
    , m_nUpdateFrequency(1.0)
    , m_nLastUpdate(0) {
  m_anPosition[0] = 0.05f;
  m_anPosition[1] = 0.9f;

  if (pDisplayManager->GetViewportsConstRef().empty()) {
    vstr::warnp() << "[VistaFramerateDisplay]: Trying to create"
                  << " FramerateDisplay, but no viewports exist" << std::endl;
  } else if (bOnAllViewports) {
    for (std::map<std::string, VistaViewport*>::const_iterator itViewport =
             pDisplayManager->GetViewportsConstRef().begin();
         itViewport != pDisplayManager->GetViewportsConstRef().end(); ++itViewport) {
      Vista2DText* pText = pDisplayManager->New2DText((*itViewport).first);
      if (pText) {
        m_vecDisplayTexts.push_back(pText);
        pText->Init(" ", m_anPosition[0], m_anPosition[1], (int)(255 * m_oColor[0]),
            (int)(255 * m_oColor[1]), (int)(255 * m_oColor[2]), m_nSize);
      }
    }
  } else {
    Vista2DText* pText =
        pDisplayManager->New2DText((*pDisplayManager->GetViewportsConstRef().begin()).first);
    if (pText) {
      m_vecDisplayTexts.push_back(pText);
      pText->Init(" ", m_anPosition[0], m_anPosition[1], (int)(255 * m_oColor[0]),
          (int)(255 * m_oColor[1]), (int)(255 * m_oColor[2]), m_nSize);
    }
  }

  SetIsEnabled(m_bEnabled);
}

VistaFramerateDisplay::VistaFramerateDisplay(VistaDisplayManager* pDisplayManager,
    VistaFrameLoop* pFrameLoop, const VistaPropertyList& oConfig)
    : m_pFrameLoop(pFrameLoop)
    , m_nLastUpdate(0) {
  m_nSize = oConfig.GetValueOrDefault<int>("TEXTSIZE", 20);

  if (oConfig.GetValueAsArray<2, float>("POSITION", m_anPosition) == false) {
    m_anPosition[0] = 0.05f;
    m_anPosition[1] = 0.9f;
  }
  m_oColor           = oConfig.GetValueOrDefault<VistaColor>("COLOR", VistaColor::YELLOW);
  m_nUpdateFrequency = oConfig.GetValueOrDefault<VistaType::microtime>("UPDATE_FREQUENCY", 1.0f);
  m_bEnabled         = oConfig.GetValueOrDefault<bool>("ENABLED", false);
  m_sPrefix          = oConfig.GetValueOrDefault<std::string>("PREFIX", "Framerate: ");
  m_sPostfix         = oConfig.GetValueOrDefault<std::string>("POSTFIX", "");

  std::vector<std::string> vecViewports;

  if (oConfig.GetValue("VIEWPORTS", vecViewports)) {
    // we got a list already
  } else if (oConfig.GetValueOrDefault<bool>("ALL_VIEWPORTS", true)) {
    for (std::map<std::string, VistaViewport*>::const_iterator itViewport =
             pDisplayManager->GetViewportsConstRef().begin();
         itViewport != pDisplayManager->GetViewportsConstRef().end(); ++itViewport) {
      vecViewports.push_back((*itViewport).first);
    }
    std::vector<std::string> vecExcludePorts;
    if (oConfig.GetValue("EXCLUDE_VIEWPORTS", vecExcludePorts)) {
      for (std::vector<std::string>::const_iterator itExclude = vecExcludePorts.begin();
           itExclude != vecExcludePorts.end(); ++itExclude) {
        std::vector<std::string>::iterator itRem =
            std::remove(vecViewports.begin(), vecViewports.end(), (*itExclude));
        vecViewports.erase(itRem, vecViewports.end());
      }
    }
  } else // just on first viewport
  {
    vecViewports.push_back((*pDisplayManager->GetViewportsConstRef().begin()).first);
  }

  for (std::vector<std::string>::const_iterator itViewport = vecViewports.begin();
       itViewport != vecViewports.end(); ++itViewport) {
    VistaViewport* pViewport = pDisplayManager->GetViewportByName((*itViewport));
    if (pViewport == NULL) {
      vstr::warnp() << "[VistaFramerateDisplay]: Viewport \"" << (*itViewport)
                    << "\" does not exist" << std::endl;
      continue;
    }

    Vista2DText* pText = pDisplayManager->New2DText((*itViewport));
    if (pText) {
      m_vecDisplayTexts.push_back(pText);
      pText->Init(" ", m_anPosition[0], m_anPosition[1], (int)(255 * m_oColor[0]),
          (int)(255 * m_oColor[1]), (int)(255 * m_oColor[2]), m_nSize);
    }
  }

  SetIsEnabled(m_bEnabled);
}

VistaFramerateDisplay::~VistaFramerateDisplay() {
  for (std::vector<Vista2DText*>::iterator itText = m_vecDisplayTexts.begin();
       itText != m_vecDisplayTexts.end(); ++itText) {
    delete (*itText);
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaFramerateDisplay::Notify(const VistaEvent* pEvent) {
  if (pEvent->GetId() == VistaSystemEvent::VSE_POSTGRAPHICS &&
      pEvent->GetTime() - m_nLastUpdate > m_nUpdateFrequency && m_pFrameLoop->GetFrameCount() > 3) {
    SetText();
    m_nLastUpdate = pEvent->GetTime();
  }
}

VistaColor VistaFramerateDisplay::GetColor() const {
  return m_oColor;
}

void VistaFramerateDisplay::SetColor(const VistaColor& oValue) {
  m_oColor = oValue;
  for (std::vector<Vista2DText*>::iterator itText = m_vecDisplayTexts.begin();
       itText != m_vecDisplayTexts.end(); ++itText) {
    (*itText)->SetColor(
        (int)(255.0f * m_oColor[0]), (int)(255.0f * m_oColor[1]), (int)(255.0f * m_oColor[2]));
  }
}

int VistaFramerateDisplay::GetTextSize() const {
  return m_nSize;
}

void VistaFramerateDisplay::SetTextSize(const int oValue) {
  m_nSize = oValue;
  for (std::vector<Vista2DText*>::iterator itText = m_vecDisplayTexts.begin();
       itText != m_vecDisplayTexts.end(); ++itText) {
    (*itText)->SetSize(m_nSize);
  }
}

void VistaFramerateDisplay::GetPosition(float afPosition[2]) const {
  afPosition[0] = m_anPosition[0];
  afPosition[1] = m_anPosition[1];
}

VistaVector3D VistaFramerateDisplay::GetPosition() const {
  return VistaVector3D(m_anPosition[0], m_anPosition[1], 0);
}

void VistaFramerateDisplay::SetPosition(const float anPosition[2]) {
  m_anPosition[0] = anPosition[0];
  m_anPosition[1] = anPosition[1];
  for (std::vector<Vista2DText*>::iterator itText = m_vecDisplayTexts.begin();
       itText != m_vecDisplayTexts.end(); ++itText) {
    (*itText)->SetPosition(m_anPosition[0], m_anPosition[1]);
  }
}

void VistaFramerateDisplay::SetPosition(const VistaVector3D& v3Position) {
  m_anPosition[0] = v3Position[0];
  m_anPosition[1] = v3Position[1];
  for (std::vector<Vista2DText*>::iterator itText = m_vecDisplayTexts.begin();
       itText != m_vecDisplayTexts.end(); ++itText) {
    (*itText)->SetPosition(m_anPosition[0], m_anPosition[1]);
  }
}

VistaType::microtime VistaFramerateDisplay::GetUpdateFrequency() const {
  return m_nUpdateFrequency;
}

void VistaFramerateDisplay::SetUpdateFrequency(const VistaType::microtime& oValue) {
  m_nUpdateFrequency = oValue;
}

std::string VistaFramerateDisplay::GetPrefix() const {
  return m_sPrefix;
}

void VistaFramerateDisplay::SetPrefix(const std::string& oValue) {
  m_sPrefix = oValue;
}

std::string VistaFramerateDisplay::GetPostfix() const {
  return m_sPostfix;
}

void VistaFramerateDisplay::SetPostfix(const std::string& oValue) {
  m_sPostfix = oValue;
}

void VistaFramerateDisplay::SetText() {
  char acBuffer[32];
  sprintf(acBuffer, "%4.2f", (double)m_pFrameLoop->GetFrameRate());
  std::string sText = m_sPrefix + std::string(acBuffer) + m_sPostfix;
  for (std::vector<Vista2DText*>::iterator itText = m_vecDisplayTexts.begin();
       itText != m_vecDisplayTexts.end(); ++itText) {
    (*itText)->SetText(sText);
  }
}

bool VistaFramerateDisplay::GetIsEnabled() const {
  return m_bEnabled;
}

void VistaFramerateDisplay::SetIsEnabled(const bool& oValue) {
  m_bEnabled = oValue;
  for (std::vector<Vista2DText*>::iterator itText = m_vecDisplayTexts.begin();
       itText != m_vecDisplayTexts.end(); ++itText) {
    (*itText)->SetEnabled(m_bEnabled);
  }
}
