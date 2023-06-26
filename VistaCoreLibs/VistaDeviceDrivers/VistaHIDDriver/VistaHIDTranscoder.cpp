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

/* The Windows-specific code (especially the InitDriver routine) has been
 * copied from the VRPN toolkit, which is released in the public domain.
 * Those program parts stay in the public domain, eventually to be replaced by
 * an own implementation.
 * All other parts are Copyright of RWTH Aachen university and covered under the
 * terms of the GNU Lesser General Public License, as stated above.
 */

#include "VistaHIDCommonShare.h"

#include <iomanip>
#include <iostream>
#include <string>

#include <cstdio>

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

namespace {

/* -------------------------------------------------------------------------- */
/*
         VistaHIDDriverRawTranscode

         exports measure as coming from the driver in the properties

         AXIS_0 - AXIS_15       : double
         BUTTON_0 - BUTTON_511  : bool
*/

class VistaHIDDriverRawTranscode : public IVistaMeasureTranscode {
 public:
  VistaHIDDriverRawTranscode();
  virtual ~VistaHIDDriverRawTranscode(){};

  static std::string GetTypeString();

  REFL_INLINEIMP(VistaHIDDriverRawTranscode, IVistaMeasureTranscode);
};

VistaHIDDriverRawTranscode::VistaHIDDriverRawTranscode() {
  // inherited as protected member
  m_nNumberOfScalars = 544;
}

std::string VistaHIDDriverRawTranscode::GetTypeString() {
  return "VistaHIDDriverRawTranscode";
}

class VistaHIDDriverRawAxisGet : public IVistaMeasureTranscode::TTranscodeValueGet<double> {
 public:
  VistaHIDDriverRawAxisGet(const std::string& sPropname, const std::string& sClassname,
      const std::string& sDescription, int index);

  virtual bool   GetValue(const VistaSensorMeasure* pMeasure, double& dScalar) const;
  virtual double GetValue(const VistaSensorMeasure* pMeasure) const;

 private:
  int m_nIndex;
};

VistaHIDDriverRawAxisGet::VistaHIDDriverRawAxisGet(const std::string& sPropname,
    const std::string& sClassname, const std::string& sDescription, int index)
    : IVistaMeasureTranscode::TTranscodeValueGet<double>(sPropname, sClassname, sDescription)
    , m_nIndex(index) {
}

bool VistaHIDDriverRawAxisGet::GetValue(const VistaSensorMeasure* pMeasure, double& dScalar) const {
  if (!pMeasure)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  dScalar                                   = m->nAxes[m_nIndex];
  return true;
}

double VistaHIDDriverRawAxisGet::GetValue(const VistaSensorMeasure* pMeasure) const {
  if (!pMeasure)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  return m->nAxes[m_nIndex];
}

class VistaHIDDriverRawButtonGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  VistaHIDDriverRawButtonGet(const std::string& sPropname, const std::string& sClassname,
      const std::string& sDescription, int index);

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, bool& bScalar) const;
  virtual bool GetValue(const VistaSensorMeasure* pMeasure) const;

 private:
  int m_nIndex;
};

VistaHIDDriverRawButtonGet::VistaHIDDriverRawButtonGet(const std::string& sPropname,
    const std::string& sClassname, const std::string& sDescription, int index)
    : IVistaMeasureTranscode::TTranscodeValueGet<bool>(sPropname, sClassname, sDescription)
    , m_nIndex(index) {
}

bool VistaHIDDriverRawButtonGet::GetValue(const VistaSensorMeasure* pMeasure, bool& bScalar) const {
  if (!pMeasure)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  bScalar                                   = (m->nButtons[m_nIndex] ? true : false);
  return true;
}

bool VistaHIDDriverRawButtonGet::GetValue(const VistaSensorMeasure* pMeasure) const {
  if (!pMeasure)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  return (m->nButtons[m_nIndex] ? true : false);
}

