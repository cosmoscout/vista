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


#include "VistaKernelDfnNodeCreators.h"

#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnTimerNode.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaDataFlowNet/VdfnDumpHistoryNode.h>

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaProjection.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/Stuff/ProximityWarning/VistaProximityFadeout.h>
#include <VistaKernel/Stuff/ProximityWarning/VistaProximitySign.h>
#include <VistaKernel/Stuff/ProximityWarning/VistaProximityBarrierTape.h>
#include <VistaKernel/InteractionManager/VistaUserPlatform.h>

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnEventSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnWindowSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnTrackballNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnViewerSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnViewerSinkNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnViewportSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnProjectionSourceNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnSensorFrameNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnSystemTriggerControlNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnNavigationNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfn3DMouseTransformNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnMouseWheelChangeDetectNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnReferenceFrameTransformNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnClusterNodeInfoNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnDeviceDebugNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnCropViewportNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnKeyCallbackNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnKeyStateNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnProximityWarningNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnFadeoutNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnSimpleTextNode.h>
#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnGeometryNode.h>

#include <list>
#include <iomanip>
#include <sstream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaKernelDfnNodeCreators::RegisterNodeCreates( VistaSystem* pVistaSystem )
{
	VdfnNodeFactory* pFac = VdfnNodeFactory::GetSingleton();

	pFac->SetNodeCreator( "ViewerSource", 
					new VistaDfnViewerSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "ViewerSink", 
					new VistaDfnViewerSinkNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "WindowSource", 
					new VistaDfnWindowSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "ViewportSource",
					new VistaDfnViewportSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "ProjectionSource", 
					new VistaDfnProjectionSourceNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "3DMouseTransform",
					new Vista3DMouseTransformNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "MousewheelChangeDetect",
					new VistaDfnMouseWheelChangeDetectNodeCreate );
	pFac->SetNodeCreator( "ReferenceFrameTransform",
					new VistaDfnReferenceFrameTransformNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "SensorFrame",
					new TVdfnDefaultNodeCreate<VistaDfnSensorFrameNode> );
	pFac->SetNodeCreator( "DeviceDebug",
					new VistaDfnDeviceDebugNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "SystemTriggerControl",
					new VistaDfnSystemTriggerControlNodeCreate( pVistaSystem->GetKeyboardSystemControl() ) );
	pFac->SetNodeCreator( "Navigation", 
					new VistaDfnNavigationNodeCreate() );
	pFac->SetNodeCreator( "ClusterNodeInfo", 
					new VdfnClusterNodeInfoNodeCreate( pVistaSystem->GetClusterMode() ) );
	pFac->SetNodeCreator( "Frameclock",
					new VistaDfnFrameclockNodeCreate( pVistaSystem->GetClusterMode() ) );

	// this is an override, the old pointer is lost, so expect a memory lead
	// in a profile. The policy of overriding is currently not defined. @todo
	pFac->SetNodeCreator( "DumpHistory",
					new VdfnDumpHistoryNodeClusterCreate( pVistaSystem->GetClusterMode() ) );

	pFac->SetNodeCreator( "Trackball",
					new VistaDfnTrackballNodeCreate() );
	pFac->SetNodeCreator( "EventSource",
					new VistaDfnEventSourceNodeCreate(pVistaSystem-> GetEventManager(), pVistaSystem->GetInteractionManager() ) );
	pFac->SetNodeCreator( "TextOverlay[int]",
					new VistaDfnTextOverlayNodeCreate<int>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[unsigned int]",
					new VistaDfnTextOverlayNodeCreate<unsigned int>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[int64]",
					new VistaDfnTextOverlayNodeCreate<VistaType::sint64>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[bool]",
					new VistaDfnTextOverlayNodeCreate<bool>( pVistaSystem->GetDisplayManager()  ) );
	pFac->SetNodeCreator( "TextOverlay[double]", new VistaDfnTextOverlayNodeCreate<double>( pVistaSystem->GetDisplayManager()  ) );
	pFac->SetNodeCreator( "TextOverlay[microtime]",
					new VistaDfnTextOverlayNodeCreate<VistaType::microtime>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[float]",
					new VistaDfnTextOverlayNodeCreate<float>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[string]",
		new VistaDfnTextOverlayNodeCreate<std::string>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[VistaVector3D]",
					new VistaDfnTextOverlayNodeCreate<VistaVector3D>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[VistaQuaternion]",
					new VistaDfnTextOverlayNodeCreate<VistaQuaternion>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "TextOverlay[VistaTransformMatrix]",
					new VistaDfnTextOverlayNodeCreate<VistaTransformMatrix>( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "SimpleText",
					new VistaDfnSimpleTextNodeCreate( pVistaSystem->GetDisplayManager() ) );

	pFac->SetNodeCreator( "CropViewport",
					new VistaDfnCropViewportNodeCreate( pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "KeyCallback",
					new VistaDfnKeyStateNodeCreate( pVistaSystem->GetKeyboardSystemControl() ) );
	pFac->SetNodeCreator( "KeyState",
		new VistaDfnKeyStateNodeCreate( pVistaSystem->GetKeyboardSystemControl() ) );	
	pFac->SetNodeCreator( "ProximityWarning",
					new VistaDfnProximityWarningNodeCreate( pVistaSystem ) );
	pFac->SetNodeCreator( "Fadeout",
					new VistaDfnFadeoutNodeCreate( pVistaSystem->GetClusterMode(),
																pVistaSystem->GetDisplayManager() ) );
	pFac->SetNodeCreator( "Geometry", new VistaDfnGeometryNodeCreate( pVistaSystem->GetGraphicsManager()->GetSceneGraph() ) );
	pFac->SetNodeCreator( "PointingRay", new VistaDfnPointingRayGeometryNodeCreate( pVistaSystem->GetGraphicsManager()->GetSceneGraph() ) );


	return true;
}


// #############################################################################

Vista3DMouseTransformNodeCreate::Vista3DMouseTransformNodeCreate(VistaDisplayManager *pSys)
	: m_pMgr(pSys)
{
}

IVdfnNode *Vista3DMouseTransformNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{

		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strDisplaySystem;
		std::string strViewport;
		if( subs.GetValue( "displaysystem", strDisplaySystem )
			&& subs.GetValue( "viewport", strViewport ) )
		{
			VistaDisplaySystem *pSys = m_pMgr->GetDisplaySystemByName(strDisplaySystem);
			VistaViewport *pViewport = m_pMgr->GetViewportByName( strViewport );
			if(pViewport && pSys)
			{
				float fOriginOffset = 0;
				subs.GetValue( "origin_offset_along_ray", fOriginOffset );
				VistaVdfn3DMouseTransformNode *pNd = new VistaVdfn3DMouseTransformNode(
																	pSys, pViewport, fOriginOffset );
				bool bToFrame = subs.GetValueOrDefault<bool>( "in_world_coordinates", false );
				pNd->SetTransformPositionFromFrame(bToFrame);
				return pNd;
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}


// #############################################################################

VdfnClusterNodeInfoNodeCreate::VdfnClusterNodeInfoNodeCreate(VistaClusterMode *pClusterMode)
	: m_pClusterMode(pClusterMode)
{
}

IVdfnNode *VdfnClusterNodeInfoNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		return new VdfnClusterNodeInfoNode(m_pClusterMode);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################


VistaDfnDeviceDebugNodeCreate::VistaDfnDeviceDebugNodeCreate( VistaDisplayManager* pDisplayManager )
: m_pDisplayManager( pDisplayManager )
{
}

IVdfnNode *VistaDfnDeviceDebugNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strTagName;
		if( subs.GetValue( "drivername", strTagName ) == false || strTagName.empty() )
			strTagName = "<none>";

		std::string sViewportName = subs.GetValueOrDefault<std::string>( "viewport", "" );

		std::list<std::string> strPorts;
		subs.GetValue( "showlist", strPorts );

		VistaDfnDeviceDebugNode *pRet = new VistaDfnDeviceDebugNode( m_pDisplayManager, strTagName, strPorts, sViewportName);

		if( subs.HasProperty( "red" )
			|| subs.HasProperty( "green" )
			|| subs.HasProperty( "blue" ) )
		{
			float nRed = subs.GetValueOrDefault<float>( "red", 1.0f );
			float nGreen = subs.GetValueOrDefault<float>( "green", 1.0f );
			float nBlue = subs.GetValueOrDefault<float>( "blue", 0.0f );

			pRet->SetColor( nRed, nGreen, nBlue );
		}

		bool bShowType = subs.GetValueOrDefault<bool>( "showtype", false );
		pRet->SetShowType(bShowType);

		return pRet;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################

VdfnDumpHistoryNodeClusterCreate::VdfnDumpHistoryNodeClusterCreate( VistaClusterMode *pClusterMode )
	: VdfnNodeFactory::IVdfnNodeCreator()
	, m_pClusterMode(pClusterMode)
{

}

IVdfnNode *VdfnDumpHistoryNodeClusterCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strFileName;
		subs.GetValue( "file", strFileName );
		std::string strHostName = m_pClusterMode->GetNodeName();
		std::string strFinalName = strHostName.empty() ? strFileName
								   : strHostName+"."+strFileName;

		return new VdfnDumpHistoryNode(strFinalName);
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

// #############################################################################

VistaDfnEventSourceNodeCreate::VistaDfnEventSourceNodeCreate( VistaEventManager *pEvMgr,
								VistaInteractionManager *pInMa )
								: VdfnNodeFactory::IVdfnNodeCreator(),
								m_pEvMgr(pEvMgr),
								m_pInMa(pInMa)
{
}


IVdfnNode* VistaDfnEventSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VistaDfnEventSourceNode( m_pEvMgr, m_pInMa, oParams.GetValueOrDefault<std::string>( "tag", "" ) );
}

// #############################################################################

IVdfnNode *VistaDfnNavigationNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList &oPars = oParams.GetSubListConstRef( "param" );

	int iDefaultMode = 0;
	if( oPars.GetValue<int>( "default_navigation_mode", iDefaultMode ) == false )
	{
		std::string sDefaultMode;
		if( oPars.GetValue<std::string>( "default_navigation_mode", sDefaultMode ) )
		{
			if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sDefaultMode, "unconstrained" ) )
				iDefaultMode = 0;
			else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sDefaultMode, "yaw_only" ) )
				iDefaultMode = 1;
			else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sDefaultMode, "no_roll" ) )
				iDefaultMode = 2;
			else
			{
				vstr::warnp() << "[VistaDfnNavigationNodeCreate]: unknown value ["
							<< sDefaultMode << "] for param \"default_navigation_mode\"" << std::endl;
			}
		}
	}

	float fDefaultLinearVelocity = oPars.GetValueOrDefault<float>( "default_linear_velocity", 1.0f );
	float fDefaultAngularVelocity = oPars.GetValueOrDefault<float>( "default_angular_velocity", Vista::Pi );
	float fLinearAcceleration = oPars.GetValueOrDefault<float>( "linear_acceleration", 0.0f );
	float fLinearDeceleration = oPars.GetValueOrDefault<float>( "linear_deceleration", 0.0f );
	float fAngularAcceleration = oPars.GetValueOrDefault<float>( "angular_acceleration", 0.0f );
	float fAngularDeceleration = oPars.GetValueOrDefault<float>( "angular_deceleration", 0.0f );

	return new VistaDfnNavigationNode( iDefaultMode,
										fDefaultLinearVelocity,
										fDefaultAngularVelocity,
										fLinearAcceleration,
										fLinearDeceleration,
										fAngularAcceleration,
										fAngularDeceleration );
}


