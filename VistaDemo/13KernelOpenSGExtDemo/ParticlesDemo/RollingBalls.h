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


#ifndef _CROLLINGBALLS_H_
#define _CROLLINGBALLS_H_

#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaTools/VistaRandomNumberGenerator.h>
#include <VistaKernelOpenSGExt/VistaOpenSGParticleManager.h>

/**********************************************************************************/
/* Please note that we got an implementation with and without the ParticleManager */
/**********************************************************************************/

class RollingBallsOriginSet : public VistaOpenSGParticleManager::IParticleChanger
{

//RollingBallsOridinSet(VistaVector3D(10,0,10),VistaVector3D(4,0,10),VistaVector3D())

public:

	RollingBallsOriginSet(
		VistaVector3D v3ParticleVelocities,
		VistaVector3D v3ParticleOrigin,
		VistaVector3D v3ParticleSize)
		: IParticleChanger(),
		m_v3ParticleVelocities(v3ParticleVelocities),
		m_v3ParticleOrigin(v3ParticleOrigin),
		m_v3ParticleSize(v3ParticleSize)
	{
		m_fLifetime = 1;
	}

	void ChangeParticle(SParticle& oParticle,
		VistaType::microtime dCurrentTime,
		VistaType::microtime dDeltaT  )
	{
		*oParticle.m_pRemainingLifeTime = (float)m_fLifetime;

		CalculatePositionAndVelocities(oParticle, dCurrentTime);
		CalculateSize(oParticle, dCurrentTime);

	}

	void CalculatePositionAndVelocities(SParticle& oParticle, VistaType::microtime fEventTime)
	{

		oParticle.m_a3fVelocity[0] = m_v3ParticleVelocities[0] * ( (float)m_pRand->GenerateDouble3() - 0.5f );
		oParticle.m_a3fVelocity[1] = 0;
		oParticle.m_a3fVelocity[2] = m_v3ParticleVelocities[2] * ( (float)m_pRand->GenerateDouble3() - 0.5f );

		oParticle.m_a3fPosition[0] = m_v3ParticleOrigin[0] * (float)m_pRand->GenerateDouble3();
		oParticle.m_a3fPosition[1] = 0;
		oParticle.m_a3fPosition[2] = m_v3ParticleOrigin[2] * (float)m_pRand->GenerateDouble3();

	}

	void CalculateSize(SParticle& oParticle, VistaType::microtime fEventTime)
	{

		oParticle.m_a3fSize[0] = m_v3ParticleSize[0];
		oParticle.m_a3fSize[1] = m_v3ParticleSize[1];
		oParticle.m_a3fSize[2] = m_v3ParticleSize[2];

	}

private:

	VistaVector3D m_v3ParticleVelocities;
	VistaVector3D m_v3ParticleOrigin;
	VistaVector3D m_v3ParticleSize;

};

//RollingBallsFrameChange(VistaVector3D(4,0,10));
class RollingBallsFrameChange : public VistaOpenSGParticleManager::IParticleChanger
{

public:

	RollingBallsFrameChange(VistaVector3D v3FieldSize)
		: IParticleChanger(),
		m_v3FieldSize(v3FieldSize)
	{
	}

	void ChangeParticle( SParticle& oParticle,
		VistaType::microtime dCurrentTime,
		VistaType::microtime dDeltaT  )
	{
		CalculatePositionAndVelocities(oParticle, dDeltaT);
	}

	void CalculatePositionAndVelocities(SParticle& oParticle, VistaType::microtime dt)
	{

		// bounce
		if(oParticle.m_a3fPosition[0] > m_v3FieldSize[0])
		{
			oParticle.m_a3fPosition[0] = m_v3FieldSize[0];
			oParticle.m_a3fVelocity[0] *= -1;
		}
		else if(oParticle.m_a3fPosition[0] < 0)
		{
			oParticle.m_a3fPosition[0] = 0;
			oParticle.m_a3fVelocity[0] *= -1;
		}

		if(oParticle.m_a3fPosition[2] > m_v3FieldSize[2])
		{
			oParticle.m_a3fPosition[2] = m_v3FieldSize[2];
			oParticle.m_a3fVelocity[2] *= -1;
		}
		else if(oParticle.m_a3fPosition[2] < 0)
		{
			oParticle.m_a3fPosition[2] = 0;
			oParticle.m_a3fVelocity[2] *= -1;
		}

		oParticle.m_a3fPosition[0] += (float)(dt) * oParticle.m_a3fVelocity[0];
		oParticle.m_a3fPosition[2] += (float)(dt) * oParticle.m_a3fVelocity[2];
	}

private:

