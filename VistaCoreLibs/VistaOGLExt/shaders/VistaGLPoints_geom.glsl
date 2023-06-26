/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2011 RWTH Aachen University               */
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
// $Id: VistaGLLine_Default_geom.glsl 33944 2013-02-18 16:15:38Z jh864363 $
#version 330 compatibility

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform float u_fPointSize;
uniform vec3  u_v3Dir;
uniform bool  u_bDirInViewCoord;

/*============================================================================*/
/* VARYING VARIABLES														  */
/*============================================================================*/
in vec4 var_v4PrimaryColor[];
in vec4 var_v4SecondaryColor[];

out vec3 var_v3Position;
out vec2 var_v2TexCoord;
out vec3 var_v3LocalXAxis;
out vec3 var_v3LocalYAxis;
out vec4 var_v4Color1;
out vec4 var_v4Color2;

/*============================================================================*/
/* GLOBAL VARIABLES	AND FUNKTIONS											  */
/*============================================================================*/
void CreateVertex( vec4 v4Pos, vec3 v3Offset, vec2 v2TexCoord )
{
	var_v2TexCoord = v2TexCoord;
	var_v3Position = v4Pos.xyz + u_fPointSize*v3Offset;
	gl_Position    = gl_ProjectionMatrix*vec4( var_v3Position, 1 );
	EmitVertex();
}

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main ()
{	
	vec3 v3Dir = u_v3Dir;
	
	if( !u_bDirInViewCoord )
		v3Dir = gl_NormalMatrix*v3Dir;
	
	vec3 v3X = normalize( cross( gl_in[0].gl_Position.xyz, v3Dir ) );
	vec3 v3Y = normalize( cross( v3X, gl_in[0].gl_Position.xyz  ) );

	// pass values to fragment shader
	var_v3LocalXAxis = v3X;
	var_v3LocalYAxis = v3Y;
	var_v4Color1     = var_v4PrimaryColor[0];
	var_v4Color2     = var_v4SecondaryColor[0];

	CreateVertex( gl_in[0].gl_Position, -v3X -v3Y, vec2(0.0, 0.0));
	CreateVertex( gl_in[0].gl_Position, -v3X +v3Y, vec2(0.0, 1.0));
	CreateVertex( gl_in[0].gl_Position, +v3X -v3Y, vec2(1.0, 0.0));
	CreateVertex( gl_in[0].gl_Position, +v3X +v3Y, vec2(1.0, 1.0));

	EndPrimitive();
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
