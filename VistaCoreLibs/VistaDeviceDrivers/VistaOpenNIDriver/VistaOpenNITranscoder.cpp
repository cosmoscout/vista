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

#include "VistaOpenNIDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {
class OpenNISkeletonTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(OpenNISkeletonTranscoder, IVistaMeasureTranscode)
 public:
  OpenNISkeletonTranscoder() {
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "OpenNISkeletonTranscoder";
  }
};

class OpenNIJointPosGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaVector3D>> {
 public:
  OpenNIJointPosGet()
      : IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaVector3D>>("POSITION",
            OpenNISkeletonTranscoder::GetTypeString(),
            "vector with positions of the skeleton joints") {
  }

  virtual std::vector<VistaVector3D> GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaOpenNIDriver::SkeletonData* pSkelMeasure =
        reinterpret_cast<const VistaOpenNIDriver::SkeletonData*>(&pMeasure->m_vecMeasures[0]);
    std::vector<VistaVector3D> vecJoints(VistaOpenNIDriver::SKELETON_COUNT);
    for (int i = 0; i < VistaOpenNIDriver::SKELETON_COUNT; ++i)
      vecJoints[i] = VistaVector3D(pSkelMeasure->m_aJoints[i].m_a3fPosition);
    return vecJoints;
  }

  virtual bool GetValue(
      const VistaSensorMeasure* pMeasure, std::vector<VistaVector3D>& vecOut) const {
    const VistaOpenNIDriver::SkeletonData* pSkelMeasure =
        reinterpret_cast<const VistaOpenNIDriver::SkeletonData*>(&pMeasure->m_vecMeasures[0]);
    vecOut.resize(VistaOpenNIDriver::SKELETON_COUNT);
    for (int i = 0; i < VistaOpenNIDriver::SKELETON_COUNT; ++i)
      vecOut[i] = VistaVector3D(pSkelMeasure->m_aJoints[i].m_a3fPosition);
    return true;
  }
};

class OpenNIJointOriGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaQuaternion>> {
 public:
  OpenNIJointOriGet()
      : IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaQuaternion>>("ORIENTATION",
            OpenNISkeletonTranscoder::GetTypeString(),
            "vector with orientations of the skeleton joints") {
  }

  virtual std::vector<VistaQuaternion> GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaOpenNIDriver::SkeletonData* pSkelMeasure =
        reinterpret_cast<const VistaOpenNIDriver::SkeletonData*>(&pMeasure->m_vecMeasures[0]);
    std::vector<VistaQuaternion> vecJoints(VistaOpenNIDriver::SKELETON_COUNT);
    VistaTransformMatrix         matConvMatrix;
    for (int i = 0; i < VistaOpenNIDriver::SKELETON_COUNT; ++i) {
      /** @todo: check if transposed */
      matConvMatrix.SetBasisMatrix(pSkelMeasure->m_aJoints[i].m_a3x3fOrientation);
      vecJoints[i] = VistaQuaternion(matConvMatrix);
    }
    return vecJoints;
  }

  virtual bool GetValue(
      const VistaSensorMeasure* pMeasure, std::vector<VistaQuaternion>& vecOut) const {
    const VistaOpenNIDriver::SkeletonData* pSkelMeasure =
        reinterpret_cast<const VistaOpenNIDriver::SkeletonData*>(&pMeasure->m_vecMeasures[0]);
    vecOut.resize(VistaOpenNIDriver::SKELETON_COUNT);
    VistaTransformMatrix matConvMatrix;
    for (int i = 0; i < VistaOpenNIDriver::SKELETON_COUNT; ++i) {
      /** @todo: check if transposed */
      matConvMatrix.SetBasisMatrix(pSkelMeasure->m_aJoints[i].m_a3x3fOrientation);
      vecOut[i] = VistaQuaternion(matConvMatrix);
    }
    return true;
  }
};

