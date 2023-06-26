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

#include "VistaWiimoteDriver.h"
#include "VistaWiimoteCommonShare.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <VistaBase/VistaStreamUtils.h>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4510 4610)
#endif

#include <wiiuse.h>
#ifdef WIN32
#pragma warning(pop)
#endif

#include <stdio.h>
#include <stdlib.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaWiimoteDriverCreationMethod::VistaWiimoteDriverCreationMethod(
    IVistaTranscoderFactoryFactory* metaFac)
    : IVistaDriverCreationMethod(metaFac) {
  RegisterSensorType("WIIMOTE", sizeof(VistaWiimoteDriverCommonShare::MeasureType), 100,
      metaFac->CreateFactoryForType("WIIMOTE"));

  RegisterSensorType("NUNCHUK", sizeof(VistaWiimoteDriverCommonShare::MeasureType), 100,
      metaFac->CreateFactoryForType("NUNCHUK"));

  RegisterSensorType("CLASSIC", sizeof(VistaWiimoteDriverCommonShare::MeasureType), 100,
      metaFac->CreateFactoryForType("CLASSIC"));

  RegisterSensorType("STATUS", sizeof(VistaWiimoteDriverCommonShare::_wiistateMeasure), 100,
      metaFac->CreateFactoryForType("STATUS"));

  RegisterSensorType("GUITAR", sizeof(VistaWiimoteDriverCommonShare::MeasureType), 100,
      metaFac->CreateFactoryForType("GUITAR"));
}

IVistaDeviceDriver* VistaWiimoteDriverCreationMethod::CreateDriver() {
  return new VistaWiimoteDriver(this);
}

// #######################################################################

namespace {
const std::string SsReflectionName("VistaWiiMoteParameters");

IVistaPropertyGetFunctor* SaParameterGetter[] = {
    new TVistaPropertyGet<bool, VistaWiimoteDriver::WiiMoteParameters, VistaProperty::PROPT_BOOL>(
        "DOESREPORTACCELERATION", SsReflectionName,
        &VistaWiimoteDriver::WiiMoteParameters::GetDoesAccelerationReports,
        "returns the state of reporting acceleration values from wiimote"),
    new TVistaPropertyGet<bool, VistaWiimoteDriver::WiiMoteParameters, VistaProperty::PROPT_BOOL>(
        "DOESREPORTIR", SsReflectionName, &VistaWiimoteDriver::WiiMoteParameters::GetDoesIRReports,
        "returns the state of reporting IR values from wiimote"),
    new TVistaPropertyGet<float, VistaWiimoteDriver::WiiMoteParameters,
        VistaProperty::PROPT_DOUBLE>("SMOOTHALPHA", SsReflectionName,
        &VistaWiimoteDriver::WiiMoteParameters::GetSmoothAlpha,
        "returns the value of the currently used smoothing alpha"),
    new TVistaPropertyGet<int, VistaWiimoteDriver::WiiMoteParameters, VistaProperty::PROPT_INT>(
        "ACCELERATIONTHRESHOLD", SsReflectionName,
        &VistaWiimoteDriver::WiiMoteParameters::GetAccelerationThreshold,
        "returns the value of the currently used threshold for reporting accelaration"),
    new TVistaPropertyGet<int, VistaWiimoteDriver::WiiMoteParameters, VistaProperty::PROPT_INT>(
        "WAITTIMEOUT", SsReflectionName, &VistaWiimoteDriver::WiiMoteParameters::GetWaitTimeout,
        "returns the value of the currently used timeout for connecting (in secs)"),
    new TVistaPropertyGet<std::string, VistaWiimoteDriver::WiiMoteParameters,
        VistaProperty::PROPT_STRING>("WIIMOTE_ID", SsReflectionName,
        &VistaWiimoteDriver::WiiMoteParameters::GetWiiMoteId, "returns the current wiimote id"),
    NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {
    new TVistaPropertySet<bool, bool, VistaWiimoteDriver::WiiMoteParameters>(
        "DOESREPORTACCELERATION", SsReflectionName,
        &VistaWiimoteDriver::WiiMoteParameters::SetDoesAccelerationReports,
        "enables / disables the reporting of acceleration values from the wiimote"),
    new TVistaPropertySet<bool, bool, VistaWiimoteDriver::WiiMoteParameters>("DOESREPORTIR",
        SsReflectionName, &VistaWiimoteDriver::WiiMoteParameters::SetDoesIRReports,
        "enables / disables the reporting of IR values from the wiimote"),
    new TVistaPropertySet<float, float, VistaWiimoteDriver::WiiMoteParameters>("SMOOTHALPHA",
        SsReflectionName, &VistaWiimoteDriver::WiiMoteParameters::SetSmoothAlpha,
        "sets the smoothing alpha to use for orientation values"),
    new TVistaPropertySet<int, int, VistaWiimoteDriver::WiiMoteParameters>("ACCELERATIONTHRESHOLD",
        SsReflectionName, &VistaWiimoteDriver::WiiMoteParameters::SetAccelerationThreshold,
        "sets the threshold for reporting of acceleration changes"),
    new TVistaPropertySet<int, int, VistaWiimoteDriver::WiiMoteParameters>("WAITTIMEOUT",
        SsReflectionName, &VistaWiimoteDriver::WiiMoteParameters::SetWaitTimeout,
        "sets the timeout to use for waiting during connect"),
    new TVistaPropertySet<const std::string&, std::string, VistaWiimoteDriver::WiiMoteParameters>(
        "WIIMOTE_ID", SsReflectionName, &VistaWiimoteDriver::WiiMoteParameters::SetWiiMoteId,
        "sets the current wiimote id"),
    NULL};
} // namespace

#if !defined(WIN32)
static void releaseParameterProps() __attribute__((destructor));
#else
static void releaseParameterProps();
#endif

#if defined(WIN32)

#include <windows.h>

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    if (lpReserved == 0)
      releaseParameterProps();
    break;
  }
  return TRUE;
}

