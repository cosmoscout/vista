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
#include "VistaRenderToVertexArray.h"
#include "VistaBufferObject.h"
#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
#include <cassert>


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaRenderToVertexArray::VistaRenderToVertexArray( int iVertexCount,
		int iComponents, GLenum eType)
:	m_pVertexArray( new VistaBufferObject() )
,	m_iVertexCount( iVertexCount )
,	m_iComponents( iComponents )
,	m_eType( eType )
,	m_eFormat( GL_RGBA )
,	m_eInternalFormat( GL_RGBA )
,	m_iBytesPerComponent( 0 )
,	m_eUsage( GL_STREAM_COPY )
{
	if( !GLEW_ARB_vertex_buffer_object )
	{
		vstr::errp() << "[VistaRenderToVertexArray]"
					 << " Missing ARB_vertex_buffer_object extension..."
					 << std::endl;
		return;
	}

	if( !GLEW_ARB_pixel_buffer_object )
	{
		vstr::warnp() << "[VistaRenderToVertexArray]"
					  << " Missing ARB_pixel_buffer_object extension..."
					  << std::endl;

		if( !GLEW_EXT_pixel_buffer_object )
		{
			vstr::errp() << "[VistaRenderToVertexArray]"
						 << " Missing EXT_pixel_buffer_object extension, too..."
						 << std::endl;
			return;
		}
		
		vstr::warni() << "\t\tUsing EXT_pixel_buffer_object extension..."
					  << std::endl;
	}

	switch( m_eType )
	{
	case GL_UNSIGNED_BYTE:
	case GL_BYTE:		m_iBytesPerComponent = 1;				break;
	case GL_HALF_FLOAT: m_iBytesPerComponent = 2;				break;
	case GL_FLOAT:		m_iBytesPerComponent = sizeof( float ); break;
	default:
		vstr::errp() << "[VistaRenderToVertexArray] Unsupported data type: "
					 << eType << std::endl;
		return;
	}

	switch( m_iComponents )
	{
	case 1:
		m_eFormat = GL_LUMINANCE;
		if( m_iBytesPerComponent == 1 )
			m_eInternalFormat = GL_LUMINANCE;
		else if( m_iBytesPerComponent == 2 )
			m_eInternalFormat = GL_LUMINANCE16F_ARB;
		else
			m_eInternalFormat = GL_LUMINANCE32F_ARB;
		break;
	case 2:
		m_eFormat = GL_LUMINANCE_ALPHA;
		if( m_iBytesPerComponent == 1 )
			m_eInternalFormat = GL_LUMINANCE_ALPHA;
		else if( m_iBytesPerComponent == 2 )
			m_eInternalFormat = GL_LUMINANCE_ALPHA16F_ARB;
		else
			m_eInternalFormat = GL_LUMINANCE_ALPHA32F_ARB;
		break;
	case 3:
		m_eFormat = GL_RGB;
		if( m_iBytesPerComponent == 1 )
			m_eInternalFormat = GL_RGB;
		else if( m_iBytesPerComponent == 2 )
			m_eInternalFormat = GL_RGB16F;
		else
			m_eInternalFormat = GL_RGB32F;
		break;
	case 4:
		m_eFormat = GL_RGBA;
		if( m_iBytesPerComponent == 1 )
			m_eInternalFormat = GL_RGBA;
		else if( m_iBytesPerComponent == 2 )
			m_eInternalFormat = GL_RGBA16F;
		else
			m_eInternalFormat = GL_RGBA32F;
		break;
	default:
		vstr::errp() << "[VistaRenderToVertexArray]"
					 << " Invalid component count: "
					 << iComponents << std::endl;
		return;
	}

	if( m_iVertexCount <= 0 )
	{
		vstr::errp() << "[VistaRenderToVertexArray]"
					 << " Invalid vertex count: "
					 << m_iVertexCount << std::endl;
		return;
	}

	m_pVertexArray->BindAsPixelPackBuffer();
	m_pVertexArray->BufferData(
		m_iVertexCount * m_iBytesPerComponent * m_iComponents, 0, m_eUsage );
	m_pVertexArray->Release();
}

VistaRenderToVertexArray::~VistaRenderToVertexArray()
{
	delete m_pVertexArray;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
void VistaRenderToVertexArray::SetData( void* pData )
{
	m_pVertexArray->BindAsPixelPackBuffer();
	m_pVertexArray->BufferData( 
		m_iVertexCount * m_iBytesPerComponent * m_iComponents, pData, m_eUsage );
	m_pVertexArray->Release();
}

void VistaRenderToVertexArray::GetData( void *pData )
{
	m_pVertexArray->BindAsPixelPackBuffer();
	m_pVertexArray->RetrieveBufferSubData(
		0, m_iVertexCount * m_iBytesPerComponent * m_iComponents, pData );
	m_pVertexArray->Release();
}


void VistaRenderToVertexArray::Read( GLenum eBuffer, int iWidth, int iHeight )
{
	m_pVertexArray->BindAsPixelPackBuffer();
	glReadBuffer( eBuffer );
	glReadPixels( 0, 0, iWidth, iHeight, m_eFormat, m_eType, 0 );
	m_pVertexArray->Release();
}

void VistaRenderToVertexArray::Read( GLenum eBuffer, int iX, int iY, int iW,
		int iH, int iOffset )
{
	m_pVertexArray->BindAsPixelPackBuffer();
	glReadBuffer( eBuffer );
	glReadPixels( iX, iY, iW, iH, m_eFormat, m_eType, BUFFER_OFFSET( iOffset ) );
	m_pVertexArray->Release();
}


void VistaRenderToVertexArray::Bind()
{
	m_pVertexArray->BindAsVertexDataBuffer();
}

void VistaRenderToVertexArray::Release()
{
	m_pVertexArray->Release();
}


GLuint VistaRenderToVertexArray::GetId() const
{
	return m_pVertexArray->GetBufferHandle();
}


std::size_t VistaRenderToVertexArray::GetSize() const
{
	assert( m_iVertexCount * m_iBytesPerComponent * m_iComponents ==
			m_pVertexArray->GetSizeInBytes() );
	return m_pVertexArray->GetSizeInBytes();
}


int VistaRenderToVertexArray::GetVertexCount() const
{
	return m_iVertexCount;
}

int VistaRenderToVertexArray::GetComponentCount() const
{
	return m_iComponents;
}

GLenum VistaRenderToVertexArray::GetType() const
{
	return m_eType;
}

GLenum VistaRenderToVertexArray::GetFormat() const
{
	return m_eFormat;
}

GLenum VistaRenderToVertexArray::GetInternalFormat() const
{
	return m_eInternalFormat;
}

int VistaRenderToVertexArray::GetBytesPerComponent() const
{
	return m_iBytesPerComponent;
}

bool VistaRenderToVertexArray::IsSupported() const
{
	return ( GLEW_ARB_vertex_buffer_object 
		&& ( GLEW_ARB_pixel_buffer_object || GLEW_EXT_pixel_buffer_object ) ) 
		? true : false;
}


/*============================================================================*/
/*  END OF FILE								                                  */
/*============================================================================*/
