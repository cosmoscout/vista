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


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <GL/glew.h>

#include "VistaSDL2WindowingToolkit.h"
#include "VistaSDL2TextEntity.h"

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/GraphicsManager/VistaGLTexture.h>
#include <VistaKernel/GraphicsManager/VistaImage.h>

#include <VistaBase/VistaExceptionBase.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <cassert>

// @TODO: switch to glew for VSync 
#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
typedef bool (APIENTRY* PFNWGLSWAPINTERVALEXT) (int);
typedef int (APIENTRY* PFNWGLGETSWAPINTERVALEXT) (void);
PFNWGLSWAPINTERVALEXT		SetSwapIntervalFunction = NULL;
PFNWGLGETSWAPINTERVALEXT    GetSwapIntervalFunction = NULL;
#elif defined LINUX
#include <GL/gl.h>
#include <GL/glx.h>

typedef int (* PFNGLXSWAPINTERVALSGIPROC) (int interval);
typedef int (* PFNGLXGETSWAPINTERVALSGIPROC) ();
PFNGLXSWAPINTERVALSGIPROC SetSwapIntervalFunction = NULL;
PFNGLXGETSWAPINTERVALSGIPROC GetSwapIntervalFunction = NULL;
#elif defined(DARWIN)
#include <OpenGL/OpenGL.h>
typedef int (* PFNGLXSWAPINTERVALSGIPROC) (int interval);
typedef int (* PFNGLXGETSWAPINTERVALSGIPROC) ();
PFNGLXSWAPINTERVALSGIPROC SetSwapIntervalFunction = NULL;
PFNGLXGETSWAPINTERVALSGIPROC GetSwapIntervalFunction = NULL;
#endif

#ifdef DEBUG
#define DEBUG_CHECK_GL( sPrefix ) \
	{ \
		GLenum nError = glGetError(); \
		while( nError != GL_NONE ) \
		{ \
			vstr::warnp() << "[SDLWindoingTK]: Cought gl error after " << sPrefix << ": " << gluErrorString( nError ) << std::endl; \
			nError = glGetError(); \
		} \
	}
#else
#define DEBUG_CHECK_GL( sPrefix )
#endif



/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

// Maintain maps of all windows in order to be able to match incoming
// sdl callbacks to the correct callback. Since all windows usually use the
// same callback, this is only important if multiple displaysystems/VistaSystems
// are around

struct SDL2WindowInfo
{
	SDL2WindowInfo( VistaWindow* pWindow )
	: m_pWindow( pWindow )
	, m_pUpdateCallback( nullptr )
	, m_iCurrentSizeX( 0 )
	, m_iCurrentSizeY( 0 )
	, m_iCurrentPosX( 0 )
	, m_iCurrentPosY( 0 )
	, m_iPreFullscreenSizeX( 0 )
	, m_iPreFullscreenSizeY( 0 )
	, m_iPreFullscreenPosX( 0 )
	, m_iPreFullscreenPosY( 0 )
	, m_bFullscreenActive( false )
	, m_bUseStereo( false )
	, m_bUseAccumBuffer( false )
	, m_bUseStencilBuffer( false )
	, m_bDrawBorder( true )
	, m_iWindowID( -1 )
	, m_sWindowTitle( "ViSTA" )
	, m_iVSyncMode( VistaSDL2WindowingToolkit::VSYNC_STATE_UNKNOWN )
	, m_bCursorEnabled( true )
	, m_iCursor( -1 )
	, m_bIsOffscreenBuffer( false )
	, m_nFboId( 0 )
	, m_nFboDepthId( 0 )
	, m_nFboColorId( 0 )
	, m_nFboStencilId( 0 )
	, m_nBlitFboId( 0 )
	, m_nBlitFboColorId( 0 )
	, m_nBlitFboDepthId( 0 )
	, m_iContextMajor( 1 )
	, m_iContextMinor( 0 )
	, m_bIsDebugContext( false )
	, m_bIsForwardCompatible( false )
	, m_nNumMultiSamples( 0 )
	, m_bIsInitialized( false )
	{
	}

	~SDL2WindowInfo()
	{
	}

	bool				m_bIsInitialized;
	VistaWindow*		m_pWindow;
	IVistaExplicitCallbackInterface*
						m_pUpdateCallback;
	int					m_iCurrentSizeX;
	int					m_iCurrentSizeY;
	int					m_iCurrentPosX;
	int					m_iCurrentPosY;
	int					m_iPreFullscreenSizeX;
	int					m_iPreFullscreenSizeY;
	int					m_iPreFullscreenPosX;
	int					m_iPreFullscreenPosY;
	bool				m_bFullscreenActive;
	bool				m_bUseStereo;
	bool				m_bUseAccumBuffer;
	bool				m_bUseStencilBuffer;
	int					m_nNumMultiSamples;
	bool				m_bDrawBorder;
	int					m_iWindowID;
	std::string			m_sWindowTitle;
	int					m_iVSyncMode;
	int					m_iContextMajor;
	int					m_iContextMinor;
	bool				m_bIsDebugContext;
	bool				m_bIsForwardCompatible;
	bool				m_bCursorEnabled;
	int					m_iCursor;
	// for Offscreen Buffer
	bool				m_bIsOffscreenBuffer;
	GLuint				m_nFboId;
	GLuint				m_nFboColorId;
	GLuint				m_nFboDepthId;
	GLuint				m_nFboStencilId;
	GLuint				m_nBlitFboId;
	GLuint				m_nBlitFboColorId;
	GLuint				m_nBlitFboDepthId;
};

namespace
{
	std::map<int, SDL2WindowInfo*> S_mapWindowInfo;

	void DisplayUpdate()
	{
		std::map<int, SDL2WindowInfo*>::const_iterator itWindowInfo =
												S_mapWindowInfo.find( 0 );
		assert( itWindowInfo != S_mapWindowInfo.end() );

		(*itWindowInfo).second->m_pUpdateCallback->Do();
	}

