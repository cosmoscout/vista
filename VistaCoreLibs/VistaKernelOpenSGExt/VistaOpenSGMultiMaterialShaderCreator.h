/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
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


#ifndef _VISTAOPENSGMULTIMATERIALSHADERCREATOR_H
#define _VISTAOPENSGMULTIMATERIALSHADERCREATOR_H

#include "VistaKernelOpenSGExtConfig.h"

#include <string>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif
#include <OpenSG/OSGSHLChunk.h>
#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

/**
 * Creates shaders for the multi-material
 * For performance reasons, shaders and shader strings are stored in static maps
 * @TODO: rethink static buffering
 */
namespace VistaOpenSGMultiMaterialShaderCreator
{
	const unsigned int S_nBaseMapTextureId = 0;
	const unsigned int S_nBlendMapTextureId = 1;
	const unsigned int S_nSpecularMapTextureId = 2;
	const unsigned int S_nNormalMapTextureId = 3;
	const unsigned int S_nGlossMapTextureId = 4;
	const unsigned int S_nWorldEnvMapTextureId = 5;
	const unsigned int S_nObjectEnvMapTextureId = 6;

	struct VISTAKERNELOPENSGEXTAPI ShaderConfig 
	{
		ShaderConfig();

		bool m_bHasBaseMap;

		bool m_bHasBlendMap;
		float m_nBlendFactor;

		bool m_bHasSpecularMap;

		bool m_bHasNormalMap;

		bool m_bHasGlossMap;	
		bool m_bHasWorldEnvMap;
		bool m_bHasObjectEnvMap;

		float m_nEnvironmentReflactionFactor;
		float m_nEnvironmentBlur;

		bool m_bUseFog;
		
		bool operator< ( const ShaderConfig& oConfig ) const;
	};	

	VISTAKERNELOPENSGEXTAPI osg::SHLChunkPtr GetShader( const ShaderConfig& oShaderConfig );
	VISTAKERNELOPENSGEXTAPI const std::string& GetVertexShader( const ShaderConfig& oShaderConfig );	
	VISTAKERNELOPENSGEXTAPI const std::string& GetFragmentShader( const ShaderConfig& oShaderConfig );	
}

#endif // _VISTAOPENSGMULTIMATERIALSHADERCREATOR_H
