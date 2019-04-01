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


#if defined(WIN32)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#endif

#include "VistaOpenSGHaloShader.h"
#include "VistaOpenSGPerMaterialShader.h"

#include <VistaBase/VistaStreamUtils.h>

#include <OpenSG/OSGChunkMaterial.h>
#include <OpenSG/OSGMaterialChunk.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>

#include <queue>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*This class renders objects as a halo to create a glow effect for lights*/
/*There are two types of lights, which can be rendered in dependence to the over given transparency value.*/
/*The brightness of the halo should be controlled by the parent class*/

/*Type 1:*/
/*If the transparency value is below 0.5 then the light is rendered with z-value 0 and with a rotating star effect.*/
/*This means if an object has the value 0 then it is nearly fully transparent and if it is rendered with 0.49 it is very opaque rendered with a star.*/
/*Please take notice that setting the z-value to 0 requires the parent class to turn off the light manually if its non-visible (for example via ray tracing).*/
/*See LabInLab Demo car front lights.*/

/*Type 2 (SIMPLER):*/
/*If the transparency value is between 0.5 and 1 then the object is simple rendered with an approximated Gaussian.*/
/*This means if an object has the value 0.5 then it is nearly fully transparent and if it is rendered with 1 it is very opaque.*/
/*This is the simplest way for creating a glow effect. The z-value simply over given and not set to zero, thus there could be occlusion problems.*/
/*See LabInLab Demo car back lights.*/

/*TODO: - Implement all functionalities via additional parameters for a better usability*/
/*		- Transparency via textures (spherical billboards)*/

const std::string VistaOpenSGHaloShader::s_sVertexShader = \
"varying vec3 v3WorldNormal, v3WorldPos;\n"
"varying float fDepth;\n"
"\n"
"void main()\n"
"{	\n"
"	v3WorldNormal = normalize( gl_NormalMatrix * gl_Normal );	\n"		
"	v3WorldPos = vec3( gl_ModelViewMatrix * gl_Vertex );\n"
"	gl_Position = ftransform();\n"
"\n"
"	vec4 tColor = gl_FrontMaterial.diffuse;\n"
"	if( tColor[3] >= 0.5 ) {\n"
"		tColor[3] = (tColor[3]-0.5)*2.0;\n"
"		fDepth = 1.0;\n"
"	}else{\n"
"		tColor[3] = tColor[3]*2.0;\n"
"		fDepth = 0.0;\n"
"	}\n"
"	gl_FrontColor = tColor;\n"
"	gl_BackColor = tColor;\n"
"}\n";

