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


/*

VistaParticlesDemo is more than just a Demo for using particles in VistaVRToolkit.

On the one hand it'll show you how to use your own particles system the old fashioned way (create them all yourself),
or the better practice way: ParticleManager. This manager will take several tasks from you (e.g. Creating, managing them).
The only things you have to hand over are two objects that tell the ParticleManager what to do with them on creation and
while they are "traveling".
Additionally there's also one example that adds a (more or less exact) physical environment to your particles. It is possible,
that you could use that environment also for other objects. Feel free to implement your own stuff.

For further details see the implementation and comments in there.
*/


#include <VistaKernel/VistaSystem.h>

// needed for interaction
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>


// needed for the scene
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include <VistaKernelOpenSGExt/VistaOpenSGParticles.h>
#include <VistaKernelOpenSGExt/VistaOpenSGSkybox.h>

// needed for the trackball camera movement
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>

// implementation of mouse interaction
//#include <VistaKernel/Stuff/VistaPlatformTrackball.h>

// needed for the VistaEventCone
#include <VistaMath/VistaGeometries.h>

// random numbers
#include <VistaTools/VistaRandomNumberGenerator.h>


// simple methods for AABB handling

#include <VistaMath/VistaBoundingBox.h>

// needed for keyboard interaction
#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaBase/VistaExceptionBase.h>

// needed for particle animation callbacks
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaKernelOpenSGExt/VistaOpenSGParticleManager.h>
#include <VistaKernelOpenSGExt/VistaOpenSGParticleManagerParticleChanger.h>

#include <stdio.h>
#include <cstring>

#include "Smoke.h"
#include "RollingBalls.h"


#ifdef WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class GrindingSparksOriginSet : public VistaOpenSGParticleManager::IParticleChanger
{

public:
	GrindingSparksOriginSet(
		VistaVector3D v3ParticleVelocities,
		float fVelocitiesFactor,
		VistaVector3D v3ParticleOrigin,
		float fRedGlowFactor,
		VistaVector3D v3ParticleSize)
		: IParticleChanger(),
		m_v3ParticleVelocities(v3ParticleVelocities),
		m_fVelocitiesFactor(fVelocitiesFactor),
		m_v3ParticleOrigin(v3ParticleOrigin),
		m_fRedGlowFactor(fRedGlowFactor),
		m_v3ParticleSize(v3ParticleSize)
	{
		m_fLifetime = 0;
	}

	void ChangeParticle(SParticle& oParticle,
		VistaType::microtime dCurrentTime,
		VistaType::microtime dDeltaT  )
	{
		*oParticle.m_pRemainingLifeTime = m_fLifetime;

		CalculatePositionAndVelocities( oParticle, dCurrentTime );
		CalculateColorandSize( oParticle, dCurrentTime );

	}

	void CalculatePositionAndVelocities(SParticle& oParticle, VistaType::microtime fEventTime)
	{

		oParticle.m_a3fVelocity[0] = m_v3ParticleVelocities[0] - (float)(m_pRand->GenerateDouble2()) * m_fVelocitiesFactor;
		oParticle.m_a3fVelocity[1] = m_v3ParticleVelocities[1] - (float)(m_pRand->GenerateDouble2()) * m_fVelocitiesFactor;
		oParticle.m_a3fVelocity[2] = m_v3ParticleVelocities[2] - (float)(m_pRand->GenerateDouble2()) * m_fVelocitiesFactor;

		oParticle.m_a3fPosition[0] = m_v3ParticleOrigin[0];
		oParticle.m_a3fPosition[1] = m_v3ParticleOrigin[1];
		oParticle.m_a3fPosition[2] = m_v3ParticleOrigin[2];

		oParticle.m_a3fSecondPosition[0] = oParticle.m_a3fPosition[0];
		oParticle.m_a3fSecondPosition[1] = oParticle.m_a3fPosition[1];
		oParticle.m_a3fSecondPosition[2] = oParticle.m_a3fPosition[2];

	}

	void CalculateColorandSize(SParticle& oParticle, VistaType::microtime fEventTime)
	{

		oParticle.m_a3fSize[0] = m_v3ParticleSize[0];
		oParticle.m_a3fSize[1] = m_v3ParticleSize[1];
		oParticle.m_a3fSize[2] = m_v3ParticleSize[2];

		oParticle.m_a4fColor[0] = m_v3ParticleSize[2];
		oParticle.m_a4fColor[1] = m_v3ParticleSize[2];
		oParticle.m_a4fColor[2] = m_v3ParticleSize[2];
		oParticle.m_a4fColor[3] = 1.0f;
	}

private:

	VistaVector3D m_v3ParticleVelocities;
	VistaVector3D m_v3ParticleOrigin;
	VistaVector3D m_v3ParticleSize;

	float m_fRedGlowFactor;
	float m_fVelocitiesFactor;
};

class GrindSparksFrameChange : public VistaOpenSGParticleManager::IParticleChanger
{

public:

	GrindSparksFrameChange(float fSizeFactor, float fVelocitiesFactor)
		: IParticleChanger(),
		m_fSizeFactor(fSizeFactor),
		m_fVelocitiesFactor(fVelocitiesFactor)
	{
	}


	void ChangeParticle(SParticle& oParticle,
		VistaType::microtime dCurrentTime,
		VistaType::microtime dDeltaT  )
	{

		*oParticle.m_pRemainingLifeTime -= (float)dDeltaT;

		CalculatePositionAndVelocities(oParticle, dDeltaT);
		CalculateColorandSize(oParticle, dDeltaT);
	}

	void CalculatePositionAndVelocities(SParticle& oParticle, VistaType::microtime dt)
	{

		oParticle.m_a3fVelocity[2] += m_fVelocitiesFactor*(float)dt;

		oParticle.m_a3fPosition[0] += (float)dt*oParticle.m_a3fVelocity[0];
		oParticle.m_a3fPosition[1] += (float)dt*oParticle.m_a3fVelocity[1];
		oParticle.m_a3fPosition[2] += (float)dt*oParticle.m_a3fVelocity[2];

	}

	void CalculateColorandSize(SParticle& oParticle, VistaType::microtime dt)
	{

		oParticle.m_a3fSize[0] = oParticle.m_a3fVelocity[2]*m_fSizeFactor;
		oParticle.m_a3fSize[1] = 0.08f;
		oParticle.m_a3fSize[2] *= (1.0f - (float)dt*7.0f);

		oParticle.m_a4fColor[0] = oParticle.m_a3fSize[2];
		oParticle.m_a4fColor[1] = oParticle.m_a3fSize[2];
		oParticle.m_a4fColor[2] = oParticle.m_a3fSize[2];
		oParticle.m_a4fColor[3] *= (1.0f - (float)dt * 3.0f);
	}


private:

	float m_fSizeFactor;
	float m_fVelocitiesFactor;

};


template <class T>
class IncValue : public IVistaExplicitCallbackInterface
{
public:
	IncValue(T &tVarRef, const T &tIncValue) : m_oRef(tVarRef), m_tVal(tIncValue){}
	virtual ~IncValue(){}
	virtual bool Do()
	{
		m_oRef += m_tVal;
		return true;
	}
private:
	T &m_oRef;
	T  m_tVal;
};


class GrindingSparks : public VistaEventHandler
{

public:

    const int m_iNumberOfParticles;

