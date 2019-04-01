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

#include "VistaNewClusterMaster.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/Cluster/Utils/VistaMasterDataTunnel.h>
#include <VistaKernel/Cluster/Utils/VistaGSyncSwapBarrier.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaConnectionFile.h>
#include <VistaInterProcComm/IPNet/VistaSocketAddress.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataCollect.h>
#include <VistaInterProcComm/Cluster/Imps/VistaBroadcastClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaInterProcClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaBroadcastClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaInterProcClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaRecordReplayClusterDataSync.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaAspects/VistaObserver.h>

#include <VistaTools/VistaFileSystemDirectory.h>

#include <cassert>
#include <algorithm>

#ifdef WIN32
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif 

#ifdef VISTA_WITH_ZEROMQ
#include "ZeroMQExt/VistaZeroMQClusterDataSync.h"
#include "ZeroMQExt/VistaZeroMQClusterBarrier.h"
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * representing a slave unit as entity of the master.
 * Each slave has a name, an ip and so on. Individually, a slave can sync or not.
 * The slave instance keeps book of connections and other state variables.
 */
class VistaNewClusterMaster::Slave
{
public:
	Slave( const std::string &sSlaveName,
			const int nIndex,
			const std::string& sIP,
			const int nPort )
	: m_sIP( sIP )
	, m_nPort( nPort )
	, m_pConnection( NULL )
	, m_pSyncConnection( NULL )
	, m_sName( sSlaveName )
	, m_nIndex( nIndex )
	{
	}

	~Slave()
	{
		delete m_pConnection;
		delete m_pSyncConnection;
	}

	std::string			m_sName;
	std::string			m_sIP;
	int					m_nPort;
	VistaConnectionIP*	m_pConnection;
	VistaConnectionIP*	m_pSyncConnection;
	int					m_nIndex;
};


class VistaNewClusterMaster::EventObserver : public VistaEventObserver
{
public:
	EventObserver( VistaNewClusterMaster* pClusterMaster )
	: VistaEventObserver()
	, m_pClusterMaster( pClusterMaster )
	{
	}

	~EventObserver()
	{
	}

	virtual void Notify( const VistaEvent *pEvent )
	{
		// for system events, we skip the init event
		if( pEvent->GetType() == VistaSystemEvent::GetTypeId()
			&& pEvent->GetId() == VistaSystemEvent::VSE_INIT )
			return;
		// we also only want graph updates as interaction events
		if( pEvent->GetType() == VistaInteractionEvent::GetTypeId()
			&& pEvent->GetId() != VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE )
			return;

		m_pClusterMaster->DistributeEvent( pEvent );
	}	

private:
	VistaNewClusterMaster* m_pClusterMaster;
};

class VistaNewClusterMaster::SyncEntityObserver : public IVistaObserver
{
public:
	SyncEntityObserver( VistaNewClusterMaster* pClusterMaster )
	: IVistaObserver()
	, m_pClusterMaster( pClusterMaster )
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
		if( nMsg != IVistaClusterSyncEntity::MSG_FOLLOWER_LOST )
			return;

		IVistaClusterSyncEntity* pSync = static_cast<IVistaClusterSyncEntity*>( pObserveable );
		int nID = pSync->GetLastChangedFollower();

		// find slave by name - inefficient, but hopefully this doesn't happen that often
		std::string sSlaveName = pSync->GetFollowerNameForId( nID );
		m_pClusterMaster->DeactivateSlaveAfterDrop( sSlaveName );
		// we remove it from all other registered SyncEntities, so that it doesn't clutter
		// note: we only do this for the default ones, so that user-created syncs may remain valid
		for( std::vector<IVistaClusterSyncEntity*>::iterator itEntity = m_vecSyncEntities.begin();
			itEntity != m_vecSyncEntities.end(); ++itEntity )
		{
			// we unregister before, so that we dont have a recursive notify
			m_bBlockNotifies = true;
			(*itEntity)->DeactivateFollower( sSlaveName );
			m_bBlockNotifies = false;
		}
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
	VistaNewClusterMaster*	m_pClusterMaster;
	std::vector<IVistaClusterSyncEntity*>	m_vecSyncEntities;
	bool m_bBlockNotifies;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaNewClusterMaster::VistaNewClusterMaster( VistaSystem *pVistaSystem,
										const std::string& sMasterName )
: m_pVistaSystem( pVistaSystem )
, m_sMasterName( sMasterName )
, m_sMasterSectionName( sMasterName )
, m_pDefaultBarrier( NULL )
, m_pDefaultDataSync( NULL )
, m_pSwapSyncBarrier( NULL )
, m_pEventObserver( NULL )
, m_nBarrierWaitMethod( VistaMasterSlave::BARRIER_BROADCAST )
, m_nSwapSyncMethod( VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER )
, m_nDataSyncMethod( VistaMasterSlave::DATASYNC_TCP )
, m_nSwapSyncTimeout( 0 )
, m_nRecordSyncCounter( 0 )
, m_nMaxConsecutiveBarrierSyncFailures( 10 )
{
	m_pEventObserver = new EventObserver( this );
	m_pSyncEntityObserver = new SyncEntityObserver( this );

	// Register Events that should be sync'ed in cluster mode
	// @todo: memleaks
	m_oMessage.RegisterEventType( new VistaSystemEvent );
	m_oMessage.RegisterEventType( new VistaInteractionEvent( pVistaSystem->GetInteractionManager() ) );
	m_oMessage.RegisterEventType( new VistaExternalMsgEvent );
}

VistaNewClusterMaster::~VistaNewClusterMaster()
{
	// The DisplayBridge - and thus the window - has already been destroyed, so that
	// we don't have to explicitly leave the gsync swap barrier here

	delete m_pDefaultDataSync;
	delete m_pDefaultBarrier;
	delete m_pSwapSyncBarrier;

	delete m_pEventObserver;

	for( std::vector<Slave*>::iterator itSlave = m_vecSlaves.begin();
			itSlave != m_vecSlaves.end(); ++itSlave )
	{
		delete (*itSlave);
	}
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaNewClusterMaster::Init( const std::string& sClusterSection,
								const VistaPropertyList& oConfig )
{
	vstr::outi() << "[VistaNewClusterMaster]: Initializing" << std::endl;
	vstr::IndentObject oIndent;

	m_dFrameClock = VistaTimeUtils::GetStandardTimer().GetSystemTime();

	m_sMasterSectionName = sClusterSection;

	if( oConfig.HasSubList( sClusterSection ) == false )
	{
		vstr::errp() << "Cluster Master cannot be initialized - section ["
			<< sClusterSection << "] does not exist!" << std::endl;
		return false;
	}
	const VistaPropertyList& oSection = oConfig.GetSubListConstRef( sClusterSection );

	m_sMasterName = oSection.GetValueOrDefault<std::string>( "NAME", sClusterSection );
	vstr::outi() << "Setting master name to [" << m_sMasterName << "]" << std::endl;

	ParseParameters( oSection );

	std::list<std::string> liSlaveList;
	oSection.GetValue( "SLAVES", liSlaveList );
	for( std::list<std::string>::const_iterator itSlave = liSlaveList.begin();
		 itSlave != liSlaveList.end(); ++itSlave )
	{
		if( oConfig.HasSubList( (*itSlave) ) == false )
		{
			vstr::warnp() << "Slave [" << (*itSlave)
						<< "] specified, but no section exists" << std::endl;
			continue;
		}
		const VistaPropertyList& oSlaveConfig = oConfig.GetSubListConstRef( (*itSlave) );

		std::string sSlaveIP;
		if( oSlaveConfig.GetValue( "SLAVEIP", sSlaveIP ) == false )
		{
			vstr::warnp() << "No IP given for slave in section ["
					  << *itSlave << "]" << std::endl;
			continue;
		}

		int nPort;
		if( oSlaveConfig.GetValue( "SLAVEPORT", nPort ) == false )
		{
			vstr::warnp() << "No valid port for slave ["
					  << sSlaveIP << "] from section ["
					  << *itSlave << "]" << std::endl;
			continue;
		}
		ConnectToSlave( (*itSlave), sSlaveIP, nPort );
	}

	// Create Default Sync and Barrier
	
	vstr::outi() << "Creating default sync entities" << std::endl;
	if( CreateDefaultSyncs() == false )
	{
		vstr::errp() << "Could not create default sync objects - aborting" << std::endl;
		return false;
	}

	InitSwapSync();	

	// sync first frame clock
	m_pDefaultDataSync->SyncTime( m_dFrameClock );

	TransmitClusterSetupInfo();

	// Last but not least: let's register with the desired events
	VistaEventManager* pEventManager = m_pVistaSystem->GetEventManager();
	pEventManager->RegisterObserver( m_pEventObserver, VistaSystemEvent::GetTypeId() );
	pEventManager->RegisterObserver( m_pEventObserver, VistaExternalMsgEvent::GetTypeId() );
	pEventManager->RegisterObserver( m_pEventObserver, VistaInteractionEvent::GetTypeId() );

	PrintClusterSetupInfo();

	vstr::outi() << "[VistaNewClusterMaster]: Initialization finished" << std::endl;

	return true;
}

bool VistaNewClusterMaster::Init( const std::string& sRecordDataFolder )
{
	vstr::outi() << "[VistaNewClusterMaster]: Initializing as record-only master" << std::endl;
	vstr::IndentObject oIndent;

	m_dFrameClock = VistaTimeUtils::GetStandardTimer().GetSystemTime();

	m_sMasterName = "RECORD_MASTER";

	SetRecordDataFolder( sRecordDataFolder );

	// for simple recording, we only need dummy barriers & syncs
	m_nBarrierWaitMethod = VistaMasterSlave::BARRIER_DUMMY;
	m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER;
	m_nDataSyncMethod = VistaMasterSlave::DATASYNC_DUMMY;

	// Create Default Sync and Barrier
	
	vstr::outi() << "Creating default sync entities" << std::endl;
	if( CreateDefaultSyncs() == false )
	{
		vstr::errp() << "Could not create default sync objects - aborting" << std::endl;
		return false;
	}

	InitSwapSync();	

	// sync first frame clock
	m_pDefaultDataSync->SyncTime( m_dFrameClock );

	TransmitClusterSetupInfo();

	// Last but not least: let's register with the desired events
	VistaEventManager* pEventManager = m_pVistaSystem->GetEventManager();
	pEventManager->RegisterObserver( m_pEventObserver, VistaSystemEvent::GetTypeId() );
	pEventManager->RegisterObserver( m_pEventObserver, VistaExternalMsgEvent::GetTypeId() );
	pEventManager->RegisterObserver( m_pEventObserver, VistaInteractionEvent::GetTypeId() );

	PrintClusterSetupInfo();

	vstr::outi() << "[VistaNewClusterMaster]: Initialization finished" << std::endl;

	return true;
}

void VistaNewClusterMaster::ParseParameters( const VistaPropertyList& oSection )
{
	// broadcast settings
	std::string sPortListEntry;
	if( oSection.GetValue( "BROADCASTPORTS", sPortListEntry ) )
	{
		VistaMasterSlave::FillPortListFromIni( sPortListEntry, m_vecFreeBroadcastPorts );
	}
	m_sBroadcastIP = oSection.GetValueOrDefault<std::string>( "BROADCASTIP", "" );
	m_nNumBroadcastGroups = oSection.GetValueOrDefault<int>( "BROADCASTGROUPS", 1 );
	if( oSection.GetValue( "ZEROMQ_PORTS", sPortListEntry ) )
	{
		VistaMasterSlave::FillPortListFromIni( sPortListEntry, m_vecZeroMQPorts );
	}
	oSection.GetValue<std::string>( "ZEROMQ_ADDRESS", m_sZeroMQAddress );

	m_nMaxConsecutiveBarrierSyncFailures = oSection.GetValueOrDefault<int>( "MAX_ALLOWED_BARRIER_FAILURES", 10 );

	m_oClusterInfo.m_bClusterDebuggingActive = oSection.GetValueOrDefault( "DEBUG_OUTPUT", false );
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		m_oClusterInfo.m_bClusterDebuggingrependTimestamp = oSection.GetValueOrDefault( "DEBUG_PREPEND_TIMESTAMP", true );
		m_oClusterInfo.m_sClusterDebuggingStreamName = oSection.GetValueOrDefault< std::string >( "DEBUG_STREAM_NAME", "ClusterDebugging" );
		m_oClusterInfo.m_sClusterDebuggingFile = oSection.GetValueOrDefault( "DEBUG_OUTPUT_FILE", m_oClusterInfo.m_sClusterDebuggingStreamName );
		vstr::GetStreamManager()->AddNewLogFileStream( m_oClusterInfo.m_sClusterDebuggingStreamName,
													m_oClusterInfo.m_sClusterDebuggingFile,
													"log", true, true, false );
	}

	ParseDataSyncType( oSection );
	ParseSwapSyncType( oSection );
	ParseBarrierType( oSection );

	if( m_sRecordDataFolder.empty() ) // if not already set explicitely
	{
		SetRecordDataFolder( oSection.GetValueOrDefault<std::string>( "RECORD", "" ) );
	}
}

void VistaNewClusterMaster::ParseDataSyncType( const VistaPropertyList& oSection )
{
	std::string sDataSyncType = oSection.GetValueOrDefault<std::string>( "DATASYNCTYPE", "TCP/IP" );
	VistaAspectsComparisonStuff::StringCompareObject oCmp( false );

	if( oCmp( sDataSyncType, "DUMMY" ) )
	{
		m_nDataSyncMethod = VistaMasterSlave::DATASYNC_DUMMY;
	}
	else if( oCmp( sDataSyncType, "TCP" ) || oCmp( sDataSyncType, "TCPIP" ) || oCmp( sDataSyncType, "TCP/IP" ) )
	{
		m_nDataSyncMethod = VistaMasterSlave::DATASYNC_TCP;
	}
	// Broadcast Datasync is unreliable, so we dont allow it natively
	//else if( oCmp( sDataSyncType, "BROADCAST" ) || oCmp( sDataSyncType, "BC" ) || sDataSyncType.empty() )
	//{
	//	m_nDataSyncMethod = VistaMasterSlave::DATASYNC_BROADCAST;
	//	if( m_sBroadcastIP.empty() )
	//	{
	//		vstr::warnp() << "DataSyncType BROADCAST required BROADCASTIP to be specified - "
	//					<< "falling back to TCP DataSync" << std::endl;
	//		m_nDataSyncMethod = VistaMasterSlave::DATASYNC_TCP;
	//	}	
	//	if( m_vecFreeBroadcastPorts.empty() )
	//	{
	//		vstr::warnp() << "No BROADCASTPORTS defined - using default [24000-24999]" << std::endl;
	//		m_vecFreeBroadcastPorts.push_back( VistaMasterSlave::PortRange( 24000, 24999 ) );
	//	}	
	//}
	else if( oCmp( sDataSyncType, "ZEROMQ" ) || oCmp( sDataSyncType, "ZMQ" ) )
	{
		if( m_vecZeroMQPorts.empty() || m_sZeroMQAddress.empty() )
		{
			vstr::warnp() << "DataSyncType ZEROMQ requires setting ZEROMQ_ADDRESS"
						<< " and ZEROMQ_PORTS" << std::endl;
		}
		else
		{
			m_nDataSyncMethod = VistaMasterSlave::DATASYNC_ZEROMQ;
		}
	}
	else if( oCmp( sDataSyncType, "INTERPROC" ) )
	{
		m_nDataSyncMethod = VistaMasterSlave::DATASYNC_INTERPROC;
	}
	else
	{
		vstr::warnp() << "Unknown DATASYNCTYPE value [" << sDataSyncType
					<< "] - using default TCP sync" << std::endl;
		m_nSwapSyncMethod = VistaMasterSlave::DATASYNC_TCP;
	}
}

void VistaNewClusterMaster::ParseSwapSyncType( const VistaPropertyList& oSection )
{
	std::string sSyncType = oSection.GetValueOrDefault<std::string>( "SWAPSYNCTYPE", "BC" );
	VistaAspectsComparisonStuff::StringCompareObject oCmp( false );

	if( oCmp( sSyncType, "NONE" ) || oCmp( sSyncType, "FALSE" ) )
	{
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_NONE;
	}
	else if( oCmp( sSyncType, "DUMMY" ) )
	{
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_DUMMY;
	}
	else if( oCmp( sSyncType, "TCP" ) || oCmp( sSyncType, "TCPIP" ) || oCmp( sSyncType, "TCP/IP" ) )
	{
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_TCP;
	}
	else if( oCmp( sSyncType, "BROADCAST" ) || oCmp( sSyncType, "BC" ) )
	{
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_BROADCAST;
	}
	else if( oCmp( sSyncType, "DEFAULT" ) || oCmp( sSyncType, "TRUE" ) || oCmp( sSyncType, "BARRIER" ) || sSyncType.empty() )
	{
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER;
	}
	else if( oCmp( sSyncType, "GSYNC" ) || oCmp( sSyncType, "GSYNCGROUP" ) || oCmp( sSyncType, "NVIDIA" ) )
	{
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_GSYNCGROUP;
	}
	else if( oCmp( sSyncType, "MUTEX" ) | oCmp( sSyncType, "INTERPROC" ) )
	{
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_INTERPROC;
	}
	else if( oCmp( sSyncType, "ZEROMQ" ) | oCmp( sSyncType, "ZMQ" ) )
	{
		if( m_vecZeroMQPorts.empty() || m_sZeroMQAddress.empty() )
		{
			vstr::warnp() << "BarrierType ZEROMQ requires setting ZEROMQ_ADDRESS"
						<< " and ZEROMQ_PORTS" << std::endl;
		}
		else
		{
			m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_ZEROMQ;
		}
	}
	else
	{
		vstr::warnp() << "Unknown SWAPSYNC value [" << sSyncType
					<< "] - using default sync" << std::endl;
		m_nSwapSyncMethod = VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER;
	}
}

void VistaNewClusterMaster::ParseBarrierType( const VistaPropertyList& oSection )
{
	std::string sBarrierType = oSection.GetValueOrDefault<std::string>( "BARRIERTYPE", "BC" );
	VistaAspectsComparisonStuff::StringCompareObject oCmp( false );

	if( oCmp( sBarrierType, "DUMMY" ) )
	{
		m_nBarrierWaitMethod = VistaMasterSlave::BARRIER_DUMMY;
	}
	else if( oCmp( sBarrierType, "TCP" ) || oCmp( sBarrierType, "TCPIP" ) || oCmp( sBarrierType, "TCP/IP" ) )
	{
		m_nBarrierWaitMethod = VistaMasterSlave::BARRIER_TCP;
	}
	else if( oCmp( sBarrierType, "BROADCAST" ) || oCmp( sBarrierType, "BC" ) || sBarrierType.empty() )
	{
		m_nBarrierWaitMethod = VistaMasterSlave::BARRIER_BROADCAST;
		if( m_sBroadcastIP.empty() )
		{
			vstr::warnp() << "BarrierType BROADCAST required BROADCASTIP to be specified - "
						<< "falling back to TCP Barrier" << std::endl;
			m_nBarrierWaitMethod = VistaMasterSlave::BARRIER_TCP;
		}	
		if( m_vecFreeBroadcastPorts.empty() )
		{
			vstr::warnp() << "No BROADCASTPORTS defined - using default [24000-24999]" << std::endl;
			m_vecFreeBroadcastPorts.push_back( VistaMasterSlave::PortRange( 24000, 24999 ) );
		}	
	}
	else if( oCmp( sBarrierType, "MUTEX" ) || oCmp( sBarrierType, "INTERPROC" ) )
	{
		m_nBarrierWaitMethod = VistaMasterSlave::BARRIER_INTERPROC;
	}
	else if( oCmp( sBarrierType, "ZEROMQ" ) || oCmp( sBarrierType, "ZMQ" ) )
	{
		if( m_vecZeroMQPorts.empty() || m_sZeroMQAddress.empty() )
		{
			vstr::warnp() << "BarrierType ZEROMQ requires setting ZEROMQ_ADDRESS"
						<< " and ZEROMQ_PORTS" << std::endl;
		}
		else
		{
			m_nBarrierWaitMethod = VistaMasterSlave::BARRIER_ZEROMQ;
		}
	}
	else
	{
		vstr::warnp() << "Unknown BARRIERTYPE value [" << sBarrierType
					<< "] - using default barrier type TCP" << std::endl;
		m_nSwapSyncMethod = VistaMasterSlave::BARRIER_TCP;
	}

}

bool VistaNewClusterMaster::PostInit()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "PostInit" << std::endl;
	}

	vstr::outi() << "VistaNewClusterMaster::PostInit()" << std::endl;
	m_pDefaultBarrier->BarrierWait();
	
	vstr::outi() << "VistaNewClusterMaster::PostInit() postbarrierwait" << std::endl;

	// now (after windows have been created) we can set up the gsync-barrier, if desired
	if( m_nSwapSyncMethod == VistaMasterSlave::SWAPSYNC_GSYNCGROUP )
	{
		if( VistaGSyncSwapBarrier::JoinSwapBarrier( m_pVistaSystem->GetDisplayManager() ) == false )
		{
			// It didn't succeed, but that's okay as long as the slaves joined
			vstr::warnp() << "[VistaClusterMaster]: Did not join SwapSync group - Master not on GSync ring" << std::endl;
		}
	}

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    WaitForClusterMaster finished" << std::endl;
	}
	
	return true;
}

