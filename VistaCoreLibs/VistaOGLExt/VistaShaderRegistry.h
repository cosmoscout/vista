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


#ifndef VISTASHADERREGISTRY_H
#define VISTASHADERREGISTRY_H

#include "VistaOGLExtConfig.h"

#include <list>
#include <map>
#include <string>

/**
 * The VistaShaderRegistry is a central class for loading and retrieving shader
 * sources from separate shader files (e.g. *.glsl). Renderers that require a
 * shader should directly retrieve its source via this class. To retrieve shader
 * sources the renderer needs to specify the respective shader's filename.
 * The registry will then attempt to locate the shader file in a list of search
 * directories. If the shader is found its source will be loaded and cached for
 * future access. There is no need for the user of renderers that access the
 * VistaShaderRegistry to manually load the shaders. Only the search paths need
 * to be set correctly. Besides automated loading via search paths, the user
 * has always the possibility to pre-cache shaders manually.
 *
 * IMPORTANT NOTE: Filenames of shader files _MUST_ be unique. If two different
 * renderers want to access two different shaders that have the same filename,
 * problems will eventually occur.
 */
class VISTAOGLEXTAPI VistaShaderRegistry
{
protected:
	/**
	 * Please use GetInstance() to access the public interface.
	 */
	VistaShaderRegistry();

public:
	static VistaShaderRegistry& GetInstance();
	
	/**
	 * return a list of all search paths. 
	 * Note, that the order is respected when looking for a new shader.
	 * The directory that is the first in the List will be search through first.
	 */
	std::list<std::string> GetSearchDirectories() const;
	/**
	 * Adds a directory to the list of relative or absolute search paths. 
	 * Note, that the order is respected when looking for a new shader.
	 * The directory that has been added last will be search through first.
	 *
	 * @param	strSearchDirs the search directory, e.g. "shaders/". Use '/' as
	 *			separator to maximize portability. The trailing '/' is optional
	 *			but will be added automatically if it was omitted.
	 */
	void AddSearchDirectory( const std::string& strSearchDir );
	/**
	 * removes a directory form the list of search paths.
	 */
	void RemoveSearchDirectory( const std::string& strSearchDir );

	/**
	 * Register a shader by specifying the full path to the shader. The shader's
	 * filename will be extracted from it. An optional flag allows to
	 * load & cache a shader even if one with the same name has already been
	 * cached.
	 */
	bool RegisterShader( const std::string& strShaderPath,
		bool bAllowOverwrite = false );
	/**
	 * Does the same as the other RegisterShader() method. The difference is,
	 * that a shader can be registered with a different name (alias) than its
	 * actual filename. This option can be handy if you want to inject a shader
	 * with a different filename into the cache in order to exchange the default
	 * shader of a renderer without it knowing.
	 */
	bool RegisterShader( const std::string& strShaderPath,
		const std::string& strRegisterAlias, bool bAllowOverwrite = false );
	
	/**
	 * Retrieves the shader source for the given shader and register it with an
	 * alias name. This option can be use instead of RegisterShader(), when you
	 * don't know the complete file path of the shader.
	 * Note this method casles RetrieveShader() to retrieve the shader source,
	 * hence after calling this method the shader source will be registered
	 * with its actual name and its alias name.
	 */
	bool RegisterShaderAlias(	const std::string& strShaderName,
								const std::string& strAliasName,
								bool bAllowOverwrite = false );

	/**
	 * Retrieves the shader source for the given filename. If the source has
	 * already been loaded/registered before, it will be retrieved from cache.
	 * If not, it will be retrieved by searching for the filename within the
	 * currently set search paths. In case the shader source could not be
	 * retrieved, an empty string will be returned.
	 */
	std::string RetrieveShader( const std::string& strShaderName );


	/**
	 * Drops all cached shaders from the registry. Search paths will not change.
	 */
	void Reset();
	/**
	 * Drops the source for the specified shader from the cache. If no source
	 * has been cached for that shader, nothing will happen (silently).
	 */
	void DropFromCache( const std::string& strShaderFilename );
	
protected:
	std::string LoadSourceFromDirs( const std::string& strShaderName );
	std::string LoadSourceFromFile( const std::string& strFilePath );

private:
	static VistaShaderRegistry			m_instance;

	std::list<std::string>				m_liSearchDirectories;
	std::map<std::string, std::string>	m_mapShadersFnToSource;
};

#endif // Include gurad.

