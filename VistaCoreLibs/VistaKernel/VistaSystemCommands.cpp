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


#include "VistaSystemCommands.h"

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/Stuff/VistaFramerateDisplay.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>
#include <VistaKernel/Stuff/VistaEyeTester.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/InteractionManager/VistaInteractionContext.h>
#include <VistaKernel/InteractionManager/VistaUserPlatform.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>

#include <VistaDataFlowNet/VdfnPersistence.h>
#include <VistaDataFlowNet/VdfnGraph.h>

#include <VistaAspects/VistaAspectsUtils.h>

#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaFileSystemFile.h>

#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <list>
#include <iostream>
#include <algorithm>
#include "VistaInterProcComm/Cluster/VistaClusterDataCollect.h"
#include "VistaInterProcComm/Connections/VistaByteBufferSerializer.h"
#include "VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h"
#include "VistaBase/VistaBaseTypes.h"


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaQuitCommand::VistaQuitCommand(VistaSystem *pSys)
: IVistaExplicitCallbackInterface(),
m_pSys(pSys)
{
}

bool VistaQuitCommand::Do()
{
	if(m_pSys)
	{
		VistaSystemEvent oSysEvent;
		oSysEvent.SetId( VistaSystemEvent::VSE_QUIT );
		m_pSys->GetEventManager()->ProcessEvent(&oSysEvent);
		return oSysEvent.IsHandled();
	}
	return false;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleFramerateCommand::VistaToggleFramerateCommand( VistaFramerateDisplay* pFramerateDisplay )
: m_pFramerateDisplay( pFramerateDisplay )
{
}

bool VistaToggleFramerateCommand::Do()
{
	m_pFramerateDisplay->SetIsEnabled( !m_pFramerateDisplay->GetIsEnabled() );
	return true;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleCursorCommand::VistaToggleCursorCommand(VistaDisplayManager *pDisMgr)
: m_pDisMgr(pDisMgr)
{
}

bool VistaToggleCursorCommand::Do()
{
	m_pDisMgr->GetDisplayBridge()->SetShowCursor(!m_pDisMgr->GetDisplayBridge()->GetShowCursor());
	return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaShowAvailableCommands::VistaShowAvailableCommands(VistaKeyboardSystemControl *pCtrl,
														 VistaSystem *pSys)
: m_pCtrl(pCtrl),
  m_pSys(pSys)
{
}

bool VistaShowAvailableCommands::Do()
{	
	vstr::out() << m_pCtrl->GetKeyBindingTableString() << std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VistaReloadContextGraphCommand::VistaReloadContextGraphCommand(VistaSystem *pSys,
																 VistaInteractionContext *pCtx,
																 const std::string &strRoleId,
																 bool bDumpGraph, bool bWritePorts )
: IVistaExplicitCallbackInterface(),
  m_pInteractionContext(pCtx),
  m_sRoleId(strRoleId),
  m_pSys(pSys),
  m_bDumpGraph(bDumpGraph),
  m_bWritePorts(bWritePorts)
{
}

bool VistaReloadContextGraphCommand::Do()
{
	return m_pSys->GetInteractionManager()->ReloadGraphForContext( 
							m_pInteractionContext,
							m_pSys->GetClusterMode()->GetNodeName(),
							m_bDumpGraph, m_bWritePorts );

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VistaDebugContextGraphCommand::VistaDebugContextGraphCommand( VistaInteractionContext* pCtx )
: m_pInteractionContext( pCtx )
{
}

bool VistaDebugContextGraphCommand::Do()
{
	if( m_pInteractionContext->GetDumpAsDot() )
		m_pInteractionContext->DumpStateAsDot();
	else
		m_pInteractionContext->PrintDebuggingInfo();
	return true;

}

bool VistaPrintProfilerOutputCommand::Do()
{
	vstr::outi() << "\nProfilingInfo:\n";
	if( VistaBasicProfiler::GetSingleton() )
		VistaBasicProfiler::GetSingleton()->PrintProfile( vstr::out(), 5 );
	return true;
}

VistaPrintProfilerOutputForAllNodesCommand::VistaPrintProfilerOutputForAllNodesCommand( VistaClusterMode* pClusterMode )
: m_pClusterMode( pClusterMode )
, m_pCollect( NULL )
{
}

bool VistaPrintProfilerOutputForAllNodesCommand::Do()
{
	if( m_pCollect == NULL )
	{
		m_pCollect = GetVistaSystem()->GetClusterMode()->CreateDataCollect();
	}

	
	VistaByteBufferSerializer oSerializer;
	oSerializer.WriteSerializable( *VistaBasicProfiler::GetSingleton() );

	std::vector< std::vector< VistaType::byte > > vecData;
	m_pCollect->CollectData( oSerializer.GetBuffer(), oSerializer.GetBufferSize(), vecData );

	if( vecData.empty() )
		return true;

	std::vector< VistaBasicProfiler* > vecProfilers( vecData.size() );
	VistaByteBufferDeSerializer oDeSerializer;
	for( std::size_t i = 0; i < vecData.size(); ++i )
	{
		oDeSerializer.SetBuffer( &vecData[i][0], (int)vecData[i].size() );
		vecProfilers[i] = new VistaBasicProfiler();
		vecProfilers[i]->DeSerialize( oDeSerializer );
	}

	std::vector< std::string > vecNodeNames;
	int nNumNodes = m_pClusterMode->GetNumberOfNodes();
	for( int i = 0; i < nNumNodes; ++i )
	{
		VistaClusterMode::NodeInfo oInfo = m_pClusterMode->GetNodeInfo( i );
		if( oInfo.m_bIsActive )
			vecNodeNames.push_back( oInfo.m_sNodeName );
	}
	if( vecData.size() != vecNodeNames.size() )
		return false;
	
	vstr::out() << "\nClusterProfilingInfo:\n";
	VistaBasicProfiler::PrintMultipleProfiles( vstr::out(), vecNodeNames, vecProfilers );

	for( std::size_t i = 0; i < vecData.size(); ++i )
	{
		delete vecProfilers[i];
	}

	return true;
}

bool VistaResetProfilerCommand::Do()
{
	vstr::outi() << "Resetting Profiler" << std::endl;
	VistaBasicProfiler::GetSingleton()->Reset();
	return true;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VistaToggleFrustumCullingCommand::VistaToggleFrustumCullingCommand( VistaGraphicsManager *pGfxMgr ) 
: IVistaExplicitCallbackInterface()
, m_pGfxMgr(pGfxMgr)
{

}

bool VistaToggleFrustumCullingCommand::Do()
{
	m_pGfxMgr->SetFrustumCullingEnabled(!m_pGfxMgr->GetFrustumCullingEnabled());
	vstr::outi() << "[ViSys]: Frustum culling is now "
		<< (m_pGfxMgr->GetFrustumCullingEnabled() ? "ENABLED" : "DISABLED")
		<< std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleOcclusionCullingCommand::VistaToggleOcclusionCullingCommand( VistaGraphicsManager *pGfxMgr )
: IVistaExplicitCallbackInterface()
, m_pGfxMgr(pGfxMgr)
{

}

bool VistaToggleOcclusionCullingCommand::Do()
{
	m_pGfxMgr->SetOcclusionCullingEnabled(!m_pGfxMgr->GetOcclusionCullingEnabled());
	vstr::outi() << "[ViSys]: Occlusion culling is now "
		<< (m_pGfxMgr->GetOcclusionCullingEnabled() ? "ENABLED" : "DISABLED")
		<< std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleBBoxDrawingCommand::VistaToggleBBoxDrawingCommand( VistaGraphicsManager *pGfxMgr )
: IVistaExplicitCallbackInterface()
, m_pGfxMgr(pGfxMgr)
{

}

bool VistaToggleBBoxDrawingCommand::Do()
{
	m_pGfxMgr->SetBBoxDrawingEnabled(!m_pGfxMgr->GetBBoxDrawingEnabled());
	vstr::outi() << "[ViSys]: BBox drawing is now "
		<< (m_pGfxMgr->GetBBoxDrawingEnabled() ? "ENABLED" : "DISABLED")
		<< std::endl;
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaChangeEyeDistanceCommand::VistaChangeEyeDistanceCommand( const float fChangeValue,
												VistaDisplayManager* pDisplayManager )
: IVistaExplicitCallbackInterface()
, m_fChangeValue( fChangeValue )
, m_pDisplayManager( pDisplayManager )
{
}

bool VistaChangeEyeDistanceCommand::Do()
{
	for( int i = 0; i < m_pDisplayManager->GetNumberOfDisplaySystems(); ++i )
	{
		VistaDisplaySystem* pDisplaySys = m_pDisplayManager->GetDisplaySystem( i );
		VistaVector3D v3LeftEyeOffset, v3RightEyeOffset;
		pDisplaySys->GetDisplaySystemProperties()->GetEyeOffsets( v3LeftEyeOffset, v3RightEyeOffset );
		v3LeftEyeOffset[0] -= m_fChangeValue;
		v3RightEyeOffset[0] += m_fChangeValue;
		pDisplaySys->GetDisplaySystemProperties()->SetEyeOffsets( v3LeftEyeOffset, v3RightEyeOffset );
		std::streamsize iOldPrecision = vstr::out().precision( 3 );
		vstr::outi() << "Changed x-axis eye offsets of DisplaySystem ["
				<< pDisplaySys->GetNameForNameable()
				<< "] to [" << v3LeftEyeOffset[0] << ", "
				<< v3RightEyeOffset[0] << "]" << std::endl;
		vstr::out().precision( iOldPrecision );		
	}
	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaToggleVSyncCommand::VistaToggleVSyncCommand( VistaDisplayManager* pDisplayManager )
: IVistaExplicitCallbackInterface()
, m_pDisplayManager( pDisplayManager )
{
}

bool VistaToggleVSyncCommand::Do()
{
	const std::map<std::string, VistaWindow*>& m_mapWindows = m_pDisplayManager->GetWindowsConstRef();
	bool bMode = false;
	bool bModeSet = false;
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = m_mapWindows.begin();
				itWin != m_mapWindows.end(); ++itWin )
	{
		if( !bModeSet )
		{
			int iWinMode = (*itWin).second->GetWindowProperties()->GetVSyncEnabled();
			bMode = ( iWinMode != 1 );
			bModeSet = true;
		}
		if( (*itWin).second->GetWindowProperties()->SetVSyncEnabled( bMode ) )
		{
			std::cout << "VSync [" << ( bMode ? "Enabled" : "Disabled" )
						<< "] for Window [" << (*itWin).second->GetNameForNameable()
						<< "]" << std::endl;
		}
		else
		{
			vstr::warnp() << "Setting VSync FAILED for Window ["
					<< (*itWin).second->GetNameForNameable() << "]" << std::endl;
		}
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


VistaToggleFullscreenCommand::VistaToggleFullscreenCommand( VistaDisplayManager* pDisplayManager )
: IVistaExplicitCallbackInterface()
, m_pDisplayManager( pDisplayManager )
{
}

bool VistaToggleFullscreenCommand::Do()
{
	const std::map<std::string, VistaWindow*>& m_mapWindows = m_pDisplayManager->GetWindowsConstRef();
	if( m_mapWindows.empty() )
		return false;

	bool bMode = !(*m_mapWindows.begin()).second->GetWindowProperties()->GetFullScreen();
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = m_mapWindows.begin();
				itWin != m_mapWindows.end(); ++itWin )
	{
		if( (*itWin).second->GetWindowProperties()->SetFullScreen( bMode ) )
		{
			std::cout << "Fullscreen [" << ( bMode ? "Enabled" : "Disabled" )
						<< "] for Window [" << (*itWin).second->GetNameForNameable()
						<< "]" << std::endl;
		}
		else
		{
			vstr::warnp() << "Setting Fullscreen FAILED for Window ["
					<< (*itWin).second->GetNameForNameable() << "]" << std::endl;
		}
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


VistaToggleEyeTesterCommand::VistaToggleEyeTesterCommand( VistaSystem* pSystem )
: m_pTester( NULL )
, m_pSystem( pSystem )
{	
}

VistaToggleEyeTesterCommand::~VistaToggleEyeTesterCommand()
{
	delete m_pTester;
}

bool VistaToggleEyeTesterCommand::Do()
{
	if( m_pTester )
	{
		delete m_pTester;
		m_pTester = NULL;
	}
	else
	{
		if( m_pSystem->GetDisplayManager()->GetNumberOfDisplaySystems() > 0 )
		{
			VistaTransformNode* pNode = m_pSystem->GetPlatformFor( 
								m_pSystem->GetDisplayManager()->GetDisplaySystem( 0 ) )->GetPlatformUserNode();
			m_pTester = new VistaEyeTester( pNode, m_pSystem->GetGraphicsManager()->GetSceneGraph() );
		}
		else
		{
			vstr::warnp() << "[VistaToggleEyeTesterCommand]: "
						<< "Cannot enable Eyetest - no Dispaly System available" << std::endl;
			return false;
		}		
	}
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

VistaMakeScreenshotCommand::VistaMakeScreenshotCommand( VistaDisplayManager* pDisplayManager,
													   VistaClusterMode* pCluster,
													   const std::string& sExtension,
													   const std::string& sScreenshotDir )
: m_pDisplayManager( pDisplayManager )
, m_pCluster( pCluster )
, m_sExtension( sExtension )
, m_sScreenshotDir( sScreenshotDir )
{
}

bool VistaMakeScreenshotCommand::Do()
{
	const std::map<std::string, VistaWindow*>& mapWindows = m_pDisplayManager->GetWindowsConstRef();
	if( mapWindows.empty() )
		return false;

	std::string sFilePrefix;
	
	if( m_sScreenshotDir.empty() == false )
	{
		VistaFileSystemDirectory oDir( m_sScreenshotDir );
		if( oDir.Exists() == false && oDir.CreateWithParentDirectories() == false )
		{
			vstr::warnp() << "[VistaMakeScreenshotCommand]: Could not create screenshot dir "
							<< m_sScreenshotDir << std::endl;
			return false;
		}
		sFilePrefix = m_sScreenshotDir + "/";
	}

	sFilePrefix += "screenshot_" 
							+ VistaTimeUtils::ConvertToLexicographicDateString( m_pCluster->GetFrameClock() )
							+ "_" + m_pCluster->GetNodeName();
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = mapWindows.begin();
			itWin != mapWindows.end(); ++itWin )
	{
		std::string sFilename;
		int nCount = 1;
		for( ;; )
		{
			sFilename = sFilePrefix;
			if( mapWindows.size() != 1 )
				sFilename += "_" + (*itWin).first;
			if( nCount > 1 )
				sFilename += "(" + VistaConversion::ToString( nCount ) + ")";
			sFilename += "." + m_sExtension;
			if( VistaFileSystemFile( sFilename ).Exists() == false )
				break;
			++nCount;
		}		

		vstr::outi() << "[VistaMakeScreenshotCommand]: Storing screenshot at " << sFilename << std::endl;
		m_pDisplayManager->MakeScreenshot( (*itWin).second, sFilename );
	}
	return true;
}
