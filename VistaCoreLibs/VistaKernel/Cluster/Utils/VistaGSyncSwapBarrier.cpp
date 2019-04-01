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

#include "VistaGSyncSwapBarrier.h"

#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
namespace
{
	typedef bool (APIENTRY* PFNJOINSWAPGROUP) ( HDC oContext, GLuint nGroupID );
	typedef bool (APIENTRY* PFNBINDSWAPBARRIER) ( GLuint nGroupID, GLuint nBarrierID );
	typedef bool (APIENTRY* PFNQUERYMAXSWAPGROUPS) ( HDC oContext, GLuint* nMaxGroup, GLuint* nMaxBarrier );
	PFNJOINSWAPGROUP		S_pfJoinSwapGroup = NULL;
	PFNBINDSWAPBARRIER		S_pfBindSwapBarrier = NULL;
	PFNQUERYMAXSWAPGROUPS	S_pfQueryMaxSwapGroups = NULL;
}
#elif defined LINUX
	#include <GL/gl.h>
	#include <GL/glx.h>
namespace
{
	typedef bool (* PFNJOINSWAPGROUP) ( Display* pContext, GLXDrawable drawable, GLuint nGroupID );
	typedef bool (* PFNBINDSWAPBARRIER) ( Display* pContext, GLuint nGroupID, GLuint nBarrierID );
	typedef bool (* PFNQUERYMAXSWAPGROUPS) ( Display* pContext, GLuint nDisplay, GLuint* nMaxGroup, GLuint* nMaxBarrier );
	PFNJOINSWAPGROUP		S_pfJoinSwapGroup = NULL;
	PFNBINDSWAPBARRIER		S_pfBindSwapBarrier = NULL;
	PFNQUERYMAXSWAPGROUPS	S_pfQueryMaxSwapGroups = NULL;
}
#endif
bool			S_bSearchPerformed = false;


