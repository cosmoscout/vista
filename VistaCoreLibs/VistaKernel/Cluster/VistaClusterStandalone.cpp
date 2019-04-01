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



#include "VistaClusterStandalone.h"

#include <VistaKernel/Cluster/Utils/VistaStandaloneDataTunnel.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterDataSync.h>
#include <VistaInterProcComm/Cluster/Imps/VistaDummyClusterDataCollect.h>

#include <VistaBase/VistaTimeUtils.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterStandalone::VistaClusterStandalone()
: VistaClusterMode()
, m_pDefaultDataSync( NULL )
, m_pDefaultBarrier( NULL )
{
}

VistaClusterStandalone::~VistaClusterStandalone()
{	
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaClusterStandalone::Init( const std::string& sClusterSection,
									const VistaPropertyList& oConfig )
{
	m_dFrameClock = VistaTimeUtils::GetStandardTimer().GetSystemTime();

	NodeInfo oInfo;
	oInfo.m_sNodeName = GetNodeName();
	oInfo.m_bIsActive = true;
	oInfo.m_iNodeID = 0;
	oInfo.m_eNodeType = NT_STANDALONE;

	m_oClusterInfo.m_vecNodeInfos.push_back( oInfo );

	return true;
}

bool VistaClusterStandalone::PostInit()
{
	return true;
}

int VistaClusterStandalone::GetClusterMode() const
{
	return CM_STANDALONE;
}

std::string VistaClusterStandalone::GetClusterModeName() const
{
	return "STANDALONE";
}

int VistaClusterStandalone::GetNodeType() const
{
	return NT_STANDALONE;
}

std::string VistaClusterStandalone::GetNodeTypeName() const
{
	return "STANDALONE";
}

std::string VistaClusterStandalone::GetNodeName() const
{
	return "standalone";
}

std::string VistaClusterStandalone::GetConfigSectionName() const
{
	return "SYSTEM";
}

std::string VistaClusterStandalone::GetNodeName( const int iNodeID ) const
{
	if( iNodeID == 0 )
		return GetNodeName();
	else
		return "";
}

int VistaClusterStandalone::GetNodeID() const
{
	return 0;
}


bool VistaClusterStandalone::GetIsLeader() const
{
	return true;
}

bool VistaClusterStandalone::GetIsFollower() const
{
	return false;
}

int VistaClusterStandalone::GetNumberOfNodes() const
{
	return 1;
}

bool VistaClusterStandalone::StartFrame()
{
	m_dFrameClock = VistaTimeUtils::GetStandardTimer().GetSystemTime();
	++m_iFrameCount;
	return true;
}

bool VistaClusterStandalone::ProcessFrame()
{
	return true;
}

bool VistaClusterStandalone::EndFrame()
{
	return true;
}

void VistaClusterStandalone::SwapSync()
{
	return;
}

bool VistaClusterStandalone::CreateConnections( std::vector<VistaConnectionIP*>& vecConnections )
 {
	 return true;
 }
bool VistaClusterStandalone::CreateNamedConnections( 
				std::vector<std::pair<VistaConnectionIP*, std::string> >& vecConnections )
{
	return true;
}

IVistaDataTunnel* VistaClusterStandalone::CreateDataTunnel( IDLVistaDataPacket* pPacketProto )
{
	return new VistaStandaloneDataTunnel;
}

void VistaClusterStandalone::Debug( std::ostream& oStream ) const
{
	oStream << "VistaClusterMode: STANDALONE" << std::endl;
}

IVistaClusterDataSync* VistaClusterStandalone::CreateDataSync()
{
	return new VistaDummyClusterDataSync;
}

IVistaClusterDataSync* VistaClusterStandalone::GetDefaultDataSync()
{
	if( m_pDefaultDataSync == NULL )
		m_pDefaultDataSync = new VistaDummyClusterDataSync;
	return m_pDefaultDataSync;
}

IVistaClusterBarrier* VistaClusterStandalone::CreateBarrier()
{
	return new VistaDummyClusterBarrier;
}

IVistaClusterBarrier* VistaClusterStandalone::GetDefaultBarrier()
{
	if( m_pDefaultBarrier == NULL )
		m_pDefaultBarrier = new VistaDummyClusterBarrier;
	return m_pDefaultBarrier;
}

IVistaClusterDataCollect* VistaClusterStandalone::CreateDataCollect()
{
	return new VistaDummyClusterDataCollect;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


