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


/**
 * Description:
 * This is the ClusterModeDemo, which explains how to set up the VistaClusterMode,
 * how it works, and what to take care of to keep nodes synchronized.
 */

#include "ClusterModeDemo.h"

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include "VistaBase/VistaTimeUtils.h"
#include "VistaBase/VistaUtilityMacros.h"

#include "VistaKernel/VistaSystem.h"

#include "VistaInterProcComm/Concurrency/VistaThread.h"
#include "VistaInterProcComm/Concurrency/VistaMutex.h"
#include "VistaInterProcComm/Cluster/VistaClusterBarrier.h"
#include "VistaInterProcComm/Cluster/VistaClusterDataSync.h"
#include "VistaInterProcComm/Connections/VistaMsg.h"

#include "VistaTools/VistaRandomNumberGenerator.h"

#include "VistaKernel/GraphicsManager/VistaTransformNode.h"
#include "VistaKernel/GraphicsManager/VistaSceneGraph.h"
#include "VistaKernel/GraphicsManager/VistaGraphicsManager.h"
#include "VistaKernel/EventManager/VistaEventManager.h"
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>
#include "VistaKernel/DisplayManager/VistaDisplayManager.h"
#include "VistaKernel/EventManager/VistaSystemEvent.h"
#include "VistaKernel/DisplayManager/VistaSimpleTextOverlay.h"
#include "VistaKernel/DisplayManager/VistaTextEntity.h"
#include "VistaKernel/Cluster/VistaClusterMode.h"
#include "VistaKernel/VistaFrameLoop.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * This is a computation thread, that pretends to do some work...
 * It will be used later in this file
 */
class ComputationThread : public VistaThread
{
public:
	ComputationThread()
	: VistaThread()
	, m_bFinishFlag( false )
	{
	}
	virtual void ThreadBody() 
	{
		// we do some random "computations";

		// let's sleep a little - for a random time
		// however, we can't use the StandardRNG, because that would "steal"
		// a number from the main thread's RNG, which can result in different
		// RNGs and diverging clients. Thus, we'll use out thread's own RNG
		// ThreadLocal RNGs always have a default seed set when they are first
		// requested, so they would produce the same values on each run
		// use thread id to get a different seed for each thread
		VistaRandomNumberGenerator::GetThreadLocalRNG()->SetSeed( (unsigned int)GetThreadIdentity() );
		int nSleepTime = VistaRandomNumberGenerator::GetThreadLocalRNG()->GenerateInt32( 2000, 4000 );

		VistaTimeUtils::Sleep( nSleepTime );
		m_sComputedData = "I did't really work, just idled around";

		{
			VistaMutexLock oLock( m_oFinishMutex );
			m_bFinishFlag = true;
		}
	}
	bool GetIsFinished() const
	{
		VistaMutexLock oLock( m_oFinishMutex );
		return m_bFinishFlag;
	}
	std::string GetResult()
	{
		return m_sComputedData;
	}
private:
	mutable VistaMutex m_oFinishMutex;
	bool m_bFinishFlag;
	std::string m_sComputedData;
};

const int s_nComputationExternalMessageTag = 666;

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

ClusterModeDemo::ClusterModeDemo( int argc, char  *argv[] )
: VistaEventHandler()
, m_pVistaSystem( new VistaSystem() )
, m_pComputeThread( NULL )
, m_pDataSync( NULL )
, m_pBarrier( NULL )
, m_pMessage( NULL )
, m_pComputeFinishedEvent( NULL )
, m_pTextOverlay(0)
{
	// ViSTA's standard intro message
	m_pVistaSystem->IntroMsg();

	// init vista as usual
	if( m_pVistaSystem->Init( argc, argv ) == false )
	{
		std::cerr << "Initialization FAILED - aborting!" << std::endl;
		return;
	}
	if ( m_pVistaSystem->GetDisplayManager()->GetDisplaySystem() == NULL )
		VISTA_THROW( "No DisplaySystem found" , 1 );

	// We register to receive system events
	m_pVistaSystem->GetEventManager()->AddEventHandler( this, 
														VistaSystemEvent::GetTypeId(),
														VistaSystemEvent::VSE_POSTAPPLICATIONLOOP );

	// laod something to fill the scenery
	VistaSceneGraph* pSG = m_pVistaSystem->GetGraphicsManager()->GetSceneGraph();
	IVistaNode* pCow = pSG->LoadNode( "../data/cow.obj" );
	VistaTransformNode* pCowTrans = pSG->NewTransformNode( pSG->GetRoot() );
	pCowTrans->Translate( 0, 0, -5 );
	pCowTrans->AddChild( pCow );

	// we add a text overlay to indicate which window belongs to which node
	// for this, we get info from the cluster mode
	VistaClusterMode* pCluster = m_pVistaSystem->GetClusterMode();
	m_pTextOverlay = new VistaSimpleTextOverlay( m_pVistaSystem->GetDisplayManager() );
	AddTextToOverlay( "Cluster Mode Type  : " + pCluster->GetClusterModeName() );
	AddTextToOverlay( "Cluster Node       : " + pCluster->GetClusterModeName() );
	AddTextToOverlay( "Cluster Node Name  : " + pCluster->GetNodeName() );
	AddTextToOverlay( "Cluster Node ID    : " + VistaConversion::ToString( pCluster->GetNodeID() ) );
	
	// register for external message events - you'll learn about them further down this file
	SetupExternalMessageEvent();
	
}

