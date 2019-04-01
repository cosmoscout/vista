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


#include <GL/glew.h>

#include "VistaNewClusterSlave.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>
#include <VistaInterProcComm/IPNet/VistaIPAddress.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataCollect.h>
#include <VistaInterProcComm/Cluster/Imps/VistaBroadcastClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaInterProcClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaBroadcastClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaInterProcClusterDataSync.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaObserver.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/Cluster/Utils/VistaSlaveDataTunnel.h>
#include <VistaKernel/Cluster/Utils/VistaGSyncSwapBarrier.h>
#include <VistaKernel/VistaSystem.h>

#ifdef VISTA_WITH_ZEROMQ
#include <VistaKernel/Cluster/ZeroMQExt/VistaZeroMQClusterDataSync.h>
#include <VistaKernel/Cluster/ZeroMQExt/VistaZeroMQClusterBarrier.h>
#endif

#include <cassert>


#if defined(WIN32)
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif 

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


class VistaNewClusterSlave::SyncEntityObserver : public IVistaObserver
{
public:
	SyncEntityObserver( VistaNewClusterSlave* pClusterSlave )
	: IVistaObserver()
	, m_pClusterSlave( pClusterSlave )
	, m_bBlockNotifies( false )
	{
	}

	~SyncEntityObserver()
	{
	}

	virtual bool Observes( IVistaObserveable* pObserveable ) 
	{
		IVistaClusterSyncEntity* pSync = dynamic_cast<IVistaClusterSyncEntity*>( pObserveable );
		if( pObserveable == NULL )
			return false;
		std::vector<IVistaClusterSyncEntity*>::const_iterator itEntry 
						= std::find( m_vecSyncEntities.begin(), m_vecSyncEntities.end(), pSync );
		return( itEntry != m_vecSyncEntities.end() );
	}

	virtual void Observe( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		// no one except the clustermaster has access, and we can control this pretty well
		// thus we can be sure that there 
		IVistaClusterSyncEntity* pSync = static_cast<IVistaClusterSyncEntity*>( pObserveable );
		pSync->AttachObserver( this );
		m_vecSyncEntities.push_back( pSync );
	}

	virtual void ObserveableDelete( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		for( std::vector<IVistaClusterSyncEntity*>::iterator itEntry = m_vecSyncEntities.begin();
				itEntry != m_vecSyncEntities.end(); ++itEntry )
		{
			if( static_cast<IVistaObserveable*>( (*itEntry) ) == pObserveable )
			{
				m_vecSyncEntities.erase( itEntry );
				return;
			}
		}
	}

	virtual void ObserverUpdate( IVistaObserveable* pObserveable, int nMsg, int nTicket ) 
	{
		if( m_bBlockNotifies )
			return;
		if( nMsg != IVistaClusterSyncEntity::MSG_FATAL_ERROR )
			return;

		m_pClusterSlave->HandleMasterConnectionDrop();
	} 

	virtual void ReleaseObserveable( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		IVistaClusterSyncEntity* pSync = dynamic_cast<IVistaClusterSyncEntity*>( pObserveable );
		if( pObserveable == NULL )
			return;
		std::vector<IVistaClusterSyncEntity*>::iterator itEntry 
						= std::find( m_vecSyncEntities.begin(), m_vecSyncEntities.end(), pSync );
		if( itEntry != m_vecSyncEntities.end() )
		{
			m_vecSyncEntities.erase( itEntry );
		}
	}

