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


#include "VistaOptitrackDriver.h"

#include "VistaOptitrackCommonShare.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaUtilityMacros.h>


#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

#include <NatNetTypes.h>
#include <NatNetClient.h>
#include <NatNetCAPI.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace
{
	void DataCallback( sFrameOfMocapData* pFrameOfData, void* pUserData )
	{
		assert( pUserData != NULL );
		VistaOptitrackDriver* pDriver = static_cast< VistaOptitrackDriver* >( pUserData );
		pDriver->SetCurrentFrameData( pFrameOfData );
		pDriver->Update();
	}

	void MessageCallback( Verbosity nVerbosity, const char* sMessage )
	{
		vstr::outi() << "[Optitrack]: \"" << sMessage << "\"" << std::endl;
	}
}

/*============================================================================*/
/* CREATION METHOD                                                           */
/*============================================================================*/


VistaOptitrackCreationMethod::VistaOptitrackCreationMethod( IVistaTranscoderFactoryFactory* pMetaFactory )
: IVistaDriverCreationMethod( pMetaFactory )
{
	RegisterSensorType( "RIGID_BODY", sizeof( VistaOptitrackMeasures::RigidBodyMeasure ),
							240, pMetaFactory->CreateFactoryForType( "RIGID_BODY" ) );
}

IVistaDeviceDriver* VistaOptitrackCreationMethod::CreateDriver()
{
	return new VistaOptitrackDriver(this);
}

/*============================================================================*/
/* PARAMETERS                                                                 */
/*============================================================================*/

REFL_IMPLEMENT_FULL( VistaOptitrackDriver::Parameters, VistaDriverGenericParameterAspect::IParameterContainer );

namespace
{
	
	IVistaPropertyGetFunctor* s_aParameterGetter[] =
	{
		new TVistaPublicVariablePropertyGet< VistaOptitrackDriver::Parameters, bool, VistaProperty::PROPT_BOOL >(
						"REPORT_MESSAGES",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_bReportMessages,
						"If true, messages from the server will be printed to the console" ),
		new TVistaPublicVariablePropertyGet< VistaOptitrackDriver::Parameters, bool, VistaProperty::PROPT_BOOL >(
						"SERVER_USES_MULTICAST",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_bServerUsesMulticast,
						"indicates wether the server distributes data using multicast or unicast UDP" ),
		new TVistaPublicVariablePropertyGet< VistaOptitrackDriver::Parameters, std::string, VistaProperty::PROPT_STRING >(
						"MULTICAST_ADDRESS",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_sMulticastAddress,
						"Multicast address used by the server" ),
		new TVistaPublicVariablePropertyGet< VistaOptitrackDriver::Parameters, std::string, VistaProperty::PROPT_STRING >(
						"SERVER_HOSTNAME",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_sServerHostName,
						"host name of the optitrack server" ),
		new TVistaPublicVariablePropertyGet< VistaOptitrackDriver::Parameters, std::string, VistaProperty::PROPT_STRING >(
						"OWN_HOSTNAME",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_sOwnHostName,
						"own host nameP" ),
		new TVistaPublicVariablePropertyGet< VistaOptitrackDriver::Parameters, int, VistaProperty::PROPT_INT >(
						"SERVER_COMMAND_PORT",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_nServerCommandPort,
						"command port of the optitrack server (uses default if <= 0)" ),
		new TVistaPublicVariablePropertyGet< VistaOptitrackDriver::Parameters, int, VistaProperty::PROPT_INT >(
						"SERVER_DATA_PORT",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_nServerDataPort,
						"data port of the optitrack server (uses default if <= 0)" ),
		NULL
	};

