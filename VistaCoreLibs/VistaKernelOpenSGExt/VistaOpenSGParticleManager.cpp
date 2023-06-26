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

// For further example code and how to use OpenSG Particle Manager please see the Vista
// ParticleDemo!

#include "VistaOpenSGParticleManager.h"

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaKernelOpenSGExt/VistaOpenSGParticles.h>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include <cstring>
#include <stdio.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaOpenSGParticleManager::VistaOpenSGParticleManager(
    VistaSceneGraph* pSG, VistaGroupNode* pParent)
    : m_pOriginObject(NULL)
    , m_pChangeObject(NULL)
    , m_afParticlePos(NULL)
    , m_afParticleSecPos(NULL)
    , m_afParticleSize(NULL)
    , m_afParticleColor(NULL)
    , m_iNumberOfParticles(0)
    , m_dLastTimeStamp(-1.0f)
    , m_iCurrentNumberOfParticles(0)
    , m_iFirstParticle(0)
    , m_iLastParticle(-1)
    , m_dLastParticleCreated(0)
    , m_dLastParticleDeleted(0)
    , m_dRemainingSeedTime(0)
    , m_bParticleManagerActive(false)
    , m_pParticles(new VistaOpenSGParticles(pSG, pParent))
    , m_pRand(VistaRandomNumberGenerator::GetStandardRNG()) {
  m_pParticles->BeginEdit();
  m_pParticles->SetColorsWithAlpha();
  m_pParticles->SetDrawOrder(VistaOpenSGParticles::BackToFront);
  m_pParticles->SetGLBlendDestFactor();
  m_pParticles->SetGLTexEnvMode(GL_MODULATE);
  m_pParticles->EndEdit();
}

