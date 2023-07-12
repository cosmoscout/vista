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

#include "VistaViewportResizeToProjectionAdapter.h"
#include "VistaBase/VistaUtilityMacros.h"
#include "VistaProjection.h"
#include "VistaViewport.h"

#include <algorithm>
#include <cassert>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

VistaViewportResizeToProjectionAdapter::VistaViewportResizeToProjectionAdapter(
    VistaViewport* pViewport)
    : m_pViewport(NULL)
    , m_eUpdateMode(NO_PROJECTION_CHANGE)
    , m_nCurrentSizeX(0)
    , m_nCurrentSizeY(0) {
  SetViewport(pViewport);
  assert(m_pViewport == pViewport);
}

VistaViewportResizeToProjectionAdapter::~VistaViewportResizeToProjectionAdapter() {
}

void VistaViewportResizeToProjectionAdapter::ObserverUpdate(
    IVistaObserveable* pObserveable, int nMsg, int nTicket) {
  assert(Observes(pObserveable));

  if (nMsg == VistaViewport::VistaViewportProperties::MSG_SIZE_CHANGE)
    UpdateProjectionAfterViewportChange();
}

std::string VistaViewportResizeToProjectionAdapter::GetNameForProjectionUpdateMode(
    const ProjectionUpdateMode eMode) {
  switch (eMode) {
  case NO_PROJECTION_CHANGE:
    return "NO_PROJECTION_CHANGE";
  case MAINTAIN_HORIZONTAL_FOV:
    return "MAINTAIN_HORIZONTAL_FOV";
  case MAINTAIN_VERTICAL_FOV:
    return "MAINTAIN_VERTICAL_FOV";
  case MAINTAIN_SCALE:
    return "MAINTAIN_SCALE";
  case UNDEFINED_PROJECTION_UPDATE_MODE:
    return "UNDEFINED";
  default:
    VISTA_THROW("Undefined ProjectionUpdateMode value", -1);
  }
}

VistaViewportResizeToProjectionAdapter::ProjectionUpdateMode
VistaViewportResizeToProjectionAdapter::GetProjectionUpdateModeFromString(
    const std::string& sString) {
  std::string sReducedString = VistaConversion::StringToUpper(sString);
  sReducedString.erase(
      std::remove(sReducedString.begin(), sReducedString.end(), ' ')); // remove spaces
  sReducedString.erase(
      std::remove(sReducedString.begin(), sReducedString.end(), '_')); // remove underscores

  if (sReducedString == "NOPROJECTIONCHANGE")
    return NO_PROJECTION_CHANGE;
  else if (sReducedString == "MAINTAIN_HORIZONTAL_FOV")
    return MAINTAIN_HORIZONTAL_FOV;
  else if (sReducedString == "MAINTAIN_VERTICAL_FOV")
    return MAINTAIN_VERTICAL_FOV;
  else
    return UNDEFINED_PROJECTION_UPDATE_MODE;
}

VistaViewport* VistaViewportResizeToProjectionAdapter::GetViewport() const {
  return m_pViewport;
}

void VistaViewportResizeToProjectionAdapter::SetViewport(VistaViewport* pViewport) {
  m_pViewport = pViewport;
  m_pViewport->GetViewportProperties()->GetSize(m_nCurrentSizeX, m_nCurrentSizeY);
  Observe(m_pViewport->GetViewportProperties());
}

VistaViewportResizeToProjectionAdapter::ProjectionUpdateMode
VistaViewportResizeToProjectionAdapter::GetUpdateMode() const {
  return m_eUpdateMode;
}

void VistaViewportResizeToProjectionAdapter::SetUpdateMode(const ProjectionUpdateMode eProjection) {
  m_eUpdateMode = eProjection;
}

void VistaViewportResizeToProjectionAdapter::UpdateProjectionAfterViewportChange() {
  if (m_pViewport == NULL || m_pViewport->GetProjection() == NULL)
    return;

  int nNewSizeX = 0;
  int nNewSizeY = 0;
  m_pViewport->GetViewportProperties()->GetSize(nNewSizeX, nNewSizeY);

  // prevent invalid projection when viewport is resized to zero dimension
  if (nNewSizeX == 0 || nNewSizeY == 0)
    return;

  double dProjectionFactorX = 1.0;
  double dProjectionFactorY = 1.0;
  double dRatioX            = (double)nNewSizeX / (double)m_nCurrentSizeX;
  double dRatioY            = (double)nNewSizeY / (double)m_nCurrentSizeY;

  m_nCurrentSizeX = nNewSizeX;
  m_nCurrentSizeY = nNewSizeY;

  switch (m_eUpdateMode) {
  case MAINTAIN_HORIZONTAL_FOV: {
    dProjectionFactorY = dRatioY / dRatioX;
    break;
  }
  case MAINTAIN_VERTICAL_FOV: {
    dProjectionFactorX = dRatioX / dRatioY;
    break;
  }
  case MAINTAIN_SCALE: {
    dProjectionFactorX = dRatioX;
    dProjectionFactorY = dRatioY;
    break;
  }
  case UNDEFINED_PROJECTION_UPDATE_MODE:
  case NO_PROJECTION_CHANGE:
  default:
    return;
  }
  VistaProjection::VistaProjectionProperties* pProjProps =
      m_pViewport->GetProjection()->GetProjectionProperties();
  double dLeft   = 0.0f;
  double dRight  = 0.0f;
  double dBottom = 0.0f;
  double dTop    = 0.0f;
  if (pProjProps->GetProjPlaneExtents(dLeft, dRight, dBottom, dTop) == false)
    return;
  dLeft *= dProjectionFactorX;
  dRight *= dProjectionFactorX;
  dBottom *= dProjectionFactorY;
  dTop *= dProjectionFactorY;
  pProjProps->SetProjPlaneExtents(dLeft, dRight, dBottom, dTop);
}
