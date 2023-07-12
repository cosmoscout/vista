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

#include "VistaIRManDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {

class VistaIRManTranscode : public IVistaMeasureTranscode {
 public:
  VistaIRManTranscode() {
    m_nNumberOfScalars = 6;
  }

  static std::string GetTypeString() {
    return "VistaIRManTranscode";
  }
  REFL_INLINEIMP(VistaIRManTranscode, IVistaMeasureTranscode);
};

class VistaIRManKeyCodeGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<VistaIRManDriver::_sIRManSample> {
 public:
  VistaIRManKeyCodeGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<VistaIRManDriver::_sIRManSample>(
            sPropName, sClassName, sDescription) {
  }

  virtual VistaIRManDriver::_sIRManSample GetValue(const VistaSensorMeasure* m) const {
    const VistaIRManDriver::_sIRManSample* s = (*m).getRead<VistaIRManDriver::_sIRManSample>();
    return *s;
  }

  virtual bool GetValue(const VistaSensorMeasure* m, VistaIRManDriver::_sIRManSample& out) const {
    out = GetValue(m);
    return true;
  }
};

class VistaIRManKeyCodeIntGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<VistaType::uint64> {
 public:
  VistaIRManKeyCodeIntGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<VistaType::uint64>(
            sPropName, sClassName, sDescription) {
  }

  virtual VistaType::uint64 GetValue(const VistaSensorMeasure* m) const {
    const VistaIRManDriver::_sIRManSample* s = (*m).getRead<VistaIRManDriver::_sIRManSample>();

    VistaType::uint64 n = 0;

    unsigned char* c = (unsigned char*)&n;
    for (int i = 2; i < 8; ++i)
      c[i] = s->m_acKeyCode[i - 2];

    return n;
  }

  virtual bool GetValue(const VistaSensorMeasure* m, VistaType::uint64& out) const {
    out = GetValue(m);
    return true;
  }
};

namespace {
IVistaPropertyGetFunctor* SaGetter[] = {
    new VistaIRManKeyCodeGet(
        "KEYCODE", VistaIRManTranscode::GetTypeString(), "a 6-byte key code from the IR"),
    new VistaIRManKeyCodeIntGet(
        "KEY", VistaIRManTranscode::GetTypeString(), "the 6-byte keycode as int64"),
    NULL};
}

// FACTORY
class VistaIRManDriverTranscoderFactory : public TDefaultTranscoderFactory<VistaIRManTranscode> {
 public:
  VistaIRManDriverTranscoderFactory()
      : TDefaultTranscoderFactory<VistaIRManTranscode>(VistaIRManTranscode::GetTypeString()) {
  }
};

} // namespace

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTAIRMANTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(TSimpleTranscoderFactoryFactory<VistaIRManDriverTranscoderFactory>)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(TSimpleTranscoderFactoryFactory<VistaIRManDriverTranscoderFactory>)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaIRManDriverTranscoderFactory>)
