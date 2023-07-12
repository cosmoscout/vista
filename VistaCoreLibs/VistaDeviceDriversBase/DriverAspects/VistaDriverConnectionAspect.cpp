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

#include "VistaDriverConnectionAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <cassert>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverConnectionAspect::m_nAspectId = -1;

int VistaDriverConnectionAspect::GetAspectId() {
  return VistaDriverConnectionAspect::m_nAspectId;
}

void VistaDriverConnectionAspect::SetAspectId(int nId) {
  assert(m_nAspectId == -1);
  m_nAspectId = nId;
}

VistaDriverConnectionAspect::ConnectionHlp::ConnectionHlp(VistaDriverConnectionAspect* pParent,
    VistaConnection* pCon, VistaDriverConnectionAspect::IVistaConnectionSequence* pAttach,
    VistaDriverConnectionAspect::IVistaConnectionSequence* pDetach, ReplaceBehavior bCanBeReplaced,
    ConnectionCollectBehavior bCollect)
    : m_pConnection(pCon)
    , m_pAttachSequence(pAttach)
    , m_pDetachSequence(pDetach)
    , m_pParent(pParent)
    , m_bCanBeReplaced(bCanBeReplaced)
    , m_bCollect(bCollect) {
}

VistaDriverConnectionAspect::ConnectionHlp::~ConnectionHlp() {
  if (m_pConnection)
    m_pParent->DetachConnection(m_pConnection);
  delete m_pAttachSequence;
  delete m_pDetachSequence;

  if (m_bCollect == COLLECT_ON_ASPECT_DELETE)
    delete m_pConnection;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverConnectionAspect::VistaDriverConnectionAspect()
    : IVistaDeviceDriver::IVistaDeviceDriverAspect()
    , m_nUpdConnectionIndex(0) {
  if (VistaDriverConnectionAspect::GetAspectId() == -1) // unregistered
    VistaDriverConnectionAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("CONNECTION"));

  SetId(VistaDriverConnectionAspect::GetAspectId());
}

