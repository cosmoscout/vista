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

#include "VistaProximityWarningBase.h"

#include <VistaMath/VistaBoundingBox.h>

#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <algorithm>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaProximityWarningBase::IVistaProximityWarningBase(
    VistaEventManager* pEventManager, const float nSafeDistance, const float nDangerDistance)
    : m_nSafeDistance(nSafeDistance)
    , m_nDangerDistance(nDangerDistance)
    , m_bWasUpdated(false)
    , m_nLastUpdate(0)
    , m_nHideTimeout(0)
    , m_nHideFadeoutTime(0)
    , m_nFlashAfterLostTracking(0)
    , m_pEventManager(pEventManager)
    , m_nDangerZoneEnterTime(0)
    , m_bFlashInDangerZone(true)
    , m_nFlashPeriod(0.5)
    , m_bIsInDangerZone(false)
    , m_bIsInTransitionZone(false)
    , m_bIsOutside(true) {
  m_pEventManager->RegisterObserver(this, VistaSystemEvent::GetTypeId());
}

IVistaProximityWarningBase::~IVistaProximityWarningBase() {
  m_pEventManager->UnregisterObserver(this, VistaSystemEvent::GetTypeId());
}

void IVistaProximityWarningBase::AddHalfPlane(
    const VistaVector3D& v3Center, const VistaVector3D& v3Normal) {
  m_vecHalfPlanes.push_back(HalfPlane());
  m_vecHalfPlanes.back().m_v3Center = v3Center;
  m_vecHalfPlanes.back().m_v3Normal = v3Normal;
}

void IVistaProximityWarningBase::SetUseExtents(
    const VistaBoundingBox& bbExtents, const VistaQuaternion& qBoxRotation) {
  VistaVector3D v3Center;
  VistaVector3D v3Normal;

  VistaVector3D v3BoxCenter = bbExtents.GetCenter();
  VistaVector3D v3Extents   = 0.5f * bbExtents.GetSize();

  v3Normal = qBoxRotation.Rotate(Vista::XAxis);
  v3Center = v3BoxCenter - v3Extents[Vista::X] * v3Normal;
  AddHalfPlane(v3Center, v3Normal);
  v3Normal = -v3Normal;
  v3Center = v3BoxCenter - v3Extents[Vista::X] * v3Normal;
  AddHalfPlane(v3Center, v3Normal);

  v3Normal = qBoxRotation.Rotate(Vista::YAxis);
  v3Center = v3BoxCenter - v3Extents[Vista::Y] * v3Normal;
  AddHalfPlane(v3Center, v3Normal);
  v3Normal = -v3Normal;
  v3Center = v3BoxCenter - v3Extents[Vista::Y] * v3Normal;
  AddHalfPlane(v3Center, v3Normal);

  v3Normal = qBoxRotation.Rotate(Vista::ZAxis);
  v3Center = v3BoxCenter - v3Extents[Vista::Z] * v3Normal;
  AddHalfPlane(v3Center, v3Normal);
  v3Normal = -v3Normal;
  v3Center = v3BoxCenter - v3Extents[Vista::Z] * v3Normal;
  AddHalfPlane(v3Center, v3Normal);
}

bool IVistaProximityWarningBase::Update(
    const VistaVector3D& v3ViewerPosition, const VistaQuaternion& qViewerOrientation) {
  return Update(v3ViewerPosition, qViewerOrientation, std::vector<VistaVector3D>());
}

bool IVistaProximityWarningBase::Update(const VistaVector3D& v3ViewerPosition,
    const VistaQuaternion&                                   qViewerOrientation,
    const std::vector<VistaVector3D>&                        vecAdditionalPoints) {
  float         nDistance = std::numeric_limits<float>::max();
  VistaVector3D v3NearestPosition;

  for (std::vector<HalfPlane>::const_iterator itPlane = m_vecHalfPlanes.begin();
       itPlane != m_vecHalfPlanes.end(); ++itPlane) {
    UpdateFromHalfplane((*itPlane), v3ViewerPosition, nDistance, v3NearestPosition);
    for (std::vector<VistaVector3D>::const_iterator itPoint = vecAdditionalPoints.begin();
         itPoint != vecAdditionalPoints.end(); ++itPoint) {
      UpdateFromHalfplane((*itPlane), v3ViewerPosition, nDistance, v3NearestPosition);
    }
  }

  float nWarningLevel = nDistance - m_nDangerDistance;
  nWarningLevel /= (m_nSafeDistance - m_nDangerDistance);
  nWarningLevel = Vista::Clamp<float>(nWarningLevel, 0, 1);
  nWarningLevel = 1 - nWarningLevel;

  m_bIsOutside = (nDistance <= 0);
  if (m_bIsOutside)
    return false;

  m_bIsInDangerZone = (nWarningLevel >= 1);

  m_bIsInTransitionZone = (nWarningLevel > 0 && nWarningLevel < 1);

  m_bWasUpdated = true;

  // std::cout << "Dist: " << nDistance << "               warn: " << nWarningLevel <<std::endl;
  return DoUpdate(
      nDistance, nWarningLevel, v3NearestPosition, v3ViewerPosition, qViewerOrientation);
}

