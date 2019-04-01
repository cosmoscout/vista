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


#include "VistaRecordReplayClusterDataSync.h"

#include <VistaInterProcComm/Connections/VistaConnectionFile.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaAspects/VistaPropertyList.h>

#include <cassert>
#include "VistaBase/VistaTimeUtils.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* RECORD                                                                     */
/*============================================================================*/

VistaRecordClusterLeaderDataSync::VistaRecordClusterLeaderDataSync( const std::string& sFilename,
																   IVistaClusterDataSync* pOriginalSync,
																   const bool bManageOriginalSyncDeletion,
																   const bool bVerbose )
: IVistaClusterDataSync( bVerbose, true )
, m_pOriginalSync( pOriginalSync )
, m_bManageOriginalSyncDeletion( bManageOriginalSyncDeletion )
, m_pRecordFile( new VistaConnectionFile( sFilename, VistaConnectionFile::WRITE ) )
{
	if( m_pRecordFile->GetIsOpen() == false )
		VISTA_THROW( ( "VistaRecordClusterLeaderDataSync - invalid file [" + sFilename + "]" ).c_str(), -1 );
}

VistaRecordClusterLeaderDataSync::VistaRecordClusterLeaderDataSync( VistaConnectionFile* pRecordFile,
																   IVistaClusterDataSync* pOriginalSync,
																   const bool bManageOriginalSyncDeletion,
																   const bool bVerbose )
: IVistaClusterDataSync( bVerbose, true )
, m_pOriginalSync( pOriginalSync )
, m_bManageOriginalSyncDeletion( bManageOriginalSyncDeletion )
, m_pRecordFile( pRecordFile )
{

}

VistaRecordClusterLeaderDataSync::~VistaRecordClusterLeaderDataSync()
{
	m_pRecordFile->Close();
	delete m_pRecordFile;

	if( m_bManageOriginalSyncDeletion )
		delete m_pOriginalSync;
}