int VistaNewClusterMaster::GetClusterMode() const
{
	return CM_MASTER_SLAVE;
}
std::string VistaNewClusterMaster::GetClusterModeName() const
{
	return "MASTER_SLAVE";
}

int VistaNewClusterMaster::GetNodeType() const
{
	return NT_MASTER;
}
std::string VistaNewClusterMaster::GetNodeTypeName() const
{
	return "MASTER";
}
std::string VistaNewClusterMaster::GetNodeName() const
{
	return m_sMasterName;
}
std::string VistaNewClusterMaster::GetConfigSectionName() const
{
	return m_sMasterSectionName;
}

int VistaNewClusterMaster::GetNodeID() const
{
	// we are always ID 0, slaves start at 1
	return 0;
}

bool VistaNewClusterMaster::GetIsLeader() const
{
	return true;
}
bool VistaNewClusterMaster::GetIsFollower() const
{
	return false;
}

bool VistaNewClusterMaster::StartFrame()
{
	++m_iFrameCount;
	m_oMessage.SetFrameCount( m_iFrameCount );
	m_dFrameClock = VistaTimeUtils::GetStandardTimer().GetSystemTime();
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "Starting Frame " << m_iFrameCount << ", frameclock "
						<< vstr::formattime( m_dFrameClock, 6 ) << std::endl;
	}
	m_oMessage.SetStartFrameMsg( m_dFrameClock );
	m_pDefaultDataSync->SyncData( m_oMessage );
	return true;
}