#endif

static void releaseParameterProps() {
  IVistaPropertyGetFunctor** git = SaParameterGetter;
  IVistaPropertySetFunctor** sit = SaParameterSetter;

  while (*git)
    delete *git++;

  while (*sit)
    delete *sit++;
}

// #######################################################################

// helper functions
int FindCorrectWiimote(wiimote** motes, int nNum, const std::string& strId) {
  for (int n = 0; n < nNum; ++n) {
#if !defined(WIN32)
    if (motes[n] && (std::string(motes[n]->bdaddr_str) == strId))
      return n;
#else
    /** @todo how to identify those? */
    if (motes[n] && (motes[n]->dev_handle))
      return n;
#endif
  }
  return -1;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SYMBOLIC GETTERS
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

REFL_IMPLEMENT(
    VistaWiimoteDriver::WiiMoteParameters, VistaDriverGenericParameterAspect::IParameterContainer);

VistaWiimoteDriver::WiiMoteParameters::WiiMoteParameters(VistaWiimoteDriver* pDriver)
    : VistaDriverGenericParameterAspect::IParameterContainer()
    , m_pDriver(pDriver)
    , m_bDoesAcc(false)
    , m_bDoesIR(false)
    , m_nSmoothAlpha(0.0f)
    , m_nAccelThreshold(0)
    , m_nWaitTimeout(5)
    , m_pLock(new VistaMutex) {
}

std::string VistaWiimoteDriver::WiiMoteParameters::GetWiiMoteId() const {
  return m_strWiiMoteId;
}

bool VistaWiimoteDriver::WiiMoteParameters::GetDoesAccelerationReports() const {
  return m_bDoesAcc;
}

bool VistaWiimoteDriver::WiiMoteParameters::SetDoesAccelerationReports(bool bDoesIt) {
  VistaMutexLock oLock(*m_pLock);
  if (bDoesIt != m_bDoesAcc) {
    m_bDoesAcc = bDoesIt;
    m_pDriver->SetParamChangeFlag();
    Notify(MSG_ACCELREPORT_CHG);
    return true;
  }
  return true;
}

bool VistaWiimoteDriver::WiiMoteParameters::GetDoesIRReports() const {
  return m_bDoesIR;
}

bool VistaWiimoteDriver::WiiMoteParameters::SetDoesIRReports(bool bDoesIt) {
  VistaMutexLock oLock(*m_pLock);
  if (bDoesIt != m_bDoesIR) {
    m_bDoesIR = bDoesIt;
    m_pDriver->SetParamChangeFlag();
    Notify(MSG_IRREPORT_CHG);
    return true;
  }
  return true;
}

bool VistaWiimoteDriver::WiiMoteParameters::SetWiiMoteId(const std::string& strWiiMoteId) {
  VistaMutexLock oLock(*m_pLock);
  if (m_pDriver->GetIsConnected()) {
    vstr::debug() << "[WiiMoteParameters::SetWiiMoteId]: Already connected, can't set ID anymore"
                  << std::endl;
    return false;
  }
  return compAssignAndNotify<std::string>(strWiiMoteId, m_strWiiMoteId, *this, MSG_MOTEID_CHG);
}

float VistaWiimoteDriver::WiiMoteParameters::GetSmoothAlpha() const {
  return m_nSmoothAlpha;
}

bool VistaWiimoteDriver::WiiMoteParameters::SetSmoothAlpha(float nAlpha) {
  VistaMutexLock oLock(*m_pLock);
  if (nAlpha != m_nSmoothAlpha) {
    m_nSmoothAlpha = nAlpha;
    m_pDriver->SetParamChangeFlag();
    Notify(MSG_SMOOTHALPHA_CHG);
    return true;
  }
  return false;
}

int VistaWiimoteDriver::WiiMoteParameters::GetAccelerationThreshold() const {
  return m_nAccelThreshold;
}

bool VistaWiimoteDriver::WiiMoteParameters::SetAccelerationThreshold(int nThreshold) {
  VistaMutexLock oLock(*m_pLock);
  if (nThreshold != m_nAccelThreshold) {
    m_nAccelThreshold = nThreshold;
    m_pDriver->SetParamChangeFlag();
    Notify(MSG_ACCELTHRESHOLD_CHG);
    return true;
  }
  return false;
}

int VistaWiimoteDriver::WiiMoteParameters::GetWaitTimeout() const {
  return m_nWaitTimeout;
}

bool VistaWiimoteDriver::WiiMoteParameters::SetWaitTimeout(int nTimeoutInSecs) {
  VistaMutexLock oLock(*m_pLock);
  if (m_pDriver->GetIsConnected()) {
    vstr::debug()
        << "[WiiMoteParameters::SetWiiMoteId]: Already connected, can't set wait timeout anymore"
        << std::endl;
    return false;
  }
  return compAssignAndNotify<int>(nTimeoutInSecs, m_nWaitTimeout, *this, MSG_WAITTIMEOUT_CHG);
}

// ############################################################################
// RUMBLE FORCE FEEDBACK
// ############################################################################

class WiimoteRumble : public IVistaDriverForceFeedbackAspect {
 public:
  WiimoteRumble()
      : IVistaDriverForceFeedbackAspect()
      , m_pMote(NULL) {
  }

  /**
   * Low-level API to apply forces, this should be called
   * only in very simple cases, for example vibrating joysticks. For more advanced
   * support, check the SetForceConstraintModel() API
   * @see SetForceConstraintModel()
   * @param v3Force the force vector to set on the device API (dir+length)
   * @param qTorque the angular force to apply, not all devices may support this
   */
  virtual bool SetForce(const VistaVector3D& v3Force, const VistaVector3D& v3Torque) {
    if (!m_pMote)
      return false;

    wiiuse_rumble(
        m_pMote, (v3Force.GetLengthSquared() > std::numeric_limits<float>::epsilon() ? 1 : 0));
    return true;
  }

  /**
   * @return the number of DOF for force input (0 for none)
   */
  virtual int GetNumInputDOF() const {
    return 0;
  }

  /**
   * @return the number of DOF for force output (0 for none)
   */
  virtual int GetNumOutputDOF() const {
    return 1;
  }

  /**
   * Returns the maximum stiffness of this device.
   * @return 0 when this parameter is not present, > 0 else
   */
  virtual float GetMaximumStiffness() const {
    return 0;
  }

  /**
   * Returns the maximal force to be output by the device.
   * @return 0 for not applicable, > 0 else
   */
  virtual float GetMaximumForce() const {
    return 0;
  }

  /**
   * Enable / Disable the force output. Calls to SetForce() can
   * still be done in state disabled, but have no effect.
   */
  virtual bool SetForcesEnabled(bool bEnabled) {
    return true;
  }

  /**
   * @return true when forces are enabled for this aspect, false else.
   * @see SetForcesEnabled()
   */
  virtual bool GetForcesEnabled() const {
    return true;
  }

  wiimote_t* m_pMote;
};

typedef TVistaDriverEnableAspect<VistaWiimoteDriver> VistaWiimoteEnableAspect;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaWiimoteDriver::VistaWiimoteDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pInfo(new VistaDriverInfoAspect)
    ,
    //  m_pUpdate(NULL),
    m_pProtocol(new IVistaDriverProtocolAspect)
    , m_pMappingAspect(new VistaDriverSensorMappingAspect(crm))
    , m_nWiimoteId(~0u)
    , m_nNunchukId(~0u)
    , m_nStatusId(~0u)
    , m_nClassicId(~0u)
    , m_pMote(NULL)
    , m_nState(VistaWiimoteDriverCommonShare::ST_NOEXP)
    , m_bConnected(false)
    , m_pRumble(new WiimoteRumble)
    , m_bParamChangeFlag(false) {
  m_pParams       = new VistaDriverGenericParameterAspect(new WiiMoteParameterCreate(this));
  m_pThreadAspect = new VistaDriverThreadAspect(this);
  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);
  RegisterAspect(m_pInfo);
  RegisterAspect(m_pProtocol);
  RegisterAspect(m_pMappingAspect);
  RegisterAspect(m_pParams);
  RegisterAspect(m_pRumble);
  RegisterAspect(m_pThreadAspect);

  RegisterAspect(new VistaWiimoteEnableAspect(this, &VistaWiimoteDriver::PhysicalEnable));
}

