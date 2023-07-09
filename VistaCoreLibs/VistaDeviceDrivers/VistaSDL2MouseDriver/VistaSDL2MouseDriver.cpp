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

#include "VistaSDL2MouseDriver.h"
#include "VistaBase/VistaStreamUtils.h"
#include "VistaKernel/VistaSystem.h"
#include <SDL.h>
#include <SDL2/SDL_mouse.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>
#include <algorithm>
#include <cstring>
#include <map>
#include <array>

VistaSDL2MouseDriverCreationMethod::VistaSDL2MouseDriverCreationMethod(
    IVistaTranscoderFactoryFactory* fac)
    : IVistaDriverCreationMethod(fac) {
  RegisterSensorType(
      "", sizeof(IVistaMouseDriver::_sMouseMeasure), 20, fac->CreateFactoryForType(""));
}

IVistaDeviceDriver* VistaSDL2MouseDriverCreationMethod::CreateDriver() {
  return new VistaSDL2MouseDriver(this);
}

VistaSDL2MouseDriver::VistaSDL2MouseDriver(IVistaDriverCreationMethod* crm)
    : IVistaMouseDriver(crm)
    , m_mouseSensor(new VistaDeviceSensor)
    , m_lastFrameValue(false)
    , m_connected(false) {
  Uint8 mouseState = SDL_GetMouseState(nullptr, nullptr);
  m_currentMouseState = mouseState;
  m_lastMouseState = mouseState;

  AddDeviceSensor(m_mouseSensor, 0);
}

VistaSDL2MouseDriver::~VistaSDL2MouseDriver() {
  RemDeviceSensor(m_mouseSensor);
  delete m_mouseSensor;
}

bool VistaSDL2MouseDriver::DoSensorUpdate(VistaType::microtime dTs) {
  if (!m_connected) {
    return true;
  }

  int x;
  int y;
  m_currentMouseState = SDL_GetMouseState(&x, &y);

  double lmb = m_currentMouseState & SDL_BUTTON_LMASK ? 1.0 : 0.0;
  double mmb = m_currentMouseState & SDL_BUTTON_MMASK ? 1.0 : 0.0;
  double rmb = m_currentMouseState & SDL_BUTTON_RMASK ? 1.0 : 0.0;

  MeasureStart(0, dTs);
  UpdateMouseButton(0, IVistaMouseDriver::BT_LEFT, lmb);
  UpdateMouseButton(0, IVistaMouseDriver::BT_MIDDLE, mmb);
  UpdateMouseButton(0, IVistaMouseDriver::BT_RIGHT, rmb);
  UpdateMousePosition(0, x, y);
  MeasureStop(0);

  m_lastMouseState = m_currentMouseState;

  return true;
};

bool VistaSDL2MouseDriver::DoConnect() {
  m_connected = true;
  return true;
}

bool VistaSDL2MouseDriver::DoDisconnect() {
  m_connected = false;
  return true;
}

