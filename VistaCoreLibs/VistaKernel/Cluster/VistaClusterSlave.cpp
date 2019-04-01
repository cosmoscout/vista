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

#include "VistaClusterSlave.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaMsg.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/IPNet/VistaTCPServer.h>
#include <VistaInterProcComm/IPNet/VistaTCPSocket.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataCollect.h>

#include <VistaAspects/VistaAspectsUtils.h>
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
#include <VistaKernel/VistaSystem.h>

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

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterSlave::VistaClusterSlave(VistaEventManager* pEventManager,
									   const std::string& sSlaveName,
									   VistaInteractionManager* pInteractionMngr )
: m_pEventManager( pEventManager )
, m_sSlaveName( sSlaveName )
, m_sSlaveSectionName( sSlaveName )
, m_nSlaveIndex( ~0 )
, m_pConnection( NULL )
, m_pAckConnection( NULL )
, m_pSystemEvent( new VistaSystemEvent )
, m_pExternalMsgEvent( new VistaExternalMsgEvent )
, m_pMsg( new VistaMsg )
, m_pInteractionEvent( NULL )
, m_bDoSwap( false )
, m_pSyncSocket( NULL )
, m_bDoOglFinish( false )
, m_nBeforeAckDelay( 0 )
, m_nBeforeSwapDelay( 0 )
, m_nAfterSwapSyncDelay( 0 )
, m_iOwnSlaveID( -1 )
, m_pAvgUpd( new VistaWeightedAverageTimer )
, m_pAvgSwap( new VistaWeightedAverageTimer )
, m_iDataTunnelQueueSize( 2 )
, m_pMessageDeSer( new VistaByteBufferDeSerializer )
, m_nSyncTimeout( 0 )
, m_pDefaultBarrier( NULL )
, m_pDefaultDataSync( NULL )
{
	m_pExternalMsgEvent->SetThisMsg( m_pMsg );
	m_pInteractionEvent = new VistaInteractionEvent( pInteractionMngr );
	vstr::warnp() << "Using outdated VistaClusterSlave -- please switch to the new VistaNewClusterSlave instead, "
					<< " the VistaClusterSlave will be removed in future releases" << std::endl;
}

VistaClusterSlave::~VistaClusterSlave()
{
	delete m_pAvgUpd;
	delete m_pAvgSwap;

	delete m_pMessageDeSer;

	vstr::outi() << "VistaClusterSlave::~VistaClusterSlave() -- closing Ack Channel" << std::endl;

	if( m_pAckConnection )
		m_pAckConnection->WaitForSendFinish();
	delete m_pAckConnection;

	vstr::outi() << "VistaClusterSlave::~VistaClusterSlave() -- closing Main Channel" << std::endl;

	if( m_pConnection )
		m_pConnection->WaitForSendFinish();
	delete m_pConnection;

	delete m_pSyncSocket;

	delete m_pSystemEvent;
	delete m_pInteractionEvent;
	delete m_pMsg;

	delete m_pExternalMsgEvent;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaClusterSlave::Init( const std::string& sClusterSection,
								const VistaPropertyList& oConfig )
{
	vstr::outi() << "[VistaClusterSlave]: Initializing" << std::endl;
	{
		vstr::IndentObject oIndent;

		m_sSlaveSectionName = sClusterSection;

		if( oConfig.HasSubList( sClusterSection ) == false )
		{
			vstr::errp() << "Cluster Slave cannot be initialized - section ["
				<< sClusterSection << "] does not exist!" << std::endl;
			return false;
		}
		const VistaPropertyList& oSection = oConfig.GetSubListConstRef( sClusterSection );

		m_sSlaveName = oConfig.GetValueOrDefault<std::string>( "NAME", sClusterSection );
		vstr::outi() << "Setting slave name to [" << m_sSlaveName << "]" << std::endl;

		vstr::outi() << "Slave Section: [" << m_sSlaveSectionName << "]\n";
		vstr::outi() << "Setting slave name to [" << m_sSlaveName << "]" << std::endl;

		int nPort = 0;
		int nAckPort = 0;

		if( oSection.GetValue( "SLAVEIP", m_sSlaveHostName ) == false
			|| m_sSlaveHostName.empty() )
		{
			vstr::errp() << "No SLAVEIP given" << std::endl;
			return false;
		}

		if( oSection.GetValue( "SLAVEPORT", nPort ) == false )
		{
			vstr::errp() << "No SLAVEPORT given" << std::endl;
			return false;
		}
		if( nPort <= 0 )
		{
			vstr::errp() << "SLAVEPORT invalid" << std::endl;
			return false;
		}

		if( oSection.GetValue( "ACKPORT", nAckPort ) == false )
		{
			vstr::errp() << "No ACKPORT given" << std::endl;
			return false;
		}
		if( nAckPort <= 0 || nAckPort == nPort )
		{
			vstr::errp() << "ACKPORT invalid" << std::endl;
			return false;
		}

		// we use the deserializer's swapflag as default - it is set depending on platform architecture
		m_bDoSwap = oSection.GetValueOrDefault<bool>( "BYTESWAP", m_pMessageDeSer->GetByteorderSwapFlag() == VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES ? true : false );
		bool bDoSync = oSection.GetValueOrDefault<bool>( "SYNC", true );

		if( bDoSync )
		{
			std::string strSyncIp;
			if( oSection.GetValue( "SYNCIP", strSyncIp ) == false )
			{
				vstr::errp() << "SYNCing requested, but no SYNCIP was given" << std::endl;
				return false;
			}
			int nSyncPort;
			if( oSection.GetValue( "SYNCPORT", nSyncPort ) == false )
			{
				vstr::errp() << "SYNCing requested, but no SYNCPORT was given" << std::endl;
				return false;
			}

			if( oSection.GetValue( "SYNCTIMEOUT", m_nSyncTimeout ) == false )
			{
				m_nSyncTimeout = 0;
			}

			vstr::outi() << "Slave is told to do a sync on IP ["
					<< strSyncIp << "] - Port [" << nSyncPort << "]" << std::endl;

			m_pSyncSocket = new VistaUDPSocket;
			if( m_pSyncSocket->OpenSocket() )
			{
				m_pSyncSocket->SetIsBlocking(true);
				m_pSyncSocket->SetPermitBroadcast(1);
				m_pSyncSocket->SetSocketReuse( true );

				VistaSocketAddress adr(strSyncIp, nSyncPort);
				if(!m_pSyncSocket->BindToAddress(adr))
				{
					vstr::errp() << "Could not bind SYNC-Socket, check ip and port" << std::endl;
					delete m_pSyncSocket;
					m_pSyncSocket = NULL;
				}
			}
			else
			{
				delete m_pSyncSocket;
				m_pSyncSocket = NULL;
				vstr::errp() << "Could not open sync socket. This slave will not sync!" << std::endl;
			}

		}

		// read available ports - these can be used for connections between slave and master
		std::list<std::string> liPortRanges;
		if( oSection.GetValue( "FREEPORTS", liPortRanges ) )
		{
			for( std::list<std::string>::const_iterator itRange = liPortRanges.begin();
					itRange != liPortRanges.end(); ++itRange )
			{
				size_t nDashPos = (*itRange).find( '-' );
				if( nDashPos == std::string::npos )
				{
					// no range, just a single port
					int iPort;
					if( VistaConversion::FromString( (*itRange), iPort ) )
						m_liFreePorts.push_back( std::pair<int, int>( iPort, iPort ) );
				}
				else
				{
					std::string sMin = (*itRange).substr( 0, nDashPos - 1 );
					std::string sMax = (*itRange).substr( nDashPos + 1, std::string::npos );
					int nMin = 0;
					int nMax = 0;
					if( VistaConversion::FromString( sMin, nMin )
						&& VistaConversion::FromString( sMax, nMax ) )
					{
						if( nMax < nMin )
						{
							vstr::warnp() << "Received invalid free port range ["
								<< nMin << "-" << nMax << "]" << std::endl;
							continue;
						}
						m_liFreePorts.push_back( std::pair<int, int>( nMin, nMax ) );
					}
					else
					{
						vstr::warnp() << "Cannot parse port range ["
								<< (*itRange) << "]" << std::endl;
							continue;
					}
				}
			}
		}

		m_bDoOglFinish = oSection.GetValueOrDefault<bool>( "DOGLFINISH", false );
		vstr::outi() << "Slave is told "
				  << (m_bDoOglFinish ? "" : "not ")
				  << "to do glFinish() before swap" << std::endl;


		m_nBeforeAckDelay     = oSection.GetValueOrDefault<int>( "BEFOREACKDELAY", 0 );
		m_nBeforeSwapDelay    = oSection.GetValueOrDefault<int>( "BEFORESWAPDELAY", 0 );
		m_nAfterSwapSyncDelay = oSection.GetValueOrDefault<int>( "AFTERSWAPSYNCDELAY", 0 );

		if( m_nBeforeAckDelay > 0 )
			vstr::outi() << "BeforeAckDelay     : \t" << m_nBeforeAckDelay     << std::endl;
		if( m_nBeforeSwapDelay > 0 )
			vstr::outi() << "BeforeSyncDelay    : \t" << m_nBeforeSwapDelay    << std::endl;
		if( m_nAfterSwapSyncDelay > 0 )
			vstr::outi() << "AfterSwapDelay     : \t" << m_nAfterSwapSyncDelay << std::endl;
		vstr::outi() << "ByteSwap           : \t" << ( m_bDoSwap ? "TRUE" : "FALSE" ) << std::endl;

		m_iDataTunnelQueueSize = oSection.GetValueOrDefault<int>( "DATATUNNEL_QUEUE_SIZE", 12 );

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// +++ HANDSHAKE WITH MASTER
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		if( WaitForClusterMaster( m_sSlaveHostName, nPort, nAckPort ) == false )
		{
			vstr::errp() << "WaitForClusterMaster() failed on IP ["
					  << m_sSlaveHostName << "] - Port [" << nPort << "]" << std::endl;
			return false;
		}

		if( ReceiveClusterSetupInformation() == false )
			return false;
	}

	vstr::outi() << "[VistaClusterSlave]: Initialization finished" << std::endl;
	return true;
}

bool VistaClusterSlave::PostInit()
{
	bool bSucces = true;
	try
	{
		VistaType::sint32 iAlive = 42;
		if( m_pConnection->WriteInt32( iAlive ) != sizeof(VistaType::sint32) )
		{
			vstr::errp() << "VistaClusterSlave::PostInit() could not write alive token" << std::endl;
			bSucces = false;
		}
	}
	catch( VistaExceptionBase& )
	{
		vstr::errp() << "VistaClusterSlave::PostInit() could not write alive token" << std::endl;
		bSucces = false;
	}
	if( bSucces == false )
		VISTA_THROW( "SLAVE_SEND_ALIVE_SIGNAL_FAILED", -1 );
	return true;
}


int VistaClusterSlave::GetClusterMode() const
{
	return CM_MASTER_SLAVE;
}

std::string VistaClusterSlave::GetClusterModeName() const
{
	return "MASTER_SLAVE";
}


int VistaClusterSlave::GetNodeType() const
{
	return NT_SLAVE;
}

std::string VistaClusterSlave::GetNodeTypeName() const
{
	return "SLAVE";
}

std::string VistaClusterSlave::GetNodeName() const
{
	return m_sSlaveName;
}

std::string VistaClusterSlave::GetConfigSectionName() const
{
	return m_sSlaveSectionName;
}

int VistaClusterSlave::GetNodeID() const
{
	return m_iOwnSlaveID;
}


bool VistaClusterSlave::GetIsLeader() const
{
	return false;
}

bool VistaClusterSlave::GetIsFollower() const
{
	return true;
}


int VistaClusterSlave::GetNumberOfNodes() const
{
	return ( (int)m_vecSlaveInfo.size() + 1 ); // +1 is Master
}

std::string VistaClusterSlave::GetNodeName( const int iNodeID ) const
{
	if( iNodeID == 0 )
	{
		return m_oMasterInfo.m_sNodeName;
	}
	else if( iNodeID <= (int)m_vecSlaveInfo.size() )
	{
		return m_vecSlaveInfo[iNodeID - 1].m_sNodeName;
	}
	else
		return "";
}

bool VistaClusterSlave::GetNodeInfo( const int iNodeID, NodeInfo& oInfo ) const
{
	if( iNodeID == 0 )
	{
		oInfo = m_oMasterInfo;
	}
	else if( iNodeID <= (int)m_vecSlaveInfo.size() )
	{
		oInfo = m_vecSlaveInfo[iNodeID - 1];
	}
	else
		return false;

	return true;
}


bool VistaClusterSlave::StartFrame()
{
	++m_iFrameCount;

	// sync frameclock with slave
	VistaType::sint32 iMasterFrameCount = 0;
	bool bSuccess = true;
	try
	{
		if( m_pConnection->ReadInt32( iMasterFrameCount ) != sizeof(VistaType::sint32)
			|| m_pConnection->ReadDouble( m_dFrameClock ) != sizeof(double) )
		{
			vstr::errp() << "VistaCLuserSlave::StartFrame() - ConnectionRead failed" << std::endl;
			bSuccess = false;
		}
		else
		{
			assert( iMasterFrameCount == m_iFrameCount );
		}
	}
	catch( VistaExceptionBase& e )
	{
		vstr::errp() << "VistaCluserSlave::StartFrame() - ConnectionRead failed with Exception\n"
				<< e.GetPrintStatement() << std::endl;
		bSuccess = false;
	}	
	if( bSuccess == false )
		VISTA_THROW( "CLUSTER_SLAVE_CONNECTION_ERROR", -1 );
	return true;
}

bool VistaClusterSlave::ProcessFrame()
{
	bool bRet = false;
	// wait blocking on the tcp socket
	if( m_pConnection == NULL )
		return false; // exit, something is wrong.

	m_pAvgUpd->StartRecording();

	try
	{
		bool bEndOfFrame = false;

		while(!bEndOfFrame)
		{		
			// This reads a message with prefixed size, and stores it in m_pMessageDeSer
			ReceiveDataFromMaster();

			bEndOfFrame							= ReadBoolFromMsg();
			/*VistaType::sint32 nPackageCount	= */ ReadIntFromMsg();
			m_iFrameCount						= ReadIntFromMsg();
			/*VistaType::sint32 nEventCount		= */ ReadIntFromMsg();
			/*VistaType::sint32 nCounter		= */ ReadIntFromMsg();
			/*double dTime						= */ ReadDoubleFromMsg();			
			VistaType::sint32 nType				= ReadIntFromMsg();
			VistaType::sint32 nId				= ReadIntFromMsg();

			VistaEvent *pEvent = NULL;

			if( nType == VistaSystemEvent::GetTypeId() )
			{
				pEvent = m_pSystemEvent;
				VistaKernelProfileStartSection( VistaSystemEvent::GetIdString( nId ) );
			}
			else if(nType == VistaInteractionEvent::GetTypeId())
			{
				pEvent = m_pInteractionEvent;
			}
			else if(nType == VistaExternalMsgEvent::GetTypeId())
			{
				pEvent = m_pExternalMsgEvent;
			}
			else
				VISTA_THROW( "CLUSTER_SLAVE_UNSUPPORTED_EVENT_TYPE", -1 );

			if( m_pMessageDeSer->ReadSerializable( *pEvent ) <= 0 )
				VISTA_THROW( "CLUSTER_SLAVE_EVENT_DESERIALIZE_FAILED", -1 );

			if(pEvent->GetId() == VistaSystemEvent::VSE_EXIT)
				bRet = true;
			
			m_pEventManager->ProcessEvent( pEvent );

			if( nType == VistaSystemEvent::GetTypeId() )
				VistaKernelProfileStopSection();
		} // only leave on error, or end of frame mark!
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		throw x;
	}

	m_pAvgUpd->RecordTime();

	return bRet;
}

bool VistaClusterSlave::EndFrame()
{
	return true;
}

bool VistaClusterSlave::CreateConnections( std::vector<VistaConnectionIP*>& vecConnections )
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaClusterSlave::CreateConnections() -- "
					<< "Could not establish connection to master" << std::endl;
		return false;
	}

	vecConnections.push_back( pConnection );

	return true;
}
bool VistaClusterSlave::CreateNamedConnections( 
				std::vector<std::pair<VistaConnectionIP*, std::string> >& vecConnections )
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaClusterSlave::CreateConnections() -- "
					<< "Could not establish connection to master" << std::endl;
		return false;
	}

	vecConnections.push_back( std::pair<VistaConnectionIP*, std::string>(
												pConnection, m_oMasterInfo.m_sNodeName ) );

	return true;
}