// #############################################################################

VistaDfnWindowSourceNodeCreate::VistaDfnWindowSourceNodeCreate( VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnWindowSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strWindowName;
		if( subs.GetValue( "value", strWindowName ) )
		{			
			VistaWindow *pWindow = m_pMgr->GetWindowByName(strWindowName);
			if(pWindow)
			{
				return new VistaDfnWindowSourceNode( pWindow );
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VistaDfnViewportSourceNodeCreate::VistaDfnViewportSourceNodeCreate( VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnViewportSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strWindowName;
		if( subs.GetValue( "value", strWindowName ) )
		{			
			VistaViewport *pVp = m_pMgr->GetViewportByName(strWindowName);
			if(pVp)
			{
				return new VistaDfnViewportSourceNode( pVp );
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VistaDfnViewerSinkNodeCreate::VistaDfnViewerSinkNodeCreate(VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnViewerSinkNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

		VistaDisplaySystem* pSystem = NULL;
		std::string sDisplaySystemName;
		if( oSubs.GetValue( "displaysystem", sDisplaySystemName ) )
		{
			pSystem = m_pMgr->GetDisplaySystemByName( sDisplaySystemName );
			if( pSystem == NULL )
			{
				vstr::warnp() << "[VistaDfnViewerSinkNodeCreate]:requested display system ["
						<< sDisplaySystemName << "] does not exist!" << std::endl;
				return NULL;
			}
		}
		else if( m_pMgr->GetNumberOfDisplaySystems() == 0 )
		{
			vstr::warnp() << "[VistaDfnViewerSinkNodeCreate]: No display systems available!" << std::endl;
			return NULL;
		}
		else if( m_pMgr->GetNumberOfDisplaySystems() == 1 )
		{
			pSystem = m_pMgr->GetDisplaySystem( 0 );
		}
		else
		{
			vstr::warnp() << "[VistaDfnViewerSinkNodeCreate]: More than one display system available "
						" - please specify a \"displaysystem\" parameter!" << std::endl;
		}

		return new VistaDfnViewerSinkNode( pSystem );
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

VistaDfnTrackballNodeCreate::VistaDfnTrackballNodeCreate()
{}

IVdfnNode *VistaDfnTrackballNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VistaVdfnTrackball();
}

// #############################################################################

VistaDfnSystemTriggerControlNodeCreate::VistaDfnSystemTriggerControlNodeCreate(VistaKeyboardSystemControl *pCtrl )
								: VdfnNodeFactory::IVdfnNodeCreator(), m_pCtrl(pCtrl)

{
}


IVdfnNode *VistaDfnSystemTriggerControlNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VistaDfnSystemTriggerControlNode( m_pCtrl );
}

// #############################################################################

VistaDfnReferenceFrameTransformNodeCreate::VistaDfnReferenceFrameTransformNodeCreate(
						VistaDisplayManager* pDisplayManager )
:	m_pDisplayManager( pDisplayManager )
{
}

IVdfnNode* VistaDfnReferenceFrameTransformNodeCreate::CreateNode( 
												const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	VistaDisplaySystem* pDisplaySystem;
	std::string sDisplaySystemName;
	if( oSubs.GetValue( "displaysystem", sDisplaySystemName ) )
	{		
		pDisplaySystem = m_pDisplayManager->GetDisplaySystemByName( sDisplaySystemName );		
	}
	else
	{
		pDisplaySystem = m_pDisplayManager->GetDisplaySystem();
	}
	if( pDisplaySystem == NULL )
			return NULL;

	bool bTransformToFrame = false;
	std::string sTransformMode;
	if( oSubs.GetValue( "transform_mode", sTransformMode ) )
	{
		if( sTransformMode == "to_frame" )
			bTransformToFrame = true;
		else if( sTransformMode == "from_frame" )
			bTransformToFrame = false;
		else
		{
			vstr::warnp() << "VistaDfnReferenceFrameTransformNodeCreate -- "
				<< " Parameter [transform_mode] has unknown value [" << sTransformMode
				<< "] - valid options are to_frame and from_frame" << std::endl;
			return NULL; 
		}
	}

	int iNrPositionPorts = oSubs.GetValueOrDefault<int>( "num_position_ports", 1 );
	int iNrOrientationPorts = oSubs.GetValueOrDefault<int>( "num_orientation_ports", 1 );
	int iNrMatrixPorts = oSubs.GetValueOrDefault<int>( "num_matrix_ports", 0 );

	return ( new VistaDfnReferenceFrameTransformNode(
									pDisplaySystem->GetReferenceFrame(),
									bTransformToFrame,
									iNrPositionPorts,
									iNrOrientationPorts,
									iNrMatrixPorts ) );
}


// #############################################################################

VistaDfnProjectionSourceNodeCreate::VistaDfnProjectionSourceNodeCreate( VistaDisplayManager *pMgr )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMgr(pMgr)
{
}

IVdfnNode *VistaDfnProjectionSourceNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		std::string strWindowName;
		if( subs.GetValue( "value", strWindowName ) )
		{			
			VistaProjection *pVp = m_pMgr->GetProjectionByName(strWindowName);
			if(pVp)
			{
				return new VistaDfnProjectionSourceNode( pVp );
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}

	return NULL;
}

// #############################################################################

class VistaDfnFrameclockNodeCreate::TimerNodeFrameclockGet : public VdfnTimerNode::CGetTime
{
public:
	TimerNodeFrameclockGet( VistaClusterMode* pClusterAux )
	: m_pClusterAux( pClusterAux )
	{
	}
	virtual double GetTime()
	{
		return m_pClusterAux->GetFrameClock();
	}
private:
	VistaClusterMode* m_pClusterAux;
};

VistaDfnFrameclockNodeCreate::VistaDfnFrameclockNodeCreate( VistaClusterMode* pClusterAux ) 
: VdfnNodeFactory::IVdfnNodeCreator()
, m_pClusterAux( pClusterAux )
{
}

IVdfnNode* VistaDfnFrameclockNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	return new VdfnTimerNode( new TimerNodeFrameclockGet( m_pClusterAux ), false );
}

// #############################################################################


VistaDfnCropViewportNodeCreate::VistaDfnCropViewportNodeCreate( VistaDisplayManager* pDisplayManager )
: m_pDisplayManager( pDisplayManager )
{
}

IVdfnNode* VistaDfnCropViewportNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	std::vector<std::string> vecViewportNames;
	std::vector<VistaViewport*> vecViewports;
	if( oParams.GetValue( "viewports", vecViewportNames ) )
	{
		for( std::vector<std::string>::const_iterator itViewportName = vecViewportNames.begin();
				itViewportName != vecViewportNames.end(); ++itViewportName )
		{
			VistaViewport* pViewport = m_pDisplayManager->GetViewportByName( (*itViewportName) );
			if( pViewport )
			{
				vecViewports.push_back( pViewport );
			}
			else
			{
				vstr::warnp() << "[VistaCropViewportNodeCreate]: Viewport of name ["
							<< (*itViewportName) << "] does not exist" << std::endl;
			}
		}
	}
	else
	{
		for( std::map<std::string, VistaViewport*>::const_iterator itViewport = m_pDisplayManager->GetViewportsConstRef().begin();
				itViewport != m_pDisplayManager->GetViewportsConstRef().end(); ++itViewport )
		{
			vecViewports.push_back( (*itViewport).second );
		}
	}

	bool bUseProjExtents = oSubs.GetValueOrDefault<bool>( "use_projection_extents", false );
	float nLeft = oSubs.GetValueOrDefault<float>( "crop_left", 0.0f );
	float nRight = oSubs.GetValueOrDefault<float>( "crop_right", 0.0f );
	float nBottom = oSubs.GetValueOrDefault<float>( "crop_bottom", 0.0f );
	float nTop = oSubs.GetValueOrDefault<float>( "crop_top", 0.0f );

	VistaColor oCropColor = oSubs.GetValueOrDefault<VistaColor>( "color", VistaColor::BLACK );

	VistaDfnCropViewportNode* pNode = new VistaDfnCropViewportNode( nLeft, nRight, nBottom, nTop, bUseProjExtents, oCropColor );

	VistaVector3D v3Normal;
	if( oSubs.GetValue( "filter_projection_normal", v3Normal ) )
	{
		v3Normal.Normalize();
		for( std::vector<VistaViewport*>::const_iterator itViewport = vecViewports.begin();
				itViewport != vecViewports.end(); ++itViewport )
		{
			VistaVector3D v3ProjectionNormal;
			(*itViewport)->GetProjection()->GetProjectionProperties()->GetProjPlaneNormal(
						v3ProjectionNormal[0], v3ProjectionNormal[1], v3ProjectionNormal[2] );
			float nDot = v3ProjectionNormal.Dot( v3Normal );
			if( 1.0f - nDot < Vista::Epsilon )
			{
				pNode->AddViewport( (*itViewport) );
			}
		}
	}
	else
	{
		for( std::vector<VistaViewport*>::const_iterator itViewport = vecViewports.begin();
				itViewport != vecViewports.end(); ++itViewport )
		{
			pNode->AddViewport( (*itViewport) );
		}
	}

	return pNode;
}


// #############################################################################

VistaDfnKeyCallbackNodeCreate::VistaDfnKeyCallbackNodeCreate( VistaKeyboardSystemControl* pKeyboard )
: m_pKeyboard( pKeyboard )
{
}

IVdfnNode* VistaDfnKeyCallbackNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	
	int nKeyCode;
	int nModCode = VISTA_KEYMOD_ANY;
	if( oSubs.GetValue<int>( "key_value", nKeyCode ) == false )
	{
		std::string sKey;
		if( oSubs.GetValue<std::string>( "key", sKey ) )
		{
			if( VistaKeyboardSystemControl::GetKeyAndModifiersValueFromString( sKey, nKeyCode, nModCode ) == false )
			{
				vstr::warnp() << "[DfnKeyCallbackNodeCreate]: invalid \"key\"-entry ["
								<< sKey << "]" << std::endl;
				return NULL;
			}
		}
		else
		{
			vstr::warnp() << "[DfnKeyCallbackNodeCreate]: neither \"key\" nor \"key_value\" specified" << std::endl;
			return NULL;
		}
	}
	if( oSubs.GetValue<int>( "mod_value", nModCode ) == false )
	{
		std::string sMod;
		if( oSubs.GetValue<std::string>( "mod", sMod ) )
		{
			nModCode = VistaKeyboardSystemControl::GetModifiersValueFromString( sMod );
			if( nModCode == -1 )
			{
				vstr::warnp() << "[DfnKeyCallbackNodeCreate]: invalid \"mod\"-entry ["
								<< sMod << "]" << std::endl;
				return NULL;
			}
		}
	}
	std::string sHelpText = oSubs.GetValueOrDefault<std::string>( "description", "<none>" );
	bool bForce = oSubs.GetValueOrDefault<bool>( "force", false );

	VistaDfnKeyCallbackNode* pNode = new VistaDfnKeyCallbackNode;
	pNode->SetupKeyboardCallback(m_pKeyboard, nKeyCode, nModCode, sHelpText, bForce );
	return pNode;
}

// #############################################################################

VistaDfnKeyStateNodeCreate::VistaDfnKeyStateNodeCreate( VistaKeyboardSystemControl* pKeyboard )
	: m_pKeyboard( pKeyboard )
{
}

IVdfnNode* VistaDfnKeyStateNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );


	int nKeyCode;
	int nModCode = VISTA_KEYMOD_ANY;
	if( oSubs.GetValue<int>( "key_value", nKeyCode ) == false )
	{
		std::string sKey;
		if( oSubs.GetValue<std::string>( "key", sKey ) )
		{
			if( VistaKeyboardSystemControl::GetKeyAndModifiersValueFromString( sKey, nKeyCode, nModCode ) == false )
			{
				vstr::warnp() << "[DfnKeyCallbackNodeCreate]: invalid \"key\"-entry ["
					<< sKey << "]" << std::endl;
				return NULL;
			}
		}
		else
		{
			vstr::warnp() << "[DfnKeyCallbackNodeCreate]: neither \"key\" nor \"key_value\" specified" << std::endl;
			return NULL;
		}
	}
	if( oSubs.GetValue<int>( "mod_value", nModCode ) == false )
	{
		std::string sMod;
		if( oSubs.GetValue<std::string>( "mod", sMod ) )
		{
			nModCode = VistaKeyboardSystemControl::GetModifiersValueFromString( sMod );
			if( nModCode == -1 )
			{
				vstr::warnp() << "[DfnKeyCallbackNodeCreate]: invalid \"mod\"-entry ["
					<< sMod << "]" << std::endl;
				return NULL;
			}
		}
	}
	std::string sHelpText = oSubs.GetValueOrDefault<std::string>( "description", "<none>" );
	bool bForce = oSubs.GetValueOrDefault<bool>( "force", false );

	VistaDfnKeyStateNode* pNode = new VistaDfnKeyStateNode();
	pNode->SetupKeyboardCallback(m_pKeyboard, nKeyCode, nModCode, sHelpText, bForce );
	return pNode;
}