    GrindingSparks(VistaGroupNode *pParent, VistaSceneGraph *pSG)
		: m_iTexSize(32)
		, m_dSigma(5)
		, m_dM(12)
		, m_iNumberOfParticles(150)
	{
        m_aLifetime = new float[m_iNumberOfParticles];
        m_aActive = new bool [m_iNumberOfParticles];
		m_pParticles = new VistaOpenSGParticles(pSG, pParent);
		m_pParticles->SetColorsWithAlpha();
		m_pParticles->SetNumParticles( m_iNumberOfParticles, true, true);
		m_vecVelocities.resize( m_iNumberOfParticles);

		m_pParticles->BeginEdit();

		//Initializing Particles and setting them to ShaderParticles-Mode (ShaderQuads)!
		m_pParticles->SetMode(VistaOpenSGParticles::PM_DIRECTEDQUADS);
		m_pParticles->SetUseSpriteImage("../../data/spark.png");


		m_pParticles->SetDrawOrder(VistaOpenSGParticles::BackToFront);

		//Initializing all used (class-global) variables!
		pos = m_pParticles->GetParticlePositions3fField();
		secpos = m_pParticles->GetSecParticlePositions3fField();
		siz = m_pParticles->GetParticleSizes3fField();
		col = m_pParticles->GetParticleColors4fField();
		pRand = VistaRandomNumberGenerator::GetStandardRNG();

		// init
		for(int i = 0; i < m_pParticles->GetNumParticles(); ++i)
		{

			resetParticle(i);

		}

		m_EventTime = -1.0f;
		m_pParticles->EndEdit();

	}

	virtual ~GrindingSparks()
	{
        delete [] m_aLifetime;
        delete [] m_aActive;
		delete m_pParticles;
	}

	virtual void HandleEvent(VistaEvent *pEvent)
	{
		if (m_EventTime == -1.0)
			m_EventTime=pEvent->GetTime();
		float dt = (float)(pEvent->GetTime()) - (float)(m_EventTime);
		m_EventTime = pEvent->GetTime();

		m_pParticles->CopyPosToSecpos();

		int idx3;

		for(int i = 0; i < m_pParticles->GetNumParticles(); i++)
		{
			if (m_aLifetime[i] >= 0)
			{
				m_aLifetime[i] -= (float)(dt);
				m_vecVelocities[i][2] += 0.25f * (float)(dt);

				idx3 = 3*i;

				pos[idx3+0] += dt*m_vecVelocities[i][0];
				pos[idx3+1] += dt*m_vecVelocities[i][1];
				pos[idx3+2] += dt*m_vecVelocities[i][2];

				//Size of Particles!
				siz[idx3+0] = m_vecVelocities[i][2]*0.1f;
				siz[idx3+1] = 0.08f;

				//Factor for brightness!
				siz[idx3+2] *= (1.0f - dt * 7.0f);
				//Factor for Colors !!! No negative values!
				col[i*4+0] = siz[idx3+2];
				col[i*4+1] = siz[idx3+2];
				col[i*4+2] = siz[idx3+2];
				//Alpha is seperate
				col[i*4+3] *= (1.0f - dt * 3.0f);
			}
			else
			{
				resetParticle(i);
			}
		}
	}

	//KeyboardControls
	void ChangeMode(int &m_SetToMode)
	{
		if (m_SetToMode == 1)
		{
			m_pParticles->SetMode(VistaOpenSGParticles::PM_VIEWERQUADS);
			m_pParticles->SetUseSpriteImage("../../data/spark.png");
		}
		else if (m_SetToMode == 2)
		{
			m_pParticles->SetMode(VistaOpenSGParticles::PM_DIRECTEDQUADS);
			m_pParticles->SetUseSpriteImage("../../data/spark.png");
		}
		else if (m_SetToMode == 3)
		{

		}

	}

	VistaOpenSGParticles *GetParticles(){return m_pParticles;}

	void RegisterKeyboardControls(VistaKeyboardSystemControl *pCtrl)
	{
		int eins = 1;
		int zwei = 2;

		pCtrl->BindAction( '1', new SpriteParamChanger<int>(this, eins,0), "Normal Billboarding");
		pCtrl->BindAction( '2', new SpriteParamChanger<int>(this, zwei,0), "Special Particles");
	}

private:

