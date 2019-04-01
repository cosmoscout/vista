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


#ifndef _VISTAOPENSGPARTICLEMANAGERPARTICLECHANGER_H__
#define _VISTAOPENSGPARTICLEMANAGERPARTICLECHANGER_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaKernelOpenSGExt/VistaOpenSGParticleManager.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**PMPhysicalEnvironment
 * 
 * This is supposed to add (pseudo) physical correct behavior to our Particles.
 * This is no attempt to include anything and everything that happens in 
 * real world, too. However, it'll include most important forces and stuff.
 * Below I'll list what's included so far:
 * - Gravity (Y) [Force]
 * - Air drag (X,Y,Z) [Force]
 * - Dynamic Lift (Y) [Force]
 * - Static Lift (Y) [Force]
 * - Winds (change streaming velocities) [m/s]
 * - (net) Thrust [Force]
 * Please calculate Coefficient by this rule:
 * LiftCoefficient = 2*(Lift force)/(density*(fluid velocity)^2 * working_surface) [dimensionless]
 * DragCoefficient = LiftCoefficient. Just take (Drag force) instead of (Lift force) ...!!
*/
class VISTAKERNELOPENSGEXTAPI PMPhysicalEnvironment
{
public:
	PMPhysicalEnvironment( float fObjectMass,
							float fObjectVolume,
							float fObjectDenisty,
							float fGravity,
							float fDensity,
							const VistaVector3D& v3LiftCoefficient,
							const VistaVector3D& v3DragCoefficientXYZ,
							const VistaVector3D& v3WorkingSurfaceXYZ );

	PMPhysicalEnvironment( float fObjectMass,
							float fGravity,
							float fDensity,
							const VistaVector3D& v3DragCoefficientXYZ,
							const VistaVector3D& v3WorkingSurfaceXYZ);
	virtual ~PMPhysicalEnvironment();

	//Applies Physics to the object calling this function! Currently only used for particles, but other objects might profit, too!
	VistaVector3D ApplyPhysics(float *v3VelocitiesVector,
								float *v3PositionVector,
								VistaType::microtime dDeltaT,
								VistaType::microtime dParticleTime);

	void SetConstantWind( const VistaVector3D& v3Wind );
	void SetThrust( const VistaVector3D& v3Thrust );

protected:	
	float CalculateDrag(int direction);
	void ChangeWind( int direction, float fvalue );
protected:

	float m_fObjectMass;
	float m_fGravity;
	float m_fDensity;
	float m_fDensityAtParticle;

	float m_fObjectVolume;
	float m_fObjectDensity;

	VistaVector3D m_v3VelocitiesVector;
	VistaVector3D m_v3LiftCoefficientXYZ;
	VistaVector3D m_v3WorkingSurfaceXYZ;
	VistaVector3D m_v3DragCoefficientXYZ;

	VistaVector3D m_v3Wind;
	VistaVector3D m_v3Thrust;
	VistaVector3D m_v3StreamingVelocities;
	VistaVector3D m_v3Forces;
	VistaVector3D m_v3PositionVector;

	//With these flags you can have your physics as complex as you want ;)
	bool m_bDynamicLiftOn;
	bool m_bStaticLiftOn;
	bool m_bWindsOn;
	bool m_bThrustOn;
};

//This ParticleChanger is very similiar to the rest. However, it uses a
//PhyicalEnvironment to calculate Velocities (and this position) for the next frame.
class VISTAKERNELOPENSGEXTAPI PMPhysicalFrameChange : public VistaOpenSGParticleManager::IParticleChanger
{
public:
	PMPhysicalFrameChange( float fColorFactor, float fSizeFactor, float fVelocitiesFactor );
	virtual ~PMPhysicalFrameChange();

	void SetPhysicsEngine( PMPhysicalEnvironment *pPhysics );

	void ChangeParticle( SParticle& oParticle,
						VistaType::microtime dCurrentTime,
						VistaType::microtime dDeltaT );
	void CalculatePositionAndVelocities(SParticle& oParticle, double dt);
	void CalculateColor(SParticle& oParticle, double dt);
	void CalculateSize(SParticle& oParticle, double dt);

private:
	float m_fColorFactor;
	float m_fSizeFactor;
	float m_fVelocitiesFactor;

	PMPhysicalEnvironment *m_pPhysicEngine;
};

//This PMOriginSet creates particles at their origin. Particles that got deleted or newly created
//will receive information (color, position, origin, size ...) from this object.
class VISTAKERNELOPENSGEXTAPI PMOriginSet : public VistaOpenSGParticleManager::IParticleChanger
{
public:
	PMOriginSet( const VistaVector3D& v3ParticleVelocities,
						float fVelocitiesFactor,
						const VistaVector3D& v3ParticleOrigin,
						float fOriginFactor,
						const VistaVector3D& v3ParticleColor,
						float fColorFactor,
						float fAlphaFactor,
						const VistaVector3D& v3ParticleSize,
						float fSizeFactor );
	virtual ~PMOriginSet();

	void ChangeParticle( SParticle& oParticle,
						VistaType::microtime dCurrentTime,
						VistaType::microtime dDeltaT );
	void CalculatePositionAndVelocities(SParticle& oParticle, VistaType::microtime fEventTime);
	void CalculateColor(SParticle& oParticle, VistaType::microtime fEventTime);
	void CalculateSize(SParticle& oParticle, VistaType::microtime fEventTime);

private:
	VistaVector3D m_v3ParticleVelocities;
	float m_fVelocitiesFactor;
	VistaVector3D m_v3ParticleOrigin;
	float m_fOriginFactor;
	VistaVector3D m_v3ParticleColor;
	float m_fColorFactor;
	float m_fAlphaFactor;
	VistaVector3D m_v3ParticleSize;
	float m_fSizeFactor;
};

//This FrameChanger will change Position/Velocities,Color and Size each frame according to the
//given factor.
class VISTAKERNELOPENSGEXTAPI PMLinearFrameChange : public VistaOpenSGParticleManager::IParticleChanger
{
public:
	PMLinearFrameChange(float fColorFactor, float fSizeFactor, float fVelocitiesFactor);
	virtual ~PMLinearFrameChange();

	void ChangeParticle( SParticle& oParticle,
						VistaType::microtime dCurrentTime,
						VistaType::microtime dDeltaT );
	void CalculatePositionAndVelocities(SParticle& oParticle, double dt);
	void CalculateColor(SParticle& oParticle, double dt);
	void CalculateSize(SParticle& oParticle, double dt);

private:
	float m_fColorFactor;
	float m_fSizeFactor;
	float m_fVelocitiesFactor;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