// #############################################################################


VistaDfnProximityWarningNodeCreate::VistaDfnProximityWarningNodeCreate( VistaSystem* pVistaSystem )
: m_pVistaSystem( pVistaSystem )
{
}

IVdfnNode* VistaDfnProximityWarningNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	bool bUseBox = false;
	VistaBoundingBox oBox;
	VistaQuaternion qBoxRotation;

	if( oSubs.GetValue<VistaVector3D>( "box_min", oBox.m_v3Min )
		&& oSubs.GetValue<VistaVector3D>( "box_max", oBox.m_v3Max ) )
	{
		qBoxRotation = oSubs.GetValueOrDefault<VistaQuaternion>( "box_orientation" );
		bUseBox = true;
	}
	std::vector<float> vecPlanes;
	oSubs.GetValue( "halfplanes", vecPlanes );

	if( vecPlanes.size() % 6 != 0 )
	{
		vstr::warnp() << "[VistaDfnProximityWarningNodeCreate]: entry \"halfplanes\""
						" must have exactly 3 values for center and 3 values for normal"
						" per half plane" << std::endl;
		return NULL;
	}

	if( bUseBox == false && vecPlanes.empty() )
	{
		vstr::warnp() << "[VistaDfnProximityWarningNodeCreate]: Neither \"halfplanes\""
						" nor \"box_min\"+\"box_max\" specified" << std::endl;
		return NULL;
	}

	float nSafeDistance = 0;
	float nDangerDistance = 0;
	if( oSubs.GetValue( "safe_distance", nSafeDistance ) == false
		|| oSubs.GetValue( "danger_distance", nDangerDistance ) == false )
	{
		vstr::warnp() << "[VistaDfnProximityWarningNodeCreate]: \"safe_distance\""
						" or \"danger_distance\" entry missing" << std::endl;
		return NULL;
	}	

	std::string sType = oSubs.GetValueOrDefault<std::string>( "type", "" );

	IVistaProximityWarningBase* pWarn;

	if( sType.empty()
		|| VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sType, "fadeout" ) )
	{
		VistaColor oColor = oSubs.GetValueOrDefault<VistaColor>( "color", VistaColor::BLACK );

		VistaProximityFadeout* pFadeout = new VistaProximityFadeout( m_pVistaSystem->GetEventManager(),
																		nSafeDistance, nDangerDistance );
		pFadeout->SetFadeoutColor( oColor );
		std::vector<std::string> vecViewports;
		if( oSubs.GetValue( "viewports", vecViewports ) )
		{
			for( std::vector<std::string>::const_iterator itViewport = vecViewports.begin();
					itViewport != vecViewports.end(); ++itViewport )
			{
				VistaViewport* pViewport = m_pVistaSystem->GetDisplayManager()->GetViewportByName( (*itViewport) );
				if( pViewport == NULL )
				{
					vstr::warnp() << "[VistaDfnProximityFadeoutNodeCreate]: requested viewport ["
						<< (*itViewport) << "] does not exist" << std::endl;
				}
				else
					pFadeout->AddViewport( pViewport );
			}
		}
		else
		{
			pFadeout->AddAllViewports( m_pVistaSystem->GetDisplayManager() );
		}
		pWarn = pFadeout;
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sType, "sign" )  )
	{
		bool bDisableOcclusion = oSubs.GetValueOrDefault<bool>( "disable_occlusion", true );
		VistaProximitySign* pSign = new VistaProximitySign( m_pVistaSystem->GetEventManager(),
																nSafeDistance, nDangerDistance,
																bDisableOcclusion,
																m_pVistaSystem->GetGraphicsManager() );
		std::string sTexture;
		if( oSubs.GetValue<std::string>( "texture", sTexture ) )
		{
			if( pSign->SetTexture( sTexture ) == false )
			{
				vstr::warnp() << "[VistaDfnProximityWarningNodeCreate]: specified \"texture\" "
						<< " for warning type \"sign\" could not be loaded - using default texture" << std::endl;
				pSign->SetDefaultTexture();
			}			
		}
		else
			pSign->SetDefaultTexture();
		float nSize[2];
		if( oSubs.GetValueAsArray<2, float>( "size", nSize ) == false )
		{
			if( oSubs.GetValue<float>( "size", nSize[0] ) )
			{
				nSize[1] = nSize[0];
			}
			else
			{
				nSize[0] = 1.0f;
				nSize[1] = 1.0f;
			}
		}
		
		pSign->SetScale( nSize[0], nSize[1] );

		if( m_pVistaSystem->GetDisplayManager()->GetNumberOfDisplaySystems() > 0 )
		{
			VistaUserPlatform* pPlatform = m_pVistaSystem->GetPlatformFor(
								m_pVistaSystem->GetDisplayManager()->GetDisplaySystem(0) );
			pSign->SetParentNode( pPlatform->GetPlatformNode() );
		}

		pWarn = pSign;
	}
	else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sType, "tape" )  )
	{
		/*if( bUseBox == false || vecPlanes.empty() == false )
		{
			vstr::warnp() << "[VistaDfnProximityWarningNodeCreate]: type \"tape\" "
						<< " only supports box mode, but no halfplanes" << std::endl;
			return NULL;
		}*/
		bool bDisableOcclusion = oSubs.GetValueOrDefault<bool>( "disable_occlusion", true );
		VistaProximityBarrierTape* pTape = new VistaProximityBarrierTape( m_pVistaSystem->GetEventManager(),
																		nSafeDistance, nDangerDistance,
																		bDisableOcclusion,
																		m_pVistaSystem->GetGraphicsManager() );
		pTape->SetTapeHeight( oSubs.GetValueOrDefault( "tape_height", 1.2f ) );
		pTape->SetTapeWidth( oSubs.GetValueOrDefault( "tape_width", 0.15f ) );
		pTape->SetTapeSpacing( oSubs.GetValueOrDefault( "tape_spacing", 0.0f ) );
		pTape->SetDistanceFromWall( oSubs.GetValueOrDefault( "distance_from_wall", 0.0f ) );
		VistaProximityBarrierTape::HeightMode eMode = VistaProximityBarrierTape::HM_FIXED_HEIGHT;
		std::string sHeightModeString;
		if( oSubs.GetValue( "height_mode", sHeightModeString ) )
		{
			VistaAspectsComparisonStuff::StringCompareObject oCompare( false );
			if( oCompare( sHeightModeString, "fixed" ) )
				eMode = VistaProximityBarrierTape::HM_FIXED_HEIGHT;
			else if( oCompare( sHeightModeString, "adjust_to_user" ) )
				eMode = VistaProximityBarrierTape::HM_ADJUST_TO_VIEWER_POS;
			else if( oCompare( sHeightModeString, "adjust_to_user_start" ) )
				eMode = VistaProximityBarrierTape::HM_ADJUST_TO_VIEWER_POS_AT_START;
			else if( oCompare( sHeightModeString, "adjust_to_object" ) )
				eMode = VistaProximityBarrierTape::HM_ADJUST_TO_OBJECT_POS;
			else if( oCompare( sHeightModeString, "adjust_to_object_start" ) )
				eMode = VistaProximityBarrierTape::HM_ADJUST_TO_OBJECT_POS_AT_START;			
			else
			{
				vstr::warnp() << "[VistaDfnProximityWarningNodeCreate]: Undefined mode ["
						<< sHeightModeString << "] for parameter \"height_mode\"" << std::endl;
				eMode = VistaProximityBarrierTape::HM_FIXED_HEIGHT;
			}
			pTape->SetHeightMode( eMode );
		}

		if( m_pVistaSystem->GetDisplayManager()->GetNumberOfDisplaySystems() > 0 )
		{
			VistaUserPlatform* pPlatform = m_pVistaSystem->GetPlatformFor(
								m_pVistaSystem->GetDisplayManager()->GetDisplaySystem(0) );
			pTape->SetParentNode( pPlatform->GetPlatformNode() );
		}
		pWarn = pTape;
	}
	else
	{
		vstr::warnp() << "[VistaDfnProximityWarningNodeCreate]: \"type\"-value ["
					<< sType << "] invalid" << std::endl;
		return NULL;
	}

	if( bUseBox )
		pWarn->SetUseExtents( oBox, qBoxRotation );
	for( std::size_t i = 0; i < vecPlanes.size(); i += 6 )
	{
		pWarn->AddHalfPlane( VistaVector3D( &vecPlanes[i+0] ), VistaVector3D( &vecPlanes[i+3] ) );
	}

	// additional general parameters
	VistaType::microtime nFlashPeriod = oSubs.GetValueOrDefault<VistaType::microtime>( "flash_period", 0.5 );
	bool bFlashInDangerZone = oSubs.GetValueOrDefault<bool>( "flash_in_dangerzone", false );
	VistaType::microtime nFlashAfterLostTracking = oSubs.GetValueOrDefault<VistaType::microtime>( "flash_after_lost_tracking", 0 );
	VistaType::microtime nHideTimeout = oSubs.GetValueOrDefault<VistaType::microtime>( "hide_timeout", 0 );
	VistaType::microtime nHideFadeoutTime = oSubs.GetValueOrDefault<VistaType::microtime>( "hide_fadeout_time", 2 );

	pWarn->SetFlashInDangerZone( bFlashInDangerZone );	
	pWarn->SetFlashAfterLostTracking( nFlashAfterLostTracking );
	pWarn->SetFlashPeriod( nFlashPeriod );

	pWarn->SetTimeout( nHideTimeout, nHideFadeoutTime );
		
	VistaDfnProximityWarningNode* pNode = new VistaDfnProximityWarningNode;
	pNode->AddWarning( pWarn );

	bool bEnabled = oSubs.GetValueOrDefault( "enabled", false );
	pNode->SetIsEnabled( bEnabled );

	return pNode;
}