bool VistaNewClusterMaster::ProcessFrame()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "Process frame" << std::endl;
	}
	return true;
}

bool VistaNewClusterMaster::EndFrame()
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "End frame" << std::endl;
	}
	m_oMessage.SetEndFrameMsg( VistaTimeUtils::GetStandardTimer().GetSystemTime() );
	m_pDefaultDataSync->SyncData( m_oMessage );
	return true;
}


void VistaNewClusterMaster::SwapSync()
{
	VistaKernelProfileScope( "MASTER_SWAPSYNC" );

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "SwapSync" << std::endl;
	}
	
	switch( m_nSwapSyncMethod )
	{	
		case VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER:
		{
			if( m_pDefaultBarrier )
				m_pDefaultBarrier->BarrierWait( m_nSwapSyncTimeout );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_GSYNCGROUP:
		{
			// syncing is done by the graphics card on buffer swap
			break;
		}
		case VistaMasterSlave::SWAPSYNC_NONE:
		{			
			break;
		}
		case VistaMasterSlave::SWAPSYNC_BROADCAST:
		case VistaMasterSlave::SWAPSYNC_DUMMY:
		case VistaMasterSlave::SWAPSYNC_TCP:
		case VistaMasterSlave::SWAPSYNC_ZEROMQ:
		case VistaMasterSlave::SWAPSYNC_INTERPROC:
		default:
		{
			if( m_pSwapSyncBarrier )
				m_pSwapSyncBarrier->BarrierWait( m_nSwapSyncTimeout );
			break;
		}
	};

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    swapsync finished" << std::endl;
	}
}


bool VistaNewClusterMaster::CreateConnections(
					std::vector<VistaConnectionIP*>& vecConnections )
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "CrateConnections" << std::endl;
	}
	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		VistaConnectionIP* pConnection = CreateConnectionToSlave( (*itSlave) );	
		if( pConnection == NULL )
		{
			vstr::errp() << "VistaNewClusterMaster::CreateIPConnections() -- "
					<< "Could not establish connection to ["
					<< (*itSlave)->m_sName << "]" << std::endl;
			continue;
		}
		vecConnections.push_back( pConnection );
	}
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    CreateConnections finished" << std::endl;
	}
	return true;
}

bool VistaNewClusterMaster::CreateNamedConnections(
				std::vector<std::pair<VistaConnectionIP*, std::string> >& vecConnections )
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "CreateNamedConnections" << std::endl;
	}
	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		VistaConnectionIP* pConnection = CreateConnectionToSlave( (*itSlave) );	
		if( pConnection == NULL )
		{
			vstr::errp() << "VistaNewClusterMaster::CreateIPConnections() -- "
					<< "Could not establish connection to ["
					<< (*itSlave)->m_sName << "]" << std::endl;
			continue;
		}
		vecConnections.push_back( std::pair<VistaConnectionIP*, std::string>( 
												pConnection, (*itSlave)->m_sName ) );
	}
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    CreateNamedConnections finished" << std::endl;
	}
	return true;
}

IVistaDataTunnel* VistaNewClusterMaster::CreateDataTunnel( IDLVistaDataPacket* pPacketProto )
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "CreateDataTunnel" << std::endl;
	}
	std::vector<VistaConnectionIP*> vecConnections;
	if( CreateConnections( vecConnections ) == false || vecConnections.empty() )
	{
		vstr::errp() << "VistaNewClusterMaster::CreateDataTunnel() -- "
					<< "Could not create data tunnel" << std::endl;
		return NULL;
	}	

	//create the tunnel itself
	VistaMasterDataTunnel* pNewTunnel = new VistaMasterDataTunnel( vecConnections );

	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    CreateDataTunnel finished" << std::endl;
	}

	return pNewTunnel;
}

bool VistaNewClusterMaster::ConnectToSlave( const std::string& sName,
									const std::string &sIP, int nPort )
{		
	vstr::outi() << "Connecting to slave [" << sName << "] at ["
				<< sIP << ":" << nPort << "] ..." << std::flush;
	vstr::IndentObject oIndent;
	int nSlaveIndex = (int)m_vecSlaves.size() + 1;

	Slave* pSlave = new Slave( sName, nSlaveIndex, sIP, nPort );
	m_vecSlaves.push_back( pSlave );

	VistaConnectionIP* pConn = NULL;
	VistaConnectionIP* pSyncConn = NULL;
	try
	{
		pConn = new VistaConnectionIP( VistaConnectionIP::CT_TCP, sIP, nPort );

		// since the slaves know if they have to swap, we never do so
		pConn->SetByteorderSwapFlag( VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );
		if( pConn->GetIsOpen() == false )
		{
			// connection failed.
			vstr::outi() << std::endl;
			vstr::errp() << " Failed to init connection to slave at [" << sIP << ":" 
					<< nPort << "]" << std::endl;
			delete pConn;
			return false;
		}

		pConn->SetIsBlocking( true );
		pConn->SetIsBuffering( false );

		// connection established.
		// write initial frame clock
		vstr::outi() << " connection successfull" << std::endl;

		// Write initial information:
		// a 1 to detetmine enfianess/need for byte swap
		// curretn frame clock
		// slave's index
		pConn->WriteInt32( 1 );
		pConn->WriteDouble( m_dFrameClock );
		pConn->WriteInt32( nSlaveIndex );
		
		pConn->ReadEncodedString( pSlave->m_sName );

		pConn->WriteInt32( (VistaType::sint32)~0 ); // Dummy, this should be the SyncMode
		pConn->WriteInt32( (VistaType::sint32)m_nBarrierWaitMethod );
		pConn->WriteInt32( (VistaType::sint32)m_nSwapSyncMethod );
		pConn->WriteInt32( (VistaType::sint32)m_nDataSyncMethod );
	}
	catch( VistaExceptionBase& )
	{
		vstr::outi() << std::endl;
		m_vecDeadSlaves.push_back( pSlave );
		delete pConn;
		pConn = NULL;
		vstr::errp() << " Failed to init event channel at [" << pSlave->m_sIP << ":" 
			<< pSlave->m_nPort << "]" << std::endl;
	}	
	try
	{
		// we establish another connection - for default sync entities
		VistaType::sint32 nSyncConnPort;
		pConn->ReadInt32( nSyncConnPort );
		pSyncConn = new VistaConnectionIP( VistaConnectionIP::CT_TCP, sIP, nSyncConnPort );
		pSyncConn->SetByteorderSwapFlag( VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );
		if( pSyncConn->GetIsOpen() == false )
		{
			// connection failed.
			vstr::errp() << " Failed to init sync connection to slave at [" << sIP << ":" 
					<< nSyncConnPort << "]" << std::endl;
			delete pConn;
			delete pSyncConn;
			pConn = NULL;
			pSyncConn = NULL;
			return false;
		}
		pSyncConn->SetIsBlocking( true );
		pSyncConn->SetIsBuffering( false );
	}
	catch( VistaExceptionBase& )
	{
		m_vecDeadSlaves.push_back( pSlave );
		delete pConn;
		delete pSyncConn;
		vstr::errp() << " Failed to init event channel at [" << pSlave->m_sIP << ":" 
			<< pSlave->m_nPort << "]" << std::endl;
	}

	

	pSlave->m_pConnection = pConn;
	pSlave->m_pSyncConnection = pSyncConn;
	m_vecActiveSlaves.push_back( pSlave );
	
	return true;
}

IVistaClusterDataSync* VistaNewClusterMaster::CreateDataSync()
{
	return CreateTypedDataSync( m_nDataSyncMethod, false );	
}

IVistaClusterDataSync* VistaNewClusterMaster::GetDefaultDataSync()
{
	return m_pDefaultDataSync;
}

