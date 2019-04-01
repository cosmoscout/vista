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


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaBufferObject.h"
#include "VistaOGLUtils.h"

#include <cassert>


/*============================================================================*/
/* CON-/DESTRUCTOR                                                            */
/*============================================================================*/
VistaBufferObject::VistaBufferObject()
:	m_uiBufferHnd( 0 )
,	m_eBindTarget( 0 )
,	m_eMapTarget( 0 )
,	m_nSizeInBytes( -1 )
{
	// This only reserves the buffer name.
	// The buffer will be created on first bind operation!
	glGenBuffers( 1, &m_uiBufferHnd );
}

VistaBufferObject::~VistaBufferObject()
{
	// Must be ok on delete! But the user has to ensure that since we do not
	// want to fiddle with the OpenGL state here since this leads to
	// unpredictable side effects for the user.
	assert( 0 == GetBindTarget() && 0 == GetMapTarget() );
	glDeleteBuffers( 1, &m_uiBufferHnd );
}


/*============================================================================*/
/* PUBLIC INTERFACE                                                           */
/*============================================================================*/
bool VistaBufferObject::Bind( GLenum eTarget )
{
	if( !Release() )
		return false;

	m_eBindTarget = eTarget;
	glBindBuffer( m_eBindTarget, m_uiBufferHnd );

	return !VistaOGLUtils::CheckForOGLError( __FILE__, __LINE__ );
}

bool VistaBufferObject::Release()
{
	if( 0 == GetBindTarget() )
		return true;

	if( GetIsMapped() )
		UnmapBuffer();

	glBindBuffer( m_eBindTarget, 0 );
	m_eBindTarget = 0;

	return true;
}

bool VistaBufferObject::BindAsVertexDataBuffer()
{
	return Bind( GL_ARRAY_BUFFER );
}

bool VistaBufferObject::BindAsIndexBuffer()
{
	return Bind( GL_ELEMENT_ARRAY_BUFFER );
}

bool VistaBufferObject::BindAsPixelPackBuffer()
{
	return Bind( GL_PIXEL_PACK_BUFFER );
}

bool VistaBufferObject::BindAsPixelUnpackBuffer()
{
	return Bind( GL_PIXEL_UNPACK_BUFFER );
}


bool VistaBufferObject::BindBufferBase( GLenum eTarget, GLuint uiIndex )
{
	glBindBufferBase( eTarget, uiIndex, m_uiBufferHnd );
	const bool bSuccess = !VistaOGLUtils::CheckForOGLError( __FILE__, __LINE__ );
	if( bSuccess )
		m_eBindTarget = eTarget;
	return bSuccess;
}

bool VistaBufferObject::BindBufferRange( GLenum eTarget, GLuint uiIndex,
		GLintptr nOffsetInBytes, GLsizei nLengthInBytes )
{
	glBindBufferRange( eTarget, uiIndex, m_uiBufferHnd, nOffsetInBytes,
					   nLengthInBytes );
	const bool bSuccess = !VistaOGLUtils::CheckForOGLError( __FILE__, __LINE__ );
	if( bSuccess )
		m_eBindTarget = eTarget;
	return bSuccess;
}


bool VistaBufferObject::BufferData( GLsizeiptr nSizeInBytes, const void *pData,
		GLenum eUsage ) const
{
	if( 0 == m_eBindTarget )
		return false;

	glBufferDataARB( m_eBindTarget, nSizeInBytes, pData, eUsage );
	m_nSizeInBytes = nSizeInBytes;
	return true;
}

bool VistaBufferObject::BufferSubData( GLintptr iOffset, GLsizeiptr iSize,
		const void *pData ) const
{
	if( 0 == m_eBindTarget )
		return false;

	glBufferSubData( m_eBindTarget, iOffset, iSize, pData );
	return true;
}

bool VistaBufferObject::RetrieveBufferSubData( GLintptr iOffset,
		GLsizeiptr nSizeInBytes, void* pTarget ) const
{
	if( 0 == m_eBindTarget )
		return false;

	glGetBufferSubData( m_eBindTarget, iOffset, nSizeInBytes, pTarget );
	return true;
}


void* VistaBufferObject::MapBuffer( GLenum eAccess )
{
	if( 0 == GetBindTarget() )
		return 0;

	m_eMapTarget = m_eBindTarget;
	return glMapBuffer( m_eMapTarget, eAccess );
}

bool VistaBufferObject::UnmapBuffer()
{
	if( !GetIsMapped() )
		return true;
	
	const bool bSuccess = GL_TRUE == glUnmapBuffer( GetBindTarget() );
	if( bSuccess )
		m_eMapTarget = 0;
	return bSuccess;
}


bool VistaBufferObject::GetIsValid() const
{
	return ( GL_TRUE == glIsBuffer( m_uiBufferHnd ) );
}

std::size_t VistaBufferObject::GetSizeInBytes() const
{
	return static_cast< std::size_t >( m_nSizeInBytes );
}

int VistaBufferObject::GetUsageMode() const
{	
	if( 0 == m_eBindTarget )
		return -1;

	int iHelper = -1;
	glGetBufferParameteriv( m_eBindTarget, GL_BUFFER_USAGE, &iHelper );
	return iHelper;
}

int VistaBufferObject::GetAccessMode() const
{	
	if( 0 == m_eBindTarget )
		return -1;
	
	int iHelper = -1;
	glGetBufferParameteriv(m_eMapTarget, GL_BUFFER_ACCESS, &iHelper);
	return iHelper;
}

bool VistaBufferObject::GetIsMapped() const
{
	return ( GetMapTarget() != 0 );
}

void* VistaBufferObject::GetMapPointer() const
{
	void *pPtr = 0;
	if( 0 == GetMapTarget() )
		return pPtr;

	glGetBufferPointerv( m_eMapTarget, GL_BUFFER_MAP_POINTER, &pPtr );
	return pPtr;
}

int VistaBufferObject::GetBindTarget() const
{
	return m_eBindTarget;
}

int VistaBufferObject::GetMapTarget() const
{
	return m_eMapTarget;
}

GLuint VistaBufferObject::GetBufferHandle() const
{
	return m_uiBufferHnd;
}


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
