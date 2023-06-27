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

#include "VistaVirtualPlatform.h"
#include <VistaAspects/VistaAspectsUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaVirtualPlatform::VistaVirtualPlatform()
    : VistaReferenceFrame()
    , IVistaObserveable() {
}

VistaVirtualPlatform::~VistaVirtualPlatform() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetTranslation                                              */
/*                                                                            */
/*============================================================================*/
bool VistaVirtualPlatform::SetTranslation(const VistaVector3D& refTranslation) {
  if (VistaReferenceFrame::SetTranslation(refTranslation)) {
    Notify(MSG_TRANSLATION_CHANGE);
    return true;
  }
  return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetRotation                                                 */
/*                                                                            */
/*============================================================================*/
bool VistaVirtualPlatform::SetRotation(const VistaQuaternion& refRotation) {
  if (VistaReferenceFrame::SetRotation(refRotation)) {
    Notify(MSG_ROTATION_CHANGE);
    return true;
  }
  return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetScale                                                    */
/*                                                                            */
/*============================================================================*/
bool VistaVirtualPlatform::SetScale(float fScale) {
  if (VistaReferenceFrame::SetScale(fScale)) {
    Notify(MSG_SCALE_CHANGE);
    return true;
  }
  return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNameForNameable                                          */
/*                                                                            */
/*============================================================================*/
std::string VistaVirtualPlatform::GetNameForNameable() const {
  return m_strNameForNameable;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetNameForNameable                                          */
/*                                                                            */
/*============================================================================*/
void VistaVirtualPlatform::SetNameForNameable(const string& sNewName) {
  if (VistaAspectsComparisonStuff::StringEquals(sNewName, m_strNameForNameable, true) == false) {
    m_strNameForNameable = sNewName;
    Notify(MSG_NAME_CHANGE);
  }
}