IVistaClusterBarrier* VistaNewClusterMaster::CreateBarrier()
{
	return CreateTypedBarrier( m_nBarrierWaitMethod, false, false );
}


IVistaClusterBarrier* VistaNewClusterMaster::GetDefaultBarrier()
{
	return m_pDefaultBarrier;
}


int VistaNewClusterMaster::GetNextFreeBroadcastPort()
{
	return VistaMasterSlave::GetFreePortFromPortList( m_vecFreeBroadcastPorts );
	// @todo: verify port availability
}

int VistaNewClusterMaster::GetNextFreeZeroMQPort()
{
	return VistaMasterSlave::GetFreePortFromPortList( m_vecZeroMQPorts );
	// @todo: verify port availability
}

VistaConnectionIP* VistaNewClusterMaster::CreateConnectionToSlave( Slave* pSlave )
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "CreateConnectionToSlave " << pSlave->m_sName << std::endl;
	}
	try
	{
		int nPort = -1;
		if( pSlave->m_pConnection->ReadInt32( nPort ) != sizeof(VistaType::sint32) )
		{
			vstr::errp() << "[VistaNewClusterMaster]:"
				<< "Could not create connection to slave ["
				<< pSlave->m_sName << "] - no port received" << std::endl;
			return NULL;
		}
		if( nPort == -1 )
		{
			vstr::warnp()  << "[VistaNewClusterMaster]:"
				<< "Could not create connection to slave ["
				<< pSlave->m_sName << "] - no free port left on slave" << std::endl;
			return NULL;
		}
		else if( nPort < 0 )
		{
			vstr::warnp()  << "[VistaNewClusterMaster]:"
				<< "Could not create connection to slave ["
				<< pSlave->m_sName << "] - unspecified error" << std::endl;
			return NULL;
		}

		VistaConnectionIP* pConnection = new VistaConnectionIP( VistaConnectionIP::CT_TCP,
																pSlave->m_sIP, nPort );
		if( pConnection->GetIsConnected() == false && pConnection->Connect() == false )
		{
			vstr::errp() << "[VistaNewClusterMaster]:"
						<< "Could not create connection to slave ["
						<< pSlave->m_sName << "] at Host [" << pSlave->m_sIP 
						<< "] - Port [" << nPort << "]" << std::endl;
			return NULL;
		}
		pConnection->SetIsBlocking( true );
		pConnection->SetIsBuffering( false );
		pConnection->SetByteorderSwapFlag( VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );

		if( m_oClusterInfo.m_bClusterDebuggingActive )
		{
			GetDebugStream() << "    CreateConnectionToSlave " << pSlave->m_sName << " finished" << std::endl;
		}

		return pConnection;
	}
	catch( VistaExceptionBase& oException )
	{
		if( m_oClusterInfo.m_bClusterDebuggingActive )
		{
			GetDebugStream() << "    CreateConnectionToSlave " << pSlave->m_sName << " failed" << std::endl;
		}
		vstr::errp() << "VistaNewClusterMaster::CreateConnectionToSlave() -- "
			<< "Failed with exception\n" << oException.GetExceptionText() <<  std::endl;
	}
	return NULL;
}

bool VistaNewClusterMaster::InitSwapSync()
{
	switch( m_nSwapSyncMethod )
	{
		case VistaMasterSlave::SWAPSYNC_DEFAULTBARRIER:
		case VistaMasterSlave::SWAPSYNC_NONE:
		{
			// nothing to do
			break;
		}
		case VistaMasterSlave::SWAPSYNC_BROADCAST:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_BROADCAST, true, true );
			m_pSyncEntityObserver->Observe( m_pSwapSyncBarrier );
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
			m_pSyncEntityObserver->Observe( m_pSwapSyncBarrier );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_INTERPROC:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_INTERPROC, true, true );
			m_pSyncEntityObserver->Observe( m_pSwapSyncBarrier );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_ZEROMQ:
		{
			m_pSwapSyncBarrier = CreateTypedBarrier( VistaMasterSlave::BARRIER_ZEROMQ, true, true );
			m_pSyncEntityObserver->Observe( m_pSwapSyncBarrier );
			break;
		}
		case VistaMasterSlave::SWAPSYNC_GSYNCGROUP:
		{
			// we delay this to the PostInit call - we have to wait for the windows to exist
			vstr::out() << "chose GSYNC swapping" << std::endl;
			break;
		}
		default:
			VISTA_THROW( "Unknown SwapSync mode", -1 );
	}
	return true;
}

void VistaNewClusterMaster::TransmitClusterSetupInfo()
{
	// first: create correct clusterinfo
	m_oClusterInfo.m_vecNodeInfos.resize( m_vecSlaves.size() + 1 );
	
	// entry 0 is us
	NodeInfo& oMasterInfo = m_oClusterInfo.m_vecNodeInfos[0];
	oMasterInfo.m_sNodeName = m_sMasterName;
	oMasterInfo.m_iNodeID = 0;
	oMasterInfo.m_eNodeType = NT_MASTER;
	oMasterInfo.m_bIsActive = true;

	for( std::size_t i = 0; i < m_vecSlaves.size(); ++i )
	{
		NodeInfo& oSlaveInfo = m_oClusterInfo.m_vecNodeInfos[i + 1];
		oSlaveInfo.m_sNodeName = m_vecSlaves[i]->m_sName;
		oSlaveInfo.m_iNodeID = (int)i + 1;
		oSlaveInfo.m_eNodeType = NT_SLAVE;
		oSlaveInfo.m_bIsActive = ( m_vecSlaves[i]->m_pConnection != NULL );
	}

	// now, sync this so that slaves have the same info
	m_pDefaultDataSync->SyncData( m_oClusterInfo );
}

IVistaClusterBarrier* VistaNewClusterMaster::CreateTypedBarrier( int nType,
														bool bUseDefaultConnection,
														bool bIsSwapSyncBarrier )
{
	IVistaClusterBarrier* pBarrier;
	switch( nType )
	{
		case VistaMasterSlave::BARRIER_DUMMY:
		{
			pBarrier = new VistaDummyClusterBarrier;
			break;
		}
		case VistaMasterSlave::BARRIER_TCP:
		{			
			pBarrier = CreateTCPBarrier( bUseDefaultConnection, bIsSwapSyncBarrier );
			break;
		}
		case VistaMasterSlave::BARRIER_BROADCAST:
		{			
			pBarrier = CreateBroadcastBarrier( bUseDefaultConnection, bIsSwapSyncBarrier );
			break;
		}
		case VistaMasterSlave::BARRIER_INTERPROC:
		{			
			pBarrier = CreateInterProcBarrier( bIsSwapSyncBarrier );
			break;
		}
		case VistaMasterSlave::BARRIER_ZEROMQ:
		{			
			pBarrier = CreateZeroMQBarrier( bUseDefaultConnection, bIsSwapSyncBarrier );
			break;
		}
		default:
			VISTA_THROW( "VistaClusterMaster undefined barrier type", -1 )
	}
	ProcessSyncEntityDelay();

	if( pBarrier == NULL )
	{
		vstr::warnp() << "Barrier creation failed - using DummyBarrier instead" << std::endl;
		pBarrier = new VistaDummyClusterBarrier;
	}
	else if( pBarrier->GetIsValid() == false )
	{
		vstr::warnp() << "[VistaNewClusterMaster::CreateBarrier]: "
					<< "BarrierWait of type [" << pBarrier->GetBarrierType()
					<< "] is invalid" << std::endl;
	}
	return pBarrier;
}

