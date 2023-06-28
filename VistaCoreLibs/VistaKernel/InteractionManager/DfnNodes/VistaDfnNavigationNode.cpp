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
#include <cmath>
#include <iomanip>
#include <iostream>

#include "VistaDfnNavigationNode.h"

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnNavigationNode::VistaDfnNavigationNode(const int iDefaultNavigationMode,
    const float fDefaultLinearSpeed, const float fDefaultAngularSpeed,
    const float fDefaultLinearAcceleration, const float fDefaultLinearDeceleration,
    const float fDefaultAngularAcceleration, const float fDefaultAngularDeceleration)
    : IVdfnNode()
    , m_pTransform(NULL)
    , m_pOut(new TVdfnPort<VistaTransformMatrix>)
    , m_pTranslation(NULL)
    , m_pRotation(NULL)
    , m_pRotationPivot(NULL)
    , m_pDeltaTime(NULL)
    , m_pNavigationMode(NULL)
    , m_iNavigationMode(iDefaultNavigationMode)
    , m_fDefaultLinearSpeed(fDefaultLinearSpeed)
    , m_fLinearAcceleration(fDefaultLinearAcceleration)
    , m_fLinearDeceleration(fDefaultLinearDeceleration)
    , m_fTargetAngularSpeed(fDefaultAngularSpeed)
    , m_fAngularAcceleration(fDefaultAngularAcceleration)
    , m_fAngularDecceleration(fDefaultAngularDeceleration)
    , m_nLastDTUpateCount(0)
    , m_fCurrentAngularSpeed(0)
    , m_fDeltaTime(0) {
  // in-ports:
  // - navigation mode (heli, object in hand, ...) : int
  // - translation : VistaVector3D
  // - rotation : VistaQuaternion
  // - linear_velocity: float
  // - angular_velocity: float
  RegisterInPortPrototype("dt", new TVdfnPortTypeCompare<TVdfnPort<double>>);
  RegisterInPortPrototype("navigation_mode", new TVdfnPortTypeCompare<TVdfnPort<int>>);
  RegisterInPortPrototype("translation", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("rotation", new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion>>);
  RegisterInPortPrototype("pivot", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("transform", new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix>>);
  // correct term is speed, but velocity variants are still in here for backward compability
  RegisterInPortPrototype("linear_speed", new TVdfnPortTypeCompare<TVdfnPort<float>>);
  RegisterInPortPrototype("angular_speed", new TVdfnPortTypeCompare<TVdfnPort<float>>);
  RegisterInPortPrototype("linear_velocity", new TVdfnPortTypeCompare<TVdfnPort<float>>);
  RegisterInPortPrototype("angular_velocity", new TVdfnPortTypeCompare<TVdfnPort<float>>);

  // out-ports:
  // - transformation : VistaTransformationMatrix
  RegisterOutPort("transform", m_pOut);
}

VistaDfnNavigationNode::~VistaDfnNavigationNode() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaDfnNavigationNode::PrepareEvaluationRun() {
  m_pTransform      = dynamic_cast<TVdfnPort<VistaTransformMatrix>*>(GetInPort("transform"));
  m_pTranslation    = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("translation"));
  m_pRotation       = dynamic_cast<TVdfnPort<VistaQuaternion>*>(GetInPort("rotation"));
  m_pRotationPivot  = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("pivot"));
  m_pDeltaTime      = dynamic_cast<TVdfnPort<double>*>(GetInPort("dt"));
  m_pNavigationMode = dynamic_cast<TVdfnPort<int>*>(GetInPort("navigation_mode"));

  // correct term is speed, but velocity variants are still in here for backward compability
  m_pLinearSpeed = dynamic_cast<TVdfnPort<float>*>(GetInPort("linear_speed"));
  if (m_pLinearSpeed == NULL) {
    m_pLinearSpeed = dynamic_cast<TVdfnPort<float>*>(GetInPort("linear_velocity"));
    if (m_pLinearSpeed) {
      vstr::warn() << "[VistaDfnNavigationNode]: port name \"linear_velocity\" is deprecated "
                      "- use \"linear_speed\" instead"
                   << std::endl;
    }
  }
  m_pAngularSpeed = dynamic_cast<TVdfnPort<float>*>(GetInPort("angular_speed"));
  if (m_pAngularSpeed == NULL) {
    m_pAngularSpeed = dynamic_cast<TVdfnPort<float>*>(GetInPort("angular_velocity"));
    if (m_pAngularSpeed) {
      vstr::warn() << "[VistaDfnNavigationNode]: port name \"angular_velocity\" is deprecated "
                      "- use \"angular_speed\" instead"
                   << std::endl;
    }
  }
  return GetIsValid();
}

bool VistaDfnNavigationNode::GetIsValid() const {
  return ((m_pTranslation || m_pRotation) && m_pDeltaTime);
}