	virtual bool ObserveableDeleteRequest( IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE ) 
	{
		return true;
	}

private:
	VistaNewClusterSlave*	m_pClusterSlave;
	std::vector<IVistaClusterSyncEntity*>	m_vecSyncEntities;
	bool m_bBlockNotifies;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaNewClusterSlave::VistaNewClusterSlave( VistaSystem* pVistaSystem,
									   const std::string& sSlaveName )
: m_pVistaSystem( pVistaSystem )
, m_sSlaveName( sSlaveName )
, m_pConnection( NULL )
, m_pSyncConnection( NULL )
, m_pSystemEvent( new VistaSystemEvent )
, m_pExternalMsgEvent( new VistaExternalMsgEvent )
, m_pMsg( new VistaMsg )
, m_pInteractionEvent( NULL )
, m_bDoByteSwap( false )
, m_bByteSwapSpecified( false )
, m_bDoOglFinish( false )
, m_nOwnSlaveID( -1 )
, m_iDataTunnelQueueSize( 2 )
, m_pDefaultBarrier( NULL )
, m_pDefaultDataSync( NULL )
, m_pSwapSyncBarrier( NULL )
, m_nSwapSyncMethod( VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER )
, m_nBarrierWaitMethod( VistaMasterSlave::BARRIER_BROADCAST )
, m_nBroadcastGroup( 0 )
, m_nSwapSyncTimeout( 0 )
, m_eForwardBarrier( DONT_FORWARD )
, m_eForwardDataSync( DONT_FORWARD )
, m_eForwardSwapBarrier( DONT_FORWARD )
, m_nUsedInterProcs( 0 )
, m_nDataSyncMethod( VistaMasterSlave::DATASYNC_TCP )
{
	m_pExternalMsgEvent->SetThisMsg( m_pMsg );
	m_pInteractionEvent = new VistaInteractionEvent( m_pVistaSystem->GetInteractionManager() );

	m_pSyncEntityObserver = new SyncEntityObserver( this );

	// Register Events
	m_oClusterMessage.RegisterEventType( m_pSystemEvent );
	m_oClusterMessage.RegisterEventType( m_pInteractionEvent );
	m_oClusterMessage.RegisterEventType( m_pExternalMsgEvent );
}

VistaNewClusterSlave::~VistaNewClusterSlave()
{
	// The DisplayBridge - and thus the window - has already been destroyed, so that
	// we don't have to unregister
	//if( m_nSwapSyncMethod == VistaMasterSlave::SWAPSYNC_GSYNCGROUP )
	//{
		//VistaGSyncSwapBarrier::LeaveSwapBarrier( m_pVistaSystem->GetDisplayManager() );
	//}

	vstr::outi() << "VistaNewClusterSlave::~VistaNewClusterSlave() -- closing connection" << std::endl;
	if( m_pConnection )
		m_pConnection->WaitForSendFinish();	
	delete m_pConnection;
	if( m_pSyncConnection )
		m_pSyncConnection->WaitForSendFinish();
	delete m_pSyncConnection;

	delete m_pDefaultDataSync;
	delete m_pDefaultBarrier;
	delete m_pSwapSyncBarrier;

	delete m_pSystemEvent;
	delete m_pInteractionEvent;
	delete m_pExternalMsgEvent;
	delete m_pMsg;

}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaNewClusterSlave::Init( const std::string& sClusterSection,
								const VistaPropertyList& oConfig )
{
	m_sSlaveSectionName = sClusterSection;
	m_sSlaveName = oConfig.GetValueOrDefault<std::string>( "NAME", sClusterSection );

	vstr::outi() << "[VistaNewClusterSlave]: Initializing" << std::endl;
	vstr::IndentObject oIndent;

	if( oConfig.HasSubList( sClusterSection ) == false )
	{
		vstr::errp() << "Cluster Slave cannot be initialized - section ["
			<< sClusterSection << "] does not exist!" << std::endl;
		return false;
	}
	const VistaPropertyList& oSection = oConfig.GetSubListConstRef( sClusterSection );

	if( ParseParameters( oSection ) == false )
		return false;	

	vstr::outi() << "Slave is told "
			  << (m_bDoOglFinish ? "" : "not ")
			  << "to do glFinish() before swap" << std::endl;

	
	if( WaitForClusterMaster( m_nServerPort ) == false )
	{
		vstr::errp() << "WaitForClusterMaster() failed on IP ["
					<< m_sOwnIP << ":" << m_nServerPort << "]" << std::endl;
		return false;
	}

	PrintClusterSetupInfo();

	vstr::outi() << "[VistaNewClusterSlave]: Initialization finished" << std::endl;
	return true;
}

bool VistaNewClusterSlave::ParseParameters( const VistaPropertyList& oSection )
{
	if( oSection.GetValue( "SLAVEIP", m_sOwnIP ) == false
		|| m_sOwnIP.empty() )
	{
		vstr::errp() << "No SLAVEIP given" << std::endl;
		return false;
	}
	if( oSection.GetValue( "SLAVEPORT", m_nServerPort ) == false )
	{
		vstr::errp() << "No SLAVEPORT given" << std::endl;
		return false;
	}
	if( m_nServerPort <= 0 )
	{
		vstr::errp() << "SLAVEPORT [" << m_nServerPort << "] invalid" << std::endl;
		return false;
	}

	std::string sPortListEntry;
	if( oSection.GetValue( "FREEPORTS", sPortListEntry ) )
	{
		VistaMasterSlave::FillPortListFromIni( sPortListEntry, m_vecFreePorts );
	}


	// The master never swaps, so we have to to it. if no value is specified, we
	// check endianess from the first token we receive from the master
	m_bByteSwapSpecified = oSection.GetValue<bool>( "BYTESWAP", m_bDoByteSwap );
	m_bDoOglFinish = oSection.GetValueOrDefault<bool>( "DOGLFINISH", false );
	m_iDataTunnelQueueSize = oSection.GetValueOrDefault<int>( "DATATUNNEL_QUEUE_SIZE", 12 );
	m_nBroadcastGroup = oSection.GetValueOrDefault<int>( "BROADCASTGROUP", 0 );

	oSection.GetValue<std::string>( "INTERPROC_NAME", m_sInterProcName );
	oSection.GetValue<std::vector<std::string> >( "INTERPROC_NAMES", m_vecInterProcNames );
	
	m_nSwapSyncTimeout = oSection.GetValueOrDefault<int>( "SWAPSYNCTIMEOUT", 0 );

	ParseSyncForward( oSection, "FORWARD_DATASYNC", m_eForwardDataSync );
	ParseSyncForward( oSection, "FORWARD_BARRIER", m_eForwardBarrier );
	ParseSyncForward( oSection, "FORWARD_SWAPSYNC", m_eForwardSwapBarrier );

	return true;
}

void VistaNewClusterSlave::ParseSyncForward(  const VistaPropertyList& oSection,
											 const std::string& sName,
											 ForwardMode& eForwardVariable )
{
	eForwardVariable = DONT_FORWARD;
	std::string sSyncForwardType;
	if( oSection.GetValue<std::string>( sName, sSyncForwardType ) )
	{
		if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sSyncForwardType, "NONE" )
			|| VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sSyncForwardType, "FALSE" ) )
		{
			eForwardVariable = DONT_FORWARD;
		}
		if(  VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sSyncForwardType, "MASTER" ) )
		{			
			if( m_vecInterProcNames.empty() )
				vstr::warnp() << sName << " set as MASTER, but no INTERPROC_NAMES specified" << std::endl;
			else
				eForwardVariable = FORWARD_MASTER;
				
		}
		else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sSyncForwardType, "SLAVE" ) )
		{
			if( m_sInterProcName.empty() )
				vstr::warnp() << sName << " set as MASTER, but no INTERPROC_NAMES specified" << std::endl;
			else
				eForwardVariable = FORWARD_SLAVE;
		}	
		else
		{
			vstr::warnp() << sName << " has invalid value [" << sSyncForwardType
							<< "] - specify MASTER, SLAVE or NONE" << std::endl;
		}
	}
}



bool VistaNewClusterSlave::PostInit()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "PostInit" << std::endl;
	}
	m_pDefaultBarrier->BarrierWait( m_nSwapSyncTimeout );

	// now (after windows have been created) we can set up the gsync-barrier, if desired/
	if( m_nSwapSyncMethod == VistaMasterSlave::SWAPSYNC_GSYNCGROUP )
	{
		if( VistaGSyncSwapBarrier::JoinSwapBarrier( m_pVistaSystem->GetDisplayManager() ) == false )
		{
			vstr::errp() << "[VistaClusterSlave]: could not join GSync-based swap group!" 
						<< "Syncing will not work on this slave!" << std::endl;
		}
	}

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    PostInit finished" << std::endl;
	}

	return true;
}


int VistaNewClusterSlave::GetClusterMode() const
{
	return CM_MASTER_SLAVE;
}

std::string VistaNewClusterSlave::GetClusterModeName() const
{
	return "MASTER_SLAVE";
}


int VistaNewClusterSlave::GetNodeType() const
{
	return NT_SLAVE;
}