IVistaClusterBarrier* VistaNewClusterMaster::CreateTCPBarrier( bool bUseDefaultConnection,
														bool bIsSwapSyncBarrier )
{
	VistaTCPIPClusterLeaderBarrier* pBarrier = new VistaTCPIPClusterLeaderBarrier( true );

	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		if( CheckSyncEntityDelay( (*itSlave) ) )
			continue;

		VistaConnectionIP* pConnection;
		if( bUseDefaultConnection )
			pConnection = (*itSlave)->m_pSyncConnection;
		else
			pConnection = CreateConnectionToSlave( (*itSlave) );
		if( pConnection == NULL )
		{
			vstr::warnp() << "VistaNewClusterMaster::CreateTCPBarrier() -- "
					<< "Could not establish connection to ["
					<< (*itSlave)->m_sName << "] - syncing on it will not work" << std::endl;
			continue;
		}
		pBarrier->AddFollower( (*itSlave)->m_sName, pConnection, !bUseDefaultConnection );
		pBarrier->SetAllowedConsecutiveSyncFailures( m_nMaxConsecutiveBarrierSyncFailures );
	}

	return pBarrier;
}


IVistaClusterBarrier* VistaNewClusterMaster::CreateZeroMQBarrier( bool bUseDefaultConnection, 
																bool bIsSwapSyncBarrier )
{
#ifdef VISTA_WITH_ZEROMQ
	int nPort = GetNextFreeZeroMQPort();
	std::string sAddress = m_sZeroMQAddress + ":" + VistaConversion::ToString( nPort );

	VistaZeroMQClusterLeaderBarrier* pBarrier = new VistaZeroMQClusterLeaderBarrier( sAddress, true );

	std::vector< VistaConnectionIP* > vecWaitConnections;
	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		if( CheckSyncEntityDelay( (*itSlave) ) )
			continue;

		VistaConnectionIP* pConnection;
		if( bUseDefaultConnection )
			pConnection = (*itSlave)->m_pSyncConnection;
		else
			pConnection = CreateConnectionToSlave( (*itSlave) );
		if( pConnection == NULL )
		{
			vstr::warnp() << "VistaNewClusterMaster::CreateZeroMQBarrier() -- "
					<< "Could not establish connection to ["
					<< (*itSlave)->m_sName << "] - syncing on it will not work" << std::endl;
			continue;
		}
		pBarrier->AddFollower( (*itSlave)->m_sName, pConnection, !bUseDefaultConnection );
		pBarrier->SetAllowedConsecutiveSyncFailures( m_nMaxConsecutiveBarrierSyncFailures );
		pConnection->WriteEncodedString( sAddress );
		vecWaitConnections.push_back( pConnection );
	}
	pBarrier->WaitForConnection( vecWaitConnections );

	return pBarrier;
#else
	VISTA_THROW( "ZeroMQ capability not available", -1 );
#endif
}


IVistaClusterBarrier* VistaNewClusterMaster::CreateBroadcastBarrier( bool bUseDefaultConnection,
														bool bIsSwapSyncBarrier )
{
	std::vector<VistaUDPSocket*> vecSockets;
	std::vector<int> vecBCPorts;
	
	// find free ports
	for( int i = 0; i < m_nNumBroadcastGroups; ++i )
	{
		int nPort = GetNextFreeBroadcastPort();
		if( nPort < 0 )
		{
			vstr::warnp() << "[VistaNewClusterMaster::CreateClusterBarrier]: "
					<< "No more free broadcast port available, aborting"
					<< std::endl;
			return NULL;
		}
		vecBCPorts.push_back( nPort );
	}

	// create broadcast sockets
	bool bSuccess = true;
	for( int i = 0; i < m_nNumBroadcastGroups; ++i )
	{
		VistaSocketAddress oAdress( m_sBroadcastIP, vecBCPorts[i] );		
		if( oAdress.GetIsValid() == false )
		{
			vstr::warnp() << "[VistaNewClusterMaster::CreateBCBarrier]: "
					<< "Could not open swap sync socket. Address ["
					<< m_sBroadcastIP << ":" << vecBCPorts[i] << "] is invalid " << std::endl;
			bSuccess = false;
			break;
		}

		VistaUDPSocket *pSyncSocket = new VistaUDPSocket;
		if( pSyncSocket->OpenSocket() == false )
		{
			vstr::warnp() << "[VistaNewClusterMaster::CreateBCBarrier]: "
						<< "Could not open socket" << std::endl;
			bSuccess = false;
			break;
		}
		pSyncSocket->SetPermitBroadcast(1);
		if( pSyncSocket->ConnectToAddress( oAdress ) == false )
		{
			vstr::warnp() << "[VistaNewClusterMaster::CreateBCBarrier]: "
						<< "Could not connect bc-socket to address ["
						<< m_sBroadcastIP << ":" << vecBCPorts[i] << "]" << std::endl;
			bSuccess = false;
			break;
		}

		vecSockets.push_back( pSyncSocket );
	}

	if( bSuccess )
	{
		VistaBroadcastClusterLeaderBarrier* pBarrier = new VistaBroadcastClusterLeaderBarrier( vecSockets, true );

		for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
				itSlave != m_vecActiveSlaves.end(); ++itSlave )
		{
			if( CheckSyncEntityDelay( (*itSlave) ) )
				continue;

			VistaConnectionIP* pConnection;
			if( bUseDefaultConnection )
				pConnection = (*itSlave)->m_pSyncConnection;
			else
				pConnection = CreateConnectionToSlave( (*itSlave) );	
			if( pConnection == NULL )
			{			
				vstr::warnp() << "VistaNewClusterMaster::CreateClusterBarrier() -- "
						<< "Could not establish connection to ["
						<< (*itSlave)->m_sName << "] - syncing on it will not work" << std::endl;
				continue;
			}
			// read requested broadcast group from slave, and inform him of the requested bc ip:port
			VistaType::sint32 nBCGroup;
			pConnection->ReadInt32( nBCGroup );
			if( nBCGroup < 0 || nBCGroup > m_nNumBroadcastGroups )
			{
				vstr::warnp() << "[VistaClusterMaster::CreateBCBarrier] Slave ["
							<< (*itSlave)->m_sName << "] requested invalid broadcast group ["
							<< nBCGroup << "] - reverting to group 0" << std::endl;
				nBCGroup = 0;
			}
			pConnection->WriteInt32( (VistaType::sint32)vecBCPorts[nBCGroup] );
			pConnection->WriteEncodedString( m_sBroadcastIP );

			pBarrier->AddFollower( (*itSlave)->m_sName, pConnection, !bUseDefaultConnection );
		}

		pBarrier->SetAllowedConsecutiveSyncFailures( m_nMaxConsecutiveBarrierSyncFailures );
		return pBarrier;
	}
	else
	{
		for( std::vector<VistaUDPSocket*>::iterator itSocket = vecSockets.begin();
				itSocket != vecSockets.end(); ++itSocket )
		{
			delete (*itSocket);
		}

		return NULL;
	}	
}


IVistaClusterBarrier* VistaNewClusterMaster::CreateInterProcBarrier( bool bIsSwapSyncBarrier )
{
	VistaInterProcClusterLeaderBarrier* pBarrier = new VistaInterProcClusterLeaderBarrier( NULL );
	for( std::vector<Slave*>::const_iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		try
		{			
			if( CheckSyncEntityDelay( (*itSlave) ) )
				continue;

			std::string sName;
			(*itSlave)->m_pConnection->ReadEncodedString( sName );
			if( sName.empty() )
				continue;
			pBarrier->AddInterProc( sName );
		}
		catch( VistaExceptionBase& )
		{
			vstr::warnp() << "[VistaClusterMaster]: Exception while - "
				<< "no INTERPROC_NAMES specified" << std::endl;
		}		
	}
	return pBarrier;
}




IVistaClusterDataSync* VistaNewClusterMaster::CreateTypedDataSync( int nType,
																  bool bUseDefaultConnection )
{
	IVistaClusterDataSync* pSync;
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
	ProcessSyncEntityDelay();

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

	if( m_sRecordDataFolder.empty() == false )
	{
		VistaFileSystemDirectory oDir( m_sRecordDataFolder );
		if( oDir.Exists() == false && oDir.CreateWithParentDirectories() == false )
		{
			vstr::warnp() << "[VistaClusterMaster]: Cannot create directory [" << m_sRecordDataFolder
				<< "] for recording the session" << std::endl;
		}
		else
		{
			std::string sFilename = m_sRecordDataFolder + "/record_" + VistaConversion::ToString( m_nRecordSyncCounter++ );
			VistaConnectionFile* pFile = new VistaConnectionFile( sFilename, VistaConnectionFile::WRITE );
			pFile->Open();
			if( pFile->GetIsOpen() == false )
			{
				delete pFile;
				vstr::warnp() << "[VistaClusterMaster]: Cannot open record file [" << sFilename
						<< "] for recording the session" << std::endl;
			}
			else
			{
				vstr::outi() << "[VistaClusterMaster]: Recording session to record file [" << sFilename
						<< "]" << std::endl;
				VistaRecordClusterLeaderDataSync* pRecordSync = new VistaRecordClusterLeaderDataSync( pFile, pSync, true );
				pSync = pRecordSync;
			}
		}
	}

	return pSync;
}



IVistaClusterDataSync* VistaNewClusterMaster::CreateTCPIPDataSync( bool bUseDefaultConnection )
{
	VistaTCPIPClusterLeaderDataSync* pSync = new VistaTCPIPClusterLeaderDataSync;

	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		if( CheckSyncEntityDelay( (*itSlave) ) )
			continue;

		VistaConnectionIP* pConnection;
		if( bUseDefaultConnection )
			pConnection = (*itSlave)->m_pSyncConnection;
		else
			pConnection = CreateConnectionToSlave( (*itSlave) );	
		if( pConnection == NULL )
		{
			vstr::errp() << "VistaNewClusterMaster::CreateIPConnections() -- "
					<< "Could not establish connection to ["
					<< (*itSlave)->m_sName << "]" << std::endl;
			continue;
		}
		pSync->AddFollower( (*itSlave)->m_sName, pConnection, !bUseDefaultConnection );
	}
	
	return pSync;
}

// Broadcast Datasync turned out to be unreliable
//IVistaClusterDataSync* VistaNewClusterMaster::CreateBroadcastDataSync( bool bUseDefaultConnection )
//{
//	std::vector<int> vecBCPorts;
//	
//	// find free ports
//	for( int i = 0; i < m_nNumBroadcastGroups; ++i )
//	{
//		int nPort = GetNextFreeBroadcastPort();
//		if( nPort < 0 )
//		{
//			vstr::warnp() << "[VistaNewClusterMaster::CreateClusterBarrier]: "
//					<< "No more free broadcast port available, aborting"
//					<< std::endl;
//			return NULL;
//		}
//		vecBCPorts.push_back( nPort );
//	}
//
//	VistaBroadcastClusterLeaderDataSync* pSync = new VistaBroadcastClusterLeaderDataSync;
//
//	// create broadcast sockets
//	bool bSuccess = true;
//	for( int i = 0; i < m_nNumBroadcastGroups; ++i )
//	{
//		VistaSocketAddress oAdress( m_sBroadcastIP, vecBCPorts[i] );		
//		if( oAdress.GetIsValid() == false )
//		{
//			vstr::warnp() << "[VistaNewClusterMaster::CreateBCBarrier]: "
//					<< "Could not open swap sync socket. Address ["
//					<< m_sBroadcastIP << ":" << vecBCPorts[i] << "] is invalid " << std::endl;
//			bSuccess = false;
//			break;
//		}
//
//		VistaUDPSocket *pSyncSocket = new VistaUDPSocket;
//		if( pSyncSocket->OpenSocket() == false )
//		{
//			vstr::warnp() << "[VistaNewClusterMaster::CreateBCBarrier]: "
//						<< "Could not open socket" << std::endl;
//			bSuccess = false;
//			break;
//		}
//		pSyncSocket->SetPermitBroadcast(1);
//		if( pSyncSocket->ConnectToAddress( oAdress ) == false )
//		{
//			vstr::warnp() << "[VistaNewClusterMaster::CreateBCBarrier]: "
//						<< "Could not connect bc-socket to address ["
//						<< m_sBroadcastIP << ":" << vecBCPorts[i] << "]" << std::endl;
//			bSuccess = false;
//			break;
//		}
//
//		pSync->AddBroadcast( pSyncSocket, true );
//	}
//
//	if( bSuccess )
//	{
//		std::vector<VistaConnectionIP*> vecWaitConns;
//		for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
//				itSlave != m_vecActiveSlaves.end(); ++itSlave )
//		{
//			if( CheckSyncEntityDelay( (*itSlave) ) )
//				continue;
//
//			// read requested broadcast group from slave, and inform him of the requested bc ip:port
//			VistaType::sint32 nBCGroup;
//			(*itSlave)->m_pConnection->ReadInt32( nBCGroup );
//			if( nBCGroup < 0 || nBCGroup > m_nNumBroadcastGroups )
//			{
//				vstr::warnp() << "[VistaClusterMaster::CreateBCBarrier] Slave ["
//							<< (*itSlave)->m_sName << "] requested invalid broadcast group ["
//							<< nBCGroup << "] - reverting to group 0" << std::endl;
//				nBCGroup = 0;
//			}
//			(*itSlave)->m_pConnection->WriteInt32( (VistaType::sint32)vecBCPorts[nBCGroup] );
//			(*itSlave)->m_pConnection->WriteEncodedString( m_sBroadcastIP );
//			vecWaitConns.push_back( (*itSlave)->m_pConnection );
//		}
//		pSync->WaitForConnection( vecWaitConns );
//	}
//	else
//	{
//		delete pSync;
//		return NULL;
//	}	
//	
//	return pSync;
//}




IVistaClusterDataCollect* VistaNewClusterMaster::CreateDataCollect()
{
	VistaTCPIPClusterLeaderDataCollect* pCollect = new VistaTCPIPClusterLeaderDataCollect;

	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		//if( CheckSyncEntityDelay( (*itSlave) ) )
		//	continue;

		VistaConnectionIP* pConnection = CreateConnectionToSlave( (*itSlave) );	
		if( pConnection == NULL )
		{
			vstr::errp() << "VistaNewClusterMaster::CreateIPConnections() -- "
					<< "Could not establish connection to ["
					<< (*itSlave)->m_sName << "]" << std::endl;
			continue;
		}
		pCollect->AddFollower( (*itSlave)->m_sName, pConnection, true );
	}
	
	return pCollect;
}

bool VistaNewClusterMaster::DistributeEvent( const VistaEvent* pEvent )
{
	VistaKernelProfileScope( "CLUSTERMASTER_DIST_EVENT" );
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "Distributing event of type " << pEvent->GetType() <<", id "
						<< pEvent->GetId() << ", time " << pEvent->GetTime() << std::endl;
	}
	m_oMessage.SetEventMsg( pEvent );
	bool bRes = m_pDefaultDataSync->SyncData( m_oMessage );
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    Event distribution finished" << std::endl;
	}

	return bRes;
}

