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

#include "VistaVirtualPlatformAdapter.h"
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaVirtualPlatformAdapter::VistaVirtualPlatformAdapter()
    : IVistaTransformable()
    , m_pPlatform(NULL) {
}

VistaVirtualPlatformAdapter::~VistaVirtualPlatformAdapter() {
}

VistaVirtualPlatform* VistaVirtualPlatformAdapter::GetVirtualPlatform() const {
  return m_pPlatform;
}

void VistaVirtualPlatformAdapter::SetVirtualPlatform(VistaVirtualPlatform* pPlatform) {
  m_pPlatform = pPlatform;
}

// #######################################################################
// TRANSFORMABLE API
// #######################################################################
bool VistaVirtualPlatformAdapter::SetTranslation(const VistaVector3D& v3Translation) {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->SetTranslation(v3Translation);
}
bool VistaVirtualPlatformAdapter::Translate(const VistaVector3D& v3Translation) {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->SetTranslation(v3Translation + m_pPlatform->GetTranslationConstRef());
}
bool VistaVirtualPlatformAdapter::SetRotation(const VistaQuaternion& qRotation) {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->SetRotation(qRotation);
}
bool VistaVirtualPlatformAdapter::Rotate(const VistaQuaternion& qRotation) {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->SetRotation(qRotation * m_pPlatform->GetRotationConstRef());
}
bool VistaVirtualPlatformAdapter::Scale(const VistaVector3D& v3Scale) {
  if (m_pPlatform == NULL)
    return false;
  if (v3Scale[0] != v3Scale[1] || v3Scale[0] != v3Scale[2])
    return false;

  return m_pPlatform->SetScale(m_pPlatform->GetScale() * v3Scale[0]);
}
bool VistaVirtualPlatformAdapter::SetScale(const VistaVector3D& v3Scale) {
  if (m_pPlatform == NULL)
    return false;
  if (v3Scale[0] != v3Scale[1] || v3Scale[0] != v3Scale[2])
    return false;

  return m_pPlatform->SetScale(v3Scale[0]);
}
bool VistaVirtualPlatformAdapter::SetTransform(const VistaTransformMatrix& matTransform) {
  if (m_pPlatform == NULL)
    return false;

  /** @todo: decomp yields strange transform */
  VistaVector3D   v3Translation;
  VistaQuaternion qOrientation;
  VistaVector3D   v3Scale;
  VistaQuaternion qScaleOrientation;
  matTransform.Decompose(v3Translation, qOrientation, v3Scale, qScaleOrientation);

  // our scaling has to be uniform
  // we don't have to check the scale orientation because of this
  if (std::abs(v3Scale[0] - v3Scale[1]) > 0.99999f || std::abs(v3Scale[0] - v3Scale[2]) > 0.99999f)
    return false;

  m_pPlatform->SetScale(v3Scale[0]);

  m_pPlatform->SetRotation(qOrientation);
  m_pPlatform->SetTranslation(v3Translation);

  // m_pPlatform->SetRotation( VistaQuaternion( matTransform ) );
  // m_pPlatform->SetScale( 1 );
  // m_pPlatform->SetTranslation( matTransform.GetTranslation() );

  return true;
}
bool VistaVirtualPlatformAdapter::Transform(const VistaTransformMatrix& matTransform) {
  if (m_pPlatform == NULL)
    return false;

  VistaVector3D   v3Translation;
  VistaQuaternion qOrientation;
  VistaVector3D   v3Scale;
  VistaQuaternion qScaleOrientation;
  matTransform.Decompose(v3Translation, qOrientation, v3Scale, qScaleOrientation);
  // we require that we have no shearing, just normal scaling
  if (1 - qScaleOrientation[Vista::W] > Vista::Epsilon)
    return false;
  // even further, our scaling has to be uniform
  if (v3Scale[0] != v3Scale[1] || v3Scale[0] != v3Scale[2])
    return false;

  m_pPlatform->SetRotation(qOrientation * m_pPlatform->GetRotationConstRef());
  m_pPlatform->SetScale(v3Scale[0] * m_pPlatform->GetScale());
  m_pPlatform->SetTranslation(v3Translation + m_pPlatform->GetTranslationConstRef());
  return true;
}

