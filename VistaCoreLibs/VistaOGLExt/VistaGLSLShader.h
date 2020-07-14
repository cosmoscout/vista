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


#ifndef _VistaGLSLShader_h
#define _VistaGLSLShader_h

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <GL/glew.h>

#include "VistaOGLExtConfig.h"

#include <vector>
#include <map>
#include <string>


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class implements an object wrapper for OpenGL Shading Language shaders.
 * It is designed against the OpenGL 2.0 standard. Hence, it supports geometry
 * shaders via an OpenGL extension.
 */
class VISTAOGLEXTAPI VistaGLSLShader
{
public:
	VistaGLSLShader();
	~VistaGLSLShader();

	/**
	 * Create and destroy the shader.
	 * To use this object, initialize vertex and/or geometry and/or fragment 
	 * shaders from files and/or strings and call Link().
	 * NOTE: If geometry shaders are used, set geometry shader input and output 
	 * types as well as geometry shader vertex output count 
	 * _before_ calling Link().
	 */
	bool InitFromFiles(	const std::string &strVertexShaderFile,
						const std::string &strFragmentShaderFile,
						const std::string &strPrefix = "");
	bool InitFromFiles(	const std::string &strVertexShaderFile,
						const std::string &strGeometryShaderFile,
						const std::string &strFragmentShaderFile,
						const std::string &strPrefix);

	bool InitFromStrings(	const std::string &strVertexShaderString,
							const std::string &strFragmentShaderString);
	bool InitFromStrings(	const std::string &strVertexShaderString,
							const std::string &strGeometryShaderString,
							const std::string &strFragmentShaderString);

	bool InitVertexShaderFromFile(	const std::string &strVertexShaderFile, 
									const std::string &strPrefix = "");
	bool InitVertexShaderFromString(const std::string &strVertexShaderString);

	bool InitGeometryShaderFromFile(const std::string &strGeometryShaderFile,
									const std::string &strPrefix = "");
	bool InitGeometryShaderFromString(
									const std::string &strGeometryShaderString);

	bool InitFragmentShaderFromFile(const std::string &strFragmentShaderFile,
									const std::string &strPrefix = "");
	bool InitFragmentShaderFromString(
									const std::string &strFragmentShaderString);

    bool InitComputeShaderFromFile(const std::string& strComputeShaderFile,
                                   const std::string& strPrefix = "");
    bool InitComputeShaderFromString(
                                    const std::string& strComputeShaderString);

	bool InitShaderFromFile(	const unsigned int uiShaderTyp,
								const std::string &strShaderFile,
								const std::string &strPrefix = "");
	bool InitShaderFromString(	const unsigned int uiShaderTyp,
								const std::string &strShaderString);

	/**
	 * Set additional parameters iff geometry shaders are used.
	 */
	bool SetGeometryShaderInputType(int iInputType);
	bool SetGeometryShaderOutputType(int iOutputType);
	bool SetGeometryShaderVertexOutputCount(int iCount);

	bool Link();

	void Destroy();

	/**
	 * (De-)Activate the shader.
	 */
	void Bind();
	void Release();

	/**
	 * Retrieve additional information about this GLSL shader.
	 */
	GLuint GetProgram() const;
	GLuint GetVertexShader( const std::size_t nIdx = 0 ) const;
	GLuint GetTessControlShader( const std::size_t nIdx = 0 ) const;
	GLuint GetTessEvalShader( const std::size_t nIdx = 0 ) const;
	GLuint GetGeometryShader( const std::size_t nIdx = 0 ) const;
	GLuint GetFragmentShader( const std::size_t nIdx = 0 ) const;
	GLuint GetComputeShader( const std::size_t nIdx = 0 ) const;

	/**
	 * Retrieves the location index of the specified uniform variable.
	 * 
	 * @param strName Name of the uniform variable whose location to retrieve.
	 * @return The location index, or -1 if the uniform does not exists in the
	 *		   program.
	 */
	GLint GetUniformLocation( const std::string& strName ) const;
	
	void SetUniform( int iLocation, float fUniform1 );
	void SetUniform( int iLocation, int iUniform1 );
	bool SetUniform( const std::string& strName, int iUniform1 );
	void SetUniform( int iLocation, float fUniform1, float fUniform2 );
	void SetUniform( int iLocation, float fUniform1, float fUniform2,
					 float fUniform3 );
	void SetUniform( int iLocation, float fUniform1, float fUniform2,
					 float fUniform3, float fUniform4 );
	void SetUniform( int iLocation, float *pUniform );
	

	void SetUniform(int iLocation, int iCount,  const float* fUniform,bool bTranspose);
	 
	/**
	 * Modify uniform shader parameters in the form of arrays.
	 * The number of components determines, whether this maps to 1-,
	 * 2-, 3-, or 4-components vectors.
	 * The given count specifies the number of n-component vectors to set.
	 *
	 * NOTE: Typically, you have to Bind() the shader before setting these.
	 */
	void SetUniform( int iLocation, int iComponents, int iCount,
					 const float *pData);
	void SetUniform( int iLocation, int iComponents, int iCount,
					 const int *pData);


    /**
	 * Retrieves the index of the uniform block specified.
	 * 
	 * @param strBlockName Name of the uniform block whose index to retrieve.
	 * @return The block index, or GL_INVALID_INDEX if the uniform block does
	 *		   not exist in the program.
	 */
	GLuint GetUniformBlockIndex( const std::string& strBlockName ) const;	
	/**
	 * Binds the block of the specified index to the specified block bind point.
	 *
	 * @param uiBlockIndex The index of a block of the shader.
	 * @param uiBindPoint A valid bind point.
	 * @return Returns 'true' if the binding was successfully established.
	 */
	bool SetUniformBlockBinding( GLuint uiBlockIndex, GLuint uiBindPoint );
	bool SetUniformBlockBinding( const std::string& strBlockName,
								 GLuint uiBindPoint );
	/**
	 * Queries the GL to determine the current bind point for the specified
	 * block index.
	 * @param uiBlockIndex Index of the uniform block whose bind point is to be
	 *		  queried.
	 * @param uiBindPoint The output variable for the current bind point. The
	 *		  bind point is also 0 if no bind point was specified before.
	 * @return True, if the bind point for the given block index could be
	 *		   retrieved.
	 */
	bool GetUniformBlockBinding( GLuint uiBlockIndex, GLuint& uiBindPoint );
	bool GetUniformBlockBinding( const std::string& strBlockName,
								 GLuint& uiBindPoint );


	/**
	 * Check for hardware / driver support of OpenGL 2.0.
	 * To determine whether a certain shader type (i.e. vertex shader,
	 * geometry shader or fragment shader) is supported, try to init 
	 * and link it.
	 */
	bool IsSupported() const;

	bool IsReadyForUse() const;

protected:
	// Pre-caches the uniform locations when the shader is linked
	void MapUniforms();
	// Pre-caches the uniform block indices when the shader is linked
	void MapUniformBlocks();

private:
	bool m_bIsReadyForUse;

	GLuint m_uiProgram;
    std::vector<GLuint> m_vecComputeShader;
	std::vector<GLuint>	m_vecVertexShader;
	std::vector<GLuint> m_vecTessControlShader;
	std::vector<GLuint> m_vecTessEvalShader;
	std::vector<GLuint>	m_vecGeometryShader;
	std::vector<GLuint> m_vecFragmentShader;	

	typedef std::map<std::string, GLint> UniformC;
	UniformC m_Uniforms;

	typedef std::map<std::string, GLuint> UniformBlockC;
	UniformBlockC m_UniformBlocks;
};

#endif // Include guard


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
