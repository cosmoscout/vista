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

#include "VdfnNodeFactory.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace {
VdfnNodeFactory* SSingleton = NULL;
}

VdfnNodeFactory* VdfnNodeFactory::GetSingleton() {
  if (SSingleton == NULL)
    SSingleton = new VdfnNodeFactory;

  return SSingleton;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnNodeFactory::VdfnNodeFactory() {
}

VdfnNodeFactory::~VdfnNodeFactory() {
  for (CRMAP::iterator it = m_mpCreators.begin(); it != m_mpCreators.end(); ++it) {
    delete (*it).second;
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVdfnNode* VdfnNodeFactory::CreateNode(
    const std::string& strTypeSymbol, const VistaPropertyList& oParams) const {
  CRMAP::const_iterator it = m_mpCreators.find(strTypeSymbol);
  if (it == m_mpCreators.end())
    return NULL;

  return (*it).second->CreateNode(oParams);
}

bool VdfnNodeFactory::SetNodeCreator(const std::string& strTypeSymbol, IVdfnNodeCreator* pCreator) {
  m_mpCreators[strTypeSymbol] = pCreator;
  return true;
}

bool VdfnNodeFactory::UnSetNodeCreator(const std::string& strTypeSymbol) {
  CRMAP::iterator it = m_mpCreators.find(strTypeSymbol);
  if (it != m_mpCreators.end()) {
    delete (*it).second;
    m_mpCreators.erase(it);
    return true;
  }

  return false;
}

std::list<std::string> VdfnNodeFactory::GetNodeCreators() const {
  std::list<std::string> liRet;
  for (CRMAP::const_iterator cit = m_mpCreators.begin(); cit != m_mpCreators.end(); ++cit)
    liRet.push_back((*cit).first);
  return liRet;
}

bool VdfnNodeFactory::GetHasCreator(const std::string& strCreatorName) const {
  return (m_mpCreators.find(strCreatorName) != m_mpCreators.end());
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
