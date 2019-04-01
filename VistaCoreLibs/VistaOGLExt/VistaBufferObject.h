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


#ifndef _VistaBufferObject_h
#define _VistaBufferObject_h

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <GL/glew.h>
#include "VistaOGLExtConfig.h"
#include <cstddef>


/*============================================================================*/
/* CLASS DEFINITION                                                           */
/*============================================================================*/
/**
 * This class is a wrapper for OpenGL buffer objects.
 */
class VISTAOGLEXTAPI VistaBufferObject
{
public:
	VistaBufferObject();
	~VistaBufferObject();

	bool Bind( GLenum eTarget );
	bool Release();

	bool BindAsVertexDataBuffer();
	bool BindAsIndexBuffer();
	bool BindAsPixelPackBuffer();
	bool BindAsPixelUnpackBuffer();

	/**
	 * Binds the buffer to an indexed buffer target. The buffer will be bound to
	 * the target specified and additionally to the given index of that target.
	 * 
	 * @param eTarget The target to bind to. Must be a index target. See the
	 *		  OpenGL documentation at www.opengl.org/registry (glBindBufferBase)
	 *		  for available targets.
	 * @param uiIndex The index of the specified target.
	 * @return 'True' if the binding was successful.
	 */
	bool BindBufferBase( GLenum eTarget, GLuint uiIndex );
	/**
	 * Binds the buffer to an indexed buffer target and also allows to only bind
	 * a sub-range of the buffer to that target. The buffer will be bound to
	 * the target specified and additionally to the given index of that target.
	 * Only the specified range will be accessible in a shader.
	 * 
	 * @param eTarget The target to bind to. Must be a index target. See the
	 *		  OpenGL documentation at www.opengl.org/registry (glBindBufferBase)
	 *		  for available targets.
	 * @param uiIndex The index of the specified target.
	 * @param nOffsetInBytes The offset in bytes into the buffer.
	 * @param nLengthInBytes The number of bytes that will be used for the given
	 *		  index.
	 * @return 'True' if the binding was successful.
	 */
	bool BindBufferRange( GLenum eTarget, GLuint uiIndex,
						  GLintptr nOffsetInBytes,
						  GLsizei nLengthInBytes );

	/**
	 * Inits and copies data to the buffer object.
	 * 
	 * @param nSizeInBytes The size in bytes that the buffer shall be
	 *		  init'ed to.
	 * @param pData The data to copy to the buffer object on the GPU. To only
	 *		  init the buffer object to a specific size without copying data
	 *		  yet, i.e. to only reserve space, pass NULL here.
	 * @param uiUsage The way the buffer object is going to be used. Combine
     *		  usage parameter from the following list:
	 *		   - GL_STATIC... --> 1-to-n: data specified once, no changes
	 *				afterwards
	 *		   - GL_DYNAMIC... --> m-to-n: data often updated, but re-used
	 *				multiple times per update
	 *		   - GL_STREAM... --> 1-to-1: data frequently updated, but only
	 *				used once per update
	 *		   - ..._READ --> data is read back to CPU/host system
	 *		   - ..._DRAW --> data is send to the GPU to be used there
	 *		   - ..._COPY --> combination of _READ and _DRAW
	 *		  Mind that it is critical to choose the right usage flag as it
	 *		  usually has a major impact on performance. E.g. GL_STREAM_READ
	 *		  will be good for reading frequently updated data from the GPU but
	 *		  will be a bad idea for rendering static vertex buffer objects (use
	 *		  GL_STATIC_DRAW for the latter).
	 * @return Returns 'true' iff the command completed successfully.
	 *
	 * NOTE: The buffer object needs to be bound before this function will work.
	 */
	bool BufferData( GLsizeiptr nSizeInBytes, const void *pData,
					 GLenum eUsage ) const;
	/**
	 * Same as BufferData() but only updates a sub-interval of the memory.
	 *
	 * @param iOffset The offset in number of bytes from the beginning of the
	 *		  buffer object's memory.
	 * @param iSize The number of byte to be copied to the buffer object.
	 * @param pData A pointer to the data to be copied.
	 * @return Returns 'true' iff the command completed successfully.
	 *
	 * NOTE: This function must _not_ be used before the buffer object has been
	 *		 init'ed via the function BufferData(). See that function for more
	 *		 details (and side effects of this function).
	 * @sa BufferData()
	 */
	bool BufferSubData( GLintptr iOffset, GLsizeiptr iSize,
						const void* pData ) const;
	/**
	 * Copies a sub-interval of the buffer object's memory to the host system.
	 *
	 * @param iOffset Offset in num of bytes from the beginning of the memory.
	 * @param nSizeInBytes Number of bytes to be copied.
	 * @param pTarget A pointer to the host memory to where data is copied.
	 * @return Returns 'true' if the copy command was completed successfully.
	 */
	bool RetrieveBufferSubData( GLintptr iOffset, GLsizeiptr nSizeInBytes,
								void* pTarget ) const;

	/**
	 * Returns a pointer to the beginning of the buffer's memory on the GPU.
	 * 
	 * @param uiAccess The access mode desired: only write to the GPU memory
	 *		  (GL_WRITE_ONLY), only read from it (GL_READ_ONLY), or read & write
	 *		  to it (GL_READ_WRITE). Read & write mode is the default.
	 * @return Returns NULL if an error occured, e.g. the buffer could not
	 *		   be mapped. Otherwise, the address to the beginning of the
	 *		   memory buffer on the GPU is returned.
	 */
	void* MapBuffer( GLenum eAccess = GL_READ_WRITE );
	/**
	 * @return Returns 'true' iff the buffer object is not mapped after the
	 *
	 * NOTE: The buffer object _must_ be bound to the same target as it was
	 *		 during mapping in order to successfully unmap it.
	 */
	bool UnmapBuffer();

	//! Can be used to check the state of the buffer object.
	bool GetIsValid() const;
	/**
	 * Returns the size of the buffer object (in bytes).
	 *
	 * @return Buffer object's size in byte or -1 on error.
	 */
	std::size_t GetSizeInBytes() const;
	/**
	 * Returns the usage mode of the buffer object (iff valid).
	 *
	 * @return Usage mode as GLenum or -1 on error.
	 */
	int GetUsageMode() const;
	/**
	 * Returns the access mode of the buffer object (iff mapped).
	 * 
	 * @return Access mode as GLenum or -1 on error.
	 */
	int GetAccessMode() const;
	//! Returns whether the buffer object is mapped or not.
	bool GetIsMapped() const;
	//! Returns the address to buffer object's memory (iff mapped).
	void* GetMapPointer() const;
	/**
	 * Returns the target to which the buffer object was bound as OpenGL enum.
	 *
	 * @return Bind target as GLenum or '0' if not mapped.
	 */
	int GetBindTarget() const;
	//! Returns the target to which the mapped object was bound as OpenGL enum.
	/**
	 * @return Map target as GLenum or '0' if not mapped.
	 */
	int GetMapTarget() const;

	/**
	 * Returns the handle to the OpenGL Buffer Object.
	 * Refrain from using it, unless you really have a reason to. 
	 */
	GLuint GetBufferHandle() const;

private:
	GLuint				m_uiBufferHnd;
	GLenum				m_eBindTarget;
	GLenum				m_eMapTarget;
	mutable GLsizeiptr	m_nSizeInBytes;
};

#endif // Include guard.


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
