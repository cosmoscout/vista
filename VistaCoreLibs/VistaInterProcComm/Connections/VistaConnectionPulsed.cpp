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

#include "VistaConnectionPulsed.h"

#include <VistaBase/VistaTimeUtils.h>

VistaConnectionPulsed::VistaConnectionPulsed(VistaConnection* main_connection,
    unsigned int pulse_distance_in_ms, SubConnectionDeleteBehavior sub_con_del_behavior)
    : VistaConnection()
    , m_main_connection(main_connection)
    , m_ticker(new VistaTicker(pulse_distance_in_ms))
    , m_context(0)
    , m_sub_con_del_behavior(sub_con_del_behavior) {
}

VistaConnectionPulsed::~VistaConnectionPulsed() {
  delete m_ticker;

  m_main_connection->Close();

  if (m_sub_con_del_behavior == DELETE_SUB_CONNECTION)
    delete m_main_connection;
}

bool VistaConnectionPulsed::Open() {
  return m_main_connection->Open();
}

void VistaConnectionPulsed::Close() {
  m_main_connection->Close();
}

int VistaConnectionPulsed::Receive(void* pBuffer, const int nLength, int iTimeout) {
  if (m_context == 0)
    return m_main_connection->Receive(pBuffer, nLength, iTimeout);

  unsigned int nTimeout = iTimeout;
  if (((unsigned int)iTimeout) < m_ticker->GetGranularity()) // timeout before tick!
    nTimeout = m_ticker->GetGranularity() - iTimeout;

  int return_value = m_main_connection->Receive(pBuffer, nLength, nTimeout);

  // this receive was context-triggered, so reset the co-responding event
  m_context->Reset();

  return return_value;
}

int VistaConnectionPulsed::Send(const void* pBuffer, const int nLength) {
  return -1; // input only for now
}

bool VistaConnectionPulsed::HasPendingData() const {
  return m_main_connection->HasPendingData();
}

unsigned long VistaConnectionPulsed::WaitForIncomingData(int timeout) {
  // adjust to desired pulse
  unsigned int nTimeout = timeout;
  if (((unsigned int)timeout) < m_ticker->GetGranularity()) // timeout before tick!
    nTimeout = m_ticker->GetGranularity() - timeout;

  return m_main_connection->WaitForIncomingData(nTimeout);
}

unsigned long VistaConnectionPulsed::WaitForSendFinish(int timeout) {
  return 0; // receive only connection
}

unsigned long VistaConnectionPulsed::PendingDataSize() const {
  return m_main_connection->PendingDataSize();
}

bool VistaConnectionPulsed::GetIsBlocking() const {
  return m_main_connection->GetIsBlocking();
}

bool VistaConnectionPulsed::GetIsBuffering() const {
  return m_main_connection->GetIsBuffering();
}

void VistaConnectionPulsed::SetIsBlocking(bool bBlocking) {
  m_main_connection->SetIsBlocking(bBlocking);
}

void VistaConnectionPulsed::SetIsBuffering(bool bBuffering) {
  m_main_connection->SetIsBuffering(bBuffering);
}

int VistaConnectionPulsed::GetReadTimeout() const {
  return m_main_connection->GetReadTimeout();
}

bool VistaConnectionPulsed::SetReadTimeout(int nReadTimeout) {
  return m_main_connection->SetReadTimeout(nReadTimeout);
}

bool VistaConnectionPulsed::Flush() {
  return m_main_connection->Flush();
}

bool VistaConnectionPulsed::GetIsFine() const {
  return m_main_connection->GetIsFine();
}

HANDLE VistaConnectionPulsed::GetConnectionDescriptor() const {
  return m_main_connection->GetConnectionDescriptor();
}

HANDLE VistaConnectionPulsed::GetConnectionWaitForDescriptor() {
  if (m_context == 0) {
    m_context = new VistaTicker::TriggerContext(m_ticker->GetGranularity(), true);
    m_ticker->AddTrigger(m_context);
    m_ticker->StartTicker();
  }
  return m_context->GetEventWaitHandle();
}

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
