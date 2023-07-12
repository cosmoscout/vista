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

#include "VistaDTrackSDKDriverConfig.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

namespace {
class VistaDTrackSDKMarkerTranscode : public IVistaMeasureTranscode {
 public:
  VistaDTrackSDKMarkerTranscode() {
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VistaDTrackSDKMarkerTranscode";
  }
  REFL_INLINEIMP(VistaDTrackSDKMarkerTranscode, IVistaMeasureTranscode);
};

class VistaDTrackSDKBodyTranscode : public IVistaMeasureTranscode {
 public:
  VistaDTrackSDKBodyTranscode() {
    m_nNumberOfScalars = 0;
  }

  static std::string GetTypeString() {
    return "VistaDTrackSDKBodyTranscode";
  }
  REFL_INLINEIMP(VistaDTrackSDKBodyTranscode, IVistaMeasureTranscode);
};

class VistaDTrackSDKQualityTranscode : public IVistaMeasureTranscode::DoubleGet {
 public:
  VistaDTrackSDKQualityTranscode(
      const std::string& strTranscoderClassName, const std::string& strDescription)
      : IVistaMeasureTranscode::DoubleGet("QUALITY", strTranscoderClassName, strDescription) {
  }

  double GetValue(const VistaSensorMeasure* pMeasure) const {
    // for quality, all measures have the same layout
    VistaDTrackSDKConfig::VISTA_dtrack_marker_type* m =
        (VistaDTrackSDKConfig::VISTA_dtrack_marker_type*)&(*pMeasure).m_vecMeasures[0];
    return double(m->quality);
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, double& q) const {
    q = GetValue(pMeasure);
    return true;
  }
};

class VistaDTrackSDKIdTranscode : public IVistaMeasureTranscode::UIntGet {
 public:
  VistaDTrackSDKIdTranscode(
      const std::string& strTranscoderClassName, const std::string& strDescription)
      : IVistaMeasureTranscode::UIntGet("ID", strTranscoderClassName, strDescription) {
  }

  unsigned int GetValue(const VistaSensorMeasure* pMeasure) const {
    // for quality, all measures have the same layout
    VistaDTrackSDKConfig::VISTA_dtrack_marker_type* m =
        (VistaDTrackSDKConfig::VISTA_dtrack_marker_type*)&(*pMeasure).m_vecMeasures[0];
    return (unsigned int)m->id;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, unsigned int& id) const {
    id = GetValue(pMeasure);
    return true;
  }
};

class VistaDTrackSDKPosTranscode : public IVistaMeasureTranscode::V3Get {
 public:
  VistaDTrackSDKPosTranscode(
      const std::string& strTranscodeClassName, const std::string& strDescription)
      : IVistaMeasureTranscode::V3Get("POSITION", strTranscodeClassName, strDescription) {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const {
    // maker and body structs have the same layout
    VistaDTrackSDKConfig::VISTA_dtrack_marker_type* m =
        (VistaDTrackSDKConfig::VISTA_dtrack_marker_type*)&(*pMeasure).m_vecMeasures[0];

    return VistaVector3D(float(m->loc[0]), float(m->loc[1]), float(m->loc[2]));
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const {
    v3Pos = GetValue(pMeasure);
    return true;
  }
};

class VistaDTrackSDKBodyOrientationTranscode : public IVistaMeasureTranscode::QuatGet {
 public:
  VistaDTrackSDKBodyOrientationTranscode()
      : IVistaMeasureTranscode::QuatGet("ORIENTATION", VistaDTrackSDKBodyTranscode::GetTypeString(),
            "DTrack SDK 3D orientation (right-handed)") {
  }

  virtual VistaQuaternion GetValue(const VistaSensorMeasure* pMeasure) const {
    VistaDTrackSDKConfig::VISTA_dtrack_body_type* m =
        (VistaDTrackSDKConfig::VISTA_dtrack_body_type*)&(*pMeasure).m_vecMeasures[0];
    VistaTransformMatrix mt(float(m->rot[0]), float(m->rot[3]), float(m->rot[6]), 0,
        float(m->rot[1]), float(m->rot[4]), float(m->rot[7]), 0, float(m->rot[2]), float(m->rot[5]),
        float(m->rot[8]), 0, 0, 0, 0, 1);
    VistaQuaternion      q(mt);
    q.Normalize();
    return q;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaQuaternion& qRot) const {
    qRot = GetValue(pMeasure);
    return true;
  }
};

IVistaPropertyGetFunctor* SapGetter[] = {
    new VistaDTrackSDKPosTranscode(
        VistaDTrackSDKMarkerTranscode::GetTypeString(), "dtrack SDK MARKER 3D position values"),
    new VistaDTrackSDKPosTranscode(
        VistaDTrackSDKBodyTranscode::GetTypeString(), "dtrack SDK BODY 3D position values"),
    new VistaDTrackSDKBodyOrientationTranscode,
    new VistaDTrackSDKIdTranscode(
        VistaDTrackSDKMarkerTranscode::GetTypeString(), "dtrack SDK MARKER 3D id"),
    new VistaDTrackSDKIdTranscode(
        VistaDTrackSDKBodyTranscode::GetTypeString(), "dtrack SDK BODY 3D id"),
    new VistaDTrackSDKQualityTranscode(
        VistaDTrackSDKBodyTranscode::GetTypeString(), "dtrack SDK BODY quality"),
    new VistaDTrackSDKQualityTranscode(
        VistaDTrackSDKMarkerTranscode::GetTypeString(), "dtrack SDK MARKER quality"),
    NULL};

class VistaDTrackSDKMarkerTranscodeFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder() {
    return new VistaDTrackSDKMarkerTranscode;
  }
};

class VistaDTrackSDKBodyTranscodeFactory : public IVistaMeasureTranscoderFactory {
 public:
  virtual IVistaMeasureTranscode* CreateTranscoder() {
    return new VistaDTrackSDKBodyTranscode;
  }
};

/*
A
A
A
A
A
A
A
A
A
A
A
A
A
*/

class VistaDTrackSDKTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory {
 public:
  VistaDTrackSDKTranscoderFactoryFactory() {
    CreateCreators(m_creators);
  }

  ~VistaDTrackSDKTranscoderFactoryFactory() {
    CleanupCreators(m_creators);
  }

  typedef std::map<std::string, ICreateTranscoder*> CRMAP;
  CRMAP                                             m_creators;

  static void CreateCreators(CRMAP& mp) {
    mp["BODY"]   = new TCreateTranscoder<VistaDTrackSDKBodyTranscode>;
    mp["MARKER"] = new TCreateTranscoder<VistaDTrackSDKMarkerTranscode>;
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

#ifdef VISTADTRACKSDKTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(VistaDTrackSDKTranscoderFactoryFactory)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(VistaDTrackSDKDriverTranscoderFactoryFactory)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaDTrackSDKTranscoderFactoryFactory)
