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
/* INCLUDES			                                                          */
/*============================================================================*/
#include "VistaGLSLShader.h"
#include "VistaOGLUtils.h"

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
#include <fstream>
#include <cassert>


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
VistaGLSLShader::VistaGLSLShader()
:	m_uiProgram( 0 )
,	m_bIsReadyForUse( false )
{ }

VistaGLSLShader::~VistaGLSLShader()
{
	Destroy();
}


bool VistaGLSLShader::InitFromFiles(
		const std::string &strVertexShaderFile,
		const std::string &strFragmentShaderFile,
		const std::string &strPrefix )
{
	if(    !InitShaderFromFile( GL_VERTEX_SHADER, strVertexShaderFile, strPrefix )
		|| !InitShaderFromFile( GL_FRAGMENT_SHADER, strFragmentShaderFile, strPrefix ) )
	{
		return false;
	}

	return true;
}

bool VistaGLSLShader::InitFromFiles(
		const std::string &strVertexShaderFile,
		const std::string &strGeometryShaderFile,
		const std::string &strFragmentShaderFile,
		const std::string &strPrefix )
{
	if(    !InitShaderFromFile( GL_VERTEX_SHADER, strVertexShaderFile, strPrefix )
		|| !InitShaderFromFile( GL_GEOMETRY_SHADER, strGeometryShaderFile, strPrefix )
		|| !InitShaderFromFile( GL_FRAGMENT_SHADER, strFragmentShaderFile, strPrefix ) )
	{
		return false;
	}

	return true;
}


bool VistaGLSLShader::InitFromStrings(
		const std::string &strVertexShaderString,
		const std::string &strFragmentShaderString )
{
	if(	   !InitShaderFromString( GL_VERTEX_SHADER, strVertexShaderString )
		|| !InitShaderFromString( GL_FRAGMENT_SHADER,strFragmentShaderString) )
	{
		return false;
	}

	return true;
}

bool VistaGLSLShader::InitFromStrings(
		const std::string &strVertexShaderString,
		const std::string &strGeometryShaderString,
		const std::string &strFragmentShaderString )
{
	if(    !InitShaderFromString( GL_VERTEX_SHADER, strVertexShaderString )
		|| !InitShaderFromString( GL_GEOMETRY_SHADER, strGeometryShaderString )
		|| !InitShaderFromString( GL_FRAGMENT_SHADER, strFragmentShaderString ) )
	{
		return false;
	}

	return true;
}


bool VistaGLSLShader::InitVertexShaderFromFile(
		const std::string &strVertexShaderFile,
		const std::string &strPrefix )
{
	return InitShaderFromFile( GL_VERTEX_SHADER, strVertexShaderFile, strPrefix );
}

bool VistaGLSLShader::InitVertexShaderFromString(
		const std::string &strVertexShaderString )
{
	return InitShaderFromString( GL_VERTEX_SHADER, strVertexShaderString );
}


bool VistaGLSLShader::InitGeometryShaderFromFile(
		const std::string &strGeometryShaderFile,
		const std::string &strPrefix )
{
	return InitShaderFromFile( GL_GEOMETRY_SHADER, strGeometryShaderFile, strPrefix );
}

bool VistaGLSLShader::InitGeometryShaderFromString(
		const std::string &strGeometryShaderString )
{
	return InitShaderFromString( GL_GEOMETRY_SHADER, strGeometryShaderString );
}


bool VistaGLSLShader::InitFragmentShaderFromFile(
		const std::string &strFragmentShaderFile,
		const std::string &strPrefix )
{
	return InitShaderFromFile( GL_FRAGMENT_SHADER, strFragmentShaderFile, strPrefix );
}

bool VistaGLSLShader::InitFragmentShaderFromString(
		const std::string &strFragmentShaderString )
{
	return InitShaderFromString( GL_FRAGMENT_SHADER, strFragmentShaderString );
}

bool VistaGLSLShader::InitComputeShaderFromFile(
    const std::string& strComputeShaderFile, 
    const std::string& strPrefix) 
{
  return InitShaderFromFile( GL_COMPUTE_SHADER, strComputeShaderFile, strPrefix);
}

bool VistaGLSLShader::InitComputeShaderFromString(
    const std::string& strComputeShaderString) 
{
  return InitShaderFromString( GL_COMPUTE_SHADER, strComputeShaderString);
}


bool VistaGLSLShader::InitShaderFromFile( 
		const unsigned int uiShaderTyp,
		const std::string &strShaderFile,
		const std::string &strPrefix /* = "" */ )
{
#ifdef DEBUG
	vstr::debugi() << "[VistaGLSLShader] Initializing shader from file '"
				<< strShaderFile << "'..." << std::endl;
#endif

	std::fstream file( strShaderFile.c_str(), std::ios_base::in | std::ios_base::binary );
	bool bSuccess = false;
	if( file.is_open() )
	{
		// determine file size
		file.seekg( 0, file.end );
		std::size_t nProgramLength = file.tellg();
		file.seekg( 0, file.beg );

		// read the program from file
		char* pProgramString = new char[ nProgramLength+1 ];
		file.read( pProgramString, nProgramLength );
		file.close();
		pProgramString[ nProgramLength ] = '\0';

		std::string strProgramString( strPrefix + pProgramString );
		bSuccess = InitShaderFromString( uiShaderTyp, strProgramString );
		delete [] pProgramString;
	}
	else
	{
		vstr::errp() << "[VistaGLSLShader] unable to find shader file..." << std::endl;
		vstr::IndentObject oIndent;
		vstr::erri() << "file name: " << strShaderFile << std::endl;
	}

	return bSuccess;
}

bool VistaGLSLShader::InitShaderFromString(
		const unsigned int uiShaderTyp,
		const std::string& strShaderString )
{
	// Check for OpenGL 2.0
	if( !GLEW_VERSION_2_0 )
	{
		vstr::errp() << "[VistaGLSLShader] OpenGL 2.0 not supported." << std::endl;
		return false;
	}

	// If geometry shaders are to be used check for them
	if( GL_GEOMETRY_SHADER == uiShaderTyp && !GLEW_VERSION_3_2 && !GLEW_ARB_geometry_shader4 )
	{
		vstr::errp() << "[VistaGLSLShader] Geometry shaders not supported." << std::endl;
		return false;
	}


	if( !m_uiProgram )
	{
		m_uiProgram = glCreateProgram();
#ifdef DEBUG
		vstr::debugi() << "[VistaGLSLShader] Created program " << m_uiProgram 
					   << " ..." << std::endl;
#endif
	}

	GLuint uiShader = glCreateShader( uiShaderTyp );
	switch( uiShaderTyp )
	{
		case GL_VERTEX_SHADER:
			m_vecVertexShader.push_back( uiShader );
			break;
		case GL_TESS_CONTROL_SHADER:
			m_vecTessControlShader.push_back( uiShader );
			break;
		case GL_TESS_EVALUATION_SHADER:
			m_vecTessEvalShader.push_back( uiShader );
			break;
		case GL_GEOMETRY_SHADER:
			m_vecGeometryShader.push_back( uiShader );
			break;
		case  GL_FRAGMENT_SHADER:
			m_vecFragmentShader.push_back( uiShader );
			break;
		case GL_COMPUTE_SHADER:
			m_vecComputeShader.push_back( uiShader );
			break;
		default:
			vstr::errp() << "[VistaGLSLShader] Unknown shader type "
				         << uiShaderTyp << " ..." << std::endl;
			return false;
	}
	
	const char *pSource = strShaderString.c_str();
	glShaderSource( uiShader, 1, &pSource, NULL );
	glCompileShader( uiShader );

	GLint iCompileSuccess = 0;
	glGetShaderiv( uiShader, GL_COMPILE_STATUS, &iCompileSuccess );

#ifdef DEBUG
	if(iCompileSuccess != GL_TRUE) 
	{
		vstr::errp() << "Failed to compile shader program:" << std::endl;
		vstr::debug() << pSource << std::endl;

		int iLogSize;
		glGetShaderiv( uiShader, GL_INFO_LOG_LENGTH, &iLogSize );

		if (iLogSize > 0)
		{
			char *sLog = new char[iLogSize];
			glGetShaderInfoLog(uiShader, iLogSize, NULL, sLog);

			vstr::debug() << "glCompileShader error messages:" << std::endl;
			vstr::debug() << sLog << std::endl;
			delete[] sLog;
		}
		
	}
#endif
	glAttachShader( m_uiProgram, uiShader );

	return true;
}