IVistaDataTunnel* VistaClusterSlave::CreateDataTunnel( IDLVistaDataPacket* pPacketProto )
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaClusterSlave::CreateNetworkSync() -- "
			<< "Could not establish connection for syncing" << std::endl;
		return NULL;
	}	
	
	return new VistaSlaveDataTunnel( pConnection, m_iDataTunnelQueueSize, pPacketProto );
}

void VistaClusterSlave::Debug( std::ostream& oStream ) const
{
	oStream << "VistaClusterSlave::Debug() -- " << std::endl;
	vstr::IndentObject oIndent;
	oStream << "Name      : [" << m_sSlaveName << "]" << std::endl;
	oStream << "IniSection: [" << m_sSlaveSectionName << "]" << std::endl;
	oStream << "Connected : [" << (m_pConnection ? "YES" : "NO") << "]" << std::endl;
	oStream << "Host      : [" << (m_pConnection ? m_pConnection->GetPeerName() : "<->") << "]" << std::endl;
	oStream << "Port      : [" << (m_pConnection ? m_pConnection->GetPeerPort() : -1) << "]" << std::endl;
	oStream << "acks      : [" << (m_pAckConnection ? "YES" : "NO") << "]" << std::endl;
	oStream << "sync      : [" << (m_pSyncSocket ? "YES" : "NO" ) << "]" << std::endl;
	oStream << "swap      : [" << (m_bDoSwap ? "YES" : "NO") << "]" << std::endl;	
	oStream << "before-ack-delay: [" << m_nBeforeAckDelay << "]" << std::endl;
	oStream << "before-swap-delay: [" << m_nBeforeSwapDelay << "]" << std::endl;
	oStream << "after-swap-delay: [" << m_nAfterSwapSyncDelay << "]" << std::endl;
	oStream << "DoGLFinish: [" << (m_bDoOglFinish ? "YES" : "NO") << "]" << std::endl;
}

