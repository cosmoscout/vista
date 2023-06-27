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

#include "VistaTCPIPClusterBarrier.h"

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/IPNet/VistaUDPSocket.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* LEADER                                                                     */
/*============================================================================*/

VistaTCPIPClusterLeaderBarrier::VistaTCPIPClusterLeaderBarrier(const bool bVerbose)
    : VistaClusterLeaderBarrierIPBase(bVerbose) {
}

VistaTCPIPClusterLeaderBarrier::~VistaTCPIPClusterLeaderBarrier() {
}

bool VistaTCPIPClusterLeaderBarrier::BarrierWait(int iTimeOut) {
  if (GetIsValid() == false)
    return false;

  if (VistaClusterLeaderBarrierIPBase::WaitForAllFollowers(iTimeOut) == false)
    return false;

  // send out go signal
  // ip is not well suited for this, but if there is no alternative...
  std::vector<Follower*>  vecDiedFollowers;
  std::vector<Follower*>& vecFollowers = GetFollowersRef();
  for (std::vector<Follower*>::iterator itFollower = vecFollowers.begin();
       itFollower != vecFollowers.end(); ++itFollower) {
    try {
      VistaType::sint32 nBarrierCount = GetBarrierWaitCount();
      if ((*itFollower)->m_pLeaderConn->WriteInt32(nBarrierCount) != sizeof(VistaType::uint32)) {
        if (GetIsVerbose()) {
          vstr::warnp() << "[" << GetBarrierType() << "]: "
                        << "Writing Go-Token for follower [" << (*itFollower)->m_sName
                        << "] failed - removing it from sync list" << std::endl;
        }
        vecDiedFollowers.push_back((*itFollower));
        continue;
      }
    } catch (VistaExceptionBase&) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[" << GetBarrierType() << "]: "
                      << "Exception while writing go-token for follower [" << (*itFollower)->m_sName
                      << "] - removing it from sync list" << std::endl;
      }
      vecDiedFollowers.push_back((*itFollower));
      continue;
    }
  }

  // kill dead followers
  for (std::vector<Follower*>::iterator itDied = vecDiedFollowers.begin();
       itDied != vecDiedFollowers.end(); ++itDied) {
    RemoveFollower((*itDied));
  }
  vecDiedFollowers.clear();

  return GetIsValid();
}

bool VistaTCPIPClusterLeaderBarrier::GetIsValid() const {
  return VistaClusterLeaderBarrierIPBase::GetIsValid();
}

std::string VistaTCPIPClusterLeaderBarrier::GetBarrierType() const {
  return "TCPIPLeaderBarrier";
}

/*============================================================================*/
/*  FOLLOWER                                                                  */
/*============================================================================*/

VistaTCPIPClusterFollowerBarrier::VistaTCPIPClusterFollowerBarrier(
    VistaConnectionIP* pLeaderConn, const bool bManageDeletion, const bool bVerbose)
    : VistaClusterFollowerBarrierIPBase(pLeaderConn, bManageDeletion, bVerbose) {
}

VistaTCPIPClusterFollowerBarrier::~VistaTCPIPClusterFollowerBarrier() {
}

bool VistaTCPIPClusterFollowerBarrier::GetIsValid() const {
  return VistaClusterFollowerBarrierIPBase::GetIsValid();
}

bool VistaTCPIPClusterFollowerBarrier::BarrierWait(int iTimeOut) {
  if (GetIsValid() == false)
    return false;

  if (VistaClusterFollowerBarrierIPBase::SendReadyTokenToLeader(iTimeOut) == false)
    return false;

  VistaType::sint32 nReceive;
  try {
    VistaConnectionIP* pConnection = GetConnection();
    pConnection->SetReadTimeout(iTimeOut);
    int nReturn = pConnection->ReadInt32(nReceive);
    pConnection->SetReadTimeout(0);
    if (nReturn != sizeof(VistaType::sint32)) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[" << GetBarrierType() << "]: "
                      << "Error receiving go token from leader" << std::endl;
      }
      ProcessError();
      return false;
    }

    if (GetDoesSwap())
      VistaSerializingToolset::Swap(&nReceive, sizeof(VistaType::uint64));

    if (nReceive != GetBarrierWaitCount()) {
      if (GetIsVerbose()) {
        vstr::warnp() << "[" << GetBarrierType() << "]: "
                      << "Received BarrierCounter is [" << nReceive << "], but expected ["
                      << GetBarrierWaitCount() << "]" << std::endl;
      }
      SetBarrierWaitCount(nReceive);
      return false;
    }
  } catch (VistaExceptionBase&) {
    if (GetIsVerbose()) {
      vstr::warnp() << "[" << GetBarrierType() << "]: "
                    << "Exception during syncing" << std::endl;
    }
    ProcessError();
    return false;
  }
  return true;
}

std::string VistaTCPIPClusterFollowerBarrier::GetBarrierType() const {
  return "TCPIPFollowerBarrier";
}
