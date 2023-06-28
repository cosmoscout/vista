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

#ifndef _VISTAOPENSGPARTICLEMANAGER_H
#define _VISTAOPENSGPARTICLEMANAGER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernelOpenSGExt/VistaKernelOpenSGExtConfig.h>
#include <VistaKernelOpenSGExt/VistaOpenSGParticles.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGroupNode;
class VistaSceneGraph;
class VistaRandomNumberGenerator;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

// Please see 13ParticlesDemo for how to use the Particle Manager!

class VISTAKERNELOPENSGEXTAPI VistaOpenSGParticleManager : public VistaEventHandler {
 public:
  class VISTAKERNELOPENSGEXTAPI IParticleChanger {
   public:
    struct SParticle {
      float* m_a3fPosition;
      float* m_a3fSecondPosition;
      float* m_a3fVelocity;
      float* m_a4fColor;
      float* m_a3fSize;
      float* m_pRemainingLifeTime;
      int    m_iParticleID;
    };

    virtual ~IParticleChanger();
    IParticleChanger();

    /**
     * Implement this function to define the behavior of the particles
     * This can either be used to initialize a new particles, or to update
     * an existing on. If dCurrentTime == 0, the particle is to be created,
     * and thus, the values in SVistaParticle are not yet defined, but
     * have to be set by the function. Otherwise, the particle exists
     * already and only has to be updated, data in the particle struct
     * is valid.
     */
    virtual void ChangeParticle(IParticleChanger::SParticle& oParticle,
        VistaType::microtime dCurrentTime, VistaType::microtime dDeltaT) = 0;

    void SetRandomNumberGenerator(VistaRandomNumberGenerator* pRand);

    void SetLifetime(float iSetLifetime);

    float GetLifetime();

   protected:
    float                       m_fLifetime;
    VistaRandomNumberGenerator* m_pRand;
  };

 public:
  VistaOpenSGParticleManager(VistaSceneGraph* pSG, VistaGroupNode* pParent = NULL);
  virtual ~VistaOpenSGParticleManager();

  void SetMaximumParticles(int iMaxParticles);
  void SetParticlesPerSecond(float fParticlesPerSec);
  void SetParticlesLifetime(float fLifetime);

  void SetParticlesSpriteImage(const std::string& sFilename);

  void SetParticlesViewMode(VistaOpenSGParticles::PARTICLESMODES eMode);
  void SetParticlesViewMode(VistaOpenSGParticles::PARTICLESMODES eMode,
      const std::string& sVertexShader, const std::string& sFragment);
  bool ReloadParticles();

  void SetChangeObject(IParticleChanger* pChangeObject);
  void SetOriginObject(IParticleChanger* pOriginObject);

  void StartParticleManager();
  void StopParticleManager();
  void ReloadParticleManager();

  void SetRandomGenerator(VistaRandomNumberGenerator* pRand);

  virtual void HandleEvent(VistaEvent* pEvent);
  void         UpdateParticles(VistaType::microtime dCurrentTime, VistaType::microtime dDeltaT);

  VistaOpenSGParticles* GetParticles() const;

 private:
  void PrepareParticleAtOrigin(int iParticleIndex);
  void CalculateParticle(int iParticleIndex);

  VistaType::microtime m_dLastTimeStamp;
  VistaType::microtime m_dCurrentDeltaT;
  VistaType::microtime m_dCurrentTimeStamp;
  VistaType::microtime m_dLastParticleDeleted;
  VistaType::microtime m_dLastParticleCreated;
  VistaType::microtime m_dRemainingSeedTime;
  bool                 m_bParticleManagerActive;

  int m_iNumberOfParticles;
  int m_iMaxNumberOfParticles;
  int m_iFirstParticle;
  int m_iLastParticle;

  float m_fParticlesPerSecond;
  int   m_iCurrentNumberOfParticles;

  VistaOpenSGParticles* m_pParticles;

  float*                     m_afParticlePos;
  float*                     m_afParticleSecPos;
  float*                     m_afParticleColor;
  float*                     m_afParticleSize;
  std::vector<VistaVector3D> m_vecVelocities;
  std::vector<float>         m_vecLifeTime;

  VistaRandomNumberGenerator* m_pRand;

  IParticleChanger* m_pOriginObject;
  IParticleChanger* m_pChangeObject;

  IParticleChanger::SParticle m_oCalculationParticle;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTAOPENSGPARTICLEMANAGER_H
