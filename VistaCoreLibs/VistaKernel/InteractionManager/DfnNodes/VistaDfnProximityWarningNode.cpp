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
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDfnProximityWarningNode.h"

#include <VistaKernel/Stuff/ProximityWarning/VistaProximityWarningBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnProximityWarningNode::VistaDfnProximityWarningNode()
    : m_pUserOrientationPort(NULL)
    , m_pUserPositionPort(NULL)
    , m_pEnabledPort(NULL)
    , m_bEnabled(true) {
  RegisterInPortPrototype("user_position", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("user_orientation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
  RegisterInPortPrototype("enabled", new TVdfnPortTypeCompare<TVdfnPort<bool>>);
}

VistaDfnProximityWarningNode::~VistaDfnProximityWarningNode() {
  for (std::vector<IVistaProximityWarningBase*>::iterator itWarning = m_vecWarnings.begin();
       itWarning != m_vecWarnings.end(); ++itWarning) {
    delete (*itWarning);
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaDfnProximityWarningNode::PrepareEvaluationRun() {
  m_pUserPositionPort    = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("user_position"));
  m_pUserOrientationPort = dynamic_cast<TVdfnPort<VistaQuaternion>*>(GetInPort("user_orientation"));
  m_pEnabledPort         = dynamic_cast<TVdfnPort<bool>*>(GetInPort("enabled"));

  return GetIsValid();
}

bool VistaDfnProximityWarningNode::GetIsValid() const {
  return (m_pUserPositionPort != NULL && m_vecWarnings.empty() == false);
}

bool VistaDfnProximityWarningNode::DoEvalNode() {
  if (m_vecWarnings.empty())
    return true;
  if (m_pEnabledPort) {
    if (m_pEnabledPort->GetValue() != m_bEnabled) {
      m_bEnabled = m_pEnabledPort->GetValue();
      for (std::vector<IVistaProximityWarningBase*>::iterator itWarn = m_vecWarnings.begin();
           itWarn != m_vecWarnings.end(); ++itWarn) {
        (*itWarn)->SetIsEnabled(m_bEnabled);
      }
    }
  }
  if (m_bEnabled == false)
    return true;

  // we don't have to update unless we have at least a postion set
  if (m_pUserPositionPort->GetUpdateCounter() == 0)
    return true;

  VistaVector3D   v3Position = m_pUserPositionPort->GetValue();
  VistaQuaternion qOrientation;
  if (m_pUserOrientationPort)
    qOrientation = m_pUserOrientationPort->GetValue();
  std::vector<VistaVector3D> vecAdditionalPositions;
  for (std::vector<TVdfnPort<VistaVector3D>*>::const_iterator itAdd = m_vecAdditionalPorts.begin();
       itAdd != m_vecAdditionalPorts.end(); ++itAdd) {
    vecAdditionalPositions.push_back((*itAdd)->GetValue());
  }

  for (std::vector<IVistaProximityWarningBase*>::iterator itWarn = m_vecWarnings.begin();
       itWarn != m_vecWarnings.end(); ++itWarn) {
    (*itWarn)->Update(v3Position, qOrientation, vecAdditionalPositions);
  }
  return true;
}

bool VistaDfnProximityWarningNode::SetInPort(const std::string& sName, IVdfnPort* pPort) {
  if (IVdfnNode::SetInPort(sName, pPort))
    return true;

  TVdfnPort<VistaVector3D>* pVecPort = dynamic_cast<TVdfnPort<VistaVector3D>*>(pPort);
  if (pVecPort == NULL)
    return false;

  m_mpInPrototypes[sName] = pPort->GetPortTypeCompare();
  m_vecAdditionalPorts.push_back(pVecPort);
  return DoSetInPort(sName, pPort);
}

void VistaDfnProximityWarningNode::AddWarning(IVistaProximityWarningBase* pWarn) {
  m_vecWarnings.push_back(pWarn);
}

bool VistaDfnProximityWarningNode::GetIsEnabled() const {
  return m_bEnabled;
}

bool VistaDfnProximityWarningNode::SetIsEnabled(const bool bSet) {
  if (m_bEnabled != bSet)
    return true;
  for (std::vector<IVistaProximityWarningBase*>::iterator itWarn = m_vecWarnings.begin();
       itWarn != m_vecWarnings.end(); ++itWarn) {
    (*itWarn)->SetIsEnabled(bSet);
  }

  m_bEnabled = bSet;
  return true;
}

void VistaDfnProximityWarningNode::OnActivation(double dTs) {
  if (m_bEnabled) {
    for (std::vector<IVistaProximityWarningBase*>::iterator itWarn = m_vecWarnings.begin();
         itWarn != m_vecWarnings.end(); ++itWarn) {
      (*itWarn)->SetIsEnabled(true);
    }
  }
}

void VistaDfnProximityWarningNode::OnDeactivation(double dTs) {
  for (std::vector<IVistaProximityWarningBase*>::iterator itWarn = m_vecWarnings.begin();
       itWarn != m_vecWarnings.end(); ++itWarn) {
    (*itWarn)->SetIsEnabled(false);
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