void VistaNewClusterMaster::DeactivateSlaveAfterDrop( Slave* pSlave )
{
	if( m_oClusterInfo.m_bClusterDebuggingActive )
	{
		GetDebugStream() << "    Dropping slave [" << pSlave->m_sName << "] after disconnect" << std::endl;
	}
	vstr::errp() << "[VistaClusterMaster]: Slave [" 
				<< pSlave->m_sName << "] has been disconnected" << std::endl;
	std::vector<Slave*>::iterator itEntry = std::find( m_vecActiveSlaves.begin(),
								m_vecActiveSlaves.end(), pSlave );
	if( itEntry != m_vecActiveSlaves.end() )
		m_vecActiveSlaves.erase( itEntry );
	m_vecDeadSlaves.push_back( pSlave );
	m_oClusterInfo.m_vecNodeInfos[pSlave->m_nIndex].m_bIsActive = false;
	delete pSlave->m_pConnection;
	pSlave->m_pConnection = NULL;
	// slave is already removed from other sync entities in the observer update
}

void VistaNewClusterMaster::DeactivateSlaveAfterDrop( const std::string& sName )
{
	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		if( (*itSlave)->m_sName == sName )
		{
			DeactivateSlaveAfterDrop( (*itSlave ) );
			return;
		}
	}
}

bool VistaNewClusterMaster::CreateDefaultSyncs()
{
	m_pDefaultDataSync = CreateTypedDataSync( m_nDataSyncMethod, true );
	m_pDefaultBarrier = CreateTypedBarrier( m_nBarrierWaitMethod, true, false );
	m_pSyncEntityObserver->Observe( m_pDefaultDataSync );
	m_pSyncEntityObserver->Observe( m_pDefaultBarrier );
	return ( m_pDefaultBarrier != NULL && m_pDefaultDataSync != NULL );
}

IVistaClusterDataSync* VistaNewClusterMaster::CreateZeroMQDataSync()
{
#ifdef VISTA_WITH_ZEROMQ
	int nPort = GetNextFreeZeroMQPort();
	std::string sAddress = m_sZeroMQAddress + ":" + VistaConversion::ToString( nPort );
	
	VistaZeroMQClusterLeaderDataSync* pLeader = new VistaZeroMQClusterLeaderDataSync( sAddress );

	// send out publisher address
	std::vector<VistaConnectionIP*> vecWaitForConns;
	for( std::vector<Slave*>::iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{		
		if( CheckSyncEntityDelay( (*itSlave) ) )
			continue;

		(*itSlave)->m_pConnection->WriteEncodedString( sAddress );
		vecWaitForConns.push_back( (*itSlave)->m_pConnection );
	}

	pLeader->WaitForConnection( vecWaitForConns );

	return pLeader;
#else
	VISTA_THROW( "ZeroMq capability not available", -1 );
#endif
}

IVistaClusterDataSync* VistaNewClusterMaster::CreateInterProcDataSync()
{
	VistaInterProcClusterLeaderDataSync* pSync = new VistaInterProcClusterLeaderDataSync( false );
	for( std::vector<Slave*>::const_iterator itSlave = m_vecActiveSlaves.begin();
			itSlave != m_vecActiveSlaves.end(); ++itSlave )
	{
		try
		{
			if( CheckSyncEntityDelay( (*itSlave) ) )
				continue;

			std::string sName;
			(*itSlave)->m_pConnection->ReadEncodedString( sName );
			if( sName.empty() )
			{
				vstr::warnp() << "[VistaClusterMaster]: Slave [" << (*itSlave)->m_sName
							<< "] did not report an INTERPROC name, which is required to create "
							<< "a InterProcDataSync instance";
				continue;
			}
			pSync->AddInterProc( sName );
		}
		catch( VistaExceptionBase& )
		{
			vstr::warnp() << "[VistaClusterMaster]: Exception while reading INTERPROC name from slave ["
							<< (*itSlave)->m_sName << "]" << std::endl;
			continue;
		}		
	}
	pSync->ConnectPipes();
	return pSync;
}

void VistaNewClusterMaster::PrintClusterSetupInfo()
{
	vstr::outi() << "[VistaNewClusterMaster]: Configuration:\n";
	vstr::IndentObject oIndent;
	vstr::outi() << "Slaves           : " << m_vecActiveSlaves.size() << " of " << m_vecSlaves.size() << " connected\n";
	vstr::outi() << "DataSync Mode    : " << VistaMasterSlave::GetDataSyncModeName( m_nDataSyncMethod ) << "\n";
	vstr::outi() << "Barrier Mode     : " << VistaMasterSlave::GetBarrierModeName( m_nBarrierWaitMethod ) << "\n";
	vstr::outi() << "SwapSync Mode    : " << VistaMasterSlave::GetSwapSyncModeName( m_nSwapSyncMethod ) << "\n";
	vstr::outi() << "SwapSync timeout : " << m_nSwapSyncTimeout << std::endl;
}


void VistaNewClusterMaster::Debug( std::ostream& oStream ) const
{
	oStream << "[VistaNewClusterMaster]:\n";
	oStream << "    Name             : " << m_sMasterName << "\n";
	oStream << "    Config Section   : " << m_sMasterSectionName << "\n";
	oStream << "    DataSync Mode    : " << VistaMasterSlave::GetDataSyncModeName( m_nDataSyncMethod ) << "\n";
	oStream << "    Barrier Mode     : " << VistaMasterSlave::GetBarrierModeName( m_nBarrierWaitMethod ) << "\n";
	oStream << "    SwapSync Mode    : " << VistaMasterSlave::GetSwapSyncModeName( m_nSwapSyncMethod ) << "\n";
	oStream << "    SwapSync timeout : " << m_nSwapSyncTimeout << "\n";
	oStream << "    Slaves           : " << m_vecActiveSlaves.size() << " of " << m_vecSlaves.size() << " connected\n";
	oStream << "    DebugOutput      : " << ( m_oClusterInfo.m_bClusterDebuggingActive ? "Enabled" : "Disabled" ) << "\n";

	for( std::vector<Slave*>::const_iterator itSlave = m_vecSlaves.begin();
	     itSlave != m_vecSlaves.end(); ++itSlave )
	{
		oStream << "        ["   << (*itSlave)->m_sName << "] at ["
				<<  (*itSlave)->m_sIP << ":" << (*itSlave)->m_nPort 
				<< "] is " << ( ( (*itSlave)->m_pConnection == NULL ) ? ( "!DEAD! ") : ( "alive " ) )
				<< "\n";
	}
	oStream << std::flush;
}

bool VistaNewClusterMaster::CheckSyncEntityDelay( Slave* pSlave )
{	
	bool bDelay;
	pSlave->m_pConnection->ReadBool( bDelay );
	if( bDelay )
		m_vecSyncEntityDelayedSlaves.push_back( pSlave );
	return bDelay;
}

void VistaNewClusterMaster::ProcessSyncEntityDelay()
{
	for( std::vector<Slave*>::iterator itSlave = m_vecSyncEntityDelayedSlaves.begin();
			itSlave != m_vecSyncEntityDelayedSlaves.end(); ++itSlave )
	{
		(*itSlave)->m_pConnection->WriteBool( true );
	}
	m_vecSyncEntityDelayedSlaves.clear();
}

void VistaNewClusterMaster::SetRecordDataFolder( const std::string& sFolder )
{
	m_sRecordDataFolder = sFolder;
	ReplaceDataInRecordFilename( m_sRecordDataFolder, VistaTimeUtils::GetStandardTimer().GetSystemTime()  );
}

std::string VistaNewClusterMaster::GetRecordDataFolder() const
{
	return m_sRecordDataFolder;
}

void VistaNewClusterMaster::ReplaceDataInRecordFilename( std::string& sFolderName, VistaType::systemtime nTime )
{
	std::size_t nPos = sFolderName.find( "%DATE%" );
	if( nPos == std::string::npos )
		return;
	std::string sDateString = VistaTimeUtils::ConvertToFormattedTimeString( nTime, "%y%m%d_%H%M%S" );
	sFolderName.replace( nPos, sDateString.length(), sDateString );
}

std::ostream& VistaNewClusterMaster::GetDebugStream()
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