bool VistaVirtualPlatformAdapter::SetTranslation(const float fX, const float fY, const float fZ) {
  return SetTranslation(VistaVector3D(fX, fY, fZ));
}
bool VistaVirtualPlatformAdapter::SetTranslation(const float a3fTranslation[3]) {
  return SetTranslation(VistaVector3D(a3fTranslation));
}
bool VistaVirtualPlatformAdapter::SetTranslation(const double a3dTranslation[3]) {
  return SetTranslation(VistaVector3D(a3dTranslation));
}
bool VistaVirtualPlatformAdapter::Translate(const float fX, const float fY, const float fZ) {
  return Translate(VistaVector3D(fX, fY, fZ));
}
bool VistaVirtualPlatformAdapter::Translate(const float a3fTranslation[3]) {
  return Translate(VistaVector3D(a3fTranslation));
}
bool VistaVirtualPlatformAdapter::Translate(const double a3dTranslation[3]) {
  return Translate(VistaVector3D(a3dTranslation));
}
bool VistaVirtualPlatformAdapter::SetRotation(
    const float fX, const float fY, const float fZ, const float fW) {
  return SetRotation(VistaQuaternion(fX, fY, fZ, fW));
}
bool VistaVirtualPlatformAdapter::SetRotation(const float a4fRotation[4]) {
  return SetRotation(VistaQuaternion(a4fRotation));
}
bool VistaVirtualPlatformAdapter::SetRotation(const double a4dRotation[4]) {
  return SetRotation(VistaQuaternion(a4dRotation));
}
bool VistaVirtualPlatformAdapter::Rotate(
    const float fX, const float fY, const float fZ, const float fW) {
  return Rotate(VistaQuaternion(fX, fY, fZ, fW));
}
bool VistaVirtualPlatformAdapter::Rotate(const float a4fRotation[4]) {
  return Rotate(VistaQuaternion(a4fRotation));
}
bool VistaVirtualPlatformAdapter::Rotate(const double a4dRotation[4]) {
  return Rotate(VistaQuaternion(a4dRotation));
}
bool VistaVirtualPlatformAdapter::SetTransform(
    const float a16fTransform[16], const bool bColumnMajor) {
  return SetTransform(VistaTransformMatrix(a16fTransform, bColumnMajor));
}
bool VistaVirtualPlatformAdapter::SetTransform(
    const double a16dTransform[16], const bool bColumnMajor) {
  return SetTransform(VistaTransformMatrix(a16dTransform, bColumnMajor));
}
bool VistaVirtualPlatformAdapter::Transform(
    const float a16fTransform[16], const bool bColumnMajor) {
  return Transform(VistaTransformMatrix(a16fTransform, bColumnMajor));
}
bool VistaVirtualPlatformAdapter::Transform(
    const double a16dTransform[16], const bool bColumnMajor) {
  return Transform(VistaTransformMatrix(a16dTransform, bColumnMajor));
}
bool VistaVirtualPlatformAdapter::SetScale(const float fX, const float fY, const float fZ) {
  return SetScale(VistaVector3D(fX, fY, fZ));
}
bool VistaVirtualPlatformAdapter::SetScale(const float a3fScale[3]) {
  return SetScale(VistaVector3D(a3fScale));
}
bool VistaVirtualPlatformAdapter::SetScale(const double a3dScale[3]) {
  return SetScale(VistaVector3D(a3dScale));
}
bool VistaVirtualPlatformAdapter::Scale(const float fX, const float fY, const float fZ) {
  return Scale(VistaVector3D(fX, fY, fZ));
}
bool VistaVirtualPlatformAdapter::Scale(const float a3fScale[3]) {
  return Scale(VistaVector3D(a3fScale));
}
bool VistaVirtualPlatformAdapter::Scale(const double a3dScale[3]) {
  return Scale(VistaVector3D(a3dScale));
}

