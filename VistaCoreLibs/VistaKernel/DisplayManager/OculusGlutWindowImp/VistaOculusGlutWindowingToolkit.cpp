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
// $Id: VistaGlutWindowingToolkit.cpp 42600 2014-06-18 19:23:49Z dr165799 $

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaOculusGlutWindowingToolkit.h"

#include "VistaBase/VistaStreamUtils.h"
#include "VistaBase/VistaUtilityMacros.h"
#include "VistaKernel/DisplayManager//VistaWindow.h"
#include <OVR.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include "../VistaViewport.h"
#include "GL/glut.h"
#include "VistaBase/VistaTimeUtils.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaOculusGlutWindowingToolkit::Internal {
 public:
  Internal()
      : m_bIsInitalized(false) {
  }
  bool m_bIsInitalized;
  struct WindowDataExt {
    WindowDataExt()
        : m_pHmd(NULL) {
    }
    ovrHmd m_pHmd;
  };
  std::map<const VistaWindow*, WindowDataExt> m_mapWindowData;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOculusGlutWindowingToolkit::VistaOculusGlutWindowingToolkit()
    : VistaGlutWindowingToolkit()
    , m_pData(new Internal()) {
}

VistaOculusGlutWindowingToolkit::~VistaOculusGlutWindowingToolkit() {
  if (m_pData->m_bIsInitalized == false) {
    ovr_Shutdown();
  }
}

bool VistaOculusGlutWindowingToolkit::RegisterWindow(VistaWindow* pWindow) {
  if (m_pData->m_bIsInitalized == false) {
    ovr_Initialize();
    m_pData->m_bIsInitalized = true;
  }
  VistaTimeUtils::Sleep(1000);
  int  nRequestedHMDIndex = (int)m_pData->m_mapWindowData.size();
  int  nNumHmds           = ovrHmd_Detect();
  bool bHasHmd            = false;

  // @TODO: when to use debug hmd, when to share existing hmds?
  Internal::WindowDataExt& oData = m_pData->m_mapWindowData[pWindow];
  if (nNumHmds == 0) {
    vstr::warnp() << "[VistaOculusGlutWindowingToolkit]: No Oculus HMD detected - "
                  << "Creating DebugHmd instead" << std::endl;
    oData.m_pHmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    bHasHmd      = true;
  } else if (nNumHmds <= nRequestedHMDIndex) {
    vstr::warnp()
        << "[VistaOculusGlutWindowingToolkit]: New Oculus HMD requested, but windows for all "
        << nNumHmds << " connected HMD(s) already created - "
        << "Creating DebugHmd instead" << std::endl;
    oData.m_pHmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    bHasHmd      = true;
  } else {
    oData.m_pHmd = ovrHmd_Create(nRequestedHMDIndex);
    bHasHmd      = true;
  }

  bool bSuccess = VistaGlutWindowingToolkit::RegisterWindow(pWindow);
  // we set the default parameters for the window - can be overwritten afterwards
  if (bHasHmd) {
    Internal::WindowDataExt& oData = m_pData->m_mapWindowData[pWindow];
    pWindow->GetWindowProperties()->SetPosition(
        oData.m_pHmd->WindowsPos.x, oData.m_pHmd->WindowsPos.y);
    pWindow->GetWindowProperties()->SetSize(oData.m_pHmd->Resolution.w, oData.m_pHmd->Resolution.h);
    pWindow->GetWindowProperties()->SetDrawBorder(false);
  }

  return bSuccess;
}

bool VistaOculusGlutWindowingToolkit::UnregisterWindow(VistaWindow* pWindow) {
  std::map<const VistaWindow*, Internal::WindowDataExt>::const_iterator itEntry =
      m_pData->m_mapWindowData.find(pWindow);
  {
    ovrHmd_Destroy((*itEntry).second.m_pHmd);
    m_pData->m_mapWindowData.erase(itEntry);
  }
  return VistaGlutWindowingToolkit::UnregisterWindow(pWindow);
}

bool VistaOculusGlutWindowingToolkit::InitWindow(VistaWindow* pWindow) {
  if (VistaGlutWindowingToolkit::InitWindow(pWindow) == false)
    return false;
  BindWindow(pWindow);
  std::map<const VistaWindow*, Internal::WindowDataExt>::const_iterator itEntry =
      m_pData->m_mapWindowData.find(pWindow);
  if (itEntry == m_pData->m_mapWindowData.end())
    return true;
    // @IMGTODO: hacky
#ifdef WIN32
  void* pHandle = FindWindow(NULL, pWindow->GetWindowProperties()->GetTitle().c_str());
#else
  void* pHandle = (void*)((long)pWindow->GetWindowProperties()->GetWindowId);
#endif
  ovrHmd_AttachToWindow((*itEntry).second.m_pHmd, pHandle, NULL, NULL);

  // setting capabilities @TODO: how exactly?
  ovrHmd_SetEnabledCaps(
      (*itEntry).second.m_pHmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);
  ovrHmd_ConfigureTracking((*itEntry).second.m_pHmd,
      ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);

  return true;
}

void VistaOculusGlutWindowingToolkit::DisplayWindow(const VistaWindow* pWindow) {
  std::map<const VistaWindow*, Internal::WindowDataExt>::const_iterator itEntry =
      m_pData->m_mapWindowData.find(pWindow);
  if (itEntry == m_pData->m_mapWindowData.end() ||
      pWindow->GetViewport(0)->GetViewportProperties()->GetType() !=
          VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT) {
    VistaGlutWindowingToolkit::DisplayWindow(pWindow);
  } else {
    PushWindow(pWindow);
    // SwapBuffers handled by ovrEndFrame in viewport @IMGTODO
    glutPostRedisplay();
    PopWindow();
  }
}

ovrHmd VistaOculusGlutWindowingToolkit::GetHmdForWindow(VistaWindow* pWindow) {
  std::map<const VistaWindow*, Internal::WindowDataExt>::iterator itEntry =
      m_pData->m_mapWindowData.find(pWindow);
  if (itEntry == m_pData->m_mapWindowData.end())
    return NULL;
  return (*itEntry).second.m_pHmd;
}

bool VistaOculusGlutWindowingToolkit::FillOculusRenderConfigForWindow(
    VistaWindow* pWindow, ovrGLConfig& oOculusConfig) {
  std::map<const VistaWindow*, Internal::WindowDataExt>::iterator itEntry =
      m_pData->m_mapWindowData.find(pWindow);
  if (itEntry == m_pData->m_mapWindowData.end())
    return false;
  oOculusConfig.OGL.Header.API              = ovrRenderAPI_OpenGL;
  oOculusConfig.OGL.Header.BackBufferSize.w = (*itEntry).second.m_pHmd->Resolution.w;
  oOculusConfig.OGL.Header.BackBufferSize.h = (*itEntry).second.m_pHmd->Resolution.h;
  oOculusConfig.OGL.Header.Multisample      = 1;
#ifdef WIN32
  oOculusConfig.OGL.Window = FindWindow(NULL, pWindow->GetWindowProperties()->GetTitle().c_str());
  oOculusConfig.OGL.DC     = GetDC(oOculusConfig.OGL.Window);
#else
  VISTA_FUNCTION_NOT_IMPLEMENTED("::VistaOculusGlutWindowingToolkit::"
                                 "FillOculusRenderConfigForWindow( pWindow, oOculusConfig )");
#endif
  return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
