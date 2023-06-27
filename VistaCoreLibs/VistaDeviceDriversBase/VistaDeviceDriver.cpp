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

#include "VistaDeviceDriver.h"
#include "DriverAspects/VistaDriverEnableAspect.h"
#include "DriverAspects/VistaDriverMeasureHistoryAspect.h"
#include "VistaDeviceSensor.h"

#include <VistaInterProcComm/DataLaVista/Base/VistaRTC.h>

#include <VistaAspects/VistaPropertyFunctor.h>
#include <VistaAspects/VistaPropertyFunctorRegistry.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimer.h>

#include <algorithm>
#include <cassert>
#include <cmath>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

size_t               IVistaDeviceDriver::ANYPOS                  = ~0u;
unsigned int         IVistaDeviceDriver::INVALID_INDEX           = ~0u;
size_t               IVistaDeviceDriver::UNKNOWN_SIZE            = ~0u;
VistaType::microtime IVistaDeviceDriver::UNKNOWN_ACCESS_DURATION = ~0u;

const unsigned int IVistaDriverCreationMethod::INVALID_SIZE     = ~0u;
const unsigned int IVistaDriverCreationMethod::INVALID_ESTIMATE = ~0u;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaDeviceDriver::IVistaDeviceDriver(IVistaDriverCreationMethod* fac, unsigned int nDefHistSize)
    : m_eUpdType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL)
    , m_pHistoryAspect(new VistaDriverMeasureHistoryAspect)
    , m_pFrequencyTimer(new VistaWindowAverageTimer(50))
    , m_pUpdateTimer(new VistaAverageTimer)
    , m_defaultHistorySizes()
    , m_pFactory(fac)
    , m_pEnableAspect(0)
    , m_bEnabledFlag(false)
    , m_isConnected(false) {
  RegisterAspect(m_pHistoryAspect);
}

