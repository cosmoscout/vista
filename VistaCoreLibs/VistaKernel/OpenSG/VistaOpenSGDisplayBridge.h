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


#ifndef _VISTAOPENSGDISPLAYBRIDGE_H
#define _VISTAOPENSGDISPLAYBRIDGE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/DisplayManager/VistaDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaDisplayEntity.h>
#include <VistaKernel/DisplayManager/VistaProjection.h>

/** @todo move 2d draw obj to dispmgr folder */
#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>

#include <VistaKernel/OpenSG/VistaOpenSGTextForeground.h>
#include <VistaKernel/OpenSG/VistaOpenSGGLOverlayForeground.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <memory>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGPassiveWindow.h>
#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGViewport.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGComponentTransform.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGProjectionCameraDecorator.h>
#include <OpenSG/OSGPerspectiveCamera.h>
#include <OpenSG/OSGImageForeground.h>
#include <OpenSG/OSGFileGrabForeground.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGSolidBackground.h>
#include <OpenSG/OSGPassiveBackground.h>
#include <OpenSG/OSGSHLChunk.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaOpenSGNodeData;
class VistaDisplayManager;
class IVistaWindowingToolkit;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaOpenSGDisplayBridge : public IVistaDisplayBridge
{
public:
	// forward declarations of local classes
	class DisplaySystemData;
	class DisplayData;
	class WindowData;
	class ViewportData;
	class ProjectionData;

	class VistaOpenSG2DBitmap;

	// CONSTRUCTOR / DESTRUCTOR
	VistaOpenSGDisplayBridge( osg::RenderAction* pRenderAction,
								osg::NodePtr pRealRoot );
	virtual ~VistaOpenSGDisplayBridge();

	osg::RenderAction* GetRenderAction() const;

	void OnWindowSizeUpdate( VistaWindow *pTarget );

	void SetDisplayManager(VistaDisplayManager *);
	VistaDisplayManager *GetDisplayManager() const;
	
	void UpdateDisplaySystem( VistaDisplaySystem* pDisplaySystem  );



	// ###############################################################
	// DISPLAYBRIDGE interface Implementation
	// ###############################################################

	/**
	 * Update display systems. All necessary information is to be retrieved
	 * from the registered display manager.
	 */
	virtual bool UpdateDisplaySystems( VistaDisplayManager* pDisplayManager );

	/**
	 * Render the current scene to all active windows
	 */
	virtual bool DrawFrame();
	/**
	 * Swap the Buffers of all active Windows, and displays them
	 */
	virtual bool DisplayFrame();
	/**
	 * Sets the callbacks that should be called for the main update loop.
	 * Should be set by the VistaFrameLoop, but can be re-set after initialization,
	 * but do so only! if you have a very good reason to do so (like defining your
	 * own FrameLoop)
	 */
	virtual bool SetDisplayUpdateCallback( IVistaExplicitCallbackInterface* pCallback );	

	/**
	 * Methods for display system management.
	 */
	virtual VistaDisplaySystem* CreateDisplaySystem( VistaVirtualPlatform* pReferenceFrame,
									VistaDisplayManager* pDisplayManager,
									const VistaPropertyList& refProps );
	virtual bool DestroyDisplaySystem( VistaDisplaySystem* pDisplaySystem );
	virtual void DebugDisplaySystem( std::ostream& oStream,
									VistaDisplaySystem* pTarget );
	virtual void SetViewerPosition( const VistaVector3D& v3Pos,
									VistaDisplaySystem* pTarget );
	virtual void GetViewerPosition( VistaVector3D& v3Pos,
									const VistaDisplaySystem* pTarget );
	virtual void SetViewerOrientation( const VistaQuaternion& qOrient,
									VistaDisplaySystem* pTarget );
	virtual void GetViewerOrientation( VistaQuaternion& qOrient,
									const VistaDisplaySystem* pTarget );
	virtual void SetEyeOffsets( const VistaVector3D& v3LeftOffset,
									const VistaVector3D& v3RightOffset,
									VistaDisplaySystem* pTarget );
	virtual void GetEyeOffsets( VistaVector3D& v3LeftOffset,
									VistaVector3D& v3RightOffset,
									const VistaDisplaySystem* pTarget );
	virtual void SetLocalViewer( bool bLocalViewer,
									VistaDisplaySystem* pTarget );
	virtual bool GetLocalViewer( const VistaDisplaySystem* pTarget );
	virtual void SetHMDModeActive( bool bSet, VistaDisplaySystem* pTarget );
	virtual bool GetHMDModeActive( const VistaDisplaySystem* pTarget );
	virtual void ObserverUpdate( IVistaObserveable* pObserveable,
									int nMessage,
									int nTicket,
									VistaDisplaySystem* pTarget );

