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

#if defined(WIN32)

#ifndef __VISTADIRECTXGAMEPAD_H
#define __VISTADIRECTXGAMEPAD_H

#pragma warning(disable : 4996) // disable deprecated warning

#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#define DIRECTINPUT_VERSION 0x0800

#include "VistaDirectXGamepadSensorMeasures.h"
#include <XInput.h> // XInput API
#include <dinput.h>
#include <dinputd.h>
#include <windows.h>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTADIRECTXGAMEPADDRIVER_STATIC)
#ifdef VISTADIRECTXGAMEPADDRIVER_EXPORTS
#define VISTADIRECTXGAMEPADAPI __declspec(dllexport)
#else
#define VISTADIRECTXGAMEPADAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTADIRECTXGAMEPADAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverAbstractWindowAspect;
class IVistaDriverProtocolAspect;
class VistaDirectXGamepadScalarTranscode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * DirectX compatible joystick driver. Supports joysticks with up to two axes.
 * Needs a DirectX SDK installed in order to be compiled. The DirectInput
 * API was frozen somewhat in DirectX 8, so older revisions seem to be fine.
 * The Code supports the usage of DirectX specific force feedback effects,
 * but one needs to know that the DirectX API is going to be used, which is
 * ok in case you want to design custom effects.
 * The driver <b>needs</b> a properly configured and working
 * VistaDriverAbstractWindowAspect in order to work. As it is a windows device, it
 * needs a message port for processing. It gets it by using the window API
 * of the window aspect. The aspect has to be set <i>before</i> a call to connect.
 * The measure struct directly stores a copy of the structure delivered by DirectX
 * so in case you need more information, get the sensor and cast the slot pointer
 * to DIJOYSTATE2 type.
 * @todo add support for workspace aspect
 *
 */
class VISTADIRECTXGAMEPADAPI VistaDirectXGamepad : public IVistaDeviceDriver {
  friend class VistaDirectXForceFeedbackAspect;

 public:
  VistaDirectXGamepad(IVistaDriverCreationMethod* crm);
  ~VistaDirectXGamepad();

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // FORCE FEEDBACK API
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  class VISTADIRECTXGAMEPADAPI VistaDirectXForceFeedbackAspect
      : public IVistaDriverForceFeedbackAspect {
    friend class VistaDirectXGamepad;

   public:
    class VISTADIRECTXGAMEPADAPI Effect {
      friend class VistaDirectXForceFeedbackAspect;

     public:
      bool SetParameters(DIEFFECT& params);
      bool GetParameters(DIEFFECT& params);

      LPDIRECTINPUTEFFECT m_pEffect;

     private:
      Effect();
      ~Effect();
    };

    Effect* CreateEffect(REFGUID effectType);
    bool    DeleteEffect(Effect*);

    bool Start(Effect* pEffect, int nIterations);
    bool Stop(Effect* pEffect);

    virtual bool SetForce(const VistaVector3D& v3Force, const VistaVector3D& v3Ignored);

    bool    SetCurrentEffect(Effect*);
    Effect* GetCurrentEffect() const;

    virtual int GetNumInputDOF() const;
    virtual int GetNumOutputDOF() const;

    virtual bool SetForcesEnabled(bool bEnabled);
    virtual bool GetForcesEnabled() const;

    virtual float GetMaximumStiffness() const;
    virtual float GetMaximumForce() const;

   private:
    VistaDirectXForceFeedbackAspect(VistaDirectXGamepad* pPad);
    virtual ~VistaDirectXForceFeedbackAspect();

    VistaDirectXGamepad* m_pParent;
    std::vector<Effect*> m_vecEffects;
    Effect*              m_pCurrentEffect;
  };

  enum eDriver {
    TP_UNKNOWN = -1,
    TP_XINPUT  = 0, /**< use this for XINPUT style processing */
    TP_DIRECTX      /**< use this for DirectInput style processing */
  };

  unsigned int GetSensorMeasureSize() const;
  // static IVistaDriverCreationMethod *GetDriverFactoryMethod();

  // ########################################################################

  enum ePadType { TP_UNKNWON = -1, TP_RUMBLEPAD = 0, TP_XBOX360 = 1 };

  ePadType GetPadType() const;

  unsigned int GetNumberOfButtons() const;

  bool GetRanges(int& xMin, int& yMin, int& zMin, int& xMax, int& yMax, int& zMax) const;

