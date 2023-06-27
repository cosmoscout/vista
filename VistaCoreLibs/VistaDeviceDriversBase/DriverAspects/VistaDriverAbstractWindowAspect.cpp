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

#include "VistaDriverAbstractWindowAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"

#include <algorithm>
#include <cassert>

namespace {
template <class Pred>
class _find_by_id
    : public std::unary_function<const VistaDriverAbstractWindowAspect::IWindowHandle&, bool> {
 public:
  _find_by_id(Pred id)
      : m_Id(id) {
  }

  bool operator()(const VistaDriverAbstractWindowAspect::IWindowHandle& handle) const {
    if ((Pred)handle == m_Id)
      return true;
    return false;
  }

  Pred m_Id;
};
} // namespace

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverAbstractWindowAspect::m_nAspectId = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverAbstractWindowAspect::VistaDriverAbstractWindowAspect()
    : IVistaDeviceDriver::IVistaDeviceDriverAspect()
    , m_pTouchSeq(NULL) {
  if (VistaDriverAbstractWindowAspect::GetAspectId() == -1) // unregistered
    VistaDriverAbstractWindowAspect::SetAspectId(
        VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("WINDOW"));

  SetId(VistaDriverAbstractWindowAspect::GetAspectId());
}

VistaDriverAbstractWindowAspect::~VistaDriverAbstractWindowAspect() {
  delete m_pTouchSeq;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDriverAbstractWindowAspect::AttachToWindow(IWindowHandle* oWindow) {
  if (!m_pTouchSeq)
    return false;

  return (*m_pTouchSeq).AttachSequence(oWindow);
}

bool VistaDriverAbstractWindowAspect::DetachFromWindow(IWindowHandle* oWindow) {
  if (!m_pTouchSeq)
    return false;
  return (*m_pTouchSeq).DetachSequence(oWindow);
}

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int VistaDriverAbstractWindowAspect::GetAspectId() {
  return VistaDriverAbstractWindowAspect::m_nAspectId;
}

void VistaDriverAbstractWindowAspect::SetAspectId(int nId) {
  assert(VistaDriverAbstractWindowAspect::m_nAspectId == -1);
  VistaDriverAbstractWindowAspect::m_nAspectId = nId;
}

bool VistaDriverAbstractWindowAspect::SetTouchSequence(
    IVistaDriverAbstractWindowTouchSequence* pAtSeq) {
  m_pTouchSeq = pAtSeq;
  return true;
}

VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence*
VistaDriverAbstractWindowAspect::GetTouchSequence() const {
  return m_pTouchSeq;
}

std::list<VistaDriverAbstractWindowAspect::IWindowHandle*>
VistaDriverAbstractWindowAspect::GetWindowList() const {
  if (m_pTouchSeq)
    return (*m_pTouchSeq).GetWindowList();

  return std::list<IWindowHandle*>();
}

// ##########################################################################################

VistaDriverAbstractWindowAspect::WindowLookup::WindowLookup() {
}

VistaDriverAbstractWindowAspect::WindowLookup::~WindowLookup() {
}

// VistaDriverAbstractWindowAspect::IWindowHandle
// *VistaDriverAbstractWindowAspect::WindowLookup::GetWindowById(int id) const
//{
//	std::set<IWindowHandle*>::const_iterator it = std::find_if( m_windows.begin(),
//m_windows.end(), _find_by_id<int>( id ) ); 	if( it == m_windows.end() )
//	{
//		static NativeWindowHandle null_window(0,0);
//
//		return &null_window;
//	}
//	return (*it);
//}
//
// VistaDriverAbstractWindowAspect::IWindowHandle
// VistaDriverAbstractWindowAspect::WindowLookup::GetWindowByHandle( 		IWindowHandle::OSHANDLE handle)
//const
//{
//	std::set<IWindowHandle>::const_iterator it = std::find_if( m_windows.begin(),
//m_windows.end(), _find_by_id<IWindowHandle::OSHANDLE>( handle ) ); 	if( it == m_windows.end() )
//		return IWindowHandle(0, NULL);
//	return (*it);
//}

void VistaDriverAbstractWindowAspect::WindowLookup::RegisterWindow(IWindowHandle* window) {
  m_windows.insert(window);
}

bool VistaDriverAbstractWindowAspect::WindowLookup::UnregisterWindow(IWindowHandle* window) {
  std::set<IWindowHandle*>::iterator it = std::find(m_windows.begin(), m_windows.end(), window);
  if (it == m_windows.end())
    return false;

  m_windows.erase(it);
  return true;
}

VistaDriverAbstractWindowAspect::WindowLookupCollection::WindowLookupCollection() {
}

const VistaDriverAbstractWindowAspect::WindowLookup*
VistaDriverAbstractWindowAspect::WindowLookupCollection::GetWindowLookupBySystemTag(
    const std::string& tag) const {
  std::map<std::string, WindowLookup*>::const_iterator it = m_tagMap.find(tag);
  if (it == m_tagMap.end())
    return NULL;
  return (*it).second;
}

bool VistaDriverAbstractWindowAspect::WindowLookupCollection::RegisterWindowLookup(
    const std::string& tag, WindowLookup* lookup) {
  m_tagMap[tag] = lookup;
  return true;
}

VistaDriverAbstractWindowAspect::WindowLookup*
VistaDriverAbstractWindowAspect::WindowLookupCollection::UnregisterWindowLookup(
    const std::string& tag) {
  std::map<std::string, WindowLookup*>::iterator it = m_tagMap.find(tag);
  if (it == m_tagMap.end())
    return NULL;

  WindowLookup* l = (*it).second;

  m_tagMap.erase(it);
  return l;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