std::string VistaNewClusterSlave::GetNodeTypeName() const
{
	return "SLAVE";
}

std::string VistaNewClusterSlave::GetNodeName() const
{
	return m_sSlaveName;
}

std::string VistaNewClusterSlave::GetConfigSectionName() const
{
	return m_sSlaveSectionName;
}

int VistaNewClusterSlave::GetNodeID() const
{
	return m_nOwnSlaveID;
}


bool VistaNewClusterSlave::GetIsLeader() const
{
	return false;
}

bool VistaNewClusterSlave::GetIsFollower() const
{
	return true;
}


bool VistaNewClusterSlave::StartFrame()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "StartFrame" << std::endl;
	}
	++m_iFrameCount;

	if( m_pDefaultDataSync == NULL || !m_pDefaultDataSync->GetIsValid() )
		return false;

	if( m_pDefaultDataSync->SyncData( m_oClusterMessage ) == false )
	{
		vstr::errp() << "[VistaCluserSlave]: Fatal communication error - aborting" << std::endl;
		VISTA_THROW( "ClusterSlave Error: Communication failure", -1 );
	}

	if( m_oClusterMessage.GetType() != VistaMasterSlave::Message::CMSG_STARTFRAME )
	{
		VISTA_THROW( "ClusterSlave Protocol Error: received unexpected message type", -1 );
	}

	if( m_oClusterMessage.GetFrameCount() != m_iFrameCount )
	{
		VISTA_THROW( "ClusterSlave Error: received unexpected frame count", -1 );
	}

	m_dFrameClock = m_oClusterMessage.GetClock();

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    StartFrame finished, frameid " << m_iFrameCount 
						<< ", frameclock " << m_dFrameClock << std::endl;
	}
	return true;
}

bool VistaNewClusterSlave::ProcessFrame()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "ProcessFrame" << std::endl;
	}
	while( m_pDefaultDataSync && m_pDefaultDataSync->GetIsValid() )
	{
		if( m_pDefaultDataSync->SyncData( m_oClusterMessage ) == false )
		{
			if( m_oClusterInfo.m_bClusterDebuggingActive )
			{
				GetDebugStream() << "ProcessFrame Sync error" << std::endl;
			}
			VISTA_THROW( "ClusterSlave Error: Communication failure", -1 );
		}
		switch( m_oClusterMessage.GetType() )
		{
			case VistaMasterSlave::Message::CMSG_ENDFRAME:
			{
				if( m_oClusterInfo.m_bClusterDebuggingActive )
				{
					GetDebugStream() << "ProcessFrame EndFrame" << std::endl;
				}
				return true;
			}
			case VistaMasterSlave::Message::CMSG_EVENT:
			{
				// special distinction for profiling of system events
				// @todo this is somewhat hacked currently
				VistaEvent* pEvent = m_oClusterMessage.GetEvent();
				VistaSystemEvent* pSysEvent = dynamic_cast<VistaSystemEvent*>( pEvent );
				if( pSysEvent )
				{
					VistaKernelProfileScope( VistaSystemEvent::GetIdString( pSysEvent->GetId() ) );
					if( m_oClusterInfo.m_bClusterDebuggingActive )
					{
						GetDebugStream() << "ProcessFrame SystemEventEvent "
									<< VistaSystemEvent::GetIdString( pSysEvent->GetId() )
									<< ", time " << vstr::formattime( pEvent->GetTime(), 6 )
							<< std::endl;
					}
					m_pVistaSystem->GetEventManager()->ProcessEvent( pSysEvent );
				}
				else
				{
					if( m_oClusterInfo.m_bClusterDebuggingActive )
					{
						GetDebugStream() << "ProcessFrame Event type " << pEvent->GetType()
							<< ", id " << pEvent->GetId() << ", time " << vstr::formattime( pEvent->GetTime(), 6 )
							<< std::endl;
					}
					m_pVistaSystem->GetEventManager()->ProcessEvent( m_oClusterMessage.GetEvent() );
				}
				break;
			}
			case VistaMasterSlave::Message::CMSG_QUIT:
			{
				if( m_oClusterInfo.m_bClusterDebuggingActive )
				{
					GetDebugStream() << "ProcessFrame Quit"	<< std::endl;
				}
				// @todo
				break;
			}
			case VistaMasterSlave::Message::CMSG_INVALID:
			case VistaMasterSlave::Message::CMSG_STARTFRAME:
			default:
			{
				if( m_oClusterInfo.m_bClusterDebuggingActive )
				{
					GetDebugStream() << "ProcessFrame encountered unexpected event type" << std::endl;
				}
				VISTA_THROW( "VistaClusterSlave unexpected event type", -1 );
			}
		}
	}

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "ProcessFrame failed" << std::endl;
	}

	return false;
}

bool VistaNewClusterSlave::EndFrame()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "EndFrame" << std::endl;
	}
	return true;
}

bool VistaNewClusterSlave::CreateConnections( std::vector<VistaConnectionIP*>& vecConnections )
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaNewClusterSlave::CreateConnections() -- "
					<< "Could not establish connection to master" << std::endl;
		return false;
	}

	vecConnections.push_back( pConnection );

	return true;
}
bool VistaNewClusterSlave::CreateNamedConnections( 
				std::vector<std::pair<VistaConnectionIP*, std::string> >& vecConnections )
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaNewClusterSlave::CreateConnections() -- "
					<< "Could not establish connection to master" << std::endl;
		return false;
	}

	vecConnections.push_back( std::pair<VistaConnectionIP*, std::string>(
								pConnection, m_oClusterInfo.m_vecNodeInfos[0].m_sNodeName ) );

	return true;
}

IVistaDataTunnel* VistaNewClusterSlave::CreateDataTunnel( IDLVistaDataPacket* pPacketProto )
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaNewClusterSlave::CreateNetworkSync() -- "
			<< "Could not establish connection for syncing" << std::endl;
		return NULL;
	}	
	
	return new VistaSlaveDataTunnel( pConnection, m_iDataTunnelQueueSize, pPacketProto );
}