IVistaDeviceDriver::~IVistaDeviceDriver() {
  UnregisterAspect(m_pHistoryAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT,
      IVistaDeviceDriver::FORCE_UNREGISTRATION);
  delete m_pHistoryAspect;

  delete m_pFrequencyTimer;
  delete m_pUpdateTimer;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

const VistaWindowAverageTimer& IVistaDeviceDriver::GetFrequencyTimer() const {
  return *m_pFrequencyTimer;
}

VistaType::microtime IVistaDeviceDriver::GetAverageUpdateFrequency() const {
  return (1.0 / m_pFrequencyTimer->GetAverageTime());
}

const VistaAverageTimer& IVistaDeviceDriver::GetUpdateTimer() const {
  return *m_pUpdateTimer;
}

VistaType::microtime IVistaDeviceDriver::GetAverageUpdateTime() const {
  return m_pUpdateTimer->GetAverageTime();
}

VistaSensorMeasure* IVistaDeviceDriver::MeasureStart(
    unsigned int nSensorIdx, VistaType::microtime dTs, eMeasureStartBehavior getCurrent) {
  VistaDeviceSensor* pSen = GetSensorByIndex(nSensorIdx);
  if (pSen)
    return MeasureStart(*pSen, dTs, getCurrent);

  return NULL;
}

VistaSensorMeasure* IVistaDeviceDriver::MeasureStart(
    VistaDeviceSensor& Sen, VistaType::microtime dTs, eMeasureStartBehavior getCurrent) {
  m_pHistoryAspect->MeasureStart(&Sen, dTs);
  if (getCurrent == RETURN_CURRENT_SLOT)
    return m_pHistoryAspect->GetCurrentSlot(&Sen);

  return NULL;
}

void IVistaDeviceDriver::MeasureStop(unsigned int nSensorIdx) {
  VistaDeviceSensor* pSen = GetSensorByIndex(nSensorIdx);
  if (pSen)
    MeasureStop(*pSen);
}

void IVistaDeviceDriver::MeasureStop(VistaDeviceSensor& sensor) {
  m_pHistoryAspect->MeasureStop(&sensor);
}

IVistaDeviceDriver::eUpdateType IVistaDeviceDriver::GetUpdateType() const {
  return m_eUpdType;
}

void IVistaDeviceDriver::SetUpdateType(eUpdateType eTp) {
  m_eUpdType = eTp;
}

void IVistaDeviceDriver::SwapSensorMeasures(std::vector<int>* liIds) {
}

unsigned int IVistaDeviceDriver::GetNumberOfSensors() const {
  return (unsigned int)m_vecSensors.size();
}

VistaDeviceSensor* IVistaDeviceDriver::GetSensorByIndex(unsigned int uiIndex) const {
  if (uiIndex >= GetNumberOfSensors())
    return NULL;

  return m_vecSensors[uiIndex];
}

VistaDeviceSensor* IVistaDeviceDriver::GetSensorByName(const std::string& strName) const {
  for (std::vector<VistaDeviceSensor*>::const_iterator cit = m_vecSensors.begin();
       cit != m_vecSensors.end(); ++cit) {
    if (strName == (*cit)->GetSensorName())
      return (*cit);
  }
  return NULL;
}

bool IVistaDeviceDriver::HasSensorPointer(VistaDeviceSensor* pSensor) const {
  return (std::find(m_vecSensors.begin(), m_vecSensors.end(), pSensor) != m_vecSensors.end());
}

unsigned int IVistaDeviceDriver::AddDeviceSensor(
    VistaDeviceSensor* pSensor, size_t pos, AllocMemoryFunctor* amf, ClaimMemoryFunctor* cmf) {
  bool bReRegister = true;
  if (!HasSensorPointer(pSensor)) {
    if (pos == ANYPOS || m_vecSensors.empty())
      m_vecSensors.push_back(pSensor);
    else
      m_vecSensors.insert(m_vecSensors.begin() + pos, pSensor);

    pSensor->SetParent(this);
    bReRegister = false;
  }

  // prepare history
  m_pHistoryAspect->RegisterSensor(pSensor, amf, cmf, bReRegister);
  pSensor->SetParent(this);
  // if( m_nDefaultHistorySize != ~0 )
  //{
  //	SetupSensorHistory( pSensor, m_nDefaultHistorySize, 66.6 );
  //}
  return (unsigned int)m_vecSensors.size() - 1;
}

bool IVistaDeviceDriver::RemDeviceSensor(VistaDeviceSensor* pSensor) {
  std::vector<VistaDeviceSensor*>::iterator beg =
      std::remove(m_vecSensors.begin(), m_vecSensors.end(), pSensor);
  if (beg != m_vecSensors.end()) {
    m_pHistoryAspect->UnregisterSensor(pSensor);
    m_vecSensors.erase(beg, m_vecSensors.end());
    pSensor->SetParent(NULL);
    return true;
  }
  return false;
}

bool IVistaDeviceDriver::RemDeviceSensorByIndex(unsigned int uiIndex) {

  if (uiIndex >= m_vecSensors.size())
    return false;

  std::vector<VistaDeviceSensor*>::iterator it = m_vecSensors.end();
  for (unsigned int i = 0; i < uiIndex; ++i) {
    if (it == m_vecSensors.end())
      it = m_vecSensors.begin();
    else
      ++it;
  }

  if (it != m_vecSensors.end()) {
    m_pHistoryAspect->UnregisterSensor(*it);
    (*it)->SetParent(NULL);
    m_vecSensors.erase(it);

    return true;
  }

  return false;
}

bool IVistaDeviceDriver::SetupSensorHistory(VistaDeviceSensor* pSensor,
    unsigned int nMaxSlotsToRead, VistaType::microtime nMaxHistoryAccessTimeInMsec) {
  unsigned int nUpdateRateOfSensorInHz =
      GetFactory()->GetUpdateEstimatorFor(GetFactory()->GetTypeFor(pSensor->GetTypeHint()));
  if (nUpdateRateOfSensorInHz == IVistaDriverCreationMethod::INVALID_ESTIMATE)
    return false;

  int nNum = int(
      ceilf((float)nMaxHistoryAccessTimeInMsec / float(1000.0f / float(nUpdateRateOfSensorInHz))));
  unsigned int nMeasureSize =
      GetFactory()->GetMeasureSizeFor(GetFactory()->GetTypeFor(pSensor->GetTypeHint()));
  if (nMeasureSize == IVistaDriverCreationMethod::INVALID_SIZE)
    nMeasureSize = 0;

  return m_pHistoryAspect->SetHistorySize(pSensor, nMaxSlotsToRead, 2 * nNum, nMeasureSize);
}

bool IVistaDeviceDriver::RequestHistorySize(VistaDeviceSensor* pSensor,
    unsigned int nMaxSlotsToRead, VistaType::microtime nMaxHistoryAccessTimeInMsec) {
  // FIXME
  unsigned int nUpdateRateOfSensorInHz =
      GetFactory()->GetUpdateEstimatorFor(GetFactory()->GetTypeFor(pSensor->GetTypeHint()));
  if (nUpdateRateOfSensorInHz == IVistaDriverCreationMethod::INVALID_ESTIMATE)
    return false;

  unsigned int nNum = (unsigned int)(ceilf(
      (float)nMaxHistoryAccessTimeInMsec / float(1000.0f / float(nUpdateRateOfSensorInHz))));
  if (2 * nNum > m_pHistoryAspect->GetUserReadHistorySize(pSensor)) {
    unsigned int measureSize =
        GetFactory()->GetMeasureSizeFor(GetFactory()->GetTypeFor(pSensor->GetTypeHint()));

    if (measureSize == IVistaDriverCreationMethod::INVALID_SIZE)
      measureSize = 1; // need to have at least one byte!

    return m_pHistoryAspect->SetHistorySize(pSensor, nMaxSlotsToRead, 2 * nNum, measureSize);
  }
  return false;
}

void IVistaDeviceDriver::RegisterSensorInfo(
    unsigned int nSensorIndex, const SensorInfo& sensor_info) {
  m_defaultHistorySizes[nSensorIndex] = sensor_info;
}

IVistaDeviceDriver::SensorInfo IVistaDeviceDriver::GetSensorInfo(unsigned int nSensorIndex) const {
  SENSORINFOMAP::const_iterator cit = m_defaultHistorySizes.find(nSensorIndex);
  if (cit != m_defaultHistorySizes.end())
    return (*cit).second;
  return SensorInfo();
}

bool IVistaDeviceDriver::Update() {
  if (!GetIsEnabled())
    return false;

  m_pUpdateTimer->StartRecording();
  VistaType::microtime d0   = m_pUpdateTimer->GetMicroTime();
  bool                 bRet = DoSensorUpdate(d0);

  if (bRet) {
    // record successful update
    m_pUpdateTimer->RecordTime();
    m_pFrequencyTimer->RecordTime();
  }
  return bRet;
}

bool IVistaDeviceDriver::PreUpdate() {
  return true;
}

bool IVistaDeviceDriver::PostUpdate() {
  return true;
}

bool IVistaDeviceDriver::GetIsEnabled() const {
  if (m_pEnableAspect)
    return m_pEnableAspect->GetIsEnabled();

  // drivers that do not have an enabler are always enabled
  return m_bEnabledFlag;
}

void IVistaDeviceDriver::SetIsEnabled(bool bEnabled) {
  if (m_pEnableAspect) {
    if (m_pEnableAspect->SetIsEnabled(bEnabled) == false)
      vstr::warnp() << "[IVistaDeviceDriver]: PhysicalEnable call failed." << std::endl;
    else
      return;
  }

  // version for drivers that do not support an enabled aspect
  m_bEnabledFlag = bEnabled;
}

void IVistaDeviceDriver::DeleteAllSensors() {
  for (std::vector<VistaDeviceSensor*>::iterator it = m_vecSensors.begin();
       it != m_vecSensors.end(); ++it) {
    m_pHistoryAspect->UnregisterSensor(*it);
    delete (*it);
  }
  m_vecSensors.clear();
}

IVistaDriverCreationMethod* IVistaDeviceDriver::GetFactory() const {
  return m_pFactory;
}

// void IVistaDeviceDriver::SetDefaultHistorySize(unsigned int nSize)
//{
//	m_nDefaultHistorySize = nSize;
//}

bool IVistaDeviceDriver::Connect() {
  SetIsConnected(DoConnect());
  return GetIsConnected();
}

bool IVistaDeviceDriver::Disconnect() {
  SetIsConnected(DoDisconnect());
  return !GetIsConnected(); // return true on disconnection
}

void IVistaDeviceDriver::SetIsConnected(bool bConnected) {
  m_isConnected = bConnected;
}

bool IVistaDeviceDriver::GetIsConnected() const {
  return m_isConnected;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const unsigned int IVistaDriverCreationMethod::INVALID_TYPE = ~0;

IVistaDriverCreationMethod::IVistaDriverCreationMethod(IVistaTranscoderFactoryFactory* metaFac)
    : m_nTypeCount(0)
    , m_metaFac(metaFac) {
}

IVistaDriverCreationMethod::~IVistaDriverCreationMethod() {
  std::list<std::string> liTranscoders = GetTypeNames();
  std::for_each(
      liTranscoders.begin(), liTranscoders.end(), _unregisterAndDeleteTranscoderFac(this));
}

IVistaTranscoderFactoryFactory* IVistaDriverCreationMethod::GetMetaFactory() const {
  return m_metaFac;
}

unsigned int IVistaDriverCreationMethod::RegisterSensorType(const std::string& strName,
    unsigned int nMeasureSize, unsigned int nUpdateEstimator,
    IVistaMeasureTranscoderFactory* pFac) {
  // first, lookup the type name
  if (m_mapTypeNames.find(strName) != m_mapTypeNames.end())
    return INVALID_TYPE; // indicate failure

  // ok, this name is not yet registered, so create a mapping
  m_mapTypeNames[strName] = m_nTypeCount;

  // push factory to its proper place

  m_mapFactories[m_nTypeCount] = _sL(pFac, nMeasureSize, nUpdateEstimator); // copy in
  AddTranscoderType(pFac->GetTranscoderName());
  return m_nTypeCount++; // increase counter on leave for the next registration
}

bool IVistaDriverCreationMethod::SetMeasureSizeFor(
    const std::string& strType, unsigned int nNewSize) {
  // first, lookup the type name
  MAPNAME::const_iterator cit = m_mapTypeNames.find(strType);
  if (cit == m_mapTypeNames.end())
    return false;

  unsigned int nType = (*cit).second;
  _sL&         l     = m_mapFactories[nType];

  l.m_nMeasureSize = nNewSize;

  return true;
}

bool IVistaDriverCreationMethod::UnregisterType(const std::string& strType, bool bDeleteFactories) {
  MAPNAME::iterator it = m_mapTypeNames.find(strType);
  if (it == m_mapTypeNames.end())
    return true; // not registered == unregistered

  // lookup the _sL
  FACMAP::iterator fit = m_mapFactories.find((*it).second);

  // should not be end() !!
  if (bDeleteFactories && m_metaFac)
    m_metaFac->DestroyTranscoderFactory((*fit).second.m_pFac); // kill fac

  m_mapFactories.erase(fit); // erase from map
  m_mapTypeNames.erase(it);  // erase from map

  return true;
}

std::list<std::string> IVistaDriverCreationMethod::GetTypeNames() const {
  std::list<std::string> liRet;
  for (MAPNAME::const_iterator cit = m_mapTypeNames.begin(); cit != m_mapTypeNames.end(); ++cit) {
    liRet.push_back((*cit).first);
  }

  return liRet;
}

bool IVistaDriverCreationMethod::GetTypeNameFor(unsigned int nTypeId, std::string& strDest) const {
  for (MAPNAME::const_iterator cit = m_mapTypeNames.begin(); cit != m_mapTypeNames.end(); ++cit) {
    if ((*cit).second == nTypeId) {
      strDest = (*cit).first;
      return true;
    }
  }
  return false;
}

std::list<unsigned int> IVistaDriverCreationMethod::GetTypes() const {
  std::list<unsigned int> liRet;
  for (MAPNAME::const_iterator cit = m_mapTypeNames.begin(); cit != m_mapTypeNames.end(); ++cit) {
    liRet.push_back((*cit).second);
  }

  liRet.sort();

  return liRet;
}

IVistaMeasureTranscoderFactory* IVistaDriverCreationMethod::GetTranscoderFactoryForSensor(
    const std::string& strTypeName) const {
  MAPNAME::const_iterator cit = m_mapTypeNames.find(strTypeName);
  if (cit == m_mapTypeNames.end())
    return NULL;
  return GetTranscoderFactoryForSensor((*cit).second);
}

unsigned int IVistaDriverCreationMethod::GetTypeFor(const std::string& strType) const {
  MAPNAME::const_iterator cit = m_mapTypeNames.find(strType);
  if (cit == m_mapTypeNames.end())
    return INVALID_TYPE;
  return (*cit).second;
}

unsigned int IVistaDriverCreationMethod::GetMeasureSizeFor(unsigned int nType) const {
  FACMAP::const_iterator it = m_mapFactories.find(nType);
  if (it == m_mapFactories.end())
    return INVALID_TYPE; // indicate failure

  const _sL& sL = (*it).second;
  return sL.m_nMeasureSize;
}

unsigned int IVistaDriverCreationMethod::GetUpdateEstimatorFor(unsigned int nType) const {
  FACMAP::const_iterator it = m_mapFactories.find(nType);
  if (it == m_mapFactories.end())
    return INVALID_TYPE; // indicate failure

  const _sL& sL = (*it).second;
  return sL.m_nUpdateEstimator;
}

IVistaMeasureTranscoderFactory* IVistaDriverCreationMethod::GetTranscoderFactoryForSensor(
    unsigned int nType) const {
  FACMAP::const_iterator it = m_mapFactories.find(nType);
  if (it != m_mapFactories.end())
    return (*it).second.m_pFac;

  return NULL;
}

void IVistaDriverCreationMethod::OnUnload() {
  VistaPropertyFunctorRegistry* pReg = VistaPropertyFunctorRegistry::GetSingleton();

  for (TRANTP::const_iterator cit = m_liTranscoderTypes.begin(); cit != m_liTranscoderTypes.end();
       ++cit) {
    // get all getter symbols from prop fac with the co-responding type tag.
    std::list<IVistaPropertyGetFunctor*> liGetterByClass = pReg->GetGetterByClass(*cit);
    std::list<IVistaPropertySetFunctor*> liSetterByClass = pReg->GetSetterByClass(*cit);

    // unregister all getters first

    for (std::list<IVistaPropertyGetFunctor*>::iterator git = liGetterByClass.begin();
         git != liGetterByClass.end(); ++git) {
      // should call unregister (ok... this is internal knowledge...)
      delete *git; // pReg->UnregisterGetter( (*git)->GetNameForNameable(),
                   // (*git)->GetPropertyClassName() );
    }

    ;
    for (std::list<IVistaPropertySetFunctor*>::iterator sit = liSetterByClass.begin();
         sit != liSetterByClass.end(); ++sit) {
      delete *sit;
      // pReg->UnregisterGetter( (*sit)->GetNameForNameable(), (*sit)->GetPropertyClassName() );
    }
  }
}

void IVistaDriverCreationMethod::AddTranscoderType(const std::string& strType) {
  m_liTranscoderTypes.push_back(strType);
  m_liTranscoderTypes.unique(); // sort out duplicates (maybe use a set instead?)
}

std::list<std::string> IVistaDriverCreationMethod::GetTranscoderTypes() const {
  return m_liTranscoderTypes;
}

void IVistaDriverCreationMethod::_unregisterAndDeleteTranscoderFac::operator()(
    const std::string& strTypeName) const {
  IVistaMeasureTranscoderFactory* fac = m_crm->GetTranscoderFactoryForSensor(strTypeName);
  if (fac) {
    m_crm->UnregisterType(strTypeName);
    if (m_crm->GetMetaFactory())
      m_crm->GetMetaFactory()->DestroyTranscoderFactory(fac);
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ASPECTS
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int IVistaDeviceDriver::IVistaDeviceDriverAspect::m_nAspectId = -1;

IVistaDeviceDriver::IVistaDeviceDriverAspect::IVistaDeviceDriverAspect(bool bUnregisterable)
    : m_nMyAspectId(~0)
    , m_bCanBeUnregistered(bUnregisterable) {
}

IVistaDeviceDriver::IVistaDeviceDriverAspect::~IVistaDeviceDriverAspect() {
}

int IVistaDeviceDriver::IVistaDeviceDriverAspect::GetId() const {
  return m_nMyAspectId;
}

int IVistaDeviceDriver::IVistaDeviceDriverAspect::GetAspectId() {
  return IVistaDeviceDriverAspect::m_nAspectId;
}

void IVistaDeviceDriver::IVistaDeviceDriverAspect::SetAspectId(int nId) {
  IVistaDeviceDriverAspect::m_nAspectId = nId;
}

void IVistaDeviceDriver::IVistaDeviceDriverAspect::SetId(int id) {
  m_nMyAspectId = id;
}

void IVistaDeviceDriver::IVistaDeviceDriverAspect::Print(std::ostream& out) const {
  out << "aspect has id [ " << m_nMyAspectId << "] and is of type ["
      << VistaConversion::CleanOSTypeName(typeid(*this).name()) << "]" << std::endl;
  vstr::indent(out);
  out << "This aspect can " << (m_bCanBeUnregistered ? "" : "not")
      << " be unregistered without force." << std::endl;
}

IVistaDeviceDriver::IVistaDeviceDriverAspect* IVistaDeviceDriver::GetAspectById(int nId) const {
  ASPECTMAP::const_iterator it = m_mapAspects.find(nId);
  if (it == m_mapAspects.end())
    return NULL;
  return (*it).second;
}

bool IVistaDeviceDriver::RegisterAspect(IVistaDeviceDriverAspect* pDriverAspect) {
  int nId = pDriverAspect->GetId();
  assert(nId >= 0);

  m_mapAspects[nId] = pDriverAspect;

  // update cached pointers
  if (pDriverAspect->GetId() == VistaDriverMeasureHistoryAspect::GetAspectId())
    m_pHistoryAspect = dynamic_cast<VistaDriverMeasureHistoryAspect*>(pDriverAspect);

  if (pDriverAspect->GetId() == VistaDriverEnableAspect::GetAspectId())
    m_pEnableAspect = dynamic_cast<VistaDriverEnableAspect*>(pDriverAspect);

  return true;
}

bool IVistaDeviceDriver::UnregisterAspect(IVistaDeviceDriverAspect* pDriverAspect,
    eDeleteAspectBehavior bDelete, eUnregistrationBehavior bForce) {
  if (pDriverAspect == NULL)
    return true; // that is ok ;)

  if (!pDriverAspect->m_bCanBeUnregistered && !(bForce == FORCE_UNREGISTRATION))
    return false;

  ASPECTMAP::iterator it = m_mapAspects.find(pDriverAspect->GetId());
  if (it == m_mapAspects.end())
    return false;

  m_mapAspects.erase(it);

  if (bDelete == DELETE_ASPECT)
    delete pDriverAspect;

  if (pDriverAspect == m_pEnableAspect)
    m_pEnableAspect = 0;

  if (pDriverAspect == m_pHistoryAspect)
    m_pHistoryAspect = 0;

  return true;
}

std::list<int> IVistaDeviceDriver::EnumerateAspectIds() const {
  std::list<int> liRet;

  for (ASPECTMAP::const_iterator cit = m_mapAspects.begin(); cit != m_mapAspects.end(); ++cit) {
    liRet.push_back((*cit).first);
  }

  return liRet;
}

bool IVistaDeviceDriver::GetAttachOnly() const {
  return false; // default, try to init
}

void IVistaDeviceDriver::SetAttachOnly(bool bAttach) {
  // default: ignore
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
