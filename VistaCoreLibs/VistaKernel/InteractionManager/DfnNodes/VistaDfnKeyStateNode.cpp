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

#include "VistaDfnKeyStateNode.h"

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaDfnKeyStateNode::VistaDfnKeyStateNode()
    : VistaDfnKeyCallbackNode()
    , m_pKeyState(new TVdfnPort<bool>)
    , m_bKeyState(false)
    , m_uiUpdateScore(1) {
  m_pCallbackKeyUp = new CounterCallback(this);
  RegisterOutPort("state", m_pKeyState);
}

VistaDfnKeyStateNode::~VistaDfnKeyStateNode() {
  if (m_pCallbackKeyUp)
    m_pKeyboard->UnbindAction(m_pCallbackKeyUp);
  delete m_pCallbackKeyUp;
}

bool VistaDfnKeyStateNode::PrepareEvaluationRun() {
  return GetIsValid();
}

bool VistaDfnKeyStateNode::GetIsValid() const {
  return (m_pCallbackKeyUp != NULL && VistaDfnKeyCallbackNode::GetIsValid());
}

bool VistaDfnKeyStateNode::DoEvalNode() {
  VistaDfnKeyCallbackNode::DoEvalNode();
  if (m_pKeyState->GetValue() != m_bKeyState)
    m_pKeyState->SetValue(m_bKeyState, GetUpdateTimeStamp());
  return true;
}

unsigned int VistaDfnKeyStateNode::CalcUpdateNeededScore() const {
  return m_uiUpdateScore;
}

void VistaDfnKeyStateNode::Callback(const CounterCallback* pCallback) {
  VistaDfnKeyCallbackNode::Callback(pCallback);

  if (pCallback == m_pCallbackKeyDown)
    m_bKeyState = true;
  else if (pCallback == m_pCallbackKeyUp)
    m_bKeyState = false;
  ++m_uiUpdateScore;
}

void VistaDfnKeyStateNode::SetupKeyboardCallback(VistaKeyboardSystemControl* pKeyboard,
    const int nKeyCode, const int nModCode, const std::string& sDescription, const bool bForce) {
  if (m_pKeyboard == NULL) {
    m_pKeyboard = pKeyboard;
    if (m_pKeyboard->BindAction(
            nKeyCode, nModCode, m_pCallbackKeyDown, sDescription, false, false, bForce) == false) {
      delete m_pCallbackKeyDown;
      m_pCallbackKeyDown = NULL;
    }

    if (m_pKeyboard->BindAction(
            -nKeyCode, nModCode, m_pCallbackKeyUp, sDescription, false, false, bForce) == false) {
      delete m_pCallbackKeyUp;
      m_pCallbackKeyUp = NULL;
    }
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
