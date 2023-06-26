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

#include <list>
using namespace std;

#include "VdfnSerializer.h"

#include "VdfnUtil.h"

#include "VdfnGraph.h"
#include "VdfnNode.h"

#include "VdfnDriverSensorNode.h"
#include "VdfnHistoryProjectNode.h"

#include "VdfnPortFactory.h"

#include "VdfnActionObject.h"

#include "VdfnNodeCreators.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>

#include "VdfnDriverSensorNode.h"
#include "VdfnHistoryProjectNode.h"
#include "VdfnObjectRegistry.h"

#include "VdfnConstantValueNode.h"

#include <VistaBase/VistaVectorMath.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaConversion.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

namespace {
template <class T>
class PropertyGetAsString : public VdfnPortFactory::StringGet {
 public:
  typedef std::string (*Convert)(const T&);
  PropertyGetAsString(Convert pFConvert = NULL)
      : m_pFC(pFConvert) {
    if (m_pFC == NULL)
      m_pFC = &VistaConversion::ToString<T>;
  }

  virtual bool GetStringValue(
      IVistaPropertyGetFunctor* pF, const VistaSensorMeasure* pMeasure, std::string& strOut) {
    IVistaMeasureTranscode::TTranscodeValueGet<T>* pGet =
        dynamic_cast<IVistaMeasureTranscode::TTranscodeValueGet<T>*>(pF);
    if (pGet != NULL) {
      T value;
      if (pGet->GetValue(pMeasure, value)) {
        strOut = m_pFC(value);
        return true;
      }
      return false;
    }

    IVistaMeasureTranscode::TTranscodeIndexedGet<T>* pGetIdx =
        dynamic_cast<IVistaMeasureTranscode::TTranscodeIndexedGet<T>*>(pF);
    if (pGetIdx == NULL)
      return false;

    unsigned int n = 0;
    while (n < 256) {
      T value;
      if (pGetIdx->GetValueIndexed(pMeasure, value, n) == true) {
        strOut += std::string(" ") + m_pFC(value);
      } else
        return true;
      ++n;
    }

    return false;
  }

 private:
  Convert m_pFC;
};

template <typename T>
void RegisterDefaultPortAndFunctorAccess(VdfnPortFactory* pFac) {
  pFac->AddPortAccess(
      typeid(T).name(), new VdfnPortFactory::CPortAccess(new VdfnTypedPortCreate<T>,
                            new TVdfnTranscodePortSet<T>, new VdfnTypedPortStringGet<T>));

  pFac->AddFunctorAccess(typeid(T).name(),
      new VdfnPortFactory::CFunctorAccess(new VdfnTypedPortTypeCompareCreate<T>, new TActionSet<T>,
          new VdfnTypedPortCreate<T>, new PropertyGetAsString<T>));
}
} // namespace

namespace VdfnUtil {
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool RegisterBasicPortSetters() {
  VdfnPortFactory* pFac = VdfnPortFactory::GetSingleton();

  RegisterDefaultPortAndFunctorAccess<bool>(pFac);
  RegisterDefaultPortAndFunctorAccess<int>(pFac);
  RegisterDefaultPortAndFunctorAccess<unsigned int>(pFac);
  RegisterDefaultPortAndFunctorAccess<VistaType::sint64>(pFac);
  RegisterDefaultPortAndFunctorAccess<VistaType::microtime>(pFac);
  RegisterDefaultPortAndFunctorAccess<float>(pFac);
  RegisterDefaultPortAndFunctorAccess<double>(pFac);

  RegisterDefaultPortAndFunctorAccess<std::string>(pFac);

  RegisterDefaultPortAndFunctorAccess<VistaVector3D>(pFac);
  RegisterDefaultPortAndFunctorAccess<VistaQuaternion>(pFac);
  RegisterDefaultPortAndFunctorAccess<VistaTransformMatrix>(pFac);
  RegisterDefaultPortAndFunctorAccess<VistaAxisAndAngle>(pFac);

  RegisterDefaultPortAndFunctorAccess<std::vector<bool>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<int>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<unsigned int>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<VistaType::sint64>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<VistaType::microtime>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<float>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<double>>(pFac);

  RegisterDefaultPortAndFunctorAccess<std::vector<std::string>>(pFac);

  RegisterDefaultPortAndFunctorAccess<std::vector<VistaVector3D>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<VistaQuaternion>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<VistaTransformMatrix>>(pFac);
  RegisterDefaultPortAndFunctorAccess<std::vector<VistaAxisAndAngle>>(pFac);
  RegisterDefaultPortAndFunctorAccess<VdfnHistoryPortData*>(pFac);

  pFac->AddPortAccess(typeid(VistaVector<float, 4>).name(),
      new VdfnPortFactory::CPortAccess(new VdfnTypedPortCreate<VistaVector<float, 4>>,
          new TVdfnTranscodePortSet<VistaVector<float, 4>>));

  pFac->AddFunctorAccess(typeid(VistaVector<float, 4>).name(),
      new VdfnPortFactory::CFunctorAccess(new VdfnTypedPortTypeCompareCreate<VistaVector<float, 4>>,
          new TActionSet<VistaVector<float, 4>>, new VdfnTypedPortCreate<VistaVector<float, 4>>,
          new PropertyGetAsString<VistaVector<float, 4>>));

  pFac->AddPortAccess(typeid(std::map<std::string, float>).name(),
      new VdfnPortFactory::CPortAccess(new VdfnTypedPortCreate<std::map<std::string, float>>,
          new TVdfnTranscodePortSet<std::map<std::string, float>>));

  return true;
}

bool InitVdfn(VistaDriverMap* pDrivers, VdfnObjectRegistry* pObjRegistry) {
  bool bRet = RegisterBasicPortSetters();
  bRet      = bRet && VdfnNodeCreators::RegisterNodeCreators(
                     VdfnNodeFactory::GetSingleton(), pDrivers, pObjRegistry);
  return bRet;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

} // namespace VdfnUtil
