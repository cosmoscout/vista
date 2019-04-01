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


//For further example code and how to use OpenSG Particle Manager please see the Vista ParticleDemo!

#include "VistaOpenSGParticleManagerParticleChanger.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaKernelOpenSGExt/VistaOpenSGParticles.h>

#include <VistaTools/VistaRandomNumberGenerator.h>

//Pretty simple signum function.

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

template<typename T>
T signum( T n )
{
	if (n<0)
	{
		return -1;
	}
	else if (n > 0)
	{
		return 1;
	}
	else
		return 0;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/* PMPHYSICALENVIRONMENT                                                      */

VistaVector3D PMPhysicalEnvironment::ApplyPhysics(float *v3VelVec,
								float *v3PosVec,
								VistaType::microtime dDeltaT,
								VistaType::microtime dParticleTime)
{
	//Creating the vectors we're later working with :-)

	/*
	Question is whether we need m_v3Vel/Pos-Vectors each time. the m_v3VelVector is used for StreamingVel and as output, the PosVector only used once so it might be cheaper to not
	use it here. However, I am pretty confident one could use it further .....
	*/
	m_v3VelocitiesVector = VistaVector3D(v3VelVec[0],v3VelVec[1],v3VelVec[2]);
	//m_v3PositionVector = VistaVector3D(v3PosVec[0],v3PosVec[1],v3PosVec[2]);
	m_v3StreamingVelocities = VistaVector3D(0,0,0);
	m_v3Forces = VistaVector3D(0,0,0);

	m_fDensityAtParticle = m_fDensity * std::pow( ( 1 - ( 0.0065f * ( v3PosVec[1] ) / 288.15f ) ), 5.255f );
	//In case m_v3PositionVector makes sense, change to: m_v3PositionVector=m_fDensity*pow((1-(0.0065*(v3PositionVector[1])/288.15)),5.255);

	/*
	
	We are pretty static here. I think we could have some list (or array for the beginning) that contains Force-Objects that calculate forces. Then each run we go through this array
	and add the returned values to our m_v3Forces vector :-)

	*/

	//Add wind speed!
	if (m_bWindsOn)
	{
		m_v3StreamingVelocities = (-1)*m_v3VelocitiesVector + m_v3Wind;
	} 
	else
	{
		m_v3StreamingVelocities = (-1)*m_v3VelocitiesVector;
	}

	//Gravity
	m_v3Forces += VistaVector3D(0,-m_fGravity*m_fObjectMass,0);

	//Standard Drag X, Y, Z
	m_v3Forces += VistaVector3D(CalculateDrag(0),CalculateDrag(1),CalculateDrag(2));

	//Thrust
	m_v3Forces += m_v3Thrust;

	//Static Lift
	if (m_bStaticLiftOn)
	{
		m_v3Forces += VistaVector3D(0,m_fGravity*m_fObjectVolume*(m_fDensityAtParticle-m_fObjectDensity),0);
	}

	//Dynamic Lift
	// Y direction only
	if (m_bDynamicLiftOn)
	{

		float fForcesY;

		fForcesY = 0;
		fForcesY += m_v3LiftCoefficientXYZ[0] * m_fDensityAtParticle * 0.5f * m_v3StreamingVelocities[0] * m_v3StreamingVelocities[0] * m_v3WorkingSurfaceXYZ[0];
		fForcesY += m_v3LiftCoefficientXYZ[2] * m_fDensityAtParticle * 0.5f * m_v3StreamingVelocities[2] * m_v3StreamingVelocities[2] * m_v3WorkingSurfaceXYZ[2];

		if (m_v3Wind[1]>0)
		{
			fForcesY += m_v3LiftCoefficientXYZ[1] * m_fDensityAtParticle * 0.5f * m_v3StreamingVelocities[1] * m_v3StreamingVelocities[1] * m_v3WorkingSurfaceXYZ[1];
		}

		m_v3Forces += VistaVector3D(0,fForcesY,0);
	}


	m_v3VelocitiesVector = m_v3VelocitiesVector + m_v3Forces * (float)dDeltaT / m_fObjectMass;

	/*
	Calculate Forces F for each direction!
	
	To get velocities ... simply do this trick:
	F = m*a <=> F/m = a <=> F/m = dv/dt <=> (F/m) * dt = dv  and
	v = v_0 + dv <=> v = v + (F/m) * (float)dt;

	Hint:

	All forces/winds here (except anything that got to deal with density) are static (even those that deal with density, but we try to influence them by height).

	You could (and maybe SHOULD) make them non-static to somehow have a feeling of reality. This can be solved by function or lambda expressions once you're running on C++0x!
	*/

	return m_v3VelocitiesVector;
}

void PMPhysicalEnvironment::SetConstantWind( const VistaVector3D& v3Wind )
{
	m_v3Wind = v3Wind;
}

void PMPhysicalEnvironment::SetThrust( const VistaVector3D& v3Thrust )
{
	m_v3Thrust = v3Thrust;
}

PMPhysicalEnvironment::PMPhysicalEnvironment( float fObjectMass,
											float fObjectVolume, 
											float fObjectDenisty,
											float fGravity,
											float fDensity,
											const VistaVector3D& v3LiftCoefficient,
											const VistaVector3D& v3DragCoefficientXYZ,
											const VistaVector3D& v3WorkingSurfaceXYZ )
: m_fObjectMass(fObjectMass)
, m_fObjectVolume(fObjectVolume)
, m_fObjectDensity(fObjectDenisty)
, m_fGravity(fGravity)
, m_fDensity(fDensity)
, m_v3LiftCoefficientXYZ(v3LiftCoefficient)
, m_v3DragCoefficientXYZ(v3DragCoefficientXYZ)
, m_v3WorkingSurfaceXYZ(v3WorkingSurfaceXYZ)
, m_bDynamicLiftOn (true )
, m_bStaticLiftOn( true )
, m_bWindsOn( true )
, m_bThrustOn (true )
, m_v3Wind( VistaVector3D(0,0,0) )
, m_v3Thrust( VistaVector3D(0,0,0) )
{
	
}

PMPhysicalEnvironment::PMPhysicalEnvironment( float fObjectMass,
											float fGravity,
											float fDensity,
											const VistaVector3D& v3DragCoefficientXYZ,
											const VistaVector3D& v3WorkingSurfaceXYZ )
: m_fObjectMass( fObjectMass )
, m_fGravity( fGravity )
, m_fDensity( fDensity )
, m_v3DragCoefficientXYZ( v3DragCoefficientXYZ )
, m_v3WorkingSurfaceXYZ( v3WorkingSurfaceXYZ )
, m_bThrustOn( false )
, m_bWindsOn( false )
, m_bStaticLiftOn( false )
, m_bDynamicLiftOn( false )
, m_fObjectDensity( 0 )
, m_fObjectVolume( 0 )
, m_v3LiftCoefficientXYZ( VistaVector3D(0,0,0) )
, m_v3Wind( VistaVector3D(0,0,0) )
, m_v3Thrust( VistaVector3D(0,0,0) )
{	
}

PMPhysicalEnvironment::~PMPhysicalEnvironment()
{

}

float PMPhysicalEnvironment::CalculateDrag( int iDirection )
{
	return ( signum<float>( m_v3StreamingVelocities[iDirection] )
			* m_v3DragCoefficientXYZ[iDirection] 
			* m_fDensityAtParticle * 0.5f * m_v3StreamingVelocities[iDirection] 
			* m_v3StreamingVelocities[iDirection] * m_v3WorkingSurfaceXYZ[iDirection] );
}

void PMPhysicalEnvironment::ChangeWind( int iDirection, float fValue )
{
	m_v3Wind[iDirection] += fValue;
	std::cout << m_v3Wind << std::endl;
}



/* PMPHYSICALFRAMECHANGE                                                     */

void PMPhysicalFrameChange::ChangeParticle( SParticle& oParticle, 
											VistaType::microtime dCurrentTime,
											VistaType::microtime dDeltaT )
{
	if (m_pPhysicEngine == NULL)
	{
		return;
	}

	*oParticle.m_pRemainingLifeTime -= (float)dDeltaT;

	CalculatePositionAndVelocities(oParticle, dDeltaT);
	CalculateColor(oParticle, dDeltaT);
	CalculateSize(oParticle, dDeltaT);
}

void PMPhysicalFrameChange::CalculatePositionAndVelocities( SParticle& oParticle, double dt )
{
	//This little line adds real physics to our particles :-)
	VistaVector3D mReturnVelocities = m_pPhysicEngine->ApplyPhysics(oParticle.m_a3fVelocity, oParticle.m_a3fPosition,dt,0);

	oParticle.m_a3fVelocity[0] = mReturnVelocities[0];
	oParticle.m_a3fVelocity[1] = mReturnVelocities[1];
	oParticle.m_a3fVelocity[2] = mReturnVelocities[2];

	oParticle.m_a3fPosition[0] += (float)dt * oParticle.m_a3fVelocity[0];
	oParticle.m_a3fPosition[1] += (float)dt * oParticle.m_a3fVelocity[1];
	oParticle.m_a3fPosition[2] +=(float) dt * oParticle.m_a3fVelocity[2];
}

void PMPhysicalFrameChange::CalculateColor( SParticle& oParticle, double dt )
{
	oParticle.m_a4fColor[0] += m_fColorFactor * (float)dt;
	oParticle.m_a4fColor[1] += m_fColorFactor * (float)dt;
	oParticle.m_a4fColor[2] += m_fColorFactor * (float)dt;
	oParticle.m_a4fColor[3] = 1.0f;
}

void PMPhysicalFrameChange::CalculateSize( SParticle& oParticle, double dt )
{
	oParticle.m_a3fSize[0] += m_fSizeFactor * (float)dt;
	oParticle.m_a3fSize[1] += m_fSizeFactor * (float)dt;
	oParticle.m_a3fSize[2] += m_fSizeFactor * (float)dt;
}

PMPhysicalFrameChange::PMPhysicalFrameChange( float fColorFactor, 
											float fSizeFactor, 
											float fVelocitiesFactor ) 
: IParticleChanger()
, m_fColorFactor(fColorFactor)
, m_fSizeFactor(fSizeFactor)
, m_fVelocitiesFactor(fVelocitiesFactor)
, m_pPhysicEngine( NULL )
{	
}

void PMPhysicalFrameChange::SetPhysicsEngine( PMPhysicalEnvironment *pPhysics )
{
	m_pPhysicEngine = pPhysics;
}

PMPhysicalFrameChange::~PMPhysicalFrameChange()
{

}


/* PMORIGINSET                                                      */

void PMOriginSet::CalculatePositionAndVelocities( SParticle& oParticle, VistaType::microtime fEventTime )
{
	oParticle.m_a3fVelocity[0] = m_v3ParticleVelocities[0] + (float)m_pRand->GenerateDouble2() * m_fVelocitiesFactor;
	oParticle.m_a3fVelocity[1] = m_v3ParticleVelocities[1] + (float)m_pRand->GenerateDouble2() * m_fVelocitiesFactor;
	oParticle.m_a3fVelocity[2] = m_v3ParticleVelocities[2] + (float)m_pRand->GenerateDouble2() * m_fVelocitiesFactor;

	oParticle.m_a3fPosition[0] = m_v3ParticleOrigin[0] + (float)m_pRand->GenerateDouble2() * m_fOriginFactor;
	oParticle.m_a3fPosition[1] = m_v3ParticleOrigin[1] +(float) m_pRand->GenerateDouble2() * m_fOriginFactor;
	oParticle.m_a3fPosition[2] = m_v3ParticleOrigin[2] +(float) m_pRand->GenerateDouble2() * m_fOriginFactor;
}

void PMOriginSet::CalculateColor( SParticle& oParticle, VistaType::microtime fEventTime )
{
	oParticle.m_a4fColor[0] = m_v3ParticleColor[0] + (float)m_pRand->GenerateDouble2() * m_fColorFactor;
	oParticle.m_a4fColor[1] = m_v3ParticleColor[1] + (float)m_pRand->GenerateDouble2() * m_fColorFactor;
	oParticle.m_a4fColor[2] = m_v3ParticleColor[2] + (float)m_pRand->GenerateDouble2() * m_fColorFactor;
	oParticle.m_a4fColor[3] = 0;
}

void PMOriginSet::CalculateSize( SParticle& oParticle, VistaType::microtime fEventTime )
{
	oParticle.m_a3fSize[0] = m_v3ParticleSize[0] + (float)m_pRand->GenerateDouble2() * m_fSizeFactor;
	oParticle.m_a3fSize[1] = m_v3ParticleSize[1] + (float)m_pRand->GenerateDouble2() * m_fSizeFactor;
	oParticle.m_a3fSize[2] = m_v3ParticleSize[2] + (float)m_pRand->GenerateDouble2()* m_fSizeFactor;
}

PMOriginSet::PMOriginSet( const VistaVector3D& v3ParticleVelocities, 
						float fVelocitiesFactor, 
						const VistaVector3D& v3ParticleOrigin, 
						float fOriginFactor, 
						const VistaVector3D& v3ParticleColor, 
						float fColorFactor,
						float fAlphaFactor, 
						const VistaVector3D& v3ParticleSize,
						float fSizeFactor )
: IParticleChanger(),
m_v3ParticleVelocities(v3ParticleVelocities),
m_fVelocitiesFactor(fVelocitiesFactor),
m_v3ParticleOrigin(v3ParticleOrigin),
m_fOriginFactor(fOriginFactor),
m_v3ParticleColor(v3ParticleColor),
m_fColorFactor(fColorFactor),
m_fAlphaFactor(fAlphaFactor),
m_v3ParticleSize(v3ParticleSize),
m_fSizeFactor(fSizeFactor)
{
	m_fLifetime = 0;
}

void PMOriginSet::ChangeParticle( SParticle& oParticle, VistaType::microtime dCurrentTime, VistaType::microtime dDeltaT )
{
	*oParticle.m_pRemainingLifeTime = (float)m_fLifetime;

	CalculatePositionAndVelocities(oParticle, dCurrentTime);
	CalculateColor(oParticle, dCurrentTime);
	CalculateSize(oParticle, dCurrentTime);
}

PMOriginSet::~PMOriginSet()
{

}


/* PMLINEARFRAMECHANGE                                                      */

void PMLinearFrameChange::ChangeParticle( SParticle& oParticle, VistaType::microtime dCurrentTime, VistaType::microtime dDeltaT )
{
	*oParticle.m_pRemainingLifeTime -= (float)dDeltaT;

	CalculatePositionAndVelocities(oParticle, dDeltaT);
	CalculateColor(oParticle, dDeltaT);
	CalculateSize(oParticle, dDeltaT);
}

void PMLinearFrameChange::CalculatePositionAndVelocities( SParticle& oParticle, double dt )
{
	oParticle.m_a3fPosition[0] += (float)dt * oParticle.m_a3fVelocity[0];
	oParticle.m_a3fPosition[1] += (float)dt * oParticle.m_a3fVelocity[1];
	oParticle.m_a3fPosition[2] += (float)dt * oParticle.m_a3fVelocity[2];

	oParticle.m_a3fVelocity[0] += m_fVelocitiesFactor * (float)dt;
	oParticle.m_a3fVelocity[1] += m_fVelocitiesFactor * (float)dt;
	oParticle.m_a3fVelocity[2] += m_fVelocitiesFactor * (float)dt;
}

void PMLinearFrameChange::CalculateColor( SParticle& oParticle, double dt )
{
	oParticle.m_a4fColor[0] += m_fColorFactor * (float)dt;
	oParticle.m_a4fColor[1] += m_fColorFactor * (float)dt;
	oParticle.m_a4fColor[2] += m_fColorFactor * (float)dt;
	oParticle.m_a4fColor[3] = 1.0f;
	// @TODO: use alpha factor
}

void PMLinearFrameChange::CalculateSize( SParticle& oParticle, double dt )
{
	oParticle.m_a3fSize[0] += m_fSizeFactor * (float)dt;
	oParticle.m_a3fSize[1] += m_fSizeFactor * (float)dt;
	oParticle.m_a3fSize[2] += m_fSizeFactor * (float)dt;
}

PMLinearFrameChange::PMLinearFrameChange( float fColorFactor, float fSizeFactor, float fVelocitiesFactor ) : IParticleChanger(),
m_fColorFactor(fColorFactor),
m_fSizeFactor(fSizeFactor),
m_fVelocitiesFactor(fVelocitiesFactor)
{

}

PMLinearFrameChange::~PMLinearFrameChange()
{

}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
