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
/* INCLUDES																	  */
/*============================================================================*/
#include "VistaABuffer_Pages.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaOGLExt/VistaGLSLShader.h>
#include <VistaOGLExt/VistaShaderRegistry.h>
#include <VistaOGLExt/VistaOGLUtils.h>

/*============================================================================*/
/* DEFINES			                                                          */
/*============================================================================*/
using namespace std;
/*============================================================================*/
/* GLOBAL VARIABLES                                                           */
/*============================================================================*/
const std::string g_strShader_Page_Frag = "VistaABuffer_Pages_frag.glsl";
const std::string g_strShader_Page_Aux  = "VistaABuffer_Pages_aux.glsl";

/*============================================================================*/
/* VistaABufferLinkedList    	                                              */
/*============================================================================*/
/******************************************************************************/
/* CON-/DESTRCUTOR	                                                          */
/******************************************************************************/
VistaABufferPages::VistaABufferPages( unsigned int nPageSize  )
	:	IVistaABuffer()
	,	m_uiPagePointerBuffer( 0 )
	,	m_uiNextPointerBuffer( 0 ) 
	,	m_uiSemaphoreBuffer( 0 )
	,	m_uiFragCountBuffer( 0 )
	,	m_addrPagePointerBuffer( 0 )
	,	m_addrNextPointerBuffer( 0 )
	,	m_addrSemaphoreBuffer( 0 )
	,	m_addrFragCountBuffer( 0 )
	,	m_uiAtomicPageCounter( 0 )
	,	m_nPageSize( nPageSize )
{
	m_uiFragmenstPerPixel = nPageSize;
	m_uiBufferSize	= m_uiFragmenstPerPixel * m_uiWidth * m_uiHeight;
	m_nNumPages		= m_uiBufferSize/m_nPageSize;
}

VistaABufferPages::~VistaABufferPages()
{
	if( m_uiPagePointerBuffer ) glDeleteBuffers( 1, &m_uiPagePointerBuffer );
	if( m_uiNextPointerBuffer ) glDeleteBuffers( 1, &m_uiNextPointerBuffer );
	if( m_uiSemaphoreBuffer   ) glDeleteBuffers( 1, &m_uiSemaphoreBuffer   );
	if( m_uiFragCountBuffer	  ) glDeleteBuffers( 1, &m_uiFragCountBuffer   );
	if( m_uiAtomicPageCounter ) glDeleteBuffers( 1, &m_uiAtomicPageCounter );
}

/******************************************************************************/
/* PUBLIC INTERFACE	                                                          */
/******************************************************************************/
bool VistaABufferPages::ClearABuffer()
{
	ResetAtomicCounter();
	return IVistaABuffer::ClearABuffer();
}

bool VistaABufferPages::SetABufferSize( unsigned int uiWidth, unsigned int uiHeight )
{
	if( m_uiWidth == uiWidth &&  m_uiHeight == uiHeight )
		return false;

	m_uiWidth		= uiWidth;
	m_uiHeight		= uiHeight;
	m_uiBufferSize	= m_uiFragmenstPerPixel * m_uiWidth * m_uiHeight;
	m_nNumPages		= m_uiBufferSize/m_nPageSize;

	IVistaABuffer::ResizeBuffer( 
		m_uiNextPointerBuffer,
		m_addrNextPointerBuffer, 
	    sizeof(unsigned int)*m_nNumPages );

	ResizePerPixelBuffer();
	ResizeDataFileds();
	UpdateUniforms();
	return true;
}

bool VistaABufferPages::SetFragmenstPerPixel( unsigned int uiFragmenstPerPixel )
{
	if( uiFragmenstPerPixel % m_nPageSize != 0 )
	{
		vstr::errp() << "[VistaABufferPages] uiFragmenstPerPixel must be a multiple of nPageSize " << endl;
		return false;
	}

	if( m_uiFragmenstPerPixel == uiFragmenstPerPixel )
		return false;

	m_uiFragmenstPerPixel	= uiFragmenstPerPixel;
	m_uiBufferSize			= uiFragmenstPerPixel * m_uiWidth * m_uiHeight;
	m_nNumPages				= m_uiBufferSize/m_nPageSize;

	IVistaABuffer::ResizeBuffer( m_uiNextPointerBuffer, m_addrNextPointerBuffer, 
	                             sizeof(unsigned int)*m_nNumPages );

	ResizeDataFileds();
	UpdateUniforms();
	return true;
}

VistaGLSLShader* VistaABufferPages::CreateShaderPrototype() const
{
	VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();
	std::string strShaderSource = rShaderReg.RetrieveShader( g_strShader_Page_Aux );

	if( strShaderSource.empty() ) return NULL;

	VistaGLSLShader* pShader = new VistaGLSLShader();

	pShader->InitFragmentShaderFromString( strShaderSource );

	return pShader;
}
/******************************************************************************/
/* PROTECTED INTERFACE	                                                      */
/******************************************************************************/
bool VistaABufferPages::InitShader()
{
#ifdef _DEBUG
	vstr::debugi() << "[VistaABufferPages] INIT SHADER" << endl;
	vstr::IndentObject oIndent;
#endif // _DEBUG

	VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

	std::string strShader;

	strShader = rShaderReg.RetrieveShader( g_strShader_Page_Frag );

	if( strShader.empty() )
	{
		vstr::errp() << "[VistaABufferPages] - required shader not found."  << endl;
		vstr::IndentObject oIndent;
		vstr::erri() << "Can't find " << g_strShader_Page_Frag	<< endl;
		return false;
	}

	m_pClearShader = new VistaGLSLShader();

	m_pClearShader->InitFragmentShaderFromString( strShader );

	return m_pClearShader->Link();
}

bool VistaABufferPages::InitBuffer()
{
	CreateAtomicCounter();

	if( !m_uiPagePointerBuffer ) glGenBuffers( 1, &m_uiPagePointerBuffer );
	if( !m_uiNextPointerBuffer ) glGenBuffers( 1, &m_uiNextPointerBuffer );
	if( !m_uiSemaphoreBuffer   ) glGenBuffers( 1, &m_uiSemaphoreBuffer   );
	if( !m_uiFragCountBuffer   ) glGenBuffers( 1, &m_uiFragCountBuffer   );

	IVistaABuffer::ResizeBuffer( m_uiPagePointerBuffer, m_addrPagePointerBuffer, 
	                             sizeof(unsigned int)*m_uiWidth*m_uiHeight );

	IVistaABuffer::ResizeBuffer( m_uiNextPointerBuffer, m_addrNextPointerBuffer, 
	                             sizeof(unsigned int)*m_nNumPages );

	IVistaABuffer::ResizeBuffer( m_uiSemaphoreBuffer, m_addrSemaphoreBuffer, 
	                             sizeof(unsigned int)*m_uiWidth*m_uiHeight );

	IVistaABuffer::ResizeBuffer( m_uiFragCountBuffer, m_addrFragCountBuffer, 
	                             sizeof(unsigned int)*m_uiWidth*m_uiHeight );

	UpdateUniforms();

	return !VistaOGLUtils::CheckForOGLError( __FILE__, __LINE__ );
}


void VistaABufferPages::ResizePerPixelBuffer()
{
	IVistaABuffer::ResizeBuffer( 
		m_uiPagePointerBuffer,  
		m_addrPagePointerBuffer, 
		sizeof(unsigned int)*m_uiWidth*m_uiHeight );

	IVistaABuffer::ResizeBuffer( 
		m_uiSemaphoreBuffer, 
		m_addrSemaphoreBuffer, 
		sizeof(unsigned int)*m_uiWidth*m_uiHeight );

	IVistaABuffer::ResizeBuffer( 
		m_uiFragCountBuffer, 
		m_addrFragCountBuffer, 
		sizeof(unsigned int)*m_uiWidth*m_uiHeight );
}

/******************************************************************************/
void VistaABufferPages::AssignUniforms( VistaGLSLShader* pShader )
{
	if( pShader==NULL ) return;
	
	unsigned int uiProgram = pShader->GetProgram();

	int aiULoc[8];

	aiULoc[0] = pShader->GetUniformLocation( "g_nWidth"             );
	aiULoc[1] = pShader->GetUniformLocation( "g_nHeight"            );
	aiULoc[2] = pShader->GetUniformLocation( "g_nPageSize"          );
	aiULoc[3] = pShader->GetUniformLocation( "g_nNumPages"          );
	aiULoc[4] = pShader->GetUniformLocation( "g_pPagePointerBuffer" );
	aiULoc[5] = pShader->GetUniformLocation( "g_pNextPointerBuffer" );
	aiULoc[6] = pShader->GetUniformLocation( "g_pSemaphoreBuffer"   );
	aiULoc[7] = pShader->GetUniformLocation( "g_pFragCountBuffer"   );

	if( aiULoc[0] != -1 )
		glProgramUniform1uiEXT( uiProgram, aiULoc[0], m_uiWidth );
	if( aiULoc[1] != -1 )
		glProgramUniform1uiEXT( uiProgram, aiULoc[1], m_uiHeight );
	if( aiULoc[2] != -1 )
		glProgramUniform1uiEXT( uiProgram, aiULoc[2], m_nPageSize );
	if( aiULoc[3] != -1 )
		glProgramUniform1uiEXT( uiProgram, aiULoc[3], m_nNumPages );

	if( aiULoc[4] != -1 )
		glProgramUniformui64NV( uiProgram, aiULoc[4], m_addrPagePointerBuffer );
	if( aiULoc[5] != -1 )
		glProgramUniformui64NV( uiProgram, aiULoc[5], m_addrNextPointerBuffer );
	if( aiULoc[6] != -1 )
		glProgramUniformui64NV( uiProgram, aiULoc[6], m_addrSemaphoreBuffer );
	if( aiULoc[7] != -1 )
		glProgramUniformui64NV( uiProgram, aiULoc[7], m_addrFragCountBuffer );

	for( size_t n = 0; n <m_vecDataFileds.size(); ++n)
	{
		int iULoc = pShader->GetUniformLocation( m_vecDataFileds[n].m_strName );
		if( iULoc != -1 )
			glProgramUniformui64NV( uiProgram, iULoc, m_vecDataFileds[n].m_addrBuffer );
	}
}

#define GL_ATOMIC_COUNTER_BUFFER 0x92C0

void VistaABufferPages::CreateAtomicCounter()
{
	if( !m_uiAtomicPageCounter ) glGenBuffers( 1, &m_uiAtomicPageCounter );
	glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, m_uiAtomicPageCounter );
	glBufferData( GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBufferBase( GL_ATOMIC_COUNTER_BUFFER, 0, m_uiAtomicPageCounter );
}

void VistaABufferPages::ResetAtomicCounter()
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_uiAtomicPageCounter);
	GLuint* ptr = (GLuint*)glMapBufferRange(
		GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
		GL_MAP_WRITE_BIT | 
		GL_MAP_INVALIDATE_BUFFER_BIT | 
		GL_MAP_UNSYNCHRONIZED_BIT);
	ptr[0] = 0;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0); 
}
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