	void DisplayReshape( int iWidth, int iHeight )
	{
		std::map<int, SDL2WindowInfo*>::const_iterator itWindowInfo =
												S_mapWindowInfo.find( 0 );
		assert( itWindowInfo != S_mapWindowInfo.end() );
		SDL2WindowInfo* pInfo = itWindowInfo->second;
		if( pInfo->m_iCurrentSizeX != iWidth || pInfo->m_iCurrentSizeY != iHeight )
		{
			pInfo->m_iCurrentSizeX = iWidth;
			pInfo->m_iCurrentSizeY = iHeight;
			(*itWindowInfo).second->m_pWindow->GetProperties()->Notify(
								VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE );
		}	
	}

	void CloseFunction()
	{
		if( GetVistaSystem() )
		{
			vstr::warni() << "SDL2Window closed - Quitting Vista" << std::endl;
			GetVistaSystem()->Quit();
		}
	}
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaSDL2WindowingToolkit::VistaSDL2WindowingToolkit()
: m_bQuitLoop( false )
, m_pUpdateCallback( NULL )
, m_iTmpWindowID( -1 )
, m_iGlobalVSyncAvailability( ~0 )
, m_bHasFullWindow( false )
, m_nFullWindowId( -1 )
, m_nDummyWindowId( -1 )
{
	if( SDL_Init( SDL_INIT_VIDEO ) != 0 || TTF_Init() != 0 ) {
		vstr::warni() << "SDL2 init failed - Quitting Vista" << std::endl;
		GetVistaSystem()->Quit();
	}
}

VistaSDL2WindowingToolkit::~VistaSDL2WindowingToolkit()
{
}

/*============================================================================*/
/* FACTORY METHODS                                                            */
/*============================================================================*/

IVistaTextEntity* VistaSDL2WindowingToolkit::CreateTextEntity()
{
	return new VistaSDL2TextEntity(); //TODO
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaSDL2WindowingToolkit::Run()
{
	while( !m_bQuitLoop )
	{
		if( m_bHasFullWindow )
			glutMainLoopEvent();
		else
			m_pUpdateCallback->Do();
	}
}

void VistaSDL2WindowingToolkit::Quit()
{
	m_bQuitLoop = true;
}

void VistaSDL2WindowingToolkit::DisplayWindow( const VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) == false )		
	{
		vstr::warnp() << "[GlutWindowing]: Trying to render invalid window" << std::endl;
		return;
	}
	
	if( pInfo->m_bIsOffscreenBuffer == false )
	{
		glutSwapBuffers();
		glutPostRedisplay();
	}
	else if( pInfo->m_nNumMultiSamples > 1 )
	{
		// read from multisample fbo
		glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, pInfo->m_nFboId );
		// to normal fbo
		glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, pInfo->m_nBlitFboId );
		// blit!
		glBlitFramebuffer( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY,
						   GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST );

		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, pInfo->m_nFboId );
		
		DEBUG_CHECK_GL( "Multisample-FBO-blit" );
	}
	
	PopWindow();
}
void VistaGlutWindowingToolkit::DisplayAllWindows()
{
	if( m_mapWindowInfo.empty() )
		return;

	int iActiveID = glutGetWindow();
	for( WindowInfoMap::const_iterator 
			itWindowID = m_mapWindowInfo.begin(); 
			itWindowID != m_mapWindowInfo.end();
			++itWindowID )
	{		
		DisplayWindow( (*itWindowID).first );
	}
	glutSetWindow( iActiveID );	
}

bool VistaGlutWindowingToolkit::RegisterWindow( VistaWindow* pWindow )
{
	WindowInfoMap::const_iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists != m_mapWindowInfo.end() )
		return false;
	GlutWindowInfo* pInfo = new GlutWindowInfo( pWindow );	
	m_mapWindowInfo[pWindow] = pInfo;
	return true;
}
bool VistaGlutWindowingToolkit::UnregisterWindow( VistaWindow* pWindow )
{
	WindowInfoMap::iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists == m_mapWindowInfo.end() )
		return false;
	int iID = (*itExists).second->m_iWindowID;
	if( iID != -1 )
	{
		PushWindow( (*itExists).second );
		// unset the close function
		glutCloseFunc( NULL );
		PopWindow();
	}
	else if( (*itExists).second->m_bIsOffscreenBuffer )
	{
		glDeleteFramebuffers( 1, &(*itExists).second->m_nFboId );
		glDeleteRenderbuffers( 1, &(*itExists).second->m_nFboDepthId );
		glDeleteRenderbuffers( 1, &(*itExists).second->m_nFboColorId );
		if( (*itExists).second->m_nFboStencilId != 0 )
			glDeleteRenderbuffers( 1, &(*itExists).second->m_nFboStencilId );
		if( (*itExists).second->m_nNumMultiSamples > 1 )
		{
			glDeleteFramebuffers( 1, &(*itExists).second->m_nBlitFboId );
			glDeleteRenderbuffers( 1, &(*itExists).second->m_nBlitFboDepthId );
			glDeleteRenderbuffers( 1, &(*itExists).second->m_nBlitFboColorId );
		}
		DEBUG_CHECK_GL( "Post-OffscreenBuffer-Win-delete" );
	}
	delete (*itExists).second;
	m_mapWindowInfo.erase( itExists );
	return true;
}

bool VistaGlutWindowingToolkit::InitWindow( VistaWindow* pWindow )
{
	WindowInfoMap::iterator itExists = m_mapWindowInfo.find( pWindow );
	if( itExists == m_mapWindowInfo.end() )
	{
		vstr::errp() << "[GlutWindowingTollkit]: "
				<< "Trying to initialize Window that was not registered before"
				<< std::endl;
		return false;
	}
	GlutWindowInfo* pInfo = (*itExists).second;

	if( pInfo->m_bIsInitialized )
	{
		vstr::errp() << "[GlutWindowingTollkit]: "
				<< "Trying to initialize Window [" << pWindow->GetNameForNameable()
				<< "] which was already initialized" << std::endl;
		return false;
	}

	if( pInfo->m_bIsOffscreenBuffer == false )
	{
		if( InitAsNormalWindow( pWindow ) == false )
			return false;
		DestroyDummyWindow();
	}
	else // is RenderToTexture
	{
		if( InitAsFbo( pWindow ) == false )
			return false;
	}

	
	pInfo->m_bIsInitialized = true;
	return true;
}