	IVistaPropertySetFunctor* s_aParameterSetter[] =
	{
		new TVistaPublicVariablePropertySet< VistaOptitrackDriver::Parameters, bool >(
						"REPORT_MESSAGES",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_bReportMessages,
						"If true, messages from the server will be printed to the console" ),
		new TVistaPublicVariablePropertySet< VistaOptitrackDriver::Parameters, bool >(
						"SERVER_USES_MULTICAST",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_bServerUsesMulticast,
						"indicates wether the server distributes data using multicast or unicast UDP" ),
		new TVistaPublicVariablePropertySet< VistaOptitrackDriver::Parameters, std::string >(
						"MULTICAST_ADDRESS",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_sMulticastAddress,
						"Multicast address used by the server" ),
		new TVistaPublicVariablePropertySet< VistaOptitrackDriver::Parameters, std::string >(
						"SERVER_HOSTNAME",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_sServerHostName,
						"host name of the optitrack server" ),
		new TVistaPublicVariablePropertySet< VistaOptitrackDriver::Parameters, std::string >(
						"OWN_HOSTNAME",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_sOwnHostName,
						"own host nameP" ),
		new TVistaPublicVariablePropertySet< VistaOptitrackDriver::Parameters, int >(
						"SERVER_COMMAND_PORT",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_nServerCommandPort,
						"command port of the optitrack server (uses default if <= 0)" ),
		new TVistaPublicVariablePropertySet< VistaOptitrackDriver::Parameters, int >(
						"SERVER_DATA_PORT",
						SsReflectionName,
						&VistaOptitrackDriver::Parameters::m_nServerDataPort,
						"data port of the optitrack server (uses default if <= 0)" ),
		
		NULL
	};
}

VistaOptitrackDriver::Parameters::Parameters( IVistaDeviceDriver* )
: IParameterContainer()
, m_bReportMessages( true )
, m_bServerUsesMulticast( false )
, m_nServerCommandPort( -1 )
, m_nServerDataPort( -1 )
{
}


/*============================================================================*/
/* DRIVER                                                                     */
/*============================================================================*/

typedef TVistaDriverEnableAspect< VistaOptitrackDriver > VistaOptitrackEnableAspect;

VistaOptitrackDriver::VistaOptitrackDriver( IVistaDriverCreationMethod* pCreationMethod )
: IVistaDeviceDriver( pCreationMethod )
, m_pParamAspect( NULL )
, m_pSensors( new VistaDriverSensorMappingAspect( pCreationMethod ) )
, m_pClient( NULL )
, m_pCurrentFrameData( NULL )
{
	m_pParamAspect = new VistaDriverGenericParameterAspect( new TParameterCreate< VistaOptitrackDriver, Parameters >( this ) );

	RegisterAspect( m_pSensors );
	RegisterAspect( m_pParamAspect );

	RegisterAspect( new VistaOptitrackEnableAspect( this, &VistaOptitrackDriver::PhysicalEnable ) );

	SetUpdateType( IVistaDeviceDriver::UPDATE_CUSTOM_THREADED );	
}

