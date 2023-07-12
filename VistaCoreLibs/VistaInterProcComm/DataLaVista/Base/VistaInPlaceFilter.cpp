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

#include "VistaInPlaceFilter.h"

#include "VistaDataPacket.h"
#include "VistaPacketQueue.h"
#include "VistaRegistration.h"

#include <string>
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

IDLVistaInPlaceFilter::IDLVistaInPlaceFilter()
    : IDLVistaFilter() {
  m_pPacketName = new string;
}

IDLVistaInPlaceFilter::~IDLVistaInPlaceFilter() {
  delete m_pPacketName;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool IDLVistaInPlaceFilter::RecycleDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  // give back to producer
  (*m_pInput).RecycleDataPacket(pPacket, this);
  return true;
}

bool IDLVistaInPlaceFilter::AcceptDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  IDLVistaDataPacket* p = FilterPacketL(pPacket);
  if (p) // ok, filtering worked
  {
    // forward to output
    return this->m_pOutput->AcceptDataPacket(p, this);
  } else {
    // ok, we should recycle the old packet, filtering did not work or
    // packet was rejected.
    RecycleDataPacket(pPacket, this);
  }
  // something failed, give back false, the caller should be wise to recycle the packet properly
  return false;
}

IDLVistaDataPacket* IDLVistaInPlaceFilter::GivePacket(bool bBlock) {
  IDLVistaDataPacket* p = this->m_pInput->GivePacket(bBlock);
  if (p) {
    // ok, got a new packet
    IDLVistaDataPacket* p1 = FilterPacketL(p);
    if (p1) {
      // ok, got a new one and it was filtered, we can return this
      return p1;
    } else {
      // hmm... got new packet, but filtering did not work out for this one...
      RecycleDataPacket(p, this);
    }
  }
  return NULL;
}

IDLVistaDataPacket* IDLVistaInPlaceFilter::CreatePacket() {
  return NULL;
}

void IDLVistaInPlaceFilter::DeletePacket(IDLVistaDataPacket*) {
}

bool IDLVistaInPlaceFilter::InitPacketMgmt() {
  return true;
}

int IDLVistaInPlaceFilter::GetInputPacketType() const {
  IDLVistaRegistration* pReg = IDLVistaRegistration::GetRegistrationSingleton();
  return pReg ? pReg->GetTypeIdForName((*m_pPacketName)) : -1;
}

int IDLVistaInPlaceFilter::GetOutputPacketType() const {
  IDLVistaRegistration* pReg = IDLVistaRegistration::GetRegistrationSingleton();
  return pReg ? pReg->GetTypeIdForName((*m_pPacketName)) : -1;
}

void IDLVistaInPlaceFilter::SetPacketTypeName(const string& SName) {
  (*m_pPacketName) = SName;
}
