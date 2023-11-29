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

#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>

#include <VistaKernel/VistaKernelConfig.h>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>

#include <map>
#include <functional>
#include <string>

class VistaDisplayManager;
class VistaImage;
struct SDL2WindowInfo;

/**
 * SDL2 implementation of IVistaWindowingToolkit. See IVistaWindowingToolkit.h
 * for documentation.
 *
 * Additionally to providing all required windowing functions this toolkit also provides functions
 * for registering event callbacks. See RegisterEventCallback and UnregisterEventCallback below.
 */
class VISTAKERNELAPI VistaSDL2WindowingToolkit : public IVistaWindowingToolkit {
 public:
  /**
   * Create a new SDL2WindowingToolkit. It initializes the following SDL subsystems and libraries:
   * - Events
   * - Joystick
   * - Gamecontroller
   * - Video
   * - TTF
   */
  VistaSDL2WindowingToolkit();

  /**
   * Cleans all the SDL subsystems up.
   */
  ~VistaSDL2WindowingToolkit();

  /**
   * This starts and executes the main loop. The window update callbacks are being called and all
   * events are being handled here.
   */
  void Run() final;

  /**
   * Stops the main loop.
   */
  void Quit() final;

  void DisplayWindow(const VistaWindow* window) override;
  void DisplayAllWindows() final;

  bool RegisterWindow(VistaWindow* window) override;
  bool UnregisterWindow(VistaWindow* window) override;

  bool InitWindow(VistaWindow* window) override;

  bool SetWindowUpdateCallback(IVistaExplicitCallbackInterface* callback) final;

  bool GetWindowPosition(VistaWindow const* window, int& x, int& y) const final;
  bool SetWindowPosition(VistaWindow* window, int x, int y) final;
  bool GetWindowSize(VistaWindow const* window, int& width, int& height) const final;
  bool SetWindowSize(VistaWindow* window, int width, int height) final;

  bool GetFullscreen(VistaWindow const* window) const final;
  bool SetFullscreen(VistaWindow* window, bool bSet) final;

  std::string GetWindowTitle(VistaWindow const* window) const final;
  bool        SetWindowTitle(VistaWindow* window, const std::string& title) final;

  bool GetCursorIsEnabled(VistaWindow const* window) const final;
  bool SetCursorIsEnabled(VistaWindow* window, bool set) final;

  bool GetUseStereo(VistaWindow const* window) const final;
  bool SetUseStereo(VistaWindow* window, bool set) final;

  bool GetUseAccumBuffer(VistaWindow const* window) const final;
  bool SetUseAccumBuffer(VistaWindow* window, bool set) final;

  bool GetUseStencilBuffer(VistaWindow const* window) const final;
  bool SetUseStencilBuffer(VistaWindow* window, bool set) final;

  int  GetMultiSamples(VistaWindow const* window) const final;
  bool SetMultiSamples(VistaWindow const* window, int numSamples) final;

  bool GetDrawBorder(VistaWindow const* window) const final;
  bool SetDrawBorder(VistaWindow* window, bool set) final;

  bool GetUseOffscreenBuffer(VistaWindow const* window) const final;
  bool SetUseOffscreenBuffer(VistaWindow* window, bool set) final;

  bool GetContextVersion(int& major, int& minor, const VistaWindow* target) const final;
  bool SetContextVersion(int major, int minor, VistaWindow* target) final;
  bool GetIsDebugContext(VistaWindow const* target) const final;
  bool SetIsDebugContext(bool isDebug, VistaWindow* target) final;
  bool GetIsForwardCompatible(VistaWindow const* target) const final;
  bool SetIsForwardCompatible(bool isForwardCompatible, VistaWindow* target) final;

  int  GetRGBImage(VistaWindow const* window, VistaType::byte* data, int bufferSize) const final;
  bool GetRGBImage(VistaWindow const* window, std::vector<VistaType::byte>& vecData) const final;
  int  GetDepthImage(VistaWindow const* window, VistaType::byte* data, int bufferSize) const final;
  bool GetDepthImage(VistaWindow const* window, std::vector<VistaType::byte>& vecData) const final;
  VistaImage GetRGBImage(VistaWindow const* window) final;
  VistaImage GetDepthImage(VistaWindow const* window) final;

  enum {
    VSYNC_STATE_UNAVAILABLE = -2,
    VSYNC_STATE_UNKNOWN     = -1,
    VSYNC_DISABLED          = 0,
    VSYNC_ENABLED           = 1,
    ADAPTIVE_VSYNC_ENABLED  = 2 // Currently not supported by Vista.
  };

  bool GetVSyncCanBeModified(VistaWindow const* window) final;
  bool SetVSyncMode(VistaWindow* window, bool enabled) final;
  int  GetVSyncMode(VistaWindow const* window) final;

  bool SetCursor(VistaWindow const* window, int cursor) const;
  int  GetCursor(VistaWindow const* window) const;

  IVistaTextEntity* CreateTextEntity() final;

  int  GetWindowId(VistaWindow const* window) const final;
  void BindWindow(VistaWindow* window) final;
  void UnbindWindow(VistaWindow* window) final;

  using SDLEventCallback = std::function<void(SDL_Event const&)>;

  /**
   * You can use this function to register a callback to a specific SDL event type. You will then
   * be notified of all events matching this type. The returned id is needed for unregistering.
   */
  size_t RegisterEventCallback(SDL_EventType eventType, SDLEventCallback callback);

  /**
   * Unregisters an event of the given type and with the given id. The event type MUST be identical
   * to the one given at RegisterEventCallback!
   */
  void UnregisterEventCallback(SDL_EventType eventType, size_t callbackId);

 protected:
  bool CheckVSyncAvailability();

  SDL2WindowInfo* GetWindowInfo(VistaWindow const* window) const;

  bool InitAsNormalWindow(VistaWindow* window);
  bool InitAsFbo(VistaWindow* window);
  bool InitAsMultisampleFbo(VistaWindow* window);

  bool CreateDummyWindow(VistaWindow* window);
  void DestroyDummyWindow();

 private:
  /**
   * Returns the WindowInfo from the given id. The id matches SDLs internal window ids.
   */
  SDL2WindowInfo* GetWindowFromId(Uint32 windowId) const;

  /**
   * Handle SDL events. Also notifies event listeners.
   */
  void HandleEvents();

  /**
   * Handles window specific events.
   */
  void HandleWindowEvents(SDL_WindowEvent const& windowEvent) const;

  /**
   * Handles display specific events.
   */
  void HandleDisplayEvent(SDL_DisplayEvent const& event) const;

  size_t                                                      m_callbackCounter;
  std::map<SDL_EventType, std::map<size_t, SDLEventCallback>> m_eventCallbacks;

  using WindowInfoMap = std::map<const VistaWindow*, SDL2WindowInfo*>;
  WindowInfoMap                    m_windowInfo;
  bool                             m_quitLoop;
  IVistaExplicitCallbackInterface* m_updateCallback;
  mutable SDL_Window*              m_tmpWindow;
  int                              m_globalVSyncAvailability;

  bool        m_hasFullWindow;
  SDL_Window* m_fullWindow;

  SDL_Window*   m_dummyWindow;
  SDL_GLContext m_dummyContextId;

  int m_cursor;
};

#endif
