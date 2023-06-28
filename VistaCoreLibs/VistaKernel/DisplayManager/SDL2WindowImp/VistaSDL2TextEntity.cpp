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

#include "VistaSDL2TextEntity.h"

#include <VistaAspects/VistaAspectsUtils.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

const char* SANS_FONT_FILE = "OpenSans-Regular.ttf";
const char* SERIF_FONT_FILE = "SourceSerifPro-Regular.ttf";
const char* MONO_FONT_FILE = "SometypeMono-Regular.ttf";

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaSDL2TextEntity::VistaSDL2TextEntity(SDL_Renderer *renderer)
{
	m_nXPos = 0;
	m_nYPos = 0;
	m_nZPos = 0;
	m_bEnabled = true;
	m_oColor = VistaColor::BRICK_RED;
	m_sText = "";
	m_iPointSize = 18;
	m_pFontType = TTF_OpenFont(SANS_FONT_FILE, m_iPointSize);
	m_pRenderer = renderer;
}

VistaSDL2TextEntity::~VistaSDL2TextEntity()
{
	TTF_CloseFont(m_pFontType);
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaSDL2TextEntity::SetFont( const std::string& sFamily , int iSize )
{
	TTF_CloseFont(m_pFontType);

	m_iPointSize = iSize;

	if( VistaAspectsComparisonStuff::StringEquals( sFamily, "MONOSPACE", false ) ) {
		m_pFontType = TTF_OpenFont(MONO_FONT_FILE, iSize);
	} else if( VistaAspectsComparisonStuff::StringEquals( sFamily, "SERIF", false)  ) {
		m_pFontType = TTF_OpenFont(SERIF_FONT_FILE, iSize);
	} else {
		m_pFontType = TTF_OpenFont(SANS_FONT_FILE, iSize);
	}
}

int VistaSDL2TextEntity::GetFontSize() const
{
	return m_iPointSize;
}

std::string VistaSDL2TextEntity::GetFontFamily() const
{
	return TTF_FontFaceFamilyName(m_pFontType);
}

TTF_Font* VistaSDL2TextEntity::GetFontType()
{
	return m_pFontType;
}

void VistaSDL2TextEntity::DrawCharacters()
{
	SDL_Color color = {m_oColor.GetRed() * 255, m_oColor.GetGreen() * 255, m_oColor.GetBlue() * 255};
	SDL_Surface* surface = TTF_RenderText_Solid(m_pFontType, m_sText.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_pRenderer, surface);

	int w;
	int h;
	TTF_SizeUTF8(m_pFontType, m_sText.c_str(), &w, &h);
	SDL_Rect bb = {0, 0, w, h};

	SDL_RenderCopy(m_pRenderer, texture, nullptr, &bb);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


