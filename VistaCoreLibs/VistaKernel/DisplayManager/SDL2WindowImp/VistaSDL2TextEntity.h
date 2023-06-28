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


#ifndef _VISTASDL2TEXTENTITY_H
#define _VISTASDL2TEXTENTITY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/DisplayManager/VistaTextEntity.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

struct TTF_Font;
struct SDL_Renderer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * SDL2 implementation of IVistaTextEntity. See VistaTextEntity.h for
 * documentation. Default value is Helvetice 18.
 */
class VISTAKERNELAPI VistaSDL2TextEntity : public IVistaTextEntity
{
public:
	explicit VistaSDL2TextEntity(SDL_Renderer *renderer);
	virtual ~VistaSDL2TextEntity();

	/**
	 * \param fontfamily
	 * \param fontsize
	 */
	void SetFont( const std::string& sFamily, int iFontSize );
	/**
	 * This method returns the *real* fontsize. This must not be the same
	 * as given in SetFont, because of the bestfit algorithm.
	 * \return fontsize
	 */
	int GetFontSize() const;
	/**
	 * This method returns the *real* fontfmaily. This must not be the same
	 * as given in SetFont, because of the bestfit algorithm.
	 * \return fontfamily
	 */
	std::string GetFontFamily() const;

	void DrawCharacters();

	/**
	 * Gives the native SDL2 font pointer.
	 */
	TTF_Font* GetFontType();

private:
	TTF_Font* m_pFontType;
	SDL_Renderer* m_pRenderer;
	int m_iPointSize;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASDL2TEXTENTITY_H
