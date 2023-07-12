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

// include header here
#include "Bob.h"

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaBase/VistaVectorMath.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

Bob::Bob(VistaSystem* pVistaSystem)
    : m_pVistaSystem(pVistaSystem) {
  // Loading Scene content
  VistaGraphicsManager* pMgr = pVistaSystem->GetGraphicsManager();
  VistaSceneGraph*      pSG  = pMgr->GetSceneGraph();

  IVistaNode* pNode = pSG->LoadNode("hippo.ac");

  if (pNode) {
    // model loaded, add to root node.
    pSG->GetRoot()->AddChild(pNode);
  } else {
    // Model couldn't be loaded. Let's try another path :)
    pNode = pSG->LoadNode("../hippo.ac");

    if (pNode) {
      pSG->GetRoot()->AddChild(pNode);
    }
  }

  // now determine the reference frame from a given display system
  // we query that from the display manager here
  VistaDisplayManager* pDisplayManager = pVistaSystem->GetDisplayManager();

  // for this demo, we simply take the first display system (index = 0)
  VistaDisplaySystem* pDispSys = pDisplayManager->GetDisplaySystem(0);

  if (pDispSys == 0) {
    VISTA_THROW("No DisplaySystem found", 1);
  }

  // each display system has a reference frame, which represents a virtual
  // camera, so we claim that.
  m_pVirtualPlatform = pDispSys->GetReferenceFrame();
}

Bob::~Bob() {
}

bool Bob::OnCmdAction(BOB_COMMANDS eDirection, float fValue) {
  switch (eDirection) {
  case CMD_RESOLUTION:
    // supposed to check fValue >= 3 but doesn't in initial implementation!
    return true;
    break;

  case CMD_ZOOM_IN:

    this->Translate(VistaVector3D(0, 0, 0.25f * fValue));
    break;

  case CMD_ZOOM_OUT:

    this->Translate(VistaVector3D(0, 0, 0.25f * fValue));
    break;

  case CMD_TURN_LEFT:

    this->Rotate(0.5f);
    break;

  case CMD_TURN_RIGHT:

    this->Rotate(-0.5f);
    break;

  case CMD_MOVE_LEFT:

    this->Translate(VistaVector3D(-0.25f, 0, 0));
    break;

  case CMD_MOVE_RIGHT:

    this->Translate(VistaVector3D(0.25f, 0, 0));
    break;

  case CMD_MOVE_UP:

    this->Translate(VistaVector3D(0, 0.25f, 0));
    break;

  case CMD_MOVE_DOWN:

    this->Translate(VistaVector3D(0, -0.25f, 0));
    break;

  default:
    return true;
  }

  return true;
}

bool Bob::Translate(const VistaVector3D& m_v3Trans) {
  // move the camera, but keep the movement in
  // the view alignments, so get the current rotation
  // of the vp
  VistaQuaternion q = m_pVirtualPlatform->GetRotation();

  // rotate the given world vector along the view direction
  VistaVector3D v3Trans = q.Rotate(m_v3Trans);

  // Translate to that direction
  return m_pVirtualPlatform->SetTranslation(m_pVirtualPlatform->GetTranslation() + v3Trans);
}

bool Bob::Rotate(float nDeg) {
  // rotation is simple: apply an incremental roation
  // the AxisAndAngle constructs a quat that defines a rotation
  // around the global given axis, keep in mind that the rotation
  // is given in radians.
  VistaAxisAndAngle aaa(VistaVector3D(0, 1, 0), Vista::DegToRad(nDeg));
  VistaQuaternion   q(aaa);

  // due to numerical reasons, a quat might degenerate during the
  // manipulation, so it might be a good idea to normalize it once and then
  // here, we do it on every call, but that might be too much
  VistaQuaternion qRot = m_pVirtualPlatform->GetRotation();
  qRot.Normalize();

  // set the rotation as an incremental rotation
  return m_pVirtualPlatform->SetRotation(qRot * q);

  // the rotation is around the center of the virtual platform.
  // iff you decide to have the viewpoint not in 0,0,0, this might
  // look akward. In that case, you have to apply a transform
  // that includes a normalization around the view point position.
  // for this demo, we chose the viewpoint to be 0,0,0 and the projection
  // plane midpoint in 0,0,-2. So the rotation look ''natural'' around the
  // view point (as it is in 0,0,0).
}