bool VistaClusterSlave::ReceiveClusterSetupInformation()
{
	vstr::outi() << "Receiving Cluster Setup Information" << std::endl;
	vstr::IndentObject oIndent;
	
	assert( m_pConnection != NULL );

	ReceiveDataFromMaster();

	ReadStringFromMsg( m_oMasterInfo.m_sNodeName );
	m_oMasterInfo.m_bIsActive = true;
	m_oMasterInfo.m_eNodeType = NT_MASTER;
	m_oMasterInfo.m_iNodeID = 0;
	
	VistaType::sint32 iNrOfNodes = ReadIntFromMsg();
	m_vecSlaveInfo.resize( iNrOfNodes - 1 );
	int iID = 1;
	for( std::vector<NodeInfo>::iterator itSlaveInfo = m_vecSlaveInfo.begin();
			itSlaveInfo != m_vecSlaveInfo.end(); ++itSlaveInfo, ++iID )
	{
		ReadStringFromMsg( (*itSlaveInfo).m_sNodeName );
		(*itSlaveInfo).m_bIsActive = ReadBoolFromMsg();
		(*itSlaveInfo).m_eNodeType = NT_SLAVE;
		(*itSlaveInfo).m_iNodeID = iID;
	}
	return true;
}

VistaType::microtime VistaClusterSlave::GetAvgUpdateTime() const
{
	return m_pAvgUpd->GetAverageTime();
}

