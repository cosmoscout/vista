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

#include <GL/glew.h>

#include "VistaSDL2TextEntity.h"
#include "VistaSDL2WindowingToolkit.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_keyboard.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/GraphicsManager/VistaGLTexture.h>
#include <VistaKernel/GraphicsManager/VistaImage.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaBase/VistaExceptionBase.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <sys/types.h>

#ifdef DEBUG
#define DEBUG_CHECK_GL(sPrefix)                                                                    \
  {                                                                                                \
    GLenum nError = glGetError();                                                                  \
    while (nError != GL_NONE) {                                                                    \
      vstr::warnp() << "[SDL2WindowingTK]: Cought gl error after " << sPrefix << ": "              \
                    << gluErrorString(nError) << std::endl;                                        \
      nError = glGetError();                                                                       \
    }                                                                                              \
  }
#else
#define DEBUG_CHECK_GL(sPrefix)
#endif


// Maintain maps of all windows in order to be able to match incoming
// sdl callbacks to the correct callback. Since all windows usually use the
// same callback, this is only important if multiple displaysystems/VistaSystems
// are around

struct SDL2WindowInfo {
  SDL2WindowInfo(VistaWindow* window)
      : window(window)
      , updateCallback(nullptr)
      , currentSizeX(0)
      , currentSizeY(0)
      , currentPosX(SDL_WINDOWPOS_CENTERED)
      , currentPosY(SDL_WINDOWPOS_CENTERED)
      , preFullscreenSizeX(0)
      , preFullscreenSizeY(0)
      , preFullscreenPosX(0)
      , preFullscreenPosY(0)
      , fullscreenActive(false)
      , useStereo(false)
      , useAccumBuffer(false)
      , useStencilBuffer(false)
      , drawBorder(true)
      , sdlWindow(nullptr)
      , windowId(-1)
      , glContext(nullptr)
      , windowTitle("ViSTA")
      , vSyncMode(VistaSDL2WindowingToolkit::VSYNC_STATE_UNKNOWN)
      , cursorEnabled(true)
      , cursor(-1)
      , isOffscreenBuffer(false)
      , fboId(0)
      , fboDepthId(0)
      , fboColorId(0)
      , fboStencilId(0)
      , blitFboId(0)
      , blitFboColorId(0)
      , blitFboDepthId(0)
      , contextMajor(1)
      , contextMinor(0)
      , isDebugContext(false)
      , isForwardCompatible(false)
      , numMultiSamples(0)
      , isInitialized(false) {
  }

  ~SDL2WindowInfo() = default;

  bool                             isInitialized;
  VistaWindow*                     window;
  IVistaExplicitCallbackInterface* updateCallback;
  int                              currentSizeX;
  int                              currentSizeY;
  int                              currentPosX;
  int                              currentPosY;
  int                              preFullscreenSizeX;
  int                              preFullscreenSizeY;
  int                              preFullscreenPosX;
  int                              preFullscreenPosY;
  bool                             fullscreenActive;
  bool                             useStereo;
  bool                             useAccumBuffer;
  bool                             useStencilBuffer;
  int                              numMultiSamples;
  bool                             drawBorder;
  SDL_Window*                      sdlWindow;
  unsigned int                     windowId;
  SDL_GLContext                    glContext;
  std::string                      windowTitle;
  int                              vSyncMode;
  int                              contextMajor;
  int                              contextMinor;
  bool                             isDebugContext;
  bool                             isForwardCompatible;
  bool                             cursorEnabled;
  int                              cursor;
  // for Offscreen Buffer
  bool   isOffscreenBuffer;
  GLuint fboId;
  GLuint fboColorId;
  GLuint fboDepthId;
  GLuint fboStencilId;
  GLuint blitFboId;
  GLuint blitFboColorId;
  GLuint blitFboDepthId;
};

VistaSDL2WindowingToolkit::VistaSDL2WindowingToolkit()
    : m_quitLoop(false)
    , m_updateCallback(nullptr)
    , m_callbackCounter(0)
    , m_tmpWindowID(nullptr)
    , m_globalVSyncAvailability(~0)
    , m_hasFullWindow(false)
    , m_fullWindowId(nullptr)
    , m_dummyWindowId(nullptr)
    , m_dummyContextId(nullptr) {
  if (SDL_InitSubSystem(SDL_INIT_EVENTS) != 0) {
    vstr::warni() << "SDL2 Error: " << SDL_GetError() << std::endl;
    vstr::warni() << "SDL2 init of the events system failed - Quitting Vista" << std::endl;
    GetVistaSystem()->Quit();
  }
  
  if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
    vstr::warni() << "SDL2 Error: " << SDL_GetError() << std::endl;
    vstr::warni() << "SDL2 init of the joystick system failed - Quitting Vista" << std::endl;
    GetVistaSystem()->Quit();
  }

  if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
    vstr::warni() << "SDL2 Error: " << SDL_GetError() << std::endl;
    vstr::warni() << "SDL2 init of the controller system failed - Quitting Vista" << std::endl;
    GetVistaSystem()->Quit();
  }

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
    vstr::warni() << "SDL2 Error: " << SDL_GetError() << std::endl;
    vstr::warni() << "SDL2 init of the video system failed - Quitting Vista" << std::endl;
    GetVistaSystem()->Quit();
  }

  if (TTF_Init() != 0) {
    vstr::warni() << "TTF Error: " << TTF_GetError() << std::endl;
    vstr::warni() << "SDL2 init of the TTF library failed - Quitting Vista" << std::endl;
    GetVistaSystem()->Quit();
  }
}