ClusterModeDemo::~ClusterModeDemo()
{
	delete m_pMessage;
	delete m_pComputeFinishedEvent;
	delete m_pDataSync;
	delete m_pBarrier;
	delete m_pComputeThread;
	delete m_pTextOverlay;
	for( std::vector< IVistaTextEntity*>::iterator itText = m_vecTexts.begin();
			itText != m_vecTexts.end(); ++itText )
		delete (*itText);
	m_pVistaSystem->GetEventManager()->RemEventHandler( this, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREAPPLICATIONLOOP );
	delete m_pVistaSystem;
}

void ClusterModeDemo::Run()
{
	// Start Application
	m_pVistaSystem->Run();
}

void ClusterModeDemo::HandleEvent( VistaEvent* pEvent )
{
	if( pEvent->GetType() == VistaSystemEvent::GetTypeId() )
	{
		// we'll start a computation on the master, which will compute something
		// on the slaves, we have to do nothing. This will show how to synchronize 
		// infrequent events using the VistaExternalMessageEvent
		if( m_pVistaSystem->GetClusterMode()->GetIsLeader() )
		{
			CheckThreadedComputation();
		}

		// do some work for a synchronised amount of iterations
		PerformSomeComputation();
	}
	else if( pEvent->GetType() == VistaExternalMsgEvent::GetTypeId() )
	{
		HandleExternalMessageEvent( pEvent );
	}
}


void ClusterModeDemo::SetupExternalMessageEvent()
{
	VistaEventManager* pEventManager = m_pVistaSystem->GetEventManager();
	pEventManager->AddEventHandler( this, VistaExternalMsgEvent::GetTypeId(),
										VistaExternalMsgEvent::VEID_INCOMING_MSG );
	// configure our event				
	m_pComputeFinishedEvent = new VistaExternalMsgEvent();
	m_pComputeFinishedEvent->SetId( VistaExternalMsgEvent::VEID_INCOMING_MSG );
	m_pMessage = new VistaMsg;
	// we set a ticket to identify the message
	m_pMessage->SetMsgTicket( s_nComputationExternalMessageTag );
	m_pComputeFinishedEvent->SetThisMsg( m_pMessage );
}

void ClusterModeDemo::CheckThreadedComputation()
{
	// here, well compute some data asynchronously in a thread, but it might as
	// well be data received from an external application, e.g. a HPC backend or a tablet.
	if( m_pComputeThread == NULL && m_pVistaSystem->GetFrameLoop()->GetFrameCount() == 20 )
	{
		// no computation running yet, start it once on frame 20
		std::cout << "Starting a computation thread" << std::endl;
		m_pComputeThread = new ComputationThread();
		m_pComputeThread->Run();
		return;
	}

	// if a computation is running: check if it has finished
	if( m_pComputeThread )
	{
		if( m_pComputeThread->GetIsFinished() )
		{
			// yay, data! lets inform the slaves of our success by creating and
			// sending an ExternalMessageEvent with a specific type
			// this type needs to be registered beforehand, which we'll do
			// write our data
			std::string sData = m_pComputeThread->GetResult();
			VistaMsg::AssignMsgByString( sData, m_pMessage->GetThisMsgRef() );
			// and finally send the event
			// Since all ExternalMessageEvents will we distributed, the slaves will receive
			// them too. However, this should be regarded with a little care: On the master,
			// the event will be emitted right away, so the event handlers will be called
			// right away. However, the cluster mode sequentializes all distributed events,
			// and thus may change the order of execution. Example:
			// Master:                                    Slave:
			// Handle  PostApplicationLoopEvent           Handle PostApplicationLoopEvent
			//   |- do stuff                                |- do stuff
			//   |- set bDoMore = false                     |- set bDoMore = true
			//   |- Emit ExternalMsgEvent                   | - if( bDoMore ) DoMoreStuff
			//       |- Handle ExternalMsgEventEvent                 DoMoreStuff will NOT be execute
			//           |- set bDoMore = true            Handle ExternalMsgEventEvent
			//   | - if( bDoMore ) DoMoreStuff              |- set bDoMore = true 
			//          DoMoreStuff will be executed
			// Thus, ExternalMsgEvents should be emitted pretty late in a system event,
			// and no following code in this event should depend on the handle order.
			m_pVistaSystem->GetEventManager()->ProcessEvent( m_pComputeFinishedEvent );
			// clean up
			m_pComputeThread->Join();
			delete m_pComputeThread;
			m_pComputeThread = NULL;
		}

	}	
}

