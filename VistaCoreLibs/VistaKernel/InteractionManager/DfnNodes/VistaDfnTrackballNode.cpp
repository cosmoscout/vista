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

#include "VistaDfnTrackballNode.h"
#include <VistaBase/VistaExceptionBase.h>

#include <VistaAspects/VistaPropertyAwareable.h>

#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace {
const float TRACKBALLSIZE = 1.0f;

void axis_to_quat(VistaVector3D& a, float phi, VistaQuaternion& q) {
  a.Normalize();
  q[0] = a[0] * sinf(phi / 2.0f);
  q[1] = a[1] * sinf(phi / 2.0f);
  q[2] = a[2] * sinf(phi / 2.0f);
  q[3] = cosf(phi / 2.0f);
}

float tb_project_to_sphere(float r, float x, float y) {
  float d, t, z;

  d = std::sqrt(x * x + y * y);
  if (d < r * 0.70710678118654752440f) { /* Inside sphere */
    z = std::sqrt(r * r - d * d);
  } else { /* On hyperbola */
    t = r / 1.41421356237309504880f;
    z = t * t / d;
  }
  return z;
}

void CalcRotation(
    float p1x, float p1y, float p2x, float p2y, VistaQuaternion& result, float nScale) {
  float phi; /* how much to rotate about axis */
  float t;

  if (p1x == p2x && p1y == p2y) {
    /* Zero rotation */
    result = VistaQuaternion();
    return;
  }

  /*
   * First, figure out z-coordinates for projection of P1 and P2 to
   * deformed sphere
   */

  VistaVector3D v1(p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE * nScale, p1x, p1y));
  VistaVector3D v2(p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE * nScale, p2x, p2y));

  /*
   *  Now, we want the cross product of V1 and V2
   */

  VistaVector3D axis = v1.Cross(v2);

  /*
   *  Figure out how much to rotate around that axis.
   */
  VistaVector3D diff = v2 - v1;

  t = diff.GetLength() / 2.0f * TRACKBALLSIZE * nScale;

  /*
   * Avoid problems with out-of-control values...
   */
  if (t > 1.0f)
    t = 1.0f;
  if (t < -1.0f)
    t = -1.0f;

  phi = 2.0f * asinf(t);

  axis_to_quat(axis, phi, result);
}
} // namespace

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaVdfnTrackball::VistaVdfnTrackball()
    : IVdfnNode()
    , m_pCenter(NULL)
    , m_pGrab(NULL)
    , m_pOffset(NULL)
    , m_iUpdateCountOffset(0)
    , m_pOut(new TVdfnPort<VistaTransformMatrix>) {
  RegisterInPortPrototype("center", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("grab", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);
  RegisterInPortPrototype("offset", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D>>);

  RegisterOutPort("transform", m_pOut);
}

VistaVdfnTrackball::~VistaVdfnTrackball() {
}

bool VistaVdfnTrackball::PrepareEvaluationRun() {
  m_pCenter = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("center"));
  m_pGrab   = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("grab"));
  m_pOffset = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("offset"));

  return GetIsValid();
}

bool VistaVdfnTrackball::GetIsValid() const {
  // expects transformation matrix, the 2D coordinates.
  // rotation center is not mandatory
  return (m_pGrab && m_pOffset);
}

// #############################################################################

bool VistaVdfnTrackball::DoEvalNode() {
  if (m_pOffset->GetUpdateCounter() > m_iUpdateCountOffset) {
    m_iUpdateCountOffset = m_pOffset->GetUpdateCounter();
    if (m_pCenter)
      m_v3TBCenter = m_pCenter->GetValue();

    m_v3BegDrag = m_pGrab->GetValue();

    HandleRotate();

    m_pOut->IncUpdateCounter();
  }

  return true;
}

bool VistaVdfnTrackball::HandleRotate() {

  VistaQuaternion      q;
  const VistaVector3D& v3Pos = m_v3BegDrag + m_pOffset->GetValueConstRef();

  CalcRotation(m_v3BegDrag[0], m_v3BegDrag[1], v3Pos[0], v3Pos[1], q, 1.0f);
  // m_v3BegDrag = v3Pos; // swap old and new
  q.Normalize();

  VistaTransformMatrix mRot(q);
  VistaTransformMatrix mTrans(m_v3TBCenter);
  VistaTransformMatrix mInvTrans(-m_v3TBCenter);

  VistaTransformMatrix& m = m_pOut->GetValueRef();

  m = mTrans * mRot * mInvTrans;

  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