void VistaNewClusterSlave::Debug( std::ostream& oStream ) const
{
	oStream << "[VistaNewClusterSlave]\n"
			<< "    Name             : " << m_sSlaveName << "\n"
			<< "    Config Section   : " << m_sSlaveSectionName << "\n"
			<< "    SlaveId          : " << m_nOwnSlaveID << "\n"
			<< "    Byteswap         : " << (m_bDoByteSwap ? "YES" : "NO") << "\n"
			<< "    DoGLFinish       : " << (m_bDoOglFinish ? "YES" : "NO") << "\n"
			<< "    DataSync Mode    : " << VistaMasterSlave::GetDataSyncModeName( m_nDataSyncMethod ) << "\n"
			<< "    Barrier Mode     : " << VistaMasterSlave::GetBarrierModeName( m_nBarrierWaitMethod ) << "\n"
			<< "    SwapSync Mode    : " << VistaMasterSlave::GetSwapSyncModeName( m_nSwapSyncMethod ) << "\n"
			<< "    SwapSync timeout : " << vstr::formattime( m_nSwapSyncTimeout, 3 ) << "\n"
			<< "    Connected        : " << (m_pConnection ? "YES" : "NO") << "\n"
			<< "    DebugOutput      : " << ( m_oClusterInfo.m_bClusterDebuggingActive ? "Enabled" : "Disabled" );
	if( m_pConnection )
	{
		VistaSocketAddress oOwnAddress = m_pConnection->GetLocalAddress();
		std::string sOwnHostname;
		oOwnAddress.GetIPAddress().GetHostName( sOwnHostname );
		oStream << "    Slave Address    : " << sOwnHostname
							<< ":" << oOwnAddress.GetPortNumber() << "\n"
				<< "    Master Address   : " << m_pConnection->GetPeerName() 
							<< ":" << m_pConnection->GetPeerPort() << "\n";
	}
	oStream.flush();
}

void VistaNewClusterSlave::PrintClusterSetupInfo() const
{
	vstr::outi() << "[VistaNewClusterSlave]: Configuration:\n";
	vstr::IndentObject oIndent;
	vstr::outi() << "DataSync Mode    : " << VistaMasterSlave::GetDataSyncModeName( m_nDataSyncMethod ) << "\n";
	vstr::outi() << "Barrier Mode     : " << VistaMasterSlave::GetBarrierModeName( m_nBarrierWaitMethod ) << "\n";
	vstr::outi() << "SwapSync Mode    : " << VistaMasterSlave::GetSwapSyncModeName( m_nSwapSyncMethod ) << "\n";
	vstr::outi() << "SwapSync timeout : " << vstr::formattime( m_nSwapSyncTimeout, 3 ) << std::endl;
}

bool VistaNewClusterSlave::WaitForClusterMaster( int nPort )
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "WaitForClusterMaster" << std::endl;
	}
	try
	{
		vstr::outi() << "Waiting for master on ["
				<< m_sOwnIP << ":" << nPort << "]..." << std::endl;

		VistaTCPServer oServer( m_sOwnIP, nPort, 0, true );
		if( oServer.GetIsValid() == false )
		{
			vstr::errp() << "Could not create main TCP server!" << std::endl;
			VISTA_THROW( "ClusterSlave Error: WaitForClusterMaster failed", -1 );
		}

		VistaTCPSocket *pSocket = oServer.GetNextClient();

		if( pSocket == NULL )
		{
			vstr::errp() << "WaitForClusterMaster failed - connected socket is NULL" << std::endl;
			VISTA_THROW( "ClusterSlave Error: WaitForClusterMaster failed", -1 );
		}

		m_pConnection = new VistaConnectionIP( pSocket );
		m_pConnection->SetIsBlocking( true );
		m_pConnection->SetIsBuffering( false );
		m_pConnection->SetByteorderSwapFlag( VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );

		// read first int to determine the byteorder - if it is a 1, no swap is required,
		// otherwise, we'll have to swap
		VistaType::sint32 nSwapIndicator;
		m_pConnection->ReadInt32( nSwapIndicator );
		bool bSwapIndicated = ( nSwapIndicator != 1 );
		if( m_bByteSwapSpecified )
		{
			if( bSwapIndicated != m_bDoByteSwap )
			{
				vstr::warnp() << "Configuration set ByteSwapping ["
						<< ( m_bDoByteSwap ? "ON" : "OFF" )
						<< "] but checks indicate the opposite - this is likely to cause errors"
						<< std::endl;
			}
		}
		else
		{
			m_bDoByteSwap = bSwapIndicated;
		}
		
		m_pConnection->SetByteorderSwapFlag( m_bDoByteSwap == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );

		// read drameclock and own id
		m_dFrameClock = 0;
		VistaType::sint32 nIndex;
		m_pConnection->ReadDouble( m_dFrameClock );
		m_pConnection->ReadInt32( nIndex );
		m_nOwnSlaveID = nIndex;

		// write own name
		m_pConnection->WriteEncodedString( m_sSlaveName );


		// read info about Syncing
		VistaType::sint32 nReadValue;
		m_pConnection->ReadInt32( nReadValue );
		// syncmode: always TCP, ignore for now
		m_pConnection->ReadInt32( nReadValue );
		m_nBarrierWaitMethod = nReadValue;
		m_pConnection->ReadInt32( nReadValue );
		m_nSwapSyncMethod = nReadValue;
		m_pConnection->ReadInt32( nReadValue );
		m_nDataSyncMethod = nReadValue;

		// establish default sync connection
		VistaTCPSocket *pSyncSocket = NULL;
		int nFailCount = 0;
		while( pSyncSocket == NULL )
		{
			int nSyncPort = GetNextFreePort();
			if( nSyncPort <= 0 )
			{
				// it's okay, we'll let the TCPserver decide on a free port
				nSyncPort = 0;
			}
			VistaTCPServer oSyncServer( m_sOwnIP, nSyncPort, 0, true );
			if( oSyncServer.GetIsValid() == false )
			{
				vstr::warnp() << "Could not create sync TCP server on [" 
								<< m_sOwnIP << ":" << nSyncPort 
								<< "]" << std::endl;
				if( ++nFailCount == 10 )
				{
					vstr::errp() << "Repeatedly failed to open server - aborting" << std::endl;
					VISTA_THROW( "ClusterSlave Error: WaitForClusterMaster failed", -1 );
				}
			}

			if( nSyncPort == 0 )
				nSyncPort = oSyncServer.GetServerAddress().GetPortNumber();

			m_pConnection->WriteInt32( nSyncPort );
			pSyncSocket = oSyncServer.GetNextClient();
		}

		if( pSyncSocket == NULL )
		{
			vstr::errp() << "WaitForClusterMaster failed - no sync conn was established" << std::endl;
			VISTA_THROW( "ClusterSlave Error: WaitForClusterMaster failed", -1 );
		}
		m_pSyncConnection = new VistaConnectionIP( pSyncSocket );
		m_pSyncConnection->SetIsBlocking( true );
		m_pSyncConnection->SetIsBuffering( false );
		m_pSyncConnection->SetByteorderSwapFlag( m_bDoByteSwap == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );

		
		vstr::outi() << "Creating ClusterSync" << std::endl;
		if( CreateDefaultSyncs() == false )
		{
			vstr::errp() << "Could not create default sync objects - aborting" << std::endl;
			return false;
		}
		InitSwapSync();

		// sync first frame clock
		m_pDefaultDataSync->SyncTime( m_dFrameClock );

		vstr::outi() << "Receiving ClusterInfo" << std::endl;
		m_pDefaultDataSync->SyncData( m_oClusterInfo );
		if( m_oClusterInfo.m_bClusterDebuggingActive )
		{
			vstr::GetStreamManager()->AddNewLogFileStream( m_oClusterInfo.m_sClusterDebuggingStreamName,
														m_oClusterInfo.m_sClusterDebuggingFile,
														"log", true, true, false );
		}
	}
	catch( VistaExceptionBase& e )
	{
		if( m_oClusterInfo.m_bClusterDebuggingActive )
		{
			GetDebugStream() << "    WaitForClusterMaster failed" << std::endl;
		}
		vstr::errp() << "WaitForClusterMaster failed with Exception:\n";
		e.PrintException();//( vstr::err(), true );
		VISTA_THROW( "ClusterSlave Error: WaitForClusterMaster failed", -1 );
	}

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    WaitForClusterMaster finished" << std::endl;
	}
	return true;
}

