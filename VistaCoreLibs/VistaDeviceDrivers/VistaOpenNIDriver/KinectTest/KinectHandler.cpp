/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                             .                              */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      Header   :                               RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :                               RR  R    WW  WW  TT  HH  HH  */
/*                                                                            */
/*      Project  :  Vista                          Rheinisch-Westfaelische    */
/*                                               Technische Hochschule Aachen */
/*      Purpose  :  DFN action Node to receive position and orientation       */
/*                  and orientation data from input devices, and also         */
/*                  records command key events                                */
/*                                                                            */
/*                                                 Copyright (c)  1998-2016   */
/*                                                 by  RWTH-Aachen, Germany   */
/*                                                 All rights reserved.       */
/*                                             .                              */
/*============================================================================*/


#include "KinectHandler.h" 

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include <VistaDeviceDrivers/VistaOpenNIDriver/VistaOpenNIDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

REFL_IMPLEMENT_FULL(KinectHandler, IVdfnActionObject);

namespace
{
	//HEre we define some inports
	IVistaPropertySetFunctor *SaSetter[] =
	{
		new TActionSetFunctor<KinectHandler, 
						std::vector<VistaVector3D>, const std::vector<VistaVector3D>&>(
									&KinectHandler::SetPosition,
									NULL,
									"position", 
									SsReflectionName, 
									"sets position of objects" ),
		new TActionSetFunctor<KinectHandler, 
						VistaTransformMatrix, const VistaTransformMatrix&>(
									&KinectHandler::SetTransformation,
									NULL,
									"transform", 
									SsReflectionName, 
									"sets transformation from device to world" ),
		NULL
	};
}

enum MY_ELEMENTS
{
	ELEM_HEAD = 0,
	ELEM_TORSO,
	ELEM_RIGHT_UPPER_ARM,
	ELEM_RIGHT_LOWER_ARM,
	ELEM_RIGHT_HAND,
	ELEM_RIGHT_UPPER_LEG,
	ELEM_RIGHT_LOWER_LEG,
	ELEM_RIGHT_FOOT,
	ELEM_LEFT_UPPER_ARM,
	ELEM_LEFT_LOWER_ARM,
	ELEM_LEFT_HAND,
	ELEM_LEFT_UPPER_LEG,
	ELEM_LEFT_LOWER_LEG,
	ELEM_LEFT_FOOT,
	ELEM_COUNT,
};

static const int S_afElementFromJoint[] = 
{
	VistaOpenNIDriver::SKELETON_HEAD,			// ELEM_HEAD,
	VistaOpenNIDriver::SKELETON_NECK,			// ELEM_TORSO,
	VistaOpenNIDriver::SKELETON_RIGHT_SHOULDER, // ELEM_RIGHT_UPPER_ARM,
	VistaOpenNIDriver::SKELETON_RIGHT_ELBOW,	// ELEM_RIGHT_LOWER_ARM,
	VistaOpenNIDriver::SKELETON_RIGHT_HAND,		// ELEM_RIGHT_HAND,
	VistaOpenNIDriver::SKELETON_RIGHT_HIP,		// ELEM_RIGHT_UPPER_LEG,
	VistaOpenNIDriver::SKELETON_RIGHT_KNEE,		// ELEM_RIGHT_LOWER_LEG,
	VistaOpenNIDriver::SKELETON_RIGHT_FOOT,		// ELEM_RIGHT_FOOT,
	VistaOpenNIDriver::SKELETON_LEFT_SHOULDER,	// ELEM_LEFT_UPPER_ARM,
	VistaOpenNIDriver::SKELETON_LEFT_ELBOW,		// ELEM_LEFT_LOWER_ARM,
	VistaOpenNIDriver::SKELETON_LEFT_HAND,		// ELEM_LEFT_HAND,
	VistaOpenNIDriver::SKELETON_LEFT_HIP,		// ELEM_LEFT_UPPER_LEG,
	VistaOpenNIDriver::SKELETON_LEFT_KNEE,		// ELEM_LEFT_LOWER_LEG,
	VistaOpenNIDriver::SKELETON_LEFT_FOOT		// ELEM_LEFT_FOOT,
};