  /**
   * @param strJoyName the name of the joystick to search and use.
            When no name is set, the <b>default</b> joystick as set in
            the windows settings is used for processing. Otherwise,
            a case-sensitive search during connect is done.
   */
  void SetJoystickName(const std::string& strJoyName);

  /**
   * @return the name set by SetJoystickName()
   */
  std::string GetJoystickName() const;

  VistaDirectXForceFeedbackAspect* GetDirectXForceFeedbackAspect() const;

  virtual bool RegisterAspect(IVistaDeviceDriverAspect* pAspect);

  /**
   * Unregisters an aspect with this driver. Can be used to delete the aspect
   * while unregistering it.
   * @param pAspect the aspect instance to unregister.
   * @param bDelete true iff delete is to be called on the pointer after the
                                    un-registration process.
   * @return false iff the aspect was not registered with the driver instance
   */
  virtual bool UnregisterAspect(IVistaDeviceDriverAspect* pAspect,
      eDeleteAspectBehavior                               bDelete = DELETE_ASPECT,
      eUnregistrationBehavior                             bForce  = DO_NOT_FORCE_UNREGISTRATION);

 protected:
  bool DoSensorUpdate(VistaType::microtime dTs);

  bool DoConnect();
  bool DoDisconnect();

 protected:
 private:
  friend class GamepadAttach;
  bool InitInput();
  bool InitDirectXInput();
  bool UpdateDirectXInput(VistaType::microtime dTs);

  bool InitXInput();

  eDriver              m_eDriver;
  LPDIRECTINPUT8       m_pDI;
  LPDIRECTINPUTDEVICE8 m_pJoystick;
  DIJOYSTATE2          m_oldState;
  unsigned int         m_nNumberOfButtons;

  // ViSTA specific
  VistaDriverAbstractWindowAspect* m_pWindowAspect;
  IVistaDriverProtocolAspect*      m_pProtocol;

  // FORCE FEEDBACK
  IVistaDriverForceFeedbackAspect* m_pForceAspect;
  std::string                      m_strJoyName;
};

class VISTADIRECTXGAMEPADAPI VistaDirectXGamepadCreationMethod : public IVistaDriverCreationMethod {
 public:
  VistaDirectXGamepadCreationMethod(IVistaTranscoderFactoryFactory* fac)
      : IVistaDriverCreationMethod(fac) {
  }

  ~VistaDirectXGamepadCreationMethod() {
    UnregisterType("", false);
  }

  virtual IVistaDeviceDriver* CreateDriver() {
    return new VistaDirectXGamepad(this);
  }
};

// VistaDirectXGamepadCreationMethod *SpFactory = NULL;
/**
 * This is a proxy class for the original driver creation
 * method. The plugin dll is defined in the scope of Vista,
 * as we need a window handle for this driver to function
 * properly. For that, we inject currently a kernel
 * dependency to the VistaWindowAspect, which we create and
 * register with each new driver.
 * for all the other methods, we do a simple forwarding.
 */

class VISTADIRECTXGAMEPADAPI CreateProxy : public IVistaDriverCreationMethod {
 private:
  IVistaDriverCreationMethod* m_pOrig;

 public:
  CreateProxy(IVistaTranscoderFactoryFactory* fac);

  ~CreateProxy();

  virtual IVistaDeviceDriver* CreateDriver();

  // the rest of this API is a simple forwarder!

  //@TODO: pFac needs to be a specific MeasureTranscoderFactory

  virtual unsigned int RegisterSensorType(const std::string& strName, unsigned int nMeasureSize,
      unsigned int nUpdateEstimator, IVistaMeasureTranscoderFactory* pFac,
      const std::string& strTranscoderTypeString);

  virtual bool UnregisterType(const std::string& strType, bool bDeleteFactories = false);

  virtual unsigned int GetTypeFor(const std::string& strType) const;

  virtual bool GetTypeNameFor(unsigned int nTypeId, std::string& strDest) const;

  virtual unsigned int GetMeasureSizeFor(unsigned int nType) const;

  virtual unsigned int GetUpdateEstimatorFor(unsigned int nType) const;

  virtual std::list<std::string> GetTypeNames() const;

  virtual std::list<unsigned int> GetTypes() const;

  virtual IVistaMeasureTranscoderFactory* GetTranscoderFactoryForSensor(unsigned int nType) const;

  virtual IVistaMeasureTranscoderFactory* GetTranscoderFactoryForSensor(
      const std::string& strTypeName) const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //__VISTASYSTEM_H

#endif // WIN32