void VistaNewClusterSlave::SwapSync()
{
	VistaKernelProfileScope( "SLAVE_SWAPSYNC" );

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "SwapSync" << std::endl;
	}

	// user told us to wait until glFinish() returns
	// this is a best guess that the gfx hardware is really
	// done with rendering and waits for the buffer swap
	if( m_bDoOglFinish )
	{
		VistaKernelProfileScope( "GL_FINISH" );
		glFinish();
	}
	
	switch( m_nSwapSyncMethod )
	{
		case VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER:
		{
			if( m_pDefaultBarrier )
			{
				if( m_pDefaultBarrier->BarrierWait( m_nSwapSyncTimeout ) == false )
				{
					vstr::warnp() << "[VistaNewClusterSlave] SwapSync failed!" << std::endl;
					if( m_oClusterInfo.m_bClusterDebuggingActive )
					{
						GetDebugStream() << "SwapSync failed (timed out?)" << std::endl;
					}
				}
			}
			break;
		}
		case VistaMasterSlave::SWAPSYNC_GSYNCGROUP:
		{
			// syncing is performed via GSync during swapbuffers;
			break;
		}
		case VistaMasterSlave::SWAPSYNC_NONE:
		{
			break;
		}
		case VistaMasterSlave::SWAPSYNC_ZEROMQ:
		case VistaMasterSlave::SWAPSYNC_BROADCAST:
		case VistaMasterSlave::SWAPSYNC_DUMMY:
		case VistaMasterSlave::SWAPSYNC_TCP:
		case VistaMasterSlave::SWAPSYNC_INTERPROC:
		{
			if( m_pSwapSyncBarrier )
			{
				if( m_pSwapSyncBarrier->BarrierWait( m_nSwapSyncTimeout ) == false )
				{
					vstr::warnp() << "[VistaNewClusterSlave] SwapSync failed!" << std::endl;
					if( m_oClusterInfo.m_bClusterDebuggingActive )
					{
						GetDebugStream() << "SwapSync failed (timed out?)" << std::endl;
					}
				}
			}
			break;
		}
		default:
			VISTA_THROW( "Unknown SwapSync mode", -1 );
	};

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    SwapSync finished" << std::endl;
	}
}

VistaConnectionIP* VistaNewClusterSlave::CreateConnectionToMaster()
{
	VistaConnectionIP* pConnection = NULL;
	try
	{	
		VistaTCPSocket* pSocket = NULL;
		int nFailCount = 0;
		while( pSocket == NULL )
		{
			int iPort = GetNextFreePort();
			if( iPort < 0 )
			{
				// it's okay, we'll let the TCPServer choose a free port
				iPort = 0;
			}

			VistaTCPServer oServer( m_sOwnIP, iPort, 0, true);
			if( oServer.GetIsValid() == false )
			{
				vstr::warnp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
								"Could not create sync TCP server on [" 
								<< m_sOwnIP << ":" << iPort 
								<< "]" << std::endl;
				if( ++nFailCount == 10 )
				{
					vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
								"Repeatedly failed to open server - aborting" << std::endl;
					m_pConnection->WriteInt32( -2 );
					return NULL;
				}
			}

			if( iPort == 0 )
				iPort = oServer.GetServerAddress().GetPortNumber();

			vstr::outi() << "VistaClusterSlave::CreateConnectionToMaster() -- "
				<< "Waiting for connection from master on IP [" << m_sOwnIP 
				<< "] - Port [" << iPort << "]" << std::endl;

			if( m_pConnection->WriteInt32( iPort ) != sizeof(VistaType::sint32) )
			{
				vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
						<< "Could not send own port to master" << std::endl;
				return NULL;
			}

			pSocket = oServer.GetNextClient();
			if( pSocket == NULL )
			{
				vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
						<< "No connection from master on IP [" << m_sOwnIP
						<< "] - Port [" << iPort << "]" << std::endl;
				return NULL;
			}
		}

		vstr::outi() << "VistaClusterSlave::CreateConnectionToMaster() -- "
				<< "Connection established" << std::endl;

		pConnection = new VistaConnectionIP( pSocket );
		pConnection->SetIsBlocking( true );
		pConnection->SetIsBuffering( false );
		pConnection->SetByteorderSwapFlag( m_bDoByteSwap ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );
	}
	catch( VistaExceptionBase& e )
	{
		vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster failed with exception\n"
				<< e.GetPrintStatement() << std::endl;
		return NULL;
	}	
	
	return pConnection;
}