VistaSDL2WindowingToolkit::~VistaSDL2WindowingToolkit() {
  TTF_Quit();
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
  SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
  SDL_QuitSubSystem(SDL_INIT_EVENTS);
  SDL_Quit();
}

IVistaTextEntity* VistaSDL2WindowingToolkit::CreateTextEntity() {
  return new VistaSDL2TextEntity();
}

void VistaSDL2WindowingToolkit::HandleWindowEvents(const SDL_WindowEvent& event) const {
  SDL2WindowInfo* window = GetWindowFromId(event.windowID);

  switch (event.event) {
    case SDL_WINDOWEVENT_SHOWN:
      // ignored
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      // ignored
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      // ignored
      break;
    case SDL_WINDOWEVENT_MOVED:
      window->currentPosX = event.data1;
      window->currentPosY = event.data2;
      window->window->GetWindowProperties()->Notify(VistaWindow::VistaWindowProperties::MSG_POSITION_CHANGE);
      break;
    case SDL_WINDOWEVENT_RESIZED:
      window->currentSizeX = event.data1;
      window->currentSizeY = event.data2;
      window->window->GetWindowProperties()->Notify(VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE);
      break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      window->currentSizeX = event.data1;
      window->currentSizeY = event.data2;
      window->window->GetWindowProperties()->Notify(VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE);
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      // ignored
      break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      // ignored
      break;
    case SDL_WINDOWEVENT_RESTORED:
      // ignored
      break;
    case SDL_WINDOWEVENT_ENTER:
      // ignored
      break;
    case SDL_WINDOWEVENT_LEAVE:
      // ignored
      break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      // ignored
      break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      // ignored
      break;
    case SDL_WINDOWEVENT_CLOSE:
      // ignored
      break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
      // ignored
      break;
    case SDL_WINDOWEVENT_HIT_TEST:
      // ignored
      break;
    case SDL_WINDOWEVENT_ICCPROF_CHANGED:
      // ignored
      break;
    case SDL_WINDOWEVENT_DISPLAY_CHANGED:
      // ignored
      break;
    default:
      vstr::warnp() << "[SDL2WindowingToolkit]: Window " << event.windowID << " got unknown event " << std::to_string(event.event) << std::endl;
      break;
  }
}

void VistaSDL2WindowingToolkit::HandleDisplayEvent(const SDL_DisplayEvent& event) const {
  switch (event.event) {
    case SDL_DISPLAYEVENT_CONNECTED:
      // ignored
      break;
    case SDL_DISPLAYEVENT_DISCONNECTED:
      // ignored
      break;
    case SDL_DISPLAYEVENT_ORIENTATION:
      // ignored
      break;
  }
}

void VistaSDL2WindowingToolkit::HandleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    SDL_EventType eventType = static_cast<SDL_EventType>(e.type);

    for (const auto& callback : m_eventCallbacks[eventType]) {
      callback.second(e);
    }

    switch(eventType) {
      // Quit
      case SDL_QUIT:
        m_quitLoop = true;
        break;

      // Display
      case SDL_DISPLAYEVENT:
        HandleDisplayEvent(e.display);
        break;
        
      // Window
      case SDL_WINDOWEVENT:
        HandleWindowEvents(e.window);
        break;

      default:
        break;
    }
  }
}

void VistaSDL2WindowingToolkit::Run() {
  while (!m_quitLoop) {
    if (m_hasFullWindow) {
      HandleEvents();
      for (auto const& window : m_windowInfo) {
        window.second->updateCallback->Do();
      }
    } else {
      m_updateCallback->Do();
    }
  }
}

size_t VistaSDL2WindowingToolkit::RegisterEventCallback(SDL_EventType eventType, SDLEventCallback callback) {
  size_t id = m_callbackCounter++;
  m_eventCallbacks[eventType].emplace(id, callback);
  return m_callbackCounter;
}

void VistaSDL2WindowingToolkit::unregisterEventCallback(SDL_EventType eventType, size_t callbackId) {
  m_eventCallbacks[eventType].erase(callbackId);
}

void VistaSDL2WindowingToolkit::Quit() {
  m_quitLoop = true;
}

void VistaSDL2WindowingToolkit::DisplayWindow(const VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);
  if (!info->isOffscreenBuffer) {
    SDL_GL_SwapWindow(info->sdlWindow);
  } else if (info->numMultiSamples > 1) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, info->fboId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, info->blitFboId);
    glBlitFramebuffer(0, 0, info->currentSizeX, info->currentSizeY, 0, 0, info->currentSizeX, info->currentSizeY, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, info->fboId);

    DEBUG_CHECK_GL("Multisample-FBO-blit");
  }
}

void VistaSDL2WindowingToolkit::DisplayAllWindows() {
  if (m_windowInfo.empty()) {
    return;
  }

  for (auto const& window : m_windowInfo) {
    DisplayWindow(window.first);
  }
}

bool VistaSDL2WindowingToolkit::RegisterWindow(VistaWindow* window) {
  auto itExists = m_windowInfo.find(window);
  if (itExists != m_windowInfo.end()) {
    return false;
  }
  
  m_windowInfo[window] = new SDL2WindowInfo(window);
  return true;
}
bool VistaSDL2WindowingToolkit::UnregisterWindow(VistaWindow* window) {
  auto itExists = m_windowInfo.find(window);
  if (itExists == m_windowInfo.end()) {
    return false;
  }

  const SDL2WindowInfo* info = itExists->second;
  SDL_Window* sdlWindow = info->sdlWindow;
  if (info->isOffscreenBuffer) {
    glDeleteFramebuffers(1, &info->fboId);
    glDeleteRenderbuffers(1, &info->fboDepthId);
    glDeleteRenderbuffers(1, &info->fboColorId);
    
    if (info->fboStencilId != 0) {
      glDeleteRenderbuffers(1, &info->fboStencilId);
    }

    if (info->numMultiSamples > 1) {
      glDeleteFramebuffers(1, &info->blitFboId);
      glDeleteRenderbuffers(1, &info->blitFboDepthId);
      glDeleteRenderbuffers(1, &info->blitFboColorId);
    }

    DEBUG_CHECK_GL("Post-OffscreenBuffer-Win-delete");
  }

  if (info->glContext) {
    SDL_GL_DeleteContext(info->glContext);
  }

  if (info->sdlWindow) {
    SDL_DestroyWindow(info->sdlWindow);
  }

  delete info;
  m_windowInfo.erase(itExists);
  return true;
}

bool VistaSDL2WindowingToolkit::InitWindow(VistaWindow* window) {
  auto itExists = m_windowInfo.find(window);
  if (itExists == m_windowInfo.end()) {
    vstr::errp() << "[SDL2WindowingToolkit]: "
                 << "Trying to initialize Window that was not registered before" << std::endl;
    return false;
  }
  SDL2WindowInfo* info = itExists->second;

  if (info->isInitialized) {
    vstr::errp() << "[SDL2WindowingToolkit]: "
                 << "Trying to initialize Window [" << window->GetNameForNameable()
                 << "] which was already initialized" << std::endl;
    return false;
  }

  if (!info->isOffscreenBuffer) {
    if (!InitAsNormalWindow(window)) {
      return false;
    }
    DestroyDummyWindow();
  } else { // is RenderToTexture
    if (!InitAsFbo(window)) {
      return false;
    }
  }

  info->isInitialized = true;
  return true;
}

bool VistaSDL2WindowingToolkit::InitAsNormalWindow(VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->useStereo) {
    SDL_GL_SetAttribute(SDL_GL_STEREO, 1);
  }

  if (info->useAccumBuffer) {
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
  }

  if (info->useStencilBuffer) {
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
  }

  if (info->contextMajor != 1 || info->contextMinor != 0) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, info->contextMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, info->contextMinor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  }

  if (info->numMultiSamples > 1) {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, info->numMultiSamples);
  }

  if (info->isDebugContext || info->isForwardCompatible) {
    uint32_t contextFlags{};
    if (info->isDebugContext) {
      contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    }
    
    if (info->isForwardCompatible) {
      contextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
  }

  
  uint32_t flags = SDL_WINDOW_OPENGL;
  if (!info->drawBorder) {
    flags |= SDL_WINDOW_BORDERLESS;
  }

  uint32_t windowOptions = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  if (info->fullscreenActive) {
    windowOptions |= SDL_WINDOW_FULLSCREEN;
  }

  info->sdlWindow = SDL_CreateWindow(info->windowTitle.c_str(), info->currentPosX, info->currentPosY, info->currentSizeX, info->currentSizeY, windowOptions);
  if (!info->sdlWindow) {
    vstr::errp() << "[SDL2WindowingToolkit]: Could not create window!" << std::endl;
    GetVistaSystem()->Quit();
  }

  info->windowId = SDL_GetWindowID(info->sdlWindow);

  info->glContext = SDL_GL_CreateContext(info->sdlWindow);
  if (!info->glContext) {
    vstr::errp() << "[SDL2WindowingToolkit]: "
                 << "OpenGL context could not be created!" << std::endl;
    GetVistaSystem()->Quit();
  }
  
  GLenum glewStatus = glewInit();
  if (glewStatus != GLEW_OK) {
    vstr::errp() << "[SDL2WindowingToolkit]: "
                 << "GLEW initialization failed!\n"
                 << "GLEW Error: " << glewGetErrorString(glewStatus)
                 << std::endl;
    GetVistaSystem()->Quit();
  }

  m_windowInfo[window] = info;

  if (m_updateCallback) {
    info->updateCallback = m_updateCallback;
  }

  if (info->fullscreenActive) {
    info->preFullscreenPosX  = info->currentPosX;
    info->preFullscreenPosY  = info->currentPosY;
    info->preFullscreenSizeX = info->currentSizeX;
    info->preFullscreenSizeY = info->currentSizeY;
  }

  SDL_GetWindowPosition(info->sdlWindow, &info->currentPosX, &info->currentPosY);
  SDL_GetWindowSizeInPixels(info->sdlWindow, &info->currentSizeX, &info->currentSizeY);

  info->isInitialized = true; // we already set the flag here, because otherwise
                              // SetVSyncMode doesn't actually set the mode

  if (info->vSyncMode == VSYNC_ENABLED) {
    SetVSyncMode(window, true);
  } else if (info->vSyncMode == VSYNC_DISABLED) {
    SetVSyncMode(window, false);
  }

  if (!m_hasFullWindow) {
    m_hasFullWindow = true;
    m_fullWindowId  = info->sdlWindow;
  }

  return true;
}

