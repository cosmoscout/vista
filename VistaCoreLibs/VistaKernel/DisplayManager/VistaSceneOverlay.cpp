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

#include "VistaSceneOverlay.h"

#include <VistaAspects/VistaObserver.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * Internal EventHandler to handle VDE_RATIOCHANGE events.
 * Those are fired, when window size changes.
 * This Handler is not in the interface cause there maybe some
 * other handling on the events.
 */
class IVistaSceneOverlay::ViewportResizeObserver : public IVistaObserver {
 public:
  ViewportResizeObserver(IVistaSceneOverlay* pOverlay, VistaViewport* pViewport)
      : IVistaObserver()
      , m_pViewportProps(pViewport->GetViewportProperties())
      , m_pOverlay(pOverlay) {
    m_pViewportProps->AttachObserver(this);
  }

  ~ViewportResizeObserver() {
    m_pViewportProps->DetachObserver(this);
  }

  virtual bool ObserveableDeleteRequest(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    return true;
  }

  virtual void ObserveableDelete(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    if (pObserveable == m_pViewportProps)
      m_pViewportProps = NULL;
  }

  virtual void ReleaseObserveable(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    if (m_pViewportProps == pObserveable)
      m_pViewportProps = NULL;
  }

  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int nMsg, int nTicket) {
    if (nMsg == VistaViewport::VistaViewportProperties::MSG_SIZE_CHANGE) {
      int iPosX, iPosY, iWidth, iHeight;
      m_pViewportProps->GetSize(iWidth, iHeight);
      m_pViewportProps->GetPosition(iPosX, iPosY);
      m_pOverlay->UpdateOnViewportChange(iWidth, iHeight, iPosX, iPosY);
    } else if (nMsg == VistaViewport::VistaViewportProperties::MSG_POSITION_CHANGE) {
      int iPosX, iPosY, iWidth, iHeight;
      m_pViewportProps->GetSize(iWidth, iHeight);
      m_pViewportProps->GetPosition(iPosX, iPosY);
      m_pOverlay->UpdateOnViewportChange(iWidth, iHeight, iPosX, iPosY);
    }
  }

  virtual bool Observes(IVistaObserveable* pObserveable) {
    return (pObserveable == m_pViewportProps);
  }

  virtual void Observe(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    // won't happen
  }

  VistaViewport* GetViewport() {
    return static_cast<VistaViewport*>(m_pViewportProps->GetParent());
  }

 private:
  IVistaSceneOverlay*                     m_pOverlay;
  VistaViewport::VistaViewportProperties* m_pViewportProps;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaSceneOverlay::IVistaSceneOverlay(
    VistaDisplayManager* pDisplayManager, const std::string& sViewportName)
    : m_pViewportObserver(NULL) {
  if (sViewportName.empty()) {
    if (pDisplayManager->GetViewportsConstRef().size() == 1) {
      VistaViewport* pViewport = pDisplayManager->GetViewportsConstRef().begin()->second;
      m_pViewportObserver      = new ViewportResizeObserver(this, pViewport);
      pDisplayManager->AddSceneOverlay(this, pViewport);
    } else {
      vstr::warnp() << "[IVistaSceneOverlay]: "
                    << "Trying to add Overlay to default viewport, but there is not "
                    << "exactly one viewport, but "
                    << pDisplayManager->GetViewportsConstRef().size()
                    << " - Overlay will not be displayed! " << std::endl;
    }
  } else {
    VistaViewport* pViewport = pDisplayManager->GetViewportByName(sViewportName);
    if (pViewport != NULL) {
      m_pViewportObserver = new ViewportResizeObserver(this, pViewport);
      pDisplayManager->AddSceneOverlay(this, pViewport);
    } else {
      vstr::warnp() << "[IVistaSceneOverlay]: "
                    << "Trying to add Overlay to non-existant viewport [" << sViewportName
                    << "] - Overlay will not be displayed! " << std::endl;
    }
  }
}

IVistaSceneOverlay::IVistaSceneOverlay(VistaViewport* pViewport) {
  pViewport->GetDisplaySystem()->GetDisplayManager()->AddSceneOverlay(this, pViewport);
  m_pViewportObserver = new ViewportResizeObserver(this, pViewport);
}

IVistaSceneOverlay::~IVistaSceneOverlay() {
  if (m_pViewportObserver) {
    m_pViewportObserver->GetViewport()->GetDisplaySystem()->GetDisplayManager()->RemSceneOverlay(
        this, m_pViewportObserver->GetViewport());
    delete m_pViewportObserver;
  }
}

VistaViewport* IVistaSceneOverlay::GetAttachedViewport() const {
  if (m_pViewportObserver == NULL)
    return NULL;
  return m_pViewportObserver->GetViewport();
}

bool IVistaSceneOverlay::GetBoundingBox(VistaBoundingBox& bb) {
  return false;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
