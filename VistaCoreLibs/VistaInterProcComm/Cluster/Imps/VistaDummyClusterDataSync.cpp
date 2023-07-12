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

#include "VistaDummyClusterDataSync.h"

#include <VistaBase/VistaTimeUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

VistaDummyClusterDataSync::VistaDummyClusterDataSync()
    : IVistaClusterDataSync(false, true) {
}

VistaDummyClusterDataSync::~VistaDummyClusterDataSync() {
}

VistaType::systemtime VistaDummyClusterDataSync::GetSyncTime() {
  return VistaTimeUtils::GetStandardTimer().GetSystemTime();
}

bool VistaDummyClusterDataSync::SyncTime(VistaType::systemtime& nTime) {
  return true;
}

bool VistaDummyClusterDataSync::SyncData(VistaPropertyList& oList) {
  return true;
}

bool VistaDummyClusterDataSync::SyncData(IVistaSerializable& oSerializable) {
  return true;
}

bool VistaDummyClusterDataSync::SyncData(VistaType::byte* pData, const int iDataSize) {
  return true;
}

bool VistaDummyClusterDataSync::SyncData(
    VistaType::byte* pDataBuffer, const int iBufferSize, int& iDataSize) {
  return true;
}

bool VistaDummyClusterDataSync::SyncData(std::vector<VistaType::byte>& vecData) {
  return true;
}

bool VistaDummyClusterDataSync::SyncData(std::string& sData) {
  return true;
}

bool VistaDummyClusterDataSync::GetIsValid() const {
  return true;
}

std::string VistaDummyClusterDataSync::GetDataSyncType() const {
  return "DummySync";
}

bool VistaDummyClusterDataSync::SetSendBlockingThreshold(const int nNumBytes) {
  return false;
}

int VistaDummyClusterDataSync::GetSendBlockingThreshold() const {
  return -1;
}
