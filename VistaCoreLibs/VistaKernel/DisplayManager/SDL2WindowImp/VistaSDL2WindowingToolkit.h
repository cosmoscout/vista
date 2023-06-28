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


#ifndef _VISTASDLWINDOWINGTOOLKIT_H
#define _VISTASDLWINDOWINGTOOLKIT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>

#include <string>
#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaGLTexture;
class VistaImage;
struct SDL2WindowInfo;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * SDL2 implementation of IVistaWindowingToolkit. See IVistaWindowingToolkit.h
 * for documentation.
 */
class VISTAKERNELAPI VistaSDL2WindowingToolkit : public IVistaWindowingToolkit
{
public:
	VistaSDL2WindowingToolkit();
	~VistaSDL2WindowingToolkit();

	virtual void Run();
	virtual void Quit();

	virtual void DisplayWindow( const VistaWindow* pWindow );
	virtual void DisplayAllWindows();

	virtual bool RegisterWindow( VistaWindow* pWindow );
	virtual bool UnregisterWindow( VistaWindow* pWindow );

	virtual bool InitWindow( VistaWindow* pWindow );	

	virtual bool SetWindowUpdateCallback( IVistaExplicitCallbackInterface* pCallback );

	virtual bool GetWindowPosition( const VistaWindow* pWindow, int &iX, int& iY ) const;
	virtual bool SetWindowPosition( VistaWindow* pWindow, const int iX , const int iY );
	virtual bool GetWindowSize( const VistaWindow* pWindow, int& iWidth, int& iHeight ) const;
	virtual bool SetWindowSize( VistaWindow* pWindow, const int iWidth, const int iHeight );


	virtual bool GetFullscreen( const VistaWindow* pWindow  ) const;
	virtual bool SetFullscreen( VistaWindow* pWindow, const bool bSet );	
	
	virtual std::string GetWindowTitle( const VistaWindow* pWindow ) const;
	virtual bool SetWindowTitle( VistaWindow* pWindow, const std::string& sTitle );
	
	virtual bool GetCursorIsEnabled( const VistaWindow* pWindow ) const;
	virtual bool SetCursorIsEnabled( VistaWindow* pWindow, bool bSet );

	virtual bool GetUseStereo( const VistaWindow* pWindow ) const;
	virtual bool SetUseStereo( VistaWindow* pWindow, const bool bSet );

	virtual bool GetUseAccumBuffer( const VistaWindow* pWindow ) const;
	virtual bool SetUseAccumBuffer( VistaWindow* pWindow, const bool bSet );

	virtual bool GetUseStencilBuffer( const VistaWindow* pWindow ) const;
	virtual bool SetUseStencilBuffer( VistaWindow* pWindow, const bool bSet );

	virtual int GetMultiSamples( const VistaWindow* pWindow ) const;
	virtual bool SetMultiSamples( const VistaWindow* pWindow, const int nNumSamples );

	virtual bool GetDrawBorder( const VistaWindow* pWindow  ) const;
	virtual bool SetDrawBorder( VistaWindow* pWindow, const bool bSet );

	virtual bool GetUseOffscreenBuffer( const VistaWindow* pWindow  ) const;
	virtual bool SetUseOffscreenBuffer( VistaWindow* pWindow, const bool bSet );
	
	virtual bool GetContextVersion( int& nMajor, int& nMinor, const VistaWindow* pTarget ) const;
	virtual bool SetContextVersion( int nMajor, int nMinor, VistaWindow* pTarget );
	virtual bool GetIsDebugContext( const VistaWindow* pTarget ) const;
	virtual bool SetIsDebugContext( const bool bIsDebug, VistaWindow* pTarget );
	virtual bool GetIsForwardCompatible( const VistaWindow* pTarget ) const;
	virtual bool SetIsForwardCompatible( const bool bIsForwardCompatible, VistaWindow* pTarget );

	virtual int GetRGBImage( const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize ) const;
	virtual bool GetRGBImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData ) const;
	virtual int GetDepthImage( const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize ) const;
	virtual bool GetDepthImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData ) const;
	virtual VistaImage GetRGBImage( const VistaWindow* pWindow );
	virtual VistaImage GetDepthImage( const VistaWindow* pWindow );

	enum
	{
		VSYNC_STATE_UNAVAILABLE = -2,
		VSYNC_STATE_UNKNOWN = -1,
		VSYNC_DISABLED = 0,
		VSYNC_ENABLED = 1
	};
	virtual bool GetVSyncCanBeModified( const VistaWindow* pWindow  );
	virtual bool SetVSyncMode( VistaWindow* pWindow, const bool bEnabled );
	virtual int GetVSyncMode( const VistaWindow* pWindow  );

	virtual bool SetCursor( VistaWindow* pWindow, int iCursor );
	virtual int GetCursor( const VistaWindow* pWindow );
	
	virtual IVistaTextEntity* CreateTextEntity();

	virtual int GetWindowId( const VistaWindow* pWindow  ) const;	

	virtual void BindWindow( VistaWindow* pWindow );
	virtual void UnbindWindow( VistaWindow* pWindow );

protected:
	bool PushWindow( const VistaWindow* pWindow ) const;
	bool PushWindow( const SDL2WindowInfo* pInfo ) const;
	void PopWindow() const;
	bool CheckVSyncAvailability();

	SDL2WindowInfo* GetWindowInfo( const VistaWindow* pWindow  ) const;

	bool InitAsNormalWindow( VistaWindow* pWindow );
	bool InitAsFbo( VistaWindow* pWindow );
	bool InitAsMultisampleFbo( VistaWindow* pWindow );

	bool CreateDummyWindow( VistaWindow* pWindow );
	void DestroyDummyWindow();

private:
	typedef std::map<const VistaWindow*, SDL2WindowInfo*>	WindowInfoMap;
	WindowInfoMap						m_mapWindowInfo;
	bool								m_bQuitLoop;
	IVistaExplicitCallbackInterface*	m_pUpdateCallback;
	mutable int							m_iTmpWindowID;
	int									m_iGlobalVSyncAvailability;

	bool								m_bHasFullWindow;
	int									m_nFullWindowId;
	int									m_nDummyWindowId;

	int									m_iCursor;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
#endif
