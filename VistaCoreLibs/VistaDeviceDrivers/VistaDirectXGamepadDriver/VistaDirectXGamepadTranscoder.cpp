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

#include "VistaDirectXGamepadSensorMeasures.h"

#if defined(WIN32)
#include <windows.h>
#endif

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

#if !defined(WIN32)
typedef unsigned long  DWORD;
typedef unsigned short WORD;
#endif

#if !defined(LOWORD)
#define LOWORD(I) ((WORD)I)
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

namespace {
class VistaDirectXGamepadMeasureTranscode : public IVistaMeasureTranscode {
 public:
  VistaDirectXGamepadMeasureTranscode() {
    m_nNumberOfScalars = 0;
  }

  virtual ~VistaDirectXGamepadMeasureTranscode() {
  }
  static std::string GetTypeString() {
    return "VistaDirectXGamepadMeasureTranscode";
  }
  REFL_INLINEIMP(VistaDirectXGamepadMeasureTranscode, IVistaMeasureTranscode);
};

class VistaDirectXGamepadScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VistaDirectXGamepadScalarTranscode()
      : IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
            VistaDirectXGamepadMeasureTranscode::GetTypeString(),
            "get scalars as doubles from directX gamepad")
      , m_nBtOffset(0) {
  }

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const {
    if (!pMeasure)
      return false;

    VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure* m =
        (VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure*)&(*pMeasure)
            .m_vecMeasures[0];
    m_nBtOffset = (unsigned int)(m->m_iButtonCount);

    if (nIndex < m_nBtOffset) {
      dScalar = m->m_pDirectXJoystate2.rgbButtons[nIndex] ? 1.0 : 0.0;
      return true;
    }

    int nNormIdx = nIndex - m_nBtOffset;
    switch (nNormIdx) {
    case VistaDirectXGamepadSensorMeasures::AXIS_X1:
      dScalar = m->m_pDirectXJoystate2.lX;
      return true;
    case VistaDirectXGamepadSensorMeasures::AXIS_Y1:
      dScalar = m->m_pDirectXJoystate2.lY;
      return true;
    case VistaDirectXGamepadSensorMeasures::AXIS_X2:
      dScalar = m->m_pDirectXJoystate2.lRx;
      return true;
    case VistaDirectXGamepadSensorMeasures::AXIS_Y2:
      dScalar = m->m_pDirectXJoystate2.lRy;
      return true;
    case VistaDirectXGamepadSensorMeasures::AXIS_Z1:
      dScalar = m->m_pDirectXJoystate2.lZ;
      return true;
    case VistaDirectXGamepadSensorMeasures::AXIS_Z2:
      dScalar = m->m_pDirectXJoystate2.lRz;
      return true;
    case VistaDirectXGamepadSensorMeasures::AXIS_POV:
      if ((LOWORD(m->m_pDirectXJoystate2.rgdwPOV[0]) == 0xFFFF))
        dScalar = -1.0;
      else
        dScalar = m->m_pDirectXJoystate2.rgdwPOV[0];
      return true;

    default:
      break;
    }
    //  LONG    lX;                     /* x-axis position              */
    //  LONG    lY;                     /* y-axis position              */
    //  LONG    lZ;                     /* z-axis position              */
    //  LONG    lRx;                    /* x-axis rotation              */
    //  LONG    lRy;                    /* y-axis rotation              */
    //  LONG    lRz;                    /* z-axis rotation              */
    //  LONG    rglSlider[2];           /* extra axes positions         */
    //  DWORD   rgdwPOV[4];             /* POV directions               */
    //  BYTE    rgbButtons[128];        /* 128 buttons                  */
    //  LONG    lVX;                    /* x-axis velocity              */
    //  LONG    lVY;                    /* y-axis velocity              */
    //  LONG    lVZ;                    /* z-axis velocity              */
    //  LONG    lVRx;                   /* x-axis angular velocity      */
    //  LONG    lVRy;                   /* y-axis angular velocity      */
    //  LONG    lVRz;                   /* z-axis angular velocity      */
    //  LONG    rglVSlider[2];          /* extra axes velocities        */
    //  LONG    lAX;                    /* x-axis acceleration          */
    //  LONG    lAY;                    /* y-axis acceleration          */
    //  LONG    lAZ;                    /* z-axis acceleration          */
    //  LONG    lARx;                   /* x-axis angular acceleration  */
    //  LONG    lARy;                   /* y-axis angular acceleration  */
    //  LONG    lARz;                   /* z-axis angular acceleration  */
    //  LONG    rglASlider[2];          /* extra axes accelerations     */
    //  LONG    lFX;                    /* x-axis force                 */
    //  LONG    lFY;                    /* y-axis force                 */
    //  LONG    lFZ;                    /* z-axis force                 */
    //  LONG    lFRx;                   /* x-axis torque                */
    //  LONG    lFRy;                   /* y-axis torque                */
    //  LONG    lFRz;                   /* z-axis torque                */
    //  LONG    rglFSlider[2];          /* extra axes forces            */
    //      }
    // dScalar = m->m_nButtons[nIndex];
    return false;
  }

  mutable unsigned int m_nBtOffset;
};

