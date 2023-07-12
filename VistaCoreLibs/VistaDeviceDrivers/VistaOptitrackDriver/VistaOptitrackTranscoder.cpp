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

#include "VistaOptitrackCommonShare.h"

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

#include <cstdio>
#include <cstring>

#include <string>

namespace {
class VistaOptitrackRigidBodyTranscoder : public IVistaMeasureTranscode {
 public:
  VistaOptitrackRigidBodyTranscoder()
      : IVistaMeasureTranscode() {
  }
  static std::string GetTypeString() {
    return "VistaOptitrackRigidBodyTranscoder";
  }
  REFL_INLINEIMP(VistaOptitrackRigidBodyTranscoder, IVistaMeasureTranscode);
};
IVistaPropertyGetFunctor* s_aRigidBodyGetter[] = {
    new IVistaMeasureTranscode::TTranscodeMemberGet<VistaOptitrackMeasures::RigidBodyMeasure, int>(
        "RIGID_BODY_ID", "VistaOptitrackRigidBodyTranscoder", "optitrack's ID of the rigid body",
        &VistaOptitrackMeasures::RigidBodyMeasure::m_nIndex),
    new IVistaMeasureTranscode::TTranscodeMemberGet<VistaOptitrackMeasures::RigidBodyMeasure,
        float>("MEAN_ERROR", "VistaOptitrackRigidBodyTranscoder",
        "mean error of the rigid body matching",
        &VistaOptitrackMeasures::RigidBodyMeasure::m_fMeanError),
    new IVistaMeasureTranscode::TTranscodeMemberGet<VistaOptitrackMeasures::RigidBodyMeasure,
        VistaVector3D>("POSITION", "VistaOptitrackRigidBodyTranscoder",
        "position of the rigid body", &VistaOptitrackMeasures::RigidBodyMeasure::m_v3Position),
    new IVistaMeasureTranscode::TTranscodeMemberGet<VistaOptitrackMeasures::RigidBodyMeasure,
        VistaQuaternion>("ORIENTATION", "VistaOptitrackRigidBodyTranscoder",
        "orientation of the rigid body", &VistaOptitrackMeasures::RigidBodyMeasure::m_qOrientation),
};

class VistaOptitrackDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory {
 public:
  typedef std::map<std::string, ICreateTranscoder*> CreatorsMap;

  VistaOptitrackDriverTranscoderFactoryFactory() {
    CreateCreators(m_mapCreators);
  }

  ~VistaOptitrackDriverTranscoderFactoryFactory() {
    CleanupCreators(m_mapCreators);
  }

  static void CreateCreators(CreatorsMap& m_mapCreators) {
    m_mapCreators["RIGID_BODY"] = new TCreateTranscoder<VistaOptitrackRigidBodyTranscoder>();
  }

  static void CleanupCreators(CreatorsMap& m_mapCreators) {
    for (CreatorsMap::iterator it = m_mapCreators.begin(); it != m_mapCreators.end(); ++it)
      delete (*it).second;

    m_mapCreators.clear();
  }

  virtual IVistaMeasureTranscoderFactory* CreateFactoryForType(const std::string& strTypeName) {
    CreatorsMap::const_iterator it = m_mapCreators.find(strTypeName);
    if (it == m_mapCreators.end())
      return NULL;
    return (*it).second->Create();
  }

  virtual void DestroyTranscoderFactory(IVistaMeasureTranscoderFactory* pFactory) {
    delete pFactory;
  }

  static void OnUnload() {
    CreatorsMap mapCreators;
    CreateCreators(mapCreators);
    for (CreatorsMap::iterator itCreator = mapCreators.begin(); itCreator != mapCreators.end();
         ++itCreator)
      (*itCreator).second->OnUnload();
    CleanupCreators(mapCreators);
  }

 private:
  CreatorsMap m_mapCreators;
};
} // namespace

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTAOPTITRACKTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(VistaOptitrackDriverTranscoderFactoryFactory)
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(VistaOptitrackDriverTranscoderFactoryFactory)
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaOptitrackDriverTranscoderFactoryFactory)

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