bool VistaGlutWindowingToolkit::InitAsNormalWindow( VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	int iDisplayMode = GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE;
	if( pInfo->m_bUseStereo )
		iDisplayMode = iDisplayMode | GLUT_STEREO;
	if( pInfo->m_bUseAccumBuffer )
		iDisplayMode = iDisplayMode | GLUT_ACCUM;
	if( pInfo->m_bUseStencilBuffer )
		iDisplayMode = iDisplayMode | GLUT_STENCIL;

	if( pInfo->m_bDrawBorder == false )
	{
#if HAS_FREEGLUT_28
		iDisplayMode = iDisplayMode | GLUT_BORDERLESS;
#else
		vstr::warnp() << "[GlutWindowingTollkit]: "
			<< "Borderless windows only available with freeglut 2.8+" << std::endl;
#endif
	}

	if( pInfo->m_iContextMajor != 1 || pInfo->m_iContextMinor != 0 )
	{
#if !HAS_FREEGLUT_28
		vstr::warnp() << "[GlutWindowingToolkit]: "
			<< "Context Version only available with freeglut" << std::endl;
#else
		glutInitContextVersion(pInfo->m_iContextMajor, pInfo->m_iContextMinor);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
#endif
	}

	if( pInfo->m_nNumMultiSamples > 1 )
	{
#ifdef USE_NATIVE_GLUT
		vstr::warnp() << "[GlutWindowingToolkit]: "
			<< "Multisample window requested - only available for freeglut" << std::endl;	
#else
		iDisplayMode = iDisplayMode | GLUT_MULTISAMPLE;
		glutSetOption( GLUT_MULTISAMPLE, pInfo->m_nNumMultiSamples );
#endif
	}

	if( pInfo->m_bIsDebugContext || pInfo->m_bIsForwardCompatible )
	{
		int iContextFlags = 0;
#if !HAS_FREEGLUT_28
		vstr::warnp() << "[GlutWindowingToolkit]: "
			<< "Context Flags (DebugContext, ForwardCompatible) only available with freeglut 2.8+" << std::endl;
#else
		if( pInfo->m_bIsDebugContext )
		{
			iContextFlags |= GLUT_DEBUG;
		}
		if( pInfo->m_bIsForwardCompatible )
		{
			iContextFlags |= GLUT_FORWARD_COMPATIBLE;
		}

		glutInitContextFlags( iContextFlags );
#endif
	}

	glutInitDisplayMode( iDisplayMode );
	if( pInfo->m_iCurrentPosX != -1 && pInfo->m_iCurrentPosY != -1 )
	{
		glutInitWindowPosition( pInfo->m_iCurrentPosX, pInfo->m_iCurrentPosY );
	}
	if( pInfo->m_iCurrentSizeX != -1 && pInfo->m_iCurrentSizeY != -1 )
	{
		glutInitWindowSize(  pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	}

	pInfo->m_iWindowID = glutCreateWindow( pInfo->m_sWindowTitle.c_str() );
	glewInit();

#ifdef WIN32
	if( m_bHasFullWindow )
	{
		glutSetWindow( m_nFullWindowId );
		HGLRC pFirstContext = wglGetCurrentContext();
		glutSetWindow( pInfo->m_iWindowID );
		HGLRC pOwnContext = wglGetCurrentContext();
		wglShareLists( pFirstContext, pOwnContext );
	}
#endif

	m_mapWindowInfo[pWindow] = pInfo;
	S_mapWindowInfo[pInfo->m_iWindowID] = pInfo;

	glutSetWindow( pInfo->m_iWindowID );

	if( m_pUpdateCallback )
	{
		pInfo->m_pUpdateCallback = m_pUpdateCallback;
		glutDisplayFunc( &DisplayUpdate );
		glutIdleFunc( &DisplayUpdate );
	}
	glutReshapeFunc( &DisplayReshape );

	if( pInfo->m_bFullscreenActive )
	{
		// store size/position to restore it when turning off game mode
		pInfo->m_iPreFullscreenPosX = glutGet( GLUT_WINDOW_X );
		pInfo->m_iPreFullscreenPosY = glutGet( GLUT_WINDOW_Y );
		pInfo->m_iPreFullscreenSizeX = glutGet( GLUT_WINDOW_WIDTH );
		pInfo->m_iPreFullscreenSizeY = glutGet( GLUT_WINDOW_HEIGHT );
		glutFullScreen();
	}

	// retrieve actual size/position
	pInfo->m_iCurrentPosX = glutGet( GLUT_WINDOW_X );
	pInfo->m_iCurrentPosY = glutGet( GLUT_WINDOW_Y );
	pInfo->m_iCurrentSizeX = glutGet( GLUT_WINDOW_WIDTH );
	pInfo->m_iCurrentSizeY = glutGet( GLUT_WINDOW_HEIGHT );

	pInfo->m_bIsInitialized = true; // we already set the flag here, because otherwise
									// SetVSyncMode doesn't actually set the mode
	if( pInfo->m_iVSyncMode == VSYNC_ENABLED )
		SetVSyncMode( pWindow, true );
	else if( pInfo->m_iVSyncMode == VSYNC_DISABLED )
		SetVSyncMode( pWindow, false );

	if( m_bHasFullWindow == false )
	{
		m_bHasFullWindow = true;
		m_nFullWindowId = pInfo->m_iWindowID;
	}
		
#ifndef USE_NATIVE_GLUT
	// set the close function to catch window close attempts
	glutCloseFunc( CloseFunction );
	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION );
#endif // USE_NATIVE_GLUT

	glutPostRedisplay();

	return true;
}