class VistaDirectXGamepadAxisVectorTranscode : public IVistaMeasureTranscode::V3Get {
 public:
  enum eMode {
    TR_POS = 0,
    TR_ROT,
    TR_VEL,
    TR_AVEL,
    TR_ACCEL,
    TR_AACCEL,
    TR_FORCE,
    TR_TORQUE,
    TR_AXIS1,
    TR_AXIS2,
    TR_POVAXIS,
  };
  VistaDirectXGamepadAxisVectorTranscode(
      eMode eMd, const std::string& strName, const std::string& strDesc)
      : IVistaMeasureTranscode::V3Get(
            strName, VistaDirectXGamepadMeasureTranscode::GetTypeString(), strDesc)
      , m_eMode(eMd) {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pM) const {
    VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure* m =
        (VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure*)&(*pM)
            .m_vecMeasures[0];

    switch (m_eMode) {
    case TR_AXIS2: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lZ), float(m->m_pDirectXJoystate2.lRz), 0);
    }
    case TR_AXIS1: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lX), float(m->m_pDirectXJoystate2.lY), 0);
    }
    case TR_POS: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lX), float(m->m_pDirectXJoystate2.lY),
          float(m->m_pDirectXJoystate2.lZ));
    }
    case TR_ROT: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lRx), float(m->m_pDirectXJoystate2.lRy),
          float(m->m_pDirectXJoystate2.lRz));
    }
    case TR_VEL: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lVX), float(m->m_pDirectXJoystate2.lVY),
          float(m->m_pDirectXJoystate2.lVZ));
    }
    case TR_AVEL: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lVRx), float(m->m_pDirectXJoystate2.lVRy),
          float(m->m_pDirectXJoystate2.lVRz));
    }
    case TR_ACCEL: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lAX), float(m->m_pDirectXJoystate2.lAY),
          float(m->m_pDirectXJoystate2.lAZ));
    }
    case TR_AACCEL: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lARx), float(m->m_pDirectXJoystate2.lARy),
          float(m->m_pDirectXJoystate2.lARz));
    }
    case TR_FORCE: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lFX), float(m->m_pDirectXJoystate2.lFY),
          float(m->m_pDirectXJoystate2.lFZ));
    }
    case TR_TORQUE: {
      return VistaVector3D(float(m->m_pDirectXJoystate2.lFRx), float(m->m_pDirectXJoystate2.lFRy),
          float(m->m_pDirectXJoystate2.lFRz));
    }
    case TR_POVAXIS: {
      return GetPOV2Vec(m->m_pDirectXJoystate2.rgdwPOV);
    }
    default:
      break;
    }
    return VistaVector3D();
  }

  virtual bool GetValue(const VistaSensorMeasure* pM, VistaVector3D& v3Out) const {
    if (!pM)
      return false;

    v3Out = GetValue(pM);
    return true;
  }

  static VistaVector3D GetPOV2Vec(DWORD rgdwPOV[4]) {
    if ((LOWORD(rgdwPOV[0]) == 0xFFFF))
      return VistaVector3D(0, 0, 0); // not pressed

    DWORD val = rgdwPOV[0];
    switch (val) {
    case 0: {
      // ahead
      return VistaVector3D(0, 0, -1);
    }
    case 4500: {
      // upper right
      VistaVector3D vRet(1, 0, -1);
      vRet.Normalize();
      return vRet;
    }
    case 9000: {
      // right
      return VistaVector3D(1, 0, 0);
    }
    case 13500: {
      // lower right
      VistaVector3D vRet(1, 0, 1);
      vRet.Normalize();
      return vRet;
    }
    case 18000: {
      // back
      VistaVector3D vRet(0, 0, 1);
      vRet.Normalize();
      return vRet;
    }
    case 22500: {
      // lower left
      VistaVector3D vRet(-1, 0, 1);
      vRet.Normalize();
      return vRet;
    }
    case 27000: {
      // left
      return VistaVector3D(-1, 0, 0);
    }
    case 31500: {
      // upper left
      VistaVector3D vRet(-1, 0, -1);
      vRet.Normalize();
      return vRet;
    }
    default:
      break;
    }

    return VistaVector3D(); // no value
  }

 private:
  eMode m_eMode;
};

class VistaDirectXGamepadPOVGetTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaDirectXGamepadPOVGetTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<int>("POV",
            VistaDirectXGamepadMeasureTranscode::GetTypeString(),
            "return the angle value of the POV") {
  }

  virtual int GetValue(const VistaSensorMeasure* pM) const {
    VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure* m =
        (VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure*)&(*pM)
            .m_vecMeasures[0];

    if ((LOWORD(m->m_pDirectXJoystate2.rgdwPOV[0]) == 0xFFFF))
      return -1;
    else
      return (m->m_pDirectXJoystate2.rgdwPOV[0]);
  }

  virtual bool GetValue(const VistaSensorMeasure* pM, int& nOut) const {
    if (!pM)
      return false;

    nOut = GetValue(pM);
    return true;
  }
};

class VistaDirectXGamepadDIJOYSTATE2Get
    : public IVistaMeasureTranscode::TTranscodeValueGet<
          VistaDirectXGamepadSensorMeasures::VISTADIRECTXJOYSTATE2> {
 public:
  VistaDirectXGamepadDIJOYSTATE2Get()
      : IVistaMeasureTranscode::TTranscodeValueGet<
            VistaDirectXGamepadSensorMeasures::VISTADIRECTXJOYSTATE2>("DIJOYSTATE",
            VistaDirectXGamepadMeasureTranscode::GetTypeString(),
            "return the raw DIJOYSTATE/VISTADIRECTXJOYSTAT2 from DI") {
  }

  virtual VistaDirectXGamepadSensorMeasures::VISTADIRECTXJOYSTATE2 GetValue(
      const VistaSensorMeasure* pM) const {
    VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure* m =
        (VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure*)&(*pM)
            .m_vecMeasures[0];
    return m->m_pDirectXJoystate2;
  }

  virtual bool GetValue(const VistaSensorMeasure*               pM,
      VistaDirectXGamepadSensorMeasures::VISTADIRECTXJOYSTATE2& oOut) const {
    if (!pM)
      return false;

    VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure* m =
        (VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure*)&(*pM)
            .m_vecMeasures[0];
    oOut = m->m_pDirectXJoystate2;
    return true;
  }
};

class VistaDirectXGamepadPOVRotGet : public IVistaMeasureTranscode::QuatGet {
 public:
  VistaDirectXGamepadPOVRotGet()
      : IVistaMeasureTranscode::QuatGet("POVROT",
            VistaDirectXGamepadMeasureTranscode::GetTypeString(),
            "the POV as a quat, rotating 0,0,-1 to the axis of the POV in the xz plane") {
  }

  virtual VistaQuaternion GetValue(const VistaSensorMeasure* pM) const {
    VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure* m =
        (VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure*)&(*pM)
            .m_vecMeasures[0];

    if ((LOWORD(m->m_pDirectXJoystate2.rgdwPOV[0]) == 0xFFFF))
      return VistaQuaternion(0, 0, 0, 1); // zero rotation
    else {
      VistaVector3D vRes =
          VistaDirectXGamepadAxisVectorTranscode::GetPOV2Vec(m->m_pDirectXJoystate2.rgdwPOV);
      return VistaQuaternion(VistaVector3D(0, 0, -1), vRes);
    }
  }

  virtual bool GetValue(const VistaSensorMeasure* pM, VistaQuaternion& nOut) const {
    if (!pM)
      return false;

    nOut = GetValue(pM);
    return true;
  }
};

static IVistaPropertyGetFunctor* SapGetter[] = {new VistaDirectXGamepadDIJOYSTATE2Get,
    new VistaDirectXGamepadScalarTranscode, new VistaDirectXGamepadPOVGetTranscode,
    new VistaDirectXGamepadPOVRotGet,
    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_POS, "POSITION", "axis position"),
    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_AXIS1, "AXIS1", "axis 1"),
    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_ROT, "ROTATION", "axis rotation"),
    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_AXIS2, "AXIS2", "axis 2"),
    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_VEL, "VELOCITY", "axis velocity"),

    new VistaDirectXGamepadAxisVectorTranscode(VistaDirectXGamepadAxisVectorTranscode::TR_AVEL,
        "ANGULAR_VELOCITY", "axis angular velocity"),

    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_ACCEL, "ACCELERATION", "axis accelleration"),

    new VistaDirectXGamepadAxisVectorTranscode(VistaDirectXGamepadAxisVectorTranscode::TR_AACCEL,
        "ANGULAR_ACCELERATION", "axis angular accelleration"),

    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_FORCE, "FORCE", "axis force"),

    new VistaDirectXGamepadAxisVectorTranscode(
        VistaDirectXGamepadAxisVectorTranscode::TR_TORQUE, "TORQUE", "axis torque"),

    new VistaDirectXGamepadAxisVectorTranscode(VistaDirectXGamepadAxisVectorTranscode::TR_POVAXIS,
        "POVAXIS", "the POV as a vector in the xz plane"),

    NULL};

class VistaDirectXGamepadTranscodeFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder() {
    return new VistaDirectXGamepadMeasureTranscode;
  }

  virtual void DestroyTranscoder(IVistaMeasureTranscode* transcoder) {
    delete transcoder;
  }

  virtual std::string GetTranscoderName() const {
    return "VistaDirectXGamepadTranscodeFactory";
  }
};

} // namespace

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#ifdef VISTADIRECTXGAMEPADTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(TSimpleTranscoderFactoryFactory<VistaDirectXGamepadTranscodeFactory>)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(TSimpleTranscoderFactoryFactory<VistaDirectXGamepadTranscodeFactory>)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaDirectXGamepadTranscodeFactory>)
