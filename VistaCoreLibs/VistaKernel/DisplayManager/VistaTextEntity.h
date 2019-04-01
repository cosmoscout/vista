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


#ifndef _VISTATEXTENTITY_H
#define _VISTATEXTENTITY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaBase/VistaColor.h>
//#include <VistaKernel/GraphicsManager/VistaGeometry.h>

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
 * This defines a class to describe an text with toolkit
 * independent settings like font family and font size. The toolkit specific
 * methods are implemented in the subclasses.
 */
class VISTAKERNELAPI IVistaTextEntity
{
public:
	IVistaTextEntity(){};
	virtual ~IVistaTextEntity(){ /* No pointer no cleanup */ };

	/*
	 * Interface stuff
	 */
	/**
	 * The font is given by String and int as
	 * fontfamily and size.
	 * \param fontfamily
	 * \param size
	 */
	virtual void SetFont( const std::string& sFamily, int iTextSize ) = 0;
	/**
	 * Returns the real fontsize.
	 * Note: If your SetFont method has an best-fit
	 * algorithm for non-existing font sizes, the
	 * size you get from the getter may be different
	 * from the setter.
	 * \return size
	 */
	virtual int GetFontSize() const = 0;
	/**
	 * Returns the real fontfamily.
	 * Note: If your SetFont method has an best-fit
	 * algorithm for non-existing font families, the
	 * family you get from the getter may be different
	 * from the setter.
	 * \return fontfamily
	 */
	virtual std::string GetFontFamily() const = 0;

	/**
	 * Draws the individual characters. Does, however, not
	 * care about position or projection setup
	*/
	virtual void DrawCharacters() = 0;

	/**
	 * Getter and Setter for common attributes
	 */
	inline float GetXPos() const { return m_nXPos; };
	inline float GetYPos() const { return m_nYPos; };
	inline float GetZPos() const { return m_nZPos; };

	inline void SetXPos( float f ) { m_nXPos = f; };
	inline void SetYPos( float f ) { m_nYPos = f; };
	inline void SetZPos( float f ) { m_nZPos = f; };

	inline bool GetEnabled() const { return m_bEnabled; };
	inline void SetEnabled( bool b ) { m_bEnabled = b; };

	inline VistaColor GetColor() const { return m_oColor; };
	inline void SetColor( VistaColor c ) { m_oColor = c; };

	inline std::string GetText() { return m_sText; };
	inline void SetText( const std::string& sText ) { m_sText = sText; };

protected:

	float m_nXPos, m_nYPos, m_nZPos;

	bool m_bEnabled;

	VistaColor  m_oColor;
	std::string m_sText;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATEXTENTITY_H