static const int S_afElementToJoint[] = 
{
	VistaOpenNIDriver::SKELETON_NECK,			// ELEM_HEAD,
	VistaOpenNIDriver::SKELETON_TORSO,			// ELEM_TORSO,
	VistaOpenNIDriver::SKELETON_RIGHT_ELBOW,	// ELEM_RIGHT_UPPER_ARM,
	VistaOpenNIDriver::SKELETON_RIGHT_HAND,		// ELEM_RIGHT_LOWER_ARM,
	NULL,										// ELEM_RIGHT_HAND,
	VistaOpenNIDriver::SKELETON_RIGHT_KNEE,		// ELEM_RIGHT_UPPER_LEG,
	VistaOpenNIDriver::SKELETON_RIGHT_FOOT,		// ELEM_RIGHT_LOWER_LEG,
	NULL,										// ELEM_RIGHT_FOOT,
	VistaOpenNIDriver::SKELETON_LEFT_ELBOW,		// ELEM_LEFT_UPPER_ARM,
	VistaOpenNIDriver::SKELETON_LEFT_HAND,		// ELEM_LEFT_LOWER_ARM,
	NULL,										// ELEM_LEFT_HAND,
	VistaOpenNIDriver::SKELETON_LEFT_KNEE,		// ELEM_LEFT_UPPER_LEG,
	VistaOpenNIDriver::SKELETON_LEFT_FOOT,		// ELEM_LEFT_LOWER_LEG,
	NULL										// ELEM_LEFT_FOOT,
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
KinectHandler::KinectHandler( VistaSystem* pVistaSystem )
: IVdfnActionObject()
, m_pSceneGraph( pVistaSystem->GetGraphicsManager()->GetSceneGraph() )
{
	VistaGeometryFactory oGeomFac( m_pSceneGraph );

	m_pManRoot = m_pSceneGraph->NewTransformNode( m_pSceneGraph->GetRoot() );
	m_pManRoot->Rotate( 0, 1, 0, 0 );
	m_pManRoot->Translate( 0, 0, -1.5f );

	m_vecJoints.resize( ELEM_COUNT );
	VistaGeometry* pGeom = NULL;
	VistaVector3D v3Offset;	

	VistaColor oFlesh( 1.0f, 0.8f, 0.6f );
	VistaColor oShirt( 1.0f, 0.2f, 0.2f );
	VistaColor oTrousers( 0.2f, 0.2f, 0.9f );
	
	pGeom = oGeomFac.CreateEllipsoid( 0.1f, 0.13f, 0.1f, 32, 32, oFlesh ); 
	v3Offset = VistaVector3D( 0.0f, -0.125f, 0.0f );
	CreateElement( ELEM_HEAD, pGeom, v3Offset );

	pGeom = oGeomFac.CreateEllipsoid( 0.2f, 0.28f, 0.15f, 32, 32, oShirt ); 
	v3Offset = VistaVector3D( 0.0f, -0.28f, 0.0f );
	CreateElement( ELEM_TORSO, pGeom, v3Offset );	

	pGeom = oGeomFac.CreateEllipsoid( 0.05f, 0.15f, 0.05f, 32, 32, oShirt ); 
	v3Offset = VistaVector3D( 0.0f, -0.135f, 0.0f );
	CreateElement( ELEM_RIGHT_UPPER_ARM, pGeom, v3Offset );	
	CreateElement( ELEM_RIGHT_LOWER_ARM, pGeom, v3Offset );	
	CreateElement( ELEM_LEFT_UPPER_ARM, pGeom, v3Offset );	
	CreateElement( ELEM_LEFT_LOWER_ARM, pGeom, v3Offset );	

	pGeom = oGeomFac.CreateEllipsoid( 0.05f, 0.1f, 0.025f, 32, 32, oFlesh ); 
	v3Offset = VistaVector3D( 0.0f, -0.135f, 0.0f );
	CreateElement( ELEM_RIGHT_HAND, pGeom, v3Offset );	
	CreateElement( ELEM_LEFT_HAND, pGeom, v3Offset );	

	pGeom = oGeomFac.CreateEllipsoid( 0.1f, 0.2f, 0.1f, 32, 32, oTrousers ); 
	v3Offset = VistaVector3D( 0.0f, -0.19f, 0.0f );
	CreateElement( ELEM_RIGHT_UPPER_LEG, pGeom, v3Offset );	
	CreateElement( ELEM_LEFT_UPPER_LEG, pGeom, v3Offset );	

	pGeom = oGeomFac.CreateEllipsoid( 0.065f, 0.2f, 0.065f, 32, 32, oTrousers ); 
	v3Offset = VistaVector3D( 0.0f, -0.19f, 0.0f );
	CreateElement( ELEM_RIGHT_LOWER_LEG, pGeom, v3Offset );	
	CreateElement( ELEM_LEFT_LOWER_LEG, pGeom, v3Offset );	

	pGeom = oGeomFac.CreateEllipsoid( 0.08f, 0.06f, 0.15f, 32, 32, oTrousers ); 
	v3Offset = VistaVector3D( 0.0f, -0.04f, -0.06f );
	CreateElement( ELEM_RIGHT_FOOT, pGeom, v3Offset );	
	CreateElement( ELEM_LEFT_FOOT, pGeom, v3Offset );	

}

KinectHandler::~KinectHandler()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool KinectHandler::SetPosition( const std::vector<VistaVector3D>& vecPos )
{
	VistaVector3D v3Position;
	VistaQuaternion qOrientation;
	for( int i = 0; i < ELEM_COUNT; ++i )
	{
		v3Position = vecPos[S_afElementFromJoint[i]];
		v3Position = m_matTransform * v3Position;
		if( S_afElementToJoint[i] != NULL )
		{
			VistaVector3D v3To = vecPos[S_afElementToJoint[i]];
			v3To = m_matTransform * v3To;
			v3To = v3Position - v3To;
			qOrientation = VistaQuaternion( VistaVector3D( 0, 1, 0 ), v3To );
		}
		m_vecJoints[i]->SetTranslation( v3Position );
		m_vecJoints[i]->SetRotation( qOrientation );
	}
		
	return true;
}


bool KinectHandler::SetTransformation( const VistaTransformMatrix& matTransform )
{
	m_matTransform = matTransform;
	return true;
}

void KinectHandler::CreateElement( int nElement, VistaGeometry* pGeom, const VistaVector3D& v3Offset )
{
	m_vecJoints[nElement] = m_pSceneGraph->NewTransformNode( m_pManRoot );
	VistaTransformNode* pOffsetNode = m_pSceneGraph->NewTransformNode( m_vecJoints[nElement] );
	pOffsetNode->SetTranslation( v3Offset );
	m_pSceneGraph->NewGeomNode( pOffsetNode, pGeom );
}

void KinectHandler::SetIsEnabled( const bool bSet )
{
	m_pManRoot->SetIsEnabled( bSet );
}

bool KinectHandler::GetIsEnabled()
{
	return m_pManRoot->GetIsEnabled();
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE "ViSkISpatialDFN.cpp"                                          */
/*============================================================================*/