class _CHlpRawGetCreate {
 public:
  _CHlpRawGetCreate() {
    char prop[11];
    char desc[15];

    // register 32 axis getter
    for (int i = 0; i < 32; i++) {
      sprintf(prop, "AXIS_%d", i);
      sprintf(desc, "HID Axis %d", i);
      new VistaHIDDriverRawAxisGet(prop, VistaHIDDriverRawTranscode::GetTypeString(), desc, i);
    }

    // register 512 button getter
    for (int i = 0; i < 512; i++) {
      sprintf(prop, "BUTTON_%d", i);
      sprintf(desc, "HID Button %d", i);
      new VistaHIDDriverRawButtonGet(prop, VistaHIDDriverRawTranscode::GetTypeString(), desc, i);
    }
  }
};

class VistaHIDDriverRawTranscodeFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder();
};

IVistaMeasureTranscode* VistaHIDDriverRawTranscodeFactory::CreateTranscoder() {
  return new VistaHIDDriverRawTranscode;
}

/* -------------------------------------------------------------------------- */
/*
   VistaHIDDriverSpaceNavigatorTranscode

   output slots are mapped like this:

   axes: 0-5
   0: shift left/right
   1: shift forward/backward
   2: shift up/down
   3: tilt forward/backward
   4: tilt left/right
   5: twist
   are returned by Pos/Ori transcode as vector/quat

   buttons: 0-1
   0: button1
   1: button2
   are returned by scalar transcode
*/

class VistaHIDDriverSpaceNavigatorTranscode : public IVistaMeasureTranscode {
 public:
  VistaHIDDriverSpaceNavigatorTranscode();
  virtual ~VistaHIDDriverSpaceNavigatorTranscode(){};

  static std::string GetTypeString();

  REFL_INLINEIMP(VistaHIDDriverSpaceNavigatorTranscode, IVistaMeasureTranscode);
};

VistaHIDDriverSpaceNavigatorTranscode::VistaHIDDriverSpaceNavigatorTranscode() {
  // inherited as protected member
  m_nNumberOfScalars = 2;
}

std::string VistaHIDDriverSpaceNavigatorTranscode::GetTypeString() {
  return "VistaHIDDriverSpaceNavigatorTranscode";
}

class VistaHIDDriverSpaceNavigatorPosGet : public IVistaMeasureTranscode::V3Get {
 public:
  VistaHIDDriverSpaceNavigatorPosGet();

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const;
  virtual bool          GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const;
};

VistaHIDDriverSpaceNavigatorPosGet::VistaHIDDriverSpaceNavigatorPosGet()
    : IVistaMeasureTranscode::V3Get("POSITION",
          VistaHIDDriverSpaceNavigatorTranscode::GetTypeString(),
          "translation offset of the 3 space-nav axes") {
}

VistaVector3D VistaHIDDriverSpaceNavigatorPosGet::GetValue(
    const VistaSensorMeasure* pMeasure) const {
  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  return VistaVector3D(float(m->nAxes[0]), float(m->nAxes[1]), float(m->nAxes[2]));
}
bool VistaHIDDriverSpaceNavigatorPosGet::GetValue(
    const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const {
  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  v3Pos[0]                                  = float(m->nAxes[0]);
  v3Pos[1]                                  = float(-m->nAxes[2]);
  v3Pos[2]                                  = float(m->nAxes[1]);
  return true;
}

class VistaHIDDriverSpaceNavigatorOriGet : public IVistaMeasureTranscode::QuatGet {
 public:
  VistaHIDDriverSpaceNavigatorOriGet();

  virtual VistaQuaternion GetValue(const VistaSensorMeasure* pMeasure) const;
  virtual bool            GetValue(const VistaSensorMeasure* pMeasure, VistaQuaternion& qOri) const;
};

VistaHIDDriverSpaceNavigatorOriGet::VistaHIDDriverSpaceNavigatorOriGet()
    : IVistaMeasureTranscode::QuatGet("ORIENTATION",
          VistaHIDDriverSpaceNavigatorTranscode::GetTypeString(),
          "orientation of the space-nav in rotation around coordinate axes") {
}

VistaQuaternion VistaHIDDriverSpaceNavigatorOriGet::GetValue(
    const VistaSensorMeasure* pMeasure) const {
  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();

  VistaAxisAndAngle    a1(VistaVector3D(1, 0, 0), m->nAxes[3] / 200.0f);
  VistaTransformMatrix xrot(a1);

  VistaAxisAndAngle    a2(VistaVector3D(0, 1, 0), -m->nAxes[5] / 200.0f);
  VistaTransformMatrix yrot(a2);

  VistaAxisAndAngle    a3(VistaVector3D(0, 0, 1), m->nAxes[4] / 200.0f);
  VistaTransformMatrix zrot(a3);

  VistaTransformMatrix result = xrot * yrot * zrot;
  return VistaQuaternion(result);
}
bool VistaHIDDriverSpaceNavigatorOriGet::GetValue(
    const VistaSensorMeasure* pMeasure, VistaQuaternion& qOri) const {
  qOri = GetValue(pMeasure);
  return true;
}

class VistaHIDDriverSpaceNavigatorScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VistaHIDDriverSpaceNavigatorScalarGet();

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const;
};

VistaHIDDriverSpaceNavigatorScalarGet::VistaHIDDriverSpaceNavigatorScalarGet()
    : IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
          VistaHIDDriverSpaceNavigatorTranscode::GetTypeString(), "the space-nav buttons 1 and 2") {
}

bool VistaHIDDriverSpaceNavigatorScalarGet::GetValueIndexed(
    const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const {
  if (!pMeasure || nIndex > 1)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  dScalar                                   = m->nButtons[256 + nIndex];
  return true;
}

class VistaHIDDriverSpaceNavigatorFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder();
};

IVistaMeasureTranscode* VistaHIDDriverSpaceNavigatorFactory::CreateTranscoder() {
  return new VistaHIDDriverSpaceNavigatorTranscode;
}

/* -------------------------------------------------------------------------- */

/*
   VistaHIDDriverSpaceNavigatorCmdTranscode

   output slots are mapped like this:

   axes: 0-5
   0: shift left/right
   1: shift forward/backward
   2: shift up/down
   3: tilt forward/backward
   4: tilt left/right
   5: twist
   buttons: 6-7
   6: button1
   7: button2

   all values returned by scalar transcode
*/

class VistaHIDDriverSpaceNavigatorCmdTranscode : public IVistaMeasureTranscode {
 public:
  VistaHIDDriverSpaceNavigatorCmdTranscode();
  virtual ~VistaHIDDriverSpaceNavigatorCmdTranscode(){};

  static std::string GetTypeString();

  REFL_INLINEIMP(VistaHIDDriverSpaceNavigatorCmdTranscode, IVistaMeasureTranscode);
};

VistaHIDDriverSpaceNavigatorCmdTranscode::VistaHIDDriverSpaceNavigatorCmdTranscode() {
  // inherited as protected member
  m_nNumberOfScalars = 8;
}

std::string VistaHIDDriverSpaceNavigatorCmdTranscode::GetTypeString() {
  return "VistaHIDDriverSpaceNavigatorCmdTranscode";
}

class VistaHIDDriverSpaceNavigatorCmdScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VistaHIDDriverSpaceNavigatorCmdScalarGet();

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const;
};

VistaHIDDriverSpaceNavigatorCmdScalarGet::VistaHIDDriverSpaceNavigatorCmdScalarGet()
    : IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
          VistaHIDDriverSpaceNavigatorCmdTranscode::GetTypeString(),
          "spacenav axes 0-5 and buttons 6-7") {
}

