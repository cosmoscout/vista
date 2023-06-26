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

#include "VistaClusterMaster.h"

#include <VistaKernel/Cluster/Utils/VistaMasterDataTunnel.h>
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <VistaKernel/EventManager/VistaExternalMsgEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterBarrier.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataCollect.h>
#include <VistaInterProcComm/Cluster/Imps/VistaTCPIPClusterDataSync.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaSocketAddress.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaUtilityMacros.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaSerializingToolset.h>

#include <algorithm>
#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * representing a slave unit as entity of the master.
 * Each slave has a name, an ip and so on. Individually, a slave can sync or not.
 * The slave instance keeps book of connections and other state variables.
 */
class Slave {
 public:
  Slave(const std::string& sSlaveName, const int nIndex, const std::string& sIP, int nPort,
      bool bSwap, bool bDoSync)
      : m_sName(sSlaveName)
      , m_sIP(sIP)
      , m_nPort(nPort)
      , m_pCon(NULL)
      , m_bSwap(bSwap)
      , m_bSwapSyncs(bDoSync)
      , m_pAckCon(NULL)
      , m_nIndex(nIndex) {
  }

  Slave(const std::string& sSlaveName, const int nIndex, VistaConnectionIP* pCon,
      VistaConnectionIP* pAckCon, bool bDoSync)
      : m_sIP(pCon->GetPeerName())
      , m_nPort(pCon->GetPeerPort())
      , m_pCon(pCon)
      , m_sName(sSlaveName)
      , m_bSwap(pCon->GetByteorderSwapFlag() == VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                    ? true
                    : false)
      , m_bSwapSyncs(bDoSync)
      , m_pAckCon(pAckCon)
      , m_nIndex(nIndex) {
  }

  ~Slave() {
    // if( m_pAckCon && m_pAckCon->GetIsOpen() )
    //{
    //	m_pAckCon->SetIsBlocking(true);
    //	m_pAckCon->WaitForSendFinish();
    //	m_pAckCon->Close();
    //}
    delete m_pAckCon;

    // if( m_pCon && m_pCon->GetIsOpen() )
    //{
    //	m_pCon->SetIsBlocking(true);
    //	m_pCon->WaitForSendFinish();
    //	m_pCon->Close();
    //}
    delete m_pCon;
  }

  std::string        m_sName;
  std::string        m_sIP;
  int                m_nPort;
  bool               m_bSwap;
  bool               m_bSwapSyncs;
  VistaConnectionIP* m_pCon;
  VistaConnectionIP* m_pAckCon;
  int                m_nIndex;
};

/**
 * helper class to hold a buffer for a serialized event. As the events are sent
 * using the asynchronous ip socket calls, the buffer to send has to remain
 * valid until finally sent. This class also defines a serialization routine
 * CreateBufferFromEvent() that is utilized by the cluster master below.
 * Note that this implicitly defines the header and part of the protocol.
 */
class EventBuffer {
 public:
  /**
   * @param decide about byte-swapping upon serialization of an event
   */
  EventBuffer(bool bDoSwap)
      : m_pSerializer(new VistaByteBufferSerializer(1024))
      , m_nEventCount(0)
      , m_nCounter(0) {
    m_pSerializer->SetByteorderSwapFlag(
        bDoSwap ? VistaSerializingToolset::SWAPS_MULTIBYTE_VALUES
                : VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
  }

  ~EventBuffer() {
    delete m_pSerializer;
  }

  /**
   * this method effectively defines the header for each event,
   * thus some kind of protocol to follow.
   * @param the event to serialize
   */
  void CreateBufferFromEvent(const VistaEvent& oEvent) {
    // reset write head
    (*m_pSerializer).ClearBuffer();

    // first we write a dummy header here
    // this will be later filled by the cluster master upon
    // the send of the packet to the slaves
    int  nDummy = 0;
    bool bDummy = false;
    (*m_pSerializer).WriteInt32(nDummy); // value will be re-written
    (*m_pSerializer).WriteBool(bDummy);  // value will be re-written
    (*m_pSerializer).WriteInt32(nDummy); // value will be re-written
    (*m_pSerializer).WriteInt32(nDummy); // value will be re-written

    // now the event 'header'
    (*m_pSerializer).WriteInt32(m_nEventCount++);
    (*m_pSerializer).WriteInt32(m_nCounter++);
    (*m_pSerializer).WriteDouble(oEvent.GetTime());
    (*m_pSerializer).WriteInt32(oEvent.GetType());
    (*m_pSerializer).WriteInt32(oEvent.GetId());

    // now the event itself
    (*m_pSerializer).WriteSerializable(oEvent);
  }

  /**
   * get a pointer to the first address of the serialized event buffer.
   */
  VistaType::byte* GetBuffer() const {
    return const_cast<VistaType::byte*>((*m_pSerializer).GetBuffer());
  }

  /**
   * returns the number of bytes stored in this buffer
   */
  unsigned int GetBufferSize() const {
    return (*m_pSerializer).GetBufferSize();
  }

  // public API, use with care.

  VistaByteBufferSerializer* m_pSerializer; /**< can grow and shrink, the constructor
                                           decides to set it to 32k, which should
                                           be fine for most events, even devices
                                           with a large history. */
  VistaType::sint32 m_nEventCount;          /**< some statistics, the number of events sent
                                                                               counted for each (type/id) */
  VistaType::sint32 m_nCounter;             /**< @todo check whether this is needed */
};

/**
 * some helper routines that are not really bound to a class
 */
namespace {
/**
 * needed for call to for_each() functors, plainly calles delete on the pointer.
 * @param pBuffer a pointer to the buffer to delete
 */
void DeleteBuffer(EventBuffer* pBuffer) {
  delete pBuffer;
}

/**
 * creates a number of buffers and stores them in the vecBuffer resulting vector
 */
void SetupBuffers(std::vector<EventBuffer*>& vecBuffer, int nNum, bool bDoSwap) {
  for (int n = 0; n < nNum; ++n) {
    vecBuffer.push_back(new EventBuffer(bDoSwap));
  }
}

VistaConnectionIP* CreateConnectionToSlave(Slave* pSlave) {
  try {
    int nPort = -1;
    if (pSlave->m_pAckCon->ReadInt32(nPort) != sizeof(VistaType::sint32)) {
      vstr::errp() << "[VistaClusterMaster]:"
                   << "Could not create conection to slave [" << pSlave->m_sName
                   << "] - no port received" << std::endl;
      return NULL;
    }
    if (nPort == -1) {
      vstr::warnp() << "[VistaClusterMaster]:"
                    << "Could not create conection to slave [" << pSlave->m_sName
                    << "] - no free port left on slave" << std::endl;
      return NULL;
    } else if (nPort < 0) {
      vstr::warnp() << "[VistaClusterMaster]:"
                    << "Could not create conection to slave [" << pSlave->m_sName << "]"
                    << std::endl;
      return NULL;
    }
    VistaConnectionIP* pConnection =
        new VistaConnectionIP(VistaConnectionIP::CT_TCP, pSlave->m_sIP, nPort);
    if (pConnection->GetIsConnected() == false && pConnection->Connect() == false) {
      vstr::errp() << "[VistaClusterMaster]:"
                   << "Could not create connection to slave [" << pSlave->m_sName << "] at Host ["
                   << pSlave->m_sIP << "] - Port [" << nPort << "]" << std::endl;
      return NULL;
    }
    pConnection->SetIsBlocking(true);
    pConnection->SetIsBuffering(false);
    pConnection->SetByteorderSwapFlag(VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
    return pConnection;
  } catch (VistaExceptionBase& e) {
    vstr::errp() << "VistaClusterMaster::CreateConnectionToSlave() -- "
                 << "Failed with exception\n"
                 << e.GetPrintStatement() << std::endl;
  }
  return NULL;
}

} // namespace

/**
 * does the bulk load of the network communication in the master/slave scheme.
 * it collects events from the event bus and decides to forward the to all
 * slaves attached. It creates and maintains a number of buffers for each possible
 * event type that can be transmitted.
 */
class VistaClusterMaster::EventObserver : public VistaEventObserver {
 public:
  EventObserver()
      : VistaEventObserver()
      , m_nFrameIndex(0)
      , m_nPacketIndex(0)
      , m_oSerializer(0)
      , m_pAvgSendTimer(new VistaWeightedAverageTimer) {
    SetupBuffers(
        m_vecSystemBuffers, VistaSystemEvent::VSE_UPPER_BOUND - VistaSystemEvent::VSE_FIRST, false);
    SetupBuffers(m_vecInteractionBuffers,
        VistaInteractionEvent::VEID_LAST - VistaInteractionEvent::VEID_FIRST, false);
    SetupBuffers(m_vecExternalBuffers,
        VistaExternalMsgEvent::VEID_LAST - VistaExternalMsgEvent::VEID_FIRST, false);
  }

