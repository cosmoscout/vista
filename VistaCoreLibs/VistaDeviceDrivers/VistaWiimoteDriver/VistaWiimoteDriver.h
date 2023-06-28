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

#ifndef __VISTAWIIMOTEDRIVER_H
#define __VISTAWIIMOTEDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Shared library support
// Windows DLL build
#if defined(WIN32) && !defined(VISTAWIIMOTE_STATIC)
#ifdef VISTAWIIMOTEDRIVER_EXPORTS
#define VISTAWIIMOTEDRIVERAPI __declspec(dllexport)
#else
#define VISTAWIIMOTEDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAWIIMOTEDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverInfoAspect;
class IVistaDriverProtocolAspect;
class VistaDriverSensorMappingAspect;
class VistaDriverGenericParameterAspect;
class VistaDriverThreadAspect;
class WiimoteRumble;
class VistaMutex;

struct wiimote_t;
typedef wiimote_t wiimote;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAWIIMOTEDRIVERAPI VistaWiimoteDriver : public IVistaDeviceDriver {
 public:
  VistaWiimoteDriver(IVistaDriverCreationMethod* crm);
  virtual ~VistaWiimoteDriver();

  class VistaWiimoteUpdate;

  class VISTAWIIMOTEDRIVERAPI WiiMoteParameters
      : public VistaDriverGenericParameterAspect::IParameterContainer {
    REFL_DECLARE
   public:
    WiiMoteParameters(VistaWiimoteDriver* pDriver);

    enum {
      MSG_MOTEID_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
      MSG_ACCELREPORT_CHG,
      MSG_IRREPORT_CHG,
      MSG_SMOOTHALPHA_CHG,
      MSG_ACCELTHRESHOLD_CHG,
      MSG_WAITTIMEOUT_CHG,
      MSG_LAST
    };

    std::string GetWiiMoteId() const;
    bool        SetWiiMoteId(const std::string& strWiiMoteId);

    bool GetDoesAccelerationReports() const;
    bool SetDoesAccelerationReports(bool bDoesIt);

    bool GetDoesIRReports() const;
    bool SetDoesIRReports(bool bDoesIt);

    float GetSmoothAlpha() const;
    bool  SetSmoothAlpha(float nAlpha);

    int  GetAccelerationThreshold() const;
    bool SetAccelerationThreshold(int nThreshold);

    int  GetWaitTimeout() const;
    bool SetWaitTimeout(int nTimeoutInSecs);

   private:
    VistaWiimoteDriver* m_pDriver;

    std::string m_strWiiMoteId;
    bool        m_bDoesAcc, m_bDoesIR;

    float m_nSmoothAlpha;
    int   m_nAccelThreshold, m_nWaitTimeout;

    VistaMutex* m_pLock;
  };

  WiiMoteParameters* GetParameters() const;
  bool               GetIsConnected() const;

  bool EnableWiiMoteSensor(unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec);
  bool EnableNunchukSensor(unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec);
  bool EnableClassicSensor(unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec);
  bool EnableGuitarSensor(unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec);
  bool EnableStateSensor(unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec);

  void SetParamChangeFlag();

 protected:
  virtual bool DoConnect();
  virtual bool DoDisconnect();

  virtual bool DoSensorUpdate(VistaType::microtime nTs);
  virtual bool PhysicalEnable(bool bEnable);

 private:
  bool RecordMoteEvent(VistaType::microtime nTs);
  bool RecordStateEvent(VistaType::microtime nTs);
  bool SetupWiiMote();
  bool EnableSensorTyped(
      const std::string& strType, unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec);

  template <class T>
  bool RecordExtSensor(VistaType::microtime nTs, unsigned int nSensorIdx, const T* msg) {
    if (nSensorIdx == ~0u)
      return false;

    VistaDeviceSensor* pSensor = GetSensorByIndex(nSensorIdx);

    MeasureStart(nSensorIdx, nTs);
    // get the current place for the decoding for sensor 0
    VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

    // for the sake of readability: interpret the memory blob
    // as a place to store a sample for the IRMan
    T* s = pM->getWrite<T>();

    memcpy(s, msg, sizeof(T));

    // we are done. Indicate that to the history
    MeasureStop(nSensorIdx);
    return true;
  }

  int DetermineSetup();

  wiimote** m_pMote;

  VistaDriverInfoAspect*             m_pInfo;
  IVistaDriverProtocolAspect*        m_pProtocol;
  VistaDriverSensorMappingAspect*    m_pMappingAspect;
  VistaDriverGenericParameterAspect* m_pParams;
  VistaDriverThreadAspect*           m_pThreadAspect;
  WiimoteRumble*                     m_pRumble;

  unsigned int m_nWiimoteId, m_nNunchukId, m_nStatusId, m_nClassicId, m_nGuitarId;

  bool m_bConnected;
  bool m_bParamChangeFlag;

  int m_nState;
};

class VISTAWIIMOTEDRIVERAPI WiiMoteParameterCreate
    : public VistaDriverGenericParameterAspect::IContainerCreate {
 public:
  WiiMoteParameterCreate(VistaWiimoteDriver* pDriver)
      : m_pDriver(pDriver) {
  }

  virtual VistaDriverGenericParameterAspect::IParameterContainer* CreateContainer() {
    return new VistaWiimoteDriver::WiiMoteParameters(m_pDriver);
  }

  virtual bool DeleteContainer(VistaDriverGenericParameterAspect::IParameterContainer* pCont) {
    delete pCont;
    return true;
  }

 private:
  VistaWiimoteDriver* m_pDriver;
};

class VISTAWIIMOTEDRIVERAPI VistaWiimoteDriverCreationMethod : public IVistaDriverCreationMethod {
 public:
  VistaWiimoteDriverCreationMethod(IVistaTranscoderFactoryFactory* metaFac);
  virtual IVistaDeviceDriver* CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //__VISTAWIIMOTEDRIVER_H
