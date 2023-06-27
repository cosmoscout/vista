/*============================================================================*/
/*                    ViSTA VR toolkit - Jsw Joystick driver                  */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/

#ifndef __VISTAJSWJOYSTICKDRIVER_H
#define __VISTAJSWJOYSTICKDRIVER_H

#ifndef LINUX
#error VistaJswJoystickDriver only works under LINUX
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <jsw.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverWorkspaceAspect;
class IVistaDriverProtocolAspect;
class VistaDriverInfoAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaJswJoystickDriver : public IVistaDeviceDriver {
 public:
  struct _sJoyMeasure {
    time_t m_nAxesTs[16];
    int    m_nAxes[16];
    time_t m_nButtonsTs[32];
    bool   m_nButtons[32];
  };

  VistaJswJoystickDriver(IVistaDriverCreationMethod*);
  virtual ~VistaJswJoystickDriver();

 protected:
  bool DoSensorUpdate(VistaType::microtime nTs);
  /**
   * Connects to the libjsw joystick representation.
   * Use a ProtocolAspect to change calibration file (protocol-revision tag) and device name
   * configuration (protocol-name) for this device.
   * @todo change this to a proper Aspect.
   */
  bool DoConnect();
  bool DoDisconnect();

 private:
  js_data_struct m_jsd;
  bool           m_bOpened;
  _sJoyMeasure   m_nState;

  VistaDriverWorkspaceAspect* m_pWorkspace;
  IVistaDriverProtocolAspect* m_pProtocol;
  VistaDriverInfoAspect*      m_pInfo;
};

class VistaJswJoystickDriverCreationMethod : public IVistaDriverCreationMethod {
 public:
  VistaJswJoystickDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //__VISTAJSWJOYSTICK_H
