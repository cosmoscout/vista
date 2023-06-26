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

#ifndef _VISTADISPLAYBRIDGE_H
#define _VISTADISPLAYBRIDGE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaViewport.h> // for viewporttype enum

#include <iostream>
#include <list>
#include <vector>

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplayManager;
class IVistaDisplayEntityData;
class VistaDisplaySystem;
class VistaDisplay;
class VistaWindow;
class VistaViewport;
class VistaProjection;
class VistaPropertyList;
class VistaVirtualPlatform;
class IVistaObserveable;
class IVistaWindowingToolkit;

class Vista2DText;
class Vista2DBitmap;
class Vista2DLine;
class Vista2DRectangle;
class Vista2DDrawingObject;

class VistaGLTexture;
class VistaImage;

class IVistaSceneOverlay;
class IVistaExplicitCallbackInterface;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * IVistaDisplayBridge is ...
 */
class VISTAKERNELAPI IVistaDisplayBridge {
 public:
  virtual ~IVistaDisplayBridge();

  /*******************************************/
  /* Non-abstract functions                  */
  /*******************************************/

  /**
   * Update display systems. All necessary information is to be retrieved
   * from the registered display manager.
   */
  virtual bool UpdateDisplaySystems(VistaDisplayManager* pDisplayManager);

  /**
   * Methods for the creation of new display entities.
   */
  virtual VistaDisplaySystem* NewDisplaySystem(VistaVirtualPlatform* pReferenceFrame,
      IVistaDisplayEntityData* pData, VistaDisplayManager* pDisplayManager);
  virtual VistaDisplay*       NewDisplay(
            IVistaDisplayEntityData* pData, VistaDisplayManager* pDisplayManager);
  virtual VistaWindow*   NewWindow(VistaDisplay* pDisplay, IVistaDisplayEntityData* pData);
  virtual VistaViewport* NewViewport(
      VistaDisplaySystem* pDisplaySystem, VistaWindow* pWindow, IVistaDisplayEntityData* pData);
  virtual VistaProjection* NewProjection(VistaViewport* pViewport, IVistaDisplayEntityData* pData);

  /*******************************************/
  /* Update Methods                          */
  /*******************************************/

  /**
   * Render the current scene to all active windows
   */
  virtual bool DrawFrame() = 0;
  /**
   * Swap the Buffers of all active Windows, and displays them
   */
  virtual bool DisplayFrame() = 0;
  /**
   * Sets the callbacks that should be called for the main update loop.
   * Should be set by the VistaFrameLoop, but can be re-set after initialization,
   * but do so only! if you have a very good reason to do so (like defining your
   * own FrameLoop)
   */
  virtual bool SetDisplayUpdateCallback(IVistaExplicitCallbackInterface* pCallback) = 0;

  /*******************************************/
  /* DisplaySystem Methods                   */
  /*******************************************/
  virtual VistaDisplaySystem* CreateDisplaySystem(VistaVirtualPlatform* pReferenceFrame,
      VistaDisplayManager* pDisplayManager, const VistaPropertyList& refProps)            = 0;
  virtual bool                DestroyDisplaySystem(VistaDisplaySystem* pDisplaySystem)    = 0;
  virtual void DebugDisplaySystem(std::ostream& oStream, VistaDisplaySystem* pTarget)     = 0;
  virtual void GetViewerPosition(VistaVector3D& v3Pos, const VistaDisplaySystem* pTarget) = 0;
  virtual void SetViewerPosition(const VistaVector3D& v3Pos, VistaDisplaySystem* pTarget) = 0;
  virtual void GetViewerOrientation(
      VistaQuaternion& qOrient, const VistaDisplaySystem* pTarget) = 0;
  virtual void SetViewerOrientation(
      const VistaQuaternion& qOrient, VistaDisplaySystem* pTarget)            = 0;
  virtual void GetEyeOffsets(VistaVector3D& v3LeftOffset, VistaVector3D& v3RightOffset,
      const VistaDisplaySystem* pTarget)                                      = 0;
  virtual void SetEyeOffsets(const VistaVector3D& v3LeftOffset, const VistaVector3D& v3RightOffset,
      VistaDisplaySystem* pTarget)                                            = 0;
  virtual bool GetLocalViewer(const VistaDisplaySystem* pTarget)              = 0;
  virtual void SetLocalViewer(bool bLocalViewer, VistaDisplaySystem* pTarget) = 0;
  virtual bool GetHMDModeActive(const VistaDisplaySystem* pTarget)            = 0;
  virtual void SetHMDModeActive(bool bSet, VistaDisplaySystem* pTarget)       = 0;
  virtual void ObserverUpdate(
      IVistaObserveable* pObserveable, int nMessage, int nTicket, VistaDisplaySystem* pTarget);

