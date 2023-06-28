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
// $Id: VistaSerializable.cpp 28992 2012-01-27 09:12:29Z dr165799 $

#include "VistaSharedCoreOwner.h"

#include "VistaSharedCore.h"

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaSharedCoreOwner::IVistaSharedCoreOwner(
    IVistaSharedCore* pCore, const DataHandling eDataHandlingMode)
    : m_pCore(pCore)
    , m_eDataHandlingMode(eDataHandlingMode) {
  m_pCore->IncReferenceCount();
}

IVistaSharedCoreOwner::IVistaSharedCoreOwner(const IVistaSharedCoreOwner& oCopy)
    : m_eDataHandlingMode(oCopy.m_eDataHandlingMode) {
  switch (m_eDataHandlingMode) {
  case DH_ALWAYS_COPY: {
    m_pCore = oCopy.m_pCore->Clone();
  }
  case DH_ALWAYS_SHARE:
  case DH_COPY_ON_WRITE: {
    m_pCore = oCopy.m_pCore;
  }
  }
  m_pCore->IncReferenceCount();
}

IVistaSharedCoreOwner::~IVistaSharedCoreOwner() {
  m_pCore->DecReferenceCount();
}

const IVistaSharedCore* IVistaSharedCoreOwner::GetCore() const {
  return m_pCore;
}

IVistaSharedCore* IVistaSharedCoreOwner::GetCoreForWriting() {
  PrepareCoreForWriting();
  return m_pCore;
}

void IVistaSharedCoreOwner::PrepareCoreForWriting() {
  switch (m_eDataHandlingMode) {
  case DH_ALWAYS_COPY:
  case DH_ALWAYS_SHARE:
    break;
  case DH_COPY_ON_WRITE: {
    MakeCoreUniquelyUsed();
    break;
  }
  }
}

void IVistaSharedCoreOwner::MakeCoreUniquelyUsed() {
  if (m_pCore->GetReferenceCount() > 1) {
    IVistaSharedCore* pNewCore = m_pCore->Clone();
    pNewCore->IncReferenceCount();
    m_pCore->DecReferenceCount();
    m_pCore = pNewCore;
  }
}

IVistaSharedCoreOwner::DataHandling IVistaSharedCoreOwner::GetDataHandlingMode() const {
  return m_eDataHandlingMode;
}

void IVistaSharedCoreOwner::SetDataHandlingMode(const DataHandling eMode) {
  m_eDataHandlingMode = eMode;
  switch (m_eDataHandlingMode) {
  case DH_ALWAYS_SHARE:
  case DH_COPY_ON_WRITE:
    break;
  case DH_ALWAYS_COPY: {
    MakeCoreUniquelyUsed();
    break;
  }
  }
}

IVistaSharedCoreOwner& IVistaSharedCoreOwner::operator=(const IVistaSharedCoreOwner& oOther) {
  m_eDataHandlingMode = oOther.m_eDataHandlingMode;
  if (m_pCore != oOther.m_pCore) {
    m_pCore->DecReferenceCount();
    switch (m_eDataHandlingMode) {
    case DH_ALWAYS_COPY: {
      m_pCore = oOther.m_pCore->Clone();
    }
    case DH_ALWAYS_SHARE:
    case DH_COPY_ON_WRITE: {
      m_pCore = oOther.m_pCore;
    }
    }
    m_pCore->IncReferenceCount();
  }

  return (*this);
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
