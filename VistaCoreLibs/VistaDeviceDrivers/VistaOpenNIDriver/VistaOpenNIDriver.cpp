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


#include "VistaOpenNIDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <stdio.h>
#include <string.h>

#include <algorithm>

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#define CHECK_KINECT_STATUS( iCode, sErrorName )							\
	if( iCode != XN_STATUS_OK )												\
	{																		\
		std::cerr << "[VistaOpenNIDriver]: " << sErrorName << " - ["		\
				<< xnGetStatusString( nReturn ) << "]" << std::endl;		\
		Cleanup();															\
		return false;														\
	}

namespace 
{
	XnSkeletonJoint S_aOpenNIJointIDIndices[] =
	{
			XN_SKEL_HEAD,
			XN_SKEL_NECK,
			XN_SKEL_TORSO,
			XN_SKEL_LEFT_SHOULDER,
			XN_SKEL_LEFT_ELBOW,
			XN_SKEL_LEFT_HAND,
			XN_SKEL_LEFT_HIP,
			XN_SKEL_LEFT_KNEE,
			XN_SKEL_LEFT_FOOT,
			XN_SKEL_RIGHT_SHOULDER,
			XN_SKEL_RIGHT_ELBOW,
			XN_SKEL_RIGHT_HAND,
			XN_SKEL_RIGHT_HIP,
			XN_SKEL_RIGHT_KNEE,
			XN_SKEL_RIGHT_FOOT,
	};
}
/*============================================================================*/
/* OpenNI Callbacks                                                           */
/*============================================================================*/

void XN_CALLBACK_TYPE NewUserCallback( xn::UserGenerator& oGenerator, XnUserID nId, void* pCookie )
{
	std::cout << "[Kinect]: Detected new user " << nId << std::endl;
	if( oGenerator.GetSkeletonCap().NeedPoseForCalibration() )
	{
		char sPose[50];
		oGenerator.GetSkeletonCap().GetCalibrationPose( sPose );
		oGenerator.GetPoseDetectionCap().StartPoseDetection( sPose, nId );
	}
	else
	{
		oGenerator.GetSkeletonCap().RequestCalibration( nId, TRUE );
	}
}

void XN_CALLBACK_TYPE LostUserCallback( xn::UserGenerator& oGenerator, XnUserID nId, void* pCookie )
{	
	std::cout << "[Kinect]: LOST user " << nId << std::endl;
}

void XN_CALLBACK_TYPE UserPoseDetected( xn::PoseDetectionCapability& oPose,
										const XnChar* sPose, XnUserID nId, void* pCookie )
{
	std::cout << "[Kinect]: Detected pose [" << sPose << "] of User " << nId << std::endl;
	oPose.StopPoseDetection( nId );
	static_cast<VistaOpenNIDriver*>( pCookie )
				->GetUserGen()->GetSkeletonCap()
						.RequestCalibration( nId, true );
}

void XN_CALLBACK_TYPE StartCalibrationCallback( xn::SkeletonCapability& oSkelleton,
													   XnUserID nId, void* pCookie )
{
	std::cout << "[Kinect]: Starting calibration for User " << nId << std::endl;
}

void XN_CALLBACK_TYPE StopCalibrationCallback( xn::SkeletonCapability& oSkeleton, 
											  XnUserID nId, XnBool bSuccess, void* pCookie )
{
	std::cout << "[Kinect]: Ending calibration for User " << nId;
	if( bSuccess )
	{
		std::cout << " successfully - starting tracking!" << std::endl;
		oSkeleton.StartTracking(nId);
		if( oSkeleton.IsTracking(nId) == false )
			std::cout << "Couldnt start tracking!" << std::endl;
	}
	else
	{
		std::cout << " FAILED!" << std::endl;
		if( oSkeleton.NeedPoseForCalibration() )
		{
			char sPose[50];
			oSkeleton.GetCalibrationPose( sPose );
			static_cast<VistaOpenNIDriver*>( pCookie )
						->GetUserGen()->GetPoseDetectionCap()
								.StartPoseDetection( sPose, nId);
		}
		else
		{
			oSkeleton.RequestCalibration( nId, true );
		}
	}
};
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaOpenNIDriverCreationMethod::VistaOpenNIDriverCreationMethod( 
								IVistaTranscoderFactoryFactory *pTransFac )
: IVistaDriverCreationMethod( pTransFac )
{
	RegisterSensorType( "SKELETON",
						 sizeof( VistaOpenNIDriver::SkeletonData ),
						 30, pTransFac->CreateFactoryForType( "SKELETON" ) );
	RegisterSensorType( "DEPTHMAP",
		sizeof( VistaOpenNIDriver::DepthData ),
						 30, pTransFac->CreateFactoryForType( "DEPTHMAP" ) );
	RegisterSensorType( "IMAGE",
						 sizeof( VistaOpenNIDriver::ImageData ),
						 30, pTransFac->CreateFactoryForType( "IMAGE" ) );
}

IVistaDeviceDriver* VistaOpenNIDriverCreationMethod::CreateDriver()
{
	return new VistaOpenNIDriver( this );
}


typedef TVistaDriverEnableAspect< VistaOpenNIDriver > VistaOpenNIEnableAspect;


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenNIDriver::VistaOpenNIDriver( IVistaDriverCreationMethod* pCreaMethod )
: IVistaDeviceDriver( pCreaMethod )
, m_pMappingAspect( new VistaDriverSensorMappingAspect( pCreaMethod ) )
, m_pContext( NULL )
, m_pDepthGen( NULL )
, m_pUserGen( NULL )
, m_pImageGen( NULL )
, m_iTrackedUser( -1 )
, m_dTrackedUserLastSeen( 0 )
{
	m_pThreadAspect = new VistaDriverThreadAspect( this );
	SetUpdateType( IVistaDeviceDriver::UPDATE_CUSTOM_THREADED );
	RegisterAspect( m_pThreadAspect );
	RegisterAspect( m_pMappingAspect );
	RegisterAspect( new VistaOpenNIEnableAspect( this, &VistaOpenNIDriver::PhysicalEnable ) );
}