void IVistaProximityWarningBase::UpdateFromHalfplane(const HalfPlane& oPlane,
    const VistaVector3D& v3UserPosition, float& nDistance, VistaVector3D& v3ClostestPoint) {
  VistaVector3D v3Delta      = v3UserPosition - oPlane.m_v3Center;
  float         nOwnDistance = v3Delta.Dot(oPlane.m_v3Normal);
  if (nOwnDistance < nDistance) {
    v3ClostestPoint = v3UserPosition - nOwnDistance * oPlane.m_v3Normal;
    nDistance       = nOwnDistance;
  }
}

float IVistaProximityWarningBase::GetSafeDistance() const {
  return m_nSafeDistance;
}

void IVistaProximityWarningBase::SetSafeDistance(const float nDistance) {
  m_nSafeDistance = nDistance;
}

float IVistaProximityWarningBase::GetDangerDistance() const {
  return m_nDangerDistance;
}

void IVistaProximityWarningBase::SetDangerDistance(const float nDistance) {
  m_nDangerDistance = nDistance;
}

void IVistaProximityWarningBase::SetTimeout(
    const VistaType::microtime nHideTimeout, VistaType::microtime nFadeoutTime) {
  m_nHideTimeout     = nHideTimeout;
  m_nHideFadeoutTime = nFadeoutTime;
}

void IVistaProximityWarningBase::Notify(const VistaEvent* pEvent) {
  if (pEvent->GetId() == VistaSystemEvent::VSE_PREGRAPHICS) {
    float nOpacity = 1.0f;
    if (m_bWasUpdated) {
      m_bWasUpdated = false;
      m_nLastUpdate = pEvent->GetTime();
    } else {
      VistaType::microtime nDelta = pEvent->GetTime() - m_nLastUpdate;
      if (m_nHideTimeout > 0 && nDelta > m_nHideTimeout) {
        nOpacity =
            std::max(0.0f, 1.0f - (float)(nDelta - m_nHideTimeout) / (float)m_nHideFadeoutTime);
      }
    }

    bool bFlashState = false;
    if (m_bIsInDangerZone) {
      if (m_nDangerZoneEnterTime == 0)
        m_nDangerZoneEnterTime = pEvent->GetTime();
      else if (m_bFlashInDangerZone && m_nFlashPeriod > 0) {
        VistaType::microtime nTimeInDanger = pEvent->GetTime() - m_nDangerZoneEnterTime;
        nTimeInDanger /= m_nFlashPeriod;
        bFlashState = ((int)nTimeInDanger % 2 == 1);
      }
    } else if (m_bIsInTransitionZone && m_nFlashAfterLostTracking > 0)

    {
      VistaType::microtime nDelta = pEvent->GetTime() - m_nLastUpdate;
      if (nDelta > m_nFlashAfterLostTracking) {
        nDelta -= m_nFlashAfterLostTracking;
        nDelta /= m_nFlashPeriod;
        bFlashState = ((int)nDelta % 2 == 0);
      }
    }

    DoTimeUpdate(pEvent->GetTime(), nOpacity, bFlashState);
  }
}

bool IVistaProximityWarningBase::GetFlashInDangerZone() const {
  return m_bFlashInDangerZone;
}

void IVistaProximityWarningBase::SetFlashInDangerZone(const bool& oValue) {
  m_bFlashInDangerZone = oValue;
}

VistaType::microtime IVistaProximityWarningBase::GetDangerZoneFlashTime() const {
  return m_nFlashPeriod;
}

void IVistaProximityWarningBase::SetFlashPeriod(const VistaType::microtime oValue) {
  m_nFlashPeriod = oValue;
}

VistaType::microtime IVistaProximityWarningBase::GetFlashAfterLostTracking() const {
  return m_nFlashAfterLostTracking;
}

void IVistaProximityWarningBase::SetFlashAfterLostTracking(const VistaType::microtime& oValue) {
  m_nFlashAfterLostTracking = oValue;
}