bool VistaRecordClusterLeaderDataSync::SyncTime( VistaType::systemtime& nTime )
{
	bool bRes = m_pOriginalSync->SyncTime( nTime ) == false;
	//m_pRecordFile->WriteBool( bRes );
	//if( bRes )	
	{
		m_pRecordFile->WriteFloat64( m_oTimer.GetLifeTime() );
		m_pRecordFile->WriteFloat64( nTime );
	}
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::SyncData( VistaPropertyList& oList )
{
	bool bRes = m_pOriginalSync->SyncData( oList );
	//m_pRecordFile->WriteBool( bRes );
	//if( bRes )	
	{
		m_pRecordFile->WriteFloat64( m_oTimer.GetLifeTime() );
		VistaPropertyList::SerializePropertyList( *m_pRecordFile, oList, "blah" );
	}
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::SyncData( IVistaSerializable& oSerializable )
{
	bool bRes = m_pOriginalSync->SyncData( oSerializable );
	//m_pRecordFile->WriteBool( bRes );
	//if( bRes )	
	{
		m_pRecordFile->WriteFloat64( m_oTimer.GetLifeTime() );
		m_pRecordFile->WriteSerializable( oSerializable );
	}
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::SyncData( VistaType::byte* pData, const int iDataSize )
{
	bool bRes = m_pOriginalSync->SyncData( pData, iDataSize );
	//m_pRecordFile->WriteBool( bRes );
	//if( bRes )	
	{
		m_pRecordFile->WriteFloat64( m_oTimer.GetLifeTime() );
		m_pRecordFile->WriteRawBuffer( pData, iDataSize );
	}
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::SyncData( VistaType::byte* pDataBuffer, const int iBufferSize, int& iDataSize )
{
	bool bRes = m_pOriginalSync->SyncData( pDataBuffer, iBufferSize, iDataSize ) ;
	//m_pRecordFile->WriteBool( bRes );
	//if( bRes )	
	{
		m_pRecordFile->WriteFloat64( m_oTimer.GetLifeTime() );
		m_pRecordFile->WriteInt32( (VistaType::sint32)iDataSize );
		m_pRecordFile->WriteRawBuffer( pDataBuffer, iDataSize );
	}
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::SyncData( std::vector<VistaType::byte>& vecData )
{
	bool bRes = m_pOriginalSync->SyncData( vecData ) ;
	//m_pRecordFile->WriteBool( bRes );
	//if( bRes )	
	{
		m_pRecordFile->WriteFloat64( m_oTimer.GetLifeTime() );
		m_pRecordFile->WriteInt32( (VistaType::sint32)vecData.size() );
		m_pRecordFile->WriteRawBuffer( &vecData[0], (int)vecData.size() );
	}
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::SyncData( std::string& sData )
{
	
	bool bRes = m_pOriginalSync->SyncData( sData ) ;
	//m_pRecordFile->WriteBool( bRes );
	//if( bRes )	
	{
		m_pRecordFile->WriteInt32( (VistaType::sint32)sData.size() );
		m_pRecordFile->WriteRawBuffer( &sData[0], (int)sData.size() );
	}
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::GetIsValid() const
{
	bool bRes = m_pOriginalSync->GetIsValid();
	//m_pRecordFile->WriteBool( bRes );	
	return bRes;
}

std::string VistaRecordClusterLeaderDataSync::GetDataSyncType() const
{
	std::string sRes = m_pOriginalSync->GetDataSyncType();
	//m_pRecordFile->WriteEncodedString( sRes );
	return sRes;
}

int VistaRecordClusterLeaderDataSync::GetNumberOfFollowers() const
{
	VistaType::uint32 nRes = m_pOriginalSync->GetNumberOfFollowers();
	//m_pRecordFile->WriteInt32( nRes );
	return (int)nRes;
}

int VistaRecordClusterLeaderDataSync::GetNumberOfActiveFollowers() const
{
	VistaType::uint32 nRes = m_pOriginalSync->GetNumberOfActiveFollowers();
	//m_pRecordFile->WriteInt32( nRes );
	return (int)nRes;
}

int VistaRecordClusterLeaderDataSync::GetNumberOfDeadFollowers() const
{
	VistaType::uint32 nRes = m_pOriginalSync->GetNumberOfDeadFollowers();
	//m_pRecordFile->WriteInt32( nRes );
	return (int)nRes;
}

std::string VistaRecordClusterLeaderDataSync::GetFollowerNameForId( const int nID ) const
{
	std::string sRes = m_pOriginalSync->GetFollowerNameForId( nID );
	//m_pRecordFile->WriteEncodedString( sRes );
	return sRes;
}

int VistaRecordClusterLeaderDataSync::GetFollowerIdForName( const std::string& sName ) const
{
	VistaType::uint32 nRes = m_pOriginalSync->GetFollowerIdForName( sName );
	//m_pRecordFile->WriteInt32( nRes );
	return (int)nRes;
}

bool VistaRecordClusterLeaderDataSync::GetFollowerIsAlive( const int nID ) const
{
	bool bRes = m_pOriginalSync->GetFollowerIsAlive( nID );
	//m_pRecordFile->WriteBool( bRes );	
	return bRes;
}

int VistaRecordClusterLeaderDataSync::GetLastChangedFollower()
{
	VistaType::uint32 nRes = m_pOriginalSync->GetLastChangedFollower();
	//m_pRecordFile->WriteInt32( nRes );
	return (int)nRes;
}

bool VistaRecordClusterLeaderDataSync::DeactivateFollower( const std::string& sName )
{
	bool bRes = m_pOriginalSync->DeactivateFollower( sName );
	//m_pRecordFile->WriteBool( bRes );	
	return bRes;
}

bool VistaRecordClusterLeaderDataSync::DeactivateFollower( const int nID )
{
	bool bRes = m_pOriginalSync->DeactivateFollower( nID );
	//m_pRecordFile->WriteBool( bRes );	
	return bRes;
}

int VistaRecordClusterLeaderDataSync::GetSendBlockingThreshold() const
{
	VistaType::uint32 nRes = m_pOriginalSync->GetSendBlockingThreshold();
	//m_pRecordFile->WriteInt32( nRes );
	return (int)nRes;
}

bool VistaRecordClusterLeaderDataSync::SetSendBlockingThreshold( const int nNumBytes )
{
	bool bRes = m_pOriginalSync->SetSendBlockingThreshold( nNumBytes );
	//m_pRecordFile->WriteBool( bRes );	
	return bRes;
}

/*============================================================================*/
/*  REPLAYFOLLOWER                                                            */
/*============================================================================*/

VistaReplayClusterFollowerDataSync::VistaReplayClusterFollowerDataSync( const std::string& sFilename,
																   const bool bVerbose )
: IVistaClusterDataSync( bVerbose, false )
, m_pReplayFile( new VistaConnectionFile( sFilename, VistaConnectionFile::READ ) )
, m_bWaitForSyncTime( true )
{
	if( m_pReplayFile->Open() == false )
		VISTA_THROW( ( "VistaReplayClusterLeaderDataSync - invalid file [" + sFilename + "]" ).c_str(), -1 );
}

VistaReplayClusterFollowerDataSync::VistaReplayClusterFollowerDataSync( VistaConnectionFile* pReplayFile,
																   const bool bVerbose )
: IVistaClusterDataSync( bVerbose, false )
, m_pReplayFile( pReplayFile )
{

}

VistaReplayClusterFollowerDataSync::~VistaReplayClusterFollowerDataSync()
{
	m_pReplayFile->Close();
	delete m_pReplayFile;
}

bool VistaReplayClusterFollowerDataSync::SyncTime( VistaType::systemtime& nTime )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );
	//if( bRes )	
	{
		Wait();
		m_pReplayFile->ReadFloat64( nTime );
	}
	return true;
}

bool VistaReplayClusterFollowerDataSync::SyncData( VistaPropertyList& oList )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );
	//if( bRes )	
	{
		Wait();
		std::string sDummy;
		VistaPropertyList::DeSerializePropertyList( *m_pReplayFile, oList, sDummy );
	}
	return true;
}

bool VistaReplayClusterFollowerDataSync::SyncData( IVistaSerializable& oSerializable )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );
	//if( bRes )	
	{
		Wait();
		m_pReplayFile->ReadSerializable( oSerializable );
	}
	return true;
}

bool VistaReplayClusterFollowerDataSync::SyncData( VistaType::byte* pData, const int iDataSize )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );
	//if( bRes )	
	{
		Wait();
		m_pReplayFile->ReadRawBuffer( pData, iDataSize );
	}
	return true;
}

