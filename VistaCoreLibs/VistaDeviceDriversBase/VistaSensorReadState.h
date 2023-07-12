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

#ifndef _VISTASENSORSTATE_H_
#define _VISTASENSORSTATE_H_

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"
#include <VistaAspects/VistaSerializable.h>
#include <VistaBase/VistaBaseTypes.h>
#include <cstddef>
#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaDeviceDriver;
class VistaDeviceSensor;
class VistaSensorMeasure;
class VistaMeasureHistory;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A SensorReadState allows thread-safe reading from a sensor's history with a defined
 * state.
 * Multiple ReadStates can be used per DriverSensor, and calling SetToLatestSample
 * set's a read head which is used in following reads and always provides the same
 * measure (important when clustering) and also allows serialization/deserialization
 * of the data, including the history (also important for clustering/distribution).
 * Each ReadState should be accessed from one thread only.
 */
class VISTADEVICEDRIVERSAPI VistaSensorReadState : public IVistaSerializable {
 public:
  VistaSensorReadState(VistaDeviceSensor* pSensor, bool bUseIncrementalSerialization,
      bool bUseOwnHistoryForDeSerialization = true);
  virtual ~VistaSensorReadState();

  bool                 GetIsValid() const;
  VistaType::uint32    GetNewAvailableMeasureCount(VistaType::uint32& nLastRead) const;
  VistaType::uint32    GetMeasureCount() const;
  VistaType::uint32    GetActivationCount() const;
  VistaType::microtime GetUpdateTimeStamp() const;

  const VistaSensorMeasure* GetCurrentMeasure() const;
  const VistaSensorMeasure* GetPastMeasure(int nStepBack) const;

  const VistaMeasureHistory* GetHistory() const;

  VistaDeviceSensor* GetSensor() const;

  /**
   * Set's this ReadState's head to the current history's head. Thus, from
   * now on all Measures retrieved via this ReadState will use the updated value.
   * Note: a ReadState should only be either using this function plus serialization,
   * or deserialization, but not deserialization and sample updating.
   */
  bool SetToLatestSample();

  // Serializable Implementation
  virtual int         Serialize(IVistaSerializer&) const;
  virtual int         DeSerialize(IVistaDeSerializer&);
  virtual std::string GetSignature() const;

  /**
   * Determines whether the serialization always sends the full history,
   * or uses incremental history transmission where only new samples since
   * the last (de)serialization are transmitted. While incremental transmission
   * is faster, it only works for a 1:n relation of serialized and deserialized ReadStates,
   * i.e. you can serialize a ReadState multiple times with different serializers for
   * different target readstate, but you can only deserialize a ReadState from a single
   * serializer. The incremental number of nodes to send is stored for each serializer, so
   * only use this if you use the same serializer the whole time.
   */
  bool GetUseIncrementalSerialization() const;
  void SetUseIncrementalSerialization(bool bSet);

  /**
   * When deserializing data into the sensor's original history, different ReadStates
   * on this sensor may conflict, leading to an inconsistent history state. Instead, each
   * ReadState can create a dummy history to serialize into, which is only accessible via
   * this very ReadState, ensuring consistent data.
   */
  bool GetUseOwnHistoryForDeSerialization() const;
  void SetUseOwnHistoryForDeSerialization(bool bSet);

  static VistaType::uint32 InvalidState;

 private:
  VistaType::uint32 m_nSnapshotReadHead, /**< index of the starting position for all
                                             readers that use this state for reading.
                                             Is fixed at the time of read on the read state.
                                             Initial value / invalid state: ~0 */
      m_nActivationCount,                /**< number of swaps/activations called for this state,
                                             typically used for debugging or statistics. */
      m_nCurrentMeasureCount;            /**< the number of measures available in this
                          state. */
  VistaType::microtime m_nLastUpdateTs;  /**< the timestamp records the moment in time this
                                             state was filled up last. */

  VistaDeviceSensor*   m_pSensor;
  VistaMeasureHistory* m_pHistory;
  bool m_bOwnHistory; /** this determines if we use an own history (i.e. as created when
                       * deserializing readstate, or if we use the sensor's original history) */
  bool                                     m_bUseOwnHistoryForDeSerialization;
  bool                                     m_bUseIncrementalSerialization;
  mutable std::map<IVistaSerializer*, int> m_mapLastSerializeMeasureCounts;
  mutable VistaType::uint32                m_nLastDeSerializeMeasureCount;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTASENSORSTATE_H_