bool VistaHIDDriverSpaceNavigatorCmdScalarGet::GetValueIndexed(
    const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const {
  if (!pMeasure || nIndex > 7)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();
  /*if( nIndex == 0 )
    dScalar =	 m->nAxes[0];
    if( nIndex == 1 )
    dScalar = -m->nAxes[2];
    if( nIndex == 2 )
    dScalar =	 m->nAxes[1];
    if( nIndex == 3 )
    dScalar =	 m->nAxes[3];
    if( nIndex == 4 )
    dScalar =	 m->nAxes[5];
    if( nIndex == 5 )
    dScalar =	 m->nAxes[4];
    if( nIndex >= 6 && nIndex <= 7 )
    dScalar = m->nButtons[256+nIndex-6];*/
  if (nIndex == 0)
    dScalar = m->nAxes[0];
  if (nIndex == 1)
    dScalar = m->nAxes[1];
  if (nIndex == 2)
    dScalar = m->nAxes[2];
  if (nIndex == 3)
    dScalar = m->nAxes[3];
  if (nIndex == 4)
    dScalar = m->nAxes[4];
  if (nIndex == 5)
    dScalar = m->nAxes[5];
  if (nIndex >= 6 && nIndex <= 7)
    dScalar = m->nButtons[nIndex - 6 + 256];
  return true;
}

class VistaHIDDriverSpaceNavigatorCmdFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder();
};

IVistaMeasureTranscode* VistaHIDDriverSpaceNavigatorCmdFactory::CreateTranscoder() {
  return new VistaHIDDriverSpaceNavigatorCmdTranscode;
}

/* -------------------------------------------------------------------------- */

/*
   VistaHIDDriverLogitechPadTranscode

   output slots are mapped like this:

   0-3:	 axes
   4-13: buttons

   0: left anvddlog stick horizontal axis
   1: left anvddlog stick vertical axis
   2: right anvddlog stick horizontal axis
   3: right anvddlog stick vertical axis

   4-13: buttons 1..10
*/

class VistaHIDDriverLogitechPadTranscode : public IVistaMeasureTranscode {
 public:
  VistaHIDDriverLogitechPadTranscode();
  virtual ~VistaHIDDriverLogitechPadTranscode(){};

  static std::string GetTypeString();

  REFL_INLINEIMP(VistaHIDDriverLogitechPadTranscode, IVistaMeasureTranscode);
};

VistaHIDDriverLogitechPadTranscode::VistaHIDDriverLogitechPadTranscode() {
  // inherited as protected member
  m_nNumberOfScalars = 14;
}

std::string VistaHIDDriverLogitechPadTranscode::GetTypeString() {
  return "VistaHIDDriverLogitechPadTranscode";
}

class VistaHIDDriverLogitechPadScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VistaHIDDriverLogitechPadScalarGet();

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const;
};

VistaHIDDriverLogitechPadScalarGet::VistaHIDDriverLogitechPadScalarGet()
    : IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
          VistaHIDDriverLogitechPadTranscode::GetTypeString(),
          "the logitech gamepads axes [0-3] and buttons [4-13]") {
}

bool VistaHIDDriverLogitechPadScalarGet::GetValueIndexed(
    const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const {
  if (!pMeasure || nIndex > 13)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();

  if (nIndex == 0)
    dScalar = m->nAxes[0];
  if (nIndex == 1)
    dScalar = m->nAxes[1];
  if (nIndex == 2)
    dScalar = m->nAxes[2];
  if (nIndex == 3)
    dScalar = m->nAxes[5];
  if (nIndex >= 4 && nIndex <= 13)
    dScalar = m->nButtons[300 + nIndex]; // button 1 has keycode 304
  return true;
}

class VistaHIDDriverLogitechPadFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder();
};

IVistaMeasureTranscode* VistaHIDDriverLogitechPadFactory::CreateTranscoder() {
  return new VistaHIDDriverLogitechPadTranscode;
}

/* -------------------------------------------------------------------------- */

/*
   VistaHIDDriverPowerMateTranscode

   Device supports only one relative axis (gives increments 1/-1)
   and one button, which is 0 or 1.

   dscalar output slots are mapped like this:

   0:	 axis
   1:    button
*/

class VistaHIDDriverPowerMateTranscode : public IVistaMeasureTranscode {
 public:
  VistaHIDDriverPowerMateTranscode();
  virtual ~VistaHIDDriverPowerMateTranscode(){};

  static std::string GetTypeString();

  REFL_INLINEIMP(VistaHIDDriverPowerMateTranscode, IVistaMeasureTranscode);
};