  ~EventObserver() {
    delete m_pAvgSendTimer;

    if (m_vecSwapSyncSockets.empty() == false) {
      SyncBC();
      for (std::vector<VistaUDPSocket*>::iterator itSocket = m_vecSwapSyncSockets.begin();
           itSocket != m_vecSwapSyncSockets.end(); ++itSocket) {
        if ((*itSocket) == NULL)
          continue;
        (*itSocket)->CloseSocket();
        delete (*itSocket);
      }
    }

    for (std::vector<Slave*>::iterator itSlave = m_vecSlaves.begin(); itSlave != m_vecSlaves.end();
         ++itSlave) {
      delete (*itSlave);
    }

    std::for_each(m_vecSystemBuffers.begin(), m_vecSystemBuffers.end(), DeleteBuffer);
    std::for_each(m_vecExternalBuffers.begin(), m_vecExternalBuffers.end(), DeleteBuffer);
    std::for_each(m_vecInteractionBuffers.begin(), m_vecInteractionBuffers.end(), DeleteBuffer);
  }

  /**
   * event notification. the cluster master intercepts:
   * - INIT: this is NOT transferred to the slaves, instead,
       every slave utters and dispatches its own init event.
   * - EXIT: sets a special flag which causes event dispatching on
       the slaves to end.
   * - VSE_PREGRAPHICS: shuffles the send list, in order to "trick"
       TCP congestion control a bit or other possible switch optimization.
       The effect of this is not really known...
   * - interaction/GRAPH_UPDATE: is the only interaction event forward to
       slaves, contains the current DFN graph (in the event) and causes
       event dispatching and graph evaluation on slave nodes.
   * - EXTERNAL_MESSAGE: one way to transmit data from master to slave in
       frame. Is, for example used  in conjunction with external applications
       that share simulation results.
   * all events are collected (serialized) and immediately sent (asynchronously,
   * so serialization should be the only noticeable effect on latency for each event)
   */
  virtual void Notify(const VistaEvent* pEvent) {
    EventBuffer* pBuffer     = NULL;
    bool         bEndOfFrame = false;
    if (pEvent->GetType() == VistaSystemEvent::GetTypeId()) {
      pBuffer = m_vecSystemBuffers[pEvent->GetId()];
      switch (pEvent->GetId()) {
      case VistaSystemEvent::VSE_INIT: {
        return;
      }
      case VistaSystemEvent::VSE_PREGRAPHICS: {
        std::random_shuffle(m_vecSendList.begin(), m_vecSendList.end());
        bEndOfFrame = true;
        break;
      }
      case VistaSystemEvent::VSE_EXIT: {
        bEndOfFrame = true;
        break;
      }
      default:
        break;
      }
    } else if (pEvent->GetType() == VistaInteractionEvent::GetTypeId()) {
      if (pEvent->GetId() == VistaInteractionEvent::VEID_CONTEXT_GRAPH_UPDATE)
        pBuffer = m_vecInteractionBuffers[pEvent->GetId()];
    } else if (pEvent->GetType() == VistaExternalMsgEvent::GetTypeId()) {
      pBuffer = m_vecExternalBuffers[pEvent->GetId()];
    }

    if (pBuffer) {
      VistaKernelProfileScope("CLUSTER_DIST_EVENT");
      m_pAvgSendTimer->StartRecording();

      pBuffer->CreateBufferFromEvent(*pEvent);
      BroadcastEvent(pBuffer, bEndOfFrame);

      m_pAvgSendTimer->RecordTime();
    }
  }

  /**
   * sends sync signal on the UDP sync socket. This is a broadcast to
   * all waiting slaves. The message sent is a counter, which can be
   * used by the slaves to see whether they are still in sync with the
   * master node.
   */
  void SyncBC() {
    for (std::vector<VistaUDPSocket*>::iterator itSocket = m_vecSwapSyncSockets.begin();
         itSocket != m_vecSwapSyncSockets.end(); ++itSocket) {
      if ((*itSocket) == NULL)
        continue;
      try {
        // frame was successfully dispatched by the master to all slaves.
        (*itSocket)->SendRaw(&m_nFrameIndex, sizeof(int));
      } catch (VistaExceptionBase&) {
        vstr::warnp() << "VistaClusterMaster::SwapSync() -- "
                      << "Sending Sync signal failed - disabling syncing" << std::endl;
        delete (*itSocket);
        (*itSocket) = NULL;
      }
    }
  }

  /**
   * method to deliver the current event to all waiting slaves.
   * Currently implemented TCP based 1:n communication.
   * @param pBuffer the buffer to send
   * @param bEndOfFrame marks the current buffer with the end of frame
            token, to simplify the state processing on the slave and
            to enable visual update (e.g., leaving the receive loop)
   */
  void BroadcastEvent(EventBuffer* pBuffer, bool bEndOfFrame) {
    bool bRefreshSerializer = true; // on first run, we have to refresh

    // take note of every slave that is broken
    std::list<Slave*> liRemoveList;
    for (std::vector<Slave*>::const_iterator itSlave = m_vecSendList.begin();
         itSlave != m_vecSendList.end(); ++itSlave) {
      VistaConnectionIP* pCon = (*itSlave)->m_pCon;
      assert(pCon != NULL);

      // check if we need to rebuild the Serializer - this is necessary if
      // the current client has a different ByteorderSwapFlag than the last
      // TODO: since the slaves swap, we don't have to - verify this
      // if( m_oSerializer.GetByteorderSwapFlag() != pCon->GetByteorderSwapFlag() )
      //	bRefreshSerializer = true;

      if (bRefreshSerializer) {
        // first: respect the client's which to have a byte-swap
        // (can be different for each slave)
        m_oSerializer.SetByteorderSwapFlag(pCon->GetByteorderSwapFlag());

        // this call 'adopts' the buffer and positions the write head
        // at index 0
        m_oSerializer.SetBuffer((*pBuffer).GetBuffer(), (*pBuffer).GetBufferSize(), 0);

        // this effectively overwrites the first int32
        // which was written with a dummy value above
        m_oSerializer.WriteInt32((*pBuffer).GetBufferSize());

        // same for the bEndOfFrame flag
        m_oSerializer.WriteBool(bEndOfFrame);

        // now the current packet index.
        m_oSerializer.WriteInt32(m_nPacketIndex);

        // finally the frame index
        m_oSerializer.WriteInt32(m_nFrameIndex);
      }

      try {
        // test whether the send buffer of the real connection
        // is free to go
        unsigned long nL = pCon->WaitForSendFinish();
        if (nL == 0) {
          vstr::errp() << "[ViClMaster]: error during WaitForSendFinish() [" << (*itSlave)->m_sName
                       << "]\n"
                       << vstr::indent << vstr::singleindent << "removing it from send-list"
                       << std::endl;
          liRemoveList.push_back(*itSlave);
          continue;
        }

        // write the complete buffer in a single step
        // note that the write starts at the (*pBuffer).GetBuffer()
        // and writes the complete buffer, including the header
        int nRet = pCon->WriteRawBuffer((*pBuffer).GetBuffer(), (*pBuffer).GetBufferSize());

        if (nRet != (int)(*pBuffer).GetBufferSize()) {
          vstr::errp()
              << "VistaClusterMater::BroadcastEvent() -- error during WriteRawBuffer() for slave ["
              << (*itSlave)->m_sName << "] - removing it from send-list" << std::endl;
          liRemoveList.push_back(*itSlave);
        }
        ++m_nPacketIndex;

      } catch (VistaExceptionBase& x) {
        vstr::errp() << "[ViClMaster]: error during send to slave [" << (*itSlave)->m_sName
                     << "] - removing it from send-list\n"
                     << x.GetPrintStatement() << std::endl;
        liRemoveList.push_back(*itSlave);
      }
    }

    for (std::list<Slave*>::iterator itRemSlave = liRemoveList.begin();
         itRemSlave != liRemoveList.end(); ++itRemSlave) {
      DeactivateSlave((*itRemSlave));
    }
  }

  void BroadcastTime(VistaType::systemtime dTime) {
    bool bRefreshSerializer = true; // on first run, we have to refresh

    // take note of every slave that is broken
    std::list<Slave*> liRemoveList;
    for (std::vector<Slave*>::const_iterator itSlave = m_vecSendList.begin();
         itSlave != m_vecSendList.end(); ++itSlave) {
      VistaConnectionIP* pCon = (*itSlave)->m_pCon;
      assert(pCon != NULL);

      // check if we need to rebuild the Serializer - this is necessary if
      // the current client has a different ByteorderSwapFlag than the last
      if (m_oSerializer.GetByteorderSwapFlag() != pCon->GetByteorderSwapFlag())
        bRefreshSerializer = true;

      if (bRefreshSerializer) {
        m_oSerializer.SetByteorderSwapFlag(pCon->GetByteorderSwapFlag());

        m_oSerializer.ClearBuffer();

        m_oSerializer.WriteInt32(m_nFrameIndex);
        m_oSerializer.WriteDouble(dTime);
      }

      try {
        // test whether the send buffer of the real connection
        // is free to go
        unsigned long nL = pCon->WaitForSendFinish();
        if (nL == 0) {
          vstr::errp() << "[ViClMaster]: error during WaitForSendFinish() [" << (*itSlave)->m_sName
                       << "] - removing it from send-list" << std::endl;
          liRemoveList.push_back(*itSlave);
        }

        // write the complete buffer in a single step
        // note that the write starts at the (*pBuffer).GetBuffer()
        // and writes the complete buffer, including the header
        int nRet = pCon->WriteRawBuffer(m_oSerializer.GetBuffer(), m_oSerializer.GetBufferSize());

        if (nRet != m_oSerializer.GetBufferSize()) {
          vstr::errp() << "[ViClMaster]: error during WriteRawBuffer() for slave ["
                       << (*itSlave)->m_sName << "] - removing it from send-list" << std::endl;
          liRemoveList.push_back(*itSlave);
        }

      } catch (VistaExceptionBase& x) {
        vstr::errp() << "[ViClMaster]: error during send to client [" << (*itSlave)->m_sName << "]."
                     << std::endl
                     << "[ViClMaster]: removing it from send-list\n"
                     << x.GetExceptionText() << std::endl;
        liRemoveList.push_back(*itSlave);
      }
    }
  }

