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

class TranslateAction : public IVistaExplicitCallbackInterface {
 public:
  TranslateAction(VistaVirtualPlatform* pPlatform, const VistaVector3D& v3Trans)
      : m_pPlatform(pPlatform)
      , m_v3Trans(v3Trans) {
  }

  bool Do() {
    // move the camera, but keep the movement in
    // the view alignments, so get the current rotation
    // of the virtual platform
    VistaQuaternion qOrientation = m_pPlatform->GetRotation();

    // rotate the given world vector along the view direction
    VistaVector3D v3Trans = qOrientation.Rotate(m_v3Trans);

    // translate to that direction
    return m_pPlatform->SetTranslation(m_pPlatform->GetTranslation() + v3Trans);
  }

 private:
  VistaVirtualPlatform* m_pPlatform;
  VistaVector3D         m_v3Trans;
};

class RotateAction : public IVistaExplicitCallbackInterface {
 public:
  RotateAction(VistaVirtualPlatform* pPlatform, float nDeg)
      : m_pVirtualPlatform(pPlatform)
      , m_fRot(nDeg) {
  }

  bool Do() {
    // rotation is simple: apply an incremental rotation
    // the VistaAxisAndAngle constructs a quat that defines a rotation
    // around the global given axis, keep in mind that the rotation
    // is given in radians.

    VistaQuaternion qAxisAngle(VistaAxisAndAngle(Vista::UpVector, Vista::DegToRad(m_fRot)));

    // due to numerical reasons, a quat might degenerate during the
    // manipulation, so it might be a good idea to normalize it once and then
    // here, we do it on every call, but that might be too much
    VistaQuaternion qRotation = m_pVirtualPlatform->GetRotation();
    qRotation.Normalize();

    // set the rotation as an incremental rotation
    return m_pVirtualPlatform->SetRotation(qRotation * qAxisAngle);

    // the rotation is around the center of the virtual platform.
    // iff you decide to have the viewpoint not in 0,0,0, this might
    // look akward. In that case, you have to apply a transform
    // that includes a normalization around the view point position.
    // for this demo, we chose the viewpoint to be 0,0,0 and the projection
    // plane midpoint in 0,0,-2. So the rotation look ''natural'' around the
    // view point (as it is in 0,0,0).
  }

 private:
  VistaVirtualPlatform* m_pVirtualPlatform;
  float                 m_fRot;
};
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int main(int argc, char** argv) {
  try {
    std::list<std::string> liSearchPath;
    liSearchPath.push_back("../configfiles/");

    VistaSystem* pVistaSystem = new VistaSystem;

    pVistaSystem->SetIniSearchPaths(liSearchPath);
    if (pVistaSystem->Init(argc, argv)) {
      // load a model, so we have something to look at
      // a ''room-like'' model is more nice than an object
      // like model.
      VistaGraphicsManager* pMgr = pVistaSystem->GetGraphicsManager();
      VistaSceneGraph*      pSG  = pMgr->GetSceneGraph();

      IVistaNode* pNode = pSG->LoadNode("../data/hippo.ac");

      if (pNode) {
        // model loaded, add to root node.
        pSG->GetRoot()->AddChild(pNode);
      }

      // now determine the reference frame from a given display system
      // we query that from the display manager here
      VistaDisplayManager* pDispMgr = pVistaSystem->GetDisplayManager();

      // for this demo, we simply take the first display system (index = 0)
      VistaDisplaySystem* pDispSys = pDispMgr->GetDisplaySystem(0);

      if (pDispSys == 0)
        VISTA_THROW("No DisplaySystem found", 1);

      // each display system has a reference frame, which represents a virtual
      // camera, so we claim that.
      VistaVirtualPlatform* pVirtualPlatform = pDispSys->GetReferenceFrame();

      // register some callbacks for some keyboard strokes.
      // these have to be registered with the keyboard system control
      VistaKeyboardSystemControl* pCtrl = pVistaSystem->GetKeyboardSystemControl();

      // self descriptive.
      pCtrl->BindAction('a', new TranslateAction(pVirtualPlatform, VistaVector3D(-0.25f, 0, 0)),
          "translate to -x");
      pCtrl->BindAction('d', new TranslateAction(pVirtualPlatform, VistaVector3D(0.25f, 0, 0)),
          "translate to  x");
      pCtrl->BindAction('w', new TranslateAction(pVirtualPlatform, VistaVector3D(0, 0, -0.25f)),
          "translate to -z");
      pCtrl->BindAction('s', new TranslateAction(pVirtualPlatform, VistaVector3D(0, 0, 0.25f)),
          "translate to  z");

      pCtrl->BindAction(VISTA_KEY_UPARROW,
          new TranslateAction(pVirtualPlatform, VistaVector3D(0, 0.25f, 0)), "translate to y");
      pCtrl->BindAction(VISTA_KEY_DOWNARROW,
          new TranslateAction(pVirtualPlatform, VistaVector3D(0, -0.25f, 0)), "translate to -y");

      pCtrl->BindAction(VISTA_KEY_RIGHTARROW, new RotateAction(pVirtualPlatform, -0.5f),
          "rotate -0.5 deg about the y-axis");
      pCtrl->BindAction(VISTA_KEY_LEFTARROW, new RotateAction(pVirtualPlatform, 0.5f),
          "rotate 0.5 deg about the y-axis");

      pVistaSystem->GetDisplayManager()
          ->GetWindowByName("MAIN_WINDOW")
          ->GetWindowProperties()
          ->SetTitle(argv[0]);

      // start the simulation.
      pVistaSystem->Run();
    }
    // clean up!
    delete pVistaSystem;

  } catch (VistaExceptionBase& e) { e.PrintException(); }
}