bool VistaGlutWindowingToolkit::InitAsFbo( VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( m_bHasFullWindow == false && m_nDummyWindowId == -1 )
	{
		vstr::warnp() << "Using offscreen window without valid real window - creating dummy win for context" << std::endl;
		CreateDummyWindow( pWindow );
	}		

	if( pInfo->m_bUseAccumBuffer )
	{
		vstr::warnp() << "[GlutWindowingToolkit]: "
				<< "Window [" << pWindow->GetNameForNameable() << "] is offscreen buffer, but requests accum "
				<< "buffer - combination invalid, accum buffer will be unavailable" << std::endl;
		pInfo->m_bUseAccumBuffer = false;
	}
	if( pInfo->m_bUseStereo )
	{
		vstr::warnp() << "[GlutWindowingToolkit]: "
				<< "Window [" << pWindow->GetNameForNameable() << "] is offscreen buffer, but requests stereo mode "
				<< "buffer - combination invalid, stereo will be unavailable" << std::endl;
		pInfo->m_bUseStereo = false;
	}

	if( glewGetExtension("GL_EXT_framebuffer_object") == GL_FALSE )
	{
		vstr::errp() << "[GlutWindowingTollkit]: "
			<< "Trying to initialize Window [" << pWindow->GetNameForNameable()
			<< "] as offscreen buffer failed - framebuffer objects not supported" << std::endl;
		return false;
	}
	assert( __glewGenFramebuffersEXT != NULL );

	// GL_MAX_FRAMEBUFFER_WIDTH seem to not be available in all glew versions
#ifdef GL_MAX_FRAMEBUFFER_WIDTH
	GLint nMaxWidth, nMaxHeight;
	glGetIntegerv( GL_MAX_FRAMEBUFFER_WIDTH, &nMaxWidth );
	glGetIntegerv( GL_MAX_FRAMEBUFFER_HEIGHT, &nMaxHeight );
	if( pInfo->m_iCurrentSizeX > nMaxWidth
		|| pInfo->m_iCurrentSizeY > nMaxHeight )
	{
		vstr::errp() << "[GLuWindow]: cannot create render-to-texture window - size exceeds allowed max ["
						<< nMaxWidth << "x" << nMaxHeight << "]" << std::endl;
		return false;
	}
#endif
	if( pInfo->m_nNumMultiSamples > 1  )
	{
		return InitAsMultisampleFbo( pWindow );
	}

	// create Fbo
	glGenFramebuffers( 1, &pInfo->m_nFboId );
	glBindFramebuffer( GL_FRAMEBUFFER, pInfo->m_nFboId );

	glGenRenderbuffers( 1, &pInfo->m_nFboColorId );
	glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboColorId );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGB, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pInfo->m_nFboColorId ); 
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	glGenRenderbuffers( 1, &pInfo->m_nFboDepthId );
	glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboDepthId );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pInfo->m_nFboDepthId ); 
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	if( pInfo->m_bUseStencilBuffer )
	{
		glGenRenderbuffers( 1, &pInfo->m_nFboStencilId );
		glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboStencilId );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_STENCIL_INDEX, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pInfo->m_nFboStencilId ); 
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}
		
	GLenum nStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if( nStatus != GL_FRAMEBUFFER_COMPLETE )
		VISTA_THROW( "Failed to set up frame buffer window", -1 );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	
	DEBUG_CHECK_GL( "Post-OffscreenBuffer-Win-Swap" );

	return true;
}

bool VistaGlutWindowingToolkit::InitAsMultisampleFbo( VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( glewGetExtension("GL_EXT_framebuffer_multisample") == GL_FALSE )
	{
		vstr::errp() << "[GlutWindowingTollkit]: "
			<< "Trying to initialize Window [" << pWindow->GetNameForNameable()
			<< "] as multisample offscreen buffer failed - multisample framebuffer objects not supported" << std::endl;
		return false;
	}
	assert( __glewRenderbufferStorageMultisampleEXT != NULL );

	// create Fbo
	glGenFramebuffers( 1, &pInfo->m_nFboId );
	glBindFramebuffer( GL_FRAMEBUFFER, pInfo->m_nFboId );

	glGenRenderbuffers( 1, &pInfo->m_nFboColorId );
	glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboColorId );
	glRenderbufferStorageMultisample( GL_RENDERBUFFER, pInfo->m_nNumMultiSamples, GL_RGB, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pInfo->m_nFboColorId ); 
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	glGenRenderbuffers( 1, &pInfo->m_nFboDepthId );
	glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboDepthId );
	glRenderbufferStorageMultisample( GL_RENDERBUFFER, pInfo->m_nNumMultiSamples, GL_DEPTH_COMPONENT24, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pInfo->m_nFboDepthId ); 
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	if( pInfo->m_bUseStencilBuffer )
	{
		glGenRenderbuffers( 1, &pInfo->m_nFboStencilId );
		glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboStencilId );
		glRenderbufferStorageMultisample( GL_RENDERBUFFER, pInfo->m_nNumMultiSamples, GL_STENCIL_INDEX, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pInfo->m_nFboStencilId ); 
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}
		
	GLenum nStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if( nStatus != GL_FRAMEBUFFER_COMPLETE )
		VISTA_THROW( "Failed to set up frame buffer window", -1 );

	glBindFramebuffer( GL_FRAMEBUFFER, 0);
	
	DEBUG_CHECK_GL( "Post-OffscreenBuffer-Win-Swap" );

	// multisample framebuffer is complete, but we still need one to blit the multisample-image to
	glGenFramebuffers( 1, &pInfo->m_nBlitFboId );
	glBindFramebuffer( GL_FRAMEBUFFER, pInfo->m_nBlitFboId );

	glGenRenderbuffers( 1, &pInfo->m_nBlitFboColorId );
	glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nBlitFboColorId );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGB, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pInfo->m_nBlitFboColorId ); 
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	glGenRenderbuffers( 1, &pInfo->m_nBlitFboDepthId );
	glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nBlitFboDepthId );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pInfo->m_nBlitFboDepthId ); 
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	nStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if( nStatus != GL_FRAMEBUFFER_COMPLETE )
		VISTA_THROW( "Failed to set up frame buffer window", -1 );

	glBindFramebuffer( GL_FRAMEBUFFER, 0);

	return true;
}

bool VistaGlutWindowingToolkit::SetWindowUpdateCallback( 
									IVistaExplicitCallbackInterface* pCallback )
{
	m_pUpdateCallback = pCallback;

	int iActiveID = glutGetWindow();
	for( WindowInfoMap::const_iterator itWindow = m_mapWindowInfo.begin();
			itWindow != m_mapWindowInfo.end(); ++itWindow )
	{
		(*itWindow).second->m_pUpdateCallback = pCallback;
		int iID = (*itWindow).second->m_iWindowID;
		if( iID != -1 )
		{
			glutSetWindow( iID );
			glutDisplayFunc( &DisplayUpdate );
			glutIdleFunc( &DisplayUpdate );
		}
		
	}
	if( m_mapWindowInfo.empty() == false )
		glutPostRedisplay();
	glutSetWindow( iActiveID );
	return true;
}
bool VistaGlutWindowingToolkit::GetWindowPosition( const VistaWindow* pWindow,
												  int &iX, int& iY ) const
{
	if( PushWindow( pWindow ) )
	{
		iX = glutGet( GLUT_WINDOW_X );
		iY = glutGet( GLUT_WINDOW_Y );	
		PopWindow();
	}
	else
	{
		GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
		iX = pInfo->m_iCurrentPosX;
		iY = pInfo->m_iCurrentPosY;
	}
	return true;
}

bool VistaGlutWindowingToolkit::SetWindowPosition( VistaWindow* pWindow,
												  const int iX, const int iY )
{	
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}

	if( PushWindow( pInfo ) )
	{
		if( pInfo->m_bFullscreenActive )
		{
			pInfo->m_iPreFullscreenPosX = iX;
			pInfo->m_iPreFullscreenPosY = iY;
		}
		else
		{
			glutPositionWindow( iX, iY );			
			pInfo->m_iCurrentPosX = iX;
			pInfo->m_iCurrentPosY = iY;		
		}
		PopWindow();
	}
	else
	{
		pInfo->m_iCurrentPosX = iX;
		pInfo->m_iCurrentPosY = iY;		
	}
	return true;
}

bool VistaGlutWindowingToolkit::GetWindowSize( const VistaWindow* pWindow,
											  int& iWidth, int& iHeight ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	iWidth = pInfo->m_iCurrentSizeX;
	iHeight = pInfo->m_iCurrentSizeY;
	return true;
}

bool VistaGlutWindowingToolkit::SetWindowSize( VistaWindow* pWindow,
											  int iWidth, int iHeight )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer && pInfo->m_nFboId > 0 )
	{
		DEBUG_CHECK_GL( "Pre-OffscreenBuffer-Win-Resize" );

		pInfo->m_iCurrentSizeX = iWidth;
		pInfo->m_iCurrentSizeY = iHeight;			

		if( pInfo->m_nNumMultiSamples <= 1 )
		{
			glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboColorId );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_RGB, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
			glBindRenderbuffer( GL_RENDERBUFFER, 0 );

			glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboDepthId );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
			glBindRenderbuffer( GL_RENDERBUFFER, 0 );

			if( pInfo->m_bUseStencilBuffer )
			{
				glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboStencilId );
				glRenderbufferStorage( GL_RENDERBUFFER, GL_STENCIL_INDEX, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
				glBindRenderbuffer( GL_RENDERBUFFER, 0 );
			}
#ifdef DEBUG
			glBindFramebuffer( GL_FRAMEBUFFER, pInfo->m_nFboId );
			GLenum nStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
			if( nStatus != GL_FRAMEBUFFER_COMPLETE )
				VISTA_THROW( "Failed to set up frame buffer window", -1 );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#endif
		}
		else
		{
			glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboColorId );
			glRenderbufferStorageMultisample( GL_RENDERBUFFER, pInfo->m_nNumMultiSamples, GL_RGB, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
			glBindRenderbuffer( GL_RENDERBUFFER, 0 );

			glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboDepthId );
			glRenderbufferStorageMultisample( GL_RENDERBUFFER, pInfo->m_nNumMultiSamples, GL_DEPTH_COMPONENT24, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
			glBindRenderbuffer( GL_RENDERBUFFER, 0 );

			if( pInfo->m_bUseStencilBuffer )
			{
				glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nFboStencilId );
				glRenderbufferStorageMultisample( GL_RENDERBUFFER, pInfo->m_nNumMultiSamples, GL_STENCIL_INDEX, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
				glBindRenderbuffer( GL_RENDERBUFFER, 0 );
			}
#ifdef DEBUG
			glBindFramebuffer( GL_FRAMEBUFFER, pInfo->m_nFboId );
			GLenum nStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
			if( nStatus != GL_FRAMEBUFFER_COMPLETE )
				VISTA_THROW( "Failed to set up frame buffer window", -1 );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#endif

			glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nBlitFboColorId );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_RGB, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
			glBindRenderbuffer( GL_RENDERBUFFER, 0 );

			glBindRenderbuffer( GL_RENDERBUFFER, pInfo->m_nBlitFboDepthId );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY );
			glBindRenderbuffer( GL_RENDERBUFFER, 0 );

#ifdef DEBUG
			glBindFramebuffer( GL_FRAMEBUFFER, pInfo->m_nBlitFboId );
			nStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
			if( nStatus != GL_FRAMEBUFFER_COMPLETE )
				VISTA_THROW( "Failed to set up frame buffer window", -1 );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#endif
		}
		
		
		DEBUG_CHECK_GL( "Post-OffscreenBuffer-Win-Resize" );
		return true;
	}
	if( PushWindow( pInfo ) )
	{
		if( pInfo->m_bFullscreenActive )
		{
			pInfo->m_iPreFullscreenSizeX = iWidth;
			pInfo->m_iPreFullscreenSizeY = iHeight;
		}
		else
		{
			glutReshapeWindow( iWidth, iHeight );
			pInfo->m_iCurrentSizeX = iWidth;
			pInfo->m_iCurrentSizeY = iHeight;
		}
		PopWindow();
	}
	else
	{
		pInfo->m_iCurrentSizeX = iWidth;
		pInfo->m_iCurrentSizeY = iHeight;		
	}
	return true;
}

bool VistaGlutWindowingToolkit::GetFullscreen( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bFullscreenActive;
}