VistaOpenSGParticleManager::~VistaOpenSGParticleManager() {
  delete m_pParticles;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaOpenSGParticleManager::SetParticlesSpriteImage(const std::string& sFilename) {
  m_pParticles->BeginEdit();
  m_pParticles->SetUseSpriteImage(sFilename);
  m_pParticles->SetGLBlendDestFactor();
  m_pParticles->SetGLTexEnvMode(GL_MODULATE);
  m_pParticles->EndEdit();
}

void VistaOpenSGParticleManager::SetParticlesViewMode(VistaOpenSGParticles::PARTICLESMODES eMode) {
  m_pParticles->BeginEdit();
  m_pParticles->SetMode(eMode);
  m_pParticles->EndEdit();
}

void VistaOpenSGParticleManager::SetParticlesViewMode(VistaOpenSGParticles::PARTICLESMODES eMode,
    const std::string& sVertexShader, const std::string& sFragment) {
  m_pParticles->BeginEdit();
  m_pParticles->SetMode(eMode);
  m_pParticles->SetShaderPrograms(sVertexShader, sFragment);
  m_pParticles->EndEdit();
}

void VistaOpenSGParticleManager::SetChangeObject(IParticleChanger* pChangeObject) {
  m_pChangeObject = pChangeObject;
}

void VistaOpenSGParticleManager::SetOriginObject(IParticleChanger* pOriginObject) {
  m_pOriginObject = pOriginObject;
}

void VistaOpenSGParticleManager::SetMaximumParticles(int iMaxParticles) {
  m_iMaxNumberOfParticles = iMaxParticles;
  ReloadParticles();
}

void VistaOpenSGParticleManager::SetParticlesLifetime(float fLifetime) {
  if (fLifetime == -1.0f) {
    m_fParticlesPerSecond = -1;
    m_pOriginObject->SetLifetime(-1);
    return;
  }

  if (m_fParticlesPerSecond * fLifetime <= m_iMaxNumberOfParticles) {
    m_iNumberOfParticles = (int)(m_fParticlesPerSecond * fLifetime);
    m_pOriginObject->SetLifetime(fLifetime);
  } else {
    vstr::warnp() << "VistaOpenSGParticleManager::SetParticlesLifetime() -- "
                  << "More particles requested than available!" << std::endl;
    return;
  }
}

void VistaOpenSGParticleManager::SetParticlesPerSecond(float fParticlesPerSec) {
  if (fParticlesPerSec == -1) {
    m_fParticlesPerSecond = -1;
    m_pOriginObject->SetLifetime(-1);
    return;
  }

  if (fParticlesPerSec * m_pOriginObject->GetLifetime() <= m_iMaxNumberOfParticles) {
    m_fParticlesPerSecond = fParticlesPerSec;
  } else {
    m_fParticlesPerSecond = fParticlesPerSec;
    float fNewLifetime    = m_iMaxNumberOfParticles / m_fParticlesPerSecond;

    vstr::warnp() << "VistaOpenSGParticleManager::SetParticlesPerSecond() -- "
                  << "Particle Rate of [" << fParticlesPerSec << "] at Lifetime ["
                  << m_pOriginObject->GetLifetime()
                  << "would exceed maximal Particle count, reducing lifetime to [" << fNewLifetime
                  << "]" << std::endl;

    m_pOriginObject->SetLifetime(fNewLifetime);
  }
}

void VistaOpenSGParticleManager::SetRandomGenerator(VistaRandomNumberGenerator* pRand) {
  m_pRand = pRand;
}

bool VistaOpenSGParticleManager::ReloadParticles() {
  m_pParticles->BeginEdit();
  m_pParticles->SetNumParticles(m_iMaxNumberOfParticles, true, true);
  m_vecVelocities.resize(m_iMaxNumberOfParticles);
  m_vecLifeTime.resize(m_iMaxNumberOfParticles);
  if (m_fParticlesPerSecond == -1) {
    m_iFirstParticle = 0;
    m_iLastParticle  = m_iMaxNumberOfParticles - 1;
  } else {
    m_iFirstParticle = 0;
    m_iLastParticle  = -1;
  }
  m_dLastParticleCreated = 0;
  m_dLastParticleDeleted = 0;
  m_dRemainingSeedTime   = 0;

  m_afParticlePos = m_pParticles->GetParticlePositions3fField();

  switch (m_pParticles->GetMode()) {
  case (VistaOpenSGParticles::PM_DIRECTEDQUADS):
    m_afParticleSecPos = m_pParticles->GetSecParticlePositions3fField();
    break;

  case (VistaOpenSGParticles::PM_VIEWERQUADS):
    m_afParticleSecPos = NULL;
    break;

  default:
    m_afParticleSecPos = m_pParticles->GetSecParticlePositions3fField();
  }

  m_afParticleSize  = m_pParticles->GetParticleSizes3fField();
  m_afParticleColor = m_pParticles->GetParticleColors4fField();
  m_pParticles->EndEdit();

  for (int i = 0; i < m_iMaxNumberOfParticles; ++i) {
    PrepareParticleAtOrigin(i);
    m_afParticleSize[3 * i]     = 0;
    m_afParticleSize[3 * i + 1] = 0;
    m_afParticleSize[3 * i + 2] = 0;

    if (!m_pParticles->GetColorsWithAlpha()) {
      m_afParticleColor[4 * i + 3] = 1.0;
    }
  }

  if (m_fParticlesPerSecond == -1) {
    for (int i = 0; i < m_iMaxNumberOfParticles; ++i) {
      PrepareParticleAtOrigin(i);
    }

    m_iFirstParticle = 0;
    m_iLastParticle  = m_iMaxNumberOfParticles - 1;
  } else {
    PrepareParticleAtOrigin(m_iFirstParticle);
  }

  return true;
}

void VistaOpenSGParticleManager::PrepareParticleAtOrigin(int iParticleIndex) {
  m_oCalculationParticle.m_a3fPosition        = &m_afParticlePos[3 * iParticleIndex];
  m_oCalculationParticle.m_a3fSecondPosition  = &m_afParticleSecPos[3 * iParticleIndex];
  m_oCalculationParticle.m_a3fVelocity        = &m_vecVelocities[iParticleIndex][0];
  m_oCalculationParticle.m_a3fSize            = &m_afParticleSize[3 * iParticleIndex];
  m_oCalculationParticle.m_a4fColor           = &m_afParticleColor[4 * iParticleIndex];
  m_oCalculationParticle.m_pRemainingLifeTime = &m_vecLifeTime[iParticleIndex];
  m_oCalculationParticle.m_iParticleID        = iParticleIndex;

  m_pOriginObject->ChangeParticle(m_oCalculationParticle, 0.0, 0.0);
}

void VistaOpenSGParticleManager::UpdateParticles(
    VistaType::microtime dCurrentTime, VistaType::microtime dDeltaT) {
  m_dCurrentTimeStamp = dCurrentTime;
  m_dCurrentDeltaT    = dDeltaT;
  m_dRemainingSeedTime += dDeltaT;

  if (!m_bParticleManagerActive) {
    return;
  }

  if ((m_dLastParticleCreated == 0) || (m_dLastParticleDeleted == 0)) {
    m_dLastParticleDeleted = m_dCurrentTimeStamp;
    m_dLastParticleCreated = m_dCurrentTimeStamp;
    return;
  }

  m_pParticles->BeginEdit();

  if (m_afParticleSecPos != NULL) {
    m_pParticles->CopyPosToSecpos();
  }

  if ((m_iLastParticle < m_iFirstParticle) && (m_iLastParticle >= 0)) {

    for (int i = m_iFirstParticle; i < m_iMaxNumberOfParticles; ++i) {
      CalculateParticle(i);
    }

    for (int i = 0; i <= (m_iLastParticle); ++i) {
      CalculateParticle(i);
    }
  } else {
    for (int i = m_iFirstParticle; i <= m_iLastParticle; ++i) {
      CalculateParticle(i);
    }
  }

  // Create New Paticles :D
  if ((m_fParticlesPerSecond != -1) && (m_dRemainingSeedTime >= 0)) {
    while (m_dRemainingSeedTime >= 0) {
      ++m_iLastParticle;
      if (m_iLastParticle >= m_iMaxNumberOfParticles)
        m_iLastParticle = 0;

      if (m_iLastParticle == m_iFirstParticle) {
        ++m_iFirstParticle;
        if (m_iFirstParticle >= m_iMaxNumberOfParticles)
          m_iFirstParticle = 0;
      }

      int nChangeParticle = m_iLastParticle % m_iMaxNumberOfParticles;

      PrepareParticleAtOrigin(nChangeParticle);
      m_oCalculationParticle.m_a3fPosition        = &m_afParticlePos[3 * nChangeParticle];
      m_oCalculationParticle.m_a3fSecondPosition  = &m_afParticleSecPos[3 * nChangeParticle];
      m_oCalculationParticle.m_a3fVelocity        = &m_vecVelocities[nChangeParticle][0];
      m_oCalculationParticle.m_a3fSize            = &m_afParticleSize[3 * nChangeParticle];
      m_oCalculationParticle.m_a4fColor           = &m_afParticleColor[4 * nChangeParticle];
      m_oCalculationParticle.m_pRemainingLifeTime = &m_vecLifeTime[nChangeParticle];
      m_oCalculationParticle.m_iParticleID        = nChangeParticle;

      m_pChangeObject->ChangeParticle(
          m_oCalculationParticle, m_dCurrentTimeStamp, m_dRemainingSeedTime);
      m_dRemainingSeedTime -= (1.0f / m_fParticlesPerSecond);
    }
  }

  m_pParticles->EndEdit();
}

void VistaOpenSGParticleManager::HandleEvent(VistaEvent* pEvent) {

  if (m_dLastTimeStamp < 0) {
    m_dLastTimeStamp = pEvent->GetTime();
    return;
  }

  m_dCurrentTimeStamp = pEvent->GetTime();
  m_dCurrentDeltaT    = m_dCurrentTimeStamp - m_dLastTimeStamp;

  if (!m_bParticleManagerActive) {
    return;
  }

  UpdateParticles(m_dCurrentTimeStamp, m_dCurrentDeltaT);

  m_dLastTimeStamp = m_dCurrentTimeStamp;
}

void VistaOpenSGParticleManager::CalculateParticle(int iParticleIndex) {

  if ((m_vecLifeTime[iParticleIndex] != -1.0f) && (m_vecLifeTime[iParticleIndex] < 0)) {

    m_afParticleSize[3 * iParticleIndex]     = 0.0f;
    m_afParticleSize[3 * iParticleIndex + 1] = 0.0f;
    m_afParticleSize[3 * iParticleIndex + 2] = 0.0f;

    m_afParticlePos[3 * iParticleIndex]     = 0;
    m_afParticlePos[3 * iParticleIndex + 1] = 0;
    m_afParticlePos[3 * iParticleIndex + 2] = 0;

    m_afParticleColor[4 * iParticleIndex + 3] = 0;

    m_iFirstParticle++;

    if (m_iFirstParticle >= m_iMaxNumberOfParticles) {
      m_iFirstParticle = 0;
    }

    m_iCurrentNumberOfParticles--;

    return;
  }

  m_oCalculationParticle.m_a3fPosition        = &m_afParticlePos[3 * iParticleIndex];
  m_oCalculationParticle.m_a3fSecondPosition  = &m_afParticleSecPos[3 * iParticleIndex];
  m_oCalculationParticle.m_a3fVelocity        = &m_vecVelocities[iParticleIndex][0];
  m_oCalculationParticle.m_a3fSize            = &m_afParticleSize[3 * iParticleIndex];
  m_oCalculationParticle.m_a4fColor           = &m_afParticleColor[4 * iParticleIndex];
  m_oCalculationParticle.m_pRemainingLifeTime = &m_vecLifeTime[iParticleIndex];
  m_oCalculationParticle.m_iParticleID        = iParticleIndex;

  m_pChangeObject->ChangeParticle(m_oCalculationParticle, m_dCurrentTimeStamp, m_dCurrentDeltaT);
}

void VistaOpenSGParticleManager::StartParticleManager() {
  if (m_afParticlePos == NULL) {
    ReloadParticleManager();
  }
  m_dLastParticleCreated   = 0;
  m_dLastParticleDeleted   = 0;
  m_bParticleManagerActive = true;
}

void VistaOpenSGParticleManager::ReloadParticleManager() {
  m_bParticleManagerActive = false;
  ReloadParticles();
  StartParticleManager();
}

void VistaOpenSGParticleManager::StopParticleManager() {
  m_bParticleManagerActive = false;
}

VistaOpenSGParticles* VistaOpenSGParticleManager::GetParticles() const {
  return m_pParticles;
}

//######################################################################

void VistaOpenSGParticleManager::IParticleChanger::SetRandomNumberGenerator(
    VistaRandomNumberGenerator* pRand) {
  m_pRand = pRand;
}

void VistaOpenSGParticleManager::IParticleChanger::SetLifetime(float iSetLifetime) {
  m_fLifetime = iSetLifetime;
}

float VistaOpenSGParticleManager::IParticleChanger::GetLifetime() {
  return m_fLifetime;
}

VistaOpenSGParticleManager::IParticleChanger::IParticleChanger()
    : m_pRand(VistaRandomNumberGenerator::GetStandardRNG())
    , m_fLifetime(-1) {
}

VistaOpenSGParticleManager::IParticleChanger::~IParticleChanger() {
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