VistaType::microtime VistaClusterSlave::GetAvgSwapTime() const
{
	return m_pAvgSwap->GetAverageTime();
}

bool VistaClusterSlave::WaitForClusterMaster( const std::string &strIp,
		                                       int nPort, int nAckPort )
{
	try
	{
		vstr::outi() << "Waiting for master on ["
				<< strIp << ":" << nPort << "]..." << std::endl;

		VistaTCPServer oServer( strIp, nPort,0, true);
		if( oServer.GetIsValid() )
		{
			VistaTCPSocket *pSocket = oServer.GetNextClient();

			if(pSocket == NULL)
			{
				vstr::errp() << "WaitForClusterMaster failed - connected socket is NULL" << std::endl;
				return false;
			}

			m_pConnection = new VistaConnectionIP( pSocket );
			m_pConnection->SetIsBlocking( true );
			m_pConnection->SetIsBuffering( false );
			m_pConnection->SetByteorderSwapFlag( m_bDoSwap == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );

			m_dFrameClock = 0;
			VistaType::sint32 nIndex;
			m_pConnection->ReadDouble( m_dFrameClock );
			m_pConnection->ReadInt32( nIndex );
			m_nSlaveIndex = nIndex;

			m_pMessageDeSer->SetByteorderSwapFlag( m_bDoSwap == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );
		}
		else
		{
			vstr::errp() << "Could not create Main TCP socket!" << std::endl;
			return false;
		}

		vstr::outi() << "Creating ACK connection on ["
			<< strIp << " ; " << nAckPort << "]" << std::endl;

		VistaTCPServer pAckServer( strIp, nAckPort, 1 );
		if( !pAckServer.GetIsValid() )
		{
			vstr::errp() << "Could not create Ack TCP socket" << std::endl;
			return false;
		}

		m_pConnection->WriteInt32( nAckPort );

		VistaTCPSocket *pAckSocket = pAckServer.GetNextClient();
		if( pAckSocket == NULL )
		{
			vstr::errp() << "Could not establish Ack Connection!" << std::endl;
			m_pConnection->Close();
			delete m_pConnection;
			m_pConnection = NULL;
			return false;
		}

		m_pAckConnection = new VistaConnectionIP( pAckSocket );
		if( m_pAckConnection->GetIsOpen() )
		{
			m_pAckConnection->SetIsBlocking(true);
			m_pAckConnection->SetIsBuffering(false);
			m_pAckConnection->SetByteorderSwapFlag(m_bDoSwap == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );
		}
		else
		{
			vstr::errp() << "Ack Connection failed" << std::endl;
			delete m_pAckConnection;
			m_pAckConnection = NULL;
			m_pConnection->Close();
			delete m_pConnection;
			m_pConnection = NULL;
			return false;
		}
	}
	catch(VistaExceptionBase &x)
	{
		vstr::errp() << "WaitForClusterMaster failed with Exception:\n"
				 << x.GetPrintStatement() << std::endl;
		return false;
	}

	return true;
}

