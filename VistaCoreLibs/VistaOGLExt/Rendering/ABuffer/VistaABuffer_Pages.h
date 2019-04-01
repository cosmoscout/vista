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


#ifndef VISTA_A_BUFFER_PAGES_H
#define VISTA_A_BUFFER_PAGES_H
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaABuffer.h"

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This implementation of the A-Buffer is a mix between the implementation 
 * with arrays and the implementation with linked lists.
 * There for each pixel a list of pages is stored. Each page contains an array 
 * of fragments.
 */
class VISTAOGLEXTAPI VistaABufferPages : public IVistaABuffer
{
public: 
	/**************************************************************************/
	/* CONSTRUCTORS / DESTRUCTOR                                              */
	/**************************************************************************/
	VistaABufferPages( unsigned int nPageSize );
	virtual ~VistaABufferPages();

	/**************************************************************************/
	/* PUBLIC INTERFACE                                                       */
	/**************************************************************************/
	virtual bool ClearABuffer();

	virtual bool SetABufferSize( unsigned int uiWidth, unsigned int uiHeight );
	virtual bool SetFragmenstPerPixel( unsigned int uiFragmenstPerPixel );

	virtual VistaGLSLShader* CreateShaderPrototype() const;

protected:
	virtual bool InitShader();
	virtual bool InitBuffer();
	virtual void ResizePerPixelBuffer();

	virtual void AssignUniforms( VistaGLSLShader* pShader );

	void CreateAtomicCounter();
	void ResetAtomicCounter();

private:
	GLuint			m_uiPagePointerBuffer;
	GLuint			m_uiNextPointerBuffer;
	GLuint			m_uiSemaphoreBuffer;
	GLuint			m_uiFragCountBuffer;
	GLuint64EXT		m_addrPagePointerBuffer;
	GLuint64EXT		m_addrNextPointerBuffer;
	GLuint64EXT		m_addrSemaphoreBuffer;
	GLuint64EXT		m_addrFragCountBuffer;

	GLuint			m_uiAtomicPageCounter;

	unsigned int	m_nPageSize;
	unsigned int	m_nNumPages;
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
