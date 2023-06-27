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

#ifndef _VISTAZEROMQCLUSTERDATASYNC_H
#define _VISTAZEROMQCLUSTERDATASYNC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaInterProcComm/Cluster/Imps/VistaClusterBytebufferDataSyncBase.h>
#include <VistaInterProcComm/Cluster/VistaClusterDataSync.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>

#include <string>
#include <vector>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
namespace zmq {
class socket_t;
class message_t;
} // namespace zmq
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaZeroMQClusterLeaderDataSync
    : public VistaClusterBytebufferLeaderDataSyncBase {
 public:
  VistaZeroMQClusterLeaderDataSync(
      const std::string& sHostname, const int nServerPort, const bool bVerbose = true);
  explicit VistaZeroMQClusterLeaderDataSync(
      const std::string& sZeroMQAddress, const bool bVerbose = true);
  virtual ~VistaZeroMQClusterLeaderDataSync();

  bool WaitForConnection(const std::vector<VistaConnectionIP*>& vecConnections);

  virtual bool GetIsValid() const;

  virtual std::string GetDataSyncType() const;

  virtual int  GetSendBlockingThreshold() const;
  virtual bool SetSendBlockingThreshold(const int nNumBytes);

 private:
  virtual bool DoSendMessage();

 private:
  zmq::socket_t* m_pSocket;
};

class VISTAKERNELAPI VistaZeroMQClusterFollowerDataSync
    : public VistaClusterBytebufferFollowerDataSyncBase {
 public:
  VistaZeroMQClusterFollowerDataSync(const std::string& sHostname, const int nServerPort,
      const bool bSwap, const bool bVerbose = true);
  explicit VistaZeroMQClusterFollowerDataSync(
      const std::string& sZeroMQAddress, const bool bSwap, const bool bVerbose = true);
  virtual ~VistaZeroMQClusterFollowerDataSync();

  bool WaitForConnection(VistaConnectionIP* pConnection);

  virtual bool GetIsValid() const;

  virtual std::string GetDataSyncType() const;

  virtual int  GetSendBlockingThreshold() const;
  virtual bool SetSendBlockingThreshold(const int nNumBytes);

 private:
  virtual bool DoReceiveMessage();

 private:
  zmq::socket_t*  m_pSocket;
  zmq::message_t* m_pZmqMessage;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAZEROMQCLUSTERDATASYNC_H
