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

#include "VistaHapticDeviceEmulatorCommonShare.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace {
class VistaHapticDeviceEmulatorDriverMeasureTranscode : public IVistaMeasureTranscode {
 public:
  VistaHapticDeviceEmulatorDriverMeasureTranscode() {
    // inherited as protected member
    m_nNumberOfScalars = 13;
  }

  virtual ~VistaHapticDeviceEmulatorDriverMeasureTranscode() {
  }
  static std::string GetTypeString() {
    return "VistaHapticDeviceEmulatorDriverMeasureTranscode";
  }
  REFL_INLINEIMP(VistaHapticDeviceEmulatorDriverMeasureTranscode, IVistaMeasureTranscode);
};

class VistaHapticDeviceEmulatorPosTranscode : public IVistaMeasureTranscode::V3Get {
 public:
  VistaHapticDeviceEmulatorPosTranscode()
      : IVistaMeasureTranscode::V3Get("POSITION",
            VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString(),
            "haptic device emulator 3D position values") {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure* m =
        pMeasure->getRead<VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure>();

    return VistaVector3D(m->m_afPosition[0], m->m_afPosition[1], m->m_afPosition[2]);
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const {
    v3Pos = GetValue(pMeasure);
    return true;
  }
};

class VistaHapticDeviceEmulatorPosSCPTranscode : public IVistaMeasureTranscode::V3Get {
 public:
  VistaHapticDeviceEmulatorPosSCPTranscode()
      : IVistaMeasureTranscode::V3Get("POSITION_SCP",
            VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString(),
            "haptic device emulator 3D position surface contact point values") {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure* m =
        pMeasure->getRead<VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure>();

    return VistaVector3D(m->m_afPosSCP[0], m->m_afPosSCP[1], m->m_afPosSCP[2]);
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const {
    v3Pos = GetValue(pMeasure);
    return true;
  }
};

class VistaHapticDeviceEmulatorVelocityTranscode : public IVistaMeasureTranscode::V3Get {
 public:
  VistaHapticDeviceEmulatorVelocityTranscode()
      : IVistaMeasureTranscode::V3Get("VELOCITY",
            VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString(),
            "haptic device emulator 3D position surface contact point values") {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure* m =
        pMeasure->getRead<VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure>();

    return VistaVector3D(m->m_afVelocity[0], m->m_afVelocity[1], m->m_afVelocity[2]);
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const {
    v3Pos = GetValue(pMeasure);
    return true;
  }
};

class VistaHapticDeviceEmulatorForceReadTranscode : public IVistaMeasureTranscode::V3Get {
 public:
  VistaHapticDeviceEmulatorForceReadTranscode()
      : IVistaMeasureTranscode::V3Get("FORCE",
            VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString(),
            "haptic device emulator 3D force values") {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure* m =
        pMeasure->getRead<VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure>();

    return VistaVector3D(m->m_afForce[0], m->m_afForce[1], m->m_afForce[2]);
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const {
    v3Pos = GetValue(pMeasure);
    return true;
  }
};

class VistaHapticDeviceEmulatorScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VistaHapticDeviceEmulatorScalarTranscode()
      : IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
            VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString(),
            "haptic device emulator scalar values") {
  }

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const {
    if (!pMeasure)
      return false;

    const VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure* m =
        pMeasure->getRead<VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure>();

    switch (nIndex) {
    case 0:
    case 1:
      dScalar = (m->m_nButtonState == static_cast<int>(nIndex + 1)) ? 1.0 : 0.0;
      break;
    case 2: {
      float dVal = m->m_afOverheatState[0];

      for (int i = 1; i < 6; i++)
        if (dVal < m->m_afOverheatState[i])
          dVal = m->m_afOverheatState[i];
      dScalar = dVal;
      break;
    }
    case 3: {
      dScalar = m->m_afForce[0];
      break;
    }
    case 4: {
      dScalar = m->m_afForce[1];
      break;
    }
    case 5: {
      dScalar = m->m_afForce[2];
      break;
    }
    case 6: {
      dScalar = m->m_afVelocity[0];
      break;
    }
    case 7: {
      dScalar = m->m_afVelocity[1];
      break;
    }
    case 8: {
      dScalar = m->m_afVelocity[2];
      break;
    }
    case 9: {
      dScalar = m->m_afPosSCP[0];
      break;
    }
    case 10: {
      dScalar = m->m_afPosSCP[1];
      break;
    }
    case 11: {
      dScalar = m->m_afPosSCP[2];
      break;
    }
    case 12:
      dScalar = m->m_nUpdateRate;
      break;

    default:
      return false;
    }
    return true;
  }
};

class VistaHapticDeviceEmulatorRotationGet : public IVistaMeasureTranscode::QuatGet {
 public:
  VistaHapticDeviceEmulatorRotationGet()
      : IVistaMeasureTranscode::QuatGet("ORIENTATION",
            VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString(),
            "haptic device emulator body orientation") {
  }

  VistaQuaternion GetValue(const VistaSensorMeasure* pMeasure) const {
    VistaQuaternion q;
    GetValue(pMeasure, q);
    return q;
  }

  bool GetValue(const VistaSensorMeasure* pMeasure, VistaQuaternion& qQuat) const {
    if (!pMeasure)
      return false;

    const VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure* m =
        pMeasure->getRead<VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure>();
    VistaTransformMatrix t(float(m->m_afRotMatrix[0]), float(m->m_afRotMatrix[1]),
        float(m->m_afRotMatrix[2]), 0, float(m->m_afRotMatrix[3]), float(m->m_afRotMatrix[4]),
        float(m->m_afRotMatrix[5]), 0, float(m->m_afRotMatrix[6]), float(m->m_afRotMatrix[7]),
        float(m->m_afRotMatrix[8]), 0, 0, 0, 0, 1);

    qQuat = -VistaQuaternion(t);
    return true;
  }
};

static IVistaPropertyGetFunctor* SapGetter[] = {new VistaHapticDeviceEmulatorPosTranscode,
    new VistaHapticDeviceEmulatorForceReadTranscode, new VistaHapticDeviceEmulatorScalarTranscode,
    new VistaHapticDeviceEmulatorPosSCPTranscode, new VistaHapticDeviceEmulatorVelocityTranscode,
    new VistaHapticDeviceEmulatorRotationGet, NULL};
} // namespace

/*
class VistaHapticDeviceEmulatorTranscoderFactory : public IVistaMeasureTranscoderFactory
{
public:
        virtual IVistaMeasureTranscode *CreateTranscoder()
        {
                return new VistaHapticDeviceEmulatorDriverMeasureTranscode;
        }
};
*/

class VistaHapticDeviceEmulatorTranscoderFactory
    : public TDefaultTranscoderFactory<VistaHapticDeviceEmulatorDriverMeasureTranscode> {
 public:
  VistaHapticDeviceEmulatorTranscoderFactory()
      : TDefaultTranscoderFactory<VistaHapticDeviceEmulatorDriverMeasureTranscode>(
            VistaHapticDeviceEmulatorDriverMeasureTranscode::GetTypeString()) {
  }
};

#ifdef VISTAHAPTICDEVICEEMULATORTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(
    TSimpleTranscoderFactoryFactory<VistaHapticDeviceEmulatorTranscoderFactory>)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(
    TSimpleTranscoderFactoryFactory<VistaHapticDeviceEmulatorTranscoderFactory>)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(
    TSimpleTranscoderFactoryFactory<VistaHapticDeviceEmulatorTranscoderFactory>)
