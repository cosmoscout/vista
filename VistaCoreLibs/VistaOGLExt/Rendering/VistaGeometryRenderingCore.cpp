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

#include "VistaGeometryData.h"
#include "VistaGeometryRenderingCore.h"

#include <VistaOGLExt/VistaGLSLShader.h>
#include <VistaOGLExt/VistaVertexArrayObject.h>
#include <VistaOGLExt/VistaTexture.h>
#include <VistaOGLExt/VistaShaderRegistry.h>

VistaGLSLShader* VistaGeometryRenderingCore::m_pDefaultShader = NULL;

VistaGeometryRenderingCore::VistaGeometryRenderingCore()
	:	m_pData( NULL )
	,	m_pSurfaceShader( NULL )
	,	m_pLineSchader( NULL )
	,	m_oSurfaceColor( VistaColor::RED )
	,	m_oLineColor( VistaColor::BLACK )
	,	m_bOwnTextures( false )
{ }

VistaGeometryRenderingCore::~VistaGeometryRenderingCore()
{
	if(m_bOwnTextures)
	{
		for(std::vector<VistaTexture*>::iterator it = m_vecUsedTextures.begin();
			it != m_vecUsedTextures.end(); ++it)
		{
			delete (*it);
		}
	}
}
/******************************************************************************/
void VistaGeometryRenderingCore::SetData( const VistaGeometryData* pData )
{
	m_pData = pData;
}
/******************************************************************************/
void VistaGeometryRenderingCore::SetSurfaceShader( VistaGLSLShader* pShader )
{
	m_pSurfaceShader = pShader;
}
void VistaGeometryRenderingCore::SetLineShader( VistaGLSLShader* pShader )
{
	m_pLineSchader = pShader;
}

VistaGLSLShader* VistaGeometryRenderingCore::GetSurfaceShader() const
{
	return m_pSurfaceShader;
}
VistaGLSLShader* VistaGeometryRenderingCore::GetLineShader() const
{
	return m_pLineSchader;
}

void VistaGeometryRenderingCore::SetSurfaceColor( const VistaColor& rColor )
{
	m_oSurfaceColor = rColor;
}
void VistaGeometryRenderingCore::SetLineColor( const VistaColor& rColor )
{
	m_oLineColor = rColor;
}
const VistaColor& VistaGeometryRenderingCore::GetSurfaceColor() const
{
	return m_oSurfaceColor;
}
const VistaColor& VistaGeometryRenderingCore::GetLineColor() const
{
	return m_oLineColor;
}

void VistaGeometryRenderingCore::SetUsedTextures( const std::vector<VistaTexture*>& vecTuextres,
												  bool bTransferOwnership )
{
	m_vecUsedTextures = vecTuextres;
	m_bOwnTextures = bTransferOwnership;
}

std::vector<VistaTexture*>& VistaGeometryRenderingCore::GetUsedTextures()
{
	return m_vecUsedTextures;
}
/******************************************************************************/
void VistaGeometryRenderingCore::SetUniform( const std::string& strName, float f1)
{
	UpdateUniform( UniformVariable(strName, 1, f1) );
}
void VistaGeometryRenderingCore::SetUniform( const std::string& strName, float f1, float f2)
{
	UpdateUniform( UniformVariable(strName, 2, f1, f2) );
}
void VistaGeometryRenderingCore::SetUniform( const std::string& strName, float f1, float f2, float f3)
{
	UpdateUniform( UniformVariable(strName, 3, f1, f2, f3 ) );
}
void VistaGeometryRenderingCore::SetUniform( const std::string& strName, float f1, float f2, float f3, float f4)
{
	UpdateUniform( UniformVariable(strName, 4, f1, f2, f3, f4 ) );
}
void VistaGeometryRenderingCore::SetUniform( const std::string& strName, int i)
{
	UpdateUniform( UniformVariable(strName, i ) );
}

/******************************************************************************/
void VistaGeometryRenderingCore::Draw()
{
	if( m_pData && m_pData->GetVertexArrayObject() )
	{
		PrepareRendering();
		RenderSurface();
		RenderLines();
		CleanUpRendering();
	}
}

VistaGLSLShader* VistaGeometryRenderingCore::GetDefaultShader()
{
	if(m_pDefaultShader == NULL)
	{
		VistaShaderRegistry& rShaderReg = VistaShaderRegistry::GetInstance();

		std::string strVert = rShaderReg.RetrieveShader("VistaGeometryRenderingCore_vert.glsl");
		std::string strFrag = rShaderReg.RetrieveShader("VistaGeometryRenderingCore_frag.glsl");
		std::string strAux  = rShaderReg.RetrieveShader("Vista_PhongLighting_aux.glsl");

		m_pDefaultShader = new VistaGLSLShader();

		m_pDefaultShader->InitFromStrings( strVert, strFrag );
		m_pDefaultShader->InitFragmentShaderFromString( strAux );

		m_pDefaultShader->Link();
	}
	return m_pDefaultShader;
}

void VistaGeometryRenderingCore::PrepareRendering()
{
	glPrimitiveRestartIndex(~0U);
	glEnable( GL_PRIMITIVE_RESTART );
	glDisable( GL_LIGHTING );

	m_pData->GetVertexArrayObject()->Bind();

	size_t nNumTextures = m_vecUsedTextures.size();
	for( size_t n=0; n<nNumTextures; ++n )
	{
		m_vecUsedTextures[n]->Bind( static_cast<GLenum>(GL_TEXTURE0 + n) );
	}
}

void VistaGeometryRenderingCore::CleanUpRendering()
{
	size_t nNumTextures = m_vecUsedTextures.size();
	for( size_t n=nNumTextures; n>0; --n)
	{
		m_vecUsedTextures[n-1]->Unbind( static_cast<GLenum>(GL_TEXTURE0 + n-1) );
	}

	m_pData->GetVertexArrayObject()->Release();

	glDisable( GL_PRIMITIVE_RESTART );
}

void VistaGeometryRenderingCore::RenderSurface()
{
	if(m_pSurfaceShader != NULL)
	{
		m_pSurfaceShader->Bind();
		RefreshUniformsForShader( m_pSurfaceShader );
	}

	glColor4fv( &m_oSurfaceColor[0] );
	unsigned int uiNumPolygons  = m_pData->GetPolygonCount();
	unsigned int uiNumStrips    = m_pData->GetStripCount();
	unsigned int uiNumTriangles = m_pData->GetTriangleCount();

	void* pPolygonOffset  = ((char*)(0)) + m_pData->GetPolygonOffset();
	void* pStripOffset    = ((char*)(0)) + m_pData->GetStripOffset();
	void* pTriangleOffset = ((char*)(0)) + m_pData->GetTriangleOffset();

	if(uiNumPolygons > 0)
	{
		glDrawElements( GL_POLYGON, uiNumPolygons, GL_UNSIGNED_INT, pPolygonOffset );
	}
	if(uiNumStrips > 0)
	{
		glDrawElements( GL_TRIANGLE_STRIP, uiNumStrips, GL_UNSIGNED_INT, pStripOffset );
	}
	if(uiNumTriangles > 0)
	{
		glDrawElements( GL_TRIANGLES, uiNumTriangles, GL_UNSIGNED_INT, pTriangleOffset );
	}

	if(m_pSurfaceShader != NULL)
		m_pSurfaceShader->Release();
}

void VistaGeometryRenderingCore::RenderLines()
{
	unsigned int uiNumLines = m_pData->GetLineCount();
	void* pLineOffset = ((char*)(0)) + m_pData->GetLineOffset();

	if( uiNumLines == 0 )
		return;

	glColor4fv( &m_oLineColor[0] );

	if( m_pLineSchader != NULL )
	{
		m_pLineSchader->Bind();
		RefreshUniformsForShader( m_pLineSchader );
	}
	
	glDrawElements( GL_LINE_STRIP, uiNumLines, GL_UNSIGNED_INT, pLineOffset );

	if( m_pLineSchader != NULL )
		m_pLineSchader->Release();
}

void VistaGeometryRenderingCore::UpdateUniform( const UniformVariable& rUniform )
{
	bool bDoesAlreadyExist = false;
	for(size_t n=0; n<m_pUniformVariables.size(); ++n)
	{
		if( m_pUniformVariables[n].GetName() == rUniform.GetName() )
		{
			bDoesAlreadyExist = true;
			m_pUniformVariables[n] = rUniform;
		}
	}

	if( !bDoesAlreadyExist )
	{
		m_pUniformVariables.push_back( rUniform );
	}
}

void VistaGeometryRenderingCore::RefreshUniformsForShader( VistaGLSLShader* pShader )
{
	for(size_t n=0; n<m_pUniformVariables.size(); ++n)
	{
		m_pUniformVariables[n].RefreshUniformForShader(pShader);
	}
}

VistaGeometryRenderingCore::UniformVariable::UniformVariable( 
		const std::string& strName, 
		unsigned int uiNumComponents, 
		float f1, float f2, float f3, float f4 
	)
	:	m_strName( strName )
	,	m_uiType(uiNumComponents)
	,	m_iValue( 0 )
{
	m_fValues[0] = f1;
	m_fValues[1] = f2;
	m_fValues[2] = f3;
	m_fValues[3] = f4;
}
VistaGeometryRenderingCore::UniformVariable::UniformVariable( 
	const std::string& strName, 
	int i
	)
	:	m_strName( strName )
	,	m_uiType(0)
	,	m_iValue(i)
{ }

VistaGeometryRenderingCore::UniformVariable::~UniformVariable()
{}

void VistaGeometryRenderingCore::UniformVariable::RefreshUniformForShader( VistaGLSLShader* pShader )
{
	int uiLoc = pShader->GetUniformLocation( m_strName );

	if(uiLoc<0)return;

	switch(m_uiType)
	{
	case 0: pShader->SetUniform(uiLoc, m_iValue ); break;
	case 1: pShader->SetUniform(uiLoc, m_fValues[0] ); break;
	case 2: pShader->SetUniform(uiLoc, m_fValues[0], m_fValues[1] ); break;
	case 3: pShader->SetUniform(uiLoc, m_fValues[0], m_fValues[1], m_fValues[2] ); break;
	case 4: pShader->SetUniform(uiLoc, m_fValues[0], m_fValues[1], m_fValues[2], m_fValues[3] ); break;
	}
}
const std::string& VistaGeometryRenderingCore::UniformVariable::GetName() const
{
	return m_strName;
}
/******************************************************************************/