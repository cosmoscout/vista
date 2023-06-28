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

#include "VdfnNode.h"

#include "VdfnPort.h"
#include "VdfnPortFactory.h"

#include <VistaTools/VistaBasicProfiler.h>

#include <VistaBase/VistaStreamUtils.h>

#include <algorithm>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VdfnNoMatchCompare : public IVdfnPortTypeCompare {
 public:
  VdfnNoMatchCompare() {
  }
  virtual bool IsTypeOf(const IVdfnPort*) const {
    return false;
  }
  virtual IVdfnPortTypeCompare* Clone() const {
    return new VdfnNoMatchCompare;
  }
  virtual bool Assign(IVdfnPort*) {
    return false;
  }
  virtual std::string GetTypeDescriptor() const {
    return "<none>";
  }

  virtual IVdfnPort* CreatePort() const {
    return 0;
  }
};

namespace {
VdfnNoMatchCompare SNoMatchCompare;
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVdfnNode::IVdfnNode()
    : IVistaNameable()
    , m_nLastUpdate(0)
    , m_nUpdateCount(0)
    , m_bUnconditionalUpdate(false)
    , m_bEnabled(true) {
}

IVdfnNode::~IVdfnNode() {
  for (PortMap::iterator it1 = m_mpOutPorts.begin(); it1 != m_mpOutPorts.end(); ++it1) {
    delete (*it1).second;
  }

  for (TypeMap::iterator it2 = m_mpInPrototypes.begin(); it2 != m_mpInPrototypes.end(); ++it2) {
    delete (*it2).second;
  }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NAMEABLE INTERFACE
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::string IVdfnNode::GetNameForNameable() const {
  return m_strNodeName;
}

void IVdfnNode::SetNameForNameable(const std::string& sNewName) {
  m_strNodeName = sNewName;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NODE EVALUATION STUFF
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool IVdfnNode::NeedsEval() const {
  if (!m_bEnabled)
    return false;

  unsigned int nUpdCount = CalcUpdateNeededScore();
  if (nUpdCount > m_nUpdateCount) {
    return true;
  }
  return false;
}

unsigned int IVdfnNode::GetUpdateCount() const {
  return m_nUpdateCount;
}

bool IVdfnNode::EvalNode(double nTimeStamp) {
  // VistaProfileScope( "Eval[" + GetNameForNameable() + "]" );

  if (!m_bEnabled)
    return true;

  unsigned int nUpdCount = CalcUpdateNeededScore();
  if (nUpdCount == ~0u) {
    SetUpdateTimeStamp(nTimeStamp);
    ++m_nUpdateCount;
    return DoEvalNode(); // unconditional update
  }

  if (nUpdCount > m_nUpdateCount) {
    SetUpdateTimeStamp(nTimeStamp);
    if (DoEvalNode()) {
      m_nUpdateCount = nUpdCount;
      return true;
    } else
      return false;
  }
  return true; // input is unchanged, we assume no change of output
}

bool IVdfnNode::DoEvalNode() {
  return true; // default: it's the empty node
}

bool IVdfnNode::UnconditionalEvaluation() const {
  return m_bUnconditionalUpdate; // calc update score
}

void IVdfnNode::SetEvaluationFlag(bool bDoUnconditionalUpdate) {
  m_bUnconditionalUpdate = bDoUnconditionalUpdate;
}

bool IVdfnNode::GetIsEnabled() const {
  return m_bEnabled;
}

void IVdfnNode::SetIsEnabled(bool bEnabled) {
  m_bEnabled = bEnabled;
}

bool IVdfnNode::PreparePorts() {
  return true;
}

void IVdfnNode::OnActivation(double dTs) {
}

void IVdfnNode::OnDeactivation(double dTs) {
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PORT-STUFF
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool IVdfnNode::SetInPort(const std::string& sName, IVdfnPort* pPort) {
  IVdfnPortTypeCompare* pCmp = CheckInPortRegistration(sName, pPort);
  if (pCmp != NULL) {
    if (DoSetInPort(sName, pPort)) {
      /** @todo we do not return the success of assign here, */
      // as this is new code... but should to one day...
      //#if defined(DEBUG)
      //			bool bAssigned = pCmp->Assign( pPort );
      //			std::cout << "Assigned port ["
      //			          << sName
      //			          << "] of node ["
      //					  << GetNameForNameable()
      //					  << "] -- "
      //					  << (bAssigned ? "OK" : "FAILED")
      //					  << std::endl;
      //#else
      pCmp->Assign(pPort);
      //#endif
      return true;
    }
  }

  return false;
}

bool IVdfnNode::DoSetInPort(const std::string& sName, IVdfnPort* pPort) {
#if defined(DEBUG)
  if (m_mpInPorts.find(sName) != m_mpInPorts.end())
    vstr::warnp() << "IVdfnNode[" << GetNameForNameable() << "]::DoSetInPort() -- "
                  << "port [" << sName << "] was already assigned" << std::endl;
#endif

  m_mpInPorts[sName] = pPort;
  return true;
}

IVdfnPort* IVdfnNode::GetInPort(const std::string& sName) const {
  PortMap::const_iterator cit = m_mpInPorts.find(sName);
  if (cit == m_mpInPorts.end())
    return NULL;

  return (*cit).second;
}

bool IVdfnNode::RegisterOutPort(const std::string& sName, IVdfnPort* pPort) {
  m_mpOutPorts[sName] = pPort;
  return true;
}

IVdfnPort* IVdfnNode::GetOutPort(const std::string& sName) const {
  PortMap::const_iterator cit = m_mpOutPorts.find(sName);
  if (cit == m_mpOutPorts.end())
    return NULL;

  return (*cit).second;
}

bool IVdfnNode::GetNameForOutPort(IVdfnPort* pPort, std::string& strName) const {
  // reverse lookup in m_mpInPortMap
  for (PortMap::const_iterator cit = m_mpOutPorts.begin(); cit != m_mpOutPorts.end(); ++cit) {
    if ((*cit).second == pPort) {
      strName = (*cit).first;
      return true;
    }
  }
  return false;
}

std::list<std::string> IVdfnNode::GetOutPortNames() const {
  std::list<std::string> liRet;
  for (PortMap::const_iterator cit = m_mpOutPorts.begin(); cit != m_mpOutPorts.end(); ++cit) {
    liRet.push_back((*cit).first);
  }

  liRet.sort();
  return liRet;
}

std::list<std::string> IVdfnNode::GetInPortNames() const {
  std::list<std::string> liRet;
  for (TypeMap::const_iterator cit = m_mpInPrototypes.begin(); cit != m_mpInPrototypes.end();
       ++cit) {
    liRet.push_back((*cit).first);
  }

  liRet.sort();
  return liRet;
}

std::list<std::string> IVdfnNode::GetConnectedInPortNames() const {
  std::list<std::string> liRet;
  for (PortMap::const_iterator cit = m_mpInPorts.begin(); cit != m_mpInPorts.end(); ++cit) {
    liRet.push_back((*cit).first);
  }

  liRet.sort();
  return liRet;
}

double IVdfnNode::GetUpdateTimeStamp() const {
  return m_nLastUpdate;
}

void IVdfnNode::SetUpdateTimeStamp(double nTs) {
  m_nLastUpdate = nTs;
}

unsigned int IVdfnNode::CalcUpdateNeededScore() const {
  if (m_bUnconditionalUpdate)
    return ~0u;

  unsigned int nRet = 0;
  for (std::map<std::string, IVdfnPort*>::const_iterator cit = m_mpInPorts.begin();
       cit != m_mpInPorts.end(); ++cit) {
    nRet += (*cit).second->GetUpdateCounter();
  }

  return nRet;
}

bool IVdfnNode::RemInPort(const std::string& sName) {
  std::map<std::string, IVdfnPort*>::iterator it = m_mpInPorts.find(sName);
  if (it != m_mpInPorts.end()) {
    m_mpInPorts.erase(it);
    m_nUpdateCount = CalcUpdateNeededScore();
  }

  return true;
}

unsigned int IVdfnNode::GetInPortCount() const {
  return (unsigned int)m_mpInPorts.size();
}

unsigned int IVdfnNode::GetInPortPrototypeCount() const {
  return (unsigned int)m_mpInPrototypes.size();
}

unsigned int IVdfnNode::GetOutPortCount() const {
  return (unsigned int)m_mpOutPorts.size();
}

bool IVdfnNode::GetIsValid() const {
  return (m_mpInPorts.size() == m_mpInPrototypes.size());
}

bool IVdfnNode::RegisterInPortPrototype(const std::string& sName, IVdfnPortTypeCompare* pComp) {
  // simply add/overwrite to map
  m_mpInPrototypes[sName] = pComp;
  return true;
}

IVdfnPortTypeCompare* IVdfnNode::CheckInPortRegistration(
    const std::string& sName, IVdfnPort* pPort) const {
  // make sure, we have that in-port as a name in prototype list

  TypeMap::const_iterator cit = m_mpInPrototypes.find(sName);
  if (cit == m_mpInPrototypes.end())
    return NULL; // no, refuse registration

  // yes, we do have an inport with that name
  // is the type ok?
  // query be prototype
  if ((*cit).second->IsTypeOf(pPort))
    return (*cit).second;
  return NULL;
}

bool IVdfnNode::GetIsValidInPort(const std::string& sPortName, IVdfnPort* pPort) const {
  return CheckInPortRegistration(sPortName, pPort) != NULL;
}

const IVdfnPortTypeCompare& IVdfnNode::GetPortTypeCompareFor(const std::string& sPortName) const {
  TypeMap::const_iterator cit = m_mpInPrototypes.find(sPortName);
  if (cit == m_mpInPrototypes.end())
    return SNoMatchCompare; // no, return a "no-match-in-any-case-compare"

  return *(*cit).second;
}

bool IVdfnNode::GetHasInPort(const std::string& strInPortName) const {
  TypeMap::const_iterator cit = m_mpInPrototypes.find(strInPortName);
  if (cit == m_mpInPrototypes.end())
    return false;

  return true;
}

bool IVdfnNode::PrepareEvaluationRun() {
  return true;
}

void IVdfnNode::SetUserTag(const std::string& strTag) {
  m_strUserTag = strTag;
}

bool IVdfnNode::GetUserTag(std::string& strTag) const {
  strTag = m_strUserTag;
  return GetHasUserTag();
}

bool IVdfnNode::GetHasUserTag() const {
  return !m_strUserTag.empty();
}

void IVdfnNode::SetTypeTag(const std::string& strTypeTag) {
  m_strTypeTag = strTypeTag;
}

std::string IVdfnNode::GetTypeTag() const {
  return m_strTypeTag;
}

void IVdfnNode::SetGroupTag(const std::string& strGroupTag) {
  m_strGroupTag = strGroupTag;
}

std::string IVdfnNode::GetGroupTag() const {
  return m_strGroupTag;
}

void IVdfnNode::PrintInfo(std::ostream& oStream) const {
  oStream << "============== NODE ===============\n"
          << "Name    : " << m_strNodeName << "\n"
          << "Type    : " << VistaConversion::GetTypeName(*this) << "\n"
          << "UserTag : " << m_strUserTag << "\n"
          << "TypeTag : " << m_strTypeTag << "\n"
          << "GroupTag: " << m_strGroupTag << "\n"
          << "------         Inports       ------\n";
  for (PortMap::const_iterator itInPort = m_mpInPorts.begin(); itInPort != m_mpInPorts.end();
       ++itInPort) {
    oStream << "Name: " << (*itInPort).first
            << "       Type: " << (*itInPort).second->GetTypeDescriptor() << '\n'
            << "Last Update: " << vstr::formattime((*itInPort).second->GetLastUpdate())
            << "       Update Count: " << (*itInPort).second->GetUpdateCounter() << '\n';
    VdfnPortFactory::CPortAccess* pAccess =
        VdfnPortFactory::GetSingleton()->GetPortAccess((*itInPort).second->GetTypeDescriptor());
    if (pAccess && pAccess->m_pStringGet != NULL) {
      oStream << "Value: " << pAccess->m_pStringGet->GetValueAsString((*itInPort).second) << '\n';
    } else {
      oStream << "Value: <no-string-getter>\n";
    }
  }
  oStream << "------        Outports       ------\n";
  for (PortMap::const_iterator itOutPort = m_mpOutPorts.begin(); itOutPort != m_mpOutPorts.end();
       ++itOutPort) {
    oStream << "Name: " << (*itOutPort).first
            << "       Type: " << (*itOutPort).second->GetTypeDescriptor() << '\n'
            << "Last Update: " << vstr::formattime((*itOutPort).second->GetLastUpdate())
            << "       Update Count: " << (*itOutPort).second->GetUpdateCounter() << '\n';
    VdfnPortFactory::CPortAccess* pAccess =
        VdfnPortFactory::GetSingleton()->GetPortAccess((*itOutPort).second->GetTypeDescriptor());
    if (pAccess && pAccess->m_pStringGet != NULL) {
      oStream << "Value: " << pAccess->m_pStringGet->GetValueAsString((*itOutPort).second) << '\n';
    } else {
      oStream << "Value: <no-string-getter>\n";
    }
  }
  oStream << "-----------------------------------\n";
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
