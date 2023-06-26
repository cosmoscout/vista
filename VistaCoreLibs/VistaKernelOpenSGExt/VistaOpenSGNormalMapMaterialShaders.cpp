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

#include "VistaOpenSGNormalMapMaterial.h"
#include <string>

std::string VistaOpenSGNormalMapMaterial::_vs_normalmap =
    "uniform vec3 fvLightPosition;\n"
    "uniform vec3 OSGCameraPosition;\n"

    "varying vec2 Texcoord;\n"
    "varying vec3 ViewDirection;\n"
    "varying vec3 LightDirection;\n"

    // OpenSG SHL chunk does not support custom vertex attributes :(
    // we use the 2nd and 3rd texture coordinates for this...
    //"attribute vec3 rm_Binormal;\n"
    //"attribute vec3 rm_Tangent;\n"

    "void main( void )\n"
    "{\n"
    "   gl_Position = ftransform();\n"
    "   Texcoord    = gl_MultiTexCoord0.xy;\n"

    "   vec3 rm_Tangent  = gl_MultiTexCoord1.xyz;\n"
    "   vec3 rm_Binormal = gl_MultiTexCoord2.xyz;\n"

    "   vec4 fvObjectPosition = gl_ModelViewMatrix * gl_Vertex;\n"

    "   vec3 fvViewDirection  = OSGCameraPosition - fvObjectPosition.xyz;\n"
    "   vec3 fvLightDirection = fvLightPosition - fvObjectPosition.xyz;\n"

    "   vec3 fvNormal         = gl_NormalMatrix * gl_Normal;\n"
    "   vec3 fvBinormal       = gl_NormalMatrix * rm_Binormal;\n"
    "   vec3 fvTangent        = gl_NormalMatrix * rm_Tangent;\n"

    "   ViewDirection.x  = dot( fvTangent, fvViewDirection );\n"
    "   ViewDirection.y  = dot( fvBinormal, fvViewDirection );\n"
    "   ViewDirection.z  = dot( fvNormal, fvViewDirection );\n"

    "   LightDirection.x  = dot( fvTangent, fvLightDirection.xyz );\n"
    "   LightDirection.y  = dot( fvBinormal, fvLightDirection.xyz );\n"
    "   LightDirection.z  = dot( fvNormal, fvLightDirection.xyz );\n"

    "}\n";

std::string VistaOpenSGNormalMapMaterial::_fs_normalmap =
    "uniform vec4 fvAmbient;\n"
    "uniform vec4 fvDiffuse;\n"
    "uniform vec4 fvSpecular;\n"
    "uniform float fSpecularPower;\n"

    "uniform sampler2D normalMap;\n"

    "varying vec2 Texcoord;\n"
    "varying vec3 ViewDirection;\n"
    "varying vec3 LightDirection;\n"

    "void main( void )\n"
    "{\n"
    "   vec3  fvLightDirection = normalize( LightDirection );\n"
    "   vec3  fvNormal         = normalize( ( texture2D( normalMap, Texcoord ).xyz * 2.0 ) - 1.0 "
    ");\n"
    "   float fNDotL           = dot( fvNormal, fvLightDirection ); \n"

    "   vec3  fvReflection     = normalize( ( ( 2.0 * fvNormal ) * fNDotL ) - fvLightDirection ); "
    "\n"
    "   vec3  fvViewDirection  = normalize( ViewDirection );\n"
    "   float fRDotV           = max( 0.0, dot( fvReflection, fvViewDirection ) );\n"

    "   vec4  fvBaseColor      = vec4(1.0, 1.0, 1.0, 1.0);\n"

    "   vec4  fvTotalAmbient   = fvAmbient * fvBaseColor; \n"
    "   vec4  fvTotalDiffuse   = fvDiffuse * fNDotL * fvBaseColor; \n"
    "   vec4  fvTotalSpecular  = fvSpecular * ( pow( fRDotV, fSpecularPower ) );\n"

    "   gl_FragColor = ( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular );\n"
    //"   gl_FragColor = texture2D( normalMap, Texcoord);\n"

    "}\n";

std::string VistaOpenSGNormalMapMaterial::_fs_texturednormalmap =
    "uniform vec4 fvAmbient;\n"
    "uniform vec4 fvDiffuse;\n"
    "uniform vec4 fvSpecular;\n"
    "uniform float fSpecularPower;\n"

    "uniform sampler2D normalMap;\n"
    "uniform sampler2D baseMap;\n"

    "varying vec2 Texcoord;\n"
    "varying vec3 ViewDirection;\n"
    "varying vec3 LightDirection;\n"

    "void main( void )\n"
    "{\n"
    "   vec3  fvLightDirection = normalize( LightDirection );\n"
    "   vec3  fvNormal         = normalize( ( texture2D( normalMap, Texcoord ).xyz * 2.0 ) - 1.0 "
    ");\n"
    "   float fNDotL           = dot( fvNormal, fvLightDirection ); \n"

    "   vec3  fvReflection     = normalize( ( ( 2.0 * fvNormal ) * fNDotL ) - fvLightDirection ); "
    "\n"
    "   vec3  fvViewDirection  = normalize( ViewDirection );\n"
    "   float fRDotV           = max( 0.0, dot( fvReflection, fvViewDirection ) );\n"

    "   vec4  fvBaseColor      = texture2D( baseMap, Texcoord );\n"

    "   vec4  fvTotalAmbient   = fvAmbient * fvBaseColor; \n"
    "   vec4  fvTotalDiffuse   = fvDiffuse * fNDotL * fvBaseColor; \n"
    "   vec4  fvTotalSpecular  = fvSpecular * ( pow( fRDotV, fSpecularPower ) );\n"

    "   gl_FragColor = ( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular );\n"
    "}\n";
