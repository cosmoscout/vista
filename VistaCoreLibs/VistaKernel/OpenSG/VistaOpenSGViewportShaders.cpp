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
// $Id: VistaOpenSGThreadImp.cpp 34621 2013-04-18 15:04:56Z dr165799 $

#include "VistaOpenSGViewportShaders.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace
{
	const std::string g_sDefaultVertexShader =
		"void main() \n"
		"{ \n"
		"    gl_TexCoord[0] = gl_MultiTexCoord0; \n"
		"    gl_TexCoord[1] = gl_MultiTexCoord1; \n"
		"    gl_Position = ftransform(); \n"
		"} \n";

	const std::string g_sAnaglyphCyanMagentaFragmentShader =
		"uniform sampler2D texture_lefteye; \n"
		"uniform sampler2D texture_righteye; \n"
		"void main()\n"
		"{		 \n"
		"    vec4 v3LeftColor = texture2D( texture_lefteye, gl_TexCoord[0].st ); \n"
		"    vec4 v3RightColor = texture2D( texture_righteye, gl_TexCoord[0].st ); \n"
		"    gl_FragColor = vec4( v3LeftColor.r, v3RightColor.g, v3RightColor.b, 1.0 ); \n"
		"} \n";

	const std::string g_sAnaglyphCyanMagentaMonochromeFragmentShader =
		"uniform sampler2D texture_lefteye; \n"
		"uniform sampler2D texture_righteye; \n"
		"void main()\n"
		"{		 \n"
		"    vec4 v3LeftColor = texture2D( texture_lefteye, gl_TexCoord[0].st ); \n"
		"    vec4 v3RightColor = texture2D( texture_righteye, gl_TexCoord[0].st ); \n"
		"    float nLeftAccum = ( v3LeftColor.r + v3LeftColor.g + v3LeftColor.b ) / 3.0; \n"
		"    float nRightAccum = ( v3RightColor.r + v3RightColor.g + v3RightColor.b ) / 3.0; \n"
		"    gl_FragColor = vec4( nLeftAccum, nRightAccum, nRightAccum, 1.0 ); \n"
		"} \n";
	
	const std::string g_sANullPostPorcessFragmentShader = 
		"uniform sampler2D texture; \n"
		"void main()\n"
		"{\n"
		"    vec4 v4Color = texture2D( texture, gl_TexCoord[0].st ); \n"
		"    gl_FragColor = v4Color;\n"
		"} \n";
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

const std::string& VistaOSGViewportShader::GetStandardVertexShader()
{
	return g_sDefaultVertexShader;
}

const std::string& VistaOSGViewportShader::GetAnaglyphCyanMagentaFragmentShader()
{
	return g_sAnaglyphCyanMagentaFragmentShader;
}

const std::string& VistaOSGViewportShader::GetAnaglyphCyanMagentaMonochromeFragmentShader()
{
	return g_sAnaglyphCyanMagentaMonochromeFragmentShader;
}

const std::string& VistaOSGViewportShader::GetNullPostProcessFragmentShader()
{
	return g_sANullPostPorcessFragmentShader;
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