// #############################################################################

VistaDfnFadeoutNodeCreate::VistaDfnFadeoutNodeCreate( VistaClusterMode* pClusterMode,
																	   VistaDisplayManager* pDisplayManager )
: VdfnNodeFactory::IVdfnNodeCreator()
, m_pDisplayManager( pDisplayManager )
, m_pClusterMode( pClusterMode )
{
}

IVdfnNode* VistaDfnFadeoutNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	VistaDfnFadeoutNode* pNode = new VistaDfnFadeoutNode( m_pClusterMode );
	std::vector<std::string> vecViewports;
	if( oSubs.GetValue( "viewports", vecViewports ) )
	{
		for( std::vector<std::string>::const_iterator itViewport = vecViewports.begin();
				itViewport != vecViewports.end(); ++itViewport )
		{
			VistaViewport* pViewport = m_pDisplayManager->GetViewportByName( (*itViewport) );
			if( pViewport == NULL )
			{
				vstr::warnp() << "[VistaDfnProximityFadeoutNodeCreate]: requested viewport ["
					<< (*itViewport) << "] does not exist" << std::endl;
			}
			else
				pNode->AddToViewport( pViewport );
		}
	}
	else
	{
		pNode->AddToAllViewports( m_pDisplayManager );
	}
	pNode->SetColor( oSubs.GetValueOrDefault<VistaColor>( "color", VistaColor::GRAY ) );
	pNode->SetFadeinTime( oSubs.GetValueOrDefault<VistaType::microtime>( "fadein_time", 1.0 ) );
	pNode->SetFadeoutTime( oSubs.GetValueOrDefault<VistaType::microtime>( "fadeout_time", 1.0 ) );
		
	return pNode;
}

