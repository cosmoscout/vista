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


#include "VistaFrameLoop.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaFrameLoop::DisplayUpdateCallback : public IVistaExplicitCallbackInterface
{
public:
	DisplayUpdateCallback( VistaFrameLoop* pLoop )
	: IVistaExplicitCallbackInterface()
	, m_pLoop( pLoop )
	{
	}

	~DisplayUpdateCallback()
	{
	}

	virtual bool Do()
	{
		m_pLoop->FrameUpdate();
		return true;
	}

private:
	VistaFrameLoop* m_pLoop;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaFrameLoop::VistaFrameLoop()
: m_pFrameRate( NULL )
, m_pAvgLoopTime( NULL )
, m_iFrameCount( -1 )
, m_pUpdateCallback( NULL )
, m_pSystemEvent( NULL )
{
}

VistaFrameLoop::~VistaFrameLoop()
{
	delete m_pUpdateCallback;
	delete m_pSystemEvent;
	delete m_pFrameRate;
	delete m_pAvgLoopTime;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaFrameLoop::Init( VistaSystem* pVistaSystem )
{
	m_pDisplayManager = pVistaSystem->GetDisplayManager();
	m_pEventManager = pVistaSystem->GetEventManager();
	m_pClusterMode = pVistaSystem->GetClusterMode();

	m_pEventManager->SetResetEventTimeToLocalTime( m_pClusterMode->GetIsLeader() );

	m_pUpdateCallback = new DisplayUpdateCallback( this );
	m_pDisplayManager->SetDisplayUpdateCallback( m_pUpdateCallback );

	m_pSystemEvent = new VistaSystemEvent;

	m_pFrameRate = new VistaWeightedAverageTimer;
	m_pAvgLoopTime = new VistaWeightedAverageTimer;

	return true;
}

void VistaFrameLoop::Run()
{
	m_pDisplayManager->GetWindowingToolkit()->Run();

	m_pSystemEvent->SetId( VistaSystemEvent::VSE_EXIT );
	m_pEventManager->ProcessEvent( m_pSystemEvent );
}


void VistaFrameLoop::Quit()
{
	m_pDisplayManager->GetWindowingToolkit()->Quit();
}

void VistaFrameLoop::FrameUpdate()
{
	if( m_iFrameCount >= 0 )
		VistaKernelProfileStopNamedSection( "WINDOWTOOLKIT_LOOP" );
	
	++m_iFrameCount;
	VistaKernelProfileNewFrame();

	m_pClusterMode->StartFrame();

	if( m_pClusterMode->GetIsLeader() )
	{
		// now on with the next frame!
		EmitSystemEvent( VistaSystemEvent::VSE_POSTGRAPHICS );
		EmitSystemEvent( VistaSystemEvent::VSE_PREAPPLICATIONLOOP );
		EmitSystemEvent( VistaSystemEvent::VSE_UPDATE_INTERACTION );
		//@todo: not implemented, but in theory, displays should be updated right before draw -> move behind DELAYED_INTERACTION?
		EmitSystemEvent( VistaSystemEvent::VSE_UPDATE_DISPLAYS );
		EmitSystemEvent( VistaSystemEvent::VSE_POSTAPPLICATIONLOOP );
		EmitSystemEvent( VistaSystemEvent::VSE_UPDATE_DELAYED_INTERACTION );
		EmitSystemEvent( VistaSystemEvent::VSE_PREGRAPHICS );		
	}
	
	m_pClusterMode->ProcessFrame();
	
	m_pClusterMode->EndFrame();

	m_pAvgLoopTime->RecordTime();

	{
		VistaKernelProfileScope( "RENDER" );
	

		m_pDisplayManager->DrawFrame();

		m_pClusterMode->SwapSync();

		m_pDisplayManager->DisplayFrame();

		m_pFrameRate->RecordTime();
	}

	VistaKernelProfileStartSection( "WINDOWTOOLKIT_LOOP" );
}

float VistaFrameLoop::GetFrameRate()
{
	VistaType::microtime nAverage = m_pFrameRate->GetAverageTime();
	if( nAverage == 0 )
		return 0;
	return ( 1.0f / (float)nAverage );
}

int VistaFrameLoop::GetFrameCount()
{
	return m_iFrameCount;
}

VistaType::microtime VistaFrameLoop::GetAverageLoopTime()
{
	return m_pAvgLoopTime->GetAverageTime();
}

void VistaFrameLoop::EmitSystemEvent( const int iSystemEventId )
{
	VistaKernelProfileScope( VistaSystemEvent::GetIdString( iSystemEventId ) );
	m_pSystemEvent->SetId( iSystemEventId );
	m_pEventManager->ProcessEvent( m_pSystemEvent );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