bool VistaGLSLShader::SetGeometryShaderInputType( int iInputType )
{
	if( m_vecGeometryShader.empty() )
		return false;

	glProgramParameteri( m_uiProgram, GL_GEOMETRY_INPUT_TYPE, iInputType );
	return true;
}

bool VistaGLSLShader::SetGeometryShaderOutputType(int iOutputType)
{
	if( m_vecGeometryShader.empty() )
		return false;

	glProgramParameteri( m_uiProgram, GL_GEOMETRY_OUTPUT_TYPE, iOutputType );
	return true;
}

bool VistaGLSLShader::SetGeometryShaderVertexOutputCount( int iCount )
{
	if( m_vecGeometryShader.empty() )
		return false;

	int iMaxOutputVertices = -1;
	glGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES, &iMaxOutputVertices );
	if( iCount > iMaxOutputVertices )
	{
		vstr::errp() << "[VistaGLSLShader] Geometry shader vertex output"
					 << " count too high ( count: " << iCount << " )." << std::endl;
		vstr::IndentObject oIndent;
		vstr::erri() << "Maximum supported vertex output: "
				     << iMaxOutputVertices << std::endl;
		return false;
	}

	glProgramParameteri( m_uiProgram, GL_GEOMETRY_VERTICES_OUT, iCount );
	return true;
}


bool VistaGLSLShader::Link()
{
#ifdef DEBUG
	vstr::debugi() << "[VistaGLSLShader] Linking shaders ..." << std::endl;
#endif

	m_bIsReadyForUse = false;

	if( !m_uiProgram )
	{
		vstr::IndentObject oIndent;
		vstr::err() << "Failed. Shader program not created, yet." << std::endl;
		return false;
	}

	glLinkProgram( m_uiProgram );

	GLint iLinkSuccess = -1;
	glGetProgramiv( m_uiProgram, GL_LINK_STATUS, &iLinkSuccess );

	if( iLinkSuccess == GL_FALSE )
	{
		vstr::errp() << "[VistaGLSLShader] Error while linking shaders."
			<< " See below for details." << std::endl;
		
		// Indent detailed error output a bit.
		vstr::IndentObject oIndent;
		
		char buf[4096];
		GLsizei iSize = -1;
		
		glGetProgramInfoLog( m_uiProgram, 4096, &iSize, buf );
		vstr::erri() << "[VistaGLSLShader] Program info log:" << std::endl;
		vstr::out() << buf << std::endl << std::endl;
				
		for( size_t i=0; i<m_vecVertexShader.size(); ++i )
		{
			glGetShaderInfoLog( m_vecVertexShader[i], 4096, &iSize, buf );
			vstr::erri() << "[VistaGLSLShader] Vertex shader #" <<  i 
					<< " info log: " << std::endl;
			vstr::out() << buf << std::endl;
		}

		for( size_t i=0; i<m_vecTessControlShader.size(); ++i )
		{
			glGetShaderInfoLog( m_vecTessControlShader[i], 4096, &iSize, buf );
			vstr::erri() << "[VistaGLSLShader] Tesselation control shader #" <<  i 
					<< " info log: " << std::endl;
			vstr::out() << buf << std::endl;
		}

		for( size_t i=0; i<m_vecTessEvalShader.size(); ++i )
		{
			glGetShaderInfoLog( m_vecTessEvalShader[i], 4096, &iSize, buf );
			vstr::erri() << "[VistaGLSLShader] Tesselation evaluation shader #" <<  i 
				<< " info log: " << std::endl;
			vstr::out() << buf << std::endl;
		}
		
		for( size_t i=0; i<m_vecGeometryShader.size(); ++i )
		{
			glGetShaderInfoLog( m_vecGeometryShader[i], 4096, &iSize, buf );
			vstr::erri() << "[VistaGLSLShader] Geometry shader #" <<  i
					<< " info log: " << std::endl;
			vstr::out() << buf << std::endl;
		}

		for(size_t i=0; i<m_vecFragmentShader.size(); ++i)
		{
			glGetShaderInfoLog( m_vecFragmentShader[i], 4096, &iSize, buf );
			vstr::erri() << "[VistaGLSLShader] Fragment shader #" << i
					<< " info log: " << std::endl;
			vstr::out() << buf << std::endl;
		}

    for (size_t i = 0; i < m_vecComputeShader.size(); ++i) {
      glGetShaderInfoLog( m_vecComputeShader[i], 4096, &iSize, buf );
      vstr::erri() << "[VistaGLSLShader] Compute shader #" << i 
          << " info log: " << std::endl;
      vstr::out() << buf << std::endl;
    }
	}
	else
	{
		m_bIsReadyForUse = true;
	}

#ifdef DEBUG
	vstr::debugi() << "[VistaGLSLShader] - Linking shader progam " << std::flush;
	if( m_bIsReadyForUse )
		vstr::debug() << "succeeded." << std::endl;
	else
		vstr::err() << "FAILED!" << std::endl;
#endif

	MapUniforms();
	MapUniformBlocks();

	return m_bIsReadyForUse;
}


void VistaGLSLShader::MapUniforms()
{
	m_Uniforms.clear();
	int iNumUniforms, iMaxNameLength;

	glGetProgramiv( m_uiProgram, GL_ACTIVE_UNIFORMS, &iNumUniforms );
	glGetProgramiv( m_uiProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &iMaxNameLength );

	if( iNumUniforms == 0 || iMaxNameLength == 0 )
		return;

	GLchar* aName = new GLchar[ iMaxNameLength ];
	GLsizei iLength;
	GLint iSize;
	GLenum iType;

	for( int i=0; i<iNumUniforms; ++i )
	{
		glGetActiveUniform( m_uiProgram, static_cast<GLuint>( i ), 
			iMaxNameLength, &iLength, &iSize, &iType, aName );
		const GLint iLocation = glGetUniformLocation( m_uiProgram, aName );
		m_Uniforms[ std::string( aName ) ] = iLocation;
	}
	delete [] aName;
}


void VistaGLSLShader::MapUniformBlocks()
{
	m_UniformBlocks.clear();
	int iNumUniformBlocks, iMaxNameLength;

	glGetProgramiv( m_uiProgram, GL_ACTIVE_UNIFORM_BLOCKS, &iNumUniformBlocks );
	glGetProgramiv( m_uiProgram, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &iMaxNameLength );

	if( iNumUniformBlocks == 0 || iMaxNameLength == 0 )
		return;

	GLchar* aName = new GLchar[ iMaxNameLength ];
	GLsizei iLength;
		
	for( int i=0; i<iNumUniformBlocks; ++i )
	{
		glGetActiveUniformBlockName( m_uiProgram, static_cast<GLuint>( i ),
			iMaxNameLength, &iLength, aName );
		const GLuint uiIndex = glGetUniformBlockIndex( m_uiProgram, aName );
		m_UniformBlocks[ std::string( aName ) ] = uiIndex;		
	}

	delete [] aName;
}



