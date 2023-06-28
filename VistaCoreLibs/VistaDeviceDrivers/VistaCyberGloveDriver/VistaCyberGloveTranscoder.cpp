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

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaCyberGloveCommonShare.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include <cstring>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
// Transcoders
namespace {

class VistaCyberGloveDriverTranscode : public IVistaMeasureTranscode {
 public:
  VistaCyberGloveDriverTranscode() {
    /** @todo  ?? */
    m_nNumberOfScalars = sizeof(VistaCyberGloveCommonShare::sCyberGloveSample);
  }

  virtual ~VistaCyberGloveDriverTranscode() {
  }

  static std::string GetTypeString() {
    return "VistaCyberGloveDriverTranscode";
  }

  REFL_INLINEIMP(VistaCyberGloveDriverTranscode, IVistaMeasureTranscode);
};

class VistaCyberGloveTranscodeFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder() {
    return new VistaCyberGloveDriverTranscode;
  }

  virtual void DestroyTranscoder(IVistaMeasureTranscode* transc) {
    delete transc;
  }

  virtual std::string GetTranscoderName() const {
    return VistaCyberGloveDriverTranscode::GetTypeString();
  }
};

class VistaCyberGloveRawSampleGet : public IVistaMeasureTranscode::TTranscodeValueGet<
                                        VistaCyberGloveCommonShare::sCyberGloveSample> {
 public:
  VistaCyberGloveRawSampleGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<VistaCyberGloveCommonShare::sCyberGloveSample>(
            sPropName, sClassName, sDescription) {
  }

  virtual VistaCyberGloveCommonShare::sCyberGloveSample GetValue(
      const VistaSensorMeasure* m) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();
    return *s;
  }

  virtual bool GetValue(
      const VistaSensorMeasure* m, VistaCyberGloveCommonShare::sCyberGloveSample& out) const {
    out = GetValue(m);
    return true;
  }
};

class VistaCyberGloveSensorDataGet
    : public IVistaMeasureTranscode::TTranscodeIndexedGet<unsigned int> {
 public:
  VistaCyberGloveSensorDataGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeIndexedGet<unsigned int>(
            sPropName, sClassName, sDescription) {
  }

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* m, unsigned int& out, unsigned int nIdx) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();
    out = (unsigned int)((unsigned char)s->m_cRecord[nIdx + 1]);

    return true;
  }
};

class VistaCyberGloveNumSensorsGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<unsigned int> {
 public:
  VistaCyberGloveNumSensorsGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<unsigned int>(
            sPropName, sClassName, sDescription) {
  }

  virtual unsigned int GetValue(const VistaSensorMeasure* m) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();

    unsigned int nSensorsInPacket = (unsigned int)strlen(s->m_cRecord) - 1;

    // CyberGlove status -> 1 extra byte
    if (s->m_bIncStatusByte) {
      --nSensorsInPacket;
    }

    // timestamp -> 5 extra bytes
    if (s->m_bIncTimestamp) {
      nSensorsInPacket -= 5;
    }

    return nSensorsInPacket;
  }

  virtual bool GetValue(const VistaSensorMeasure* m, unsigned int& out) const {
    out = GetValue(m);
    return true;
  }
};

/*
 *  TIMESTAMP
 *  ---------
 *  "...five additional
 *	bytes that provide the count of an internal timer in the CGIU are sent after the joint data
 *bytes (and before the Glove-Status-Byte, if enabled). Of these five bytes, the last four represent
 *a 32-bit count. Each count of the internal timer represents 8.68048 microseconds. Thus, the total
 *count is unique for approximately 10.36 hours, at which time the 32-bit counter rolls over."
 *
 *  (from CyberGlove Reference Manual, p. 28)
 *
 */

// Decodes the Timestamp included in a data packet
// It consists of 5 Bytes of which the first Byte has the following format:
// 1xxxDCBA with x=don't care and A,B,C,D indicating whether Byte 1-4 are to be set to zero
// A = 1 -> Set Byte 1 to 0
// The method returns the Timestamp or -1 in case of an error
static int DecodeTimestamp(const char* pBuffer) {
  char acBuffer[5];
  memcpy(acBuffer, pBuffer, 5 * sizeof(char));
  int nTimestamp = -1;

  // Check leading 1 to identify illegal timestamp packets
  // This is documented in the CG User Manual, but does not seem to be true
  // The MSB is never set in reality (th)
  // if(! (pBuffer[0] & 0x80) )
  //    return -1L;

  // Test if any of the timestamp bytes need to be set to zero

  // Byte 1
  if (acBuffer[0] & 0x1)
    acBuffer[1] = 0;

  // Byte 2
  if (acBuffer[0] & 0x2)
    acBuffer[2] = 0;

  // Byte 3
  if (acBuffer[0] & 0x4)
    acBuffer[3] = 0;

  // Byte 4
  if (acBuffer[0] & 0x8)
    acBuffer[4] = 0;

  // Build Timestamp
  // Format B4B3B2B1
  nTimestamp = (int)acBuffer[4]; // MSB
  nTimestamp = (nTimestamp << 8) | ((long)acBuffer[3]);
  nTimestamp = (nTimestamp << 8) | ((long)acBuffer[2]);
  nTimestamp = (nTimestamp << 8) | ((long)acBuffer[1]); // LSB

  return nTimestamp;
}

class VistaCyberGloveTimestampGet : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaCyberGloveTimestampGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<int>(sPropName, sClassName, sDescription) {
  }

  virtual bool GetValue(const VistaSensorMeasure* m, int& out) const {
    out = GetValue(m);
    return true;
  }

  virtual int GetValue(const VistaSensorMeasure* m) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();
    if (s->m_bIncTimestamp) {
      return DecodeTimestamp(&(s->m_cRecord[s->m_nSensorsInSample + 1]));
    } else {
      return -1;
    }
  }
};

class VistaCyberGloveSensorVectorGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<unsigned int>> {
 public:
  VistaCyberGloveSensorVectorGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<std::vector<unsigned int>>(
            sPropName, sClassName, sDescription) {
  }

  virtual std::vector<unsigned int> GetValue(const VistaSensorMeasure* m) const {
    std::vector<unsigned int> vOut;
    GetValue(m, vOut);
    return vOut;
  }

  virtual bool GetValue(const VistaSensorMeasure* m, std::vector<unsigned int>& out) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();

    out.resize(s->m_nSensorsInSample);

    for (int n = 0; n < s->m_nSensorsInSample; ++n)
      out[n] = (unsigned int)((unsigned char)s->m_cRecord[n + 1]);
    return true;
  }
};

/*	STATUS-BYTE
 *	-----------
 *
 *	bit number
 *	0				CyberGlove In/Out		(read only)
 *	1				Switch Status
 *	2				Light Status
 *	3-6				Reserved
 *	7				1						(read only)
 *
 *	(from CyberGlove Reference Manual, p. 41)
 *
 */

class VistaCyberGloveStatusByteGet : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaCyberGloveStatusByteGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<int>(sPropName, sClassName, sDescription) {
  }

  virtual int GetValue(const VistaSensorMeasure* m) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();
    if (s->m_bIncStatusByte) {
      // Bits 3-7 do not contain any information. Mask them! -> 0x7 ~ 0...00000111
      int out = s->m_cRecord[strlen(s->m_cRecord) - 1] & 0x7;
      return out;
    } else {
      // no status byte available
      return -1;
    }
  }

  virtual bool GetValue(const VistaSensorMeasure* m, int& out) const {
    out = GetValue(m);
    return true;
  }
};

class VistaCyberGloveLightStateGet : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaCyberGloveLightStateGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<int>(sPropName, sClassName, sDescription) {
  }

  virtual int GetValue(const VistaSensorMeasure* m) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();
    if (s->m_bIncStatusByte) {
      // light state is stored in bit 2. 0x4 ~ 0....0100
      bool state = (s->m_cRecord[strlen(s->m_cRecord) - 1] & 0x4) == 0x4;
      return state ? 1 : 0;
    } else {
      // no information regarding the light state available
      return -1;
    }
  }

  virtual bool GetValue(const VistaSensorMeasure* m, int& out) const {
    out = GetValue(m);
    return true;
  }
};

class VistaCyberGloveSwitchStateGet : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaCyberGloveSwitchStateGet(
      const std::string& sPropName, const std::string& sClassName, const std::string& sDescription)
      : IVistaMeasureTranscode::TTranscodeValueGet<int>(sPropName, sClassName, sDescription) {
  }

  virtual int GetValue(const VistaSensorMeasure* m) const {
    const VistaCyberGloveCommonShare::sCyberGloveSample* s =
        m->getRead<VistaCyberGloveCommonShare::sCyberGloveSample>();
    if (s->m_bIncStatusByte) {
      // light state is stored in bit 1. 0x2 ~ 0....010
      bool state = (s->m_cRecord[strlen(s->m_cRecord) - 1] & 0x2) == 0x2;
      return state ? 1 : 0;
    } else {
      // no information regarding the switch state available
      return -1;
    }
  }

  virtual bool GetValue(const VistaSensorMeasure* m, int& out) const {
    out = GetValue(m);
    return true;
  }
};

IVistaPropertyGetFunctor* SaGetter[] = {
    new VistaCyberGloveRawSampleGet("RAW_SAMPLE", VistaCyberGloveDriverTranscode::GetTypeString(),
        "Raw data sample from the CyberGlove."),
    new VistaCyberGloveNumSensorsGet("NUM_SENSORS", VistaCyberGloveDriverTranscode::GetTypeString(),
        "Get number of sensors in sample."),
    new VistaCyberGloveSensorDataGet("SENSOR_DATA", VistaCyberGloveDriverTranscode::GetTypeString(),
        "Get sensor data by index."),
    new VistaCyberGloveStatusByteGet("STATUS_BYTE", VistaCyberGloveDriverTranscode::GetTypeString(),
        "Get status-byte: -1 if no status byte available."),
    new VistaCyberGloveLightStateGet("LIGHT_STATE", VistaCyberGloveDriverTranscode::GetTypeString(),
        "Get state of the led on the glove switch: 0 = on, 1 = off, -1 = information not "
        "available."),
    new VistaCyberGloveSwitchStateGet("SWITCH_STATE",
        VistaCyberGloveDriverTranscode::GetTypeString(),
        "Get state of the glove switch: 0 = on, 1 = off, -1 = information not available."),
    new VistaCyberGloveSensorVectorGet("SENSOR_VEC",
        VistaCyberGloveDriverTranscode::GetTypeString(),
        "Get vector of sensor values of type unsigned int."),
    new VistaCyberGloveSensorVectorGet("STATE_VEC", VistaCyberGloveDriverTranscode::GetTypeString(),
        "Get state vec as vector of ints"),
    NULL};
} // namespace

#ifdef VISTACYBERGLOVETRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(TSimpleTranscoderFactoryFactory<VistaCyberGloveTranscodeFactory>)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(TSimpleTranscoderFactoryFactory<VistaCyberGloveTranscodeFactory>)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaCyberGloveTranscodeFactory>)

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