  virtual VistaVector3D   GetViewerPosition(const VistaDisplaySystem* pTarget);
  virtual VistaQuaternion GetViewerOrientation(const VistaDisplaySystem* pTarget);

  /*******************************************/
  /* Display Methods                         */
  /*******************************************/

  virtual VistaDisplay* CreateDisplay(
      VistaDisplayManager* pDisplayManager, const VistaPropertyList& refProps)  = 0;
  virtual bool DestroyDisplay(VistaDisplay* pDisplay)                           = 0;
  virtual void DebugDisplay(std::ostream& oStream, const VistaDisplay* pTarget) = 0;

  /*******************************************/
  /* Window Management                       */
  /*******************************************/

  virtual IVistaWindowingToolkit* CreateWindowingToolkit(const std::string& sName) = 0;
  virtual IVistaWindowingToolkit* GetWindowingToolkit() const                      = 0;

  virtual VistaWindow* CreateVistaWindow(
      VistaDisplay* pDisplay, const VistaPropertyList& refProps)                               = 0;
  virtual bool DestroyVistaWindow(VistaWindow* pWindow)                                        = 0;
  virtual bool GetWindowStereo(const VistaWindow* pTarget)                                     = 0;
  virtual bool SetWindowStereo(bool bStereo, VistaWindow* pTarget)                             = 0;
  virtual bool GetWindowAccumBufferEnabled(const VistaWindow* pTarget)                         = 0;
  virtual bool SetWindowAccumBufferEnabled(bool bAccumBufferEnabled, VistaWindow* pTarget)     = 0;
  virtual bool GetWindowStencilBufferEnabled(const VistaWindow* pTarget)                       = 0;
  virtual bool SetWindowStencilBufferEnabled(bool bStencilBufferEnabled, VistaWindow* pTarget) = 0;
  virtual bool GetWindowDrawBorder(const VistaWindow* pTarget)                                 = 0;
  virtual bool SetWindowDrawBorder(bool bDrawBorder, VistaWindow* pTarget)                     = 0;
  virtual bool GetWindowPosition(int& nXPos, int& nYPos, const VistaWindow* pTarget)           = 0;
  virtual bool SetWindowPosition(int nXPos, int nYPos, VistaWindow* pTarget)                   = 0;
  virtual bool GetWindowSize(int& nWidth, int& nHeight, const VistaWindow* pTarget)            = 0;
  virtual bool SetWindowSize(int nWidth, int nHeight, VistaWindow* pTarget)                    = 0;
  virtual int  GetWindowVSync(const VistaWindow* pTarget)                                      = 0;
  virtual bool SetWindowVSync(bool bEnabled, VistaWindow* pTarget)                             = 0;
  virtual bool SetWindowFullScreen(bool bFullScreen, VistaWindow* pTarget)                     = 0;
  virtual bool GetWindowFullScreen(const VistaWindow* pTarget)                                 = 0;
  virtual bool GetWindowIsOffscreenBuffer(const VistaWindow* pWindow) const                    = 0;
  virtual bool SetWindowIsOffscreenBuffer(VistaWindow* pWindow, const bool bSet)               = 0;
  virtual int  GetWindowMultiSamples(const VistaWindow* pWindow) const                         = 0;
  virtual bool SetWindowMultiSamples(VistaWindow* pWindow, const int nNumSamples)              = 0;
  virtual bool GetWindowRGBImage(
      const VistaWindow* pWindow, std::vector<VistaType::byte>& vecData) const = 0;
  virtual int GetWindowRGBImage(
      const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize) const = 0;
  virtual bool GetWindowDepthImage(
      const VistaWindow* pWindow, std::vector<VistaType::byte>& vecData) const = 0;
  virtual int GetWindowDepthImage(
      const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize) const = 0;
  virtual VistaImage GetWindowRGBImage(const VistaWindow* pWindow) const               = 0;
  virtual VistaImage GetWindowDepthImage(const VistaWindow* pWindow) const             = 0;
  virtual bool       GetWindowContextVersion(
            int& nMajor, int& nMinor, const VistaWindow* pTarget) const              = 0;
  virtual bool SetWindowContextVersion(int nMajor, int nMinor, VistaWindow* pTarget) = 0;
  virtual bool GetWindowIsDebugContext(const VistaWindow* pTarget) const             = 0;
  virtual bool SetWindowIsDebugContext(const bool bIsDebug, VistaWindow* pTarget)    = 0;
  virtual bool GetWindowIsForwardCompatible(const VistaWindow* pTarget) const        = 0;
  virtual bool SetWindowIsForwardCompatible(
      const bool bIsForwardCompatible, VistaWindow* pTarget)                              = 0;
  virtual int         GetWindowId(const VistaWindow* pTarget)                             = 0;
  virtual std::string GetWindowTitle(const VistaWindow* pTarget)                          = 0;
  virtual bool        SetWindowTitle(const std::string& strTitle, VistaWindow* pTarget)   = 0;
  virtual void        DebugVistaWindow(std::ostream& oStream, const VistaWindow* pTarget) = 0;
  virtual bool        RenderWindow(VistaWindow* pWindow)                                  = 0;

  /*******************************************/
  /* Viewport Methods                        */
  /*******************************************/

  virtual VistaViewport* CreateViewport(VistaDisplaySystem* pDisplaySystem, VistaWindow* pWindow,
      const VistaPropertyList& refProps)                                                       = 0;
  virtual bool           DestroyViewport(VistaViewport* pViewport)                             = 0;
  virtual void GetViewportPosition(int& nXPos, int& nYPos, const VistaViewport* pTarget)       = 0;
  virtual void SetViewportPosition(int nXPos, int nYPos, VistaViewport* pTarget)               = 0;
  virtual void GetViewportSize(int& nWidth, int& nHeight, const VistaViewport* pTarget)        = 0;
  virtual void SetViewportSize(int nWidth, int nHeight, VistaViewport* pTarget)                = 0;
  virtual void GetViewportTextureSize(int& nWidth, int& nHeight, const VistaViewport* pTarget) = 0;
  virtual void SetViewportTextureSize(int nWidth, int nHeight, VistaViewport* pTarget)         = 0;
  virtual VistaViewport::VistaViewportProperties::ViewportType GetViewportType(
      const VistaViewport* pTarget) = 0;
  virtual bool SetViewportType(
      VistaViewport::VistaViewportProperties::ViewportType eType, VistaViewport* pTarget)       = 0;
  virtual int             GetViewportPriority(const VistaViewport* pTarget)                     = 0;
  virtual bool            SetViewportPriority(int nPriority, VistaViewport* pTarget)            = 0;
  virtual bool            GetViewportIsEnabled(const VistaViewport* pTarget)                    = 0;
  virtual bool            SetViewportIsEnabled(bool bSet, VistaViewport* pTarget)               = 0;
  virtual VistaGLTexture* GetViewportTextureTarget(const VistaViewport* pTarget) const          = 0;
  virtual VistaGLTexture* GetViewportSecondaryTextureTarget(const VistaViewport* pTarget) const = 0;
  virtual std::string     GetViewportPostProcessShaderFile(const VistaViewport* pTarget) const  = 0;
  virtual bool            SetViewportPostProcessShaderFile(
                 const std::string& sShader, VistaViewport* pTarget)                    = 0;
  virtual bool GetViewportHasPassiveBackground(const VistaViewport* pTarget)            = 0;
  virtual void SetViewportHasPassiveBackground(bool bSet, VistaViewport* pTarget)       = 0;
  virtual bool GetViewportResizeWithWindowResize(const VistaViewport* pTarget)          = 0;
  virtual void SetViewportResizeWithWindowResize(bool bSet, VistaViewport* pTarget)     = 0;
  virtual bool GetViewportRepositionWithWindowResize(const VistaViewport* pTarget)      = 0;
  virtual void SetViewportRepositionWithWindowResize(bool bSet, VistaViewport* pTarget) = 0;
  virtual void DebugViewport(std::ostream& oStream, const VistaViewport* pTarget)       = 0;
  virtual bool RenderViewport(VistaViewport* pViewport)                                 = 0;

  /*******************************************/
  /* Projection Methods                      */
  /*******************************************/

  virtual VistaProjection* CreateProjection(
      VistaViewport* pViewport, const VistaPropertyList& refProps) = 0;
  virtual bool DestroyProjection(VistaProjection* pProjection)     = 0;
  virtual void GetProjectionPlane(VistaVector3D& v3MidPoint, VistaVector3D& v3NormalVector,
      VistaVector3D& v3UpVector, const VistaProjection* pTarget)   = 0;
  virtual void SetProjectionPlane(const VistaVector3D& v3MidPoint,
      const VistaVector3D& v3NormalVector, const VistaVector3D& v3UpVector,
      VistaProjection* pTarget)                                    = 0;
  virtual void GetProjPlaneExtents(double& dLeft, double& dRight, double& dBottom, double& dTop,
      const VistaProjection* pTarget)                              = 0;
  virtual void SetProjPlaneExtents(
      double dLeft, double dRight, double dBottom, double dTop, VistaProjection* pTarget) = 0;
  virtual void GetProjClippingRange(
      double& dNear, double& dFar, const VistaProjection* pTarget)                       = 0;
  virtual void SetProjClippingRange(double dNear, double dFar, VistaProjection* pTarget) = 0;
  virtual int  GetProjStereoMode(const VistaProjection* pTarget)                         = 0;
  virtual void SetProjStereoMode(int iMode, VistaProjection* pTarget)                    = 0;
  virtual void DebugProjection(std::ostream& oStream, const VistaProjection* pTarget)    = 0;

  /*******************************************/
  /* Various functions                       */
  /*******************************************/

  virtual bool MakeScreenshot(const VistaWindow& pWindow, const std::string& strFilename) const = 0;

  virtual Vista2DText*      New2DText(const std::string& strWindowName = "")      = 0;
  virtual Vista2DBitmap*    New2DBitmap(const std::string& strWindowName = "")    = 0;
  virtual Vista2DLine*      New2DLine(const std::string& strWindowName = "")      = 0;
  virtual Vista2DRectangle* New2DRectangle(const std::string& strWindowName = "") = 0;

  virtual bool Get2DOverlay(
      const std::string& strWindowName, std::list<Vista2DDrawingObject*>&) = 0;

  virtual bool AddSceneOverlay(IVistaSceneOverlay* pDraw, VistaViewport* pViewport) = 0;
  virtual bool RemSceneOverlay(IVistaSceneOverlay* pDraw, VistaViewport* pViewport) = 0;

  /**
   * returns an RBG or RGBA image with 8 bits per channel
   * when you no longer need the raw-buffer, use free to clear up the space
   */
  virtual bool DoLoadBitmap(const std::string& strNewFName, VistaType::byte** pBitmapData,
      int& nWidth, int& nHeight, bool& bAlpha) = 0;

  virtual bool Delete2DDrawingObject(Vista2DDrawingObject* p2DObject) = 0;

  virtual bool GetShowCursor() const           = 0;
  virtual void SetShowCursor(bool bShowCursor) = 0;

  virtual const VistaDisplayManager::RenderInfo* GetCurrentRenderInfo() const = 0;

 protected:
  IVistaDisplayBridge();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTADISPLAYBRIDGE_H)
