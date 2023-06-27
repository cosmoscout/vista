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

#include "VistaOpenSGMultiMaterialShaderCreator.h"

namespace {
enum FragmentShaderMode {
  FS_NO_TEXTURE,
  FS_STANDARD_TEXTURE,
  FS_NORMAL_MAP,
};

std::map<VistaOpenSGMultiMaterialShaderCreator::ShaderConfig, osg::SHLChunkPtr> S_mapShaders;
std::map<FragmentShaderMode, std::string> S_mapVertexShaderStrings;
std::map<VistaOpenSGMultiMaterialShaderCreator::ShaderConfig, std::string>
    S_mapFragmentShaderStrings;
} // namespace

osg::SHLChunkPtr VistaOpenSGMultiMaterialShaderCreator::GetShader(const ShaderConfig& oConfig) {
  std::map<ShaderConfig, osg::SHLChunkPtr>::iterator itShader = S_mapShaders.find(oConfig);
  if (itShader != S_mapShaders.end())
    return (*itShader).second;

  const std::string& sVertexShader   = GetVertexShader(oConfig);
  const std::string& sFragmentShader = GetFragmentShader(oConfig);

  osg::SHLChunkPtr pShader = osg::SHLChunk::create();
  osg::addRefCP(pShader);

  pShader->setVertexProgram(sVertexShader);
  pShader->setFragmentProgram(sFragmentShader);

  pShader->setUniformParameter("OSGLight0Active", 0);
  pShader->setUniformParameter("OSGLight1Active", 0);
  pShader->setUniformParameter("OSGLight2Active", 0);
  pShader->setUniformParameter("OSGLight3Active", 0);
  pShader->setUniformParameter("OSGLight4Active", 0);
  pShader->setUniformParameter("OSGLight5Active", 0);
  pShader->setUniformParameter("OSGLight6Active", 0);
  pShader->setUniformParameter("OSGLight7Active", 0);

  if (oConfig.m_bHasBaseMap)
    pShader->setUniformParameter("spBaseMap", (osg::Int32)S_nBaseMapTextureId);
  if (oConfig.m_bHasBlendMap) {
    pShader->setUniformParameter("spBlendMap", (osg::Int32)S_nBlendMapTextureId);
    pShader->setUniformParameter("nBlendFactor", oConfig.m_nBlendFactor);
  }
  if (oConfig.m_bHasSpecularMap)
    pShader->setUniformParameter("spSpecularMap", (osg::Int32)S_nSpecularMapTextureId);
  if (oConfig.m_bHasNormalMap)
    pShader->setUniformParameter("spNormalMap", (osg::Int32)S_nNormalMapTextureId);
  if (oConfig.m_bHasObjectEnvMap || oConfig.m_bHasWorldEnvMap) {
    pShader->setUniformParameter("OSGInvViewMatrix", 0);
    pShader->setUniformParameter("nReflectionFactor", oConfig.m_nEnvironmentReflactionFactor);
    pShader->setUniformParameter("nEnvMapBias", oConfig.m_nEnvironmentBlur);
    if (oConfig.m_bHasWorldEnvMap) {
      pShader->setUniformParameter("spFixedEnvMap", (osg::Int32)S_nWorldEnvMapTextureId);
    }
    if (oConfig.m_bHasObjectEnvMap) {
      pShader->setUniformParameter("spObjectEnvMap", (osg::Int32)S_nObjectEnvMapTextureId);
      pShader->setUniformParameter("OSGInvWorldMatrix", 0);
    }
    if (oConfig.m_bHasGlossMap)
      pShader->setUniformParameter("spGlossMap", (osg::Int32)S_nGlossMapTextureId);
  }

  S_mapShaders[oConfig] = pShader;

  return pShader;
}

const std::string& VistaOpenSGMultiMaterialShaderCreator::GetVertexShader(
    const ShaderConfig& oConfig) {
  FragmentShaderMode nId = FS_NO_TEXTURE;
  if (oConfig.m_bHasBaseMap || oConfig.m_bHasBlendMap || oConfig.m_bHasSpecularMap ||
      (oConfig.m_bHasGlossMap && (oConfig.m_bHasWorldEnvMap || oConfig.m_bHasObjectEnvMap))) {
    nId = FS_STANDARD_TEXTURE;
  }
  if (oConfig.m_bHasNormalMap) {
    nId = FS_NORMAL_MAP;
  }

  std::string& sString = S_mapVertexShaderStrings[nId];
  if (sString.empty() == false)
    return sString;

  sString += "varying vec3 v3WorldNormal;\n"
             "varying vec3 v3WorldPos;\n";
  if (nId != FS_NO_TEXTURE) {
    sString += "uniform vec4 u_v4TexCoordScale;\n";
  }
  sString += "\n"
             "void main()\n"
             "{\n"
             "  v3WorldNormal = normalize( gl_NormalMatrix * gl_Normal );\n"
             "  v3WorldPos = vec3( gl_ModelViewMatrix * gl_Vertex );\n";
  sString += "  gl_Position = ftransform();\n"
             "  gl_FrontColor = gl_Color;\n"
             "  gl_BackColor = gl_Color;\n";
  if (nId != FS_NO_TEXTURE) {
    sString += "  gl_TexCoord[0] = u_v4TexCoordScale * gl_MultiTexCoord0;\n";
  }
  if (nId == FS_NORMAL_MAP) {
    sString += "  gl_TexCoord[1].xyz = gl_NormalMatrix * gl_MultiTexCoord1.xyz;\n";
  }
  sString += "}\n";

  return sString;
}

const std::string& VistaOpenSGMultiMaterialShaderCreator::GetFragmentShader(
    const ShaderConfig& oConfig) {
  ShaderConfig oReducedConfig(oConfig);
  // variables don't influence shader code, so we null them out
  oReducedConfig.m_nBlendFactor                 = 0.0f;
  oReducedConfig.m_nEnvironmentBlur             = 0.0f;
  oReducedConfig.m_nEnvironmentReflactionFactor = 0.0f;

  bool bHasEnvMap = oReducedConfig.m_bHasObjectEnvMap || oReducedConfig.m_bHasWorldEnvMap;
  if (bHasEnvMap == false)
    oReducedConfig.m_bHasGlossMap = false; // dont use gloss if no env

  std::string& sString = S_mapFragmentShaderStrings[oReducedConfig];
  if (sString.empty() == false)
    return sString;

  sString = "varying vec3 v3WorldNormal;\n"
            "varying vec3 v3WorldPos;\n"
            "uniform bool OSGLight0Active;\n"
            "uniform bool OSGLight1Active;\n"
            "uniform bool OSGLight2Active;\n"
            "uniform bool OSGLight3Active;\n"
            "uniform bool OSGLight4Active;\n"
            "uniform bool OSGLight5Active;\n"
            "uniform bool OSGLight6Active;\n"
            "uniform bool OSGLight7Active;\n";
  if (bHasEnvMap)
    sString += "uniform mat4 OSGInvViewMatrix;\n";
  if (oConfig.m_bHasObjectEnvMap)
    sString += "uniform mat4 OSGInvWorldMatrix;\n";
  if (oConfig.m_bUseFog)
    sString += "uniform int nFogType;\n";

  if (oConfig.m_bHasBaseMap)
    sString += "uniform sampler2D spBaseMap;\n";
  if (oConfig.m_bHasBlendMap)
    sString += "uniform sampler2D spBlendMap;\n";
  if (oConfig.m_bHasNormalMap)
    sString += "uniform sampler2D spNormalMap;\n";
  if (oConfig.m_bHasSpecularMap)
    sString += "uniform sampler2D spSpecularMap;\n";
  if (oConfig.m_bHasGlossMap && bHasEnvMap)
    sString += "uniform sampler2D spGlossMap;\n";
  if (oConfig.m_bHasWorldEnvMap)
    sString += "uniform samplerCube spFixedEnvMap;\n";
  if (oConfig.m_bHasObjectEnvMap)
    sString += "uniform samplerCube spObjectEnvMap;\n";

  if (bHasEnvMap) {
    sString += "uniform float nReflectionFactor;\n"
               "uniform float nEnvMapBias;\n";
  }
  if (oConfig.m_bHasBlendMap) {
    sString += "uniform float nBlendFactor;\n";
  }

  if (oConfig.m_bHasSpecularMap)
    sString += "void ProcessLight( in int iLight, in vec3 v3Normal, in vec3 v3ToPosDir, in float "
               "nSpecularityInfluence, inout vec3 v3Color )\n";
  else
    sString += "void ProcessLight( in int iLight, in vec3 v3Normal, in vec3 v3ToPosDir, inout vec3 "
               "v3Color )\n";
  sString +=
      "{\n"
      "    vec3 v3LightDir;\n"
      "    float fAttenuation = 1.0;\n"
      "    // calculating Light Direction and Attenuation based on light type\n"
      "    if( gl_LightSource[iLight].position.w == 0.0 )\n"
      "    {\n"
      "        // directional light\n"
      "        v3LightDir = normalize(gl_LightSource[iLight].position.xyz);\n"
      "    }\n"
      "    else // point or spot light\n"
      "    {\n"
      "        // point light\n"
      "        vec3 v3Aux = gl_LightSource[iLight].position.xyz - v3WorldPos;\n"
      "        v3LightDir = normalize(v3Aux);\n"
      "        float fDist = length(v3Aux);\n"
      "        fAttenuation = 1.0 / ( gl_LightSource[iLight].constantAttenuation +\n"
      "            gl_LightSource[iLight].linearAttenuation * fDist +\n"
      "            gl_LightSource[iLight].quadraticAttenuation * fDist * fDist );\n"
      "        if( gl_LightSource[iLight].spotCutoff <= 90.0 )\n"
      "        {\n"
      "            // it's a spotlight\n"
      "            vec3 v3SpotDir = normalize( gl_LightSource[iLight].spotDirection );\n"
      "            float fCosSpotAngle = dot( -v3LightDir, v3SpotDir );\n"
      "            if( fCosSpotAngle < gl_LightSource[iLight].spotCosCutoff )\n"
      "            {\n // outside the spot\n"
      "                fAttenuation = 0.0;\n"
      "            }\n"
      "            else\n"
      "            {\n // inside the main spot area\n"
      "                fAttenuation *= pow( fCosSpotAngle, gl_LightSource[iLight].spotExponent );\n"
      "            }\n"
      "        }\n"
      "    }\n"
      "    // Ambient Term\n"
      "    vec3 v3LocalColor = gl_FrontMaterial.ambient.rgb * gl_LightSource[iLight].ambient.rgb;\n"
      "    vec3 v3HalfVec = normalize( v3ToPosDir + v3LightDir );\n"
      "    float nShininess = gl_FrontMaterial.shininess;\n"
      "    float fNdotL, fNdotHV;\n"
      "    fNdotL = dot( v3Normal, v3LightDir );\n"
      "    if( fNdotL > 0.0 )\n"
      "    {\n"
      "        fNdotHV = max( dot( v3Normal, v3HalfVec ), 0.0 );\n"
      "        //Diffuse Term\n"
      "        v3LocalColor += gl_Color.rgb * gl_LightSource[iLight].diffuse.rgb * fNdotL;\n"
      "        //Specular Term\n"
      "        if( gl_FrontMaterial.shininess > 0.0 )\n"
      "        {\n"
      "            vec3 v3SpecularityColor = gl_FrontMaterial.specular.rgb * "
      "gl_LightSource[iLight].specular.rgb;\n"
      "            v3SpecularityColor *= fNdotL * max( pow( fNdotHV, nShininess ), 0.0 );\n";
  if (oConfig.m_bHasSpecularMap) {
    sString += "            v3SpecularityColor *= nSpecularityInfluence;\n";
  }
  sString += "            v3LocalColor += v3SpecularityColor;\n"
             "        }\n"
             "    }\n"
             "    \n"
             "    v3LocalColor *= fAttenuation;\n"
             "    v3Color += v3LocalColor;\n"
             "}\n"
             "\n";

  if (oConfig.m_bUseFog) {
    sString += "void ProcessFog( inout vec3 v3Color ) \n"
               "{\n"
               "    float fFogFactor = 0.0;\n"
               "    if( nFogType == 0 )\n"
               "    {\n"
               "        if( gl_Fog.end <= gl_Fog.start )"
               "            return;\n"
               "        float nDistance = length( v3WorldPos );\n"
               "        fFogFactor = ( gl_Fog.end - nDistance ) * gl_Fog.scale;\n"
               "    }\n"
               "    else if( nFogType == 1 )\n"
               "    {\n"
               "        if( gl_Fog.density <= 0.0 )"
               "            return;\n"
               "        float nDistance = length( v3WorldPos );\n"
               "        fFogFactor = exp( -gl_Fog.density * nDistance );\n"
               "    }\n"
               "    else\n"
               "    {\n"
               "        if( gl_Fog.density <= 0.0 )"
               "            return;\n"
               "        float nSqDistance = dot( v3WorldPos, v3WorldPos );\n"
               "        fFogFactor = exp( -gl_Fog.density * gl_Fog.density * nSqDistance );\n"
               "    }\n"
               "    fFogFactor = clamp( fFogFactor, 0.0, 1.0 );\n"
               "    v3Color = mix( gl_Fog.color.rgb, v3Color, fFogFactor );\n"
               "}\n";
  }

  sString += "\n"
             "void main()\n"
             "{\n"
             "    vec3 v3Normal = normalize( v3WorldNormal );\n";
  if (oConfig.m_bHasNormalMap) {
    sString += "    // process data for normal mapping \n"
               "    vec3 v3Tangent = normalize( gl_TexCoord[1].xyz );\n"
               "    vec3 v3BiNormal = cross( v3Normal, v3Tangent );\n"
               "    mat3 mat3TangentToWorld = mat3( v3Tangent, v3BiNormal, v3Normal );\n"
               "    v3Normal = normalize( ( texture2D( spNormalMap, gl_TexCoord[0].st ).xyz * 2.0 "
               ") - 1.0 );\n"
               "    // retransform from tangent space to world space\n"
               "    v3Normal = mat3TangentToWorld * v3Normal;\n"
               "    \n";
  }
  sString += "    if( gl_FrontFacing == false ) //render back side\n"
             "        v3Normal = -v3Normal;\n"
             "    \n"
             "    vec3 v3Color = gl_FrontLightModelProduct.sceneColor.rgb;\n"
             "    float nAlpha = gl_FrontMaterial.diffuse.a;\n"
             "    \n"
             "    vec3 v3ToPosDir = -normalize(v3WorldPos);\n";
  if (oConfig.m_bHasSpecularMap) {
    sString +=
        "    float nSpecularityInfluence = texture2D( spSpecularMap, gl_TexCoord[0].st ).r;\n"
        "    if( OSGLight0Active )\n"
        "        ProcessLight( 0, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n"
        "    if( OSGLight1Active )\n"
        "        ProcessLight( 1, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n"
        "    if( OSGLight2Active )\n"
        "        ProcessLight( 2, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n"
        "    if( OSGLight3Active )\n"
        "        ProcessLight( 3, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n"
        "    if( OSGLight4Active )\n"
        "        ProcessLight( 4, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n"
        "    if( OSGLight5Active )\n"
        "        ProcessLight( 5, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n"
        "    if( OSGLight6Active )\n"
        "        ProcessLight( 6, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n"
        "    if( OSGLight7Active )\n"
        "        ProcessLight( 7, v3Normal, v3ToPosDir, nSpecularityInfluence, v3Color );\n";
  } else {
    sString += "    if( OSGLight0Active )\n"
               "        ProcessLight( 0, v3Normal, v3ToPosDir, v3Color );\n"
               "    if( OSGLight1Active )\n"
               "        ProcessLight( 1, v3Normal, v3ToPosDir, v3Color );\n"
               "    if( OSGLight2Active )\n"
               "        ProcessLight( 2, v3Normal, v3ToPosDir, v3Color );\n"
               "    if( OSGLight3Active )\n"
               "        ProcessLight( 3, v3Normal, v3ToPosDir, v3Color );\n"
               "    if( OSGLight4Active )\n"
               "        ProcessLight( 4, v3Normal, v3ToPosDir, v3Color );\n"
               "    if( OSGLight5Active )\n"
               "        ProcessLight( 5, v3Normal, v3ToPosDir, v3Color );\n"
               "    if( OSGLight6Active )\n"
               "        ProcessLight( 6, v3Normal, v3ToPosDir, v3Color );\n"
               "    if( OSGLight7Active )\n"
               "        ProcessLight( 7, v3Normal, v3ToPosDir, v3Color );\n";
  }
  if (oConfig.m_bHasBaseMap) {
    sString += "    \n"
               "    vec4 v4BaseMapColor = texture2D( spBaseMap, gl_TexCoord[0].st );\n"
               "    v3Color *= v4BaseMapColor.rgb;\n"
               "    nAlpha *= v4BaseMapColor.a;\n";
  }
  if (oConfig.m_bHasBlendMap) {
    sString += "    \n"
               "    vec4 v4BlendColor = texture2D( spBlendMap, gl_TexCoord[0].st );\n"
               "    float nInfluence = nBlendFactor * v4BlendColor.a;\n"
               "    v3Color = mix( v3Color, v4BlendColor.rgb, nInfluence );\n";
  }
  if (bHasEnvMap) {
    if (oConfig.m_bHasGlossMap)
      sString += "\n    float nReflInfluence = nReflectionFactor * texture2D( spGlossMap, "
                 "gl_TexCoord[0].st ).r;\n";
    else
      sString += "\n    float nReflInfluence = nReflectionFactor;\n";

    if (oConfig.m_bHasObjectEnvMap && oConfig.m_bHasWorldEnvMap) {
      sString +=
          "    vec4 v4Reflected = vec4( reflect( v3WorldPos, v3WorldNormal ), 0 );\n"
          "    vec3 v3ReflectedFixed = normalize( vec3( OSGInvViewMatrix * v4Reflected );\n"
          "    vec3 v3ReflectedObject = normalize( vec3( OSGInvViewMatrix * v4Reflected ) );\n"
          "    vec4 v4CubeColorFixed = textureCube( spFixedEnvMap, v3ReflectedFixed, nEnvMapBias "
          ");\n"
          "    vec4 v4CubeColorObject = textureCube( spObjectEnvMap, v3ReflectedObject, "
          "nEnvMapBias );\n"
          "    v4CubeColorObject += ( 1 - v4CubeColorObject.a ) * v4CubeColorFixed;\n"
          "    nReflInfluence *= v4CubeColorObject.a;\n"
          "    v3Color = mix( v3Color, v4CubeColorObject.rgb, nReflInfluence );\n";
    } else if (oConfig.m_bHasWorldEnvMap) {
      sString += "    vec3 v3Reflected = normalize( vec3( OSGInvViewMatrix * vec4( reflect( "
                 "v3WorldPos, v3WorldNormal ), 0 ) ) );\n"
                 "    vec4 v4CubeColor = textureCube( spFixedEnvMap, v3Reflected, nEnvMapBias );\n"
                 "    nReflInfluence *= v4CubeColor.a;\n"
                 "    v3Color = mix( v3Color, v4CubeColor.rgb, nReflInfluence );\n";
    } else // object env map
    {
      sString += "    vec3 v3Reflected = normalize( vec3( OSGInvWorldMatrix * OSGInvViewMatrix * "
                 "vec4( reflect( v3WorldPos, v3WorldNormal ), 0 ) ) );\n"
                 "    vec4 v4CubeColor = textureCube( spObjectEnvMap, v3Reflected, nEnvMapBias );\n"
                 "    nReflInfluence *= v4CubeColor.a;\n"
                 "    v3Color = mix( v3Color, v4CubeColor.rgb, nReflInfluence );\n";
    }
  }
  if (oConfig.m_bUseFog) {
    sString += "    ProcessFog( v3Color );\n";
  }
  sString += "    \n"
             "    gl_FragColor = vec4( v3Color, nAlpha );\n"
             "}\n";

  return sString;
}