bool PrepareCallbacks()
{

	if( S_bSearchPerformed == false)
	{
#ifdef WIN32
		S_pfJoinSwapGroup = (PFNJOINSWAPGROUP)wglGetProcAddress( "wglJoinSwapGroupNV" );
		S_pfBindSwapBarrier = (PFNBINDSWAPBARRIER)wglGetProcAddress( "wglBindSwapBarrierNV" );
		S_pfQueryMaxSwapGroups = (PFNQUERYMAXSWAPGROUPS)wglGetProcAddress( "wglQueryMaxSwapGroupsNV" );
#elif defined LINUX
		S_pfJoinSwapGroup = (PFNJOINSWAPGROUP)glXGetProcAddress( (const GLubyte*)"glXJoinSwapGroupNV" );
		S_pfBindSwapBarrier = (PFNBINDSWAPBARRIER)glXGetProcAddress( (const GLubyte*)"glXBindSwapBarrierNV" );
		S_pfQueryMaxSwapGroups = (PFNQUERYMAXSWAPGROUPS)glXGetProcAddress( (const GLubyte*)"glXQueryMaxSwapGroupsNV" );
#endif

		if( S_pfBindSwapBarrier == NULL
			|| S_pfQueryMaxSwapGroups == NULL
			|| S_pfJoinSwapGroup == NULL )
		{			
			S_pfQueryMaxSwapGroups = NULL;
			S_pfBindSwapBarrier = NULL;
			S_pfJoinSwapGroup = NULL;
		}

		
		S_bSearchPerformed = true;
	}

	return ( S_pfJoinSwapGroup != NULL ); // already found functions
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/




/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VISTAKERNELAPI 
bool VistaGSyncSwapBarrier::JoinSwapBarrier( VistaDisplayManager* pDisplayManager )
{
	if( PrepareCallbacks() == false )
	{
		vstr::warnp() << "VistaGSyncSwapBarrier::JoinSwapBarrier() -- "
					<< "required gl extensions are not available" << std::endl;
		return false;
	}

	IVistaWindowingToolkit* pWindowingToolkit = pDisplayManager->GetWindowingToolkit();

	for( std::map<std::string, VistaWindow*>::const_iterator itWin = pDisplayManager->GetWindowsConstRef().begin();
			itWin != pDisplayManager->GetWindowsConstRef().end(); ++itWin )
	{		

		pWindowingToolkit->BindWindow( (*itWin).second );
				
#ifdef WIN32
		HDC pContext = wglGetCurrentDC();
#else
		Display* pContext = glXGetCurrentDisplay();
		GLXDrawable oDrawable = glXGetCurrentDrawable();
#endif
		if( pContext == NULL )
		{
			vstr::warnp() << "VistaGSyncSwapBarrier::JoinSwapBarrier() -- "
						<< "no gl context available" << std::endl;
			return false;
		}

		GLuint nMaxSwapGroups = 0;
		GLuint nMaxSwapBarriers = 0;
		bool bSuccess;
		
#ifdef WIN32
		bSuccess = S_pfQueryMaxSwapGroups( pContext, &nMaxSwapGroups, &nMaxSwapBarriers );
#else
		bSuccess = S_pfQueryMaxSwapGroups( pContext, 0, &nMaxSwapGroups, &nMaxSwapBarriers );
#endif
		if( bSuccess == false )
		{
			vstr::warnp() << "VistaGSyncSwapBarrier::JoinSwapBarrier() -- "
						<< "calling S_pfQueryMaxSwapGroups failed" << std::endl;
			return false;
		}
		
		vstr::outi() << "S_pfQueryMaxSwapGroups() = "
					<< nMaxSwapGroups << ", " << nMaxSwapBarriers << std::endl;
					
		if( nMaxSwapGroups < 1 || nMaxSwapBarriers < 1 )
		{
			vstr::warnp() << "VistaGSyncSwapBarrier::JoinSwapBarrier() -- "
						<< "no swap groups/barriers available" << std::endl;
			return false;
		}
		
		vstr::outi() << "S_pfJoinSwapGroup" << std::endl;
#ifdef WIN32 
		bSuccess = S_pfJoinSwapGroup( pContext, 1 );
#else
		bSuccess = S_pfJoinSwapGroup( pContext, oDrawable, 1 );
#endif
		if( bSuccess == false )
		{
			vstr::warnp() << "VistaGSyncSwapBarrier::JoinSwapBarrier() -- "
						<< "JoinSwapGroup call failed" << std::endl;
			return false;
		}
		
		vstr::outi() << "S_pfBindSwapBarrier" << std::endl;
#ifdef WIN32 
		bSuccess = S_pfBindSwapBarrier( 1, 1 );
#else
		bSuccess = S_pfBindSwapBarrier( pContext, 1, 1 );
#endif
		if( bSuccess == false )
		{
			vstr::warnp() << "VistaGSyncSwapBarrier::JoinSwapBarrier() -- "
						<< "BindSwapBarrier call failed" << std::endl;
			return false;
		}
		vstr::outi() << "Joined Swap group!" << std::endl;
	}

	return true;
}

bool VistaGSyncSwapBarrier::LeaveSwapBarrier( VistaDisplayManager* pDisplayManager )
{
	if( PrepareCallbacks() == false )
	{
		vstr::warnp() << "VistaGSyncSwapBarrier::LeaveSwapBarrier() -- "
					<< "required gl extensions are not available" << std::endl;
		return false;
	}
	
	IVistaWindowingToolkit* pWindowingToolkit = pDisplayManager->GetWindowingToolkit();

	for( std::map<std::string, VistaWindow*>::const_iterator itWin = pDisplayManager->GetWindowsConstRef().begin();
			itWin != pDisplayManager->GetWindowsConstRef().end(); ++itWin )
	{		

		pWindowingToolkit->BindWindow( (*itWin).second );
			
#ifdef WIN32
		HDC pContext = wglGetCurrentDC();
#else
		Display* pContext = glXGetCurrentDisplay();
		GLXDrawable oDrawable = glXGetCurrentDrawable();
#endif
		if( pContext == NULL )
		{
			vstr::warnp() << "VistaGSyncSwapBarrier::LeaveSwapBarrier() -- "
						<< "no gl context available" << std::endl;
			return false;
		}

#ifdef WIN32 
		bool bSuccess = S_pfJoinSwapGroup( pContext, 0 );
#else
		bool bSuccess = S_pfJoinSwapGroup( pContext, oDrawable, 0 );
#endif
		if( bSuccess == false )
		{
			vstr::warnp() << "VistaGSyncSwapBarrier::JoinSwapBarrier() -- "
						<< "JoinSwapGroup call failed" << std::endl;
			return false;
		}
	}

	return true;
}