void ClusterModeDemo::HandleExternalMessageEvent( VistaEvent* pEvent )
{
	// we got an external message event - most likely from a finished Computation
	// but lets check the message ticker
	VistaExternalMsgEvent* pMsgEvent = Vista::assert_cast< VistaExternalMsgEvent* >( pEvent );
	if( pMsgEvent->GetThisMsg()->GetMsgTicket() == s_nComputationExternalMessageTag )
	{
		// yay, data!		
		std::string sData;
		VistaMsg::AssignStringByMsg( pMsgEvent->GetThisMsg()->GetThisMsgConstRef(), sData );
		std::cout << "Received ExternalMessageEvent with data ["
					<< sData << "]" << std::endl;
	}
}

// we've just seen how to synchronize infrequent events using  ExternalMessageEvents
void ClusterModeDemo::PerformSomeComputation()
{
	// let's assume we want to perform a computation for a fixed amount of time,
	// e.g. we want to run af 60Hz and know that rendering takes around 5ms, so we
	// could do calculations for around 10ms per frame, and continue to perform computation
	// steps until time is up. However, we have to make sure that the same number of iterations
	// is performed on all cluster nodes
	// There are multiple ways to achieve this

	// we could time on the master, and send out calculation events
	// while( time not up )
	// {
	//    SendExternalMsgEvent()
	// }
	// HandleExternalMsgEvent() -> compute
	// check the threaded computation example for more info about this

	// another option is to use synchronized times
	// we'll to computations until 10ms are up
	// for this, we'll create a cluster data sync object
	// this allows us to send data (e.g. a timestamp) from the master to all slaves.
	// Note that if you can use datasyncs in threads, too: just create a data sync
	// in the main thread, and pass it to a worker thread. Do NOT use one data sync
	// from multiple threads!
	if( m_pDataSync == NULL )
		m_pDataSync = m_pVistaSystem->GetClusterMode()->CreateDataSync();
	VistaTimer oTimer; // to measure actual computation time
	VistaType::microtime nStartTime = m_pDataSync->GetSyncTime();
	int nIterationCount = 0;
	for( ;; )
	{
		ComputeSomething();
		++nIterationCount;
		VistaType::microtime nTime = m_pDataSync->GetSyncTime() - nStartTime;
		if( nTime > 0.01 )
			break;
	}
	VistaType::microtime nActualDuration = oTimer.GetLifeTime();
	std::cout << "\nComputation method 1 performed " << nIterationCount
				<< " and took " << vstr::formattime( 1e3 * nActualDuration, 1 ) << "ms (aim 10ms)" << std::endl;

	// this works reasonably well when the computation on all nodes takes about
	// the same time (which should usually be the case).
	// However, sometimes you may need to sync computations with very different durations
	// here, it's only reasonable to start a new computation if all nodes are already finished.
	// For this, we can use the barrier, which waits until all slaves tell the master that
	// they reached the specified point in the execution, and then sends out a collective go.
	// Since we are now waiting for the slowest slave in each cycle, many nodes will idle for a
	// significant time, so we'll most likely achieve fewer iterations, but a better
	// match of the aim time than without the barrier

	if( m_pBarrier == NULL )
		m_pBarrier = m_pVistaSystem->GetClusterMode()->CreateBarrier();
	oTimer.ResetLifeTime();
	nStartTime = m_pDataSync->GetSyncTime();
	nIterationCount = 0;
	for( ;; )
	{
		ComputeSomething();
		++nIterationCount;
		m_pBarrier->BarrierWait();
		VistaType::microtime nTime = m_pDataSync->GetSyncTime() - nStartTime;
		if( nTime > 0.01 )
			break;
	}
	nActualDuration = oTimer.GetLifeTime();
	std::cout << "Computation method 2 performed " << nIterationCount
				<< " and took " << vstr::formattime( 1e3 * nActualDuration, 1 ) << "ms (aim 10ms)" << std::endl;

}

void ClusterModeDemo::ComputeSomething()
{
	// we'll "compute" for 1 to 2 milliseconds
	VistaRandomNumberGenerator* pRNG = VistaRandomNumberGenerator::GetStandardRNG();
	VistaType::systemtime nTimeToCompute = pRNG->GenerateDouble( 0.001, 0.002 );
	VistaTimer oTimer;
	while( oTimer.GetLifeTime() < nTimeToCompute )
	{
		// busy computing...
	}
}

void ClusterModeDemo::AddTextToOverlay( const std::string& sText )
{
	IVistaTextEntity* pText = m_pVistaSystem->GetDisplayManager()->CreateTextEntity();
	m_pTextOverlay->AddText( pText );
	pText->SetXPos( 1 );
	pText->SetYPos( (float)m_vecTexts.size() + 1 );
	pText->SetColor( VistaColor::GREEN );
	pText->SetText( sText );
	pText->SetFont( "MONOSPACE", 15 );
	m_vecTexts.push_back( pText );
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
