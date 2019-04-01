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


#ifndef _VistaVertexArrayObject_h
#define _VistaVertexArrayObject_h

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaOGLExtConfig.h"

#include <GL/glew.h>

#if defined(DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <vector>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaBufferObject;
class VistaRenderToVertexArray;


/*============================================================================*/
/* CLASS DEFINITION                                                           */
/*============================================================================*/
/**
 * This class wraps OpenGL's vertex array objects (VAOs).
 */
class VISTAOGLEXTAPI VistaVertexArrayObject
{
public:
	VistaVertexArrayObject();
	~VistaVertexArrayObject();

	void Bind();
	void Release();

	void EnableAttributeArray( const GLuint uiIndex );
	void DisableAttributeArray( const GLuint uiIndex );
	bool GetIsAttributeArrayActive( const GLuint uiIndex ) const;
	std::size_t GetMaxNumberOfAttributeArrays() const;
	
	void SpecifyAttributeArrayFloat( const GLuint uiIndex,
		const GLint iNumComponents, const GLenum eDataType,
		const GLboolean bNormalizeInteger,  const GLsizei nStride,
		GLuint uiDataOffset, VistaBufferObject* pBufferObject );
	void SpecifyAttributeArrayInteger( const GLuint uiIndex,
		const GLint iNumComponents, const GLenum eDataType,
		const GLsizei nStride, GLuint uiDataOffset,
		VistaBufferObject* pBufferObject );
	void SpecifyAttributeArrayDouble( const GLuint uiIndex,
		const GLint iNumComponents, const GLenum eDataType,
		const GLsizei nStride, GLuint uiDataOffset,
		VistaBufferObject* pBufferObject );

	void SpecifyAttributeArrayFloat( const GLuint uiIndex,
		const GLint iNumComponents,
		const GLboolean bNormalizeInteger,  const GLsizei nStride,
		GLuint uiDataOffset, VistaRenderToVertexArray* pBufferObject );
	void SpecifyAttributeArrayInteger( const GLuint uiIndex,
		const GLint iNumComponents,
		const GLsizei nStride, GLuint uiDataOffset,
		VistaRenderToVertexArray* pBufferObject );
	void SpecifyAttributeArrayDouble( const GLuint uiIndex,
		const GLint iNumComponents,
		const GLsizei nStride, GLuint uiDataOffset,
		VistaRenderToVertexArray* pBufferObject );


	void SpecifyIndexBufferObject( 
		VistaBufferObject* pIndexBufferObject, 
		GLenum eType = GL_UNSIGNED_INT );

	bool GetIsIndexBufferObjectSpecifed() const;
	GLenum GetIndexBufferObjectType() const;

private:
	// If FastBind() is used, FastRelease() needs to be called before control
	// returns to user code (or VAO bind state might become corrupt).
	void FastBind();
	void FastRelease();

	GLuint				m_uiObjectId;
	static GLuint		s_uiCurrentlyBoundVAO;
	
	std::vector<bool>	m_AttribEnableState;

	bool	m_bIsIndexBufferObjectSpecifed;
	GLenum	m_eIndexBufferObjectType;
};

#endif
