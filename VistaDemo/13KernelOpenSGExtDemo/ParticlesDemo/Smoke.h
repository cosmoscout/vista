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

#ifndef _CSMOKE_H_
#define _CSMOKE_H_

#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaTools/VistaRandomNumberGenerator.h>

class Smoke : public VistaEventHandler {
 public:
  Smoke(VistaGroupNode* pParent, VistaSceneGraph* pSG)
      : m_pParticles(NULL)
      , m_dLastT(-1) {
    VistaRandomNumberGenerator* pRand = VistaRandomNumberGenerator::GetStandardRNG();
    m_pParticles                      = new VistaOpenSGParticles(pSG, pParent);
    m_pParticles->SetColorsWithAlpha();
    m_pParticles->SetNumParticles(150, true, true);
    m_pParticles->BeginEdit();
    float* pos = m_pParticles->GetParticlePositions3fField();
    float* col = m_pParticles->GetParticleColors4fField();
    float* siz = m_pParticles->GetParticleSizes3fField();
    m_vVelocities.resize(m_pParticles->GetNumParticles());

    // init
    const int sz = m_pParticles->GetNumParticles();
    for (int i = 0; i < sz; ++i) {
      const int idx3 = i * 3;
      const int idx4 = i * 4;

      // positions
      pos[idx3 + 0] = ((float)(pRand->GenerateDouble3()) - 0.5f) * 0.1f;
      pos[idx3 + 1] = 0.0f;
      pos[idx3 + 2] = ((float)(pRand->GenerateDouble3()) - 0.5f) * 0.1f;

      // alpha is the TTL, too
      col[idx4 + 3] = 1 - (float)i / (float)sz;

      // colors (black to white)
      col[idx4 + 0] = col[idx4 + 1] = col[idx4 + 2] = 1.0f - col[idx4 + 3];

      // sizes
      siz[idx3 + 0] = 0.2f - 0.1f * (float)(pRand->GenerateDouble3());
      siz[idx3 + 1] = siz[idx3 + 2] = siz[idx3 + 0];

      // velocities

      // make them ascend
      m_vVelocities[i][1] = 0.8f + (float)(pRand->GenerateDouble3()) * 0.1f;

      // add drift to the right(wind)
      m_vVelocities[i][0] = 0.2f + ((float)(pRand->GenerateDouble3()) - 0.5f) * 0.01f;
      m_vVelocities[i][2] = ((float)(pRand->GenerateDouble3()) - 0.5f) * 0.01f;
    }

    m_pParticles->EndEdit();
    m_pParticles->SetUseSpriteImage("../../data/smoke.rgb");
    m_pParticles->SetDrawOrder(VistaOpenSGParticles::BackToFront);
    m_pParticles->SetGLTexEnvMode(GL_MODULATE);
    m_pParticles->SetGLBlendDestFactor();
  }

  ~Smoke() {
  }

  virtual void HandleEvent(VistaEvent* pEvent) {
    // update particles
    // particle behaviour: particles get bigger and "thinner" on time, upwards-speed decreases.

    if (m_dLastT == -1)
      m_dLastT = pEvent->GetTime();
    const double dt = pEvent->GetTime() - m_dLastT;
    m_dLastT        = pEvent->GetTime();

    VistaRandomNumberGenerator* pRand = VistaRandomNumberGenerator::GetStandardRNG();

    float* pos = m_pParticles->GetParticlePositions3fField();
    float* siz = m_pParticles->GetParticleSizes3fField();
    float* col = m_pParticles->GetParticleColors4fField();

    m_pParticles->BeginEdit();
    for (unsigned int i = 0; i < m_vVelocities.size(); ++i) {
      const int idx3 = i * 3;
      const int idx4 = i * 4;

      // move smoke particles
      pos[idx3 + 0] += (float)(dt)*m_vVelocities[i][0];
      pos[idx3 + 1] += (float)(dt)*m_vVelocities[i][1];
      pos[idx3 + 2] += (float)(dt)*m_vVelocities[i][2];

      // grow smoke particles
      float nSize = siz[idx3 + 2] + (float)(dt)*0.5f;
      ;
      for (int j = 0; j < 3; ++j)
        siz[idx3 + j] = nSize;

      // make smoke particles thinner	30% per second
      col[idx4 + 0] = col[idx4 + 1] = col[idx4 + 2] = 1.0f - col[idx4 + 3];
      col[idx4 + 3] *= 1.0f - (float)(dt)*0.25f;

      // slowdown
      m_vVelocities[i][1] *= 1.0f - (float)(dt)*0.2f;

      // rebirth particles
      if (col[idx4 + 3] < 0.1f) {
        // positions
        pos[idx3 + 0] = ((float)(pRand->GenerateDouble3()) - 0.5f) * 0.1f;
        pos[idx3 + 1] = (float)(pRand->GenerateDouble3()) * 0.3f;
        pos[idx3 + 2] = ((float)(pRand->GenerateDouble3()) - 0.5f) * 0.1f;

        // alpha is the TTL, too
        col[idx4 + 3] = (float)(pRand->GenerateDouble3());

        // colors (black to white)
        col[idx4 + 0] = col[idx4 + 1] = col[idx4 + 2] = 1.0f - col[idx4 + 3];

        // sizes
        siz[idx3 + 0] = 0.2f - 0.1f * (float)(pRand->GenerateDouble3());
        siz[idx3 + 1] = siz[idx3 + 2] = siz[idx3 + 0];

        // reset upwards drift
        m_vVelocities[i][1] = 0.8f + (float)(pRand->GenerateDouble3()) * 0.1f;
      }
    }
    m_pParticles->EndEdit();
  }

 private:
  VistaOpenSGParticles*      m_pParticles;
  std::vector<VistaVector3D> m_vVelocities;

  double m_dLastT;
};

#endif
