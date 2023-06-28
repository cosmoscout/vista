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

#include <VistaKernel/InteractionManager/VistaInteractionEvent.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaKernel/DisplayManager //Vista3DTextOverlay.h>
#include <VistaKernel/DisplayManager/VistaTextEntity.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaExceptionBase.h>

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

class ToggleOverlay : public IVistaExplicitCallbackInterface {
 public:
  ToggleOverlay(IVistaSceneOverlay* pOverlay, VistaDisplayManager* pDspMngr)
      : m_pOverlay(pOverlay)
      , m_pDspMngr(pDspMngr) {
  }

  bool Do() {
    // toggle the overlay

    // try to remove the overlay
    m_pOverlay->SetIsEnabled(!m_pOverlay->GetIsEnabled());

    return true;
  }

 private:
  IVistaSceneOverlay*  m_pOverlay;
  VistaDisplayManager* m_pDspMngr;
};

class CameraSweep : public VistaEventHandler {
 public:
  CameraSweep(
      VistaVirtualPlatform* pPlatform, float angle = Vista::Pi / 4.0f, float duration = 3.0f)
      : m_pVp(pPlatform)
      , axis(angle)
      , m_fDuration(duration) {
  }

  virtual void HandleEvent(VistaEvent* pEvent) {
    float frac = (float)fmod((double)pEvent->GetTime(), (double)m_fDuration * 2.0) / m_fDuration;
    if (frac > 1.0f) {
      frac = 2.0f - frac;
    }

    float a = axis * (frac - 0.5f);

    m_pVp->SetRotation(VistaQuaternion(VistaEulerAngles(0, a, 0)));
  }

 private:
  VistaVirtualPlatform* m_pVp;
  float                 axis;
  float                 m_fDuration;
};

class YawIndicatorOverlay : public IVistaSceneOverlay {
 public:
  YawIndicatorOverlay(VistaVirtualPlatform* pPlatform, VistaDisplayManager* pDispManager,
      const std::string& sViewportName = "")
      : IVistaSceneOverlay(pDispManager, sViewportName)
      , m_pPlatform(pPlatform)
      , m_bEnabled(true) {
  }

  virtual bool Do() {
    if (m_bEnabled == false)
      return false;

    //////////////////////////////////////////
    // draw a small camera rotation-indicator
    //////////////////////////////////////////

    // save some GL state values
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

    // reset transformation stacks
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // do the actual drawing:

    // move to lower left corner
    glTranslatef(-0.85f, -0.8f, 0);

    // draw a nice box
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.15f, -0.2f);
    glVertex2f(0.15f, -0.2f);
    glVertex2f(0.15f, 0.2f);
    glVertex2f(-0.15f, 0.2f);
    glVertex2f(-0.15f, -0.2f);
    glEnd();

    // draw a small arrow indicating the platform yaw
    VistaAxisAndAngle oAxisAngle = m_pPlatform->GetRotation().GetAxisAndAngle();
    glRotatef(180.0f * oAxisAngle.m_fAngle / Vista::Pi, 0, 0, oAxisAngle.m_v3Axis[1]);

    glColor3f(1.0f, 0.2f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.00f, 0.1f);
    glVertex2f(-0.06f, 0.0f);
    glVertex2f(0.06f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(-0.03f, -0.0f);
    glVertex2f(-0.03f, -0.1f);
    glVertex2f(0.03f, -0.1f);
    glVertex2f(0.03f, -0.0f);
    glEnd();

    // restore GL state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(matrixMode);

    return true;
  }

  virtual void UpdateOnViewportChange(int iWidth, int iHeight, int iPosX, int iPosY) {
    return;
  }

  virtual bool GetIsEnabled() const {
    return m_bEnabled;
  }

  virtual void SetIsEnabled(bool bEnabled) {
    m_bEnabled = bEnabled;
  }

 private:
  VistaVirtualPlatform* m_pPlatform;
  bool                  m_bEnabled;
};

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int main(int argc, char** argv) {
  try {

    VistaSystem* pVistaSystem = new VistaSystem;

    // add "configfiles" to searchpath
    std::list<std::string> liSearchPath;
    liSearchPath.push_back("../configfiles/");
    pVistaSystem->SetIniSearchPaths(liSearchPath);

    if (pVistaSystem->Init(argc, argv)) {
      // add a cube, so we have something to look at
      VistaGraphicsManager* pMgr = pVistaSystem->GetGraphicsManager();
      VistaSceneGraph*      pSG  = pMgr->GetSceneGraph();
      VistaGeometryFactory  geometryFactory(pSG);

      VistaGeometry*      box          = geometryFactory.CreateBox();
      VistaTransformNode* boxTransform = pSG->NewTransformNode(pSG->GetRoot());
      boxTransform->SetTranslation(0, 0.5f, 0);
      boxTransform->SetRotation(VistaQuaternion(VistaEulerAngles(0, (Vista::Pi / 3.0f), 0)));
      box->SetColor(VistaColor(VistaColor::LIGHT_GRAY));
      pSG->NewGeomNode(boxTransform, box);

      VistaGeometry* floor = geometryFactory.CreatePlane(40, 40, 40, 40, Vista::Y);
      floor->SetColor(VistaColor(VistaColor::GRAY));
      pSG->NewGeomNode(pSG->GetRoot(), floor);

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
      pVirtualPlatform->SetTranslation(VistaVector3D(0.0f, 1.2f, 4.5f));

      ////////////////////////////////////////////////////////////////////////////
      // OVERLAYS
      ////////////////////////////////////////////////////////////////////////////
      // add overlays

      // 1st: a 3D text overlay - 2D text hooked to a 3D position in the scene.
      Vista3DTextOverlay* oTextOverlay3D = new Vista3DTextOverlay(pDispMgr);
      IVistaTextEntity*   pTextEntity    = pDispMgr->CreateTextEntity();
      pTextEntity->SetText("overlay text hooked to a 3D position");
      pTextEntity->SetColor(VistaColor::RED);
      pTextEntity->SetYPos(1.2f);
      oTextOverlay3D->AddText(pTextEntity);

      // 2nd: a 2D text overlay
      Vista2DText* p2DText = pDispMgr->New2DText();
      p2DText->Init("use [i] and [t] to toggle overlays!", 0.16f, 0.01f, 0, 0, 0, 18);

      // 3rd: a self drawn overlay
      YawIndicatorOverlay oYawIndicator(pVirtualPlatform, pDispMgr);
      pDispMgr->AddSceneOverlay(&oYawIndicator);

      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////

      // register some callbacks for some keyboard strokes.
      // these have to be registered with the keyboard system control
      VistaKeyboardSystemControl* pCtrl = pVistaSystem->GetKeyboardSystemControl();

      // self descriptive.
      pCtrl->BindAction('t', new ToggleOverlay(oTextOverlay3D, pDispMgr), "Toggle 3D text overlay");

      pCtrl->BindAction(
          'i', new ToggleOverlay(&oYawIndicator, pDispMgr), "Toggle indicator overlay");

      // add simple camera animation
      pVistaSystem->GetEventManager()->AddEventHandler(new CameraSweep(pVirtualPlatform),
          VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

      // trigger "show help" command
      pCtrl->GetActionForToken('?')->Do();

      pVistaSystem->GetDisplayManager()
          ->GetWindowByName("MAIN_WINDOW")
          ->GetWindowProperties()
          ->SetTitle(argv[0]);

      // start the simulation.
      pVistaSystem->Run();
    }

    // clean up
    delete pVistaSystem;
  } catch (VistaExceptionBase& e) { e.PrintException(); }

  return 0;
}