bool VistaDfnNavigationNode::DoEvalNode() {
  if (m_pDeltaTime->GetUpdateCounter() == m_nLastDTUpateCount)
    return true;

  if (m_pNavigationMode)
    m_iNavigationMode = m_pNavigationMode->GetValue();

  m_nLastDTUpateCount = m_pDeltaTime->GetUpdateCounter();

  // += ensures that we don't loose time on many very small updates
  m_fDeltaTime += float(m_pDeltaTime->GetValueConstRef());
  if (m_fDeltaTime < Vista::Epsilon)
    return true;

  UpdateVelocities();

  VistaTransformMatrix matTransform;
  if (m_pTransform)
    matTransform = m_pTransform->GetValueConstRef();

  switch (m_iNavigationMode) {
  case 1: {
    // Helicopter Mode: No roll, no pitch, just yaw
    ApplyRotationYawOnly(matTransform);
    ApplyTranslation(matTransform);
    break;
  }
  case 2: {
    // Camera Mode: No roll, just yaw/pitch
    ApplyRotationNoRoll(matTransform);
    ApplyTranslation(matTransform);
    break;
  }
  case 0:
  default: {
    // Free Mode: All rotations allowed
    ApplyRotationFull(matTransform);
    ApplyTranslation(matTransform);
    break;
  }
  }

  m_pOut->GetValueRef() = matTransform;
  m_pOut->IncUpdateCounter();

  m_fDeltaTime = 0;

  return true;
}

void VistaDfnNavigationNode::ApplyTranslation(VistaTransformMatrix& matTransform) {
  if (m_pTranslation == NULL)
    return;

  VistaVector3D v3Change = m_fDeltaTime * m_v3CurrentLinearVelocity;

  v3Change = matTransform.TransformVector(v3Change);

  VistaVector3D v3Trans;
  matTransform.GetTranslation(v3Trans);
  matTransform.SetTranslation(v3Trans + v3Change);
}

void VistaDfnNavigationNode::ApplyRotationFull(VistaTransformMatrix& matTransform) {
  if (m_pRotation == NULL || m_fCurrentAngularSpeed == 0)
    return;

  VistaVector3D v3Pivot;
  if (m_pRotationPivot)
    v3Pivot = m_pRotationPivot->GetValue();

  // calculate rotation slerp-interpolated by the fraction
  VistaAxisAndAngle aaRotationFraction = m_aaCurrentAngularVelocity;
  aaRotationFraction.m_fAngle *= m_fDeltaTime;
  VistaQuaternion qRotDt(aaRotationFraction);

  // apply rotation relative to pivot point
  matTransform = matTransform * VistaTransformMatrix(v3Pivot) * VistaTransformMatrix(qRotDt) *
                 VistaTransformMatrix(-v3Pivot);
}

void VistaDfnNavigationNode::ApplyRotationYawOnly(VistaTransformMatrix& matTransform) {
  if (m_pRotation == NULL || m_fCurrentAngularSpeed == 0)
    return;

  VistaVector3D v3Pivot;
  if (m_pRotationPivot)
    v3Pivot = m_pRotationPivot->GetValue();

  // calculate rotation slerp-interpolated by the fraction
  VistaAxisAndAngle aaRotationFraction = m_aaCurrentAngularVelocity;
  aaRotationFraction.m_fAngle *= m_fDeltaTime;
  VistaQuaternion qRotDt(aaRotationFraction);

  VistaVector3D vY = qRotDt.Rotate(Vista::ViewVector);
  vY[Vista::Y]     = 0.0f;
  vY.Normalize();
  VistaQuaternion qY = VistaQuaternion(Vista::ViewVector, vY);

  // apply rotation relative to pivot point
  matTransform = matTransform * VistaTransformMatrix(v3Pivot) * VistaTransformMatrix(qY) *
                 VistaTransformMatrix(-v3Pivot);
}

void VistaDfnNavigationNode::ApplyRotationNoRoll(VistaTransformMatrix& matTransform) {
  if (m_pRotation == NULL || m_fCurrentAngularSpeed == 0)
    return;

  VistaVector3D v3Pivot;
  if (m_pRotationPivot)
    v3Pivot = m_pRotationPivot->GetValue();

  // calculate rotation slerp-interpolated by the fraction
  VistaAxisAndAngle aaRotationFraction = m_aaCurrentAngularVelocity;
  aaRotationFraction.m_fAngle *= m_fDeltaTime;
  VistaQuaternion qRotDt(aaRotationFraction);

  VistaVector3D v3OrigTranslation;
  matTransform.GetTranslation(v3OrigTranslation);
  VistaQuaternion qFullRot(matTransform);
  qFullRot = qFullRot * qRotDt;

  // determine rotation around global y
  VistaVector3D vY = qFullRot.Rotate(Vista::ViewVector);
  vY[Vista::Y]     = 0.0f;
  vY.Normalize();
  VistaQuaternion qY = VistaQuaternion(Vista::ViewVector, vY);

  // Calculate reduced Rotation
  qFullRot = qY.GetInverted() * qFullRot;

  // determine rotation around global X
  VistaVector3D vX = qFullRot.Rotate(Vista::ViewVector);
  vX[Vista::X]     = 0.0f;
  vX.Normalize();
  VistaQuaternion qX = VistaQuaternion(Vista::ViewVector, vX);

  // Prevent Gimbal Lock
  VistaAxisAndAngle aaaX = qX.GetAxisAndAngle();
  if (aaaX.m_fAngle > 0.45f * Vista::Pi) {
    aaaX.m_fAngle = 0.45f * Vista::Pi;
    qX            = VistaQuaternion(aaaX);
  } else if (aaaX.m_fAngle < -0.45f * Vista::Pi) {
    aaaX.m_fAngle = -0.45f * Vista::Pi;
    qX            = VistaQuaternion(aaaX);
  }

  // @todo: use pivot in this mode

  // Determine Final rotation
  qFullRot = qY * qX;
  matTransform.SetBasisMatrix(qFullRot);
}

