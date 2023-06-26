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

/**
 * Description:
 * This is the RecordReplayDemo. The code is very simple - just loading a
 * geometric model. The main explanation can be found in the README file.
 */

#include <VistaBase/VistaExceptionBase.h>

#include "VistaAspects/VistaExplicitCallbackInterface.h"
#include "VistaKernel/DisplayManager/VistaDisplayManager.h"
#include "VistaKernel/DisplayManager/VistaDisplaySystem.h"
#include "VistaKernel/DisplayManager/VistaVirtualPlatform.h"
#include "VistaKernel/GraphicsManager/VistaSceneGraph.h"
#include "VistaKernel/GraphicsManager/VistaTransformNode.h"
#include "VistaKernel/InteractionManager/VistaKeyboardSystemControl.h"
#include "VistaKernel/VistaSystem.h"

#include <iostream>
#include <list>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

class NaviKeyCallback : public IVistaExplicitCallbackInterface {
 public:
  enum NaviMode {
    NM_FORWARD,
    NM_BACKWARD,
    NM_LEFT,
    NM_RIGHT,
    NM_UP,
    NM_DOWN,
    NM_TURN_LEFT,
    NM_TURN_RIGHT,
    NM_TURN_UP,
    NM_TURN_DOWN,
  };

  NaviKeyCallback(VistaVirtualPlatform* pFrame, NaviMode eMode, float nValue)
      : m_eMode(eMode)
      , m_nValue(nValue)
      , m_pFrame(pFrame) {
  }
  ~NaviKeyCallback() {
  }

  virtual bool Do() {
    VistaQuaternion qRot = m_pFrame->GetRotation();
    switch (m_eMode) {
    case NM_FORWARD: {
      VistaVector3D v3Trans = m_pFrame->GetTranslation();
      v3Trans += qRot.Rotate(-m_nValue * Vista::ZAxis);
      m_pFrame->SetTranslation(v3Trans);
      break;
    }
    case NM_BACKWARD: {
      VistaVector3D v3Trans = m_pFrame->GetTranslation();
      v3Trans += qRot.Rotate(m_nValue * Vista::ZAxis);
      m_pFrame->SetTranslation(v3Trans);
      break;
    }
    case NM_LEFT: {
      VistaVector3D v3Trans = m_pFrame->GetTranslation();
      v3Trans += qRot.Rotate(-m_nValue * Vista::XAxis);
      m_pFrame->SetTranslation(v3Trans);
      break;
    }
    case NM_RIGHT: {
      VistaVector3D v3Trans = m_pFrame->GetTranslation();
      v3Trans += qRot.Rotate(m_nValue * Vista::XAxis);
      m_pFrame->SetTranslation(v3Trans);
      break;
    }
    case NM_UP: {
      VistaVector3D v3Trans = m_pFrame->GetTranslation();
      v3Trans += qRot.Rotate(m_nValue * Vista::YAxis);
      m_pFrame->SetTranslation(v3Trans);
      break;
    }
    case NM_DOWN: {
      VistaVector3D v3Trans = m_pFrame->GetTranslation();
      v3Trans += qRot.Rotate(-m_nValue * Vista::YAxis);
      m_pFrame->SetTranslation(v3Trans);
      break;
    }
    case NM_TURN_LEFT: {
      qRot *= VistaQuaternion(VistaAxisAndAngle(Vista::YAxis, m_nValue));
      m_pFrame->SetRotation(qRot);
      break;
    }
    case NM_TURN_RIGHT: {
      qRot *= VistaQuaternion(VistaAxisAndAngle(Vista::YAxis, -m_nValue));
      m_pFrame->SetRotation(qRot);
      break;
    }
    case NM_TURN_UP: {
      qRot *= VistaQuaternion(VistaAxisAndAngle(Vista::XAxis, m_nValue));
      m_pFrame->SetRotation(qRot);
      break;
    }
    case NM_TURN_DOWN: {
      qRot *= VistaQuaternion(VistaAxisAndAngle(Vista::XAxis, -m_nValue));
      m_pFrame->SetRotation(qRot);
      break;
    }
    }
    return true;
  }

