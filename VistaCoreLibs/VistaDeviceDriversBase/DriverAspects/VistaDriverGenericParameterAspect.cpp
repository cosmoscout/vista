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

#include "VistaDriverGenericParameterAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"

#include <cassert>
#include <list>
using std::list;

REFL_IMPLEMENT(VistaDriverGenericParameterAspect::IParameterContainer, IVistaReflectionable);

VistaDriverGenericParameterAspect::IParameterContainer::IParameterContainer() {
}

VistaDriverGenericParameterAspect::IParameterContainer::~IParameterContainer() {
}
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverGenericParameterAspect::m_nAspectId = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverGenericParameterAspect::VistaDriverGenericParameterAspect(IContainerCreate* pCreate)
    : IVistaDeviceDriver::IVistaDeviceDriverAspect(false)
    , m_pCreate(pCreate)
    , m_pParams(NULL)

{
  if (VistaDriverGenericParameterAspect::GetAspectId() == -1) // unregistered
    VistaDriverGenericParameterAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("PARAMETER"));

  SetId(VistaDriverGenericParameterAspect::GetAspectId());
}

VistaDriverGenericParameterAspect::~VistaDriverGenericParameterAspect() {
  m_pCreate->DeleteContainer(m_pParams);
  delete m_pCreate;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaDriverGenericParameterAspect::IParameterContainer*
VistaDriverGenericParameterAspect::GetParameterContainer() const {
  if (m_pParams == NULL)
    m_pParams = m_pCreate->CreateContainer();
  return m_pParams;
}

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int VistaDriverGenericParameterAspect::GetAspectId() {
  return VistaDriverGenericParameterAspect::m_nAspectId;
}

void VistaDriverGenericParameterAspect::SetAspectId(int nId) {
  assert(m_nAspectId == -1);
  m_nAspectId = nId;
}

void VistaDriverGenericParameterAspect::Print(std::ostream& out) const {
  IVistaDeviceDriver::IVistaDeviceDriverAspect::Print(out);
  out << "This aspect represents to following properties:" << std::endl;

  VistaDriverGenericParameterAspect::IParameterContainer* c = GetParameterContainer();
  if (c) {
    VistaPropertyList list;
    c->GetPropertiesByList(list);
    list.Print(out, 1);
  } else {
    out << "Could not create container?" << std::endl;
  }
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
