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

#include "VistaTestDriver.h"
#include "VistaBase/VistaTimeUtils.h"
#include "VistaBase/VistaTimer.h"

namespace {
class VistaTestDriverMeasureTranscode : public IVistaMeasureTranscode {
 public:
  VistaTestDriverMeasureTranscode() {
  }
  virtual ~VistaTestDriverMeasureTranscode() {
  }
  static std::string GetTypeString() {
    return "VistaTestDriverMeasureTranscode";
  }

  class Get : public IVistaMeasureTranscode::IntGet {
   public:
    Get()
        : IVistaMeasureTranscode::IntGet(
              "VALUE", VistaTestDriverMeasureTranscode::GetTypeString(), "value") {
    }

    virtual int GetValue(const VistaSensorMeasure* pMeasure) const {
      const VistaTestDriverMeasure* pMeasureData = pMeasure->getRead<VistaTestDriverMeasure>();
      return pMeasureData->m_nValue;
    }

    virtual bool GetValue(const VistaSensorMeasure* pMeasure, int& nValue) const {
      const VistaTestDriverMeasure* pMeasureData = pMeasure->getRead<VistaTestDriverMeasure>();
      nValue                                     = pMeasureData->m_nValue;
      return true;
    }
  };

  REFL_INLINEIMP(VistaTestDriverMeasureTranscode, IVistaMeasureTranscode);
};
static IVistaPropertyGetFunctor* s_pTranscodeGetter = new VistaTestDriverMeasureTranscode::Get();

class TestDriverTranscoderFactory : public IVistaMeasureTranscoderFactory {
  virtual IVistaMeasureTranscode* CreateTranscoder() override {
    return new VistaTestDriverMeasureTranscode();
  }

  virtual void DestroyTranscoder(IVistaMeasureTranscode* pTranscode) override {
    delete pTranscode;
  }
  virtual std::string GetTranscoderName() const override {
    return "TestDriverTranscoderFactory";
  }
};
} // namespace
class VistaTestDriver::CreationMethod : public IVistaDriverCreationMethod {
 public:
  CreationMethod(IVistaTranscoderFactoryFactory* pMetaFactory)
      : IVistaDriverCreationMethod(pMetaFactory) {
    RegisterSensorType(
        "", sizeof(VistaTestDriverMeasure), 1000, pMetaFactory->CreateFactoryForType(""));
  }

  virtual IVistaDeviceDriver* CreateDriver() {
    return new VistaTestDriver(this);
  }
};

VistaTestDriver::VistaTestDriver(IVistaDriverCreationMethod* pCreationMethod)
    : IVistaDeviceDriver(pCreationMethod)
    , m_nReadBuffer(0) {
  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);
  VistaDeviceSensor* pSensor = new VistaDeviceSensor();
  pSensor->SetMeasureTranscode(
      pCreationMethod->GetTranscoderFactoryForSensor("")->CreateTranscoder());
  pSensor->SetTypeHint("");
  AddDeviceSensor(pSensor);
}

bool VistaTestDriver::DoSensorUpdate(VistaType::microtime dTs) {
  VistaDeviceSensor* pSensor = GetSensorByIndex(0);

  assert(pSensor != NULL);

  MeasureStart(0, dTs);
  VistaSensorMeasure*     pMeasure     = m_pHistoryAspect->GetCurrentSlot(pSensor);
  VistaTestDriverMeasure* pMeasureData = pMeasure->getWrite<VistaTestDriverMeasure>();

  pMeasureData->m_nValue = m_nReadBuffer;

  MeasureStop(0);
  return true;
}

void VistaTestDriver::PushValue(int nValue) {
  m_nReadBuffer = nValue;
  DoSensorUpdate(VistaTimeUtils::GetStandardTimer().GetSystemTime());
}

bool VistaTestDriver::DoConnect() {
  return true;
}

bool VistaTestDriver::DoDisconnect() {
  return true;
}

VistaTestDriverWrapper::VistaTestDriverWrapper()
    : m_pMetaFactory(new TSimpleTranscoderFactoryFactory<TestDriverTranscoderFactory>()) {
  m_pCreationMethod = std::unique_ptr<VistaTestDriver::CreationMethod>(
      new VistaTestDriver::CreationMethod(m_pMetaFactory.get()));
  m_pDriver = std::unique_ptr<VistaTestDriver>(new VistaTestDriver(m_pCreationMethod.get()));
  m_pSensor = m_pDriver->GetSensorByIndex(0);
  m_pDriver->SetupSensorHistory(m_pSensor, 100, 1.0 / 60.0);
}
VistaTestDriverWrapper::~VistaTestDriverWrapper() {
}

VistaTestDriver* VistaTestDriverWrapper::GetDriver() {
  return m_pDriver.get();
}

VistaDeviceSensor* VistaTestDriverWrapper::GetSensor() {
  return m_pSensor;
}

const VistaMeasureHistory& VistaTestDriverWrapper::GetHistory() {
  return m_pSensor->GetMeasures();
}

IVistaMeasureTranscode::IntGet* VistaTestDriverWrapper::GetTranscodeGet() {
  return dynamic_cast<IVistaMeasureTranscode::IntGet*>(
      m_pSensor->GetMeasureTranscode()->GetMeasureProperty("VALUE"));
}