VistaDriverConnectionAspect::~VistaDriverConnectionAspect() {
  for (CONVEC::iterator it = m_vecConnections.begin(); it != m_vecConnections.end();) {
    delete (*it);
    // destructor does access the m_vecConnections
    // vector, we erase the current element,
    // catching up with the current one
    it = m_vecConnections.erase(it);
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

unsigned int VistaDriverConnectionAspect::GetUpdateConnectionIndex() const {
  return m_nUpdConnectionIndex;
}

void VistaDriverConnectionAspect::SetUpdateConnectionIndex(unsigned int nIndex) {
  m_nUpdConnectionIndex = nIndex;
}

VistaConnection* VistaDriverConnectionAspect::GetConnection(unsigned int nIdx) const {
  if (nIdx >= m_vecConnections.size())
    return NULL;

  return m_vecConnections[nIdx]->m_pConnection;
}

unsigned int VistaDriverConnectionAspect::EnumerateRoles(std::list<std::string>& liTokens) const {
  for (CONVEC::const_iterator cit = m_vecConnections.begin(); cit != m_vecConnections.end();
       ++cit) {
    liTokens.push_back((*cit)->m_strName);
  }
  return (unsigned int)liTokens.size();
}

unsigned int VistaDriverConnectionAspect::GetIndexForRole(const std::string& sToken) const {
  unsigned int n = 0;
  for (CONVEC::const_iterator cit = m_vecConnections.begin(); cit != m_vecConnections.end();
       ++cit, ++n) {
    if (VistaAspectsComparisonStuff::StringEquals(sToken, (*cit)->m_strName, false))
      return n;
  }
  return ~0;
}

void VistaDriverConnectionAspect::SetConnection(unsigned int nIndex, VistaConnection* pConnection,
    const std::string& sName, ReplaceBehavior bCanBeReplaced, ForceAlreadySetBehavior bForce,
    ConnectionCollectBehavior bCollectConnection) {
  // create enough storage, even when inserting NULLs
  if (nIndex >= m_vecConnections.size())
    m_vecConnections.resize(nIndex + 1);

  // see whether we have an entry at the given index
  if (m_vecConnections[nIndex] == NULL)
    // no, we create on, and set the connection
    m_vecConnections[nIndex] =
        new ConnectionHlp(this, pConnection, NULL, NULL, bCanBeReplaced, bCollectConnection);
  else {
    // yes, there is an entry, check if the connection is valid
    if (m_vecConnections[nIndex]->m_pConnection) {
      // yes, call a detach, we are replacing the connection
      // below
      if (m_vecConnections[nIndex]->m_bCanBeReplaced == ALLOW_CONNECTION_REPLACEMENT ||
          (bForce == REPLACE_ALSO_WHEN_ALREADY_SET))
        DetachConnection(m_vecConnections[nIndex]->m_pConnection);
      else
        return; // no change and no force
    }
  }

  // lookup the hlp structure
  ConnectionHlp* pHlp = m_vecConnections[nIndex];
  pHlp->m_strName     = sName; // replace/set name

  // set connection (might be redundant, but hey...)
  pHlp->m_pConnection = pConnection;
  pHlp->m_bCollect    = bCollectConnection;

  // ok, check whether the new connection is valid
  if (pHlp->m_pConnection)
    // yes, call attach on the new connection
    AttachConnection(pHlp->m_pConnection);
}

void VistaDriverConnectionAspect::DetachConnection(unsigned int nIndex) {
  if (nIndex >= m_vecConnections.size())
    return;

  // see whether we have en entry at the given index
  if (m_vecConnections[nIndex]->m_pConnection) {
    // yes, call a detach, we are replacing the connection
    // below
    DetachConnection(m_vecConnections[nIndex]->m_pConnection);
  }
}

bool VistaDriverConnectionAspect::AttachConnection(VistaConnection* pCon) {
  ConnectionHlp* pHlp = GetHlpByConnection(pCon);
  if (pHlp && pHlp->m_pAttachSequence)
    (*pHlp->m_pAttachSequence)(pCon); // call

  return true;
}

bool VistaDriverConnectionAspect::DetachConnection(VistaConnection* pCon) {
  ConnectionHlp* pHlp = GetHlpByConnection(pCon);

  if (pHlp && pHlp->m_pDetachSequence)
    (*pHlp->m_pDetachSequence)(pCon); // call
  return true;
}

bool VistaDriverConnectionAspect::SetAttachSequence(
    unsigned int nConIdx, IVistaConnectionSequence* pSequence) {
  // create enough storage, even when inserting NULLs
  if (nConIdx >= m_vecConnections.size())
    m_vecConnections.resize(nConIdx + 1);

  if (m_vecConnections[nConIdx] == NULL)
    m_vecConnections[nConIdx] = new ConnectionHlp(this, NULL, pSequence);
  else
    m_vecConnections[nConIdx]->m_pAttachSequence = pSequence;

  return true;
}

VistaDriverConnectionAspect::IVistaConnectionSequence*
VistaDriverConnectionAspect::GetAttachSequence(unsigned int nConIdx) const {
  if (nConIdx >= m_vecConnections.size())
    return NULL;
  return m_vecConnections[nConIdx]->m_pAttachSequence;
}

bool VistaDriverConnectionAspect::SetDetachSequence(
    unsigned int nConIdx, IVistaConnectionSequence* pSequence) {
  // create enough storage, even when inserting NULLs
  if (nConIdx >= m_vecConnections.size())
    m_vecConnections.resize(nConIdx + 1);

  if (m_vecConnections[nConIdx] == NULL)
    m_vecConnections[nConIdx] = new ConnectionHlp(this, NULL, NULL, pSequence);
  else
    m_vecConnections[nConIdx]->m_pDetachSequence = pSequence;

  return true;
}

VistaDriverConnectionAspect::IVistaConnectionSequence*
VistaDriverConnectionAspect::GetDetachSequence(unsigned int nConIdx) const {
  if (nConIdx >= m_vecConnections.size())
    return NULL;
  return m_vecConnections[nConIdx]->m_pDetachSequence;
}

bool VistaDriverConnectionAspect::SendCommand(
    unsigned int nIndex, const void* pcCmd, unsigned int unLength, unsigned int nPostSendDelay) {
  if (nIndex >= m_vecConnections.size())
    return false;

  ConnectionHlp* pHlp = m_vecConnections[nIndex];

  if (pHlp->m_pConnection &&
      ((unsigned int)pHlp->m_pConnection->Send(pcCmd, unLength) == unLength)) {
    if (nPostSendDelay != 0)
      VistaTimeUtils::Sleep(nPostSendDelay);

    return true;
  }

  return false;
}

int VistaDriverConnectionAspect::GetCommand(
    unsigned int nIndex, void* pcBuffer, unsigned int nMaxLength, int nTimeout, bool* pTOMark) {
  if (nIndex >= m_vecConnections.size())
    return false;

  ConnectionHlp* pHlp = m_vecConnections[nIndex];
  if (pHlp->m_pConnection) {
    int nRet = pHlp->m_pConnection->Receive(pcBuffer, nMaxLength, nTimeout);
    if (nRet == -1)
      return -1;
    if ((nRet == 0) && pHlp->m_pConnection->GetIsBlocking()) {
      if (pTOMark)
        *pTOMark = true;
    }
    return nRet;
  }
  return -1;
}

int VistaDriverConnectionAspect::GetTerminatedCommand(unsigned int nIndex, void* pvBuffer,
    unsigned int nMaxLength, char cMark, int nTimeout, bool* bTOMark) {
  if (nIndex >= m_vecConnections.size())
    return false;

  ConnectionHlp* pHlp = m_vecConnections[nIndex];

  if (!pHlp->m_pConnection)
    return -1;

  unsigned int nCur     = 0;
  char*        pcBuffer = (char*)pvBuffer;
  while (nCur < nMaxLength) {
    int nRet = 0;
    nRet     = pHlp->m_pConnection->Receive((void*)&pcBuffer[nCur], 1, nTimeout);
    if (nRet == 1) {
      if (pcBuffer[nCur++] == cMark)
        return nCur;
    } else if ((nRet == 0) && (pHlp->m_pConnection->GetIsBlocking())) // timeout
    {
      if (bTOMark)
        *bTOMark = true;
      break;
    } else // timeout... hmm... the user specified a timout value
           // so we return with an error, no matter if we want it
           // to be blocking or not
    {
      if (pHlp->m_pConnection->GetIsBlocking())
        break; // error?

      if (nTimeout)
        break; // no timout given, but bRet
               // else: non-blocking with no timeout value returned 0
               // which is an effective poll... should do...
    }
  }
  return nCur;
}

bool VistaDriverConnectionAspect::GetIsBlocking(unsigned int nIndex) const {
  if (nIndex >= m_vecConnections.size())
    return false;

  ConnectionHlp* pHlp = m_vecConnections[nIndex];

  if (!pHlp->m_pConnection)
    return false;
  return pHlp->m_pConnection->GetIsBlocking();
}

bool VistaDriverConnectionAspect::SetIsBlocking(unsigned int nIndex, bool bBlocking) {
  if (nIndex >= m_vecConnections.size())
    return false;

  ConnectionHlp* pHlp = m_vecConnections[nIndex];
  if (!pHlp->m_pConnection)
    return false;

  pHlp->m_pConnection->SetIsBlocking(bBlocking);
  return (pHlp->m_pConnection->GetIsBlocking() == bBlocking);
}

VistaDriverConnectionAspect::ConnectionHlp* VistaDriverConnectionAspect::GetHlpByConnection(
    VistaConnection* pCon) const {
  for (CONVEC::const_iterator cit = m_vecConnections.begin(); cit != m_vecConnections.end();
       ++cit) {
    if ((*cit)->m_pConnection == pCon)
      return (*cit);
  }
  return NULL;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
