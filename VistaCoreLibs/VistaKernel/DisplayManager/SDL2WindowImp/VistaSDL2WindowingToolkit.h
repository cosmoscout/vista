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

#ifndef _VISTASDLWINDOWINGTOOLKIT_H
#define _VISTASDLWINDOWINGTOOLKIT_H

#include <SDL2/SDL_video.h>
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>

#include <map>
#include <string>

class VistaDisplayManager;
class VistaGLTexture;
class VistaImage;
struct SDL2WindowInfo;

/**
 * SDL2 implementation of IVistaWindowingToolkit. See IVistaWindowingToolkit.h
 * for documentation.
 */
class VISTAKERNELAPI VistaSDL2WindowingToolkit : public IVistaWindowingToolkit {
 public:
  VistaSDL2WindowingToolkit();
  ~VistaSDL2WindowingToolkit();

  void Run() final;
  void Quit() final;

  void DisplayWindow(const VistaWindow* window) final;
  void DisplayAllWindows() final;

  bool RegisterWindow(VistaWindow* window) final;
  bool UnregisterWindow(VistaWindow* window) final;

  bool InitWindow(VistaWindow* window) final;

  bool SetWindowUpdateCallback(IVistaExplicitCallbackInterface* callback) final;

  bool GetWindowPosition(const VistaWindow* window, int& x, int& y) const final;
  bool SetWindowPosition(VistaWindow* window, int x, int y) final;
  bool GetWindowSize(const VistaWindow* window, int& width, int& height) const final;
  bool SetWindowSize(VistaWindow* window, int width, int height) final;

  bool GetFullscreen(const VistaWindow* window) const final;
  bool SetFullscreen(VistaWindow* window, const bool bSet) final;

  std::string GetWindowTitle(const VistaWindow* window) const final;
  bool        SetWindowTitle(VistaWindow* window, const std::string& title) final;

  bool GetCursorIsEnabled(const VistaWindow* window) const final;
  bool SetCursorIsEnabled(VistaWindow* window, bool set) final;

  bool GetUseStereo(const VistaWindow* window) const final;
  bool SetUseStereo(VistaWindow* window, bool set) final;

  bool GetUseAccumBuffer(const VistaWindow* window) const final;
  bool SetUseAccumBuffer(VistaWindow* window, bool set) final;

  bool GetUseStencilBuffer(const VistaWindow* window) const final;
  bool SetUseStencilBuffer(VistaWindow* window, bool set) final;

  int  GetMultiSamples(const VistaWindow* window) const final;
  bool SetMultiSamples(const VistaWindow* window, int numSamples) final;

  bool GetDrawBorder(const VistaWindow* window) const final;
  bool SetDrawBorder(VistaWindow* window, bool set) final;

  bool GetUseOffscreenBuffer(const VistaWindow* window) const final;
  bool SetUseOffscreenBuffer(VistaWindow* window, bool set) final;

  bool GetContextVersion(int& major, int& minor, const VistaWindow* target) const final;
  bool SetContextVersion(int major, int minor, VistaWindow* target) final;
  bool GetIsDebugContext(const VistaWindow* target) const final;
  bool SetIsDebugContext(const bool isDebug, VistaWindow* target) final;
  bool GetIsForwardCompatible(const VistaWindow* target) const final;
  bool SetIsForwardCompatible(const bool isForwardCompatible, VistaWindow* target) final;

  int GetRGBImage(const VistaWindow* window, VistaType::byte* data, int bufferSize) const final;
  bool GetRGBImage(const VistaWindow* window, std::vector<VistaType::byte>& vecData) const final;
  int  GetDepthImage(const VistaWindow* window, VistaType::byte* data, int bufferSize) const final;
  bool GetDepthImage(const VistaWindow* window, std::vector<VistaType::byte>& vecData) const final;
  VistaImage GetRGBImage(const VistaWindow* window) final;
  VistaImage GetDepthImage(const VistaWindow* window) final;

  enum {
    VSYNC_STATE_UNAVAILABLE = -2,
    VSYNC_STATE_UNKNOWN     = -1,
    VSYNC_DISABLED          = 0,
    VSYNC_ENABLED           = 1,
    ADAPTIVE_VSYNC_ENABLED  = 2
  };
  bool GetVSyncCanBeModified(const VistaWindow* window) final;
  bool SetVSyncMode(VistaWindow* window, bool enabled) final;
  int  GetVSyncMode(const VistaWindow* window) final;

  bool SetCursor(VistaWindow* window, int cursor);
  int  GetCursor(const VistaWindow* pWindow);

  IVistaTextEntity* CreateTextEntity() final;

  int GetWindowId(const VistaWindow* window) const final;
  void BindWindow(VistaWindow* window) final;
  void UnbindWindow(VistaWindow* window) final;

 protected:
  bool CheckVSyncAvailability();

  SDL2WindowInfo* GetWindowInfo(const VistaWindow* window) const;

  bool InitAsNormalWindow(VistaWindow* window);
  bool InitAsFbo(VistaWindow* window);
  bool InitAsMultisampleFbo(VistaWindow* window);

  bool CreateDummyWindow(VistaWindow* window);
  void DestroyDummyWindow();

 private:
  typedef std::map<const VistaWindow*, SDL2WindowInfo*> WindowInfoMap;
  WindowInfoMap                                         m_windowInfo;
  bool                                                  m_quitLoop;
  IVistaExplicitCallbackInterface*                      m_updateCallback;
  mutable SDL_Window*                                   m_tmpWindowID;
  int                                                   m_globalVSyncAvailability;

  bool        m_hasFullWindow;
  SDL_Window* m_fullWindowId;
  SDL_Window* m_dummyWindowId;
  int         m_windowIdCounter;
  int m_cursor;
};

#endif
