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

#ifndef _VISTASDL2TEXTENTITY_H
#define _VISTASDL2TEXTENTITY_H

#include <VistaKernel/DisplayManager/VistaTextEntity.h>

#include <SDL2/SDL_ttf.h>

/**
 * SDL2 implementation of IVistaTextEntity. See VistaTextEntity.h for
 * documentation. Default value is a Sans Serif font with 18pt.
 *
 * This text entity requires font files to be provided in the display ini file.
 *
 * Example:
 *
 * [FONTS]
 * MONO_FONT  = fonts/a-mono-font.ttf
 * SANS_FONT  = fonts/a-sans-font.ttf
 * SERIF_FONT = fonts/a-serif-font.ttf
 */
class VISTAKERNELAPI VistaSDL2TextEntity final : public IVistaTextEntity {
 public:
  explicit VistaSDL2TextEntity();
  ~VistaSDL2TextEntity() final;

  /**
   * \param fontfamily
   * \param fontsize
   */
  void SetFont(const std::string& family, int fontSize) final;

  /**
   * This method returns the *real* fontsize. This must not be the same
   * as given in SetFont, because of the bestfit algorithm.
   * \return fontsize
   */
  int GetFontSize() const final;

  /**
   * This method returns the *real* fontfmaily. This must not be the same
   * as given in SetFont, because of the bestfit algorithm.
   * \return fontfamily
   */
  std::string GetFontFamily() const final;

  /**
   * This method renders the given string to a location set with glWindowPos* or glRasterPos*.
   */
  void DrawCharacters() final;

  /**
   * Gives the native SDL2 font pointer.
   */
  TTF_Font* GetFontType();

 private:
  TTF_Font* m_fontType;
  int       m_pointSize;
};

#endif //_VISTASDL2TEXTENTITY_H
