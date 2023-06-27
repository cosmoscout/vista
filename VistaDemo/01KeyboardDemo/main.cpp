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

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/GraphicsManager/Vista3DText.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaSwitchNode.h>
#include <VistaKernel/GraphicsManager/VistaTextNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaExceptionBase.h>

#include <iostream>
#include <list>
#include <string>

/**
 * This demo shows two things:
 * a) using the VistaKeyboardSystemControl to register callback actions
 *    for normal action processing, reacting on triggers, or keys.
 *    Note that this is the desired way of implementing actions, as
 *    the code might be re-used by different parts of the application,
 *    e.g., react on key strokes as well as button pushes or messages
 *    from the outside. However, you can only attach callbacks, you lose
 *    the information on the state of the modifier.
 * b) using the IVistaDirectKeySink API to capture all keystrokes as
 *    raw. Can be useful sometimes, e.g., implementing consoles.
 */
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/**
 * For direct key dispatching, we can derive from an interface defined in
 * the VistaKeyboardSystemControl, the IVistaDirectKeySink. It will force us
 * to write a routine that does a key handling as ''raw'', but gives us the
 * modifiers and raw key codes.
 */
class DirectKeyInput : public VistaKeyboardSystemControl::IVistaDirectKeySink {
 public:
  DirectKeyInput(VistaKeyboardSystemControl* pCtrl, VistaTextNode* pTextNode,
      VistaTextNode* pModNode, VistaSwitchNode* pSwitch)
      : VistaKeyboardSystemControl::IVistaDirectKeySink()
      , m_pCtrl(pCtrl)
      , m_pTextNode(pTextNode)
      , m_pModNode(pModNode)
      , m_pSwitchNode(pSwitch) {
  }

  ~DirectKeyInput() {
  }

  bool HandleKeyPress(int nKey, int nMod, bool bIsKeyRepeat) {
    if (nKey == VISTA_KEY_ESC) // we hard-wire the 'turn-off' condition to be ESC
    {
      // disable me
      m_pCtrl->SetDirectKeySink(NULL);  // remove pointer from control
      m_pSwitchNode->SetActiveChild(0); // reflect state in GUI
      return true; // swallow key, otherwise the action for ESC would be triggered
                   // which, in this example, would re-enable this instance as direct input
    }

    // try to print the character, if down key
    if (nKey > 0) // >0 is down key, < 0 is up key, 0 is no key
    {
      //// little conversion from int over char[2] to string
      //// as we can not pass an int directly to std::string
      // char c[2];
      // c[0] = (char)nKey;
      // c[1] = 0x0;

      // set the key as text (may look distorted)
      m_pTextNode->SetText(VistaKeyboardSystemControl::GetKeyName(nKey));

      // reset the mod node's text if no modifier is given
      if (nMod <= 0) {
        m_pModNode->SetText("");
      }
    }

    if (nMod > 0) // evaluate modifier as a bit mask
    {
      std::string strModText = VistaKeyboardSystemControl::GetModifiersName(nMod);

      m_pModNode->SetText(strModText); // set this value as text to the mod node
    } else
      m_pModNode->SetText(""); // set this value as text to the mod node

    return true; // swallow key, do not let it pass to the normal actions
  }

 private:
  VistaKeyboardSystemControl* m_pCtrl;     // needed to change state, e.g., disabling direct input
  VistaTextNode *             m_pTextNode, // reflects the pressed down-key as text
      *m_pModNode; // reflects the pressed modifier as concatenated text, depending on the modifier
  VistaSwitchNode*
      m_pSwitchNode; // just for this demo, make a switch node to switch between states in the view
};

/**
 * This callback is just for demo purposes to show how to work with
 * callbacks and the VistaKeyboardSystemControl. It does not do
 * anything to useful, but should make you think on how to write
 * these callbacks for your application (e.g., factorisation of parameters)
 */
class ActivationCallback : public IVistaExplicitCallbackInterface {
 public:
  ActivationCallback(VistaTextNode* pTextNode, const std::string& sTextToSet)
      : IVistaExplicitCallbackInterface()
      , m_pTextNode(pTextNode)
      , m_sTextToSet(sTextToSet) {
  }

  bool Do() {
    m_pTextNode->SetText(m_sTextToSet);
    return true;
  }

 private:
  VistaTextNode* m_pTextNode;
  std::string    m_sTextToSet;
};

/**
 * This is a state callback that once triggered will enable the direct
 * input and by the resulting state machine will disable the Action
 * callbacks until the ESC key was pressed (which all is done by the
 * CDirectInput instance.
 */
class SwitchAction : public IVistaExplicitCallbackInterface {
 public:
  SwitchAction(VistaSwitchNode* pSwitch, DirectKeyInput* pInput, VistaKeyboardSystemControl* pCtrl)
      : IVistaExplicitCallbackInterface()
      , m_pSwitchNode(pSwitch)
      , m_pInput(pInput)
      , m_pCtrl(pCtrl) {
  }

  bool Do() {
    m_pCtrl->SetDirectKeySink(m_pInput); // keys are swallowed from now on
                                         // see code in the CDirectInput::HandleKeyPress() method
    m_pSwitchNode->SetActiveChild(1);    // reflect change in the GUI
    return true;
  }

