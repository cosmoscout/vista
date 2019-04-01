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


#ifndef _VistaRenderToVertexArray_h
#define _VistaRenderToVertexArray_h

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <GL/glew.h>
#include "VistaOGLExtConfig.h"
#include <cstddef>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaBufferObject;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaRenderToVertexArray provides a basic abstraction for 
 * render-to-vertex-array functionality based on vertex buffer objects and
 * pixel buffer objects.
 */
class VISTAOGLEXTAPI VistaRenderToVertexArray
{
public:
	VistaRenderToVertexArray(int iVertexCount, int iComponents, GLenum eType);
	~VistaRenderToVertexArray();

	/**
	 * Set the vertex buffer and/or pixel buffer data.
	 * NOTE: Any previously bound buffer object will be released.
	 */
	void SetData( void* pData );

	/**
	 * Retrieve the data from the vertex buffer / pixel buffer object
	 * and copy it to the given memory location.
	 * NOTE: The given memory area is supposed to be large enough to 
	 *       contain the whole data. The size of the data can be queried
	 *       via GetSize().
	 * NOTE: Any previously bound buffer object will be released.
	 */
	void GetData( void* pData );

	/**
	 * Read the data from the given renderbuffer (i.e. part of the framebuffer
	 * or part of a currently bound framebuffer object) into the vertex
	 * array.
	 * NOTE: Any previously bound buffer object will be released.
	 */
	void Read( GLenum eBuffer, int iWidth, int iHeight );
	void Read( GLenum eBuffer, int iX, int iY, int iW, int iH, int iOffset );

	/**
	 * Bind the vertex buffer object for rendering from.
	 */
	void Bind();

	/**
	 * Disable any vertex buffer object, i.e. bind buffer object 0.
	 */
	void Release();

	/**
	 * Retrieve this buffer's id.
	 */
	GLuint GetId() const;

	/**
	 * Retrieve the size of the buffer object.
	 */
	std::size_t GetSize() const;

	/** 
	 * Retrieve some internal data.
	 */
	int GetVertexCount() const;
	int GetComponentCount() const;
	GLenum GetType() const;
	GLenum GetFormat() const;
	GLenum GetInternalFormat() const;
	int GetBytesPerComponent() const;

	/**
	 * Check for support of all required extensions.
	 */
	bool IsSupported() const;

protected:
	VistaBufferObject* m_pVertexArray;
	int		m_iVertexCount;
	int		m_iComponents;
	GLenum	m_eType;
	GLenum	m_eFormat;
	GLenum	m_eInternalFormat;
	int		m_iBytesPerComponent;
	GLenum	m_eUsage;
};

#endif // Include guard.


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