IVistaClusterDataSync* VistaNewClusterSlave::CreateDataSync()
{
	return CreateTypedDataSync( m_nDataSyncMethod, false );
}

IVistaClusterDataSync* VistaNewClusterSlave::GetDefaultDataSync()
{
	if( m_pDefaultDataSync == NULL )
		m_pDefaultDataSync = CreateDataSync();
	return m_pDefaultDataSync;
}

IVistaClusterBarrier* VistaNewClusterSlave::CreateBarrier()
{
	return CreateTypedBarrier( m_nBarrierWaitMethod, false, false );
}

IVistaClusterBarrier* VistaNewClusterSlave::GetDefaultBarrier()
{
	if( m_pDefaultBarrier == NULL )
		m_pDefaultBarrier = CreateBarrier();
	return m_pDefaultBarrier;
}

int VistaNewClusterSlave::GetNextFreePort()
{
	return VistaMasterSlave::GetFreePortFromPortList( m_vecFreePorts );
}

bool VistaNewClusterSlave::InitSwapSync()
{	
	switch( m_nSwapSyncMethod )
	{
		case VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER:
		case VistaMasterSlave::SWAPSYNC_NONE:
		{
			// nothing to do
			return true;
		}
		case VistaMasterSlave::SWAPSYNC_BROADCAST:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_BROADCAST, true, true );			
			break;
		}
		case VistaMasterSlave::SWAPSYNC_DUMMY:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_DUMMY, true, true );
			m_pSyncEntityObserver->Observe( m_pSwapSyncBarrier );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_TCP:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_TCP, true, true );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_INTERPROC:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_INTERPROC, true, true );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_ZEROMQ:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_ZEROMQ, true, true );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_GSYNCGROUP:
		{
			// we delay this to the PostInit call - we have to wait for the windows to exist
			return true;
		}
		default:
		{
			VISTA_THROW( "Unknown SwapSync Method", -1 );
		}
	}
	if( m_pSwapSyncBarrier == NULL )
		return false;
	m_pSyncEntityObserver->Observe( m_pSwapSyncBarrier );
	return true;
}

IVistaClusterBarrier* VistaNewClusterSlave::CreateTypedBarrier( int nType, bool bUseDefaultConnection,
																bool bIsSwapSyncBarrier )
{
	ForwardMode eMode = m_eForwardBarrier;
	if( bIsSwapSyncBarrier )
		eMode = m_eForwardSwapBarrier;

	// we want to indicate to the master to skip our sync if we are configured as FORWARD_SLAVE
	m_pConnection->WriteBool( ( eMode == FORWARD_SLAVE ) );	
	
	
	IVistaClusterBarrier* pBarrier = NULL;

	if( eMode )
	{
		bool bDummy;
		m_pConnection->ReadBool( bDummy );
		assert( bDummy == true );
		std::string sFullName = m_sInterProcName + "_" + VistaConversion::ToString( m_nUsedInterProcs++ );
		pBarrier = new VistaInterProcClusterFollowerBarrier( sFullName );
	}
	else
	{
		switch( nType )
		{
			case VistaMasterSlave::BARRIER_DUMMY:
			{
				pBarrier = new VistaDummyClusterBarrier;
				break;
			}
			case VistaMasterSlave::BARRIER_TCP:
			{			
				pBarrier = CreateTCPBarrier( bUseDefaultConnection );
				break;
			}
			case VistaMasterSlave::BARRIER_BROADCAST:
			{			
				pBarrier = CreateBCBarrier( bUseDefaultConnection );
				break;
			}
			case VistaMasterSlave::BARRIER_INTERPROC:
			{		
				pBarrier = CreateInterProcBarrier();
				break;
			}
				case VistaMasterSlave::BARRIER_ZEROMQ:
			{		
				pBarrier = CreateZeroMQBarrier( bUseDefaultConnection );
				break;
			}
			default:
				VISTA_THROW( "VistaClusterSlave undefined barrier type", -1 )
		}

		if( eMode == FORWARD_MASTER )
		{
			VistaInterProcClusterLeaderBarrier* pIPSync = new VistaInterProcClusterLeaderBarrier( pBarrier );
			std::string sPostfix = "_" + VistaConversion::ToString( m_nUsedInterProcs++ );
			for( std::vector<std::string>::const_iterator itName = m_vecInterProcNames.begin();
					itName != m_vecInterProcNames.end(); ++itName )
			{
				pIPSync->AddInterProc( (*itName) + sPostfix );
			}
			pBarrier = pIPSync;
		}
	}

	if( pBarrier == NULL )
	{
		vstr::warnp() << "Barrier creation failed - using DummyBarrier instead" << std::endl;
		pBarrier = new VistaDummyClusterBarrier;
	}
	else if( pBarrier->GetIsValid() == false )
	{
		vstr::warnp() << "[VistaNewClusterSlave::CreateBarrier]: "
					<< "BarrierWait of type [" << pBarrier->GetBarrierType()
					<< "] is invalid" << std::endl;
	}

	return pBarrier;
}

IVistaClusterBarrier* VistaNewClusterSlave::CreateTCPBarrier( bool bUseDefaultConnection )
{
	VistaConnectionIP* pConnection;
	if( bUseDefaultConnection )
		pConnection = m_pSyncConnection;
	else
		pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "[VistaNewClusterSlave::CreateTCPBarrier]:"
					<< "Could not create barrier - connection failed"
					<< std::endl;
	}
	VistaTCPIPClusterFollowerBarrier* pBarrier = new VistaTCPIPClusterFollowerBarrier( pConnection, !bUseDefaultConnection );

	return pBarrier;
}