// #######################################################################
// LOCATABLE API
// #######################################################################
bool VistaVirtualPlatformAdapter::GetTranslation(VistaVector3D& v3Translation) const {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->GetTranslation(v3Translation);
}
bool VistaVirtualPlatformAdapter::GetWorldPosition(VistaVector3D& v3Position) const {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->GetTranslation(v3Position);
}
bool VistaVirtualPlatformAdapter::GetRotation(VistaQuaternion& qRotation) const {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->GetRotation(qRotation);
}
bool VistaVirtualPlatformAdapter::GetWorldOrientation(VistaQuaternion& qOrientation) const {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->GetRotation(qOrientation);
}
bool VistaVirtualPlatformAdapter::GetScale(VistaVector3D& v3Scale) const {
  if (m_pPlatform == NULL)
    return false;
  float fScale = m_pPlatform->GetScale();
  v3Scale[0]   = fScale;
  v3Scale[1]   = fScale;
  v3Scale[2]   = fScale;
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldScale(VistaVector3D& v3Scale) const {
  if (m_pPlatform == NULL)
    return false;
  float fScale = m_pPlatform->GetScale();
  v3Scale[0]   = fScale;
  v3Scale[1]   = fScale;
  v3Scale[2]   = fScale;
  return true;
}
bool VistaVirtualPlatformAdapter::GetTransform(VistaTransformMatrix& matTransform) const {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->GetMatrix(matTransform);
}
bool VistaVirtualPlatformAdapter::GetWorldTransform(VistaTransformMatrix& matTransform) const {
  if (m_pPlatform == NULL)
    return false;
  return m_pPlatform->GetMatrix(matTransform);
}
bool VistaVirtualPlatformAdapter::GetParentWorldTransform(
    VistaTransformMatrix& matTransform) const {
  return false;
}

// GENERIC IMPLEMENTATION

bool VistaVirtualPlatformAdapter::GetTranslation(float& fX, float& fY, float& fZ) const {
  VistaVector3D v3Result;
  if (GetTranslation(v3Result) == false)
    return false;
  fX = v3Result[Vista::X];
  fY = v3Result[Vista::Y];
  fZ = v3Result[Vista::Z];
  return false;
}
bool VistaVirtualPlatformAdapter::GetTranslation(float a3fTranslation[3]) const {
  VistaVector3D v3Result;
  if (GetTranslation(v3Result) == false)
    return false;
  v3Result.GetValues(a3fTranslation);
  return true;
}
bool VistaVirtualPlatformAdapter::GetTranslation(double a3dTranslation[3]) const {
  VistaVector3D v3Result;
  if (GetTranslation(v3Result) == false)
    return false;
  v3Result.GetValues(a3dTranslation);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldPosition(float& fX, float& fY, float& fZ) const {
  VistaVector3D v3Result;
  if (GetWorldPosition(v3Result) == false)
    return false;
  fX = v3Result[Vista::X];
  fY = v3Result[Vista::Y];
  fZ = v3Result[Vista::Z];
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldPosition(float a3fPosition[3]) const {
  VistaVector3D v3Result;
  if (GetWorldPosition(v3Result) == false)
    return false;
  v3Result.GetValues(a3fPosition);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldPosition(double a3dPosition[3]) const {
  VistaVector3D v3Result;
  if (GetWorldPosition(v3Result) == false)
    return false;
  v3Result.GetValues(a3dPosition);
  return true;
}
bool VistaVirtualPlatformAdapter::GetRotation(float& fX, float& fY, float& fZ, float& fW) const {
  VistaQuaternion qResult;
  if (GetRotation(qResult) == false)
    return false;
  fX = qResult[Vista::X];
  fY = qResult[Vista::Y];
  fZ = qResult[Vista::Z];
  fW = qResult[Vista::W];
  return true;
}
bool VistaVirtualPlatformAdapter::GetRotation(float a4fRotation[4]) const {
  VistaQuaternion qResult;
  if (GetRotation(qResult) == false)
    return false;
  qResult.GetValues(a4fRotation);
  return true;
}
bool VistaVirtualPlatformAdapter::GetRotation(double a4dRotation[4]) const {
  VistaQuaternion qResult;
  if (GetRotation(qResult) == false)
    return false;
  qResult.GetValues(a4dRotation);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldOrientation(
    float& fX, float& fY, float& fZ, float& fW) const {
  VistaQuaternion qResult;
  if (GetWorldOrientation(qResult) == false)
    return false;
  fX = qResult[Vista::X];
  fY = qResult[Vista::Y];
  fZ = qResult[Vista::Z];
  fW = qResult[Vista::W];
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldOrientation(float a4fOrientation[4]) const {
  VistaQuaternion qResult;
  if (GetWorldOrientation(qResult) == false)
    return false;
  qResult.GetValues(a4fOrientation);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldOrientation(double a4dOrientation[4]) const {
  VistaQuaternion qResult;
  if (GetWorldOrientation(qResult) == false)
    return false;
  qResult.GetValues(a4dOrientation);
  return true;
}
bool VistaVirtualPlatformAdapter::GetScale(float& fX, float& fY, float& fZ) const {
  VistaVector3D v3Result;
  if (GetScale(v3Result) == false)
    return false;
  fX = v3Result[Vista::X];
  fY = v3Result[Vista::Y];
  fZ = v3Result[Vista::Z];
  return true;
}
bool VistaVirtualPlatformAdapter::GetScale(float a3fScale[3]) const {
  VistaVector3D v3Result;
  if (GetScale(v3Result) == false)
    return false;
  v3Result.GetValues(a3fScale);
  return true;
}
bool VistaVirtualPlatformAdapter::GetScale(double a3dScale[4]) const {
  VistaVector3D v3Result;
  if (GetScale(v3Result) == false)
    return false;
  v3Result.GetValues(a3dScale);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldScale(float& fX, float& fY, float& fZ) const {
  VistaVector3D v3Result;
  if (GetWorldScale(v3Result) == false)
    return false;
  fX = v3Result[Vista::X];
  fY = v3Result[Vista::Y];
  fZ = v3Result[Vista::Z];
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldScale(float a3fScale[3]) const {
  VistaVector3D v3Result;
  if (GetWorldScale(v3Result) == false)
    return false;
  v3Result.GetValues(a3fScale);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldScale(double a3dScale[4]) const {
  VistaVector3D v3Result;
  if (GetWorldScale(v3Result) == false)
    return false;
  v3Result.GetValues(a3dScale);
  return true;
}
bool VistaVirtualPlatformAdapter::GetTransform(
    float a16fTransform[16], const bool bColumnMajor) const {
  VistaTransformMatrix matTransform;
  if (GetTransform(matTransform) == false)
    return false;
  if (bColumnMajor)
    matTransform.GetTransposedValues(a16fTransform);
  else
    matTransform.GetValues(a16fTransform);
  return true;
}
bool VistaVirtualPlatformAdapter::GetTransform(
    double a16dTransform[16], const bool bColumnMajor) const {
  VistaTransformMatrix matTransform;
  if (GetTransform(matTransform) == false)
    return false;
  if (bColumnMajor)
    matTransform.GetTransposedValues(a16dTransform);
  else
    matTransform.GetValues(a16dTransform);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldTransform(
    float a16fTransform[16], const bool bColumnMajor) const {
  VistaTransformMatrix matTransform;
  if (GetWorldTransform(matTransform) == false)
    return false;
  if (bColumnMajor)
    matTransform.GetTransposedValues(a16fTransform);
  else
    matTransform.GetValues(a16fTransform);
  return true;
}
bool VistaVirtualPlatformAdapter::GetWorldTransform(
    double a16dTransform[16], const bool bColumnMajor) const {
  VistaTransformMatrix matTransform;
  if (GetWorldTransform(matTransform) == false)
    return false;
  if (bColumnMajor)
    matTransform.GetTransposedValues(a16dTransform);
  else
    matTransform.GetValues(a16dTransform);
  return true;
}
bool VistaVirtualPlatformAdapter::GetParentWorldTransform(
    float a16fTransform[16], const bool bColumnMajor) const {
  VistaTransformMatrix matTransform;
  if (GetParentWorldTransform(matTransform) == false)
    return false;
  if (bColumnMajor)
    matTransform.GetTransposedValues(a16fTransform);
  else
    matTransform.GetValues(a16fTransform);
  return true;
}
bool VistaVirtualPlatformAdapter::GetParentWorldTransform(
    double a16dTransform[16], const bool bColumnMajor) const {
  VistaTransformMatrix matTransform;
  if (GetParentWorldTransform(matTransform) == false)
    return false;
  if (bColumnMajor)
    matTransform.GetTransposedValues(a16dTransform);
  else
    matTransform.GetValues(a16dTransform);
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