VistaWiimoteDriver::~VistaWiimoteDriver() {
  VistaWiimoteEnableAspect* enabler =
      GetAspectAs<VistaWiimoteEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;

  UnregisterAspect(m_pInfo, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pInfo;

  UnregisterAspect(m_pProtocol, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pProtocol;

  UnregisterAspect(m_pMappingAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pMappingAspect;

  UnregisterAspect(m_pRumble, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pRumble;

  m_pThreadAspect->StopProcessing();

  UnregisterAspect(m_pThreadAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pThreadAspect;

  if (m_pMote) {
    wiiuse_set_leds(m_pMote[0], WIIMOTE_LED_NONE);
    wiiuse_cleanup(m_pMote, 1);
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaWiimoteDriver::DoDisconnect() {
  if (m_pMote) {
    wiiuse_set_leds(m_pMote[0], WIIMOTE_LED_NONE);
    wiiuse_cleanup(m_pMote, 1);

    m_pMote = 0;
  }

  return m_pMote == 0;
}

bool VistaWiimoteDriver::DoConnect() {
  if (m_bConnected)
    return true;

  vstr::debug() << "VistaWiimoteDriver::Connect() -- put mote in discover mode.\n";
  VistaWiimoteDriver::WiiMoteParameters* pParams =
      dynamic_cast<VistaWiimoteDriver::WiiMoteParameters*>(m_pParams->GetParameterContainer());

#ifndef WIN32
  wiimote** wiimotes;
  wiimotes = wiiuse_init(4); // 4 seems to be the max

  int found = wiiuse_find(wiimotes, 4, pParams->GetWaitTimeout());
  if (!found) {
    vstr::debug() << "[WiimoteDriver::Connect]: No wiimotes found." << std::endl;
    return 0;
  } else
    vstr::debug() << "[WiimoteDriver::Connect]: found " << found << " wiimotes" << std::endl;

  std::string strWiiMoteId = pParams->GetWiiMoteId();
  int nIndex = (strWiiMoteId.empty() ? 0 : FindCorrectWiimote(wiimotes, found, strWiiMoteId));
  if (nIndex == -1)
    return false;

  m_pMote = wiiuse_init(1);

  // copy-in the correct/desired index
  memcpy(m_pMote[0], wiimotes[nIndex], sizeof(wiimote_t));

  for (int n = 0; n < found; n++)
    // we can not call wiiuse_cleanup here,
    // as this will free AND disconnect the structure
    free(wiimotes[n]); // give back some memory

  // give back the container memory
  free(wiimotes);
#else
  wiimote** wiimotes;
  wiimotes = wiiuse_init(4);

  int found = wiiuse_find(wiimotes, 4, pParams->GetWaitTimeout());
  if (!found) {
    vstr::debug() << "[WiimoteDriver::Connect]: No wiimotes found." << std::endl;
    return 0;
  } else
    vstr::debug() << "[WiimoteDriver::Connect]: found " << found << " wiimotes" << std::endl;

  std::string strWiiMoteId = pParams->GetWiiMoteId();
  int         nIndex       = 0;
  if (VistaConversion::FromString(strWiiMoteId, nIndex) == false) {
    vstr::warnp() << "[WiimoteDriver::Connect]: Wiimote index [" << strWiiMoteId
                  << "] does not represent an int" << std::endl;
    return false;
  }
  if (nIndex < 0 || nIndex > found) {
    vstr::warnp() << "[WiimoteDriver::Connect]: Wiimote index [" << nIndex << "] out of range"
                  << std::endl;
    return false;
  }

  m_pMote = wiiuse_init(1);

  // copy-in the correct/desired index
  memcpy(m_pMote[0], wiimotes[nIndex], sizeof(wiimote_t));

  //@todo: why does this crash?
  // for( int n=0; n < found; n++ )
  //	// we can not call wiiuse_cleanup here,
  //	// as this will free AND disconnect the structure
  //	free( wiimotes[n] ); // give back some memory

  // give back the container memory
  // free( wiimotes );

  int iLed;
  switch (nIndex) {
  case 0:
    iLed = WIIMOTE_LED_1;
    break;
  case 1:
    iLed = WIIMOTE_LED_2;
    break;
  case 2:
    iLed = WIIMOTE_LED_3;
    break;
  case 3:
    iLed = WIIMOTE_LED_4;
    break;
  default:
    iLed = WIIMOTE_LED_NONE;
    break;
  }
  wiiuse_set_leds(m_pMote[0], iLed);
#endif

  // now connect to the wiimote of desire...
  if (wiiuse_connect(m_pMote, 1) == 1) {
    // setup other parameters as given by the user:
    m_bConnected       = true;
    m_pRumble->m_pMote = m_pMote[0];
    return SetupWiiMote();
  } else {
    // should cleanup
    wiiuse_cleanup(m_pMote, 1);
    m_pMote = NULL;
  }
  return false;
}

bool VistaWiimoteDriver::SetupWiiMote() {
  VistaWiimoteDriver::WiiMoteParameters* pParams =
      dynamic_cast<VistaWiimoteDriver::WiiMoteParameters*>(m_pParams->GetParameterContainer());

  wiiuse_motion_sensing(m_pMote[0], pParams->GetDoesAccelerationReports() ? 1 : 0);
  wiiuse_set_ir(m_pMote[0], pParams->GetDoesIRReports() ? 1 : 0);

  int nThreshold = pParams->GetAccelerationThreshold();
  wiiuse_set_accel_threshold(m_pMote[0], nThreshold);
  if (nThreshold == 0)
    wiiuse_set_flags(m_pMote[0], 0, WIIUSE_ORIENT_THRESH);
  else
    wiiuse_set_flags(m_pMote[0], WIIUSE_ORIENT_THRESH, 0);

  m_bParamChangeFlag = false;

  return true;
}

bool VistaWiimoteDriver::PhysicalEnable(bool bEnable) {
  if (bEnable) {
    if (m_pThreadAspect->GetIsProcessing() && m_nWiimoteId != ~0u)
      return true;

    DetermineSetup();
    m_pThreadAspect->UnpauseProcessing();
  } else {
    m_pThreadAspect->PauseProcessing();
    m_nWiimoteId = m_nGuitarId = m_nNunchukId = m_nStatusId = m_nClassicId = ~0;
  }
  return true;
}

int VistaWiimoteDriver::DetermineSetup() {
  // want normal mote?
  m_nWiimoteId = m_pMappingAspect->GetSensorId(m_pMappingAspect->GetTypeId("WIIMOTE"), 0);
  m_nGuitarId  = m_pMappingAspect->GetSensorId(m_pMappingAspect->GetTypeId("GUITAR"), 0);
  m_nNunchukId = m_pMappingAspect->GetSensorId(m_pMappingAspect->GetTypeId("NUNCHUK"), 0);
  m_nStatusId  = m_pMappingAspect->GetSensorId(m_pMappingAspect->GetTypeId("STATUS"), 0);
  m_nClassicId = m_pMappingAspect->GetSensorId(m_pMappingAspect->GetTypeId("CLASSIC"), 0);

  return 0;
}

bool VistaWiimoteDriver::DoSensorUpdate(VistaType::microtime nTs) {
  if (!m_bConnected)
    return false;

  if (m_bParamChangeFlag)
    SetupWiiMote();

  bool bDoMoteRecord = false; // WIIUSE_USING_ACC( m_pMote[0] ) || WIIUSE_USING_IR( m_pMote[0] );

  if (wiiuse_poll(m_pMote, 1)) {
    // new values!
    switch (m_pMote[0]->event) {
    case WIIUSE_EVENT: {
      switch (m_pMote[0]->exp.type) {
      case EXP_CLASSIC: {
        RecordExtSensor<classic_ctrl_t>(
            nTs, m_nClassicId, reinterpret_cast<const classic_ctrl_t*>(&m_pMote[0]->exp.classic));
        break;
      }
      case EXP_GUITAR_HERO_3: {
        RecordExtSensor<guitar_hero_3_t>(
            nTs, m_nGuitarId, reinterpret_cast<const guitar_hero_3_t*>(&m_pMote[0]->exp.gh3));
        break;
      }
      case EXP_NUNCHUK: {
        RecordExtSensor<nunchuk_t>(
            nTs, m_nNunchukId, reinterpret_cast<const nunchuk_t*>(&m_pMote[0]->exp.nunchuk));
        break;
      }
      default:
        break;
      }
      bDoMoteRecord = true;
      break;
    }
    case WIIUSE_STATUS: {
      RecordStateEvent(nTs);
      break;
    }
    case WIIUSE_DISCONNECT:
    case WIIUSE_UNEXPECTED_DISCONNECT: {
      vstr::debug() << "DISCONNECT event.\n";
      RecordStateEvent(nTs);
      m_bConnected       = false;
      m_pRumble->m_pMote = NULL;
      break;
    }
    case WIIUSE_NUNCHUK_INSERTED: {
      m_nState = m_nState | VistaWiimoteDriverCommonShare::ST_NUNCHUK;
      vstr::debug() << "Extension NUNCHUK INSERT event.\n";
      RecordStateEvent(nTs);
      break;
    }
    case WIIUSE_CLASSIC_CTRL_INSERTED: {
      m_nState = m_nState | VistaWiimoteDriverCommonShare::ST_CLASSIC;
      vstr::debug() << "Extension CLASSIC INSERT event.\n";
      RecordStateEvent(nTs);
      break;
    }
    case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED: {
      m_nState = m_nState | VistaWiimoteDriverCommonShare::ST_GUITAR;
      vstr::debug() << "Extension GUITAR INSERT event.\n";
      RecordStateEvent(nTs);
      break;
    }
    case WIIUSE_NUNCHUK_REMOVED: {
      m_nState = m_nState & ~VistaWiimoteDriverCommonShare::ST_NUNCHUK;
      vstr::debug() << "Extension NUNCHUK INSERT event.\n";
      RecordStateEvent(nTs);
      break;
    }
    case WIIUSE_CLASSIC_CTRL_REMOVED: {
      m_nState = m_nState & ~VistaWiimoteDriverCommonShare::ST_CLASSIC;
      vstr::debug() << "Extension CLASSIC INSERT event.\n";
      RecordStateEvent(nTs);
      break;
    }
    case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED: {
      m_nState = m_nState & ~VistaWiimoteDriverCommonShare::ST_GUITAR;
      vstr::debug() << "Extension GUITAR INSERT event.\n";
      RecordStateEvent(nTs);
      break;
    }
    default:
      break;
    }
  }

  if (bDoMoteRecord)
    RecordMoteEvent(nTs);

  IVistaDriverForceFeedbackAspect::IForceAlgorithm* pFF = m_pRumble->GetForceAlgorithm();
  if (pFF != NULL) {
    VistaVector3D crPos((float)m_pMote[0]->ir.x, (float)m_pMote[0]->ir.y, (float)m_pMote[0]->ir.z);
    VistaVector3D crVel =
        crPos + VistaVector3D(m_pMote[0]->accel.x, m_pMote[0]->accel.y, m_pMote[0]->accel.z);
    VistaTransformMatrix m;
    m.SetToRotationMatrix(VistaQuaternion(VistaEulerAngles(
        m_pMote[0]->orient.yaw, m_pMote[0]->orient.roll, m_pMote[0]->orient.pitch)));

    // m.Rotation(m_pMote[0]->orient.yaw,
    //		   m_pMote[1]->orient.roll,
    //                 m_pMote[0]->orient.pitch);

    VistaQuaternion q(m);

    VistaVector3D rV3;
    VistaVector3D v3Dummy;
    if (pFF->UpdateForce(double(nTs), crPos, crVel, q, rV3, v3Dummy)) {
      m_pRumble->SetForce(rV3, v3Dummy);
    }
  }
  return true;
}

bool VistaWiimoteDriver::RecordMoteEvent(VistaType::microtime nTs) {
  if (m_nWiimoteId == ~0u)
    return false;
  // the mote event is somewhat special, as we need to distill the
  // relevant information out of a pool of the complete event
  // structure.

  // first: get some memory.
  VistaDeviceSensor* pSensor = GetSensorByIndex(m_nWiimoteId);

  MeasureStart(m_nWiimoteId, nTs);
  // get the current place for the decoding for sensor 0
  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

  // for the sake of readability: interpret the memory blob
  // as a place to store a sample for the IRMan
  VistaWiimoteDriverCommonShare::MeasureType* s =
      pM->getWrite<VistaWiimoteDriverCommonShare::MeasureType>();

  // now copy field-wise
  memcpy(&s->m_Acceleration, &m_pMote[0]->accel, sizeof(vec3b_t));
  memcpy(&s->m_Orientation, &m_pMote[0]->orient, sizeof(orient_t));
  memcpy(&s->m_GravityForce, &m_pMote[0]->gforce, sizeof(gforce_t));
  memcpy(&s->m_IR, &m_pMote[0]->ir, sizeof(ir_t));
  memcpy(&s->m_Acceleration_calib, &m_pMote[0]->accel_calib, sizeof(accel_t));

  s->m_buttons = m_pMote[0]->btns;

  MeasureStop(m_nWiimoteId);
  pSensor->SetUpdateTimeStamp(nTs);

  return true;
}

bool VistaWiimoteDriver::RecordStateEvent(VistaType::microtime nTs) {
  if (m_nStatusId == ~0u)
    return false;
  // the mote event is somewhat special, as we need to distill the
  // relevant information out of a pool of the complete event
  // structure.

  // first: get some memory.
  VistaDeviceSensor* pSensor = GetSensorByIndex(m_nStatusId);

  MeasureStart(m_nStatusId, nTs);
  // get the current place for the decoding for sensor 0
  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

  VistaWiimoteDriverCommonShare::StateMeasureType* s =
      pM->getWrite<VistaWiimoteDriverCommonShare::StateMeasureType>();

  // now copy field-wise
  s->m_nState        = m_pMote[0]->state;
  s->m_nBatteryLevel = m_pMote[0]->battery_level;
  s->m_cLEDs         = m_pMote[0]->leds;

  s->m_nExpansionState = m_nState;

  MeasureStop(m_nStatusId);
  pSensor->SetUpdateTimeStamp(nTs);
  return true;
}

bool VistaWiimoteDriver::EnableWiiMoteSensor(
    unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec) {
  return EnableSensorTyped("WIIMOTE", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableNunchukSensor(
    unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec) {
  return EnableSensorTyped("NUNCHUK", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableClassicSensor(
    unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec) {
  return EnableSensorTyped("CLASSIC", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableGuitarSensor(
    unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec) {
  return EnableSensorTyped("GUITAR", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

bool VistaWiimoteDriver::EnableStateSensor(
    unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec) {
  return EnableSensorTyped("STATUS", nMaxSlotsToRead, nMaxHistoryAccessTimeInMsec);
}

void VistaWiimoteDriver::SetParamChangeFlag() {
  m_bParamChangeFlag = true;
}

bool VistaWiimoteDriver::EnableSensorTyped(
    const std::string& strType, unsigned int nMaxSlotsToRead, float nMaxHistoryAccessTimeInMsec) {
  if (m_pMappingAspect->GetIsType(strType) == false)
    return false;

  VistaDeviceSensor*              pSensor = new VistaDeviceSensor;
  IVistaMeasureTranscoderFactory* pFac
      //= GetDriverFactoryMethod()->GetTranscoderFactoryForSensor(strType);
      = GetFactory()->GetTranscoderFactoryForSensor(strType);

  pSensor->SetMeasureTranscode(pFac->CreateTranscoder());
  pSensor->SetTypeHint("");

  unsigned int nId = AddDeviceSensor(pSensor);
  // unsigned int nSensorType = VistaWiimoteDriver::GetDriverFactoryMethod()->GetTypeFor(strType);
  unsigned int nSensorType = m_pMappingAspect->GetTypeId(strType);
  // only inspect 1 slot in 1 msec
  SetupSensorHistory(pSensor, nMaxSlotsToRead, (VistaType::microtime)nMaxHistoryAccessTimeInMsec);
  m_pMappingAspect->SetSensorId(nSensorType, 0, nId);
  return true;
}

VistaWiimoteDriver::WiiMoteParameters* VistaWiimoteDriver::GetParameters() const {
  return dynamic_cast<VistaWiimoteDriver::WiiMoteParameters*>(m_pParams->GetParameterContainer());
}

bool VistaWiimoteDriver::GetIsConnected() const {
  return m_bConnected;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/************************** CR / LF nicht vergessen! **************************/