IVistaClusterBarrier* VistaNewClusterSlave::CreateBCBarrier( bool bUseDefaultConnection )
{
	VistaBroadcastClusterFollowerBarrier* pBarrier = NULL;
	try
	{
		VistaConnectionIP* pConnection;
		if( bUseDefaultConnection )
			pConnection = m_pSyncConnection;
		else
			pConnection = CreateConnectionToMaster();
		if( pConnection == NULL )
		{
			vstr::warnp() << "[VistaNewClusterSlave::CreateClusterBarrier]: "
				<< "Could not establish barrier connection" << std::endl;
			return NULL;
		}

		// Send out broadcast group, and receive a corresponding port and ip
		pConnection->WriteInt32( (VistaType::sint32)m_nBroadcastGroup );
		VistaType::sint32 nPort = -1;
		pConnection->ReadInt32( nPort );
		if( nPort < 0 )
		{
			vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
						<< "Master did not report broadcast port - aborting" << std::endl;
			return NULL;
		}
		std::string sIP;
		pConnection->ReadEncodedString( sIP );

		VistaUDPSocket* pSocket = new VistaUDPSocket;
		if( pSocket->OpenSocket() ==  false )
		{		
			vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
						<< "Could not open broadcast socket. This slave will report it's rediness, "
						<< "but not wait for the common go signal" << std::endl;
			delete pSocket;
			pSocket = NULL;
		}
		else
		{
			pSocket->SetIsBlocking( true );
			pSocket->SetPermitBroadcast( 1 );
			// set reuse, so that multiple clients can listen to one BC port
			pSocket->SetSocketReuse( true );

			VistaSocketAddress oAddress( sIP, nPort );
			if( pSocket->BindToAddress( oAddress ) == false )
			{
				vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
								<< "Could not bind Broadcast-Socket to ["
								<< sIP << ":" << nPort << "]. "
								<< "This slave will report it's readiness, "
								<< "but not wait for the common go signal" << std::endl;
				delete pSocket;
				pSocket = NULL;
			}
		}

		pBarrier = new VistaBroadcastClusterFollowerBarrier( pSocket, pConnection, true, !bUseDefaultConnection );			
	}
	catch( VistaExceptionBase& e )
	{
		vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
					<< "Exception while creating broadcast barrier:\n" 
					<< e.GetPrintStatement() << std::endl;
		return NULL;
	}

	return pBarrier;
}


IVistaClusterBarrier* VistaNewClusterSlave::CreateZeroMQBarrier(bool bUseDefaultConnection)
{
#ifdef VISTA_WITH_ZEROMQ
	
	VistaConnectionIP* pConnection;
	if( bUseDefaultConnection )
		pConnection = m_pSyncConnection;
	else
		pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "[VistaNewClusterSlave::CreateZeroMQBarrier]: "
			<< "Could not establish barrier connection" << std::endl;
		return NULL;
	}
	
	std::string sAddress;
	pConnection->ReadEncodedString( sAddress );

	VistaZeroMQClusterFollowerBarrier* pBarrier = new VistaZeroMQClusterFollowerBarrier( sAddress, pConnection, true, !bUseDefaultConnection );			
	pBarrier->WaitForConnection( pConnection );
	return pBarrier;
#else
	VISTA_THROW( "Vista not built with ZeroMQ capabilities", -1 );
#endif
}


IVistaClusterBarrier* VistaNewClusterSlave::CreateInterProcBarrier()
{
	try
	{
		// send our string to master (he also checks for empty strings)
		m_pConnection->WriteEncodedString( m_sInterProcName );
		if( m_sInterProcName.empty() )
		{
			vstr::warnp() << "VistaNewClusterMaster::CreateInterProcBarrier() -- "
						<< "no INTERPROC_NAME specified!" << std::endl;
			return NULL;
		}
		VistaInterProcClusterFollowerBarrier* pBarrier = new VistaInterProcClusterFollowerBarrier( m_sInterProcName );
		if( pBarrier->GetIsValid() == false )
		{
			delete pBarrier;
			vstr::warnp() << "VistaNewClusterMaster::CreateInterProcBarrier() -- "
						<< "creating of barrier with name [" << m_sInterProcName << "failed!" << std::endl;
			return NULL;
		}	
		return pBarrier;
	}
	catch( VistaExceptionBase& )
	{
		vstr::warnp() << "VistaNewClusterMaster::CreateInterProcBarrier() -- "
						<< "Exception while creating barrier" << std::endl;
		return NULL;
	}
}


IVistaClusterDataSync* VistaNewClusterSlave::CreateTypedDataSync( int nType, bool bUseDefaultConnection )
{
	// we want to indicate to the master to skip our sync if we are configured as FORWARD_SLAVE
	m_pConnection->WriteBool( ( m_eForwardDataSync == FORWARD_SLAVE ) );

	
	IVistaClusterDataSync* pSync = NULL;

	if( m_eForwardDataSync == FORWARD_SLAVE )
	{
		bool bDummy;
		m_pConnection->ReadBool( bDummy );
		assert( bDummy == true );
		std::string sFullName = m_sInterProcName + "_" + VistaConversion::ToString( m_nUsedInterProcs++ );
		VistaInterProcClusterFollowerDataSync* pIPSync = new VistaInterProcClusterFollowerDataSync( sFullName );
		pIPSync->ConnectPipe();
		pSync = pIPSync;
	}
	else
	{
		VistaInterProcClusterRedirectDataSync* pIPSync = NULL;
		if( m_eForwardDataSync == FORWARD_MASTER )
		{
			pIPSync = new VistaInterProcClusterRedirectDataSync;
			std::string sPostfix = "_" + VistaConversion::ToString( m_nUsedInterProcs++ );
			for( std::vector<std::string>::const_iterator itName = m_vecInterProcNames.begin();
					itName != m_vecInterProcNames.end(); ++itName )
			{
				pIPSync->AddInterProc( (*itName) + sPostfix );
			}
		}

		switch( nType )
		{
			case VistaMasterSlave::DATASYNC_DUMMY:
			{
				pSync = new VistaDummyClusterDataSync;
				break;
			}
			case VistaMasterSlave::DATASYNC_TCP:
			{
				pSync = CreateTCPIPDataSync( bUseDefaultConnection );
				break;
			}
			case VistaMasterSlave::DATASYNC_ZEROMQ:
			{
				pSync = CreateZeroMQDataSync();
				break;
			}
			case VistaMasterSlave::DATASYNC_INTERPROC:
			{
				pSync = CreateInterProcDataSync();
				break;
			}
			default:
				VISTA_THROW( "VistaClusterMaster Unknown DataSync type", -1 );
		}

		if( m_eForwardDataSync == FORWARD_MASTER )
		{
			pIPSync->SetDependentDataSync( pSync );
			pIPSync->ConnectPipes();
		}
	}

	if( pSync == NULL )
	{
		vstr::warnp() << "DataSync creation failed - using DummySync instead" << std::endl;
		pSync = new VistaDummyClusterDataSync;
	}
	else if( pSync->GetIsValid() == false )
	{
		vstr::warnp() << "[VistaNewClusterMaster::CreateDataSync]: "
					<< "Datasync of type [" << pSync->GetDataSyncType()
					<< "] is invalid" << std::endl;
	}

	return pSync;
}