bool VistaReplayClusterFollowerDataSync::SyncData( VistaType::byte* pDataBuffer, const int iBufferSize, int& iDataSize )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );
	//if( bRes )	
	{
		Wait();
		VistaType::sint32 nActualData = -1;
		m_pReplayFile->ReadInt32( nActualData );
		m_pReplayFile->ReadRawBuffer( pDataBuffer, nActualData );
		iDataSize = nActualData;
	}
	return true;
}

bool VistaReplayClusterFollowerDataSync::SyncData( std::vector<VistaType::byte>& vecData )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );
	//if( bRes )	
	{
		Wait();
		VistaType::sint32 nActualData = -1;
		m_pReplayFile->ReadInt32( nActualData );
		vecData.resize( nActualData );
		m_pReplayFile->ReadRawBuffer( &vecData[0], nActualData );
	}
	return true;
}

bool VistaReplayClusterFollowerDataSync::SyncData( std::string& sData )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );
	//if( bRes )	
	{
		Wait();
		VistaType::sint32 nActualData = -1;
		m_pReplayFile->ReadInt32( nActualData );
		sData.resize( nActualData );
		m_pReplayFile->ReadRawBuffer( &sData[0], nActualData );
	}
	return true;
}

bool VistaReplayClusterFollowerDataSync::GetIsValid() const
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );	
	//return bRes;
	return true;
}

std::string VistaReplayClusterFollowerDataSync::GetDataSyncType() const
{
	//std::string sRes;
	//m_pReplayFile->ReadEncodedString( sRes );
	//return sRes;
	return "VistaReplayClusterFollowerDataSync";
}

int VistaReplayClusterFollowerDataSync::GetNumberOfFollowers() const
{
	//VistaType::uint32 nRes;
	//m_pReplayFile->ReadInt32( nRes );
	//return (int)nRes;
	return 0;
}

int VistaReplayClusterFollowerDataSync::GetNumberOfActiveFollowers() const
{
	//VistaType::uint32 nRes;
	//m_pReplayFile->ReadInt32( nRes );
	//return (int)nRes;
	return 0;
}

int VistaReplayClusterFollowerDataSync::GetNumberOfDeadFollowers() const
{
	//VistaType::uint32 nRes;
	//m_pReplayFile->ReadInt32( nRes );
	//return (int)nRes;
	return 0;
}

std::string VistaReplayClusterFollowerDataSync::GetFollowerNameForId( const int nID ) const
{
	//std::string sRes;
	//m_pReplayFile->ReadEncodedString( sRes );
	//return sRes;
	return "";
}

int VistaReplayClusterFollowerDataSync::GetFollowerIdForName( const std::string& sName ) const
{
	//VistaType::uint32 nRes;
	//m_pReplayFile->ReadInt32( nRes );
	//return (int)nRes;
	return -1;
}

bool VistaReplayClusterFollowerDataSync::GetFollowerIsAlive( const int nID ) const
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );	
	//return bRes;
	return false;
}

int VistaReplayClusterFollowerDataSync::GetLastChangedFollower()
{
	//VistaType::uint32 nRes;
	//m_pReplayFile->ReadInt32( nRes );
	//return (int)nRes;
	return -1;
}

bool VistaReplayClusterFollowerDataSync::DeactivateFollower( const std::string& sName )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );	
	//return bRes;
	return false;
}

bool VistaReplayClusterFollowerDataSync::DeactivateFollower( const int nID )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );	
	//return bRes;
	return false;
}

int VistaReplayClusterFollowerDataSync::GetSendBlockingThreshold() const
{
	//VistaType::uint32 nRes;
	//m_pReplayFile->ReadInt32( nRes );
	//return (int)nRes;
	return 0;
}

bool VistaReplayClusterFollowerDataSync::SetSendBlockingThreshold( const int nNumBytes )
{
	//bool bRes;
	//m_pReplayFile->ReadBool( bRes );	
	//return bRes;
	return false;
}

void VistaReplayClusterFollowerDataSync::Wait()
{
	VistaType::microtime nTargetTime;
	m_pReplayFile->ReadFloat64( nTargetTime );

	if( m_bWaitForSyncTime )
	{
		while( nTargetTime - m_oTimer.GetLifeTime() > 0.015 )
		{
			VistaTimeUtils::Sleep( (int) ( nTargetTime - m_oTimer.GetLifeTime() ) * 1000 );
		}
		// half a millisec tolerance
		while( nTargetTime - m_oTimer.GetLifeTime() > 0.0005 )
		{
			// @todo: microsleep?
		}
	}
}