bool VistaOpenSGMultiMaterialShaderCreator::ShaderConfig::operator<(
    const ShaderConfig& oConfig) const {
  if (m_bHasBaseMap != oConfig.m_bHasBaseMap)
    return m_bHasBaseMap;

  if (m_bHasBlendMap != oConfig.m_bHasBlendMap) {
    return m_bHasBlendMap;
  } else if (m_bHasBlendMap) {
    if (m_nBlendFactor != oConfig.m_nBlendFactor)
      return (m_nBlendFactor < oConfig.m_nBlendFactor);
  }

  if (m_bHasSpecularMap != oConfig.m_bHasSpecularMap)
    return m_bHasSpecularMap;

  if (m_bHasNormalMap != oConfig.m_bHasNormalMap)
    return m_bHasNormalMap;

  if (m_bHasWorldEnvMap != oConfig.m_bHasWorldEnvMap) {
    return m_bHasWorldEnvMap;
  }

  if (m_bHasObjectEnvMap != oConfig.m_bHasObjectEnvMap) {
    return m_bHasObjectEnvMap;
  }

  if (m_bHasObjectEnvMap || m_bHasWorldEnvMap) {
    if (m_bHasGlossMap != oConfig.m_bHasGlossMap)
      return m_bHasGlossMap;
    if (m_nEnvironmentReflactionFactor != oConfig.m_nEnvironmentReflactionFactor)
      return (m_nEnvironmentReflactionFactor < oConfig.m_nEnvironmentReflactionFactor);
    if (m_nEnvironmentBlur != oConfig.m_nEnvironmentBlur)
      return (m_nEnvironmentBlur < oConfig.m_nEnvironmentBlur);
  }

  if (m_bUseFog != oConfig.m_bUseFog) {
    return m_bUseFog;
  }

  return false; // equal
}

VistaOpenSGMultiMaterialShaderCreator::ShaderConfig::ShaderConfig()
    : m_bHasBaseMap(false)
    , m_bHasBlendMap(false)
    , m_nBlendFactor(0)
    , m_bHasSpecularMap(false)
    , m_bHasNormalMap(false)
    , m_bHasGlossMap(false)
    , m_bHasWorldEnvMap(false)
    , m_bHasObjectEnvMap(false)
    , m_nEnvironmentReflactionFactor(0)
    , m_nEnvironmentBlur(0)
    , m_bUseFog(false) {
}