IVistaClusterDataSync* VistaNewClusterSlave::CreateTCPIPDataSync( bool bUseDefaultConnection )
{
	VistaConnectionIP* pConnection;
	if( bUseDefaultConnection )
		pConnection = m_pSyncConnection;
	else
		pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaNewClusterSlave::CreateNetworkSync() -- "
			<< "Could not establish connection for syncing" << std::endl;
		return NULL;
	}
	return new VistaTCPIPClusterFollowerDataSync( pConnection, m_bDoByteSwap, !bUseDefaultConnection );
}

// Broadcast datasync turned out to be unreliable
//IVistaClusterDataSync* VistaNewClusterSlave::CreateBroadcastDataSync( bool bUseDefaultConnection )
//{
//	VistaBroadcastClusterFollowerDataSync* pDataSync = NULL;
//	try
//	{
//		// Send out broadcast group, and receive a corresponding port and ip
//		m_pConnection->WriteInt32( (VistaType::sint32)m_nBroadcastGroup );
//		VistaType::sint32 nPort = -1;
//		m_pConnection->ReadInt32( nPort );
//		if( nPort < 0 )
//		{
//			vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
//						<< "Master did not report broadcast port - aborting" << std::endl;
//			return NULL;
//		}
//		std::string sIP;
//		m_pConnection->ReadEncodedString( sIP );
//
//		VistaUDPSocket* pSocket = new VistaUDPSocket;
//		if( pSocket->OpenSocket() ==  false )
//		{		
//			vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
//						<< "Could not open broadcast socket. This slave will report it's rediness, "
//						<< "but not wait for the common go signal" << std::endl;
//			delete pSocket;
//			pSocket = NULL;
//		}
//		else
//		{
//			pSocket->SetIsBlocking( true );
//			pSocket->SetPermitBroadcast( 1 );
//			// set reuse, so that multiple clients can listen to one BC port
//			pSocket->SetSocketReuse( true );
//
//			VistaSocketAddress oAddress( sIP, nPort );
//			if( pSocket->BindToAddress( oAddress ) == false )
//			{
//				vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
//								<< "Could not bind Broadcast-Socket to ["
//								<< sIP << ":" << nPort << "]. "
//								<< "This slave will report it's readiness, "
//								<< "but not wait for the common go signal" << std::endl;
//				delete pSocket;
//				pSocket = NULL;
//			}
//			pDataSync = new VistaBroadcastClusterFollowerDataSync( pSocket, m_bDoByteSwap, true );
//			pDataSync->WaitForConnection( m_pConnection );
//		}
//		
//	}
//	catch( VistaExceptionBase& e )
//	{
//		vstr::warnp() << "[VistaNewClusterSlave::CreateBCBarrier]: "
//					<< "Exception while creating broadcast barrier:\n" 
//					<< e.GetPrintStatement() << std::endl;
//		delete pDataSync;
//		return NULL;
//	}
//
//	return pDataSync;
//}


void VistaNewClusterSlave::HandleMasterConnectionDrop()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "Connection to master dropped!" << std::endl;
	}
	VISTA_THROW( "VistaClusterSlave: Connection to master dropped!", -1 );
}

bool VistaNewClusterSlave::CreateDefaultSyncs()
{
	m_pDefaultDataSync = CreateTypedDataSync( m_nDataSyncMethod, true );	
	m_pDefaultBarrier = CreateTypedBarrier( m_nBarrierWaitMethod, true, false );
	m_pSyncEntityObserver->Observe( m_pDefaultDataSync );
	m_pSyncEntityObserver->Observe( m_pDefaultBarrier );
	return ( m_pDefaultBarrier != NULL && m_pDefaultDataSync != NULL );
}

IVistaClusterDataCollect* VistaNewClusterSlave::CreateDataCollect()
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaNewClusterSlave::CreateNetworkSync() -- "
			<< "Could not establish connection for syncing" << std::endl;
		return NULL;
	}
	return new VistaTCPIPClusterFollowerDataCollect( pConnection, m_bDoByteSwap, true );
}

IVistaClusterDataSync* VistaNewClusterSlave::CreateZeroMQDataSync()
{
#ifdef VISTA_WITH_ZEROMQ
	std::string sAddress;
	m_pConnection->ReadEncodedString( sAddress );
	
	VistaZeroMQClusterFollowerDataSync* pFollower;
	pFollower = new VistaZeroMQClusterFollowerDataSync( sAddress, m_bDoByteSwap );

	pFollower->WaitForConnection( m_pConnection );

	return pFollower;
#else
	VISTA_THROW( "ZeroMq capability not available", -1 );
#endif
}

IVistaClusterDataSync* VistaNewClusterSlave::CreateInterProcDataSync()
{
	std::string sName = m_sInterProcName + "_" + VistaConversion::ToString( m_nUsedInterProcs );
	++m_nUsedInterProcs;

	VistaInterProcClusterFollowerDataSync* pSync = new VistaInterProcClusterFollowerDataSync( sName, true );
	try
	{
		// send our string to master (he also checks for empty strings)
		m_pConnection->WriteEncodedString( sName );
		if( m_sInterProcName.empty() )
		{
			vstr::warnp() << "VistaNewClusterMaster::CreateInterProcBarrier() -- "
						<< "no INTERPROC_NAME specified!" << std::endl;
			return NULL;
		}	
	}
	catch( VistaExceptionBase& )
	{
		vstr::warnp() << "VistaNewClusterSlave::CreateInterProcDataSync() -- exception while "
			<< "sending INTERPROC_NAME to master" << std::endl;
	}

	pSync->ConnectPipe();
	return pSync;
}

std::ostream& VistaNewClusterSlave::GetDebugStream()
{
	std::ostream& oStream = vstr::Stream( m_oClusterInfo.m_sClusterDebuggingStreamName );
	oStream << vstr::framecount << " | ";
	if( m_oClusterInfo.m_bClusterDebuggingrependTimestamp )
		oStream << vstr::relativetime << std::endl;
	return oStream;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