bool VistaSDL2WindowingToolkit::InitAsFbo(VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (!m_hasFullWindow && m_dummyWindowId == nullptr) {
    vstr::warnp()
        << "Using offscreen window without valid real window - creating dummy win for context"
        << std::endl;
    CreateDummyWindow(window);
  }

  if (info->useAccumBuffer) {
    vstr::warnp() << "[SDL2WindowingToolkit]: "
                  << "Window [" << window->GetNameForNameable()
                  << "] is offscreen buffer, but requests accum "
                  << "buffer - combination invalid, accum buffer will be unavailable" << std::endl;
    info->useAccumBuffer = false;
  }
  if (info->useStereo) {
    vstr::warnp() << "[SDL2WindowingToolkit]: "
                  << "Window [" << window->GetNameForNameable()
                  << "] is offscreen buffer, but requests stereo mode "
                  << "buffer - combination invalid, stereo will be unavailable" << std::endl;
    info->useStereo = false;
  }

  // GL_MAX_FRAMEBUFFER_WIDTH seem to not be available in all glew versions
#ifdef GL_MAX_FRAMEBUFFER_WIDTH
  GLint maxWidth;
  GLint maxHeight;
  glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &maxWidth);
  glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &maxHeight);
  if (info->currentSizeX > maxWidth || info->currentSizeY > maxHeight) {
    vstr::errp()
        << "[GLuWindow]: cannot create render-to-texture window - size exceeds allowed max ["
        << maxWidth << "x" << maxHeight << "]" << std::endl;
    return false;
  }
#endif
  if (info->numMultiSamples > 1) {
    return InitAsMultisampleFbo(window);
  }

  // create Fbo
  glGenFramebuffers(1, &info->fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, info->fboId);

  glGenRenderbuffers(1, &info->fboColorId);
  glBindRenderbuffer(GL_RENDERBUFFER, info->fboColorId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, info->currentSizeX, info->currentSizeY);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, info->fboColorId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glGenRenderbuffers(1, &info->fboDepthId);
  glBindRenderbuffer(GL_RENDERBUFFER, info->fboDepthId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, info->currentSizeX, info->currentSizeY);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, info->fboDepthId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  if (info->useStencilBuffer) {
    glGenRenderbuffers(1, &info->fboStencilId);
    glBindRenderbuffer(GL_RENDERBUFFER, info->fboStencilId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, info->currentSizeX, info->currentSizeY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, info->fboStencilId);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    VISTA_THROW("Failed to set up frame buffer window", -1);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  DEBUG_CHECK_GL("Post-OffscreenBuffer-Win-Swap");

  return true;
}

bool VistaSDL2WindowingToolkit::InitAsMultisampleFbo(VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  // create Fbo
  glGenFramebuffers(1, &info->fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, info->fboId);

  glGenRenderbuffers(1, &info->fboColorId);
  glBindRenderbuffer(GL_RENDERBUFFER, info->fboColorId);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, info->numMultiSamples, GL_RGB, info->currentSizeX, info->currentSizeY);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, info->fboColorId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glGenRenderbuffers(1, &info->fboDepthId);
  glBindRenderbuffer(GL_RENDERBUFFER, info->fboDepthId);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, info->numMultiSamples, GL_DEPTH_COMPONENT24, info->currentSizeX, info->currentSizeY);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, info->fboDepthId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  if (info->useStencilBuffer) {
    glGenRenderbuffers(1, &info->fboStencilId);
    glBindRenderbuffer(GL_RENDERBUFFER, info->fboStencilId);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, info->numMultiSamples, GL_STENCIL_INDEX, info->currentSizeX, info->currentSizeY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, info->fboStencilId);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    VISTA_THROW("Failed to set up frame buffer window", -1);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  DEBUG_CHECK_GL("Post-OffscreenBuffer-Win-Swap");

  // multisample framebuffer is complete, but we still need one to blit the multisample-image to
  glGenFramebuffers(1, &info->blitFboId);
  glBindFramebuffer(GL_FRAMEBUFFER, info->blitFboId);

  glGenRenderbuffers(1, &info->blitFboColorId);
  glBindRenderbuffer(GL_RENDERBUFFER, info->blitFboColorId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, info->currentSizeX, info->currentSizeY);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, info->blitFboColorId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glGenRenderbuffers(1, &info->blitFboDepthId);
  glBindRenderbuffer(GL_RENDERBUFFER, info->blitFboDepthId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, info->currentSizeX, info->currentSizeY);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, info->blitFboDepthId);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    VISTA_THROW("Failed to set up frame buffer window", -1);
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}

bool VistaSDL2WindowingToolkit::SetWindowUpdateCallback(IVistaExplicitCallbackInterface* callback) {
  m_updateCallback = callback;

  for (auto const& window : m_windowInfo) {
    window.second->updateCallback = callback;
    SDL_Window* sdlWindow = window.second->sdlWindow;
    if (sdlWindow != nullptr) {
      SDL_GL_SwapWindow(sdlWindow);
    }
  }
  
  return true;
}

bool VistaSDL2WindowingToolkit::GetWindowPosition(const VistaWindow* window, int& x, int& y) const {
  SDL2WindowInfo* info = GetWindowInfo(window);
  
  x = info->currentPosX;
  y = info->currentPosY;

  return true;
}

bool VistaSDL2WindowingToolkit::SetWindowPosition(VistaWindow* window, int x, int y) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  if (info->fullscreenActive) {
    info->preFullscreenPosX = x;
    info->preFullscreenPosY = y;
  } else {
    SDL_SetWindowPosition(info->sdlWindow, x, y);
    info->currentSizeX = x;
    info->currentSizeY = y;
  }

  return true;
}

bool VistaSDL2WindowingToolkit::GetWindowSize(const VistaWindow* window, int& width, int& height) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  width  = info->currentSizeX;
  height = info->currentSizeY;

  return true;
}

bool VistaSDL2WindowingToolkit::SetWindowSize(VistaWindow* window, int width, int height) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer && info->fboId > 0) {
    DEBUG_CHECK_GL("Pre-OffscreenBuffer-Win-Resize");

    info->currentSizeX = width;
    info->currentSizeY = height;

    if (info->numMultiSamples <= 1) {
      glBindRenderbuffer(GL_RENDERBUFFER, info->fboColorId);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, info->currentSizeX, info->currentSizeY);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      glBindRenderbuffer(GL_RENDERBUFFER, info->fboDepthId);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, info->currentSizeX, info->currentSizeY);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      if (info->useStencilBuffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, info->fboStencilId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, info->currentSizeX, info->currentSizeY);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
      }
#ifdef DEBUG
      glBindFramebuffer(GL_FRAMEBUFFER, info->fboId);
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) {
        VISTA_THROW("Failed to set up frame buffer window", -1);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
    } else {
      glBindRenderbuffer(GL_RENDERBUFFER, info->fboColorId);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, info->numMultiSamples, GL_RGB, info->currentSizeX, info->currentSizeY);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      glBindRenderbuffer(GL_RENDERBUFFER, info->fboDepthId);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, info->numMultiSamples, GL_DEPTH_COMPONENT24, info->currentSizeX, info->currentSizeY);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      if (info->useStencilBuffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, info->fboStencilId);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, info->numMultiSamples, GL_STENCIL_INDEX, info->currentSizeX, info->currentSizeY);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
      }
#ifdef DEBUG
      glBindFramebuffer(GL_FRAMEBUFFER, info->fboId);
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) {
        VISTA_THROW("Failed to set up frame buffer window", -1);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

      glBindRenderbuffer(GL_RENDERBUFFER, info->blitFboColorId);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, info->currentSizeX, info->currentSizeY);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      glBindRenderbuffer(GL_RENDERBUFFER, info->blitFboDepthId);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, info->currentSizeX, info->currentSizeY);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
#ifdef DEBUG
      glBindFramebuffer(GL_FRAMEBUFFER, info->blitFboId);
      status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) {
        VISTA_THROW("Failed to set up frame buffer window", -1);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
    }

    DEBUG_CHECK_GL("Post-OffscreenBuffer-Win-Resize");
    return true;
  }

  if (info->fullscreenActive) {
    info->preFullscreenSizeX = width;
    info->preFullscreenSizeY = height;
  } else {
    SDL_SetWindowSize(info->sdlWindow, width, height);
    info->currentSizeX = width;
    info->currentSizeY = height;
  }

  return true;
}

bool VistaSDL2WindowingToolkit::GetFullscreen(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);
  return info->fullscreenActive;
}

bool VistaSDL2WindowingToolkit::SetFullscreen(VistaWindow* window, bool enabled) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  if (info->fullscreenActive == enabled) {
    return true;
  }

  if (!info->isInitialized) {
    // before initialization - store for later
    info->fullscreenActive = true;
    return true;
  }

  if (enabled) {
    SDL_GetWindowPosition(info->sdlWindow, &info->preFullscreenPosX, &info->preFullscreenPosY);
    SDL_GetWindowSizeInPixels(info->sdlWindow, &info->preFullscreenSizeX, &info->preFullscreenSizeY);

    SDL_SetWindowFullscreen(info->sdlWindow, SDL_WINDOW_FULLSCREEN);
    info->fullscreenActive = true;
  } else {
    SDL_SetWindowFullscreen(info->sdlWindow, 0);
    info->fullscreenActive = false;

    SDL_SetWindowPosition(info->sdlWindow, info->preFullscreenPosX, info->preFullscreenPosY);
    SDL_SetWindowSize(info->sdlWindow, info->preFullscreenSizeX, info->preFullscreenSizeY);
  }

  return true;
}
bool VistaSDL2WindowingToolkit::SetWindowTitle(VistaWindow* window, const std::string& title) {
  SDL2WindowInfo* info = GetWindowInfo(window);
  info->windowTitle = title;

  if (info->isOffscreenBuffer) {
    return false;
  }

  SDL_SetWindowTitle(info->sdlWindow, title.c_str());

  return true;
}

std::string VistaSDL2WindowingToolkit::GetWindowTitle(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);
  return info->windowTitle;
}

bool VistaSDL2WindowingToolkit::SetCursorIsEnabled(VistaWindow* window, bool set) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  info->cursorEnabled = set;
  if (set) {
    auto cursor = SDL_CreateSystemCursor((SDL_SystemCursor) info->cursor);
    SDL_SetCursor(cursor);
    SDL_ShowCursor(SDL_ENABLE);
  } else {
    SDL_ShowCursor(SDL_DISABLE);
  }

  return true;
}
bool VistaSDL2WindowingToolkit::GetCursorIsEnabled(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  return info->cursorEnabled;
}

bool VistaSDL2WindowingToolkit::GetUseStereo(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  return info->useStereo;
}
bool VistaSDL2WindowingToolkit::SetUseStereo(VistaWindow* window, bool set) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change stereo mode on window ["
                  << window->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  info->useStereo = set;
  return true;
}

bool VistaSDL2WindowingToolkit::GetUseAccumBuffer(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  return info->useAccumBuffer;
}
bool VistaSDL2WindowingToolkit::SetUseAccumBuffer(VistaWindow* window, bool set) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change accum buffer mode on window ["
                  << window->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  info->useAccumBuffer = set;
  return true;
}

bool VistaSDL2WindowingToolkit::GetUseStencilBuffer(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  return info->useStencilBuffer;
}
bool VistaSDL2WindowingToolkit::SetUseStencilBuffer(VistaWindow* window, bool set) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change stencil buffer mode on window ["
                  << window->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  info->useStencilBuffer = set;
  return true;
}

int VistaSDL2WindowingToolkit::GetMultiSamples(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  return info->numMultiSamples;
}

bool VistaSDL2WindowingToolkit::SetMultiSamples(const VistaWindow* window, int numSamples) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change multisampling on window ["
                  << window->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }


  info->numMultiSamples = numSamples;
  return true;
}

bool VistaSDL2WindowingToolkit::GetUseOffscreenBuffer(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  return info->isOffscreenBuffer;
}

bool VistaSDL2WindowingToolkit::GetDrawBorder(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);
  return info->drawBorder;
}

bool VistaSDL2WindowingToolkit::SetDrawBorder(VistaWindow* window, bool set) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change borderless prop on window ["
                  << window->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  SDL_SetWindowBordered(info->sdlWindow, set ? SDL_TRUE : SDL_FALSE);
  info->drawBorder = set;
  return true;
}

bool VistaSDL2WindowingToolkit::SetUseOffscreenBuffer(VistaWindow* window, bool set) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change render-to-texture mode on window ["
                  << window->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  info->isOffscreenBuffer = set;
  return true;
}

bool VistaSDL2WindowingToolkit::GetRGBImage(const VistaWindow* window, std::vector<VistaType::byte>& vecData) const {
  SDL2WindowInfo* info = GetWindowInfo(window);
  int numPixels = info->currentSizeX * info->currentSizeY;
  int dataSize  = numPixels * 3;
  vecData.resize(dataSize);
  int ret = GetRGBImage(window, vecData.data(), dataSize);
  return (ret == dataSize);
}

int VistaSDL2WindowingToolkit::GetRGBImage(const VistaWindow* window, VistaType::byte* data, int bufferSize) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  DEBUG_CHECK_GL("Pre-GetRGBImage");

  int numPixels = info->currentSizeX * info->currentSizeY;
  int dataSize  = numPixels * 3;
  if (bufferSize < dataSize) {
    return 0;
  }

  if (!info->isOffscreenBuffer) {
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, data);
  } else if (info->numMultiSamples > 1) {
    glBindFramebuffer(GL_FRAMEBUFFER, info->blitFboId);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindFramebuffer(GL_FRAMEBUFFER, info->fboId);
  } else {
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, data);
  }

  DEBUG_CHECK_GL("Post-GetRGBImage");
  return dataSize;
}

bool VistaSDL2WindowingToolkit::GetDepthImage(const VistaWindow* window, std::vector<VistaType::byte>& vecData) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  int numPixels = info->currentSizeX * info->currentSizeY;
  int dataSize  = numPixels * sizeof(float);
  vecData.resize(dataSize);
  int ret = GetDepthImage(window, vecData.data(), dataSize);
  return (ret == dataSize);
}