void VistaGLSLShader::Destroy()
{
	for( size_t i=0; i<m_vecVertexShader.size(); ++i )
		glDeleteShader( m_vecVertexShader[i] );
	for( size_t i=0; i<m_vecTessControlShader.size(); ++i )
		glDeleteShader( m_vecTessControlShader[i] );
	for( size_t i=0; i<m_vecTessEvalShader.size(); ++i )
		glDeleteShader( m_vecTessEvalShader[i] );
	for( size_t i=0; i<m_vecGeometryShader.size(); ++i )
		glDeleteShader( m_vecGeometryShader[i] );
	for( size_t i=0; i<m_vecFragmentShader.size(); ++i )
		glDeleteShader( m_vecFragmentShader[i] );
	for (size_t i = 0; i<m_vecComputeShader.size(); ++i)
		glDeleteShader( m_vecComputeShader[i] );
	if( m_uiProgram )
		glDeleteProgram( m_uiProgram );
	m_bIsReadyForUse = false;
	
	// This is here for calls from user-code, i.e. for resetting the shader
	m_Uniforms.clear();
	m_UniformBlocks.clear();
}


void VistaGLSLShader::Bind()
{
	if( !m_bIsReadyForUse )
	{
		vstr::warnp() << "Shader program with id " << m_uiProgram
			<< " is not ready for use. Bind has no effect." << std::endl;
	}
	
	if( m_bIsReadyForUse )
		glUseProgram( m_uiProgram );
}

void VistaGLSLShader::Release()
{
	glUseProgram( 0 );
}


GLuint VistaGLSLShader::GetProgram() const
{
	return m_uiProgram;
}


GLuint VistaGLSLShader::GetVertexShader( const std::size_t nIdx ) const
{
	return ( nIdx >= m_vecVertexShader.size() ?
			 0 : m_vecVertexShader[ nIdx ] );
}

GLuint VistaGLSLShader::GetTessControlShader( const std::size_t nIdx ) const
{
	return ( nIdx >= m_vecTessControlShader.size() ?
			 0 : m_vecTessControlShader[ nIdx ] );
}

GLuint VistaGLSLShader::GetTessEvalShader( const std::size_t nIdx ) const
{
	return ( nIdx >= m_vecTessEvalShader.size() ?
			 0 : m_vecTessEvalShader[ nIdx ] );
}

GLuint VistaGLSLShader::GetGeometryShader( const std::size_t nIdx ) const
{
	return ( nIdx >= m_vecGeometryShader.size() ?
			 0 : m_vecGeometryShader[ nIdx ] );
}

GLuint VistaGLSLShader::GetFragmentShader( const std::size_t nIdx ) const
{
	return ( nIdx >= m_vecFragmentShader.size() ?
			 0 : m_vecFragmentShader[ nIdx ] );
}

GLuint VistaGLSLShader::GetComputeShader( const std::size_t nIdx ) const 
{
	return (nIdx >= m_vecComputeShader.size() ? 
			 0 : m_vecComputeShader[ nIdx ] );
}

int VistaGLSLShader::GetUniformLocation( const std::string &strParameterName ) const
{
	std::map<std::string, GLint>::const_iterator cit =
			m_Uniforms.find( strParameterName );

	if( cit != m_Uniforms.end() )
		return (*cit).second;
	else
		return -1;
}

void VistaGLSLShader::SetUniform( int iLocation, float fUniform1 )
{
	glUniform1f( iLocation, fUniform1 );
}

void VistaGLSLShader::SetUniform( int iLocation, float fUniform1,
		float fUniform2 )
{
	glUniform2f( iLocation, fUniform1, fUniform2 );
}

void VistaGLSLShader::SetUniform( int iLocation, float fUniform1,
		float fUniform2, float fUniform3 )
{
	glUniform3f( iLocation, fUniform1, fUniform2, fUniform3 );
}

void VistaGLSLShader::SetUniform( int iLocation, float fUniform1,
		float fUniform2, float fUniform3, float fUniform4 )
{
	glUniform4f( iLocation, fUniform1, fUniform2, fUniform3, fUniform4 );
}

void VistaGLSLShader::SetUniform( int iLocation, float* pUniform)
{
	glUniform4fv( iLocation, 1, pUniform );
}

void VistaGLSLShader::SetUniform( int iLocation, int iUniform1 )
{
	glUniform1i( iLocation, iUniform1 );
}

bool VistaGLSLShader::SetUniform( const std::string& strName, int iUniform1 )
{
	const int iLoc = GetUniformLocation( strName );
	if( iLoc == -1 )
		return false;

	glUniform1i( iLoc, iUniform1 );
	return true;
}

void VistaGLSLShader::SetUniform(int iLocation, int iCount, const float* fUniform,bool bTranspose)
{
	glUniformMatrix4fv(iLocation,  iCount,  bTranspose,  fUniform);
}
	

void VistaGLSLShader::SetUniform( int iLocation, int iComponents, int iCount,
		const float *pData )
{
	switch( iComponents )
	{
	case 1:
		glUniform1fv( iLocation, iCount, pData );
		break;
	case 2:
		glUniform2fv( iLocation, iCount, pData );
		break;
	case 3:
		glUniform3fv( iLocation, iCount, pData );
		break;
	case 4:
		glUniform4fv( iLocation, iCount, pData );
		break;
	default:
		assert( false );
		break;
	}
}

void VistaGLSLShader::SetUniform( int iLocation, int iComponents, int iCount,
		const int *pData )
{
	switch( iComponents )
	{
	case 1:
		glUniform1iv( iLocation, iCount, pData );
		break;
	case 2:
		glUniform2iv( iLocation, iCount, pData );
		break;
	case 3:
		glUniform3iv( iLocation, iCount, pData );
		break;
	case 4:
		glUniform4iv( iLocation, iCount, pData );
		break;
	default:
		assert( false );
		break;
	}
}


GLuint VistaGLSLShader::GetUniformBlockIndex(
		const std::string& strBlockName ) const
{
	UniformBlockC::const_iterator cit = m_UniformBlocks.find( strBlockName );
	return( cit != m_UniformBlocks.end() ? cit->second : GL_INVALID_INDEX );
}

bool VistaGLSLShader::SetUniformBlockBinding( GLuint uiBlockIndex,
		GLuint uiBindPoint )
{
	glUniformBlockBinding( m_uiProgram, uiBlockIndex, uiBindPoint );
	return !VistaOGLUtils::CheckForOGLError( __FILE__, __LINE__ );
}

bool VistaGLSLShader::SetUniformBlockBinding( const std::string& strBlockName,
		GLuint uiBindPoint )
{
	const GLuint uiIndex = GetUniformBlockIndex( strBlockName );
	return SetUniformBlockBinding( uiIndex, uiBindPoint );
}

bool VistaGLSLShader::GetUniformBlockBinding( GLuint uiBlockIndex,
		GLuint& uiBindPoint )
{
	GLint iResult = -1;
	glGetActiveUniformBlockiv( m_uiProgram, uiBlockIndex,
			GL_UNIFORM_BLOCK_BINDING, &iResult );
	uiBindPoint = static_cast<GLuint>( iResult );
	return !VistaOGLUtils::CheckForOGLError( __FILE__, __LINE__ );
}

bool VistaGLSLShader::GetUniformBlockBinding( const std::string& strBlockName,
		GLuint& uiBindPoint )
{
	const GLuint uiIndex = GetUniformBlockIndex( strBlockName );
	return GetUniformBlockBinding( uiIndex, uiBindPoint );
}


bool VistaGLSLShader::IsSupported() const
{
	return ( GLEW_VERSION_2_0 ? true : false );
}

bool VistaGLSLShader::IsReadyForUse() const
{
	return m_bIsReadyForUse;
}


