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


// include header here

#include "VistaDirectXGamepad.h" 

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaUserPlatform.h>
#include <VistaKernel/InteractionManager/VistaDriverWindowAspect.h>
#include <VistaDeviceDrivers/Base/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDrivers/Base/VistaDriverMap.h>

#include <VistaKernel/InteractionManager/VistaUserPlatform.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>


#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSG.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaKernel/VistaSystem.h>
#include <VistaMath/VistaBoundingBox.h>

#include <list>
#include <string>

using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

class CSimpleFly
{
public:
	CSimpleFly(VistaUserPlatform *pPlatform)
		: m_pPlatform(pPlatform)
	{
		
	}
	~CSimpleFly();


	/**
	 * @param vTrans in WCS
	 */
	bool Fly( const VistaVector3D &vTrans )
	{
		VistaVirtualPlatform *pPlatform = m_pPlatform->GetPlatform();
		VistaQuaternion q = pPlatform->GetRotation();
		VistaVector3D tr = q.Rotate(vTrans);
		m_pPlatform->GetPlatform()->SetTranslation(
			m_pPlatform->GetPlatform()->GetTranslation() + tr);
		return true;
	}

	bool SetViewAndUpVector( const VistaVector3D &v3View,
							 const VistaVector3D &vUp )
	{
		return false;
	}

public:

	VistaUserPlatform *m_pPlatform;
};


class CFlyCmd : public IVistaExplicitCallbackInterface
{
public:
	CFlyCmd( CSimpleFly *pFly,
			 const VistaVector3D &trans )
		: m_pFly(pFly),
		  m_v3Trans(trans)
	{
	}

	bool Do()
	{
		m_pFly->Fly(m_v3Trans);
		return true;
	}
private:
	CSimpleFly *m_pFly;
	VistaVector3D m_v3Trans;
};

class CForceEnable : public IVistaExplicitCallbackInterface
{
public:
	CForceEnable( VistaDirectXGamepad *pPad,
				  const VistaVector3D &force)
		: IVistaExplicitCallbackInterface(),
		  m_pPad(pPad),
		  m_v3Force(force),
		  m_bEnabled(false)
	{
		m_pFF = m_pPad->GetDirectXForceFeedbackAspect();
		if(m_pFF)
			m_pEffect = m_pFF->CreateEffect( GUID_ConstantForce );
	}

	bool Do()
	{
		if(!m_pFF)
			return false;

		if(!m_bEnabled)
		{
			m_pFF->SetCurrentEffect(m_pEffect);
			m_bEnabled = m_pFF->SetForce(m_v3Force, VistaQuaternion());
		}
		else
		{
			m_bEnabled = !m_pFF->Stop(m_pEffect);
		}
		return true;
	}

private:
	bool                  m_bEnabled;
	VistaDirectXGamepad *m_pPad;
	VistaVector3D        m_v3Force;
	VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CEffect *m_pEffect;
	VistaDirectXGamepad::VistaDirectXForceFeedbackAspect *m_pFF;
};
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


int main(int argc, char *argv[])
{
	VistaSystem *pSys = new VistaSystem;


	list<string> liPath;
	liPath.push_back("configfiles/");
	pSys->SetIniSearchPaths(liPath);

	pSys->IntroMsg();

	VistaDirectXGamepad *pGamepad = new VistaDirectXGamepad;

	VistaDriverWindowAspect *pAsp = dynamic_cast<VistaDriverWindowAspect*>(pGamepad->GetAspectById( VistaDriverWindowAspect::GetAspectId() ));
	if(!pAsp)
	{
		pAsp = new VistaDriverWindowAspect;
		pGamepad->RegisterAspect( pAsp );
	}



	if(pSys->Init(argc, argv))
	{
		VistaWindow *pWindow = pSys->GetDisplayManager()->GetWindowByName("MONO_WINDOW");
		if(pWindow)
		{


			pAsp->AttachToWindow(pWindow);


			if(pGamepad->Connect())
			{
				pSys->GetDriverMap()->AddDeviceDriver("GAMEPAD", pGamepad);
			}
			else
			{
				std::cerr << "connect failed on GAMEPAD... sad...\n";
			}

			VistaDriverMeasureHistoryAspect *pHist 
				= dynamic_cast<VistaDriverMeasureHistoryAspect*>(pGamepad->GetAspectById( VistaDriverMeasureHistoryAspect::GetAspectId() ));
			if(pHist)
				pHist->SetHistorySize( pGamepad->GetSensorByIndex(0), 10, 5, pGamepad->GetSensorMeasureSize() );

			pSys->GetInteractionManager()->ActivateDeviceDriver( pGamepad );

			VistaUserPlatform *pUserPlatform = new VistaUserPlatform( pSys->GetGraphicsManager()->GetSceneGraph(), 
																		pSys->GetDisplayManager()->GetDisplaySystemByName("MONO") );



			CSimpleFly *pFly = new CSimpleFly( pUserPlatform );
				
			pSys->GetKeyboardSystemControl()->RegisterAction('d',
				new CFlyCmd( pFly, VistaVector3D(0,0,1) ),
				"fly direction 0,0,1" );

			pSys->GetKeyboardSystemControl()->RegisterAction('e',
				new CFlyCmd( pFly, VistaVector3D(0,0,-1) ),
				"fly direction 0,0,-1" );

			pSys->GetKeyboardSystemControl()->RegisterAction('s',
				new CFlyCmd( pFly, VistaVector3D(-1,0,0) ),
				"fly direction -1,0,0" );

			pSys->GetKeyboardSystemControl()->RegisterAction('f',
				new CFlyCmd( pFly, VistaVector3D(1,0,0) ),
				"fly direction 1,0,0" );

			pSys->GetKeyboardSystemControl()->RegisterAction('f',
				new CFlyCmd( pFly, VistaVector3D(1,0,0) ),
				"fly direction 1,0,0" );

			pSys->GetKeyboardSystemControl()->RegisterAction('w',
				new CFlyCmd( pFly, VistaVector3D(0,1,0) ),
				"fly direction 0,1,0" );

			pSys->GetKeyboardSystemControl()->RegisterAction('x',
				new CFlyCmd( pFly, VistaVector3D(0,-1,0) ),
				"fly direction 0,-1,0" );



			pSys->GetKeyboardSystemControl()->RegisterAction('g',
				new CForceEnable( pGamepad, VistaVector3D(-1,0,0) ),
				"Enable/Disable force for -1,0,0" );

			pSys->GetKeyboardSystemControl()->RegisterAction('h',
				new CForceEnable( pGamepad, VistaVector3D(1,0,0) ),
				"Enable/Disable force for 1,0,0" );

		}
		pSys->Run();
	}

	delete pSys;

	return 0;
}




/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/


