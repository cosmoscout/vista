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


#ifndef _VISTAGLUTTEXTENTITY_H
#define _VISTAGLUTTEXTENTITY_H

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

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * GLUT implementation of IVistaTextEntity. See VistaTextEntity.h for
 * documentation. Default value is Helvetice 18.
 */
class VISTAKERNELAPI VistaGlutTextEntity : public IVistaTextEntity
{
public:
	VistaGlutTextEntity();
	virtual ~VistaGlutTextEntity();

	/**
	 * GLUT does not support any combination of font size and fontfamily.
	 * The given paramters are fitted to an avaliable fontsize / -family pair.
	 * First try is to match the nearest fontsize. If the font size is in the
	 * correct range the algorithm looks at the fontfamily.
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
	 * Gives the native GLUT font pointer.
	 */
	void* GetFontType();

private:

	void* m_pFontType;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATEXTENTITY_H
