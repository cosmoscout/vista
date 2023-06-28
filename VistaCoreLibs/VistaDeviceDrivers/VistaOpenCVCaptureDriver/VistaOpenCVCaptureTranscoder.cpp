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

#include "VistaOpenCVCaptureDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/
namespace {
class VistaOpenCVCaptureVideoTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(VistaOpenCVCaptureVideoTranscoder, IVistaMeasureTranscode);

 public:
  VistaOpenCVCaptureVideoTranscoder() {
    // inherited as protected member
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VistaOpenCVCaptureVideoTranscoder";
  }
};

class VistaOpenCVCaptureTypeTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(VistaOpenCVCaptureTypeTranscoder, IVistaMeasureTranscode);

 public:
  VistaOpenCVCaptureTypeTranscoder() {
    // inherited as protected member
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VistaOpenCVCaptureTypeTranscoder";
  }
};

class VistaRawValueTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<const void*> {
 public:
  VistaRawValueTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<const void*>("RAWFIELD",
            VistaOpenCVCaptureVideoTranscoder::GetTypeString(),
            "returns a pointer to the raw void* field (assume RGB8).") {
  }

  virtual const void* GetValue(const VistaSensorMeasure* pMeasure) const {
    return &(*pMeasure).m_vecMeasures[0];
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, const void*& nValue) const {
    nValue = GetValue(pMeasure);
    return true;
  }
};

static IVistaPropertyGetFunctor* SaGetter[] = {new VistaRawValueTranscode, NULL};

// ###############################################################################################
// FACTORYFACTORY
// ###############################################################################################

class VistaOpenCVCaptureDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory {
 public:
  virtual IVistaMeasureTranscoderFactory* CreateFactoryForType(const std::string& strTypeName) {
    if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(strTypeName, "VIDEO")) {
      return new TDefaultTranscoderFactory<VistaOpenCVCaptureVideoTranscoder>(
          VistaOpenCVCaptureVideoTranscoder::GetTypeString());
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(strTypeName, "TYPE")) {
      return new TDefaultTranscoderFactory<VistaOpenCVCaptureTypeTranscoder>(
          VistaOpenCVCaptureTypeTranscoder::GetTypeString());
    } else
      return NULL;
  }

  virtual void DestroyTranscoderFactory(IVistaMeasureTranscoderFactory* fac) {
    delete fac;
  }

  static void OnUnload() {
    TDefaultTranscoderFactory<VistaOpenCVCaptureTypeTranscoder> a(
        VistaOpenCVCaptureTypeTranscoder::GetTypeString());
    a.OnUnload();
    TDefaultTranscoderFactory<VistaOpenCVCaptureVideoTranscoder> b(
        VistaOpenCVCaptureVideoTranscoder::GetTypeString());
    b.OnUnload();
  }
};
} // namespace

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTAOPENCVCAPTURETRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(VistaOpenCVCaptureDriverTranscoderFactoryFactory)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(VistaOpenCVCaptureDriverTranscoderFactoryFactory)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaOpenCVCaptureDriverTranscoderFactoryFactory)
