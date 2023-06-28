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

#include "VistaPriority.h"
#include "Imp/VistaPriorityImp.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPriority::VistaPriority(int iPrio) {
  m_pPriorityImp   = IVistaPriorityImp::CreatePriorityImp();
  m_iVistaPriority = iPrio;
}

VistaPriority::VistaPriority(const VistaPriority& prio) {
  m_pPriorityImp   = IVistaPriorityImp::CreatePriorityImp();
  m_iVistaPriority = prio.m_iVistaPriority;
}

VistaPriority::~VistaPriority() {
  delete m_pPriorityImp;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int VistaPriority::GetVistaPriority() const {
  return m_iVistaPriority;
}

void VistaPriority::SetVistaPriority(int iPrio) {
  m_iVistaPriority = iPrio;
}

int VistaPriority::GetSystemPriority() const {
  return m_pPriorityImp->ScalePriorityToSystemPriority(m_iVistaPriority);
}

int VistaPriority::GetVistaPriorityForSystemPriority(int iSysPrio) const {
  return m_pPriorityImp->ScaleSystemPriorityToPriority(iSysPrio);
}

VistaPriority& VistaPriority::operator=(const VistaPriority& inPrio) {
  m_iVistaPriority = inPrio.m_iVistaPriority;
  return *this;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