bool VistaGlutWindowingToolkit::SetFullscreen( VistaWindow* pWindow,
												   const bool bEnabled )
{	
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}
	
	if( pInfo->m_bFullscreenActive == bEnabled )
		return true;

	if( pInfo->m_bIsInitialized == false )
	{
		// before initialization - store for later
		pInfo->m_bFullscreenActive = true;
		return true;
	}

	if( bEnabled )
	{
		PushWindow( pInfo );
		pInfo->m_iPreFullscreenSizeX = glutGet( GLUT_WINDOW_WIDTH );
		pInfo->m_iPreFullscreenSizeY = glutGet( GLUT_WINDOW_HEIGHT );
		pInfo->m_iPreFullscreenPosX = glutGet( GLUT_WINDOW_X );
		pInfo->m_iPreFullscreenPosY = glutGet( GLUT_WINDOW_Y );
		
		glutFullScreen();
		PopWindow();
		pInfo->m_bFullscreenActive = true;
	}
	else
	{		
		PushWindow( pInfo );
		glutPositionWindow( pInfo->m_iPreFullscreenPosX, pInfo->m_iPreFullscreenPosY );
		glutReshapeWindow( pInfo->m_iPreFullscreenSizeX, pInfo->m_iPreFullscreenSizeY );
		PopWindow();
		pInfo->m_bFullscreenActive = false;	
	}
	return true;
}
bool VistaGlutWindowingToolkit::SetWindowTitle( VistaWindow* pWindow, 
											   const std::string& sTitle )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	pInfo->m_sWindowTitle = sTitle;

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}

	if( PushWindow( pInfo ) )
	{
		glutSetWindowTitle( sTitle.c_str() );
		PopWindow();
	}
	return true;
}

std::string VistaGlutWindowingToolkit::GetWindowTitle( const VistaWindow* pWindow ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_sWindowTitle;
}

bool VistaGlutWindowingToolkit::SetCursorIsEnabled( VistaWindow* pWindow, bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}

	pInfo->m_bCursorEnabled = bSet;
	if( PushWindow( pWindow ) )
	{
		if( bSet )
			glutSetCursor( pInfo->m_iCursor );
		else
			glutSetCursor( GLUT_CURSOR_NONE );
		PopWindow();
	}
	return true;
}
bool VistaGlutWindowingToolkit::GetCursorIsEnabled( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}
	
	return pInfo->m_bCursorEnabled;
}

bool VistaGlutWindowingToolkit::GetUseStereo( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}

	return pInfo->m_bUseStereo;
}
bool VistaGlutWindowingToolkit::SetUseStereo( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}

	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change stereo mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_bUseStereo = bSet;
	return true;
}

bool VistaGlutWindowingToolkit::GetUseAccumBuffer( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}

	return pInfo->m_bUseAccumBuffer;
}
bool VistaGlutWindowingToolkit::SetUseAccumBuffer( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}

	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change accum buffer mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_bUseAccumBuffer = bSet;
	return true;
}

bool VistaGlutWindowingToolkit::GetUseStencilBuffer( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	
	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}
	
	return pInfo->m_bUseStencilBuffer;
}
bool VistaGlutWindowingToolkit::SetUseStencilBuffer( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change stencil buffer mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_bUseStencilBuffer = bSet;
	return true;
}

int VistaGlutWindowingToolkit::GetMultiSamples( const VistaWindow* pWindow ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );	
	
	return pInfo->m_nNumMultiSamples;
}

bool VistaGlutWindowingToolkit::SetMultiSamples(  const VistaWindow* pWindow, const int nNumSamples )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	
	// @TODO: allow enabling/disabling?
	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change multisampling on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_nNumMultiSamples = nNumSamples;
	return true;
}


bool VistaGlutWindowingToolkit::GetUseOffscreenBuffer( const VistaWindow* pWindow ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	return pInfo->m_bIsOffscreenBuffer;
}


bool VistaGlutWindowingToolkit::GetDrawBorder( const VistaWindow* pWindow ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_bDrawBorder;
}

bool VistaGlutWindowingToolkit::SetDrawBorder( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change borderless prop on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_bDrawBorder = bSet;
	return true;
}


bool VistaGlutWindowingToolkit::SetUseOffscreenBuffer( VistaWindow* pWindow, const bool bSet )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change render-to-texture mode on window ["
				<< pWindow->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}

	pInfo->m_bIsOffscreenBuffer = bSet;
	return true;
}

bool VistaGlutWindowingToolkit::GetRGBImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) == false )
		return false;

	int nNumPixels = pInfo->m_iCurrentSizeX * pInfo->m_iCurrentSizeY;
	int nDataSize = nNumPixels * 3;
	vecData.resize( nDataSize );
	int nRet = GetRGBImage( pWindow, &vecData[0], nDataSize );
	return( nRet == nDataSize );
}

int VistaGlutWindowingToolkit::GetRGBImage( const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize ) const
{	
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) == false )
		return 0;

	DEBUG_CHECK_GL( "Pre-GetRGBImage" );

	int nNumPixels = pInfo->m_iCurrentSizeX * pInfo->m_iCurrentSizeY;
	int nDataSize = nNumPixels * 3;
	if( nBufferSize < nDataSize )
		return 0;

	if( pInfo->m_bIsOffscreenBuffer == false )
	{
		glReadBuffer( GL_FRONT );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_RGB, GL_UNSIGNED_BYTE, pData );
	}
	else if( pInfo->m_nNumMultiSamples > 1 )
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, pInfo->m_nBlitFboId );
		glReadBuffer( GL_COLOR_ATTACHMENT0 );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_RGB, GL_UNSIGNED_BYTE, pData );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, pInfo->m_nFboId );
	}
	else
	{
		glReadBuffer( GL_COLOR_ATTACHMENT0 );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_RGB, GL_UNSIGNED_BYTE, pData );
	}

	PopWindow();

	DEBUG_CHECK_GL( "Post-GetRGBImage" );
	return nDataSize;
}

bool VistaGlutWindowingToolkit::GetDepthImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData ) const
{

	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) == false )
		return false;

	int nNumPixels = pInfo->m_iCurrentSizeX * pInfo->m_iCurrentSizeY;
	int nDataSize = nNumPixels * sizeof(float);
	vecData.resize( nDataSize );
	int nRet = GetDepthImage( pWindow, &vecData[0], nDataSize );
	return( nRet == nDataSize );	
}

