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

#include "VistaKeyboardSystemControl.h"

#include <VistaKernel/EventManager/VistaEventManager.h>

#include "VistaAspects/VistaSimpleCallback.h"
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaConversion.h>
#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <sstream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
VistaKeyboardSystemControl::KeyboardAction::KeyboardAction()
    : m_pCallback(NULL) {
}

VistaKeyboardSystemControl::KeyboardAction::KeyboardAction(
    IVistaExplicitCallbackInterface* pCommand, const std::string& strHelpText,
    const bool bCallOnKeyRepreat, const bool bManageDeletion)
    : m_pCallback(pCommand)
    , m_strHelpText(strHelpText)
    , m_bManageDeletion(bManageDeletion)
    , m_bCallOnKeyRepreat(bCallOnKeyRepreat) {
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaKeyboardSystemControl::VistaKeyboardSystemControl(const bool bCheckForKeyRepeat)
    : m_pKeySink(NULL)
    , m_bCheckForKeyRepeat(bCheckForKeyRepeat)
    , m_vecModdedCommandMaps(8)
    , m_bProcessKeyCallbacksImmediately(true) {
}

VistaKeyboardSystemControl::~VistaKeyboardSystemControl() {
  for (CommandMap::iterator it = m_mapCommandMap.begin(); it != m_mapCommandMap.end(); ++it) {
    if ((*it).second.m_bManageDeletion)
      delete (*it).second.m_pCallback;
  }

  for (int i = 0; i < (int)m_vecModdedCommandMaps.size(); ++i) {
    for (CommandMap::iterator it = m_vecModdedCommandMaps[i].begin();
         it != m_vecModdedCommandMaps[i].end(); ++it) {
      if ((*it).second.m_bManageDeletion)
        delete (*it).second.m_pCallback;
    }
  }
}

bool VistaKeyboardSystemControl::GetProcessKeyCallbacksImmediately() const {
  return m_bProcessKeyCallbacksImmediately;
}

void VistaKeyboardSystemControl::SetProcessKeyCallbacksImmediately(const bool bSet) {
  m_bProcessKeyCallbacksImmediately = bSet;
}

void VistaKeyboardSystemControl::ProcessBufferedKeyCallbacks() {
  EmitBufferedKeyActions();
}

bool VistaKeyboardSystemControl::GetCheckForKeyRepeat() const {
  return m_bCheckForKeyRepeat;
}
void VistaKeyboardSystemControl::SetCheckForKeyRepeat(const bool bSet) {
  m_bCheckForKeyRepeat = bSet;
}

bool VistaKeyboardSystemControl::InjectKeys(const std::vector<int>& vecKeyCodeList,
    const std::vector<int>& vecModCodeList, std::vector<int>& vecUnswallowedKeys,
    std::vector<int>& vecUnswallowedMods) {
  assert(vecKeyCodeList.size() == vecModCodeList.size());

  std::vector<int>::const_iterator itKey  = vecKeyCodeList.begin();
  std::vector<int>::const_iterator itMods = vecModCodeList.begin();

  if (m_bCheckForKeyRepeat) {
#ifdef WIN32
    for (; itKey != vecKeyCodeList.end(); ++itKey, ++itMods) {
      // check for key repeat
      bool bWasPressed = false;
      if (m_vecLastFrameKeys.empty() == false) {
        std::vector<int>::const_iterator itLastPress =
            std::find(m_vecLastFrameKeys.begin(), m_vecLastFrameKeys.end(), (*itKey));
        if (itLastPress != m_vecLastFrameKeys.end()) // was the key pressed last frame?
        {
          std::vector<int>::const_iterator itLastReleased =
              std::find<std::vector<int>::const_iterator, int>(
                  itLastPress, m_vecLastFrameKeys.end(), -(*itKey));
          if (itLastReleased == m_vecLastFrameKeys.end()) // was not released again last frame
            bWasPressed = true;
        }
      }
      if (InjectKey((*itKey), (*itMods), bWasPressed) == false) // unswallowed?
      {
        vecUnswallowedKeys.push_back(*itKey);
        vecUnswallowedMods.push_back(*itMods);
      }
    }
    m_vecLastFrameKeys = vecKeyCodeList;
#else // LINUX
    for (; itKey != vecKeyCodeList.end(); ++itKey, ++itMods) {
      // check for key repeat
      bool bWasPressed = false;
      if ((*itKey) < 0) {
        std::vector<int>::const_iterator itNextKey = itKey;
        if (++itNextKey != vecKeyCodeList.end()) {
          std::vector<int>::const_iterator itNextMods = itMods;
          ++itNextMods;
          if ((*itKey) == -(*itNextKey) && (*itMods) == (*itNextMods)) {
            ++itKey;
            ++itMods;
            bWasPressed = true;
          }
        }
      }
      if (InjectKey((*itKey), (*itMods), bWasPressed) == false) // unswallowed?
      {
        vecUnswallowedKeys.push_back(*itKey);
        vecUnswallowedMods.push_back(*itMods);
      }
    }
    m_vecLastFrameKeys = vecKeyCodeList;
#endif
  } else {

    for (; itKey != vecKeyCodeList.end(); ++itKey, ++itMods) {
      if (InjectKey((*itKey), (*itMods), false) == false) // unswallowed?
      {
        vecUnswallowedKeys.push_back(*itKey);
        vecUnswallowedMods.push_back(*itMods);
      }
    }
  }

  // true: both are empty: all keys and mods were swallowed
  // false: at least on key/mod pair unswallowed
  return (vecUnswallowedKeys.empty() && vecUnswallowedMods.empty());
}

bool VistaKeyboardSystemControl::InjectKey(int nKeyCode, int nModifier, bool bIsKeyRepeat) {
  if (m_pKeySink) {
    if (m_pKeySink->HandleKeyPress(nKeyCode, nModifier, bIsKeyRepeat) == true)
      return true; // swallow
  }
  CommandMap::iterator itAction = m_vecModdedCommandMaps[nModifier].find(nKeyCode);

  if (itAction == m_vecModdedCommandMaps[nModifier].end()) {
    itAction = m_mapCommandMap.find(nKeyCode);
    if (itAction == m_mapCommandMap.end())
      return false; // no entry
  }

  if (bIsKeyRepeat && (*itAction).second.m_bCallOnKeyRepreat == false)
    return false; // ignore key repeats

  bool bRes = false;
  if (m_bProcessKeyCallbacksImmediately) {
    bRes = EmitKeyAction(&(*itAction).second);
  } else {
    BufferKeyAction(&(*itAction).second);
    bRes = true;
  }

  return bRes;
}

bool VistaKeyboardSystemControl::BindAction(int nKeyCode,
    IVistaExplicitCallbackInterface* pCallback, const std::string& sHelpText, bool bManageDeletion,
    bool bCallOnKeyRepeat, bool bForce) {
  return BindAction(
      nKeyCode, VISTA_KEYMOD_ANY, pCallback, sHelpText, bManageDeletion, bCallOnKeyRepeat, bForce);
}

bool VistaKeyboardSystemControl::BindAction(int nKeyCode, int nModifiers,
    IVistaExplicitCallbackInterface* pCallback, const std::string& sHelpText, bool bManageDeletion,
    bool bCallOnKeyRepeat, bool bForce)

{
  CommandMap& mapCommands =
      (nModifiers == VISTA_KEYMOD_ANY) ? (m_mapCommandMap) : (m_vecModdedCommandMaps[nModifiers]);
  CommandMap::iterator itCurrent = mapCommands.find(nKeyCode);
  if (itCurrent != mapCommands.end()) {
    if (bForce) {
      if ((*itCurrent).second.m_bManageDeletion)
        delete (*itCurrent).second.m_pCallback;
      vstr::warnp() << "[KeyboardSysControl::BindAction]: Binding Callback - Key ["
                    << GetModifiersName(nModifiers) << GetKeyName(nKeyCode)
                    << "] already in use by (" << (*itCurrent).second.m_strHelpText
                    << "), wil be replaced" << std::endl;
    } else {
      vstr::warnp() << "[KeyboardSysControl::BindAction]: Cannot bind Callback - Key ["
                    << GetModifiersName(nModifiers) << GetKeyName(nKeyCode)
                    << "] already in use by (" << (*itCurrent).second.m_strHelpText << ") !"
                    << std::endl;
      if (bManageDeletion)
        delete pCallback;
      return false;
    }
  }
  mapCommands[nKeyCode] = KeyboardAction(pCallback, sHelpText, bCallOnKeyRepeat, bManageDeletion);
  return true;
}

bool VistaKeyboardSystemControl::BindAction(int nKeyCode, std::function<void(void)> funcCallback,
    const std::string& sHelpText /*= "<none>"*/, bool bManageDeletion /*= true*/,
    bool bCallOnKeyRepeat /*= true*/, bool bForce /*= false*/) {
  return BindAction(nKeyCode, VISTA_KEYMOD_ANY, funcCallback, sHelpText, bManageDeletion,
      bCallOnKeyRepeat, bForce);
}

bool VistaKeyboardSystemControl::BindAction(int nKeyCode, int nModifiers,
    std::function<void(void)> funcCallback, const std::string& sHelpText /*= "<none>"*/,
    bool bManageDeletion /*= true*/, bool bCallOnKeyRepeat /*= true*/, bool bForce /*= false*/) {
  return BindAction(nKeyCode, nModifiers, new VistaSimpleCallback(funcCallback), sHelpText,
      bManageDeletion, bCallOnKeyRepeat, bForce);
}

IVistaExplicitCallbackInterface* VistaKeyboardSystemControl::GetActionForToken(
    int nKeyCode, int nModifiers) const {
  const CommandMap& mapCommands =
      (nModifiers == VISTA_KEYMOD_ANY) ? (m_mapCommandMap) : (m_vecModdedCommandMaps[nModifiers]);
  CommandMap::const_iterator cit = mapCommands.find(nKeyCode);
  if (cit == mapCommands.end())
    return NULL;

  return (*cit).second.m_pCallback;
}

bool VistaKeyboardSystemControl::UnbindAction(int nKeyCode, int nModifiers) {
  CommandMap& mapCommands =
      (nModifiers == VISTA_KEYMOD_ANY) ? (m_mapCommandMap) : (m_vecModdedCommandMaps[nModifiers]);

  CommandMap::iterator cit = mapCommands.find(nKeyCode);
  if (cit == mapCommands.end())
    return false;

  RemoveActionFromBufferedActions(&(*cit).second);

  if ((*cit).second.m_bManageDeletion)
    delete (*cit).second.m_pCallback;

  mapCommands.erase(cit);
  return true;
}

bool VistaKeyboardSystemControl::UnbindAction(IVistaExplicitCallbackInterface* pAction) {
  for (CommandMap::iterator itAction = m_mapCommandMap.begin(); itAction != m_mapCommandMap.end();
       ++itAction) {
    if ((*itAction).second.m_pCallback == pAction) {
      if ((*itAction).second.m_bManageDeletion)
        delete (*itAction).second.m_pCallback;
      RemoveActionFromBufferedActions(&(*itAction).second);
      m_mapCommandMap.erase(itAction);
      return true;
    }
  }
  for (std::vector<CommandMap>::iterator itMap = m_vecModdedCommandMaps.begin();
       itMap != m_vecModdedCommandMaps.end(); ++itMap) {
    for (CommandMap::iterator itAction = (*itMap).begin(); itAction != (*itMap).end(); ++itAction) {
      if ((*itAction).second.m_pCallback == pAction) {
        if ((*itAction).second.m_bManageDeletion)
          delete (*itAction).second.m_pCallback;
        RemoveActionFromBufferedActions(&(*itAction).second);
        (*itMap).erase(itAction);
        return true;
      }
    }
  }
  return false;
}

int VistaKeyboardSystemControl::GetTokenList(std::list<int>& liTarget, int nModifiers) const {
  const CommandMap& mapCommands =
      (nModifiers == VISTA_KEYMOD_ANY) ? (m_mapCommandMap) : (m_vecModdedCommandMaps[nModifiers]);

  for (CommandMap::const_iterator cit = mapCommands.begin(); cit != mapCommands.end(); ++cit) {
    liTarget.push_back((*cit).first);
  }

  return (int)liTarget.size();
}

std::string VistaKeyboardSystemControl::GetHelpTextForToken(int nKeyCode, int nModifiers) const {
  const CommandMap& mapCommands =
      (nModifiers == VISTA_KEYMOD_ANY) ? (m_mapCommandMap) : (m_vecModdedCommandMaps[nModifiers]);

  CommandMap::const_iterator cit = mapCommands.find(nKeyCode);
  if (cit == mapCommands.end())
    return "";
  return (*cit).second.m_strHelpText;
}

VistaKeyboardSystemControl::IVistaDirectKeySink*
VistaKeyboardSystemControl::GetDirectKeySink() const {
  return m_pKeySink;
}

void VistaKeyboardSystemControl::SetDirectKeySink(IVistaDirectKeySink* pSink) {
  m_pKeySink = pSink;
}

std::string VistaKeyboardSystemControl::GetKeyName(const int nKeyCode) {
  int nActual = abs(nKeyCode);
  switch (nActual) {
  case VISTA_KEY_UPARROW:
    return "UP";
  case VISTA_KEY_DOWNARROW:
    return "DOWN";
  case VISTA_KEY_RIGHTARROW:
    return "RIGHT";
  case VISTA_KEY_LEFTARROW:
    return "LEFT";
  case VISTA_KEY_ESC:
    return "ESC";
  case VISTA_KEY_F1:
    return "F1";
  case VISTA_KEY_F2:
    return "F2";
  case VISTA_KEY_F3:
    return "F3";
  case VISTA_KEY_F4:
    return "F4";
  case VISTA_KEY_F5:
    return "F5";
  case VISTA_KEY_F6:
    return "F6";
  case VISTA_KEY_F7:
    return "F7";
  case VISTA_KEY_F8:
    return "F8";
  case VISTA_KEY_F9:
    return "F9";
  case VISTA_KEY_F10:
    return "F10";
  case VISTA_KEY_F11:
    return "F11";
  case VISTA_KEY_F12:
    return "F12";
  case VISTA_KEY_ENTER:
    return "ENTER";
  case VISTA_KEY_TAB:
    return "TAB";
  case VISTA_KEY_BACKSPACE:
    return "BACK";
  case VISTA_KEY_DELETE:
    return "DEL";
  case VISTA_KEY_HOME:
    return "HOME";
  case VISTA_KEY_END:
    return "END";
  case VISTA_KEY_PAGEUP:
    return "PG_UP";
  case VISTA_KEY_PAGEDOWN:
    return "PG_DN";
  case VISTA_KEY_MIDDLE:
    return "MID";
  case VISTA_KEY_ALT_LEFT:
    return "ALT_LEFT";
  case VISTA_KEY_ALT_RIGHT:
    return "ALT_RIGHT";
  case VISTA_KEY_SHIFT_LEFT:
    return "SHIFT_LEFT";
  case VISTA_KEY_SHIFT_RIGHT:
    return "SHIFT_RIGHT";
  case VISTA_KEY_CTRL_LEFT:
    return "CTRL_LEFT";
  case VISTA_KEY_CTRL_RIGHT:
    return "CTRL_RIGHT";
  default:
    return std::string(1, (char)nActual);
  }
}

std::string VistaKeyboardSystemControl::GetModifiersName(const int nModifiers) {
  if (nModifiers == VISTA_KEYMOD_ANY)
    return "";
  else if (nModifiers == VISTA_KEYMOD_NONE)
    return "NONE";
  std::string sReturn = "";
  bool        bEmpty  = true;
  if (nModifiers & VISTA_KEYMOD_SHIFT) {
    sReturn += "SHIFT";
    bEmpty = false;
  }
  if (nModifiers & VISTA_KEYMOD_CTRL) {
    if (bEmpty == false)
      sReturn += "+CTRL";
    else
      sReturn += "CTRL";
    bEmpty = false;
  }
  if (nModifiers & VISTA_KEYMOD_ALT) {
    if (bEmpty == false)
      sReturn += "+ALT";
    else
      sReturn += "ALT";
  }

  return sReturn;
}

int VistaKeyboardSystemControl::GetKeyValueFromString(const std::string& sKeyString) {
  if (sKeyString.size() == 1) {
    // just one entry - use this as ascii char
    return sKeyString[0];
  }

  VistaAspectsComparisonStuff::StringCompareObject oCompare(false);

  std::string sCleanedString = sKeyString;
  sCleanedString.erase(
      std::remove(sCleanedString.begin(), sCleanedString.end(), '_'), sCleanedString.end());
  if (oCompare(sCleanedString.substr(0, 5), "VISTA"))
    sCleanedString = sCleanedString.substr(5);
  if (oCompare(sCleanedString.substr(0, 3), "KEY"))
    sCleanedString = sCleanedString.substr(3);

  if (oCompare(sCleanedString, "UP") || oCompare(sCleanedString, "UPARROW"))
    return VISTA_KEY_UPARROW;
  else if (oCompare(sCleanedString, "DOWN") || oCompare(sCleanedString, "DOWNARROW"))
    return VISTA_KEY_DOWNARROW;
  else if (oCompare(sCleanedString, "RIGHT") || oCompare(sCleanedString, "RIGHTARROW"))
    return VISTA_KEY_RIGHTARROW;
  else if (oCompare(sCleanedString, "LEFT") || oCompare(sCleanedString, "LEFTARROW"))
    return VISTA_KEY_LEFTARROW;
  else if (oCompare(sCleanedString, "ESC"))
    return VISTA_KEY_ESC;
  else if (oCompare(sCleanedString, "F1"))
    return VISTA_KEY_F1;
  else if (oCompare(sCleanedString, "F2"))
    return VISTA_KEY_F2;
  else if (oCompare(sCleanedString, "F3"))
    return VISTA_KEY_F3;
  else if (oCompare(sCleanedString, "F4"))
    return VISTA_KEY_F4;
  else if (oCompare(sCleanedString, "F5"))
    return VISTA_KEY_F5;
  else if (oCompare(sCleanedString, "F6"))
    return VISTA_KEY_F6;
  else if (oCompare(sCleanedString, "F7"))
    return VISTA_KEY_F7;
  else if (oCompare(sCleanedString, "F8"))
    return VISTA_KEY_F8;
  else if (oCompare(sCleanedString, "F9"))
    return VISTA_KEY_F9;
  else if (oCompare(sCleanedString, "F10"))
    return VISTA_KEY_F10;
  else if (oCompare(sCleanedString, "F11"))
    return VISTA_KEY_F11;
  else if (oCompare(sCleanedString, "F12"))
    return VISTA_KEY_F12;
  else if (oCompare(sCleanedString, "ENTER"))
    return VISTA_KEY_ENTER;
  else if (oCompare(sCleanedString, "TAB") || oCompare(sCleanedString, "TABULATOR"))
    return VISTA_KEY_TAB;
  else if (oCompare(sCleanedString, "BACK") || oCompare(sCleanedString, "BACKSPACE"))
    return VISTA_KEY_BACKSPACE;
  else if (oCompare(sCleanedString, "DEL") || oCompare(sCleanedString, "DELETE"))
    return VISTA_KEY_DELETE;
  else if (oCompare(sCleanedString, "HOME") || oCompare(sCleanedString, "POS1"))
    return VISTA_KEY_HOME;
  else if (oCompare(sCleanedString, "END"))
    return VISTA_KEY_END;
  else if (oCompare(sCleanedString, "PGUP") || oCompare(sCleanedString, "PAGEUP"))
    return VISTA_KEY_PAGEUP;
  else if (oCompare(sCleanedString, "PGDN") || oCompare(sCleanedString, "PAGEDOWN"))
    return VISTA_KEY_PAGEDOWN;
  else if (oCompare(sCleanedString, "MID") || oCompare(sCleanedString, "MIDDLE"))
    return VISTA_KEY_MIDDLE;
  else if (oCompare(sCleanedString, "ALTLEFT") || oCompare(sCleanedString, "ALTL"))
    return VISTA_KEY_ALT_LEFT;
  else if (oCompare(sCleanedString, "ALTRIGHT") || oCompare(sCleanedString, "ALTR"))
    return VISTA_KEY_ALT_RIGHT;
  else if (oCompare(sCleanedString, "SHIFTLEFT") || oCompare(sCleanedString, "SHIFTL"))
    return VISTA_KEY_SHIFT_LEFT;
  else if (oCompare(sCleanedString, "SHIFTRIGHT") || oCompare(sCleanedString, "SHIFTR"))
    return VISTA_KEY_SHIFT_RIGHT;
  else if (oCompare(sCleanedString, "CTRLLEFT") || oCompare(sCleanedString, "CTRLL"))
    return VISTA_KEY_CTRL_LEFT;
  else if (oCompare(sCleanedString, "CTRLRIGHT") || oCompare(sCleanedString, "CTRLR"))
    return VISTA_KEY_CTRL_RIGHT;

  if (sKeyString.size() > 1) {
    int nKeyCode;
    if (VistaConversion::FromString<int>(sKeyString, nKeyCode) && nKeyCode >= 0)
      return nKeyCode;
  }

  return -1;
}

int VistaKeyboardSystemControl::GetModifiersValueFromString(const std::string& sModString) {
  std::size_t nStart = 0;
  std::size_t nEnd;
  int         nModifier = 0;
  std::string sSubString;

  if (sModString.empty())
    return VISTA_KEYMOD_ANY;

  VistaAspectsComparisonStuff::StringCompareObject oCompare(false);

  if (oCompare(sModString, "NONE"))
    return VISTA_KEYMOD_NONE;

  for (;;) {
    nEnd = sModString.find('+', nStart);
    if (nEnd == std::string::npos)
      sSubString = sModString.substr(nStart);
    else
      sSubString = sModString.substr(nStart, nEnd - nStart);

    if (oCompare(sSubString, "CTRL") || oCompare(sSubString, "CONTROL"))
      nModifier |= VISTA_KEYMOD_CTRL;
    else if (oCompare(sSubString, "SHIFT"))
      nModifier |= VISTA_KEYMOD_SHIFT;
    else if (oCompare(sSubString, "ALT"))
      nModifier |= VISTA_KEYMOD_ALT;
    else
      return -1; // invalid

    if (nEnd == std::string::npos)
      break;

    nStart = nEnd + 1;
  }
  return nModifier;
}

bool VistaKeyboardSystemControl::GetKeyAndModifiersValueFromString(
    const std::string& sKeyModString, int& nKey, int& nModifiers) {
  std::string sKeyString;
  std::string sModString;

  // check for last plus, separating modifiers and keys
  // take care not to select last char, because it can be a plus key symbol
  std::size_t nPlusPos = sKeyModString.rfind('+', sKeyModString.size() - 2);
  if (nPlusPos == std::string::npos || nPlusPos == sKeyModString.size() - 1) {
    // no +, so just the key value
    sKeyString = sKeyModString;
  } else {
    sModString = sKeyModString.substr(0, nPlusPos);
    sKeyString = sKeyModString.substr(nPlusPos + 1);
  }

  int nKeyValue = GetKeyValueFromString(sKeyString);
  if (nKeyValue == -1)
    return false;

  int nModValue = GetModifiersValueFromString(sModString);
  if (nModValue == -1)
    return false;

  nKey       = nKeyValue;
  nModifiers = nModValue;
  return true;
}

void VistaKeyboardSystemControl::BufferKeyAction(KeyboardAction* pAction) {
  m_vecActionsToProcess.push_back(pAction);
}

void VistaKeyboardSystemControl::EmitBufferedKeyActions() {
  std::vector<KeyboardAction*>::iterator itAction    = m_vecActionsToProcess.begin();
  std::vector<KeyboardAction*>::iterator itActionEnd = m_vecActionsToProcess.end();
  for (; itAction != itActionEnd; ++itAction) {
    if ((*itAction) !=
        NULL) // actions may be NULLed if they were unregistered qhile already in the queue
      EmitKeyAction((*itAction));
  }
  m_vecActionsToProcess.clear();
}

bool VistaKeyboardSystemControl::EmitKeyAction(KeyboardAction* pAction) {
  pAction->m_pCallback->PrepareCallback();

  // swallow this key? use the return value of Do() as a trigger
  bool bRet = pAction->m_pCallback->Do();
  pAction->m_pCallback->PostCallback();

  return bRet;
}

void VistaKeyboardSystemControl::RemoveActionFromBufferedActions(KeyboardAction* pAction) {
  std::vector<KeyboardAction*>::iterator       itAction    = m_vecActionsToProcess.begin();
  const std::vector<KeyboardAction*>::iterator itActionEnd = m_vecActionsToProcess.end();
  for (; itAction != itActionEnd; ++itAction) {
    if ((*itAction) == pAction)
      (*itAction) = NULL;
  }
}

void VistaKeyboardSystemControl::HandleEvent(VistaEvent* pEvent) {
  EmitBufferedKeyActions();
}

std::string VistaKeyboardSystemControl::GetKeyBindingTableString() const {
  std::stringstream oStream;
  oStream << "------------------------------------------------------------------------\n";
  oStream << std::right << std::setw(15) << " " << std::left << std::setw(5) << " Key "
          << " ) | "
          << "ASCII "
          << " ) | "
          << "Description \n";
  oStream << "------------------------------------------------------------------------\n";
  for (CommandMap::const_iterator itAction = m_mapCommandMap.begin();
       itAction != m_mapCommandMap.end(); ++itAction) {
    oStream << std::right << std::setw(15) << " " << std::left << std::setw(5)
            << GetKeyName((*itAction).first) << " ) | " << std::setw(5) << (*itAction).first
            << " ) | " << (*itAction).second.m_strHelpText << "\n";
  }
  for (int i = 0; i <= 7; ++i) {
    for (CommandMap::const_iterator itAction = m_vecModdedCommandMaps[i].begin();
         itAction != m_vecModdedCommandMaps[i].end(); ++itAction) {
      oStream << std::right << std::setw(14) << GetModifiersName(i) << "+" << std::left
              << std::setw(5) << GetKeyName((*itAction).first) << " ) | " << std::setw(5)
              << (*itAction).first << " ) | " << (*itAction).second.m_strHelpText << "\n";
    }
  }
  oStream << "------------------------------------------------------------------------\n";
  return oStream.str();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
