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

#ifndef _VISTAWINDOW_H
#define _VISTAWINDOW_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaDisplayEntity.h"

#include <vector>

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplay;
class VistaViewport;
class IVistaDisplayBridge;
class VistaGLTexture;
class VistaImage;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaWindow is ...
 */
class VISTAKERNELAPI VistaWindow : public VistaDisplayEntity {
  friend class IVistaDisplayBridge;
  friend class VistaDisplayManager;

 public:
  virtual ~VistaWindow();

  VistaDisplay* GetDisplay() const;
  std::string   GetDisplayName() const;

  unsigned int           GetNumberOfViewports() const;
  std::list<std::string> GetViewportNames() const;

  VistaViewport*               GetViewport(unsigned int iIndex) const;
  std::vector<VistaViewport*>& GetViewports();

  bool Render();

  virtual void Debug(std::ostream& out) const;

  VistaImage ReadRGBImage() const;
  VistaImage ReadDepthImage() const;
  // @IMGTODO: VistaGLTexture Attach for RTT

  /**
   * Reads the RGB data from the current framebuffer, and writes
   * the data into vecData. Format is three bytes per pixel, one
   * each for R, G, and B.
   * Takes a buffer with a specified size, and returns the number of
   * bytes that were written (0 if failed or buffer too small)
   */
  int  ReadRGBImage(VistaType::byte* pData, const int nBufferSize) const;
  bool ReadRGBImage(std::vector<VistaType::byte>& vecData) const;

  /**
   * Reads the Depth data from the current framebuffer, and writes
   * the data into vecData. Format is four bytes per pixel, representing
   * a float describing the pixels depth.
   * Takes a buffer with a specified size, and returns the number of
   * bytes that were written (0 if failed or buffer too small)
   */
  int  ReadDepthImage(VistaType::byte* pData, const int nBufferSize) const;
  bool ReadDepthImage(std::vector<VistaType::byte>& vecData) const;

  /**
   * Get/set window properties. The following keys are understood:
   *
   * STEREO               -   [bool]
   * USE_ACCUM_BUFFER     -   [bool]
   * USE_STENCIL_BUFFER   -   [bool]
   * OFFSCREEN_BUFFER     -   [bool]
   * STEREO               -   [bool]
   * DRAW_BORDER          -   [bool]
   * CONTEXT_VERSION      -   list of ints - 2 items]
   * DEBUG_CONTEXT        -   [bool]
   * FORWARD_COMPATIBLE   -   [bool]
   * MULTISAMPLED         -   [int]
   * POSITION             -   [list of ints - 2 items]
   * SIZE                 -   [list of ints - 2 items]
   * FULLSCREEN           -   [bool]
   * VSYNC                -   [set: bool] [get: int (-2: unavailable, -1: unknown, 0: disabled, 1:
   * enabled)] TITLE                -   [string] DISPLAY_NAME         -   [string][read only]
   * NUMBER_OF_VIEWPORTS  -   [int][read only]
   * VIEWPORT_NAMES       -   [list of strings][read only]
   * WINDOW_ID		    -   [int][read only]
   */
  class VISTAKERNELAPI VistaWindowProperties : public IVistaDisplayEntityProperties {
    friend class VistaWindow;

   public:
    enum {
      MSG_STEREO_CHANGE = IVistaDisplayEntityProperties::MSG_LAST,
      MSG_ACCUM_BUFFER_CHANGE,
      MSG_STENCIL_BUFFER_CHANGE,
      MSG_OFFSCREEN_BUFFER_CHANGE,
      MSG_POSITION_CHANGE,
      MSG_SIZE_CHANGE,
      MSG_FULLSCREEN_CHANGE,
      MSG_TITLE_CHANGE,
      MSG_VSYNC_CHANGE,
      MSG_DRAW_BORDER_CHANGE,
      MSG_CONTEXT_VERSION_CHANGE,
      MSG_DEBUG_CONTEXT_CHANGE,
      MSG_FORWARD_COMPATIBLE_CHANGE,
      MSG_MULTISAMPLES_CHANGED,
      MSG_LAST
    };

    bool SetName(const std::string& sName);

    bool GetStereo() const;
    bool SetStereo(const bool bStereo);

    bool GetUseAccumBuffer() const;
    bool SetUseAccumBuffer(const bool bStereo);

    bool GetUseStencilBuffer() const;
    bool SetUseStencilBuffer(const bool bStereo);

    bool GetDrawBorder() const;
    bool SetDrawBorder(const bool bDrawBorder);

    bool GetPosition(int& x, int& y) const;
    bool SetPosition(const int x, const int y);

    bool GetSize(int& w, int& h) const;
    bool SetSize(const int w, const int h);

    bool SetFullScreen(bool bFullScreen);
    bool GetFullScreen() const;

    bool GetIsOffscreenBuffer() const;
    bool SetIsOffscreenBuffer(const bool bSet);

    /**
     * returns the number of multisamples used for gl antialiasing
     * a value of 1 disables multisample aa (even system-wide settings, if possible )
     * a value of 0 or less maintains the default settings
     */
    int GetMultiSamples() const;
    /**
     * sets the number of multisamples used for gl antialiasing
     * a value of 1 disables multisample aa (even system-wide settings, if possible )
     * a value of 0 or less maintains the default settings
     */
    bool SetMultiSamples(const int nNumSamples);

    bool GetContextVersion(int& nMajor, int& nMinor) const;
    bool SetContextVersion(int nMajor, int nMinor);
    bool GetIsDebugContext() const;
    bool SetIsDebugContext(bool bIsDebug);
    bool GetIsForwardCompatible() const;
    bool SetIsForwardCompatible(bool bIsForwardCompatible);

    /**
     * returns the vsync mode:
     * -2/-1: unavailabe (can't be determined on this machine)
     * 0: off
     * 1: on
     */
    bool SetVSyncEnabled(bool bVSync);
    int  GetVSyncEnabled() const;

    std::string GetTitle() const;
    bool        SetTitle(const std::string& strTitle);

    int GetWindowId() const;

    virtual std::string GetReflectionableType() const;

   protected:
    virtual int AddToBaseTypeList(std::list<std::string>& rBtList) const;
    virtual ~VistaWindowProperties();

   private:
    VistaWindowProperties(VistaWindow*, IVistaDisplayBridge*);
  };

  VistaWindowProperties* GetWindowProperties() const;

 protected:
  virtual IVistaDisplayEntityProperties* CreateProperties();
  VistaWindow(VistaDisplay* pDisplay, IVistaDisplayEntityData* pData, IVistaDisplayBridge* pBridge);

  VistaDisplay* m_pDisplay;

 private:
  std::vector<VistaViewport*> m_vecViewports;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAWINDOW_H)
