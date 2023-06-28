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

#ifndef _VISTAZEROMQCLUSTERBARRIER_H
#define _VISTAZEROMQCLUSTERBARRIER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaInterProcComm/Cluster/Imps/VistaClusterBarrierIPBase.h>

#include <string>
#include <vector>

/*============================================================================*/
/* FORWARD DECLERATIONS                                                       */
/*============================================================================*/
namespace zmq {
class socket_t;
class message_t;
} // namespace zmq
class VistaConnectionIP;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaZeroMQClusterLeaderBarrier : public VistaClusterLeaderBarrierIPBase {
 public:
  explicit VistaZeroMQClusterLeaderBarrier(
      const std::string& sZeroMQAddress, const bool bVerbose = true);
  virtual ~VistaZeroMQClusterLeaderBarrier();

  virtual bool BarrierWait(int iTimeOut = 0);

  virtual bool GetIsValid() const;

  virtual std::string GetBarrierType() const;

  bool WaitForConnection(const std::vector<VistaConnectionIP*>& vecConnections);

 private:
  zmq::socket_t* m_pSocket;
};

class VISTAKERNELAPI VistaZeroMQClusterFollowerBarrier : public VistaClusterFollowerBarrierIPBase {
 public:
  explicit VistaZeroMQClusterFollowerBarrier(const std::string& sZeroMQAddress,
      VistaConnectionIP* pLeaderConnection, const bool bManageConnection = true,
      const bool bVerbose = true);
  virtual ~VistaZeroMQClusterFollowerBarrier();

  virtual bool BarrierWait(int iTimeOut = 0);

  virtual bool GetIsValid() const;

  virtual std::string GetBarrierType() const;

  bool WaitForConnection(VistaConnectionIP* pConnection);

 private:
  zmq::socket_t*  m_pSocket;
  zmq::message_t* m_pZmqMessage;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAZEROMQCLUSTERBARRIER_H
