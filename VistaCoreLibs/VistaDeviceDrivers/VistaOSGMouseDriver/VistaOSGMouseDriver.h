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

#ifndef _VISTAOSGMOUSEDRIVER_H
#define _VISTAOSGMOUSEDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/Drivers/VistaMouseDriver.h>
#include <VistaKernel/VistaKernelConfig.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAOSGMOUSEDRIVER_STATIC)
#ifdef VISTAOSGMOUSEDRIVER_EXPORTS
#define VISTAOSGMOUSEDRIVERAPI __declspec(dllexport)
#else
#define VISTAOSGMOUSEDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAOSGMOUSEDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaWindow;
class IVistaDriverCreationMethod;
class VistaDriverAbstractWindowAspect;
class VistaOSGWindowingToolkit;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAOSGMOUSEDRIVERAPI VistaOSGMouseDriver : public IVistaMouseDriver {
 public:
  VistaOSGMouseDriver(IVistaDriverCreationMethod* crm);
  virtual ~VistaOSGMouseDriver();

  bool RegisterWindow(VistaWindow* pWindow);
  bool UnregisterWindow(VistaWindow* pWindow);

 protected:
  virtual bool DoConnect();
  virtual bool DoDisconnect();

  virtual bool DoSensorUpdate(VistaType::microtime dTs);

 private:
  VistaDriverAbstractWindowAspect* m_pWindowAspect;
  VistaOSGWindowingToolkit*        m_pWindowingToolkit;
  struct WindowInfo {
    unsigned int m_nSensorIndex;
    bool         m_nButtonStates[3];
    int          m_nWheelNumber;
    int          m_nWheelDirection;
  };
  typedef std::map<VistaWindow*, WindowInfo> WindowMap;
  WindowMap                                  m_mapWindows;
  bool                                       m_bGrabCursorChanged;
};

class VISTAOSGMOUSEDRIVERAPI VistaOSGMouseDriverCreationMethod : public IVistaDriverCreationMethod {
 public:
  VistaOSGMouseDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();

 protected:
 private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
