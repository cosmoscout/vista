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

#include "VistaDisplaySystem.h"
#include "VistaDisplayBridge.h"
#include "VistaDisplayManager.h"
#include "VistaViewport.h"
#include "VistaVirtualPlatform.h"
#include <VistaAspects/VistaAspectsUtils.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaDisplaySystem::VistaDisplaySystem(VistaDisplayManager* pDisplayManager,
    VistaVirtualPlatform* pPlatform, IVistaDisplayEntityData* pData, IVistaDisplayBridge* pBridge)
    : VistaDisplayEntity(pData, pBridge)
    , IVistaObserver()
    , m_pDisplayManager(pDisplayManager)
    , m_pPlatform(pPlatform) {
}

VistaDisplaySystem::~VistaDisplaySystem() {
  // release observeables...
  std::list<IVistaObserveable*>           liObs = m_liObserveables;
  std::list<IVistaObserveable*>::iterator it;
  for (it = liObs.begin(); it != liObs.end(); ++it) {
    ReleaseObserveable(*it, IVistaObserveable::TICKET_NONE);
  }

  m_liObserveables.clear();
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetReferenceFrameName                                       */
/*                                                                            */
/*============================================================================*/
std::string VistaDisplaySystem::GetReferenceFrameName() const {
  string sReferenceFrameName;

  if (m_pPlatform) {
    sReferenceFrameName = m_pPlatform->GetNameForNameable();
  }
  return sReferenceFrameName;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetReferenceFrame                                           */
/*                                                                            */
/*============================================================================*/
VistaVirtualPlatform* VistaDisplaySystem::GetReferenceFrame() const {
  return m_pPlatform;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNumberOfViewports                                        */
/*                                                                            */
/*============================================================================*/
unsigned int VistaDisplaySystem::GetNumberOfViewports() const {
  return (unsigned int)m_vecViewports.size();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportNames                                            */
/*                                                                            */
/*============================================================================*/
std::list<std::string> VistaDisplaySystem::GetViewportNames() const {
  list<string> liNames;

  for (int i = 0; i < (int)m_vecViewports.size(); ++i) {
    liNames.push_back(m_vecViewports[i]->GetNameForNameable());
  }
  return liNames;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewport                                                 */
/*                                                                            */
/*============================================================================*/
VistaViewport* VistaDisplaySystem::GetViewport(unsigned int iIndex) const {
  if (iIndex < m_vecViewports.size())
    return m_vecViewports[iIndex];
  return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewports                                                */
/*                                                                            */
/*============================================================================*/
std::vector<VistaViewport*>& VistaDisplaySystem::GetViewports() {
  return m_vecViewports;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaDisplaySystem::Debug(std::ostream& out) const {
  // VistaDisplayEntity::Debug(out);

  out << vstr::indent << "[VistaDisplaySystem] - Name                   : " << GetNameForNameable()
      << std::endl;

  VistaVector3D v3Temp, v3Temp2;
  GetDisplaySystemProperties()->GetViewerPosition(v3Temp);
  out << vstr::indent << "[VistaDisplaySystem] - viewer position        : " << v3Temp << std::endl;

  VistaQuaternion qTemp;
  GetDisplaySystemProperties()->GetViewerOrientation(qTemp);
  out << vstr::indent << "[VistaDisplaySystem] - viewer orientation     : " << qTemp << std::endl;

  GetDisplaySystemProperties()->GetEyeOffsets(v3Temp, v3Temp2);
  out << vstr::indent << "[VistaDisplaySystem] - left eye offset        : " << v3Temp << std::endl;
  out << vstr::indent << "[VistaDisplaySystem] - right eye offset       : " << v3Temp2 << std::endl;

  out << vstr::indent << "[VistaDisplaySystem] - local viewer           : "
      << (GetDisplaySystemProperties()->GetLocalViewer() ? "true" : "false") << std::endl;
  out << vstr::indent << "[VistaDisplaySystem] - HMD mode               : "
      << (GetDisplaySystemProperties()->GetHMDModeActive() ? "true" : "false") << std::endl;

  if (m_vecViewports.empty()) {
    out << vstr::indent << "[VistaDisplaySystem] - viewports              : *none*" << std::endl;
  } else {
    out << vstr::indent
        << "[VistaDisplaySystem] - viewports              : " << m_vecViewports.size() << std::endl;
    out << vstr::indent << "[VistaDisplaySystem] - viewport names         : ";
    unsigned int i;
    for (i = 0; i < m_vecViewports.size(); ++i) {
      if (i > 0)
        out << ", ";
      out << m_vecViewports[i]->GetNameForNameable();
    }
    out << std::endl;
  }

  if (m_pPlatform) {
    out << vstr::indent
        << "[VistaDisplaySystem] - reference frame name   : " << m_pPlatform->GetNameForNameable()
        << std::endl;
    out << vstr::indent
        << "[VistaDisplaySystem] - ref-frame translation  : " << m_pPlatform->GetTranslation()
        << std::endl;
    out << vstr::indent
        << "[VistaDisplaySystem] - ref-frame rotation     : " << m_pPlatform->GetRotation()
        << std::endl;
    out << vstr::indent
        << "[VistaDisplaySystem] - ref-frame scale        : " << m_pPlatform->GetScale()
        << std::endl;
  } else {
    out << vstr::indent << "[VistaDisplaySystem] - ReferenceFrame name    : *none*" << std::endl;
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayManager                                           */
/*                                                                            */
/*============================================================================*/
VistaDisplayManager* VistaDisplaySystem::GetDisplayManager() const {
  return m_pDisplayManager;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ObserveableDeleteRequest                                    */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::ObserveableDeleteRequest(IVistaObserveable* pObserveable, int) {
  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ObserveableDelete                                           */
/*                                                                            */
/*============================================================================*/
void VistaDisplaySystem::ObserveableDelete(IVistaObserveable* pObserveable, int) {
  m_liObserveables.remove(pObserveable);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ReleaseObserveable                                          */
/*                                                                            */
/*============================================================================*/
void VistaDisplaySystem::ReleaseObserveable(IVistaObserveable* pObserveable, int) {
  if (Observes(pObserveable)) {
    pObserveable->DetachObserver(this);
    m_liObserveables.remove(pObserveable);
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ObserverUpdate                                              */
/*                                                                            */
/*============================================================================*/
void VistaDisplaySystem::ObserverUpdate(IVistaObserveable* pObserveable, int msg, int ticket) {
  // We let the implementation-specific bridge decide how to react
  // on notifications. In this case, we can be pretty sure, that the
  // reference frame (i.e. the viewing platform) has changed...
  GetDisplayBridge()->ObserverUpdate(pObserveable, msg, ticket, this);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Observes                                                    */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::Observes(IVistaObserveable* pObserveable) {
  std::list<IVistaObserveable*>::iterator it;
  for (it = m_liObserveables.begin(); it != m_liObserveables.end(); ++it) {
    if ((*it) == pObserveable)
      break;
  }

  return it != m_liObserveables.end();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Observe                                                     */
/*                                                                            */
/*============================================================================*/
void VistaDisplaySystem::Observe(IVistaObserveable* pObserveable, int eTicket) {
  if (!pObserveable)
    return;

  if (!Observes(pObserveable)) {
    if (pObserveable->AttachObserver(this, eTicket))
      m_liObserveables.push_back(pObserveable);
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProperties                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplayEntity::IVistaDisplayEntityProperties* VistaDisplaySystem::CreateProperties() {
  return new VistaDisplaySystemProperties(this, GetDisplayBridge());
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystemProperties                                  */
/*                                                                            */
/*============================================================================*/
VistaDisplaySystem::VistaDisplaySystemProperties*
VistaDisplaySystem::GetDisplaySystemProperties() const {
  return static_cast<VistaDisplaySystemProperties*>(GetProperties());
}

namespace {
const std::string sSReflectionTypeVDS("VistaDisplaySystem");

IVistaPropertyGetFunctor* aCgFunctorsVDS[] = {
    new TVistaProperty3RefGet<float, VistaDisplaySystem::VistaDisplaySystemProperties,
        VistaProperty::PROPT_DOUBLE>("VIEWER_POSITION", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerPosition),
    new TVistaProperty4RefGet<float, VistaDisplaySystem::VistaDisplaySystemProperties,
        VistaProperty::PROPT_DOUBLE>("VIEWER_ORIENTATION", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerOrientation),
    new TVistaProperty3RefGet<float, VistaDisplaySystem::VistaDisplaySystemProperties,
        VistaProperty::PROPT_DOUBLE>("LEFT_EYE_OFFSET", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::GetLeftEyeOffset),
    new TVistaProperty3RefGet<float, VistaDisplaySystem::VistaDisplaySystemProperties,
        VistaProperty::PROPT_DOUBLE>("RIGHT_EYE_OFFSET", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::GetRightEyeOffset),
    new TVistaPropertyGet<bool, VistaDisplaySystem::VistaDisplaySystemProperties,
        VistaProperty::PROPT_BOOL>("LOCAL_VIEWER", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::GetLocalViewer),
    new TVistaPropertyGet<bool, VistaDisplaySystem::VistaDisplaySystemProperties,
        VistaProperty::PROPT_BOOL>("HMD_MODE", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::GetHMDModeActive),
    new TVistaDisplayEntityParentPropertyGet<std::string, VistaDisplaySystem,
        VistaProperty::PROPT_STRING>(
        "REFERENCE_FRAME_NAME", sSReflectionTypeVDS, &VistaDisplaySystem::GetReferenceFrameName),
    new TVistaDisplayEntityParentPropertyGet<unsigned int, VistaDisplaySystem,
        VistaProperty::PROPT_DOUBLE>(
        "NUMBER_OF_VIEWPORTS", sSReflectionTypeVDS, &VistaDisplaySystem::GetNumberOfViewports),
    new TVistaDisplayEntityParentPropertyGet<std::list<std::string>, VistaDisplaySystem,
        VistaProperty::PROPT_LIST>(
        "VIEWPORT_NAMES", sSReflectionTypeVDS, &VistaDisplaySystem::GetViewportNames),
    NULL};

IVistaPropertySetFunctor* aCsFunctorsVDS[] = {
    new TVistaPropertySet<const VistaVector3D&, VistaVector3D,
        VistaDisplaySystem::VistaDisplaySystemProperties>("VIEWER_POSITION", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::SetViewerPosition),
    new TVistaPropertySet<const VistaQuaternion&, VistaQuaternion,
        VistaDisplaySystem::VistaDisplaySystemProperties>("VIEWER_ORIENTATION", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::SetViewerOrientation),
    new TVistaProperty3ValSet<float, VistaDisplaySystem::VistaDisplaySystemProperties>(
        "LEFT_EYE_OFFSET", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::SetLeftEyeOffset),
    new TVistaProperty3ValSet<float, VistaDisplaySystem::VistaDisplaySystemProperties>(
        "RIGHT_EYE_OFFSET", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::SetRightEyeOffset),
    new TVistaPropertySet<bool, bool, VistaDisplaySystem::VistaDisplaySystemProperties>(
        "LOCAL_VIEWER", sSReflectionTypeVDS,
        &VistaDisplaySystem::VistaDisplaySystemProperties::SetLocalViewer),
    new TVistaPropertySet<bool, bool, VistaDisplaySystem::VistaDisplaySystemProperties>("HMD_MODE",
        sSReflectionTypeVDS, &VistaDisplaySystem::VistaDisplaySystemProperties::SetHMDModeActive),
    new TVistaPropertySet<const string&, string, VistaDisplaySystem::VistaDisplaySystemProperties>(
        "NAME", sSReflectionTypeVDS, &VistaDisplaySystem::VistaDisplaySystemProperties::SetName),
    NULL};
} // namespace

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetName                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::SetName(const std::string& sName) {
  VistaDisplaySystem* pDS        = static_cast<VistaDisplaySystem*>(GetParent());
  string              strOldName = pDS->GetNameForNameable();

  if (strOldName != "") {
    if (!pDS->GetDisplayManager()->RenameDisplaySystem(strOldName, sName))
      return false;
  }
  return IVistaDisplayEntityProperties::SetName(sName);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetViewerPosition                                       */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerPosition(
    float& x, float& y, float& z) const {
  VistaVector3D v3VPos;

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetViewerPosition(v3VPos, pDS);

  x = v3VPos[0];
  y = v3VPos[1];
  z = v3VPos[2];

  return true;
}

bool VistaDisplaySystem::VistaDisplaySystemProperties::SetViewerPosition(
    const float x, const float y, const float z) {
  VistaVector3D v3CurVPos, v3SetVPos(x, y, z);

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetViewerPosition(v3CurVPos, pDS);

  if (v3CurVPos == v3SetVPos)
    return false;
  else {
    GetDisplayBridge()->SetViewerPosition(v3SetVPos, pDS);
    Notify(MSG_VIEWER_POSITION_CHANGE);
    return true;
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetViewerOrientation                                    */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerOrientation(
    float& x, float& y, float& z, float& w) const {
  VistaQuaternion q4VOri;

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetViewerOrientation(q4VOri, pDS);

  x = q4VOri[0];
  y = q4VOri[1];
  z = q4VOri[2];
  w = q4VOri[3];

  return true;
}

bool VistaDisplaySystem::VistaDisplaySystemProperties::SetViewerOrientation(
    const float x, const float y, const float z, const float w) {
  VistaQuaternion q4CurVOri, q4SetVOri(x, y, z, w);

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetViewerOrientation(q4CurVOri, pDS);

  if (q4CurVOri == q4SetVOri)
    return false;
  else {
    GetDisplayBridge()->SetViewerOrientation(q4SetVOri, pDS);
    Notify(MSG_VIEWER_ORIENTATION_CHANGE);
    return true;
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetLeftEyeOffset                                        */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::GetLeftEyeOffset(
    float& x, float& y, float& z) const {
  VistaVector3D v3Left, v3Right;

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetEyeOffsets(v3Left, v3Right, pDS);

  x = v3Left[0];
  y = v3Left[1];
  z = v3Left[2];

  return true;
}

bool VistaDisplaySystem::VistaDisplaySystemProperties::SetLeftEyeOffset(
    const float x, const float y, const float z) {
  VistaVector3D v3CurLEOffset, v3CurREOffset, v3SetLEOffset(x, y, z);

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetEyeOffsets(v3CurLEOffset, v3CurREOffset, pDS);

  if (v3CurLEOffset == v3SetLEOffset)
    return false;
  else {
    GetDisplayBridge()->SetEyeOffsets(v3SetLEOffset, v3CurREOffset, pDS);
    Notify(MSG_LEFT_EYE_OFFSET_CHANGE);
    return true;
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetRightEyeOffset                                       */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::GetRightEyeOffset(
    float& x, float& y, float& z) const {
  VistaVector3D v3Left, v3Right;

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetEyeOffsets(v3Left, v3Right, pDS);

  x = v3Right[0];
  y = v3Right[1];
  z = v3Right[2];

  return true;
}

bool VistaDisplaySystem::VistaDisplaySystemProperties::SetRightEyeOffset(
    const float x, const float y, const float z) {
  VistaVector3D v3CurLEOffset, v3CurREOffset, v3SetREOffset(x, y, z);

  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetEyeOffsets(v3CurLEOffset, v3CurREOffset, pDS);

  if (v3CurREOffset == v3SetREOffset)
    return false;
  else {
    GetDisplayBridge()->SetEyeOffsets(v3CurLEOffset, v3SetREOffset, pDS);
    Notify(MSG_RIGHT_EYE_OFFSET_CHANGE);
    return true;
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetLocalViewer                                          */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::GetLocalViewer() const {
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  return GetDisplayBridge()->GetLocalViewer(pDS);
}

bool VistaDisplaySystem::VistaDisplaySystemProperties::SetLocalViewer(bool bLocalViewer) {
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  if (GetDisplayBridge()->GetLocalViewer(pDS) == bLocalViewer)
    return false;
  else {
    GetDisplayBridge()->SetLocalViewer(bLocalViewer, pDS);
    Notify(MSG_LOCAL_VIEWER_CHANGE);
    return true;
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetHMDModeActive                                        */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::GetHMDModeActive() const {
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  return GetDisplayBridge()->GetHMDModeActive(pDS);
}

bool VistaDisplaySystem::VistaDisplaySystemProperties::SetHMDModeActive(bool bSet) {
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  if (GetDisplayBridge()->GetHMDModeActive(pDS) == bSet)
    return false;
  else {
    GetDisplayBridge()->SetHMDModeActive(bSet, pDS);
    Notify(MSG_HMDMODE_CHANGE);
    return true;
  }
}

/*============================================================================*/
/*  CONVENIENCE API BEGIN                                                     */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetViewerPosition                                       */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::SetViewerPosition(
    const VistaVector3D& v3Pos) {
  VistaVector3D       v3CurPos;
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());

  GetDisplayBridge()->GetViewerPosition(v3CurPos, pDS);
  if (v3CurPos == v3Pos) {
    return false;
  } else {
    GetDisplayBridge()->SetViewerPosition(v3Pos, pDS);
    Notify(MSG_VIEWER_POSITION_CHANGE);
    return true;
  }
}

void VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerPosition(
    VistaVector3D& v3Pos) const {
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetViewerPosition(v3Pos, pDS);
}

VistaVector3D VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerPosition() const {
  VistaVector3D       v3Pos;
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());

  GetDisplayBridge()->GetViewerPosition(v3Pos, pDS);
  return v3Pos;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetViewerOrientation                                    */
/*                                                                            */
/*============================================================================*/
bool VistaDisplaySystem::VistaDisplaySystemProperties::SetViewerOrientation(
    const VistaQuaternion& qOrient) {
  VistaQuaternion     qCurOrient;
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());

  GetDisplayBridge()->GetViewerOrientation(qCurOrient, pDS);
  if (qCurOrient == qOrient) {
    return false;
  } else {
    GetDisplayBridge()->SetViewerOrientation(qOrient, pDS);
    Notify(MSG_VIEWER_ORIENTATION_CHANGE);
    return true;
  }
}

void VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerOrientation(
    VistaQuaternion& qOrient) const {
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetViewerOrientation(qOrient, pDS);
}

VistaQuaternion VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerOrientation() const {
  VistaQuaternion     qOrient;
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());

  GetDisplayBridge()->GetViewerOrientation(qOrient, pDS);
  return qOrient;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewerViewDirection                                      */
/*                                                                            */
/*============================================================================*/
void VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerViewDirection(
    VistaVector3D& v3Dir) const {
  VistaQuaternion     q;
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetViewerOrientation(q, pDS);

  // with the current orientation, rotate the camera-view direction
  // and interpret the resulting quat as a direction vector
  v3Dir = q.Rotate(VistaVector3D(0, 0, -1));
}

VistaVector3D VistaDisplaySystem::VistaDisplaySystemProperties::GetViewerViewDirection() const {
  VistaVector3D v3Dir;
  GetViewerViewDirection(v3Dir);
  return v3Dir;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetEyeOffsets                                           */
/*                                                                            */
/*============================================================================*/
void VistaDisplaySystem::VistaDisplaySystemProperties::GetEyeOffsets(
    VistaVector3D& v3LeftOffset, VistaVector3D& v3RightOffset) const {
  VistaDisplaySystem* pDS = static_cast<VistaDisplaySystem*>(GetParent());
  GetDisplayBridge()->GetEyeOffsets(v3LeftOffset, v3RightOffset, pDS);
}

void VistaDisplaySystem::VistaDisplaySystemProperties::SetEyeOffsets(
    const VistaVector3D& v3LeftOffset, const VistaVector3D& v3RightOffset) {
  SetLeftEyeOffset(v3LeftOffset[0], v3LeftOffset[1], v3LeftOffset[2]);
  SetRightEyeOffset(v3RightOffset[0], v3RightOffset[1], v3RightOffset[2]);
}

/*============================================================================*/
/*  CONVENIENCE API END                                                       */
/*============================================================================*/

string VistaDisplaySystem::VistaDisplaySystemProperties::GetReflectionableType() const {
  return sSReflectionTypeVDS;
}

int VistaDisplaySystem::VistaDisplaySystemProperties::AddToBaseTypeList(
    list<string>& rBtList) const {
  int nSize = IVistaDisplayEntityProperties::AddToBaseTypeList(rBtList);
  rBtList.push_back(sSReflectionTypeVDS);
  return nSize + 1;
}

VistaDisplaySystem::VistaDisplaySystemProperties::~VistaDisplaySystemProperties() {
}

VistaDisplaySystem::VistaDisplaySystemProperties::VistaDisplaySystemProperties(
    VistaDisplaySystem* pParent, IVistaDisplayBridge* pDisplayBridge)
    : IVistaDisplayEntityProperties(pParent, pDisplayBridge) {
}

/*============================================================================*/
/*  LOCAL VARS / FUNCTIONS                                                    */
/*============================================================================*/
