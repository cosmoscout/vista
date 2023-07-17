/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2023 German Aerospace Center (DLR)           */
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

#include "VistaSDL2TextEntity.h"

#include <GL/glew.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaTools/VistaIniFileParser.h>

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <array>

/**
 * This returns the path to the requested font by looking it up in the display ini.
 */
std::string GetFontPath(const std::string& font) {
  VistaIniFileParser parser(true);
  VistaPropertyList  displayProperties;
  parser.ReadProplistFromFile(GetVistaSystem()->GetDisplayIniFile(), displayProperties, true);

  std::string fontFile = "";

  if (VistaAspectsComparisonStuff::StringEquals(font, "MONOSPACE", false)) {
    displayProperties.GetValueInSubList("MONO_FONT", "FONTS", fontFile);
  } else if (VistaAspectsComparisonStuff::StringEquals(font, "SERIF", false)) {
    displayProperties.GetValueInSubList("SERIF_FONT", "FONTS", fontFile);
  } else {
    displayProperties.GetValueInSubList("SANS_FONT", "FONTS", fontFile);
  }

  return fontFile;
}

VistaSDL2TextEntity::VistaSDL2TextEntity() {
  m_nXPos     = 0;
  m_nYPos     = 0;
  m_nZPos     = 0;
  m_bEnabled  = true;
  m_oColor    = VistaColor::BRICK_RED;
  m_sText     = "";
  m_pointSize = 18;
  m_fontType  = TTF_OpenFont(GetFontPath("SANS").c_str(), m_pointSize);
}

VistaSDL2TextEntity::~VistaSDL2TextEntity() {
  TTF_CloseFont(m_fontType);
}

void VistaSDL2TextEntity::SetFont(const std::string& family, int size) {
  TTF_CloseFont(m_fontType);

  m_pointSize = size;
  m_fontType  = TTF_OpenFont(GetFontPath(family).c_str(), size);

  if (m_fontType == nullptr) {
    vstr::errp() << "[VistaSDL2TextEntity] Could not open font: " << family << std::endl;
  }
}

int VistaSDL2TextEntity::GetFontSize() const {
  return m_pointSize;
}

std::string VistaSDL2TextEntity::GetFontFamily() const {
  return TTF_FontFaceFamilyName(m_fontType);
}

TTF_Font* VistaSDL2TextEntity::GetFontType() {
  return m_fontType;
}

void VistaSDL2TextEntity::DrawCharacters() {
  if (!m_fontType) {
    return;
  }

  SDL_Color color = {
      static_cast<Uint8>(m_oColor.GetRed() * 255),
      static_cast<Uint8>(m_oColor.GetGreen() * 255),
      static_cast<Uint8>(m_oColor.GetBlue() * 255),
      static_cast<Uint8>(255)
  };

  SDL_Surface* surface = TTF_RenderUTF8_Blended(m_fontType, m_sText.c_str(), color);

  // Everything below here is very antiquated OpenGL, since the calling code is using functions like
  // glWindowPos* and glRasterPos* to set the rendering position.

  glPushAttrib(GL_BLEND);
  glPushAttrib(GL_BLEND_EQUATION);
  glPushAttrib(GL_CLIENT_PIXEL_STORE_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // We need to tell OpenGL, the length of a row in pixel. SDL gives the row length in bits, so we
  // have to devide it by the BytesPerPixel.
  glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->pitch / surface->format->BytesPerPixel);

  // We get the current raster position, because we later need it for some transformation.
  std::array<float, 4> rasterPos;
  glGetFloatv(GL_CURRENT_RASTER_POSITION, rasterPos.data());
  float x = rasterPos.at(0);
  float y = rasterPos.at(1);

  // We will offset the rendered text by the height. Afterwards we will flip it vertically, which
  // puts it at the original position again. We need to do this as the SDL_surface has saved the
  // images upside down in OpenGL coordinates.
  glWindowPos2f(x, y + surface->h);
  glPixelZoom(1, -1);

  // We render the text using the SDL buffer. Somehow the GL_BGRA format works, althoug SDL claims
  // that they use the ARGB format for their surface. Maybe this is a result of SDL also saving the
  // image upside down.
  glDrawPixels(surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

  glPopAttrib();
  glPopAttrib();
  glPopAttrib();

  SDL_FreeSurface(surface);
}