void VistaClusterSlave::SwapSync()
{
	VistaKernelProfileScope( "SLAVE_SWAPSYNC" );
	m_pAvgSwap->StartRecording();

	// user told us to wait until glFinish() returns
	// this is a best guess that the gfx hardware is really
	// done with rendering and waits for the buffer swap
	if(m_bDoOglFinish)
	{
		VistaKernelProfileScope( "GL_FINISH" );
		glFinish();
	}

	// debugging: simulate a little delay
	if(m_nBeforeAckDelay)
		VistaTimeUtils::Sleep(m_nBeforeAckDelay);

	// ack that we have processed the current frame
	// and maybe wait at the barrier
	if(m_pAckConnection)
		m_pAckConnection->WriteInt32(m_iFrameCount);

	// debugging: simulate a little delay
	if(m_nBeforeSwapDelay)
		VistaTimeUtils::Sleep(m_nBeforeSwapDelay);

	// blocking read on barrier
	if(m_pSyncSocket)
	{
		int iDummy = 0;
		if( m_pSyncSocket->ReceiveRaw( &iDummy, sizeof(int), m_nSyncTimeout ) != sizeof(int) )
		{
			vstr::warnp() << "[ViClSlave]: wait for swap sync failed - timeout of "
						<< m_nSyncTimeout << "ms exceeded" << std::endl;
		}
		if(iDummy != m_iFrameCount)
		{
			vstr::errp() << "[ViClSlave]: wait for swap sync returned the wrong "
			          << "token, something is out of sync.\n"
					  << vstr::indent << "expected [" << m_iFrameCount << "], but got: ["
			          << iDummy << "]" << " pending: " << m_pSyncSocket->HasBytesPending()
					  << std::endl;
		}
		if(m_nAfterSwapSyncDelay)
			VistaTimeUtils::Sleep(m_nAfterSwapSyncDelay);
	}

		
	m_pAvgSwap->RecordTime();
}

int VistaClusterSlave::GetNextFreePort()
{
	if( m_liFreePorts.empty() )
		return -1;
	int iNewIP = m_liFreePorts.front().first;
	if( ++m_liFreePorts.front().first > m_liFreePorts.front().second )
		m_liFreePorts.pop_front();
	return iNewIP;
}

VistaConnectionIP* VistaClusterSlave::CreateConnectionToMaster()
{
	VistaConnectionIP* pConnection = NULL;
	try
	{		
		int iPort = GetNextFreePort();
		if( iPort < 0 )
		{
			vstr::warnp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
					<< "Could not create new Socket for master - no free ports are available"
					<< std::endl;
			m_pAckConnection->WriteInt32( -1 );
			return NULL;
		}

		vstr::outi() << "VistaClusterSlave::CreateConnectionToMaster() -- "
			<< "Waiting for connection from master on IP [" << m_sSlaveHostName 
			<< "] - Port [" << iPort << "]" << std::endl;

	
		VistaTCPServer oServer( m_sSlaveHostName, iPort, 0, true);
		if( oServer.GetIsValid() == false )
		{
			vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
					<< "Could not create Main TCP socket on IP [" << m_sSlaveHostName
					<< "] - Port [" << iPort << "]" << std::endl;
			m_pAckConnection->WriteInt32( -2 );
			return NULL;
		}

		if( m_pAckConnection->WriteInt32( iPort ) != sizeof(VistaType::sint32) )
		{
			vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
					<< "Could not send own port to master" << std::endl;
			return NULL;
		}

		VistaTCPSocket *pSocket = oServer.GetNextClient();
		if( pSocket == NULL )
		{
			vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster() -- "
					<< "No connection from master on IP [" << m_sSlaveHostName
					<< "] - Port [" << iPort << "]" << std::endl;
			return NULL;
		}

		vstr::outi() << "VistaClusterSlave::CreateConnectionToMaster() -- "
				<< "Connection established" << std::endl;

		pConnection = new VistaConnectionIP( pSocket );
		pConnection->SetIsBlocking( true );
		pConnection->SetIsBuffering( false );
		pConnection->SetByteorderSwapFlag( m_bDoSwap == true ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES );
	}
	catch( VistaExceptionBase& e )
	{
		vstr::errp() << "VistaClusterSlave::CreateConnectionToMaster failed with exception\n"
				<< e.GetPrintStatement() << std::endl;
		return NULL;
	}
	
	
	return pConnection;
}