	template <class T>
	class SpriteParamChanger : public IncValue<T>
	{
	public:
		SpriteParamChanger(GrindingSparks *pObj, T &tVarRef, const T &tIncValue)
			: IncValue<T>(tVarRef, tIncValue), m_pObj(pObj), ChangeVal(tVarRef){}

		virtual bool Do()
		{
			m_pObj->ChangeMode(ChangeVal);
			return true;
		}

	private:
		GrindingSparks *m_pObj;
		T ChangeVal;
	};

	void resetParticle(int particle)
	{
		m_aLifetime[particle] = (float)(pRand->GenerateInt32(0,2500))/1000.0f;

		m_vecVelocities[particle][2] = -1.3f-(float)(pRand->GenerateDouble2())*0.1f;
		m_vecVelocities[particle][1] = -(float)(pRand->GenerateDouble2())*0.1f;
		m_vecVelocities[particle][0] = 0.05f-0.1f*(float)(pRand->GenerateDouble2());

		int idx3 = 3*particle;

		pos[idx3+0] = 0.0f;
		pos[idx3+1] = 0.0f;
		pos[idx3+2] = 0.0f;

		secpos[idx3+0] = pos[idx3+0] ;
		secpos[idx3+1] = pos[idx3+1] ;
		secpos[idx3+2] = pos[idx3+2] ;

		//Size of Particles!
		siz[idx3+0] = 0.01f;
		siz[idx3+1] = 0.01f;

		//Factor of Redglow!
		siz[idx3+2] = 0.7f;

		//Alpha Value
		col[particle*4+0] = siz[idx3+2];
		col[particle*4+1] = siz[idx3+2];
		col[particle*4+2] = siz[idx3+2];
		col[particle*4+3] = 1.0f;

	}

	VistaOpenSGParticles *m_pParticles;
	int    m_iTexSize;
	double m_dSigma;
	double m_dM;
	std::vector<VistaVector3D> m_vecVelocities;

	float* m_aLifetime;
	bool* m_aActive ;
	double m_EventTime;

	//For performance issues we rather get some RAM instead of fetching our arrays each time for each particle ...
	float *pos,*secpos,*col,*siz;
	VistaRandomNumberGenerator *pRand;
};



class LightSaber : public VistaEventHandler
{
public:
	LightSaber(VistaGroupNode *pParent, VistaSceneGraph *pSG)
		: m_iTexSize(32)
		, m_dSigma(5)
		, m_dM(12)
	{
		VistaRandomNumberGenerator *pRand = VistaRandomNumberGenerator::GetStandardRNG();
		m_pParticles = new VistaOpenSGParticles(pSG, pParent);
		m_pParticles->SetNumParticles(500, true, true);
		m_pParticles->BeginEdit();
		float *pos = m_pParticles->GetParticlePositions3fField();
		float *col = m_pParticles->GetParticleColors3fField();
		float *siz = m_pParticles->GetParticleSizes3fField();

		// init
		for(int i = 0; i < m_pParticles->GetNumParticles(); ++i)
		{
			const int idx3 = i*3;

			// positions
			pos[idx3+0] = ((float)(pRand->GenerateDouble3())-0.5f) * 0.01f;
			pos[idx3+1] = 0;
			pos[idx3+2] = (float)(pRand->GenerateDouble3()) * 1.2f;

			// colors (blue)
			col[idx3+0] = (float)(pRand->GenerateDouble3()) * 0.1f;
			col[idx3+1] = (float)(pRand->GenerateDouble3()) * 0.075f;
			col[idx3+2] = 0.1f + (float)(pRand->GenerateDouble3()) * 0.6f;

			// sizes
			siz[idx3+0] = 0.08f - 0.2f*(float)(pRand->GenerateDouble3());
			siz[idx3+1] = siz[idx3+2] = siz[idx3+0];

		}




		m_pParticles->EndEdit();
		m_pParticles->SetUseGaussBlobTexture(m_iTexSize, (float)(m_dSigma), (float)(m_dM));
		m_pParticles->SetGLTexEnvMode(GL_MODULATE);
		m_pParticles->SetGLBlendSrcFactor(GL_SRC_ALPHA);
		m_pParticles->SetGLBlendDestFactor(GL_ONE);

	}