int VistaGlutWindowingToolkit::GetDepthImage( const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) == false )
		return 0;

	int nNumPixels = pInfo->m_iCurrentSizeX * pInfo->m_iCurrentSizeY;
	int nDataSize = nNumPixels * sizeof(float);
	if( nBufferSize < nDataSize )
		return 0;
	DEBUG_CHECK_GL( "Pre-GetDepthImage" );

	if( pInfo->m_bIsOffscreenBuffer == false )
	{
		glReadBuffer( GL_FRONT );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, pData );		
	}
	else if( pInfo->m_nNumMultiSamples > 1 )
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, pInfo->m_nBlitFboId );
		glReadBuffer( GL_DEPTH_COMPONENT );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_RGB, GL_UNSIGNED_BYTE, pData );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, pInfo->m_nFboId );
	}
	else
	{
		glReadBuffer( GL_DEPTH_COMPONENT );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, pData );
	}

	PopWindow();

	DEBUG_CHECK_GL( "Post-GetDepthImage" );

	return nDataSize;
}


VistaImage VistaGlutWindowingToolkit::GetRGBImage( const VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) == false )
		return VistaImage();

	DEBUG_CHECK_GL( "Pre-GetRGBImage" );

	VistaImage oResult;

	if( pInfo->m_bIsOffscreenBuffer == false )
	{
		oResult.Set2DData( pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, NULL, GL_RGB, GL_UNSIGNED_BYTE );

		glReadBuffer( GL_BACK );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_RGB, GL_UNSIGNED_BYTE, oResult.GetDataWrite() );
	}
	else
	{		
		oResult.Set2DData( pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, NULL, GL_RGB, GL_UNSIGNED_BYTE );

		glReadBuffer( GL_COLOR_ATTACHMENT0 );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_RGB, GL_UNSIGNED_BYTE, oResult.GetDataWrite() );
	}

	PopWindow();

	DEBUG_CHECK_GL( "Post-GetRGBImage" );
	return oResult;
}

VistaImage VistaGlutWindowingToolkit::GetDepthImage( const VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	if( PushWindow( pInfo ) == false )
		return VistaImage();

	VistaImage oResult;

	DEBUG_CHECK_GL( "Pre-GetDepthImage" );

	if( pInfo->m_bIsOffscreenBuffer == false )
	{
		oResult.Set2DData( pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, NULL, GL_LUMINANCE, GL_FLOAT );
		
		glReadBuffer( GL_BACK );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, oResult.GetDataWrite() );		
	}
	else
	{
		oResult.Set2DData( pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, NULL, GL_LUMINANCE, GL_FLOAT );
		
		glReadBuffer( GL_FRONT );
		glReadPixels( 0, 0, pInfo->m_iCurrentSizeX, pInfo->m_iCurrentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, oResult.GetDataWrite() );
	}

	PopWindow();

	DEBUG_CHECK_GL( "Post-GetDepthImage" );

	return oResult;
}
int VistaGlutWindowingToolkit::GetWindowId( const VistaWindow* pWindow  ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
		return -1;
	return pInfo->m_iWindowID;	
}
void VistaGlutWindowingToolkit::BindWindow( VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	PushWindow( pInfo );
	if( pInfo->m_nNumMultiSamples == 1 )
		glDisable( GL_MULTISAMPLE );
	else
		glEnable( GL_MULTISAMPLE );
}
void VistaGlutWindowingToolkit::UnbindWindow( VistaWindow* pWindow )
{
	PopWindow();
}

bool VistaGlutWindowingToolkit::PushWindow( const VistaWindow* pWindow ) const
{
	WindowInfoMap::const_iterator itWindowInfo = m_mapWindowInfo.find( pWindow );
	assert( itWindowInfo != m_mapWindowInfo.end() );
	return PushWindow( (*itWindowInfo).second );	
}

bool VistaGlutWindowingToolkit::PushWindow( const GlutWindowInfo* pInfo ) const
{
	if( pInfo->m_bIsInitialized == false )
	{
		// okay, not initialized yet
		return false;
	} 

	if( pInfo->m_bIsOffscreenBuffer )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, pInfo->m_nFboId );
		m_iTmpWindowID = -1;
		return true;
	}
	else
	{
		int iID = pInfo->m_iWindowID;
		assert( iID != -1 );
		if( m_iTmpWindowID == -1 )
			m_iTmpWindowID = iID;
		glutSetWindow( iID );
	}
	return true;
}

void VistaGlutWindowingToolkit::PopWindow() const
{
	if( m_iTmpWindowID != -1 )
	{
		glutSetWindow( m_iTmpWindowID );
		m_iTmpWindowID = -1;
	}
	else
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

bool VistaGlutWindowingToolkit::GetVSyncCanBeModified( const VistaWindow* pWindow  )
{
	return CheckVSyncAvailability();
}

int VistaGlutWindowingToolkit::GetVSyncMode( const VistaWindow* pWindow  )
{
	if( CheckVSyncAvailability() == VSYNC_DISABLED )
		return VSYNC_STATE_UNAVAILABLE;
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( GetSwapIntervalFunction && pInfo->m_iVSyncMode == VSYNC_STATE_UNKNOWN )
	{
		if( PushWindow( pWindow ) )
		{			
			int iInterval = GetSwapIntervalFunction();
			if( iInterval == 0 )
				m_iGlobalVSyncAvailability = VSYNC_DISABLED;
			else if( iInterval >= 1 )
				m_iGlobalVSyncAvailability = VSYNC_ENABLED;

			PopWindow();
		}
	}
	return m_iGlobalVSyncAvailability;
}

bool VistaGlutWindowingToolkit::SetCursor( VistaWindow* pWindow, int iCursor )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsOffscreenBuffer )
	{
		return false;
	}


	pInfo->m_iCursor = iCursor;
	if( PushWindow( pWindow ) )
	{
		if( pInfo->m_bCursorEnabled )
			glutSetCursor( iCursor );
		PopWindow();
	}
	return true;
}

int VistaGlutWindowingToolkit::GetCursor( const VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	return pInfo->m_iCursor;
}

bool VistaGlutWindowingToolkit::SetVSyncMode( VistaWindow* pWindow, const bool bEnabled )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );

	if( pInfo->m_bIsInitialized == false )
	{
		// pre-init, just store the value
		if( bEnabled )
			pInfo->m_iVSyncMode = VSYNC_ENABLED;
		else
			pInfo->m_iVSyncMode = VSYNC_DISABLED;
		return true;
	}

	if( PushWindow( pWindow ) == false )
		return false;

	if( CheckVSyncAvailability() == false )
		return false;		

	int iInterval = bEnabled ? 1 : 0;	
#ifdef WIN32
	if( SetSwapIntervalFunction( iInterval ) )
	{
		if( GetSwapIntervalFunction )
		{
			int iGetValue = GetSwapIntervalFunction();
			if( iGetValue == iInterval )
				m_iGlobalVSyncAvailability = iInterval;
			else
			{
				vstr::errp() << "VistaGlutWindowingToolkit::SetVSyncEnabled -"
						<< "Setting VSync failed - does driver config enforce on/off?"
						<< std::endl;
				m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
			}
		}
		else
			m_iGlobalVSyncAvailability = iInterval;
	}
	else
	{
		vstr::errp() << "VistaGlutWindowingToolkit::SetVSyncEnabled -"
				<< "Setting VSync failed" << std::endl;
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	}
#elif defined LINUX
	if( SetSwapIntervalFunction( iInterval ) == 0 )
	{
		m_iGlobalVSyncAvailability = iInterval;
	}
	else
	{
		vstr::errp() << "VistaGlutWindowingToolkit::SetVSyncEnabled -"
			<< "Setting VSync failed - does driver config enforce on/off?"
			<< std::endl;
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	}
#endif

	PopWindow();
	return ( m_iGlobalVSyncAvailability != VSYNC_STATE_UNAVAILABLE );
}

bool VistaGlutWindowingToolkit::CheckVSyncAvailability()
{
	if( m_iGlobalVSyncAvailability != ~0 )
		return ( m_iGlobalVSyncAvailability != VSYNC_STATE_UNAVAILABLE );


#ifdef WIN32
	m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	SetSwapIntervalFunction = (PFNWGLSWAPINTERVALEXT)glutGetProcAddress( "wglSwapIntervalEXT" );
	if( SetSwapIntervalFunction )
	{
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNKNOWN;

		GetSwapIntervalFunction = (PFNWGLGETSWAPINTERVALEXT)glutGetProcAddress("wglGetSwapIntervalEXT");
	}
#elif defined LINUX
	SetSwapIntervalFunction = (PFNGLXSWAPINTERVALSGIPROC)glutGetProcAddress( "glXSwapIntervalSGI" );
	if( SetSwapIntervalFunction )
	{
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNKNOWN;
	}
	else
	{
		m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
	}

#else
	m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
#endif

	return ( m_iGlobalVSyncAvailability != VSYNC_STATE_UNAVAILABLE );

#endif
}

GlutWindowInfo* VistaGlutWindowingToolkit::GetWindowInfo( const VistaWindow* pWindow  ) const
{
	WindowInfoMap::const_iterator itWindow = m_mapWindowInfo.find( pWindow );
	if( itWindow == m_mapWindowInfo.end() )
		return NULL;
	return (*itWindow).second;
}

bool VistaGlutWindowingToolkit::GetContextVersion( int& nMajor, int& nMinor, const VistaWindow* pTarget ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pTarget );
	nMajor = pInfo->m_iContextMajor;
	nMinor = pInfo->m_iContextMinor;
	return true;
}

bool VistaGlutWindowingToolkit::SetContextVersion( int nMajor, int nMinor, VistaWindow* pTarget )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pTarget );
	
	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change context version on window ["
				<< pTarget->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}
	
	pInfo->m_iContextMajor = nMajor;
	pInfo->m_iContextMinor = nMinor;
	return true;
}

bool VistaGlutWindowingToolkit::GetIsDebugContext( const VistaWindow* pTarget ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pTarget );
	return pInfo->m_bIsDebugContext;
}

bool VistaGlutWindowingToolkit::SetIsDebugContext( const bool bIsDebug, VistaWindow* pTarget )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pTarget );
	
	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change debug context flag on window ["
				<< pTarget->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}
	
	pInfo->m_bIsDebugContext = bIsDebug;
	return true;
}

bool VistaGlutWindowingToolkit::GetIsForwardCompatible( const VistaWindow* pTarget ) const
{
	GlutWindowInfo* pInfo = GetWindowInfo( pTarget );
	return pInfo->m_bIsForwardCompatible;
}

bool VistaGlutWindowingToolkit::SetIsForwardCompatible( const bool bIsForwardCompatible, VistaWindow* pTarget )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pTarget );
	
	if( pInfo->m_bIsInitialized )
	{
		vstr::warnp() << "[GlutWindow]: Trying to change forward compatible flag on window ["
				<< pTarget->GetNameForNameable() << "] - this can only be done before initialization"
				<< std::endl;
		return false;
	}
	
	pInfo->m_bIsForwardCompatible = bIsForwardCompatible;
	return true;
}

bool VistaGlutWindowingToolkit::CreateDummyWindow( VistaWindow* pWindow )
{
	GlutWindowInfo* pInfo = GetWindowInfo( pWindow );
	int iDisplayMode = GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE;

	if( pInfo->m_iContextMajor != 1 || pInfo->m_iContextMinor != 0 )
	{
#if !HAS_FREEGLUT_28
		vstr::warnp() << "[GlutWindowingToolkit]: "
			<< "Context Version only available with freeglut 2.8+" << std::endl;
#else
		glutInitContextVersion(pInfo->m_iContextMajor, pInfo->m_iContextMinor);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
#endif
	}

	if( pInfo->m_bIsDebugContext || pInfo->m_bIsForwardCompatible )
	{
		int iContextFlags = 0;
#if !HAS_FREEGLUT_28
		vstr::warnp() << "[GlutWindowingToolkit]: "
			<< "Context Flags (DebugContext, ForwardCompatible) only available with freeglut 2.8+" << std::endl;
#else
		if( pInfo->m_bIsDebugContext )
		{
			iContextFlags |= GLUT_DEBUG;
		}
		if( pInfo->m_bIsForwardCompatible )
		{
			iContextFlags |= GLUT_FORWARD_COMPATIBLE;
		}

		glutInitContextFlags( iContextFlags );
#endif
	}


	glutInitDisplayMode( iDisplayMode );
	m_nDummyWindowId = glutCreateWindow( "dummy" );	
	glewInit();
	glutSetWindow( m_nDummyWindowId );
	glutHideWindow();

	return true;
}

void VistaGlutWindowingToolkit::DestroyDummyWindow()
{
	if( m_nDummyWindowId )
		glutDestroyWindow( m_nDummyWindowId );
	m_nDummyWindowId = -1;
}


