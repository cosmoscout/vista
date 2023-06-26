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

#include "VistaSensorReadState.h"
#include <VistaBase/VistaBaseTypes.h>

#include "VistaAspects/VistaDeSerializer.h"
#include "VistaAspects/VistaSerializer.h"
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <algorithm>
#include <cassert>

//#include <cstdint>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaType::uint32 VistaSensorReadState::InvalidState = ~0;

int SerializeMeasure(IVistaSerializer& oSer, const VistaSensorMeasure& oMeasure) {
  oSer << oMeasure;
  return static_cast<int>(oMeasure.GetSerializedSize());
}

int DeSerializeMeasure(IVistaDeSerializer& oDeSer, VistaSensorMeasure& oMeasure) {
  oDeSer >> oMeasure;
  return static_cast<int>(oMeasure.GetSerializedSize());
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaSensorReadState::VistaSensorReadState(VistaDeviceSensor* pSensor,
    bool bUseIncrementalSerialization, bool bUseOwnHistoryForDeSerialization)
    : m_nSnapshotReadHead(InvalidState)
    , m_nActivationCount(0)
    , m_nCurrentMeasureCount(0)
    , m_nLastUpdateTs(0)
    , m_pSensor(pSensor)
    , m_pHistory(NULL)
    , m_bOwnHistory(false)
    , m_nLastDeSerializeMeasureCount(0)
    , m_bUseIncrementalSerialization(bUseIncrementalSerialization)
    , m_bUseOwnHistoryForDeSerialization(bUseOwnHistoryForDeSerialization) {
  if (m_pSensor != 0)
    m_pHistory = &m_pSensor->GetDriverMeasures();
}

VistaSensorReadState::~VistaSensorReadState() {
  if (m_bOwnHistory)
    delete m_pHistory;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaSensorReadState::GetIsValid() const {
  return (m_pSensor != 0 && m_pHistory != 0);
}

VistaType::uint32 VistaSensorReadState::GetNewAvailableMeasureCount(
    VistaType::uint32& nLastRead) const {
  VistaType::uint32 nNewMeasureCount = m_nCurrentMeasureCount - nLastRead;
  nNewMeasureCount = std::min<VistaType::uint32>(nNewMeasureCount, m_pHistory->GetClientReadSize());
  nLastRead        = m_nCurrentMeasureCount;
  return nNewMeasureCount;
}

VistaType::uint32 VistaSensorReadState::GetMeasureCount() const {
  return m_nCurrentMeasureCount;
}

VistaType::uint32 VistaSensorReadState::GetActivationCount() const {
  return m_nActivationCount;
}

const VistaSensorMeasure* VistaSensorReadState::GetCurrentMeasure() const {
  return m_pHistory->GetMeasure(m_nSnapshotReadHead);
}

const VistaSensorMeasure* VistaSensorReadState::GetPastMeasure(int nStepBack) const {
  return m_pHistory->GetPast(nStepBack, m_nSnapshotReadHead);
}

VistaDeviceSensor* VistaSensorReadState::GetSensor() const {
  return m_pSensor;
}

VistaType::microtime VistaSensorReadState::GetUpdateTimeStamp() const {
  return m_nLastUpdateTs;
}

bool VistaSensorReadState::SetToLatestSample() {
  if (m_bOwnHistory) {
    vstr::warnp() << "VistaSensorReadState::SetToLatestSample() called for read state with "
                     "remotely received history - "
                  << "reverting to driver history" << std::endl;
    delete m_pHistory;
    m_pHistory = &m_pSensor->GetDriverMeasures();

    // now we do not own the history anymore.
    m_bOwnHistory = false;
  }

  if (GetIsValid() == false)
    return false;

  VistaType::uint32 nMeasureCount = m_pHistory->GetMeasureCount();
  if (nMeasureCount == 0)
    return false; // no measures yet -> no new measures

  if (nMeasureCount <= m_nCurrentMeasureCount)
    return false; // no new measure available

  int                       nCurrentReadHead = m_pHistory->GetCurrentReadHead();
  const VistaSensorMeasure* pMeasure = m_pSensor->GetMeasures().GetMeasure(nCurrentReadHead);

  ++m_nActivationCount;
  m_nSnapshotReadHead    = nCurrentReadHead;
  m_nCurrentMeasureCount = nMeasureCount;
  m_nLastUpdateTs        = pMeasure->m_nMeasureTs;
  return true;
}

int VistaSensorReadState::Serialize(IVistaSerializer& oSerializer) const {
  int nWritten = 0;
  nWritten += oSerializer.WriteInt32(m_nSnapshotReadHead);
  nWritten += oSerializer.WriteInt32(m_nCurrentMeasureCount);
  nWritten += oSerializer.WriteInt32(m_nActivationCount);
  nWritten += oSerializer.WriteDouble(m_nLastUpdateTs);

  nWritten += oSerializer.WriteInt32(m_pHistory->GetClientReadSize());
  nWritten += oSerializer.WriteInt32(m_pHistory->GetDriverWriteSize());
  nWritten += oSerializer.WriteInt32(m_pHistory->GetSensorMeasureSize());

  nWritten += oSerializer.WriteInt32((VistaType::uint32)m_pHistory->GetReadBuffer().GetFirst());
  nWritten += oSerializer.WriteInt32((VistaType::uint32)m_pHistory->GetReadBuffer().GetNext());
  nWritten +=
      oSerializer.WriteInt32((VistaType::uint32)m_pHistory->GetReadBuffer().GetCurrentVal());

  if (m_bUseIncrementalSerialization) {
    int nLastSerializeCount =
        m_mapLastSerializeMeasureCounts[&oSerializer]; // initializes to default if not yet presents

    VistaType::uint32 nNewMeasures = m_nCurrentMeasureCount - nLastSerializeCount;
    // never transmit more measures than the data size
    nNewMeasures =
        std::min(nNewMeasures, (VistaType::uint32)m_pHistory->GetReadBuffer().GetBufferSize());
    nWritten += oSerializer.WriteInt32(nNewMeasures);

    VistaMeasureHistory::Buffer::const_iterator itMeasure =
        m_pHistory->GetReadBuffer().index(m_nSnapshotReadHead);
    for (VistaType::uint32 i = 0; i < nNewMeasures; ++i, --itMeasure) {
      nWritten += SerializeMeasure(oSerializer, (*itMeasure));
    }
    m_mapLastSerializeMeasureCounts[&oSerializer] = m_nCurrentMeasureCount;
  } else {
    const VistaMeasureHistory::Buffer::container_type& oContainer =
        m_pHistory->GetReadBuffer().GetRawAccess();

    for (VistaMeasureHistory::Buffer::container_type::const_iterator itMeasure = oContainer.begin();
         itMeasure != oContainer.end(); ++itMeasure) {
      nWritten += SerializeMeasure(oSerializer, (*itMeasure));
    }
  }
  return nWritten;
}

int VistaSensorReadState::DeSerialize(IVistaDeSerializer& oDeSerializer) {
  int nRead = 0;
  nRead += oDeSerializer.ReadInt32(m_nSnapshotReadHead);
  nRead += oDeSerializer.ReadInt32(m_nCurrentMeasureCount);
  nRead += oDeSerializer.ReadInt32(m_nActivationCount);
  nRead += oDeSerializer.ReadDouble(m_nLastUpdateTs);

  VistaType::uint32 nClientReadSize, nDriverWriteSize, nMeasureSize;
  nRead += oDeSerializer.ReadInt32(nClientReadSize);
  nRead += oDeSerializer.ReadInt32(nDriverWriteSize);
  nRead += oDeSerializer.ReadInt32(nMeasureSize);

  VistaType::uint32 nFirst, nNext, nCurrent;
  nRead += oDeSerializer.ReadInt32(nFirst);
  nRead += oDeSerializer.ReadInt32(nNext);
  nRead += oDeSerializer.ReadInt32(nCurrent);

  if (m_bUseOwnHistoryForDeSerialization && m_bOwnHistory == false) {
    // clone history, and use it to serialize into it
    // if we did not deserialize anytime before, we don't need to copy the
    // original history, otherwise we do
    if (m_nLastDeSerializeMeasureCount == 0)
      m_pHistory = new VistaMeasureHistory(*m_pHistory, false);
    else
      m_pHistory = new VistaMeasureHistory(*m_pHistory, true);
    m_bOwnHistory = true;
  }

  if (m_pHistory->GetWriteBuffer().GetBufferSize() != (nClientReadSize + nDriverWriteSize)) {
    if (m_bOwnHistory == false) {
      // this is only a problem if the history is not our own - if it is a local
      // deserialized history, we miss resize calls on the original history, so we have to
      // react here to any resizes
      vstr::warnp()
          << "[VistaSensorReadState]: Deserializing received history with different size than "
             "local history"
             " - history will be resized, but this shouldn't happen too often (once per readstate)"
          << std::endl;
    }
    m_pHistory->Resize(nClientReadSize, nDriverWriteSize, nMeasureSize);
  }

  assert(m_pHistory->GetSensorMeasureSize() == nMeasureSize);
  assert(m_pHistory->GetClientReadSize() == nClientReadSize);
  assert(m_pHistory->GetDriverWriteSize() == nDriverWriteSize);

  m_pHistory->GetWriteBuffer().SetFirst(nFirst);
  m_pHistory->GetWriteBuffer().SetNext(nNext);
  m_pHistory->GetWriteBuffer().SetCurrent(nCurrent);

  if (m_bUseIncrementalSerialization) {
    VistaType::uint32 nNewMeasures;
    nRead += oDeSerializer.ReadInt32(nNewMeasures);

    assert(nNewMeasures == std::min(m_nCurrentMeasureCount - m_nLastDeSerializeMeasureCount,
                               (VistaType::uint32)m_pHistory->GetWriteBuffer().GetBufferSize()));

    VistaMeasureHistory::Buffer::iterator itMeasure =
        m_pHistory->GetWriteBuffer().index(m_nSnapshotReadHead);
    for (VistaType::uint32 i = 0; i < nNewMeasures; ++i) {
      nRead += DeSerializeMeasure(oDeSerializer, (*itMeasure));
      --itMeasure;
    }
    m_nLastDeSerializeMeasureCount = m_nCurrentMeasureCount;
  } else {
    // read off the single measures index by index
    for (unsigned int n = 0; n < m_pHistory->GetWriteBuffer().GetBufferSize(); ++n) {
      nRead += DeSerializeMeasure(oDeSerializer, (*m_pHistory->GetWriteBuffer().index(n)));
    }
  }
  return nRead;
}

std::string VistaSensorReadState::GetSignature() const {
  return "VistaSensorReadState";
}

const VistaMeasureHistory* VistaSensorReadState::GetHistory() const {
  return m_pHistory;
}

bool VistaSensorReadState::GetUseIncrementalSerialization() const {
  return m_bUseIncrementalSerialization;
}

void VistaSensorReadState::SetUseIncrementalSerialization(const bool bSet) {
  if (m_bUseIncrementalSerialization == bSet)
    return;
  m_bUseIncrementalSerialization = bSet;
  m_nLastDeSerializeMeasureCount = 0;
  m_mapLastSerializeMeasureCounts.clear();
}

bool VistaSensorReadState::GetUseOwnHistoryForDeSerialization() const {
  return m_bUseOwnHistoryForDeSerialization;
}

void VistaSensorReadState::SetUseOwnHistoryForDeSerialization(const bool bSet) {
  m_bUseOwnHistoryForDeSerialization = bSet;
  if (bSet == false && m_bOwnHistory) {
    delete m_pHistory;
    m_bOwnHistory                  = false;
    m_nLastDeSerializeMeasureCount = 0;
    m_mapLastSerializeMeasureCounts.clear();
    m_pHistory = &m_pSensor->GetDriverMeasures();
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