// #############################################################################

VistaDfnSimpleTextNodeCreate::VistaDfnSimpleTextNodeCreate( VistaDisplayManager* pDisplayManager )
: m_pDisplayManager( pDisplayManager )
{
}

IVdfnNode* VistaDfnSimpleTextNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	VistaDfnSimpleTextNode* pNode = new VistaDfnSimpleTextNode( m_pDisplayManager );
	pNode->SetTextSize( oSubs.GetValueOrDefault<int>( "textsize", 20 ) );
	pNode->SetCaptionColumnWidth( oSubs.GetValueOrDefault<int>( "caption_column_width", 0 ) );
	pNode->SetColor( oSubs.GetValueOrDefault<VistaColor>( "color", VistaColor::RED ) );

	return pNode;
}

// #############################################################################

VistaDfnGeometryNodeCreate::VistaDfnGeometryNodeCreate( VistaSceneGraph* pSceneGraph )
: m_pSceneGraph( pSceneGraph )
{
}

IVdfnNode* VistaDfnGeometryNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );
	VistaDfnGeometryNode* pNode = NULL;
	std::string sFilename;
	if( oSubs.GetValue( "modelfile", sFilename ) )
	{
		// @todo map optimizations
		IVistaNode* pModelNode = m_pSceneGraph->LoadNode( sFilename );
		if( pModelNode == NULL )
		{
			vstr::warnp() << "[VistaDfnGeometryNodeCreate]: could not load model file ["
							<< sFilename << "]" << std::endl;
			return NULL;
		}
		pNode = new VistaDfnGeometryNode( pModelNode, m_pSceneGraph );
	}
	else
	{
		// interpret it as proplist config
		pNode = new VistaDfnGeometryNode( oSubs, m_pSceneGraph );
	}

	if( pNode->GetIsValid() == false )
	{
		vstr::warnp() << "[VistaDfnGeometryNodeCreate]: configuration of geometry failed" << std::endl;
	}
	else
	{
		VistaVector3D v3OffsetTrans;
		VistaVector3D v3OffsetScale;
		float nOffsetScaleUniform;
		VistaQuaternion qOffsetRotation;
		VistaTransformMatrix matOffsetTrans;
		if( oSubs.GetValue( "offset_translation", v3OffsetTrans ) )
			pNode->GetOffsetNode()->SetTranslation( v3OffsetTrans );
		if( oSubs.GetValue( "offset_scale", v3OffsetScale ) )
			pNode->GetOffsetNode()->SetScale( v3OffsetScale );
		if( oSubs.GetValue( "offset_uniform_scale", nOffsetScaleUniform ) )
			pNode->GetOffsetNode()->SetScale( nOffsetScaleUniform, nOffsetScaleUniform, nOffsetScaleUniform );
		if( oSubs.GetValue( "offset_rotation", qOffsetRotation ) )
			pNode->GetOffsetNode()->SetRotation( qOffsetRotation );
		if( oSubs.GetValue( "offset_transform", matOffsetTrans ) )
			pNode->GetOffsetNode()->SetTransform( matOffsetTrans );
	}

	return pNode;
}