void VistaDfnNavigationNode::UpdateVelocities() {
  if (m_pTranslation) {
    if (m_fLinearAcceleration == 0 && m_fLinearDeceleration == 0) {
      m_v3CurrentLinearVelocity = m_pTranslation->GetValueConstRef();
      if (m_pLinearSpeed)
        m_v3CurrentLinearVelocity *= m_pLinearSpeed->GetValue();
      else
        m_v3CurrentLinearVelocity *= m_fDefaultLinearSpeed;
    } else {
      VistaVector3D v3TargetVelocity = m_pTranslation->GetValueConstRef();
      if (m_pLinearSpeed)
        v3TargetVelocity *= m_pLinearSpeed->GetValue();
      else
        v3TargetVelocity *= m_fDefaultLinearSpeed;

      VistaVector3D v3Delta = v3TargetVelocity - m_v3CurrentLinearVelocity;

      // we split it into tangential and normal components
      // only components in negative normal dir are decelerated, others accelerated
      VistaVector3D v3NormalDir       = m_v3CurrentLinearVelocity.GetNormalized();
      float         fNormalProjection = v3Delta * v3NormalDir;

      VistaVector3D v3TangentDir = v3Delta - fNormalProjection * v3NormalDir;
      v3TangentDir.Normalize();
      float fTangentProjection = v3Delta * v3TangentDir;

      if (fNormalProjection >= 0) // acceleration
      {
        if (m_fLinearAcceleration > 0)
          fNormalProjection = std::min(m_fLinearAcceleration * m_fDeltaTime, fNormalProjection);
      } else // deceleration
      {
        if (m_fLinearDeceleration > 0)
          fNormalProjection = std::max(-m_fLinearDeceleration * m_fDeltaTime, fNormalProjection);
      }

      if (m_fLinearAcceleration > 0) {
        if (fTangentProjection > 0)
          fTangentProjection = std::min(m_fLinearAcceleration * m_fDeltaTime, fTangentProjection);
        else
          fTangentProjection = std::max(-m_fLinearAcceleration * m_fDeltaTime, fTangentProjection);
      }

      // update current velocity
      m_v3CurrentLinearVelocity +=
          fTangentProjection * v3TangentDir + fNormalProjection * v3NormalDir;
    }
  }

  if (m_pRotation) {
    // for now, we only accelerate/decelerate the angle, not the direction
    VistaQuaternion qRot       = m_pRotation->GetValueConstRef();
    m_aaCurrentAngularVelocity = qRot.GetAxisAndAngle();
    m_fTargetAngularSpeed      = m_aaCurrentAngularVelocity.m_fAngle;
    if (m_pAngularSpeed)
      m_fTargetAngularSpeed *= m_pAngularSpeed->GetValue();

    if (m_fCurrentAngularSpeed > m_fTargetAngularSpeed) {
      if (m_fCurrentAngularSpeed > 0) // decelerate
      {
        if (m_fAngularDecceleration > 0) {
          m_fCurrentAngularSpeed -= m_fAngularDecceleration * m_fDeltaTime;
          if (m_fCurrentAngularSpeed < m_fTargetAngularSpeed)
            m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
        } else
          m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
      } else // accelerate
      {
        if (m_fAngularAcceleration > 0) {
          m_fCurrentAngularSpeed -= m_fAngularAcceleration * m_fDeltaTime;
          if (m_fCurrentAngularSpeed < m_fTargetAngularSpeed)
            m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
        } else
          m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
      }
    } else if (m_fCurrentAngularSpeed < m_fTargetAngularSpeed) {
      if (m_fCurrentAngularSpeed < 0) // decelerate
      {
        if (m_fAngularDecceleration > 0) {
          m_fCurrentAngularSpeed += m_fAngularDecceleration * m_fDeltaTime;
          if (m_fCurrentAngularSpeed > m_fTargetAngularSpeed)
            m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
        } else
          m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
      } else // accelerate
      {
        if (m_fAngularAcceleration > 0) {
          m_fCurrentAngularSpeed += m_fAngularAcceleration * m_fDeltaTime;
          if (m_fCurrentAngularSpeed > m_fTargetAngularSpeed)
            m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
        } else
          m_fCurrentAngularSpeed = m_fTargetAngularSpeed;
      }
    }

    m_aaCurrentAngularVelocity.m_fAngle = m_fCurrentAngularSpeed;
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
