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


#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>

#include "VistaVertexArrayObject.h"
#include "VistaBufferObject.h"
#include "VistaRenderToVertexArray.h"

#include <cassert>


GLuint VistaVertexArrayObject::s_uiCurrentlyBoundVAO = 0;

VistaVertexArrayObject::VistaVertexArrayObject()
:	m_uiObjectId( 0 )
,	m_bIsIndexBufferObjectSpecifed(false)
,	m_eIndexBufferObjectType( GL_UNSIGNED_INT )
{
	glGenVertexArrays( 1, &m_uiObjectId );
	
	GLint iMaxVertexAttribs = -1;
	glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &iMaxVertexAttribs );
	assert( iMaxVertexAttribs != -1 );
	m_AttribEnableState.resize( static_cast<size_t>( iMaxVertexAttribs ), false );
}

VistaVertexArrayObject::~VistaVertexArrayObject()
{
	if( m_uiObjectId == s_uiCurrentlyBoundVAO )
		Release();
	glDeleteVertexArrays( 1, &m_uiObjectId );
}


void VistaVertexArrayObject::Bind()
{
	if( s_uiCurrentlyBoundVAO == m_uiObjectId )
		return; // already bound

	if(s_uiCurrentlyBoundVAO != 0)
	{
		vstr::warnp() << "[VistaVertexArrayObject] An other VAO is currently bound!" << std::endl;
		vstr::warni() << "Old bind state will be overwritten!" << std::endl;
	}

	glBindVertexArray( m_uiObjectId );
	s_uiCurrentlyBoundVAO = m_uiObjectId;
}

void VistaVertexArrayObject::Release()
{
	if( s_uiCurrentlyBoundVAO != m_uiObjectId )
	{
		vstr::warnp() << "[VistaVertexArrayObject] VAO, that should be released,"
			<< " is not the current bound VAO!"<< std::endl;
	}

	glBindVertexArray( 0 );
	s_uiCurrentlyBoundVAO = 0;
}


void VistaVertexArrayObject::EnableAttributeArray( const GLuint uiIndex )
{
	FastBind();
	glEnableVertexAttribArray( uiIndex );
	m_AttribEnableState[uiIndex] = true;
	FastRelease();
}

void VistaVertexArrayObject::DisableAttributeArray( const GLuint uiIndex )
{
	FastBind();
	glDisableVertexAttribArray( uiIndex );
	m_AttribEnableState[uiIndex] = false;
	FastRelease();
}

bool VistaVertexArrayObject::GetIsAttributeArrayActive( const GLuint uiIndex ) const
{
	return m_AttribEnableState[uiIndex];
}

size_t VistaVertexArrayObject::GetMaxNumberOfAttributeArrays() const
{
	return m_AttribEnableState.size();
}


void VistaVertexArrayObject::SpecifyAttributeArrayFloat( const GLuint uiIndex,
	const GLint iNumComponents, const GLenum eDataType,
	const GLboolean bNormalizeInteger, const GLsizei nStride,
	GLuint uiDataOffset, VistaBufferObject* pBufferObject )
{
	FastBind();
	pBufferObject->Bind( GL_ARRAY_BUFFER );
	glVertexAttribPointer( uiIndex, iNumComponents, eDataType,
		bNormalizeInteger, nStride, reinterpret_cast<GLvoid*>( uiDataOffset ) );
	pBufferObject->Release();
	FastRelease();
}

void VistaVertexArrayObject::SpecifyAttributeArrayInteger( const GLuint uiIndex,
	const GLint iNumComponents, const GLenum eDataType, const GLsizei nStride,
	GLuint uiDataOffset, VistaBufferObject* pBufferObject )
{
	FastBind();
	pBufferObject->Bind( GL_ARRAY_BUFFER );
	glVertexAttribIPointer( uiIndex, iNumComponents, eDataType, nStride,
		reinterpret_cast<GLvoid*>( uiDataOffset ) );
	pBufferObject->Release();
	FastRelease();
}

void VistaVertexArrayObject::SpecifyAttributeArrayDouble( const GLuint uiIndex,
	const GLint iNumComponents, const GLenum eDataType, const GLsizei nStride,
	GLuint uiDataOffset, VistaBufferObject* pBufferObject )
{
	FastBind();
	pBufferObject->Bind( GL_ARRAY_BUFFER );
	glVertexAttribLPointer( uiIndex, iNumComponents, eDataType, nStride,
		reinterpret_cast<GLvoid*>( uiDataOffset ) );
	pBufferObject->Release();
	FastRelease();
}

void VistaVertexArrayObject::SpecifyAttributeArrayFloat( 
	const GLuint uiIndex, const GLint iNumComponents, 
	const GLboolean bNormalizeInteger, const GLsizei nStride,
	GLuint uiDataOffset, VistaRenderToVertexArray* pBufferObject )
{
	FastBind();
	pBufferObject->Bind( );
	glVertexAttribPointer( uiIndex, iNumComponents, pBufferObject->GetType(),
		bNormalizeInteger, nStride, reinterpret_cast<GLvoid*>( uiDataOffset ) );
	pBufferObject->Release();
	FastRelease();
}

void VistaVertexArrayObject::SpecifyAttributeArrayInteger( 
	const GLuint uiIndex, const GLint iNumComponents,  const GLsizei nStride,
	GLuint uiDataOffset, VistaRenderToVertexArray* pBufferObject )
{
	FastBind();
	pBufferObject->Bind( );
	glVertexAttribIPointer( uiIndex, iNumComponents, pBufferObject->GetType(),
		nStride, reinterpret_cast<GLvoid*>( uiDataOffset ) );
	pBufferObject->Release();
	FastRelease();
}

void VistaVertexArrayObject::SpecifyAttributeArrayDouble(
	const GLuint uiIndex, const GLint iNumComponents, const GLsizei nStride,
	GLuint uiDataOffset, VistaRenderToVertexArray* pBufferObject )
{

	FastBind();
	pBufferObject->Bind( );
	glVertexAttribLPointer( uiIndex, iNumComponents, pBufferObject->GetType(),
		nStride, reinterpret_cast<GLvoid*>( uiDataOffset ) );
	pBufferObject->Release();
	FastRelease();
}

void VistaVertexArrayObject::SpecifyIndexBufferObject( 
	VistaBufferObject* pIBO, GLenum eType )
{
	FastBind();
	if( pIBO )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pIBO->GetBufferHandle() );
		m_bIsIndexBufferObjectSpecifed = true;
		m_eIndexBufferObjectType = eType;
	}
	else
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 
		m_bIsIndexBufferObjectSpecifed = false;
	}
	FastRelease();
}

bool VistaVertexArrayObject::GetIsIndexBufferObjectSpecifed() const
{
	return m_bIsIndexBufferObjectSpecifed;
}

GLenum VistaVertexArrayObject::GetIndexBufferObjectType() const
{
	return m_eIndexBufferObjectType;
}

void VistaVertexArrayObject::FastBind()
{
	if( s_uiCurrentlyBoundVAO != m_uiObjectId )
		glBindVertexArray( m_uiObjectId );
}

void VistaVertexArrayObject::FastRelease()
{
	if( s_uiCurrentlyBoundVAO != m_uiObjectId )
		glBindVertexArray( s_uiCurrentlyBoundVAO );
}
