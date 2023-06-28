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

#include "VistaOpenALRecordDriver.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/
namespace {
std::string GetFormatStringFor(int format) {
  switch (format) {
  case AL_FORMAT_MONO8:
    return "MONO8";
  case AL_FORMAT_MONO16:
    return "MONO16";
  case AL_FORMAT_STEREO8:
    return "STEREO8";
  case AL_FORMAT_STEREO16:
    return "STEREO16";
  default:
    return "<UNKNOWN-FORMAT>";
  }
}

class VistaOpenALRecordAudioTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(VistaOpenALRecordAudioTranscoder, IVistaMeasureTranscode);

 public:
  VistaOpenALRecordAudioTranscoder() {
    // inherited as protected member
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VistaOpenALRecordAudioTranscoder";
  }
};

class VistaOpenALRecordTypeTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(VistaOpenALRecordTypeTranscoder, IVistaMeasureTranscode);

 public:
  VistaOpenALRecordTypeTranscoder() {
    // inherited as protected member
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VistaOpenALRecordTypeTranscoder";
  }
};

// ###############################################################################################
// TRANSCODERS
// ###############################################################################################

class VistaRawValueTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<const void*> {
 public:
  VistaRawValueTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<const void*>("RAWFIELD",
            VistaOpenALRecordAudioTranscoder::GetTypeString(),
            "returns a pointer to the raw void* field.") {
  }

  virtual const void* GetValue(const VistaSensorMeasure* pMeasure) const {
    return &(*pMeasure).m_vecMeasures[0];
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, const void*& nValue) const {
    nValue = GetValue(pMeasure);
    return true;
  }
};

// ###############################################################################################
// ###############################################################################################

class VistaFormatTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<std::string> {
 public:
  VistaFormatTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<std::string>("FORMATSTRING",
            VistaOpenALRecordTypeTranscoder::GetTypeString(),
            "returns the current format as a string.") {
  }

  virtual std::string GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaOpenALRecordDriver::_audiotype* m =
        (*pMeasure).getRead<VistaOpenALRecordDriver::_audiotype>();
    return GetFormatStringFor(m->nFormat);
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, std::string& value) const {
    value = GetValue(pMeasure);
    return true;
  }
};

class VistaFrequencyTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaFrequencyTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<int>("FREQUENCY",
            VistaOpenALRecordTypeTranscoder::GetTypeString(), "returns the current frequency.") {
  }

  virtual int GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaOpenALRecordDriver::_audiotype* m =
        (*pMeasure).getRead<VistaOpenALRecordDriver::_audiotype>();
    return m->nFreq;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, int& value) const {
    value = GetValue(pMeasure);
    return true;
  }
};

class VistaMeasureSizeTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaMeasureSizeTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<int>("AUDIOMEASURESIZE",
            VistaOpenALRecordTypeTranscoder::GetTypeString(),
            "returns the size of the current audiomeasure size.") {
  }

  virtual int GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaOpenALRecordDriver::_audiotype* m =
        (*pMeasure).getRead<VistaOpenALRecordDriver::_audiotype>();
    return m->nMeasureSize;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, int& value) const {
    value = GetValue(pMeasure);
    return true;
  }
};
// ###############################################################################################
// ###############################################################################################

// ###############################################################################################
// GETTER / SETTER
// ###############################################################################################
static IVistaPropertyGetFunctor* SaGetter[] = {new VistaRawValueTranscode, new VistaFormatTranscode,
    new VistaFrequencyTranscode, new VistaMeasureSizeTranscode, NULL};

// ###############################################################################################
// FACTORYFACTORY
// ###############################################################################################

class VistaOpenALRecordDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory {
 public:
  virtual IVistaMeasureTranscoderFactory* CreateFactoryForType(const std::string& strTypeName) {
    if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(strTypeName, "AUDIO")) {
      return new TDefaultTranscoderFactory<VistaOpenALRecordAudioTranscoder>(
          VistaOpenALRecordAudioTranscoder::GetTypeString());
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(strTypeName, "TYPE")) {
      return new TDefaultTranscoderFactory<VistaOpenALRecordTypeTranscoder>(
          VistaOpenALRecordTypeTranscoder::GetTypeString());
    } else
      return NULL;
  }

  virtual void DestroyTranscoderFactory(IVistaMeasureTranscoderFactory* fac) {
    delete fac;
  }

  static void OnUnload() {
    TDefaultTranscoderFactory<VistaOpenALRecordTypeTranscoder> a(
        VistaOpenALRecordTypeTranscoder::GetTypeString());
    a.OnUnload();
    TDefaultTranscoderFactory<VistaOpenALRecordAudioTranscoder> b(
        VistaOpenALRecordAudioTranscoder::GetTypeString());
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

#ifdef VISTAOPENALRECORDTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(VistaOpenALRecordDriverTranscoderFactoryFactory)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(VistaOpenALRecordDriverTranscoderFactoryFactory)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaOpenALRecordDriverTranscoderFactoryFactory)