int VistaSDL2WindowingToolkit::GetDepthImage(const VistaWindow* window, VistaType::byte* data, const int bufferSize) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  int numPixels = info->currentSizeX * info->currentSizeY;
  int dataSize  = numPixels * sizeof(float);
  if (bufferSize < dataSize) {
    return 0;
  }
  
  DEBUG_CHECK_GL("Pre-GetDepthImage");

  if (!info->isOffscreenBuffer) {
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, data);
  } else if (info->numMultiSamples > 1) {
    glBindFramebuffer(GL_FRAMEBUFFER, info->blitFboId);
    glReadBuffer(GL_DEPTH_COMPONENT);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindFramebuffer(GL_FRAMEBUFFER, info->fboId);
  } else {
    glReadBuffer(GL_DEPTH_COMPONENT);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, data);
  }

  DEBUG_CHECK_GL("Post-GetDepthImage");

  return dataSize;
}

VistaImage VistaSDL2WindowingToolkit::GetRGBImage(const VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  DEBUG_CHECK_GL("Pre-GetRGBImage");

  VistaImage result;

  if (!info->isOffscreenBuffer) {
    result.Set2DData(info->currentSizeX, info->currentSizeY, NULL, GL_RGB, GL_UNSIGNED_BYTE);

    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, result.GetDataWrite());
  } else {
    result.Set2DData(info->currentSizeX, info->currentSizeY, NULL, GL_RGB, GL_UNSIGNED_BYTE);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, result.GetDataWrite());
  }

  DEBUG_CHECK_GL("Post-GetRGBImage");
  return result;
}

VistaImage VistaSDL2WindowingToolkit::GetDepthImage(const VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  VistaImage result;

  DEBUG_CHECK_GL("Pre-GetDepthImage");

  if (!info->isOffscreenBuffer) {
    result.Set2DData(info->currentSizeX, info->currentSizeY, NULL, GL_LUMINANCE, GL_FLOAT);

    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, result.GetDataWrite());
  } else {
    result.Set2DData(info->currentSizeX, info->currentSizeY, NULL, GL_LUMINANCE, GL_FLOAT);

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_DEPTH_COMPONENT, GL_FLOAT, result.GetDataWrite());
  }

  DEBUG_CHECK_GL("Post-GetDepthImage");

  return result;
}

int VistaSDL2WindowingToolkit::GetWindowId(const VistaWindow* window) const {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return -1;
  }
  
  return info->windowId;
}

SDL2WindowInfo* VistaSDL2WindowingToolkit::GetWindowFromId(Uint32 windowID) const {
  for (auto const& window : m_windowInfo) {
    if (window.second->windowId == windowID) {
      return window.second;
    }
  }

  return nullptr;
}

void VistaSDL2WindowingToolkit::BindWindow(VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->numMultiSamples == 1) {
    glDisable(GL_MULTISAMPLE);
  } else {
    glEnable(GL_MULTISAMPLE);
  }
}

void VistaSDL2WindowingToolkit::UnbindWindow(VistaWindow* window) {
}

bool VistaSDL2WindowingToolkit::GetVSyncCanBeModified(const VistaWindow* window) {
  return CheckVSyncAvailability();
}

int VistaSDL2WindowingToolkit::GetVSyncMode(const VistaWindow* window) {
  if (CheckVSyncAvailability() == VSYNC_DISABLED) {
    return VSYNC_STATE_UNAVAILABLE;
  }

  SDL2WindowInfo* info = GetWindowInfo(window);
  if (info->vSyncMode == VSYNC_STATE_UNKNOWN) {
    int interval = SDL_GL_GetSwapInterval();
    if (interval == 0) {
      m_globalVSyncAvailability = VSYNC_DISABLED;
    } else if (interval == 1) {
      m_globalVSyncAvailability = VSYNC_ENABLED;
    } else if (interval == -1) {
      m_globalVSyncAvailability = ADAPTIVE_VSYNC_ENABLED;
    }
  }
  return m_globalVSyncAvailability;
}

bool VistaSDL2WindowingToolkit::SetCursor(VistaWindow* window, int cursor) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isOffscreenBuffer) {
    return false;
  }

  info->cursor = cursor;
  if (info->cursorEnabled) {
    SDL_SetCursor(SDL_CreateSystemCursor((SDL_SystemCursor) cursor));
  }
  
  return true;
}

int VistaSDL2WindowingToolkit::GetCursor(const VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);
  return info->cursor;
}

bool VistaSDL2WindowingToolkit::SetVSyncMode(VistaWindow* window, bool enabled) {
  SDL2WindowInfo* info = GetWindowInfo(window);

  if (info->isInitialized == false) {
    // pre-init, just store the value
    if (enabled) {
      info->vSyncMode = VSYNC_ENABLED;
    } else {
      info->vSyncMode = VSYNC_DISABLED;
    }

    return true;
  }

  int interval = enabled ? 1 : 0;

  if (SDL_GL_SetSwapInterval(interval) == 0) {
    m_globalVSyncAvailability = interval;
  } else {
    vstr::errp() << "VistaSDL2WindowingToolkit::SetVSyncEnabled -"
                 << "Setting VSync failed - does driver config enforce on/off?" << std::endl;
    m_globalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
  }

  return (m_globalVSyncAvailability != VSYNC_STATE_UNAVAILABLE);
}

