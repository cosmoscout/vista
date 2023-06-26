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

#include <string>
using namespace std;

#include "Imp/VistaMutexImp.h"
#include "VistaMutex.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaMutex::VistaMutex() {
  m_pImp = IVistaMutexImp::CreateMutexImp("", IVistaMutexImp::eIntraProcess);
}

VistaMutex::VistaMutex(const string& sName) {
  m_pImp = IVistaMutexImp::CreateMutexImp(sName, IVistaMutexImp::eInterProcess);
}

VistaMutex::VistaMutex(const VistaMutex&)
    : m_pImp(NULL) {
}

VistaMutex& VistaMutex::operator=(const VistaMutex&) {
  return *this;
}

VistaMutex::~VistaMutex() {
  delete m_pImp;
}

void VistaMutex::Lock() {
  m_pImp->Lock();
}

void VistaMutex::Unlock() {
  m_pImp->Unlock();
}

bool VistaMutex::TryLock() {
  return m_pImp->TryLock();
}
