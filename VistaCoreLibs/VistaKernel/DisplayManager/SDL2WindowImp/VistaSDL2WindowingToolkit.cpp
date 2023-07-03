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
  int                              windowId;
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

/*
namespace {
std::map<SDL_Window*, SDL2WindowInfo*> windowInfo;

void DisplayUpdate() {
  auto firstWindow = windowInfo.find(0);
  assert(itWindowInfo != windowInfo.end());

  firstWindow->second->updateCallback->Do();
}

void DisplayReshape(int width, int height) {
  auto firstWindow = windowInfo.find(0);
  assert(itWindowInfo != windowInfo.end());
  SDL2WindowInfo* info = firstWindow->second;
  if (info->currentSizeX != width || info->currentSizeY != height) {
    info->currentSizeX = width;
    info->currentSizeY = height;
    firstWindow->second->window->GetProperties()
        ->Notify(VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE);
  }
}

void CloseFunction() {
  if (GetVistaSystem()) {
    vstr::warni() << "SDL2Window closed - Quitting Vista" << std::endl;
    GetVistaSystem()->Quit();
  }
}
} // namespace
*/

VistaSDL2WindowingToolkit::VistaSDL2WindowingToolkit()
    : m_quitLoop(false)
    , m_updateCallback(nullptr)
    , m_tmpWindowID(nullptr)
    , m_globalVSyncAvailability(~0)
    , m_hasFullWindow(false)
    , m_fullWindowId(nullptr)
    , m_dummyWindowId(nullptr)
    , m_windowIdCounter(0) {
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0) {
    vstr::warni() << "SDL2 init failed - Quitting Vista" << std::endl;
    GetVistaSystem()->Quit();
  }
}

VistaSDL2WindowingToolkit::~VistaSDL2WindowingToolkit() {
}

IVistaTextEntity* VistaSDL2WindowingToolkit::CreateTextEntity() {
  return new VistaSDL2TextEntity();
}

void VistaSDL2WindowingToolkit::Run() {
  while (!m_quitLoop) {
    if (m_hasFullWindow)
      ;// glutMainLoopEvent();
    else
      m_updateCallback->Do();
  }
}

void VistaSDL2WindowingToolkit::Quit() {
  m_quitLoop = true;
}

void VistaSDL2WindowingToolkit::DisplayWindow(const VistaWindow* window) {
  SDL2WindowInfo* info = GetWindowInfo(window);
  if (info->isOffscreenBuffer == false) {
    SDL_GL_SwapWindow(info->sdlWindow);
    // glutPostRedisplay();
  } else if (info->numMultiSamples > 1) {
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, info->fboId);
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, info->blitFboId);
    glBlitFramebuffer(0, 0, info->currentSizeX, info->currentSizeY, 0, 0, info->currentSizeX, info->currentSizeY, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, info->fboId);

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
  info->windowId = m_windowIdCounter++;
  glewInit();

  m_windowInfo[window] = info;

  if (m_updateCallback) {
    info->updateCallback = m_updateCallback;
    //glutDisplayFunc(&DisplayUpdate);
    //glutIdleFunc(&DisplayUpdate);
  }
  //glutReshapeFunc(&DisplayReshape);

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

/*
#ifndef USE_NATIVE_GLUT
  // set the close function to catch window close attempts
  glutCloseFunc(CloseFunction);
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif // USE_NATIVE_GLUT

  glutPostRedisplay();
*/
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

  if (glewGetExtension("GL_EXT_framebuffer_object") == GL_FALSE) {
    vstr::errp() << "[SDL2WindowingToolkit]: "
                 << "Trying to initialize Window [" << window->GetNameForNameable()
                 << "] as offscreen buffer failed - framebuffer objects not supported" << std::endl;
    return false;
  }
  assert(__glewGenFramebuffersEXT != NULL);

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

  if (glewGetExtension("GL_EXT_framebuffer_multisample") == GL_FALSE) {
    vstr::errp() << "[SDL2WindowingToolkit]: "
                 << "Trying to initialize Window [" << window->GetNameForNameable()
                 << "] as multisample offscreen buffer failed - multisample framebuffer objects "
                    "not supported"
                 << std::endl;
    return false;
  }
  assert(__glewRenderbufferStorageMultisampleEXT != NULL);

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

bool VistaSDL2WindowingToolkit::SetFullscreen(VistaWindow* window, const bool enabled) {
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
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, info->blitFboId);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, info->fboId);
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
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, info->blitFboId);
    glReadBuffer(GL_DEPTH_COMPONENT);
    glReadPixels(0, 0, info->currentSizeX, info->currentSizeY, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, info->fboId);
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
  if (m_globalVSyncAvailability != ~0)
    return (m_globalVSyncAvailability != VSYNC_STATE_UNAVAILABLE);

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
  WindowInfoMap::const_iterator itWindow = m_windowInfo.find(window);
  if (itWindow == m_windowInfo.end()) {
    return nullptr;
  }

  return itWindow->second;
}

bool VistaSDL2WindowingToolkit::GetContextVersion(int& major, int& minor, const VistaWindow* target) const {
  SDL2WindowInfo* info = GetWindowInfo(target);
  major                = info->contextMajor;
  minor                = info->contextMinor;

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
  glewInit();

  return true;
}

void VistaSDL2WindowingToolkit::DestroyDummyWindow() {
  if (m_dummyWindowId) {
    SDL_DestroyWindow(m_dummyWindowId);
  }
  m_dummyWindowId = nullptr;
}