bool VistaSDL2WindowingToolkit::CheckVSyncAvailability() {
  if (m_globalVSyncAvailability != ~0) {
    return (m_globalVSyncAvailability != VSYNC_STATE_UNAVAILABLE);
  }
  
  void* SetSwapIntervalFunction = nullptr;
  void* GetSwapIntervalFunction = nullptr;

#ifdef WIN32

  m_globalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
  SetSwapIntervalFunction    = SDL_GL_GetProcAddress("wglSwapIntervalEXT");
  if (SetSwapIntervalFunction) {
    m_globalVSyncAvailability = VSYNC_STATE_UNKNOWN;
    GetSwapIntervalFunction = SDL_GL_GetProcAddress("wglGetSwapIntervalEXT");
  }

#elif defined LINUX

  SetSwapIntervalFunction = SDL_GL_GetProcAddress("glXSwapIntervalSGI");
  if (SetSwapIntervalFunction) {
    m_globalVSyncAvailability = VSYNC_STATE_UNKNOWN;
  } else {
    m_globalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
  }

#else
  m_iGlobalVSyncAvailability = VSYNC_STATE_UNAVAILABLE;
#endif

  return (m_globalVSyncAvailability != VSYNC_STATE_UNAVAILABLE);
}

SDL2WindowInfo* VistaSDL2WindowingToolkit::GetWindowInfo(const VistaWindow* window) const {
  auto itWindow = m_windowInfo.find(window);
  if (itWindow == m_windowInfo.end()) {
    return nullptr;
  }

  return itWindow->second;
}

bool VistaSDL2WindowingToolkit::GetContextVersion(int& major, int& minor, const VistaWindow* target) const {
  SDL2WindowInfo* info = GetWindowInfo(target);

  major = info->contextMajor;
  minor = info->contextMinor;
  return true;
}

bool VistaSDL2WindowingToolkit::SetContextVersion(int major, int minor, VistaWindow* target) {
  SDL2WindowInfo* info = GetWindowInfo(target);

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change context version on window ["
                  << target->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  info->contextMajor = major;
  info->contextMinor = minor;
  return true;
}

bool VistaSDL2WindowingToolkit::GetIsDebugContext(const VistaWindow* target) const {
  SDL2WindowInfo* info = GetWindowInfo(target);
  return info->isDebugContext;
}

bool VistaSDL2WindowingToolkit::SetIsDebugContext(bool isDebug, VistaWindow* target) {
  SDL2WindowInfo* info = GetWindowInfo(target);

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change debug context flag on window ["
                  << target->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  info->isDebugContext = isDebug;
  return true;
}

bool VistaSDL2WindowingToolkit::GetIsForwardCompatible(const VistaWindow* target) const {
  SDL2WindowInfo* info = GetWindowInfo(target);
  return info->isForwardCompatible;
}

bool VistaSDL2WindowingToolkit::SetIsForwardCompatible(bool isForwardCompatible, VistaWindow* target) {
  SDL2WindowInfo* info = GetWindowInfo(target);

  if (info->isInitialized) {
    vstr::warnp() << "[SDL2Window]: Trying to change forward compatible flag on window ["
                  << target->GetNameForNameable()
                  << "] - this can only be done before initialization" << std::endl;
    return false;
  }

  info->isForwardCompatible = isForwardCompatible;
  return true;
}

bool VistaSDL2WindowingToolkit::CreateDummyWindow(VistaWindow* window) {
  SDL2WindowInfo* info        = GetWindowInfo(window);

  if (info->contextMajor != 1 || info->contextMinor != 0) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, info->contextMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, info->contextMinor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  }

  if (info->isDebugContext || info->isForwardCompatible) {
    uint32_t contextFlags{};
    if (info->isDebugContext) {
      contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    }
    
    if (info->isForwardCompatible) {
      contextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
  }

  m_dummyWindowId = SDL_CreateWindow("dummy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
  if (!m_dummyWindowId) {
    vstr::errp() << "[SDL2WindowingToolkit]: Could not create dummy window!" << std::endl;
    GetVistaSystem()->Quit();
  }

  m_dummyContextId = SDL_GL_CreateContext(m_dummyWindowId);
  if (!m_dummyContextId) {
    vstr::errp() << "[SDL2WindowingToolkit]: Could not create dummy context!" << std::endl;
    GetVistaSystem()->Quit();
  }

  GLenum glewStatus = glewInit();
  if (glewStatus != GLEW_OK) {
    vstr::errp() << "[SDL2WindowingToolkit]: "
                 << "GLEW initialization failed!\n"
                 << "GLEW Error: " << glewGetErrorString(glewStatus)
                 << std::endl;
    GetVistaSystem()->Quit();
  }

  return true;
}

void VistaSDL2WindowingToolkit::DestroyDummyWindow() {
  if (m_dummyWindowId) {
    SDL_DestroyWindow(m_dummyWindowId);
  }

  if (m_dummyContextId) {
    SDL_GL_DeleteContext(m_dummyContextId);
  }

  m_dummyWindowId  = nullptr;
  m_dummyContextId = nullptr;
}