  /**
   * create a connection to the ip/port given and transmits the first
   * frame clock as a handshake. It returns two things
   * - the connection describing the event channel
   * - the connection describing the ack channel
   * the other side decides about the port number of the ack channel, this
   * is returned after receiving the handshake frame clock
   * @return true when the connection was established and evChannel and ackChannel
             are both valid
   */
  bool EstablishConnection(Slave* pSlave, double dFrameClock) {
    VistaConnectionIP* pAckChannel   = NULL;
    VistaConnectionIP* pEventChannel = NULL;
    try {
      pEventChannel =
          new VistaConnectionIP(VistaConnectionIP::CT_TCP, pSlave->m_sIP, pSlave->m_nPort);

      // pEventChannel->SetByteorderSwapFlag( pSlave->m_bSwap );
      // since the slaves know if they have to swap, we never do so
      pEventChannel->SetByteorderSwapFlag(VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
      if (pEventChannel->GetIsOpen()) {
        // connection established.
        // write initial frame clock
        vstr::outi() << "Event channel established at IP [" << pSlave->m_sIP << " - Port ["
                     << pSlave->m_nPort << "]" << std::endl;

        pEventChannel->WriteDouble(dFrameClock);
        pEventChannel->WriteInt32((VistaType::sint32)pSlave->m_nIndex);
        int nAckPort = 0;
        VISTA_VERIFY(pEventChannel->ReadInt32(nAckPort), sizeof(VistaType::sint32));

        pEventChannel->SetIsBlocking(false);
        pEventChannel->SetIsBuffering(false);

        vstr::outi() << "Slave transmitted ack port [" << nAckPort << "]" << std::endl;

        pAckChannel = new VistaConnectionIP(VistaConnectionIP::CT_TCP, pSlave->m_sIP, nAckPort);
        if (pAckChannel->GetIsOpen()) {
          pAckChannel->SetIsBlocking(true);
          pAckChannel->SetIsBuffering(false);
          pAckChannel->SetByteorderSwapFlag(
              VistaSerializingToolset::DOES_NOT_SWAP_MULTIBYTE_VALUES);
          vstr::outi() << "Ack channel established at IP [" << pSlave->m_sIP << " - Port ["
                       << nAckPort << "]" << std::endl;
        } else {
          // connection failed.
          vstr::warnp() << "Failed to init ack channel at IP [" << pSlave->m_sIP << " - Port ["
                        << nAckPort << "]" << std::endl;

          delete pEventChannel;

          delete pAckChannel;
          return false;
        }

      } else {
        // connection failed.
        vstr::errp() << " Failed to init event channel at IP [" << pSlave->m_sIP << " - Port ["
                     << pSlave->m_nPort << "]" << std::endl;

        delete pEventChannel;
        return false;
      }
      pSlave->m_pCon    = pEventChannel;
      pSlave->m_pAckCon = pAckChannel;
      return true;
    } catch (VistaExceptionBase& e) {
      delete pEventChannel;
      delete pAckChannel;
      vstr::errp() << " Failed to init event channel at IP [" << pSlave->m_sIP << " - Port ["
                   << pSlave->m_nPort << "] with exception\n"
                   << e.GetPrintStatement() << std::endl;
    }
    return false;
  }

  void TransmitClusterSetupInfo(const std::string& m_sMasterName) {
    vstr::outi() << "Publishing cluster setup info to slaves" << std::endl;

    bool bRefreshSerializer = true; // on first run, we have to refresh

    // take note of every slave that is broken
    std::list<Slave*> liRemoveList;
    for (std::vector<Slave*>::const_iterator itSlave = m_vecSendList.begin();
         itSlave != m_vecSendList.end(); ++itSlave) {
      VistaConnectionIP* pCon = (*itSlave)->m_pCon;
      assert(pCon != NULL);

      if (m_oSerializer.GetByteorderSwapFlag() != pCon->GetByteorderSwapFlag())
        bRefreshSerializer = true;

      if (bRefreshSerializer) {
        m_oSerializer.SetByteorderSwapFlag(pCon->GetByteorderSwapFlag());

        m_oSerializer.ClearBuffer();

        VistaType::sint32 nMessageSize = 0;

        nMessageSize += m_oSerializer.WriteInt32(0); // Dummy

        nMessageSize += m_oSerializer.WriteInt32((VistaType::sint32)m_sMasterName.size());
        nMessageSize += m_oSerializer.WriteString(m_sMasterName);
        nMessageSize += m_oSerializer.WriteInt32((VistaType::sint32)m_vecSlaves.size());
        for (std::vector<Slave*>::iterator itSlaveData = m_vecSlaves.begin();
             itSlaveData != m_vecSlaves.end(); ++itSlaveData) {
          nMessageSize +=
              m_oSerializer.WriteInt32((VistaType::sint32)(*itSlaveData)->m_sName.size());
          nMessageSize += m_oSerializer.WriteString((*itSlaveData)->m_sName);
          nMessageSize += m_oSerializer.WriteBool(((*itSlaveData)->m_pCon != NULL));
        }
        // overwrite dummy written before
        // nasty, find a better way
        VistaType::sint32 nWriteSize = nMessageSize;
        if (m_oSerializer.GetByteorderSwapFlag())
          VistaSerializingToolset::Swap4(&nWriteSize);
        memcpy(const_cast<VistaType::byte*>(m_oSerializer.GetBuffer()), &nWriteSize,
            sizeof(VistaType::sint32));
      }

      try {
        // test whether the send buffer of the real connection
        // is free to go
        unsigned long nL = pCon->WaitForSendFinish();
        if (nL == 0) {
          vstr::errp() << "[ViClMaster]: error during WaitForSendFinish() [" << (*itSlave)->m_sName
                       << "] - removing it from send-list" << std::endl;
          liRemoveList.push_back(*itSlave);
        }

        // write the complete buffer in a single step
        // note that the write starts at the (*pBuffer).GetBuffer()
        // and writes the complete buffer, including the header
        int nRet = pCon->WriteRawBuffer(m_oSerializer.GetBuffer(), m_oSerializer.GetBufferSize());

        if (nRet != m_oSerializer.GetBufferSize()) {
          vstr::errp() << "[ViClMaster]: error during WriteRawBuffer() for slave ["
                       << (*itSlave)->m_sName << "] - removing it from send-list" << std::endl;
          liRemoveList.push_back(*itSlave);
        }

      } catch (VistaExceptionBase& x) {
        vstr::errp() << "[ViClMaster]: error during send to client [" << (*itSlave)->m_sName << "]."
                     << std::endl
                     << "[ViClMaster]: removing it from send-list\n"
                     << x.GetExceptionText() << std::endl;
        liRemoveList.push_back(*itSlave);
      }
    }

    for (std::list<Slave*>::iterator itRemSlave = liRemoveList.begin();
         itRemSlave != liRemoveList.end(); ++itRemSlave) {
      DeactivateSlave((*itRemSlave));
    }
  }

  /**
   * check, whether a slave named strName exists in the list of registered slaves.
   * @param strName case-sensitive name to check
   * @return false no slave with name strName exists
   */
  bool GetIsSlave(const std::string& strName) const {
    for (std::vector<Slave*>::const_iterator it = m_vecSlaves.begin(); it != m_vecSlaves.end();
         ++it) {
      if (strName == (*it)->m_sName)
        return true;
    }
    return false;
  }

  void RegisterSlave(Slave* pSlave, bool bAlive) {
    m_vecSlaves.push_back(pSlave);
    if (bAlive) {
      m_vecSendList.push_back(pSlave);
      m_vecAckList.push_back(pSlave);
    }
  }

  /**
   * DeactivateSlave removes a slave from the Ack and Send list, but
   * keeps it in the Slaves list
   */
  bool DeactivateSlave(Slave* pSlave) {
    std::vector<Slave*>::iterator itSlave =
        std::find(m_vecSendList.begin(), m_vecSendList.end(), pSlave);
    if (itSlave != m_vecSendList.end())
      m_vecSendList.erase(itSlave);

    itSlave = std::find(m_vecAckList.begin(), m_vecAckList.end(), pSlave);
    if (itSlave != m_vecAckList.end())
      m_vecAckList.erase(itSlave);

    if (pSlave->m_pCon) {
      try {
        pSlave->m_pCon->WaitForSendFinish(0);
        pSlave->m_pCon->Close();
      } catch (VistaExceptionBase&) {
        // its okay
      }
      delete pSlave->m_pCon;
      pSlave->m_pCon = NULL;
    }

    if (pSlave->m_pAckCon) {
      try {
        pSlave->m_pAckCon->WaitForSendFinish(0);
        pSlave->m_pAckCon->Close();
      } catch (VistaExceptionBase&) {
        // its okay
      }
      delete pSlave->m_pAckCon;
      pSlave->m_pAckCon = NULL;
    }
    return true;
  }

  // public API for master access
  std::vector<Slave*> m_vecSlaves;   /**< all declared slaves */
  std::vector<Slave*> m_vecSendList; /**< all active slaves */
  std::vector<Slave*> m_vecAckList;  /**< all slaves that are required to give an ack */

  unsigned int m_nFrameIndex; /**< the current frame index */
  unsigned int m_nPacketIndex;

  std::vector<VistaUDPSocket*> m_vecSwapSyncSockets; /**< UPD broadcast sockets for back-linking */

  std::vector<EventBuffer*>  m_vecSystemBuffers;      /**< store system events */
  std::vector<EventBuffer*>  m_vecInteractionBuffers; /**< store interaction events */
  std::vector<EventBuffer*>  m_vecExternalBuffers;    /**< store external msg events */
  VistaByteBufferSerializer  m_oSerializer; /**< the serializer to (re-)use during dispatching */
  VistaWeightedAverageTimer* m_pAvgSendTimer;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaClusterMaster::VistaClusterMaster(
    VistaEventManager* pEventManager, const std::string& sMasterName)
    : m_pEventManager(pEventManager)
    , m_sMasterName(sMasterName)
    , m_sMasterSectionName(sMasterName)
    , m_pEventObserver(NULL)
    , m_pAvgBc(new VistaWeightedAverageTimer)
    , m_pDefaultBarrier(NULL)
    , m_pDefaultDataSync(NULL) {
  m_pEventObserver = new EventObserver;
  m_pEventManager->RegisterObserver(m_pEventObserver, VistaSystemEvent::GetTypeId());
  m_pEventManager->RegisterObserver(m_pEventObserver, VistaInteractionEvent::GetTypeId());
  m_pEventManager->RegisterObserver(m_pEventObserver, VistaExternalMsgEvent::GetTypeId());
  vstr::warnp() << "Using outdated VistaClusterMaster -- please switch to the new "
                   "VistaNewClusterMaster instead, "
                << " the VistaClusterMaster will be removed in future releases" << std::endl;
}

VistaClusterMaster::~VistaClusterMaster() {
  m_pEventManager->UnregisterObserver(m_pEventObserver, VistaSystemEvent::GetTypeId());
  m_pEventManager->UnregisterObserver(m_pEventObserver, VistaInteractionEvent::GetTypeId());
  m_pEventManager->UnregisterObserver(m_pEventObserver, VistaExternalMsgEvent::GetTypeId());

  delete m_pEventObserver;
  delete m_pAvgBc;
  VistaTimeUtils::Sleep(1000); // give a few secs to flush the tcp sockets
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaClusterMaster::GetClusterMode() const {
  return CM_MASTER_SLAVE;
}
std::string VistaClusterMaster::GetClusterModeName() const {
  return "MASTER_SLAVE";
}

int VistaClusterMaster::GetNodeType() const {
  return NT_MASTER;
}
std::string VistaClusterMaster::GetNodeTypeName() const {
  return "MASTER";
}
std::string VistaClusterMaster::GetNodeName() const {
  return m_sMasterName;
}
std::string VistaClusterMaster::GetConfigSectionName() const {
  return m_sMasterSectionName;
}

int VistaClusterMaster::GetNodeID() const {
  // we are always ID 0, slaves start at 1
  return 0;
}

bool VistaClusterMaster::GetNodeInfo(const int iNodeID, NodeInfo& oInfo) const {
  if (iNodeID == 0) {
    oInfo.m_sNodeName = GetNodeName();
    oInfo.m_bIsActive = true;
    oInfo.m_iNodeID   = 0;
    oInfo.m_eNodeType = NT_STANDALONE;
    return true;
  } else if (iNodeID > 0 && iNodeID <= (int)m_pEventObserver->m_vecSlaves.size()) {
    Slave* pSlave     = m_pEventObserver->m_vecSlaves[iNodeID - 1];
    oInfo.m_sNodeName = pSlave->m_sName;
    oInfo.m_bIsActive = (pSlave->m_pCon != NULL);
    oInfo.m_iNodeID   = iNodeID;
    oInfo.m_eNodeType = NT_SLAVE;
  }
  return false;
}

bool VistaClusterMaster::GetIsLeader() const {
  return true;
}
bool VistaClusterMaster::GetIsFollower() const {
  return false;
}

int VistaClusterMaster::GetNumberOfNodes() const {
  return ((int)m_pEventObserver->m_vecSlaves.size() + 1);
}

std::string VistaClusterMaster::GetNodeName(const int iNodeID) const {
  if (iNodeID == 0)
    return GetNodeName();
  else
    return m_pEventObserver->m_vecSlaves[iNodeID - 1]->m_sName;
}

bool VistaClusterMaster::StartFrame() {
  ++m_iFrameCount;
  m_pEventObserver->m_nFrameIndex = m_iFrameCount;
  m_dFrameClock                   = VistaTimeUtils::GetStandardTimer().GetSystemTime();
  m_pEventObserver->BroadcastTime(m_dFrameClock);
  return true;
}

bool VistaClusterMaster::ProcessFrame() {
  return true;
}

bool VistaClusterMaster::EndFrame() {
  return true;
}

/**
 * collect all acks from all active slaves, mark broken slaves as broken and remove
 * and call broadcast back-link sync after the collection of all acks.
 * @return currently true, always.
 */
void VistaClusterMaster::SwapSync() {
  VistaKernelProfileScope("MASTER_SWAPSYNC");
  m_pAvgBc->StartRecording();

  // wait till all clients reach this point
  /** @todo think about moving this to the event observer as well... */
  std::list<Slave*> liRemoveList;

  for (std::vector<Slave*>::iterator itSlave = m_pEventObserver->m_vecAckList.begin();
       itSlave != m_pEventObserver->m_vecAckList.end(); ++itSlave) {
    try {
      // get the current frame index from each slave
      int nReadToken = 0;
      int nRet       = (*itSlave)->m_pAckCon->ReadInt32(nReadToken);
      if (nRet == -1 || nRet == 0) // both cases are errors from the client state
      {
        vstr::errp() << "[ViClMaster]: Slave [" << (*itSlave)->m_sName << "] failed during ack"
                     << std::endl;
        liRemoveList.push_back(*itSlave);
      } else if (nReadToken !=
                 (int)m_pEventObserver->m_nFrameIndex) // see whether frame index matches
      {
        vstr::errp() << "[ViClMaster]: slave [" << (*itSlave)->m_sName << "] failed to ack correct"
                     << " frame token" << std::endl;
        vstr::errp() << "[ViClMaster]: expected [" << m_pEventObserver->m_nFrameIndex
                     << "] but was [" << nReadToken << "], socket: [" << nRet << "]" << std::endl;
        liRemoveList.push_back(*itSlave);
      }
    } catch (VistaExceptionBase& x) {
      x.PrintException();
      liRemoveList.push_back(*itSlave);
    }
  }

  // sync using broadcast
  m_pEventObserver->SyncBC();

  m_pAvgBc->RecordTime();
}

bool VistaClusterMaster::CreateConnections(std::vector<VistaConnectionIP*>& vecConnections) {
  for (std::vector<Slave*>::iterator itSlave = m_pEventObserver->m_vecSendList.begin();
       itSlave != m_pEventObserver->m_vecSendList.end(); ++itSlave) {
    VistaConnectionIP* pConnection = CreateConnectionToSlave((*itSlave));
    if (pConnection == NULL) {
      vstr::errp() << "VistaClusterMaster::CreateIPConnections() -- "
                   << "Could not establish connection to [" << (*itSlave)->m_sName << "]"
                   << std::endl;
      continue;
    }
    vecConnections.push_back(pConnection);
  }
  return true;
}

bool VistaClusterMaster::CreateNamedConnections(
    std::vector<std::pair<VistaConnectionIP*, std::string>>& vecConnections) {
  for (std::vector<Slave*>::iterator itSlave = m_pEventObserver->m_vecSendList.begin();
       itSlave != m_pEventObserver->m_vecSendList.end(); ++itSlave) {
    VistaConnectionIP* pConnection = CreateConnectionToSlave((*itSlave));
    if (pConnection == NULL) {
      vstr::errp() << "VistaClusterMaster::CreateIPConnections() -- "
                   << "Could not establish connection to [" << (*itSlave)->m_sName << "]"
                   << std::endl;
      continue;
    }
    vecConnections.push_back(
        std::pair<VistaConnectionIP*, std::string>(pConnection, (*itSlave)->m_sName));
  }
  return true;
}
//
// IVistaClusterSync* VistaClusterMaster::CreateClusterSync( bool bUseExistingConnections )
//{
//	VistaMasterNetworkSync* pNetSync = new VistaMasterNetworkSync;
//
//	for( std::vector<Slave*>::iterator itSlave = m_pEventObserver->m_vecSendList.begin();
//			itSlave != m_pEventObserver->m_vecSendList.end(); ++itSlave )
//	{
//		if( bUseExistingConnections )
//		{
//			pNetSync->AddSlave( (*itSlave)->m_sName, (*itSlave)->m_pAckCon, false );
//		}
//		else
//		{
//			VistaConnectionIP* pConnection = CreateConnectionToSlave( (*itSlave) );
//			if( pConnection == NULL )
//			{
//				vstr::errp() << "VistaClusterMaster::CreateIPConnections() -- "
//						<< "Could not establish connection to ["
//						<< (*itSlave)->m_sName << "]" << std::endl;
//				continue;
//			}
//			pNetSync->AddSlave( (*itSlave)->m_sName, pConnection, true );
//		}
//	}
//
//	return pNetSync;
//}

IVistaDataTunnel* VistaClusterMaster::CreateDataTunnel(IDLVistaDataPacket* pPacketProto) {
  std::vector<VistaConnectionIP*> vecConnections;
  if (CreateConnections(vecConnections) == false || vecConnections.empty()) {
    vstr::errp() << "VistaClusterMaster::CreateDataTunnel() -- "
                 << "Could not create data tunnel" << std::endl;
    return NULL;
  }

  // create the tunnel itself
  VistaMasterDataTunnel* pNewTunnel = new VistaMasterDataTunnel(vecConnections);
  return pNewTunnel;
}

void VistaClusterMaster::Debug(std::ostream& oStream) const {
  oStream << "[VistaClusterMaster]: configured as [" << m_sMasterName << "]\n"
          << "configured from section [" << m_sMasterSectionName << "]\n"
          << "[" << m_pEventObserver->m_vecSlaves.size() << "] attached slaves" << std::endl;

  for (std::vector<Slave*>::iterator cit = m_pEventObserver->m_vecSlaves.begin();
       cit != m_pEventObserver->m_vecSlaves.end(); ++cit) {
    oStream << "[" << (*cit)->m_sName << "]\n";
    oStream << "\t" << (((*cit)->m_pCon == NULL) ? ("!DEAD!") : ("ALIVE")) << (*cit)->m_sIP << " ; "
            << (*cit)->m_nPort << " ; " << ((*cit)->m_bSwap ? "SWAP" : "NOSWAP") << " ; "
            << ((*cit)->m_pCon ? "CONNECTED" : "UNAVAILABLE") << " ; "
            << ((*cit)->m_pAckCon ? "ACKS" : "SILENT") << " ; "
            << ((*cit)->m_bSwapSyncs ? "DOES SWAPSYNC" : "NO SWAPSYNC") << std::endl;
  }
}

bool VistaClusterMaster::Init(
    const std::string& sClusterSection, const VistaPropertyList& oConfig) {
  vstr::outi() << "[VistaClusterMaster]: Initializing" << std::endl;
  {
    vstr::IndentObject oIndent;

    m_dFrameClock = VistaTimeUtils::GetStandardTimer().GetSystemTime();

    m_sMasterSectionName = sClusterSection;

    if (oConfig.HasSubList(sClusterSection) == false) {
      vstr::errp() << "Cluster Master cannot be initialized - section [" << sClusterSection
                   << "] does not exist!" << std::endl;
      return false;
    }
    const VistaPropertyList& oSection = oConfig.GetSubListConstRef(sClusterSection);

    m_sMasterName = oSection.GetValueOrDefault<std::string>("NAME", sClusterSection);
    vstr::outi() << "Setting master name to [" << m_sMasterName << "]" << std::endl;

    std::string      sSyncIP;
    int              nSyncPort = -1;
    std::vector<int> vecPorts;
    if (oSection.GetValue("SYNCIP", sSyncIP) == false) {
      vstr::warnp() << "No swap sync ip given. Sync will not work!" << std::endl;
    }
    if (oSection.GetValue("SYNCPORT", nSyncPort)) {
      vecPorts.push_back(nSyncPort);
    } else if (oSection.GetValue("SYNCPORTS", vecPorts) == false || vecPorts.empty()) {
      vstr::warnp() << "No swap ports given. Sync will not work!" << std::endl;
    }

    if (sSyncIP.empty() == false && vecPorts.empty() == false) {
      for (std::vector<int>::const_iterator itPort = vecPorts.begin(); itPort != vecPorts.end();
           ++itPort) {
        vstr::outi() << "Creating swap sync socket on IP [" << sSyncIP << "], Port [" << (*itPort)
                     << "]" << std::endl;
        VistaUDPSocket* pSyncSocket = new VistaUDPSocket;

        VistaSocketAddress oAdress(sSyncIP, (*itPort));

        if (!oAdress.GetIsValid() || ((*pSyncSocket).OpenSocket() == false)) {
          vstr::errp() << "Could not open swap sync socket. Sync will not work." << std::endl;
          delete pSyncSocket;
          pSyncSocket = NULL;
        } else {
          (*pSyncSocket).SetPermitBroadcast(1);
          (*pSyncSocket).ConnectToAddress(oAdress);
          m_pEventObserver->m_vecSwapSyncSockets.push_back(pSyncSocket);
        }
      }
    }

    std::list<std::string> liSlaveList;
    oSection.GetValue("SLAVES", liSlaveList);
    for (std::list<std::string>::const_iterator itSlave = liSlaveList.begin();
         itSlave != liSlaveList.end(); ++itSlave) {
      if (oConfig.HasSubList((*itSlave)) == false) {
        vstr::warnp() << "ClusterMaster::Init() -- Slave [" << (*itSlave)
                      << "] specified, but no section exists" << std::endl;
        continue;
      }
      const VistaPropertyList& oSlaveConfig = oConfig.GetSubListConstRef((*itSlave));

      std::string sSlaveIP;
      if (oSlaveConfig.GetValue("SLAVEIP", sSlaveIP) == false) {
        vstr::warnp() << "No IP given for slave in section [" << *itSlave << "]" << std::endl;
        continue;
      }

      int nPort;
      if (oSlaveConfig.GetValue("SLAVEPORT", nPort) == false) {
        vstr::warnp() << "No valid port for slave [" << sSlaveIP << "] from section [" << *itSlave
                      << "]" << std::endl;
        continue;
      }

      bool bDoSwap = oSlaveConfig.GetValueOrDefault<bool>("BYTESWAP", false);
      bool bDoSync = oSlaveConfig.GetValueOrDefault<bool>("SYNC", true);
      AddSlave((*itSlave), sSlaveIP, nPort, bDoSwap, bDoSync);
    }

    m_pEventObserver->TransmitClusterSetupInfo(m_sMasterName);

    // transfer info to ClusterInfo construct
    VistaClusterMode::NodeInfo oNodeInfo;
    oNodeInfo.m_iNodeID   = 0;
    oNodeInfo.m_bIsActive = true;
    oNodeInfo.m_eNodeType = NT_MASTER;
    oNodeInfo.m_sNodeName = m_sMasterName;
    m_oClusterInfo.m_vecNodeInfos.push_back(oNodeInfo);
    oNodeInfo.m_eNodeType = NT_SLAVE;
    for (std::vector<Slave*>::const_iterator itSlave = m_pEventObserver->m_vecSlaves.begin();
         itSlave != m_pEventObserver->m_vecSlaves.end(); ++itSlave) {
      oNodeInfo.m_bIsActive = ((*itSlave)->m_pCon != NULL);
      oNodeInfo.m_sNodeName = (*itSlave)->m_sName;
      ++oNodeInfo.m_iNodeID;
      m_oClusterInfo.m_vecNodeInfos.push_back(oNodeInfo);
    }
  }
  vstr::outi() << "[VistaClusterMaster]: Initialization finished" << std::endl;
  return true;
}

bool VistaClusterMaster::PostInit() {
  vstr::outi() << "\n[ViClMaster]: Main Initialitation finished - Waiting for ACK from Slaves."
               << std::endl;
  vstr::IndentObject oIndent;

  for (std::vector<Slave*>::const_iterator cit = m_pEventObserver->m_vecSlaves.begin();
       cit != m_pEventObserver->m_vecSlaves.end(); ++cit) {
    if ((*cit)->m_pCon == NULL) {
      continue;
    }

    int iAlive = 0;

    vstr::outi() << "Waiting for alive signal from slave at [" << (*cit)->m_pCon->GetPeerName()
                 << "] ..." << std::flush;

    try {
      (*cit)->m_pCon->SetIsBlocking(true);
      int iRet = (*cit)->m_pCon->ReadInt32(iAlive);
      (*cit)->m_pCon->SetIsBlocking(false);
      if (iRet == sizeof(VistaType::sint32))
        vstr::outi() << vstr::singleindent << "Slave is alive" << std::endl;
      else {
        vstr::outi() << std::endl;
        vstr::erri() << vstr::singleindent << vstr::errprefix << "No response form slave ["
                     << (*cit)->m_sName << "] - removing slave!" << std::endl;
        m_pEventObserver->DeactivateSlave((*cit));
        continue;
      }

    } catch (VistaExceptionBase& x) {
      vstr::outi() << std::endl;
      vstr::errp() << "Exception during wait for alive signal from slave [" << (*cit)->m_sName
                   << "]:\n"
                   << x.GetPrintStatement() << std::endl;
      m_pEventObserver->DeactivateSlave((*cit));
      continue;
    }
  }

  return true;
}

bool VistaClusterMaster::AddSlave(
    const std::string& sName, const std::string& sIP, int iPort, bool bDoSwap, bool bDoSync) {
  if (m_pEventObserver->GetIsSlave(sName)) {
    vstr::warnp() << "Multiple slaves with name [" << sName
                  << "] given - only first one will be used!" << std::endl;
    return false;
  }

  vstr::outi() << "Connecting to slave [" << sName << "] ..." << std::endl;
  vstr::IndentObject oIndent;
  int                nSlaveIndex = (int)m_pEventObserver->m_vecSlaves.size() + 1;
  Slave*             pSlave      = new Slave(sName, nSlaveIndex, sIP, iPort, bDoSwap, bDoSync);

  bool bSuc = m_pEventObserver->EstablishConnection(pSlave, m_dFrameClock);
  m_pEventObserver->RegisterSlave(pSlave, bSuc);
  return bSuc;
}

IVistaClusterDataSync* VistaClusterMaster::CreateDataSync() {
  VistaTCPIPClusterLeaderDataSync* pSync = new VistaTCPIPClusterLeaderDataSync;

  for (std::vector<Slave*>::iterator itSlave = m_pEventObserver->m_vecSendList.begin();
       itSlave != m_pEventObserver->m_vecSendList.end(); ++itSlave) {
    VistaConnectionIP* pConnection = CreateConnectionToSlave((*itSlave));
    if (pConnection == NULL) {
      vstr::errp() << "VistaClusterMaster::CreateIPConnections() -- "
                   << "Could not establish connection to [" << (*itSlave)->m_sName << "]"
                   << std::endl;
      continue;
    }
    pSync->AddFollower((*itSlave)->m_sName, pConnection);
  }

  return pSync;
}

IVistaClusterDataSync* VistaClusterMaster::GetDefaultDataSync() {
  if (m_pDefaultDataSync == NULL)
    m_pDefaultDataSync = CreateDataSync();
  return m_pDefaultDataSync;
}

IVistaClusterBarrier* VistaClusterMaster::CreateBarrier() {
  VistaTCPIPClusterLeaderBarrier* pBarrier = new VistaTCPIPClusterLeaderBarrier;

  for (std::vector<Slave*>::iterator itSlave = m_pEventObserver->m_vecSendList.begin();
       itSlave != m_pEventObserver->m_vecSendList.end(); ++itSlave) {
    VistaConnectionIP* pConnection = CreateConnectionToSlave((*itSlave));
    if (pConnection == NULL) {
      vstr::errp() << "VistaClusterMaster::CreateIPConnections() -- "
                   << "Could not establish connection to [" << (*itSlave)->m_sName << "]"
                   << std::endl;
      continue;
    }
    pBarrier->AddFollower((*itSlave)->m_sName, pConnection);
  }

  return pBarrier;
}

IVistaClusterBarrier* VistaClusterMaster::GetDefaultBarrier() {
  if (m_pDefaultBarrier == NULL)
    m_pDefaultBarrier = CreateBarrier();
  return m_pDefaultBarrier;
}

IVistaClusterDataCollect* VistaClusterMaster::CreateDataCollect() {
  VistaTCPIPClusterLeaderDataCollect* pCollect = new VistaTCPIPClusterLeaderDataCollect;

  for (std::vector<Slave*>::iterator itSlave = m_pEventObserver->m_vecSendList.begin();
       itSlave != m_pEventObserver->m_vecSendList.end(); ++itSlave) {
    VistaConnectionIP* pConnection = CreateConnectionToSlave((*itSlave));
    if (pConnection == NULL) {
      vstr::errp() << "VistaNewClusterMaster::CreateIPConnections() -- "
                   << "Could not establish connection to [" << (*itSlave)->m_sName << "]"
                   << std::endl;
      continue;
    }
    pCollect->AddFollower((*itSlave)->m_sName, pConnection, true);
  }

  return pCollect;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
