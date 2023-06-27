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

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

#include "VistaDataSink.h"
#include <cstddef>

#include <cstddef>

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaDataSink::DLVistaDataSink() {
}

DLVistaDataSink::~DLVistaDataSink() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool DLVistaDataSink::PullPacket(bool bBlock) {
  IDLVistaDataPacket* pPacket = this->m_pDataInput->GivePacket(bBlock);
  if (pPacket)
    return this->ConsumePacket(pPacket);

  return false;
}

bool DLVistaDataSink::AcceptDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  return ConsumePacket(pPacket);
}

bool DLVistaDataSink::RecycleDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  return m_pDataInput->RecycleDataPacket(pPacket, this, bBlock);
}

IDLVistaDataPacket* DLVistaDataSink::GivePacket(bool bBlock) {
  return 0;
}

bool DLVistaDataSink::InitPacketMgmt() {
  return true;
}

IDLVistaDataPacket* DLVistaDataSink::CreatePacket() {
  return NULL;
}

void DLVistaDataSink::DeletePacket(IDLVistaDataPacket*) {
}
