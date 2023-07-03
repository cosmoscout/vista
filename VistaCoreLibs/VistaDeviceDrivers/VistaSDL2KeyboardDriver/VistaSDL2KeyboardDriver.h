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

#ifndef _VISTASDL2KEYBOARDDRIVER_H
#define _VISTASDL2KEYBOARDDRIVER_H

#include <VistaDeviceDriversBase/Drivers/VistaKeyboardDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <map>
#include <vector>

// Windows DLL build
#if defined(WIN32) && !defined(VISTASDL2KEYBOARDDRIVER_STATIC)
#ifdef VISTASDL2KEYBOARDDRIVER_EXPORTS
#define VISTASDL2KEYBOARDDRIVERAPI __declspec(dllexport)
#else
#define VISTASDL2KEYBOARDDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTASDL2KEYBOARDDRIVERAPI
#endif

class VistaDriverAbstractWindowAspect;

class VISTASDL2KEYBOARDDRIVERAPI VistaSDL2KeyboardDriver : public IVistaKeyboardDriver {
 public:
  VistaSDL2KeyboardDriver(IVistaDriverCreationMethod*);
  virtual ~VistaSDL2KeyboardDriver();

  static void KeyDownFunction(unsigned char, int, int);
  static void KeyUpFunction(unsigned char, int, int);
  static void SpecialKeyDownFunction(int, int, int);
  static void SpecialKeyUpFunction(int, int, int);

 protected:
  virtual bool DoSensorUpdate(VistaType::microtime dTs);

  virtual bool DoConnect();
  virtual bool DoDisconnect();

 private:
  static void SetKeyValue(VistaSDL2KeyboardDriver*, unsigned char ucKey, bool bIsUp, int nModifier);
  static void SetSpecialKeyValue(
      VistaSDL2KeyboardDriver* pKeyboard, int nKeyValue, bool bIsUp, int nModifier);

  struct _sKeyHlp {
    _sKeyHlp()
        : m_nKey(-1)
        , m_nModifier(-1)
        , m_bUpKey(false) {
    }

    _sKeyHlp(int nKey, int nModifier, bool bUp)
        : m_nKey(nKey)
        , m_nModifier(nModifier)
        , m_bUpKey(bUp) {
    }

    int  m_nKey, m_nModifier;
    bool m_bUpKey;
  };

  void Receive(const _sKeyHlp&);

  std::vector<_sKeyHlp> m_vecKeyVec;
  bool                  m_bLastFrameValue;
  bool                  m_bConnected;

  VistaDriverAbstractWindowAspect* m_pWindowAspect;
  VistaMutex                       m_update_vec_lock;
};

class VISTASDL2KEYBOARDDRIVERAPI VistaSDL2KeyboardDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  VistaSDL2KeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();
};

#endif //_VISTAOPENSGSDL2KEYBOARDDRIVER_H
