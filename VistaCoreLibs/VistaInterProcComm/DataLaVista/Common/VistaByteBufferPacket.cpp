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

#include "VistaByteBufferPacket.h"
#include <VistaAspects/VistaDeSerializer.h>
#include <VistaAspects/VistaSerializer.h>

#include <cstdio>
#include <cstring>
#include <string>
using namespace std;

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaByteBufferPacket::DLVistaByteBufferPacket(IDLVistaPipeComponent* pProd)
    : IDLVistaDataPacket(pProd) {
  m_pSBuffer = new string;
}

DLVistaByteBufferPacket::~DLVistaByteBufferPacket() {
  delete m_pSBuffer;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
IDLVistaDataPacket* DLVistaByteBufferPacket::CreateInstance(IDLVistaPipeComponent* pProd) const {
  return new DLVistaByteBufferPacket(pProd);
}

IDLVistaDataPacket* DLVistaByteBufferPacket::Clone() const {
  return 0;
}

int DLVistaByteBufferPacket::GetPacketType() const {
  return -1;
}

void DLVistaByteBufferPacket::AppendToBuffer(const string& SAppendString) {
  (*m_pSBuffer).append(SAppendString);
}

void DLVistaByteBufferPacket::AppendToBuffer(const char* pcBuffer) {
  (*m_pSBuffer).append(pcBuffer);
}

const char* DLVistaByteBufferPacket::GetCharBuffer() const {
  return (*m_pSBuffer).c_str();
}

string* DLVistaByteBufferPacket::GetStringBuffer() const {
  return m_pSBuffer;
}

unsigned int DLVistaByteBufferPacket::GetCharBufferLength() const {
  return (int)strlen((*m_pSBuffer).c_str());
}

void* DLVistaByteBufferPacket::GetByteBuffer() const {
  return (void*)(*m_pSBuffer).data();
}

void DLVistaByteBufferPacket::SetBuffer(const char* pcBuf) {
  *m_pSBuffer = pcBuf;
}

void DLVistaByteBufferPacket::SetBuffer(const string& SBuf) {
  (*m_pSBuffer) = SBuf;
}

void DLVistaByteBufferPacket::SetDataSize(int iNewSize) {
  (*m_pSBuffer).resize(iNewSize);
}

int DLVistaByteBufferPacket::GetDataSize() const {
  return (int)(*m_pSBuffer).length();
}

int DLVistaByteBufferPacket::Serialize(IVistaSerializer& rSerializer) const {
  // write signature for id purposes
  int iNumBytes = rSerializer.WriteInt32(VistaType::uint32(this->GetSignature().length()));
  iNumBytes += rSerializer.WriteString(this->GetSignature());
  // write content itself
  iNumBytes += rSerializer.WriteInt32(this->GetDataSize());
  iNumBytes += rSerializer.WriteString(*m_pSBuffer);
  return iNumBytes;
}

int DLVistaByteBufferPacket::DeSerialize(IVistaDeSerializer& rDeSerializer) {
  int iBufferLen;
  // read and check signature
  int    iNumBytes = rDeSerializer.ReadInt32(iBufferLen);
  string strSig;
  iNumBytes += rDeSerializer.ReadString(strSig, iBufferLen);
  if (strSig == this->GetSignature()) {
    // read packet content
    iNumBytes += rDeSerializer.ReadInt32(iBufferLen);
    iNumBytes += rDeSerializer.ReadString(*m_pSBuffer, iBufferLen);
    return iNumBytes;
  } else {
    printf("\n\n[DLVistaByteBufferPacket::DeSerialize] ERROR: Wrong signature encountered! Packet "
           "could not be deser'ed\n\n");
    return -1;
  }
}