	virtual ~LightSaber()
	{
		delete m_pParticles;
	}

	virtual void HandleEvent(VistaEvent *pEvent)
	{
		float *pos = m_pParticles->GetParticlePositions3fField();
		VistaRandomNumberGenerator *pRand = VistaRandomNumberGenerator::GetStandardRNG();
		for(int i = 0; i < m_pParticles->GetNumParticles() * 3; i += 3)
			pos[i+2] = (float)(pRand->GenerateDouble3()) * 1.2f;
	}

	void ApplySpriteParams()
	{
		printf("sprite parameters: res: %d sigma: %f m: %f\n", m_iTexSize, m_dSigma, m_dM);
		m_pParticles->SetUseGaussBlobTexture(m_iTexSize, (float)(m_dSigma), (float)(m_dM));
		m_pParticles->SetGLTexEnvMode(GL_MODULATE);
		m_pParticles->SetGLBlendSrcFactor(GL_SRC_ALPHA);
		m_pParticles->SetGLBlendDestFactor(GL_ONE);
	}

	void RegisterKeyboardControls(VistaKeyboardSystemControl *pCtrl)
	{
		pCtrl->BindAction( '3', new SpriteParamChanger<double>( this, m_dSigma,  0.25 ), "increase sigma");
		pCtrl->BindAction( '4', new SpriteParamChanger<double>( this, m_dSigma, -0.25 ), "decrease sigma");
		pCtrl->BindAction( '5', new SpriteParamChanger<double>( this, m_dM,  1.0 ), "increase m");
		pCtrl->BindAction( '6', new SpriteParamChanger<double>( this, m_dM, -1.0 ), "decrease m");
	}

	VistaOpenSGParticles *GetParticles()
	{
		return m_pParticles;
	}

private:

	template <class T>
	class SpriteParamChanger : public IncValue<T>
	{
	public:
		SpriteParamChanger(LightSaber *pObj, T &tVarRef, const T &tIncValue)
			: IncValue<T>(tVarRef, tIncValue), m_pObj(pObj){}

		virtual bool Do()
		{
			bool rv = IncValue<T>::Do();
			m_pObj->ApplySpriteParams();
			return rv;
		}

	private:
		LightSaber *m_pObj;
	};

	VistaOpenSGParticles *m_pParticles;
	int    m_iTexSize;
	double m_dSigma;
	double m_dM;
};

void CreateScene(VistaSystem *pSystem)
{
	/**
	* @bug segfault when using skybox
	*/

	// create a skybox
	VistaOpenSGSkybox *pSkybox = new VistaOpenSGSkybox(
		"../../data/top.jpg",
		"../../data/bottom.jpg",
		"../../data/left.jpg",
		"../../data/right.jpg",
		"../../data/front.jpg",
		"../../data/back.jpg"
		);

	for(
		std::map<std::string, VistaViewport*>::const_iterator iter
		= pSystem->GetDisplayManager()->GetViewportsConstRef().begin();
	iter != pSystem->GetDisplayManager()->GetViewportsConstRef().end();
	++iter
		)
	{
		pSkybox->AttachToViewport(iter->second);
	}

	// we need access to the scene, which
	// we get from the graphics manager
	VistaGraphicsManager *pGraphicsManager = pSystem->GetGraphicsManager();

	// claim sg
	VistaSceneGraph *pSG = pGraphicsManager->GetSceneGraph();

	// claim root, we want to attach then in world space
	VistaGroupNode *pRoot = pSG->GetRoot();

	// the user is standing in 0,0,2, looking dir 0,0,-1
	// so we start with the scene a bit away at 0,0,-1
	VistaTransformNode *pTransformNode = pSG->NewTransformNode(pRoot);
	pTransformNode->SetTranslation(0, 0, -1);

	//Old-fashioned way for creating ParticleSystems commented out pelow:
	//RollingBalls *pCallMe1 = new RollingBalls(pTrans, pSG);

	//New Way. See RollingBalls.h for further detail. See further below for more detail on this matter.
	VistaTransformNode *pRollingBallsNode = pSG->NewTransformNode(pRoot);
	VistaOpenSGParticleManager *mRollingBallsManager = new VistaOpenSGParticleManager(pSG, pRollingBallsNode);
	mRollingBallsManager->SetParticlesViewMode(VistaOpenSGParticles::PM_VIEWERQUADS);
	mRollingBallsManager->SetParticlesSpriteImage("../../data/ball.png");
	mRollingBallsManager->SetOriginObject(new RollingBallsOriginSet(VistaVector3D(10,0,10),VistaVector3D(4,0,10),VistaVector3D(0.25,0.25,0.25)));
	mRollingBallsManager->SetChangeObject(new RollingBallsFrameChange(VistaVector3D(4,0,10)));
	//We have a system with never dying particles. We hand over -1 for PPS and PL so the
	//manager knows to use MaxParticles all the time!
	mRollingBallsManager->SetParticlesPerSecond(-1);
	mRollingBallsManager->SetParticlesLifetime(-1);
	mRollingBallsManager->SetMaximumParticles(50);
	pSystem->GetEventManager()->AddEventHandler(mRollingBallsManager,VistaSystemEvent::GetTypeId(),VistaSystemEvent::VSE_PREGRAPHICS);

	LightSaber   *poLightSaber = new LightSaber(pTransformNode, pSG);
	//Old fashioned way again commented out:
	//GrindingSparks   *pCallMe3 = new GrindingSparks(pTrans, pSG);
	
	VistaTransformNode *pGrindingSparksNode = pSG->NewTransformNode(pRoot);
	VistaOpenSGParticleManager *mGrindingSparksManager = new VistaOpenSGParticleManager(pSG, pGrindingSparksNode);
	mGrindingSparksManager->SetParticlesViewMode(VistaOpenSGParticles::PM_DIRECTEDQUADS);
	mGrindingSparksManager->SetParticlesSpriteImage("../../data/spark.png");
	mGrindingSparksManager->SetOriginObject(new GrindingSparksOriginSet(VistaVector3D(0.05f,0,-1.3f),0.1f,VistaVector3D(0,0,-1),0.7f,VistaVector3D(0.01f,0.01f,0.7f)));
	mGrindingSparksManager->SetChangeObject(new GrindSparksFrameChange(0.1f,0.25f));
	mGrindingSparksManager->SetMaximumParticles(151);
	mGrindingSparksManager->SetParticlesPerSecond(50);
	mGrindingSparksManager->SetParticlesLifetime(3);
	pSystem->GetEventManager()->AddEventHandler(mGrindingSparksManager,VistaSystemEvent::GetTypeId(),VistaSystemEvent::VSE_PREGRAPHICS);

	VistaTransformNode *pTransSmoke = pSG->NewTransformNode(pRoot);
	pTransSmoke->SetTranslation(0, 0, -1.0125f);

	VistaGeometryFactory pFactory( pSG );
	VistaGeometry* pCube = pFactory.CreateBox(0.05f,0.05f,0.05f,1,1,1,VistaColor(255,255,0));
	VistaTransformNode *pTransCone = pSG->NewTransformNode(pSG->GetRoot());
	pSG->NewGeomNode(pTransCone, pCube);
	pTransCone->Translate(VistaVector3D(0,0,-1.025f));
	Smoke *poSmoke = new Smoke(pTransSmoke, pSG);

	VistaGeometry* pSwimmingpool = pFactory.CreateBox(1.0f,1.0f,0.1f,1,1,1,VistaColor::WHITE);
	VistaGeometry* pWater = pFactory.CreateBox(0.8f,0.8f,0.11f,1,1,1,VistaColor::BLUE);

	VistaTransformNode* pSwimmingpoolNode = pSG->NewTransformNode(pSG->GetRoot());
	pSG->NewGeomNode(pSwimmingpoolNode,pSwimmingpool);
	pSG->NewGeomNode(pSwimmingpoolNode,pWater);

	pSwimmingpoolNode->Rotate( VistaQuaternion( VistaEulerAngles(-1.5, 0, 0 ) ) );
	pSwimmingpoolNode->Translate(VistaVector3D(0,-1.5,-1));


	/************************************************************************/
	/*                                                                      */
	/*            Sample implementation for ParticleManager                 */
	/*                                                                      */
	/************************************************************************/

	//Creating the TransformNode for the Particles
	VistaTransformNode *pVistaParticleManagerNode = pSG->NewTransformNode(pRoot);
	//Creates the ParticleManager which will create the Particles on this TransformNode.
	VistaOpenSGParticleManager *mVistaParticleManager = new VistaOpenSGParticleManager(pSG, pVistaParticleManagerNode);
	//Set the ViewMode. See the OpenSGParticles for reference for the two supported ViewModes. ATTENTION: This needs to be set BEFORE setting the SpriteImage!
	//@TODO Alternatively reset the SpriteImage each time the ViewMode gets changed!
	mVistaParticleManager->SetParticlesViewMode(VistaOpenSGParticles::PM_VIEWERQUADS);
	//Set the SpriteImage
	mVistaParticleManager->SetParticlesSpriteImage("../../data/lemming.png");
	
	//VistaparticleManagerOriginSet and VistaParticleManagerLinearPerFrameChange are inherited from VistaParticleManagerParticle::IParticleChanger. You can create your own!
	//Creating the Object that sets the Particles at their Origin (at the very beginning, as well as after lifetime < 0). See OpenSGParticleManager.h for reference!
	mVistaParticleManager->SetOriginObject(new PMOriginSet(VistaVector3D(0,-0.4f,0),0.0,VistaVector3D(0,0,-1.025f),0.0,VistaVector3D(0,0,0),1.0f,1.0f,VistaVector3D(0.1f,0.1f,0.1f),0.0));
	//Creating the Object that will change Particle related values each frame (or whenever the HandleEvent/UpdateParticles method gets called!).
	mVistaParticleManager->SetChangeObject(new PMLinearFrameChange(0.0,0.0,0.0));
	//Defines the maximum Number of Particles. This value is used to determine the Lifetime of the Particles.
	//When calling this function, all Particles will be reset!
	mVistaParticleManager->SetMaximumParticles(12);
	//This function will calculate the Lifetime of the Particles (maxNumberofParticles/ParticlesPerSecond = Lifetime).
	//Attention: This doesn't mean you'll get 10 particles every second ... but you'll get ParticlesperSecond * timedelay Particles per timedelay ;)
	//You can change PartcilesPerSecond anytime during the run. However, the new Lifetime will only be available to NEW Particles!
	mVistaParticleManager->SetParticlesPerSecond(1);
	mVistaParticleManager->SetParticlesLifetime(3.7f);
	//This kind of sets the ParticleManager active. To start the particle system, StartParticleManager();!
	pSystem->GetEventManager()->AddEventHandler(mVistaParticleManager,VistaSystemEvent::GetTypeId(),VistaSystemEvent::VSE_PREGRAPHICS);
	//You now can still change the Particles, Stop the Manager, Continue or Reload the Manager. You can change number of Particles, too.

	/************************************************************************/
	/*                                                                      */
	/*            Sample implementation for PMPhysics                       */
	/*                                                                      */
	/************************************************************************/


	VistaTransformNode *pPMPhysicsNode = pSG->NewTransformNode(pRoot);
	VistaOpenSGParticleManager *mPMPhysicsManager = new VistaOpenSGParticleManager( pSG, pPMPhysicsNode );
	mPMPhysicsManager->SetParticlesViewMode(VistaOpenSGParticles::PM_VIEWERQUADS);
	mPMPhysicsManager->SetParticlesSpriteImage("../../data/lemming.png");

	//This is the Mega-Lemmings-Mode that includes static/dynamic lift, air drag, gravity, thrust, winds
	PMPhysicalEnvironment *PMPhysEnv = new PMPhysicalEnvironment(0.5f,0,0.18f,9.81f,1.23f,VistaVector3D(0,3.0f,0),VistaVector3D(0.5f,2.0f,0.5f),VistaVector3D(0.05f,0.1f,0.05f));
	//This is the low power mode that only includes gravity and air drag.
	//PMPhysicalEnvironment *PMPhysEnv = new PMPhysicalEnvironment(0.5f,9.81f,1.23f,VistaVector3D(0.5f,2.0f,0.5f),VistaVector3D(0.05f,0.1f,0.05f));
	PMPhysEnv->SetConstantWind(VistaVector3D(0,2.8f,0));
	PMPhysEnv->SetThrust(VistaVector3D(0,0,0));

	PMOriginSet *VMPhysOriginSet = new PMOriginSet(VistaVector3D(0,0,0),0,VistaVector3D(0,0,-1.025f),0,VistaVector3D(0,0,0),0,1.0,VistaVector3D(0.1f,0.1f,0.1f),0.0);
	PMPhysicalFrameChange *VMPPhysFrameChange = new PMPhysicalFrameChange(0,0,0);
	VMPPhysFrameChange->SetPhysicsEngine(PMPhysEnv);

	mPMPhysicsManager->SetOriginObject(VMPhysOriginSet);
	mPMPhysicsManager->SetChangeObject(VMPPhysFrameChange);
	mPMPhysicsManager->SetMaximumParticles(12);
	mPMPhysicsManager->SetParticlesPerSecond(2);
	mPMPhysicsManager->SetParticlesLifetime(4);
	pSystem->GetEventManager()->AddEventHandler(mPMPhysicsManager,VistaSystemEvent::GetTypeId(),VistaSystemEvent::VSE_PREGRAPHICS);

	pSystem->GetEventManager()->AddEventHandler(poLightSaber,
		VistaSystemEvent::GetTypeId(),
		VistaSystemEvent::VSE_PREGRAPHICS);
	pSystem->GetEventManager()->AddEventHandler(poSmoke,
		VistaSystemEvent::GetTypeId(),
		VistaSystemEvent::VSE_PREGRAPHICS);

	poLightSaber->RegisterKeyboardControls(pSystem->GetKeyboardSystemControl());

	//Now start the ParticleManagers!
	mVistaParticleManager->StartParticleManager();
	mRollingBallsManager->StartParticleManager();
	mGrindingSparksManager->StartParticleManager();
	mPMPhysicsManager->StartParticleManager();
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
int main(int argc, char **argv)
{
	try
	{
		VistaSystem vistaSystem;

		std::list<std::string> liSearchPaths;
		liSearchPaths.push_back("../../configfiles/");
		vistaSystem.SetIniSearchPaths(liSearchPaths);

		if(vistaSystem.Init(argc, argv))
		{
			// create the scene
			CreateScene(&vistaSystem);

			if (vistaSystem.GetDisplayManager()->GetDisplaySystem(0)==0)
				VISTA_THROW("No DisplaySystem found",1);

			vistaSystem.GetDisplayManager()->GetWindowByName("MAIN_WINDOW")->GetWindowProperties()->SetTitle(argv[0]);

			// done, run
			vistaSystem.Run();
		}
	}
	catch ( VistaExceptionBase &e )
	{
		e.PrintException();
	}
	return 0;
}


