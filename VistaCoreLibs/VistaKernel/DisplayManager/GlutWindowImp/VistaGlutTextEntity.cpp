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

#include <GL/glew.h>

#include "VistaGlutTextEntity.h"

#include <VistaAspects/VistaAspectsUtils.h>

#ifdef WIN32
// disable warnings from unreferenced functions in glut.h
// sadly, C4505 cannot be disabled locally with push/pop
#pragma warning(disable : 4505)
#endif
#if defined(DARWIN) // we use the mac os GLUT framework on darwin
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutTextEntity::VistaGlutTextEntity() {
  m_nXPos     = 0;
  m_nYPos     = 0;
  m_nZPos     = 0;
  m_bEnabled  = true;
  m_oColor    = VistaColor::BRICK_RED;
  m_sText     = "";
  m_pFontType = GLUT_BITMAP_HELVETICA_18;
}

VistaGlutTextEntity::~VistaGlutTextEntity() {
  /**
   * m_pFontType is NOT a valid pointer!!!
   * This is OpenGL Utility Toolkit stuff
   */
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaGlutTextEntity::SetFont(const std::string& sFamily, int iSize) {
  if (VistaAspectsComparisonStuff::StringEquals(sFamily, "MONOSPACE", false)) {
    if (iSize < 14)
      m_pFontType = GLUT_BITMAP_8_BY_13;
    else
      m_pFontType = GLUT_BITMAP_9_BY_15;
  } else if (iSize <= 11) {
    if (VistaAspectsComparisonStuff::StringEquals(sFamily, "SERIF", false)) {
      m_pFontType = GLUT_BITMAP_TIMES_ROMAN_10;
    } else {
      m_pFontType = GLUT_BITMAP_HELVETICA_10;
    }
  } else if (iSize < 16) {
    m_pFontType = GLUT_BITMAP_HELVETICA_12;
  } else if (iSize < 20) {
    m_pFontType = GLUT_BITMAP_HELVETICA_18;
  } else if (iSize > 20) {
    m_pFontType = GLUT_BITMAP_TIMES_ROMAN_24;
  }
}

int VistaGlutTextEntity::GetFontSize() const {
  if (m_pFontType == GLUT_BITMAP_TIMES_ROMAN_10 || m_pFontType == GLUT_BITMAP_HELVETICA_10)
    return 10;
  else if (m_pFontType == GLUT_BITMAP_HELVETICA_12)
    return 12;
  else if (m_pFontType == GLUT_BITMAP_HELVETICA_18)
    return 18;
  else if (m_pFontType == GLUT_BITMAP_TIMES_ROMAN_24)
    return 24;
  else if (m_pFontType == GLUT_BITMAP_8_BY_13)
    return 13;
  else if (m_pFontType == GLUT_BITMAP_9_BY_15)
    return 15;
  else
    return 0;
}

std::string VistaGlutTextEntity::GetFontFamily() const {
  if (m_pFontType == GLUT_BITMAP_HELVETICA_10 || m_pFontType == GLUT_BITMAP_HELVETICA_18 ||
      m_pFontType == GLUT_BITMAP_HELVETICA_12)
    return "SANS";
  else if (m_pFontType == GLUT_BITMAP_TIMES_ROMAN_10 || m_pFontType == GLUT_BITMAP_TIMES_ROMAN_24)
    return "SERIF";
  else if (m_pFontType == GLUT_BITMAP_8_BY_13 || m_pFontType == GLUT_BITMAP_9_BY_15)
    return "MONOSPACE";
  else
    return "";
}

void* VistaGlutTextEntity::GetFontType() {
  return m_pFontType;
}

void VistaGlutTextEntity::DrawCharacters() {
  // Iterator generates segfaults from time to time. So using the "classic" way
  // for(std::string::const_iterator sit = (*cit)->GetText().begin();
  //	sit != m_sText.end(); ++sit)
  for (unsigned int i = 0; i < m_sText.length(); ++i) {
    glutBitmapCharacter(m_pFontType, m_sText[i]);
  }
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
