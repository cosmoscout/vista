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

#ifndef _VISTAOPENVRSDL2WINDOWINGTOOLKIT_H
#define _VISTAOPENVRSDL2WINDOWINGTOOLKIT_H

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/DisplayManager/SDL2WindowImp/VistaSDL2WindowingToolkit.h>

#include <openvr/openvr.h>
#include <openvr/openvr_capi.h>

/**
 * OpenVR Window extensions around the SDL2 windowing toolkit
 */
class VISTAKERNELAPI VistaOpenVRSDL2WindowingToolkit : public VistaSDL2WindowingToolkit {
 public:
  VistaOpenVRSDL2WindowingToolkit();
  ~VistaOpenVRSDL2WindowingToolkit();

  void DisplayWindow(const VistaWindow* window) final;

  bool RegisterWindow(VistaWindow* window) final;

  vr::IVRSystem* GetVRSystem();

  bool InitWindow(VistaWindow* window) final;

 private:
  vr::IVRSystem* m_vrSystem;
};

#endif // _VISTAOPENVRSDL2WINDOWINGTOOLKIT_H