void VistaClusterSlave::ReceiveDataFromMaster()
{
	bool bFail = false;
	VistaType::sint32 nSize = -1;
	try
	{		
		// read off with correct endianess
		m_pConnection->ReadInt32( nSize );

		// important: nSize still contains the 4 bytes
		// for the message size tag
		nSize = nSize - sizeof(VistaType::sint32);

		if( m_vecMessage.size() < (std::size_t)nSize )
			m_vecMessage.resize( nSize );

		int nRet = m_pConnection->ReadRawBuffer( reinterpret_cast<void*>( &m_vecMessage[0] ), nSize );
		if( nRet != nSize )
		{
			vstr::errp() << "[VistaClusterSlave]: Reading data from connection failed - requested "
					<< nSize << " bytes, received " << nRet << std::endl;
			bFail = true;
		}
	}
	catch( VistaExceptionBase& e )
	{
		vstr::errp() << "[VistaClusterSlave]: Reading data from connection failed wiht exception\n"
				<< e.GetPrintStatement() << std::endl;
		bFail = true;
	}
	
	if( bFail )
		VISTA_THROW( "CLUSTER_SLAVE_CONNECTION_ERROR", -1 );

	m_pMessageDeSer->SetBuffer( &m_vecMessage[0], nSize );
}

bool VistaClusterSlave::ReadBoolFromMsg()
{
	bool bVal;
	if( m_pMessageDeSer->ReadBool( bVal ) != sizeof(bool) )
		VISTA_THROW( "CLUSTER_SLAVE_INCOMPLETE_MESSAGE", -1 );
	return bVal;
}

double VistaClusterSlave::ReadDoubleFromMsg()
{
	double bVal;
	if( m_pMessageDeSer->ReadDouble( bVal ) != sizeof(double) )
		VISTA_THROW( "CLUSTER_SLAVE_INCOMPLETE_MESSAGE", -1 );
	return bVal;
}

VistaType::sint32 VistaClusterSlave::ReadIntFromMsg()
{
	VistaType::sint32 bVal;
	if( m_pMessageDeSer->ReadInt32( bVal ) != sizeof(VistaType::sint32) )
		VISTA_THROW( "CLUSTER_SLAVE_INCOMPLETE_MESSAGE", -1 );
	return bVal;
}

void VistaClusterSlave::ReadStringFromMsg( std::string& sTarget )
{
	VistaType::sint32 nSize;
	if( m_pMessageDeSer->ReadInt32( nSize ) != sizeof(VistaType::sint32) )
		VISTA_THROW( "CLUSTER_SLAVE_INCOMPLETE_MESSAGE", -1 );
	if( m_pMessageDeSer->ReadString( sTarget, nSize ) != (int)( nSize * sizeof(char) ) )
		VISTA_THROW( "CLUSTER_SLAVE_INCOMPLETE_MESSAGE", -1 );
}

IVistaClusterDataSync* VistaClusterSlave::CreateDataSync()
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaClusterSlave::CreateNetworkSync() -- "
			<< "Could not establish connection for syncing" << std::endl;
		return NULL;
	}
	return new VistaTCPIPClusterFollowerDataSync( pConnection, m_bDoSwap, true );
}

IVistaClusterDataSync* VistaClusterSlave::GetDefaultDataSync()
{
	if( m_pDefaultDataSync == NULL )
		m_pDefaultDataSync = CreateDataSync();
	return m_pDefaultDataSync;
}

IVistaClusterBarrier* VistaClusterSlave::CreateBarrier()
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaClusterSlave::CreateNetworkSync() -- "
			<< "Could not establish connection for syncing" << std::endl;
		return NULL;
	}
	return new VistaTCPIPClusterFollowerBarrier( pConnection, true );
}

IVistaClusterBarrier* VistaClusterSlave::GetDefaultBarrier()
{
	if( m_pDefaultBarrier == NULL )
		m_pDefaultBarrier = CreateBarrier();
	return m_pDefaultBarrier;
}

IVistaClusterDataCollect* VistaClusterSlave::CreateDataCollect()
{
	VistaConnectionIP* pConnection = CreateConnectionToMaster();
	if( pConnection == NULL )
	{
		vstr::warnp() << "VistaNewClusterSlave::CreateNetworkSync() -- "
			<< "Could not establish connection for syncing" << std::endl;
		return NULL;
	}
	return new VistaTCPIPClusterFollowerDataCollect( pConnection, m_bDoSwap, true );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