  static bool RegisterDefaultCallbacks(VistaKeyboardSystemControl* pKeyControl,
      VistaVirtualPlatform* pPlatform, const float fMovementSpeed = 0.15f,
      const float fRotationSpeed = 0.1f) {
    bool bRet = true;
    bRet &= pKeyControl->BindAction('e',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_FORWARD, fMovementSpeed),
        "move forward by " + VistaConversion::ToString(fMovementSpeed) + "m");
    bRet &= pKeyControl->BindAction('d',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_BACKWARD, fMovementSpeed),
        "move backward by " + VistaConversion::ToString(fMovementSpeed) + "m");
    bRet &= pKeyControl->BindAction('s',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_LEFT, fMovementSpeed),
        "move left by " + VistaConversion::ToString(fMovementSpeed) + "m");
    bRet &= pKeyControl->BindAction('f',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_RIGHT, fMovementSpeed),
        "move right by " + VistaConversion::ToString(fMovementSpeed) + "m");
    bRet &= pKeyControl->BindAction('t',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_UP, fMovementSpeed),
        "move left by " + VistaConversion::ToString(fMovementSpeed) + "m");
    bRet &= pKeyControl->BindAction('g',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_DOWN, fMovementSpeed),
        "move right by " + VistaConversion::ToString(fMovementSpeed) + "m");
    bRet &= pKeyControl->BindAction('w',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_TURN_LEFT, fRotationSpeed),
        "rotate left by " + VistaConversion::ToString(Vista::RadToDeg(fRotationSpeed)) + "degree");
    bRet &= pKeyControl->BindAction('r',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_TURN_RIGHT, fRotationSpeed),
        "rotate right by " + VistaConversion::ToString(Vista::RadToDeg(fRotationSpeed)) + "degree");
    bRet &= pKeyControl->BindAction('z',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_TURN_UP, fRotationSpeed),
        "rotate left by " + VistaConversion::ToString(Vista::RadToDeg(fRotationSpeed)) + "degree");
    bRet &= pKeyControl->BindAction('h',
        new NaviKeyCallback(pPlatform, NaviKeyCallback::NM_TURN_DOWN, fRotationSpeed),
        "rotate right by " + VistaConversion::ToString(Vista::RadToDeg(fRotationSpeed)) + "degree");
    return bRet;
  }

 private:
  NaviMode              m_eMode;
  VistaVirtualPlatform* m_pFrame;
  float                 m_nValue;
};

int main(int argc, char* argv[]) {
  try {
    VistaSystem* pSystem = new VistaSystem();

    std::list<std::string> liSearchPath;
    liSearchPath.push_back("../configfiles/");
    liSearchPath.push_back("configfiles/");
    pSystem->SetIniSearchPaths(liSearchPath);

    pSystem->Init(argc, argv);

    // laod something to fill the scenery
    VistaSceneGraph*    pSG       = pSystem->GetGraphicsManager()->GetSceneGraph();
    IVistaNode*         pCow      = pSG->LoadNode("../data/cow.obj");
    VistaTransformNode* pCowTrans = pSG->NewTransformNode(pSG->GetRoot());
    pCowTrans->Translate(0, 0, -5);
    pCowTrans->AddChild(pCow);

    NaviKeyCallback::RegisterDefaultCallbacks(pSystem->GetKeyboardSystemControl(),
        pSystem->GetDisplayManager()->GetDisplaySystem(0)->GetReferenceFrame());

    pSystem->Run();

    delete pSystem;
  } catch (VistaExceptionBase& e) { e.PrintException(); } catch (std::exception& e) {
    std::cerr << "Exception:" << e.what() << std::endl;
  }

  return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