	VistaVector3D m_v3FieldSize;

};

class RollingBalls : public VistaEventHandler
{
public:
	RollingBalls(VistaGroupNode *pParent, VistaSceneGraph *pSG, const int &nParticles = 50)
		: m_fWidth(4)
		, m_fDepth(10)
		, m_dLastT(-1)
	{
		VistaRandomNumberGenerator *pRand = VistaRandomNumberGenerator::GetStandardRNG();
		m_pParticles = new VistaOpenSGParticles( pSG, pParent );
		m_pParticles->SetNumParticles( nParticles, false, false );
		m_vecVelocities.resize( nParticles );
		m_pParticles->BeginEdit();
		float *pos = m_pParticles->GetParticlePositions3fField();
		float *siz = m_pParticles->GetParticleSizes3fField();

		siz[0] = 0.25;
		siz[1] = 0.25;
		siz[2] = 0.25;

		// init positions
		for(int i = 0; i < m_pParticles->GetNumParticles() * 3; i += 3)
		{
			pos[i+0] = (float)(pRand->GenerateDouble3()) * m_fWidth;
			pos[i+1] = 0;
			pos[i+2] = (float)(pRand->GenerateDouble3()) * m_fDepth;
		}

		// init velocities (keep on floor)
		for(unsigned int i = 0; i < m_vecVelocities.size(); ++i)
		{
			m_vecVelocities[i][0] = 10 * ((float)(pRand->GenerateDouble3())-0.5f);
			m_vecVelocities[i][1] = 0.0;
			m_vecVelocities[i][2] = 10 * ((float)(pRand->GenerateDouble3())-0.5f);
		}

		m_pParticles->EndEdit();
		m_pParticles->SetUseSpriteImage( "../../data/ball.png" );
		m_pParticles->SetDrawOrder( VistaOpenSGParticles::BackToFront );
	}

	virtual ~RollingBalls()
	{
		delete m_pParticles;
	}

	virtual void HandleEvent(VistaEvent *pEvent)
	{
		// update particles
		if(m_dLastT == -1) m_dLastT = pEvent->GetTime();
		const double dt = pEvent->GetTime()-m_dLastT;
		m_dLastT = pEvent->GetTime();
		
		
		float *pos = m_pParticles->GetParticlePositions3fField();
		for(unsigned int i = 0; i < m_vecVelocities.size(); ++i)
		{
			// bounce
			if(pos[i*3+0] > m_fWidth)
			{
				pos[i*3+0] = m_fWidth;
				m_vecVelocities[i][0] *= -1;
			}
			else if(pos[i*3+0] < 0)
			{
				pos[i*3+0] = 0;
				m_vecVelocities[i][0] *= -1;
			}

			if(pos[i*3+2] > m_fDepth)
			{
				pos[i*3+2] = m_fDepth;
				m_vecVelocities[i][2] *= -1;
			}
			else if(pos[i*3+2] < 0)
			{
				pos[i*3+2] = 0;
				m_vecVelocities[i][2] *= -1;
			}

			pos[i*3+0] += (float)dt * m_vecVelocities[i][0];
			//pos[i*3+1] += dt * m_vecVelocities[i][1]; // we know that this is 0...skip
			pos[i*3+2] += (float)dt * m_vecVelocities[i][2];
		}
	}

	VistaOpenSGParticles *GetParticles()
	{
		return m_pParticles;
	}

private:
	float m_fWidth, m_fDepth;
	double m_dLastT;
	VistaOpenSGParticles *m_pParticles;
	std::vector<VistaVector3D> m_vecVelocities;
};

#endif