VistaOptitrackDriver::~VistaOptitrackDriver()
{
	if( m_pClient )
	{
		m_pClient->Disconnect();
		delete m_pClient;
	}

	UnregisterAspect( m_pParamAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pParamAspect;
	UnregisterAspect( m_pSensors, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pSensors;

	VistaOptitrackEnableAspect *enabler = GetAspect< VistaOptitrackEnableAspect >();
	UnregisterAspect( enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete enabler;

}

bool VistaOptitrackDriver::DoSensorUpdate( VistaType::microtime dTimestamp )
{
	const unsigned int nRigidBodySensorType = m_pSensors->GetTypeId( "RIGID_BODY" );

	for( int nRigidBody = 0; nRigidBody < m_pCurrentFrameData->nRigidBodies; ++nRigidBody )
	{
		sRigidBodyData& oRigidBody = m_pCurrentFrameData->RigidBodies[ nRigidBody ];
		unsigned int nRigidBodySensorIndex = m_pSensors->GetSensorId( nRigidBodySensorType, oRigidBody.ID );
		if( nRigidBodySensorIndex != VistaDriverSensorMappingAspect::INVALID_TYPE )
		{
			VistaDeviceSensor* pSensor = GetSensorByIndex( nRigidBodySensorIndex );
			assert( pSensor );

			VistaSensorMeasure* pMeasure = MeasureStart( *pSensor, dTimestamp, RETURN_CURRENT_SLOT );
			assert( pMeasure );

			VistaOptitrackMeasures::RigidBodyMeasure* pData = pMeasure->getWrite< VistaOptitrackMeasures::RigidBodyMeasure >();
			pData->m_nIndex       = oRigidBody.ID;
			pData->m_fMeanError   = oRigidBody.MeanError;
			pData->m_v3Position   = VistaVector3D( oRigidBody.x, oRigidBody.y, oRigidBody.z );
			pData->m_qOrientation = VistaQuaternion( oRigidBody.qx, oRigidBody.qy, oRigidBody.qz, oRigidBody.qw );

			MeasureStop( *pSensor );
		}
	}

	return true;
}

bool VistaOptitrackDriver::PhysicalEnable(bool bEnable)
{
	if( bEnable )
	{
		m_pClient->SetFrameReceivedCallback( &DataCallback, this );
	}
	else
	{
		m_pClient->SetFrameReceivedCallback( NULL );
	}
	return true;
}

const VistaOptitrackDriver::Parameters* VistaOptitrackDriver::GetParameters() const
{
	return Vista::assert_cast< Parameters* >( m_pParamAspect->GetParameterContainer() );
}

VistaOptitrackDriver::Parameters* VistaOptitrackDriver::GetParameters()
{
	return Vista::assert_cast< Parameters* >( m_pParamAspect->GetParameterContainer() );
}

bool VistaOptitrackDriver::DoConnect()
{
	if( m_pClient )
	{
		m_pClient->Disconnect();
		delete m_pClient;
		m_pClient = NULL;
	}

	Parameters* pParams = GetParameters();
	
	if( pParams->m_sOwnHostName.empty() )
	{
		vstr::errp() << "[VistaOptitrackDriver]: no OWN_HOSTNAME given" << std::endl;
		return false;
	}
	if( pParams->m_sOwnHostName.empty() )
	{
		vstr::errp() << "[VistaOptitrackDriver]: no OWN_HOSTNAME given" << std::endl;
		return false;
	}
	
	m_pClient = new NatNetClient();

	if( pParams->m_bReportMessages )
		NatNet_SetLogCallback( MessageCallback );

	bool bHasDataPort = ( pParams->m_nServerDataPort > 0 );
	bool bHasCommandPort = ( pParams->m_nServerCommandPort > 0 );
	bool bHasMulticastAddress = ( pParams->m_sMulticastAddress.empty() == false );

	sNatNetClientConnectParams params;
	params.connectionType = ConnectionType_Unicast;
	params.serverAddress = &pParams->m_sServerHostName[0];
	params.localAddress = &pParams->m_sOwnHostName[0];

	if ( bHasMulticastAddress )
	{
		params.multicastAddress = &pParams->m_sMulticastAddress[0];
	}
	else 
	{
		params.connectionType = ConnectionType_Unicast;
	}

	if( bHasDataPort && bHasCommandPort )
	{
		params.serverDataPort = pParams->m_nServerDataPort;
		params.serverCommandPort = pParams->m_nServerCommandPort;
	}
	else if( bHasCommandPort )
	{
		params.serverCommandPort = pParams->m_nServerCommandPort;
	}

	int nReturnValue = m_pClient->Connect( params );

	if( nReturnValue != ErrorCode_OK )
	{
		vstr::errp() << "[VistaOptitrackDriver]: initialization failed" << std::endl;
		return false;
	}

	return true;
}


bool VistaOptitrackDriver::DoDisconnect()
{
	if( m_pClient )
	{
		m_pClient->Disconnect();
		delete m_pClient;
		m_pClient = NULL;
	}
	return true;
}

void VistaOptitrackDriver::SetCurrentFrameData( sFrameOfMocapData* pValue )
{
	m_pCurrentFrameData = pValue;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

