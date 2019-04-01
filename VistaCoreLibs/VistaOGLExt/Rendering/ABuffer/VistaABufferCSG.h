/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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

#ifndef VISTA_ABUFFER_CSG_H
#define VISTA_ABUFFER_CSG_H
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../../VistaOGLExtConfig.h"

#include "VistaABufferOIT.h"

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class allows you tho render CSG objects with order independent transparency.
 * There for the A-Buffer is used to store fragments and later display them.
 *
 * @TODO !WARNING:
 *	This is an early Prototype. 
 *	The interface of this class will probably change.
 */
class VISTAOGLEXTAPI VistaABufferCSG : public VistaABufferOIT
{
public: 
	/**************************************************************************/
	/* CONSTRUCTORS / DESTRUCTOR                                              */
	/**************************************************************************/
	VistaABufferCSG();
	virtual ~VistaABufferCSG();

	//Extension
	void SetDispalyExtensionShaderName( const std::string& strName );

	/**
	 * See VistaABufferOIT::GetShaderPrototype
	 */
	virtual VistaGLSLShader* CreateShaderPrototype();


protected:
	virtual bool InitABuffer( unsigned int uiFragmentesPerPixel, unsigned int uiPageSize );
	virtual bool InitShader();

	bool InitDefaultShader(); 
	bool InitDisplayShader(); 

protected:
	std::string m_strDisplayShaderExt;
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
