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


#ifndef VISTA_A_BUFFER_LINKED_LIST_H
#define VISTA_A_BUFFER_LINKED_LIST_H
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaABuffer.h"

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class implements the A-Buffer by using Linked-Lists to store Fragments.
 * Every List-Elements represents one Fragment. For every Pixel a pointer to 
 * the first element of the list of fragments, that are associated with thees
 * pixel, is stored. 
 * All List-Elements are stored in a single storage area, there for the amount of 
 * fragments, that can be stored for one Pixel, is only limited by the total
 * size of the storage area. This allows a better use of memory, since no 
 * memory is consumed by pixels, to which no fragment is mapped.
 */
class VISTAOGLEXTAPI VistaABufferLinkedList : public IVistaABuffer
{
public: 
	/**************************************************************************/
	/* CONSTRUCTORS / DESTRUCTOR                                              */
	/**************************************************************************/
	VistaABufferLinkedList();
	virtual ~VistaABufferLinkedList();

	/**************************************************************************/
	/* PUBLIC INTERFACE                                                       */
	/**************************************************************************/
	virtual bool ClearABuffer();

	virtual VistaGLSLShader* CreateShaderPrototype() const;

protected:
	virtual bool InitShader();
	virtual bool InitBuffer();
	virtual void ResizePerPixelBuffer();

	virtual void AssignUniforms( VistaGLSLShader* pShader );

	void CreateAtomicCounter();
	void ResetAtomicCounter();

private:
	GLuint			m_uiHeadPointerBuffer;
	GLuint64EXT		m_addrHeadPointerBuffer;

	GLuint			m_uiAtomicFragCounter;
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