// #############################################################################

VistaDfnPointingRayGeometryNodeCreate::VistaDfnPointingRayGeometryNodeCreate( VistaSceneGraph* pSceneGraph )
: m_pSceneGraph( pSceneGraph )
{
}

IVdfnNode* VistaDfnPointingRayGeometryNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	const VistaPropertyList& oSubs = oParams.GetSubListConstRef( "param" );

	int nNumSides = oSubs.GetValueOrDefault( "sides", 24 );
	float nRadius = oSubs.GetValueOrDefault( "radius", 0.0075f );
	float nRadiusTip = oSubs.GetValueOrDefault( "tip_radius", nRadius );
	float nLength = oSubs.GetValueOrDefault( "length", 2.0f );
	VistaColor oColor = oSubs.GetValueOrDefault<VistaColor>( "color", VistaColor( 0.8f, 0.8f, 0.83f ) );

	VistaGeometryFactory oFactory( m_pSceneGraph );
	VistaGeometry* pGeom = oFactory.CreateCone( nRadius, nRadiusTip, nLength,
											nNumSides, 1, 1, oColor );

	VistaDfnGeometryNode* pNode = new VistaDfnGeometryNode( pGeom, m_pSceneGraph );

	// rotate and translate pointer to start at center and point towards -z );
	VistaTransformMatrix matTransform( VistaQuaternion( 0.70710778f, 0, 0, -0.70710778f ),
										VistaVector3D( 0, 0, -0.5f * nLength ) );
	pNode->GetOffsetNode()->SetTransform( matTransform );

	return pNode;
}

// #############################################################################
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