 private:
  VistaSwitchNode*            m_pSwitchNode; // GUI feedback
  DirectKeyInput*             m_pInput;      // direct input instance to set
  VistaKeyboardSystemControl* m_pCtrl;       // ctrl to set the direct instance to
};

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
int main(int argc, char** argv) {
  try {
    std::list<std::string> liSearchPath;
    liSearchPath.push_back("../configfiles");

    VistaSystem oVistaSystem;
    oVistaSystem.SetIniSearchPaths(liSearchPath);

    if (oVistaSystem.Init(argc, argv)) {

      // setup the scene
      // We want a text node to switch some text using callback actions
      // and another two text nodes to show a currently pressed key
      // and the state of the modifiers.
      VistaGraphicsManager* pGrMgr = oVistaSystem.GetGraphicsManager();

      // get a pointer to the scene graph and root
      VistaSceneGraph* pSceneGraph = pGrMgr->GetSceneGraph();
      VistaGroupNode*  pRoot       = pSceneGraph->GetRoot();

      // setup a node first for the callbacks later on
      VistaTransformNode* pCbNode = pSceneGraph->NewTransformNode(pRoot);

      // move it a bit away
      pCbNode->SetTranslation(VistaVector3D(-0.5f, 0, -1));

      // we use this node for a simple state control, it will switch
      // between the 'callback-enabled' and 'direct-input-no-callback' mode
      VistaSwitchNode* pSwitch = pSceneGraph->NewSwitchNode(pCbNode);

      // create a text node for the callback mode
      const std::string sFontName("SANS"); // if you have problems, you may try "sans"
      VistaTextNode*    pText = pSceneGraph->NewTextNode(pSwitch, sFontName);
      pText->GetTextImp()->SetFontSize(
          0.2f);                         // make it a bit smaller, so we can read it (hopefully)
      pText->SetText("press l, n or z"); // set a default text.
      // pText->SetText("");
      pText->SetName("Text Node");

      pSwitch->SetActiveChild(0); // set as active child in the switch node, once a child of that

      // now, the view for the first state (callback enabled) is ready to be set-up,
      // we will add some actions to toggle the text on the push of a button
      VistaKeyboardSystemControl* pCtrl = oVistaSystem.GetKeyboardSystemControl();
      // register a few callbacks to switch the text up key stroke

      pCtrl->BindAction('z', new ActivationCallback(pText, "z as in zorro"), "set text to zorro");
      pCtrl->BindAction(
          'l', new ActivationCallback(pText, "l as in leave me"), "set text to leave me");
      pCtrl->BindAction(
          'n', new ActivationCallback(pText, "n as in nothing"), "set text to nothing");

      // ok, now for the second part, which looks a bit more complicated, but only
      // for the scene view
      //              pGroup                  .
      //            /        \                .
      //      (-0.5,0,-1)   (0.5,0,-1)        .
      //           /           \              .
      // pDirect text       pModifier text    .

      VistaGroupNode* pGroup = pSceneGraph->NewGroupNode(pSwitch);
      pGroup->SetName("Direct Group");

      VistaTransformNode* pDirectTextGroup = pSceneGraph->NewTransformNode(pGroup);
      pDirectTextGroup->SetTranslation(VistaVector3D(-0.5, 0, -1));
      pDirectTextGroup->SetName("DT Group");

      VistaTransformNode* pModGroup = pSceneGraph->NewTransformNode(pGroup);
      pModGroup->SetTranslation(VistaVector3D(0.5, 0, -1));
      pModGroup->SetName("Mod Group");

      pText = pSceneGraph->NewTextNode(pDirectTextGroup, sFontName);
      pText->GetTextImp()->SetFontSize(0.2f);
      pText->SetText("direct");
      pText->SetName("Direct Text Node");

      VistaTransformNode* pModTextGroup = pSceneGraph->NewTransformNode(pGroup);
      pModTextGroup->SetTranslation(VistaVector3D(0.5, 0, -1));
      pModTextGroup->SetName("MT Group");
      VistaTextNode* pModNode = pSceneGraph->NewTextNode(pModTextGroup, sFontName);
      pModNode->GetTextImp()->SetFontSize(0.2f);
      pModNode->SetText("");
      pModNode->SetName("Mode Node");

      // create action for the ESC key.
      // the switch node should toggle upon ESC and the input processing will change a bit.
      // try it out.
      pCtrl->BindAction(VISTA_KEY_ESC,
          new SwitchAction(pSwitch, new DirectKeyInput(pCtrl, pText, pModNode, pSwitch), pCtrl),
          "switch direct input");

      if (oVistaSystem.GetDisplayManager()->GetDisplaySystem(0) == 0)
        VISTA_THROW("No DisplaySystem found", 1);

      oVistaSystem.GetDisplayManager()
          ->GetWindowByName("MAIN_WINDOW")
          ->GetWindowProperties()
          ->SetTitle(argv[0]);

      // run the application
      oVistaSystem.Run();
    }
  } catch (VistaExceptionBase& e) { e.PrintException(); }

  return 0;
}
