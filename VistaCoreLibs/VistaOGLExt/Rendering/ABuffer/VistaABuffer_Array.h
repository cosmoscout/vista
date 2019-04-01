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


#ifndef VISTA_A_BUFFER_ARRAY_H
#define VISTA_A_BUFFER_ARRAY_H
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaABuffer.h"

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class implements the A-Buffer by using an Array to store Fragments.
 * For each pixel a separate array is used to store the data of fragments,
 * which are associated with that pixel.
 * All thetas Arrays have the same size, there for only a fixed amount of 
 * fragments can be stored for each Pixel.
 */
class VISTAOGLEXTAPI VistaABufferArray : public IVistaABuffer
{
public: 
	/**************************************************************************/
	/* CONSTRUCTORS / DESTRUCTOR                                              */
	/**************************************************************************/
	VistaABufferArray();
	virtual ~VistaABufferArray();

	/**************************************************************************/
	/* PUBLIC INTERFACE                                                       */
	/**************************************************************************/
	virtual VistaGLSLShader* CreateShaderPrototype() const;

protected:
	virtual bool InitShader();
	virtual bool InitBuffer();
	virtual void ResizePerPixelBuffer();

	virtual void AssignUniforms( VistaGLSLShader* pShader );

private:
	GLuint			m_uiFragCountBuffer;
	GLuint64EXT		m_addrFragCountBuffer;
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