VistaHIDDriverPowerMateTranscode::VistaHIDDriverPowerMateTranscode() {
  m_nNumberOfScalars = 2;
}

std::string VistaHIDDriverPowerMateTranscode::GetTypeString() {
  return "VistaHIDDriverPowerMateTranscode";
}

class VistaHIDDriverPowerMateScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VistaHIDDriverPowerMateScalarGet();

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const;
};

VistaHIDDriverPowerMateScalarGet::VistaHIDDriverPowerMateScalarGet()
    : IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
          VistaHIDDriverPowerMateTranscode::GetTypeString(),
          "the Griffin PowerMate's axis [0] and button [1]") {
}

bool VistaHIDDriverPowerMateScalarGet::GetValueIndexed(
    const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const {
  if (!pMeasure || nIndex > 1)
    return false;

  const VistaHIDCommonShare::sHIDMeasure* m = pMeasure->getRead<VistaHIDCommonShare::sHIDMeasure>();

  if (nIndex == 0)
    dScalar = m->nAxes[7];
  if (nIndex == 1)
    dScalar = m->nButtons[256];

  return true;
}

class VistaHIDDriverPowerMateFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder();
};

IVistaMeasureTranscode* VistaHIDDriverPowerMateFactory::CreateTranscoder() {
  return new VistaHIDDriverPowerMateTranscode;
}

/* -------------------------------------------------------------------------- */

// static initialization of get-functors

IVistaPropertyGetFunctor* SaGetter[] = {new VistaHIDDriverSpaceNavigatorPosGet,
    new VistaHIDDriverSpaceNavigatorOriGet, new VistaHIDDriverSpaceNavigatorScalarGet,
    new VistaHIDDriverSpaceNavigatorCmdScalarGet,

    new VistaHIDDriverLogitechPadScalarGet, new VistaHIDDriverPowerMateScalarGet};

_CHlpRawGetCreate helper_get_create;

class VistaHIDDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory {
 public:
  VistaHIDDriverTranscoderFactoryFactory() {
    CreateCreators(m_creators);
  }

  ~VistaHIDDriverTranscoderFactoryFactory() {
    CleanupCreators(m_creators);
  }

  typedef std::map<std::string, ICreateTranscoder*> CRMAP;
  CRMAP                                             m_creators;

  static void CreateCreators(CRMAP& mp) {
    mp["HID_RAW"]                    = new TCreateTranscoder<VistaHIDDriverRawTranscode>;
    mp["3DCONNEXION_SPACENAVIGATOR"] = new TCreateTranscoder<VistaHIDDriverSpaceNavigatorTranscode>;
    mp["3DCONNEXION_SPACENAVIGATOR_CMD"] =
        new TCreateTranscoder<VistaHIDDriverSpaceNavigatorCmdTranscode>;
    mp["LOGITECH_RUMBLEPAD2"] = new TCreateTranscoder<VistaHIDDriverLogitechPadTranscode>;
    mp["GRIFFIN_POWERMATE"]   = new TCreateTranscoder<VistaHIDDriverPowerMateTranscode>;
  }

  static void CleanupCreators(CRMAP& mp) {
    for (CRMAP::iterator it = mp.begin(); it != mp.end(); ++it)
      delete (*it).second;

    mp.clear();
  }

  virtual IVistaMeasureTranscoderFactory* CreateFactoryForType(const std::string& strTypeName) {
    CRMAP::const_iterator it = m_creators.find(strTypeName);
    if (it == m_creators.end())
      return NULL;
    return (*it).second->Create();
  }

  virtual void DestroyTranscoderFactory(IVistaMeasureTranscoderFactory* fac) {
    delete fac;
  }

  static void OnUnload() {
    CRMAP mp;
    CreateCreators(mp);
    for (CRMAP::iterator it = mp.begin(); it != mp.end(); ++it)
      (*it).second->OnUnload();
    CleanupCreators(mp);
  }
};

} // namespace

#ifdef VISTAHIDTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(VistaHIDDriverTranscoderFactoryFactory)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(VistaHIDDriverTranscoderFactoryFactory)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaHIDDriverTranscoderFactoryFactory)
