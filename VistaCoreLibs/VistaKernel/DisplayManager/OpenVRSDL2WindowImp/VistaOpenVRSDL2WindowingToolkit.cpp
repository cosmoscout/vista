/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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
/*============================================================================*/

#include "VistaOpenVRSDL2WindowingToolkit.h"

#include "../VistaViewport.h"
#include "VistaBase/VistaTimeUtils.h"

VistaOpenVRSDL2WindowingToolkit::VistaOpenVRSDL2WindowingToolkit()
    : VistaSDL2WindowingToolkit()
    , m_vrSystem(nullptr) {

  if (!vr::VR_IsHmdPresent()) {
    vstr::errp()
        << "Error: No OpenVR HMD present while constructing VistaOpenVRSDL2WindowingToolkit!\n";
  }

  vr::EVRInitError error = vr::VRInitError_None;
  m_vrSystem             = vr::VR_Init(&error, vr::VRApplication_Scene);

  if (error != 0) {
    vstr::errp() << "Error while connecting to OpenVR API in VistaOpenVRSDL2WindowingToolkit ctor: "
                 << vr::VR_GetVRInitErrorAsEnglishDescription(error) << std::endl;
  }

  if (!vr::VRCompositor()) {
    vstr::errp() << "Error: OpenVR compositor initialization failed in "
                    "VistaOpenVRSDL2WindowingToolkit ctor!\n";
  }
}

VistaOpenVRSDL2WindowingToolkit::~VistaOpenVRSDL2WindowingToolkit() {
  vr::VR_Shutdown();
}

bool VistaOpenVRSDL2WindowingToolkit::RegisterWindow(VistaWindow* window) {
  bool bSuccess = VistaSDL2WindowingToolkit::RegisterWindow(window);

  uint32_t width, height;
  m_vrSystem->GetRecommendedRenderTargetSize(&width, &height);
  window->GetWindowProperties()->SetSize(width, height);

  return bSuccess;
}

bool VistaOpenVRSDL2WindowingToolkit::InitWindow(VistaWindow* window) {
  if (!VistaSDL2WindowingToolkit::InitWindow(window)) {
    vstr::errp() << "Couldn't initialize Window for a OpenVR application." << std::endl;
    return false;
  }

  BindWindow(window);

  return true;
}
void VistaOpenVRSDL2WindowingToolkit::DisplayWindow(const VistaWindow* window) {
  VistaSDL2WindowingToolkit::DisplayWindow(window);
  vr::VRCompositor()->PostPresentHandoff();
}

vr::IVRSystem* VistaOpenVRSDL2WindowingToolkit::GetVRSystem() {
  return m_vrSystem;
}