	/**
	 * Methods for display management.
	 */
	virtual VistaDisplay* CreateDisplay( VistaDisplayManager* pDisplayManager,
										 const VistaPropertyList& refProps );
	virtual bool DestroyDisplay( VistaDisplay* pDisplay );
	virtual void DebugDisplay( std::ostream& oStream, const VistaDisplay* pTarget );

	/**
	 * Methods for window management.
	 */
	virtual IVistaWindowingToolkit* CreateWindowingToolkit( const std::string& sName );
	virtual IVistaWindowingToolkit* GetWindowingToolkit() const;

	virtual VistaWindow* CreateVistaWindow( VistaDisplay* pDisplay,
									const VistaPropertyList& refProps );
	virtual bool DestroyVistaWindow( VistaWindow* pWindow );
	virtual bool SetWindowStereo( bool bStereo,
									VistaWindow* pTarget);
	virtual bool GetWindowStereo( const VistaWindow* pTarget );
	virtual bool SetWindowAccumBufferEnabled( bool bAccumBufferEnabled,
									VistaWindow* pTarget );
	virtual bool GetWindowAccumBufferEnabled( const VistaWindow*  pTarget );
	virtual bool SetWindowStencilBufferEnabled( bool bStencilBufferEnabled,
									VistaWindow* pTarget );
	virtual bool GetWindowStencilBufferEnabled( const VistaWindow* pTarget );
	virtual bool GetWindowDrawBorder( const VistaWindow* pTarget );
	virtual bool SetWindowDrawBorder( bool bDrawBorder,
									VistaWindow* pTarget );
	virtual bool SetWindowPosition( int nXPos, int nYPos,
									VistaWindow* pTarget);
	virtual bool GetWindowPosition( int& nXPos, int& nYPos,
									const VistaWindow* pTarget );
	virtual bool SetWindowSize( int nWidth, int nHeight,
									VistaWindow* pTarget);
	virtual bool GetWindowSize( int& nWidth, int& nHeight,
									const VistaWindow* pTarget);
	virtual bool SetWindowVSync( bool bEnabled,
									VistaWindow* pTarget );
	virtual int GetWindowVSync( const VistaWindow* pTarget );
	virtual bool SetWindowFullScreen( bool bFullScreen,
									VistaWindow* pTarget);
	virtual bool GetWindowFullScreen( const VistaWindow* pTarget );
	virtual bool GetWindowIsOffscreenBuffer( const VistaWindow* pWindow ) const;
	virtual bool SetWindowIsOffscreenBuffer( VistaWindow* pWindow, const bool bSet );
	virtual int GetWindowMultiSamples( const VistaWindow* pWindow ) const;
	virtual bool SetWindowMultiSamples( VistaWindow* pWindow, const int nNumSamples );
	virtual bool GetWindowRGBImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData ) const;
	virtual int GetWindowRGBImage( const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize ) const;
	virtual bool GetWindowDepthImage( const VistaWindow* pWindow, std::vector< VistaType::byte >& vecData ) const;
	virtual int GetWindowDepthImage( const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize ) const;
	virtual VistaImage GetWindowRGBImage( const VistaWindow* pWindow ) const;
	virtual VistaImage GetWindowDepthImage( const VistaWindow* pWindow ) const;
	virtual bool GetWindowContextVersion( int& nMajor, int& nMinor,
										const VistaWindow* pTarget) const;
	virtual bool SetWindowContextVersion( int nMajor, int nMinor,
										VistaWindow* pTarget);
	virtual bool GetWindowIsDebugContext( const VistaWindow* pTarget ) const;
	virtual bool SetWindowIsDebugContext( const bool bIsDebug,
										VistaWindow* pTarget );
	virtual bool GetWindowIsForwardCompatible( const VistaWindow* pTarget ) const;
	virtual bool SetWindowIsForwardCompatible( const bool bIsForwardCompatible,
										VistaWindow* pTarget );
	virtual int GetWindowId( const VistaWindow* pTarget );
	virtual bool SetWindowTitle( const std::string& strTitle,
									VistaWindow* pTarget );
	virtual std::string GetWindowTitle( const VistaWindow* pTarget );
	virtual void DebugVistaWindow( std::ostream& oStream,
									const VistaWindow* pTarget );
	virtual bool RenderWindow( VistaWindow* pWindow );

	/**
	 * Methods for viewport management.
	 */
	virtual VistaViewport* CreateViewport( VistaDisplaySystem* pDisplaySystem,
									VistaWindow* pWindow,
									const VistaPropertyList& refProps );
	virtual bool DestroyViewport( VistaViewport* pViewport );
	virtual void GetViewportPosition( int& nXPos, int& nYPos, const VistaViewport* pTarget);
	virtual void SetViewportPosition( int nXPos, int nYPos, VistaViewport* pTarget);
	virtual void GetViewportSize(int& nWidth, int& nHeight, const VistaViewport* pTarget);
	virtual void SetViewportSize(int nWidth, int nHeight, VistaViewport* pTarget);
	virtual void GetViewportTextureSize(int& nWidth, int& nHeight, const VistaViewport* pTarget);
	virtual void SetViewportTextureSize(int nWidth, int nHeight, VistaViewport* pTarget);
	virtual VistaViewport::VistaViewportProperties::ViewportType GetViewportType( const VistaViewport* pTarget );
	virtual bool SetViewportType( VistaViewport::VistaViewportProperties::ViewportType eType, VistaViewport* pTarget );
	virtual int GetViewportPriority( const VistaViewport* pTarget );
	virtual bool SetViewportPriority( int nPriority, VistaViewport* pTarget );
	virtual bool GetViewportIsEnabled( const VistaViewport* pTarget );
	virtual bool SetViewportIsEnabled( bool bSet, VistaViewport* pTarget );
	virtual VistaGLTexture* GetViewportTextureTarget( const VistaViewport* pTarget ) const;
	virtual VistaGLTexture* GetViewportSecondaryTextureTarget( const VistaViewport* pTarget ) const;
	virtual std::string GetViewportPostProcessShaderFile( const VistaViewport* pTarget ) const;
	virtual bool SetViewportPostProcessShaderFile( const std::string& sShader, VistaViewport* pTarget );
	virtual bool GetViewportHasPassiveBackground( const VistaViewport* pTarget );
	virtual void SetViewportHasPassiveBackground( bool bSet, VistaViewport* pTarget );
	virtual bool GetViewportResizeWithWindowResize( const VistaViewport* pTarget );
	virtual void SetViewportResizeWithWindowResize( bool bSet, VistaViewport* pTarget );
	virtual bool GetViewportRepositionWithWindowResize( const VistaViewport* pTarget );
	virtual void SetViewportRepositionWithWindowResize( bool bSet, VistaViewport* pTarget );
	virtual void DebugViewport( std::ostream& oStream, const VistaViewport* pTarget );
	virtual bool RenderViewport( VistaViewport* pViewport );

	/**
	 * Methods for projection management.
	 */
	virtual VistaProjection* CreateProjection( VistaViewport* pViewport,
									const VistaPropertyList& refProps );
	virtual bool DestroyProjection( VistaProjection* pProjection );
	virtual void SetProjectionPlane( const VistaVector3D& v3MidPoint,
									const VistaVector3D& v3NormalVector,
									const VistaVector3D& v3UpVector,
									VistaProjection* pTarget );
	virtual void GetProjectionPlane( VistaVector3D& v3MidPoint,
									VistaVector3D& v3NormalVector,
									VistaVector3D& v3UpVector,
									const VistaProjection* pTarget );
	virtual void SetProjPlaneExtents( double dLeft, double dRight, double dBottom, double dTop,
									VistaProjection* pTarget );
	virtual void GetProjPlaneExtents( double& dLeft, double& dRight, double& dBottom, double& dTop,
									const VistaProjection* pTarget );
	virtual void SetProjClippingRange( double dNear, double dFar,
									VistaProjection* pTarget );
	virtual void GetProjClippingRange( double& dNear, double& dFar,
									const VistaProjection* pTarget );
	virtual void SetProjStereoMode( int iMode, VistaProjection* pTarget );
	virtual int  GetProjStereoMode( const VistaProjection* pTarget);
	virtual void DebugProjection( std::ostream& oStream, const VistaProjection* pTarget );

	
	virtual bool MakeScreenshot( const VistaWindow& pWindow,
									const std::string& strFilename ) const;

	virtual Vista2DText* New2DText( const std::string& strWindowName = "" );
	virtual Vista2DBitmap* New2DBitmap( const std::string& strWindowName = "" );
	virtual Vista2DLine* New2DLine( const std::string& strWindowName = "" );
	virtual Vista2DRectangle* New2DRectangle( const std::string& strWindowName = "" );

	virtual bool Get2DOverlay( const std::string& strWindowName,
							  std::list<Vista2DDrawingObject*>& );

	virtual bool AddSceneOverlay( IVistaSceneOverlay* pDraw,
									VistaViewport* pViewport );
	virtual bool RemSceneOverlay( IVistaSceneOverlay* pDraw,
									VistaViewport* pViewport );

	/// returns an RBG or RGBA image with 8 bits per channel
	/// when you no longer need the raw-buffer, use free to clear up the space
	virtual bool DoLoadBitmap( const std::string& strNewFName,
							  VistaType::byte** pBitmapData,
							  int& nWidth,
							  int& nHeight,
							  bool& bAlpha );

	virtual bool Delete2DDrawingObject( Vista2DDrawingObject* p2DObject );

	virtual bool GetShowCursor() const;
	virtual void SetShowCursor( bool bShowCursor );

	virtual const VistaDisplayManager::RenderInfo* GetCurrentRenderInfo() const;

	// ###############################################################
	// LOCAL CLASSES
	// ###############################################################

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CDisplaySystemData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI DisplaySystemData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		DisplaySystemData();
		virtual ~DisplaySystemData();

		VistaVector3D   GetLeftEyeOffset()     const;
		VistaVector3D   GetRightEyeOffset()    const;

		void SetPlatformTransformation( const VistaTransformMatrix& matrix );

		void SetPlatformTranslation( const VistaVector3D& v3Pos );
		VistaVector3D GetPlatformTranslation() const;

		void SetPlatformOrientation( const VistaQuaternion& qOri );
		VistaQuaternion GetPlatformOrientation() const;

		void SetCameraPlatformTransformation( const VistaTransformMatrix& matrix );
		VistaTransformMatrix GetCameraPlatformTransformation() const;

		void SetCameraPlatformTranslation( const VistaVector3D& v3Pos );
		VistaVector3D GetCameraPlatformTranslation() const;

		void SetCameraPlatformOrientation( const VistaQuaternion& qOri );
		VistaQuaternion GetCameraPlatformOrientation() const;


		void SetHMDModeActive( const bool bSet );
		bool GetHMDModeActive() const;

	private:
		bool                m_bLocalViewer;
		VistaVector3D		m_v3InitialViewerPosition;
		VistaQuaternion    m_qInitialViewerOrientation;
		VistaVector3D      m_v3LeftEyeOffset;
		VistaVector3D      m_v3RightEyeOffset;
		osg::NodeRefPtr	    m_ptrPlatformBeacon;
		osg::NodeRefPtr		m_ptrCameraBeacon;
		bool				m_bHMDModeActive;
	};

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CDisplayData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI DisplayData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		DisplayData();
		DisplayData( const std::string& sDisplayName );

		std::string GetDisplayName() const;
	private:
		std::string m_sDisplayName;
	};

 	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CWindowData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI WindowData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		WindowData();
		virtual ~WindowData();

		osg::WindowPtr GetOpenSGWindow() const;
		void ObserveWindow( VistaWindow* pWindow, VistaOpenSGDisplayBridge* pBridge );

	private:
		class WindowObserver;
		WindowObserver*	m_pObserver;
		osg::PassiveWindowRefPtr  m_ptrWindow;
	};


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CViewportData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI ViewportData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:
		ViewportData();
		~ViewportData();

		bool GetStereo() const;

		osg::ViewportPtr GetOpenSGViewport() const;
		osg::ViewportPtr GetOpenSGRightViewport() const;
		osg::VistaOpenSGTextForegroundPtr GetTextForeground() const;
		osg::VistaOpenSGGLOverlayForegroundPtr GetOverlayForeground() const;
		osg::SolidBackgroundPtr GetSolidBackground() const;
		osg::PassiveBackgroundPtr GetPassiveBackground() const;

		bool GetIsEnabled() const { return m_bIsEnabled; }
		void SetIsEnabled( const bool oValue ) { m_bIsEnabled = oValue; } 
		int GetPriority() const { return m_nPriority; }
		void SetPriority( const int oValue ) { m_nPriority = oValue; }
		VistaViewport::VistaViewportProperties::ViewportType GetType() const { return m_eType; }
		void SetType( const VistaViewport::VistaViewportProperties::ViewportType oValue ) { m_eType = oValue; }
		VistaGLTexture* GetMainTextureTarget() const;
		VistaGLTexture* GetSecondaryTextureTarget() const;

		void SetupRenderTargets( const int nWidth, const int nHeight );
		void ClearRenderTargets();
		void UpdateRenderTargetSize( const int nWidth, const int nHeight );
		void BindRenderTargets( const int nIndex );
		void UnbindRenderTargets( const int nIndex );
		void DrawViewportQuad( const bool bSplit = false, const bool bSplitIsLeft = false );

		/**
		 * @todo think about a concept to wrap this in the DisplayManager layer
		 */
		void ReplaceViewport(osg::ViewportPtr pNewPort, bool bCopyData = false, bool bRight = false);
		void OnWindowResize( VistaViewport* pViewport, const int nNewWindowWidth, const int nNewWindowHeight );
	private:
		bool             m_bStereo; // just in case someone asks ;-)
		bool             m_bAccumBufferEnabled;
		bool             m_bStencilBufferEnabled;
		bool             m_bHasPassiveBackground;
		bool             m_bResizeWithWindow;
		bool             m_bRepositionWithWindow;
		bool             m_bUseRenderToTexture;
		bool             m_bIsEnabled;
		int              m_nPriority;
		VistaViewport::VistaViewportProperties::ViewportType m_eType;
		osg::ViewportRefPtr m_Viewport;
		osg::ViewportRefPtr m_RightViewport;
		osg::VistaOpenSGTextForegroundRefPtr m_TextForeground;
		osg::VistaOpenSGGLOverlayForegroundRefPtr m_pOverlays;
		osg::ImageForegroundRefPtr m_oBitmaps;
		osg::SolidBackgroundRefPtr m_pSolidBackground;
		osg::PassiveBackgroundRefPtr m_pPassiveBackground;
		// FBO id @IMGTODO: use glext buffer object
		GLuint m_nFBOId[2];
		GLuint m_nDepthBufferId[2];
		bool m_bUseTextureSwapping;
		int m_nRTTViewportSizePosX;
		int m_nRTTViewportSizePosY;
		bool m_bTextureSizeSetExplicitely;
		int m_nRTTViewportSizeWidth;
		int m_nRTTViewportSizeHeight;
		VistaGLTexture* m_pReadTextureTarget[2];
		VistaGLTexture* m_pWriteTextureTarget[2];
		std::string m_sShaderFilename;
		osg::SHLChunkRefPtr m_pPostProcessShader;
		osg::SHLChunkRefPtr m_pPostProcessShaderAlt;
		GLenum m_nMinFilterState;
		GLenum m_nMagFilterState;
		float m_fRelativeViewportToWindowWidth;
		float m_fRelativeViewportToWindowHeight;
		float m_fRelativeViewportToWindowPosX;
		float m_fRelativeViewportToWindowPosY;
		bool m_bIsPerformingWindowResizeUpdate;

		std::list<Vista2DDrawingObject*> m_liOverlays;
	};


	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CProjectionData
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI ProjectionData : public IVistaDisplayEntityData
	{
		friend class VistaOpenSGDisplayBridge;
	public:

		ProjectionData();
		virtual ~ProjectionData();

		VistaVector3D GetLeftEye()      const;
		VistaVector3D GetRightEye()     const;
		VistaVector3D GetMidPoint()     const;
		VistaVector3D GetNormalVector() const;
		VistaVector3D GetUpVector()     const;
		double         GetLeft()         const;
		double         GetRight()        const;
		double         GetBottom()       const;
		double         GetTop()          const;
		int            GetStereoMode()   const;

		void SetCameraTransformation( const VistaVector3D& v3CamPos,
									  const VistaQuaternion& qCamOri );

		void GetCameraTransformation( VistaVector3D& v3CamPos,
									  VistaQuaternion& qCamOri ) const;
		void SetCameraTranslation( const VistaVector3D& v3CamPos );
		VistaVector3D GetCameraTranslation() const;
		void SetCameraOrientation( const VistaQuaternion& qCamOri );
		VistaQuaternion GetCameraOrientation() const;

		void SetProjectionPlane();
		void SetEyes( const VistaVector3D& v3LeftOffset, const VistaVector3D& v3RightOffset );

	private:
		osg::NodeRefPtr                      m_ptrBeacon;
		osg::NodeRefPtr                      m_ptrEyeBeacon;
		osg::NodeRefPtr                      m_ptrRightEyeBeacon;
		osg::ProjectionCameraDecoratorRefPtr m_ptrCamDeco;
		osg::ProjectionCameraDecoratorRefPtr m_ptrRightCamDeco;
		osg::PerspectiveCameraRefPtr         m_ptrPerspCam;
		osg::PerspectiveCameraRefPtr         m_ptrRightPerspCam;

		VistaVector3D m_v3LeftEye, m_v3RightEye; // left and right eye...
		VistaVector3D m_v3MidPoint, m_v3NormalVector, m_v3UpVector;
		double         m_dLeft, m_dRight, m_dBottom, m_dTop;

		int m_iStereoMode;
	};

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// VistaOpenSG2DBitmap
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VISTAKERNELAPI VistaOpenSG2DBitmap : public Vista2DBitmap
	{
	public:

		VistaOpenSG2DBitmap(IVistaDisplayBridge *pBr,
			const std::string &sVpName);

		~VistaOpenSG2DBitmap();

		// ###############################################
		// OVERLOADED BITMAP2D API
		// ###############################################

		bool SetBitmap(const std::string &strNewFName);
		bool GetDimensions(int& nWidth, int& nHeight);
		bool SetPosition(float fPosX, float fPosY);

		osg::ImageRefPtr m_oImage;
		std::string m_sVpName;
	};


	bool ReplaceForegroundImage(const std::string &sVpName,
								osg::ImagePtr oOld,
								osg::ImagePtr oNew,
								float xPos, float yPos);


private:

	static void ReshapeFunction(int width, int height);
	void FillRenderInfo( VistaViewport* pViewport,
						osg::ViewportPtr pOsgViewport ) const;
	void FillRenderInfo( VistaViewport* pViewport,
						osg::ViewportPtr pOsgViewport,
						const VistaDisplayManager::RenderInfo::EyeRenderMode eMode ) const;

	bool m_bShowCursor;

	osg::RenderAction* m_pRenderAction;
	std::unique_ptr< VistaDisplayManager::RenderInfo > m_pRenderInfo;

	osg::NodeRefPtr m_pRealRoot;
	IVistaWindowingToolkit* m_pWindowingToolkit;
	VistaDisplayManager *m_pDisplayManager;
	mutable std::vector<WindowData*> m_vecFileGrabWindows;

	bool m_bOculusOffsetPositionRequiresInitialization;
	bool m_bOculusOffsetOrientationRequiresInitialization;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAOPENSGDISPLAYBRIDGE_H)