class OpenNIJointConfidenceGet
    : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<float>> {
 public:
  OpenNIJointConfidenceGet()
      : IVistaMeasureTranscode::TTranscodeValueGet<std::vector<float>>("POSITION",
            OpenNISkeletonTranscoder::GetTypeString(),
            "vector with confidence of the skeleton joints") {
  }

  virtual std::vector<float> GetValue(const VistaSensorMeasure* pMeasure) const {
    const VistaOpenNIDriver::SkeletonData* pSkelMeasure =
        reinterpret_cast<const VistaOpenNIDriver::SkeletonData*>(&pMeasure->m_vecMeasures[0]);
    std::vector<float> vecJoints(VistaOpenNIDriver::SKELETON_COUNT);
    for (int i = 0; i < VistaOpenNIDriver::SKELETON_COUNT; ++i)
      vecJoints[i] = pSkelMeasure->m_aJoints[i].m_fConfidence;
    return vecJoints;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, std::vector<float>& vecOut) const {
    const VistaOpenNIDriver::SkeletonData* pSkelMeasure =
        reinterpret_cast<const VistaOpenNIDriver::SkeletonData*>(&pMeasure->m_vecMeasures[0]);
    vecOut.resize(VistaOpenNIDriver::SKELETON_COUNT);
    for (int i = 0; i < VistaOpenNIDriver::SKELETON_COUNT; ++i)
      vecOut[i] = pSkelMeasure->m_aJoints[i].m_fConfidence;
    return true;
  }
};

IVistaPropertyGetFunctor* SaSkeletonGetter[] = {
    new OpenNIJointPosGet, new OpenNIJointOriGet, new OpenNIJointConfidenceGet, NULL};

class OpenNIDepthMapTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(OpenNIDepthMapTranscoder, IVistaMeasureTranscode)
 public:
  OpenNIDepthMapTranscoder() {
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "OpenNIDepthMapTranscoder";
  }
};

/** @todo getters */

IVistaPropertyGetFunctor* SaDepthMapGetter[] = {NULL};

class OpenNIImageTranscoder : public IVistaMeasureTranscode {
  REFL_INLINEIMP(OpenNIImageTranscoder, IVistaMeasureTranscode)
 public:
  OpenNIImageTranscoder() {
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "OpenNIImageTranscoder";
  }
};

/** @todo getters */

IVistaPropertyGetFunctor* SaImageGetter[] = {NULL};

class VistaOpenNIDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory {
 public:
  virtual IVistaMeasureTranscoderFactory* CreateFactoryForType(const std::string& sTypeName) {
    if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sTypeName, "SKELETON")) {
      return new TDefaultTranscoderFactory<OpenNISkeletonTranscoder>(
          OpenNISkeletonTranscoder::GetTypeString());
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sTypeName, "DEPTHMAP")) {
      return new TDefaultTranscoderFactory<OpenNIDepthMapTranscoder>(
          OpenNIDepthMapTranscoder::GetTypeString());
    } else if (VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(sTypeName, "IMAGE")) {
      return new TDefaultTranscoderFactory<OpenNIImageTranscoder>(
          OpenNIImageTranscoder::GetTypeString());
    } else
      return NULL;
  }

  virtual void DestroyTranscoderFactory(IVistaMeasureTranscoderFactory* pFac) {
    delete pFac;
  }

  static void OnUnload() {
    TDefaultTranscoderFactory<OpenNISkeletonTranscoder> oSkelTranscoder(
        OpenNISkeletonTranscoder::GetTypeString());
    oSkelTranscoder.OnUnload();

    TDefaultTranscoderFactory<OpenNIDepthMapTranscoder> oDepthTranscoder(
        OpenNIDepthMapTranscoder::GetTypeString());
    oDepthTranscoder.OnUnload();

    TDefaultTranscoderFactory<OpenNIImageTranscoder> oImageTranscoder(
        OpenNIImageTranscoder::GetTypeString());
    oImageTranscoder.OnUnload();
  }
};

//	VistaOpenNIDriverTranscoderFactoryFactory* SpFactory = NULL;
} // namespace

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTAOPENNITRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(VistaOpenNIDriverTranscoderFactoryFactory)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(VistaOpenNIDriverTranscoderFactoryFactory)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaOpenNIDriverTranscoderFactoryFactory)