const std::string VistaOpenSGHaloShader::s_sFragmentShader = \
"varying vec3 v3WorldNormal, v3WorldPos; \n"
"varying float fDepth;\n"
"\n"
"void main()\n"
"{\n"
"	vec4 tColor =  gl_Color;\n"
"	if( gl_FrontFacing ){\n"
"		float fAngle = asin(dot(v3WorldPos, v3WorldNormal) / (length(v3WorldPos) * length(v3WorldNormal)));\n"
"		float fMainBrightness = tColor[3]*pow(abs(fAngle),3.0)/12.0;\n"
"		if( fDepth < 0.5 ) \n"
"		{\n"
"			float fRotationAngle = (v3WorldPos[0]+v3WorldPos[1])/2.0;\n"
"			vec3 v3HorVec = vec3( cos(fRotationAngle), sin(fRotationAngle), 0.0 );\n"
"			vec3 v3VerVec = vec3( -sin(fRotationAngle), cos(fRotationAngle), 0.0 );\n"
"			vec3 v3DiagVec1 = vec3( cos(fRotationAngle+0.785), sin(fRotationAngle+0.785), 0.0 );\n"
"			vec3 v3DiagVec2 = vec3( -sin(fRotationAngle+0.785), cos(fRotationAngle+0.785), 0.0 );\n"
"			v3HorVec = cross(v3HorVec, v3WorldPos);\n"
"			v3VerVec = cross(v3VerVec, v3WorldPos);\n"
"			v3DiagVec1 = cross(v3DiagVec1, v3WorldPos);\n"
"			v3DiagVec2 = cross(v3DiagVec2, v3WorldPos);\n"
"			float fHorAngle = abs(asin(dot(v3HorVec, v3WorldNormal) / (length(v3HorVec)*length(v3WorldNormal))));\n"
"			float fVerAngle = abs(asin(dot(v3VerVec, v3WorldNormal) / (length(v3VerVec)*length(v3WorldNormal))));\n"
"			float fDiag1Angle = abs(asin(dot(v3DiagVec1, v3WorldNormal) / (length(v3DiagVec1)*length(v3WorldNormal))));\n"
"			float fDiag2Angle = abs(asin(dot(v3DiagVec2, v3WorldNormal) / (length(v3DiagVec2)*length(v3WorldNormal))));\n"
"			float fHorBrightness = max(0.0, fMainBrightness-tColor[3]*fHorAngle);\n"
"			float fVerBrightness = max(0.0, fMainBrightness-tColor[3]*fVerAngle);\n"
"			float fDiag1Brightness = max(0.0, fMainBrightness*(fMainBrightness-tColor[3]*fDiag1Angle));\n"
"			float fDiag2Brightness = max(0.0, fMainBrightness*(fMainBrightness-tColor[3]*fDiag2Angle));\n"
"			tColor[3] = 0.25*(fHorBrightness+fVerBrightness)+(fDiag1Brightness+fDiag2Brightness)+fMainBrightness;\n"
"		}\n"
"		else{\n"
"			tColor[3] = fMainBrightness;\n"
"		}\n"
"		if( fDepth < 0.5 ) {\n"
"			gl_FragDepth = 0.0;\n"
"		}else gl_FragDepth = gl_FragCoord.z;\n"
"		tColor[3] = min( 1.0, 2.0*tColor[3]);\n"
"	}else tColor[3] = 0.0;\n"
"	gl_FragColor = tColor;\n"
"}\n";


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaOpenSGHaloShader::VistaOpenSGHaloShader()
: VistaOpenSGPerMaterialShader()

{
	SetShadersFromString( s_sVertexShader, s_sFragmentShader );
	SetUniformParameter( VistaOpenSGPerMaterialShader::CShaderUniformParam(
				VistaOpenSGPerMaterialShader::CShaderUniformParam::OSG_ACTIVE_LIGHTS ) );
}
VistaOpenSGHaloShader::VistaOpenSGHaloShader(
									const std::string& sVertexShaderFile,
									const std::string& sFragmentShaderFile )
: VistaOpenSGPerMaterialShader()
{
	SetShadersFromFile( sVertexShaderFile, sFragmentShaderFile );
	SetUniformParameter( VistaOpenSGPerMaterialShader::CShaderUniformParam(
				VistaOpenSGPerMaterialShader::CShaderUniformParam::OSG_ACTIVE_LIGHTS ) );
}
VistaOpenSGHaloShader::~VistaOpenSGHaloShader()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaOpenSGHaloShader::ApplyToOSGMaterial( osg::ChunkMaterialPtr& pOSGMaterial )
{
	if( pOSGMaterial == osg::NullFC )
		return false;

	std::vector<VistaOpenSGPerMaterialShader::CShaderUniformParam> vecParams;
	return VistaOpenSGPerMaterialShader::ApplyToOSGMaterial( pOSGMaterial, vecParams );
}

bool VistaOpenSGHaloShader::ApplyToOSGMaterial( osg::ChunkMaterialPtr& pOSGMaterial,
												const std::vector<CShaderUniformParam>& vecUniformParams )
{
	if( pOSGMaterial == osg::NullFC )
		return false;

	std::vector<VistaOpenSGPerMaterialShader::CShaderUniformParam> vecParams;
	return VistaOpenSGPerMaterialShader::ApplyToOSGMaterial( pOSGMaterial, vecParams );
}



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