VistaOpenNIDriver::~VistaOpenNIDriver()
{
	Cleanup();

	UnregisterAspect( m_pThreadAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pThreadAspect;

	UnregisterAspect( m_pMappingAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pMappingAspect;

	VistaOpenNIEnableAspect *enabler = GetAspectAs< VistaOpenNIEnableAspect >( VistaDriverEnableAspect::GetAspectId() );
	UnregisterAspect( enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete enabler;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaOpenNIDriver::DoConnect()
{
	if( m_pContext != NULL )
		return false;

	m_pContext = new xn::Context;

	// check which sensors are requested, and buffer their IDs
	unsigned int nSensorType;
	
	nSensorType = m_pMappingAspect->GetTypeId( "DEPTH" );
	for( std::size_t n = 0; n < m_pMappingAspect->GetNumRegisteredSensorsForType(nSensorType); ++n)
	{
		unsigned int nSensorId = m_pMappingAspect->GetSensorId( nSensorType, n );
		if( nSensorId != ~0u )
			m_vecDepthMapSensorIDs.push_back( nSensorId );
	}

	nSensorType = m_pMappingAspect->GetTypeId( "IMAGE" );
	for( std::size_t n = 0; n < m_pMappingAspect->GetNumRegisteredSensorsForType(nSensorType); ++n)
	{
		unsigned int nSensorId = m_pMappingAspect->GetSensorId( nSensorType, n );
		if( nSensorId != ~0u )
			m_vecImageSensorIDs.push_back( nSensorId );
	}

	nSensorType = m_pMappingAspect->GetTypeId( "SKELETON" );
	for( std::size_t n = 0; n < m_pMappingAspect->GetNumRegisteredSensorsForType(nSensorType); ++n)
	{
		unsigned int nSensorId = m_pMappingAspect->GetSensorId( nSensorType, n );
		if( nSensorId != ~0u )
			m_vecSkeletonSensorIDs.push_back( nSensorId );
	}

	bool bCreateDepthMap = ( m_vecDepthMapSensorIDs.empty() == false
							|| m_vecSkeletonSensorIDs.empty() == false );
	bool bCreateImageMap = ( m_vecImageSensorIDs.empty() == false );
	bool bCreateSkeleton = ( m_vecSkeletonSensorIDs.empty() == false );	


	XnStatus nReturn = m_pContext->Init();
	CHECK_KINECT_STATUS( nReturn, "Could not initialize Context" );

	if( bCreateDepthMap )
	{
		m_pDepthGen = new xn::DepthGenerator;
		nReturn = m_pContext->FindExistingNode( XN_NODE_TYPE_DEPTH, *m_pDepthGen );
		
		if( nReturn != XN_STATUS_OK )
			nReturn = m_pDepthGen->Create( *m_pContext );
		CHECK_KINECT_STATUS( nReturn, "Could not create Depth generator" );

		XnMapOutputMode oDepthOutput;
		oDepthOutput.nFPS = 30;
		oDepthOutput.nXRes = 640;
		oDepthOutput.nYRes = 480;
		m_pDepthGen->SetMapOutputMode( oDepthOutput );
		m_pDepthGen->GetMirrorCap().SetMirror( true );
	}

	if( bCreateImageMap )
	{
		m_pImageGen = new xn::ImageGenerator;

		nReturn = m_pContext->FindExistingNode( XN_NODE_TYPE_IMAGE, *m_pDepthGen );
		
		if( nReturn != XN_STATUS_OK )
			nReturn = m_pImageGen->Create( *m_pContext );
		CHECK_KINECT_STATUS( nReturn, "Could not create Depth generator" );

		XnMapOutputMode oImageOutput;
		oImageOutput.nFPS = 30;
		oImageOutput.nXRes = 640;
		oImageOutput.nYRes = 480;
		m_pImageGen->SetMapOutputMode( oImageOutput );
		m_pImageGen->GetMirrorCap().SetMirror( true );
	}

	if( bCreateSkeleton )
	{
		m_pUserGen = new xn::UserGenerator;

		nReturn = m_pContext->FindExistingNode(XN_NODE_TYPE_USER, *m_pUserGen );
		if( nReturn != XN_STATUS_OK )
			nReturn = m_pUserGen->Create( *m_pContext );

		CHECK_KINECT_STATUS( nReturn, "Could not find and create user generator" );

		if( m_pUserGen->IsCapabilitySupported( XN_CAPABILITY_SKELETON ) == false )
		{
			std::cout << "[Kinect]: Supplied user generator doesn't support skeleton" << std::endl;
			Cleanup();
			return false;
		}

		XnCallbackHandle oUserCallbackHandle;
		XnCallbackHandle oSkeletonCallbackHandle;
		XnCallbackHandle oPoseCallbackHandle;

		m_pUserGen->RegisterUserCallbacks( NewUserCallback, LostUserCallback, this, oUserCallbackHandle );
		m_pUserGen->GetSkeletonCap().RegisterCalibrationCallbacks( StartCalibrationCallback, StopCalibrationCallback,
															this, oSkeletonCallbackHandle );

		if( m_pUserGen->GetSkeletonCap().NeedPoseForCalibration() )
		{
			if( m_pUserGen->IsCapabilitySupported( XN_CAPABILITY_POSE_DETECTION ) == false )
			{
				std::cout << "Skeleton detection requires pose, but Pose is not supported" << std::endl;
				Cleanup();
				return false;
			}
			m_pUserGen->GetPoseDetectionCap().RegisterToPoseCallbacks( UserPoseDetected, NULL, this, oPoseCallbackHandle );		
		}

		m_pUserGen->GetSkeletonCap().SetSkeletonProfile( XN_SKEL_PROFILE_ALL );
	}

	m_pContext->StartGeneratingAll();

	return true;
}

bool VistaOpenNIDriver::DoDisconnect()
{
	return true;
}

bool VistaOpenNIDriver::PhysicalEnable( bool bEnable )
{
	/** @todo: check return type */
	if( bEnable )
		m_pContext->StartGeneratingAll();
	else
		m_pContext->StopGeneratingAll();
	return true;
}

bool VistaOpenNIDriver::DoSensorUpdate( VistaType::microtime nTime )
{
	if( m_pContext == NULL )
		return false;


	XnStatus nReturn = m_pContext->WaitAndUpdateAll();

	if( nReturn != XN_STATUS_OK )
		return false;

	ReadDepthMeasure( nTime );
	ReadImageMeasure( nTime );
	DetermineTrackedUser( nTime );
	ReadSkeletonData( nTime );

	return true;
}

void VistaOpenNIDriver::DetermineTrackedUser( VistaType::microtime nTime )
{
	if( m_pUserGen == NULL )
		return;

	if( m_iTrackedUser != -1 )
	{
		// check if still tracking
		if( m_pUserGen->GetSkeletonCap().IsTracking( m_iTrackedUser ) == false )
		{
			/** @todo: adjustable treshold */
			if( nTime - m_dTrackedUserLastSeen > 3.0 )
				m_iTrackedUser = -1;
			return;
		}
	}

	// no user yet, let's try to find a new tracked one
	XnUserID aUsers[16];
	XnUInt16 iUserCount;
	m_pUserGen->GetUsers( aUsers, iUserCount );

	for( XnUInt16 i = 0; i < iUserCount; ++i )
	{
		if( m_pUserGen->GetSkeletonCap().IsTracking( aUsers[i] ) )
		{
			m_iTrackedUser = aUsers[i];
			break;
		}			
	}
	
	if( m_iTrackedUser != -1 )
		m_dTrackedUserLastSeen = VistaTimeUtils::GetStandardTimer().GetMicroTime();
}

void VistaOpenNIDriver::ReadDepthMeasure( VistaType::microtime nTime )
{
	if( m_vecDepthMapSensorIDs.empty() )
		return;

	xn::DepthMetaData oData;
	m_pDepthGen->GetMetaData( oData );

	for( std::vector<int>::const_iterator itSensorID = m_vecDepthMapSensorIDs.begin();
		itSensorID != m_vecDepthMapSensorIDs.end(); ++itSensorID )
	{
		VistaDeviceSensor *pSensor = GetSensorByIndex( (*itSensorID) );
		MeasureStart( (*itSensorID), nTime );

		VistaSensorMeasure* pMeasure = m_pHistoryAspect->GetCurrentSlot( pSensor );
		DepthData* pDepthMeasure = reinterpret_cast<DepthData*>( &(*pMeasure).m_vecMeasures[0] );

		int iSize = oData.XRes() * oData.YRes();
		if( pDepthMeasure->m_pBuffer == NULL )
		{
			pDepthMeasure->m_iResX = oData.XRes();
			pDepthMeasure->m_iResY = oData.YRes();
			pDepthMeasure->m_pBuffer = new unsigned short[iSize];
		}
		else if( pDepthMeasure->m_iResX != oData.XRes() 
			||  pDepthMeasure->m_iResY != oData.YRes()  )
		{
			delete [] pDepthMeasure->m_pBuffer;
			pDepthMeasure->m_iResX = oData.XRes();
			pDepthMeasure->m_iResY = oData.YRes();
			pDepthMeasure->m_pBuffer = new unsigned short[iSize];
		}

		memcpy( pDepthMeasure->m_pBuffer, oData.Data(), iSize*sizeof(unsigned short) );

		MeasureStop( (*itSensorID) );
	}	
}

void VistaOpenNIDriver::ReadImageMeasure( VistaType::microtime nTime )
{
	if( m_vecImageSensorIDs.empty() )
		return;

	xn::ImageMetaData oData;
	m_pImageGen->GetMetaData( oData );

	for( std::vector<int>::const_iterator itSensorID = m_vecImageSensorIDs.begin();
		itSensorID != m_vecImageSensorIDs.end(); ++itSensorID )
	{
		VistaDeviceSensor *pSensor = GetSensorByIndex( (*itSensorID) );
		MeasureStart( (*itSensorID), nTime );

		VistaSensorMeasure* pMeasure = m_pHistoryAspect->GetCurrentSlot( pSensor );
		ImageData* pImageMeasure = reinterpret_cast<ImageData*>( &(*pMeasure).m_vecMeasures[0] );

		int iSize = oData.XRes() * oData.YRes();
		if( pImageMeasure->m_pBuffer == NULL )
		{
			pImageMeasure->m_iResX = oData.XRes();
			pImageMeasure->m_iResY = oData.YRes();
			pImageMeasure->m_pBuffer = new ImageData::Pixel[iSize];
		}
		else if( pImageMeasure->m_iResX != oData.XRes() 
			||  pImageMeasure->m_iResY != oData.YRes()  )
		{
			delete [] pImageMeasure->m_pBuffer;
			pImageMeasure->m_iResX = oData.XRes();
			pImageMeasure->m_iResY = oData.YRes();
			pImageMeasure->m_pBuffer = new ImageData::Pixel[iSize];
		}

		memcpy( pImageMeasure->m_pBuffer, oData.Data(), iSize*sizeof(ImageData::Pixel) );

		MeasureStop( (*itSensorID) );
	}	
}

void VistaOpenNIDriver::ReadSkeletonData( VistaType::microtime nTime )
{
	if( m_vecSkeletonSensorIDs.empty() || m_iTrackedUser == -1 )
		return;

	if( m_pUserGen->GetSkeletonCap().IsTracking( m_iTrackedUser ) == false )
		return;

	for( std::vector<int>::const_iterator itSensorID = m_vecSkeletonSensorIDs.begin();
		itSensorID != m_vecSkeletonSensorIDs.end(); ++itSensorID )
	{
		xn::SkeletonCapability oSkel = m_pUserGen->GetSkeletonCap();
		VistaDeviceSensor *pSensor = GetSensorByIndex( (*itSensorID) );

		MeasureStart( (*itSensorID), nTime );

		VistaSensorMeasure* pMeasure = m_pHistoryAspect->GetCurrentSlot( pSensor );
		SkeletonData* pSkelMeasure = reinterpret_cast<SkeletonData*>( &(*pMeasure).m_vecMeasures[0] );

		// aparently, there's no way to retrieve all the data at once
		XnSkeletonJointTransformation oJoint;
		
		for( int i = SKELETON_FIRST; i < SKELETON_COUNT; ++i )
		{
			Joint& oTarget = pSkelMeasure->m_aJoints[i];
			oSkel.GetSkeletonJoint( m_iTrackedUser, S_aOpenNIJointIDIndices[i], oJoint );
			oTarget.m_a3fPosition[0] = oJoint.position.position.X;
			oTarget.m_a3fPosition[1] = oJoint.position.position.Y;
			oTarget.m_a3fPosition[2] = oJoint.position.position.Z;	
			memcpy( oTarget.m_a3x3fOrientation,
					oJoint.orientation.orientation.elements, 9*sizeof(float) );
			oTarget.m_fConfidence = oJoint.position.fConfidence;
		}		

		MeasureStop( (*itSensorID) );
	}
}

xn::Context* VistaOpenNIDriver::GetContext() const
{
	return m_pContext;
}
xn::DepthGenerator* VistaOpenNIDriver::GetDepthGen() const
{
	return m_pDepthGen;
}
xn::UserGenerator* VistaOpenNIDriver::GetUserGen() const
{
	return m_pUserGen;
}
xn::ImageGenerator* VistaOpenNIDriver::GetImageGen() const
{
	return m_pImageGen;
}

void VistaOpenNIDriver::Cleanup()
{
	delete m_pUserGen;
	m_pUserGen = NULL;
	delete m_pDepthGen;
	m_pDepthGen = NULL;
	delete m_pImageGen;
	m_pImageGen = NULL;
	delete m_pContext;
	m_pContext = NULL;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "MYDEMO.CPP"                                                   */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/

