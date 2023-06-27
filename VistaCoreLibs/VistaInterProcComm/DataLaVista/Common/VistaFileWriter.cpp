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

#include "VistaFileWriter.h"
#include "VistaByteBufferPacket.h"

#include <VistaInterProcComm/DataLaVista/Base/VistaRegistration.h>

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

DLVistaFileWriter::DLVistaFileWriter() {
  // using stdout
  m_pOutputFile  = stdout;
  m_iRecordCount = 0;
  m_bPutEndline  = false;
  m_bDBWait      = false;
}

DLVistaFileWriter::DLVistaFileWriter(const string& SFileName) {
  m_pOutputFile  = OpenOutputFile(SFileName.c_str());
  m_iRecordCount = 0;
  m_bPutEndline  = false;
  m_bDBWait      = false;
}

DLVistaFileWriter::DLVistaFileWriter(const char* pcFileName) {
  m_pOutputFile  = OpenOutputFile(pcFileName);
  m_iRecordCount = 0;
  m_bPutEndline  = false;
  m_bDBWait      = false;
}

DLVistaFileWriter::~DLVistaFileWriter() {
  printf("CLOSE: Written %d records to File\n", m_iRecordCount);
  char buffer[128];
  sprintf(buffer, "\n\n%d records written.\n", m_iRecordCount);
  fwrite(buffer, 1, strlen(buffer), m_pOutputFile);

  if (m_pOutputFile && (m_pOutputFile != stdout))
    fclose(m_pOutputFile);
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool DLVistaFileWriter::AcceptDataPacket(
    IDLVistaDataPacket* pPacket, IDLVistaPipeComponent* pComp, bool bBlock) {
  // printf("%X @ -- DLVistaFileWriter::AcceptDataPacket()\n", this);
  return ConsumePacket(pPacket);
}

bool DLVistaFileWriter::RecycleDataPacket(
    IDLVistaDataPacket*, IDLVistaPipeComponent* pComp, bool bBlock) {
  printf("%lx @ -- DLVistaFileWriter::RecycleDataPacket()\n", long(this));
  return false;
}

bool DLVistaFileWriter::ConsumePacket(IDLVistaDataPacket* pPacket) {
  printf("%lx @ -- DLVistaFileWriter::ConsumePacket(%lx)\n", long(this), long(pPacket));

  DLVistaByteBufferPacket* p = static_cast<DLVistaByteBufferPacket*>(pPacket);

  int iWritten = (int)fwrite(p->GetByteBuffer(), 1, p->GetCharBufferLength(), m_pOutputFile);
  if (iWritten != p->GetPacketSize())
    printf("warning: packet size is (%d), bytes written: [%d]\n", p->GetPacketSize(), iWritten);

  if (m_bPutEndline)
    fwrite("\n", 1, 1, m_pOutputFile);

  ++m_iRecordCount;

  if (m_bDBWait) {}
  m_pDataInput->RecycleDataPacket(pPacket, this, true);
  return true;
}

IDLVistaDataPacket* DLVistaFileWriter::GivePacket(bool bBlock) {
  printf("%lx @ -- DLVistaFileWriter::GivePacket()\n", long(this));
  return 0;
}

bool DLVistaFileWriter::InitPacketMgmt() {
  printf("%lx @ -- DLVistaFileWriter::InitPacketMgmt()\n", long(this));
  return true;
}

IDLVistaDataPacket* DLVistaFileWriter::CreatePacket() {
  return NULL;
}

void DLVistaFileWriter::DeletePacket(IDLVistaDataPacket*) {
}

FILE* DLVistaFileWriter::OpenOutputFile(const char* pcName) {
  FILE* f = fopen(pcName, "w+");
  if (!f)
    printf("## WARNING: DLVistaFileWriter::OpenOutputFile(%s) -- Could not open file!\n", pcName);
  return f;
}

int DLVistaFileWriter::GetInputPacketType() const {
  IDLVistaRegistration* pReg = IDLVistaRegistration::GetRegistrationSingleton();
  return pReg ? pReg->GetTypeIdForName("DLVistaByteBufferPacket") : -1;
}

bool DLVistaFileWriter::GetPutEndline() const {
  return m_bPutEndline;
}

void DLVistaFileWriter::SetPutEndline(bool bPutEndline) {
  m_bPutEndline = bPutEndline;
}
